// SPDX-License-Identifier: GPL-2.0+
/*
 * Copied from sdram_rk3588.c:
 * (C) Copyright 2021 Rockchip Electronics Co., Ltd.
 *
 * Minor adaptation for rv1106:
 * Copyright (c) 2025 Dmitrii Sharshakov <d3dx12.xx@gmail.com>
 *
 */

#include <config.h>
#include <dm.h>
#include <ram.h>
#include <asm/arch-rockchip/sdram.h>

// CHIP_VER_REG + 4
#define OS_REG_2 0xff020204 + 4

struct dram_info {
	struct ram_info info;
};

static int rv1106_dmc_probe(struct udevice *dev)
{
	struct dram_info *priv = dev_get_priv(dev);

	priv->info.base = CFG_SYS_SDRAM_BASE;
	priv->info.size = rockchip_sdram_size(OS_REG_2);

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
