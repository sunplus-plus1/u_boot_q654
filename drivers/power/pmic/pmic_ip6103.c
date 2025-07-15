/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <log.h>
#include <power/pmic.h>
#include <power/regulator.h>

#define IP6103_LDO_DRIVER	"ip6103_ldo"
#define IP6103_DCDC_DRIVER	"ip6103_dcdc"
#define IP6103_BUCK_DRIVER	"ip6103_buck"

static const struct pmic_child_info pmic_children_info[] = {
	{ .prefix = "ldo", .driver = IP6103_LDO_DRIVER },
	{ .prefix = "dcdc", .driver = IP6103_DCDC_DRIVER },
	{ .prefix = "buck", .driver = IP6103_BUCK_DRIVER },
	{ },
};

static int ip6103_reg_count(struct udevice *dev)
{
	return 0xFF;
}

static int ip6103_write(struct udevice *dev, uint reg, const uint8_t *buff,
			  int len)
{
	if (dm_i2c_write(dev, reg, buff, len)) {
		pr_err("write error to device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int ip6103_read(struct udevice *dev, uint reg, uint8_t *buff, int len)
{
	if (dm_i2c_read(dev, reg, buff, len)) {
		pr_err("read error from device: %p register: %#x!\n", dev, reg);
		return -EIO;
	}

	return 0;
}

static int ip6103_bind(struct udevice *dev)
{
	ofnode regulators_node;
	int children;

	regulators_node = dev_read_subnode(dev, "regulators");
	if (!ofnode_valid(regulators_node)) {
		debug("%s: %s regulators subnode not found!\n", __func__,
		      dev->name);
		return -ENXIO;
	}

	debug("%s: '%s' - found regulators subnode\n", __func__, dev->name);

	children = pmic_bind_children(dev, regulators_node, pmic_children_info);
	if (!children)
		debug("%s: %s - no child found\n", __func__, dev->name);

	return 0;
}

static struct dm_pmic_ops ip6103_ops = {
	.reg_count = ip6103_reg_count,
	.read = ip6103_read,
	.write = ip6103_write,
};

static const struct udevice_id ip6103_ids[] = {
	{ .compatible = "ip6103" },
	{ }
};

U_BOOT_DRIVER(pmic_ip6103) = {
	.name = "ip6103_pmic",
	.id = UCLASS_PMIC,
	.of_match = ip6103_ids,
	.bind = ip6103_bind,
	.ops = &ip6103_ops,
};
