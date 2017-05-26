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
 * \brief Internal header file of Job classes
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_JOBS
 * @{
 */

#ifndef UCS_JOBS_H
#define UCS_JOBS_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
struct CJob_;
typedef struct CJob_ CJob;

/*! \brief   Starts up the MOST Network
 *  \param   self       The instance
 */
typedef void (*Job_StartCb_t)(void *self);

/*------------------------------------------------------------------------------------------------*/
/* CJob Class                                                                                     */
/*------------------------------------------------------------------------------------------------*/

/*! \brief Job state */
typedef enum Job_State_
{
    JOB_S_STOPPED  = 0U,
    JOB_S_STARTED  = 1U,
    JOB_S_FINISHED = 2U

} Job_State_t;

/*! \brief Job result */
typedef enum Job_Result_
{
    JOB_R_NA       = 0U,
    JOB_R_SUCCESS  = 1U,
    JOB_R_FAILED   = 2U

} Job_Result_t;

/*! \brief      Job class
 *  \details    Definition of job class
 */
struct CJob_
{
    void           *inst_ptr;
    Job_StartCb_t   start_fptr;
    Job_State_t     state;
    Job_Result_t    result;
    CSingleSubject  subject;
};

/*------------------------------------------------------------------------------------------------*/
/* CJob Methods                                                                                   */
/*------------------------------------------------------------------------------------------------*/
void Job_Ctor(CJob *self, Job_StartCb_t start_fptr, void *inst_ptr);
void Job_Start(CJob *self, CSingleObserver *result_obs_ptr);
void Job_Stop(CJob *self);
void Job_SetResult(CJob *self, Job_Result_t result);
Job_State_t Job_GetState(CJob *self);
Job_Result_t Job_GetResult(CJob *self);


/*------------------------------------------------------------------------------------------------*/
/* CJobQ Class                                                                                    */
/*------------------------------------------------------------------------------------------------*/
struct CJobService_;
typedef struct CJobService_ CJobService;

/*! \brief      JobQ class
 *  \details    Definition of job queue class
 */
typedef struct CJobQ_
{
    Srv_Event_t     event_id;
    Job_State_t     state;
    Job_Result_t    result;
    CJob**          job_list;
    CJobService*    job_service_ptr;
    
    uint8_t         index;          /*! \brief The index of current job */
    CSingleObserver result_obs;     /*! \brief Required to get the job result */
    CSingleSubject  q_subject;      /*! \brief Notifies the JobQ result */
    CDlNode         node;           /*! \brief Required node to add JobQ to JobService class*/

} CJobQ;

/*------------------------------------------------------------------------------------------------*/
/* CJobQ Methods                                                                                  */
/*------------------------------------------------------------------------------------------------*/
void Jbq_Ctor(CJobQ *self, CJobService *job_service_ptr, Srv_Event_t event_id, CJob *job_list[]);
void Jbq_Start(CJobQ *self, CSingleObserver *result_obs_ptr);
void Jbq_Stop(CJobQ *self);
void Jbq_Service(CJobQ *self);
Srv_Event_t Jbq_GetEventId(CJobQ *self);


/*------------------------------------------------------------------------------------------------*/
/* CJobService Class                                                                              */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Job Service class
 *  \details    Definition of job service class
 */
struct CJobService_
{
    CBase          *base_ptr;
    CDlList         list;
    CService        service;
};

/*------------------------------------------------------------------------------------------------*/
/* CJobService Methods                                                                            */
/*------------------------------------------------------------------------------------------------*/
void Jbs_Ctor(CJobService *self, CBase *base_ptr);
void Jbs_RegisterJobQ(CJobService *self, CDlNode *job_q_node);
void Jbs_TriggerEvent(CJobService *self, Srv_Event_t id);


#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_JOBS_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

