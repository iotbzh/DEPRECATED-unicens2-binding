/*
 * libmostvolume example
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

#include "device_container.h"
#include "callbacks.h"
#include "ucs_api.h"

#define DEVCONT_TIME_RETRIGGER      (uint16_t)100U
#define DEVCONT_TIME_NOW            (uint16_t)0U
#define DEVCONT_TIME_STOP           (uint16_t)0xFFFFU

#define DEVCONT_UNUSED(a)     (a = a)

CDeviceContainer::CDeviceContainer()
{
    this->_idx_processing = 0U;
    this->_ucs_inst_ptr = NULL;
    this->_values_pptr = NULL;
    this->_values_sz = 0U;
    this->_tx_busy = false;
    this->_service_requested = false;
    this->_service_fptr = NULL;
}

CDeviceContainer::~CDeviceContainer()
{
    Clb_RegisterI2CResultCB(NULL, NULL);    /* avoid that the result callback is fired after object is destroyed */
}

void CDeviceContainer::RegisterValues(CDeviceValue** list_pptr, uint16_t list_sz)
{
    this->_idx_processing = 0U;
    this->_values_pptr = list_pptr;
    this->_values_sz = list_sz;
    this->_tx_busy = false;

    if ((list_pptr != NULL) && (list_sz > 0U))
    {
        this->_idx_processing = list_sz - 1U;
    }
}

void CDeviceContainer::ClearValues()
{
    this->_idx_processing = 0U;
    this->_values_pptr = NULL;
    this->_values_sz = 0U;
    this->_tx_busy = false;
}

void CDeviceContainer::SetValue(uint16_t key, uint8_t value)
{
     uint16_t idx;
     bool req_update = false;

     for (idx = 0U; idx < this->_values_sz; idx++)
     {
         if (this->_values_pptr[idx]->GetKey() == key)
         {
            this->_values_pptr[idx]->SetValue(value);
            if (this->_values_pptr[idx]->RequiresUpdate())
            {
                req_update = true;
            }
         }
     }

    if (req_update && (!this->_tx_busy))
    {
        RequestService(DEVCONT_TIME_NOW); //fire callback
    }
}

void CDeviceContainer::IncrementProcIndex(void)
{
    if ((_idx_processing + 1U) >=  this->_values_sz)
    {
        _idx_processing = 0U;
    }
    else
    {
        _idx_processing++;
    }
}

// starts at latest position, searches next value to update, waits until response
void CDeviceContainer::Update()
{
    uint16_t cnt;
    bool error = false;
    _service_requested = false;

    if (this->_ucs_inst_ptr == NULL)
    {
        return;
    }

    if (this->_tx_busy)
    {
        return;
    }
    
    for (cnt = 0u; cnt < this->_values_sz; cnt++)   /* just run one cycle */
    {
        IncrementProcIndex();

        if (_values_pptr[_idx_processing]->RequiresUpdate())
        {
            if (_values_pptr[_idx_processing]->FireUpdateMessage())
            {
                Clb_RegisterI2CResultCB(&OnI2cResult, this);
                this->_tx_busy = true;
                break;
            }
            else
            {
                error = true;
            }
        }
    }

    if (error)
    {
        RequestService(DEVCONT_TIME_RETRIGGER);
    }
}

void CDeviceContainer::HandleI2cResult(Ucs_I2c_Result_t result)
{
    DEVCONT_UNUSED(result);
    this->_tx_busy = false;
    this->RequestService(DEVCONT_TIME_NOW);
}

void CDeviceContainer::OnI2cResult(Ucs_I2c_Result_t result, void *obj_ptr)
{
    ((CDeviceContainer*)obj_ptr)->HandleI2cResult(result);
}

void CDeviceContainer::RequestService(uint16_t timeout)
{
    if (!_service_requested)
    {
        _service_requested = true;

        if (_service_fptr != NULL)
        {
            _service_fptr(timeout);
        }
    }
}
