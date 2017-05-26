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
 * \brief Internal header file of the Finite State Machine.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_FSM
 * @{
 */

#ifndef UCS_FSM_H
#define UCS_FSM_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Definitions                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Internal state machine states which are also used as return values for method 
 *         Fsm_Service().
 */
typedef enum Fsm_State_
{
    FSM_STATE_IDLE,     /*!< \brief The state machine is in idle mode */
    FSM_STATE_SERVICE,  /*!< \brief An event is pending and the state machine must be serviced */
    FSM_STATE_WAIT,     /*!< \brief Waiting for asynchronous data/signal/event */
    FSM_STATE_END,      /*!< \brief The state machine is finished */
    FSM_STATE_ERROR     /*!< \brief An error occurred while processing the state machine */

} Fsm_State_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Function signature used for state machine actions */
typedef void (*Fsm_Act_t)(void *self);

/*! \brief Structure is used to define state elements */
typedef struct Fsm_StateElem_
{
    /*! \brief Function pointer to the action that shall be executed */
    Fsm_Act_t action_fptr;
    /*! \brief Next state */
    int8_t next_state;

} Fsm_StateElem_t;

/*! \brief Class structure of the finite state machine */
typedef struct CFsm_
{
    /*! \brief Reference to transition table */
    const Fsm_StateElem_t *transition_table_ptr;
    /*! \brief Instance pointer used for actions */
    void *inst_ptr;
    /*! \brief Current event */
    int8_t event_occured;
    /*! \brief Current state */
    int8_t current_state;
    /*! \brief Maximum number of events */
    uint8_t num_events;
    /*! \brief Internal state of the state machine */
    Fsm_State_t internal_state;

} CFsm;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes                                                                                     */
/*------------------------------------------------------------------------------------------------*/
extern void Fsm_Ctor(CFsm *self, void *inst_ptr, const Fsm_StateElem_t *trans_table_ptr,
                     uint8_t num_events, int8_t init_state);
extern Fsm_State_t Fsm_Service(CFsm *self);
extern void Fsm_SetEvent(CFsm *self, int8_t e);
extern void Fsm_Wait(CFsm *self);
extern void Fsm_End(CFsm *self);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_FSM_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

