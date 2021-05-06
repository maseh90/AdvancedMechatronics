#include "i2c_master_noint.h"
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

void main(void) {
    __builtin_disable_interrupts(); // disable interrupts while initializing things
    i2c_master_setup();
    // from HW 4 code to blink the LED, we intitialize it to act as heartbeat:
    TRISAbits.TRISA4 = 0; //A4 is an output pin
    LATAbits.LATA4 = 0; //A4 is initially LO (off)
    __builtin_enable_interrupts(); // disable interrupts while initializing things
    // SET A0 (EXPANDER) as an output pin so we can read its state:
    i2c_master_start(); // start sending
    i2c_master_send(0b01000000); // WRITE address
    i2c_master_send(0x00); // REGISTER OF IODRA
    i2c_master_send(0x00); // writes to IODRA to set to output pin
    i2c_master_stop(); // stop sending
    // SET B0 (EXPANDER) as an input pin so we can read its state:
    i2c_master_start(); // start sending
    i2c_master_send(0b01000000); // WRITE address
    i2c_master_send(0x01); // REGISTER OF IODRB
    i2c_master_send(0xFF); // writes to IODRB to set to input pin
    i2c_master_stop(); // stop sending
    
    _CP0_SET_COUNT(0); // initializes the clock
    int STATE = 1; // variable to change to blink LED every time the button is evaluated
    int BUTTON; // variable to hold state of whether B7 is being pressed
    while (1) { // infinite loop
        if (_CP0_GET_COUNT() > 2400000) { // heartbeat rate ~ 20 times every second
            _CP0_SET_COUNT(0); // restarts the count to allow for another heartbeat
            LATAbits.LATA4 = STATE; //heartbeat (A4) LED turns on or off depending on current state
            STATE = !STATE; // reverses whether LED is on or off depending on current value
            
            // read from the B0 pin to see if pressed
            i2c_master_start();
            i2c_master_send(0b01000000);
            i2c_master_send(0x13);
            i2c_master_restart();
            i2c_master_send(0b01000001);
            BUTTON = i2c_master_recv();
            BUTTON = BUTTON  && 0b10000000; // using info at https://www.tutorialspoint.com/cprogramming/c_bitwise_operators.htm
            // above line sees if B0 pin is low or high (pressed or not pressed)
            // reframes BUTTON variable as 0 = HI (not pressed), and 1 = LO (pressed) for more logical evaluation
            i2c_master_ack(1);
            i2c_master_stop();

            if (BUTTON == 1) { // if button is logical 1, then it is pressed
                // as a result, write to A7 to turn the button on (HI output)
                i2c_master_start();
                i2c_master_send(0b01000000);
                i2c_master_send(0x14);
                i2c_master_send(0x00);
                i2c_master_stop();
            }
            if (BUTTON == 0) { // if button is logical 0, then it is NOT pressed
                // as a result, write to A7 to turn the button OFF (LO output)
                i2c_master_start();
                i2c_master_send(0b01000000);
                i2c_master_send(0x14);
                i2c_master_send(0xFF);
                i2c_master_stop();
            }
        }
    }
}