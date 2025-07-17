// SPDX-License-Identifier: GPL-2.0

#include <dt-bindings/pinctrl/sppctl-config-sp7350.h>
#include <linux/io.h>

#include "pinctrl_gpio_ops.h"
#include "pinctrl_sunplus.h"

struct sppctl_reg_t {
	u16 v; // value part
	u16 m; // mask part
};

#define REG_OFFSET_CTL 0x00 /* gpioxt_regs_base */
#define REG_OFFSET_OUTPUT_ENABLE 0x34 /* gpioxt_regs_base */
#define REG_OFFSET_OUTPUT 0x68 /* gpioxt_regs_base */
#define REG_OFFSET_INPUT 0x9c /* gpioxt_regs_base */
#define REG_OFFSET_INPUT_INVERT 0xbc /* gpioxt_regs_base */
#define REG_OFFSET_OUTPUT_INVERT 0xf0 /* gpioxt_regs_base */
#define REG_OFFSET_OPEN_DRAIN 0x124 /* gpioxt_regs_base */

#define REG_OFFSET_GPIO_FIRST 0x00 /* first_regs_base */

#define REG_OFFSET_SCHMITT_TRIGGER 0x00 /* padctl1_regs_base */
#define REG_OFFSET_DS0 0x10 /* padctl1_regs_base */
#define REG_OFFSET_DS1 0x20 /* padctl1_regs_base */
#define REG_OFFSET_DS2 0x30 /* padctl1_regs_base */
#define REG_OFFSET_DS3 0x40 /* padctl1_regs_base */

#define REG_OFFSET_SLEW_RATE 0x00 /* padctl2_regs_base */
#define REG_OFFSET_PULL_ENABLE 0x08 /* padctl2_regs_base */
#define REG_OFFSET_PULL_SELECTOR 0x10 /* padctl2_regs_base */
#define REG_OFFSET_STRONG_PULL_UP 0x18 /* padctl2_regs_base */
#define REG_OFFSET_PULL_UP 0x20 /* padctl2_regs_base */
#define REG_OFFSET_PULL_DOWN 0x28 /* padctl2_regs_base */
#define REG_OFFSET_MODE_SELECT 0x30 /* padctl2_regs_base */

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
#define REG_OFFSET_PRE_SCALE 0x00 /* gpio_ao_int_regs_base */
#define REG_OFFSET_DEB_TIME 0x04 /* gpio_ao_int_regs_base */
#define REG_OFFSET_DEB_EN 0x08 /* gpio_ao_int_regs_base */
#define REG_OFFSET_INT_I 0x0c /* gpio_ao_int_regs_base */
#define REG_OFFSET_DEB_I 0x10 /* gpio_ao_int_regs_base */
#define REG_OFFSET_AO_OUTPUT 0x14 /* gpio_ao_int_regs_base */
#define REG_OFFSET_OE 0x18 /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_EN 0x1c /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_MODE 0x20 /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_POL 0x24 /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_CLR 0x28 /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_STATUS 0x2c /* gpio_ao_int_regs_base */
#define REG_OFFSET_INTR_MASK 0x30 /* gpio_ao_int_regs_base */
#endif

// (/16)*4
#define R16_ROF(r) (((r) >> 4) << 2)
#define R16_BOF(r) ((r) % 16)
// (/32)*4
#define R32_ROF(r) (((r) >> 5) << 2)
#define R32_BOF(r) ((r) % 32)
#define R32_VAL(r, boff) (((r) >> (boff)) & BIT(0))
//(/30)*4
#define R30_ROF(r) (((r) / 30) << 2)
#define R30_BOF(r) ((r) % 30)

#define IS_DVIO(pin) ((pin) >= 20 && (pin) <= 79)

static void *moon1_regs_base;
static void *padctl1_regs_base;
static void *first_regs_base;
static void *padctl2_regs_base;
static void *gpioxt_regs_base;
#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
static void *gpio_ao_int_regs_base;
u32 gpio_ao_int_prescale;
u32 gpio_ao_int_debounce;
int gpio_ao_int_pins[32];
#endif

void sunplus_reg_base_addr_set(void *moon1, void *pad_ctl_1, void *first,
			       void *pad_ctl_2, void *gpioxt, void *gpio_ao_int)
{
	moon1_regs_base = moon1;
	padctl1_regs_base = pad_ctl_1;
	first_regs_base = first;
	padctl2_regs_base = pad_ctl_2;
	gpioxt_regs_base = gpioxt;
#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	gpio_ao_int_regs_base = gpio_ao_int;
#endif
}

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
int sunplus_gpio_gather_gpio_ao_int(unsigned int group,
				    const unsigned *const pins, unsigned int npins)
{
	int i;

	if (group == 284 || group == 285) { // GPIO_AO_0_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i] = pins[i];
	} else if (group == 286 || group == 287) { // GPIO_AO_1_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 1] = pins[i];
	} else if (group == 288 || group == 289) { // GPIO_AO_2_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 2] = pins[i];
	} else if (group == 290 || group == 291) { // GPIO_AO_3_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 3] = pins[i];
	} else if (group == 292 || group == 293) { // GPIO_AO_4_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 4] = pins[i];
	} else if (group == 294 || group == 295) { // GPIO_AO_5_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 5] = pins[i];
	} else if (group == 296 || group == 297) { // GPIO_AO_6_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 6] = pins[i];
	} else if (group == 298 || group == 299) { // GPIO_AO_7_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 7] = pins[i];
	} else if (group == 300 || group == 301) { // GPIO_AO_8_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 8] = pins[i];
	} else if (group == 302 || group == 303) { // GPIO_AO_9_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 9] = pins[i];
	} else if (group == 304 || group == 305) { // GPIO_AO_10_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 10] = pins[i];
	} else if (group == 306 || group == 307) { // GPIO_AO_11_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 11] = pins[i];
	} else if (group == 308 || group == 309) { // GPIO_AO_12_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 12] = pins[i];
	} else if (group == 310 || group == 311) { // GPIO_AO_13_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 13] = pins[i];
	} else if (group == 312 || group == 313) { // GPIO_AO_14_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 14] = pins[i];
	} else if (group == 314 || group == 315) { // GPIO_AO_15_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 15] = pins[i];
	} else if (group == 316 || group == 317) { // GPIO_AO_16_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 16] = pins[i];
	} else if (group == 318 || group == 319) { // GPIO_AO_17_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 17] = pins[i];
	} else if (group == 320 || group == 321) { // GPIO_AO_18_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 18] = pins[i];
	} else if (group == 322 || group == 323) { // GPIO_AO_19_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 19] = pins[i];
	} else if (group == 324 || group == 325) { // GPIO_AO_20_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 20] = pins[i];
	} else if (group == 326 || group == 327) { // GPIO_AO_21_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 21] = pins[i];
	} else if (group == 328 || group == 329) { // GPIO_AO_22_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 22] = pins[i];
	} else if (group == 330 || group == 331) { // GPIO_AO_23_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 23] = pins[i];
	} else if (group == 332 || group == 333) { // GPIO_AO_24_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 24] = pins[i];
	} else if (group == 334 || group == 335) { // GPIO_AO_25_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 25] = pins[i];
	} else if (group == 336 || group == 337) { // GPIO_AO_26_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 26] = pins[i];
	} else if (group == 338 || group == 339) { // GPIO_AO_27_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 27] = pins[i];
	} else if (group == 340 || group == 341) { // GPIO_AO_28_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 28] = pins[i];
	} else if (group == 342 || group == 343) { // GPIO_AO_29_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 29] = pins[i];
	} else if (group == 344 || group == 345) { // GPIO_AO_30_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 30] = pins[i];
	} else if (group == 346 || group == 347) { // GPIO_AO_31_INT
		for (i = 0; i < npins; i++)
			gpio_ao_int_pins[i + 31] = pins[i];
	}

	return 0;
}

#endif


int sunplus_pinmux_set(struct udevice *dev, u8 reg_offset, u8 bit_offset,
		       u8 bit_nums, u8 bit_value)
{
	struct sppctl_reg_t x;
	u32 *r;

	x.m = (~(~0 << bit_nums)) << bit_offset;
	x.v = ((uint16_t)bit_value) << bit_offset;

	pctl_info("%s(x%X,x%X,x%X,x%X) m:x%X v:x%X\n", __func__, reg_offset,
		  bit_offset, bit_nums, bit_value, x.m, x.v);
	r = (u32 *)&x;

	writel(*r, moon1_regs_base + (reg_offset << 2));

	return 0;
}

int sunplus_pinmux_get(struct udevice *dev, u8 reg_offset, u8 bit_offset,
		       u8 bit_nums)
{
	struct sppctl_reg_t *x;
	int bit_value;
	u32 r;

	r = readl(moon1_regs_base + (reg_offset << 2));

	x = (struct sppctl_reg_t *)&r;
	bit_value = (x->v >> bit_offset) & (~(~0 << bit_nums));

	pctl_info("%s(x%X,x%X,x%X) v:x%X rval:x%X\n", __func__, reg_offset,
		  bit_offset, bit_nums, x->v, bit_value);

	return bit_value;
}

/*
 * On return 1, Selected pin is in GPIO mode.
 * On return 0, Selected pin is not in GPIO mode.
 */
int sunplus_gpio_mode_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

	reg_val = readl(first_regs_base + REG_OFFSET_GPIO_FIRST +
			R32_ROF(offset));
	// pctl_info("u F reg_val:%X = %d %px off:%d\n", reg_val,
	// R32_VAL(reg_val,R32_BOF(offset)),
	// first_regs_base, REG_OFFSET_GPIO_FIRST + R32_ROF(offset));

	return R32_VAL(reg_val, R32_BOF(offset));
}

/*
 * On Return 1, Corresponding GPIO is controlled by the corresponding
 * bit of GPIO output enable and GPIO output data registers
 * in this group(default)
 * On return 0, Corresponding GPIO is controlled by IOP register.
 *
 */

int sunplus_gpio_master_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

	reg_val = readl(gpioxt_regs_base + REG_OFFSET_CTL + R16_ROF(offset));
	// pctl_info("u M reg_val:%X = %d %px off:%d\n", reg_val,
	// R32_VAL(reg_val,R16_BOF(offset)),
	// gpioxt_regs_base, REG_OFFSET_CTL + R16_ROF(offset));

	return R32_VAL(reg_val, R16_BOF(offset));
}

/*
 * @value:
 * 1:Enable selected pin into GPIO mode.
 * 0:Disable selected pin into GPIO mode.
 */

int sunplus_gpio_mode_set(struct udevice *dev, unsigned int offset,
			  unsigned int value)
{
	u32 reg_val;

	reg_val = readl(first_regs_base + REG_OFFSET_GPIO_FIRST +
			R32_ROF(offset));

	if (value)
		reg_val |= BIT(R32_BOF(offset));
	else
		reg_val &= ~BIT(R32_BOF(offset));

	writel(reg_val,
	       first_regs_base + REG_OFFSET_GPIO_FIRST + R32_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Corresponding GPIO is controlled by the corresponding
 * bit of GPIO output enable and GPIO output data registers
 * in this group(default)
 * 0:Corresponding GPIO is controlled by IOP register.
 */

int sunplus_gpio_master_set(struct udevice *dev, unsigned int offset,
			    unsigned int value)
{
	u32 reg_val;

	reg_val = (BIT(R16_BOF(offset)) << 16);

	if (value)
		reg_val |= BIT(R16_BOF(offset));
	else
		reg_val &= ~BIT(R16_BOF(offset));

	writel(reg_val, gpioxt_regs_base + REG_OFFSET_CTL + R16_ROF(offset));

	return 0;
}

/*
 * On return 1, Corresponding GPIO input value is inverted.
 * On return 0, Corresponding GPIO input value is normal(default).
 */

int sunplus_gpio_input_invert_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

	reg_val = readl(gpioxt_regs_base + REG_OFFSET_INPUT_INVERT +
			R16_ROF(offset));

	return R32_VAL(reg_val, R16_BOF(offset));
}

/*
 * On return 1, Corresponding GPIO output value is inverted.
 * On return 0, Corresponding GPIO output value is normal(default).
 */

int sunplus_gpio_output_invert_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

	reg_val = readl(gpioxt_regs_base + REG_OFFSET_OUTPUT_INVERT +
			R16_ROF(offset));

	return R32_VAL(reg_val, R16_BOF(offset));
}

/*
 * @value:
 * 1:Invert input value of Corresponding GPIO.
 * 0:Normalize input value of Corresponding GPIO.
 */

int sunplus_gpio_input_invert_set(struct udevice *dev, unsigned int offset,
				  unsigned int value)
{
	u32 reg_val;

	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((value & BIT(0)) << R16_BOF(offset));

	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_INPUT_INVERT + R16_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Invert output value of Corresponding GPIO.
 * 0:Normalize output value of Corresponding GPIO.
 */
int sunplus_gpio_output_invert_set(struct udevice *dev, unsigned int offset,
				   unsigned int value)
{
	u32 reg_val;

	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((value & BIT(0)) << R16_BOF(offset));

	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_OUTPUT_INVERT + R16_ROF(offset));

	return 0;
}

/*
 * On return 1, Corresponding GPIO is in open-drain mode.
 * On return 0, Corresponding GPIO is not in open-drain mode.
 */
int sunplus_gpio_open_drain_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

	reg_val = readl(gpioxt_regs_base + REG_OFFSET_OPEN_DRAIN +
			R16_ROF(offset));

	return R32_VAL(reg_val, R16_BOF(offset));
}

/*
 * @value:
 * 1:Enable open-drain mode for selected pin.
 * 0:Disable open-drain mode for selected pin.
 */
int sunplus_gpio_open_drain_set(struct udevice *dev, unsigned int offset,
				unsigned int value)
{
	u32 reg_val;

	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((value & BIT(0)) << R16_BOF(offset));

	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_OPEN_DRAIN + R16_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Enable Schmitt trigger input function(default).
 * 0:Disable Schmitt trigger input function;
 */
int sunplus_gpio_schmitt_trigger_set(struct udevice *dev, unsigned int offset,
				     unsigned int value)
{
	u32 reg_val;

	reg_val = readl(padctl1_regs_base + REG_OFFSET_SCHMITT_TRIGGER +
			R32_ROF(offset));
	if (value == 0)
		reg_val &= ~BIT(R32_BOF(offset));
	else
		reg_val |= BIT(R32_BOF(offset));

	writel(reg_val, padctl1_regs_base + REG_OFFSET_SCHMITT_TRIGGER +
				R32_ROF(offset));

	return 0;
}

/* slew-rate control; for GPIO only */
int sunplus_gpio_slew_rate_control_set(struct udevice *dev, unsigned int offset,
				       unsigned int value)
{
	unsigned int pin;
	u32 r;

	if (IS_DVIO(offset))
		return -EINVAL;

	pin = (offset > 19) ? offset - 60 : offset;

	r = readl(padctl2_regs_base + REG_OFFSET_SLEW_RATE + R32_ROF(pin));

	if (value)
		r |= BIT(R32_BOF(pin));
	else
		r &= ~BIT(R32_BOF(pin));

	writel(r, padctl2_regs_base + REG_OFFSET_SLEW_RATE + R32_ROF(pin));

	return 0;
}

/*
 * pull up the selected pin.
 */
int sunplus_gpio_pull_up(struct udevice *dev, unsigned int offset)
{
	unsigned int pin;
	u32 reg_val;

	if (IS_DVIO(offset)) {
		pin = offset - 20;

		/* PU=1 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_UP +
				R30_ROF(pin));
		reg_val |= BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_UP + R30_ROF(pin));

		/* PD=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_DOWN +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_DOWN + R30_ROF(pin));
	} else {
		pin = (offset > 19) ? offset - 60 : offset;
		/* PE=1 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
				R32_ROF(pin));
		reg_val |= BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
					R32_ROF(pin));

		/* PS=1 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
				R32_ROF(pin));
		reg_val |= BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
					R32_ROF(pin));

		/* SPU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
					R32_ROF(pin));
	}

	return 0;
}

/*
 * pull down the selected pin.
 */
int sunplus_gpio_pull_down(struct udevice *dev, unsigned int offset)
{
	unsigned int pin;
	u32 reg_val;

	if (IS_DVIO(offset)) {
		pin = offset - 20;

		/* PU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_UP +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_UP + R30_ROF(pin));

		/* PD=1 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_DOWN +
				R30_ROF(pin));
		reg_val |= BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_DOWN + R30_ROF(pin));
	} else {
		pin = (offset > 19) ? offset - 60 : offset;
		/* PE=1 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
				R32_ROF(pin));
		reg_val |= BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
					R32_ROF(pin));

		/* PS=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
					R32_ROF(pin));

		/* SPU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
					R32_ROF(pin));
	}

	return 0;
}

/*
 * Strongly pull up the selected pin. For GPIO only, exclude DVIO
 */
int sunplus_gpio_strong_pull_up(struct udevice *dev, unsigned int offset)
{
	unsigned int pin;
	u32 reg_val;

	if (IS_DVIO(offset))
		return -EINVAL;

	pin = (offset > 19) ? offset - 60 : offset;
	/* PE=1 */
	reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
			R32_ROF(pin));
	reg_val |= BIT(R32_BOF(pin));
	writel(reg_val,
	       padctl2_regs_base + REG_OFFSET_PULL_ENABLE + R32_ROF(pin));

	/* PS=1 */
	reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
			R32_ROF(pin));
	reg_val |= BIT(R32_BOF(pin));
	writel(reg_val,
	       padctl2_regs_base + REG_OFFSET_PULL_SELECTOR + R32_ROF(pin));

	/* SPU=1 */
	reg_val = readl(padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
			R32_ROF(pin));
	reg_val |= BIT(R32_BOF(pin));
	writel(reg_val,
	       padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP + R32_ROF(pin));

	return 0;
}

/*
 * high-Z; For DVIO only, exclude GPIO
 */
int sunplus_gpio_high_impedance(struct udevice *dev, unsigned int offset)
{
	unsigned int pin;
	u32 reg_val;

	if (!IS_DVIO(offset)) {
		pin = (offset > 19) ? offset - 60 : offset;
		/* PE=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
					R32_ROF(pin));

		/* PS=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
					R32_ROF(pin));

		/* SPU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
					R32_ROF(pin));
	} else {
		pin = offset - 20;

		/* PU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_UP +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_UP + R30_ROF(pin));

		/* PD=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_DOWN +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_DOWN + R30_ROF(pin));
	}

	return 0;
}

/*
 * Disable all bias on the selected pin.
 */
int sunplus_gpio_bias_disable(struct udevice *dev, unsigned int offset)
{
	unsigned int pin;
	u32 reg_val;

	if (IS_DVIO(offset)) {
		pin = offset - 20;

		/* PU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_UP +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_UP + R30_ROF(pin));

		/* PD=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_DOWN +
				R30_ROF(pin));
		reg_val &= ~BIT(R30_BOF(pin));
		writel(reg_val,
		       padctl2_regs_base + REG_OFFSET_PULL_DOWN + R30_ROF(pin));
	} else {
		pin = (offset > 19) ? offset - 60 : offset;
		/* PE=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_ENABLE +
					R32_ROF(pin));

		/* PS=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_PULL_SELECTOR +
					R32_ROF(pin));

		/* SPU=0 */
		reg_val = readl(padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
				R32_ROF(pin));
		reg_val &= ~BIT(R32_BOF(pin));
		writel(reg_val, padctl2_regs_base + REG_OFFSET_STRONG_PULL_UP +
					R32_ROF(pin));
	}
	return 0;
}

// voltage mode select
int sunplus_gpio_voltage_mode_select_set(struct udevice *dev,
					 enum vol_ms_group ms_group,
					 unsigned int value)
{
	unsigned int bit;
	u32 r;

	switch (ms_group) {
	case G_MX_MS_TOP_0:
		bit = 0;
		break;
	case G_MX_MS_TOP_1:
		bit = 1;
		break;
	case AO_MX_MS_TOP_0:
		bit = 2;
		break;
	case AO_MX_MS_TOP_1:
		bit = 3;
		break;
	case AO_MX_MS_TOP_2:
		bit = 4;
		break;
	default:
		return -EINVAL;
	}

	r = readl(padctl2_regs_base + REG_OFFSET_MODE_SELECT);

	if (value)
		r |= BIT(bit);
	else
		r &= ~BIT(bit);

	writel(r, padctl2_regs_base + REG_OFFSET_MODE_SELECT);

	return 0;
}

/*
 * Set driving strength in uA.
 * The selectable strength values are listed in sppctl-config.h
 */
int sunplus_gpio_drive_strength_set(struct udevice *dev, unsigned int offset,
				    unsigned int uA)
{
	u32 reg_val;
	int ret = 0;
	u32 ds0 = 0;
	u32 ds1 = 0;
	u32 ds2 = 0;
	u32 ds3 = 0;

	if (IS_DVIO(offset)) {
		switch (uA) {
		case SPPCTRL_DVIO_DRV_IOH_5100_IOL_6200UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_7600_IOL_9300UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_10100_IOL_12500UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_12600_IOL_15600UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_15200_IOL_18700UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_17700_IOL_21800UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_20200_IOL_24900UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_22700_IOL_27900UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_25200_IOL_31000UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_27700_IOL_34100UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_30300_IOL_37200UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_32800_IOL_40300UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_35300_IOL_43400UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_37800_IOL_46400UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_DVIO_DRV_IOH_40300_IOL_49500UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_DVIO_DRV_IOH_42700_IOL_52600UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 1;
			ds0 = 1;
			break;
		default:
			ret = -EINVAL;
			break;
		}
	} else {
		switch (uA) {
		case SPPCTRL_GPIO_DRV_IOH_1100_IOL_1100UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_1600_IOL_1700UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_3300_IOL_3300UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_4900_IOL_5000UA:
			ds3 = 0;
			ds2 = 0;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_6600_IOL_6600UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_8200_IOL_8300UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_9900_IOL_9900UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_11500_IOL_11600UA:
			ds3 = 0;
			ds2 = 1;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_13100_IOL_13200UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_14800_IOL_14800UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_16400_IOL_16500UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_18100_IOL_18100UA:
			ds3 = 1;
			ds2 = 0;
			ds1 = 1;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_19600_IOL_19700UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 0;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_21300_IOL_21400UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 0;
			ds0 = 1;
			break;
		case SPPCTRL_GPIO_DRV_IOH_22900_IOL_23000UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 1;
			ds0 = 0;
			break;
		case SPPCTRL_GPIO_DRV_IOH_24600_IOL_24600UA:
			ds3 = 1;
			ds2 = 1;
			ds1 = 1;
			ds0 = 1;
			break;
		default:
			ret = -EINVAL;
			break;
		}
	}

	if (ret == 0) {
		/* DS0 */
		reg_val = readl(padctl1_regs_base + REG_OFFSET_DS0 +
				R32_ROF(offset));
		if (ds0 == 0)
			reg_val &= ~BIT(R32_BOF(offset));
		else
			reg_val |= BIT(R32_BOF(offset));

		writel(reg_val,
		       padctl1_regs_base + REG_OFFSET_DS0 + R32_ROF(offset));

		/* DS1 */
		reg_val = readl(padctl1_regs_base + REG_OFFSET_DS1 +
				R32_ROF(offset));
		if (ds1 == 0)
			reg_val &= ~BIT(R32_BOF(offset));
		else
			reg_val |= BIT(R32_BOF(offset));

		writel(reg_val,
		       padctl1_regs_base + REG_OFFSET_DS1 + R32_ROF(offset));

		/* DS2 */
		reg_val = readl(padctl1_regs_base + REG_OFFSET_DS2 +
				R32_ROF(offset));
		if (ds2 == 0)
			reg_val &= ~BIT(R32_BOF(offset));
		else
			reg_val |= BIT(R32_BOF(offset));

		writel(reg_val,
		       padctl1_regs_base + REG_OFFSET_DS2 + R32_ROF(offset));

		/* DS3 */
		reg_val = readl(padctl1_regs_base + REG_OFFSET_DS3 +
				R32_ROF(offset));
		if (ds3 == 0)
			reg_val &= ~BIT(R32_BOF(offset));
		else
			reg_val |= BIT(R32_BOF(offset));

		writel(reg_val,
		       padctl1_regs_base + REG_OFFSET_DS3 + R32_ROF(offset));
	}

	return ret;
}

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
static int sunplus_gpio_to_ao_pin(struct udevice *dev,
				  unsigned int offset)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(gpio_ao_int_pins); i++) {
		if (offset == gpio_ao_int_pins[i])
			return i;
	}
	return -1;
}

int sunplus_gpio_ao_int_debounce_ctrl_set(struct udevice *dev,
					  unsigned int selector, int value)
{
	int ao_pin;
	int mask;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	mask = 1 << ao_pin;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_DEB_EN);
	if (value == 0)
		r &= ~mask; /* disable */
	else
		r |= mask; /* enable */

	writel(r, gpio_ao_int_regs_base + REG_OFFSET_DEB_EN);

	return 0;
}

int sunplus_gpio_ao_int_debounce_ctrl_get(struct udevice *dev,
					  unsigned int selector)
{
	int ao_pin;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	r = readl(gpio_ao_int_regs_base + REG_OFFSET_DEB_EN);

	return R32_VAL(r, R32_BOF(ao_pin));
}

int sunplus_gpio_ao_int_ctrl_set(struct udevice *dev, unsigned int selector,
				 int value)
{
	int ao_pin;
	int mask;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	mask = 1 << ao_pin;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_EN);
	if (value == 0)
		r &= ~mask; /* disable */
	else
		r |= mask; /* enable */

	writel(r, gpio_ao_int_regs_base + REG_OFFSET_INTR_EN);

	return 0;
}

int sunplus_gpio_ao_int_ctrl_get(struct udevice *dev, unsigned int selector)
{
	int ao_pin;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_EN);

	return R32_VAL(r, R32_BOF(ao_pin));
}

int sunplus_gpio_ao_int_trigger_mode_set(struct udevice *dev,
					 unsigned int selector, int value)
{
	int ao_pin;
	int mask;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	mask = 1 << ao_pin;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_MODE);
	if (value == 0)
		r &= ~mask; /* edge trigger */
	else
		r |= mask; /* level trigger */

	writel(r, gpio_ao_int_regs_base + REG_OFFSET_INTR_MODE);

	return 0;
}

int sunplus_gpio_ao_int_trigger_mode_get(struct udevice *dev,
					 unsigned int selector)
{
	int ao_pin;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_MODE);

	return R32_VAL(r, R32_BOF(ao_pin));
}

int sunplus_gpio_ao_int_trigger_polarity_set(struct udevice *dev,
					     unsigned int selector, int value)
{
	int ao_pin;
	int mask;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	mask = 1 << ao_pin;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_POL);
	if (value == 0)
		r &= ~mask; /* rising edge or level high */
	else
		r |= mask; /* falling edge or level low */

	writel(r, gpio_ao_int_regs_base + REG_OFFSET_INTR_POL);

	return 0;
}

int sunplus_gpio_ao_int_trigger_polarity_get(struct udevice *dev,
					     unsigned int selector)
{
	int ao_pin;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_POL);

	return R32_VAL(r, R32_BOF(ao_pin));
}

int sunplus_gpio_ao_int_mask_set(struct udevice *dev, unsigned int selector,
				 int value)
{
	int ao_pin;
	int mask;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	mask = 1 << ao_pin;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_MASK);
	if (value == 0)
		r &= ~mask;
	else
		r |= mask;
	writel(r, gpio_ao_int_regs_base + REG_OFFSET_INTR_MASK);

	return 0;
}

int sunplus_gpio_ao_int_mask_get(struct udevice *dev, unsigned int selector)
{
	int ao_pin;
	u32 r;

	ao_pin = sunplus_gpio_to_ao_pin(dev, selector);
	if (ao_pin < 0)
		return -EINVAL;

	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_MASK);

	return R32_VAL(r, R32_BOF(ao_pin));
}

int sunplus_gpio_ao_int_flag_clear(struct udevice *dev, unsigned int irq_num)
{
	int mask;
	u32 r;

	mask = 1 << irq_num;
	r = readl(gpio_ao_int_regs_base + REG_OFFSET_INTR_CLR);

	r |= mask;

	writel(r, gpio_ao_int_regs_base + REG_OFFSET_INTR_CLR);

	return 0;
}

#endif

/*
 * On return 1, Corresponding GPIO direction is input.
 * On return 0, Corresponding GPIO direction is output.
 */
int sunplus_gpio_direction_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;
		if (readl(gpio_ao_int_regs_base + REG_OFFSET_OE) & mask) // GPIO_OE
			return 0;
		else
			return 1;
	}
#endif
	reg_val = readl(gpioxt_regs_base + REG_OFFSET_OUTPUT_ENABLE +
			R16_ROF(offset));

	return R32_VAL(reg_val, R16_BOF(offset)) ^ BIT(0);
}

/*
 * Set the selected pin direction to input.
 */
int sunplus_gpio_set_direction_input(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;
		reg_val = readl(gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		reg_val &= ~mask;
		writel(reg_val, gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		return 0;
	}
#endif
	reg_val = (BIT(R16_BOF(offset)) << 16);
	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_OUTPUT_ENABLE + R16_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Enable input for the selected pin. Meanwhile, output is disabled.
 * 0:Disable input for the selected pin. Meanwhile, output is enabled.
 */
int sunplus_gpio_input_enable_set(struct udevice *dev, unsigned int offset,
				  unsigned int value)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;

		reg_val = readl(gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		if (value == 1)
			reg_val &= ~mask; /* enable */
		else
			reg_val |= mask; /* disable */

		writel(reg_val, gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		return 0;
	}
#endif

	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((!value & BIT(0)) << R16_BOF(offset));
	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_OUTPUT_ENABLE + R16_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Enable output for the selected pin. Meanwhile, input is disabled.
 * 0:Disable output for the selected pin. Meanwhile, input is enabled.
 */
int sunplus_gpio_output_enable_set(struct udevice *dev, unsigned int offset,
				   unsigned int value)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;

		reg_val = readl(gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		if (value == 1)
			reg_val |= mask; /* enable */
		else
			reg_val &= ~mask; /* disable */

		writel(reg_val, gpio_ao_int_regs_base + REG_OFFSET_OE); // GPIO_OE
		return 0;
	}
#endif

	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((value & BIT(0)) << R16_BOF(offset));
	writel(reg_val,
	       gpioxt_regs_base + REG_OFFSET_OUTPUT_ENABLE + R16_ROF(offset));

	return 0;
}

/*
 * @value:
 * 1:Output high on the selected pin.
 * 0:Output low on the selected pin.
 */
int sunplus_gpio_output_set(struct udevice *dev, unsigned int offset,
			    unsigned int value)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;

		reg_val = readl(gpio_ao_int_regs_base + REG_OFFSET_AO_OUTPUT); // GPIO_O
		if (value)
			reg_val |= mask;
		else
			reg_val &= ~mask;
		writel(reg_val, gpio_ao_int_regs_base + REG_OFFSET_AO_OUTPUT); // GPIO_O
		return 0;
	}
#endif
	reg_val = (BIT(R16_BOF(offset)) << 16) |
		  ((value & BIT(0)) << R16_BOF(offset));
	writel(reg_val, gpioxt_regs_base + REG_OFFSET_OUTPUT + R16_ROF(offset));

	return 0;
}

/*
 * On return 1, Corresponding GPIO input value is high.
 * On return 0, Corresponding GPIO input value is low.
 */
int sunplus_gpio_input_query(struct udevice *dev, unsigned int offset)
{
	u32 reg_val;

#ifdef CONFIG_PINCTRL_SUPPORT_GPIO_AO_INT
	int ao_pin, mask;

	ao_pin = sunplus_gpio_to_ao_pin(dev, offset);
	if (ao_pin >= 0) {
		mask = 1 << ao_pin;
		if (readl(gpio_ao_int_regs_base + REG_OFFSET_DEB_EN) & mask) // GPIO_DEB_EN
			reg_val = readl(gpio_ao_int_regs_base +
					REG_OFFSET_DEB_I); // GPIO_DEB_I
		else
			reg_val = readl(gpio_ao_int_regs_base + REG_OFFSET_INT_I); // GPIO_I
		if (reg_val & mask)
			return 1;
		else
			return 0;
	}
#endif

	reg_val = readl(gpioxt_regs_base + REG_OFFSET_INPUT + R32_ROF(offset));

	return R32_VAL(reg_val, R32_BOF(offset));
}
