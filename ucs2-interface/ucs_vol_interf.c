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
#include <assert.h>
#include "ucs_interface.h"
#include "libmostvolume.h"



static UCSI_channelsT ucsiChannels[] = {
    {.name= "FRONT_LEFT" , .numid= (int)LIB_MOST_VOLUME_CH_FRONT_LEFT},
    {.name= "FRONT_RIGHT", .numid= (int)LIB_MOST_VOLUME_CH_FRONT_RIGHT},
    {.name= "REAR_LEFT"  , .numid= (int)LIB_MOST_VOLUME_CH_REAR_LEFT},
    {.name= "REAR_LEFT"  , .numid= (int)LIB_MOST_VOLUME_CH_REAR_RIGHT},
    {.name= "CENTER"     , .numid= (int)LIB_MOST_VOLUME_CH_CENTER},
    {.name= "SUB"        , .numid= (int)LIB_MOST_VOLUME_CH_SUB},
    {.name= "MASTER"     , .numid= (int)LIB_MOST_VOLUME_MASTER},

    {.name= NULL}
};

// Small wrapper as UCSI and UCSVOL do not use the same handle
void UCSI_Vol_Service (UCSI_Data_t *pPriv) {
    (void)lib_most_volume_service();

}

UCSI_channelsT *UCSI_Vol_Init (UCSI_Data_t *pPriv, UCSI_VolumeServiceCB_t serviceCB) {
    int err;
    err = lib_most_volume_init(pPriv->UNICENS, (lib_most_volume_service_cb_t) serviceCB);
    if (err) return (NULL);
    else return (ucsiChannels);
}

uint8_t UCSI_Vol_Set(UCSI_Data_t *pPriv, int numid, uint8_t volume) {

    return (lib_most_volume_set((enum lib_most_volume_channel_t)numid, volume));
}


