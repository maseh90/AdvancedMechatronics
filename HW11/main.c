#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include<stdio.h>
#include "i2c_master_noint.h"
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


//WORKS CITED: I USED INFORMATION, DISCUSSIONS, and QUESTIONS PRESENTED IN OFFICE HOURS
//  to develop this code and understand what was going on behind the scenes.
void main(void) {
    __builtin_disable_interrupts(); // disable interrupts while initializing things
        
    initSPI(); // intialize SPI communication
    
    LCD_init(); // intialize LCD screen attributes
    
    i2c_master_setup(); // sets up i2c

    //next lines setup the IMU chip by writing to mentioned registers
    // from HW 4 code to blink the LED, we intitialize it to act as heartbeat:
    TRISAbits.TRISA4 = 0; //A4 is an output pin
    LATAbits.LATA4 = 0; //A4 is initially LO (off)
    
    __builtin_enable_interrupts(); // disable interrupts while initializing things
    
    // read from WHO_AM_I (0xF from YouTube demo/data sheet) to see if I2c is functional or not, and if IMU chip functional
    LCD_clearScreen(RED);
    LATAbits.LATA4 = 1;
    unsigned char ACTIVE;
    i2c_master_start();
    i2c_master_send(0b11010100); // write
    i2c_master_send(0x0F); // who am i register
    i2c_master_restart();
    i2c_master_send(0b11010101); // read
    ACTIVE = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    if (ACTIVE != 0b01101001) {
        while(1) {
            LATAbits.LATA4 = 0;
        }
    }
    LATAbits.LATA4 = 0;
    LCD_clearScreen(BLUE);
    // WRITE TO THE THREE MENTIONED REGISTERS TO SETUP THE CHIP
    //register addresses gotten from data sheet / YouTube tutorial on IMU
    //CTRL1_XL - turn on = 1.66kHz sample, 2g sens, 100 Hz filter
    i2c_master_start(); // start sending
    i2c_master_send(0b11010100); // WRITE address
    i2c_master_send(0x10); // REGISTER OF CTRL1_XL
    i2c_master_send(0b10000010); // writes data to set characteristics above
    i2c_master_stop(); // stop sending
    //CTRL2_G - turn on = 1.66 kHz, 1000 dps sens
    i2c_master_start(); // start sending
    i2c_master_send(0b11010100); // WRITE address
    i2c_master_send(0x11); // REGISTER OF CTRL2_G
    i2c_master_send(0b10001000); // writes data to set characteristics above
    i2c_master_stop(); // stop sending
    //CTRL3_C - turn on = IF_INC bit a 1, allows to read multiple registers at once
    i2c_master_start(); // start sending
    i2c_master_send(0b11010100); // WRITE address
    i2c_master_send(0x12); // REGISTER OF CTRL3_C
    i2c_master_send(0b00000100); // writes data to set characteristics above
    i2c_master_stop(); // stop sending
   
    char m1[200]; // data display
    char m2[200]; // data display
    char m3[200]; // data display
    char m4[200]; // data display
    char m5[200]; // data display
    char m6[200]; // data display
    char m7[200]; // data display
    
    _CP0_SET_COUNT(0); // initializes the clock at 0
    int STATE = 1; // state of the heartbeat LED
    unsigned char data_imu[14]; // array to hold the data from the imu = 14 bytes of data
    signed short data_con_imu[7]; // array to hold consolidated data from imu = 7 bytes of data
    while (1) {
       if (_CP0_GET_COUNT() > 2400000) { // heartbeat rate ~ 20 times every second
        _CP0_SET_COUNT(0); // restarts the count to allow for another heartbeat
        LATAbits.LATA4 = STATE; //heartbeat (A4) LED turns on or off depending on current state
        STATE = !STATE; // reverses whether LED is on or off depending on current value
        I2C_read_multiple(0x20, data_imu, 14);
        data_combine(data_imu,data_con_imu,14);
        Gyroscope_Printer_x(data_con_imu[4]);
        Gyroscope_Printer_y(data_con_imu[5]);
        }
    }
}