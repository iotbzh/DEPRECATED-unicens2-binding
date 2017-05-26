/*
 * Unicens Integration Helper Component
 *
 * Copyright (C) 2017 Microchip Technology Germany II GmbH & Co. KG
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You may also obtain this software under a propriety license from Microchip.
 * Please contact Microchip for further information.
 *
 */
#ifndef UCSI_H_
#define UCSI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ucs_config.h"
#include "ucs-xml/UcsXml.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                            Public API                                */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    
// Fulup interface added to support ucs2vol
typedef const struct {
    char* name;
    int numid;
} UCSI_channelsT;

typedef void (*UCSI_VolumeServiceCB_t)(uint16_t timeout);
UCSI_channelsT *UCSI_Vol_Init (UCSI_Data_t *pPriv, UCSI_VolumeServiceCB_t serviceCB);
uint8_t UCSI_Vol_Set  (UCSI_Data_t *pPriv, int numId, uint8_t volume);
void UCSI_Vol_Service (UCSI_Data_t *pPriv);


/**
 * \brief Initializes Unicens Integration module.
 * \note Must be called before any other function of this component
 *
 * \param pPriv - External allocated memory area for this particular
 *                instance (static allocated or allocated with malloc)
 * \param pTag - Pointer given by the integrator. This pointer will be
 *               returned by any callback function of this component
 */
void UCSI_Init(UCSI_Data_t *pPriv, void *pTag);


/**
 * \brief Executes the given configuration. If already started, all
 *        existing local and remote INIC resources will be destroyed
 * \note All given pointers must stay valid until this callback is
 *       raised: "UCSI_CB_OnStop"
 *
 * \param pPriv - private data section of this instance
 * \param ucsConfig - UCS config handle
 * \return true, configuration successfully enqueued, false otherwise
 */
bool UCSI_NewConfig(UCSI_Data_t *pPriv, UcsXmlVal_t *ucsConfig);

/**
 * \brief Offer the received control data from LLD to Unicens
 * \note Call this function only from single context (not from ISR)
 * \note This function can be called repeated until it return false
 *
 * \param pPriv - private data section of this instance
 * \param pBuffer - Received bytes from MOST control channel
 * \param len - Length of the received data array
 * \return true, if the data could be enqueued for processing, remove
 *         the data from LLD queue in this case.
 *         false, data could not be processed due to lag of resources.
 *         In this case do not discard the data. Offer the same
 *         data again after UCSI_CB_OnServiceRequired was
 *         raised or any time later.
 */
bool UCSI_ProcessRxData(UCSI_Data_t *pPriv, const uint8_t *pBuffer, uint16_t len);

/**
 * \brief Gives Unicens Integration module time to do its job
 * \note Call this function only from single context (not from ISR)
 *
 * \param pPriv - private data section of this instance
 */
void UCSI_Service(UCSI_Data_t *pPriv);


/**
 * \brief Call after timer set by UCSI_CB_OnSetServiceTimer
 *        expired.
 * \note Call this function only from single context (not from ISR)
 *
 * \param pPriv - private data section of this instance
 */
void UCSI_Timeout(UCSI_Data_t *pPriv);

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                        CALLBACK SECTION                              */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 * \brief Callback when ever a timestamp is needed
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \return timestamp in milliseconds
 */
extern uint16_t UCSI_CB_OnGetTime(void *pTag);


/**
 * \brief Callback when the implementer needs to arm a timer.
 * \note This function must be implemented by the integrator
 * \note After timer expired, call the UCSI_Timeout from service
 *       Thread. (Not from callback!)
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param timeout - milliseconds from now on to call back. (0=disable)
 */
extern void UCSI_CB_OnSetServiceTimer(void *pTag, uint16_t timeout);


/**
 * \brief Callback when ever an Unicens forms a human readable message.
 *        This can be error events or when enabled also debug messages.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param format - Zero terminated format string (following printf rules)
 * \param vargsCnt - Amount of parameters stored in "..."
 */
extern void UCSI_CB_OnUserMessage(void *pTag, const char format[],
    uint16_t vargsCnt, ...);


/**
 * \brief Callback when ever this instance needs to be serviced.
 * \note Call UCSI_Service by your scheduler at the next run
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 */
extern void UCSI_CB_OnServiceRequired(void *pTag);


/**
 * \brief Callback when ever a MOST error message was received.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \note All following parameters belong to the usual MOST message
 */
extern void UCSI_CB_OnMostError(void *pTag, uint16_t sourceAddr,
    uint8_t fblock, uint8_t inst, uint16_t function, uint8_t op,
    const uint8_t *pPayload, uint32_t payloadLen);


/**
 * \brief Callback when ever this instance wants to send a message to INIC.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param pPayload - Byte array to be sent on the INIC control channel
 * \param payloadLen - Length of pPayload in Byte
 */
extern void UCSI_CB_SendMostMessage(void *pTag,
    const uint8_t *pPayload, uint32_t payloadLen);

/**
 * \brief Callback when Unicens instance has been stopped.
 * \note This event can be used to free memory holding the resources
 *       passed with UCSI_NewConfig
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 */
extern void UCSI_CB_OnStop(void *pTag);

/**
 * \brief Callback on Unicens management results.
 * \note This function must be implemented by the integrator
 * \param pTag - Pointer given by the integrator by UCSI_Init
 * \param code - Result code
 * \param nodeAddress - Node address of the device causing this event
 * \param pNode - Pointer to node structure holding details of changed node
 */
extern void UCSI_CB_OnMgrReport(void *pTag, Ucs_MgrReport_t code, uint16_t nodeAddress, Ucs_Rm_Node_t *pNode);

#ifdef __cplusplus
}
#endif

#endif /* UCSI_H_ */