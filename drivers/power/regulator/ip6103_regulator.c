/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <common.h>
#include <fdtdec.h>
#include <errno.h>
#include <dm.h>
#include <i2c.h>
#include <power/pmic.h>
#include <power/regulator.h>

enum ip6103_ldo_regulators {
	IP6103_LDO0 = 0,
	IP6103_LDO1,
	IP6103_LDO2,
	IP6103_LDO3,
	IP6103_LDO4,
	IP6103_LDO5,
	IP6103_LDO6,

	IP6103_NUM_LDO_REGULATORS,
};

enum ip6103_dcdc_regulators {
	IP6103_DCDC0 = 0,
	IP6103_DCDC1,
	IP6103_DCDC2,
	IP6103_DCDC3,

	IP6103_NUM_DCDC_REGULATORS,
};

enum ip6103_buck_regulators {
	IP6103_BUCK0 = 0,
	IP6103_BUCK1,
	IP6103_BUCK2,

	IP6103_NUM_BUCK_REGULATORS,
};

/* IP6103 regulator ldox voltage, uV */
#define IP6103_LDO_DRIVER      "ip6103_ldo"
#define IP6103_LDO_ENABLE_REG_CNFG_LDO 0x41
#define IP6103_LDO_VSEL_REG_CNFG_LDO0 0x42
#define IP6103_LDO_VSEL_REG_CNFG_LDO_MASK 0x7F
#define IP6103_REGULATOR_LDO_VOLTAGE_MIN                700000
#define IP6103_REGULATOR_LDO_VOLTAGE_STEP               25000

/* IP6103 regulator DCDC0~3 voltage, uV */
#define IP6103_DCDC_DRIVER      "ip6103_dcdc"
#define IP6103_DCDC_ENABLE_REG_CNFG_DCDC 0x20
#define IP6103_DCDC_VSEL_REG_CNFG_DCDC0 0x21
#define IP6103_DCDC_VSEL_REG_CNFG_DCDC_MASK 0x7F
#define IP6103_REGULATOR_DCDC_VOLTAGE_MIN                600000
#define IP6103_REGULATOR_DCDC3_VOLTAGE_MIN                2200000
#define IP6103_REGULATOR_DCDC_VOLTAGE_STEP               12500

/* IP6103 regulator buck0~2 enable reg config */
#define IP6103_BUCK_DRIVER      "ip6103_buck"
#define IP6103_BUCK_ENABLE_REG_CNFG_BUCK2 0x82
#define IP6103_BUCK_ENABLE_REG_CNFG_BUCK1 0x83
#define IP6103_BUCK_ENABLE_REG_CNFG_BUCK0 0x83
#define IP6103_ENABLE_REG_CNFG_MASK_BUCK0 0x45
#define IP6103_ENABLE_REG_CNFG_MASK_BUCK1 0x68
#define IP6103_ENABLE_REG_CNFG_MASK_BUCK2 0x1D
#define IP6103_ENABLE_REG_ENABLE_VAL_BUCK0 0x05
#define IP6103_ENABLE_REG_ENABLE_VAL_BUCK1 0x28
#define IP6103_ENABLE_REG_ENABLE_VAL_BUCK2 0x05
#define IP6103_ENABLE_REG_DISABLE_VAL_BUCK0 0x04
#define IP6103_ENABLE_REG_DISABLE_VAL_BUCK1 0x20
#define IP6103_ENABLE_REG_DISABLE_VAL_BUCK2 0x04

static int ip6103_ldo_volt2hex(int ldo, int uV)
{
	int hex = 0;

	/* hex = (uV - 700000) / 25000; */
	hex = (uV - IP6103_REGULATOR_LDO_VOLTAGE_MIN) / IP6103_REGULATOR_LDO_VOLTAGE_STEP;

	if (hex >= 0 && hex <= IP6103_LDO_VSEL_REG_CNFG_LDO_MASK)
		return hex;

	pr_err("Value: %d uV is wrong for LDO%d", uV, ldo);
	return -EINVAL;
}

static int ip6103_ldo_hex2volt(int ldo, int hex)
{
	unsigned int uV = 0;

	if (hex > IP6103_LDO_VSEL_REG_CNFG_LDO_MASK) {
		pr_err("Value: %#x is wrong for ldo%d", hex, ldo);
		return -EINVAL;
	}

	/* uV = hex * 25000 + 700000; */
	uV = hex * IP6103_REGULATOR_LDO_VOLTAGE_STEP + IP6103_REGULATOR_LDO_VOLTAGE_MIN;

	return uV;
}

static int ip6103_ldo_val(struct udevice *dev, int op, int *uV)
{
	unsigned int adr;
	unsigned char val;
	int hex, ldo, ret;

	if (op == PMIC_OP_GET)
		*uV = 0;

	ldo = dev->driver_data;
	if (ldo < 0 || ldo > IP6103_NUM_LDO_REGULATORS - 1) {
		pr_err("Wrong ldo number: %d", ldo);
		return -EINVAL;
	}

	adr = IP6103_LDO_VSEL_REG_CNFG_LDO0 + ldo * 2;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= IP6103_LDO_VSEL_REG_CNFG_LDO_MASK;
		ret = ip6103_ldo_hex2volt(ldo, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}

	hex = ip6103_ldo_volt2hex(ldo, *uV);
	if (hex < 0)
		return hex;

	val &= ~IP6103_LDO_VSEL_REG_CNFG_LDO_MASK;
	val |= hex;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int ip6103_ldo_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_plat *uc_pdata;

	uc_pdata = dev_get_uclass_plat(dev);

	uc_pdata->type = REGULATOR_TYPE_LDO;

	return 0;
}

static int ldo_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = ip6103_ldo_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int ldo_set_value(struct udevice *dev, int uV)
{
	return ip6103_ldo_val(dev, PMIC_OP_SET, &uV);
}

static int ldo_get_enable(struct udevice *dev)
{
	int ret, ldo;
	unsigned int adr;
	unsigned char val;

	ldo = dev->driver_data;
	if (ldo < 0 || ldo > IP6103_NUM_LDO_REGULATORS - 1) {
		pr_err("Wrong ldo number: %d", ldo);
		return -EINVAL;
	}

	adr = IP6103_LDO_ENABLE_REG_CNFG_LDO;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	return val & BIT(ldo) ? true : false;

}

static int ldo_set_enable(struct udevice *dev, bool enable)
{
	int ldo;
	unsigned int adr;

	ldo = dev->driver_data;
	if (ldo < 0 || ldo > IP6103_NUM_LDO_REGULATORS - 1) {
		pr_err("Wrong ldo number: %d", ldo);
		return -EINVAL;
	}

	adr = IP6103_LDO_ENABLE_REG_CNFG_LDO;

	return pmic_clrsetbits(dev->parent, adr, BIT(ldo), enable ? BIT(ldo) : 0);
}

static const struct dm_regulator_ops ip6103_ldo_ops = {
	.get_value  = ldo_get_value,
	.set_value  = ldo_set_value,
	.get_enable = ldo_get_enable,
	.set_enable = ldo_set_enable,
};

U_BOOT_DRIVER(ip6103_ldo) = {
	.name = IP6103_LDO_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &ip6103_ldo_ops,
	.probe = ip6103_ldo_probe,
};

static int ip6103_dcdc_volt2hex(int dcdc, int uV)
{
	int hex = 0;

	/* hex = (uV - 600000) / 12500; */
	hex = (uV - IP6103_REGULATOR_DCDC_VOLTAGE_MIN) / IP6103_REGULATOR_DCDC_VOLTAGE_STEP;

	if (IP6103_DCDC3 == dcdc)
		hex = (uV - IP6103_REGULATOR_DCDC3_VOLTAGE_MIN) / IP6103_REGULATOR_DCDC_VOLTAGE_STEP;
	if (hex >= 0 && hex <= IP6103_DCDC_VSEL_REG_CNFG_DCDC_MASK)
		return hex;

	pr_err("Value: %d uV is wrong for DCDC%d", uV, dcdc);
	return -EINVAL;
}

static int ip6103_dcdc_hex2volt(int dcdc, int hex)
{
	unsigned int uV = 0;

	if (hex > IP6103_DCDC_VSEL_REG_CNFG_DCDC_MASK) {
		pr_err("Value: %#x is wrong for dcdc%d", hex, dcdc);
		return -EINVAL;
	}

	/* uV = hex * 12500 + 600000; */
	uV = hex * IP6103_REGULATOR_DCDC_VOLTAGE_STEP + IP6103_REGULATOR_DCDC_VOLTAGE_MIN;
	if (IP6103_DCDC3 == dcdc)
		uV = hex * IP6103_REGULATOR_DCDC_VOLTAGE_STEP + IP6103_REGULATOR_DCDC3_VOLTAGE_MIN;
	return uV;
}

static int ip6103_dcdc_val(struct udevice *dev, int op, int *uV)
{
	unsigned int adr;
	unsigned char val;
	int hex, dcdc, ret;

	if (op == PMIC_OP_GET)
		*uV = 0;

	dcdc = dev->driver_data;
	if (dcdc < 0 || dcdc > IP6103_NUM_DCDC_REGULATORS - 1) {
		pr_err("Wrong dcdc number: %d", dcdc);
		return -EINVAL;
	}

	adr = IP6103_DCDC_VSEL_REG_CNFG_DCDC0 + dcdc * 7;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	if (op == PMIC_OP_GET) {
		val &= IP6103_DCDC_VSEL_REG_CNFG_DCDC_MASK;
		ret = ip6103_dcdc_hex2volt(dcdc, val);
		if (ret < 0)
			return ret;
		*uV = ret;
		return 0;
	}

	hex = ip6103_dcdc_volt2hex(dcdc, *uV);
	if (hex < 0)
		return hex;

	val &= ~IP6103_DCDC_VSEL_REG_CNFG_DCDC_MASK;
	val |= hex;
	ret = pmic_write(dev->parent, adr, &val, 1);

	return ret;
}

static int dcdc_get_value(struct udevice *dev)
{
	int uV;
	int ret;

	ret = ip6103_dcdc_val(dev, PMIC_OP_GET, &uV);
	if (ret)
		return ret;

	return uV;
}

static int dcdc_set_value(struct udevice *dev, int uV)
{
	return ip6103_dcdc_val(dev, PMIC_OP_SET, &uV);
}

static int dcdc_get_enable(struct udevice *dev)
{
	int ret, dcdc;
	unsigned int adr;
	unsigned char val;

	dcdc = dev->driver_data;
	if (dcdc < 0 || dcdc > IP6103_NUM_DCDC_REGULATORS - 1) {
		pr_err("Wrong dcdc number: %d", dcdc);
		return -EINVAL;
	}

	adr = IP6103_DCDC_ENABLE_REG_CNFG_DCDC;

	ret = pmic_read(dev->parent, adr, &val, 1);
	if (ret)
		return ret;

	return val & BIT(dcdc) ? true : false;

}

static int dcdc_set_enable(struct udevice *dev, bool enable)
{
	int dcdc;
	unsigned int adr;

	dcdc = dev->driver_data;
	if (dcdc < 0 || dcdc > IP6103_NUM_DCDC_REGULATORS - 1) {
		pr_err("Wrong dcdc number: %d", dcdc);
		return -EINVAL;
	}

	adr = IP6103_DCDC_ENABLE_REG_CNFG_DCDC;

	return pmic_clrsetbits(dev->parent, adr, BIT(dcdc), enable ? BIT(dcdc) : 0);
}


static const struct dm_regulator_ops ip6103_dcdc_ops = {
	.get_value  = dcdc_get_value,
	.set_value  = dcdc_set_value,
	.get_enable = dcdc_get_enable,
	.set_enable = dcdc_set_enable,
};

static int ip6103_dcdc_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_plat *uc_pdata;

	uc_pdata = dev_get_uclass_plat(dev);

	uc_pdata->type = REGULATOR_TYPE_LDO;

	return 0;
}

U_BOOT_DRIVER(ip6103_dcdc) = {
	.name = IP6103_DCDC_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &ip6103_dcdc_ops,
	.probe = ip6103_dcdc_probe,
};

static int buck_get_enable(struct udevice *dev)
{
	int ret, buck;
	unsigned int adr, mask;
	unsigned char cur_val, enable_val;

	buck = dev->driver_data;
	if (buck < 0 || buck > IP6103_NUM_BUCK_REGULATORS - 1) {
		pr_err("Wrong buck number: %d", buck);
		return -EINVAL;
	}

	switch(buck) {
		case IP6103_BUCK0:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK0;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK0;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK0;
			break;
		case IP6103_BUCK1:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK1;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK1;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK1;
			break;
		case IP6103_BUCK2:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK2;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK2;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK2;
			break;
		default:
			printf("Unknown buck num: %d\n", buck);
			return -EINVAL;
	}

	ret = pmic_read(dev->parent, adr, &cur_val, 1);
	if (ret)
		return ret;

	cur_val &= mask;
	return cur_val == enable_val;
}

static int buck_set_enable(struct udevice *dev, bool enable)
{
	int buck;
	unsigned int adr, mask;
	unsigned char enable_val, disable_val;

	buck = dev->driver_data;
	if (buck < 0 || buck > IP6103_NUM_BUCK_REGULATORS - 1) {
		pr_err("Wrong buck number: %d", buck);
		return -EINVAL;
	}
	switch(buck) {
		case IP6103_BUCK0:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK0;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK0;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK0;
			disable_val = IP6103_ENABLE_REG_DISABLE_VAL_BUCK0;
			break;
		case IP6103_BUCK1:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK1;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK1;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK1;
			disable_val = IP6103_ENABLE_REG_DISABLE_VAL_BUCK1;
			break;
		case IP6103_BUCK2:
			adr = IP6103_BUCK_ENABLE_REG_CNFG_BUCK2;
			mask = IP6103_ENABLE_REG_CNFG_MASK_BUCK2;
			enable_val = IP6103_ENABLE_REG_ENABLE_VAL_BUCK2;
			disable_val = IP6103_ENABLE_REG_DISABLE_VAL_BUCK2;
			break;
		default:
			printf("Unknown buck num: %d\n", buck);
			return -EINVAL;
	}

	return pmic_clrsetbits(dev->parent, adr, mask, enable ? enable_val : disable_val);
}

static const struct dm_regulator_ops ip6103_buck_ops = {
	.get_enable = buck_get_enable,
	.set_enable = buck_set_enable,
};

static int ip6103_buck_probe(struct udevice *dev)
{
	struct dm_regulator_uclass_plat *uc_pdata;

	uc_pdata = dev_get_uclass_plat(dev);

	uc_pdata->type = REGULATOR_TYPE_FIXED;

	return 0;
}

U_BOOT_DRIVER(ip6103_buck) = {
	.name = IP6103_BUCK_DRIVER,
	.id = UCLASS_REGULATOR,
	.ops = &ip6103_buck_ops,
	.probe = ip6103_buck_probe,
};
