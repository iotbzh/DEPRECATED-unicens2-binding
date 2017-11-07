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
#define I2C_MAX_DATA_SZ    32 /* max. number of bytes to be written to i2c */

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
#include <dirent.h> 

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
} ucsContextT;

typedef struct {
    struct afb_event node_event;
    
} EventData_t;

static ucsContextT *ucsContextS;
static EventData_t *eventData = NULL;

PUBLIC void UcsXml_CB_OnError(const char format[], uint16_t vargsCnt, ...) {
    /*AFB_DEBUG (afbIface, format, args); */
    va_list args;
    va_start (args, vargsCnt);
    vfprintf (stderr, format, args);
    va_end(args);
    
    va_list argptr;
    char outbuf[300];
    va_start(argptr, vargsCnt);
    vsprintf(outbuf, format, argptr);
    va_end(argptr);
    AFB_WARNING (outbuf);
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

void UCSI_CB_OnNetworkState(void *pTag, bool isAvailable, uint16_t packetBandwidth, uint8_t amountOfNodes)
{
}

/* UCS2 Interface Timer Callback */
PUBLIC void UCSI_CB_OnSetServiceTimer(void *pTag, uint16_t timeout) {
  uint64_t usec;
  /* set a timer with  250ms accuracy */
  sd_event_now(afb_daemon_get_event_loop(), CLOCK_BOOTTIME, &usec);
  sd_event_add_time(afb_daemon_get_event_loop(), NULL, CLOCK_MONOTONIC, usec + (timeout*1000), 250, onTimerCB, pTag);

}

/**
 * \brief Callback when ever an Unicens forms a human readable message.
 *        This can be error events or when enabled also debug messages.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param format - Zero terminated format string (following printf rules)
 * \param vargsCnt - Amount of parameters stored in "..."
 */
void UCSI_CB_OnUserMessage(void *pTag, bool isError, const char format[],
    uint16_t vargsCnt, ...) {
    va_list argptr;
    char outbuf[300];
    pTag = pTag;
    va_start(argptr, vargsCnt);
    vsprintf(outbuf, format, argptr);
    va_end(argptr);
    if (isError)
        AFB_NOTICE (outbuf);
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
   sd_event_add_time(afb_daemon_get_event_loop(), NULL, CLOCK_MONOTONIC, 0, 0, OnServiceRequiredCB, pTag);
}

/* Callback when ever this UNICENS wants to send a message to INIC. */
PUBLIC void UCSI_CB_OnTxRequest(void *pTag, const uint8_t *pData, uint32_t len) {
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
            /* Silently ignore write error (only occur in non-blocking mode) */
            break;
        }
        total += (uint32_t) written;
    }
}

/**
 * \brief Callback when UNICENS instance has been stopped.
 * \note This event can be used to free memory holding the resources
 *       passed with UCSI_NewConfig
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 */
void UCSI_CB_OnStop(void *pTag) {
    AFB_NOTICE ("UNICENS stopped");

}

/** This callback will be raised, when ever an applicative message on the control channel arrived */
void UCSI_CB_OnAmsMessageReceived(void *pTag)
{
	/* If not interested, just ignore this event.
	   Otherwise UCSI_GetAmsMessage may now be called asynchronous (mainloop) to get the content. 
	   Don't forget to call UCSI_ReleaseAmsMessage after that */
}

void UCSI_CB_OnRouteResult(void *pTag, uint16_t routeId, bool isActive, uint16_t connectionLabel)
{
}

void UCSI_CB_OnGpioStateChange(void *pTag, uint16_t nodeAddress, uint8_t gpioPinId, bool isHighState)
{
}

PUBLIC void UCSI_CB_OnMgrReport(void *pTag, Ucs_MgrReport_t code, uint16_t nodeAddress, Ucs_Rm_Node_t *pNode){

    bool available;
    
    if (code == UCS_MGR_REP_AVAILABLE) {
        available = true;
    }
    else if (code == UCS_MGR_REP_NOT_AVAILABLE) {
        available = false;
    }
    else {
        /*untracked event - just exit*/
        return;
    }
    
    if (eventData) {
        
        json_object *j_event_info = json_object_new_object();
        json_object_object_add(j_event_info, "node", json_object_new_int(nodeAddress));
        json_object_object_add(j_event_info, "available", json_object_new_boolean(available));
        
        afb_event_push(eventData->node_event, j_event_info);
    }     
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
    if (0 == len)
        return 0;
    ok= UCSI_ProcessRxData(&ucsContext->ucsiData, pBuffer, (uint16_t)len);
    if (!ok) {
        AFB_DEBUG ("Buffer overrun (not handle)");
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
        afb_req_fail_f (request, "fileread-fail", "File to read fullfile '%s' size(%d!=%d)", filename, (int)readSize, (int)fdStat.st_size);
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

PUBLIC void ucs2_initialise (struct afb_req request) {
    static UcsXmlVal_t *ucsConfig;
    static ucsContextT ucsContext;

    sd_event_source *evtSource;
    int err;

    /* Read and parse XML file */
    ucsConfig = ParseFile (request);
    if (NULL == ucsConfig) goto OnErrorExit;

    /* When ucsContextS is set, do not initalize UNICENS, CDEVs or system hooks, just load new XML */
    if (!ucsContextS)
    {
        if (!ucsContextS && !InitializeCdevs(&ucsContext))  {
            afb_req_fail_f (request, "devnit-error", "Fail to initialise device [rx=%s tx=%s]", CONTROL_CDEV_RX, CONTROL_CDEV_TX);
            goto OnErrorExit;
        }

        /* Initialise UNICENS Config Data Structure */
        UCSI_Init(&ucsContext.ucsiData, &ucsContext);

        /* register aplayHandle file fd into binder mainloop */
        err = sd_event_add_io(afb_daemon_get_event_loop(), &evtSource, ucsContext.rx.fileHandle, EPOLLIN, onReadCB, &ucsContext);
        if (err < 0) {
            afb_req_fail_f (request, "register-mainloop", "Cannot hook events to mainloop");
            goto OnErrorExit;
        }

        /* save this in a statical variable until ucs2vol move to C */
        ucsContextS = &ucsContext;
    }
    /* Initialise UNICENS with parsed config */
    if (!UCSI_NewConfig(&ucsContext.ucsiData, ucsConfig))   {
        afb_req_fail_f (request, "UNICENS-init", "Fail to initialize UNICENS");
        goto OnErrorExit;
    }

    afb_req_success(request,NULL,"UNICENS-active");

 OnErrorExit:
    return;
}


// List Avaliable Configuration Files
PUBLIC void ucs2_listconfig (struct afb_req request) {
    struct json_object *queryJ, *tmpJ, *responseJ;
    DIR  *dirHandle;
    char *dirPath, *dirList;
    int error=0;

    queryJ = afb_req_json(request);
    if (queryJ && json_object_object_get_ex (queryJ, "cfgpath" , &tmpJ)) {
        dirList = strdup (json_object_get_string(tmpJ));
    } else {    
        dirList = strdup (UCS2_CFG_PATH); 
        AFB_NOTICE ("fgpath:missing uses UCS2_CFG_PATH=%s", UCS2_CFG_PATH);
    }

    responseJ = json_object_new_array();
    for (dirPath= strtok(dirList, ":"); dirPath && *dirPath; dirPath=strtok(NULL,":")) {
         struct dirent *dirEnt;
         
        dirHandle = opendir (dirPath);
        if (!dirHandle) {
            AFB_NOTICE ("ucs2_listconfig dir=%s not readable", dirPath);
            error++;
            continue;
        } 
        
        AFB_NOTICE ("ucs2_listconfig scanning: %s", dirPath);
        while ((dirEnt = readdir(dirHandle)) != NULL) {
            // Unknown type is accepted to support dump filesystems
            if (dirEnt->d_type == DT_REG || dirEnt->d_type == DT_UNKNOWN) {
                struct json_object *pathJ = json_object_new_object();
                json_object_object_add(pathJ, "dirpath", json_object_new_string(dirPath));
                json_object_object_add(pathJ, "basename", json_object_new_string(dirEnt->d_name));
                json_object_array_add(responseJ, pathJ);
            }
        }
    }
    
    free (dirList);
   
    if (!error)  afb_req_success(request,responseJ,NULL);
    else {
        char info[40];
        snprintf (info, sizeof(info), "[%d] where not scanned", error); 
         afb_req_success(request,responseJ, info);
    } 
    
    return;
}

PUBLIC void ucs2_subscribe (struct afb_req request) {
    
    if (!eventData) {
        
        eventData = malloc(sizeof(EventData_t));
        if (eventData) {
            eventData->node_event = afb_daemon_make_event ("node-availibility");
        }
        
        if (!eventData || !afb_event_is_valid(eventData->node_event)) {
            afb_req_fail_f (request, "create-event", "Cannot create or register event");
            goto OnExitError;
        }
    }
    
    if (afb_req_subscribe(request, eventData->node_event) != 0) {
        
        afb_req_fail_f (request, "subscribe-event", "Cannot subscribe to event");
        goto OnExitError;
    }
    
    afb_req_success(request,NULL,"event subscription successful"); 
    
OnExitError:
    return;
}

STATIC void ucs2_writei2c_CB (void *result_ptr, void *request_ptr) {
    
    if (request_ptr){
        afb_req *req = (afb_req *)request_ptr;
        Ucs_I2c_ResultCode_t *res = (Ucs_I2c_ResultCode_t *)result_ptr;
        
        if (!res) {
            afb_req_fail(*req, "processing","busy or lost initialization");
        }
        else if (*res != UCS_I2C_RES_SUCCESS){
            afb_req_fail_f(*req, "error-result", "result code: %d", *res);
        }
        else {
            afb_req_success(*req, NULL, "success");
        }
        
        afb_req_unref(*req);
        free(request_ptr);
    } 
    else {
        AFB_NOTICE("write_i2c: ambiguous response data");
    }
}

/* write a single i2c command */
STATIC void ucs2_writei2c_cmd(struct afb_req request, json_object *j_obj) {
    
    static uint8_t i2c_data[I2C_MAX_DATA_SZ];
    uint8_t i2c_data_sz = 0;
    uint16_t node_addr = 0;
    struct afb_req *async_req_ptr = NULL;
    
    node_addr = (uint16_t)json_object_get_int(json_object_object_get(j_obj, "node"));
    AFB_NOTICE("node_address: 0x%02X", node_addr);
    
    if (node_addr == 0) {
        afb_req_fail_f(request, "query-params","params wrong or missing");
        goto OnErrorExit;
    }
       
    if (json_object_get_type(json_object_object_get(j_obj, "data"))==json_type_array) {
        int size = json_object_array_length(json_object_object_get(j_obj, "data"));
        if ((size > 0) && (size <= I2C_MAX_DATA_SZ)) {
            
            int32_t i;
            int32_t val;
            struct json_object *j_elem;
            struct json_object *j_arr = json_object_object_get(j_obj, "data");

            for (i = 0; i < size; i++) {
                
                
                j_elem = json_object_array_get_idx(j_arr, i);
                val = json_object_get_int(j_elem);
                if ((val < 0) && (val > 0xFF)){
                    i = 0;
                    break;
                }
                i2c_data[i] = (uint8_t)json_object_get_int(j_elem);
            }
            
            i2c_data_sz = (uint8_t)i;
        }
    }
    
    if (i2c_data_sz == 0) {
        AFB_NOTICE("data: invalid or not found");
        afb_req_fail_f(request, "query-params","params wrong or missing");
        goto OnErrorExit;
    }
   
    async_req_ptr = malloc(sizeof(afb_req));
    *async_req_ptr = request;
    
    if (UCSI_I2CWrite(  &ucsContextS->ucsiData,   /* UCSI_Data_t *pPriv*/
                        node_addr,                /* uint16_t targetAddress*/
                        false,                    /* bool isBurst*/
                        0u,                       /* block count */
                        0x2Au,                    /* i2c slave address */
                        0x03E8u,                  /* timeout 1000 milliseconds */
                        i2c_data_sz,              /* uint8_t dataLen */
                        &i2c_data[0],             /* uint8_t *pData */
                        &ucs2_writei2c_CB,        /* callback*/
                        (void*)async_req_ptr      /* callback argument */
                  )) {
        /* asynchronous command is running */
        afb_req_addref(request);
    }
    else {
        AFB_NOTICE("i2c write: scheduling command failed");
        afb_req_fail_f(request, "query-command-queue","command queue overload");
        free(async_req_ptr);
        async_req_ptr = NULL;
        goto OnErrorExit;
    }
    
OnErrorExit:
    return;
}

/* parse array or single command */
PUBLIC void ucs2_writei2c (struct afb_req request) {
    
    struct json_object *j_obj;
    
    /* check UNICENS is initialised */
    if (!ucsContextS) {
        afb_req_fail_f(request, "unicens-init","Should Load Config before using setvol");
        goto OnErrorExit;
    }

    j_obj = afb_req_json(request);
    if (!j_obj) {
        afb_req_fail_f(request, "query-notjson","query=%s not a valid json entry", afb_req_value(request,""));
        goto OnErrorExit;
    };
    
    AFB_DEBUG("request: %s", json_object_to_json_string(j_obj));
    
    if (json_object_get_type(j_obj)==json_type_array) {
        
        int cnt;
        int len = json_object_array_length(j_obj);
        
        if (len != 1) {
            afb_req_fail_f(request, "query-array","query of multiple commands is not supported");
            goto OnErrorExit;
        }
        
        for (cnt = 0; cnt < len; cnt++) {
            
            json_object *j_cmd = json_object_array_get_idx(j_obj, cnt);
            ucs2_writei2c_cmd(request, j_cmd);
        }
    }
    else {
        ucs2_writei2c_cmd(request, j_obj);
    }
    
 OnErrorExit:
    return;
}
