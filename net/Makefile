# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

#ccflags-y += -DDEBUG

obj-$(CONFIG_NET)      += arp.o
obj-$(CONFIG_CMD_BOOTP) += bootp.o
obj-$(CONFIG_CMD_CDP)  += cdp.o
obj-$(CONFIG_CMD_DNS)  += dns.o
obj-$(CONFIG_DM_DSA)   += dsa-uclass.o
ifdef CONFIG_DM_ETH
obj-$(CONFIG_NET)      += eth-uclass.o
else
obj-$(CONFIG_NET)      += eth_legacy.o
endif
obj-$(CONFIG_DM_MDIO)  += mdio-uclass.o
obj-$(CONFIG_DM_MDIO_MUX) += mdio-mux-uclass.o
obj-$(CONFIG_NET)      += eth_common.o
obj-$(CONFIG_CMD_LINK_LOCAL) += link_local.o
obj-$(CONFIG_NET)      += net.o
obj-$(CONFIG_CMD_NFS)  += nfs.o
obj-$(CONFIG_CMD_PING) += ping.o
obj-$(CONFIG_CMD_PCAP) += pcap.o
obj-$(CONFIG_CMD_RARP) += rarp.o
obj-$(CONFIG_CMD_SNTP) += sntp.o
obj-$(CONFIG_CMD_TFTPBOOT) += tftp.o
obj-$(CONFIG_UDP_FUNCTION_FASTBOOT)  += fastboot.o
obj-$(CONFIG_CMD_WOL)  += wol.o
obj-$(CONFIG_PROT_UDP) += udp.o
obj-$(CONFIG_PROT_TCP) += tcp.o
obj-$(CONFIG_CMD_WGET) += wget.o

# Disable this warning as it is triggered by:
# sprintf(buf, index ? "foo%d" : "foo", index)
# and this is intentional usage.
CFLAGS_eth_common.o += -Wno-format-extra-args
