OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_entry)

/* The size of the stack used by the application. NOTE: you need to adjust  */
STACK_SIZE = 0x8000;

SECTIONS{
    . = 0xFFFD0000;
    .vector : {
	*startup_mrvl.o(.text*)
	*system_mrvl.o(.text*)
    . = ALIGN(4);
    }
    .text : {
    *(.text) *(.text*)
	. = ALIGN(4);
    }

    . = ALIGN(4);
    .rodata : { *(.rodata) *(.rodata*) }
    
	. = ALIGN(4);
    .ARM.extab   : { *(.ARM.extab*) }

    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
    __exidx_end = .;

    . = ALIGN(4);
    .got : { *(.got) *(.got*) }

    . = ALIGN(4);
    .data : { *(.data) *(.data*)}

    .note.gnu.build-id : { *(.note.gnu.build-id)}

     . = ALIGN(4);
     __bss_start = .;
     __bss_start__ = .;
     .bss : { *(.bss) *(.bss*) }
     __bss_end__ = .;
     _end = . ;

     . = ALIGN(4);
    .stack : {
        __stack_start__ = . ;
        . = . + STACK_SIZE;
        . = ALIGN(4);
        __stack_end__ = . ;
    }

    /* Remove information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
    
    /* Remove the buffer allocation to driver/set_mmu.h(.c)*/
	
    . = ALIGN(4);
    __kern_size__ = .;

    PROVIDE (end = .);	 
}
