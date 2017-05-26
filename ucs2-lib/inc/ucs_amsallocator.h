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
 * \brief Internal header file of AMS Allocator Interface
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_AMSC
 * @{
 */

#ifndef UCS_AMSALLOCATOR_H
#define UCS_AMSALLOCATOR_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Defines the usage of a requested memory chunk */
typedef enum Ams_MemUsage_
{
    AMS_MU_RX_OBJECT,            /*!< \brief  Memory is required to allocate an Rx message object */
    AMS_MU_RX_PAYLOAD,           /*!< \brief  Memory is required to allocate Rx message payload */
    AMS_MU_TX_OBJECT,            /*!< \brief  Memory is required to allocate a Tx message object */
    AMS_MU_TX_PAYLOAD            /*!< \brief  Memory is required to allocate Tx message payload */

} Ams_MemUsage_t;

/*! \brief  Callback function type that is invoked to allocate external payload for a segmented Rx message
 *  \param  inst_ptr            Reference to the (external) memory management
 *  \param  mem_size            Reference to the required memory size in bytes. Valid values: 0..65535.
 *  \param  type                Declares how the memory is used by UNICENS
 *  \param  custom_info_pptr    Reference which is related to the memory chunk and can be set by 
 *                              the application.
 *  \return Pointer to the provided memory chunk. The application has to guarantee that the memory size
 *          is equal or greater than \c mem_size. The application has to return \c NULL if it is not able 
 *          to allocate the required memory at this moment.
 */
typedef void* (*Ams_AllocMemCb_t)(void *inst_ptr, uint16_t mem_size, Ams_MemUsage_t type, void** custom_info_pptr);

/*! \brief  Callback function type that is invoked to free external payload for a segmented Rx message
 *  \param  inst_ptr        Reference to the (external) memory management
 *  \param  mem_ptr         Reference to the external payload memory
 *  \param  type            Declares how the memory is used by UNICENS
 *  \param  custom_info_ptr Reference to memory related information which was set by the application 
 *                          during memory allocation
 */
typedef void (*Ams_FreeMemCb_t)(void *inst_ptr, void *mem_ptr, Ams_MemUsage_t type, void* custom_info_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Allocator interface                                                                            */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Keeps callback functions to an external memory management for Rx payload  */
typedef struct Ams_MemAllocator_
{
    void *inst_ptr;                /*!< \brief The instance of the (external) memory management */
    Ams_AllocMemCb_t alloc_fptr;   /*!< \brief This function is invoked to allocate Rx user payload */
    Ams_FreeMemCb_t  free_fptr;    /*!< \brief This function is invoked to free Rx user payload */

} Ams_MemAllocator_t;

#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif          /* UCS_AMSALLOCATOR_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

