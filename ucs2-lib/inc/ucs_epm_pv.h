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
 * \brief Public header file of the Extended Resource Manager.
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_EPM
 * @{
 */

#ifndef UCS_EPM_PB_H
#define UCS_EPM_PB_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_xrm_pb.h"
#include "ucs_obs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief This enumerator specifies the state of EndPoint objects. */
typedef enum Ucs_Rm_EndPointState_
{
    UCS_RM_EP_IDLE               = 0x00U,     /*!< \brief Specifies the "Idle" state of the endpoint. This means that the endpoint has not been handled yet  */
    UCS_RM_EP_XRMPROCESSING      = 0x01U,     /*!< \brief Specifies that the EndPoint is under "XRM process". */
    UCS_RM_EP_BUILT              = 0x02U      /*!< \brief Specifies that the EndPoint is "Built". */

} Ucs_Rm_EndPointState_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/* Epm_Inst_t requires incomplete forward declaration, to hide internal data type.
 * The Epm_Inst_t object is allocated internally, the core library must access only the pointer to Epm_Inst_t. */
struct Epm_Inst_;

/*!\brief  EndpointManagement instance */
typedef struct Epm_Inst_ Epm_Inst_t;

/*! \brief Internal configuration structure of a Connection EndPoint. */
typedef struct Ucs_Rm_EndPointInt_
{
    /*! \brief Stores the current number of retries in case of error. */
    uint8_t num_retries;
    /*! \brief State of the endpoint object. */
    Ucs_Rm_EndPointState_t endpoint_state;
    /*! \brief connection label. */
    uint16_t connection_label;
    /*! \brief object counter. */
    uint8_t reference_cnt;
    /*! \brief last XRM result. */
    Ucs_Xrm_Result_t xrm_result;
    /*! \brief A subject object for this endpoint. */
    CSubject subject_obj;
    /*! \brief Reference to the EndPointManagement that handles this endpoint */
    Epm_Inst_t * epm_inst;
    /*! \brief magic number to signal that endpoint has already been initialized */
    uint32_t magic_number;

} Ucs_Rm_EndPointInt_t;

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_EPM_PB_H */

/*!
 * @}
 * \endcond
 */


/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

