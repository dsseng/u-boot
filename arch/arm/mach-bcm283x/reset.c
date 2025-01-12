// SPDX-License-Identifier: GPL-2.0
/*
 * (C) Copyright 2012 Stephen Warren
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 */

#include <config.h>
#include <cpu_func.h>
#include <asm/io.h>
#include <asm/arch/base.h>
#include <asm/arch/wdog.h>
#include <asm/arch/mbox.h>
#include <asm/arch/msg.h>
#include <efi_loader.h>

#define RESET_TIMEOUT 10

/*
 * The Raspberry Pi firmware uses the RSTS register to know which partiton
 * to boot from. The partiton value is spread into bits 0, 2, 4, 6, 8, 10.
 * Partiton 63 is a special partition used by the firmware to indicate halt.
 */
#define BCM2835_WDOG_RSTS_RASPBERRYPI_HALT	0x555

/* max ticks timeout */
#define BCM2835_WDOG_MAX_TIMEOUT	0x000fffff

__efi_runtime_data struct bcm2835_wdog_regs *wdog_regs;

static void __efi_runtime
__reset_cpu(struct bcm2835_wdog_regs *wdog_regs, ulong ticks)
{
	bcm2835_notify_reboot();

	uint32_t rstc, timeout;

	if (ticks == 0)
		timeout = RESET_TIMEOUT;
	else
		timeout = ticks & BCM2835_WDOG_MAX_TIMEOUT;

	rstc = readl(&wdog_regs->rstc);
	rstc &= ~BCM2835_WDOG_RSTC_WRCFG_MASK;
	rstc |= BCM2835_WDOG_RSTC_WRCFG_FULL_RESET;

	writel(BCM2835_WDOG_PASSWORD | timeout, &wdog_regs->wdog);
	writel(BCM2835_WDOG_PASSWORD | rstc, &wdog_regs->rstc);
}

void reset_cpu(void)
{
	struct bcm2835_wdog_regs *regs =
		(struct bcm2835_wdog_regs *)BCM2835_WDOG_PHYSADDR;

	__reset_cpu(regs, 0);
}

int __do_poweroff(void)
{
	u32 rsts;
	/*
	* We set the watchdog hard reset bit here to distinguish this reset
	* from the normal (full) reset. bootcode.bin will not reboot after a
	* hard reset.
	*/
	rsts = readl(&wdog_regs->rsts);
	rsts |= BCM2835_WDOG_PASSWORD;
	rsts |= BCM2835_WDOG_RSTS_RASPBERRYPI_HALT;
	writel(rsts, &wdog_regs->rsts);

	__reset_cpu(wdog_regs, 0);

	return 0;
}

int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	return __do_poweroff();
}

#ifdef CONFIG_EFI_LOADER

void __efi_runtime EFIAPI efi_reset_system(
			enum efi_reset_type reset_type,
			efi_status_t reset_status,
			unsigned long data_size, void *reset_data)
{
	if (reset_type == EFI_RESET_COLD ||
	    reset_type == EFI_RESET_WARM ||
	    reset_type == EFI_RESET_PLATFORM_SPECIFIC) {
		__reset_cpu(wdog_regs, 0);
	} else if (reset_type == EFI_RESET_SHUTDOWN) {
		__do_poweroff();
	}

	while (1) { }
}

efi_status_t efi_reset_system_init(void)
{
	int ret;

	wdog_regs = (struct bcm2835_wdog_regs *)BCM2835_WDOG_PHYSADDR;
	ret = efi_add_runtime_mmio(&wdog_regs, sizeof(*wdog_regs));
	if (ret != EFI_SUCCESS)
		return ret;

	return efi_add_runtime_mmio(
		(struct bcm2835_mbox_regs *)BCM2835_MBOX_PHYSADDR,
		sizeof(struct bcm2835_mbox_regs)
	);
}

#endif
