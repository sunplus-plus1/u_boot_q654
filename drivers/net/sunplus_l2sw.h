/*
 * Sunplus l2sw ethernet driver for u-boot
 */

#ifndef __SUNPLUS_L2SW_H__
#define __SUNPLUS_L2SW_H__

#include <net.h>
#include <linux/compiler.h>


//#define ZEBU_XTOR

#ifdef ZEBU_XTOR
// mac_force_mode0[11:10]: force_gmii_en[1:0]   = 0x3 (enable force function)
// mac_force_mode0[17:16]: force_gmii_spd0[1:0] = 0x3 (1G)
// mac_force_mode0[19:18]: force_gmii_spd1[1:0] = 0x3 (1G)
// mac_force_mode0[27:26]: force_gmii_dpx[1:0]  = 0x3 (no force duplex)
#define MAC_FORCE_MODE0 0x0c0f0c00
// mac_force_mode1[17:16]: force_gmii_xfc0[1:0] = 0x3 (full duplex and tx flow control)
// mac_force_mode1[19:18]: force_gmii_xfc1[1:0] = 0x3 (full duplex and tx flow control)
// mac_force_mode1[27:26]: force_gmii_link[1:0] = 0x3 (link up)
#define MAC_FORCE_MODE1 0x0c0f0000
#endif


// debug macros
#define eth_err(fmt, arg...)            printf(fmt, ##arg)
#if 1
#define eth_info(fmt, arg...)           printf(fmt, ##arg)
#else
#define eth_info(fmt, arg...)
#endif


// tx/rx descriptor cmd1 bits
#define OWN_BIT                         (1<<31)
#define FS_BIT                          (1<<25)
#define LS_BIT                          (1<<24)
#define LEN_MASK                        0x000007FF

// tx/rx descriptor cmd2 bits
#define EOR_BIT                         (1<<31)

// Address table search
#define MAC_ADDR_LOOKUP_IDLE            (1<<2)
#define MAC_SEARCH_NEXT_ADDR            (1<<1)
#define MAC_BEGIN_SEARCH_ADDR           (1<<0)

// Address table status
#define MAC_HASK_LOOKUP_ADDR_MASK       (0x3ff<<22)
#define MAC_AT_TABLE_END                (1<<1)
#define MAC_AT_DATA_READY               (1<<0)

// Register write & read
#define HWREG_W(M, N)                   writel(N, (void*)&l2sw_reg_base->M)
#define HWREG_R(M)                      readl((void*)&l2sw_reg_base->M)

// Queue defines
#define CONFIG_TX_DESCR_NUM             4
#define CONFIG_TX_QUEUE_NUM             2
#define CONFIG_RX_DESCR_NUM             4
#define CONFIG_RX_QUEUE_NUM             2
#define CONFIG_ETH_BUFSIZE              2048            /* must be dma aligned */
#define TX_BUF_MIN_SZ                   60

#define CONFIG_ETH_RXSIZE               2046            /* must fit in ETH_BUFSIZE */

#define TX_TOTAL_BUFSIZE                (CONFIG_ETH_BUFSIZE * CONFIG_TX_DESCR_NUM * CONFIG_TX_QUEUE_NUM)
#define RX_TOTAL_BUFSIZE                (CONFIG_ETH_BUFSIZE * CONFIG_RX_DESCR_NUM * CONFIG_RX_QUEUE_NUM)

#define CONFIG_MDIO_TIMEOUT             (3 * CONFIG_SYS_HZ)


#ifdef CONFIG_SUNPLUS_GL2SW
#define CONFIG_SYS_CACHELINE_SIZE       64
#endif
#define DCACHE_ROUNDDN(x)               (((long)(x))&(~(CONFIG_SYS_CACHELINE_SIZE-1)))
#define DCACHE_ROUNDUP(x)               (DCACHE_ROUNDDN(x-1)+CONFIG_SYS_CACHELINE_SIZE)


DECLARE_GLOBAL_DATA_PTR;

struct spl2sw_desc {
	volatile u32 cmd1;
	volatile u32 cmd2;
	volatile u32 addr1;
	volatile u32 addr2;
};

struct emac_eth_dev {
	volatile struct spl2sw_desc rx_desc[CONFIG_RX_DESCR_NUM*CONFIG_RX_QUEUE_NUM] __aligned(ARCH_DMA_MINALIGN);
	volatile struct spl2sw_desc tx_desc[CONFIG_TX_DESCR_NUM*CONFIG_TX_QUEUE_NUM] __aligned(ARCH_DMA_MINALIGN);

	volatile char rxbuffer[RX_TOTAL_BUFSIZE] __aligned(ARCH_DMA_MINALIGN);
	volatile char txbuffer[TX_TOTAL_BUFSIZE] __aligned(ARCH_DMA_MINALIGN);

	volatile u32 tx_pos;
	volatile u32 rx_pos;

	struct mii_dev *bus;

	struct phy_device *phy_dev0;
	struct phy_device *phy_dev1;

	u32 interface;
	u32 phy_addr0;
	u32 phy_addr1;
	u32 phy_configured;

	u8 mac_addr[8];
	u32 otp_mac_addr;
};


#ifdef CONFIG_SUNPLUS_L2SW
//=================================================================================================
/*
 * TYPE: RegisterFile_L2SW
 */
struct l2sw_reg {
	u32 sw_int_status;
	u32 sw_int_mask;
	u32 fl_cntl_th;
	u32 cpu_fl_cntl_th;
	u32 pri_fl_cntl;
	u32 vlan_pri_th;
	u32 En_tos_bus;
	u32 Tos_map0;
	u32 Tos_map1;
	u32 Tos_map2;
	u32 Tos_map3;
	u32 Tos_map4;
	u32 Tos_map5;
	u32 Tos_map6;
	u32 Tos_map7;
	u32 global_que_status;
	u32 addr_tbl_srch;
	u32 addr_tbl_st;
	u32 MAC_ad_ser0;
	u32 MAC_ad_ser1;
	u32 wt_mac_ad0;
	u32 w_mac_15_0;
	u32 w_mac_47_16;
	u32 PVID_config0;
	u32 PVID_config1;
	u32 VLAN_memset_config0;
	u32 VLAN_memset_config1;
	u32 port_ability;
	u32 port_st;
	u32 cpu_cntl;
	u32 port_cntl0;
	u32 port_cntl1;
	u32 port_cntl2;
	u32 sw_glb_cntl;
	u32 l2sw_rsv1;
	u32 led_port0;
	u32 led_port1;
	u32 led_port2;
	u32 led_port3;
	u32 led_port4;
	u32 watch_dog_trig_rst;
	u32 watch_dog_stop_cpu;
	u32 phy_cntl_reg0;
	u32 phy_cntl_reg1;
	u32 mac_force_mode;
	u32 VLAN_group_config0;
	u32 VLAN_group_config1;
	u32 flow_ctrl_th3;
	u32 queue_status_0;
	u32 debug_cntl;
	u32 l2sw_rsv2;
	u32 mem_test_info;
	u32 sw_int_status_1;
	u32 sw_int_mask_1;
	u32 l2sw_rsv3[76];
	u32 cpu_tx_trig;
	u32 tx_hbase_addr;
	u32 tx_lbase_addr;
	u32 rx_hbase_addr;
	u32 rx_lbase_addr;
	u32 tx_hw_addr;
	u32 tx_lw_addr;
	u32 rx_hw_addr;
	u32 rx_lw_addr;
	u32 cpu_port_cntl_reg_0;
	u32 tx_hbase_addr_1;
	u32 tx_lbase_addr_1;
	u32 rx_hbase_addr_1;
	u32 rx_lbase_addr_1;
	u32 tx_hw_addr_1;
	u32 tx_lw_addr_1;
	u32 rx_hw_addr_1;
	u32 rx_lw_addr_1;
	u32 cpu_port_cntl_reg_1;
};

#else
//=================================================================================================
/*
 * TYPE: RegisterFile_GL2SW
 */
struct l2sw_reg {
	u32 sw_int_status;
	u32 sw_int_mask;
	u32 fl_cntl_th;
	u32 cpu_fl_cntl_th;
	u32 pri_fl_cntl;
	u32 vlan_pri_th;
	u32 En_tos_bus;
	u32 Tos_map0;
	u32 Tos_map1;
	u32 Tos_map2;
	u32 Tos_map3;
	u32 Tos_map4;
	u32 Tos_map5;
	u32 Tos_map6;
	u32 Tos_map7;
	u32 global_que_status;
	u32 addr_tbl_srch;
	u32 addr_tbl_st;
	u32 MAC_ad_ser0;
	u32 MAC_ad_ser1;
	u32 wt_mac_ad0;
	u32 w_mac_15_0;
	u32 w_mac_47_16;
	u32 PVID_config0;
	u32 reserved0;
	u32 VLAN_memset_config0;
	u32 VLAN_memset_config1;
	u32 port_ability;
	u32 port_st;
	u32 cpu_cntl;
	u32 port_cntl0;
	u32 port_cntl1;
	u32 port_cntl2;
	u32 sw_glb_cntl;
	u32 sw_reset;
	u32 led_port0;
	u32 led_port1;
	u32 reserved1[3];
	u32 watch_dog_trig_rst;
	u32 watch_dog_stop_cpu;
	u32 phy_cntl_reg0;
	u32 phy_cntl_reg1;
	u32 mac_force_mode0;
	u32 VLAN_group_config0;
	u32 reserved2;
	u32 flow_ctrl_th3;
	u32 queue_status_0;
	u32 debug_cntl;
	u32 debug_info;
	u32 mem_test_info;
	u32 sw_global_signal;
	u32 pause_uc_sa_sw_15_0;
	u32 pause_uc_sa_sw_47_16;
	u32 reserved3[2];
	u32 mac_force_mode1;
	u32 p0_softpad_config;
	u32 p1_softpad_config;
	u32 reserved4[70];
	u32 cpu_tx_trig;
	u32 tx_hbase_addr;
	u32 tx_lbase_addr;
	u32 rx_hbase_addr;
	u32 rx_lbase_addr;
	u32 tx_hw_addr;
	u32 tx_lw_addr;
	u32 rx_hw_addr;
	u32 rx_lw_addr;
	u32 cpu_port_cntl_reg;
	u32 desc_addr_cntl;
};
#endif

#endif /* __SUNPLUS_L2SW_H__ */
