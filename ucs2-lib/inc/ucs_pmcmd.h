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
 * \brief Declaration of class CPmCommand
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_PM_CMD
 * @{
 */

#ifndef UCS_PMCMD_H
#define UCS_PMCMD_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"
#include "ucs_memory.h"
#include "ucs_lldpool.h"
#include "ucs_pmp.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Class CPmCommand                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Class CPmCommand  */
typedef struct CPmCommand_
{
    Lld_IntTxMsg_t      tx_obj;     /*!< \brief  Required LLD Tx structure, must be first attribute */
    uint8_t             data[10];   /*!< \brief  Reserved memory space */
    Ucs_Mem_Buffer_t    memory;     /*!< \brief  Public memory structure */
    bool                reserved;   /*!< \brief  \c true if the command is in use, otherwise \c false. */
    bool                trigger;    /*!< \brief  \c true if the command is triggered, otherwise \c false. */

} CPmCommand;


/*------------------------------------------------------------------------------------------------*/
/* Methods                                                                                        */
/*------------------------------------------------------------------------------------------------*/
extern void Pmcmd_Ctor(CPmCommand *self, Pmp_FifoId_t fifo, Pmp_MsgType_t type);
extern Ucs_Lld_TxMsg_t* Pmcmd_GetLldTxObject(CPmCommand *self);
extern bool Pmcmd_Reserve(CPmCommand *self);
extern void Pmcmd_Release(CPmCommand *self);
extern void Pmcmd_SetContent(CPmCommand *self, uint8_t sid, uint8_t ext_type, 
                             uint8_t ext_code, uint8_t add_data_ptr[], uint8_t add_data_sz);
extern void Pmcmd_UpdateContent(CPmCommand *self, uint8_t sid, uint8_t ext_type, uint8_t ext_code);
extern void Pmcmd_SetTrigger(CPmCommand *self, bool trigger);
extern bool Pmcmd_IsTriggered(CPmCommand *self);


#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_PMCMD_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

