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
 * \brief Internal header file of the doubly linked list.
 *
 * \cond UCS_INTERNAL_DOC
 * \addtogroup G_DL
 * @{
 */

#ifndef UCS_DL_H
#define UCS_DL_H

/*------------------------------------------------------------------------------------------------*/
/* Includes                                                                                       */
/*------------------------------------------------------------------------------------------------*/
#include "ucs_types_cfg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------------------------*/
/* Type definitions                                                                               */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Callback signature used by foreach-function
 *  \param d_ptr    Reference to the data of the current node
 *  \param up_ptr   Reference to the user data
 *  \return true: Stop the for-each-loop
 *  \return false: Continue the for-each-loop
 */
typedef bool(*Dl_ForeachFunc_t)(void *d_ptr, void *ud_ptr);

/*------------------------------------------------------------------------------------------------*/
/* Enumerators                                                                                    */
/*------------------------------------------------------------------------------------------------*/
/*! \brief   Standard return values of the list module. */
typedef enum Dl_Ret_
{
    DL_OK,                      /*!< \brief No error */
    DL_UNKNOWN_NODE,            /*!< \brief Unknown node */
    DL_STOPPED                  /*!< \brief Search process stopped */

} Dl_Ret_t;

/*------------------------------------------------------------------------------------------------*/
/* Structures                                                                                     */
/*------------------------------------------------------------------------------------------------*/
/*! \brief Class structure of doubly linked list node. */
typedef struct DlNode_
{
    struct DlNode_ *prev;       /*!< \brief Reference to previous node in list */
    struct DlNode_ *next;       /*!< \brief Reference to next node in list */
    void *data_ptr;             /*!< \brief Reference to optional data */
    bool in_use;                /*!< \brief Flag which signals that the node is in use */

} CDlNode;

/*! \brief Class structure of the doubly linked list. */
typedef struct CDlList_
{
    struct DlNode_ *head;       /*!< \brief Reference to head of the list */
    struct DlNode_ *tail;       /*!< \brief Reference to tail of the list */
    uint16_t size;              /*!< \brief Number of nodes in the list */
    void *ucs_user_ptr;         /*!< \brief User reference that needs to be passed in every callback function */

} CDlList;

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CDlList                                                                    */
/*------------------------------------------------------------------------------------------------*/
extern void Dl_Ctor(CDlList *self, void *ucs_user_ptr);
extern void Dl_InsertAfter(CDlList *self, CDlNode *node, CDlNode *new_node);
extern void Dl_InsertBefore(CDlList *self, CDlNode *node, CDlNode *new_node);
extern void Dl_InsertHead(CDlList *self, CDlNode *new_node);
extern void Dl_InsertTail(CDlList *self, CDlNode *new_node);
extern Dl_Ret_t Dl_Remove(CDlList *self, CDlNode *node);
extern CDlNode * Dl_PopHead(CDlList *self);
extern CDlNode * Dl_PopTail(CDlList *self);
extern CDlNode * Dl_PeekHead(CDlList *self);
extern CDlNode * Dl_PeekTail(CDlList *self);
extern CDlNode * Dl_Foreach(CDlList *self, Dl_ForeachFunc_t func_ptr, void *user_data_ptr);
extern bool Dl_IsNodeInList(CDlList *self, const CDlNode *node);
extern void Dl_AppendList(CDlList *self, CDlList *list_ptr);
extern uint16_t Dl_GetSize(CDlList *self);

/*------------------------------------------------------------------------------------------------*/
/* Prototypes of class CDlNode                                                                    */
/*------------------------------------------------------------------------------------------------*/
extern void Dln_Ctor(CDlNode *self, void *data_ptr);
extern void Dln_SetData(CDlNode *self, void *data_ptr);
extern void * Dln_GetData(CDlNode *self);
extern bool Dln_IsNodePartOfAList(CDlNode *self);

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif  /* #ifndef UCS_DL_H */

/*!
 * @}
 * \endcond
 */

/*------------------------------------------------------------------------------------------------*/
/* End of file                                                                                    */
/*------------------------------------------------------------------------------------------------*/

