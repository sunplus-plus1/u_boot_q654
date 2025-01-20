// SPDX-License-Identifier: GPL-2.0
/*
 * Author(s): Hammer Hsieh <hammer.hsieh@sunplus.com>
 */

#include <common.h>
#include "reg_disp.h"
#include "disp_tcon.h"
/**************************************************************************
 *             F U N C T I O N    I M P L E M E N T A T I O N S           *
 **************************************************************************/
void DRV_TCON_Init(int width, int height)
{
	//printf("DRV_TCON_Init w %d h %d\n", width, height);

	G199_TCON0_REG0->sft_cfg[0] = 0x00008127;
	G199_TCON0_REG0->sft_cfg[1] = 0x00008011;
	G199_TCON0_REG0->sft_cfg[2] = 0x00000011;
	G199_TCON0_REG0->sft_cfg[3] = 0x00002002;

	if ((width == 720) && (height == 480)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x00000352;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000356;
		G199_TCON0_REG0->sft_cfg[20] = 0x0000020c;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x00000352;
		G199_TCON0_REG0->sft_cfg[24] = 0x00000356;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000002cf;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	}
	else if ((width == 800) && (height == 480)) {
#if CONFIG_IS_ENABLED(DM_I2C) && defined(CONFIG_SP7350_RASPBERRYPI_DSI_PANEL)
		#if 1 //update for sync drm setting
		G199_TCON0_REG0->sft_cfg[12] = 0x0000038d;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000391;
		G199_TCON0_REG0->sft_cfg[20] = 0x000001fd;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x0000038d;
		G199_TCON0_REG0->sft_cfg[24] = 0x00000391;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x0000031f;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
		#else
		G199_TCON0_REG0->sft_cfg[12] = 0x00000361;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000365;
		G199_TCON0_REG0->sft_cfg[20] = 0x000001fd;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x00000361;
		G199_TCON0_REG0->sft_cfg[24] = 0x00000365;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x0000031f;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
		#endif
#else
	;//TBD
#endif
	}
	else if ((width == 1024) && (height == 600)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x00000538;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000538;
		G199_TCON0_REG0->sft_cfg[20] = 0x0000027a;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x00000538;
		G199_TCON0_REG0->sft_cfg[24] = 0x0000053c;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000003ff;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	} else if ((width == 480) && (height == 1280)) {
		#if 1 //update for sync drm setting
		G199_TCON0_REG0->sft_cfg[12] = 0x0000029e;//0x00000269;
		G199_TCON0_REG0->sft_cfg[13] = 0x000002a2;//0x00000269;
		G199_TCON0_REG0->sft_cfg[20] = 0x0000051f;//0x0000052d;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x0000029e;//0x00000269;
		G199_TCON0_REG0->sft_cfg[24] = 0x000002a2;//0x0000026d;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000001df;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
		#else
		G199_TCON0_REG0->sft_cfg[12] = 0x00000264;//0x00000269;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000268;//0x00000269;
		G199_TCON0_REG0->sft_cfg[20] = 0x00000521;//0x0000052d;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x00000264;//0x00000269;
		G199_TCON0_REG0->sft_cfg[24] = 0x00000268;//0x0000026d;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000001df;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
		#endif
	} else if ((width == 800) && (height == 1280)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x000003B0; //944
		G199_TCON0_REG0->sft_cfg[13] = 0x000003B4; //948
		G199_TCON0_REG0->sft_cfg[20] = 0x0000051f; //1311
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000; //0

		G199_TCON0_REG0->sft_cfg[23] = 0x000003B0; //944
		G199_TCON0_REG0->sft_cfg[24] = 0x000003B4; //948

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000; //0
		G199_TCON0_REG0->sft_cfg[26] = 0x0000031f; //799
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	} else if ((width == 240) && (height == 320)) {
		#if 1 //fine tune for screen flicker
		G199_TCON0_REG0->sft_cfg[12] = 0x000002A3;
		G199_TCON0_REG0->sft_cfg[13] = 0x000002A7;
		G199_TCON0_REG0->sft_cfg[20] = 0x00000161;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x000002A3;
		G199_TCON0_REG0->sft_cfg[24] = 0x000002A7;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000000EF;
		#else
		G199_TCON0_REG0->sft_cfg[12] = 0x000002A3;
		G199_TCON0_REG0->sft_cfg[13] = 0x000002A7;
		G199_TCON0_REG0->sft_cfg[20] = 0x0000016B;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x000002A3;
		G199_TCON0_REG0->sft_cfg[24] = 0x000002A7;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000000EF;
		#endif
	} else if ((width == 1280) && (height == 480)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x0000060a;
		G199_TCON0_REG0->sft_cfg[13] = 0x0000060a;
		G199_TCON0_REG0->sft_cfg[20] = 0x0000020c;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x0000060a;
		G199_TCON0_REG0->sft_cfg[24] = 0x0000060e;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000004ff;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	} else if ((width == 1280) && (height == 720)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x0000066A;
		G199_TCON0_REG0->sft_cfg[13] = 0x0000066E;
		G199_TCON0_REG0->sft_cfg[20] = 0x000002ED;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x0000066A;
		G199_TCON0_REG0->sft_cfg[24] = 0x0000066E;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x000004ff;
		//G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		//G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	}
	else if ((width == 1920) && (height == 1080)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x00000890;
		G199_TCON0_REG0->sft_cfg[13] = 0x00000894;
		G199_TCON0_REG0->sft_cfg[20] = 0x00000464;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x00000890;
		G199_TCON0_REG0->sft_cfg[24] = 0x00000894;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x0000077f;
		G199_TCON0_REG0->sft_cfg[27] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[28] = 0x0000077f;
	}
	else if ((width == 3840) && (height == 2880)) {
		G199_TCON0_REG0->sft_cfg[12] = 0x000011f8;
		G199_TCON0_REG0->sft_cfg[13] = 0x000011f8;
		G199_TCON0_REG0->sft_cfg[20] = 0x00000c7f;
		G199_TCON0_REG0->sft_cfg[21] = 0x00000000;

		G199_TCON0_REG0->sft_cfg[23] = 0x000011f8;
		G199_TCON0_REG0->sft_cfg[24] = 0x000011fc;

		G199_TCON0_REG0->sft_cfg[25] = 0x00000000;
		G199_TCON0_REG0->sft_cfg[26] = 0x00000eff;
		G199_TCON0_REG0->sft_cfg[27] = 0x00000028;
		G199_TCON0_REG0->sft_cfg[28] = 0x00000b69;
	}
	G199_TCON0_REG0->sft_cfg[31] = 0x00001ffe;

	//for TPG setting
	G200_TCON0_REG1->sft_cfg[4] = 0x00000000;
	G200_TCON0_REG1->sft_cfg[5] = 0x00000000;
	G200_TCON0_REG1->sft_cfg[6] = 0x00000000; //default setting

	if ((width == 720) && (height == 480)) {
		//TBD
	} else if ((width == 800) && (height == 480)) {
		//G200_TCON0_REG1->sft_cfg[6] = 0x00000001; //Internal , H color bar
		G200_TCON0_REG1->sft_cfg[7] = 0x00000394; //HTT
		G200_TCON0_REG1->sft_cfg[8] = 0x000041fd; //HSTEP=0x4 VTT
		G200_TCON0_REG1->sft_cfg[9] = 0x0000031f; //V_ACT
		G200_TCON0_REG1->sft_cfg[10] = 0x000041df; //VSTEP=0x4 V_ACT
		G200_TCON0_REG1->sft_cfg[21] = 0x00000016; //sync with G204.01 VBP
	} else if ((width == 1024) && (height == 600)) {
		//TBD
	} else if ((width == 480) && (height == 1280)) {
		//G200_TCON0_REG1->sft_cfg[6] = 0x00000001; //Internal , H color bar
		G200_TCON0_REG1->sft_cfg[7] = 0x000002a5; //HTT 0x26c -1
		G200_TCON0_REG1->sft_cfg[8] = 0x0000451f; //HSTEP=0x4 VTT
		G200_TCON0_REG1->sft_cfg[9] = 0x000001df; //V_ACT=0x1e0-1
		G200_TCON0_REG1->sft_cfg[10] = 0x000044ff; //VSTEP=0x4 V_ACT=0x500-1
		G200_TCON0_REG1->sft_cfg[21] = 0x00000016; //sync with G204.01 VBP
	} else if ((width == 240) && (height == 320)) {
		//G200_TCON0_REG1->sft_cfg[6] = 0x00000001; //Internal , H color bar
		G200_TCON0_REG1->sft_cfg[7] = 0x000002aa; //HTT 0x2ab -1
		G200_TCON0_REG1->sft_cfg[8] = 0x00004161; //HSTEP=0x4 VTT 0x16c -1
		G200_TCON0_REG1->sft_cfg[9] = 0x000000EF; //V_ACT=0xf0-1
		G200_TCON0_REG1->sft_cfg[10] = 0x0000413F; //VSTEP=0x4 V_ACT=0x140-1
		G200_TCON0_REG1->sft_cfg[21] = 0x00000019; //sync with G204.01 VBP
	} else if ((width == 1280) && (height == 480)) {
		//TBD
	} else if ((width == 1280) && (height == 720)) {
		//TBD
	} else if ((width == 1920) && (height == 1080)) {
		//G200_TCON0_REG1->sft_cfg[6] = 0x00000001; //Internal , H color bar
		G200_TCON0_REG1->sft_cfg[7] = 0x00000897; //HTT
		G200_TCON0_REG1->sft_cfg[8] = 0x00004464; //HSTEP=0x4 VTT
		G200_TCON0_REG1->sft_cfg[9] = 0x0000077f; //V_ACT
		G200_TCON0_REG1->sft_cfg[10] = 0x00004437; //VSTEP=0x4 V_ACT
		G200_TCON0_REG1->sft_cfg[21] = 0x00000028; //sync with G204.01 VBP
	} else if ((width == 3840) && (height == 2880)) {
		//TBD
	}

	G200_TCON0_REG1->sft_cfg[15] = 0x00000000;
	G200_TCON0_REG1->sft_cfg[26] = 0x00000004;

}
