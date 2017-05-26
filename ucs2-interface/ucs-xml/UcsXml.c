/*
 * Unicens XML Parser
 *
 * Copyright (C) 2017 Microchip Technology Germany II GmbH & Co. KG
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You may also obtain this software under a propriety license from Microchip.
 * Please contact Microchip for further information.
 *
 */
#include <assert.h>
#include <string.h>
#include "mxml.h"
#include "UcsXml.h"

/************************************************************************/
/* USER ADJUSTABLE CONSTANTS                                            */
/************************************************************************/

#define MAX_JOB_LIST_LEN 6

/************************************************************************/
/* PRIVATE DECLARATIONS                                                 */
/************************************************************************/

#define COMPILETIME_CHECK(cond)  (void)sizeof(int[!!(cond) - 1])

struct UcsXmlObjectList
{
    void *obj;
    struct UcsXmlObjectList *next;
};

struct UcsXmlRouteInfo
{
    bool isSource;
    char routeName[32];
    Ucs_Rm_EndPoint_t *ep;
    struct UcsXmlRouteInfo *next;
};

struct UcsXmlScriptInfo
{
    char scriptName[32];
    Ucs_Rm_Node_t *node;
    struct UcsXmlScriptInfo *next;
};

typedef enum
{
    SYNC_DATA        = 0,      /*!< \brief  Specifies the synchronous streaming data type */
    CONTROL_DATA     = 2,      /*!< \brief  Specifies the control data type */
    AV_PACKETIZED    = 3,      /*!< \brief  Specifies the A/V Packetized Isochronous 
                                            streaming data type */
    QOS_IP           = 4,      /*!< \brief  Specifies the Quality of Service IP 
                                            streaming data type*/
    DISC_FRAME_PHASE = 5,      /*!< \brief  Specifies the DiscreteFrame Isochronous
                                            streaming phase data type */
    IPC_PACKET       = 7,       /*!< \brief Specifies the IPC packet data type */
    INVALID          = 0xFF     /*!< \brief Defined invalid value */
} MData_t;

typedef enum
{
    MPORT_MOST,
    MPORT_USB,
    MPORT_MLB,
    MPORT_I2S,
    MPORT_INVALID = 0xFF
} MPort_t;

typedef enum
{
    Parse_Success = 10,
    Parse_MemoryError,
    Parse_XmlError
} ParseResult_t;

typedef struct {
    struct UcsXmlObjectList objList;
    struct UcsXmlRouteInfo *pRtLst;
    struct UcsXmlScriptInfo *pScrLst;
    Ucs_Rm_Node_t *nod;
    Ucs_Xrm_ResObject_t *inSocket;
    Ucs_Xrm_ResObject_t *outSocket;
    Ucs_Xrm_ResObject_t **jobList;
    MData_t dataType;
    uint16_t blockWidth;
    const char* chName;
    uint16_t pause;
} PrivateData_t;

/************************************************************************/
/* Constants                                                            */
/************************************************************************/

//Key section
static const char* UNICENS =                "unicens";
static const char* ASYNC_BANDWIDTH =        "async_bandwidth";
static const char* CHANNEL =                "channel";
static const char* CHANNEL_NAME =           "channel_name";
static const char* ADDRESS =                "channel_addr";
static const char* BANDWIDTH =              "bandwidth";
static const char* OFFSET =                 "offset";
static const char* DATA_TYPE =              "data_type";
static const char* DEVICE =                 "device";
static const char* DEVICE_MLB_SPEED =       "mlb_port_speed";
static const char* NODE_ADDR =              "node_address";
static const char* DIR =                    "dir";
static const char* I2S_PIN =                "i2s_pin";
static const char* PACKETS_XACT =           "packets_per_xact";
static const char* PORT =                   "port";
static const char* SOCKET =                 "socket";

static const char* I2S_FS_SPEED =           "i2s_fs_speed";
static const char* I2S_ALIGN =              "i2s_align";

//value section
static const char* PORT_MOST =              "MOST";
static const char* PORT_USB =               "USB";
static const char* PORT_MLB =               "MLB";
static const char* PORT_I2S =               "I2S";

static const char* DATATYPE_SYNC =          "SYNC";
static const char* DATATYPE_AVP =           "AVP";
static const char* DATATYPE_CTRL =          "CTRL";
static const char* DATATYPE_QOS =           "QOS";
static const char* DATATYPE_DFP =           "DFP";
static const char* DATATYPE_IPC =           "IPC";

static const char* DIR_IN =                 "IN";
static const char* DIR_OUT =                "OUT";

static const char* I2S_PIN_SRXA0 =          "SRXA0";
static const char* I2S_PIN_SRXA1 =          "SRXA1";
static const char* I2S_PIN_SRXB0 =          "SRXB0";
static const char* I2S_PIN_SRXB1 =          "SRXB1";

static const char* I2S_ALIGN_L16 =          "Left16";
static const char* I2S_ALIGN_L24 =          "Left24";
static const char* I2S_ALIGN_R16 =          "Right16";
static const char* I2S_ALIGN_R24 =          "Right24";
static const char* I2S_ALIGN_SEQUENTIAL =   "Seq";

static const char* SCRIPT =                 "script";
static const char* ACTION =                 "action";
static const char* NAME =                   "name";
static const char* TYPE =                   "type";
static const char* FBLOCK_ID =              "fblock_id";
static const char* FUNCTION_ID =            "function_id";
static const char* OP_TYPE_REQUEST =        "op_request";
static const char* OP_TYPE_RESPONSE =       "op_response";
static const char* PAYLOAD_REQ_HEX =        "load_req_hex";
static const char* PAYLOAD_RES_HEX =        "load_res_hex";
static const char* PAUSE_MS =               "pause_ms";

static const char* SEND_MSG =               "SEND_MSG";
static const char* PAUSE =                  "PAUSE";

/************************************************************************/
/* Private Function Prototypes                                          */
/************************************************************************/

static void *MCalloc(struct UcsXmlObjectList *list, uint32_t nElem, uint32_t elemSize);
static void FreeObjList(struct UcsXmlObjectList *cur);
static void FreeVal(UcsXmlVal_t *v);
static bool GetElement(mxml_node_t *element, const char *name, bool goDeep, mxml_node_t **out, bool mandatory);
static bool GetCount(mxml_node_t *element, const char *name, uint32_t *out, bool mandatory);
static bool GetString(mxml_node_t *element, const char *key, const char **out, bool mandatory);
static bool GetUInt16(mxml_node_t *element, const char *key, uint16_t *out, bool mandatory);
static bool GetUInt8(mxml_node_t *element, const char *key, uint8_t *out, bool mandatory);
static bool GetMlbSpeed(mxml_node_t *element, const char *key, Ucs_Mlb_ClockConfig_t *clock, bool mandatory);
static bool GetI2SSpeed(mxml_node_t *element, const char *key, Ucs_Stream_PortClockConfig_t *clock, bool mandatory);
static bool GetI2SPin(mxml_node_t *element, Ucs_Stream_PortPinId_t *pin, uint8_t *portIndex, bool mandatory);
static bool GetI2SAlignment(mxml_node_t *element, Ucs_Stream_PortDataAlign_t *align, bool mandatory);
static bool GetDirection(mxml_node_t *element, Ucs_SocketDirection_t *out);
static bool GetDataType(mxml_node_t *element, MData_t *out);
static bool GetPort(mxml_node_t *element, MPort_t *out);
static bool GetPayload(mxml_node_t *element, const char *name, uint8_t **pPayload, uint8_t *len, 
            struct UcsXmlObjectList *obj, bool mandatory);
static bool AddJob(Ucs_Xrm_ResObject_t **joblist, Ucs_Xrm_ResObject_t *job);
static void AddRoute(struct UcsXmlRouteInfo **pRtLst, struct UcsXmlRouteInfo *route);
static void AddScript(struct UcsXmlScriptInfo **pScrLst, struct UcsXmlScriptInfo *script);
static ParseResult_t ParseAll(mxml_node_t *tree, UcsXmlVal_t *v, PrivateData_t *vp);
static ParseResult_t ParseDevice(mxml_node_t * dev, PrivateData_t *vp);
static ParseResult_t ParseChannel(mxml_node_t * ch, PrivateData_t *vp);
static ParseResult_t ParseSocket(mxml_node_t *soc, PrivateData_t *vp);
static ParseResult_t ParseMostSoc(Ucs_Xrm_MostSocket_t *mostSoc, mxml_node_t *soc, PrivateData_t *vp);
static ParseResult_t ParseUsbSoc(Ucs_Xrm_UsbSocket_t *usbSoc, mxml_node_t *soc, PrivateData_t *vp);
static ParseResult_t ParseMlbSoc(Ucs_Xrm_MlbSocket_t *mlbSoc, mxml_node_t *soc, PrivateData_t *vp);
static ParseResult_t ParseStreamSoc(Ucs_Xrm_StrmSocket_t *strmSoc, mxml_node_t *soc, PrivateData_t *vp);
static ParseResult_t ParseScript(mxml_node_t *scr, PrivateData_t *vp);
static ParseResult_t ParseScriptAction(mxml_node_t *act, Ucs_Rm_Node_t *n, uint32_t index, PrivateData_t *vp);
static ParseResult_t ParseRoutes(UcsXmlVal_t *v, PrivateData_t *vp);

/************************************************************************/
/* Public Functions                                                     */
/************************************************************************/

UcsXmlVal_t *UcsXml_Parse(const char *xmlString)
{
    UcsXmlVal_t *v;
    ParseResult_t result = Parse_Success;
    mxml_node_t *tree = mxmlLoadString(NULL, xmlString, MXML_NO_CALLBACK);
    if (!tree)
    {
        result = Parse_XmlError;
    }
    if (Parse_Success == result)
    {
        if (!GetElement(tree, UNICENS, true, &tree, true))
            result = Parse_XmlError;
    }
    if (Parse_Success == result)
    {
        //Do not use MCalloc for the root element
        v = calloc(1, sizeof(UcsXmlVal_t));
        if (NULL == v) result = Parse_MemoryError;
    }
    if (Parse_Success == result)
    {
        //Do not use MCalloc for the private data
        v->pInternal = calloc(1, sizeof(PrivateData_t));
        if (NULL == v->pInternal) result = Parse_MemoryError;
    }
    if (Parse_Success == result)
    {
        result = ParseAll(tree, v, v->pInternal);
    }
    if (!tree)
    {
        mxmlDelete(tree);
    }
    if (Parse_Success == result)
    {
        return v;
    }
    if (Parse_MemoryError == result)
    {
        UcsXml_CB_OnError("XML error, aborting..", 0);
    }
    else
    {
        UcsXml_CB_OnError("Alloc error, aborting..", 0);
    }
    assert(false);
    FreeVal(v);
    return NULL;
}

void UcsXml_FreeVal(UcsXmlVal_t *val)
{
    FreeVal(val);
}

/************************************************************************/
/* Private Function Implementations                                     */
/************************************************************************/

static void *MCalloc(struct UcsXmlObjectList *list, uint32_t nElem, uint32_t elemSize)
{
    void *obj;
    struct UcsXmlObjectList *tail = list;
    if (NULL == list || 0 == nElem || 0 == elemSize) return NULL;
    
    obj = calloc(nElem, elemSize);
    if (NULL == obj)
    {
        assert(false);
        return NULL;
    }
    if (NULL == list->obj)
    {
        list->obj = obj;
        return obj;
    }
    while(tail->next) tail = tail->next;
    tail->next = calloc(1, sizeof(struct UcsXmlObjectList));
    if (NULL == tail->next)
    {
        assert(false);
        free(obj);
        return NULL;
    }
    tail->next->obj = obj;
    return obj;
}

static void FreeObjList(struct UcsXmlObjectList *cur)
{
    struct UcsXmlObjectList *root = cur;
    while(cur)
    {
        struct UcsXmlObjectList *next = cur->next;
        assert(NULL != cur->obj);
        if (cur->obj)
            free(cur->obj);
        if (cur != root)
            free(cur);
        cur = next;
    }
}

static void FreeVal(UcsXmlVal_t *v)
{
    PrivateData_t *vp;
    if (NULL == v || NULL == v->pInternal)
        return;
    vp = v->pInternal;
    FreeObjList(&vp->objList);
    free(v->pInternal);
    free(v);
}

static bool GetElement(mxml_node_t *element, const char *name, bool goDeep, mxml_node_t **out, bool mandatory)
{
    mxml_node_t *n = element;
    if (NULL == n || NULL == name || NULL == out) return false;
    if (goDeep)
    {
        *out = mxmlFindElement(n, n, name, NULL, NULL, MXML_DESCEND);
        return (NULL != *out);
    }
    while ((n = n->next))
    {
        if (MXML_ELEMENT != n->type)
            continue;
        if (0 == strcmp(name, n->value.opaque))
        {
            *out = n;
            return true;
        }
    }
    if (mandatory)
        UcsXml_CB_OnError("Can not find tag <%s>", 1, name);
    return false;
}

static bool GetCount(mxml_node_t *element, const char *name, uint32_t *out, bool mandatory)
{
    uint32_t cnt = 0;
    mxml_node_t *n;
    if (NULL == element || NULL == name) return false;
    if(!GetElement(element, name, true, &n, false))
        return false;
    while(NULL != n)
    {
        ++cnt;
        if(!GetElement(n, name, false, &n, false))
            break;
    }
    if (mandatory && 0 == cnt)
    {
        UcsXml_CB_OnError("element count of <%s> is zero", 1, name);
        return false;
    }
    *out = cnt;
    return true;
}

static bool GetString(mxml_node_t *element, const char *key, const char **out, bool mandatory)
{
    uint32_t i;
    if (NULL == element || NULL == key) return false;
    for (i = 0; i < element->value.element.num_attrs; i++)
    {
        mxml_attr_t *attr = &element->value.element.attrs[i];
        if (0 == strcmp(key, attr->name))
        {
            *out = attr->value;
            return true;
        }
    }
    if (mandatory)
        UcsXml_CB_OnError("Can not find attribute='%s' from element <%s>", 
            2, key, element->value.element.name);
    return false;
}

static bool GetUInt16(mxml_node_t *element, const char *key, uint16_t *out, bool mandatory)
{
    const char* txt;
    if (!GetString(element, key, &txt, mandatory)) return false;
    *out = strtol( txt, NULL, 0 );
    return true;
}

static bool GetUInt8(mxml_node_t *element, const char *key, uint8_t *out, bool mandatory)
{
    const char* txt;
    if (!GetString(element, key, &txt, mandatory)) return false;
    *out = strtol( txt, NULL, 0 );
    return true;
}

static bool GetMlbSpeed(mxml_node_t *element, const char *key, Ucs_Mlb_ClockConfig_t *clock, bool mandatory)
{
    uint16_t speed;
    if (!GetUInt16(element, DEVICE_MLB_SPEED, &speed, false))
        return false;
    switch(speed)
    {
    case 256: *clock = UCS_MLB_CLK_CFG_256_FS; break;
    case 512: *clock = UCS_MLB_CLK_CFG_512_FS; break;
    case 1024: *clock = UCS_MLB_CLK_CFG_1024_FS; break;
    case 2048: *clock = UCS_MLB_CLK_CFG_2048_FS; break;
    case 3072: *clock = UCS_MLB_CLK_CFG_3072_FS; break;
    case 4096: *clock = UCS_MLB_CLK_CFG_4096_FS; break;
    case 6144: *clock = UCS_MLB_CLK_CFG_6144_FS; break;
    case 8192: *clock = UCS_MLB_CLK_CFG_8192_FS; break;
    case 0: *clock = UCS_MLB_CLK_CFG_WILDCARD; break;
    default: 
        UcsXml_CB_OnError("Invalid MLB clock val:'%d'", 1, clock);
        return false;
    }
    return true;
}

static bool GetI2SSpeed(mxml_node_t *element, const char *key, Ucs_Stream_PortClockConfig_t *clock, bool mandatory)
{
    uint16_t speed;
    if (!GetUInt16(element, I2S_FS_SPEED, &speed, false))
        return false;
    switch(speed)
    {
    case 8: *clock = UCS_STREAM_PORT_CLK_CFG_8FS; break;
    case 16: *clock = UCS_STREAM_PORT_CLK_CFG_16FS; break;
    case 32: *clock = UCS_STREAM_PORT_CLK_CFG_32FS; break;
    case 64: *clock = UCS_STREAM_PORT_CLK_CFG_64FS; break;
    case 128: *clock = UCS_STREAM_PORT_CLK_CFG_128FS; break;
    case 256: *clock = UCS_STREAM_PORT_CLK_CFG_256FS; break;
    case 512: *clock = UCS_STREAM_PORT_CLK_CFG_512FS; break;
    case 0: *clock = UCS_MLB_CLK_CFG_WILDCARD; break;
    default: 
        UcsXml_CB_OnError("Invalid I2S clock val:'%d'", 1, clock);
        return false;
    }
    return true;
}

static bool GetI2SPin(mxml_node_t *element, Ucs_Stream_PortPinId_t *pin, uint8_t *portIndex, bool mandatory)
{
    const char *txt;
    if (!GetString(element, I2S_PIN, &txt, true))
        return false;
    if (0 == strcmp(I2S_PIN_SRXA0, txt))
    {
        *pin = UCS_STREAM_PORT_PIN_ID_SRXA0;
        *portIndex = 0;
        return true;
    }
    else if (0 == strcmp(I2S_PIN_SRXA1, txt))
    {
        *pin = UCS_STREAM_PORT_PIN_ID_SRXA1;
        *portIndex = 0;
        return true;
    }
    else if (0 == strcmp(I2S_PIN_SRXB0, txt))
    {
        *pin = UCS_STREAM_PORT_PIN_ID_SRXB0;
        *portIndex = 1;
        return true;
    }
    else if (0 == strcmp(I2S_PIN_SRXB1, txt))
    {
        *pin = UCS_STREAM_PORT_PIN_ID_SRXB1;
        *portIndex = 1;
        return true;
    }
    UcsXml_CB_OnError("Invalid I2S pin val:'%s'", 1, txt);
    return false;
}

static bool GetI2SAlignment(mxml_node_t *element, Ucs_Stream_PortDataAlign_t *align, bool mandatory)
{
    const char *txt;
    if (!GetString(element, I2S_ALIGN, &txt, true))
        return false;
    if (0 == strcmp(I2S_ALIGN_L16, txt))
        *align = UCS_STREAM_PORT_ALGN_LEFT16BIT;
    else if (0 == strcmp(I2S_ALIGN_L24, txt))
        *align = UCS_STREAM_PORT_ALGN_LEFT24BIT;
    else if (0 == strcmp(I2S_ALIGN_R16, txt))
        *align = UCS_STREAM_PORT_ALGN_RIGHT16BIT;
    else if (0 == strcmp(I2S_ALIGN_R24, txt))
        *align = UCS_STREAM_PORT_ALGN_RIGHT24BIT;
    else if (0 == strcmp(I2S_ALIGN_SEQUENTIAL, txt))
        *align = UCS_STREAM_PORT_ALGN_SEQ;
    else 
    {
        UcsXml_CB_OnError("Invalid I2S alignment:'%s'", 1, txt);
        return false;
    }
    return true;
}

static bool GetDirection(mxml_node_t *element, Ucs_SocketDirection_t *out)
{
    const char *txt;
    if (!GetString(element, DIR, &txt, true)) return false;
    if (0 == strcmp(DIR_IN, txt))
        *out = UCS_SOCKET_DIR_INPUT;
    else if (0 == strcmp(DIR_OUT, txt))
        *out = UCS_SOCKET_DIR_OUTPUT;
    else 
        return false;
    return true;
}

static bool GetDataType(mxml_node_t *element, MData_t *out)
{
    const char *txt;
    if (!GetString(element, DATA_TYPE, &txt, true)) return false;
    if (0 == strcmp(DATATYPE_SYNC, txt)) {
        *out = SYNC_DATA;
    } else if (0 == strcmp(DATATYPE_CTRL, txt)) {
        *out = CONTROL_DATA;
    } else if (0 == strcmp(DATATYPE_AVP, txt)) {
        *out = AV_PACKETIZED;
    } else if (0 == strcmp(DATATYPE_QOS, txt)) {
        *out = QOS_IP;
    } else if (0 == strcmp(DATATYPE_DFP, txt)) {
        *out = DISC_FRAME_PHASE;
    } else if (0 == strcmp(DATATYPE_IPC, txt)) {
        *out = IPC_PACKET;
    } else {
        UcsXml_CB_OnError("Unknown data type : '%s'", 1, txt);
        return false;
    }
    return true;
}

static bool GetPort(mxml_node_t *element, MPort_t *out)
{
    const char *txt;
    if (!GetString(element, PORT, &txt, true)) return false;
    if (0 == strcmp(txt, PORT_MOST)) {
            *out = MPORT_MOST;
    } else if (0 == strcmp(txt, PORT_USB)) {
        *out = MPORT_USB;
    } else if (0 == strcmp(txt, PORT_MLB)) {
        *out = MPORT_MLB;
    } else if (0 == strcmp(txt, PORT_I2S)) {
        *out = MPORT_I2S;
    } else {
        UcsXml_CB_OnError("Unknown port : '%s'", 1, txt);
        return false;
    }
    return true;
}

static bool GetPayload(mxml_node_t *element, const char *name, uint8_t **pPayload, uint8_t *outLen, struct UcsXmlObjectList *obj, bool mandatory)
{
    uint32_t tempLen, len = 0;
    uint8_t *p;
    const char *txt;
    char *txtCopy;
    char *tkPtr;
    char *token;
    if (!GetString(element, name, &txt, mandatory))
        return false;
    tempLen = strlen(txt) + 1;
    txtCopy = malloc(tempLen);
    if (NULL == txtCopy) 
        return false;
    strncpy(txtCopy, txt, tempLen);
    tempLen = tempLen / 3; /* 2 chars hex value plus space (AA )  */
    p = MCalloc(obj, tempLen, 1);
    if (NULL == p)
    {
        free(txtCopy);
        return false;
    }
    *pPayload = p;
    token = strtok_r( txtCopy, " ,.-", &tkPtr );
    while( NULL != token )
    {
        if( len >= tempLen )
        {
            UcsXml_CB_OnError("Script payload values must be stuffed to two characters", 0);
            free(txtCopy);
            assert(false);
            return 0;
        }
        p[len++] = strtol( token, NULL, 16 );
        token = strtok_r( NULL, " ,.-", &tkPtr );
    }
    *outLen = len;
    return true;
}

static bool AddJob(Ucs_Xrm_ResObject_t **joblist, Ucs_Xrm_ResObject_t *job)
{
    uint32_t i;
    if (NULL == joblist || NULL == job)
    {
        assert(false);
        return false;
    }
    for (i = 0; i < MAX_JOB_LIST_LEN; i++)
    {
        if (NULL == joblist[i]) 
        {
            joblist[i] = job;
            return true;
        }
    }
    assert(false);
    return false;
}

static void AddRoute(struct UcsXmlRouteInfo **pRtLst, struct UcsXmlRouteInfo *route)
{
    struct UcsXmlRouteInfo *tail;
    if (NULL == pRtLst || NULL == route) return;
    if (NULL == pRtLst[0])
    {
        pRtLst[0] = route;
        return;
    }
    tail = pRtLst[0];
    while(tail->next) tail = tail->next;
    tail->next = route;
}

static void AddScript(struct UcsXmlScriptInfo **pScrLst, struct UcsXmlScriptInfo *script)
{
    struct UcsXmlScriptInfo *tail;
    if (NULL == pScrLst || NULL == script) return;
    if (NULL == pScrLst[0])
    {
        pScrLst[0] = script;
        return;
    }
    tail = pScrLst[0];
    while(tail->next) tail = tail->next;
    tail->next = script;
}

static ParseResult_t ParseAll(mxml_node_t *tree, UcsXmlVal_t *v, PrivateData_t *vp)
{
    uint32_t devCount;
    mxml_node_t *sub;
    ParseResult_t result;
    if (!GetCount(tree, DEVICE, &devCount, true))
        return Parse_XmlError;
    
    v->pNod = MCalloc(&vp->objList, devCount, sizeof(Ucs_Rm_Node_t));
    if (NULL == v->pNod) return Parse_MemoryError;

    if (!GetUInt16(tree, ASYNC_BANDWIDTH, &v->packetBw, true))
        return Parse_XmlError;

    ///Iterate all devices
    if (!GetElement(tree, DEVICE, true, &sub, true))
        return Parse_XmlError;
    while(sub)
    {
        mxml_node_t *ch;
        vp->nod = &v->pNod[v->nodSize];
        if (Parse_Success != (result = ParseDevice(sub, vp)))
            return result;
        ///Iterate all channels. Device without any channel is also valid.
        if (GetElement(sub->child, CHANNEL, false, &ch, false))
        {
            while(ch)
            {
                mxml_node_t *soc;
                uint8_t sockCnt = 0;
                if (Parse_Success != (result = ParseChannel(ch, vp)))
                    return result;
                ///Iterate all sockets
                if(!GetElement(ch->child, SOCKET, false, &soc, true))
                    return Parse_XmlError;
                while(soc)
                {
                    ParseResult_t result;
                    if (Parse_Success != (result = ParseSocket(soc, vp)))
                        return result;
                    ++sockCnt;
                    if (!GetElement(soc, SOCKET, false, &soc, false))
                        break;
                }
                if (2 != sockCnt)
                {
                    UcsXml_CB_OnError("%d sockets per channel found, must be 2", 1, sockCnt);
                    return Parse_XmlError;
                }
                if (!GetElement(ch, CHANNEL, false, &ch, false))
                    break;
            }
        }
        ++v->nodSize;
        if (!GetElement(sub, DEVICE, false, &sub, false))
            break;
    }

    ///Fill route structures
    result = ParseRoutes(v, vp);
    if (Parse_MemoryError == result) return Parse_MemoryError;
    else if (Parse_XmlError == result) return Parse_XmlError;
    
    ///Iterate all scripts. No scripts at all is allowed
    if(GetElement(tree, SCRIPT, true, &sub, false))
    {
        while(sub)
        {
            ParseResult_t result = ParseScript(sub, vp);
            if (Parse_MemoryError == result) return Parse_MemoryError;
            else if (Parse_XmlError == result) return Parse_XmlError;
            if(!GetElement(sub, SCRIPT, false, &sub, false))
                break;
        }
    }
    return result;
}

static ParseResult_t ParseDevice(mxml_node_t * dev, PrivateData_t *vp)
{
    const char *txt;
    assert(NULL != dev && NULL != vp);
    vp->nod->signature_ptr = MCalloc(&vp->objList, 1, sizeof(Ucs_Signature_t));
    if(NULL == vp->nod->signature_ptr) return Parse_MemoryError;
    if (!GetUInt16(dev, NODE_ADDR, &vp->nod->signature_ptr->node_address, true))
        return Parse_XmlError;
    if (GetString(dev, SCRIPT, &txt, false))
    {
        struct UcsXmlScriptInfo *scr = MCalloc(&vp->objList, 1, sizeof(struct UcsXmlScriptInfo));
        if (NULL == scr) return Parse_MemoryError;
        scr->node = vp->nod;
        strncpy(scr->scriptName, txt, sizeof(scr->scriptName));
        AddScript(&vp->pScrLst, scr);
    }
    return Parse_Success;;
}

static ParseResult_t ParseChannel(mxml_node_t *ch, PrivateData_t *vp)
{
    vp->inSocket = NULL;
    vp->outSocket = NULL;
    vp->jobList = NULL;
    vp->dataType = 0xFF;
    vp->blockWidth = 0;
    vp->chName = NULL;
    assert(NULL != ch && NULL != vp);
    if (!GetString(ch, CHANNEL_NAME, &vp->chName, true))
        return Parse_XmlError;
    if (!GetDataType(ch, &vp->dataType))
        return Parse_XmlError;
    if (!GetUInt16(ch, BANDWIDTH, &vp->blockWidth, true))
        return Parse_XmlError;
    vp->jobList = MCalloc(&vp->objList, MAX_JOB_LIST_LEN, sizeof(Ucs_Xrm_ResObject_t *));
    if (NULL == vp->jobList) return Parse_MemoryError;
    return Parse_Success;
}

static ParseResult_t ParseSocket(mxml_node_t *soc, PrivateData_t *vp)
{
    MPort_t port;
    bool isIn = false;
    bool isSource = false;
    uint16_t offset = 0;
    assert(NULL != soc && NULL != vp);
    Ucs_SocketDirection_t direction;
    if (!GetDirection(soc, &direction))
        return Parse_XmlError;
    isIn = (UCS_SOCKET_DIR_INPUT == direction);
    if (!GetPort(soc, &port))
        return Parse_XmlError;
    switch(port)
    {
        case MPORT_MOST:
        {
            Ucs_Xrm_MostSocket_t *sock = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_MostSocket_t));
            if (NULL == sock) return Parse_MemoryError;
            if (!AddJob(vp->jobList, sock)) return Parse_XmlError;
            sock->data_type = vp->dataType;
            sock->bandwidth = vp->blockWidth;
            if (Parse_Success != ParseMostSoc(sock, soc, vp))
                return Parse_XmlError;
            isSource = (UCS_SOCKET_DIR_OUTPUT == direction);
            if (isIn) vp->inSocket = sock; else vp->outSocket = sock;
            break;
        }
        case MPORT_USB:
        {
            Ucs_Xrm_UsbSocket_t *sock = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_UsbSocket_t));
            if (NULL == sock) return Parse_MemoryError;
            if (!AddJob(vp->jobList, sock)) return Parse_XmlError;
            sock->data_type = vp->dataType;
            if (Parse_Success != ParseUsbSoc(sock, soc, vp))
                return Parse_XmlError;
            if (isIn) vp->inSocket = sock; else vp->outSocket = sock;
            break;
        }
        case MPORT_MLB:
        {
            Ucs_Xrm_MlbSocket_t *sock = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_MlbSocket_t));
            if (NULL == sock) return Parse_MemoryError;
            if (!AddJob(vp->jobList, sock)) return Parse_XmlError;
            sock->data_type = vp->dataType;
            sock->bandwidth = vp->blockWidth;
            if (Parse_Success != ParseMlbSoc(sock, soc, vp))
                return Parse_XmlError;
            if (isIn) vp->inSocket = sock; else vp->outSocket = sock;
            break;
        }
        case MPORT_I2S:
        {
            Ucs_Xrm_StrmSocket_t *sock = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_StrmSocket_t));
            if (NULL == sock) return Parse_MemoryError;
            sock->data_type = vp->dataType;
            sock->bandwidth = vp->blockWidth;
            if (Parse_Success != ParseStreamSoc(sock, soc, vp))
                return Parse_XmlError;
            if (isIn) vp->inSocket = sock; else vp->outSocket = sock;
            break;
        }
        default:
            assert(false);
            return Parse_XmlError;
    }
    if (GetUInt16(soc, OFFSET, &offset, false))
    {
        //TODO: If offset is non zero allocate Splitter / Combiner
    }
    //Connect in and out socket once they are created
    if (vp->inSocket && vp->outSocket)
    {
        switch(vp->dataType)
        {
        case AV_PACKETIZED:
            {
                Ucs_Xrm_AvpCon_t *con = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_AvpCon_t));
                if (NULL == con) return Parse_MemoryError;
                if (!AddJob(vp->jobList, con))
                    return Parse_XmlError;
                con->resource_type = UCS_XRM_RC_TYPE_AVP_CON;
                con->socket_in_obj_ptr = vp->inSocket;
                con->socket_out_obj_ptr = vp->outSocket;
                con->isoc_packet_size = UCS_ISOC_PCKT_SIZE_188; //TODO:Read from XML
                break;
            }
        case SYNC_DATA:
            {
                Ucs_Xrm_SyncCon_t *con = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_SyncCon_t));
                if (NULL == con) return Parse_MemoryError;
                if (!AddJob(vp->jobList, con))
                    return Parse_XmlError;
                con->resource_type = UCS_XRM_RC_TYPE_SYNC_CON;
                con->socket_in_obj_ptr = vp->inSocket;
                con->socket_out_obj_ptr = vp->outSocket;
                con->mute_mode = UCS_SYNC_MUTE_MODE_NO_MUTING; //TODO:Read from XML
                con->offset = offset;
                break;
            }
        default:
            UcsXml_CB_OnError("Could not connect sockets, data type not implemented: %d", 1, vp->dataType);
            return Parse_XmlError;
            break;
        }
        Ucs_Rm_EndPoint_t *ep = MCalloc(&vp->objList, 1, sizeof(Ucs_Rm_EndPoint_t));
        if (NULL == ep) return Parse_MemoryError;
        ep->endpoint_type = isSource ? UCS_RM_EP_SOURCE : UCS_RM_EP_SINK;
        ep->jobs_list_ptr = vp->jobList;
        ep->node_obj_ptr = vp->nod;

        struct UcsXmlRouteInfo *route = MCalloc(&vp->objList, 1, sizeof(struct UcsXmlRouteInfo));
        if (NULL == route) return Parse_MemoryError;
        route->isSource = isSource;
        route->ep = ep;
        strncpy(route->routeName, vp->chName, sizeof(route->routeName));
        AddRoute(&vp->pRtLst, route);
    }
    return Parse_Success;
}

static ParseResult_t ParseMostSoc(Ucs_Xrm_MostSocket_t *mostSoc, mxml_node_t *soc, PrivateData_t *vp)
{
    assert(NULL != mostSoc && NULL != soc && NULL != vp);
    COMPILETIME_CHECK(SYNC_DATA == (MData_t)UCS_MOST_SCKT_SYNC_DATA);
    COMPILETIME_CHECK(AV_PACKETIZED == (MData_t)UCS_MOST_SCKT_AV_PACKETIZED);
    COMPILETIME_CHECK(QOS_IP == (MData_t)UCS_MOST_SCKT_QOS_IP);
    COMPILETIME_CHECK(DISC_FRAME_PHASE == (MData_t)UCS_MOST_SCKT_DISC_FRAME_PHASE);
    switch((MData_t)mostSoc->data_type)
    {
        case SYNC_DATA:
        case AV_PACKETIZED:
        case QOS_IP:
        case DISC_FRAME_PHASE:
            break; //Nothing to do, valid values.
        default:
            UcsXml_CB_OnError("Invalid DataType=%d for MOST socket", 1, mostSoc->data_type);
            return Parse_XmlError;
    }
    mostSoc->resource_type = UCS_XRM_RC_TYPE_MOST_SOCKET;
    mostSoc->most_port_handle = 0x0D00;
    if (!GetDirection(soc, &mostSoc->direction))
        return Parse_XmlError;
    return Parse_Success;
}

static ParseResult_t ParseUsbSoc(Ucs_Xrm_UsbSocket_t *usbSoc, mxml_node_t *soc, PrivateData_t *vp)
{
    Ucs_Xrm_DefaultCreatedPort_t *p;
    assert(NULL != usbSoc && NULL != soc && NULL != vp);
    COMPILETIME_CHECK(SYNC_DATA == (MData_t)UCS_USB_SCKT_SYNC_DATA);
    COMPILETIME_CHECK(CONTROL_DATA == (MData_t)UCS_USB_SCKT_CONTROL_DATA);
    COMPILETIME_CHECK(AV_PACKETIZED == (MData_t)UCS_USB_SCKT_AV_PACKETIZED);
    switch((MData_t)usbSoc->data_type)
    {
        case SYNC_DATA:
        case CONTROL_DATA:
        case AV_PACKETIZED:
            break; //Nothing to do, valid values.
        default:
            UcsXml_CB_OnError("Invalid DataType=%d for USB socket", 1, usbSoc->data_type);
            return Parse_XmlError;
    }
    usbSoc->resource_type = UCS_XRM_RC_TYPE_USB_SOCKET;
    if (!GetDirection(soc, &usbSoc->direction))
        return Parse_XmlError;
    if (!GetUInt8(soc, ADDRESS, &usbSoc->end_point_addr, true))
        return Parse_XmlError;

    if (!GetUInt16(soc, PACKETS_XACT, &usbSoc->frames_per_transfer, true))
        return Parse_XmlError;

    //XML provides currently no way to open USB port, so use ConfigString default
    p = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_DefaultCreatedPort_t));
    if (NULL == p) return Parse_MemoryError;
    if (!AddJob(vp->jobList, p)) return Parse_MemoryError;
    p->resource_type = UCS_XRM_RC_TYPE_DC_PORT;
    p->port_type = UCS_XRM_PORT_TYPE_USB;
    p->index = 0;
    usbSoc->usb_port_obj_ptr = p;
    return Parse_Success;
}

static ParseResult_t ParseMlbSoc(Ucs_Xrm_MlbSocket_t *mlbSoc, mxml_node_t *soc, PrivateData_t *vp)
{
    Ucs_Mlb_ClockConfig_t clock;
    assert(NULL != mlbSoc && NULL != soc && NULL != vp);
    COMPILETIME_CHECK(SYNC_DATA == (MData_t)UCS_MLB_SCKT_SYNC_DATA);
    COMPILETIME_CHECK(CONTROL_DATA == (MData_t)UCS_MLB_SCKT_CONTROL_DATA);
    COMPILETIME_CHECK(AV_PACKETIZED == (MData_t)UCS_USB_SCKT_AV_PACKETIZED);
    COMPILETIME_CHECK(QOS_IP == (MData_t)UCS_MLB_SCKT_QOS_IP);
    COMPILETIME_CHECK(DISC_FRAME_PHASE == (MData_t)UCS_MLB_SCKT_DISC_FRAME_PHASE);
    COMPILETIME_CHECK(IPC_PACKET == (MData_t)UCS_MLB_SCKT_IPC_PACKET);
    switch((MData_t)mlbSoc->data_type)
    {
        case SYNC_DATA:
        case CONTROL_DATA:
        case AV_PACKETIZED:
        case QOS_IP:
        case DISC_FRAME_PHASE:
        case IPC_PACKET:
            break; //Nothing to do, valid values.
        default:
            UcsXml_CB_OnError("Invalid DataType=%d for MLB socket", 1, mlbSoc->data_type);
            return Parse_XmlError;
    }
    mlbSoc->resource_type = UCS_XRM_RC_TYPE_MLB_SOCKET;
    if (!GetDirection(soc, &mlbSoc->direction))
        return Parse_XmlError;
    if (!GetUInt16(soc, ADDRESS, &mlbSoc->channel_address, true))
         return Parse_XmlError;

    //Create MLB port when DEVICE_MLB_SPEED is defined, otherwise use ConfigString default
    if (GetMlbSpeed(soc, DEVICE_MLB_SPEED, &clock, false))
    {
        Ucs_Xrm_MlbPort_t *p = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_MlbPort_t));
        if (NULL == p) return Parse_MemoryError;
        if (!AddJob(vp->jobList, p)) return Parse_MemoryError;
        p->resource_type = UCS_XRM_RC_TYPE_MLB_PORT;
        p->index = 0;
        p->clock_config = clock;
        mlbSoc->mlb_port_obj_ptr = p;
    } else {
        Ucs_Xrm_DefaultCreatedPort_t *p = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_DefaultCreatedPort_t));
        if (NULL == p) return Parse_MemoryError;
        if (!AddJob(vp->jobList, p)) return Parse_MemoryError;
        p->resource_type = UCS_XRM_RC_TYPE_DC_PORT;
        p->port_type = UCS_XRM_PORT_TYPE_MLB;
        p->index = 0;
        mlbSoc->mlb_port_obj_ptr = p;
    }
    return Parse_Success;
}

static ParseResult_t ParseStreamSoc(Ucs_Xrm_StrmSocket_t *strmSoc, mxml_node_t *soc, PrivateData_t *vp)
{
    const char *txt;
    Ucs_Xrm_StrmPort_t *strPort;
    assert(NULL != strmSoc && NULL != soc && NULL != vp);
    COMPILETIME_CHECK(SYNC_DATA == (MData_t)UCS_STREAM_PORT_SCKT_SYNC_DATA);
    if (SYNC_DATA != (MData_t)strmSoc->data_type)
    {
        UcsXml_CB_OnError("Invalid DataType=%d for I2S socket", 1, strmSoc->data_type);
            return Parse_XmlError;
    }
    strPort = MCalloc(&vp->objList, 1, sizeof(Ucs_Xrm_StrmPort_t));
    if (NULL == strPort) return Parse_MemoryError;
    if (!AddJob(vp->jobList, strPort)) return Parse_MemoryError;
    if (!AddJob(vp->jobList, strmSoc)) return Parse_MemoryError;
    strmSoc->resource_type = UCS_XRM_RC_TYPE_STRM_SOCKET;
    strmSoc->stream_port_obj_ptr = strPort;

    strPort->resource_type = UCS_XRM_RC_TYPE_STRM_PORT;
    if (!GetDirection(soc, &strmSoc->direction))
        return Parse_XmlError;
    if (!GetI2SPin(soc, &strmSoc->stream_pin_id, &strPort->index, true))
        return Parse_XmlError;
    if (!GetI2SSpeed(soc, I2S_FS_SPEED, &strPort->clock_config, true))
        return Parse_XmlError;
    if (!GetString(soc, I2S_ALIGN, &txt, true))
        return Parse_XmlError;
    if (!GetI2SAlignment(soc, &strPort->data_alignment, true))
        return Parse_XmlError;
    return Parse_Success;
}

static ParseResult_t ParseScript(mxml_node_t *scr, PrivateData_t *vp)
{
    mxml_node_t *act;
    uint32_t actCnt;
    uint32_t i = 0;
    const char *txt;
    assert(NULL != scr && NULL != vp);
    vp->pause = 0;
    if (!GetString(scr, NAME, &txt, true))
        return Parse_XmlError;
    Ucs_Rm_Node_t *n = NULL;
    struct UcsXmlScriptInfo *scrlist = vp->pScrLst;
    while(NULL != scrlist)
    {
        if (0 == strcmp(txt, scrlist->scriptName))
        {
            n = scrlist->node;
            break;
        }
        scrlist = scrlist->next;
    }
    if (NULL == n)
    {
        UcsXml_CB_OnError("Script defined:'%s', which was never referenced", 1, txt);
        return Parse_XmlError;
    }
    if (!GetCount(scr, ACTION, &actCnt, true)) return Parse_XmlError;
    if (NULL == (n->script_list_ptr = MCalloc(&vp->objList, actCnt, sizeof(Ucs_Ns_Script_t))))
        return Parse_MemoryError;
    n->script_list_size = actCnt;
    ///Iterate all actions
    if (!GetElement(scr, ACTION, true, &act, true)) return false;
    while(act)
    {
        ParseResult_t result = ParseScriptAction(act, n, i, vp);
        if (Parse_Success != result) return result;
        if (!GetElement(act, ACTION, false, &act, false))
            break;
        ++i;
    }
    return Parse_Success;
}

static ParseResult_t ParseScriptAction(mxml_node_t *act, Ucs_Rm_Node_t *n, uint32_t index, PrivateData_t *vp)
{
    const char *txt;
    uint8_t opResult;
    assert(NULL != act && NULL != vp);
    Ucs_Ns_Script_t *scr =  &n->script_list_ptr[index];
    if (!GetString(act, TYPE, &txt, true))
        return Parse_XmlError;
    if (0 == strcmp(txt, SEND_MSG))
    {
        Ucs_Ns_ConfigMsg_t *req;
        scr->send_cmd = MCalloc(&vp->objList, 1, sizeof(Ucs_Ns_ConfigMsg_t));
        req = scr->send_cmd;
        if (NULL == req) return Parse_MemoryError;
        scr->pause = vp->pause;
        if (vp->pause) vp->pause = 0;
        req->InstId = 1;
        if (!GetUInt8(act, FBLOCK_ID, &req->FBlockId, true))
            return Parse_XmlError;

        if (!GetUInt16(act, FUNCTION_ID, &req->FunktId, true))
            return Parse_XmlError;

        if (!GetUInt8(act, OP_TYPE_REQUEST, &req->OpCode, true))
            return Parse_XmlError;

        if (GetUInt8(act, OP_TYPE_RESPONSE, &opResult, false))
        {
            //Waiting for response is optional
            Ucs_Ns_ConfigMsg_t *res;
            scr->exp_result = MCalloc(&vp->objList, 1, sizeof(Ucs_Ns_ConfigMsg_t));
            res = scr->exp_result;
            res->FBlockId = req->FBlockId;
            res->InstId = req->InstId;
            res->FunktId = req->FunktId;
            res->OpCode = opResult;
            //Not interested in result:
            GetPayload(act, PAYLOAD_RES_HEX, &res->DataPtr, &res->DataLen, &vp->objList, false);
        }
        if (!GetPayload(act, PAYLOAD_REQ_HEX, &req->DataPtr, &req->DataLen, &vp->objList, true))
            return Parse_XmlError;
        if (0 == req->DataLen || NULL == req->DataPtr)
            return Parse_XmlError;
    }
    else if (0 == strcmp(txt, PAUSE))
    {
        if (!GetUInt16(act, PAUSE_MS, &vp->pause, true))
            return Parse_XmlError;
    }
    return Parse_Success;
}

static ParseResult_t ParseRoutes(UcsXmlVal_t *v, PrivateData_t *vp)
{
    uint16_t routeAmount = 0;
    struct UcsXmlRouteInfo *sourceRoute;
    assert(NULL != v && NULL != vp);
    //First: Count the amount of routes and allocate the correct amount
    sourceRoute = vp->pRtLst;
    while (NULL != sourceRoute)
    {
        if (!sourceRoute->isSource) //There can be more sinks than sources, so count them
        {
            ++routeAmount;
        }
        sourceRoute = sourceRoute->next;
    }
    if (0 == routeAmount)
        return Parse_Success; //Its okay to have no routes at all (e.g. MEP traffic only)
    v->pRoutes = MCalloc(&vp->objList, routeAmount, sizeof(Ucs_Rm_Route_t));
    if (NULL == v->pRoutes) return Parse_MemoryError;
    
    //Second: Fill allocated structure now
    sourceRoute = vp->pRtLst;
    while (NULL != sourceRoute)
    {
        if (sourceRoute->isSource)
        {
            struct UcsXmlRouteInfo *sinkRoute = vp->pRtLst;
            while (NULL != sinkRoute)
            {
                if (sourceRoute != sinkRoute 
                    && !sinkRoute->isSource
                    && (0 == strncmp(sourceRoute->routeName, sinkRoute->routeName, sizeof(sourceRoute->routeName))))
                {
                    Ucs_Rm_Route_t *route = &v->pRoutes[v->routesSize++];
                    route->source_endpoint_ptr = sourceRoute->ep;
                    route->sink_endpoint_ptr = sinkRoute->ep;
                    route->active = 1;
                }
                sinkRoute = sinkRoute->next;
            }
        }
        sourceRoute = sourceRoute->next;
    }
    return Parse_Success;
}
