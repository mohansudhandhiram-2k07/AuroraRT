ENTRY(reset_handler)

_estack = 0x20020000;

MEMORY
{
    FLASH(rx): ORIGIN=0x08000000, LENGTH=1024K
    SRAM(rwx): ORIGIN=0x20000000, LENGTH=128K
    
}

SECTIONS
{
    .text:
    {
        . = ALIGN(4);
        *(.isr_vector)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    }>FLASH

    .data:
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    }>SRAM AT > FLASH

    .bss:
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        . = ALIGN(4);
        _ebss = .;
    }>SRAM
}