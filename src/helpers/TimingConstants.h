// Joseph Arhar

#ifndef TIMING_CONSTANTS_H_
#define TIMING_CONSTANTS_H_

// Amount of "horizontal" player movement in world space per second
#define DELTA_X_PER_SECOND 6.6
// 60 ticks per second
#define TICKS_PER_SECOND 60.0

// Conversions
#define SECONDS_PER_TICK (1.0 / TICKS_PER_SECOND)
#define DELTA_X_PER_MS (DELTA_X_PER_SECOND / 1000.0)
#define MS_PER_TICK (SECONDS_PER_TICK * 1000.0)
#define DELTA_X_PER_TICK (DELTA_X_PER_SECOND * SECONDS_PER_TICK)

// Platforms
#define MS_PER_PLATFORM 900
// Spacing between platform positions - no regard for platform size/scale
#define PLATFORM_X_DELTA (DELTA_X_PER_MS * MS_PER_PLATFORM)
#define PLATFORM_Y_DELTA 0.4

#endif
