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

#ifndef DEVICEVALUE_H
#define	DEVICEVALUE_H

#include "ucs_api.h"

struct SRxMessage {
    uint32_t devInst;   /*instance if multiple devices are using the same group id*/
    uint32_t sourceAddr;
    uint32_t targetAddr;
    uint32_t nFBlock;
    uint32_t nInst;
    uint32_t nFunc;
    uint32_t nOpType;
    const uint8_t *pPayload;
    uint32_t payloadLen;
};

struct STxMessage {
    uint32_t devInst;   /*instance if multiple devices are using the same group id*/
    uint32_t targetAddr;
    uint32_t nFBlock;
    uint32_t nInst;
    uint32_t nFunc;
    uint32_t nOpType;
    const uint8_t *pPayload;
    uint32_t payloadLen;
};

enum DeviceValueType {
    DEVICE_VAL_MASTER = 0,
    DEVICE_VAL_LEFT = 1,
    DEVICE_VAL_RIGHT = 2

};

class CDeviceValue {
public:
    CDeviceValue(uint16_t address, DeviceValueType type, uint16_t key);
    virtual ~CDeviceValue();

    uint16_t GetKey(){return _key;}
    DeviceValueType GetType(){return _type;}        // returns the assigned type
    void SetValue(uint8_t value){_target_value = value;}   // sets desired value

    bool RequiresUpdate();      // returns true if target is not actual value
                                // returns true if success, false if failed
                                // -> stop transmission
    bool FireUpdateMessage(void);// fires message & updates actual value

private:
    void HandleI2cResult(Ucs_I2c_Result_t result);
    void ApplyMostValue(uint8_t value, DeviceValueType type, uint8_t tx_payload[]);

    bool     _is_initial;       // ensure first update
    DeviceValueType _type;      // determines the remote i2c command
    uint16_t _key;              // lookup key
    uint16_t _address;          // target node/group address
    uint8_t  _target_value;     // desired value
    uint8_t  _actual_value;     // value set and confirmed via network
    uint8_t  _tx_payload[20];
    uint8_t  _tx_payload_sz;
};

#endif	/* DEVICEPROPERTY_H */

