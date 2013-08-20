
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


#include "mcu.h"
#include "mfs1_i2c.h"
#include "hmi_lcd_panel.h"

// ToDo: Read back LCD Busy bit instead of long wait loops (and implement RW mode of LCD)


// Custom character definition

const char cchar0[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 
const char cchar1[8] = {0x0f,0x1f,0x18,0x1f,0x1f,0x18,0x18,0x18};  // 'F'
const char cchar2[8] = {0x11,0x1b,0x1f,0x1f,0x1b,0x1b,0x1b,0x1b};  // 'M'
const char cchar3[8] = {0x1e,0x1f,0x03,0x1e,0x1e,0x03,0x1f,0x1e};  // '3'
const char cchar4[8] = {0x00,0x00,0x00,0x06,0x09,0x10,0x09,0x06};  // infinity (1)
const char cchar5[8] = {0x00,0x0c,0x12,0x01,0x01,0x12,0x0c,0x00};  // infinity (2)
const char cchar6[8] = {0x00,0x00,0x1b,0x1f,0x0e,0x04,0x00,0x00};  // heart
const char cchar7[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

#if (LCD_LINES == 4)
const char lcd_line_addresses[4] = {LCD_1st_line,LCD_2nd_line,LCD_3rd_line,LCD_4th_line};
#elif (LCD_LINES == 2)
const char lcd_line_addresses[2] = {LCD_1st_line,LCD_2nd_line};
#endif

HMI_MODULE_DATA_t hmi_pinstate;   // data structure holding the pin states of the I2C port expander pins
char HMI_lcd_buf[LCD_LINELENGTH];

#define LCD_PAUSE_XXL  10000L
#define LCD_PAUSE_LONG  3000L
#define LCD_PAUSE_SHORT 1000L

void PCF8575_set_port(unsigned char i2c_address, unsigned short int value)
{
  unsigned char port0, port1;
  
  port0 = (unsigned char) (value & 0x00ff);
  port1 = (unsigned char) (value >> 8);
  I2cStart((i2c_address<<1) | I2C_RW_WRITE);	// Write slave device address
  I2cSendByte(port0);             	// Write port 0 value
  I2cSendByte(port1);             	// Write port 1 value
  I2cWriteStop();
}

unsigned short int PCF8575_read_port(unsigned char i2c_address)
{

  unsigned char rdport0, rdport1;
  unsigned short int result;

  FM3_MFS1_I2C->TDR  = ((i2c_address<<1) | I2C_RW_READ);
  FM3_MFS1_I2C->IBCR = 0x80;         // MSS = 1: Master Mode

  while(!(FM3_MFS1_I2C->IBCR & 0x01)); // Wait for transmission complete

  FM3_MFS1_I2C->TDR = 0x00;    // Send dummy data
  FM3_MFS1_I2C->IBCR = 0xB0;   // WSEL = 1, ACKE = 1, Clear INT flag

  while(!(FM3_MFS1_I2C->IBCR & 0x01)); // Wait for transmission complete
  rdport0 = FM3_MFS1_I2C->RDR;

  FM3_MFS1_I2C->TDR = 0x00;    // Send dummy data
// FM3_MFS1_I2C->IBCR = 0x80;   // WSEL = 0, ACKE = 0, Clear INT flag
// FM3_MFS1_I2C->IBCR = 0x90;   // WSEL = 1, ACKE = 0, Clear INT flag
  FM3_MFS1_I2C->IBCR = 0xB0;   // WSEL = 1, ACKE = 1, Clear INT flag

  while(!(FM3_MFS1_I2C->IBCR & 0x01)); // Wait for transmission complete
  rdport1 = FM3_MFS1_I2C->RDR;

  // I2cReadStop();
  FM3_MFS1_I2C->IBCR = 0;
  
  result = ((unsigned short int) rdport1 << 8) + rdport0;
  
  return result;
}


void pause(unsigned long value) 
{
  volatile unsigned long i;
  for (i = 0; i < value; i++);
}

void HMI_LCD_write(unsigned char rs, unsigned char value) {

//  hmi_pinstate.u16data = PCF8575_read_port(PCF8575_ADRESS);     // hmi_pinstate should be kept valid / up-to-date anyway
//  hmi_pinstate.u16data |= 0x1f00;   // keep these pins high (button inputs)

	hmi_pinstate.bits.lcd_rs = rs;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);   // set RS

	// send higher nibble
	hmi_pinstate.bits.lcd_en = 1;
	hmi_pinstate.bits.lcd_data = (value & 0xf0) >> 4;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);    // write data + EN

	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);    // clear EN

  
	// send lower nibble
	hmi_pinstate.bits.lcd_en = 1;
	hmi_pinstate.bits.lcd_data = value & 0x0f;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);    // write data + EN

	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);    // clear EN

  hmi_pinstate.u16data &= 0xff08;                           // clear LCD pins (except BL)
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);

}

void HMI_LCD_init(void) {

	// startup sequence
	pause(LCD_PAUSE_XXL);
	hmi_pinstate.bits.lcd_data = 0x03;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_LONG);

	hmi_pinstate.bits.lcd_en = 1;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_SHORT);

	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_LONG);

	hmi_pinstate.bits.lcd_en = 1;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_SHORT);

	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_LONG);

	hmi_pinstate.bits.lcd_en = 1;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_SHORT);

	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_LONG);


	// -----------------------------------------------------
	// LCD initialization set 4 Bit interface

	hmi_pinstate.bits.lcd_data = 0x02;
	hmi_pinstate.bits.lcd_en = 1;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);

	pause(LCD_PAUSE_SHORT);
	hmi_pinstate.bits.lcd_en = 0;
  PCF8575_set_port(PCF8575_ADRESS,hmi_pinstate.u16data);
	pause(LCD_PAUSE_SHORT);
  
	// -----------------------------------------------------
	// Bit 5 set: Function set
	// Bit	0			1
	// 2	5x7 dots	5x10 dots
	// 3	one line	two lines
	// 4    4 Bit       8 Bit Interface
	HMI_LCD_write(0,0x28);  // 0x28: 4 Bit-I/F, 4 Lines, 5x7 Dots
	pause(LCD_PAUSE_SHORT);
	// -----------------------------------------------------
	// Bit 3 set: Display on/off control
	// Bit	0			1
	// 0	no blink		cursor blinks
	// 1	cursor off		cursor on
	// 2	display off		display on
	HMI_LCD_write(0,0x0c); // 0x0c: cursor off, display on
	pause(LCD_PAUSE_SHORT);
	// -----------------------------------------------------
	// Bit 2 set: Entry Mode set
	// Bit	0			1
	// 0	Display shift off	Display shift on
	// 1	Cursor move right	Cursor move left
	HMI_LCD_write(0,0x06);  //  no Display shift, Cursor move 
	pause(LCD_PAUSE_SHORT);
	// -----------------------------------------------------
	// Bit 1 set: Cursor to home position
	HMI_LCD_write(0,0x02);
	pause(LCD_PAUSE_SHORT);
	// -----------------------------------------------------
	// Bit 0 set: clear Display & Cursor to home position
	HMI_LCD_write(0,0x01);
	pause(LCD_PAUSE_XXL);
}

void HMI_LCD_string(const char *str) 
{
	while(*str)
	{
		HMI_LCD_write(1,*str++);
	}

}

void HMI_LCD_printline(char line, char length, const char *buf) {  // gives out one line, also if it contains a zero (string end)
  char i;

  if (line > LCD_LINES) return;
  HMI_LCD_position(lcd_line_addresses[line-1]);
  for (i=0; i<length; i++) {
    if (i >= LCD_LINELENGTH) break;
    HMI_LCD_write(1,buf[i]);
  }
}

void HMI_LCD_position(char position)
{
  HMI_LCD_write(0,position | 0x80);    // 0x80: 'SET DDRAM' Address command
}

void HMI_LCD_clear()
{
  HMI_LCD_write(0,0x01);
	pause(LCD_PAUSE_XXL);
}

void HMI_LCD_set_character(char charnr, const char *row_data)    // write a pre-defined custom character into CGRAM
{
  int i;
  for (i=0; i<=7; i++) {
    HMI_LCD_write(0,0x40 | ((charnr&0x07)<<3) | (i&0x07));
    HMI_LCD_write(1,row_data[i]);
  }
}

void HMI_LCD_init_CGRAM(void)    // write custom characters into CGRAM
{   
  HMI_LCD_set_character(0,cchar0);
  HMI_LCD_set_character(1,cchar1);
  HMI_LCD_set_character(2,cchar2);
  HMI_LCD_set_character(3,cchar3);
  HMI_LCD_set_character(4,cchar4);
  HMI_LCD_set_character(5,cchar5);
  HMI_LCD_set_character(6,cchar6);
  HMI_LCD_set_character(7,cchar7);
}

void HMI_LCD_number(unsigned long value, unsigned char length, char fill)				
{							
  unsigned long devider;
  unsigned char i, min;
  
  devider = 1;
  min = 0;
  
  for (i=1; i<length; i++)
    devider = 10 * devider;
    
  while (devider)
  {
    i = value / devider;
    if ((i==0) && (min==0) && (devider!=1) )
      HMI_LCD_write(1,fill);
    else  
      HMI_LCD_write(1,48+i);
    if (i>min) 
      min = i;  
    value = value - (i*devider);
    devider= devider / 10;
  }
}

void HMI_LCDshiftLeft(int count,int delay)
{	
	int i;
	for (i=count;i>0;i--)
	{
		HMI_LCD_write(0,LCD_DISPshiftl);
		pause(delay);    
	}	
}

void HMI_LCDshiftRight(int count,int delay)
{	
	int i;
	for (i=count;i>0;i--)
	{
		HMI_LCD_write(0,LCD_DISPshiftr);
		pause(delay);    
	}	
}

void ScrollLCDString16ch(char *output, const char *input, unsigned char offset) {

  char dest_pos, src_pos;
  
  for (dest_pos = 0; dest_pos < 16; dest_pos++) {
    src_pos = (dest_pos + offset) & 0x0f;
    output[dest_pos] = input[src_pos];
  }

}

void ScrollLCDString(char *output, const char *input, unsigned char output_length, unsigned char input_length, unsigned char offset) {

  unsigned char dest_pos, src_pos;
  
  src_pos = offset;
  dest_pos = 0;
  
  for (dest_pos = 0; dest_pos < output_length; dest_pos++) {
    src_pos++;
    if (src_pos >= input_length) src_pos = 0;
    output[dest_pos] = input[src_pos];
  }

}

unsigned char HMI_get_button_status(void) {            // returns a bit field containing the button status. A simple counter is used for debouncing  (has to be called periodically!)
  static unsigned char button_state[5] = {0,0,0,0,0};  // counter variable for each button for debouncing
  unsigned short int i2c_readval;
  unsigned char old_button_state, report_button_state = 0, i;
  
  i2c_readval = PCF8575_read_port(PCF8575_ADRESS);                   // read and store the button input state
  old_button_state = (unsigned char) ((i2c_readval & 0x1f00) >> 8);
  
  for (i=0; i<5; i++) {
    if ((old_button_state & (1<<i)) == 0) {                        // buttons are active low, check if pressed
      if (button_state[i] < HMI_BUTTON_DEBOUNCE_THRESHOLD) {       // if not yet above threshold, increment counter
        button_state[i]++;
      }
      else {                                                       // otherwise report button as pressed
        report_button_state |= (1<<i);
      }
    }
    else {                                                         // reset counter if currently not pressed
      button_state[i] = 0;
    }
  }
  return report_button_state;
}

void HMI_set_led_status(unsigned char pattern)  // write a pattern (3bit, active high) to the LEDs
{
  hmi_pinstate.u16data &= ~0xe000;                         // clear old pattern
  hmi_pinstate.u16data |= ((~pattern & 0x07) << 13);       // invert pattern (LEDs are active high) and write to data structure
  PCF8575_set_port(PCF8575_ADRESS, hmi_pinstate.u16data);  // send to HMI module
}

void HMI_set_backlight_on(void)   // turn on LCD backlight
{
  hmi_pinstate.bits.lcd_BL = 1;
  PCF8575_set_port(PCF8575_ADRESS, hmi_pinstate.u16data);
}

void HMI_set_backlight_off(void)   // turn off LCD backlight
{
  hmi_pinstate.bits.lcd_BL = 0;
  PCF8575_set_port(PCF8575_ADRESS, hmi_pinstate.u16data);
}

void HMI_init(void)           // init MFS1 (I2C), init LCD on HMI module, init data structure for HMI communication
{
  I2C_GPIO_I2C_BusClear();
  InitMFS1_I2C();
  
  hmi_pinstate.u16data = 0xff08;                          // LEDs off (high), input pins high (pullup), lcd pins low, backlight on
  PCF8575_set_port(PCF8575_ADRESS, hmi_pinstate.u16data); // Set pin state on HMI module (PCF8575)

  HMI_LCD_init();

  pause(0xffffff);

  HMI_LCD_init_CGRAM();

  pause(0xffff);
  
}


/*****************************************************************************
*  DESCRIPTION:    send a x-digit Dec-number (as ASCII charcaters)
*                  to terminal via ext. UART
*
*  PARAMETERS:     integer value
*
*  RETURNS:        None
*****************************************************************************/
void LcdFx_WriteDec(uint32_t x)
{
                int i;
                char buf[9];
                if (x == 0)
        { 
            //LcdFx_Write("   0");
            HMI_LCD_printline(1,1,"   0");
            return;
        }
                buf[8]='\0';                                                         /* end sign of string */
                
                for (i=8; i>0; i--) 
                {
           buf[i-1] = '0' + x % 10;
                   x = x/10;

                }

                for (i=0; buf[i]=='0'; i++) // no print of zero 
                {
                   buf[i] = ' ';
        }
        //for(i=0;i<8;i++)
        //{
                //LcdFx_WriteChar(buf[i]);
                HMI_LCD_printline(1,8,buf);
        //}
}

