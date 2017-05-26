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
 * \brief Implementation of the class CStaticMemoryManager.
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_UCS_SMM_CLASS
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_smm.h"
#include "ucs_misc.h"
#include "ucs_trace.h"

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static Smm_Descriptor_t* Smm_GetTypeDescriptor(CStaticMemoryManager *self, Ams_MemUsage_t type);
static void* Smm_Allocate(void *self, uint16_t mem_size, Ams_MemUsage_t type, void** custom_info_pptr);
static void Smm_Free(void *self, void *mem_ptr, Ams_MemUsage_t type, void* custom_info_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Constructor of the static memory manager
 *  \param self         The instance
 *  \param ucs_user_ptr User reference that needs to be passed in every callback function
 */
void Smm_Ctor(CStaticMemoryManager *self, void *ucs_user_ptr)
{
    uint8_t index;
    self->ucs_user_ptr = ucs_user_ptr;

    Dl_Ctor(&self->tx_object_descr.list, ucs_user_ptr);              /* initialize descriptor lists */
    Dl_Ctor(&self->rx_object_descr.list, ucs_user_ptr);
    Dl_Ctor(&self->tx_payload_descr.list, ucs_user_ptr);
    Dl_Ctor(&self->rx_payload_descr.list, ucs_user_ptr);
    Dl_Ctor(&self->null_descr.list, ucs_user_ptr);

    self->tx_object_descr.max_mem_size = AMSG_TX_OBJECT_SZ;         /* initialize descriptor memory sizes */
    self->rx_object_descr.max_mem_size = AMSG_RX_OBJECT_SZ;
    self->tx_payload_descr.max_mem_size = SMM_SIZE_TX_MSG;
    self->rx_payload_descr.max_mem_size = SMM_SIZE_RX_MSG;
    self->null_descr.max_mem_size = 0U;

    for (index = 0U; index < SMM_NUM_TX_MSGS; index++)              /* initialize Tx objects and payload */
    {                                                               /* CDlNode::data_ptr has to point to the memory */
        Dln_Ctor(&self->resources.tx_objects[index].node, &self->resources.tx_objects[index].object);
        Dl_InsertTail(&self->tx_object_descr.list, &self->resources.tx_objects[index].node);

        Dln_Ctor(&self->resources.tx_payload[index].node, &self->resources.tx_payload[index].data);
        Dl_InsertTail(&self->tx_payload_descr.list, &self->resources.tx_payload[index].node);
    }

    for (index = 0U; index < SMM_NUM_RX_MSGS; index++)              /* initialize Rx objects and payload */
    {                                                               /* CDlNode::data_ptr has to point to the memory */
        Dln_Ctor(&self->resources.rx_objects[index].node, &self->resources.rx_objects[index].object);
        Dl_InsertTail(&self->rx_object_descr.list, &self->resources.rx_objects[index].node);

        Dln_Ctor(&self->resources.rx_payload[index].node, &self->resources.rx_payload[index].data);
        Dl_InsertTail(&self->rx_payload_descr.list, &self->resources.rx_payload[index].node);
    }
}

/*! \brief  Load function of the static memory management plug-in.
 *  \param  self                The instance
 *  \param  allocator_ptr       Assignable interface for allocate and free functions
 *  \param  rx_def_payload_size The default Rx allocation size the AMS uses if TelId "4" is missing.
 *                              Just use for checks. Do not overrule.
 *  \return Returns \c UCS_RET_SUCCESS if the initialization succeeded, otherwise \c UCS_RET_ERR_PARAM.
 */
Ucs_Return_t Smm_LoadPlugin(CStaticMemoryManager *self, Ams_MemAllocator_t *allocator_ptr, uint16_t rx_def_payload_size)
{
    Ucs_Return_t ret = UCS_RET_SUCCESS;

    allocator_ptr->inst_ptr = self;             /* assign instance to allocator */
    allocator_ptr->alloc_fptr = &Smm_Allocate;  /* assign callback functions */
    allocator_ptr->free_fptr = &Smm_Free;

    if (rx_def_payload_size != SMM_SIZE_RX_MSG)
    {
        ret = UCS_RET_ERR_PARAM;
        TR_ERROR((self->ucs_user_ptr, "[SMM]", "SMM initialization failed: wrong configuration of rx_def_payload_size.", 0U));
    }

    return ret;
}

/*! \brief  Retrieves a descriptor for a memory type
 *  \param  self  The instance
 *  \param  type  Usage type of the requested memory
 *  \return Returns the respective descriptor for a memory type
 */
static Smm_Descriptor_t* Smm_GetTypeDescriptor(CStaticMemoryManager *self, Ams_MemUsage_t type)
{
    Smm_Descriptor_t* descr_ptr = NULL;

    switch (type)
    {
        case AMS_MU_RX_OBJECT:
            descr_ptr = &self->rx_object_descr;
            break;
        case AMS_MU_RX_PAYLOAD:
            descr_ptr = &self->rx_payload_descr;
            break;
        case AMS_MU_TX_OBJECT:
            descr_ptr = &self->tx_object_descr;
            break;
        case AMS_MU_TX_PAYLOAD:
            descr_ptr = &self->tx_payload_descr;
            break;
        default:
            TR_FAILED_ASSERT(self->ucs_user_ptr, "[SMM]");  /* requested memory for unknown type */
            descr_ptr = &self->null_descr;
            break;
    }

    return descr_ptr;
}

/*! \brief  Allocates memory of a certain type
 *  \param  self             The instance
 *  \param  mem_size         Size of the memory in bytes
 *  \param  type             The memory usage type
 *  \param  custom_info_pptr Reference to custom information
 *  \return Returns a reference to the allocated memory or \c NULL if the allocation is not possible
 */
static void* Smm_Allocate(void *self, uint16_t mem_size, Ams_MemUsage_t type, void** custom_info_pptr)
{
    CStaticMemoryManager *self_ = (CStaticMemoryManager*)self;
    void *mem_ptr = NULL;
    CDlNode *node_ptr = NULL;

    Smm_Descriptor_t* descr_ptr = Smm_GetTypeDescriptor(self_, type);

    if (mem_size <= descr_ptr->max_mem_size)
    {
        node_ptr = Dl_PopHead(&descr_ptr->list);    /* size is ok, retrieve a node from the list */
    }

    if (node_ptr != NULL)
    {
        mem_ptr = Dln_GetData(node_ptr);            /* retrieve reference of whole message object */
        *custom_info_pptr = node_ptr;
    }

    return mem_ptr;
}

/*! \brief  Frees memory of a certain type
 *  \param  self             The instance
 *  \param  mem_ptr          Reference to the memory chunk
 *  \param  type             The memory usage type
 *  \param  custom_info_ptr  Reference to custom information
 */
static void Smm_Free(void *self, void *mem_ptr, Ams_MemUsage_t type, void* custom_info_ptr)
{
    CStaticMemoryManager *self_ = (CStaticMemoryManager*)self;
    Smm_Descriptor_t* descr_ptr = Smm_GetTypeDescriptor(self_, type);

    Dl_InsertHead(&descr_ptr->list, (CDlNode*)custom_info_ptr);
    MISC_UNUSED(mem_ptr);
}

/*! \brief  Retrieves the current number of unused message objects.
 *  \param  self        The instance
 *  \param  rx_cnt_ptr  Application provided reference to write the current number of unused Rx message objects.
 *  \param  tx_cnt_ptr  Application provided reference to write the current number of unused Tx message objects.
 *  \return Returns \c UCS_RET_ERR_PARAM if \c NULL is provided otherwise \c UCS_RET_SUCCESS. 
 */
Ucs_Return_t Smm_GetFreeBufferCnt(CStaticMemoryManager *self, uint16_t *rx_cnt_ptr, uint16_t *tx_cnt_ptr)
{
    Ucs_Return_t ret = UCS_RET_SUCCESS;

    if ((tx_cnt_ptr != NULL) && (rx_cnt_ptr != NULL))
    {
        *rx_cnt_ptr = Dl_GetSize(&self->rx_object_descr.list);
        *tx_cnt_ptr = Dl_GetSize(&self->tx_object_descr.list);
    }
    else
    {
        ret = UCS_RET_ERR_PARAM;
    }

    return ret;
}


/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

