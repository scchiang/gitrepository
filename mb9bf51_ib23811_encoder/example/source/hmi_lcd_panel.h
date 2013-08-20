
/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)           */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user's obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU's liability  */
/* is restricted to intentional misconduct and gross negligence.        */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.5)                                                               */
/************************************************************************/


/***************************************************************************
*  20character x 4 Lines:
*  these addresses include the D7 data bit indicating a 'SET DDRAM' command !

*            1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20
* 1st Line: 80 81 82 83 84 85 86 87 88 89 8A 8B 8C 8D 8E 8F 90 91 92 93
* 2nd Line: C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF D0 D1 D2 D3
* 3rd Line: 94 95 96 97 98 99 9A 9B 9C 9D 9E 9F A0 A1 A2 A3 A4 A5 A6 A7
* 4th Line: D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF E0 E1 E2 E3 E4 E5 E6 E7
*
***************************************************************************/

#define LCD_LINELENGTH 16
#define LCD_LINES 2

typedef union{
  unsigned short int u16data;    // entire byte to be sent to the LCD via I2C link
  struct{                        // bits in 16FX bit order
    unsigned char lcd_rs   :1;   // LCD register select
    unsigned char lcd_rw   :1;   // LCD RW
    unsigned char lcd_en   :1;   // LCD Enable
    unsigned char lcd_BL   :1;   // LCD backlight (active high)
    unsigned char lcd_data :4;   // LCD data in 4-bit mode
    unsigned char sw1      :1;   // should be kept '1' to enable pullup of 'quasi-bidirectional IO' of PCF8575
    unsigned char sw2      :1;   // should be kept '1' to enable pullup of 'quasi-bidirectional IO' of PCF8575
    unsigned char sw3      :1;   // should be kept '1' to enable pullup of 'quasi-bidirectional IO' of PCF8575
    unsigned char sw4      :1;   // should be kept '1' to enable pullup of 'quasi-bidirectional IO' of PCF8575
    unsigned char sw5      :1;   // should be kept '1' to enable pullup of 'quasi-bidirectional IO' of PCF8575
    unsigned char led1     :1;   // Active low
    unsigned char led2     :1;   // Active low
    unsigned char led3     :1;   // Active low
  }bits;
}HMI_MODULE_DATA_t;

#define PCF8575_ADRESS 0x20  // A0 = 1, A1 = 1, A2 = 1 (PCF8575)

#define HMI_LED_ON 0   // LEDs are active low
#define HMI_LED_OFF 1

#define HMI_LED1_GREEN  (1<<2)
#define HMI_LED2_YELLOW (1<<1)
#define HMI_LED3_RED    (1<<0)

#define HMI_BUTTON_BM_UP (1<<0)
#define HMI_BUTTON_BM_OK (1<<1)
#define HMI_BUTTON_BM_DOWN (1<<2)
#define HMI_BUTTON_BM_LEFT (1<<3)
#define HMI_BUTTON_BM_RIGHT (1<<4)

#define HMI_BUTTON_DEBOUNCE_THRESHOLD 3  // threshold for the button debounce counter. A button is reported as pressed after this number of polls.

#define LCD_CLR		 	    0x01
#define LCD_4Bit        0x28
#define LCD_HOME	      0x03
#define LCD_CUROFF	 	  0x0C
#define LCD_CURON	 	    0x0F
#define LCD_NOSHIFT	 	  0x0A
#define LCD_DISPON	 	  0x0C		
#define LCD_DISPOFF  	  0x08
#define LCD_DISPshiftl 	0x18
#define LCD_DISPshiftr	0x1C
#define LCD_1st_line    0x80
#define LCD_2nd_line 	  0xC0
#if (LCD_LINELENGTH == 16)
  #define LCD_3rd_line 	  0x90
  #define LCD_4th_line 	  0xD0
#endif
#if (LCD_LINELENGTH == 20)
  #define LCD_3rd_line 	  0x94
  #define LCD_4th_line 	  0xD4
#endif

extern HMI_MODULE_DATA_t hmi_pinstate;
extern char HMI_lcd_buf[LCD_LINELENGTH];

void PCF8575_set_port(unsigned char, unsigned short int);
unsigned short int PCF8575_read_port(unsigned char);

void HMI_LCD_write(unsigned char rs, unsigned char value);
void HMI_LCD_init(void);
void HMI_LCD_string(const char *str);
void HMI_LCD_printline(char line, char length, const char *buf);
void HMI_LCD_position(char position);
void HMI_LCD_clear();
void HMI_LCD_number(unsigned long value, unsigned char length, char fill);
void HMI_LCDshiftLeft(int count,int delay);
void HMI_LCDshiftRight(int count,int delay);
void HMI_LCD_init_CGRAM(void);

void ScrollLCDString16ch(char *, const char *, unsigned char);
void ScrollLCDString(char *, const char *, unsigned char, unsigned char, unsigned char);

unsigned char HMI_get_button_status(void); // returns a bit field containing the button status. A simple counter is used for debouncing (has to be called periodically!)
void HMI_set_led_status(unsigned char);
void HMI_set_backlight_on(void);
void HMI_set_backlight_off(void);
void HMI_init(void);
void LcdFx_WriteDec(uint32_t x);
