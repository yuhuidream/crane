OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry)

MEMORY {
   ATCM : ORIGIN = 0x00000000, LENGTH = 0x00010000
   DTCM : ORIGIN = 0xB0020000, LENGTH = 0x00010000
}

/* The size of the stack used by the application. NOTE: you need to adjust  */
STACK_SIZE = 0x1000;

SECTIONS{
    .text_bbu 0x0 : AT(0x100000) {
	__code_entry = .;	
        *startup_mrvl.o(.text*)
        *mpu.o(.text*)
        *mpu-asm.o(.text*)
        *set_mmu.o(.text*)
        *cache_l2c310.o(.text*)
        *cache_ops-r5.o(.text*)
        *cache_ops.o(.text*)
        *system_mrvl.o(.text*)
        *utils.o(.text*)
        *common_tests.o(.text*)
        *bbu_help.o(.text*)
        *bbu_setv.o(.text*)
        *cli.o(.text*)
        *main.o(.text*)
        *mem_tests.o(.text*)
        *core_tests.o(.text*)
        *gpio.o(.text*)
        *GIC.o(.text*)
        *interrupt.o(.text*)
        *bsp.o(.text*)
        *reset.o(.text*)
        *ppset.o(.text*)
        *i2c_dec.o(.text*)
        *pmic.o(.text*)
        *uart.o(.text*)
        *fuse.o(.text*)
        *timer.o(.text*)
        *print.o(.text*)
         . = ALIGN(4);
    }>ATCM

    .text_lib 0xB0020000 : AT(0x110000) {        
        *(.text) *(.text*)
    }>DTCM

    .rel.plt : { *(.rel.plt) }>DTCM
    
    .note.gnu.build-id : {*(.note.gnu.build-id)}>DTCM

    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }>DTCM
    __exidx_end = .;

    . = ALIGN(4);
    .rodata : { *(.rodata) *(.rodata*)}>DTCM
    . = ALIGN(4);

    .got : { *(.got) *(.got*)}>DTCM

    . = ALIGN(4);
    .data : { *(.data) *(.data*)}>DTCM

     . = ALIGN(4);
     __bss_start = .;
     __bss_start__ = .;
     .bss : { *(.bss) *(.bss*)}>DTCM
     __bss_end__ = .;
     _end = . ;

     . = ALIGN(4);
    .stack : {
        __stack_start__ = . ;
        . = . + STACK_SIZE;
        . = ALIGN(4);
        __stack_end__ = . ;
    }>DTCM

   PROVIDE (__init_array_start = .);
   init_array     : { *(.init_array) }   
   PROVIDE (__init_array_end = .);

/* Remove information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    . = ALIGN(4);
    __kern_size__ = .;

    PROVIDE (end = .);    
}
