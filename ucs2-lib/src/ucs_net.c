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
 * \brief Implementation of the Network Management.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_NET
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_net.h"
#include "ucs_misc.h"
#include "ucs_trace.h"

/*------------------------------------------------------------------------------------------------*/
/* Service parameters                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! Priority of the NET service used by scheduler */
static const uint8_t NET_SRV_PRIO                   = 251U;   /* parasoft-suppress  MISRA2004-8_7 "Value shall be part of the module, not part of a function." */
/*! \brief Event to trigger notification of MOST Network Status */
static const Srv_Event_t NET_EVENT_NOTIFY_NW_STATUS = 1U;
/*! \brief Event to trigger notification of MOST Network Configuration */
static const Srv_Event_t NET_EVENT_NOTIFY_NW_CONFIG = 2U;

/*------------------------------------------------------------------------------------------------*/
/* Internal constants                                                                             */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Default value used for INIC sender handles */
static const uint16_t NET_DEFAULT_SENDER_HANDLE = 0x0001U;  /* parasoft-suppress  MISRA2004-8_7 "Value shall be part of the module, not part of a function." */
/*! \brief Initialization timeout in milliseconds (t = 7s) */
static const uint16_t NET_PBW_TIMEOUT           = 7000U;    /* parasoft-suppress  MISRA2004-8_7 "Value shall be part of the module, not part of a function." */

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static void Net_Service(void *self);
static void Net_UpdateNetworkStatus(void *self, void *data_ptr);
static void Net_UpdateNetworkConfiguration(void *self, void *data_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Implementation of class CNetworkManagement                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Constructor of the Network Management class.
 *  \param self        Instance pointer
 *  \param init_ptr    Reference to the initialization data
 */
void Net_Ctor(CNetworkManagement *self, Net_InitData_t *init_ptr)
{
    MISC_MEM_SET(self, 0, sizeof(*self));
    self->base_ptr = init_ptr->base_ptr;
    self->inic_ptr = init_ptr->inic_ptr;
    Obs_Ctor(&self->network_status.observer, self, &Net_UpdateNetworkStatus);
    Inic_AddObsrvNwStatus(self->inic_ptr, &self->network_status.observer);
    self->network_status.param.change_mask = 0xFFFFU;               /* Used for initial notification! */
    Sub_Ctor(&self->network_status.pre_subject, self->base_ptr->ucs_user_ptr);
    Sub_Ctor(&self->network_status.subject, self->base_ptr->ucs_user_ptr);

    Obs_Ctor(&self->network_configuration.observer, self, &Net_UpdateNetworkConfiguration);
    Inic_AddObsvrNwConfig(self->inic_ptr, &self->network_configuration.observer);
    self->network_configuration.param.change_mask = 0xFFFFU;        /* Used for initial notification! */
    Sub_Ctor(&self->network_configuration.pre_subject, self->base_ptr->ucs_user_ptr);
    Sub_Ctor(&self->network_configuration.subject, self->base_ptr->ucs_user_ptr);

    Srv_Ctor(&self->net_srv, NET_SRV_PRIO, self, &Net_Service);     /* Initialize Network Management service */
    (void)Scd_AddService(&self->base_ptr->scd, &self->net_srv);     /* Add NET service to scheduler */
}

/*! \brief Service function of the network management.
 *  \param self    Instance pointer
 */
static void Net_Service(void *self)
{
    CNetworkManagement *self_ = (CNetworkManagement *)self;
    Srv_Event_t event_mask;
    Srv_GetEvent(&self_->net_srv, &event_mask);
    /* Notification of MOST Network Status triggered? */
    if((event_mask & NET_EVENT_NOTIFY_NW_STATUS) == NET_EVENT_NOTIFY_NW_STATUS)
    {
        Srv_ClearEvent(&self_->net_srv, NET_EVENT_NOTIFY_NW_STATUS);
        self_->network_status.param.change_mask = 0xFFFFU;
        Sub_Notify(&self_->network_status.pre_subject, &self_->network_status.param);
        self_->network_status.param.change_mask = 0U;
        (void)Sub_SwitchObservers(&self_->network_status.subject,
                                  &self_->network_status.pre_subject);
    }
    /* Notification of MOST Network Configuration triggered? */
    if((event_mask & NET_EVENT_NOTIFY_NW_CONFIG) == NET_EVENT_NOTIFY_NW_CONFIG)
    {
        Srv_ClearEvent(&self_->net_srv, NET_EVENT_NOTIFY_NW_CONFIG);
        self_->network_configuration.param.change_mask = 0xFFFFU;
        Sub_Notify(&self_->network_configuration.pre_subject, &self_->network_configuration.param);
        self_->network_configuration.param.change_mask = 0U;
        (void)Sub_SwitchObservers(&self_->network_configuration.subject,
                                  &self_->network_configuration.pre_subject);
    }
}

/*! \brief Adds an observer which is called if the network status has been changed.
 *  \param self                Instance pointer
 *  \param obs_ptr             Reference to an observer
 */
void Net_AddObserverNetworkStatus(CNetworkManagement *self, CMaskedObserver *obs_ptr)
{
    (void)Msub_AddObserver(&self->network_status.pre_subject, obs_ptr);
    Srv_SetEvent(&self->net_srv, NET_EVENT_NOTIFY_NW_STATUS);
}

/*! \brief Removes an observer registered by Net_AddObserverNetworkStatus().
 *  \param self        Instance pointer
 *  \param obs_ptr     Reference to observer to be removed

 */
void Net_DelObserverNetworkStatus(CNetworkManagement *self, CMaskedObserver *obs_ptr)
{
    (void)Msub_RemoveObserver(&self->network_status.pre_subject, obs_ptr);
    (void)Msub_RemoveObserver(&self->network_status.subject, obs_ptr);
}

/*! \brief Adds an observer which is called if the network configuration has been changed.
 *  \param self                Instance pointer
 *  \param obs_ptr             Reference to an observer
 */
void Net_AddObserverNetworkConfig(CNetworkManagement *self, CMaskedObserver *obs_ptr)
{
    (void)Msub_AddObserver(&self->network_configuration.pre_subject, obs_ptr);
    Srv_SetEvent(&self->net_srv, NET_EVENT_NOTIFY_NW_CONFIG);
}

/*! \brief Removes an observer registered by Net_AddObserverNetworkConfig().
 *  \param self        Instance pointer
 *  \param obs_ptr     Reference to observer to be removed
 */
void Net_DelObserverNetworkConfig(CNetworkManagement *self, CMaskedObserver *obs_ptr)
{
    (void)Msub_RemoveObserver(&self->network_configuration.pre_subject, obs_ptr);
    (void)Msub_RemoveObserver(&self->network_configuration.subject, obs_ptr);
}

/*! \brief Observer callback used for the MOST Network Status
 *  \param self        Instance pointer
 *  \param data_ptr    Reference to the data structure
 */
static void Net_UpdateNetworkStatus(void *self, void *data_ptr)
{
    Inic_StdResult_t *data_ptr_ = (Inic_StdResult_t *)data_ptr;

    if(data_ptr_->result.code == UCS_RES_SUCCESS)
    {
        CNetworkManagement *self_ = (CNetworkManagement *)self;
        Inic_NetworkStatus_t result = *((Inic_NetworkStatus_t *)data_ptr_->data_info);

        /* Check for changes */
        if(result.events != 0U)     /* Notify only if at least one event flag is set */
        {
            self_->network_status.param.change_mask |= 0x0001U;
        }
        if(self_->network_status.param.availability != result.availability)
        {
            self_->network_status.param.change_mask |= 0x0002U;
        }
        if(self_->network_status.param.avail_info != result.avail_info)
        {
            self_->network_status.param.change_mask |= 0x0004U;
        }
        if(self_->network_status.param.avail_trans_cause != result.avail_trans_cause)
        {
            self_->network_status.param.change_mask |= 0x0008U;
        }
        if(self_->network_status.param.node_address != result.node_address)
        {
            self_->network_status.param.change_mask |= 0x0010U;
        }
        if(self_->network_status.param.node_position != result.node_position)
        {
            self_->network_status.param.change_mask |= 0x0020U;
        }
        if(self_->network_status.param.max_position != result.max_position)
        {
            self_->network_status.param.change_mask |= 0x0040U;
        }
        if(self_->network_status.param.packet_bw != result.packet_bw)
        {
            self_->network_status.param.change_mask |= 0x0080U;
        }

        /* Update MOST Network Status parameters */
        self_->network_status.param.events            = result.events;
        self_->network_status.param.availability      = result.availability;
        self_->network_status.param.avail_info        = result.avail_info;
        self_->network_status.param.avail_trans_cause = result.avail_trans_cause;
        self_->network_status.param.node_address      = result.node_address;
        self_->network_status.param.node_position     = result.node_position;
        self_->network_status.param.max_position      = result.max_position;
        self_->network_status.param.packet_bw         = result.packet_bw;

        /* Notify observer? */
        Msub_Notify(&self_->network_status.subject,
                    &self_->network_status.param,
                    (uint32_t)self_->network_status.param.change_mask);

        /* Clear change-mask */
        self_->network_status.param.change_mask = 0U;
    }
}

/*! \brief Observer callback used for the MOST Network Configuration
 *  \param self        Instance pointer
 *  \param data_ptr    Reference to the data structure
 */
static void Net_UpdateNetworkConfiguration(void *self, void *data_ptr)
{
    Inic_StdResult_t *data_ptr_ = (Inic_StdResult_t *)data_ptr;

    if(data_ptr_->result.code == UCS_RES_SUCCESS)
    {
        CNetworkManagement *self_ = (CNetworkManagement *)self;
        Inic_NetworkConfig_t result = *((Inic_NetworkConfig_t *)data_ptr_->data_info);

        /* Check for changes */
        if(self_->network_configuration.param.node_address != result.node_address)
        {
            self_->network_configuration.param.change_mask |= 0x0001U;
        }
        if(self_->network_configuration.param.group_address != result.group_address)
        {
            self_->network_configuration.param.change_mask |= 0x0002U;
        }
        if(self_->network_configuration.param.llrbc != result.llrbc)
        {
            self_->network_configuration.param.change_mask |= 0x0004U;
        }

        /* Update MOST Network Configuration parameters */
        self_->network_configuration.param.node_address  = result.node_address;
        self_->network_configuration.param.group_address = result.group_address;
        self_->network_configuration.param.llrbc         = result.llrbc;

        /* Notify observer? */
        Msub_Notify(&self_->network_configuration.subject,
                    &self_->network_configuration.param,
                    (uint32_t)self_->network_configuration.param.change_mask);

        /* Clear change-mask */
        self_->network_configuration.param.change_mask = 0U;
    }
}

/*! \brief Checks if the given address matches with the own node address, node position or group address.
 *  \param self     Instance pointer
 *  \param address  Address to be checked
 *  \return Possible return values are shown in the table below.
 *          Value                 | Description 
 *          --------------------- | -------------------------------------------------------------
 *          NET_IS_OWN_ADDR_NODE  | Is own node position address or own logical node address
 *          NET_IS_OWN_ADDR_GROUP | Is own group address
 *          NET_IS_OWN_ADDR_NONE  | Is foreign address
 */
Net_IsOwnAddrResult_t Net_IsOwnAddress(CNetworkManagement *self, uint16_t address)
{
    Net_IsOwnAddrResult_t ret_val;

    if((self->network_status.param.node_address == address) ||
       (((uint16_t)self->network_status.param.node_position + (uint16_t)0x400) == address))
    {
        ret_val = NET_IS_OWN_ADDR_NODE;
    }
    else if(self->network_configuration.param.group_address == address)
    {
        ret_val = NET_IS_OWN_ADDR_GROUP;
    }
    else
    {
        ret_val = NET_IS_OWN_ADDR_NONE; 
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

