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

#include <device_common.h>
#include <io.h>
#include <prcm_core.h>
#include <hwmod.h>
#include <ddr.h>
#include <msg.h>

/*
 * Values recommended by the HW team. These change the pulls
 * on certain IOs of DATA and CMD macros
 */
#define AM335X_SUSP_IO_PULL_DATA      0x3FF00003
#define AM335X_SUSP_IO_PULL_CMD1      0xFFE0018B
#define AM335X_SUSP_IO_PULL_CMD2      0xFFA0098B

#define AM437X_SUSP_IO_PULL_DATA	0x3FF00003
#define AM437X_SUSP_IO_PULL_CMD1_DDR3	0x08000000
#define AM437X_SUSP_IO_PULL_CMD2_DDR3	0xF7FFFFFF
#define AM437X_SUSP_IO_PULL_CMD1_LPDDR2	0x00000000
#define AM437X_SUSP_IO_PULL_CMD2_LPDDR2	0xFFFFFFFF

struct ddr_io_context {
	unsigned long cmd0_ioctrl;
	unsigned long cmd1_ioctrl;
	unsigned long cmd2_ioctrl;

	unsigned long data0_ioctrl;
	unsigned long data1_ioctrl;
	unsigned long data2_ioctrl;
	unsigned long data3_ioctrl;
};

static struct ddr_io_context io_ctx;

void ddr_io_save_context(void)
{
	io_ctx.cmd0_ioctrl = __raw_readl(DDR_CMD0_IOCTRL);
	io_ctx.cmd1_ioctrl = __raw_readl(DDR_CMD1_IOCTRL);
	io_ctx.cmd2_ioctrl = __raw_readl(DDR_CMD2_IOCTRL);

	io_ctx.data0_ioctrl = __raw_readl(DDR_DATA0_IOCTRL);
	io_ctx.data1_ioctrl = __raw_readl(DDR_DATA1_IOCTRL);

	if (soc_id == AM43XX_SOC_ID) {
		io_ctx.data2_ioctrl = __raw_readl(DDR_DATA2_IOCTRL);
		io_ctx.data3_ioctrl = __raw_readl(DDR_DATA3_IOCTRL);
	}
}

void ddr_am335x_io_suspend(void) {
	unsigned int var;

	ddr_io_save_context();

	/* mddr mode selection required only for PG1.0 */
	if (soc_rev == AM335X_REV_ES1_0 && (mem_type == MEM_TYPE_DDR2 ||
	    mem_type == MEM_TYPE_DDR3)) {
	        var = __raw_readl(DDR_IO_CTRL_REG);
	        var |= DDR_IO_MDDR_SEL;
	        __raw_writel(var, DDR_IO_CTRL_REG);
	}

	/* Weak pull down for DQ, DM */
	__raw_writel(AM335X_SUSP_IO_PULL_DATA, DDR_DATA0_IOCTRL);
	__raw_writel(AM335X_SUSP_IO_PULL_DATA, DDR_DATA1_IOCTRL);

        /* Weak pull down for macro CMD0/1 */
        __raw_writel(AM335X_SUSP_IO_PULL_CMD1, DDR_CMD0_IOCTRL);
        __raw_writel(AM335X_SUSP_IO_PULL_CMD1, DDR_CMD1_IOCTRL);

        /*
	 * Weak pull down for macro CMD2
	 * exception: keep DDR_RESET pullup
	 */
        __raw_writel(AM335X_SUSP_IO_PULL_CMD2, DDR_CMD2_IOCTRL);
}

void ddr_am437x_io_suspend(void)
{
	unsigned int var;

	ddr_io_save_context();

	/* Weak pull down for DQ, DM */
	__raw_writel(AM437X_SUSP_IO_PULL_DATA, DDR_DATA0_IOCTRL);
	__raw_writel(AM437X_SUSP_IO_PULL_DATA, DDR_DATA1_IOCTRL);
	__raw_writel(AM437X_SUSP_IO_PULL_DATA, DDR_DATA2_IOCTRL);
	__raw_writel(AM437X_SUSP_IO_PULL_DATA, DDR_DATA3_IOCTRL);

	if (mem_type == MEM_TYPE_LPDDR2) {
		/* Configure LPDDR2 Dynamic power down */
		var = __raw_readl(EMIF_SDRAM_CONFIG_EXT);
		var |= DYNAMIC_PWR_DOWN;
		__raw_writel(var, EMIF_SDRAM_CONFIG_EXT);

		__raw_writel(AM437X_SUSP_IO_PULL_CMD1_LPDDR2, DDR_CMD1_IOCTRL);
		__raw_writel(AM437X_SUSP_IO_PULL_CMD2_LPDDR2, DDR_CMD2_IOCTRL);
	} else if (mem_type ==  MEM_TYPE_DDR3) {
		/* Weak pull down for macro CMD0/1 */
		__raw_writel(AM437X_SUSP_IO_PULL_CMD1_DDR3, DDR_CMD1_IOCTRL);

		/*
		 * Weak pull down for macro CMD2
		 * exception: keep DDR_RESET pullup
		 */
		__raw_writel(AM437X_SUSP_IO_PULL_CMD2_DDR3, DDR_CMD2_IOCTRL);
	}
}

void ddr_io_suspend(void)
{
	if (soc_id == AM335X_SOC_ID)
		ddr_am335x_io_suspend();
	else if (soc_id == AM43XX_SOC_ID)
		ddr_am437x_io_suspend();
}

void ddr_io_resume(void)
{
	unsigned int var;

	/* mddr mode selection required only for PG1.0 */
	if (soc_id == AM335X_SOC_ID && soc_rev == AM335X_REV_ES1_0 &&
	    (mem_type == MEM_TYPE_DDR2 || mem_type == MEM_TYPE_DDR3)) {
		var = __raw_readl(DDR_IO_CTRL_REG);
		var &= ~DDR_IO_MDDR_SEL;
		/* Take out IO of mDDR mode */
		__raw_writel(var, DDR_IO_CTRL_REG);
	}

	__raw_writel(io_ctx.cmd0_ioctrl, DDR_CMD0_IOCTRL);
	__raw_writel(io_ctx.cmd1_ioctrl, DDR_CMD1_IOCTRL);
	__raw_writel(io_ctx.cmd2_ioctrl, DDR_CMD2_IOCTRL);

	__raw_writel(io_ctx.data0_ioctrl, DDR_DATA0_IOCTRL);
	__raw_writel(io_ctx.data1_ioctrl, DDR_DATA1_IOCTRL);

	if (soc_id == AM43XX_SOC_ID) {
		__raw_writel(io_ctx.data2_ioctrl, DDR_DATA2_IOCTRL);
		__raw_writel(io_ctx.data3_ioctrl, DDR_DATA3_IOCTRL);
	}
}

/* same offsets for SA and Aegis */
void vtp_enable(void)
{
	unsigned int var;

	/* clear the register */
	__raw_writel(0x0, VTP0_CTRL_REG);

	/* write the filter value */
	__raw_writel(0x6, VTP0_CTRL_REG);

	/* set the VTP enable bit */
	var = __raw_readl(VTP0_CTRL_REG);
	__raw_writel((var | VTP_CTRL_ENABLE), VTP0_CTRL_REG);

	/* toggle the CLRZ bit */
	var = __raw_readl(VTP0_CTRL_REG);

	__raw_writel((var & ~VTP_CTRL_START_EN), VTP0_CTRL_REG);
	__raw_writel((var | VTP_CTRL_START_EN), VTP0_CTRL_REG);

	/* poll for VTP ready */
	while (!(__raw_readl(VTP0_CTRL_REG) & VTP_CTRL_READY));
}

/* same offsets for SA and Aegis */
void vtp_disable(void)
{
	if (mem_type == MEM_TYPE_DDR2)
		__raw_writel(VTP_CTRL_VAL_DDR2, VTP0_CTRL_REG);
	else
		__raw_writel(VTP_CTRL_VAL_DDR3, VTP0_CTRL_REG);
}

void vtt_high(void)
{
	if (vtt_toggle == false)
		return;

	hwmod_enable(HWMOD_GPIO0);

	__raw_writel((1 << vtt_gpio_pin), GPIO0_BASE + GPIO_SETDATAOUT);

	hwmod_disable(HWMOD_GPIO0);
}

void vtt_low(void)
{
	if (vtt_toggle == false)
		return;

	hwmod_enable(HWMOD_GPIO0);

	__raw_writel((1 << vtt_gpio_pin), GPIO0_BASE + GPIO_CLEARDATAOUT);

	hwmod_disable(HWMOD_GPIO0);
}

/* RESET line is applicable only to DDR3 */
void set_ddr_reset(void)
{
	unsigned int var;

	/*
	 * TODO: Make this a one-time change in MPU code itself
	 */
	if (mem_type == MEM_TYPE_DDR3) {
		/* hold DDR_RESET high via control module */
		var = __raw_readl(DDR_IO_CTRL_REG);
		var |= DDR3_RST_DEF_VAL;
		__raw_writel(var, DDR_IO_CTRL_REG);
	}
}

void clear_ddr_reset(void)
{
	unsigned int var;

	/*
	 * TODO: Make this a one-time change in MPU code itself
	 */
	if (mem_type == MEM_TYPE_DDR3) {
		/* make DDR_RESET low via control module */
		var = __raw_readl(DDR_IO_CTRL_REG);
		var &= ~DDR3_RST_DEF_VAL;
		__raw_writel(var, DDR_IO_CTRL_REG);
	}
}

