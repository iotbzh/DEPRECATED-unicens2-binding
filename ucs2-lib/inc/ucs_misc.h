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
 * \brief Internal header file of the library module which contains miscellaneous helper functions.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_MISC
 * @{
 */

#ifndef UCS_MISC_H
#define UCS_MISC_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Standard library functions                                                                     */
/*------------------------------------------------------------------------------------------------*/
/* parasoft suppress item MISRA2004-19_7 reason "function-like macros allowed for stdlib and helper functions" */

/*! \def     MISC_MEM_SET
 *  \brief   Macro to encapsulate memset function
 *  \details By defining the macro UCS_MEM_SET the application is able to specify its own memset
 *           function. If the macro is not defined UNICENS internal memset function
 *           Misc_MemSet() is used.
 *  \param   dest   Pointer to the block of memory to fill
 *  \param   value  Value to be set
 *  \param   size   Number of bytes to be set to the value.
 */
#ifdef UCS_MEM_SET
#define MISC_MEM_SET(dest, value, size)   (UCS_MEM_SET((dest), (value), (size)))
#else
#define MISC_MEM_SET(dest, value, size)   (Misc_MemSet((dest), (value), (size)))
#endif

/*! \def     MISC_MEM_CPY
 *  \brief   Macro to encapsulate memcpy function
 *  \details By defining the macro UCS_MEM_CPY the application is able to specify its own memcpy
 *           function. If the macro is not defined UNICENS internal memcpy function
 *           Misc_MemCpy() is used.
 *  \param   dest   Pointer to the destination array where the content is to be copied
 *  \param   src    Pointer to the source of data to be copied
 *  \param   size   Number of bytes to copy
 */
#ifdef UCS_MEM_CPY
#define MISC_MEM_CPY(dest, src, size)   (UCS_MEM_CPY((dest), (src), (size)))
#else
#define MISC_MEM_CPY(dest, src, size)   (Misc_MemCpy((dest), (src), (size)))
#endif

/*------------------------------------------------------------------------------------------------*/
/* Helper Macros                                                                                  */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Macro to avoid compiler warning "Unused Parameter" */
#define MISC_UNUSED(p)      ((p) = (p))

/*! \brief High Byte of 16-bit value */
#define MISC_HB(value)      ((uint8_t)((uint16_t)(value) >> 8))

/*! \brief Low Byte of 16-bit value */
#define MISC_LB(value)      ((uint8_t)((uint16_t)(value) & (uint16_t)0xFF))

/*! \brief Big-Endian to target 16 bit */
#define MISC_DECODE_WORD(w_ptr, msb_ptr) (*(w_ptr) = \
            (uint16_t)((uint16_t)((uint16_t)(msb_ptr)[0] << 8) | (uint16_t)(msb_ptr)[1]))

/*! \brief Big-Endian to target 32 bit */
#define MISC_DECODE_DWORD(dw_ptr, msb_ptr) (*(dw_ptr) = \
            (uint32_t)((uint32_t)((uint32_t)(msb_ptr)[0] << 24) | \
            (uint32_t)((uint32_t)(msb_ptr)[1] << 16) | \
            (uint32_t)((uint32_t)(msb_ptr)[2] << 8) | (uint32_t)(msb_ptr)[3]))

/*! \brief Checks if a value is inside a certain range */
#define MISC_IS_VALUE_IN_RANGE(val, min, max)   ((((val) >= (min)) && ((val) <= (max))) ? true : false)

/*! \brief Checks if the given size is a multiple of 4. If not, the given size is corrected 
 *         by that macro.
 */
#define MISC_QUADLET_ALGINED_SIZE(size)         (((((size)+4U)-1U)/4U)*4U)

/* parasoft unsuppress item MISRA2004-19_7 reason "function-like macros allowed for stdlib and helper functions" */

/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
extern void Misc_MemSet(void *dst_ptr, int32_t value, uint32_t size);
extern void Misc_MemCpy(void *dst_ptr, void *src_ptr, uint32_t size);

/*!
 * @}
 * \endcond
 */

/*!
 *  \def     UCS_MEM_SET
 *  \brief   Customer assignment of memset function
 *  \details By defining the macro UCS_MEM_SET the application is able to specify its own memset
 *           function to be used by UNICENS. If the macro is not set will use byte wise write operations.
 *  \ingroup G_UCS_MISC_CFG
 */ 
#ifndef UCS_MEM_SET
#define UCS_MEM_SET
#endif

/*!
 *  \def     UCS_MEM_CPY
 *  \brief   Customer assignment of memcpy function
 *  \details By defining the macro UCS_MEM_CPY the application is able to specify its own memcpy
 *           function to be used by UNICENS. If the macro is not set UNICENS will use byte wise copy operations.
 *  \ingroup G_UCS_MISC_CFG
 */ 
#ifndef UCS_MEM_CPY
#define UCS_MEM_CPY
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* UCS_MISC_H */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

