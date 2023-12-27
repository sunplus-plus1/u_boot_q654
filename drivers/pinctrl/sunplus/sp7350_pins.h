/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SP7350_PINS_H
#define __SP7350_PINS_H

#include <linux/types.h>

const char *sunplus_get_pin_name_by_selector(unsigned int selector);
size_t sunplus_get_pins_count(void);

#endif /* __SP7350_PINS_H */
