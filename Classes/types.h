/*
 * Types.h – useful declarations 'n junk
 *
 */

#ifndef TYPES_H
#define TYPES_H

#define NO_PREV_POSITION -27139
#define INFINITAH 9999999

// Game stages
#define TUTORIAL_STAGE 2649
#define DEMON_STAGE 2651

// Coordinates

typedef struct intcoord {
	int x, y;
} intcoord;
typedef struct floatcoord {
	float a, b;
} floatcoord;

#define LEFTWARD  0
#define UPWARD    1
#define RIGHTWARD 2
#define DOWNWARD  3

typedef int direction;
typedef struct door {
	intcoord coord;
	direction orientation;
} door;

enum colour_name { BLACK, WHITE, RED, GREEN, BLUE, YELLOW };

#endif
