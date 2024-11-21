// SPDX-License-Identifier: GPL-2.0 OR MIT
/*
 * Date & Time support for Raspberry Pi 5 RTC. No alarms or battery charging support
 *
 * Based on goldfish_rtc.c by Heinrich Schuchardt
 * 
 * Firmware interface reference from:
 * linux-rpi-6.12.y/drivers/rtc/rtc-rpi.c
 * https://github.com/raspberrypi/linux/blob/ab9bb48bcf5bebc8a75df84fc992096ed9d1c57b/drivers/rtc/rtc-rpi.c
 *
 * Dmitrii Sharshakov <d3dx12.xx@gmail.com>
 */

#include <dm.h>
#include <rtc.h>
#include <memalign.h>
#include <asm/arch/msg.h>

// No support for alarms and battery charging in U-Boot
enum {
	REG_RTC_TIME = 0,
	REG_RTC_ALARM,
	REG_RTC_ALARM_PENDING,
	REG_RTC_ALARM_ENABLE,
	REG_RTC_BBAT_CHG_VOLTS,
	REG_RTC_BBAT_CHG_VOLTS_MIN,
	REG_RTC_BBAT_CHG_VOLTS_MAX,
	REG_RTC_BBAT_VOLTS
};

static int rpi_rtc_get(struct udevice *dev, struct rtc_time *time)
{
	u32 now = 0;

	int ret = bcm2712_rtc_read_register(REG_RTC_TIME, &now);
	if (ret != 0) return ret;

	rtc_to_tm(now, time);
	return 0;
}

static int rpi_rtc_set(struct udevice *dev, const struct rtc_time *time)
{
	return bcm2712_rtc_write_register(REG_RTC_TIME, rtc_mktime(time));
}

static const struct rtc_ops rpi_rtc_ops = {
	.get = rpi_rtc_get,
	.set = rpi_rtc_set,
};

static int rpi_rtc_probe(struct udevice *dev)
{
	int ret;

	ret = bcm2712_rtc_write_register(REG_RTC_ALARM_PENDING, 1);
	if (ret != 0)
		return ret;

	ret = bcm2712_rtc_write_register(REG_RTC_BBAT_CHG_VOLTS, 0);
	if (ret != 0)
		return ret;

	return 0;
}

static const struct udevice_id rpi_rtc_of_match[] = {
	{ .compatible = "raspberrypi,rpi-rtc" },
	{ }
};

U_BOOT_DRIVER(rtc_rpi) = {
	.name       = "rtc_rpi",
	.id         = UCLASS_RTC,
	.probe      = rpi_rtc_probe,
	.ops        = &rpi_rtc_ops,
	.of_match   = rpi_rtc_of_match,
};
