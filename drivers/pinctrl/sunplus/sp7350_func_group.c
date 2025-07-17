// SPDX-License-Identifier: GPL-2.0

#include <linux/compat.h>
#include <dm/devres.h>
#include "pinctrl_sunplus.h"
#include "sp7350_func_group.h"
#include "sp7350_pins.h"

static const unsigned int pins_spif[] = { 21, 22, 23, 24, 25, 26 };
static const struct sppctlgrp_t sp7350grps_spif[] = {
	EGRP("SPI_FLASH", 1, pins_spif),
};

static const unsigned int pins_emmc[] = { 20, 28, 29, 30, 31, 32,
					  33, 34, 35, 36, 37 };
static const struct sppctlgrp_t sp7350grps_emmc[] = {
	EGRP("EMMC", 1, pins_emmc),
};

static const unsigned int pins_spi_nand_x1[] = { 30, 31, 32, 33, 34, 35 };
static const unsigned int pins_spi_nand_x2[] = { 21, 22, 23, 24, 25, 26 };
static const struct sppctlgrp_t sp7350grps_spi_nand[] = {
	EGRP("SPI_NAND_X1", 1, pins_spi_nand_x1),
	EGRP("SPI_NAND_X2", 2, pins_spi_nand_x2),
};

static const unsigned int pins_sdc30[] = { 38, 39, 40, 41, 42, 43 };
static const struct sppctlgrp_t sp7350grps_sdc30[] = {
	EGRP("SD_CARD", 1, pins_sdc30),
};

static const unsigned int pins_sdio30[] = { 44, 45, 46, 47, 48, 49 };
static const struct sppctlgrp_t sp7350grps_sdio30[] = {
	EGRP("SDIO", 1, pins_sdio30),
};

static const unsigned int pins_para_nand[] = { 20, 21, 22, 23, 24, 25,
					       26, 27, 28, 29, 30, 31,
					       32, 33, 34, 35, 36 };
static const struct sppctlgrp_t sp7350grps_para_nand[] = {
	EGRP("PARA_NAND", 1, pins_para_nand),
};

static const unsigned int pins_usb_otg[] = { 18, 19 };
static const struct sppctlgrp_t sp7350grps_usb_otg[] = {
	EGRP("USB_OTG", 1, pins_usb_otg),
};

static const unsigned int pins_gmac_rgmii[] = { 3,  4,	5,  6,	7,  8,	9,
						10, 11, 12, 13, 14, 15, 16 };
static const unsigned int pins_gmac_rmii[] = { 4, 5, 6, 7, 8, 9, 10, 11, 12 };

static const struct sppctlgrp_t sp7350grps_gmac[] = {
	EGRP("GMAC_RGMII", 1, pins_gmac_rgmii),
	EGRP("GMAC_RMII", 2, pins_gmac_rmii),
};

static const unsigned int pins_pwm0_x1[] = { 78 };
static const unsigned int pins_pwm0_x2[] = { 58 };
static const struct sppctlgrp_t sp7350grps_pwm0[] = {
	EGRP("PWM0_X1", 1, pins_pwm0_x1),
	EGRP("PWM0_X2", 2, pins_pwm0_x2),
};

static const unsigned int pins_pwm1_x1[] = { 79 };
static const unsigned int pins_pwm1_x2[] = { 59 };
static const struct sppctlgrp_t sp7350grps_pwm1[] = {
	EGRP("PWM1_X1", 1, pins_pwm1_x1),
	EGRP("PWM1_X2", 2, pins_pwm1_x2),
};

static const unsigned int pins_pwm2_x1[] = { 60 };
static const unsigned int pins_pwm2_x2[] = { 92 };
static const struct sppctlgrp_t sp7350grps_pwm2[] = {
	EGRP("PWM2_X1", 1, pins_pwm2_x1),
	EGRP("PWM2_X2", 2, pins_pwm2_x2),
};

static const unsigned int pins_pwm3_x1[] = { 61 };
static const unsigned int pins_pwm3_x2[] = { 93 };
static const struct sppctlgrp_t sp7350grps_pwm3[] = {
	EGRP("PWM3_X1", 1, pins_pwm3_x1),
	EGRP("PWM3_X2", 2, pins_pwm3_x2),
};

static const unsigned int pins_uart0_x1[] = { 50, 51 };
static const unsigned int pins_uart0_x2[] = { 68, 69 };
static const struct sppctlgrp_t sp7350grps_uart0[] = {
	EGRP("UART0_X1", 1, pins_uart0_x1),
	EGRP("UART0_X2", 2, pins_uart0_x2),
};

static const unsigned int pins_uart1_x1[] = { 52, 53 };
static const unsigned int pins_uart1_x2[] = { 64, 65 };
static const struct sppctlgrp_t sp7350grps_uart1[] = {
	EGRP("UART1_X1", 1, pins_uart1_x1),
	EGRP("UART1_X2", 2, pins_uart1_x2),
};

static const unsigned int pins_uart1_fc_x1[] = { 54, 55 };
static const unsigned int pins_uart1_fc_x2[] = { 66, 67 };
static const struct sppctlgrp_t sp7350grps_uart1_fc[] = {
	EGRP("UART1_FC_X1", 1, pins_uart1_fc_x1),
	EGRP("UART1_FC_X2", 2, pins_uart1_fc_x2),
};

static const unsigned int pins_uart2_x1[] = { 56, 57 };
static const unsigned int pins_uart2_x2[] = { 76, 77 };
static const struct sppctlgrp_t sp7350grps_uart2[] = {
	EGRP("UART2_X1", 1, pins_uart2_x1),
	EGRP("UART2_X2", 2, pins_uart2_x2),
};

static const unsigned int pins_uart2_fc_x1[] = { 58, 59 };
static const unsigned int pins_uart2_fc_x2[] = { 78, 79 };
static const struct sppctlgrp_t sp7350grps_uart2_fc[] = {
	EGRP("UART2_FC_X1", 1, pins_uart2_fc_x1),
	EGRP("UART2_FC_X2", 2, pins_uart2_fc_x2),
};

#ifdef CONFIG_PINCTRL_QUIRKS_MUX_SPLIT
//AURORA used uart TX as gpio, remvoe it
static const unsigned int pins_uart3_x1[] = { 62 };
#else
static const unsigned int pins_uart3_x1[] = { 62, 63 };
#endif

static const unsigned int pins_uart3_x2[] = { 7, 8 };
static const struct sppctlgrp_t sp7350grps_uart3[] = {
	EGRP("UART3_X1", 1, pins_uart3_x1),
	EGRP("UART3_X2", 2, pins_uart3_x2),
};

static const unsigned int pins_uadbg[] = { 13, 14 };
static const struct sppctlgrp_t sp7350grps_uadbg[] = {
	EGRP("UADBG", 1, pins_uadbg),
};

static const unsigned int pins_uart6_x1[] = { 80, 81 };
static const unsigned int pins_uart6_x2[] = { 48, 49 };
static const struct sppctlgrp_t sp7350grps_uart6[] = {
	EGRP("UART6_X1", 1, pins_uart6_x1),
	EGRP("UART6_X2", 2, pins_uart6_x2),
};

static const unsigned int pins_uart7[] = { 82, 83 };
static const struct sppctlgrp_t sp7350grps_uart7[] = {
	EGRP("UART7", 1, pins_uart7),
};

static const unsigned int pins_i2c_combo0_x1[] = { 68, 69 };
static const unsigned int pins_i2c_combo0_x2[] = { 54, 55 };
static const struct sppctlgrp_t sp7350grps_i2c_combo0[] = {
	EGRP("I2C_COMBO0_X1", 1, pins_i2c_combo0_x1),
	EGRP("I2C_COMBO0_X2", 2, pins_i2c_combo0_x2),
};

static const unsigned int pins_i2c_combo1[] = { 70, 71 };
static const struct sppctlgrp_t sp7350grps_i2c_combo1[] = {
	EGRP("I2C_COMBO1", 1, pins_i2c_combo1),
};

static const unsigned int pins_i2c_combo2_x1[] = { 76, 77 };
static const unsigned int pins_i2c_combo2_x2[] = { 56, 57 };
static const struct sppctlgrp_t sp7350grps_i2c_combo2[] = {
	EGRP("I2C_COMBO2_X1", 1, pins_i2c_combo2_x1),
	EGRP("I2C_COMBO2_X2", 2, pins_i2c_combo2_x2),
};

static const unsigned int pins_i2c_combo3[] = { 88, 89 };
static const struct sppctlgrp_t sp7350grps_i2c_combo3[] = {
	EGRP("I2C_COMBO3", 1, pins_i2c_combo3),
};

static const unsigned int pins_i2c_combo4[] = { 90, 91 };
static const struct sppctlgrp_t sp7350grps_i2c_combo4[] = {
	EGRP("I2C_COMBO4", 1, pins_i2c_combo4),
};

static const unsigned int pins_i2c_combo5[] = { 92, 93 };
static const struct sppctlgrp_t sp7350grps_i2c_combo5[] = {
	EGRP("I2C_COMBO5", 1, pins_i2c_combo5),
};

static const unsigned int pins_i2c_combo6_x1[] = { 84, 85 };
static const unsigned int pins_i2c_combo6_x2[] = { 1, 2 };
static const struct sppctlgrp_t sp7350grps_i2c_combo6[] = {
	EGRP("I2C_COMBO6_X1", 1, pins_i2c_combo6_x1),
	EGRP("I2C_COMBO6_X2", 2, pins_i2c_combo6_x2),
};

static const unsigned int pins_i2c_combo7_x1[] = { 86, 87 };
static const unsigned int pins_i2c_combo7_x2[] = { 3, 4 };
static const struct sppctlgrp_t sp7350grps_i2c_combo7[] = {
	EGRP("I2C_COMBO7_X1", 1, pins_i2c_combo7_x1),
	EGRP("I2C_COMBO7_X2", 2, pins_i2c_combo7_x2),
};

static const unsigned int pins_i2c_combo8_x1[] = { 95, 96 };
static const unsigned int pins_i2c_combo8_x2[] = { 9, 10 };
static const struct sppctlgrp_t sp7350grps_i2c_combo8[] = {
	EGRP("I2C_COMBO8_X1", 1, pins_i2c_combo8_x1),
	EGRP("I2C_COMBO8_X2", 2, pins_i2c_combo8_x2),
};

static const unsigned int pins_i2c_combo9_x1[] = { 97, 98 };
static const unsigned int pins_i2c_combo9_x2[] = { 11, 12 };
static const struct sppctlgrp_t sp7350grps_i2c_combo9[] = {
	EGRP("I2C_COMBO9_X1", 1, pins_i2c_combo9_x1),
	EGRP("I2C_COMBO9_X2", 2, pins_i2c_combo9_x2),
};

static const unsigned int pins_clkgen_dgo_mcu_x1[] = { 1 };
static const unsigned int pins_clkgen_dgo_mcu_x2[] = { 52 };
static const struct sppctlgrp_t sp7350grps_clkgen_dgo_mcu[] = {
	EGRP("CLKGEN_DGO_MCU_X1", 1, pins_clkgen_dgo_mcu_x1),
	EGRP("CLKGEN_DGO_MCU_X2", 2, pins_clkgen_dgo_mcu_x2),
};

static const unsigned int pins_clkgen_dgo_wifi_x1[] = { 2 };
static const unsigned int pins_clkgen_dgo_wifi_x2[] = { 62 };
static const struct sppctlgrp_t sp7350grps_clkgen_dgo_wifi[] = {
	EGRP("CLKGEN_DGO_WIFI_X1", 1, pins_clkgen_dgo_wifi_x1),
	EGRP("CLKGEN_DGO_WIFI_X2", 2, pins_clkgen_dgo_wifi_x2),
};

static const unsigned int pins_clkgen_dgo_rtc_x1[] = { 12 };
static const unsigned int pins_clkgen_dgo_rtc_x2[] = { 63 };
static const struct sppctlgrp_t sp7350grps_clkgen_dgo_rtc[] = {
	EGRP("CLKGEN_DGO_RTC_X1", 1, pins_clkgen_dgo_rtc_x1),
	EGRP("CLKGEN_DGO_RTC_X2", 2, pins_clkgen_dgo_rtc_x2),
};

static const unsigned int pins_clkgen_dgo_phy_x1[] = { 13 };
static const unsigned int pins_clkgen_dgo_phy_x2[] = { 83 };
static const struct sppctlgrp_t sp7350grps_clkgen_dgo_phy[] = {
	EGRP("CLKGEN_DGO_PHY_X1", 1, pins_clkgen_dgo_phy_x1),
	EGRP("CLKGEN_DGO_PHY_X2", 2, pins_clkgen_dgo_phy_x2),
};

static const unsigned int pins_clkgen_dgo_gncma_x1[] = { 27 };
static const unsigned int pins_clkgen_dgo_gncma_x2[] = { 53 };
static const struct sppctlgrp_t sp7350grps_clkgen_dgo_gncma[] = {
	EGRP("CLKGEN_DGO_GNCMA_X1", 1, pins_clkgen_dgo_gncma_x1),
	EGRP("CLKGEN_DGO_GNCMA_X2", 2, pins_clkgen_dgo_gncma_x2),
};

static const unsigned int pins_spi_master0_x1[] = { 64, 65, 66, 67 };
static const unsigned int pins_spi_master0_x2[] = { 9, 10, 11, 12 };
static const struct sppctlgrp_t sp7350grps_spi_master0[] = {
	EGRP("SPI_MASTER0_X1", 1, pins_spi_master0_x1),
	EGRP("SPI_MASTER0_X2", 2, pins_spi_master0_x2),
};

static const unsigned int pins_spi_master1_x1[] = { 80, 81, 82, 83 };
static const unsigned int pins_spi_master1_x2[] = { 14, 15, 16, 17 };
static const struct sppctlgrp_t sp7350grps_spi_master1[] = {
	EGRP("SPI_MASTER1_X1", 1, pins_spi_master1_x1),
	EGRP("SPI_MASTER1_X2", 2, pins_spi_master1_x2),
};

static const unsigned int pins_spi_master2[] = { 88, 89, 90, 91 };
static const struct sppctlgrp_t sp7350grps_spi_master2[] = {
	EGRP("SPI_MASTER2", 1, pins_spi_master2),
};

static const unsigned int pins_spi_master3_x1[] = { 44, 45, 46, 47 };
static const unsigned int pins_spi_master3_x2[] = { 52, 53, 54, 55 };
static const struct sppctlgrp_t sp7350grps_spi_master3[] = {
	EGRP("SPI_MASTER3_X1", 1, pins_spi_master3_x1),
	EGRP("SPI_MASTER3_X2", 2, pins_spi_master3_x2),
};

static const unsigned int pins_spi_master4[] = { 72, 73, 74, 75 };
static const struct sppctlgrp_t sp7350grps_spi_master4[] = {
	EGRP("SPI_MASTER4", 1, pins_spi_master4),
};

static const unsigned int pins_spi_slave0_x1[] = { 94, 95, 96, 97 };
static const unsigned int pins_spi_slave0_x2[] = { 72, 73, 74, 75 };
static const struct sppctlgrp_t sp7350grps_spi_slave0[] = {
	EGRP("SPI_SLAVE0_X1", 1, pins_spi_slave0_x1),
	EGRP("SPI_SLAVE0_X2", 2, pins_spi_slave0_x2),
};

static const unsigned int pins_aud_tdmtx_xck[] = { 93 };
static const struct sppctlgrp_t sp7350grps_aud_tdmtx_xck[] = {
	EGRP("AUD_TDMTX_XCK", 1, pins_aud_tdmtx_xck),
};

static const unsigned int pins_aud_dac_xck1[] = { 71 };
static const struct sppctlgrp_t sp7350grps_aud_dac_xck1[] = {
	EGRP("AUD_DAC_XCK1", 1, pins_aud_dac_xck1),
};

static const unsigned int pins_aud_dac_xck[] = { 83 };
static const struct sppctlgrp_t sp7350grps_aud_dac_xck[] = {
	EGRP("AUD_DAC_XCK", 1, pins_aud_dac_xck),
};

static const unsigned int pins_aud_au2_data0[] = { 82 };
static const struct sppctlgrp_t sp7350grps_aud_au2_data0[] = {
	EGRP("AUD_AU2_DATA0", 1, pins_aud_au2_data0),
};

static const unsigned int pins_aud_au1_data0[] = { 58 };
static const struct sppctlgrp_t sp7350grps_aud_au1_data0[] = {
	EGRP("AUD_AU1_DATA0", 1, pins_aud_au1_data0),
};

static const unsigned int pins_aud_au2_ck[] = { 80, 81 };
static const struct sppctlgrp_t sp7350grps_aud_au2_ck[] = {
	EGRP("AUD_AU2_CK", 1, pins_aud_au2_ck),
};

static const unsigned int pins_aud_au1_ck[] = { 56, 57 };
static const struct sppctlgrp_t sp7350grps_aud_au1_ck[] = {
	EGRP("AUD_AU1_CK", 1, pins_aud_au1_ck),
};

static const unsigned int pins_aud_au_adc_data0_x1[] = { 94, 95, 96, 97 };
static const unsigned int pins_aud_au_adc_data0_x2[] = { 72, 73, 74, 75 };
static const struct sppctlgrp_t sp7350grps_aud_au_adc_data0[] = {
	EGRP("AUD_AU_ADC_DATA0_X1", 1, pins_aud_au_adc_data0_x1),
	EGRP("AUD_AU_ADC_DATA0_X2", 2, pins_aud_au_adc_data0_x2),
};

static const unsigned int pins_aud_adc2_data0[] = { 82 };
static const struct sppctlgrp_t sp7350grps_aud_adc2_data0[] = {
	EGRP("AUD_ADC2_DATA0", 1, pins_aud_adc2_data0),
};

static const unsigned int pins_aud_adc1_data0[] = { 58 };
static const struct sppctlgrp_t sp7350grps_aud_adc1_data0[] = {
	EGRP("AUD_ADC1_DATA0", 1, pins_aud_adc1_data0),
};

static const unsigned int pins_aud_tdm[] = { 94, 95, 96, 97 };
static const struct sppctlgrp_t sp7350grps_aud_tdm[] = {
	EGRP("AUD_TDM", 1, pins_aud_tdm),
};

static const unsigned int pins_spdif_x1[] = { 91 };
static const unsigned int pins_spdif_x2[] = { 53 };
static const unsigned int pins_spdif_x3[] = { 54 };
static const unsigned int pins_spdif_x4[] = { 55 };
static const unsigned int pins_spdif_x5[] = { 62 };
static const unsigned int pins_spdif_x6[] = { 52 };
static const struct sppctlgrp_t sp7350grps_spdif_in[] = {
	EGRP("SPDIF_IN_X1", 1, pins_spdif_x1),
	EGRP("SPDIF_IN_X2", 2, pins_spdif_x2),
	EGRP("SPDIF_IN_X3", 3, pins_spdif_x3),
	EGRP("SPDIF_IN_X4", 4, pins_spdif_x4),
	EGRP("SPDIF_IN_X5", 5, pins_spdif_x5),
	EGRP("SPDIF_IN_X6", 6, pins_spdif_x6),
};

static const struct sppctlgrp_t sp7350grps_spdif_out[] = {
	EGRP("SPDIF_OUT_X1", 1, pins_spdif_x1),
	EGRP("SPDIF_OUT_X2", 2, pins_spdif_x2),
	EGRP("SPDIF_OUT_X3", 3, pins_spdif_x3),
	EGRP("SPDIF_OUT_X4", 4, pins_spdif_x4),
	EGRP("SPDIF_OUT_X5", 5, pins_spdif_x5),
	EGRP("SPDIF_OUT_X6", 6, pins_spdif_x6),
};

static const unsigned int pins_ext_int0_x1[] = { 5 };
static const unsigned int pins_ext_int0_x2[] = { 59 };
static const struct sppctlgrp_t sp7350grps_ext_int0[] = {
	EGRP("EXT_INT0_X1", 1, pins_ext_int0_x1),
	EGRP("EXT_INT0_X2", 2, pins_ext_int0_x2),
};

static const unsigned int pins_ext_int1_x1[] = { 6 };
static const unsigned int pins_ext_int1_x2[] = { 70 };
static const struct sppctlgrp_t sp7350grps_ext_int1[] = {
	EGRP("EXT_INT1_X1", 1, pins_ext_int1_x1),
	EGRP("EXT_INT1_X2", 2, pins_ext_int1_x2),
};

static const unsigned int pins_ext_int2_x1[] = { 7 };
static const unsigned int pins_ext_int2_x2[] = { 3 };
static const struct sppctlgrp_t sp7350grps_ext_int2[] = {
	EGRP("EXT_INT2_X1", 1, pins_ext_int2_x1),
	EGRP("EXT_INT2_X2", 2, pins_ext_int2_x2),
};

static const unsigned int pins_ext_int3_x1[] = { 8 };
static const unsigned int pins_ext_int3_x2[] = { 4 };
static const struct sppctlgrp_t sp7350grps_ext_int3[] = {
	EGRP("EXT_INT3_X1", 1, pins_ext_int3_x1),
	EGRP("EXT_INT3_X2", 2, pins_ext_int3_x2),
};

static const unsigned int pins_int0_x1[] = { 1 };
static const unsigned int pins_int0_x2[] = { 2 };
static const unsigned int pins_int0_x3[] = { 3 };
static const unsigned int pins_int0_x4[] = { 4 };
static const unsigned int pins_int0_x5[] = { 5 };
static const unsigned int pins_int0_x6[] = { 6 };
static const unsigned int pins_int0_x7[] = { 13 };
static const unsigned int pins_int0_x8[] = { 14 };
static const unsigned int pins_int0_x9[] = { 15 };
static const struct sppctlgrp_t sp7350grps_int0[] = {
	EGRP("INT0_X1", 1, pins_int0_x1), EGRP("INT0_X2", 2, pins_int0_x2),
	EGRP("INT0_X3", 3, pins_int0_x3), EGRP("INT0_X4", 4, pins_int0_x4),
	EGRP("INT0_X5", 5, pins_int0_x5), EGRP("INT0_X6", 6, pins_int0_x6),
	EGRP("INT0_X7", 7, pins_int0_x7), EGRP("INT0_X8", 8, pins_int0_x8),
	EGRP("INT0_X9", 9, pins_int0_x9),
};

static const struct sppctlgrp_t sp7350grps_int1[] = {
	EGRP("INT1_X1", 1, pins_int0_x1), EGRP("INT1_X2", 2, pins_int0_x2),
	EGRP("INT1_X3", 3, pins_int0_x3), EGRP("INT1_X4", 4, pins_int0_x4),
	EGRP("INT1_X5", 5, pins_int0_x5), EGRP("INT1_X6", 6, pins_int0_x6),
	EGRP("INT1_X7", 7, pins_int0_x7), EGRP("INT1_X8", 8, pins_int0_x8),
	EGRP("INT1_X9", 9, pins_int0_x9),
};

static const unsigned int pins_int2_x1[] = { 5 };
static const unsigned int pins_int2_x2[] = { 6 };
static const unsigned int pins_int2_x3[] = { 7 };
static const unsigned int pins_int2_x4[] = { 8 };
static const unsigned int pins_int2_x5[] = { 9 };
static const unsigned int pins_int2_x6[] = { 10 };
static const unsigned int pins_int2_x7[] = { 11 };
static const unsigned int pins_int2_x8[] = { 16 };
static const unsigned int pins_int2_x9[] = { 17 };
static const struct sppctlgrp_t sp7350grps_int2[] = {
	EGRP("INT2_X1", 1, pins_int2_x1), EGRP("INT2_X2", 2, pins_int2_x2),
	EGRP("INT2_X3", 3, pins_int2_x3), EGRP("INT2_X4", 4, pins_int2_x4),
	EGRP("INT2_X5", 5, pins_int2_x5), EGRP("INT2_X6", 6, pins_int2_x6),
	EGRP("INT2_X7", 7, pins_int2_x7), EGRP("INT2_X8", 8, pins_int2_x8),
	EGRP("INT2_X9", 9, pins_int2_x9),
};

static const struct sppctlgrp_t sp7350grps_int3[] = {
	EGRP("INT3_X1", 1, pins_int2_x1), EGRP("INT3_X2", 2, pins_int2_x2),
	EGRP("INT3_X3", 3, pins_int2_x3), EGRP("INT3_X4", 4, pins_int2_x4),
	EGRP("INT3_X5", 5, pins_int2_x5), EGRP("INT3_X6", 6, pins_int2_x6),
	EGRP("INT3_X7", 7, pins_int2_x7), EGRP("INT3_X8", 8, pins_int2_x8),
	EGRP("INT3_X9", 9, pins_int2_x9),
};

static const unsigned int pins_int4_x1[] = { 7 };
static const unsigned int pins_int4_x2[] = { 8 };
static const unsigned int pins_int4_x3[] = { 9 };
static const unsigned int pins_int4_x4[] = { 10 };
static const unsigned int pins_int4_x5[] = { 11 };
static const unsigned int pins_int4_x6[] = { 12 };
static const unsigned int pins_int4_x7[] = { 13 };
static const unsigned int pins_int4_x8[] = { 18 };
static const unsigned int pins_int4_x9[] = { 19 };
static const struct sppctlgrp_t sp7350grps_int4[] = {
	EGRP("INT4_X1", 1, pins_int4_x1), EGRP("INT4_X2", 2, pins_int4_x2),
	EGRP("INT4_X3", 3, pins_int4_x3), EGRP("INT4_X4", 4, pins_int4_x4),
	EGRP("INT4_X5", 5, pins_int4_x5), EGRP("INT4_X6", 6, pins_int4_x6),
	EGRP("INT4_X7", 7, pins_int4_x7), EGRP("INT4_X8", 8, pins_int4_x8),
	EGRP("INT4_X9", 9, pins_int4_x9),
};

static const struct sppctlgrp_t sp7350grps_int5[] = {
	EGRP("INT5_X1", 1, pins_int4_x1), EGRP("INT5_X2", 2, pins_int4_x2),
	EGRP("INT5_X3", 3, pins_int4_x3), EGRP("INT5_X4", 4, pins_int4_x4),
	EGRP("INT5_X5", 5, pins_int4_x5), EGRP("INT5_X6", 6, pins_int4_x6),
	EGRP("INT5_X7", 7, pins_int4_x7), EGRP("INT5_X8", 8, pins_int4_x8),
	EGRP("INT5_X9", 9, pins_int4_x9),
};

static const unsigned int pins_int6_x1[] = { 9 };
static const unsigned int pins_int6_x2[] = { 10 };
static const unsigned int pins_int6_x3[] = { 11 };
static const unsigned int pins_int6_x4[] = { 12 };
static const unsigned int pins_int6_x5[] = { 13 };
static const unsigned int pins_int6_x6[] = { 14 };
static const unsigned int pins_int6_x7[] = { 15 };
static const unsigned int pins_int6_x8[] = { 16 };
static const unsigned int pins_int6_x9[] = { 17 };
static const unsigned int pins_int6_x10[] = { 18 };
static const unsigned int pins_int6_x11[] = { 19 };
static const struct sppctlgrp_t sp7350grps_int6[] = {
	EGRP("INT6_X1", 1, pins_int6_x1),
	EGRP("INT6_X2", 2, pins_int6_x2),
	EGRP("INT6_X3", 3, pins_int6_x3),
	EGRP("INT6_X4", 4, pins_int6_x4),
	EGRP("INT6_X5", 5, pins_int6_x5),
	EGRP("INT6_X6", 6, pins_int6_x6),
	EGRP("INT6_X7", 7, pins_int6_x7),
	EGRP("INT6_X8", 8, pins_int6_x8),
	EGRP("INT6_X9", 9, pins_int6_x9),
	EGRP("INT6_X10", 10, pins_int6_x10),
	EGRP("INT6_X11", 11, pins_int6_x11),
};

static const struct sppctlgrp_t sp7350grps_int7[] = {
	EGRP("INT7_X1", 1, pins_int6_x1),
	EGRP("INT7_X2", 2, pins_int6_x2),
	EGRP("INT7_X3", 3, pins_int6_x3),
	EGRP("INT7_X4", 4, pins_int6_x4),
	EGRP("INT7_X5", 5, pins_int6_x5),
	EGRP("INT7_X6", 6, pins_int6_x6),
	EGRP("INT7_X7", 7, pins_int6_x7),
	EGRP("INT7_X8", 8, pins_int6_x8),
	EGRP("INT7_X9", 9, pins_int6_x9),
	EGRP("INT7_X10", 10, pins_int6_x10),
	EGRP("INT7_X11", 11, pins_int6_x11),
};

//GPIO_AO_INT
static const unsigned int pins_gpio_ao_int0_x1[] = { 52 };
static const unsigned int pins_gpio_ao_int0_x2[] = { 68 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int0[] = {
	EGRP("GPIO_AO_0_INT_X1", 1, pins_gpio_ao_int0_x1),
	EGRP("GPIO_AO_0_INT_X2", 2, pins_gpio_ao_int0_x2),
};

static const unsigned int pins_gpio_ao_int1_x1[] = { 53 };
static const unsigned int pins_gpio_ao_int1_x2[] = { 69 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int1[] = {
	EGRP("GPIO_AO_1_INT_X1", 1, pins_gpio_ao_int1_x1),
	EGRP("GPIO_AO_1_INT_X2", 2, pins_gpio_ao_int1_x2),
};

static const unsigned int pins_gpio_ao_int2_x1[] = { 54 };
static const unsigned int pins_gpio_ao_int2_x2[] = { 70 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int2[] = {
	EGRP("GPIO_AO_2_INT_X1", 1, pins_gpio_ao_int2_x1),
	EGRP("GPIO_AO_2_INT_X2", 2, pins_gpio_ao_int2_x2),
};

static const unsigned int pins_gpio_ao_int3_x1[] = { 55 };
static const unsigned int pins_gpio_ao_int3_x2[] = { 71 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int3[] = {
	EGRP("GPIO_AO_3_INT_X1", 1, pins_gpio_ao_int3_x1),
	EGRP("GPIO_AO_3_INT_X2", 2, pins_gpio_ao_int3_x2),
};

static const unsigned int pins_gpio_ao_int4_x1[] = { 56 };
static const unsigned int pins_gpio_ao_int4_x2[] = { 72 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int4[] = {
	EGRP("GPIO_AO_4_INT_X1", 1, pins_gpio_ao_int4_x1),
	EGRP("GPIO_AO_4_INT_X2", 2, pins_gpio_ao_int4_x2),
};

static const unsigned int pins_gpio_ao_int5_x1[] = { 57 };
static const unsigned int pins_gpio_ao_int5_x2[] = { 73 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int5[] = {
	EGRP("GPIO_AO_5_INT_X1", 1, pins_gpio_ao_int5_x1),
	EGRP("GPIO_AO_5_INT_X2", 2, pins_gpio_ao_int5_x2),
};

static const unsigned int pins_gpio_ao_int6_x1[] = { 58 };
static const unsigned int pins_gpio_ao_int6_x2[] = { 74 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int6[] = {
	EGRP("GPIO_AO_6_INT_X1", 1, pins_gpio_ao_int6_x1),
	EGRP("GPIO_AO_6_INT_X2", 2, pins_gpio_ao_int6_x2),
};

static const unsigned int pins_gpio_ao_int7_x1[] = { 59 };
static const unsigned int pins_gpio_ao_int7_x2[] = { 75 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int7[] = {
	EGRP("GPIO_AO_7_INT_X1", 1, pins_gpio_ao_int7_x1),
	EGRP("GPIO_AO_7_INT_X2", 2, pins_gpio_ao_int7_x2),
};

static const unsigned int pins_gpio_ao_int8_x1[] = { 60 };
static const unsigned int pins_gpio_ao_int8_x2[] = { 76 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int8[] = {
	EGRP("GPIO_AO_8_INT_X1", 1, pins_gpio_ao_int8_x1),
	EGRP("GPIO_AO_8_INT_X2", 2, pins_gpio_ao_int8_x2),
};

static const unsigned int pins_gpio_ao_int9_x1[] = { 61 };
static const unsigned int pins_gpio_ao_int9_x2[] = { 77 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int9[] = {
	EGRP("GPIO_AO_9_INT_X1", 1, pins_gpio_ao_int9_x1),
	EGRP("GPIO_AO_9_INT_X2", 2, pins_gpio_ao_int9_x2),
};

static const unsigned int pins_gpio_ao_int10_x1[] = { 62 };
static const unsigned int pins_gpio_ao_int10_x2[] = { 78 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int10[] = {
	EGRP("GPIO_AO_10_INT_X1", 1, pins_gpio_ao_int10_x1),
	EGRP("GPIO_AO_10_INT_X2", 2, pins_gpio_ao_int10_x2),
};

static const unsigned int pins_gpio_ao_int11_x1[] = { 63 };
static const unsigned int pins_gpio_ao_int11_x2[] = { 79 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int11[] = {
	EGRP("GPIO_AO_11_INT_X1", 1, pins_gpio_ao_int11_x1),
	EGRP("GPIO_AO_11_INT_X2", 2, pins_gpio_ao_int11_x2),
};

static const unsigned int pins_gpio_ao_int12_x1[] = { 64 };
static const unsigned int pins_gpio_ao_int12_x2[] = { 80 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int12[] = {
	EGRP("GPIO_AO_12_INT_X1", 1, pins_gpio_ao_int12_x1),
	EGRP("GPIO_AO_12_INT_X2", 2, pins_gpio_ao_int12_x2),
};

static const unsigned int pins_gpio_ao_int13_x1[] = { 65 };
static const unsigned int pins_gpio_ao_int13_x2[] = { 81 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int13[] = {
	EGRP("GPIO_AO_13_INT_X1", 1, pins_gpio_ao_int13_x1),
	EGRP("GPIO_AO_13_INT_X2", 2, pins_gpio_ao_int13_x2),
};

static const unsigned int pins_gpio_ao_int14_x1[] = { 66 };
static const unsigned int pins_gpio_ao_int14_x2[] = { 82 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int14[] = {
	EGRP("GPIO_AO_14_INT_X1", 1, pins_gpio_ao_int14_x1),
	EGRP("GPIO_AO_14_INT_X2", 2, pins_gpio_ao_int14_x2),
};

static const unsigned int pins_gpio_ao_int15_x1[] = { 67 };
static const unsigned int pins_gpio_ao_int15_x2[] = { 83 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int15[] = {
	EGRP("GPIO_AO_15_INT_X1", 1, pins_gpio_ao_int15_x1),
	EGRP("GPIO_AO_15_INT_X2", 2, pins_gpio_ao_int15_x2),
};

static const unsigned int pins_gpio_ao_int16_x1[] = { 68 };
static const unsigned int pins_gpio_ao_int16_x2[] = { 84 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int16[] = {
	EGRP("GPIO_AO_16_INT_X1", 1, pins_gpio_ao_int16_x1),
	EGRP("GPIO_AO_16_INT_X2", 2, pins_gpio_ao_int16_x2),
};

static const unsigned int pins_gpio_ao_int17_x1[] = { 69 };
static const unsigned int pins_gpio_ao_int17_x2[] = { 85 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int17[] = {
	EGRP("GPIO_AO_17_INT_X1", 1, pins_gpio_ao_int17_x1),
	EGRP("GPIO_AO_17_INT_X2", 2, pins_gpio_ao_int17_x2),
};

static const unsigned int pins_gpio_ao_int18_x1[] = { 70 };
static const unsigned int pins_gpio_ao_int18_x2[] = { 86 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int18[] = {
	EGRP("GPIO_AO_18_INT_X1", 1, pins_gpio_ao_int18_x1),
	EGRP("GPIO_AO_18_INT_X2", 2, pins_gpio_ao_int18_x2),
};

static const unsigned int pins_gpio_ao_int19_x1[] = { 71 };
static const unsigned int pins_gpio_ao_int19_x2[] = { 87 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int19[] = {
	EGRP("GPIO_AO_19_INT_X1", 1, pins_gpio_ao_int19_x1),
	EGRP("GPIO_AO_19_INT_X2", 2, pins_gpio_ao_int19_x2),
};

static const unsigned int pins_gpio_ao_int20_x1[] = { 72 };
static const unsigned int pins_gpio_ao_int20_x2[] = { 88 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int20[] = {
	EGRP("GPIO_AO_20_INT_X1", 1, pins_gpio_ao_int20_x1),
	EGRP("GPIO_AO_20_INT_X2", 2, pins_gpio_ao_int20_x2),
};

static const unsigned int pins_gpio_ao_int21_x1[] = { 73 };
static const unsigned int pins_gpio_ao_int21_x2[] = { 89 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int21[] = {
	EGRP("GPIO_AO_21_INT_X1", 1, pins_gpio_ao_int21_x1),
	EGRP("GPIO_AO_21_INT_X2", 2, pins_gpio_ao_int21_x2),
};

static const unsigned int pins_gpio_ao_int22_x1[] = { 74 };
static const unsigned int pins_gpio_ao_int22_x2[] = { 90 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int22[] = {
	EGRP("GPIO_AO_22_INT_X1", 1, pins_gpio_ao_int22_x1),
	EGRP("GPIO_AO_22_INT_X2", 2, pins_gpio_ao_int22_x2),
};

static const unsigned int pins_gpio_ao_int23_x1[] = { 75 };
static const unsigned int pins_gpio_ao_int23_x2[] = { 91 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int23[] = {
	EGRP("GPIO_AO_23_INT_X1", 1, pins_gpio_ao_int23_x1),
	EGRP("GPIO_AO_23_INT_X2", 2, pins_gpio_ao_int23_x2),
};

static const unsigned int pins_gpio_ao_int24_x1[] = { 76 };
static const unsigned int pins_gpio_ao_int24_x2[] = { 91 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int24[] = {
	EGRP("GPIO_AO_24_INT_X1", 1, pins_gpio_ao_int24_x1),
	EGRP("GPIO_AO_24_INT_X2", 2, pins_gpio_ao_int24_x2),
};

static const unsigned int pins_gpio_ao_int25_x1[] = { 77 };
static const unsigned int pins_gpio_ao_int25_x2[] = { 92 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int25[] = {
	EGRP("GPIO_AO_25_INT_X1", 1, pins_gpio_ao_int25_x1),
	EGRP("GPIO_AO_25_INT_X2", 2, pins_gpio_ao_int25_x2),
};

static const unsigned int pins_gpio_ao_int26_x1[] = { 78 };
static const unsigned int pins_gpio_ao_int26_x2[] = { 93 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int26[] = {
	EGRP("GPIO_AO_26_INT_X1", 1, pins_gpio_ao_int26_x1),
	EGRP("GPIO_AO_26_INT_X2", 2, pins_gpio_ao_int26_x2),
};

static const unsigned int pins_gpio_ao_int27_x1[] = { 79 };
static const unsigned int pins_gpio_ao_int27_x2[] = { 94 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int27[] = {
	EGRP("GPIO_AO_27_INT_X1", 1, pins_gpio_ao_int27_x1),
	EGRP("GPIO_AO_27_INT_X2", 2, pins_gpio_ao_int27_x2),
};

static const unsigned int pins_gpio_ao_int28_x1[] = { 80 };
static const unsigned int pins_gpio_ao_int28_x2[] = { 95 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int28[] = {
	EGRP("GPIO_AO_28_INT_X1", 1, pins_gpio_ao_int28_x1),
	EGRP("GPIO_AO_28_INT_X2", 2, pins_gpio_ao_int28_x2),
};

static const unsigned int pins_gpio_ao_int29_x1[] = { 81 };
static const unsigned int pins_gpio_ao_int29_x2[] = { 96 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int29[] = {
	EGRP("GPIO_AO_29_INT_X1", 1, pins_gpio_ao_int29_x1),
	EGRP("GPIO_AO_29_INT_X2", 2, pins_gpio_ao_int29_x2),
};

static const unsigned int pins_gpio_ao_int30_x1[] = { 82 };
static const unsigned int pins_gpio_ao_int30_x2[] = { 97 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int30[] = {
	EGRP("GPIO_AO_30_INT_X1", 1, pins_gpio_ao_int30_x1),
	EGRP("GPIO_AO_30_INT_X2", 2, pins_gpio_ao_int30_x2),
};

static const unsigned int pins_gpio_ao_int31_x1[] = { 83 };
static const unsigned int pins_gpio_ao_int31_x2[] = { 98 };
static const struct sppctlgrp_t sp7350grps_gpio_ao_int31[] = {
	EGRP("GPIO_AO_31_INT_X1", 1, pins_gpio_ao_int31_x1),
	EGRP("GPIO_AO_31_INT_X2", 2, pins_gpio_ao_int31_x2),
};

struct func_t list_funcs[] = {
	FNCN("GPIO", F_OFF_0, 0, 0, 0),
	FNCN("IOP", F_OFF_0, 0, 0, 0),

	FNCE("SPI_FLASH", F_OFF_G, 1, 0, 1, sp7350grps_spif),
	FNCE("EMMC", F_OFF_G, 1, 1, 1, sp7350grps_emmc),
	FNCE("SPI_NAND", F_OFF_G, 1, 2, 2, sp7350grps_spi_nand),
	FNCE("SD_CARD", F_OFF_G, 1, 4, 1, sp7350grps_sdc30),
	FNCE("SDIO", F_OFF_G, 1, 5, 1, sp7350grps_sdio30),
	FNCE("PARA_NAND", F_OFF_G, 1, 6, 1, sp7350grps_para_nand),
	FNCE("USB_OTG", F_OFF_G, 1, 7, 1, sp7350grps_usb_otg),
	FNCE("GMAC", F_OFF_G, 1, 9, 1, sp7350grps_gmac),
	FNCE("PWM0", F_OFF_G, 1, 10, 2, sp7350grps_pwm0),
	FNCE("PWM1", F_OFF_G, 1, 12, 2, sp7350grps_pwm1),
	FNCE("PWM2", F_OFF_G, 1, 14, 2, sp7350grps_pwm2),

	FNCE("PWM3", F_OFF_G, 2, 0, 2, sp7350grps_pwm3),
	FNCE("UART0", F_OFF_G, 2, 2, 2, sp7350grps_uart0),
	FNCE("UART1", F_OFF_G, 2, 4, 2, sp7350grps_uart1),
	FNCE("UART1_FC", F_OFF_G, 2, 6, 2, sp7350grps_uart1_fc),
	FNCE("UART2", F_OFF_G, 2, 8, 2, sp7350grps_uart2),
	FNCE("UART2_FC", F_OFF_G, 2, 10, 2, sp7350grps_uart2_fc),
	FNCE("UART3", F_OFF_G, 2, 12, 2, sp7350grps_uart3),
	FNCE("UADBG", F_OFF_G, 2, 14, 1, sp7350grps_uadbg),

	FNCE("UART6", F_OFF_G, 3, 0, 2, sp7350grps_uart6),
	FNCE("UART7", F_OFF_G, 3, 2, 1, sp7350grps_uart7),
	FNCE("I2C_COMBO0", F_OFF_G, 3, 3, 2, sp7350grps_i2c_combo0),
	FNCE("I2C_COMBO1", F_OFF_G, 3, 5, 1, sp7350grps_i2c_combo1),
	FNCE("I2C_COMBO2", F_OFF_G, 3, 6, 2, sp7350grps_i2c_combo2),
	FNCE("I2C_COMBO3", F_OFF_G, 3, 8, 1, sp7350grps_i2c_combo3),
	FNCE("I2C_COMBO4", F_OFF_G, 3, 9, 1, sp7350grps_i2c_combo4),
	FNCE("I2C_COMBO5", F_OFF_G, 3, 10, 1, sp7350grps_i2c_combo5),
	FNCE("I2C_COMBO6", F_OFF_G, 3, 11, 2, sp7350grps_i2c_combo6),
	FNCE("I2C_COMBO7", F_OFF_G, 3, 13, 2, sp7350grps_i2c_combo7),

	FNCE("I2C_COMBO8", F_OFF_G, 4, 0, 2, sp7350grps_i2c_combo8),
	FNCE("I2C_COMBO9", F_OFF_G, 4, 2, 2, sp7350grps_i2c_combo9),
	FNCE("CLKGEN_DGO_MCU", F_OFF_G, 4, 4, 2, sp7350grps_clkgen_dgo_mcu),
	FNCE("CLKGEN_DGO_WIFI", F_OFF_G, 4, 6, 2, sp7350grps_clkgen_dgo_wifi),
	FNCE("CLKGEN_DGO_RTC", F_OFF_G, 4, 8, 2, sp7350grps_clkgen_dgo_rtc),
	FNCE("CLKGEN_DGO_PHY", F_OFF_G, 4, 10, 2, sp7350grps_clkgen_dgo_phy),
	FNCE("CLKGEN_DGO_GNCMA", F_OFF_G, 4, 12, 2,
	     sp7350grps_clkgen_dgo_gncma),
	FNCE("SPI_MASTER0", F_OFF_G, 4, 14, 2, sp7350grps_spi_master0),

	FNCE("SPI_MASTER1", F_OFF_G, 5, 1, 2, sp7350grps_spi_master1),
	FNCE("SPI_MASTER2", F_OFF_G, 5, 3, 1, sp7350grps_spi_master2),
	FNCE("SPI_MASTER3", F_OFF_G, 5, 4, 2, sp7350grps_spi_master3),
	FNCE("SPI_MASTER4", F_OFF_G, 5, 6, 1, sp7350grps_spi_master4),
	FNCE("SPI_SLAVE0", F_OFF_G, 5, 7, 2, sp7350grps_spi_slave0),
	FNCE("AUD_TDMTX_XCK", F_OFF_G, 5, 9, 1, sp7350grps_aud_tdmtx_xck),
	FNCE("AUD_DAC_XCK1", F_OFF_G, 5, 10, 1, sp7350grps_aud_dac_xck1),
	FNCE("AUD_DAC_XCK", F_OFF_G, 5, 11, 1, sp7350grps_aud_dac_xck),
	FNCE("AUD_AU2_DATA0", F_OFF_G, 5, 12, 1, sp7350grps_aud_au2_data0),
	FNCE("AUD_AU1_DATA0", F_OFF_G, 5, 13, 1, sp7350grps_aud_au1_data0),
	FNCE("AUD_AU2_CK", F_OFF_G, 5, 14, 1, sp7350grps_aud_au2_ck),
	FNCE("AUD_AU1_CK", F_OFF_G, 5, 15, 1, sp7350grps_aud_au1_ck),

	FNCE("AUD_AU_ADC_DATA0", F_OFF_G, 6, 0, 2, sp7350grps_aud_au_adc_data0),
	FNCE("AUD_ADC2_DATA0", F_OFF_G, 6, 2, 1, sp7350grps_aud_adc2_data0),
	FNCE("AUD_ADC1_DATA0", F_OFF_G, 6, 3, 1, sp7350grps_aud_adc1_data0),
	FNCE("AUD_TDM", F_OFF_G, 6, 4, 1, sp7350grps_aud_tdm),
	FNCE("SPDIF_IN", F_OFF_G, 6, 5, 3, sp7350grps_spdif_in),
	FNCE("SPDIF_OUT", F_OFF_G, 6, 8, 3, sp7350grps_spdif_out),
	FNCE("EXT_INT0", F_OFF_G, 6, 12, 2, sp7350grps_ext_int0),
	FNCE("EXT_INT1", F_OFF_G, 6, 14, 2, sp7350grps_ext_int1),

	FNCE("INT0", F_OFF_G, 7, 5, 4, sp7350grps_int0),
	FNCE("INT1", F_OFF_G, 7, 9, 4, sp7350grps_int1),
	FNCE("EXT_INT3", F_OFF_G, 7, 2, 2, sp7350grps_ext_int3),
	FNCE("EXT_INT2", F_OFF_G, 7, 0, 2, sp7350grps_ext_int2),

	FNCE("INT2", F_OFF_G, 8, 0, 4, sp7350grps_int2),
	FNCE("INT3", F_OFF_G, 8, 4, 4, sp7350grps_int3),
	FNCE("INT4", F_OFF_G, 8, 8, 4, sp7350grps_int4),
	FNCE("INT5", F_OFF_G, 8, 12, 4, sp7350grps_int5),

	FNCE("INT6", F_OFF_G, 9, 0, 4, sp7350grps_int6),
	FNCE("INT7", F_OFF_G, 9, 4, 4, sp7350grps_int7),

	FNCE("GPIO_AO_0_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int0),
	FNCE("GPIO_AO_1_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int1),
	FNCE("GPIO_AO_2_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int2),
	FNCE("GPIO_AO_3_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int3),
	FNCE("GPIO_AO_4_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int4),
	FNCE("GPIO_AO_5_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int5),
	FNCE("GPIO_AO_6_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int6),
	FNCE("GPIO_AO_7_INT", F_OFF_G, 9, 8, 2,
	     sp7350grps_gpio_ao_int7),

	FNCE("GPIO_AO_8_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int8),
	FNCE("GPIO_AO_9_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int9),
	FNCE("GPIO_AO_10_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int10),
	FNCE("GPIO_AO_11_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int11),
	FNCE("GPIO_AO_12_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int12),
	FNCE("GPIO_AO_13_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int13),
	FNCE("GPIO_AO_14_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int14),
	FNCE("GPIO_AO_15_INT", F_OFF_G, 9, 10, 2,
	     sp7350grps_gpio_ao_int15),

	FNCE("GPIO_AO_16_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int16),
	FNCE("GPIO_AO_17_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int17),
	FNCE("GPIO_AO_18_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int18),
	FNCE("GPIO_AO_19_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int19),
	FNCE("GPIO_AO_20_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int20),
	FNCE("GPIO_AO_21_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int21),
	FNCE("GPIO_AO_22_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int22),
	FNCE("GPIO_AO_23_INT", F_OFF_G, 9, 12, 2,
	     sp7350grps_gpio_ao_int23),

	FNCE("GPIO_AO_24_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int24),
	FNCE("GPIO_AO_25_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int25),
	FNCE("GPIO_AO_26_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int26),
	FNCE("GPIO_AO_27_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int27),
	FNCE("GPIO_AO_28_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int28),
	FNCE("GPIO_AO_29_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int29),
	FNCE("GPIO_AO_30_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int30),
	FNCE("GPIO_AO_31_INT", F_OFF_G, 9, 14, 2,
	     sp7350grps_gpio_ao_int31)
};

const size_t list_func_nums = ARRAY_SIZE(list_funcs);

static size_t grps_count;

static const char **group_name_list;
static struct grp2fp_map_t *g2fp_maps;

const char *sunplus_get_function_name_by_selector(unsigned int selector)
{
	return list_funcs[selector].name;
}

struct func_t *sunplus_get_function_by_selector(unsigned int selector)
{
	return &list_funcs[selector];
}

int sunplus_get_function_count(void)
{
	return list_func_nums;
}

const char *sunplus_get_group_name_by_selector(unsigned int selector)
{
	if (!group_name_list)
		return NULL;

	return group_name_list[selector];
}

struct grp2fp_map_t *sunplus_get_group_by_name(const char *name)
{
	int i;

	for (i = 0; i < grps_count; i++) {
		if (!strcmp(group_name_list[i], name))
			return &g2fp_maps[i];
	}

	return NULL;
}

struct grp2fp_map_t *sunplus_get_group_by_selector(unsigned int group_selector,
						   unsigned int func_selector)
{
	int i, j;
	struct func_t *func;

	if (group_selector > sunplus_get_pins_count() - 1)
		return &g2fp_maps[group_selector];

	/* function:GPIO */
	if (func_selector == 0)
		return &g2fp_maps[group_selector];

	/* group:GPIO0 ~GPIO105 */
	func = sunplus_get_function_by_selector(func_selector);

	for (i = 0; i < func->gnum; i++) {
		for (j = 0; j < func->grps[i].pnum; j++) {
			if (group_selector == func->grps[i].pins[j]) {
				return sunplus_get_group_by_name(
					func->grps[i].name);
			}
		}
	}

	return NULL;
}

int sunplus_get_groups_count(void)
{
	return grps_count;
}

void group_groups(struct udevice *dev)
{
	int i, k, j;

	group_name_list = NULL;
	grps_count = 0;
	size_t pins_count = sunplus_get_pins_count();

	// fill array of all groups

	grps_count = pins_count;

	// calc unique group names array size
	for (i = 0; i < list_func_nums; i++) {
		if (list_funcs[i].freg != F_OFF_G)
			continue;
		grps_count += list_funcs[i].gnum;
	}

	// fill up unique group names array
	group_name_list = devm_kzalloc(dev, (grps_count + 1) * sizeof(char *),
				       GFP_KERNEL);
	g2fp_maps = devm_kzalloc(dev,
				 (grps_count + 1) * sizeof(struct grp2fp_map_t),
				 GFP_KERNEL);

	// groups == pins
	j = 0;

	for (i = 0; i < pins_count; i++) {
		group_name_list[i] = sunplus_get_pin_name_by_selector(i);
		g2fp_maps[i].f_idx = 0;
		g2fp_maps[i].g_idx = i;
	}
	j = pins_count;

	for (i = 0; i < list_func_nums; i++) {
		if (list_funcs[i].freg != F_OFF_G)
			continue;

		for (k = 0; k < list_funcs[i].gnum; k++) {
			list_funcs[i].grps_sa[k] =
				(char *)list_funcs[i].grps[k].name;
			group_name_list[j] = list_funcs[i].grps[k].name;
			g2fp_maps[j].f_idx = i;
			g2fp_maps[j].g_idx = k;
			j++;
		}
	}
}

int sunplus_pin_function_association_query(unsigned int pin_selector,
					   unsigned int func_selector)
{
	int i, j;
	struct func_t *func = sunplus_get_function_by_selector(func_selector);

	for (i = 0; i < func->gnum; i++) {
		for (j = 0; j < func->grps[i].pnum; j++) {
			if (pin_selector == func->grps[i].pins[j])
				return 0;
		}
	}

	return -EEXIST;
}

int sunplus_group_function_association_query(unsigned int group_selector,
					     unsigned int func_selector)
{
	struct grp2fp_map_t *group_map =
		sunplus_get_group_by_selector(group_selector, func_selector);

	if (group_map->f_idx == func_selector)
		return 0;

	return -EEXIST;
}
