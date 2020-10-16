// Copyright (c) 2020
// Author: Kent Ryhorchuk

#include "accel.h"
#include <zephyr.h>

static accel_range_t range = ACCEL_RANGE_8G;
static accel_sample_rate_t rate = ACCEL_RATE_20HZ;

// 1g acceleration in counts, according to range
#define ONE_G (0x8000UL/(1UL<<range))
// For simulation, a "push" will be 0.1g
#define PUSH_G (ONE_G/100UL)

int accel_init(accel_range_t _range, accel_sample_rate_t _rate)
{
    range = _range;
    rate = _rate;
    return 0;
}

int accel_get_sample(int16_t data[3])
{
    // Emulate a waveform.
    // Motionless for 15 seconds
    // X+PUSH_G for 2.5 seconds
    // X-PUSH_G for 2.5 seconds
    // Repeat (20 second cycle)

    // Base the sample off of the tick clock, not the sample count.
    // Makes this more "realistic"
    int64_t tick = k_uptime_get(); // ms
    tick %= 20000; // look at the current 10s cycle.
    if (tick < 15000)
        data[0] = 0;
    else if (tick < 17500)
        data[0] = PUSH_G;
    else
        data[0] = -1 * PUSH_G;

    data[1] = 0;
    data[2] = ONE_G;
    return 0;
}
