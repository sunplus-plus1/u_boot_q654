/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PINCONF_GPIO_COMMON_OPS_SUNPLUS_H
#define __PINCONF_GPIO_COMMON_OPS_SUNPLUS_H

#include <asm/global_data.h>
#include <dm/device.h>

/**
 * enum vol_ms_group - the groups of voltage mode select for DVIO.
 * @G_MX_MS_TOP_0 includes pins from G_MX21 to G_MX27.
 * @G_MX_MS_TOP_1 includes pins G_MX20, and those from G_MX28 to G_MX37.
 * @AO_MX_MS_TOP_0 includes pins from A0_MX0 to AO_MX9.
 * @AO_MX_MS_TOP_1 includes pins from A0_MX10 to AO_MX19.
 * @AO_MX_MS_TOP_2 includes pins from A0_MX20 to AO_MX29.
 */

enum vol_ms_group {
	G_MX_MS_TOP_0,
	G_MX_MS_TOP_1,
	AO_MX_MS_TOP_0,
	AO_MX_MS_TOP_1,
	AO_MX_MS_TOP_2,
};

int sunplus_pinmux_set(struct udevice *dev, u8 reg_offset, u8 bit_offset,
		       u8 bit_nums, u8 reg_value);
int sunplus_pinmux_get(struct udevice *dev, u8 reg_offset, u8 bit_offset,
		       u8 bit_nums);

void sunplus_reg_base_addr_set(void *moon1, void *pad_ctl_1, void *first,
			       void *pad_ctl_2, void *gpioxt,
			       void *gpio_ao_int);
#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
int sunplus_gpio_gather_gpio_ao_int(unsigned int group,
			const unsigned *const pins, unsigned int npins);
int sunplus_gpio_ao_int_debounce_ctrl_set(struct udevice *dev,
					 unsigned int selector, int value);
int sunplus_gpio_ao_int_debounce_ctrl_get(struct udevice *dev,
					 unsigned int selector);
int sunplus_gpio_ao_int_ctrl_set(struct udevice *dev, unsigned int selector,
				int value);
int sunplus_gpio_ao_int_ctrl_get(struct udevice *dev, unsigned int selector);
int sunplus_gpio_ao_int_trigger_mode_set(struct udevice *dev,
					unsigned int selector, int value);
int sunplus_gpio_ao_int_trigger_mode_get(struct udevice *dev,
					unsigned int selector);
int sunplus_gpio_ao_int_trigger_polarity_set(struct udevice *dev,
					    unsigned int selector, int value);
int sunplus_gpio_ao_int_trigger_polarity_get(struct udevice *dev,
					    unsigned int selector);
int sunplus_gpio_ao_int_mask_set(struct udevice *dev, unsigned int selector,
				int value);
int sunplus_gpio_ao_int_mask_get(struct udevice *dev, unsigned int selector);

#endif


/*
 * On return 1, Selected pin is in GPIO mode.
 * On return 0, Selected pin is not in GPIO mode.
 */
int sunplus_gpio_mode_query(struct udevice *dev, unsigned int offset);

/*
 * On Return 1, Corresponding GPIO is controlled by the corresponding
 * bit of GPIO output enable and GPIO output data registers
 * in this group(default)
 * On return 0, Corresponding GPIO is controlled by IOP register.
 *
 */
int sunplus_gpio_master_query(struct udevice *dev, unsigned int offset);

/*
 * @value:
 * 1:Enable selected pin into GPIO mode.
 * 0:Disable selected pin into GPIO mode.
 */
int sunplus_gpio_mode_set(struct udevice *dev, unsigned int offset,
			  unsigned int value);

/*
 * @value:
 * 1:Corresponding GPIO is controlled by the corresponding
 * bit of GPIO output enable and GPIO output data registers
 * in this group(default)
 * 0:Corresponding GPIO is controlled by IOP register.
 */
int sunplus_gpio_master_set(struct udevice *dev, unsigned int offset,
			    unsigned int value);

/*
 * On return 1, Corresponding GPIO input value is inverted.
 * On return 0, Corresponding GPIO input value is normal(default).
 */
int sunplus_gpio_input_invert_query(struct udevice *dev, unsigned int offset);

/*
 * On return 1, Corresponding GPIO output value is inverted.
 * On return 0, Corresponding GPIO output value is normal(default).
 */
int sunplus_gpio_output_invert_query(struct udevice *dev, unsigned int offset);

/*
 * @value:
 * 1:Invert input value of Corresponding GPIO.
 * 0:Normalize input value of Corresponding GPIO.
 */
int sunplus_gpio_input_invert_set(struct udevice *dev, unsigned int offset,
				  unsigned int value);

/*
 * @value:
 * 1:Invert output value of Corresponding GPIO.
 * 0:Normalize output value of Corresponding GPIO.
 */
int sunplus_gpio_output_invert_set(struct udevice *dev, unsigned int offset,
				   unsigned int value);

/*
 * On return 1, Corresponding GPIO is in open-drain mode.
 * On return 0, Corresponding GPIO is not in open-drain mode.
 */
int sunplus_gpio_open_drain_query(struct udevice *dev, unsigned int offset);

/*
 * @value:
 * 1:Enable open-drain mode for selected pin.
 * 0:Disable open-drain mode for selected pin.
 */
int sunplus_gpio_open_drain_set(struct udevice *dev, unsigned int offset,
				unsigned int value);

/*
 * @value:
 * 1:Enable Schmitt trigger input function(default).
 * 0:Disable Schmitt trigger input function;
 */
int sunplus_gpio_schmitt_trigger_set(struct udevice *dev, unsigned int offset,
				     unsigned int value);

/*
 * enable/disable slew rate control
 * @value
 *	0:disable
 *	1:enable
 */
int sunplus_gpio_slew_rate_control_set(struct udevice *dev, unsigned int offset,
				       unsigned int value);

/*
 * pull up the selected pin.
 */
int sunplus_gpio_pull_up(struct udevice *dev, unsigned int offset);

/*
 * pull down the selected pin.
 */
int sunplus_gpio_pull_down(struct udevice *dev, unsigned int offset);

/*
 * Strongly pull up the selected pin. For GPIO only, exclude DVIO.
 */
int sunplus_gpio_strong_pull_up(struct udevice *dev, unsigned int offset);

/*
 * High-Z;
 */
int sunplus_gpio_high_impedance(struct udevice *dev, unsigned int offset);

/*
 * Disable all bias on the selected pin.
 */
int sunplus_gpio_bias_disable(struct udevice *dev, unsigned int offset);

/*
 * Set driving strength in uA.
 * The selectable strength values are listed in sppctl-config.h
 */
int sunplus_gpio_drive_strength_set(struct udevice *dev, unsigned int offset,
				    unsigned int uA);

/*
 * Enable or disable voltage mode select
 * @value
 *	0: 3.0V
 *	1: 1.8V
 */

int sunplus_gpio_voltage_mode_select_set(struct udevice *dev,
					 enum vol_ms_group ms_group,
					 unsigned int value);

/*
 * On return 1, Corresponding GPIO direction is input.
 * On return 0, Corresponding GPIO direction is output.
 */
int sunplus_gpio_direction_query(struct udevice *dev, unsigned int offset);

/*
 * Set the selected pin direction to input.
 */
int sunplus_gpio_set_direction_input(struct udevice *dev, unsigned int offset);

/*
 * @value:
 * 1:Enable input for the selected pin. Meanwhile, output is disabled.
 * 0:Disable input for the selected pin. Meanwhile, output is enabled.
 */
int sunplus_gpio_input_enable_set(struct udevice *dev, unsigned int offset,
				  unsigned int value);

/*
 * @value:
 * 1:Enable output for the selected pin. Meanwhile, input is disabled.
 * 0:Disable output for the selected pin. Meanwhile, input is enabled.
 */
int sunplus_gpio_output_enable_set(struct udevice *dev, unsigned int offset,
				   unsigned int value);

/*
 * @value:
 * 1:Output high on the selected pin.
 * 0:Output low on the selected pin.
 */
int sunplus_gpio_output_set(struct udevice *dev, unsigned int offset,
			    unsigned int value);

/*
 * On return 1, Corresponding GPIO input value is high.
 * On return 0, Corresponding GPIO input value is low.
 */
int sunplus_gpio_input_query(struct udevice *dev, unsigned int offset);

#endif /* __PINCONF_GPIO_COMMON_OPS_SUNPLUS_H */
