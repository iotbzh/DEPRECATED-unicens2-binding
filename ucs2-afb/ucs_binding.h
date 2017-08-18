/*
 * AlsaLibMapping -- provide low level interface with AUDIO lib (extracted from alsa-json-gateway code)
 * Copyright (C) 2015,2016,2017, Fulup Ar Foll fulup@iot.bzh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef UCS2BINDING_H
#define UCS2BINDING_H

// Use Version of AGL Application Framework API
#define AFB_BINDING_VERSION 2

#ifndef PUBLIC
  #define PUBLIC
#endif
#define STATIC static

#include <json-c/json.h>
#include <afb/afb-binding.h>

#include "ucs_interface.h"

#ifndef CONTROL_CDEV_TX
#error FATAL: CONTROL_CDEV_TX missing (check ./etc/config.cmake + rerun cmake)
#endif
#ifndef CONTROL_CDEV_RX
#error FATAL: CONTROL_CDEV_RX missing (check ./etc/config.cmake + rerun cmake)
#endif

// import from AlsaAfbBinding
extern const struct afb_binding_interface *afbIface;
extern struct afb_service afbSrv;

// API verbs prototype
PUBLIC void ucs2_configure (struct afb_req request);
PUBLIC void ucs2_subscribe (struct afb_req request);
PUBLIC void ucs2_writei2c  (struct afb_req request);

#endif /* UCS2BINDING_H */

