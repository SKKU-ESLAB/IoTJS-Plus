void binding_start_scanning(char *svc_uuid, int allow_duplicates) {
  // TODO: not implemented
}

void binding_stop_scanning(void) {
  // TODO: not implemented
}

void binding_connect(char *perip_uuid) {
  // TODO: not implemented
}

void binding_disconnect(char *perip_uuid) {
  // TODO: not implemented
}

void binding_discover_service(char *perip_uuid) {
  // TODO: not implemented
}

void binding_read_handle(char *perip_uuid, char *handle) {
  // TODO: not implemented
}

void binding_write_handle(char *perip_uuid, char *handle, 
                          char *buffer, int len, int without_response) {
  // TODO: not implemented
}

void binding_discover_included_services(char *perip_uuid, char *svc_uuid) {
  // TODO: not implemented
}

void binding_discover_characteristics(char *perip_uuid, char *svc_uuid) {
  // TODO: not implemented
}

void binding_read(char *perip_uuid, char *svc_uuid, char *char_uuid) {
  // TODO: not implemented
}

void binding_write(char *perip_uuid, char *svc_uuid, char *char_uuid,
                   char *buffer, int len, int without_response) {
  // TODO: not implemented
}

void binding_broadcast(char *perip_uuid, char *svc_uuid, char *char_uuid, 
                       int broadcast) {
  // TODO: not implemented
}

void binding_notify(char *perip_uuid, char *svc_uuid, char *char_uuid, 
                    int notify) {
  // TODO: not implemented
}

void binding_discover_descriptors(char *perip_uuid, 
                                  char *svc_uuid, 
                                  char *char_uuid) {
  // TODO: not implemented
}

void binding_read_value(char *perip_uuid, char *svc_uuid,
                        char *char_uuid, char *desc_uuid) {
  // TODO: not implemented
}

void binding_write_value(char *perip_uuid, char *svc_uuid,
                         char *char_uuid, char *desc_uuid,
                         char *buffer, int len) {
  // TODO: not implemented
}

void binding_value(void) {
}
