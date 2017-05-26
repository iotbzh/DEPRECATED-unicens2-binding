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
 * \brief Internal header file of the Command Decoder Module.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_DEC_INT
 * @{
 */

#ifndef UCS_DEC_H
#define UCS_DEC_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_dl.h"
#include "ucs_message.h"
/*#include "ucs_ams.h"*/

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Constants                                                                                      */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Stores FktID and OPType in one 16 bit value */
#define DEC_FKTOP(a,b) ((uint16_t)((((uint16_t)((a)<<4)) & (uint16_t)0xFFF0)) | ((uint16_t)(((uint16_t)(b)) & (uint16_t)0x000F))) /* parasoft-suppress  MISRA2004-19_7 "Is used in arrays and therefore cannot be converted to inline function." */

/*! \brief Denotes the end of an FktOp table */
#define DEC_FKTOP_TERMINATION   0xFFFFU

/*------------------------------------------------------------------------------------------------*/
/* Enumerations                                                                                   */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Return codes used for decoding functions */
/* Attention: these values are used as error descriptor in error messages and
              must therefore not changed!
*/
typedef enum Dec_Return_
{
    DEC_RET_SUCCESS          = 0x00,        /*!< \brief Operation successfully completed */
    DEC_RET_FKTID_NOT_FOUND  = 0x03,        /*!< \brief FunctionID not found */
    DEC_RET_OPTYPE_NOT_FOUND = 0x04         /*!< \brief Operation Type not found */

} Dec_Return_t;


/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
typedef void (*Dec_IcmCb_t)(void *self, Msg_MostTel_t *msg_ptr);
typedef void (*Dec_RcmCb_t)(void *self, Msg_MostTel_t *msg_ptr);




/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Structure of an FktID_OpType element for FBlock INIC */
typedef struct Dec_FktOpIcm_
{
    /*!< \brief FktID and OPType (combined to a 16-bit value) */
    uint16_t fkt_op;
    /*!< \brief pointer to the belonging handler function */
    Dec_IcmCb_t handler_function_ptr;

} Dec_FktOpIcm_t;

/*! \brief   Structure of an FktID_OpType element for internal INIC Shadow FBlocks e.g. FBlock EXC*/
typedef struct Dec_FktOpIsh_
{
    /*! \brief FktID and OPType (combined to a 16-bit value) */
    uint16_t fkt_op;
    /*! \brief pointer to the belonging handler function */
    Dec_RcmCb_t handler_function_ptr;

} Dec_FktOpIsh_t;


/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
extern Dec_Return_t Dec_SearchFktOpIcm(Dec_FktOpIcm_t const list[], uint16_t *index_ptr,
                                       uint16_t function_id, Ucs_OpType_t op_type);
extern Dec_Return_t Dec_SearchFktOpIsh(Dec_FktOpIsh_t const list[], uint16_t *index_ptr,
                                       uint16_t function_id, Ucs_OpType_t op_type);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_DEC_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

