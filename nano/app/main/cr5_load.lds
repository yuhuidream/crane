OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_begin)

/* The size of the stack used by the application. NOTE: you need to adjust  */
STACK_SIZE = 0x1000;

SECTIONS{
    . = 0x200000;
    .text : {
        *self_load.o(.text*)
    }

    .rel.plt : { *(.rel.plt) }

    . = ALIGN(4);
    __exidx_start = .;
    .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
    __exidx_end = .;

    . = ALIGN(4);
    .rodata : { *(.rodata) *(.rodata*)}
    . = ALIGN(4);

    .got : { *(.got) *(.got*)}

    . = ALIGN(4);
    .data : { *(.data) *(.data*)}

     . = ALIGN(4);
     __bss_start = .;
     __bss_start__ = .;
     .bss : { *(.bss) *(.bss*)}
     __bss_end__ = .;
     _end = . ;

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
