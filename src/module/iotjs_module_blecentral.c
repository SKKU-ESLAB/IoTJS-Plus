/* Copyright 2016 Eunsoo Park (esevan.park@gmail.com)
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

#include "iotjs_module_blecentral.h"

static void iotjs_blecentral_destroy(iotjs_blecentral_t *instance) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_blecentral_t, instance);

  BlecentralDestroy();

  iotjs_jobjectwrap_destroy(&_this->jobjectwrap);
  IOTJS_RELEASE(instance);
}

iotjs_blecentral_t* iotjs_blecentral_create(const iotjs_jval_t* jblecentral) {
  iotjs_blecentral_t* blecentral = IOTJS_ALLOC(iotjs_blecentral_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_blecentral_t, blecentral);
  iotjs_jobjectwrap_initialize(&_this->jobjectwrap, jblecentral,
                               (JFreeHandlerType)iotjs_blecentral_destroy);

  return blecentral;
}

const iotjs_jval_t* iotjs_blecentral_get_jblecentral() {
  return iotjs_module_get(MODULE_BLECENTRAL);
}

iotjs_blecentral_t* iotjs_blecentral_get_instance() {
  const iotjs_jval_t* jblecentral = iotjs_blecentral_get_jblecentral();
  iotjs_jobjectwrap_t* jobjectwrap =
      iotjs_jobjectwrap_from_jobject(jblecentral);

  return (iotjs_blecentral_t*)jobjectwrap;
}

#define THIS iotjs_blecentral_reqwrap_t *blecentral_wrap
iotjs_blecentral_reqwrap_t *
iotjs_blecentral_reqwrap_create(const iotjs_jval_t *jcallback,
                                BlecentralOp op) {
  iotjs_blecentral_reqwrap_t *blecentral_reqwrap = 
      IOTJS_ALLOC(iotjs_blecentral_reqwrap_t);
  IOTJS_VALIDATED_STRUCT_CONSTRUCTOR(iotjs_blecentral_reqwrap_t,
                                     blecentral_reqwrap);

  iotjs_reqwrap_initialize(&_this->reqwrap, jcallback,
                           (uv_req_t *)&_this->req);

  _this->req_data.op = op;

  return blecentral_reqwrap;
}

static void iotjs_blecentral_reqwrap_destroy(THIS) {
  IOTJS_VALIDATED_STRUCT_DESTRUCTOR(iotjs_blecentral_reqwrap_t *,
                                    blecentral_reqwrap);
  iotjs_reqwrap_destroy(&_this->reqwrap);
  
  IOTJS_RELEASE(blcentral_reqwrap);
}

void iotjs_blecentral_reqwrap_dispatched(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_blecentral_reqwrap_t *,
                                blecentral_reqwrap);

  iotjs_blecentral_reqwrap_destroy(blecentral);
}

uv_work_t *iotjs_blecentral_reqwrap_req(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_blecentral_reqwrap_t *,
                                blecentral_reqwrap);

  return &_this->req;
}

const iotjs_jval_t *iotjs_blecentral_reqwrap_jcallback(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_blecentral_reqwrap_t *,
                                blecentral_reqwrap);

  return iotjs_reqwrap_jcallback(&_this->reqwrap);
}

iotjs_blecentral_reqdata_t *iotjs_blecentral_reqwrap_data(THIS) {
  IOTJS_VALIDATED_STRUCT_METHOD(iotjs_blecentral_reqwrap_t *,
                                blecentral_reqwrap);

  return &_this->reqwrap;
}

iotjs_blecentral_reqwrap_t *
iotjs_blecentral_reqwrap_from_request(uvwork_t *req) {
  return (iotjs_blecentral_reqwrap_t *)
      iotjs_reqwrap_from_request((uv_req_t *)req);
}
void *iotjs_blecentral_req_get_userdata(uv_work_t *req) {
  return ((uv_req_t *)req)->data;
}
void iotjs_blecentral_req_set_userdata(uv_work_t *req, void *data) {
  ((uv_req_t *)req)->data = data;
}
#undef THIS

void iotjs_blecentral_event_callback(BlecentralEv ev, void *args) {
  const iotjs_jval_t *obj = iotjs_blecentral_get_jblecentral();
  iotjs_jval_t func = iotjs_jval_get_property(obj, "onEvent");
  iotjs_jargs_t argv;
  switch (ev) {
    case kBlecentralEvStateChange:
      //Example Code:
      argv = iotjs_jargs_create(1);
      int state = 1;
      iotjs_jargs_append_number(&argv, (double) state);
      break;
    case kBlecentralEvScanStart:
      argv = iotjs_jargs_create(0);
      break;
    case kBlecentralEvScanStop:
      argv = iotjs_jargs_create(0);
      break;
    case kBlecentralEvDiscover:
      //Example Code:
      argv = iotjs_jargs_create(5);
      break;
  }

  iotjs_make_callback(&func, &obj, &argv);

  iotjs_jargs_destroy(&argv);
}

#define BLECENTRAL_ASYNC(op) \
    do {                                                                 \
      uv_loop_t* loop = iotjs_environment_loop(iotjs_environment_get()); \
      uv_queue_work(loop, req, op##Worker, AfterBlecentralWork);                \
    } while (0)

void AfterBlecentralWork(uv_work_t *work_req, int status) {
  void *param = iotjs_blecentral_req_get_userdata(work_req);

  switch (req_data->op) {
    case kBlecentralOpStartScannig:
      IOTJS_RELEASE(param);
      break;
    case kBlecentralOpStopScanning:
    default:
      break;
  }
}

// serviceUuids, allowDuplicates
JHANDLER_FUNCTION(StartScanning) {
  JHANDLER_CHECK_ARGS(2, array, number);

  /* TODO: After array binding implementation
  iotjs_jval_t jsvc_uuid = JHANDLER_GET_ARG(0, array); */
  int jallow_duplicates = (int)JHANDLER_GET_ARG(1, number);

  uv_work_t *req = IOTJS_ALLOC(uv_work_t);
  iotjs_blecentral_start_scanning_param_t *param =
      IOTJS_ALLOC(iotjs_blecentral_start_scanning_param_t);

  iotjs_blecentral_req_set_userdata(req, (void *)param);

  BLECENTRAL_ASYNC(StartScanning);
}

// void
JHANDLER_FUNCTION(StopScanning) {
  JHANDLER_CHECK_ARGS(0);

  uv_work_t *req = IOTJS_ALLOC(uv_work_t);

  iotjs_blecentral_req_set_userdata(req, NULL);
  
  BLECENTRAL_ASYNC(StopScanning);
}

JHANDLER_FUNCTION(Init) {
  JHANDLER_CHECK_ARGS(0);
  
  BlecentralCreate();
}

iotjs_jval_t InitBlecentral() {
  iotjs_jval_t jblecentral = iotjs_jval_create_object();

  // Ble-central
  iotjs_jval_set_method(&jblecentral, "startScanning", StartScanning);
  iotjs_jval_set_method(&jblecentral, "stopScanning", StopScanning);

  iotjs_jval_t on_event = iotjs_jval_create_object();
  iotjs_jval_set_property_jval(&jblecentral, "onEvent", &on_event);
  // EventListener
  iotjs_jval_set_method(&jblecentral, "init", Init);

  iotjs_blecentral_t* blecentral = iotjs_blecentral_create(&jblecentral);

  return jblecentral;
}
