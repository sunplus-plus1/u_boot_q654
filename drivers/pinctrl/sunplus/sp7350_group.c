#include "pinctrl_sunplus.h"

static const unsigned pins_spif[] = { 6, 7, 8, 9, 10, 11 };
static const sppctlgrp_t sp7350grps_spif[] = {
	EGRP("SPI_FLASH", 1, pins_spif),
};

static const unsigned pins_emmc[] = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };
static const sppctlgrp_t sp7350grps_emmc[] = {
	EGRP("CARD0_EMMC", 1, pins_emmc)
};

static const unsigned pins_snand1[] = { 16, 17, 18, 19, 20, 21 };
static const unsigned pins_snand2[] = { 6,   7,  8,  9, 10, 11 };
static const sppctlgrp_t sp7350grps_snand[] = {
	EGRP("SPI_NAND1", 1, pins_snand1),
	EGRP("SPI_NAND2", 2, pins_snand2)
};

static const unsigned pins_sdc30[] = { 28, 29, 30, 31, 32, 33 };
static const sppctlgrp_t sp7350grps_sdc30[] = {
	EGRP("SD_CARD", 1, pins_sdc30)
};

static const unsigned pins_sdio30[] = { 34, 35, 36, 37, 38, 39 };
static const sppctlgrp_t sp7350grps_sdio30[] = {
	EGRP("SDIO", 1, pins_sdio30)
};

static const unsigned pins_pnand[] = { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21 };
static const sppctlgrp_t sp7350grps_pnand[] = {
	EGRP("PARA_NAND", 1, pins_pnand)
};

static const unsigned pins_usbotg[] = { 40 };
static const sppctlgrp_t sp7350grps_usbotg[] = {
	EGRP("USB_OTG", 1, pins_usbotg)
};

static const unsigned int pins_gmac[] = { 40, 41, 42, 43 };
static const sppctlgrp_t sp7350grps_gmac[] = {
	EGRP("GMAC", 1, pins_gmac)
};

static const unsigned pins_uart0[] = { 22, 23 };
static const sppctlgrp_t sp7350grps_uart0[] = {
	EGRP("UART0", 1, pins_uart0)
};

static const unsigned pins_uart1[] = { 24, 25, 26, 27 };
static const sppctlgrp_t sp7350grps_uart1[] = {
	EGRP("UART1", 1, pins_uart1)
};

static const unsigned pins_uart2[] = { 40, 41, 42, 43 };
static const sppctlgrp_t sp7350grps_uart2[] = {
	EGRP("UART2", 1, pins_uart2)
};

static const unsigned pins_uart3[] = { 44, 45 };
static const sppctlgrp_t sp7350grps_uart3[] = {
	EGRP("UART3", 1, pins_uart3)
};

static const unsigned pins_ua2axi[] = { 46, 47 };
static const sppctlgrp_t sp7350grps_ua2axi[] = {
	EGRP("UA2AXI", 1, pins_ua2axi)
};

static const unsigned pins_uart6[] = { 48, 49 };
static const sppctlgrp_t sp7350grps_uart6[] = {
	EGRP("UART6", 1, pins_uart6)
};

static const unsigned pins_uart7[] = { 50, 51 };
static const sppctlgrp_t sp7350grps_uart7[] = {
	EGRP("UART7", 1, pins_uart7)
};

static const unsigned pins_spicombo0[] = { 54, 55, 56, 57 };
static const sppctlgrp_t sp7350grps_spicombo0[] = {
	EGRP("SPI_COMBO0", 1, pins_spicombo0)
};

static const unsigned pins_spicombo1[] = { 58, 59, 60, 61 };
static const sppctlgrp_t sp7350grps_spicombo1[] = {
	EGRP("SPI_COMBO1", 1, pins_spicombo1)
};

static const unsigned pins_spicombo2[] = { 63, 64, 65, 66 };
static const sppctlgrp_t sp7350grps_spicombo2[] = {
	EGRP("SPI_COMBO2", 1, pins_spicombo2)
};

static const unsigned pins_spicombo3[] = { 67, 68, 69, 70 };
static const sppctlgrp_t sp7350grps_spicombo3[] = {
	EGRP("SPI_COMBO3", 1, pins_spicombo3)
};

static const unsigned pins_spicombo4[] = { 71, 72, 73, 74 };
static const sppctlgrp_t sp7350grps_spicombo4[] = {
	EGRP("SPI_COMBO4", 1, pins_spicombo4)
};

static const unsigned pins_spicombo5[] = { 77, 78, 79, 80 };
static const sppctlgrp_t sp7350grps_spicombo5[] = {
	EGRP("SPI_COMBO5", 1, pins_spicombo5)
};

static const unsigned pins_i2ccombo0[] = { 75, 76 };
static const sppctlgrp_t sp7350grps_i2ccombo0[] = {
	EGRP("I2C_COMBO0", 1, pins_i2ccombo0)
};

static const unsigned pins_i2ccombo1[] = { 81, 82 };
static const sppctlgrp_t sp7350grps_i2ccombo1[] = {
	EGRP("I2C_COMBO1", 1, pins_i2ccombo1)
};

static const unsigned pins_i2ccombo2[] = { 83, 84 };
static const sppctlgrp_t sp7350grps_i2ccombo2[] = {
	EGRP("I2C_COMBO2", 1, pins_i2ccombo2)
};

static const unsigned pins_i2ccombo3[] = { 85, 86 };
static const sppctlgrp_t sp7350grps_i2ccombo3[] = {
	EGRP("I2C_COMBO3", 1, pins_i2ccombo3)
};

static const unsigned pins_i2ccombo4[] = { 87, 88 };
static const sppctlgrp_t sp7350grps_i2ccombo4[] = {
	EGRP("I2C_COMBO4", 1, pins_i2ccombo4)
};

static const unsigned pins_i2ccombo5[] = { 89, 90 };
static const sppctlgrp_t sp7350grps_i2ccombo5[] = {
	EGRP("I2C_COMBO5", 1, pins_i2ccombo5)
};

static const unsigned pins_i2ccombo6[] = { 89, 90 };
static const sppctlgrp_t sp7350grps_i2ccombo6[] = {
	EGRP("I2C_COMBO6", 1, pins_i2ccombo6)
};

static const unsigned pins_i2ccombo7[] = { 89, 90 };
static const sppctlgrp_t sp7350grps_i2ccombo7[] = {
	EGRP("I2C_COMBO7", 1, pins_i2ccombo7)
};

static const unsigned pins_i2ccombo8[] = { 89, 90 };
static const sppctlgrp_t sp7350grps_i2ccombo8[] = {
	EGRP("I2C_COMBO8", 1, pins_i2ccombo8)
};

static const unsigned pins_i2ccombo9[] = { 89, 90 };
static const sppctlgrp_t sp7350grps_i2ccombo9[] = {
	EGRP("I2C_COMBO9", 1, pins_i2ccombo9)
};

static const unsigned pins_pwm[] = { 58, 59, 60, 61 };
static const sppctlgrp_t sp7350grps_pwm[] = {
	EGRP("PWM", 1, pins_pwm)
};

static const unsigned pins_aud_dac_clk[] = { 62, 63 };
static const sppctlgrp_t sp7350grps_aud_dac_clk[] = {
	EGRP("AUD_DAC_CLK", 1, pins_aud_dac_clk)
};

static const unsigned pins_aud_tdmtx_xck[] = { 62 };
static const sppctlgrp_t sp7350grps_aud_tdmtx_xck[] = {
	EGRP("AUD_TDMTX_XCK", 1, pins_aud_tdmtx_xck)
};

static const unsigned pins_aud_au2_data0[] = { 5 };
static const sppctlgrp_t sp7350grps_aud_au2_data0[] = {
	EGRP("AUD_AU2_DATA0", 1, pins_aud_au2_data0)
};

static const unsigned pins_aud_au1_data0[] = { 4 };
static const sppctlgrp_t sp7350grps_aud_au1_data0[] = {
	EGRP("AUD_AU1_DATA0", 1, pins_aud_au1_data0)
};

static const unsigned pins_aud_au2_ck[] = { 96, 97 };
static const sppctlgrp_t sp7350grps_aud_au2_ck[] = {
	EGRP("AUD_AU2_CK", 1, pins_aud_au2_ck)
};

static const unsigned pins_aud_au1_ck[] = { 94, 95 };
static const sppctlgrp_t sp7350grps_aud_au1_ck[] = {
	EGRP("AUD_AU1_CK", 1, pins_aud_au1_ck)
};

static const unsigned pins_aud_au_adc_data0[] = { 3, 64, 92, 93 };
static const sppctlgrp_t sp7350grps_aud_au_adc_data0[] = {
	EGRP("AUD_AU_ADC_DATA0", 1, pins_aud_au_adc_data0)
};

static const unsigned pins_aud_adc2_data0[] = { 5 };
static const sppctlgrp_t sp7350grps_aud_adc2_data0[] = {
	EGRP("AUD_ADC2_DATA0", 1, pins_aud_adc2_data0)
};

static const unsigned pins_aud_adc1_data0[] = { 4 };
static const sppctlgrp_t sp7350grps_aud_adc1_data0[] = {
	EGRP("AUD_ADC1_DATA0", 1, pins_aud_adc1_data0)
};

static const unsigned pins_aud_aud_tdm[] = { 3, 64, 92, 93 };
static const sppctlgrp_t sp7350grps_aud_aud_tdm[] = {
	EGRP("AUD_AUD_TDM", 1, pins_aud_aud_tdm)
};

static const unsigned pins_spdif1[] = { 91 };
static const unsigned pins_spdif2[] = { 3  };
static const unsigned pins_spdif3[] = { 4  };
static const unsigned pins_spdif4[] = { 5  };
static const unsigned pins_spdif5[] = { 62 };
static const unsigned pins_spdif6[] = { 2  };
static const sppctlgrp_t sp7350grps_spdif_in[] = {
	EGRP("SPDIF_IN_X1", 1, pins_spdif1),
	EGRP("SPDIF_IN_X2", 2, pins_spdif2),
	EGRP("SPDIF_IN_X3", 3, pins_spdif3),
	EGRP("SPDIF_IN_X4", 4, pins_spdif4),
	EGRP("SPDIF_IN_X5", 5, pins_spdif5),
	EGRP("SPDIF_IN_X6", 6, pins_spdif6)
};
static const sppctlgrp_t sp7350grps_spdif_out[] = {
	EGRP("SPDIF_OUT_X1", 1, pins_spdif1),
	EGRP("SPDIF_OUT_X2", 2, pins_spdif2),
	EGRP("SPDIF_OUT_X3", 3, pins_spdif3),
	EGRP("SPDIF_OUT_X4", 4, pins_spdif4),
	EGRP("SPDIF_OUT_X5", 5, pins_spdif5),
	EGRP("SPDIF_OUT_X6", 6, pins_spdif6)
};

static const unsigned pins_int_x1[] = { 0 };
static const unsigned pins_int_x2[] = { 1 };
static const unsigned pins_int_x3[] = { 2 };
static const unsigned pins_int_x4[] = { 3 };
static const unsigned pins_int_x5[] = { 46 };
static const unsigned pins_int_x6[] = { 106 };
static const unsigned pins_int_x7[] = { 107 };
static const sppctlgrp_t sp7350grps_int0[] = {
	EGRP("INT0_X1", 1, pins_int_x1),
	EGRP("INT0_X2", 1, pins_int_x2),
	EGRP("INT0_X3", 1, pins_int_x3),
	EGRP("INT0_X4", 1, pins_int_x4),
	EGRP("INT0_X5", 1, pins_int_x5),
	EGRP("INT0_X6", 1, pins_int_x6),
	EGRP("INT0_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int1[] = {
	EGRP("INT1_X1", 1, pins_int_x1),
	EGRP("INT1_X2", 1, pins_int_x2),
	EGRP("INT1_X3", 1, pins_int_x3),
	EGRP("INT1_X4", 1, pins_int_x4),
	EGRP("INT1_X5", 1, pins_int_x5),
	EGRP("INT1_X6", 1, pins_int_x6),
	EGRP("INT1_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int2[] = {
	EGRP("INT2_X1", 1, pins_int_x1),
	EGRP("INT2_X2", 1, pins_int_x2),
	EGRP("INT2_X3", 1, pins_int_x3),
	EGRP("INT2_X4", 1, pins_int_x4),
	EGRP("INT2_X5", 1, pins_int_x5),
	EGRP("INT2_X6", 1, pins_int_x6),
	EGRP("INT2_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int3[] = {
	EGRP("INT3_X1", 1, pins_int_x1),
	EGRP("INT3_X2", 1, pins_int_x2),
	EGRP("INT3_X3", 1, pins_int_x3),
	EGRP("INT3_X4", 1, pins_int_x4),
	EGRP("INT3_X5", 1, pins_int_x5),
	EGRP("INT3_X6", 1, pins_int_x6),
	EGRP("INT3_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int4[] = {
	EGRP("INT4_X1", 1, pins_int_x1),
	EGRP("INT4_X2", 1, pins_int_x2),
	EGRP("INT4_X3", 1, pins_int_x3),
	EGRP("INT4_X4", 1, pins_int_x4),
	EGRP("INT4_X5", 1, pins_int_x5),
	EGRP("INT4_X6", 1, pins_int_x6),
	EGRP("INT4_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int5[] = {
	EGRP("INT5_X1", 1, pins_int_x1),
	EGRP("INT5_X2", 1, pins_int_x2),
	EGRP("INT5_X3", 1, pins_int_x3),
	EGRP("INT5_X4", 1, pins_int_x4),
	EGRP("INT5_X5", 1, pins_int_x5),
	EGRP("INT5_X6", 1, pins_int_x6),
	EGRP("INT5_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int6[] = {
	EGRP("INT6_X1", 1, pins_int_x1),
	EGRP("INT6_X2", 1, pins_int_x2),
	EGRP("INT6_X3", 1, pins_int_x3),
	EGRP("INT6_X4", 1, pins_int_x4),
	EGRP("INT6_X5", 1, pins_int_x5),
	EGRP("INT6_X6", 1, pins_int_x6),
	EGRP("INT6_X7", 1, pins_int_x7)
};
static const sppctlgrp_t sp7350grps_int7[] = {
	EGRP("INT7_X1", 1, pins_int_x1),
	EGRP("INT7_X2", 1, pins_int_x2),
	EGRP("INT7_X3", 1, pins_int_x3),
	EGRP("INT7_X4", 1, pins_int_x4),
	EGRP("INT7_X5", 1, pins_int_x5),
	EGRP("INT7_X6", 1, pins_int_x6),
	EGRP("INT7_X7", 1, pins_int_x7)
};

func_t list_funcs[] = {
	FNCN("GPIO",            fOFF_0, 0x00, 0, 0),
	FNCN("IOP",             fOFF_0, 0x00, 0, 0),

	FNCE("SPI_FLASH",       fOFF_G, 1, 0,  1, sp7350grps_spif),
	FNCE("CARD0_EMMC",      fOFF_G, 1, 1,  1, sp7350grps_emmc),
	FNCE("SPI_NAND",        fOFF_G, 1, 2,  2, sp7350grps_snand),
	FNCE("SD_CARD",         fOFF_G, 1, 4,  1, sp7350grps_sdc30),
	FNCE("SDIO",            fOFF_G, 1, 5,  1, sp7350grps_sdio30),
	FNCE("PARA_NAND",       fOFF_G, 1, 6,  1, sp7350grps_pnand),
	FNCE("USB_OTG",         fOFF_G, 1, 7,  1, sp7350grps_usbotg),
	FNCE("GMAC",            fOFF_G, 1, 9,  1, sp7350grps_gmac),
	FNCE("UART3",           fOFF_G, 1, 10, 1, sp7350grps_uart3),
	FNCE("PWM",             fOFF_G, 1, 11, 1, sp7350grps_pwm),
	FNCE("UART0",           fOFF_G, 1, 12, 1, sp7350grps_uart0),
	FNCE("UART1",           fOFF_G, 1, 13, 1, sp7350grps_uart1),
	FNCE("UART2",           fOFF_G, 1, 14, 1, sp7350grps_uart2),
	FNCE("UA2AXI",          fOFF_G, 1, 15, 1, sp7350grps_ua2axi),

	FNCE("UART6",           fOFF_G, 2, 0,  1, sp7350grps_uart6),
	FNCE("UART7",           fOFF_G, 2, 1,  1, sp7350grps_uart7),
	FNCE("I2C_COMBO3",      fOFF_G, 2, 2,  1, sp7350grps_i2ccombo3),
	FNCE("I2C_COMBO4",      fOFF_G, 2, 3,  1, sp7350grps_i2ccombo4),
	FNCE("I2C_COMBO5",      fOFF_G, 2, 4,  1, sp7350grps_i2ccombo5),
	FNCE("I2C_COMBO6",      fOFF_G, 2, 5,  1, sp7350grps_i2ccombo6),
	FNCE("I2C_COMBO7",      fOFF_G, 2, 6,  1, sp7350grps_i2ccombo7),
	FNCE("SPI_COMBO0",      fOFF_G, 2, 7,  1, sp7350grps_spicombo0),
	FNCE("SPI_COMBO1",      fOFF_G, 2, 9,  1, sp7350grps_spicombo1),
	FNCE("SPI_COMBO2",      fOFF_G, 2, 10, 1, sp7350grps_spicombo2),
	FNCE("SPI_COMBO3",      fOFF_G, 2, 11, 1, sp7350grps_spicombo3),
	FNCE("SPI_COMBO4",      fOFF_G, 2, 12, 1, sp7350grps_spicombo4),
	FNCE("SPI_COMBO5",      fOFF_G, 2, 13, 1, sp7350grps_spicombo5),
	FNCE("I2C_COMBO0",      fOFF_G, 2, 14, 1, sp7350grps_i2ccombo0),
	FNCE("I2C_COMBO1",      fOFF_G, 2, 15, 1, sp7350grps_i2ccombo1),

	FNCE("I2C_COMBO2",      fOFF_G, 3, 0,  1, sp7350grps_i2ccombo2),
	FNCE("I2C_COMBO8",      fOFF_G, 3, 1,  1, sp7350grps_i2ccombo8),
	FNCE("I2C_COMBO9",      fOFF_G, 3, 2,  1, sp7350grps_i2ccombo9),
	FNCE("AUD_TDMTX_XCK",   fOFF_G, 3, 3,  1, sp7350grps_aud_tdmtx_xck),
	FNCE("AUD_DAC_CLK",     fOFF_G, 3, 4,  1, sp7350grps_aud_dac_clk),
	FNCE("AUD_AU2_DATA0",   fOFF_G, 3, 5,  1, sp7350grps_aud_au2_data0),
	FNCE("AUD_AU1_DATA0",   fOFF_G, 3, 6,  1, sp7350grps_aud_au1_data0),
	FNCE("AUD_AU2_CK",      fOFF_G, 3, 7,  1, sp7350grps_aud_au2_ck),
	FNCE("AUD_AU1_CK",      fOFF_G, 3, 8,  1, sp7350grps_aud_au1_ck),
	FNCE("AUD_AU_ADC_DATA0",fOFF_G, 3, 9,  1, sp7350grps_aud_au_adc_data0),
	FNCE("AUD_ADC2_DATA0",  fOFF_G, 3, 10, 1, sp7350grps_aud_adc2_data0),
	FNCE("AUD_ADC1_DATA0",  fOFF_G, 3, 11, 1, sp7350grps_aud_adc1_data0),
	FNCE("AUD_AUD_TDM",     fOFF_G, 3, 12, 1, sp7350grps_aud_aud_tdm),
	FNCE("SPDIF_IN",        fOFF_G, 3, 13, 3, sp7350grps_spdif_in),

	FNCE("SPDIF_OUT",       fOFF_G, 4, 0,  3, sp7350grps_spdif_out),
	FNCE("INT0",            fOFF_G, 4, 6,  3, sp7350grps_int0),
	FNCE("INT1",            fOFF_G, 4, 9,  3, sp7350grps_int1),
	FNCE("INT2",            fOFF_G, 4, 12, 3, sp7350grps_int2),
	FNCE("INT3",            fOFF_G, 5, 0,  3, sp7350grps_int3),
	FNCE("INT4",            fOFF_G, 5, 3,  3, sp7350grps_int4),
	FNCE("INT5",            fOFF_G, 5, 6,  3, sp7350grps_int5),
	FNCE("INT6",            fOFF_G, 5, 9,  3, sp7350grps_int6),
	FNCE("INT7",            fOFF_G, 5, 12, 3, sp7350grps_int7)
};

const int list_funcsSZ = ARRAY_SIZE(list_funcs);
