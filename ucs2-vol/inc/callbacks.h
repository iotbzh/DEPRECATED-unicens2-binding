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

#ifndef CALLBACKS_H
#define	CALLBACKS_H

#include "ucs_api.h"

typedef void (*clb_i2c_result_cb_t)(Ucs_I2c_Result_t result, void *obj_ptr);

extern "C" void Clb_RegisterI2CResultCB(clb_i2c_result_cb_t result_fptr, void *obj_ptr);
extern "C" void Clb_OnWriteI2CPortResult(uint16_t node_address, uint16_t i2c_port_handle, uint8_t i2c_slave_address, uint8_t data_len, Ucs_I2c_Result_t result, void *user_ptr);

#endif	/* CALLBACKS_H */

