#include <Arduino.h>


const uint8_t AIRLY_HAPPY_bits[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xfc, 0x03, 0x00, 0x00, 0xfc, 0x03, 0x00,
0x80, 0xff, 0x07, 0x00, 0x80, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x1f, 0x00, 0xc0, 0xff, 0x3f, 0x00,
0xc0, 0xff, 0x3f, 0x00, 0xe0, 0xe3, 0x78, 0x00, 0xf8, 0x5d, 0xf7, 0x03, 0xf8, 0xff, 0xff, 0x0f,
0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xfb, 0xfb, 0x1f, 0xfc, 0xf7, 0xfd, 0x0f,
0xfc, 0x0f, 0xfe, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x0d, 0x00, 0x00, 0xe0, 0x1f, 0x00,
0x00, 0xe0, 0x1f, 0x00, 0x3f, 0xe0, 0x1f, 0x00, 0x7f, 0xe0, 0x1d, 0x00, 0x7f, 0xc0, 0x0f, 0x00,
0xff, 0xe0, 0x07, 0x00, 0xff, 0xc7, 0x03, 0x00, 0xff, 0xcf, 0x03, 0x00, 0xff, 0xdf, 0x00, 0x00,
0xff, 0xef, 0x00, 0x00, 0xbf, 0xef, 0x00, 0x00, 0x2f, 0x3e, 0x00, 0x00, 0x4f, 0x07, 0x00, 0x00,
0x50, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AIRLY_Neutral_bits[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xfc, 0x03, 0x00,
0x80, 0xff, 0x0f, 0x00, 0x80, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x1f, 0x00, 0xc0, 0xff, 0x3f, 0x00,
0xc0, 0xff, 0x3f, 0x00, 0xe0, 0xff, 0x7f, 0x00, 0xf8, 0xf1, 0xf8, 0x03, 0xf8, 0xff, 0xff, 0x0f,
0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x1f, 0xfc, 0x07, 0xf8, 0x0f,
0xfc, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xd0, 0x14, 0x00, 0x00, 0xf4, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x40, 0xff, 0xff, 0x03,
0x80, 0xff, 0xff, 0x07, 0x80, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07,
0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x01, 0xe0, 0xff, 0xff, 0x00,
0xc0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x7f, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00,
0x00, 0xf8, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0xe0, 0x0f, 0x00,
0x00, 0x80, 0x07, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xe0, 0x1f, 0x00,
0x00, 0xe0, 0x1f, 0x00, 0x3f, 0xe0, 0x1f, 0x00, 0x7f, 0xe0, 0x1d, 0x00, 0x7f, 0xc0, 0x0f, 0x00,
0xff, 0xc0, 0x07, 0x00, 0xff, 0xc7, 0x03, 0x00, 0xff, 0xcf, 0x03, 0x00, 0xff, 0xdf, 0x00, 0x00,
0xff, 0xef, 0x00, 0x00, 0xbf, 0xef, 0x00, 0x00, 0x2f, 0x3e, 0x00, 0x00, 0x4f, 0x07, 0x00, 0x00,
0x50, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AIRLY_SAD_bits[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xfc, 0x03, 0x00,
0x80, 0xff, 0x07, 0x00, 0x80, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x1f, 0x00, 0xc0, 0xff, 0x3f, 0x00,
0xc0, 0xff, 0x3f, 0x00, 0xe0, 0xff, 0x7f, 0x00, 0xf8, 0x6d, 0xfb, 0x03, 0xf8, 0xf3, 0xfc, 0x07,
0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0x0f, 0xfe, 0x1f, 0xfc, 0xf7, 0xfd, 0x0f,
0xfc, 0xfb, 0xfb, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xb4, 0x00, 0x00,
0x40, 0xfb, 0x45, 0x02, 0xc8, 0xff, 0xff, 0x07, 0xf6, 0xff, 0xff, 0x0f, 0xfc, 0x7f, 0xff, 0x1f,
0xff, 0xbf, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xfb, 0xff, 0xff, 0x1f, 0xf3, 0xff, 0xff, 0x0f,
0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x0e, 0xfe, 0xff, 0xff, 0x0f,
0xfe, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0x01,
0xe0, 0xff, 0xff, 0x01, 0xe0, 0x5f, 0xff, 0x01, 0xf0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x03,
0xe0, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xf8, 0xff, 0xff, 0x03,
0xf0, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xdf, 0x01, 0xe0, 0xef, 0xdf, 0x00,
0xe0, 0xfb, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xfd, 0xff, 0x00, 0x40, 0xff, 0xff, 0x03,
0x80, 0xff, 0xff, 0x07, 0x80, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07,
0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x01, 0xc0, 0xff, 0xff, 0x00,
0xc0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x7f, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00,
0x00, 0xf8, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0xe0, 0x07, 0x00,
0x00, 0x80, 0x07, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xe0, 0x1f, 0x00,
0x00, 0xe0, 0x1f, 0x00, 0x3f, 0xe0, 0x1f, 0x00, 0x7f, 0xe0, 0x1d, 0x00, 0x7f, 0xc0, 0x0f, 0x00,
0xff, 0xc0, 0x07, 0x00, 0xff, 0xc7, 0x03, 0x00, 0xff, 0xcf, 0x03, 0x00, 0xff, 0x9f, 0x00, 0x00,
0xff, 0xef, 0x01, 0x00, 0x5f, 0xef, 0x00, 0x00, 0x5f, 0x3e, 0x00, 0x00, 0x4f, 0x07, 0x00, 0x00,
0x20, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


const uint8_t AIRLY_SAD2_bits[] PROGMEM = {
// python bmp2hex.py -i airly-sad2.bmp 
// better, but sideways
0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x3f, 0xc0, 0x00,
0x01, 0xff, 0xe0, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x03, 0xff, 0xf8, 0x00, 0x03, 0xff, 0xfc, 0x00,
0x03, 0xdf, 0xbc, 0x00, 0x07, 0xbf, 0xde, 0x00, 0x1f, 0x6f, 0x6f, 0xc0, 0x1f, 0xef, 0x7f, 0xe0,
0x3f, 0xff, 0xff, 0xf0, 0x3f, 0xff, 0xff, 0xf0, 0x3f, 0xf0, 0x7f, 0xf8, 0x3f, 0xef, 0xbf, 0xf0,
0x3f, 0xdf, 0xdf, 0xf0, 0x1f, 0xff, 0xff, 0xf0, 0x1f, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x2d, 0x00, 0x00,
0x02, 0xdf, 0xa2, 0x40, 0x13, 0xff, 0xff, 0xe0, 0x6f, 0xff, 0xff, 0xf0, 0x3f, 0xfe, 0xff, 0xf8,
0xff, 0xfd, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xdf, 0xff, 0xff, 0xf8, 0xcf, 0xff, 0xff, 0xf0,
0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0x70, 0x7f, 0xff, 0xff, 0xf0,
0x7f, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0x80,
0x07, 0xff, 0xff, 0x80, 0x07, 0xfa, 0xff, 0x80, 0x0f, 0xff, 0xff, 0xc0, 0x07, 0xff, 0xff, 0xc0,
0x07, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xc0, 0x1f, 0xff, 0xff, 0xc0,
0x0f, 0xff, 0xff, 0x80, 0x0f, 0xff, 0xff, 0x80, 0x0f, 0xff, 0xfb, 0x80, 0x07, 0xf7, 0xfb, 0x00,
0x07, 0xdf, 0xfe, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0xbf, 0xff, 0x00, 0x02, 0xff, 0xff, 0xc0,
0x01, 0xff, 0xff, 0xe0, 0x01, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xe0,
0x03, 0xff, 0xff, 0xe0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0x00,
0x03, 0xff, 0xfe, 0x00, 0x01, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xfe, 0x00, 0x00, 0x1f, 0xfe, 0x00,
0x00, 0x1f, 0xfe, 0x00, 0x00, 0x1f, 0xfe, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x07, 0xe0, 0x00,
0x00, 0x01, 0xe0, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x03, 0xe0, 0x00, 0x00, 0x07, 0xf8, 0x00,
0x00, 0x07, 0xf8, 0x00, 0xfc, 0x07, 0xf8, 0x00, 0xfe, 0x07, 0xb8, 0x00, 0xfe, 0x03, 0xf0, 0x00,
0xff, 0x03, 0xe0, 0x00, 0xff, 0xe3, 0xc0, 0x00, 0xff, 0xf3, 0xc0, 0x00, 0xff, 0xf9, 0x00, 0x00,
0xff, 0xf7, 0x80, 0x00, 0xfa, 0xf7, 0x00, 0x00, 0xfa, 0x7c, 0x00, 0x00, 0xf2, 0xe0, 0x00, 0x00,
0x04, 0x80, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// const uint8_t AIRLY_SAD2_bits[] PROGMEM = {
// // python bmp2hex.py -x airly-sad2.bmp 
// 0xff, 0xff, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0x03, 0xfc, 0xff,
// 0x7f, 0x00, 0xf8, 0xff, 0x7f, 0x00, 0xf8, 0xff, 0x3f, 0x00, 0xe0, 0xff, 0x3f, 0x00, 0xc0, 0xff,
// 0x3f, 0x04, 0xc2, 0xff, 0x1f, 0x02, 0x84, 0xff, 0x07, 0x09, 0x09, 0xfc, 0x07, 0x08, 0x01, 0xf8,
// 0x03, 0x00, 0x00, 0xf0, 0x03, 0x00, 0x00, 0xf0, 0x03, 0xf0, 0x01, 0xe0, 0x03, 0x08, 0x02, 0xf0,
// 0x03, 0x04, 0x04, 0xf0, 0x07, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff,
// 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x4b, 0xff, 0xff,
// 0xbf, 0x04, 0xba, 0xfd, 0x37, 0x00, 0x00, 0xf8, 0x09, 0x00, 0x00, 0xf0, 0x03, 0x80, 0x00, 0xe0,
// 0x00, 0x40, 0x00, 0xe0, 0x00, 0x00, 0x00, 0xe0, 0x04, 0x00, 0x00, 0xe0, 0x0c, 0x00, 0x00, 0xf0,
// 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00, 0xf1, 0x01, 0x00, 0x00, 0xf0,
// 0x01, 0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xf8, 0x01, 0x00, 0x00, 0xfe,
// 0x1f, 0x00, 0x00, 0xfe, 0x1f, 0xa0, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0xfc, 0x1f, 0x00, 0x00, 0xfc,
// 0x1f, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xfc, 0x0f, 0x00, 0x00, 0xfc, 0x07, 0x00, 0x00, 0xfc,
// 0x0f, 0x00, 0x00, 0xfe, 0x0f, 0x00, 0x00, 0xfe, 0x0f, 0x00, 0x20, 0xfe, 0x1f, 0x10, 0x20, 0xff,
// 0x1f, 0x04, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x02, 0x00, 0xff, 0xbf, 0x00, 0x00, 0xfc,
// 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0xf8,
// 0x3f, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0xfc, 0x3f, 0x00, 0x00, 0xfe, 0x3f, 0x00, 0x00, 0xff,
// 0x3f, 0x00, 0x80, 0xff, 0x7f, 0x00, 0x80, 0xff, 0xff, 0x03, 0x80, 0xff, 0xff, 0x07, 0x80, 0xff,
// 0xff, 0x07, 0x80, 0xff, 0xff, 0x07, 0x80, 0xff, 0xff, 0x1f, 0xc0, 0xff, 0xff, 0x1f, 0xf8, 0xff,
// 0xff, 0x7f, 0xf8, 0xff, 0xff, 0x7f, 0xfc, 0xff, 0xff, 0x3f, 0xf8, 0xff, 0xff, 0x1f, 0xe0, 0xff,
// 0xff, 0x1f, 0xe0, 0xff, 0xc0, 0x1f, 0xe0, 0xff, 0x80, 0x1f, 0xe2, 0xff, 0x80, 0x3f, 0xf0, 0xff,
// 0x00, 0x3f, 0xf8, 0xff, 0x00, 0x38, 0xfc, 0xff, 0x00, 0x30, 0xfc, 0xff, 0x00, 0x60, 0xff, 0xff,
// 0x00, 0x10, 0xfe, 0xff, 0xa0, 0x10, 0xff, 0xff, 0xa0, 0xc1, 0xff, 0xff, 0xb0, 0xf8, 0xff, 0xff,
// 0xdf, 0xfe, 0xff, 0xff, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
// 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
// };
// python bmp2hex.py -i -x airly-sad2.bmp 
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xfc, 0x03, 0x00,
// 0x80, 0xff, 0x07, 0x00, 0x80, 0xff, 0x07, 0x00, 0xc0, 0xff, 0x1f, 0x00, 0xc0, 0xff, 0x3f, 0x00,
// 0xc0, 0xfb, 0x3d, 0x00, 0xe0, 0xfd, 0x7b, 0x00, 0xf8, 0xf6, 0xf6, 0x03, 0xf8, 0xf7, 0xfe, 0x07,
// 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0xff, 0xff, 0x0f, 0xfc, 0x0f, 0xfe, 0x1f, 0xfc, 0xf7, 0xfd, 0x0f,
// 0xfc, 0xfb, 0xfb, 0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0xff, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00,
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xb4, 0x00, 0x00,
// 0x40, 0xfb, 0x45, 0x02, 0xc8, 0xff, 0xff, 0x07, 0xf6, 0xff, 0xff, 0x0f, 0xfc, 0x7f, 0xff, 0x1f,
// 0xff, 0xbf, 0xff, 0x1f, 0xff, 0xff, 0xff, 0x1f, 0xfb, 0xff, 0xff, 0x1f, 0xf3, 0xff, 0xff, 0x0f,
// 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x0f, 0xff, 0xff, 0xff, 0x0e, 0xfe, 0xff, 0xff, 0x0f,
// 0xfe, 0xff, 0xff, 0x0f, 0xfe, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0x07, 0xfe, 0xff, 0xff, 0x01,
// 0xe0, 0xff, 0xff, 0x01, 0xe0, 0x5f, 0xff, 0x01, 0xf0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x03,
// 0xe0, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xf0, 0xff, 0xff, 0x03, 0xf8, 0xff, 0xff, 0x03,
// 0xf0, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xff, 0x01, 0xf0, 0xff, 0xdf, 0x01, 0xe0, 0xef, 0xdf, 0x00,
// 0xe0, 0xfb, 0x7f, 0x00, 0x00, 0xff, 0x7f, 0x00, 0x00, 0xfd, 0xff, 0x00, 0x40, 0xff, 0xff, 0x03,
// 0x80, 0xff, 0xff, 0x07, 0x80, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x07,
// 0xc0, 0xff, 0xff, 0x07, 0xc0, 0xff, 0xff, 0x03, 0xc0, 0xff, 0xff, 0x01, 0xc0, 0xff, 0xff, 0x00,
// 0xc0, 0xff, 0x7f, 0x00, 0x80, 0xff, 0x7f, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00,
// 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0xe0, 0x3f, 0x00, 0x00, 0xe0, 0x07, 0x00,
// 0x00, 0x80, 0x07, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0xe0, 0x1f, 0x00,
// 0x00, 0xe0, 0x1f, 0x00, 0x3f, 0xe0, 0x1f, 0x00, 0x7f, 0xe0, 0x1d, 0x00, 0x7f, 0xc0, 0x0f, 0x00,
// 0xff, 0xc0, 0x07, 0x00, 0xff, 0xc7, 0x03, 0x00, 0xff, 0xcf, 0x03, 0x00, 0xff, 0x9f, 0x00, 0x00,
// 0xff, 0xef, 0x01, 0x00, 0x5f, 0xef, 0x00, 0x00, 0x5f, 0x3e, 0x00, 0x00, 0x4f, 0x07, 0x00, 0x00,
// 0x20, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };