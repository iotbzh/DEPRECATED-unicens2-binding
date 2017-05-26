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
 * \brief Public header file of the Routing Management.
 * \addtogroup G_UCS_ROUTING_TYPES
 * @{
 */

#ifndef UCS_RM_PB_H
#define UCS_RM_PB_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rm_pv.h"
#include "ucs_nsm_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief This enumerator specifies the type of an EndPoint object. */
typedef enum Ucs_Rm_EndPointType_
{
    UCS_RM_EP_SOURCE = 0x00U,    /*!< \brief Specifies the source endpoint. */
    UCS_RM_EP_SINK   = 0x01U     /*!< \brief Specifies the sink endpoint. */

} Ucs_Rm_EndPointType_t;

/*! \brief This enumerator specifies the possible route information returned via User callback function of \ref Ucs_Rm_Start(). */
typedef enum Ucs_Rm_RouteInfos_
{
    UCS_RM_ROUTE_INFOS_BUILT          = 0x00U,     /*!< \brief Specifies that the route has been built. */
    UCS_RM_ROUTE_INFOS_DESTROYED      = 0x01U,     /*!< \brief Specifies that the route has been destroyed. */
    UCS_RM_ROUTE_INFOS_SUSPENDED      = 0x02U,     /*!< \brief Specifies that the route has been suspended. */
    UCS_RM_ROUTE_INFOS_PROCESS_STOP   = 0x03U      /*!< \brief Specifies that the route cannot be processed anymore because of UNICENS Termination. */

} Ucs_Rm_RouteInfos_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Configuration structure of a Connection EndPoint. */
typedef struct Ucs_Rm_EndPoint_
{
    /*! \brief Type of the endpoint object. */
    Ucs_Rm_EndPointType_t endpoint_type;
    /*! \brief Reference to a job list. */
    Ucs_Xrm_ResObject_t ** jobs_list_ptr;
    /*! \brief Reference to a node object. */
    Ucs_Rm_Node_t * node_obj_ptr;
    /*! \brief Internal information of this endpoint object. */
    Ucs_Rm_EndPointInt_t internal_infos;

} Ucs_Rm_EndPoint_t;

/*! \brief Configuration structure of a Route. */
typedef struct Ucs_Rm_Route_
{
    /*! \brief Reference to a Source Endpoint object. */
    Ucs_Rm_EndPoint_t * source_endpoint_ptr;
    /*! \brief Reference to a Sink Endpoint object. */
    Ucs_Rm_EndPoint_t * sink_endpoint_ptr;
    /*! \brief Route activity. Specifies whether the route is active yet or not. */
    uint8_t active;
    /*! \brief User-defined route identifier. */
    uint16_t route_id;
    /*! \brief Internal information of the route object. */
    Ucs_Rm_RouteInt_t internal_infos; 

} Ucs_Rm_Route_t;

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_RM_PB_H */

/*! @} */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

