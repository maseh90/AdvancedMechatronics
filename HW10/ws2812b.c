// WS2812B library

#include "ws2812b.h"
// other includes if necessary for debugging

// Timer2 delay times, you can tune these if necessary
#define LOWTIME 15 // number of 48MHz cycles to be low for 0.35uS
#define HIGHTIME 65 // number of 48MHz cycles to be high for 1.65uS

// setup Timer2 for 48MHz, and setup the output pin
void ws2812b_setup() {
    T2CONbits.TCKPS = 0; // Timer2 prescaler N=1 (1:1), N = 0 => 0b000 = 0
    PR2 = 65535; // maximum period
    TMR2 = 0; // initialize Timer2 to 0
    T2CONbits.ON = 1; // turn on Timer2

    // initialize output pin (B6) as off
    // TRIS...
    TRISBbits.TRISB6 = 0; // explicitly an output
    // LAT...
    LATBbits.LATB6 = 0; // off to start
}

// build an array of high/low times from the color input array, then output the high/low bits
// takes in a color and the number of elements in the array total
// info on C arrays found at https://www.tutorialspoint.com/cprogramming/c_arrays.htm
void ws2812b_setColor(wsColor * c, int numLEDs) {
    int i = 0; int j = 0; int j1; int j2; // for loops
    int numBits = 2 * 3 * 8 * numLEDs; // the number of high/low bits to store, 2 per color bit
    volatile unsigned int delay_times[2*3*8 * 4]; // I only gave you 5 WS2812B, adjust this if you get more somewhere

    // start at time at 0
    delay_times[0] = 0;
    
    int nB = 1; // which high/low bit you are storing, 2 per color bit, 24 color bits per WS2812B
	
    // loop through each WS2812B - loop through all LEDS
    for (i = 0; i < numLEDs; i++) {
        // loop through each color bit, MSB first - loop through every bit in the 3 colors (3 colors per LED)
        for (j = 7; j >= 0; j--) {
            // if the bit is a 1
            // from office hours, I was finally able to modify slightly and identify which bit was of prime concern
            // tweaked code after that, and it allowed for colors to be sent to LEDs
            // this identifies is the current bit being looked at is high
            if ( ((c[i].r) >> j) & 0b00000001) {
                //if (/* identify the bit in c[].r, is it 1 */) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
            else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
        // do it again for green
        for (j1 = 7; j1 >= 0; j1--) {
         if ( ((c[i].g) >> j1) & 0b00000001) {
                //if (/* identify the bit in c[].r, is it 1 */) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
         else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
		// do it again for blue
        for (j2 = 7; j2 >= 0; j2--) {
         if ( ((c[i].b) >> j2) & 0b00000001) {
                //if (/* identify the bit in c[].r, is it 1 */) {
                // the high is longer
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
            } 
            // if the bit is a 0
         else {
                // the low is longer
                delay_times[nB] = delay_times[nB - 1] + LOWTIME;
                nB++;
                delay_times[nB] = delay_times[nB - 1] + HIGHTIME;
                nB++;
            }
        }
    }
    // turn on the pin for the first high/low
    LATBbits.LATB6 = 1;
    TMR2 = 0; // start the timer
    for (i = 1; i < numBits; i++) {
        while (TMR2 < delay_times[i]) {
        }
        LATBINV = 0b1000000; // invert B6
    }
    LATBbits.LATB6 = 0;
    TMR2 = 0;
    while(TMR2 < 2400){} // wait 50uS, reset condition
}

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}