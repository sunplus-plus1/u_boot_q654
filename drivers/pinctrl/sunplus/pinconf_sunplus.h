/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PINCONF_SUNPLUS_H
#define __PINCONF_SUNPLUS_H

#define PIN_CONFIG_PLACEHOLDER (PIN_CONFIG_END)
#define PIN_CONFIG_INPUT_INVERT (PIN_CONFIG_END + 1)
#define PIN_CONFIG_OUTPUT_INVERT (PIN_CONFIG_END + 2)
#define PIN_CONFIG_SLEW_RATE_CTRL (PIN_CONFIG_END + 3)
#define PIN_CONFIG_BIAS_STRONG_PULL_UP (PIN_CONFIG_END + 4)
#define PIN_CONFIG_AO_INT_CTRL (PIN_CONFIG_END + 5)
#define PIN_CONFIG_AO_INT_DEBOUNCE_CTRL (PIN_CONFIG_END + 6)
#define PIN_CONFIG_AO_INT_TRIG_MODE (PIN_CONFIG_END + 7)
#define PIN_CONFIG_AO_INT_TRIG_POL (PIN_CONFIG_END + 8)
#define PIN_CONFIG_AO_INT_MASK (PIN_CONFIG_END + 9)


int sunplus_pinconf_set(struct udevice *dev, unsigned int offset,
			unsigned int param, unsigned int argument);

int sunplus_pinconf_group_set(struct udevice *dev, unsigned int group,
			      unsigned int param, unsigned int argument);

#endif /* __PINCONF_SUNPLUS_H */
