/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Common header file for U-Boot
 *
 * This file still includes quite a few headers that should be included
 * individually as needed. Patches to remove things are welcome.
 *
 * (C) Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#ifndef __COMMON_H_
#define __COMMON_H_	1

#include "../lib/zstd/mem.h"
#ifndef __ASSEMBLY__		/* put C only stuff in this section */
#include <config.h>
#include <errno.h>
#include <time.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/string.h>
#include <stdarg.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <asm/u-boot.h> /* boot information for Linux kernel */
#include <display_options.h>
void	flush_dcache_range(unsigned long start, unsigned long stop);
void	invalidate_dcache_range(unsigned long start, unsigned long stop);
#include <vsprintf.h>
#endif	/* __ASSEMBLY__ */

/* Pull in stuff for the build system */
#ifdef DO_DEPS_ONLY
# include <env_internal.h>
#endif

#endif	/* __COMMON_H_ */
