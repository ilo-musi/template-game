ENTRY(Reset_Handler)

STACK_SIZE = 512;
HEAP_SIZE = 0;

RAM_START = 0x20000000;
RAM_BOOTLOADER_EXCLUSIVE_SIZE = 512;
RAM_TOTAL_SIZE = 4K;

FLASH_START = 0x8000000;
FLASH_BOOTLOADER_HEADER_SIZE = 4K;
FLASH_BOOTLOADER_FOOTER_SIZE = 8K; /* 12K for debug; 8K for release.*/
FLASH_TOTAL_SIZE = 32K;

BOOTLOADER_LIBRARY_FUNCTION_MAPPER_SIZE = ((26-1)*16);

_estack = (RAM_START)+(RAM_TOTAL_SIZE)-(RAM_BOOTLOADER_EXCLUSIVE_SIZE);

MEMORY
{
RAM (xrw)			: ORIGIN = RAM_START, LENGTH = (RAM_TOTAL_SIZE)-(RAM_BOOTLOADER_EXCLUSIVE_SIZE)
RAM_BOOTLOADER_EXCLUSIVE (xrw)			: ORIGIN = (RAM_START)+(RAM_TOTAL_SIZE)-(RAM_BOOTLOADER_EXCLUSIVE_SIZE), LENGTH = RAM_BOOTLOADER_EXCLUSIVE_SIZE

FLASH_BOOTLOADER_HEADER (rx)			: ORIGIN = FLASH_START, LENGTH = FLASH_BOOTLOADER_HEADER_SIZE
FLASH (rwx)			: ORIGIN = (FLASH_START)+(FLASH_BOOTLOADER_HEADER_SIZE), LENGTH = (FLASH_TOTAL_SIZE)-(FLASH_BOOTLOADER_HEADER_SIZE)-(FLASH_BOOTLOADER_FOOTER_SIZE)
FLASH_BOOTLOADER (rx)			: ORIGIN = (FLASH_START)+(FLASH_TOTAL_SIZE)-(FLASH_BOOTLOADER_FOOTER_SIZE), LENGTH = FLASH_BOOTLOADER_FOOTER_SIZE
}

SECTIONS
{ 
	/* Interrupt vector at the beginning of the FLASH */
	.isr_vector : {
		. = ALIGN(4);
		KEEP(*(.isr_vector))
		. = ALIGN(4);
	} >FLASH

	/* The program code and other data goes into FLASH */
	.text : {
		. = ALIGN(4);
		*(.text*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		*(.eh_frame) /* For unwinding */
		. = ALIGN(4);

		_etext = .; /* global variable */
	} >FLASH

	 /* For unwinding */
	.ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
	 /* For unwinding */
	.ARM : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >FLASH

	 /* For C++ constructor/destructor */
	.preinit_array : {
		PROVIDE_HIDDEN(__preinit_array_start = .);
		KEEP(*(.preinit_array*))
		PROVIDE_HIDDEN(__preinit_array_end = .);
	} >FLASH
	/* For C++ constructor/destructor */
	.init_array : {
		PROVIDE_HIDDEN(__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array*))
		PROVIDE_HIDDEN(__init_array_end = .);
	} >FLASH
	/* For C++ constructor/destructor */
	.fini_array : {
		PROVIDE_HIDDEN(__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array*))
		PROVIDE_HIDDEN(__fini_array_end = .);
	} >FLASH

	/* Data to be stored into the RAM */
	_sidata = .;
	.data : {
		. = ALIGN(4);
		_sdata = .;
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} >RAM AT> FLASH

	/* Data section for data that does not require initialization */
	.bss : {
		. = ALIGN(4);
		_sbss = .;
		__bss_start__ = .;
		*(.bss*)
		*(COMMON) /* uninitialized data */
		. = ALIGN(4);
		_ebss = .;
		__bss_end__ = .;
	} >RAM

	/* For semi-hosting */
	end = .;
	_end = .;

	/* Stack and Heap. By manipulating the cursor to move STACK_SIZE+HEAP_SIZE amount of memory,
		we can tell if we have reserved sufficient memory for the STACK and HEAP.
		If the amount of memory isn't enough, a compilation error will be raised. */
	._stack_and_heap : {
		. = ALIGN(4);
		. = . + STACK_SIZE + HEAP_SIZE;
		. = ALIGN(4);
	} >RAM

	/* Bootloader-exclusive global variables */
	.bootloader_ram : {
		. = ALIGN(4);
		KEEP(*(.bootloader_ram))
		. = ALIGN(4);
	} >RAM_BOOTLOADER_EXCLUSIVE
}


