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

void readUART1(char * string, int maxLength);
void writeUART1(const char * string);

int main() {

    __builtin_disable_interrupts(); // disable interrupts while initializing things

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    // do your TRIS and LAT commands here
    TRISAbits.TRISA4 = 0; //A4 is an output pin
    LATAbits.LATA4 = 0; //A4 is initially LO (off)
    TRISBbits.TRISB4 = 1; //B4 is an input pin
    //SETTING UP UART on B6/B7 PINS
    U1RXRbits.U1RXR = 0b0001; // sets the B6 pin - U1RX
    RPB7Rbits.RPB7R = 0b0001; // sets the B7 pin - U1TX

    // turn on UART3 without an interrupt
    U1MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U1BRG = ((48000000 / 115200) / 16) - 1;

    // 8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;

    // configure TX & RX pins as output & input pins
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1; 
    
    //flow control pins off by default

    // enable the uart
    U1MODEbits.ON = 1;
    __builtin_enable_interrupts();
    
    char message[100]; // initializes the character array
    while (1) { // if the button is pushed, blink the LED twice
        // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
        // remember the core timer runs at half the sysclk
        if (PORTBbits.RB4 == 0) {
            sprintf(message,"Button has been pressed.\r\n"); // stores this string in the message character array
            writeUART1(message); // write this message everytime the button is pressed
            LATAbits.LATA4 = 1; //A4 turns on
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() <= 12000000) {;} //core timer at 1/2 of sys clock, = 24 MHz; 12000000 ticks = 0.5 second delay
            // LED has been on for 0.5 seconds, now turn off for 0.5 seconds
            LATAbits.LATA4 = 0; //A4 turns off
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() <= 12000000) {;} // 0.5 second delay function
            // LED has been off for 0.5 seconds, now turn on for 0.5 seconds
            LATAbits.LATA4 = 1; //A4 turns on again
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() <= 12000000) {;} // 0.5 second delay function
            LATAbits.LATA4 = 0; //A4 turns off again
            _CP0_SET_COUNT(0);
            while (_CP0_GET_COUNT() <= 12000000) {;} // 0.5 second delay function
            
        }
    }
}

// Read from UART3
// block other functions until you get a '\r' or '\n'
// send the pointer to your char array and the number of elements in the array
void readUART1(char * message, int maxLength) {
  char data = 0;
  int complete = 0, num_bytes = 0;
  // loop until you get a '\r' or '\n'
  while (!complete) {
    if (U1STAbits.URXDA) { // if data is available
      data = U1RXREG;      // read the data
      if ((data == '\n') || (data == '\r')) {
        complete = 1;
      } else {
        message[num_bytes] = data;
        ++num_bytes;
        // roll over if the array is too small
        if (num_bytes >= maxLength) {
          num_bytes = 0;
        }
      }
    }
  }
  // end the string
  message[num_bytes] = '\0';
}

// Write a character array using UART3
void writeUART1(const char * string) {
  while (*string != '\0') {
    while (U1STAbits.UTXBF) {
      ; // wait until tx buffer isn't full
    }
    U1TXREG = *string;
    ++string;
  }
}