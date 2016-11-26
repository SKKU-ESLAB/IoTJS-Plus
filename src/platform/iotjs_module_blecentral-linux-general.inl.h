#ifndef IOTJS_MODULE_BLECENTRAL_LINUX_GENERAL_INL_H
#define IOTJS_MODULE_BLECENTRAL_LINUX_GENERAL_INL_H

#include "module/iotjs_module_blecentral.h"

void binding_start_scanning(const char *svc_uuid, int allow_duplicates) {
  // TODO: not implemented
}

void binding_stop_scanning(void) {
  // TODO: not implemented
}

void binding_connect(const char *perip_uuid) {
  // TODO: not implemented
}

void binding_disconnect(const char *perip_uuid) {
  // TODO: not implemented
}

void binding_discover_service(const char *perip_uuid) {
  // TODO: not implemented
}

void binding_read_handle(const char *perip_uuid, const char *handle) {
  // TODO: not implemented
}

void binding_write_handle(const char *perip_uuid, const char *handle, 
                          const char *buffer, int len, int without_response) {
  // TODO: not implemented
}

void binding_discover_included_services(const char *perip_uuid, 
                                        const char *svc_uuid) {
  // TODO: not implemented
}

void binding_discover_characteristics(const char *perip_uuid, 
                                            const char *svc_uuid) {
  // TODO: not implemented
}

void binding_read(const char *perip_uuid, 
                  const char *svc_uuid, const char *char_uuid) {
  // TODO: not implemented
}

void binding_write(const char *perip_uuid, const char *svc_uuid, 
                   const char *char_uuid,const char *buffer, 
                   int len, int without_response) {
  // TODO: not implemented
}

void binding_broadcast(const char *perip_uuid, const char *svc_uuid, 
                       const char *char_uuid, int broadcast) {
  // TODO: not implemented
}

void binding_notify(const char *perip_uuid, const char *svc_uuid, 
                    const char *char_uuid, int notify) {
  // TODO: not implemented
}

void binding_discover_descriptors(const char *perip_uuid, 
                                  const char *svc_uuid, 
                                  const char *char_uuid) {
  // TODO: not implemented
}

void binding_read_value(const char *perip_uuid, const char *svc_uuid,
                        const char *char_uuid, const char *desc_uuid) {
  // TODO: not implemented
}

void binding_write_value(const char *perip_uuid, const char *svc_uuid,
                         const char *char_uuid, const char *desc_uuid,
                         const char *buffer, int len) {
  // TODO: not implemented
}

void binding_value(void) {
}

void binding_listen(void) {

}

#endif
