/*
 * AM33XX-CM3 firmware
 *
 * Cortex-M3 (CM3) firmware for power management on Texas Instruments' AM33XX series of SoCs
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This software is licensed under the  standard terms and conditions in the Texas Instruments  Incorporated
 *  Technology and Software Publicly Available Software License Agreement , a copy of which is included in the
 *  software download.
*/

#ifndef __DPLL_H__
#define __DPLL_H__

enum dpll_id {
	DPLL_PER,
	DPLL_DISP,
	DPLL_DDR,
	DPLL_MPU,
	DPLL_CORE,

	DPLL_COUNT,
	DPLL_END = -1,
};

void plls_power_down(void);
void plls_power_up(void);

void pll_bypass(enum dpll_id dpll);
void pll_lock(enum dpll_id dpll);

void dpll_reset(void);
void dpll_init(void);

#endif
