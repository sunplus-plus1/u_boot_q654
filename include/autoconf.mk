CONFIG_CLOCKS=y
CONFIG_SYS_MMC_ENV_DEV=0
CONFIG_ENV_OVERWRITE=y
CONFIG_SYS_BAUDRATE_TABLE="{ 115200, 921600 }"
CONFIG_BOOTCOMMAND="echo [scr] bootcmd started; md.l ${bootinfo_base} 1; if itest.l *${bootinfo_base} == " __stringify(SPI_NOR_BOOT) "; then if itest ${if_zebu} == 1; then if itest ${if_qkboot} == 1; then echo [scr] qk zmem boot; run qk_zmem_boot; else echo [scr] zmem boot; run zmem_boot; fi; else if itest ${if_qkboot} == 1; then echo [scr] qk romter boot; run qk_romter_boot; elif itest.s ${sp_main_storage} == tftp; then echo [scr] tftp_boot; run tftp_boot; else echo [scr] romter boot; run romter_boot; fi; fi; elif itest.l *${bootinfo_base} == " __stringify(EMMC_BOOT) "; then if itest ${if_zebu} == 1; then echo [scr] zebu emmc boot; run zebu_emmc_boot; else if itest ${if_qkboot} == 1; then echo [scr] qk emmc boot; run qk_emmc_boot; else echo [scr] emmc boot; run emmc_boot; fi; fi; elif itest.l *${bootinfo_base} == " __stringify(SPINAND_BOOT) "; then echo [scr] spinand boot; run spinand_boot; elif itest.l *${bootinfo_base} == " __stringify(PARA_NAND_BOOT) "; then echo [scr] pnand boot; run pnand_boot; elif itest.l *${bootinfo_base} == " __stringify(USB_ISP) "; then echo [scr] ISP from USB storage; run isp_usb; elif itest.l *${bootinfo_base} == " __stringify(USB_BOOT) "; then echo [scr] Boot from USB storage; run usb_boot; elif itest.l *${bootinfo_base} == " __stringify(SDCARD_ISP) "; then echo [scr] ISP from SD Card; run isp_sdcard; elif itest.l *${bootinfo_base} == " __stringify(SDCARD_BOOT) "; then echo [scr] Boot from SD Card; run sdcard_boot; else echo Stop; fi"
CONFIG_SYS_SDRAM_BASE=0
CONFIG_ARCH_MISC_INIT=y
CONFIG_SYS_INIT_SP_ADDR="(1 << 20)"
CONFIG_SUPPORT_EMMC_BOOT=y
CONFIG_SYS_SDRAM_SIZE="(1024 << 20)"
CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS=2
