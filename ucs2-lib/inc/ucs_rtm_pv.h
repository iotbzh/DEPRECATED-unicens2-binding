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
 * \brief Public header file of the Extended Resource Manager.
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_RTM
 * @{
 */

#ifndef UCS_RTM_PV_H
#define UCS_RTM_PV_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_epm_pv.h"
#include "ucs_obs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief This enumerator specifies the kind of result - Internal, Target or Transmission. */
typedef enum Ucs_Rm_RouteState_
{
    UCS_RM_ROUTE_IDLE         = 0x00U,      /*!< \brief Specifies that the Route is in idle state, i.e. not handled yet. */
    UCS_RM_ROUTE_CONSTRUCTION = 0x01U,      /*!< \brief Specifies that the Route is under Construction. */
    UCS_RM_ROUTE_BUILT        = 0x02U,      /*!< \brief Specifies that the Route is built. */
    UCS_RM_ROUTE_DETERIORATED = 0x03U,      /*!< \brief Specifies that the Route is Deteriorated. */
    UCS_RM_ROUTE_DESTRUCTION  = 0x04U,      /*!< \brief Specifies that the Route is under Destruction. */
    UCS_RM_ROUTE_SUSPENDED    = 0x05U       /*!< \brief Specifies that the Route is Suspended. */

} Ucs_Rm_RouteState_t;

/*! \brief This enumerator specifies the kind of result - Internal, Target or Transmission. */
typedef enum Ucs_Rm_RouteResult_
{
    UCS_RM_ROUTE_NOERROR     = 0x00U,      /*!< \brief Specifies that the result is error free. */
    UCS_RM_ROUTE_UNCRITICAL  = 0x01U,      /*!< \brief Specifies that the result is uncritical. A retry is necessary. */
    UCS_RM_ROUTE_CRITICAL    = 0x02U       /*!< \brief Specifies that the result is critical. No retry necessary. */

} Ucs_Rm_RouteResult_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/* Rtm_Inst_t requires incomplete forward declaration, to hide internal data type.
 * The Rtm_Inst_t object is allocated internally, the core library must access only the pointer to Rtm_Inst_t. */
struct Rtm_Inst_;

/*!\brief   RouteManagement instance */
typedef struct Rtm_Inst_ Rtm_Inst_t;

/*! \brief Internal configuration structure of a Route. */
typedef struct Ucs_Rm_RouteInt_
{
    /*! \brief Specifies the RTM instance that manages this route. */
    Rtm_Inst_t * rtm_inst;
    /*! \brief Specifies the route state. */
    Ucs_Rm_RouteState_t route_state;
    /*! \brief Specifies the last route result. */
    Ucs_Rm_RouteResult_t last_route_result;
    /*! \brief Specifies the observer object for source endpoint. */
    CObserver source_ep_observer;
    /*! \brief Specifies whether the sink observer object is initialized or not. */
    uint8_t sink_obsvr_initialized;
    /*! \brief Specifies whether the source observer object is initialized or not. */
    uint8_t src_obsvr_initialized;
    /*! \brief Specifies whether or not the UCS termination has been notified for this route. */
    uint8_t notify_termination;
    /*! \brief Specifies the observer object for sink endpoint. */
    CObserver sink_ep_observer;

} Ucs_Rm_RouteInt_t;

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_RTM_PV_H */

/*!
 * @}
 * \endcond
 */



/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

