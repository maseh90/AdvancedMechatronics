// functions to operate the ST7789 on the PIC32
// adapted from https://github.com/sumotoy/TFT_ST7789
// and https://github.com/adafruit/Adafruit-ST7789-Library

// pin connections:
// GND - GND
// VCC - 3.3V
// SCL - B14
// SDA - B13
// RES - B15
// DC - B12
// BLK - NC

#include <xc.h>
#include "ST7789.h"
#include "spi.h"
#include "font.h"
//Below 2 #include's suggested at https://www.programiz.com/c-programming/library-function/string.h/strlen
//  for being able to access the strlen() function. this function interfaces well with variable-length
//  strings because, as stated in the website above, it recognizes null characters.
#include "stdio.h"
#include "string.h"

void LCD_command(unsigned char com) {
    LATBbits.LATB12 = 0; // DC
    spi_io(com);
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB12 = 1; // DC
    spi_io(dat);
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB12 = 1; // DC
    spi_io(dat>>8);
    spi_io(dat);
}

void LCD_init() {
  unsigned int time = 0;
  LCD_command(ST7789_SWRESET); //software reset
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.15) {}
  
  LCD_command(ST7789_SLPOUT); //exit sleep
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.5) {}
  
  LCD_command(ST7789_COLMOD);
  LCD_data(0x55);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}
  
  LCD_command(ST7789_MADCTL);
  LCD_data(0x00);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}
  
  LCD_command(ST7789_CASET);
  LCD_data(0x00);
  LCD_data(ST7789_XSTART);
  LCD_data((240+ST7789_XSTART)>>8);
  LCD_data((240+ST7789_XSTART)&0xFF);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}

  LCD_command(ST7789_RASET);
  LCD_data(0x00);
  LCD_data(ST7789_YSTART);
  LCD_data((240+ST7789_YSTART)>>8);
  LCD_data((240+ST7789_YSTART)&0xFF);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.001) {}
  
  LCD_command(ST7789_INVON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}

  LCD_command(ST7789_NORON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.01) {}
  
  LCD_command(ST7789_DISPON);
  time = _CP0_GET_COUNT();
  while (_CP0_GET_COUNT() < time + 48000000/2*0.5) {}
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // should check boundary first
  LCD_setAddr(x,y,x+1,y+1);
  LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
  LCD_command(ST7789_CASET); // Column
  LCD_data16(x0+ST7789_XSTART);
  LCD_data16(x1+ST7789_XSTART);
  
  LCD_command(ST7789_RASET); // Page
  LCD_data16(y0+ST7789_YSTART);
  LCD_data16(y1+ST7789_YSTART);

  LCD_command(ST7789_RAMWR); // Into RAM
}

void LCD_clearScreen(unsigned short color) {
  int i;
  LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
	for (i = 0;i < _GRAMSIZE; i++){
		LCD_data16(color);
	}
}

// drawChar function
void LCD_drawChar(unsigned short x, unsigned short y, unsigned short color, unsigned char letter) {
    int col;
    int row;
    letter = letter - 0x20;
    for ((col = 0); (col <= 4);(col = col + 1)) {
        for ((row = 0); (row <= 6);(row = row + 1)){
            if ((((ASCII[letter][col]) >> row) & 0b00000001 ) == 0b00000001) {
                LCD_drawPixel(x+col,y+row,color);
            }
            else {
                LCD_drawPixel(x+col,y+row,RED);
            }
        }
    }
}
// drawString function
void LCD_drawString(unsigned short x, unsigned short y, unsigned short color, unsigned char *m) {
    int character;
    int length_of_m = strlen(m);
    for (character = 0;character <= (length_of_m - 1);character = character + 1) {
        LCD_drawChar((x+(character*5)),y,color,m[character]);
    }
}

// progress bar function
void LCD_progressBar(unsigned short x, unsigned short y, unsigned short color) {
    int position;
    int position2;
    for (position = 1;position <= 101;position = position + 1) {
        LCD_drawPixel(x + position,y,color);
        LCD_drawPixel(x + position,y+11,color);
    }
    for (position2 = 0;position2 <= 11;position2 = position2 + 1) {
        LCD_drawPixel(x,y+position2,color);
        LCD_drawPixel(x + 101,y+position2,color);
    }
}

//incrementing the progress bar function
void LCD_incrementBar(unsigned short x, unsigned short y, unsigned short color, unsigned int num) {
    unsigned int position3;
    unsigned int x_bar;
    for (position3 = 1;position3 <= 10;position3 = position3 + 1) {
        LCD_drawPixel(x + num,y+position3,color);
    }
    if (num > 98) {
        for (x_bar = 1;x_bar <= 100;x_bar = x_bar + 1){
            for (position3 = 1;position3 <= 10;position3 = position3 + 1) {
                LCD_drawPixel(x + x_bar,y+position3,RED);
            }
        }
    }
}

void Gyroscope_Printer_x(signed short val) {
    signed short percentage_of_bar;
    percentage_of_bar = (signed short) ((val*10)/1700);
    int x;
    int y;
    if (percentage_of_bar >= 0) { // down direction on LCD screen from center
        for (y = 120; y <= 120 + percentage_of_bar; y = y + 1) {
            for (x = 115;x<= 125; x = x + 1){
                LCD_drawPixel(x,y,RED);
            }
        }
        for (y = 120 + percentage_of_bar;y <= 240; y = y + 1) {
            for (x = 115; x <= 125; x = x + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        for (y = 120; y >= 0; y = y - 1) {
            for (x = 115; x <= 125; x = x + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        
    }
    if (percentage_of_bar < 0) { // up direction on LCD screen from center
        for (y = 120; y >= 120 + percentage_of_bar; y = y - 1) {
            for (x = 115;x<= 125; x = x + 1){
                LCD_drawPixel(x,y,RED);
            }
        }
        for (y = 120 + percentage_of_bar; y >= 0; y = y - 1) {
            for (x = 115;x<= 125; x = x + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        for (y = 120; y <= 240; y = y + 1) {
            for (x = 115;x<= 125; x = x + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
    }
}

void Gyroscope_Printer_y(signed short val) {
    signed short percentage_of_bar;
    percentage_of_bar = (signed short) ((val*10)/1700);
    int x;
    int y;
    if (percentage_of_bar >= 0) { // down direction on LCD screen from center
        for (x = 120; x <= 120 + percentage_of_bar; x = x + 1) {
            for (y = 115;y<= 125; y = y + 1){
                LCD_drawPixel(x,y,RED);
            }
        }
        for (x = 120 + percentage_of_bar;x <= 240; x = x + 1) {
            for (y = 115; y <= 125; y = y + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        for (x = 120; x >= 0; x = x - 1) {
            for (y = 115; y <= 125; y = y + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        
    }
    if (percentage_of_bar < 0) { // up direction on LCD screen from center
        for (x = 120; x >= 120 + percentage_of_bar; x = x - 1) {
            for (y = 115;y<= 125; y = y + 1){
                LCD_drawPixel(x,y,RED);
            }
        }
        for (x = 120 + percentage_of_bar; x >= 0; x = x - 1) {
            for (y = 115;y<= 125; y = y + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
        for (x = 120; x <= 240; x = x + 1) {
            for (y = 115;y<= 125; y = y + 1){
                LCD_drawPixel(x,y,BLUE);
            }
        }
    }
}