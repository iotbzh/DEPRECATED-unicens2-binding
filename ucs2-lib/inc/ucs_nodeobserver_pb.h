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
 * \brief Public header file of the CNodeObserver class
 */
/*!
 * \addtogroup G_UCS_MGR
 * @{
 */

#ifndef UCS_NODEOBSERVER_PB_H
#define UCS_NODEOBSERVER_PB_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_rules.h"
#include "ucs_rm_pb.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! \brief Manager report codes */
typedef enum Ucs_MgrReport_
{
    UCS_MGR_REP_IGNORED_UNKNOWN   = 0, /*!< \brief A discovered node is ignored due to a wrong signature,
                                        *          a missing entry in the \ref Ucs_Mgr_InitData_t "nodes_list_ptr",
                                        *          or since the desired node address is not within the following range:
                                        *          0x200..0x2FF, 0x500..0xEFF.
                                        */
    UCS_MGR_REP_IGNORED_DUPLICATE = 1, /*!< \brief A discovered node is ignored due since it is a duplicate
                                        *          of an alredy welcomed node.
                                        */
    UCS_MGR_REP_AVAILABLE         = 2, /*!< \brief A discovered node was successfully "welcomed" 
                                        *          in the network.
                                        */
    UCS_MGR_REP_NOT_AVAILABLE     = 3  /*!< \brief A previously welcomed node became invalid and is 
                                        *          no longer accessible in the network.
                                        */
} Ucs_MgrReport_t;

/*! \brief Optional callback function that reports events on ignored, welcomed and lost nodes.
 *  \param code             Report code
 *  \param node_address     The desired node_address of the node which is defined in it's signature.
 *  \param node_ptr         Reference to the node object which is part of the \ref Ucs_Mgr_InitData_t "nodes_list_ptr".
 *                          The reference is \c NULL if \c code is \c UCS_MGR_REP_IGNORED_UNKNOWN or 
 *                          \c UCS_MGR_REP_IGNORED_DUPLICATE.
 *  \param user_ptr         User reference provided in \ref Ucs_InitData_t "Ucs_InitData_t::user_ptr".
 */
typedef void (*Ucs_MgrReportCb_t)(Ucs_MgrReport_t code, uint16_t node_address, Ucs_Rm_Node_t *node_ptr, void *user_ptr);

/*! \brief   The initialization data of the Manager */
typedef struct Ucs_Mgr_InitData_
{
    bool enabled;                       /*!< \brief If set to \c false the application must
                                         *          handle network startup, node discovery and
                                         *          rooting by hand.
                                         */
    uint16_t packet_bw;                 /*!< \brief The desired packet bandwidth.\mns_name_inic{PacketBW} */
    
    Ucs_Rm_Route_t *routes_list_ptr;    /*!< \brief Reference to a list of routes */
    uint16_t routes_list_size;          /*!< \brief Number of routes in the list */
    
    Ucs_Rm_Node_t *nodes_list_ptr;      /*!< \brief Reference to the list of nodes */
    uint16_t nodes_list_size;           /*!< \brief Number of nodes in the list */

    Ucs_MgrReportCb_t report_fptr;      /*!< \brief Optional callback function notifying node events */

} Ucs_Mgr_InitData_t;


#ifdef __cplusplus
}               /* extern "C" */
#endif

#endif  /* ifndef UCS_NODEOBSERVER_PB_H */

/*! @} */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

