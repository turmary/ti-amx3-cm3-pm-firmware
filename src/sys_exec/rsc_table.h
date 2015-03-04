/*
 * AM33XX-CM3 firmware
 *
 * Cortex-M3 (CM3) firmware for power management on Texas Instruments' AM33XX series of SoCs
 *
 * Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
 */

#include <stdint.h>
#include <stddef.h>

#define CM3_RSC_TBL_VERSION	0x1
#define CM3_RSC_NUM		0x1

/*
 * These values and structs must match those in include/linux/remoteproc.h
 * within Linux, otherwise remoteproc will not be able to parse the
 * resource table properly. Refer to kernel for remoteproc documentation.
 */

#define RSC_CARVEOUT	0
#define RSC_DEVMEM	1
#define RSC_TRACE	2
#define RSC_VDEV	3

struct resource_table {
	uint32_t ver;
	uint32_t num;
	uint32_t reserved[2];
	uint32_t offset[0];
} __attribute__ ((__packed__));

struct fw_rsc_hdr {
	uint32_t type;
	uint8_t data[0];
} __attribute__ ((__packed__));

struct fw_rsc_trace {
	uint32_t da;
	uint32_t len;
	uint32_t reserved;
	uint8_t name[32];
} __attribute__ ((__packed__));

struct cm3_resource_table {
	struct resource_table base;

	uint32_t offset[CM3_RSC_NUM]; /* Should match 'num' in actual definition */
	struct fw_rsc_hdr cm3_trace_hdr;
	struct fw_rsc_trace cm3_trace;
} __attribute__ ((__packed__));

struct cm3_resource_table rsc_tab __attribute__ ((section (".resource_table"))) = {
	{
		CM3_RSC_TBL_VERSION,	/* Version */
		CM3_RSC_NUM,		/* Number of resources */
		{0,0,},			/* Reserved */
	},

	/* List of offsets to each resource entry header */
	{
		__builtin_offsetof(struct cm3_resource_table, cm3_trace_hdr),
	},

	{RSC_TRACE,},
	{
		0x81100,	/* Device address */
		0xF00,		/* Length */
		0,		/* Reserved */
		"log_buf",	/* Name */
	},
};
