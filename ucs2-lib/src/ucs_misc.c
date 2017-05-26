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
 * \brief Implementation of the library module which contains miscellaneous helper functions.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_MISC
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief UNICENS internal memset-function.
 *  \param dst_ptr Pointer to the block of memory to fill
 *  \param value   Value to be set
 *  \param size    Number of bytes to be set to the value
 */
void Misc_MemSet(void *dst_ptr, int32_t value, uint32_t size)
{
    uint8_t *dst_ptr_ = (uint8_t *)dst_ptr;
    uint32_t i;

    for(i=0U; i<size; i++)
    {
        dst_ptr_[i] = (uint8_t)value;   /* parasoft-suppress  MISRA2004-17_4 "void pointer required for memset-function signature (stdlib)" */
    }
}

/*! \brief UNICENS internal memcpy-function.
 *  \param dst_ptr Pointer to the destination array where the content is to be copied
 *  \param src_ptr Pointer to the source of data to be copied
 *  \param size    Number of bytes to copy
 */
void Misc_MemCpy(void *dst_ptr, void *src_ptr, uint32_t size)
{
    uint8_t *dst_ptr_ = (uint8_t *)dst_ptr;
    uint8_t *src_ptr_ = (uint8_t *)src_ptr;
    uint32_t i;

    for(i=0U; i<size; i++)
    {
        dst_ptr_[i] = src_ptr_[i];  /* parasoft-suppress  MISRA2004-17_4 "void pointers required for memcpy-function signature (stdlib)" */
    }
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

