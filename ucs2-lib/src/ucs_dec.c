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
 * \brief Implementation of the Command Interpreter Module.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_DEC_INT
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_dec.h"
#include "ucs_misc.h"
#include "ucs_ret_pb.h"

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Search in a FktOp table for matching FktID and OPType. This function is used for
 *          incoming ICM messages.
 *  \param   list            FktOp table
 *  \param   index_ptr       Reference to array index of the matching array element
 *  \param   function_id     FktID
 *  \param   op_type         OPType
 *  \return  DEC_RET_SUCCESS                 Decoding was successful
 *           DEC_RET_FKTID_NOT_FOUND         FktID/OPType not found
 */
Dec_Return_t Dec_SearchFktOpIcm(Dec_FktOpIcm_t const list[], uint16_t *index_ptr, 
                                uint16_t function_id, Ucs_OpType_t op_type)
{
    uint16_t     fktop;
    uint16_t     i       = 0U;
    Dec_Return_t ret_val = DEC_RET_FKTID_NOT_FOUND;
    bool         loop    = true;

    fktop = DEC_FKTOP(function_id, op_type);
    *index_ptr = 0U;

    while ((list[i].handler_function_ptr != NULL)  && (loop != false))
    {
        if(list[i].fkt_op == fktop)
        {
            ret_val = DEC_RET_SUCCESS;
            *index_ptr = i;
            loop = false;
        }
        else if (list[i].fkt_op > fktop)
        {
            loop = false;
        }
        else
        {
            i++;
        }
    }

    return ret_val;
}

/*! \brief  Search in a FktOp table for matching FktID and OPType. This function is used for
 *          MCM messages coming from FBlocks inside the INIC.
 *  \param   list            FktOp table
 *  \param   index_ptr       Reference to array index of the matching array element
 *  \param   function_id     FktID
 *  \param   op_type         OPType
 *  \return  DEC_RET_SUCCESS                 Decoding was successful
 *           DEC_RET_FKTID_NOT_FOUND         FktID/OPType not found
 */
Dec_Return_t Dec_SearchFktOpIsh(Dec_FktOpIsh_t const list[], uint16_t *index_ptr, 
                                uint16_t function_id, Ucs_OpType_t op_type)
{
    uint16_t     fktop;
    uint16_t     i       = 0U;
    Dec_Return_t ret_val = DEC_RET_FKTID_NOT_FOUND;
    bool         loop    = true;

    fktop = DEC_FKTOP(function_id, op_type);
    *index_ptr = 0U;

    while ((list[i].handler_function_ptr != NULL)  && (loop != false))
    {
        if(list[i].fkt_op == fktop)
        {
            ret_val = DEC_RET_SUCCESS;
            *index_ptr = i;
            loop = false;
        }
        else if (list[i].fkt_op > fktop)
        {
            loop = false;
        }
        else
        {
            i++;
        }
    }

    return ret_val;
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

