// Copyright (c) 2020
// Author: Kent Ryhorchuk
#pragma once

// Accelerometer API

#include <stdint.h>

// Defines a range of the accelerometer.
typedef enum {
    ACCEL_RANGE_2G = 1,
    ACCEL_RANGE_4G,
    ACCEL_RANGE_8G,
} accel_range_t;

typedef enum {
    ACCEL_RATE_20HZ,
    ACCEL_RATE_50HZ,
    ACCEL_RATE_100HZ,
} accel_sample_rate_t;

// Initialize accelerometer (placeholder)
int accel_init(accel_range_t range, accel_sample_rate_t rate);

// Get the last measurement from the accelerometer.
int accel_get_sample(int16_t data[3]);
