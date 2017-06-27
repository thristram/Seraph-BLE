/******************************************************************************
 *  Copyright (c) 2014 - 2016 Qualcomm Technologies International, Ltd.
 *  Part of CSR uEnergy SDK 2.6.1
 *
 *  FILE
 *      memory.h
 *
 *  DESCRIPTION
 *      Memory boundary symbols
 *
 ******************************************************************************/
#ifndef __MEMORY_H__
#define __MEMORY_H__

/* 
Basic memory definitions common to CSR1000 firmware builds.
This is used by the standard CSR1000 memory versions.
*/


/**********************/
/* PROGRAM MEMORY MAP */
/**********************/
#define PROG_RAM_START 0x0000
#define PROG_RAM_SIZE 0x8000
#define PROG_ROM_START 0x8000
#define PROG_ROM_SIZE 0x8000


/*******************/
/* DATA MEMORY MAP */
/*******************/

/* Data RAM window */
#define DATA_RAM_START 0x2000
#define DATA_RAM_SIZE 0x8000

/* Fixed size areas defined to be at the end of data RAM.
 * They must not be moved or resized, as they are implicitly
 * known about by external tools.
 */
#define RADIOBUFS_SIZE 0x01B0
#define WHITELIST_SIZE 0x0020
#define RADIODATA_SIZE 0x0164
#define USTACK_SIZE 0x0200
#define ISTACK_SIZE 0x0100
#define DATA_CSTORE_START 0x9F84
#define CSTORE_SIZE 0x0058
#define SLT_SIZE 0x0004
#define PRESERVE_SIZE 0x0020
#define DATA_RAM_END 0xA000

/* Data ROM window */
#define DATA_ROM_START 0xA000
#define DATA_ROM_SIZE 0x4000

/* Data Persistent memory window - not 'normal' RAM */
#define DATA_PERSIST_START 0xEF80
#define DATA_PERSIST_SIZE 0x0080


/* The data map address at which ROM data (DATA+BSS) starts. This
 * should be as high as possible without making DATA_ROMBSS_END overlap
 * with DATA_ALIGN_START.
 */
#define DATA_ROMDATA_START 0x9360



/** Exported to SDK applications **/
/** The 'end' should be machine-generated depending on the ROM's RAM requirements */
/**************************/
/* Application memory map */
/**************************/
#define PROG_APPCODE_START 0x0000
#define PROG_APPCODE_END 0x6C00
#define DATA_APPDATA_START 0x8C40
#define DATA_APPDATA_END 0x8F00

#endif /* __MEMORY_H__ */
