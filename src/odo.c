// Copyright (c) 2020
// Author: Kent Ryhorchuk

#include "accel.h"
#include <math.h>
#include <string.h>
#include <zephyr.h>
#include <sys/printk.h>

// Configuration section
// TODO - make this more configurable
#define SAMPLE_HZ (20)
#define SAMPLE_S (0.05f)
#define BASELINE_S (20)
#define BASELINE_SAMPLES (SAMPLE_HZ * BASELINE_S)
#define g_ACCEL (9.80665f)
#define g_CNT (8.0f/32768.0f)
#define A_THRESHOLD (0.01f)
#define V_THRESHOLD (0.01f)

// High-pass filter
#define FILTER_HZ (0.75f)
#define ALPHA (1.0f/(3.1415f*SAMPLE_S*FILTER_HZ + 1.0f))

// These are for readability of the code below
#define X 0
#define Y 1
#define Z 2

// Compute res = a-b
#define VSUB(R, A, B) do { \
    R[X] = A[X] - B[X]; \
    R[Y] = A[Y] - B[Y]; \
    R[Z] = A[Z] - B[Z]; \
} while (0)

#define VADD(R, A, B) do { \
    R[X] = A[X] + B[X]; \
    R[Y] = A[Y] + B[Y]; \
    R[Z] = A[Z] + B[Z]; \
} while (0)

#define VSCALE(R, A, S) do { \
    R[X] = ((float)A[X] * S); \
    R[Y] = ((float)A[Y] * S); \
    R[Z] = ((float)A[Z] * S); \
} while (0)

// Total number of samples collected ever
static uint64_t n_samples = 0;

// Moving average filter for input.
static uint32_t baseline_p = 0;
static int16_t baseline_hist[BASELINE_SAMPLES][3] = {0};
static int32_t baseline_accum[3] = {0};

// Accumulated acceleration
static float velocity[3] = {0};

// Accumulated magnitude of acceleration
static float distance = 0.0f;

#define BASELINE_AVG(in, out, i) do {\
    baseline_accum[i] -= baseline_hist[baseline_p][i]; \
    baseline_hist[baseline_p][i] = in[i]; \
    baseline_accum[i] += in[i]; \
    out[i] = baseline_accum[i]/BASELINE_SAMPLES; \
} while (0)

// Compute moving average of each acceleration component
static void basline_filter(int16_t in[3], int16_t out[3])
{
    BASELINE_AVG(in, out, X);
    BASELINE_AVG(in, out, Y);
    BASELINE_AVG(in, out, Z);
    baseline_p += 1;
    baseline_p %= BASELINE_SAMPLES;
}

// Subtract the given baseline from the input
static void remove_baseline(int16_t in[3], int16_t baseline[3], int16_t out[3])
{
    VSUB(out, in, baseline);
}

// Convert 16-bit 8g range accelerometer input to m/s/s in float
static void convert_to_accel(int16_t in[3], float out[3])
{
    out[X] = (float)in[X] * g_CNT * g_ACCEL;
    out[Y] = (float)in[Y] * g_CNT * g_ACCEL;
    out[Z] = (float)in[Z] * g_CNT * g_ACCEL;
}

// Compute the magnitude of the given vector
static float magnitude(float in[3])
{
    double x = in[X];
    double y = in[Y];
    double z = in[Z];

    // Magnitude squared
    double mag = x*x + y*y + z*z;

    // Square root and convert to float
    float magf = sqrt(mag);

    return magf;
}

// Threshold the given vector, component-wise.
static void accel_threshold(float in[3], float out[3])
{
    float mag = magnitude(in);
    //printk("mag=%d, thresh=%d\n", (int)mag, (int)A_THRESHOLD);
    if (mag > A_THRESHOLD) {
        out[X] = in[X];
        out[Y] = in[Y];
        out[Z] = in[Z];
    } else {
        out[X] = 0.0f;
        out[Y] = 0.0f;
        out[Z] = 0.0f;
    }

}

// Accumulate with high-pass filter.
// Effect is Vn = alpha*Vn-1 + alpha*An
// This keeps the V from being stuck at a DC-offset
static void sum_accel(float accum[3], float in[3])
{
    VADD(accum, accum, in);
    VSCALE(accum, accum, ALPHA);
}

// Threshold a float
static float threshold(float v, float t)
{
    return (v > t) ? v : 0.0f;
}

void odo_work_handler(struct k_work *work)
{
    n_samples++;

    int16_t x[3] = {0};
	accel_get_sample(x);

    int16_t baseline[3] = {0};
    basline_filter(x, baseline);
    if (n_samples < BASELINE_SAMPLES) {
        // Results are bad until an accurate baseline is collected
        return;
    }

    int16_t y[3] = {0};
    remove_baseline(x, baseline, y);

    float acceleration[3];
    convert_to_accel(y, acceleration);
    accel_threshold(acceleration, acceleration);
    sum_accel(velocity, acceleration);

    distance += threshold(magnitude(velocity), V_THRESHOLD) * SAMPLE_S;

#if 0
    printk("Odo x: (%d, %d, %d)\n", x[0], x[1], x[2]);
	printk("Odo y: (%d, %d, %d)\n", y[0], y[1], y[2]);
    printf("Odo acceleration: (%f, %f, %f)\n", acceleration[0], acceleration[1], acceleration[2]);
    printf("Odo velocity: (%f, %f, %f)\n", velocity[0], velocity[1], velocity[2]);
    printf("Odo distance: %f\n", distance);
#endif
}

struct k_work odo_work;

void odo_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&odo_work);
}

struct k_timer odo_timer;

int odo_init(void)
{
    // FIXME - Timers not working on QEMU, oh well
    //k_work_init(&odo_work, odo_work_handler);
    //k_timer_init(&odo_timer, odo_timer_handler, NULL);
	//k_timer_start(&odo_timer, K_MSEC(SAMPLE_MS), K_MSEC(SAMPLE_MS));
    return 0;
}

float odo_get()
{
    return distance;
}
