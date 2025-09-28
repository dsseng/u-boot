// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2021 Rockchip Electronics Co., Ltd.
 */

#include <config.h>
#include <dm.h>
#include <ram.h>
#include <syscon.h>
#include <asm/arch-rockchip/clock.h>
#include <asm/arch-rockchip/grf_rv1106.h>
#include <asm/arch-rockchip/sdram.h>

struct dram_info {
	struct ram_info info;
	// struct rv1106_pmu1grf *pmugrf;
};

static int rv1106_dmc_probe(struct udevice *dev)
{
	struct dram_info *priv = dev_get_priv(dev);

	// priv->pmugrf = syscon_get_first_range(ROCKCHIP_SYSCON_PMUGRF);
	priv->info.base = CFG_SYS_SDRAM_BASE;
	priv->info.size = 0x8000000*2;//0xff000000;
		// rockchip_sdram_size((phys_addr_t)&priv->pmugrf->os_reg[2]) +
		// rockchip_sdram_size((phys_addr_t)&priv->pmugrf->os_reg[4]);

	return 0;
}

static int rv1106_dmc_get_info(struct udevice *dev, struct ram_info *info)
{
	struct dram_info *priv = dev_get_priv(dev);

	*info = priv->info;

	return 0;
}

static struct ram_ops rv1106_dmc_ops = {
	.get_info = rv1106_dmc_get_info,
};

static const struct udevice_id rv1106_dmc_ids[] = {
	{ .compatible = "rockchip,rv1106-dmc" },
	{ }
};

U_BOOT_DRIVER(dmc_rv1106) = {
	.name = "rockchip_rv1106_dmc",
	.id = UCLASS_RAM,
	.of_match = rv1106_dmc_ids,
	.ops = &rv1106_dmc_ops,
	.probe = rv1106_dmc_probe,
	.priv_auto = sizeof(struct dram_info),
};
