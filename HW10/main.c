#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ws2812b.h"
// DEVCFG0
#pragma config DEBUG = OFF // disable debugging
#pragma config JTAGEN = OFF // disable jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // disable flash write protect
#pragma config BWP = OFF // disable boot write protect
#pragma config CP = OFF // disable code protect

// DEVCFG1
//WHEN USING THE PRIMARY INTERNAL RESONATOR (BELOW 2 LINES)
#pragma config FNOSC = FRCPLL // use internal oscillator with pll 
#pragma config FSOSCEN = OFF
//IF YOU WERE USING THE EXTERNAL RESONATOR (BELOW 2 LINES and see additional line as well)
//#pragma config FNOSC = PRIPLL // use primary oscillator with pll
//#pragma config FSOSCEN = OFF // disable secondary oscillator
#pragma config IESO = OFF // disable switching clocks
//#pragma config POSCMOD = HS // high speed crystal mode - if using external resonator
#pragma config POSCMOD = OFF // internal RC
#pragma config OSCIOFNC = OFF // disable clock output
#pragma config FPBDIV = DIV_1 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // disable clock switch and FSCM
#pragma config WDTPS = PS1048576 // use largest wdt
#pragma config WINDIS = OFF // use non-window mode wdt
#pragma config FWDTEN = OFF // wdt disabled
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_24 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations


//WORKS CITED: I USED INFORMATION, DISCUSSIONS, and QUESTIONS PRESENTED IN OFFICE HOURS
//  to develop this code and understand what was going on behind the scenes.
void main(void) {
    __builtin_disable_interrupts(); // disable interrupts while initializing things
    ws2812b_setup();
    __builtin_enable_interrupts(); // disable interrupts while initializing things
    //from office hours, it was suggested/shown to create an array of the LED data
    //  where the array can be indexed -- > I implemented this in my code
    wsColor c_sent[4]; // array for each LED
    int ii; // counter ii - controls the main color loop
    int iii; // controls the smooth incrementation down the color wheel after ascending it
    while (1) { // continues forever
        for (ii = 0;ii < 360;ii = ii + 1){ // count to control where on color wheel we are
            if (ii <= 180) { // ascending direction up (in degrees) the color wheel
                // all LEDs are at 60 degree offsets of one another, all increment at same rate
                c_sent[1] = HSBtoRGB(ii,1,1);
                c_sent[2] = HSBtoRGB(ii+60,1,1);
                c_sent[3] = HSBtoRGB(ii+120,1,1);
                c_sent[4] = HSBtoRGB(ii+180,1,1);
                // wait this time before actually implementing
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT() < 400000) {;}
                ws2812b_setColor(c_sent,4); // send LED colors to be implemented
            }
            if (ii > 180) { // descending direction up (in degrees) the color wheel
                iii = ii - 180; // allows for smooth progression down from what colors LEDs were showing
                c_sent[1] = HSBtoRGB(180-iii,1,1);
                c_sent[2] = HSBtoRGB(240-iii,1,1);
                c_sent[3] = HSBtoRGB(300-iii,1,1);
                c_sent[4] = HSBtoRGB(360-iii,1,1);
                _CP0_SET_COUNT(0);
                while (_CP0_GET_COUNT() < 400000) {;}
                ws2812b_setColor(c_sent,4);
            }
        }
    }
}