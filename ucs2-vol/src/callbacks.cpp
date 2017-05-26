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

#include "callbacks.h"
#include "device_value.h"

typedef void (*i2c_result_t)(Ucs_I2c_Result_t result, void *obj_ptr);

static clb_i2c_result_cb_t i2c_result_fptr;
static void *i2_obj_ptr;

#define CLB_UNUSED(a)     (a = a)

extern "C" void Clb_RegisterI2CResultCB(clb_i2c_result_cb_t result_fptr, void *obj_ptr)
{
    i2c_result_fptr = result_fptr;
    i2_obj_ptr = obj_ptr;
}

extern "C" void Clb_OnWriteI2CPortResult(uint16_t node_address, uint16_t i2c_port_handle, uint8_t i2c_slave_address, uint8_t data_len, Ucs_I2c_Result_t result, void *user_ptr)
{
    CLB_UNUSED(user_ptr);
    CLB_UNUSED(data_len);
    CLB_UNUSED(i2c_slave_address);
    CLB_UNUSED(i2c_port_handle);
    CLB_UNUSED(node_address);

    if (i2c_result_fptr != NULL)
    {
        clb_i2c_result_cb_t tmp_i2c_result_fptr = i2c_result_fptr;
        void *tmp_i2_obj_ptr = i2_obj_ptr;

        i2c_result_fptr = NULL;     /* reset references before callback to allow synchronous registration of a new callback */
        i2_obj_ptr = NULL;

        tmp_i2c_result_fptr(result, tmp_i2_obj_ptr);
    }
}
