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
 * \brief Internal header file of the scheduler module. The module consists of the two classes
 *        CScheduler and CService.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_SCHEDULER
 * @{
 */

#ifndef UCS_SCHEDULER_H
#define UCS_SCHEDULER_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_dl.h"
#include "ucs_obs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Function signature used for UCS request callback function */
typedef void (*Scd_UcsServiceRequest_t)(void);
/*! \brief Function signature used for service callback functions
 *  \param self Instance pointer
 */
typedef void (*Srv_Cb_t)(void *self);
/*! \brief Data type of event masks */
typedef uint32_t Srv_Event_t;

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/
extern const Srv_Event_t SRV_EMPTY_EVENT_MASK;  /*!< \brief Empty event mask */

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Standard return values of scheduler module. */
typedef enum Scd_Ret_
{
    SCD_OK,                         /*!< \brief No error */
    SCD_UNKNOWN_SRV,                /*!< \brief Service is unknown */
    SCD_SRV_ALREADY_LISTED          /*!< \brief Service is already part of the schedulers list */

} Scd_Ret_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Initialization structure of the scheduler module. */
typedef struct Scd_InitData_
{
    /*! \brief Observer to request a UCS service call */
    CSingleObserver *service_request_obs_ptr;
    /*! \brief UNICENS instance ID */
    /*uint8_t ucs_inst_id;*/

} Scd_InitData_t;

/*! \brief   Class structure of the scheduler. */
typedef struct CScheduler_
{
    /*! \brief Subject to request a UCS service call */
    CSingleSubject service_request_subject;
    /*! \brief Service list of the scheduler */
    CDlList srv_list;
    /*! \brief Indicates if the scheduler services is running */
    bool scd_srv_is_running;
    /*! \brief UNICENS instance ID */
    void * ucs_user_ptr;

} CScheduler;

/*! \brief   Class structure of services used by the scheduler. */
typedef struct CService_
{
    CDlNode list_node;              /*!< \brief Administration area for the linked list */
    CScheduler *scd_ptr;            /*!< \brief Back link to scheduler */
    void *instance_ptr;             /*!< \brief Reference of instance passed to service_fptr() */
    Srv_Cb_t service_fptr;          /*!< \brief Reference of the service callback function */
    Srv_Event_t event_mask;         /*!< \brief Event mask of the service */
    uint8_t priority;               /*!< \brief Priority of the service */

} CService;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CScheduler                                                                 */
/*------------------------------------------------------------------------------------------------*/
extern void Scd_Ctor(CScheduler *self, Scd_InitData_t *init_ptr, void *ucs_user_ptr);
extern void Scd_Service(CScheduler *self);
extern Scd_Ret_t Scd_AddService(CScheduler *self, CService *srv_ptr);
extern Scd_Ret_t Scd_RemoveService(CScheduler *self, CService *srv_ptr);
extern bool Scd_AreEventsPending(CScheduler *self);

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CService                                                                   */
/*------------------------------------------------------------------------------------------------*/
extern void Srv_Ctor(CService *self, uint8_t priority, void *instance_ptr, Srv_Cb_t service_fptr);
extern void Srv_SetEvent(CService *self, Srv_Event_t event_mask);
extern void Srv_GetEvent(CService *self, Srv_Event_t *event_mask_ptr);
extern void Srv_ClearEvent(CService *self, Srv_Event_t event_mask);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_SCHEDULER_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

