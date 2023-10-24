/*
 * This is a driver for the ehci controller found in Sunplus Gemini SoC
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <usb.h>
#include <asm/io.h>
#include <usb/ehci-ci.h>
#include <usb/ulpi.h>
#include <linux/delay.h>

#include "ehci.h"


#define REG_BASE		0xf8000000
#define REG_BASE_AO		0xf8800000

#define RF_GRP(_grp, _reg) ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_GRP_AO(_grp, _reg) ((((_grp) * 32 + (_reg)) * 4) + REG_BASE_AO)

#define RF_MASK_V(_mask, _val)       (((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)         (((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)         (((_mask) << 16) | 0)

// usb spec 2.0 Table 7-3  VHSDSC (min, max) = (525, 625)
// default = 577 mV (374 + 7 * 29)
#define DEFAULT_UPHY_DISC	0x7   // 7 (=577mv)
#define DEFAULT_SQ_CT		0x3

struct uphy_rn_regs {
	u32 cfg[28];		       // 150.0
	u32 gctrl[3];		       // 150.28
	u32 gsts;		       // 150.31
};
#define UPHY0_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(149, 0))
#define UPHY1_RN_REG ((volatile struct uphy_rn_regs *)RF_GRP(150, 0))

struct moon0_regs {
	unsigned int stamp;            // 0.0
	unsigned int reset[15];        // 0.1 -  0.12
	unsigned int rsvd[15];         // 0.13 - 0.30
	unsigned int hw_cfg;           // 0.31
};
#define MOON0_REG ((volatile struct moon0_regs *)RF_GRP_AO(0, 0))

struct moon1_regs {
	unsigned int sft_cfg[32];
};
#define MOON1_REG ((volatile struct moon1_regs *)RF_GRP_AO(1, 0))

struct moon2_regs {
	unsigned int sft_cfg[32];
};
#define MOON2_REG ((volatile struct moon2_regs *)RF_GRP_AO(2, 0))

struct moon3_regs {
	unsigned int sft_cfg[32];
};
#define MOON3_REG ((volatile struct moon3_regs *)RF_GRP_AO(3, 0))

struct moon4_regs {
	unsigned int sft_cfg[32];
};
#define MOON4_REG ((volatile struct moon4_regs *)RF_GRP_AO(4, 0))

struct moon5_regs {
	unsigned int sft_cfg[32];
};
#define MOON5_REG ((volatile struct moon5_regs *)RF_GRP_AO(5, 0))

struct hb_gp_regs {
	unsigned int hb_otp_data[8];
	unsigned int reserved_8[24];
};
#define OTP_REG	((volatile struct hb_gp_regs *)RF_GRP_AO(71, 0))

struct sunplus_ehci_priv {
	struct ehci_ctrl ehcictrl;
	struct usb_ehci *ehci;
};

static void uphy_init(int port_num)
{
	unsigned int val, set;

	if (0 == port_num) {
		/* enable clock for UPHY, USBC and OTP */
		MOON2_REG->sft_cfg[6] = RF_MASK_V_SET(1 << 12); // UPHY0_CLKEN=1
		MOON2_REG->sft_cfg[6] = RF_MASK_V_SET(1 << 15); // USBC0_CLKEN=1
		MOON2_REG->sft_cfg[5] = RF_MASK_V_SET(1 << 13); // OTPRX_CLKEN=1

		/* disable reset for OTP */
		MOON0_REG->reset[0] = RF_MASK_V_CLR(1 << 9);  // RBUS_BLOCKB_RESET=0
		mdelay(1);
		MOON0_REG->reset[4] = RF_MASK_V_CLR(1 << 13); // OTPRX_RESET=0
		mdelay(1);

		/* reset UPHY0 */
		/* UPHY0_RESET : 1->0 */
		MOON0_REG->reset[5] = RF_MASK_V_SET(1 << 12);
		mdelay(1);
		MOON0_REG->reset[5] = RF_MASK_V_CLR(1 << 12);
		mdelay(1);

		/* Default value modification */
		/* G149.28 uphy0_gctr0 */
		UPHY0_RN_REG->gctrl[0] = 0x08888101;

		/* PLL power off/on twice */
		/* G149.30 uphy0_gctrl2 */
		UPHY0_RN_REG->gctrl[2] = 0x88;
		mdelay(1);
		UPHY0_RN_REG->gctrl[2] = 0x80;
		mdelay(1);
		UPHY0_RN_REG->gctrl[2] = 0x88;
		mdelay(1);
		UPHY0_RN_REG->gctrl[2] = 0x80;
		mdelay(1);
		UPHY0_RN_REG->gctrl[2] = 0;
		mdelay(20); /*  experience */

		/* USBC 0 reset */
		/* USBC0_RESET : 1->0 */
		MOON0_REG->reset[5] = RF_MASK_V_SET(1 << 15);
		mdelay(1);
		MOON0_REG->reset[5] = RF_MASK_V_CLR(1 << 15);
		mdelay(1);

		/* fix rx-active question */
		/* G149.19 */
		UPHY0_RN_REG->cfg[19] |= 0xf;

		/* OTP for USB phy rx clock invert */
		/* G149.29[6] */
		val = OTP_REG->hb_otp_data[2];
		if (val & 0x1)
			UPHY0_RN_REG->gctrl[1] |= (1 << 6);

		/* OTP for USB phy tx clock invert */
		/* G149.29[5] */
		val = OTP_REG->hb_otp_data[2];
		if ((val >> 1) & 0x1)
			UPHY0_RN_REG->gctrl[1] |= (1 << 5);

		/* OTP for USB DISC (disconnect voltage) */
		/* G149.7[4:0] */
		val = OTP_REG->hb_otp_data[6];
	        set = val & 0x1f;
	        if (!set)
	                set = DEFAULT_UPHY_DISC;

	        UPHY0_RN_REG->cfg[7] = (UPHY0_RN_REG->cfg[7] & 0xffffffe0) | set;
	}
}

static void usb_power_init(int is_host, int port_num)
{
	/* a. enable pin mux control	*/
	/*    Host: enable		*/
	/*    Device: disable		*/
	if (is_host) {
		MOON1_REG->sft_cfg[1] = RF_MASK_V_SET(1 << 7);
	} else {
		MOON1_REG->sft_cfg[1] = RF_MASK_V_CLR(1 << 7);
	}

	/* b. USB control register: 			*/
	/*    Host:   ctrl=1, host sel=1, type=1 	*/
	/*    Device  ctrl=1, host sel=0, type=0 	*/
	if (is_host) {
		MOON4_REG->sft_cfg[10] = RF_MASK_V_SET(7 << 0);
	} else {
		MOON4_REG->sft_cfg[10] = RF_MASK_V_SET(1 << 0);
		MOON4_REG->sft_cfg[10] = RF_MASK_V_CLR(3 << 1);
	}
}

static int ehci_sunplus_ofdata_to_platdata(struct udevice *dev)
{
	struct sunplus_ehci_priv *priv = dev_get_priv(dev);

	priv->ehci = (struct usb_ehci *)devfdt_get_addr_ptr(dev);
	if (!priv->ehci)
		return -EINVAL;

	return 0;
}

static int ehci_sunplus_probe(struct udevice *dev)
{
	struct usb_plat *plat = dev_get_plat(dev);
	struct sunplus_ehci_priv *priv = dev_get_priv(dev);
	struct ehci_hccr *hccr;
	struct ehci_hcor *hcor;

	hccr = (struct ehci_hccr *)((uint64_t)&priv->ehci->ehci_len_rev);
	hcor = (struct ehci_hcor *)((uint64_t)&priv->ehci->ehci_usbcmd);

	printf("%s.%d, dev_name:%s,port_num:%d\n",__FUNCTION__, __LINE__, dev->name, dev->seq_);

	uphy_init(dev->seq_);
	usb_power_init(1, dev->seq_);

	return ehci_register(dev, hccr, hcor, NULL, 0, plat->init_type);
}

static int ehci_usb_remove(struct udevice *dev)
{
	printf("%s.%d, dev_name:%s,port_num:%d\n",__FUNCTION__, __LINE__, dev->name, dev->seq_);
	usb_power_init(0, dev->seq_);

	return ehci_deregister(dev);
}

static const struct udevice_id ehci_sunplus_ids[] = {
	{ .compatible = "sunplus,sp7350-usb-ehci" },
	{ }
};

U_BOOT_DRIVER(ehci_sunplus) = {
	.name	= "ehci_sunplus",
	.id	= UCLASS_USB,
	.of_match = ehci_sunplus_ids,
	.of_to_plat = ehci_sunplus_ofdata_to_platdata,
	.probe = ehci_sunplus_probe,
	.remove = ehci_usb_remove,
	.ops	= &ehci_usb_ops,
	.plat_auto = sizeof(struct usb_plat),
	.priv_auto = sizeof(struct sunplus_ehci_priv),
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,
};

