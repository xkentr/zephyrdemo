// Copyright (c) 2020
// Author: Kent Ryhorchuk
#pragma once

// Odometry module

#include <stdint.h>

// Initialize internals.
// Assumes hardware is ready (accel_init has been called).
int odo_init();

// Return distance traveled in meters.
// TODO: analyze is float is the right unit here.
float odo_get();

// DEBUG - export the work handler because we are having trouble with timers.
void odo_work_handler(struct k_work *work);
