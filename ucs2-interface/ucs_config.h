/*
 * Unicens Integration Helper Component
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
#ifndef UNICENSINTEGRATION_H_
#define UNICENSINTEGRATION_H_

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                  USER ADJUSTABLE VALUES                              */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#define ENABLE_INIC_WATCHDOG    (false)
#define ENABLE_AMS_LIB          (true)
#define DEBUG_XRM
#define BOARD_PMS_TX_SIZE       (72)
#define CMD_QUEUE_LEN           (4)

#include <string.h>
#include <stdarg.h>

#include "ucs_cfg.h"
#include "ucs_api.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                          PRIVATE SECTION                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 * \brief Internal enum for Unicens Integration
 */
typedef enum
{
    ///Result is OK and the processing is finished. Safe to dequeue this command.
    UniCmdResult_OK_ProcessFinished,
    ///Result is OK but the processing is ongoing. Must wait for callback.
    UniCmdResult_OK_NeedToWaitForCB,
    ///Result is error and the processing is finished. Safe to dequeue this command.
    UniCmdResult_ERROR_ProcessFinished
} UnicensCmdResult_t;

/**
 * \brief Internal enum for Unicens Integration
 */
typedef enum
{
    UnicensCmd_Unknown,
    UnicensCmd_Init,
    UnicensCmd_Stop,
    UnicensCmd_RmSetRoute,
    UnicensCmd_NsRun
} UnicensCmd_t;

/**
 * \brief Internal struct for Unicens Integration
 */
typedef struct
{
    const Ucs_InitData_t *init_ptr;
} UnicensCmdInit_t;

/**
 * \brief Internal struct for Unicens Integration
 */
typedef struct
{
    Ucs_Rm_Route_t *routePtr;
    bool isActive;
} UnicensCmdRmSetRoute_t;

/**
 * \brief Internal struct for Unicens Integration
 */
typedef struct
{
    Ucs_Rm_Node_t * node_ptr;
} UnicensCmdNsRun_t;

/**
 * \brief Internal struct for Unicens Integration
 */
typedef struct
{
    UnicensCmd_t cmd;
    union
    {
        UnicensCmdInit_t Init;
        UnicensCmdRmSetRoute_t RmSetRoute;
        UnicensCmdNsRun_t NsRun;
    } val;
} UnicensCmdEntry_t;

/**
 * \brief Internal variables for one instance of Unicens Integration
 * \note Never touch any of this fields!
 */
typedef struct {
    volatile uint8_t *dataQueue;
    volatile uint8_t *pRx;
    volatile uint8_t *pTx;
    volatile uint32_t amountOfEntries;
    volatile uint32_t sizeOfEntry;
    volatile uint32_t rxPos;
    volatile uint32_t txPos;
} RB_t;

/**
 * \brief Internal variables for one instance of Unicens Integration
 * \note Allocate this structure for each instance (static or malloc)
 *        and pass it to UCSI_Init()
 * \note Never touch any of this fields!
 */
typedef struct
{
    uint32_t magic;
    void *tag;
    bool initialized;
    RB_t rb;
    uint8_t rbBuf[(CMD_QUEUE_LEN * sizeof(UnicensCmdEntry_t))];
    Ucs_Inst_t *UNICENS;
    Ucs_InitData_t uniInitData;
    bool triggerService;
    Ucs_Lld_Api_t *uniLld;
    void *uniLldHPtr;
    UnicensCmdEntry_t *currentCmd;
} UCSI_Data_t;

#endif /* UNICENSINTEGRATION_H_ */
