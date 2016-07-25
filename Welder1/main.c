/*
 * GccApplication1.c
 *
 * Created: 09.06.2016 18:28:01
 * Author : rafal_000
 */ 

#include "font.h"

#include <avr/io.h>

#define F_CPU 1000000
#include <util/delay.h>
#include<avr/pgmspace.h>



#define LCD_DATA_PORT PORTC
#define LCD_CONT_PORT PORTD
#define LCD_RS PD0                      // Data or Instruction ; RS=H -> DB<0:7>:Display RAM data ; RS=L -> DB<0:7>:Instruction data 
#define LCD_RW PD1                      // Read or Write. R/W=H -> Data appears at DB<0:7> and can be read by the 
                                        //                         CPU while E=H, CS1B=L, CS2B=L and CS3=H.
                                        //                R/W=L -> Display data DB<0:7> can be written at falling of E
                                        //                         when CS1B=L, CS2B=L and CS3=H.
#define LCD_EN PD2                      // Enable signal.
                                        // Write mode (R/W=L) -> data of DB<0:7> is latched at the falling edge of E
                                        // Read mode (R/W=H)  -> DB<0:7> appears the reading data while E is at high level.
#define LCD_CS1 PD3
#define LCD_CS2 PD4
#define LCD_RST PD5                     // Reset signal. When RSTB=L,
                                        // -ON/OFF register 0 set (display off)
                                        // -Display start line register 0 set (display line from 0)
                                        // After releasing reset, this condition can be changed only by instruction. 


#define LCD_CMD_SET_X 0xB8              //set X address to DB3-0 (command: 1   0   1   1   1   AC2 AC1 AC0)
#define LCD_CMD_SET_Y 0x40              //set y address to DB5-0 (command: 0   1   AC5 AC4 AC3 AC2 AC1 AC0)
#define LCD_CMD_SET_Z 0xC0              //set z address to DB5-0 (command: 1   1   AC5 AC4 AC3 AC2 AC1 AC0)
#define LCD_CMD_SET_DISPLAY 0x3F        //set display on or off  (command: 0   0   1   1   1   1   1   D)

char css[2] = { LCD_CS1, LCD_CS2 };     // array of display segments


void lcd_command_write(unsigned char cs, unsigned char command) {
    LCD_CONT_PORT=_BV(LCD_EN)|_BV(cs)|_BV(LCD_RST);
    LCD_DATA_PORT=command;
    _delay_ms(1);
    LCD_CONT_PORT=_BV(cs)|_BV(LCD_RST);
    _delay_ms(1);
}

void lcd_data_write(unsigned char cs, unsigned char data) {
    LCD_CONT_PORT=_BV(LCD_EN)|_BV(LCD_RS)|_BV(cs)|_BV(LCD_RST);
    LCD_DATA_PORT=data;
    _delay_ms(1);
    LCD_CONT_PORT=_BV(cs)|_BV(LCD_RS)|_BV(LCD_RST);
    _delay_ms(1);
}


void lcd_init() {
    for (unsigned char cs_num=0 ; cs_num < 2 ; cs_num++) { 
        lcd_command_write(css[cs_num], LCD_CMD_SET_DISPLAY + 0); // turn on the display CS2 (RS=0, RW=0, DB7-6=0, DB5-0=1)
        lcd_command_write(css[cs_num], LCD_CMD_SET_X + 0);       // set X page address to DB3-0=0 (command: DB7=0, DB5-3=1 ; 1 0 1 1 1 AC2 AC1 AC0 )
        lcd_command_write(css[cs_num], LCD_CMD_SET_Y + 0);       // set y page address to DB5-0=0 (command: DB7=0, DB6=1)
        lcd_command_write(css[cs_num], LCD_CMD_SET_Z + 0);       // display z address to DB5-0=0  (command: DB7-6=0)
    }
}


// x is set i 8 bit steps (0-7). ie. 0 = 0 ; 1 = 8 ; 2 = 16 ; etc
// y is set normally one by one 0 - 63
void lcd_set_cursor_position(unsigned char cs, unsigned char x, unsigned char y) {
    lcd_command_write(cs, LCD_CMD_SET_X + x);
    lcd_command_write(cs, LCD_CMD_SET_Y + y);
}


void lcd_clear_screen(void) {
    for (unsigned char cs_num=0 ; cs_num < 2 ; cs_num++) {       // for both cs's
        for (unsigned char i=0; i<8 ; i++) {                     // x cursor is set in 8 bit steps
            lcd_command_write(css[cs_num], LCD_CMD_SET_X+i);
            for(unsigned char j=0; j<64 ; j++) {                 // for all lines we ...
                lcd_data_write(css[cs_num], 0);                  // ... write 8 bits. Write moves cursor one line below
            }
        }
        lcd_set_cursor_position(css[cs_num], 0, 0);              // set cursor position at 0,0
    }
}

void lcd_display_string (const unsigned char str[], unsigned char length, unsigned char x, unsigned char y) {
    for (unsigned char i = 0 ; i < length ; i++) {
        lcd_set_cursor_position(LCD_CS1, x+i, y);
        for (unsigned char j = 0 ; j < 8 ; j++) {
            lcd_data_write(LCD_CS1, font8x8[str[i]][j]);
        }
    }
}

int main(void)
{
    DDRD=0xff;
    DDRC=0x1f;
    unsigned char str_witaj[] = { "Witaj!" }; 

    lcd_init();
    lcd_clear_screen();
    lcd_display_string(str_witaj, 6, 0, 0);


    /* Replace with your application code */
    while (1)  {
    }
}
