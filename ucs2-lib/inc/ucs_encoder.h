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
 * \brief Declaration of message encoder
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup  G_ENCODER
 * @{
 */

#ifndef UCS_ENCODER_H
#define UCS_ENCODER_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_message.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Defines                                                                                        */
/*------------------------------------------------------------------------------------------------*/
#define ENC_MAX_SIZE_CONTENT    16U     /*!< \brief Maximum content size in bytes, quadlet aligned */

/*------------------------------------------------------------------------------------------------*/
/* Types                                                                                          */
/*------------------------------------------------------------------------------------------------*/
/*! \brief  Retrieves the size of a MOST message header
 *  \return The size of the MOST message header in bytes.
 */
typedef uint8_t (*Enc_GetSize_t)(void);

/*! \brief  Retrieves the content type of a MOST message header
 *  \return The content type of the MOST message header in bytes.
 */
typedef uint8_t (*Enc_GetContType_t)(void);

/*! \brief  Encodes a message telegram to the MOST message header 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
typedef void    (*Enc_Encode_t)(Msg_MostTel_t *tel_ptr, uint8_t header[]);

/*! \brief  Decodes a MOST message header to a message telegram structure 
 *  \param  tel_ptr     Reference to the Msg_MostTel_t structure 
 *  \param  header      The header buffer 
 */
typedef void    (*Enc_Decode_t)(Msg_MostTel_t *tel_ptr, uint8_t header[]);

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief      Identifier for a MOST Message Content */
typedef enum Enc_MsgContent_
{
    ENC_CONTENT_00 = 0x00,  /*!< \brief Content Type "0x00": Uncompressed, excluding retry values */
    ENC_CONTENT_80 = 0x80,  /*!< \brief Content Type "0x80": Compressed, excluding retry values */
    ENC_CONTENT_81 = 0x81   /*!< \brief Content Type "0x81": Compressed, including retry values */

} Enc_MsgContent_t;

/*! \brief      Interface for message encoder */
typedef struct IEncoder_
{
    Enc_MsgContent_t    content_type;   /*!< \brief Retrieves the content type of the MOST message header */
    uint8_t             pm_hdr_sz;      /*!< \brief Retrieves the size of the Port Message header */
    uint8_t             msg_hdr_sz;     /*!< \brief Retrieves the size of the MOST message header */
    Enc_Encode_t        encode_fptr;    /*!< \brief Function required to encode a MOST message header */
    Enc_Decode_t        decode_fptr;    /*!< \brief Function required to decode a MOST message header */

} IEncoder;

/*------------------------------------------------------------------------------------------------*/
/* Function prototypes                                                                            */
/*------------------------------------------------------------------------------------------------*/
extern IEncoder *Enc_GetEncoder(Enc_MsgContent_t type);

#ifdef __cplusplus
}                                                   /* extern "C" */
#endif

#endif /* #ifndef UCS_ENCODER_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

