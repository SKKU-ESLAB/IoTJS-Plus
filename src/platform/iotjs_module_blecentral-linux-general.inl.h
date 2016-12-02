/* Copyright 2016 Jhuyeong Jhin <jjysienna@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IOTJS_MODULE_BLECENTRAL_LINUX_GENERAL_INL_H
#define IOTJS_MODULE_BLECENTRAL_LINUX_GENERAL_INL_H

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <uv.h>

#define HCI_STATE_NONE 0
#define HCI_STATE_OPEN 2
#define HCI_STATE_SCANNING 3
#define HCI_STATE_FILTERING 4

struct ble_common {
  struct hci _hci;
  struct gap _gap;
  struct bindings _bindings;
};

struct hci {
  struct hci_state _hci_state;
};

struct hci_state {
  int device_id;
  int device_handle;
  struct hci_filter of;
  int state;
  uint8_t _scan_duplicates;
  int has_error;
  char error_message[1024];
  uv_poll_t _poll_handle;
  void *cb;
} hci_state;

#define EIR_FLAGS 0x01
#define EIR_NAME_SHORT 0x08
#define EIR_NAME_COMPLETE 0x09
#define EIR_MANUFACTURE_SPECIFIC 0xff

#define C_UNIT_TEST 1

#if C_UNIT_TEST
#define BLECENTRAL_WORKER_INIT_TEMPLATE do{}while(0);

#else
#define BLECENTRAL_WORKER_INIT_TEMPLATE    \
    iotjs_blecentral_reqwrap_t *req_wrap  \
    = iotjs_blecentral_reqwrap_from_request(work_req);  \
    iotjs_blecentral_reqdata_t *req_data  \
    = iotjs_blecentral_reqwrap_data(req_wrap);
#endif

#define LE2HS(ptr) (btohs(*(uint16_t *)(ptr)))

#define EIR_NAME_SHORT 0x08
#define EIR_NAME_COMPLETE 0x09

static void eir_parse_name(uint8_t *eir, size_t eir_len, char *buf,
                           size_t buf_len){
  size_t offset;

  offset = 0;
  while (offset < eir_len) {
    uint8_t field_len = eir[0];
    size_t name_len;

    if(field_len == 0)
      break;

    if(offset + field_len > eir_len)
      goto failed;

    switch(eir[1]) {
      case EIR_NAME_SHORT:
      case EIR_NAME_COMPLETE:
        name_len = field_len - 1;
        if (name_len > buf_len)
          goto failed;

        memcpy(buf, &eir[2], name_len);
        return;

    }

    offset += field_len + 1;
    eir += field_len + 1;
  }

failed:
  snprintf(buf, buf_len, "(undefined)");

}

void hci_le_advertising_report(uint8_t status, uint8_t type, 
                               const char *address, uint8_t address_type, 
                               const char *eir_name, uint8_t rssi){
  uint8_t disc_count = 0;
  uint8_t connectable = 0'
  if(type == 0x04)
    disc_count++;

  if(disc_count){ // type == 0x04, disccount > 1, NOBLE_REPORT_ALL_HCI_EVENTS
#if C_UNIT_TEST
    printf("status: %hhu, type: %hhu, address: %s, addr_type: %s, eir_name: %s, 
           rssi: %hhu\n", status, type, address, 
           address_type ? "random" : "public", eir_name, rssi);
#else
    iotjs_blecentral_event_callback(kBlecentralEvDiscover, args);
#endif
  }
}

void poll(struct hci_state *_hci_state){

  int len = 0;
  char data[HCI_MAX_EVENT_SIZE];

  len = read(_hci_state->device_handle, data, sizeof(data));

  if(len < 0){

    if(errno == EINTR)
      return;

    if(errno == EAGIN || errno == EINTR)
      return;

    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to read socket: %s", strerror(errno));

  }

  if(len > 0){
    uint8_t ev_type = data[0];

    if(ev_type == HCI_EVENT_PKT){
      uint8_t subevent = data[1];

      if(subevent == EVT_LE_META_EVENT){
        char addr[18];
        evt_le_meta_event *meta = (void *)(data + (1 + HCI_EVENT_HDR_SIZE));
        
        len -= (1 + HCI_EVENT_HDR_SIZE);
        
        if(meta->subevent == EVT_LE_ADVERTISING_REPORT){
          char name[30];
          le_advertising_info *info = (le_advertising_info *) (meta->data + 1);
          uint8_t type = info->evt_type;
          uint8_t address_type = info->bdaddr_type; //if 1 'random', 0 'public'
          uint8_t rssi = info->data[info->length];
          
          if(info->length == 0) // no info
            return;

          memset(name, 0, sizeof(name));

          ba2str(&info->bdaddr, addr); //address in string xx:xx:xx:xx:xx:xx
          eir_parse_name(info->data, info->length, name, sizeof(name) - 1);

          hci_le_advertising_report(0, type, addr, address_type, name, rssi);

        }
      }
    }
    else if(ev_type == HCI_COMMAND_PKT){
      uint16_t cmd = LE2HS(&data[1]);
      uint8_t len = data[3];

      if(cmd == LE_SET_SCAN_ENABLE_CMD) {
        uint8_t enable = data[4] ? 1 : 0;
        uint8_t duplicates = data[5] ? 1 : 0;

        if(_hci_state->state == HCI_STATE_SCANNING 
           || _hci_state->state == HCI_STATE_FILTERING){

          if(!enable){
            iotjs_blecentral_event_callback(kBlecentralEvScanStop, NULL);
          } else if (duplicates != _hci_state->_scan_duplicates){
            _hci_state->_scan_duplicates = duplicates;

            iotjs_blecentral_event_callback(kBlecentralEvScanStart, duplicates);
          }
        } else if((_hci_state->state == HCI_STATE_OPEN) && enable) {
#if C_UNIT_TEST == 0
          iotjs_blecentral_event_callback(kBlecentralEvScanStart, &duplicates);
#endif
        }
      }
    }
    // need to handle else
  }
}

void uv_callback(uv_poll_t *handle, int status, int events){
  struct hci_state *_hci_state = (struct hci_state *)handle->data;

  _hci_state->cb(_hci_state);
}

void __close_device(struct hci_state _hci_state) {

  uv_poll_stop(&_hci_state->_poll_handle);
}


void __open_default_hci_device(struct hci_state *_hci_state){

  _hci_state->device_id = hci_get_route(NULL);

  if((_hci_state->device_handle = hci_open_dev(_hci_state->device_id)) < 0){

    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
            "Could not open device: %s", strerror(errno));
    return NULL;
  }

  int on = 1;

  if(ioctl(_hci_state->device_handle, FIONBIO, (char *)&on) <0) {

    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Could not set device to non-blocking: %s", strerror(errno));
    return NULL;
  }

  _hci_state->state = HCI_STATE_OPEN;

  uv_poll_init(uv_default_loop(), &_hci_state->_poll_handle, 
               _hci_state->device_handle);

  _hci_state->cb = poll;
  _hci_state->_poll_handle.data = _hci_state->cb;

  uv_poll_start(&_hci_state->_poll_handle, UV_READABLE, uv_callback);

}

void __do_start_scanning(struct hci_state *_hci_state,
                         char *service_uuids, int allow_duplicates) {

  int err, opt, dd;
  uint8_t own_type = 0x00;
  uint8_t scan_type = 0x01;
  uint8_t filter_type = 0;
  uint8_t filter_policy = 0x00;
  uint16_t interval = htobs(0x0010);
  uint16_t window = htobs(0x0010);
  uint8_t filter_dup = allow_duplicates ? 1 : 0;

  dd = _hci_state->device_handle;

  _hci_state->_scan_duplicates = filter_dup;

  err = hci_le_set_scan_parameters(dd, scan_type, interval, window,
                                   own_type, filter_policy, 1000);

  if(err < 0){
    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to set scan parameters: %s", strerror(errno));
    return;
  }

  err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 1000);
  if(err < 0){ 
    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to set scan enable: %s", strerror(errno));
    return;
  }

  _hci_state->state = HCI_STATE_SCANNING;

  //save HCI filter
  socklen_t len = sizeof(_hci_state->of);
  err = getsockopt(dd, SOL_HCI, HCI_FILTER, &_hci_state->of, &olen);
  if(err < 0){ 
    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to get socket option: %s", strerror(errno));
    return;
  }

  struct hci_filter nf;

  hci_filter_clear(&nf);
  hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
  hci_filter_set_event(EVT_LE_META_EVENT, &nf);

  err = setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf));
  if(err < 0){ 
    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to set socket option: %s", strerror(errno));
    return;
  }

  _hci_state->state = HCI_STATE_FILTERING;
}

void __do_stop_scanning(struct hci_state *_hci_state) {
  
  int dd, err;
  dd = _hci_state->device_handle;

  if(_hci_state->state == HCI_STATE_FILTERING) {

    err = setsockopt(dd, SOL_HCI, HCI_FILTER, &_hci_state->of,
                     sizeof(_hci_state->of));
    if(err < 0){ 
      _hci_state->has_error = TRUE;
      snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
               "Failed to set socket option: %s", strerror(errno));
      return;
    }

    _hci_state->state = HCI_STATE_SCANNING;
  }

  err = hci_le_set_scan_enable(dd, 0x00, 1, 1000);
  if(err < 0){ 
    _hci_state->has_error = TRUE;
    snprintf(_hci_state->error_message, sizeof(_hci_state->error_message),
             "Failed to set scan disable: %s", strerror(errno));
    return;
  }

  _hci_state->state = HCI_STATE_OPEN;
}


#if C_UNIT_TEST

void main(int argc, char *argv[]){

  struct hci_state *hci = malloc(sizeof(struct hci_state));

  __open_default_hci_device(hci);

  __do_start_scanning(hci);

  while(!hci->has_error){

    poll(hci);
  }
  __do_stop_scanning(hci);
  return;

}
#else
void BlecentralDestroy(){

  struct ble_common *ble 
      = (struct ble_common *)iotjs_blecentral_get_instance()->platform_handle;

  __close_device(ble->hci._hci_state);
  free(ble);
}

void BlecentralCreate(){

  struct ble_common *ble = IOTJS_ALLOC(struct ble_common);

  iotjs_blecentral_t *blecentral = iotjs_blecentral_get_instance();
  blecentral->platform_handle = (void *)ble;

  __open_default_hci_device(&ble->hci._hci_state);

  if(ble->hci._hci_state.has_error){
    // something wrong about creating
    iotjs_blecentral_event_callback(kBlecentralEvError,
                                    ble->hci._hci_state.error_message);
    free(ble);
  }

}

void BlecentralStartScanning(char **suids, int duplicates) {
  // need to handle service uuids
  struct ble_common *ble
      = (struct ble_common *)iotjs_blecentral_get_instance()->platform_handle;

  __do_start_scanning(&ble->hci._hci_state, suids, duplicates);

  if(ble->hci._hci_state.has_error){
    // something wrong about creating
    iotjs_blecentral_event_callback(kBlecentralEvError,
                                    ble->hci._hci_state.error_message);
  } 
}


void BlecentralStopScanning(void) {

  struct ble_common *ble
      = (struct ble_common *)iotjs_blecentral_get_instance()->platform_handle;
  __do_stop_scanning(&ble->hci._hci_state);

  if(ble->hci._hci_state.has_error){
    // something wrong about creating
    iotjs_blecentral_event_callback(kBlecentralEvError,
                                    ble->hci._hci_state.error_message);
  } 
}

#endif
#endif
