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
 * \brief Declaration of memory buffer and memory allocator
 */
/*!
 * \addtogroup G_UCS_LLD_TYPES
 * @{
 */

#ifndef UCS_MEMORY_PB_H
#define UCS_MEMORY_PB_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Memory buffer                                                                                  */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Memory chunk representing a message or part of a message. */
typedef struct Ucs_Mem_Buffer_
{
    struct Ucs_Mem_Buffer_  *next_buffer_ptr;   /*!< \brief Points to an additional memory buffer
                                                 *          that belongs to the same message. 
                                                 */
    uint8_t                 *data_ptr;          /*!< \brief Points to the data buffer */
    uint16_t                 data_size;         /*!< \brief Size of the data buffer */
    uint16_t                 total_size;        /*!< \brief Reserved for future use. Size of this and all concatenated data buffers */

} Ucs_Mem_Buffer_t;

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_MEMORY_PB_H */

/*! @} */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

