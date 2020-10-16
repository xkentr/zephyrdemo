// Copyright (c) 2020
// Author: Kent Ryhorchuk

#include "accel.h"
#include <zephyr.h>

// TODO - make this configurable
#define SAMPLE_MS 500

static void odo_work_handler(struct k_work *work)
{
    int16_t sample[3];
	accel_get_sample(sample);
	printk("Odo accel sample: (%d, %d, %d)\n", sample[0], sample[1], sample[2]);
}

K_WORK_DEFINE(odo_work, odo_work_handler);

static void odo_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&odo_work);
}

K_TIMER_DEFINE(odo_timer, odo_timer_handler, NULL);

int odo_init(void)
{
	k_timer_start(&odo_timer, K_MSEC(SAMPLE_MS), K_MSEC(SAMPLE_MS));
}

float odo_get()
{
    return 0.0f;
}
