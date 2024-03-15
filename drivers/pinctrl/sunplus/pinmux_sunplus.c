// SPDX-License-Identifier: GPL-2.0

#include <asm/global_data.h>
#include <dm/device.h>
#include <dm/pinctrl.h>
#include <linux/errno.h>
#include "pinctrl_gpio_ops.h"
#include "pinctrl_sunplus.h"
#include "sp7350_func_group.h"

#define PINMUX_INITIAL_CODE 0xFFFFFFFF

static unsigned int pin_mux_record[106];

int sunplus_pinmux_pin_set(struct udevice *dev, unsigned int pin_selector,
			   unsigned int func_selector)
{
	struct grp2fp_map_t *group_map;
	int pin_group_associated;
	struct func_t *func;
	unsigned int pin;
	int ret = 0;

	func = sunplus_get_function_by_selector(func_selector);
	group_map = sunplus_get_group_by_selector(pin_selector, func_selector);
	pin_group_associated = sunplus_pin_function_association_query(
		pin_selector, func_selector);
	//pctl_info("[%s:%d]pin_selector:%d;func_selector:%d\n", __FUNCTION__,
	//__LINE__, pin_selector, func_selector);

	if (!strcmp(func->name, "GPIO")) {
		pin = pin_selector;

		pctl_info("[%s:%d]set function[%s] on pin[%d]\n", __func__,
			  __LINE__, func->name, pin);
		sunplus_gpio_mode_set(dev, pin, 1);
		sunplus_gpio_master_set(dev, pin, 1);
		pin_mux_record[pin] = func_selector;
	} else {
		if (!pin_group_associated) {
			pin = pin_selector;

			pctl_info("[%s:%d]set function[%s] on pin[%d]\n",
				  __func__, __LINE__, func->name, pin);
			//pctl_info("[%s:%d]group_map.f_idx=%d, group_map.g_idx:%d\n",
			//__FUNCTION__, __LINE__, group_map->f_idx, group_map->g_idx);

			sunplus_gpio_mode_set(dev, pin, 0);
			sunplus_pinmux_set(dev, func->roff, func->boff,
					   func->blen,
					   func->grps[group_map->g_idx].gval);
			struct groupSettingExt_t *extSetting = func->grps[group_map->g_idx].extSetting;

			if (extSetting) {
				sunplus_pinmux_set(dev, extSetting->roff, extSetting->boff,
				       extSetting->blen, extSetting->bval);
			}
			pin_mux_record[pin] = func_selector;
		} else {
			pctl_err("invalid pin[%d] for function \"%s\"\n",
				 pin_selector, func->name);
		}
	}

	return ret;
}

int sunplus_pinmux_group_set(struct udevice *dev, unsigned int group_selector,
			     unsigned int func_selector)
{
	struct grp2fp_map_t *group_map;
	struct func_t *owner_func;
	int pin_group_associated;
	struct func_t *func;
	unsigned int pin;
	int ret = 0;
	int i;

	func = sunplus_get_function_by_selector(func_selector);
	group_map =
		sunplus_get_group_by_selector(group_selector, func_selector);
	//pctl_info("[%s:%d]group_selector:%d;func_selector:%d\n",
	//__FUNCTION__, __LINE__, group_selector, func_selector);

	pin_group_associated = sunplus_group_function_association_query(
		group_selector, func_selector);

	if (!strcmp(func->name, "GPIO")) {
		owner_func = sunplus_get_function_by_selector(group_map->f_idx);
		for (i = 0; i < owner_func->grps[group_map->g_idx].pnum; i++) {
			pin = owner_func->grps[group_map->g_idx].pins[i];
			pctl_info("[%s:%d]set function[%s][%s] on pin[%d]\n",
				  __func__, __LINE__, func->name,
				  owner_func->name, pin);
			//pctl_info("[%s:%d]group_map.f_idx=%d, group_map.g_idx:%d\n",
			//__FUNCTION__, __LINE__, group_map->f_idx, group_map->g_idx);
			sunplus_gpio_mode_set(dev, pin, 1);
			sunplus_gpio_master_set(dev, pin, 1);
			pin_mux_record[pin] = func_selector;
		}
	} else if (!pin_group_associated) {
		for (i = 0; i < func->grps[group_map->g_idx].pnum; i++) {
			pin = func->grps[group_map->g_idx].pins[i];
			pctl_info("[%s:%d]set function[%s] on pin[%d]\n",
				  __func__, __LINE__, func->name, pin);
			//pctl_info("[%s:%d]group_map.f_idx=%d, group_map.g_idx:%d\n",
			//__FUNCTION__, __LINE__, group_map->f_idx, group_map->g_idx);
			sunplus_gpio_mode_set(dev, pin, 0);
			pin_mux_record[pin] = func_selector;
		}

		sunplus_pinmux_set(dev, func->roff, func->boff,
					   func->blen,
					   func->grps[group_map->g_idx].gval);
		struct groupSettingExt_t *extSetting = func->grps[group_map->g_idx].extSetting;

		if (extSetting) {
			sunplus_pinmux_set(dev, extSetting->roff, extSetting->boff,
			       extSetting->blen, extSetting->bval);
		}
	} else {
		pctl_err("invalid group \"%s\" for function \"%s\"\n",
			 sunplus_get_group_name_by_selector(group_selector),
			 func->name);
	}

	return ret;
}

const char *sunplus_pinmux_get_pin_muxing_name(struct udevice *dev,
					       unsigned int pin_selector)
{
	unsigned int fun_selector;
	struct func_t *func;

	fun_selector = pin_mux_record[pin_selector];
	if (fun_selector == PINMUX_INITIAL_CODE)
		return "UNCLAIMED";

	func = sunplus_get_function_by_selector(fun_selector);
	if (!func)
		return NULL;

	return func->name;
}

int sunplus_pinmux_resource_init(struct udevice *dev)
{
	memset(pin_mux_record, PINMUX_INITIAL_CODE, sizeof(pin_mux_record));
	group_groups(dev);

	return 0;
}
