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

#ifndef UCS_PROG_H
#define UCS_PROG_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_exc.h"


#ifdef __cplusplus
extern "C"
{
#endif



/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/

#define PRG_MAX_LEN_ERROR 3U

typedef struct Prg_Error_t_
{
    Ucs_Prg_ResCode_t code;
    Ucs_Prg_Func_t function;
    uint8_t ret_len;
    uint8_t parm[PRG_MAX_LEN_ERROR];
} Prg_Error_t;




/*! \brief   Structure of class CProgramming. */
typedef struct CProgramming_
{
    CInic   *inic;                      /*!< \brief Reference to CInic object */
    CExc    *exc;                       /*!< \brief Reference to CExc object */
    CBase   *base;                      /*!< \brief Reference to CBase object */

    CSingleObserver prg_welcome;        /*!< \brief Observes the Welcome result */
    CSingleObserver prg_memopen;        /*!< \brief Observes the MemSessionOpen result */
    CSingleObserver prg_memwrite;       /*!< \brief Observes the MemoryWrite result */
    CSingleObserver prg_memclose;       /*!< \brief Observes the MemSessionClose result */

    CMaskedObserver prg_terminate;      /*!< \brief Observes events leading to termination */
    CObserver       prg_nwstatus;       /*!< \brief Observes the MOST Network status */

    CFsm            fsm;                /*!< \brief Node Discovery state machine  */
    CService        service;            /*!< \brief Service instance for the scheduler */
    CTimer          timer;              /*!< \brief timer for monitoring messages */
    bool            neton;              /*!< \brief indicates Network availability */

    uint16_t              node_id;              /*!< \brief Position address of the node to be programmed. */
    uint16_t              target_address;       /*!< \brief Actual target address */
    Ucs_Signature_t       signature;            /*!< \brief Signature of the node to be programmed. */
    Ucs_Prg_SessionType_t session_type;         /*!< \brief Defines the memory access type. */
    Ucs_Prg_Command_t*    command_list;         /*!< \brief Refers to array of programming tasks. */
    uint8_t               command_index;        /*!< \brief index for command_list */
    uint16_t              admin_node_address;   /*!< \brief Admin Node Address */
    Ucs_Prg_ReportCb_t    report_fptr;          /*!< \brief Report callback function */
    uint16_t              session_handle;       /*!< \brief Unique number used to authorize memory access. */
    Ucs_Prg_Func_t        current_function;     /*!< \brief last used function. */
    Prg_Error_t           error;                /*!< \brief stores the current error information */
}CProgramming;


/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
void Prg_Ctor(CProgramming *self, 
              CInic *inic, 
              CBase *base, 
              CExc *exc);

extern void Prg_Start(CProgramming *self,
                      uint16_t node_id, 
                      Ucs_Signature_t *signature,
                      Ucs_Prg_SessionType_t session_type, 
                      Ucs_Prg_Command_t* command_list, 
                      Ucs_Prg_ReportCb_t report_fptr);


#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* UCS_PROG_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

