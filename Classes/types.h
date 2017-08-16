/*
 * Types.h – useful constant, struct & other type declarations.
 *
 */

#ifndef TYPES_H
#define TYPES_H

#define NO_PREV_POSITION -27139

// Coordinates

typedef struct intcoord {
	int x, y;
} intcoord;

typedef struct floatcoord {
	float a, b;
} floatcoord;


// Events
typedef int dbt_event;

#define UNKNOWNEVENT -30141
#define CLOSED     -30142
#define KEYPRESS   -30143
#define MOUSEMOVE  -30144
#define LEFTCLICK  -30145
#define RIGHTCLICK -30146

// Keys

#define K_A -30147
#define K_B -30148
#define K_C -30149
#define K_D -30150
#define K_E -30151
#define K_F -30152
#define K_G -30153
#define K_H -30154
#define K_I -30155
#define K_J -30156
#define K_K -30157
#define K_L -30158
#define K_M -30159
#define K_N -30160
#define K_O -30161
#define K_P -30162
#define K_Q -30163
#define K_R -30164
#define K_S -30165
#define K_T -30166
#define K_U -30167
#define K_V -30168
#define K_W -30169
#define K_X -30170
#define K_Y -30171
#define K_Z -30172
#define K_0 -30173
#define K_1 -30174
#define K_2 -30175
#define K_3 -30176
#define K_4 -30177
#define K_5 -30178
#define K_6 -30179
#define K_7 -30180
#define K_8 -30181
#define K_9 -30182

#define K_ESC    -30183
#define K_RETURN -30184
//#define K_ESC    -30185
//#define K_ESC    -30186
//#define K_ESC    -30187
//#define K_ESC    -30188
//#define K_ESC    -30189
//#define K_ESC    -30190
//#define K_ESC    -30191
//#define K_ESC    -30192
//#define K_ESC    -30193
//#define K_ESC    -30194

#endif
