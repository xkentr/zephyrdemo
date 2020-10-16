// Copyright (c) 2020
// Author: Kent Ryhorchuk

#include <zephyr.h>
#include <sys/printk.h>
#include "accel.h"
#include "odo.h"

void main(void)
{
	accel_init(ACCEL_RANGE_8G, ACCEL_RATE_20HZ);
	odo_init();
	int cnt = 0;
	while (1) {
		// Need to emulate a 20Hz timer here because I could not get
		// k_timers to work reliably under QEMU
		k_msleep(50);
		odo_work_handler(NULL);
		if (0 == ++cnt % 20) {
			float d = odo_get();
			printf("Distance traveled: %f m\n", d);
			printk("Running for %d seconds\n", cnt/20);
		}
	}
}
