#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "ST7789.h"
#include "spi.h"
#include "font.h"

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

void main(void) {
    __builtin_disable_interrupts(); // disable interrupts while initializing things
    initSPI(); // intialize SPI communication
    LCD_init(); // intialize LCD screen attributes
    __builtin_enable_interrupts(); // disable interrupts while initializing things
    unsigned char m[300]; // to store the hello world + number
    unsigned char m2[300]; // to store the FPS reading
    unsigned int i = 0; // number to be incremented
    i = 0;
    LCD_clearScreen(RED); // sets whole screen to red
    LCD_progressBar(28,45,WHITE); // makes a white progress bar
    float FPS; // FPS var
    int time1; // time1 for calculating FPS
    int time2; // time2 for calculating FPS
    while (1) {
        if (_CP0_GET_COUNT() > 2400000) { // updates screen at 10 Hz (10 seconds to fill bar as a result))
            _CP0_SET_COUNT(0); // restarts count every loop
            time1 = _CP0_GET_COUNT(); // time 1 for reference for FPS, before filling screen
            LCD_incrementBar(28,45,GREEN,i); // increment the bar function with i
            sprintf(m,"Hello World! %d",i); // store new i number here
            LCD_drawString(28,32,WHITE,m);
            if (i >= 99) {
                i = 0;
            }
            i = i + 1; // increment i
            time2 = _CP0_GET_COUNT(); // time 2 for reference for FPS, ba filling screen
            FPS = 1/((time2-time1)*0.00000004166); // get the FPS
            sprintf(m2,"%f FPS",FPS); // store the FPS in a string to be printed
            LCD_drawString(0,9,WHITE,m2); // print the FPS
        }
    }
}