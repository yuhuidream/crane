/**HEADER*********************************************************
-----------------------------------------------------------------------------
 CONFIDENTIAL AND PROPRIETARY INFORMATION OF TRANSDIMENSION, INC. AND/OR ITS
 WHOLLY OWNED SUBSIDIARY SOFTCONNEX TECHNOLOGIES, INC. THIS NOTICE IS NOT TO
 BE DELETED, MODIFIED, MOVED OR CHANGED IN ANY WAY.

 (TransDimension, Inc. and SoftConnex Technologies, Inc. are referred to
 collectively below as TransDimension, Inc.)
 Copyright (c) 1999 - 2004 by TransDimension, Inc.

 This software is protected by copyright laws and international copyright
 treaties, as well as other intellectual property laws and treaties.  This
 software is a CONFIDENTIAL, unpublished work of authorship, and with portions
 constituting TRADE SECRETS of TransDimension, Inc..  Any unauthorized use,
 disclosure, and/or reproduction of this software, or any part of this software;
 or distribution of this software in any form or by any means; or storage of
 this software in any database or retrieval system, without the express written
 consent of, and license from, TransDimension, Inc. is strictly prohibited.
 This software is protected under the copyright and/or trade secret laws in
 other countries in addition to USA.  All Rights Reserved.  Failure to abide by
 the use, disclosure and/or reproduction restrictions may result in civil and
 or criminal penalties, and will be prosecuted to the maximum extent of the law.
 -------------------------------------------------------------------------------

*** Description:
***   Implements the queue system for the mass storage class.
***
**************************************************************************
**END*********************************************************/

#include "usb_mass_bo.h"


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_q_init
* Returned Value : None
* Comments       :
*     This function initializes a mass storage class queue.
*
*END*--------------------------------------------------------------------*/

void usb_class_mass_q_init
(
    /* [IN] interface structure pointer */
    USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr
)
{
    /* Body */
#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_q_init\r\n");
#endif

    intf_ptr->QUEUE.FIRST      = 0;
    intf_ptr->QUEUE.LAST       = 0;
    intf_ptr->QUEUE.AVAILABLE  = TRUE;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_q_init, SUCCESSFUL\r\n");
#endif

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_q_insert
* Returned Value : Position at which insertion took place in the queue.
* Comments       :
*     This function is called by class driver for inserting a command in the
*     queue.
*END*--------------------------------------------------------------------*/

int_32 usb_class_mass_q_insert
(
    /* [IN] interface structure pointer */
    USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr,

    /* [IN] command object to be inserted in the queue*/
    COMMAND_OBJECT_PTR pCmd
)
{
    /* Body */
    MASS_QUEUE_STRUCT_PTR   Q =  &intf_ptr->QUEUE;
    int_32                  tmp = -1;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_q_insert\r\n");
#endif

    /*
    ** Insert into queue, update LAST, check if full and return queue position.
    ** If queue is full -1 will be returned
    */
    USB_lock();
    if (Q->AVAILABLE)
    {
        Q->ELEMENTS[Q->LAST] = pCmd;
        tmp = Q->LAST;
        Q->LAST = (Q->LAST + 1) & (QUEUE_SIZE - 1);
        if (((Q->LAST + 1) & (QUEUE_SIZE - 1)) == Q->FIRST)
        {
            Q->AVAILABLE = FALSE;
        } /* Endif */
    } /* Endif */
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_q_insert, SUCCESSFUL\r\n");
#endif

    return tmp;
} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_deleteq
* Returned Value : None
* Comments       :
*     This routine deletes the pending request in the queue
*END*--------------------------------------------------------------------*/

void usb_class_mass_deleteq
(
    /* [IN] interface structure pointer */
    USB_MASS_CLASS_INTF_STRUCT_PTR intf_ptr
)
{
    /* Body */
    MASS_QUEUE_STRUCT_PTR   Q =  &intf_ptr->QUEUE;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_deleteq\r\n");
#endif

    /* Remove current command and increment FIRST modulo the Q size */
    USB_lock();
    Q->ELEMENTS[Q->FIRST] = NULL;
    Q->FIRST = (Q->FIRST + 1) & (QUEUE_SIZE - 1);
    Q->AVAILABLE = TRUE;
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_deleteq, SUCCESSFUL\r\n");
#endif

} /* Endbody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_mass_get_pending_request
* Returned Value : None
* Comments       :
*     This routine fetches the pointer to the first (pending) request in
*     the queue, or NULL if there is no pending requests.
*END*--------------------------------------------------------------------*/

void usb_class_mass_get_pending_request
(
    /* [IN] interface structure pointer */
    USB_MASS_CLASS_INTF_STRUCT_PTR   intf_ptr,

    /* [OUT] pointer to pointer which will hold the pending request */
    COMMAND_OBJECT_PTR _PTR_         cmd_ptr_ptr
)
{
    /* Body */
    MASS_QUEUE_STRUCT_PTR   Q = &intf_ptr->QUEUE;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_get_pending_request\r\n");
#endif

    USB_lock();
    if (Q->FIRST == Q->LAST)
    {
        *cmd_ptr_ptr = NULL;
    }
    else
    {
        *cmd_ptr_ptr = (COMMAND_OBJECT_PTR)Q->ELEMENTS[Q->FIRST];
    } /* Endif */
    USB_unlock();

#ifdef _HOST_DEBUG_
    bbu_printf("usb_class_mass_get_pending_request, SUCCESSFUL\r\n");
#endif

    return;
} /* Endbody */

/* EOF */
