/* Linker script for the firmware */

MEMORY
{
    UMEM (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00004000
    DMEM (rw) : ORIGIN = 0x00080000, LENGTH = 0x00002000
    AUXDATA (rw) : ORIGIN = 0x00081000, LENGTH = 0x00000100
    LOGBUF (rw) : ORIGIN = 0x00081100, LENGTH = 0x00000F00
}

_end_stack = 0x00081000;

SECTIONS
{
    .text :
    {
        _start_text = .;
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        *(.note*)
        _end_text = .;
    } > UMEM
    .data : AT(ADDR(.text) + SIZEOF(.text))
    {
        _start_data = .;
        *(vtable)
        *(.data*)
        _end_data = .;
    } > DMEM
    .resource_table :
    {
        _start_resource_table = .;
        *(.resource_table)
	_end_resource_table = .;
    } > DMEM
    .bss :
    {
        _start_bss = .;
        *(.bss*)
        *(COMMON)
        _end_bss = .;
    } > DMEM
    .aux_data :
    {
        _aux_data_start = .;
       . += 0x100;
        _aux_data_end = .;
    } >AUXDATA
    .logbuf :
    {
        _logbuf_start = .;
       . += 0xF00;
        _logbuf_end = .;
    } >LOGBUF
}
