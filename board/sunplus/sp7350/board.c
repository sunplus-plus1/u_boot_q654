/*
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <version.h>
#include <common.h>
#include <asm/global_data.h>
#include <linux/io.h>
#include <asm/gpio.h>
#include <env.h>
#include <blk.h>
#include <command.h>
#include <errno.h>
#include <part.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <net.h>
#include <dm.h>
#if CONFIG_IS_ENABLED(FASTBOOT)
#include <asm/psci.h>
#include <fastboot.h>
#endif

typedef enum {
	board_none,
	board_dvb1,
	//board_dvb2,
	//board_flare5,
	board_max,
}board_type;

#ifdef CONFIG_SP_SPINAND
extern void board_spinand_init(void);
#endif

#ifdef CONFIG_SP_PARANAND
extern void board_paranand_init(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

static int board = board_none;
static int init_board_info(const char *model){
	if( !strcmp(model,"Sunplus SP7350 DVB")) {
		board = board_dvb1;
	/*} else if (!strcmp(model,"Sunplus SP7350 DVB2")) {
		board = board_dvb2;*/
	}

	printf("board = %d\n", board);

	return 0;
}

int board_init(void)
{
	unsigned int reg_val = 0;
	const char *model;

	model = fdt_getprop(gd->fdt_blob, 0, "model", NULL);
	if (model)
		init_board_info(model);

	if (board != board_dvb1)
		return 0;

	if (fdt_path_offset(gd->fdt_blob, "/stmmac@f8103000") >= 0) {
		//printf("%s: CA55_JTAG_DISABLE couse GMAC pinmux\n", __func__);
		writel(0x01000000, 0xf8800084); //disable JTAG and enable GMAC via pin mux
		writel(0x00044800, 0xf8803378); //set GMAC tx delay to 2.54ns
		writel(0x00018060, 0xf880337c); //set GMAC rx delay to 7.48ns
		//set GMAC pin drive strength 25%
		reg_val = readl(0xf8803290);
		writel(reg_val|0x0001fff8, 0xf8803290);
		reg_val = readl(0xf88032a0);
		writel(reg_val|0x0001fff8, 0xf88032a0);
		reg_val = readl(0xf88032b0);
		writel(reg_val&0xfffe0007, 0xf88032b0);
		reg_val = readl(0xf88032c0);
		writel(reg_val&0xfffe0007, 0xf88032c0);
	}

	/* MS#0,MS#1 output 1.8V */
	reg_val = readl(0xf8803330);
	reg_val |= 0x13;
	writel(reg_val, 0xf8803330);

	writel(((3<<16) | (3 << 20)) | (2 | (2 << 4)) , 0xf88001dc); //set emmc clocksorce 800M

	/* GPIO2 output enable */
	reg_val = readl(0xf88033b4);
	reg_val |= 0x00040004;
	writel(reg_val, 0xF88033b4);

	/* SD timming config */
	reg_val = 0x444330;
	writel(reg_val, 0xf8003bb4);

	/* set gpio-27 as clock 24M for camera sensor */
	reg_val = readl(0xf8800090);
	reg_val |= 0x30001000;
	writel(reg_val, 0xf8800090); //enable CLK_DGO pinmux
	writel(0x8000800, 0xf8803384);//enable GPIO Output
	// config to 24M
	writel(0x01, 0xf88077e4);
	writel(0x08, 0xf88077e8);
	writel(0x04, 0xf88077ec);
	writel(0x01, 0xf88077e0);
	/* set gpio-27 as clock 24M for camera sensor */

#if 0
	if( board == board_flare5 ) {
		/* set gpio-1 as clock 24M for camera sensor */
		reg_val = readl(0xf8800090);
		reg_val |= 0x00300010;
		writel(reg_val, 0xf8800090);  //enable CLK_DGO pinmux
		writel(0x00020002, 0xf8803380);//enable GPIO Output
		// config to 24M
		writel(0x01, 0xf88077a4);
		writel(0x08, 0xf88077a8);
		writel(0x04, 0xf88077ac);
		writel(0x01, 0xf88077a0);
		/* set gpio-1 as clock 24M for camera sensor */
	}
#endif
	return 0;
}

int board_eth_init(struct bd_info *bis)
{
	return 0;
}

int misc_init_r(void)
{
	return 0;
}

void board_nand_init(void)
{
#ifdef CONFIG_SP_SPINAND
	board_spinand_init();
#endif
#ifdef CONFIG_SP_PARANAND
	board_paranand_init();
#endif
}

#define BOOT_REASON_DUMP     (0x1 << 13)
#define BOOT_REASON_FASTBOOT (0x2 << 13)
#define BOOT_REASON_RECOVERY (0x3 << 13)

#if CONFIG_IS_ENABLED(FASTBOOT)
int fastboot_set_reboot_flag(enum fastboot_reboot_reason reason)
{
	unsigned int reg_val;
	unsigned int mask = 0;

	reg_val = readl(0xf8801200);
	switch (reason) {
	case FASTBOOT_REBOOT_REASON_RECOVERY :
		mask = BOOT_REASON_RECOVERY | 0x1 << 5;
		/*clear boot reason*/
		reg_val &= 0x1FDF;
		reg_val |= mask;
		break;
	case FASTBOOT_REBOOT_REASON_BOOTLOADER :
		mask = BOOT_REASON_FASTBOOT | 0x1 << 5;
		/*clear boot reason*/
		reg_val &= 0x1FDF;
		reg_val |= mask;
		break;
	default:
		reg_val &= 0x1FDF;
		break;
	}

	writel(reg_val, 0xf8801200);
	printf("Using reboot reason: 0x%x,rtc reg=0x%x\n", reason, reg_val);

	return 0;
}
#endif

static void boot_mode_check(void)
{
	unsigned int reg_val;
	unsigned int boot_mode;

	reg_val = readl(0xf8801200);
	boot_mode = reg_val & 0xffffe000;
	/*clear boot reason*/
	reg_val &= 0x1FDF;
	writel(reg_val, 0xf8801200);
	switch (boot_mode) {
	case BOOT_REASON_DUMP:
		printf("boot mode: dump\n");
		break;
	case BOOT_REASON_FASTBOOT:
		printf("boot mode: fastboot\n");
		char *pr = "fastboot usb 1";
		run_command_list(pr, -1, 0);
		break;
	case BOOT_REASON_RECOVERY:
		printf("boot mode: recovery\n");
		break;
	default:
		printf("boot mode: normal\n");
		break;
	}
}

#define GPIO_KEY_DL_DEFAULT -1
static int key_dl_pressed(void)
{
	int gpio = fdtdec_get_int(gd->fdt_blob, 0, "dlkey", GPIO_KEY_DL_DEFAULT);

	if (GPIO_KEY_DL_DEFAULT == gpio)
		return 0;

	printf("dlkey: gpio%d\n", gpio);
	gpio_request(gpio, "key_dl");
	gpio_direction_input(gpio);

	return !gpio_get_value(gpio);
}

static void  detect_dl_mode(void)
{
	if(env_get("ltpflag")) {
		int val = env_get_hex("ltpflag",0);
		if (val == 1) {
			val = 0;
			env_set_hex("ltpflag", val);
			env_save();
			env_set("preboot", "setenv preboot; fastboot usb 1");
		}
	}
}

static int dl_mode_enter(void)
{
	int read_board_value = 0;
	char *val="1";
	char *name = "board_type";

	read_board_value =  readl(CONFIG_BOARD_TYPE_ADDR);
	//is dvb board?
	if(0xfa00 == (read_board_value & 0xff00)) {
		env_set(name, val);
		env_save();
	}

	if (key_dl_pressed()) {
		printf("key pressed, enter DL mode...\n");
		env_set("preboot", "setenv preboot; fastboot usb 1");
	} else {
		printf("key not pressed, skip DL mode...!\n");
		detect_dl_mode();
	}

	return 0;
}

#ifdef CONFIG_BOARD_LATE_INIT
static int find_part(const char * part_name,struct disk_partition *info)
{
	struct blk_desc *desc;
	int ret = -1;
	int p;

	ret = blk_get_device_by_str("mmc", "0", &desc);
	if (ret < 0) {
		printf("can not find mmc 0\n");
		return -1;
	}

	for (p = 1; p < MAX_SEARCH_PARTITIONS; p++) {
		int r = part_get_info(desc, p, info);

		if (r != 0)
			continue;

		if(!strncmp((const char *)info->name, part_name, strlen(part_name)))
			return p;
	}

	//printf("%s:%d:cannot find part %s\n", __func__, __LINE__, part_name);

	return -1;
}

__weak int sp7350_video_show_board_info(void);

int board_late_init(void)
{
	struct disk_partition info;
	int ret,i,index;
	char env_buf[1024], var_name[50];

#ifdef CONFIG_VIDEO_SP7350
	sp7350_video_show_board_info();
#endif

#ifdef CONFIG_USB_ETHER
	usb_ether_init();
#endif

	if (board != board_dvb1) {
		// enable gpio2
		writel(readl(0xf88033b4) | 0x00040004, 0xf88033b4);
	}

	boot_mode_check();

	if(!env_get("bootpart")) {
		index = 1;
		env_set_hex("bootpart", index);
		env_save();
	}

	dl_mode_enter();

	index = env_get_hex("bootpart", 1);

	if(index < 1 || index > 3) {
		index = 1;
		env_set_hex("bootpart", index);
	}

	for(i = 1; i < 3; i++) {
		snprintf(env_buf,sizeof(env_buf),"rootfs%d", i);
		ret = find_part(env_buf, &info);
		if (ret >= 0) {
			snprintf(env_buf,sizeof(env_buf),"root=/dev/mmcblk0p%d rw rootwait ", ret);
			snprintf(var_name,sizeof(var_name),"emmc_root%d", i);
			env_set(var_name, env_buf);
			if(i == index)
				env_set("emmc_root", env_buf);
		}
		snprintf(env_buf,sizeof(env_buf),"kernel%d", i);
		ret = find_part(env_buf, &info);
		if (ret >= 0) {
			snprintf(env_buf,sizeof(env_buf),"0x%lx", info.start);
			snprintf(var_name,sizeof(var_name),"addr_src_kernel%d", i);
			env_set(var_name, env_buf);
			if(i == index)
				env_set("addr_src_kernel", env_buf);
		}
		snprintf(env_buf,sizeof(env_buf),"dtb%d", i);
		ret = find_part(env_buf, &info);
		if (ret >= 0) {
			snprintf(env_buf,sizeof(env_buf),"0x%lx", info.start);
			snprintf(var_name,sizeof(var_name),"addr_src_dtb%d", i);
			env_set(var_name, env_buf);
			if(i == index)
				env_set("addr_src_dtb", env_buf);
			snprintf(env_buf,sizeof(env_buf),"0x%lx", info.size);
			snprintf(var_name,sizeof(var_name),"sz_dtb%d", i);
			env_set(var_name, env_buf);
			if(i == index)
				env_set("sz_dtb", env_buf);
		}
	}
	return 0;
}
#endif

#if defined(CONFIG_DM_USB_GADGET)
#include <clk.h>
#include <dwc3-uboot.h>
#include <sp_udc.h>

int dm_usb_gadget_handle_interrupts(struct udevice *dev)
{
	#if defined(CONFIG_USB_GADGET_SUNPLUS_UDC)
	if (!strncmp(dev->name, "usb", 3)) {
		struct sp_udc *udc = dev_get_priv(dev);

		return sp_udc_irq(udc);
	}
	#endif

	#if defined(CONFIG_USB_DWC3_GADGET)
	if (!strncmp(dev->name, "spdwc3", 6)) {
		dwc3_uboot_handle_interrupt(0);

		return 1;
	}
	#endif

	return 0;
}
#endif
