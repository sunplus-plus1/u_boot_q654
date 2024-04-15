// SPDX-License-Identifier: GPL-2.0+
/*
 *
 * DWC3 Phy driver
 *
 */

#include <dm.h>

static const struct udevice_id sunplus_dwc3phy_ids[] = {
	{ .compatible = "sunplus,usb3-phy" },
	{ }
};

U_BOOT_DRIVER(sunplus_dwc3_phy) = {
	.name	= "phy-sunplus-dwc3",
	.id	= UCLASS_PHY,
	.of_match = sunplus_dwc3phy_ids,
};
