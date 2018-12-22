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
***   This file creates the mass storage class UFI command headers
***
**************************************************************************
**END*********************************************************/

#include "usb_mass_bo.h"
#include "usb_mass_ufi.h"


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_mass_ufi_generic
* Returned Value : None
* Comments       :
*     This function initializes the mass storage class
*
*END*--------------------------------------------------------------------*/

USB_STATUS usb_mass_ufi_generic
(
    /* [IN] command object allocated by application*/
    COMMAND_OBJECT_PTR         cmd_ptr,
    uint_8                     opcode,
    uint_8                     lun,
    uint_32                    lbaddr,
    uint_32                    blen,

    uint_8                     cbwflags,

    uchar_ptr                  buf,
    uint_32                    buf_len
)
{
    /* Body */
    // clean warning USB_STATUS                    status = USB_OK;
    UFI_CBWCB_EXTENDED_STRUCT_PTR ufi_ptr = NULL;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_mass_ufi_generic\r\n");
#endif

    ufi_ptr = (pointer) & (cmd_ptr->CBW_PTR->CMD_BLOCK);

    /* Construct UFI command buffer */
    ufi_ptr->BUFIOPCODE = opcode;
    ufi_ptr->BUFILUN = lun;
    HOST_WRITE_BEOCT_32(ufi_ptr->BUFILOGICALBLOCKADDRESS, lbaddr);
    HOST_WRITE_BEOCT_32(ufi_ptr->BLENGTH, blen);

    /* Construct CBW fields (sig and tag will be filled up by class driver)*/
    htou32(cmd_ptr->CBW_PTR->DCBWDATATRANSFERLENGTH, buf_len);
    cmd_ptr->CBW_PTR->BMCBWFLAGS = cbwflags;
    TRANSFER_LOW_NIBBLE(cmd_ptr->LUN, cmd_ptr->CBW_PTR->BCBWLUN);
    TRANSFER_LOW_NIBBLE(sizeof(UFI_CBWCB_STRUCT),
                        cmd_ptr->CBW_PTR->BCBWCBLENGTH);

    /* Construct Command object */
    cmd_ptr->DATA_BUFFER = buf;
    cmd_ptr->BUFFER_LEN = buf_len;

#ifdef _HOST_DEBUG_
    bbu_printf("usb_mass_ufi_generic, SUCCESSFUL\r\n");
#endif

    /* Pass this request to class driver */
    return usb_class_mass_storage_device_command(cmd_ptr->CALL_PTR, cmd_ptr);
} /* Endbody */

/* EOF */
