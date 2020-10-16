/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>
#include "accel.h"
#include "odo.h"

void my_work_handler(struct k_work *work)
{
    float d = odo_get();
	printf("Distance traveled: %f\n", d);
}

K_WORK_DEFINE(my_work, my_work_handler);

void my_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&my_work);
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

void main(void)
{
	accel_init(ACCEL_RANGE_8G, ACCEL_RATE_20HZ);
	odo_init();
	k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));
	k_timer_status_sync(&my_timer);
	while (1) {
		k_msleep(1000);
		//printk("Hello World! %s\n", CONFIG_BOARD);
	}
}
