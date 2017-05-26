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
 * \brief Declaration of internal memory buffer
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_MEMORY
 * @{
 */

#ifndef UCS_MEMORY_H
#define UCS_MEMORY_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_memory_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* IAllocator Types                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Callback function which frees memory 
 *  \param      allocator       Reference to the Mem_Allocator_t object
 *  \param      mem_ptr         Reference to memory chunk 
 *  \param      mem_info_ptr    Customer specific information needed to free 
 *                              the related memory chunk
 */
typedef void  (*Mem_Free_t)(void *allocator, void* mem_ptr, void* mem_info_ptr);

/*! \brief      Callback function which allocated memory 
 *  \param      allocator       Reference to the Mem_Allocator_t object
 *  \param      size            Size of the demanded memory chunk 
 *  \param      mem_info_ptr    Customer specific information needed to free 
 *                              the related memory chunk
 *  \return     Reference to a memory chunk with a minimum size of \c size. 
 *              Otherwise NULL.
 */
typedef void* (*Mem_Allocate_t)(void *allocator, uint16_t size, void** mem_info_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Interface IAllocator                                                                           */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Interface which is needed to be implemented by a memory allocator */
typedef struct IAllocator_
{
    void*              base;            /*!< Reference to the base class */
    Mem_Allocate_t     allocate_fptr;   /*!< Callback function required to allocate memory */
    Mem_Free_t         free_fptr;       /*!< Callback function required to free memory */

} IAllocator;


/*------------------------------------------------------------------------------------------------*/
/* Memory buffer                                                                                  */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Memory chunk comprising non public fields */
typedef struct Mem_IntBuffer_
{
    Ucs_Mem_Buffer_t   public_buffer;  /*!< \brief      Public attributes of memory buffer
                                        *   \details    This has to be the first member in this 
                                        *               struct
                                        */
    IAllocator        *allocator_ptr;  /*!< \brief      Reference to the allocator which is 
                                        *               required to free the memory chunk 
                                        */
    void              *mem_info_ptr;   /*!< \brief      Customer specific information needed to 
                                        *               free the related memory chunk
                                        */
} Mem_IntBuffer_t;

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_MEMORY_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

