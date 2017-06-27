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

#include "device_value.h"
#include "callbacks.h"
#include "ucs_api.h"
#include "setup.h"
/*#include <iostream>*/

#define MUTE_VALUE      0x03FFU
#define MUTE_VALUE_HB   0x03U
#define MUTE_VALUE_LB   0xFFU

#define CONTROL_MASTER  0x07U
#define CONTROL_CH_1    0x08U
#define CONTROL_CH_2    0x09U

CDeviceValue::CDeviceValue(uint16_t address, DeviceValueType type, uint16_t key)
{
    this->_is_initial = true;
    this->_address = address;
    this->_target_value = 0x01u;
    this->_actual_value = 0x01u;

    this->_type = type;
    this->_key = key;

    _tx_payload[0] = CONTROL_MASTER;// 7: master, 8: channel 1, 9: Channel 2
    _tx_payload[1] = MUTE_VALUE_HB; //HB:Volume
    _tx_payload[2] = MUTE_VALUE_LB; //LB:Volume
    _tx_payload_sz = 3u;
}

CDeviceValue::~CDeviceValue()
{
}

void CDeviceValue::ApplyMostValue(uint8_t value, DeviceValueType type, uint8_t tx_payload[])
{
    uint16_t tmp = MUTE_VALUE;

    switch (type)
    {
        case DEVICE_VAL_LEFT:
            tmp = 0x80U + 0x37FU - (0x37FU * ((int32_t)value) / (0xFFU));
            //tmp = 0x3FF - (0x3FF * ((int32_t)value) / (0xFF));
            //tmp = 0x100 + 0x2FF - (0x2FF * ((int32_t)value) / (0xFF));
            tx_payload[0] = CONTROL_CH_1;
            break;
        case DEVICE_VAL_RIGHT:
            tmp = 0x80U + 0x37FU - (0x37FU * ((int32_t)value) / (0xFFU));
            //tmp = 0x3FF - (0x3FF * ((int32_t)value) / (0xFF));
            //tmp = 0x100 + 0x2FF - (0x2FF * ((int32_t)value) / (0xFF));
            tx_payload[0] = CONTROL_CH_2;
            break;
        default:
            /*std::cerr << "CDeviceValue::ApplyMostValue() error matching incorrect" << std::endl;*/
        case DEVICE_VAL_MASTER:
            tmp = 0x100U + 0x2FFU - (0x2FFU * ((int32_t)value) / (0xFFU));
            tx_payload[0] = CONTROL_MASTER;
            break;
    }

    tx_payload[1] = (uint8_t)((tmp >> 8U) & (uint16_t)0xFFU); //HB:Volume
    tx_payload[2] = (uint8_t)(tmp  & (uint16_t)0xFFU); //LB:Volume
}

// returns true if target is not actual value
bool CDeviceValue::RequiresUpdate()
{
    if (this->_target_value != this->_actual_value)
    {
        return true;
    }

    return false;
}

bool CDeviceValue::FireUpdateMessage(void)
{
    Ucs_Return_t ret;
    ApplyMostValue(this->_target_value, _type, _tx_payload);

    ret = Ucs_I2c_WritePort( CSetup::GetInstance()->RetrieveUnicensInst(),
            this->_address,
            0x0F00u,                /* i2c port handle */
            UCS_I2C_DEFAULT_MODE,   /* 0 */
            0u,                     /* block count */
            0x2Au,                  /* i2c slave address */
            0x03E8u,                /* timeout 1000 milliseconds */
            _tx_payload_sz,         /* data length */
            &_tx_payload[0],        /* data pointer */
            &Clb_OnWriteI2CPortResult
            );

    if (ret == UCS_RET_SUCCESS)
    {
        // Clb_RegisterI2CResultCB(OnI2cResult, this);
        // mark value as set!
        this->_actual_value = this->_target_value;
        return true;
    }

    return false;
}
