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
 * \brief Implementation of message encoder
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_ENCODER
 * @{
 */

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_encoder.h"
#include "ucs_misc.h"

/*------------------------------------------------------------------------------------------------*/
/* Constants                                                                                      */
/*------------------------------------------------------------------------------------------------*/
#define ENC_LLR_TIME_DEFAULT  11U /*! \brief Default LLR time required to transmit valid messages
                                   *         with ContentType 0x81
                                   */

/*------------------------------------------------------------------------------------------------*/
/* Internal prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
static void    Enc_Encode_00(Msg_MostTel_t *tel_ptr, uint8_t header[]);
static void    Enc_Decode_00(Msg_MostTel_t *tel_ptr, uint8_t header[]);

static void    Enc_Encode_80(Msg_MostTel_t *tel_ptr, uint8_t header[]);
static void    Enc_Decode_80(Msg_MostTel_t *tel_ptr, uint8_t header[]);

static void    Enc_Encode_81(Msg_MostTel_t *tel_ptr, uint8_t header[]);
static void    Enc_Decode_81(Msg_MostTel_t *tel_ptr, uint8_t header[]);

/*------------------------------------------------------------------------------------------------*/
/* Implementation                                                                                 */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Retrieves the interface of a specific encoder
 *  \details    Creates all encoder interfaces as singletons
 *  \param      type    Specifies the type of encoder to retrieve
 *  \return     The desired interface to the specified encoder 
 */
IEncoder *Enc_GetEncoder(Enc_MsgContent_t type)
{
    static IEncoder enc_content_00 = {ENC_CONTENT_00, 8U, 12U, &Enc_Encode_00, &Enc_Decode_00};
    static IEncoder enc_content_80 = {ENC_CONTENT_80, 6U, 11U, &Enc_Encode_80, &Enc_Decode_80};
    static IEncoder enc_content_81 = {ENC_CONTENT_81, 6U, 13U, &Enc_Encode_81, &Enc_Decode_81};
    IEncoder *encoder_ptr = NULL;

    switch (type)
    {
        case ENC_CONTENT_00:
            encoder_ptr = &enc_content_00;
            break;
        case ENC_CONTENT_80:
            encoder_ptr = &enc_content_80;
            break;
        case ENC_CONTENT_81:
            encoder_ptr = &enc_content_81;
            break;
        default:
            encoder_ptr = NULL;
            break;
    }

    return encoder_ptr;
}

/*------------------------------------------------------------------------------------------------*/
/* Content type "00"                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Encodes a message telegram to the "ContentType 0x00" MOST message header 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Encode_00(Msg_MostTel_t *tel_ptr, uint8_t header[])
{
    header[0]  = MISC_HB(tel_ptr->source_addr);
    header[1]  = MISC_LB(tel_ptr->source_addr);
    header[2]  = MISC_HB(tel_ptr->destination_addr);
    header[3]  = MISC_LB(tel_ptr->destination_addr);

    header[4]  = tel_ptr->id.fblock_id;
    header[5]  = tel_ptr->id.instance_id;

    header[6]  = MISC_HB(tel_ptr->id.function_id);
    header[7]  = MISC_LB(tel_ptr->id.function_id);

    header[8]  = (uint8_t)(tel_ptr->tel.tel_id << 4) | (uint8_t)((uint8_t)tel_ptr->id.op_type & 0xFU);
    header[9]  = tel_ptr->opts.llrbc;

    header[10] = tel_ptr->tel.tel_cnt;
    header[11] = tel_ptr->tel.tel_len;
}

/*! \brief  Decodes a "ContentType 0x00" MOST message header to a message telegram structure 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Decode_00(Msg_MostTel_t *tel_ptr, uint8_t header[])
{
    tel_ptr->source_addr        = (uint16_t)((uint16_t)header[0] << 8) | (uint16_t)header[1];
    tel_ptr->destination_addr   = (uint16_t)((uint16_t)header[2] << 8) | (uint16_t)header[3];

    tel_ptr->id.fblock_id       = header[4];
    tel_ptr->id.instance_id     = header[5];

    tel_ptr->id.function_id     = (uint16_t)((uint16_t)header[6] << 8) | (uint16_t)header[7];

    tel_ptr->tel.tel_id         = header[8] >> 4;                       /* high nibble: TelId */
    tel_ptr->id.op_type         = (Ucs_OpType_t)(header[8] & 0x0FU);    /* low nibble: OPType */

    tel_ptr->opts.llrbc         = header[9];
    tel_ptr->tel.tel_cnt        = header[10];
    tel_ptr->tel.tel_len        = header[11];

    tel_ptr->tel.tel_data_ptr   = &header[12];
}

/*------------------------------------------------------------------------------------------------*/
/* Content type "0x80"                                                                              */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Encodes a message telegram to the "ContentType 0x80" MOST message header 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Encode_80(Msg_MostTel_t *tel_ptr, uint8_t header[])
{             /* high nibble: TelId                    low nibble: OPType */
    header[0]  = (uint8_t)(tel_ptr->tel.tel_id << 4) | (uint8_t)((uint8_t)tel_ptr->id.op_type & 0xFU);
    header[1]  = tel_ptr->tel.tel_cnt;
    header[2]  = tel_ptr->tel.tel_len;

    header[3]  = MISC_HB(tel_ptr->id.function_id);
    header[4]  = MISC_LB(tel_ptr->id.function_id);

    header[5]  = MISC_HB(tel_ptr->source_addr);
    header[6]  = MISC_LB(tel_ptr->source_addr);

    header[7]  = MISC_HB(tel_ptr->destination_addr);
    header[8]  = MISC_LB(tel_ptr->destination_addr);

    header[9]  = tel_ptr->id.fblock_id;
    header[10] = tel_ptr->id.instance_id;
}

/*! \brief  Decodes a "ContentType 0x80" MOST message header to a message telegram structure 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Decode_80(Msg_MostTel_t *tel_ptr, uint8_t header[])
{
    tel_ptr->tel.tel_id         = header[0] >> 4;                       /* high nibble: TelId */
    tel_ptr->id.op_type         = (Ucs_OpType_t)(header[0] & 0x0FU);    /* low nibble: OPType */

    tel_ptr->tel.tel_cnt        = header[1];
    tel_ptr->tel.tel_len        = header[2];

    tel_ptr->id.function_id     = (uint16_t)((uint16_t)header[3] << 8) | (uint16_t)header[4];

    tel_ptr->source_addr        = (uint16_t)((uint16_t)header[5] << 8) | (uint16_t)header[6];
    tel_ptr->destination_addr   = (uint16_t)((uint16_t)header[7] << 8) | (uint16_t)header[8];

    tel_ptr->id.fblock_id       = header[9];
    tel_ptr->id.instance_id     = header[10];

    tel_ptr->tel.tel_data_ptr   = &header[11];
}

/*------------------------------------------------------------------------------------------------*/
/* Content type "0x81"                                                                              */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Encodes a message telegram to the "ContentType 0x81" MOST message header 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Encode_81(Msg_MostTel_t *tel_ptr, uint8_t header[])
{
    header[0]  = tel_ptr->opts.llrbc;
    header[1]  = ENC_LLR_TIME_DEFAULT; 
    /*           high nibble: TelId                    low nibble: OPType */
    header[2]  = (uint8_t)(tel_ptr->tel.tel_id << 4) | (uint8_t)((uint8_t)tel_ptr->id.op_type & 0xFU);
    header[3]  = tel_ptr->tel.tel_cnt;
    header[4]  = tel_ptr->tel.tel_len;

    header[5]  = MISC_HB(tel_ptr->id.function_id);
    header[6]  = MISC_LB(tel_ptr->id.function_id);

    header[7]  = MISC_HB(tel_ptr->source_addr);
    header[8]  = MISC_LB(tel_ptr->source_addr);

    header[9]  = MISC_HB(tel_ptr->destination_addr);
    header[10] = MISC_LB(tel_ptr->destination_addr);

    header[11] = tel_ptr->id.fblock_id;
    header[12] = tel_ptr->id.instance_id;
}

/*! \brief  Decodes a "ContentType 0x81" MOST message header to a message telegram structure 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
static void Enc_Decode_81(Msg_MostTel_t *tel_ptr, uint8_t header[])
{
    tel_ptr->opts.llrbc         = header[0];

    tel_ptr->tel.tel_id         = header[2] >> 4;                       /* high nibble: TelId */
    tel_ptr->id.op_type         = (Ucs_OpType_t)(header[2] & 0x0FU);    /* low nibble: OPType */

    tel_ptr->tel.tel_cnt        = header[3];
    tel_ptr->tel.tel_len        = header[4];

    tel_ptr->id.function_id     = (uint16_t)((uint16_t)header[5] << 8) | (uint16_t)header[6];

    tel_ptr->source_addr        = (uint16_t)((uint16_t)header[7] << 8) | (uint16_t)header[8];
    tel_ptr->destination_addr   = (uint16_t)((uint16_t)header[9] << 8) | (uint16_t)header[10];

    tel_ptr->id.fblock_id       = header[11];
    tel_ptr->id.instance_id     = header[12];

    tel_ptr->tel.tel_data_ptr   = &header[13];
}

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

