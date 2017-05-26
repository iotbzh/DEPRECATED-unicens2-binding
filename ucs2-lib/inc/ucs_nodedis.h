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
 * \brief Internal header file of class CNodeDiscovery.
 *
 * \cond UCS_INTERNAL_DOC
 */
#ifndef UCS_NODEDIS_H
#define UCS_NODEDIS_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_exc.h"


#ifdef __cplusplus
extern "C"
{
#endif


#define ND_NUM_NODES            40U     /*!< \brief max number of nodes */



/*! \brief Function signature of node evaluation callback used by Node Discovery service.
 *
 *  The Node Discovery service announces the signature of each node it has found to the 
 *  application via the evaluation function. In this function the application 
 *  decides how the Node Discovery service shall proceed with the node.
 *  The application maintains two lists:  
 * 
 *  <dl> 
 *      <dt> *set_list* </dt>
 *      <dd> Contains the signatures of the nodes the system shall contain
 *  
 *      <dt> *device_list* </dt>
 *      <dd> Contains the signatures of the nodes detected in the system
 *  </dl>
 *
 *  The evaluation has to follow these rules: 
 *  - If the node is not part of the *set_list*, it is regarded as unknown (\ref UCS_ND_CHK_UNKNOWN)
 *    and will be ignored. 
 *  - If the node is part of the *set_list* and is not yet in the *device_list*, the Node Discovery 
 *    Service shall try to add the node to network (\ref UCS_ND_CHK_WELCOME). 
 *  - If the node is already part of the *device_list*, there are two possibilities: the node in the 
 *    *device_list* experienced a reset or there are two nodes with the same signature. Evaluation 
 *    result is \ref UCS_ND_CHK_UNIQUE. The Node Discovery service will perform further tests.
 *
 *  \param    self         The instance
 *  \param    signature    Signature of the respective node
 *  \returns  UCS_ND_CHK_WELCOME  Node is ok, try to add it to the network.
 *  \returns  UCS_ND_CHK_UNIQUE   Test if this node is unique.
 *  \returns  UCS_ND_CHK_UNKNOWN  Node is unknown, no further action.
 *  \ingroup G_UCS_NODE_DISCOVERY
 */
typedef Ucs_Nd_CheckResult_t (*Nd_EvalCb_t)(void *self, Ucs_Signature_t *signature);

/*! \brief Function signature of result callback used by Node Discovery service.
 *
 *  The Node Discovery service reports the result of each node and some system events by
 *  this callback function.
 *  
 *  \note The parameter <b>signature</b> will be NULL, if parameter <b>code</b> is 
 *  \ref UCS_ND_RES_STOPPED, \ref UCS_ND_RES_NETOFF or \ref UCS_ND_RES_ERROR.
 *
 *  \param   self         The instance
 *  \param   code         Result code 
 *  \param   signature    Signature of the respective node
 *  \ingroup G_UCS_NODE_DISCOVERY
 */
typedef void (*Nd_ReportCb_t)(void *self, Ucs_Nd_ResCode_t code, Ucs_Signature_t *signature);


/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Structure decribing a node. */
typedef struct Nd_Node_
{
    /*bool               available; */      /*!< \brief node available? *//*! i todo RWI:  */
    /*uint16_t           node_address; */   /*!< \brief node address used for welcome command */
    /*uint8_t            result;     */     /*!< \brief result parameter of Welcome.Result message */
    /*uint8_t            version;  */       /*!< \brief version parameter of Hello and Welcome messages */
    Ucs_Signature_t    signature;           /*!< \brief signature of the node */
    CDlNode            node;                /*!< \brief enables listing */  

} Nd_Node;


/*! \brief  Initialization structure of the Node Discovery service. */
typedef struct Nd_InitData_
{
    void               *inst_ptr;           /*!< \brief The instance used when invoking the callback functions */           
    Nd_ReportCb_t       report_fptr;        /*!< \brief Report callback function */
    Nd_EvalCb_t         eval_fptr;          /*!< \brief Evaluation callback function */

} Nd_InitData_t;




/*! \brief   Structure of class CNodeDiscovery. */
typedef struct CNodeDiscovery_
{
    CInic   *inic;                      /*!< \brief Reference to CInic object */
    CExc    *exc;                       /*!< \brief Reference to CExc object */
    CBase   *base;                      /*!< \brief Reference to CBase object */

    bool    running;                    /*!< \brief Indicates th Node Discovery is running. */
    CSingleObserver nd_hello;           /*!< \brief Observes the Hello  result */
    CSingleObserver nd_welcome;         /*!< \brief Observes the Welcome result */
    CSingleObserver nd_signature;       /*!< \brief Observes the Signature result */
    CSingleObserver nd_init;            /*!< \brief Observes the DeviceInit result */ 

    CMaskedObserver nd_terminate;       /*!< \brief Observes events leading to termination */

    CObserver       nd_nwstatus;        /*!< \brief Observes the MOST Network status */

    CFsm     fsm;                       /*!< \brief Node Discovery state machine  */
    CService service;                   /*!< \brief Service instance for the scheduler */

    CTimer   timer;                     /*!< \brief timer for monitoring messages */


    CDlList  new_list;                  /*!< \brief list of detected nodes */
    CDlList  unused_list;               /*!< \brief list of unused node elements */
    Nd_Node  nodes[ND_NUM_NODES];       /*!< \brief device nodes */ 
    Ucs_Signature_t  current_sig;       /*!< \brief node which is checked currently */ 

    Exc_WelcomeResult_t   welcome_result;       /*!< \brief buffer for welcome result */
    Exc_SignatureStatus_t signature_status;     /*!< \brief buffer for signature status */

    bool stop_request;                  /*!< \brief indicates a request to stop node discovery */
    bool hello_mpr_request;             /*!< \brief indicates an Hello.Get request due to an MPR event*/
    bool hello_neton_request;           /*!< \brief indicates an Hello.Get request due to an NetOn event*/
    bool neton;                         /*!< \brief indicates Network availability */

    void *cb_inst_ptr;                  /*!< \brief Instance required for callback functions */
    Nd_ReportCb_t report_fptr;          /*!< \brief Report callback function */
    Nd_EvalCb_t eval_fptr;              /*!< \brief Node evaluation callback function */

}CNodeDiscovery;



/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
void Nd_Ctor(CNodeDiscovery *self, 
             CInic *inic, 
             CBase *base, 
             CExc *exc, 
             Nd_InitData_t *init_ptr);


extern Ucs_Return_t Nd_Start(CNodeDiscovery *self);
extern Ucs_Return_t Nd_Stop(CNodeDiscovery *self);
extern void Nd_InitAll(CNodeDiscovery *self);




#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* UCS_NODEDIS_H */
/*!
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

