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
 * \brief Internal header file of class CBackChannelDiag.
 *
 * \cond UCS_INTERNAL_DOC
 */

#ifndef UCS_BC_DIAG_H
#define UCS_BC_DIAG_H

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

/*! \brief   Structure of class CBackChannelDiag. */
typedef struct CBackChannelDiag_
{
    CInic   *inic;                      /*!< \brief Reference to CInic object */
    CExc    *exc;                       /*!< \brief Reference to CExc object */
    CBase   *base;                      /*!< \brief Reference to CBase object */

    CSingleObserver bcd_inic_bcd_start;  /*!< \brief Observes the INIC.BCDiag result */
    CSingleObserver bcd_inic_bcd_end;   /*!< \brief Observes the INIC.BCDiagEnd result*/
    CSingleObserver bcd_enabletx;       /*!< \brief Observes the EXC.BCEnableTx result */
    CSingleObserver bcd_diagnosis;      /*!< \brief Observes the EXC.BCDiag result */
    CSingleObserver bcd_switchback;     /*!< \brief Observes the EXC.Switchback result. */


    CMaskedObserver bcd_terminate;       /*!< \brief Observes events leading to termination */

    CObserver       bcd_nwstatus;        /*!< \brief Observes the MOST Network status */

    CFsm     fsm;                       /*!< \brief Node Discovery state machine  */
    CService service;                   /*!< \brief Service instance for the scheduler */
    CTimer   timer;                     /*!< \brief timer for monitoring messages */

    Exc_WelcomeResult_t   welcome_result;       /*!< \brief buffer for welcome result */
    bool neton;                         /*!< \brief indicates Network availability */

    Ucs_Bcd_ReportCb_t report_fptr;      /*!< \brief Report callback function */

    uint8_t current_segment;            /*!< \brief segment which is currently tested, starts with 0. */
    Exc_BCDiagResult bcd_result;        /*!< \brief Result of current tested segment. */

}CBackChannelDiag;


/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
void Bcd_Ctor(CBackChannelDiag *self,
              CInic *inic,
              CBase *base,
              CExc *exc);

extern void Bcd_Start(CBackChannelDiag *self, Ucs_Bcd_ReportCb_t report_fptr);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* UCS_BC_DIAG_H */
/*!
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

