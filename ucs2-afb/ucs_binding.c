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
 *
 * references:
 *   https://gist.github.com/ghedo/963382
 *   http://alsa-utils.sourcearchive.com/documentation/1.0.15/aplay_8c-source.html
 */

#define _GNU_SOURCE

#define BUFFER_FRAME_COUNT 10 /* max frames in buffer */
#define WAIT_TIMER_US 1000000 /* default waiting timer 1s */

#include <systemd/sd-event.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <errno.h>

#include "ucs_binding.h"
#include "ucs_interface.h"



#define MAX_FILENAME_LEN (100)
#define RX_BUFFER (64)

/** Internal structure, enabling multiple instances of this component.
 * \note Do not access any of this variables.
 *  */
typedef struct {
    int fileHandle;
    int fileFlags;
    char fileName[MAX_FILENAME_LEN];
    uint8_t rxBuffer[RX_BUFFER];
    uint32_t rxLen;
} CdevData_t;


typedef struct {
  CdevData_t rx;
  CdevData_t tx;
  UCSI_Data_t ucsiData;
  UCSI_channelsT *channels;
} ucsContextT;

static ucsContextT *ucsContextS;

PUBLIC void UcsXml_CB_OnError(const char format[], uint16_t vargsCnt, ...) {
    /*DEBUG (afbIface, format, args); */
    va_list args;
    va_start (args, vargsCnt);
    vfprintf (stderr, format, args);
    va_end(args);
}

PUBLIC uint16_t UCSI_CB_OnGetTime(void *pTag) {
    struct timespec currentTime;
    uint16_t timer;
    pTag = pTag;

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &currentTime))   {
        assert(false);
        return 0;
    }

    timer = (uint16_t) ((currentTime.tv_sec * 1000 ) + ( currentTime.tv_nsec / 1000000 ));
    return(timer);
}

STATIC int onTimerCB (sd_event_source* source,uint64_t timer, void* pTag) {
    ucsContextT *ucsContext = (ucsContextT*) pTag;

    sd_event_source_unref(source);
    UCSI_Timeout(&ucsContext->ucsiData);

    return 0;
}

/* UCS2 Interface Timer Callback */
PUBLIC void UCSI_CB_OnSetServiceTimer(void *pTag, uint16_t timeout) {
  uint64_t usec;
  /* set a timer with  250ms accuracy */
  sd_event_now(afb_daemon_get_event_loop(afbIface->daemon), CLOCK_BOOTTIME, &usec);
  sd_event_add_time(afb_daemon_get_event_loop(afbIface->daemon), NULL, CLOCK_MONOTONIC, usec + (timeout*1000), 250, onTimerCB, pTag);

}

/** Callback when ever UNICENS forms a human readable message. */
void UCSI_CB_OnUserMessage(void *pTag, const char format[],
    uint16_t vargsCnt, ...) {
    //DEBUG (afbIface, format, args);
    va_list args;
    va_start (args, format);
    vfprintf (stderr, format, args);
    va_end(args);
}

/** UCSI_Service cannot be called directly within UNICENS context, need to service stack through mainloop */
STATIC int OnServiceRequiredCB (sd_event_source *source, uint64_t usec, void *pTag) {
    ucsContextT *ucsContext = (ucsContextT*) pTag;

    sd_event_source_unref(source);
    UCSI_Service(&ucsContext->ucsiData);
    return (0);
}

/* UCS Callback fire when ever UNICENS needs to be serviced */
PUBLIC void UCSI_CB_OnServiceRequired(void *pTag) {

   /* push an asynchronous request for loopback to call UCSI_Service */
   sd_event_add_time(afb_daemon_get_event_loop(afbIface->daemon), NULL, CLOCK_MONOTONIC, 0, 0, OnServiceRequiredCB, pTag);
}


/**
 * \brief Callback when ever a MOST error message was received.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \note All following parameters belong to the usual MOST message
 */
PUBLIC void UCSI_CB_OnMostError(void *pTag, uint16_t sourceAddr,
    uint8_t fblock, uint8_t inst, uint16_t function, uint8_t op,
    const uint8_t *pPayload, uint32_t payloadLen) {

    // Error to send to syslog
    DEBUG (afbIface, "OnMostError source=0x%x", sourceAddr);
}



/* Callback when ever this UNICENS wants to send a message to INIC. */
PUBLIC void UCSI_CB_SendMostMessage(void *pTag, const uint8_t *pData, uint32_t len) {

    ucsContextT *ucsContext = (ucsContextT*) pTag;
    CdevData_t *cdevTx = &ucsContext->tx;
    uint32_t total = 0;


    if (NULL == pData || 0 == len) return;

    if (O_RDONLY == cdevTx->fileFlags) return;
    if (-1 == cdevTx->fileHandle)
        cdevTx->fileHandle = open(cdevTx->fileName, cdevTx->fileFlags);
    if (-1 == cdevTx->fileHandle)
        return;

    while(total < len) {
        ssize_t written = write(cdevTx->fileHandle, &pData[total], (len - total));
        if (0 >= written)
        {
            cdevTx->fileHandle = -1;
            return;
        }
        total += (uint32_t) written;
    }

    return;
}

/**
 * \brief Callback when UNICENS instance has been stopped.
 * \note This event can be used to free memory holding the resources
 *       passed with UCSI_NewConfig
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 */
void UCSI_CB_OnStop(void *pTag) {
    NOTICE (afbIface, "UNICENS stopped");

}

/**
 * \brief Callback on Unicens management results.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param code - Result code
 * \param nodeAddress - Node address of the device causing this event
 * \param pNode - Pointer to node structure holding details of changed node
 */
extern void UCSI_CB_OnMgrReport(void *pTag, Ucs_MgrReport_t code, uint16_t nodeAddress, Ucs_Rm_Node_t *pNode) {

    DEBUG (afbIface, "OnMgrReport: Ucs_MgrReport_t=%d nodeAdresse=0x%x", code, nodeAddress);
}

bool Cdev_Init(CdevData_t *d, const char *fileName, bool read, bool write)
{
    if (NULL == d || NULL == fileName)  goto OnErrorExit;

    memset(d, 0, sizeof(CdevData_t));
    strncpy(d->fileName, fileName, MAX_FILENAME_LEN);
    d->fileHandle = -1;

    if (read && write)
        d->fileFlags = O_RDWR | O_NONBLOCK;
    else if (read)
        d->fileFlags = O_RDONLY | O_NONBLOCK;
    else if (write)
        d->fileFlags = O_WRONLY | O_NONBLOCK;

    /* open file to enable event loop */
    d->fileHandle = open(d->fileName, d->fileFlags);
    if (d->fileHandle  <= 0) goto OnErrorExit;

    return true;

 OnErrorExit:
    return false;
}

static bool InitializeCdevs(ucsContextT *ucsContext)
{
    if(!Cdev_Init(&ucsContext->tx, CONTROL_CDEV_TX, false, true))
        return false;
    if(!Cdev_Init(&ucsContext->rx, CONTROL_CDEV_RX, true, false))
        return false;
    return true;
}

/* Callback fire when something is avaliable on MOST cdev */
int onReadCB (sd_event_source* src, int fileFd, uint32_t revents, void* pTag) {
    ucsContextT *ucsContext =( ucsContextT*) pTag;
    ssize_t len;
    uint8_t pBuffer[RX_BUFFER];
    int ok;

    len = read (ucsContext->rx.fileHandle, &pBuffer, sizeof(pBuffer));

    ok= UCSI_ProcessRxData(&ucsContext->ucsiData, pBuffer, (uint16_t)len);
    if (!ok) {
        DEBUG (afbIface, "Buffer overrun (not handle)");
        /* Buffer overrun could replay pBuffer */
    }
    return 0;
}

STATIC UcsXmlVal_t* ParseFile(struct afb_req request) {
    char *xmlBuffer;
    ssize_t readSize;
    int fdHandle ;
    struct stat fdStat;
    UcsXmlVal_t* ucsConfig;

    const char *filename = afb_req_value(request, "filename");
    if (!filename) {
        afb_req_fail_f (request, "filename-missing", "No filename given");
        goto OnErrorExit;
    }

    fdHandle = open(filename, O_RDONLY);
    if (fdHandle <= 0) {
        afb_req_fail_f (request, "fileread-error", "File not accessible: '%s' err=%s", filename, strerror(fdHandle));
        goto OnErrorExit;
    }

    /* read file into buffer as a \0 terminated string */
    fstat(fdHandle, &fdStat);
    xmlBuffer = (char*)alloca(fdStat.st_size + 1);
    readSize = read(fdHandle, xmlBuffer, fdStat.st_size);
    close(fdHandle);
    xmlBuffer[readSize] = '\0'; /* In any case, terminate it. */

    if (readSize != fdStat.st_size)  {
        afb_req_fail_f (request, "fileread-fail", "File to read fullfile '%s' size(%d!=%d)", filename, readSize, fdStat.st_size);
        goto OnErrorExit;
    }

    ucsConfig = UcsXml_Parse(xmlBuffer);
    if (!ucsConfig)  {
        afb_req_fail_f (request, "filexml-error", "File XML invalid: '%s'", filename);
        goto OnErrorExit;
    }

    return (ucsConfig);

 OnErrorExit:
    return NULL;
}

STATIC int volOnSvcCB (sd_event_source* source,uint64_t timer, void* pTag) {
    ucsContextT *ucsContext = (ucsContextT*) pTag;

    sd_event_source_unref(source);
    UCSI_Vol_Service(&ucsContext->ucsiData);

    return 0;
}

/* This callback is fire each time an volume event wait in the queue */
void volumeCB (uint16_t timeout) {
    uint64_t usec;
    sd_event_now(afb_daemon_get_event_loop(afbIface->daemon), CLOCK_BOOTTIME, &usec);
    sd_event_add_time(afb_daemon_get_event_loop(afbIface->daemon), NULL, CLOCK_MONOTONIC, usec + (timeout*1000), 250, volOnSvcCB, ucsContextS);
}

STATIC int volSndCmd (struct afb_req request, struct json_object *commandJ, ucsContextT *ucsContext) {
    int numid, vol, err;
    struct json_object *nameJ, *channelJ, *volJ;

    enum json_type jtype= json_object_get_type(commandJ);
    switch (jtype) {
        case json_type_array:
            if (!sscanf (json_object_get_string (json_object_array_get_idx(commandJ, 0)), "%d", &numid)) {
                afb_req_fail_f (request, "channel-invalid","command=%s channel is not an integer", json_object_get_string (channelJ));
                goto OnErrorExit;
            }
            if (!sscanf (json_object_get_string (json_object_array_get_idx(commandJ, 1)), "%d", &vol)) {
                afb_req_fail_f (request, "vol-invalid","command=%s vol is not an integer", json_object_get_string (channelJ));
                goto OnErrorExit;
            }
            break;

        case json_type_object:
            if (json_object_object_get_ex (commandJ, "numid", &channelJ)) {
                if (!sscanf (json_object_get_string (channelJ), "%d", &numid)) {
                    afb_req_fail_f (request, "channel-invalid","command=%s numid is not an integer", json_object_get_string (channelJ));
                    goto OnErrorExit;
                }
            } else {
                if (json_object_object_get_ex (commandJ, "channel", &nameJ)) {
                    int idx;
                    const char *name = json_object_get_string(nameJ);

                    for (idx =0; ucsContext->channels[idx].name != NULL; idx++) {
                        if (!strcasecmp(ucsContext->channels[idx].name, name)) {
                            numid = ucsContext->channels[idx].numid;
                            break;
                        }
                    }
                    if (ucsContext->channels[idx].name == NULL) {
                        afb_req_fail_f (request, "channel-invalid","command=%s channel name does not exist", name);
                        goto OnErrorExit;
                    }
                } else {
                    afb_req_fail_f (request, "channel-invalid","command=%s no valid channel name or channel", json_object_get_string(commandJ));
                    goto OnErrorExit;
                };
            }

            if (!json_object_object_get_ex (commandJ, "volume", &volJ)) {
                afb_req_fail_f (request, "vol-missing","command=%s vol not present", json_object_get_string (commandJ));
                goto OnErrorExit;
            }

            if (!sscanf (json_object_get_string (volJ), "%d", &vol)) {
                afb_req_fail_f (request, "vol-invalid","command=%s vol:%s is not an integer", json_object_get_string (commandJ), json_object_get_string (volJ));
                goto OnErrorExit;
            }

            break;

        default:
            afb_req_fail_f (request, "setvol-invalid","command=%s not valid JSON Volume Command", json_object_get_string(commandJ));
            goto OnErrorExit;
    }


    /* Fulup what's append when channel or vol are invalid ??? */
    err = UCSI_Vol_Set  (&ucsContext->ucsiData, numid, (uint8_t) vol);
    if (err) {
        /* Fulup this might only be a warning (not sure about it) */
        afb_req_fail_f (request, "vol-refused","command=%s vol was refused by UNICENS", json_object_get_string (volJ));
        goto OnErrorExit;
    }

    return 0;

  OnErrorExit:
    return 1;
}


PUBLIC void ucs2SetVol (struct afb_req request) {
    struct json_object *queryJ;
    int err;

    /* check UNICENS is initialised */
    if (!ucsContextS) {
        afb_req_fail_f (request, "UNICENS-init","Should Load Config before using setvol");
        goto OnErrorExit;
    }

    queryJ = afb_req_json(request);
    if (!queryJ) {
        afb_req_fail_f (request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        goto OnErrorExit;
    };

    enum json_type jtype= json_object_get_type(queryJ);
    switch (jtype) {
        case json_type_array:
            for (int idx=0; idx < json_object_array_length (queryJ); idx ++) {
               err= volSndCmd (request, json_object_array_get_idx (queryJ, idx), ucsContextS);
               if (err) goto OnErrorExit;
            }
            break;

        case json_type_object:
            err = volSndCmd (request, queryJ, ucsContextS);
            if (err) goto OnErrorExit;
            break;

        default:
            afb_req_fail_f (request, "query-notarray","query=%s not valid JSON Volume Command Array", afb_req_value(request,""));
            goto OnErrorExit;
    }


    afb_req_success(request,NULL,NULL);

 OnErrorExit:
    return;
}


PUBLIC void ucs2Init (struct afb_req request) {
    static UcsXmlVal_t *ucsConfig;
    static ucsContextT ucsContext;

    sd_event_source *evtSource;
    int err;

    /* Read and parse XML file */
    ucsConfig = ParseFile (request);
    if (NULL == ucsConfig) goto OnErrorExit;

    // Fulup->Thorsten BUG InitializeCdevs should fail when control does not exit
    if (!InitializeCdevs(&ucsContext))  {
        afb_req_fail_f (request, "devnit-error", "Fail to initialise device [rx=%s tx=%s]", CONTROL_CDEV_RX, CONTROL_CDEV_TX);
        goto OnErrorExit;
    }

    // Initialise Unicens Config Data Structure
    UCSI_Init(&ucsContext.ucsiData, &ucsContext);

    // Initialise Unicens with parsed config
    if (!UCSI_NewConfig(&ucsContext.ucsiData, ucsConfig))   {
        afb_req_fail_f (request, "UNICENS-init", "Fail to initialize Unicens");
        goto OnErrorExit;
    }

    // register aplayHandle file fd into binder mainloop
    err = sd_event_add_io(afb_daemon_get_event_loop(afbIface->daemon), &evtSource, ucsContext.rx.fileHandle, EPOLLIN, onReadCB, &ucsContext);
    if (err < 0) {
        afb_req_fail_f (request, "register-mainloop", "Cannot hook events to mainloop");
        goto OnErrorExit;
    }

    // init Unicens Volume Library
    ucsContext.channels = UCSI_Vol_Init (&ucsContext.ucsiData, volumeCB);
    if (!ucsContext.channels) {
        afb_req_fail_f (request, "register-volume", "Could not enqueue new Unicens config");
        goto OnErrorExit;
    }
    // save this in a statical variable until ucs2vol move to C
    ucsContextS = &ucsContext;

    afb_req_success(request,NULL,"UNICENS-active");

 OnErrorExit:
    return;
}
