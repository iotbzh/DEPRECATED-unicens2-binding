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
 * \brief Internal header file of the Connection Storage Pool.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_UCS_XRM_INT
 * @{
 */


#ifndef UCS_XRMPOOL_H
#define UCS_XRMPOOL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_xrm_pb.h"
#include "ucs_dl.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Callback signature used by foreach-function for the resources list
 *  \param rc_ptr    Reference to a resource object in the list
 *  \param ud_ptr1   Reference to the user data 1
 *  \param ud_ptr2   Reference to the user data 2
 *  \param ud_ptr3   Reference to the user data 3
 *  \return true: Stop the for-each-loop
 *  \return false: Continue the for-each-loop
 */
typedef bool (*Xrmp_ForeachFunc_t)(void *rc_ptr, void *ud_ptr1, void *ud_ptr2, void *ud_ptr3);

/*! \brief Callback signature used by _GetResourceHandle-function to check whether found resources belongs to provided jobs list
 *  \param xrm_inst  Reference to the XRM instance to be looked for
 *  \param job_ptr   Reference to the job list to be checked
 *  \return true:    Stop the for-each-loop since given job matches, otherwise
 *  \return false:   Continue the for-each-loop in order to catch new jobs.
 */
typedef bool (*Xrmp_CheckJobListFunc_t)(void * xrm_inst, void * job_ptr);

/*! \brief  Function signature used for the results and reports of the Extended Resource Manager.
 *  \param  node_address        The node address from which the results come 
 *  \param  connection_label    Returned MOST network connection label
 *  \param  result              Result of the job
 *  \param  user_arg            Reference to the user argument
 *  \ingroup G_UCS_IRM
 */
typedef void (*Ucs_Xrm_ReportCb_t)(uint16_t node_address, uint16_t connection_label, Ucs_Xrm_Result_t result, void * user_arg);

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Structure that defines a job of the Extended Resource Manager. */
typedef struct Xrm_Job_
{
    /*! \brief Reference to resource object list */
    UCS_XRM_CONST Ucs_Xrm_ResObject_t **resource_object_list_ptr;
    /*! \brief Report callback of the job */
    Ucs_Xrm_ReportCb_t report_fptr;
    /*! \brief User defined MOST connection label */
    uint16_t most_network_connection_label;
    /*! \brief MOST connection label, returned during MOST socket creation */
    uint16_t connection_label;
    /*!< \brief Node required for jobs pool */
    CDlNode node;
    /*! \brief address of the device in which the job is built */
    bool sync_lost;
    /*! \brief State of the job (valid/invalid) */
    bool valid;
    /*! \brief Notification flag */
    bool notify;
    /*!< \brief user argument */
    void * user_arg;

} Xrm_Job_t;

/*! \brief  Structure that defines an item of the resource handle list. */
typedef struct Xrm_ResourceHandleListItem_
{
    /*! \brief Reference to the job */
    Xrm_Job_t *job_ptr;
    /*! \brief Reference to the resource object */
    UCS_XRM_CONST Ucs_Xrm_ResObject_t *resource_object_ptr;
    /*! \brief INIC Resource handle */
    uint16_t resource_handle;

} Xrm_ResourceHandleListItem_t;

/*! \brief  Class structure of the Storage Pool of XRM Jobs and Resources. */
typedef struct CXrmPool_
{
    /*! \brief Job list */
    Xrm_Job_t job_list[XRM_NUM_JOBS];
    /*! \brief List of resource handles */
    Xrm_ResourceHandleListItem_t resource_handle_list[XRM_NUM_RESOURCE_HANDLES];
    /*!< \brief Reference to the resource identification table */
    Ucs_Xrm_ResIdentity_t * res_id_ptr;
    /*! \brief Size of the resources Id table.
     */
    uint16_t res_id_size;

} CXrmPool;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CXrmPool                                                                   */
/*------------------------------------------------------------------------------------------------*/
extern void Xrmp_Ctor(CXrmPool * self);
extern bool Xrmp_StoreResourceHandle(CXrmPool * self_ptr, uint16_t resource_handle, Xrm_Job_t * job_ptr, UCS_XRM_CONST Ucs_Xrm_ResObject_t * resource_object_ptr);
extern uint16_t Xrmp_GetResourceHandle(CXrmPool * self, Xrm_Job_t * job_ptr, UCS_XRM_CONST Ucs_Xrm_ResObject_t * resource_object_ptr, Xrmp_CheckJobListFunc_t func_ptr, void * usr_ptr);
extern uint8_t Xrmp_GetResourceHandleIdx(CXrmPool *self, Xrm_Job_t *job_ptr, UCS_XRM_CONST Ucs_Xrm_ResObject_t **obj_pptr);
extern Xrm_Job_t * Xrmp_GetJob(CXrmPool * self, UCS_XRM_CONST Ucs_Xrm_ResObject_t * resource_object_list[]);
extern void Xrmp_Foreach(CXrmPool *self, Xrmp_ForeachFunc_t func_ptr, void *user_data_ptr1, void *user_data_ptr2, void *user_data_ptr3);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* #ifndef UCS_XRMPOOL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

