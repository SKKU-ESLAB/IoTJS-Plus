/* Copyright 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IOTJS_MODULE_BLECENTRAL_H
#define IOTJS_MODULE_BLECENTRAL_H

#include "iotjs_def.h"
#include "iotjs_objectwrap.h"
#include "iotjs_reqwrap.h"

typedef enum {
  kBleCentralOpStartScanning,
  kBleCentralOpStopScanning,
  kBleCentralOpConnect,
  kBleCentralOpDisconnect,
  kBleCentralOpDiscoverServices,
  kBleCentralOpReadHandle,
  kBleCentralOpWriteHandle,
  kBleCentralOpDiscoverIncludedServices,
  kBleCentralOpDiscoverCharacteristics,
  kBleCentralOpRead,
  kBleCentralOpWrite,
  kBleCentralOpBroadcast,
  kBleCentralOpNotify,
  kBleCentralOpDiscoverDescriptors,
  kBleCentralOpReadValue,
  kBleCentralOpWriteValue,
} BleCentralOp;

typedef enum {
  kBleCentralErrNone = 0,
  kBleCentralErrFail = -1,
}

typedef struct {
  BleCentralOp op;
  BleCentralErr err;
} iotjs_blecentral_reqdata_t;

typedef struct {
  uvwork_t req;
  iotjs_blecentral_reqdata_t req_data;
} IOTJS_VALIDATED_STRUCT(iotjs_blecentral_reqwrap_t);

typedef struct {
  iotjs_jobjectwrap_t jobjectwrap;
} IOTJS_VALIDATED_STRUCT(iotjs_blecentral_t);

iotjs_blecentral_t* iotjs_blecentral_create(const iotjs_jval_t* jble_central);
const iotjs_jval_t* iotjs_blecentral_get_jblecentral();
iotjs_blecentral_t* iotjs_blecentral_get_instance();

#endif /* IOTJS_MODULE_BLECENTRAL_H */
