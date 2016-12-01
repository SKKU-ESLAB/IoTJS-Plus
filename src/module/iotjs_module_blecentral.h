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

#ifndef IOTJS_MODULE_BLECENTRAL_H
#define IOTJS_MODULE_BLECENTRAL_H

#include "iotjs_def.h"
#include "iotjs_objectwrap.h"
#include "iotjs_reqwrap.h"

typedef enum {
  kBlecentralOpStartScanning,
  kBlecentralOpStopScanning,
} BlecentralOp;

typedef enum {
  kBlecentralEvStateChange,
  kBlecentralEvScanStart,
  kBlecentralEvScanStop,
  kBlecentralEvDiscover,
} BlecentralEv;

typedef enum {
  kBlecentralErrNone = 0,
  kBlecentralErrFail = -1,
} BlecentralErr;

typedef struct {
  BlecentralOp op;
  BlecentralErr err;
} iotjs_blecentral_reqdata_t;

typedef struct {
  char **svc_uuids;
  int allow_duplicates;
} iotjs_blecentral_start_scanning_param_t;

typedef struct {
  iotjs_reqwrap_t reqwrap;
  uv_work_t req;
  iotjs_blecentral_reqdata_t req_data;
} IOTJS_VALIDATED_STRUCT(iotjs_blecentral_reqwrap_t);

typedef struct {
  iotjs_jobjectwrap_t jobjectwrap;
  void *platform_handle;
} IOTJS_VALIDATED_STRUCT(iotjs_blecentral_t);

iotjs_blecentral_t *iotjs_blecentral_create(const iotjs_jval_t *jble_central);
const iotjs_jval_t *iotjs_blecentral_get_jblecentral();
iotjs_blecentral_t *iotjs_blecentral_get_instance();

#define THIS iotjs_blecentral_reqwrap_t *blecentral_wrap
iotjs_blecentral_reqwrap_t *
iotjs_blecentral_reqwrap_create(const iotjs_jval_t *jcallback,
                                BlecentralOp op);
void iotjs_blecentral_reqwrap_dispatched(THIS);
uv_work_t *iotjs_blecentral_reqwrap_req(THIS);
const iotjs_jval_t *iotjs_blecentral_reqwrap_jcallback(THIS);
iotjs_blecentral_reqdata_t *iotjs_blecentral_reqwrap_data(THIS);
iotjs_blecentral_reqwrap_t *
iotjs_blecentral_reqwrap_from_request(uv_work_t *req);
void *iotjs_blecentral_req_get_userdata(uv_work_t *req);
void iotjs_blecentral_req_set_userdata(uv_work_t *req, void *data);
#undef THIS

void iotjs_blecentral_event_callback(BlecentralEv ev, void *args);

void BlecentralCreate();
void BlecentralDestroy();
void StartScanningWorker(uv_work_t *work_req);
void StopScanningWorker(uv_work_t *work_req);

#endif /* IOTJS_MODULE_BLECENTRAL_H */
