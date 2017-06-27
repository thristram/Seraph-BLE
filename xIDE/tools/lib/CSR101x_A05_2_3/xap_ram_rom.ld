/* Important info not found in manual
 * It appears that the address of an output section is set the first
 * time it is encountered. Attempts to leave gaps by setting the address
 * in subsequent instances are silently ignored
 */

INCLUDE memory.cfg
INCLUDE external_syms.ld

ENTRY(reset_vector);

$CODE$ = 0x80000000;
$DATA$ = 0x0000;

OUTPUT_ARCH(xap2+)

PROVIDE($XAP_AH  = 0xFFE0);
PROVIDE($XAP_AL  = 0xFFE1);
PROVIDE($XAP_UXH = 0xFFE2);
PROVIDE($XAP_UXL = 0xFFE3);
PROVIDE($XAP_UY  = 0xFFE4);
PROVIDE($XAP_IXH = 0xFFE5);
PROVIDE($XAP_IXL = 0xFFE6);
PROVIDE($XAP_IY  = 0xFFE7);

/************************************************/
SECTIONS
{

        /* Conventions:
        *   PROG_* : Program address of code sections.
        *   P_*    : Program (ROM) address of data sections.
        *   D_*    : Data (ROM) address of data sections in the ROM window.
        *   DATA_* : Data address of data sections at runtime (ie location in real RAM).
        *
        * A Section's load address (AT...) is its location in ROM (ie PROG_ or P_).
        */

        /*********** Callbacks *****************/
        . = 0x0008;
        .app_ram_init : {  KEEP(*(RAM_APP_INIT)) }
        .fw_patch_bitmap : { KEEP(*(FW_PATCH_BITMAP)) }
        .fw_patch_table : { KEEP(*(FW_PATCH_TABLE)) }
        PROG_APPCODE_START  = . ;

        /************** Code side **************/
        .text : AT (PROG_APPCODE_START + $CODE$)
        {
                *xap2crt0.o(.text)
                *xap2crt0.o(.text.*)
                PROG_PATCH_ADDRESS = .;
                KEEP(*(.hw_patch))
                *(.text)
                *(.text.*)
                *(.apptext)
                *(.apptext.*)
                *xap2crte.o(.text)
                *xap2crte.o(.text.*)
        }
        PROG_APPCODE_END = .;  /* last address of code section */

        /* Change to Data pointer by adding on the DATA_RAM_START offset */
        DATA_CONST_START = DATA_RAM_START + . ;

        /* Constant data mapped within the CODE memory space. Care needs
         * to be taken to ensure data in this section is given the correct
         * offset within the XAP data memory.
         */
        . = DATA_CONST_START;
        .const :
        {
                *(CONST)
                *(CONST.*)
        }
        DATA_CONST_END = .;

        /************** const data **************/
        /* Add 1 word of padding before the start of the rodata */
        DATA_RODATA_START = . ;
        . = . + 1;
        .rodata :
        {
                *(.rodata)
                *(.rodata.*)
        }
        DATA_RODATA_END = . ;

        /************** ZI data **************/
        /* WARNING! To ensure CODE & ALIGN64 section overlaps are
         *          detected, .bss MUST NOT COME FIRST OR LAST!
         *          This section must always follow .rodata
         */
        .bss :
        {
                DATA_BSS_APP_START = . ;
                *(.bss)
                *(.bss.*)
        }
        DATA_BSS_APP_END = . ;
        BSS_APP_SIZE = DATA_BSS_APP_END - DATA_BSS_APP_START;

        /************** mutable data **************/
        .data :
        {
                DATA_DATA_START = . ;
                *(.data)
                *(.data.*)
        }

        DATA_DATA_END = . ;

        /************** Fixed position data **************/
        /* These must be in a fixed, well-known place in memory */

        /****** Persistent Memory ***********/
        . = DATA_PERSIST_START;
        PERSIST_MEM :
        {
                /* The position of this section is hardware-specific.
                 * It will be stripped from the EEPROM image during
                 * the final application post-link build steps
                 */
                *(PERSIST_MEM)
                *(PERSIST_MEM.*)
        }
        DATA_PERSIST_MEM_END = . ;

        /****** Symbol Look-Up Table ***********/
        /* Allocate named section to hold Configuration Store */
        . = DATA_CSTORE_START;
        CONFIG_STORE :
        {
                KEEP(*(CONFIG_STORE))
                KEEP(*(CONFIG_STORE.*))
        }
        DATA_CSTORE_END = . ;
        CS_SIZE = DATA_CSTORE_END - DATA_CSTORE_START;

        /****** Symbol Look-Up Table ***********/
        . = DATA_RAM_END - PRESERVE_SIZE - SLT_SIZE;
        SLT :
        {
                SLT_START = . ;
                *(SLT)
                *(SLT.*)
        }

        /************** preserved data **************/
        .preserved :
        {
                PRESERVED_START = . ;
                *(PRES_DATA)
                *(PRES_DATA.*)
        }

        /************** stack **************/
        STACK_END = DATA_CSTORE_START;
        . = STACK_END - ISTACK_SIZE - USTACK_SIZE;
        .stack :
        {
                STACK_START = . ;
                USTACK_START = . ;
                USTACK_END = USTACK_START  + USTACK_SIZE;
                *(USTACK)
                *(USTACK.*)
                . = USTACK_END;
                ISTACK_START = . ;
                ISTACK_END = ISTACK_START  + ISTACK_SIZE;
                *(ISTACK)
                *(ISTACK.*)
                . = ISTACK_END;
        }

        /* These are the hardware buffers that must be located on 64-bit boundaries */
        ALIGNED_DATA_END = STACK_START;
        . = ALIGNED_DATA_END - RADIODATA_SIZE - WHITELIST_SIZE - RADIOBUFS_SIZE;
        .aligned_data ALIGN(4):
        {
                ALIGNED_DATA_START = . ;
                /* The TXRX buffers overwrite the radio init table */
                DATA_RADIOBUFS_START = . ;
                *(RADINITTBL)
                . = ALIGNED_DATA_START + RADIOBUFS_SIZE;
                DATA_WHITELIST_START = . ;
                . = DATA_WHITELIST_START + WHITELIST_SIZE;
                KEEP(*(.romradiodata))
        }
        ALIGN_END_ACTUAL = . ;
}

/* Check the Application Ram is not about to overwrite the reserved RAM. */
ASSERT( ( DATA_APPDATA_END > DATA_DATA_END ), "RAM DATA exceeds limit." )
