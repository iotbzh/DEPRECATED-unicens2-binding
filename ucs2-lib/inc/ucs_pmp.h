/*------------------------------------------------------------------------------------------------*/
/* UNICENS V2.1.0-3491                                                                            */
/* Copyright (c) 2017 Microchip Technology Germany II GmbH & Co. KG.                              */
/*                                                                                                */
/* This program is free software: you can redistribute it and/or modify                           */
/* it under the terms of the GNU General Public License as published by                           */
/* the Free Software Foundation, either version 2 of the License, or                              */
/* (at your option) any later version.                                                            */
/*                                                                                                */
/* This program is distributed in the hope that it will be useful,                                */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                                 */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                                  */
/* GNU General Public License for more details.                                                   */
/*                                                                                                */
/* You should have received a copy of the GNU General Public License                              */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.                          */
/*                                                                                                */
/* You may also obtain this software under a propriety license from Microchip.                    */
/* Please contact Microchip for further information.                                              */
/*------------------------------------------------------------------------------------------------*/

/*!
 * \file
 * \brief Internal header file of Port Message Protocol
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_PMH
 * @{
 */

#ifndef UCS_PMP_H
#define UCS_PMP_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Macros                                                                                         */
/*------------------------------------------------------------------------------------------------*/
#define PMP_PM_MAX_SIZE_HEADER    8U        /*!< \brief     Maximum size of a port message header */
#define PMP_PM_MIN_SIZE_HEADER    6U        /*!< \brief     Minimum size of a port message header */
#define PMP_MAX_NUM_FIFOS         7U        /*!< \brief     Maximum number if FIFOs an an array
                                             *   \details   Means "3" if FIFO "0" and "2" is used
                                             */
#define PMP_CREDITS_MASK       0x3FU        /*!< \brief     Valid bits for credits: 5..0 */
#define PMP_CREDITS_MIN           1U        /*!< \brief     Minimum value for credits: 1 */
#define PMP_CREDITS_MAX          63U        /*!< \brief     Maximum value for credits: 63 */

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/

/*! \brief Specifies the FIFO */
typedef enum Pmp_FifoId_
{
    PMP_FIFO_ID_MCM  = 0U,                  /*!< \brief FIFO dedicated to MOST Control Messages (MCM) */
 /* PMP_FIFO_ID_MDP  = 1U,                     < (reserved identifier) */
    PMP_FIFO_ID_ICM  = 2U,                  /*!< \brief FIFO dedicated to INIC Control Messages (ICM) */
    PMP_FIFO_ID_ALL  = 3U,                  /*!< \brief All FIFOs (ICM, MCM) */
 /* PMP_FIFO_ID_MEP  = 4U                      < (reserved identifier) */
 /* PMP_FIFO_ID_IOCM = 5U                      < (reserved identifier) */
    PMP_FIFO_ID_RCM  = 6U                   /*!< \brief FIFO dedicated to Remote Control Messages (RCM) */
 /* PMP_FIFO_ID_RSVD = 7U                      < (reserved identifier) */

} Pmp_FifoId_t;

/*! \brief Specifies the messages type */
typedef enum Pmp_MsgType_
{
    PMP_MSG_TYPE_CMD       = 0U,            /*!< \brief FIFO command message */
    PMP_MSG_TYPE_STATUS    = 1U,            /*!< \brief FIFO status message */
    PMP_MSG_TYPE_DATA      = 2U             /*!< \brief FIFO data message */

} Pmp_MsgType_t;

/*! \brief Specifies the direction of the Port Message */
typedef enum Pmp_Direction_
{
    PMP_DIR_TX = 0,                         /*!< \brief Direction Tx (EHC -> INIC) */
    PMP_DIR_RX = 1                          /*!< \brief Direction Rx (INIC -> EHC) */

} Pmp_Direction_t;

/*! \brief Specifies FIFO status types */
typedef enum Pmp_StatusType_
{
    PMP_STATUS_TYPE_FAILURE         = 0U,   /*!< \brief PMP status type "failure" */
    PMP_STATUS_TYPE_FLOW            = 1U,   /*!< \brief PMP status type "flow" */
    PMP_STATUS_TYPE_SYNCED          = 4U,   /*!< \brief PMP status type "synced" */
    PMP_STATUS_TYPE_UNSYNCED_BSY    = 5U,   /*!< \brief PMP status type "unsynced_busy" */
    PMP_STATUS_TYPE_UNSYNCED_RDY    = 6U    /*!< \brief PMP status type "unsynced_ready" */

} Pmp_StatusType_t;

/*! \brief Specifies FIFO status codes */
typedef enum Pmp_StatusCode_
{
    PMP_STATUS_CODE_BUSY            = 0U,   /*!< \brief PMP status code "busy" */
    PMP_STATUS_CODE_SUCCESS         = 1U,   /*!< \brief PMP status code "success" */
    PMP_STATUS_CODE_CANCELED        = 3U,   /*!< \brief PMP status code "canceled" */
    PMP_STATUS_CODE_NACK            = 8U    /*!< \brief PMP status code "not_acknowledge" */

} Pmp_StatusCode_t;

/*! \brief Specifies FIFO status codes */
typedef enum Pmp_UnsyncReason_
{
    PMP_UNSYNC_R_STARTUP            = 1U,   /*!< \brief PMP status code, UnsyncReason "INIC Startup" */
    PMP_UNSYNC_R_REINIT             = 2U,   /*!< \brief PMP status code, UnsyncReason "Re-init of another FIFO" */
    PMP_UNSYNC_R_COMMAND            = 3U,   /*!< \brief PMP status code, UnsyncReason "By sync or un-sync command" */
    PMP_UNSYNC_R_ACK_TIMEOUT        = 4U,   /*!< \brief PMP status code, UnsyncReason "Missing EHC Rx acknowledge" */
    PMP_UNSYNC_R_WD_TIMEOUT         = 5U,   /*!< \brief PMP status code, UnsyncReason "Missing EHC status request" */
    PMP_UNSYNC_R_TX_TIMEOUT         = 6U    /*!< \brief PMP status code, UnsyncReason "Missing EHC read, or blocked communication" */

} Pmp_UnsyncReason_t;

/*! \brief Specifies FIFO command types */
typedef enum Pmp_CommandType_
{
    PMP_CMD_TYPE_REQ_STATUS         = 0U,   /*!< \brief PMP command type "request_status" */
    PMP_CMD_TYPE_MSG_ACTION         = 1U,   /*!< \brief PMP command type "message_action" */
 /* PMP_CMD_TYPE_TRIGGER_NAOMDP     = 2U,      < (reserved identifier) */
    PMP_CMD_TYPE_SYNCHRONIZATION    = 4U    /*!< \brief PMP command type "synchronization" */

} Pmp_CommandType_t;

/*! \brief Specifies FIFO command codes */
typedef enum Pmp_CommandCode_
{
    PMP_CMD_CODE_REQ_STATUS         = 0U,   /*!< \brief PMP command code "request_status" */
 /* PMP_CMD_CODE_TRIGGER_NAOMDP     = 0U,      <  (reserved identifier)*/
    PMP_CMD_CODE_ACTION_RETRY       = 1U,   /*!< \brief PMP command type "request_status" */
    PMP_CMD_CODE_ACTION_CANCEL      = 2U,   /*!< \brief PMP command type "request_status" */
    PMP_CMD_CODE_ACTION_CANCEL_ALL  = 3U,   /*!< \brief PMP command type "request_status" */
    PMP_CMD_CODE_UNSYNC             = 10U,  /*!< \brief PMP command type "request_status" */
    PMP_CMD_CODE_SYNC               = 21U   /*!< \brief PMP command type "request_status" */

} Pmp_CommandCode_t;

/*------------------------------------------------------------------------------------------------*/
/* Header buffer operations                                                                       */
/*------------------------------------------------------------------------------------------------*/
extern void Pmp_SetPml(uint8_t header[], uint8_t length);
extern void Pmp_SetPmhl(uint8_t header[], uint8_t length);
extern void Pmp_SetFph(uint8_t header[], Pmp_FifoId_t id, Pmp_MsgType_t type);
extern void Pmp_SetExtType(uint8_t header[], uint8_t type, uint8_t code);
extern void Pmp_SetSid(uint8_t header[], uint8_t sid);

extern uint8_t       Pmp_GetPml(uint8_t header[]);
extern uint8_t       Pmp_GetPmhl(uint8_t header[]);
extern Pmp_FifoId_t  Pmp_GetFifoId(uint8_t header[]);
extern Pmp_MsgType_t Pmp_GetMsgType(uint8_t header[]);
extern uint8_t       Pmp_GetSid(uint8_t header[]);
extern uint8_t       Pmp_GetDataSize(uint8_t header[]);
extern uint8_t       Pmp_GetData(uint8_t header[], uint8_t index);
extern bool          Pmp_VerifyHeader(uint8_t header[], uint8_t buf_len);

/*------------------------------------------------------------------------------------------------*/
/* Class CPmh                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Defines the content of a Port Message Header 
 *  \details The current structure does not support  "direction" and "retransmitted" flag.
 */
typedef struct CPmh_
{
    uint8_t         pml;        /*!< \brief Port Message length */
    uint8_t         pmhl;       /*!< \brief Port Message Header length */
    Pmp_MsgType_t   msg_type;   /*!< \brief Port Message type */
    Pmp_FifoId_t    fifo_id;    /*!< \brief FIFO identifier */
    uint8_t         sid;        /*!< \brief The SequenceId */
    uint8_t         ext_type;   /*!< \brief status or content type */

} CPmh;

/*------------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern void Pmh_Ctor(CPmh *self);
extern void Pmh_BuildHeader(CPmh *self, uint8_t data[]);
extern void Pmh_DecodeHeader(CPmh *self, uint8_t data[]);
extern void Pmh_SetFph(CPmh *self, Pmp_FifoId_t fifo_id, Pmp_MsgType_t msg_type);
extern Pmp_StatusType_t Pmh_GetExtStatusType(CPmh *self);
extern Pmp_StatusCode_t Pmh_GetExtStatusCode(CPmh *self);
extern Pmp_CommandCode_t Pmh_GetExtCommandCode(CPmh *self);
extern Pmp_CommandType_t Pmh_GetExtCommandType(CPmh *self);
extern void Pmh_SetExtType(CPmh *self, uint8_t type, uint8_t code);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif                                              /* UCS_PMP_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

