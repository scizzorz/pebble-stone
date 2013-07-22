// app information
#define UUID {0x91,0x41,0xB6,0x28,0xBC,0x89,0x49,0x8E,0xB1,0x47,0xAD,0x2D,0x33,0xAA,0xF1,0xFE}
#define NAME "Two"
#define AUTHOR "John Weachock"
#define MAJOR_VERSION 0
#define MINOR_VERSION 1

// httpebble information
#define HTTP_KEY_WEATHER 1

#define POST_KEY_LAT 1
#define POST_KEY_LONG 2
#define POST_KEY_UNITS 3

#define REFRESH_DELAY 900000
#define BUFSIZE 32

// status information
#define STATUS_IDLE 0
#define STATUS_LOAD 1
#define STATUS_DISC 2
#define STATUS_ERR 3
#define STATUS_FAIL 4

// text position information
#define WIDTH 144
#define HEIGHT 168
#define TIME_H 28
#define DATE_H 26
#define WEATHER_H 26
#define PADDING 10

#define DATE_Y (HEIGHT/2 - DATE_H/2)
#define TIME_Y (DATE_Y - TIME_H - PADDING)
#define WEATHER_Y (DATE_Y + DATE_H + PADDING)
#define ICON_Y (WEATHER_Y + 8)
