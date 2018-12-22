OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry)

/* The size of the stack used by the application. NOTE: you need to adjust  */
STACK_SIZE = 0x8000;
AO_STACK_SIZE = 0x18000; /* should not less than total size of all the AO stacks*/
NCNB_SIZE  = 0x1000;   /* Non-cacheable non-bufferable buffer size */

SECTIONS{
    __code_entry = .;
    .vector 0x0 : AT(CR5_LOAD_ADDR) {
    *startup_mrvl.o(.text*)
    . = ALIGN(4);
    }
    __atcm_code_end = .;

    . = 0x1000+CR5_LOAD_ADDR;
    .text : AT(0x1000+CR5_LOAD_ADDR) {
    *(.text) *(.text*)
    . = ALIGN(4);
    }

	.rel.plt        : { *(.rel.plt) }

    . = ALIGN(4);
    .rodata : { *(.rodata) *(.rodata*)} 

    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } 
    __exidx_end = .;

    . = ALIGN(4);
    .got : { *(.got) *(.got*)} 

    . = ALIGN(4);
    .data : { *(.data) *(.data*)} 

    .note.gnu.build-id : { *(.note.gnu.build-id)}

     . = ALIGN(4);
     __bss_start = .;
     __bss_start__ = .;
     .bss : { *(.bss) *(.bss*)} 
     __bss_end__ = .;
     _end = . ;

    . = ALIGN(0x1000);
    __ncnbBuffer_start__ = .;
    ncnbBuffer :{
       . = ALIGN(4);
    }

    . = __ncnbBuffer_start__ + NCNB_SIZE;

    __ncnbBuffer_end__ = .;
     . = ALIGN(4);
    .stack : {
        __stack_start__ = . ;
        . = . + STACK_SIZE;
        . = ALIGN(4);
        __stack_end__ = . ;
    } 
    . = ALIGN(0x4000);
    .ao_stack :{
        __ao_stack_start__ = .;
        . = . + AO_STACK_SIZE;
        __ao_stack_end__ = .;
    } 

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
