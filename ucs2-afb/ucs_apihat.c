/*
 * Copyright (C) 2016 "IoT.bzh"
 * Author Fulup Ar Foll <fulup@iot.bzh>
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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <math.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ucs_binding.h"

const struct afb_binding_interface *afbIface;
struct afb_service afbSrv;

/*
 * array of the verbs exported to afb-daemon
 */
static const struct afb_verb_desc_v1 binding_verbs[] = {
  /* VERB'S NAME            SESSION MANAGEMENT          FUNCTION TO CALL         SHORT DESCRIPTION */
  { .name= "initialise", .session= AFB_SESSION_NONE,  .callback= ucs2Init,      .info= "Parse XML & initialise Unicens " },
  { .name= "setvol"    , .session= AFB_SESSION_NONE,  .callback= ucs2SetVol,    .info= "Set Volume" },
 //  { .name= "monitor"   , .session= AFB_SESSION_NONE,  .callback= ucs2Monitor,   .info= "Subscribe to network error" },


  { .name= NULL } /* marker for end of the array */
};

/*
 * description of the binding for afb-daemon
 */
static const struct afb_binding binding_description = {
  /* description conforms to VERSION 1 */
  .type= AFB_BINDING_VERSION_1,
  .v1= {
    .prefix= "UNICENS",
    .info= "UNICENS MOST Control API",
    .verbs = binding_verbs
  }
};

// this is call when after all bindings are loaded
 int afbBindingV1ServiceInit(struct afb_service service) {
   afbSrv =  service;
   return (0);
}

/*
 * activation function for registering the binding called by afb-daemon
 */
 const struct afb_binding *afbBindingV1Register(const struct afb_binding_interface *itf) {
    afbIface= itf;

    return &binding_description;	/* returns the description of the binding */
}

