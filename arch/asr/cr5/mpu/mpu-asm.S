      @PRESERVE8
      .text@   RO_CODE, CODE, READONLY         ; Name this block of code
   
      @ENTRY

	.global CR5_MpuRegionSizeEnableReg_Read 
	.func CR5_MpuRegionSizeEnableReg_Read  
CR5_MpuRegionSizeEnableReg_Read:
   MRC p15, 0, r0, c6, c1, 2
   BX  LR
   .endfunc
   
	.global CR5_MpuRegionSizeEnableReg_Write 
	.func CR5_MpuRegionSizeEnableReg_Write  
CR5_MpuRegionSizeEnableReg_Write:
   MCR p15, 0, r0, c6, c1, 2
   BX  LR
   .endfunc
   
	.global CR5_MpuRegionAccessCtrlReg_Read 
	.func CR5_MpuRegionAccessCtrlReg_Read  
CR5_MpuRegionAccessCtrlReg_Read:
   MRC p15, 0, r0, c6, c1, 4
   BX  LR
   .endfunc


	.global CR5_MpuRegionAccessCtrlReg_Write 
	.func CR5_MpuRegionAccessCtrlReg_Write  
CR5_MpuRegionAccessCtrlReg_Write:
   MCR p15, 0, r0, c6, c1, 4
   BX  LR
   .endfunc

	.global CR5_MpuRegionNumberReg_Read 
	.func CR5_MpuRegionNumberReg_Read  
CR5_MpuRegionNumberReg_Read:
   MRC p15, 0, r0, c6, c2, 0
   BX  LR
   .endfunc


	.global CR5_MpuRegionNumberReg_Write 
	.func CR5_MpuRegionNumberReg_Write  
CR5_MpuRegionNumberReg_Write:
   MCR p15, 0, r0, c6, c2, 0
   BX  LR
   .endfunc

	.global CR5_MpuRegionBaseAddressReg_Read 
	.func CR5_MpuRegionBaseAddressReg_Read  
CR5_MpuRegionBaseAddressReg_Read:
   MRC p15, 0, r0, c6, c1, 0
   BX  LR
   .endfunc

	.global CR5_MpuRegionBaseAddressReg_Write 
	.func CR5_MpuRegionBaseAddressReg_Write  
CR5_MpuRegionBaseAddressReg_Write:
   MCR p15, 0, r0, c6, c1, 0
   BX  LR
   .endfunc

	.global CR5_MpuTypeReg_Read 
	.func CR5_MpuTypeReg_Read  
CR5_MpuTypeReg_Read:
   MRC p15, 0, r0, c0, c0, 4   @ Read MPUIR
   BX  LR
   .endfunc


	.global CR5_MpuEnable 
	.func CR5_MpuEnable  
CR5_MpuEnable:
   MRC     p15, 0, r0, c1, c0, 0       @ Read CP15 Control Register
   ORR     r0, r0, #0x01               @ Set M bit to enable MPU
   DSB                                 @ Ensure all previous loads/stores have completed
   MCR     p15, 0, r0, c1, c0, 0       @ Write CP15 Control Register
   ISB                                 @ Ensure subsequent insts execute wrt new MPU settings
   BX  LR
   .endfunc


	.global CR5_MpuDisable 
	.func CR5_MpuDisable  
CR5_MpuDisable:
   MRC     p15, 0, r0, c1, c0, 0       @ Read CP15 Control Register
   BIC     r0, r0, #0x01               @ Clear M bit to disable MPU
   DSB                                 @ Ensure all previous loads/stores have completed
   MCR     p15, 0, r0, c1, c0, 0       @ Write CP15 Control Register
   ISB                                 @ Ensure subsequent insts execute wrt new MPU settings
   BX  LR
   .endfunc
   
   
   

















