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

#ifndef DEVICECONTAINER_H
#define	DEVICECONTAINER_H

#include "device_value.h"
#include "libmostvolume.h"

class CDeviceContainer {

public:
    CDeviceContainer();
    virtual ~CDeviceContainer();

    void AssignService(lib_most_volume_service_cb_t service_fptr, Ucs_Inst_t *ucs_ptr) {_service_fptr = service_fptr; _ucs_inst_ptr = ucs_ptr;}

    void RegisterValues(CDeviceValue** list_pptr, uint16_t list_sz);
    void SetValue(uint16_t key, uint8_t value);
    void ClearValues();
    void Update();

private:
    void RequestService(uint16_t timeout);
    void IncrementProcIndex(void);
    void HandleI2cResult(Ucs_I2c_Result_t result);

    static void OnI2cResult(Ucs_I2c_Result_t result, void *obj_ptr);

    Ucs_Inst_t *_ucs_inst_ptr;
    uint16_t _idx_processing;
    uint16_t _values_sz;
    CDeviceValue **_values_pptr;
    bool _tx_busy;
    bool _service_requested;
    lib_most_volume_service_cb_t _service_fptr;
};

#endif	/* DEVICECONTAINER_H */

