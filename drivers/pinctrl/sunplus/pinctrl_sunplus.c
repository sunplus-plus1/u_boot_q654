#include <common.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/pinctrl.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>

#include "pinctrl_sunplus.h"
#include <dt-bindings/pinctrl/sppctl-sp7350.h>

DECLARE_GLOBAL_DATA_PTR;

//#define PINCTRL_DEBUG

volatile u32 *moon1_regs = NULL;
volatile u32 *gpioxt_regs = NULL;
volatile u32 *first_regs = NULL;
void* pin_registered_by_udev[MAX_PINS];

#ifdef PINCTRL_DEBUG
void pinmux_grps_dump(void)
{
	int i = 0, mask, rval, val;

	struct func_t *func = &list_funcs[i];
	for (i = 0; i < list_funcsSZ; i++) {
		func = &(list_funcs[i]);

		if (func->gnum == 0)
			continue;

		if (func->freg != fOFF_G)
			continue;

		mask = (1 << func->blen) - 1;
		rval = GPIO_PINGRP(func->roff);
		val = (rval >> func->boff) & mask;
		if (val == 0)
			continue;

		printf("%s\t=%d regval:%X\n", list_funcs[i].name, val, rval);
	}
}

void gpio_reg_dump(void)
{
	u32 gpio_value[MAX_PINS];
	int i;

	printf("gpio_reg_dump (FI MA IN II OU OI OE OD)\n");
	for (i = 0; i < MAX_PINS; i++) {
		gpio_value[i] = sp_gpio_para_get(i);
		if ((i%8) == 7) {
			printf("GPIO_P%-2d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",(i/8),
			gpio_value[i-7],gpio_value[i-6],gpio_value[i-5],gpio_value[i-4],
			gpio_value[i-3],gpio_value[i-2],gpio_value[i-1],gpio_value[i]);
		} else if (i == (MAX_PINS-1)) {
			printf("GPIO_P%-2d 0x%02x 0x%02x 0x%02x 0x%02x\n",(i/8),
			gpio_value[i-3],gpio_value[i-2],gpio_value[i-1],gpio_value[i]);
		}
	}
}
#endif

static void* sp_register_pin(int pin, struct udevice *dev)
{
	// Check if pin number is within range.
	if ((pin >= 0) && (pin < MAX_PINS)) {
		if (pin_registered_by_udev[pin] == 0) {
			// If the pin has not been registered, register it by
			// save a pointer to the registering device.
			pin_registered_by_udev[pin] = dev;
			pctl_info("Register pin %d (from node: %s).\n", pin, dev->name);

			return 0;
		} else {
			pctl_err("ERROR: Pin %d of node %s has been registered (by node: %s)!\n",
				 pin, dev->name, ((struct udevice*)pin_registered_by_udev[pin])->name);
			return dev;
		}
	} else {
		pctl_err("ERROR: Invalid pin number %d from '%s'!\n", pin, dev->name);
		return (void*)-1;
	}
}

static int sp_unregister_pin(int pin)
{
	// Check if pin number is within range.
	if ((pin >= 0) && (pin < MAX_PINS))
		if (pin_registered_by_udev[pin]) {
			pin_registered_by_udev[pin] = 0;
			return 0;
		}

	return -1;
}

static int sp_pinctrl_pins(struct udevice *dev)
{
	int offset = dev_of_offset(dev);
	u32 pin_mux[MAX_PINS];
	int len, i;

	// Get property: "sunplus,pins"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "sunplus,pins",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len > 0)
		goto found_pins;

	// Get property: "sppctl,pins"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "sppctl,pins",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len > 0)
		goto found_pins;

	// Get property: "pins"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "pins",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len <= 0)
		return 0;

found_pins:
	pctl_info("Number of entries of 'pins' = %d\n", len);

	// Register all pins.
	for (i = 0; i < len; i++) {
		int pin  = (pin_mux[i] >> 24) & 0xff;

		if (sp_register_pin(pin, dev) != 0)
			break;
	}

	// If any pin was not registered successfully, return -1.
	if ((len > 0) && (i != len))
		return -1;

	// All pins were registered successfully, set up all pins.
	for (i = 0; i < len; i++) {
		int pins = pin_mux[i];
		int pin  = (pins >> 24) & 0xff;
		int type = (pins >> 16) & 0xff;
		int flag = pins & 0xff;
		pctl_info("pins = 0x%08x\n", pins);

		if (type == SPPCTL_PCTL_G_IOPP) {
			// It's a IOP pin.
			sp_gpio_first_set(pin, 1);
			sp_gpio_master_set(pin, 0);
		} else if (type == SPPCTL_PCTL_G_GPIO) {
			// It's a GPIO pin.
			sp_gpio_first_set(pin, 1);
			sp_gpio_master_set(pin, 1);

			if (flag & (SPPCTL_PCTL_L_OUT|SPPCTL_PCTL_L_OU1)) {
				if (flag & SPPCTL_PCTL_L_OUT)
					sp_gpio_out_set(pin, 0);

				if (flag & SPPCTL_PCTL_L_OU1)
					sp_gpio_out_set(pin, 1);

				sp_gpio_oe_set(pin, 1);
			} else if (flag & SPPCTL_PCTL_L_ODR) {
				sp_gpio_open_drain_set(pin, 1);
			}

			if (flag & SPPCTL_PCTL_L_INV)
				sp_gpio_input_invert_set(pin, 1);
			else
				sp_gpio_input_invert_set(pin, 0);

			if (flag & SPPCTL_PCTL_L_ONV)
				sp_gpio_output_invert_set(pin, 1);
			else
				sp_gpio_output_invert_set(pin, 0);
		}
	}

	return 0;
}

static int sp_pinctrl_zero(struct udevice *dev)
{
	int offset = dev_of_offset(dev);
	u32 pin_mux[MAX_PINS];
	int len, i, mask;

	// Get property: "sunplus,zerofunc"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "sunplus,zerofunc",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len > 0)
		goto found_zero_func;

	// Get property: "sppctl,zero_func"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "sppctl,zero_func",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len > 0)
		goto found_zero_func;

	// Get property: "zero_func"
	len = fdtdec_get_int_array_count(gd->fdt_blob, offset, "zero_func",
					 pin_mux, ARRAY_SIZE(pin_mux));
	if (len <= 0)
		return 0;

found_zero_func:
	pctl_info("Number of entries of 'zero_func' = %d\n", len);

	// All pins were registered successfully, set up all pins.
	for (i = 0; i < len; i++) {
		int func = pin_mux[i];
		pctl_info("zero_func = 0x%08x\n", func);

		// Set it to no use.
		if (func >= list_funcsSZ) {
			pctl_info("func=%d is out of range, skipping...\n", func);
			continue;
		}

		struct func_t *f = &list_funcs[func];
		switch (f->freg) {
		case fOFF_G:
			mask = (1 << f->blen) - 1;
			GPIO_PINGRP(f->roff) = (mask << (f->boff+16)) | (0 << f->boff);
			pctl_info("group %s set to 0\n", f->name);
			break;

		default:
			printf("bad zero func/group idx:%d, skipped\n", func);
			break;
		}
	}

	return 0;
}

static int sp_pinctrl_function(struct udevice *dev)
{
	int offset = dev_of_offset(dev);
	const char *pin_group;
	const char *pin_func;
	int len, i;

	// Get property: 'sppctl,function'
	pin_func = fdt_getprop(gd->fdt_blob, offset, "sppctl,function", &len);
	if (pin_func)
		goto found_function;

	// Get property: 'function'
	pin_func = fdt_getprop(gd->fdt_blob, offset, "function", &len);
	if (!pin_func)
		return 0;

found_function:
	pctl_info("function = %s (%d)\n", pin_func, len);
	if (len > 1) {
		// Find 'function' string in list: only groups
		for (i = 0; i < list_funcsSZ; i++) {
			if (list_funcs[i].gnum == 0)
				continue;

			if (list_funcs[i].freg != fOFF_G)
				continue;

			if (strcmp(pin_func, list_funcs[i].name) == 0)
				break;
		}
		if (i == list_funcsSZ) {
			pctl_err("Error: Invalid 'function' in node %s! "
				 "Cannot find \"%s\"!\n", dev->name, pin_func);
			return -1;
		}

		// 'function' is found! Next, find its 'groups'.
		// Get property: 'sppctl,groups'
		pin_group = fdt_getprop(gd->fdt_blob, offset, "sppctl,groups", &len);
		if (pin_group)
			goto found_groups;

		// Get property: 'groups'
		pin_group = fdt_getprop(gd->fdt_blob, offset, "groups", &len);

found_groups:
		pctl_info("groups = %s (%d)\n", pin_group, len);
		if (len > 1) {
			struct func_t *func = &list_funcs[i];

			// Find 'pin_group' string in list.
			for (i = 0; i < func->gnum; i++)
				if (strcmp (pin_group, func->grps[i].name) == 0)
					break;

			if (i == func->gnum) {
				pctl_err("Error: Invalid 'groups' in node %s! "
					 "Cannot find \"%s\"!\n", dev->name, pin_group);
				return -1;
			}

			// 'pin_group' is found!
			const struct sppctlgrp_t *grp = &func->grps[i];

			// Register all pins of the group.
			for (i = 0; i < grp->pnum; i++)
				if (sp_register_pin(grp->pins[i], dev) != 0)
					break;

			// Check if all pins of the group register successfully
			if (i == grp->pnum) {
				int mask;

				// All pins of the group was registered successfully.
				// Enable the pin-group.
				mask = (1 << func->blen) - 1;
				GPIO_PINGRP(func->roff) = (mask<<(func->boff+16)) | (grp->gval<<func->boff);
				pctl_info("GPIO_PINGRP[%d] <= 0x%08x\n", func->roff,
					(mask<<(func->boff+16)) | (grp->gval<<func->boff));
			} else {
				return -1;
			}
		} else if (len <= 1) {
			pctl_err("Error: Invalid 'groups' in node %s!\n", dev->name);
			return -1;
		}
	} else if (len == 1) {
		pctl_err("Error: Invalid 'function' in node %s!\n", dev->name);
		return -1;
	}

	return 0;
}

static int sp_pinctrl_set_state(struct udevice *dev, struct udevice *config)
{
	int ret;

	pctl_info("\nConfig node: %s\n", config->name);
	ret = sp_pinctrl_pins(config);
	if (ret != 0)
		return ret;

	sp_pinctrl_zero(config);

	ret = sp_pinctrl_function(config);
	if (ret != 0)
		return ret;

#ifdef PINCTRL_DEBUG
	gpio_reg_dump();
	pinmux_grps_dump();
#endif

	return 0;
}

static struct pinctrl_ops sunplus_pinctrl_ops = {
	.set_state = sp_pinctrl_set_state,
};

static int sp_gpio_request(struct udevice *dev, unsigned int offset, const char *label)
{
	int err;

	pctl_info("%s: offset = %u, label = %s\n", __func__, offset, label);

	err = sp_gpio_first_set(offset, 1);
	if (err)
		return err;

	err = sp_gpio_master_set(offset, 1);
	if (err)
		return err;

	return 0;
}

static int sp_gpio_rfree(struct udevice *dev, unsigned int offset)
{
	pctl_info("%s: offset = %u\n", __func__, offset);

	//sp_gpio_first_set(offset, 0);
	//sp_gpio_master_set(offset, 0);

	return 0;
}

static int sp_gpio_get_value(struct udevice *dev, unsigned int offset)
{
	u32 val;
	int err;

	pctl_info("%s: offset = %u\n", __func__, offset);

	err = sp_gpio_in(offset, &val);
	if (err)
		return err;

	return !!val;
}

static int sp_gpio_set_value(struct udevice *dev, unsigned int offset, int val)
{
	pctl_info("%s: offset = %u, val = %d\n", __func__, offset, val);

	return sp_gpio_out_set(offset, !!val);
}

static int sp_gpio_get_function(struct udevice *dev, unsigned int offset)
{
	u32 val;
	int err;

	pctl_info("%s: offset = %u\n", __func__, offset);

	err = sp_gpio_oe_get(offset, &val);
	if (err)
		return err;

	return val ? GPIOF_OUTPUT : GPIOF_INPUT;
}

static int sp_gpio_direction_input(struct udevice *dev, unsigned int offset)
{
	pctl_info("%s: offset = %u\n", __func__, offset);

	return sp_gpio_oe_set(offset, 0);
}

static int sp_gpio_direction_output(struct udevice *dev, unsigned int offset, int val)
{
	pctl_info("%s: offset = %u, val = %d\n", __func__, offset, val);

	sp_gpio_out_set(offset, val);

	return sp_gpio_oe_set(offset, 1);
}

static int sp_gpio_probe(struct udevice *dev)
{
	struct gpio_dev_priv *uc_priv;

	uc_priv = dev_get_uclass_priv(dev);
	uc_priv->bank_name = "GPIO";
	uc_priv->gpio_count = MAX_PINS;

	return 0;
}

static const struct dm_gpio_ops sp_gpio_ops = {
	.request = sp_gpio_request,
	.rfree = sp_gpio_rfree,
	.set_value = sp_gpio_set_value,
	.get_value = sp_gpio_get_value,
	.get_function = sp_gpio_get_function,
	.direction_input = sp_gpio_direction_input,
	.direction_output = sp_gpio_direction_output,
};

static struct driver sp_gpio_driver = {
	.name  = "sunplus_gpio",
	.id    = UCLASS_GPIO,
	.probe = sp_gpio_probe,
	.ops   = &sp_gpio_ops,
};

static int sunplus_pinctrl_probe(struct udevice *dev)
{
	struct udevice *cdev;
	ofnode node;
	int i;

	// Get address of GPIOXT registers.
	gpioxt_regs = (void*)devfdt_get_addr_name(dev, "gpioxt");
	pctl_info("gpioxt_regs = %px\n", gpioxt_regs);
	if (gpioxt_regs == (void*)-1) {
		pctl_err("Failed to get base address of GPIOXT!\n");
		return -EINVAL;
	}

	// Get address of FIRST registers.
	first_regs = (void*)devfdt_get_addr_name(dev, "first");
	pctl_info("first_regs = %px\n", first_regs);
	if (first_regs == (void*)-1) {
		pctl_err("Failed to get base address of FIRST!\n");
		return -EINVAL;
	}

	// Get address of MOON1 registers.
	moon1_regs = (void*)devfdt_get_addr_name(dev, "moon1");
	pctl_info("moon1_regs = %px\n", moon1_regs);
	if (moon1_regs == (void*)-1) {
		pctl_err("Failed to get base address of MOON1!\n");
		return -EINVAL;
	}

	// Unregister all pins.
	for (i = 0; i < MAX_PINS; i++)
		sp_unregister_pin(i);

	// Bind gpio driver.
	node = dev_ofnode(dev);
	if (ofnode_read_bool(node, "gpio-controller")) {
		if (!lists_uclass_lookup(UCLASS_GPIO)) {
			pctl_err("Cannot find GPIO driver\n");
			return -ENOENT;
		}

		return device_bind(dev, &sp_gpio_driver, "sunplus_gpio",
				   0, node, &cdev);
	}

	return 0;
}

static const struct udevice_id sunplus_pinctrl_ids[] = {
	{ .compatible = "sunplus,sp7350-pctl" },
	{ /* zero */ }
};

U_BOOT_DRIVER(pinctrl_sunplus) = {
	.name     = "sunplus_pinctrl",
	.id       = UCLASS_PINCTRL,
	.probe    = sunplus_pinctrl_probe,
	.of_match = sunplus_pinctrl_ids,
	.ops      = &sunplus_pinctrl_ops,
	.bind     = dm_scan_fdt_dev,
};
