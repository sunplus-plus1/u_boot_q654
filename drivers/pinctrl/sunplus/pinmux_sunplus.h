/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PINMUX_SUNPLUS_H
#define __PINMUX_SUNPLUS_H

int sunplus_pinmux_pin_set(struct udevice *dev, unsigned int pin_selector,
			   unsigned int func_selector);
int sunplus_pinmux_group_set(struct udevice *dev, unsigned int group_selector,
			     unsigned int func_selector);
const char *sunplus_pinmux_get_pin_muxing_name(struct udevice *dev,
					       unsigned int pin_selector);

int sunplus_pinmux_resource_init(struct udevice *dev);

#endif /* __PINMUX_SUNPLUS_H */

