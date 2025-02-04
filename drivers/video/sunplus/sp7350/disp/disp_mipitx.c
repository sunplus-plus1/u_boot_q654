// SPDX-License-Identifier: GPL-2.0+
/*
 * Author(s): Hammer Hsieh <hammer.hsieh@sunplus.com>
 */

#include <common.h>
#include "reg_disp.h"
#include "disp_mipitx.h"
#include "display2.h"
#include <asm/gpio.h>
#include <linux/delay.h>
#define SP_MIPITX_LP_MODE	0
#define SP_MIPITX_HS_MODE	1

/*
 * only for AC timing measurement
 */
//#define SP7350_MIPI_DSI_1500M_TEST
//#define SP7350_MIPI_DSI_1200M_TEST
/*
 * use fine tune timing settings
 * and set it before MIPITX module enable
 */
//#define SP7350_MIPI_DSI_TIMING_NEW

/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_mipitx_gpio_set(struct sp7350_disp_priv *sp, int sel)
{
	if (sel == 0) {
	/* reset panel */
	#if 0 //active low
	dm_gpio_set_value(&sp->reset, false);
	mdelay(15);
	dm_gpio_set_value(&sp->reset, true);
	mdelay(15);
	dm_gpio_set_value(&sp->reset, false);
	mdelay(25);
	#else //active high
	dm_gpio_set_value(&sp->reset, false);
	mdelay(15);
	dm_gpio_set_value(&sp->reset, true);
	mdelay(25);
	#endif
	} else if (sel == 1) { //for GM-70P476CK
		dm_gpio_set_value(&sp->reset, true);
		mdelay(5);
		dm_gpio_set_value(&sp->reset, false);
		mdelay(10);
		dm_gpio_set_value(&sp->reset, true);
		mdelay(120);
	}

}

void DRV_mipitx_pllclk_init(void)
{
	//printf("MIPITX CLK setting init\n");
	G205_MIPITX_REG1->sft_cfg[14] = 0x80000000; //init clock
	//init PLLH setting
	DISP_MOON3_REG->sft_cfg[15] = 0xffff40be; //PLLH BNKSEL = 0x2 (2000~2500MHz)
	DISP_MOON3_REG->sft_cfg[16] = 0xffff0009; //PLLH
	/*
	 * PRESCALE = 1, FBKDIV = 86
	 * PREDIV = 1, POSTDIV = 9, MIPITX_SEL=4
	 * PLLH Fvco = 2150MHz (fixed) = 25M * PRESCALE * FBKDIV / PREDIV
	 *
	 *                     25M * PRESCALE * FBKDIV        2150
	 * MIPITX pixel CLK = ---------------------------- = ------ = 59.72MHz
	 *                    PREDIV * POSTDIV * MIPITX_SEL  9 * 4
	 */
	DISP_MOON3_REG->sft_cfg[14] = 0xffff0b50; //PLLH POSTDIV = div9 (default)
	DISP_MOON3_REG->sft_cfg[25] = 0x07800180; //PLLH MIPITX_SEL = div4

	//init TXPLL setting
	G205_MIPITX_REG1->sft_cfg[10] = 0x00000003; //TXPLL enable and reset
	/*
	 * PRESCALE = 1, FBKDIV = 48, PREDIV = 2, POSTDIV = 1, EN_DIV5 = 0
	 * TXPLL Fvco = 600MHz = 25M * PRESCALE * FBKDIV / PREDIV
	 *                    25M * PRESCALE * FBKDIV        25M * 1 * 48
	 * MIPITX bit CLK = ----------------------------- = ----------- = 600MHz
	 *                   PREDIV * POSTDIV * 5^EN_DIV5    2 * 1 * 1
	 */
	G205_MIPITX_REG1->sft_cfg[11] = 0x00003001; //TXPLL MIPITX CLK = 600MHz
	G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 0x0 (320~640MHz)

	G205_MIPITX_REG1->sft_cfg[14] = 0x00000000; //init clock done
}

void DRV_mipitx_pllclk_set(int mode, int width, int height)
{
	if (mode == 0) { //LP mode
		//printf("MIPITX CLK setting for LP Mode\n");
		G204_MIPITX_REG0->sft_cfg[4] = 0x00000008; //(600/8/div9)=8.3MHz
	} else {
		//printf("MIPITX CLK setting for Video Mode w %d h %d\n", width, height);
		if ((width == 720) && (height == 480)) {
		#if 1//fine tune PLLH clk to fit 27.08MHz
			DISP_MOON3_REG->sft_cfg[14] = 0x00780050; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0xe << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 27.08MHz (just test)
		#else
			DISP_MOON3_REG->sft_cfg[14] = 0x00780020; //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800780; //PLLH MIPITX CLK = 26.77MHz
		#endif
			G205_MIPITX_REG1->sft_cfg[11] = 0x00021A00; //TXPLL MIPITX CLK = 162.5MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00021B00; //TXPLL MIPITX CLK = 168.75MHz

		} else if ((width == 800) && (height == 480)) {
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
			#if 1//fine tune PLLH clk to fit 26.563MHz
				#if 1 //update for sync drm setting
			DISP_MOON3_REG->sft_cfg[14] = 0x00780078; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x31 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = xxxMHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00001b00; //TXPLL MIPITX CLK = xxxMHz
				#else
			DISP_MOON3_REG->sft_cfg[14] = 0x00780058; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x15 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 26.563MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00021E00; //TXPLL MIPITX CLK = 175MHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00000d10; //TXPLL MIPITX CLK = 650MHz
				#endif
			#else
			//DISP_MOON3_REG->sft_cfg[14] = 0x00780050; //PLLH
			//DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 30MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00021E00; //TXPLL MIPITX CLK = 175MHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00000d10; //TXPLL MIPITX CLK = 650MHz
			#endif
#else
			;//TBD
#endif
		} else if ((width == 1024) && (height == 600)) { // 1024x600
			;//TBD
			//DISP_MOON3_REG->sft_cfg[14] = 0x00780020; //PLLH
			//DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 54MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00023100; //TXPLL MIPITX CLK = 306.25MHz
		} else if ((width == 480) && (height == 1280)) {
			#if 1 //update for sync drm setting
			DISP_MOON3_REG->sft_cfg[14] = 0x00780000; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x16 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800780; //PLLH MIPITX CLK = xxxMHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00023400; //TXPLL MIPITX CLK = xxxMHz
			//G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
			#else
			DISP_MOON3_REG->sft_cfg[14] = 0x00780028; //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 49MHz	
			G205_MIPITX_REG1->sft_cfg[11] = 0x00000c00; //TXPLL MIPITX CLK = 300MHz
			G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
			#endif
		} else if ((width == 800) && (height == 1280)) {
			//DISP_MOON3_REG->sft_cfg[14] = 0x00780040; //PLLH
			//DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x1A << 7)); //PLLH
			//DISP_MOON3_REG->sft_cfg[25] = 0x07800180; //PLLH MIPITX CLK = 75MHz

			DISP_MOON3_REG->sft_cfg[14] = 0x00780078; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0xB << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800080; //PLLH MIPITX CLK = 75MHz

			G205_MIPITX_REG1->sft_cfg[11] = 0x00001200; //TXPLL MIPITX CLK = 450MHz
			G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
		} else if ((width == 240) && (height == 320)) {
			#if 1 //fine tune for screen flicker
			//#if 1 //update for sync drm setting
			DISP_MOON3_REG->sft_cfg[14] = 0x80000000; //PLLH pre_scal = div1
			DISP_MOON3_REG->sft_cfg[14] = 0x00780050; //PLLH post_div = x101-0xxx
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x14 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800780; //PLLH MIPITX CLK = 14.583MHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00023800; //TXPLL MIPITX CLK = xxxMHz
			//#else
			//DISP_MOON3_REG->sft_cfg[14] = 0x80000000; //PLLH pre_scal = div1
			//DISP_MOON3_REG->sft_cfg[14] = 0x00780050; //PLLH post_div = x101-0xxx
			//DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x14 << 7)); //PLLH
			//DISP_MOON3_REG->sft_cfg[25] = 0x07800780; //PLLH MIPITX CLK = 14.583MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00000E00; //TXPLL MIPITX CLK = 350MHz
			////G205_MIPITX_REG1->sft_cfg[11] = 0x00000F00; //TXPLL MIPITX CLK = 375MHz
			//G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
			//#endif
			#else
			DISP_MOON3_REG->sft_cfg[14] = 0x80000000; //PLLH pre_scal = div1
			DISP_MOON3_REG->sft_cfg[14] = 0x00780050; //PLLH post_div = x101-0xxx
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x16 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800780; //PLLH MIPITX CLK = 14.93MHz
			G205_MIPITX_REG1->sft_cfg[11] = 0x00000E00; //TXPLL MIPITX CLK = 350MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00000F00; //TXPLL MIPITX CLK = 375MHz
			G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
			#endif
		} else if ((width == 1280) && (height == 480)) {
			;//TBD
			//DISP_MOON3_REG->sft_cfg[14] = 0x00780028; //PLLH
			//DISP_MOON3_REG->sft_cfg[25] = 0x07800380; //PLLH MIPITX CLK = 49MHz
			//G205_MIPITX_REG1->sft_cfg[11] = 0x00000c00; //TXPLL MIPITX CLK = 300MHz
			//G205_MIPITX_REG1->sft_cfg[12] = 0x00000140; //TXPLL BNKSEL = 300MHz -- 640MHz
		} else if ((width == 1280) && (height == 720)) {
		#if 1 //fine tune PLLH clk to fit 74MHz
			DISP_MOON3_REG->sft_cfg[14] = 0x00780038; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x13 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800180; //PLLH MIPITX CLK = 74MHz (just test)
		#else
			DISP_MOON3_REG->sft_cfg[14] = 0x00780040; //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800180; //PLLH MIPITX CLK = 72MHz
		#endif
			G205_MIPITX_REG1->sft_cfg[11] = 0x00012400; //TXPLL MIPITX CLK = 450MHz
		} else if ((width == 1920) && (height == 1080)) {

		#if defined(SP7350_MIPI_DSI_1500M_TEST) //case in TXPLL = 1500M, then PLLH will be 250MHz
			DISP_MOON3_REG->sft_cfg[14] = 0x00780048; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x10 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800080; //PLLH MIPITX CLK = 250MHz (just test)
			G205_MIPITX_REG1->sft_cfg[11] = 0x00001e10; //TXPLL MIPITX CLK = 1500MHz

		#elif defined(SP7350_MIPI_DSI_1200M_TEST) //case in TXPLL = 1200M, then PLLH will be 200MHz
			DISP_MOON3_REG->sft_cfg[14] = 0x00780058; //PLLH
			DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x10 << 7)); //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800080; //PLLH MIPITX CLK = 200MHz (just test)
			G205_MIPITX_REG1->sft_cfg[11] = 0x00003000; //TXPLL MIPITX CLK = 1200MHz
		#else
			#if 1 //fine tune PLLH clk to fit 148.5MHz
				DISP_MOON3_REG->sft_cfg[14] = 0x00780038; //PLLH
				DISP_MOON3_REG->sft_cfg[14] = (0x7f800000 | (0x13 << 7)); //PLLH
				DISP_MOON3_REG->sft_cfg[25] = 0x07800080; //PLLH MIPITX CLK = 148MHz (just test)
				G205_MIPITX_REG1->sft_cfg[11] = 0x00002400; //TXPLL MIPITX CLK = 900MHz
				//G205_MIPITX_REG1->sft_cfg[11] = 0x00002500; //TXPLL MIPITX CLK = 925MHz
				G205_MIPITX_REG1->sft_cfg[12] = 0x00000141; //TXPLL BNKSEL = 640MHz -- 1000MHz
			#else
				DISP_MOON3_REG->sft_cfg[14] = 0x00780040; //PLLH
				DISP_MOON3_REG->sft_cfg[25] = 0x07800080; //PLLH MIPITX CLK = 143MHz (use this)
				G205_MIPITX_REG1->sft_cfg[11] = 0x00002400; //TXPLL MIPITX CLK = 900MHz
				//G205_MIPITX_REG1->sft_cfg[11] = 0x00002500; //TXPLL MIPITX CLK = 925MHz
				G205_MIPITX_REG1->sft_cfg[12] = 0x00000141; //TXPLL BNKSEL = 640MHz -- 1000MHz
			#endif
		#endif

		} else if ((width == 3840) && (height == 2880)) { // 3840x2880
			DISP_MOON3_REG->sft_cfg[14] = 0x007800420; //PLLH
			DISP_MOON3_REG->sft_cfg[25] = 0x07800000; //PLLH MIPITX CLK = 430MHz max
			G205_MIPITX_REG1->sft_cfg[11] = 0x00003F00; //TXPLL MIPITX CLK = 1600MHz
		} else {
			printf("TBD mipitx pllclk setting\n");
		}
	}
}

void DRV_mipitx_Init(int is_mipi_dsi_tx, int width, int height)
{
	int lane;

	if ((width == 240) && (height == 320))
		lane = 1;
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
	else if ((width == 800) && (height == 480))
		lane = 1;
#endif
	//if ((width == xxx) && (height == xxx))
	//	lane = 2;
	else if ((width == 480) && (height == 1280))
		lane = 4;
	else if ((width == 800) && (height == 1280))
		lane = 4;
	else
		lane = 4;

	G205_MIPITX_REG1->sft_cfg[6] = 0x00101334; //PHY Reset(under reset) & falling edge

#ifdef SP7350_MIPI_DSI_TIMING_NEW
	// MIPITX  LANE CLOCK DATA Timing
	G204_MIPITX_REG0->sft_cfg[5] = 0x00100010; //fix-modify
	G204_MIPITX_REG0->sft_cfg[6] = 0x00100010; //fix
	G204_MIPITX_REG0->sft_cfg[7] = 0x0a120020; //fix
	G204_MIPITX_REG0->sft_cfg[8] = 0x1e080015; //fix-modify

	// MIPITX Blanking Mode
	G204_MIPITX_REG0->sft_cfg[13] = 0x00001100; //fix
#endif

	if (lane == 1)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000001; //lane num = 1 and DSI_EN and ANALOG_EN
	else if (lane == 2)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000011; //lane num = 2 and DSI_EN and ANALOG_EN
	else //if (lane == 4)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000031; //lane num = 4 and DSI_EN and ANALOG_EN

	if ((width == 240) && (height == 320))
		G204_MIPITX_REG0->sft_cfg[12] = 0x00000030; //vtf = sync pluse RGB888
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
	else if ((width == 800) && (height == 480))
		G204_MIPITX_REG0->sft_cfg[12] = 0x00000030; //vtf = sync pluse RGB888
#endif
	else if ((width == 800) && (height == 1280))
		G204_MIPITX_REG0->sft_cfg[12] = 0x00000030; //vtf = sync pluse RGB888
	else //if ((width == 480) && (height == 1280))
		G204_MIPITX_REG0->sft_cfg[12] = 0x00001030; //vtf = sync event RGB888

	G205_MIPITX_REG1->sft_cfg[6] = 0x00101335; //PHY Reset(under normal mode)

	DRV_mipitx_pllclk_init();

	if (is_mipi_dsi_tx) {
		//MIPITX DSI host send command to panel at DATA LANE0 LP mode 
		DRV_mipitx_pllclk_set(SP_MIPITX_LP_MODE, width, height); //set pll clk for LP mode

		G204_MIPITX_REG0->sft_cfg[14] = 0x00100000; //enable command transfer at LP mode

		if (lane == 1)
			G204_MIPITX_REG0->sft_cfg[15] = 0x11000003; //command mode start
		else if (lane == 2)
			G204_MIPITX_REG0->sft_cfg[15] = 0x11000013; //command mode start
		else //if (lane == 4)
			G204_MIPITX_REG0->sft_cfg[15] = 0x11000033; //command mode start

		G204_MIPITX_REG0->sft_cfg[17] = 0x00520004; //TA GET/SURE/GO
		//G204_MIPITX_REG0->sft_cfg[29] = 0x0000c350; //fix
		//G204_MIPITX_REG0->sft_cfg[29] = 0x000000af; //fix

		//transfer data from TX to RX (depends on panel manufacturer)
		DRV_mipitx_panel_Init(is_mipi_dsi_tx, width, height);
	}

	/*
	 * MIPITX Video Mode Setting
	 */
	DRV_mipitx_pllclk_set(SP_MIPITX_HS_MODE, width, height); //set pll clk for HS mode

	// MIPITX  Video Mode Horizontal/Vertial Timing
	G204_MIPITX_REG0->sft_cfg[2] |= height;
	if ((width == 720) && (height == 480)) { // 720x480
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010823; //VSA=0x01 VFP=0x08 VBP=0x23
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
	} else if ((width == 800) && (height == 480)) { // 800x480
		G204_MIPITX_REG0->sft_cfg[0] = 0x70080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00020715; //VSA=0x02 VFP=0x07 VBP=0x15
		//G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		//G204_MIPITX_REG0->sft_cfg[1] = 0x00010823; //VSA=0x01 VFP=0x08 VBP=0x23
#endif
	} else if ((width == 480) && (height == 1280)) { // 480x1280
		#if 1 //update for sync drm setting
		G204_MIPITX_REG0->sft_cfg[0] = 0x0408000c; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x0004100c; //VSA=0x01 VFP=0x11 VBP=0x10
		#else
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080004; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00011110; //VSA=0x01 VFP=0x11 VBP=0x10
		//G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		//G204_MIPITX_REG0->sft_cfg[1] = 0x00010823; //VSA=0x01 VFP=0x08 VBP=0x23
		#endif
	} else if ((width == 800) && (height == 1280)) { // 800x1280
		G204_MIPITX_REG0->sft_cfg[0] = 0x0a06102d; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x0001100f; //VSA=0x01 VFP=0x10 VBP=0x0f
	} else if ((width == 240) && (height == 320)) { // 240x320
		#if 1 //fine tune for screen flicker
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010819; //VSA=0x01 VFP=0x08 VBP=0x19
		#else
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010823; //VSA=0x01 VFP=0x08 VBP=0x23
		#endif
	} else if ((width == 1280) && (height == 480)) { // 1280x480
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010823; //VSA=0x01 VFP=0x08 VBP=0x23
	} else if ((width == 1280) && (height == 720)) { // 1280x720
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010418; //VSA=0x01 VFP=0x04 VBP=0x18
	} else if ((width == 1920) && (height == 1080)) { // 1920x1080
		#if 1
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010328; //VSA=0x01 VFP=0x03 VBP=0x28
		#else
		G204_MIPITX_REG0->sft_cfg[0] = 0x04110004; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00011615; //VSA=0x01 VFP=0x16 VBP=0x15
		#endif
	} else if ((width == 3840) && (height == 2880)) { // 3840x2880
		G204_MIPITX_REG0->sft_cfg[0] = 0x04080005; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010628; //VSA=0x01 VFP=0x06 VBP=0x28
	} else {
		printf("TBD mipitx common setting\n");
	}
	// MIPITX  Video Mode WordCount Setting
	G204_MIPITX_REG0->sft_cfg[19] |= (width << 16 | (width * 24 / 8)) ;

#ifndef SP7350_MIPI_DSI_TIMING_NEW
	// MIPITX  LANE CLOCK DATA Timing
	G204_MIPITX_REG0->sft_cfg[5] = 0x00100008; //fix
	G204_MIPITX_REG0->sft_cfg[6] = 0x00100010; //fix
	G204_MIPITX_REG0->sft_cfg[7] = 0x0a120020; //fix
	G204_MIPITX_REG0->sft_cfg[8] = 0x0a050010; //fix

	// MIPITX Blanking Mode
	G204_MIPITX_REG0->sft_cfg[13] = 0x00001100; //fix
#endif
	// MIPITX CLOCK CONTROL
	G204_MIPITX_REG0->sft_cfg[30] = 0x00000001; //fix

	// MIPITX SWITCH to Video Mode
	if (lane == 1)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000001; //video mode
	else if (lane == 2)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000011; //video mode
	else //if (lane == 4)
		G204_MIPITX_REG0->sft_cfg[15] = 0x11000031; //video mode

}

void DRV_mipitx_Init_1(int is_mipi_dsi_tx, int width, int height)
{
	G205_MIPITX_REG1->sft_cfg[6] = 0x00101330; //PHY Reset(under reset)

#ifdef SP7350_MIPI_DSI_TIMING_NEW
	// MIPITX  LANE CLOCK DATA Timing
	G204_MIPITX_REG0->sft_cfg[5] = 0x00100010; //fix-modify
	G204_MIPITX_REG0->sft_cfg[6] = 0x00100010; //fix
	G204_MIPITX_REG0->sft_cfg[7] = 0x0a120020; //fix
	G204_MIPITX_REG0->sft_cfg[8] = 0x1e080015; //fix-modify

	// MIPITX Blanking Mode
	G204_MIPITX_REG0->sft_cfg[13] = 0x00001100; //fix
#endif

	//G204_MIPITX_REG0->sft_cfg[15] = 0x11000001; //lane num = 1 and DSI_EN and ANALOG_EN
	//G204_MIPITX_REG0->sft_cfg[15] = 0x11000011; //lane num = 2 and DSI_EN and ANALOG_EN
	G204_MIPITX_REG0->sft_cfg[15] = 0x11000031; //lane num = 4 and DSI_EN and ANALOG_EN

	//G204_MIPITX_REG0->sft_cfg[12] = 0x00000030; //vtf = sync pluse
	G204_MIPITX_REG0->sft_cfg[12] = 0x00001030; //vtf = sync event

	G205_MIPITX_REG1->sft_cfg[6] = 0x00101331; //PHY Reset(under normal mode)

	DRV_mipitx_pllclk_init();

	/*
	 * MIPITX Video Mode Setting
	 */
	DRV_mipitx_pllclk_set(SP_MIPITX_HS_MODE, width, height); //set pll clk for HS mode

	// MIPITX  Video Mode Horizontal/Vertial Timing
	G204_MIPITX_REG0->sft_cfg[2] |= height;
	if ((width == 720) && (height == 480)) { // 720x480
		G204_MIPITX_REG0->sft_cfg[0] = 0x3E01003c; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x0006091e; //VSA=0x01 VFP=0x08 VBP=0x23
	} else if ((width == 1280) && (height == 720)) { // 1280x720
		#if 1
		G204_MIPITX_REG0->sft_cfg[0] = 0x280000dc; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00050514; //VSA=0x05 VFP=0x05 VBP=0x14
		#else
		G204_MIPITX_REG0->sft_cfg[0] = 0x280DC06E; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00010518; //VSA=0x01 VFP=0x05 VBP=0x18
		#endif
	} else if ((width == 1920) && (height == 1080)) { // 1920x1080
		G204_MIPITX_REG0->sft_cfg[0] = 0x2c058094; //fix
		G204_MIPITX_REG0->sft_cfg[1] = 0x00050424; //VSA=0x01 VFP=0x04 VBP=0x28
	} else {
		printf("TBD mipitx common setting\n");
	}
	// MIPITX  Video Mode WordCount Setting
	G204_MIPITX_REG0->sft_cfg[19] |= (width << 16 | (width * 24 / 8)) ;

#ifndef SP7350_MIPI_DSI_TIMING_NEW
	// MIPITX  LANE CLOCK DATA Timing
	G204_MIPITX_REG0->sft_cfg[5] = 0x00100008; //fix
	G204_MIPITX_REG0->sft_cfg[6] = 0x00100010; //fix
	G204_MIPITX_REG0->sft_cfg[7] = 0x0a120020; //fix
	G204_MIPITX_REG0->sft_cfg[8] = 0x0a050010; //fix

	// MIPITX Blanking Mode
	G204_MIPITX_REG0->sft_cfg[13] = 0x00001100; //fix
#endif

	// MIPITX CLOCK CONTROL
	G204_MIPITX_REG0->sft_cfg[30] = 0x00000001; //fix

	// MIPITX SWITCH to Video Mode
	G204_MIPITX_REG0->sft_cfg[15] = 0x11000031; //video mode
}

#define MIPITX_CMD_FIFO_FULL 0x00000001
#define MIPITX_CMD_FIFO_EMPTY 0x00000010
#define MIPITX_DATA_FIFO_FULL 0x00000100
#define MIPITX_DATA_FIFO_EMPTY 0x00001000

void check_cmd_fifo_full(void)
{
	u32 value = 0;
	int mipitx_fifo_timeout = 0;

	value = G204_MIPITX_REG0->sft_cfg[16];
	//printf("fifo_status 0x%08x\n", value);
	while((value & MIPITX_CMD_FIFO_FULL) == MIPITX_CMD_FIFO_FULL) {
		if(mipitx_fifo_timeout > 10000) //over 1 second
		{
			printf("cmd fifo full timeout\n");
			break;
		}
		value = G204_MIPITX_REG0->sft_cfg[16];
		++mipitx_fifo_timeout;
		udelay(100);
	}
}

void check_cmd_fifo_empty(void)
{
	u32 value = 0;
	int mipitx_fifo_timeout = 0;

	value = G204_MIPITX_REG0->sft_cfg[16];
	//printf("fifo_status 0x%08x\n", value);
	while((value & MIPITX_CMD_FIFO_EMPTY) != MIPITX_CMD_FIFO_EMPTY) {
		if(mipitx_fifo_timeout > 10000) //over 1 second
		{
			printf("cmd fifo empty timeout\n");
			break;
		}
		value = G204_MIPITX_REG0->sft_cfg[16];
		++mipitx_fifo_timeout;
		udelay(100);
	}
}

void check_data_fifo_full(void)
{
	u32 value = 0;
	int mipitx_fifo_timeout = 0;

	value = G204_MIPITX_REG0->sft_cfg[16];
	//printf("fifo_status 0x%08x\n", value);
	while((value & MIPITX_DATA_FIFO_FULL) == MIPITX_DATA_FIFO_FULL) {
		if(mipitx_fifo_timeout > 10000) //over 1 second
		{
			printf("data fifo full timeout\n");
			break;
		}
		value = G204_MIPITX_REG0->sft_cfg[16];
		++mipitx_fifo_timeout;
		udelay(100);
	}
}

void check_data_fifo_empty(void)
{
	u32 value = 0;
	int mipitx_fifo_timeout = 0;

	value = G204_MIPITX_REG0->sft_cfg[16];
	//printf("fifo_status 0x%08x\n", value);
	while((value & MIPITX_DATA_FIFO_EMPTY) != MIPITX_DATA_FIFO_EMPTY) {
		if(mipitx_fifo_timeout > 10000) //over 1 second
		{
			printf("data fifo empty timeout\n");
			break;
		}
		value = G204_MIPITX_REG0->sft_cfg[16];
		++mipitx_fifo_timeout;
		udelay(100);
	}
}

#if 1
/*
 * MIPI DSI (Display Command Set) for SP7350
 */
void sp7350_dcs_write_buf(const void *data, size_t len)
{
	int i;
	u8 *data1;
	u32 value, data_cnt;

	data1 = (u8 *)data;

	udelay(100);
	if (len == 0) {
		check_cmd_fifo_full();
		value = 0x00000003;
		G204_MIPITX_REG0->sft_cfg[9] = value; //G204.09
	} else if (len == 1) {
		check_cmd_fifo_full();
		value = 0x00000013 | ((u32)data1[0] << 8);
		G204_MIPITX_REG0->sft_cfg[9] = value; //G204.09
	} else if (len == 2) {
		check_cmd_fifo_full();
		value = 0x00000023 | ((u32)data1[0] << 8) | ((u32)data1[1] << 16);
		G204_MIPITX_REG0->sft_cfg[9] = value; //G204.09
	//} else if ((len >= 3) && (len <= 64)) {
	} else if (len >= 3) {
		check_cmd_fifo_full();
		value = 0x00000029 | ((u32)len << 8);
		G204_MIPITX_REG0->sft_cfg[10] = value; //G204.10
		if (len % 4) data_cnt = ((u32)len / 4) + 1;
		else data_cnt = ((u32)len / 4);

		for (i = 0; i < data_cnt; i++) {
			check_data_fifo_full();
			value = 0x00000000;
			//if (i * 4 + 0 >= len) data1[i * 4 + 0] = 0x00;
			//if (i * 4 + 1 >= len) data1[i * 4 + 1] = 0x00;
			//if (i * 4 + 2 >= len) data1[i * 4 + 2] = 0x00;
			//if (i * 4 + 3 >= len) data1[i * 4 + 3] = 0x00;
			//value |= (((u32)data1[i * 4 + 3] << 24) | ((u32)data1[i * 4 + 2] << 16) |
			//	 ((u32)data1[i * 4 + 1] << 8) | ((u32)data1[i * 4 + 0] << 0));
			if (i * 4 + 0 < len) value |= (data1[i * 4 + 0] << 0);
			if (i * 4 + 1 < len) value |= (data1[i * 4 + 1] << 8);
			if (i * 4 + 2 < len) value |= (data1[i * 4 + 2] << 16);
			if (i * 4 + 3 < len) value |= (data1[i * 4 + 3] << 24);
			G204_MIPITX_REG0->sft_cfg[11] = value; //G204.11
		}

	} else {
		printf("data length over %ld\n", len);
	}
}

#define sp7350_dcs_write_seq(seq...)			\
({							\
	const u8 d[] = { seq };			\
	sp7350_dcs_write_buf(d, ARRAY_SIZE(d));	\
})
#endif

/*
 * G204_MIPITX_REG0->sft_cfg[9] = 0x00000003; //command mode short pkt with 0 para
 * G204_MIPITX_REG0->sft_cfg[9] = 0x0000aa13; //command mode short pkt with 1 para
 * G204_MIPITX_REG0->sft_cfg[9] = 0x00bbaa23; //command mode short pkt with 2 para
 * 
 * G204_MIPITX_REG0->sft_cfg[10] = 0x00000029; //command mode long pkt
 * G204_MIPITX_REG0->sft_cfg[11] = 0xddccbbaa; //command mode payload
 */
void DRV_mipitx_panel_Init(int is_mipi_dsi_tx, int width, int height)
{
	if (is_mipi_dsi_tx) {
		if ((width == 800) && (height == 1280)) {

			printf("MIPITX DSI Panel : GM-70P476CK(%dx%d)\n", width, height);
			//Panel GM-70P476CK
			DRV_mipitx_gpio_set(( struct sp7350_disp_priv *)sp_gpio, 1);

			sp7350_dcs_write_seq(0xE0, 0x00);
			sp7350_dcs_write_seq(0xE1, 0x93);
			sp7350_dcs_write_seq(0xE2, 0x65);
			sp7350_dcs_write_seq(0xE3, 0xF8);
			sp7350_dcs_write_seq(0x80, 0x03);
			sp7350_dcs_write_seq(0xE0, 0x01);
			sp7350_dcs_write_seq(0x00, 0x00);
			sp7350_dcs_write_seq(0x01, 0x3D); //VCOM
			sp7350_dcs_write_seq(0x03, 0x10);
			sp7350_dcs_write_seq(0x04, 0x43);
			sp7350_dcs_write_seq(0x0C, 0x74);
			sp7350_dcs_write_seq(0x17, 0x00);
			sp7350_dcs_write_seq(0x18, 0xEF);
			sp7350_dcs_write_seq(0x19, 0x01);
			sp7350_dcs_write_seq(0x1A, 0x00);
			sp7350_dcs_write_seq(0x1B, 0xEF);
			sp7350_dcs_write_seq(0x1C, 0x01);
			sp7350_dcs_write_seq(0x24, 0xFE);
			sp7350_dcs_write_seq(0x37, 0x09);
			sp7350_dcs_write_seq(0x38, 0x04);
			sp7350_dcs_write_seq(0x39, 0x08);
			sp7350_dcs_write_seq(0x3A, 0x12);
			sp7350_dcs_write_seq(0x3C, 0x78);
			sp7350_dcs_write_seq(0x3D, 0xFF);
			sp7350_dcs_write_seq(0x3E, 0xFF);
			sp7350_dcs_write_seq(0x3F, 0x7F);
			sp7350_dcs_write_seq(0x40, 0x06);
			sp7350_dcs_write_seq(0x41, 0xA0);
			sp7350_dcs_write_seq(0x43, 0x14);
			sp7350_dcs_write_seq(0x44, 0x11);
			sp7350_dcs_write_seq(0x45, 0x24);
			sp7350_dcs_write_seq(0x55, 0x02);
			sp7350_dcs_write_seq(0x57, 0x69);
			sp7350_dcs_write_seq(0x59, 0x0A);
			sp7350_dcs_write_seq(0x5A, 0x29);
			sp7350_dcs_write_seq(0x5B, 0x10);

					//G2.2    //G2.5
			sp7350_dcs_write_seq(0x5D, 0x70);  //0x70
			sp7350_dcs_write_seq(0x5E, 0x5B);  //0x58
			sp7350_dcs_write_seq(0x5F, 0x4B);  //0x46
			sp7350_dcs_write_seq(0x60, 0x3F);  //0x39
			sp7350_dcs_write_seq(0x61, 0x3B);  //0x33
			sp7350_dcs_write_seq(0x62, 0x2D);  //0x24
			sp7350_dcs_write_seq(0x63, 0x2F);  //0x26
			sp7350_dcs_write_seq(0x64, 0x18);  //0x10
			sp7350_dcs_write_seq(0x65, 0x2F);  //0x27
			sp7350_dcs_write_seq(0x66, 0x2C);  //0x24
			sp7350_dcs_write_seq(0x67, 0x2B);  //0x22
			sp7350_dcs_write_seq(0x68, 0x47);  //0x3C
			sp7350_dcs_write_seq(0x69, 0x34);  //0x27
			sp7350_dcs_write_seq(0x6A, 0x3B);  //0x2C
			sp7350_dcs_write_seq(0x6B, 0x2E);  //0x1F
			sp7350_dcs_write_seq(0x6C, 0x2A);  //0x1F
			sp7350_dcs_write_seq(0x6D, 0x1F);  //0x15
			sp7350_dcs_write_seq(0x6E, 0x11);  //0x0A
			sp7350_dcs_write_seq(0x6F, 0x02);  //0x02
			sp7350_dcs_write_seq(0x70, 0x70);  //0x70
			sp7350_dcs_write_seq(0x71, 0x5B);  //0x58
			sp7350_dcs_write_seq(0x72, 0x4B);  //0x46
			sp7350_dcs_write_seq(0x73, 0x3F);  //0x39
			sp7350_dcs_write_seq(0x74, 0x3B);  //0x33
			sp7350_dcs_write_seq(0x75, 0x2D);  //0x24
			sp7350_dcs_write_seq(0x76, 0x2F);  //0x26
			sp7350_dcs_write_seq(0x77, 0x18);  //0x10
			sp7350_dcs_write_seq(0x78, 0x2F);  //0x27
			sp7350_dcs_write_seq(0x79, 0x2C);  //0x24
			sp7350_dcs_write_seq(0x7A, 0x2B);  //0x22
			sp7350_dcs_write_seq(0x7B, 0x47);  //0x3C
			sp7350_dcs_write_seq(0x7C, 0x34);  //0x27
			sp7350_dcs_write_seq(0x7D, 0x3B);  //0x2C
			sp7350_dcs_write_seq(0x7E, 0x2E);  //0x1F
			sp7350_dcs_write_seq(0x7F, 0x2A);  //0x1F
			sp7350_dcs_write_seq(0x80, 0x1F);  //0x15
			sp7350_dcs_write_seq(0x81, 0x11);  //0x0A
			sp7350_dcs_write_seq(0x82, 0x02);  //0x02
			sp7350_dcs_write_seq(0xE0, 0x02);
			sp7350_dcs_write_seq(0x00, 0x5E);
			sp7350_dcs_write_seq(0x01, 0x5F);
			sp7350_dcs_write_seq(0x02, 0x57);
			sp7350_dcs_write_seq(0x03, 0x58);
			sp7350_dcs_write_seq(0x04, 0x48);
			sp7350_dcs_write_seq(0x05, 0x4A);
			sp7350_dcs_write_seq(0x06, 0x44);
			sp7350_dcs_write_seq(0x07, 0x46);
			sp7350_dcs_write_seq(0x08, 0x40);
			sp7350_dcs_write_seq(0x09, 0x5F);
			sp7350_dcs_write_seq(0x0A, 0x5F);
			sp7350_dcs_write_seq(0x0B, 0x5F);
			sp7350_dcs_write_seq(0x0C, 0x5F);
			sp7350_dcs_write_seq(0x0D, 0x5F);
			sp7350_dcs_write_seq(0x0E, 0x5F);
			sp7350_dcs_write_seq(0x0F, 0x42);
			sp7350_dcs_write_seq(0x10, 0x5F);
			sp7350_dcs_write_seq(0x11, 0x5F);
			sp7350_dcs_write_seq(0x12, 0x5F);
			sp7350_dcs_write_seq(0x13, 0x5F);
			sp7350_dcs_write_seq(0x14, 0x5F);
			sp7350_dcs_write_seq(0x15, 0x5F);
			sp7350_dcs_write_seq(0x16, 0x5E);
			sp7350_dcs_write_seq(0x17, 0x5F);
			sp7350_dcs_write_seq(0x18, 0x57);
			sp7350_dcs_write_seq(0x19, 0x58);
			sp7350_dcs_write_seq(0x1A, 0x49);
			sp7350_dcs_write_seq(0x1B, 0x4B);
			sp7350_dcs_write_seq(0x1C, 0x45);
			sp7350_dcs_write_seq(0x1D, 0x47);
			sp7350_dcs_write_seq(0x1E, 0x41);
			sp7350_dcs_write_seq(0x1F, 0x5F);
			sp7350_dcs_write_seq(0x20, 0x5F);
			sp7350_dcs_write_seq(0x21, 0x5F);
			sp7350_dcs_write_seq(0x22, 0x5F);
			sp7350_dcs_write_seq(0x23, 0x5F);
			sp7350_dcs_write_seq(0x24, 0x5F);
			sp7350_dcs_write_seq(0x25, 0x43);
			sp7350_dcs_write_seq(0x26, 0x5F);
			sp7350_dcs_write_seq(0x27, 0x5F);
			sp7350_dcs_write_seq(0x28, 0x5F);
			sp7350_dcs_write_seq(0x29, 0x5F);
			sp7350_dcs_write_seq(0x2A, 0x5F);
			sp7350_dcs_write_seq(0x2B, 0x5F);
			sp7350_dcs_write_seq(0x2C, 0x1F);
			sp7350_dcs_write_seq(0x2D, 0x1E);
			sp7350_dcs_write_seq(0x2E, 0x17);
			sp7350_dcs_write_seq(0x2F, 0x18);
			sp7350_dcs_write_seq(0x30, 0x07);
			sp7350_dcs_write_seq(0x31, 0x05);
			sp7350_dcs_write_seq(0x32, 0x0B);
			sp7350_dcs_write_seq(0x33, 0x09);
			sp7350_dcs_write_seq(0x34, 0x03);
			sp7350_dcs_write_seq(0x35, 0x1F);
			sp7350_dcs_write_seq(0x36, 0x1F);
			sp7350_dcs_write_seq(0x37, 0x1F);
			sp7350_dcs_write_seq(0x38, 0x1F);
			sp7350_dcs_write_seq(0x39, 0x1F);
			sp7350_dcs_write_seq(0x3A, 0x1F);
			sp7350_dcs_write_seq(0x3B, 0x01);
			sp7350_dcs_write_seq(0x3C, 0x1F);
			sp7350_dcs_write_seq(0x3D, 0x1F);
			sp7350_dcs_write_seq(0x3E, 0x1F);
			sp7350_dcs_write_seq(0x3F, 0x1F);
			sp7350_dcs_write_seq(0x40, 0x1F);
			sp7350_dcs_write_seq(0x41, 0x1F);
			sp7350_dcs_write_seq(0x42, 0x1F);
			sp7350_dcs_write_seq(0x43, 0x1E);
			sp7350_dcs_write_seq(0x44, 0x17);
			sp7350_dcs_write_seq(0x45, 0x18);
			sp7350_dcs_write_seq(0x46, 0x06);
			sp7350_dcs_write_seq(0x47, 0x04);
			sp7350_dcs_write_seq(0x48, 0x0A);
			sp7350_dcs_write_seq(0x49, 0x08);
			sp7350_dcs_write_seq(0x4A, 0x02);
			sp7350_dcs_write_seq(0x4B, 0x1F);
			sp7350_dcs_write_seq(0x4C, 0x1F);
			sp7350_dcs_write_seq(0x4D, 0x1F);
			sp7350_dcs_write_seq(0x4E, 0x1F);
			sp7350_dcs_write_seq(0x4F, 0x1F);
			sp7350_dcs_write_seq(0x50, 0x1F);
			sp7350_dcs_write_seq(0x51, 0x00);
			sp7350_dcs_write_seq(0x52, 0x1F);
			sp7350_dcs_write_seq(0x53, 0x1F);
			sp7350_dcs_write_seq(0x54, 0x1F);
			sp7350_dcs_write_seq(0x55, 0x1F);
			sp7350_dcs_write_seq(0x56, 0x1F);
			sp7350_dcs_write_seq(0x57, 0x1F);
			sp7350_dcs_write_seq(0x58, 0x40);
			sp7350_dcs_write_seq(0x5B, 0x30);
			sp7350_dcs_write_seq(0x5C, 0x07);
			sp7350_dcs_write_seq(0x5D, 0x30);
			sp7350_dcs_write_seq(0x5E, 0x01);
			sp7350_dcs_write_seq(0x5F, 0x02);
			sp7350_dcs_write_seq(0x60, 0x30);
			sp7350_dcs_write_seq(0x61, 0x03);
			sp7350_dcs_write_seq(0x62, 0x04);
			sp7350_dcs_write_seq(0x63, 0x6A);
			sp7350_dcs_write_seq(0x64, 0x6A);
			sp7350_dcs_write_seq(0x65, 0x35);
			sp7350_dcs_write_seq(0x66, 0x0D);
			sp7350_dcs_write_seq(0x67, 0x73);
			sp7350_dcs_write_seq(0x68, 0x0B);
			sp7350_dcs_write_seq(0x69, 0x6A);
			sp7350_dcs_write_seq(0x6A, 0x6A);
			sp7350_dcs_write_seq(0x6B, 0x08);
			sp7350_dcs_write_seq(0x6C, 0x00);
			sp7350_dcs_write_seq(0x6D, 0x04);
			sp7350_dcs_write_seq(0x6E, 0x00);
			sp7350_dcs_write_seq(0x6F, 0x88);
			sp7350_dcs_write_seq(0x75, 0xBC);
			sp7350_dcs_write_seq(0x76, 0x00);
			sp7350_dcs_write_seq(0x77, 0x0D);
			sp7350_dcs_write_seq(0x78, 0x1B);
			sp7350_dcs_write_seq(0xE0, 0x04);
			sp7350_dcs_write_seq(0x00, 0x0E);
			sp7350_dcs_write_seq(0x02, 0xB3);
			sp7350_dcs_write_seq(0x09, 0x60);
			sp7350_dcs_write_seq(0x0E, 0x4A);
			sp7350_dcs_write_seq(0x37, 0x58); //全志平台add
			sp7350_dcs_write_seq(0x2B, 0x0F); //全志平台add
			sp7350_dcs_write_seq(0xE0, 0x05);
			sp7350_dcs_write_seq(0x15, 0x1D); //全志平台add
			sp7350_dcs_write_seq(0xE0, 0x00);

			sp7350_dcs_write_seq(0x11, 0x00);
			mdelay(120);
			sp7350_dcs_write_seq(0x29, 0x00);
			mdelay(20);
			sp7350_dcs_write_seq(0x35, 0x00);

		} else if ((width == 480) && (height == 1280)) {
			#if 1
			printf("MIPITX DSI Panel : HXM0686TFT-001(%dx%d)\n", width, height);
			//Panel HXM0686TFT-001 IPS
			DRV_mipitx_gpio_set(( struct sp7350_disp_priv *)sp_gpio, 0);

			sp7350_dcs_write_seq(0xB9, 0xF1, 0x12, 0x87);
			sp7350_dcs_write_seq(0xB2, 0x40, 0x05, 0x78);
			sp7350_dcs_write_seq(0xB3, 0x10, 0x10, 0x28, 0x28, 0x03, 0xFF, 0x00,
								0x00, 0x00, 0x00);
			sp7350_dcs_write_seq(0xB4, 0x80);
			sp7350_dcs_write_seq(0xB5, 0x09, 0x09);
			sp7350_dcs_write_seq(0xB6, 0x8D, 0x8D);
			sp7350_dcs_write_seq(0xB8, 0x26, 0x22, 0xF0, 0x63);
			sp7350_dcs_write_seq(0xBA, 0x33, 0x81, 0x05, 0xF9, 0x0E, 0x0E, 0x20,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44,
								0x25, 0x00, 0x91, 0x0A, 0x00, 0x00, 0x01, 0x4F,
								0x01, 0x00, 0x00, 0x37);
			sp7350_dcs_write_seq(0xBC, 0x47);
			sp7350_dcs_write_seq(0xBF, 0x02, 0x10, 0x00, 0x80, 0x04);
			sp7350_dcs_write_seq(0xC0, 0x73, 0x73, 0x50, 0x50, 0x00, 0x00, 0x12,
								0x70, 0x00);
			sp7350_dcs_write_seq(0xC1, 0x65, 0xC0, 0x32, 0x32, 0x77, 0xF4, 0x77,
								0x77, 0xCC, 0xCC, 0xFF, 0xFF, 0x11, 0x11, 0x00,
								0x00, 0x32);
			sp7350_dcs_write_seq(0xC7, 0x10, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00,
								0x00, 0xED, 0xC7, 0x00, 0xA5);
			sp7350_dcs_write_seq(0xC8, 0x10, 0x40, 0x1E, 0x03);
			sp7350_dcs_write_seq(0xCC, 0x0B);
			sp7350_dcs_write_seq(0xE0, 0x00, 0x04, 0x06, 0x32, 0x3F, 0x3F, 0x36,
								0x34, 0x06, 0x0B, 0x0E, 0x11, 0x11, 0x10, 0x12,
								0x10, 0x13, 0x00, 0x04, 0x06, 0x32, 0x3F, 0x3F,
								0x36, 0x34, 0x06, 0x0B, 0x0E, 0x11, 0x11, 0x10,
								0x12, 0x10, 0x13);
			sp7350_dcs_write_seq(0xE1, 0x11, 0x11, 0x91, 0x00, 0x00, 0x00, 0x00);
			sp7350_dcs_write_seq(0xE3, 0x03, 0x03, 0x0B, 0x0B, 0x00, 0x03, 0x00,
								0x00, 0x00, 0x00, 0xFF, 0x84, 0xC0, 0x10);
			sp7350_dcs_write_seq(0xE9, 0xC8, 0x10, 0x06, 0x05, 0x18, 0xD2, 0x81,
								0x12, 0x31, 0x23, 0x47, 0x82, 0xB0, 0x81, 0x23,
								0x04, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00,
								0x04, 0x04, 0x00, 0x00, 0x00, 0x88, 0x0B, 0xA8,
								0x10, 0x32, 0x4F, 0x88, 0x88, 0x88, 0x88, 0x88,
								0x88, 0x0B, 0xA8, 0x10, 0x32, 0x4F, 0x88, 0x88,
								0x88, 0x88, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
			sp7350_dcs_write_seq(0xEA, 0x96, 0x0C, 0x01, 0x01, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x4B, 0xA8,
								0x23, 0x01, 0x08, 0xF8, 0x88, 0x88, 0x88, 0x88,
								0x88, 0x4B, 0xA8, 0x23, 0x01, 0x08, 0xF8, 0x88,
								0x88, 0x88, 0x88, 0x23, 0x10, 0x00, 0x00, 0x92,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
								0x80, 0x81, 0x00, 0x00, 0x00, 0x00);
			sp7350_dcs_write_seq(0xEF, 0xFF, 0xFF, 0x01);

			sp7350_dcs_write_seq(0x36, 0x14);
			sp7350_dcs_write_seq(0x35, 0x00);
			sp7350_dcs_write_seq(0x11);
			mdelay(120);
			sp7350_dcs_write_seq(0x29);
			mdelay(20);
			#else
			printf("MIPITX DSI Panel : HXM0686TFT-001(%dx%d)\n", width, height);
			//Panel HXM0686TFT-001 IPS
			DRV_mipitx_gpio_set(( struct sp7350_disp_priv *)sp_gpio, 0);

			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x8712F1B9; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x780540B2; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000b29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x281010B3; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00ff0328; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0080B423; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x000909B5; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x008D8DB6; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000529; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0xF02226B8; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000063; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00001c29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x058133BA; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x200E0EF9; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x44000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0A910025; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x4F010000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x37000001; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0047BC23; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000629; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x001002BF; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000480; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000a29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x507373C0; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x12000050; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000070; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00001229; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x32C065C1; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x77F47732; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0xFFCCCC77; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x001111FF; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00003200; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000d29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x0A0010C7; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00C7ED00; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x000000A5; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000529; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x1E4010C8; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000003; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x000BCC23; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00002329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x060400E0; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x363F3F32; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0E0B0634; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x12101111; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x04001310; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x3F3F3206; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0B063436; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x1011110E; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00131012; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000829; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x911111E1; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000f29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x0B0303E3; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0003000B; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0xFF000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0010C084; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00004029; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x0610C8E9; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x81D21805; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x47233112; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x2381B082; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x04040004; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000404; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0xA80B8800; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x884F3210; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x88888888; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x10A80B88; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x88884F32; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00888888; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00003E29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x010C96EA; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000001; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0xA84B8800; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0xF8080123; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x88888888; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x23A84B88; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x88F80801; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x23888888; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x92000010; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x20000000; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00008180; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000000; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x01FFFFEF; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x00143623; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();

			G204_MIPITX_REG0->sft_cfg[9] = 0x00003523; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x00001113; //G204.09

			mdelay(120);

			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x00002913; //G204.09

			mdelay(20);
			#endif
		} else if ((width == 240) && (height == 320)) {
			#if 1
			pr_info("MIPITX DSI Panel : TCXD024IBLON-2(%dx%d)\n", width, height);
			//Panel TCXD024IBLON-2 240x320
			//sp7350_mipitx_gpio_set();

			sp7350_dcs_write_seq(0xDF, 0x98, 0x51, 0xE9);

			/* Page 00 setting */
			sp7350_dcs_write_seq(0xDE, 0x00);
			sp7350_dcs_write_seq(0xB7, 0x22, 0x85, 0x22, 0x2B);
			sp7350_dcs_write_seq(0xC8, 0x3F, 0x38, 0x33, 0x2F, 0x32, 0x34, 0x2F,
								0x2F, 0x2D, 0x2C, 0x27, 0x1A, 0x14, 0x0A, 0x06,
								0x0E, 0x3F, 0x38, 0x33, 0x2F, 0x32, 0x34, 0x2F,
								0x2F, 0x2D, 0x2C, 0x27, 0x1A, 0x14, 0x0A, 0x06,
								0x0E);
			sp7350_dcs_write_seq(0xB9, 0x33, 0x08, 0xCC);
			sp7350_dcs_write_seq(0xBB, 0x46, 0x7A, 0x30, 0x40, 0x7C, 0x60, 0x70,
								0x70);
			sp7350_dcs_write_seq(0xBC, 0x38, 0x3C);
			sp7350_dcs_write_seq(0xC0, 0x31, 0x20);
			sp7350_dcs_write_seq(0xC1, 0x12);
			sp7350_dcs_write_seq(0xC3, 0x08, 0x00, 0x0A, 0x10, 0x08, 0x54, 0x45,
								0x71, 0x2C);
			sp7350_dcs_write_seq(0xC4, 0x00, 0xA0, 0x79, 0x0E, 0x0A, 0x16, 0x79,
								0x0E, 0x0A, 0x16, 0x79, 0x0E, 0x0A, 0x16, 0x82,
								0x00, 0x03);
			sp7350_dcs_write_seq(0xD0, 0x04, 0x0C, 0x6B, 0x0F, 0x07, 0x03);
			sp7350_dcs_write_seq(0xD7, 0x13, 0x00);

			/* Page 02 setting */
			sp7350_dcs_write_seq(0xDE, 0x02);
			mdelay(1);
			sp7350_dcs_write_seq(0xB8, 0x1D, 0xA0, 0x2F, 0x04, 0x33);
			sp7350_dcs_write_seq(0xC1, 0x10, 0x66, 0x66, 0x01);

			/* Page 00 setting */
			sp7350_dcs_write_seq(0xDE, 0x00);
			//sp7350_dcs_write_seq(0xC2, 0x08); /* BIST_EN */
			sp7350_dcs_write_seq(0x11);
			mdelay(120);

			/* Page 02 setting */
			sp7350_dcs_write_seq(0xDE, 0x02);
			mdelay(1);
			sp7350_dcs_write_seq(0xC5, 0x4E, 0x00, 0x00);
			mdelay(1);
			sp7350_dcs_write_seq(0xCA, 0x30, 0x20, 0xF4);
			mdelay(1);

			/* Page 04 setting */
			sp7350_dcs_write_seq(0xDE, 0x04);
			mdelay(1);
			sp7350_dcs_write_seq(0xD3, 0x3C);
			mdelay(1);

			/* Page 00 setting */
			sp7350_dcs_write_seq(0xDE, 0x00);
			mdelay(1);
			sp7350_dcs_write_seq(0x29);
			#else
			printf("MIPITX DSI Panel : TCXD024IBLON-2(%dx%d)\n", width, height);
			//Panel TCXD024IBLON-2
			//DRV_mipitx_gpio_set(( struct sp7350_disp_priv *)sp_gpio, 0);

			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0xE95198DF; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0000DE23; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000529; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x228522B7; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0000002B; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00002129; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x33383FC8; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x2F34322F; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x272C2D2F; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x060A141A; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x33383F0E; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x2F34322F; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x272C2D2F; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x060A141A; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0000000E; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0xCC0833B9; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000929; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x307A46BB; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x70607C40; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000070; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x003C38BC; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x002031C0; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0012C123; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000a29; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x0A0008C3; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x45540810; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00002C71; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00001229; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x79A000C4; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x79160A0E; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x79160A0E; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x82160A0E; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000300; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000729; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x6B0C04D0; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x0003070F; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000329; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x000013D7; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0002DE23; //G204.09
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000629; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x2FA01DB8; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00003304; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000529; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x666610C1; //G204.11
			G204_MIPITX_REG0->sft_cfg[11] = 0x00000001; //G204.11
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0000DE23; //G204.09
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			//G204_MIPITX_REG0->sft_cfg[9] = 0x0008C223; //G204.09 BIST EN
			//check_cmd_fifo_empty();
			//check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x00001113; //G204.09

			mdelay(120);

			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0002DE23; //G204.09
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0x00004EC5; //G204.11
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[10] = 0x00000429; //G204.10
			G204_MIPITX_REG0->sft_cfg[11] = 0xF42030CA; //G204.11
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0004DE23; //G204.09
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x003CD323; //G204.09
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x0000DE23; //G204.09
			mdelay(1);
			check_cmd_fifo_empty();
			check_data_fifo_empty();
			G204_MIPITX_REG0->sft_cfg[9] = 0x00002913; //G204.09

			//mdelay(20);
			#endif
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
		} else if ((width == 800) && (height == 480)) {
			//printf("MIPITX DSI Panel : RASPBERRYPI_DSI_PANEL(%dx%d)\n", width, height);

			/*read i2c REG_ID*/
			/*write i2c REG_POWERON = 0 turn off*/
			/*write i2c REG_POWERON = 1 turn on*/
			/*write i2c REG_PORTB*/

			sp7350_dcs_write_seq(0x10, 0x02, 0x03, 0x00, 0x00, 0x00); /*dsi DSI_LANEENABLE = 0x0210*/
			sp7350_dcs_write_seq(0x64, 0x01, 0x05, 0x00, 0x00, 0x00); /*dsi PPI_D0S_CLRSIPOCOUNT = 0x0164*/
			sp7350_dcs_write_seq(0x68, 0x01, 0x05, 0x00, 0x00, 0x00); /*dsi PPI_D1S_CLRSIPOCOUNT = 0x0168*/
			sp7350_dcs_write_seq(0x44, 0x01, 0x00, 0x00, 0x00, 0x00); /*dsi PPI_D0S_ATMR = 0x0144*/
			sp7350_dcs_write_seq(0x48, 0x01, 0x00, 0x00, 0x00, 0x00); /*dsi PPI_D1S_ATMR = 0x0148*/
			sp7350_dcs_write_seq(0x14, 0x01, 0x03, 0x00, 0x00, 0x00); /*dsi PPI_LPTXTIMECNT = 0x0114*/

			sp7350_dcs_write_seq(0x50, 0x04, 0x00, 0x00, 0x00, 0x00); /*dsi SPICMR = 0x0450*/
			sp7350_dcs_write_seq(0x20, 0x04, 0x50, 0x01, 0x10, 0x00); /*dsi LCDCTRL = 0x0420*/
			sp7350_dcs_write_seq(0x64, 0x04, 0x0f, 0x04, 0x00, 0x00); /*dsi SYSCTRL = 0x0464*/
			mdelay(100); //msleep(100);
			sp7350_dcs_write_seq(0x04, 0x01, 0x01, 0x00, 0x00, 0x00); /*dsi PPI_STARTPPI = 0x0104*/
			sp7350_dcs_write_seq(0x04, 0x02, 0x01, 0x00, 0x00, 0x00); /*dsi DSI_STARTDSI = 0x0204*/
			mdelay(100); //msleep(100);

			/*write i2c REG_PWM = 255*/
			/*write i2c REG_PORTA = 0x04*/
#endif
		}
	} else {
		printf("DRV_mipitx_panel_Init for CSI(none)\n");
	}
}
