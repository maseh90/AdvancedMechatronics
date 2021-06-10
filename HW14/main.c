#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>

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

void GetOC(float);

// I referenced the embedded computing code/past assignments from last quarter's ME333 to help
//  troubleshoot and develop this code as well as  past assignments in this course.
//  I also referenced the RCservo reference code for this year's course, and I used
//  online coding reference resources along the way to debug this code.
void main(void) {
    // all of these Timer 2 statements (and some OC statements) are adapted directly from past reference code
    // the comments were all part of past reference code, and they help with the clarity and order of code
   RPB13Rbits.RPB13R = 0b0110; // sets pin B13 (Pin 24) as OC5 pin
  PR2 = 49999;                    //             set period register
  TMR2 = 0;                       //             initialize count to 0
  T2CONbits.TCKPS = 0b101;            //             set prescaler to 32
  T2CONbits.TGATE = 0;            //             not gated input (the default)
  T2CONbits.TCS = 0;              //             PCBLK input (the default)
  T2CONbits.ON = 1;               //             turn on Timer2
    OC5CONbits.OC32 = 0; //16 bit timer for reference
    OC5CONbits.OCTSEL = 0; // Timer 2 is the source for the interrupt
    OC5CONbits.OCM = 0b110; // PWM mode without fault pin, as shown in many ME333 reference sample programs
    OC5CONbits.ON = 1; // turn on OC5
    OC5R = 3400;
    OC5RS = 3400; // 400 can be considered 0 degrees, 3500 can be considered 180 degrees
    // I performed the calculations, but experimentation with OC5RS values allowed me to determine this
    _CP0_SET_COUNT(0);
    float degrees = 14;
    float increment = 3.2;
    int upward = 1;
    int downward = 0;
    while (1) {
        if (_CP0_GET_COUNT() > 960000) { // https://www.rapidtables.com/convert/frequency/mhz-to-hz.html
            // 960000 = 0.02 seconds (updates OC5RS every 0.02 seconds = 50 Hz)
            _CP0_SET_COUNT(0);
            // has to update 160 degrees in the span of 1 second (10 deg to 170 deg = 1/2 period in 1 seconds)
            // so, OC5RS update must reflect change of 3.2 degrees every 0.02 seconds
            if (degrees < 10.0) {
                degrees = 10;
                downward = 0;
                upward = 1;
            }
            if (degrees > 170.0) {
                degrees = 170;
                upward = 0;
                downward = 1;
            }
            if (upward == 1) {
                degrees = degrees + increment;
            }
            if (downward == 1) {
                degrees = degrees - increment;
            }
            GetOC(degrees);
        }
    }
}

void GetOC(float degrees_input) {
    float OC;
    OC = (degrees_input)*17.22 + 400; // converts degrees to OC5RS value
    // these 0 deg and 180 deg values were retrieved by observation / experimentation with the servo
    // considering 400 = 0 degrees, 3500 = 180 degrees
    //3500 - 400 = 3100. 3100 / 180 deg = 17.22 "OC" per degree
    OC5RS = (int) OC;
}