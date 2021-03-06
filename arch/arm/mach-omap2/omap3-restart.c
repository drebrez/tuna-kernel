/*
 * omap3-restart.c - Code common to all OMAP3xxx machines.
 *
 * Copyright (C) 2009, 2012 Texas Instruments
 * Copyright (C) 2010 Nokia Corporation
 * Tony Lindgren <tony@atomide.com>
 * Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/init.h>

#include "iomap.h"
#include "common.h"
#include "control.h"
#include "prm3xxx.h"

/* Global address base setup code */

/**
 * omap3xxx_restart - trigger a software restart of the SoC
 * @mode: the "reboot mode", see arch/arm/kernel/{setup,process}.c
 * @cmd: passed from the userspace program rebooting the system (if provided)
 *
 * Resets the SoC.  For @cmd, see the 'reboot' syscall in
 * kernel/sys.c.  No return value.
 */


static int in_panic;

static int panic_prep_restart(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	in_panic = 1;
	return NOTIFY_DONE;
}

static struct notifier_block panic_block = {
	.notifier_call	= panic_prep_restart,
};

void omap3xxx_restart(char mode, const char *cmd)
{
	if (cmd != NULL) {
		if ((strncmp(cmd, "bootloader", 10)) &&
		    (strncmp(cmd, "recovery", 8)))
			cmd = NULL;
	}

	if (in_panic)
		cmd = "panic";

	omap3_ctrl_write_boot_mode((cmd ? (u8)*cmd : 0));
	omap3xxx_prm_dpll3_reset(); /* never returns */
	while (1);
}

static int __init omap3xxx_restart_init(void)
{
	atomic_notifier_chain_register(&panic_notifier_list, &panic_block);
	return 0;
}
early_initcall(omap3xxx_restart_init);
