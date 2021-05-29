#ifndef WS2812B_H__
#define WS2812B_H__

#include<xc.h> // processor SFR definitions

// struct to link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

void ws2812b_setup();
void ws2812b_setColor(wsColor*,int);
wsColor HSBtoRGB(float hue, float sat, float brightness);
// color wheel at https://www.google.com/search?q=rgb+color+wheel&rlz=1C5CHFA_enUS874US875&tbm=isch&source=iu&ictx=1&fir=q1Nup3MVqVoeuM%252C7wx7aE8hLBn2XM%252C_&vet=1&usg=AI4_-kQAZsgU3NKeimyBgOi6jskXs_FEyQ&sa=X&ved=2ahUKEwj9w4W8xbvwAhWZHM0KHb-ZA3kQ9QF6BAgMEAE&biw=1231&bih=880#imgrc=gn3fGHWw0Y_4WM

#endif