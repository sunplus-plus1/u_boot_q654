/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __SP7350_GROUP_H
#define __SP7350_GROUP_H

#include <dm/device.h>
void group_groups(struct udevice *dev);
const char *sunplus_get_group_name_by_selector(unsigned int selector);
int sunplus_get_groups_count(void);

struct grp2fp_map_t *sunplus_get_group_by_selector(unsigned int group_selector,
						   unsigned int func_selector);

const char *sunplus_get_function_name_by_selector(unsigned int selector);
struct func_t *sunplus_get_function_by_selector(unsigned int selector);
int sunplus_get_function_count(void);

int sunplus_pin_function_association_query(unsigned int pin_selector,
					   unsigned int func_selector);
int sunplus_group_function_association_query(unsigned int group_selector,
					     unsigned int func_selector);

#endif /* __SP7350_GROUP_H */
