/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2019 Rockchip Electronics Co., Ltd
 * Copyright (c) 2022 Edgeble AI Technologies Pvt. Ltd.
 */

#ifndef __CONFIG_RV1106_COMMON_H
#define __CONFIG_RV1106_COMMON_H

#include "rockchip-common.h"

#define CFG_SYS_HZ_CLOCK		24000000

#define CFG_IRAM_BASE		0xff6c0000

#define GICD_BASE			0xff1f1000
#define GICC_BASE			0xff1f2000

#define CFG_SYS_SDRAM_BASE		0
#define SDRAM_MAX_SIZE			0xff000000

/*
 *   Image:  0 - 8M
 *  zImage:  8 - 12M
 *     fdt: 12 - 13M
 * ramdisk: 14 ...
 */
#define ENV_MEM_LAYOUT_SETTINGS	 \
"scriptaddr=0x00b00000\0"	\
	"pxefile_addr_r=0x00c00000\0"	\
	"fdt_addr_r=0x00c00000\0"	\
	"kernel_addr_c=0x00808000\0"	\
	"kernel_addr_r=0x00008000\0"	\
	"ramdisk_addr_r=0x000e00000\0"

#if defined(CONFIG_SPL_DFU) && defined(CONFIG_XPL_BUILD)
#define CFG_EXTRA_ENV_SETTINGS						\
	"dfu_alt_info_ram=u-boot.itb ram "				\
			__stringify(CONFIG_SPL_LOAD_FIT_ADDRESS) " "	\
			__stringify(CONFIG_SYS_DFU_MAX_FILE_SIZE) "\0"
#else
#define CFG_EXTRA_ENV_SETTINGS \
	"fdt_high=0x0fffffff\0" \
	"initrd_high=0x0fffffff\0" \
	"bootcmd=mw.l 0xff020200 0xEF08A53C;"                                                                                                  \
	"setenv dfu_alt_info \"ram 0=uImage ram 0x2000000 0x1400000;devicetree.dtb ram 0x3400000 0x400000;initcpio ram 0x5d00000 0x2000000\";" \
	"dfu 0;"                                                                                                                               \
	"setenv bootargs 'earlycon=uart8250,mmio32,0xff4c0000 console=ttyS2,115200 cma=1M ignore_loglevel tp_printk oops=panic panic=1';" \
	"bootz 0x02000000 0x5d00000:0x6a3400 0x03400000;\0"   \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"partitions=" PARTS_DEFAULT \
	ENV_MEM_LAYOUT_SETTINGS	 \
	ROCKCHIP_DEVICE_SETTINGS \
	"boot_targets=" BOOT_TARGETS "\0"
#endif /* defined(CONFIG_SPL_DFU) && defined(CONFIG_XPL_BUILD) */

#endif /* __CONFIG_RV1106_COMMON_H */
