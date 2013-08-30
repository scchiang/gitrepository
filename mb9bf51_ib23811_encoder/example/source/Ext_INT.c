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
#include "motor.h"
#include "globals.h"

//*****************************************************************************
//  Setup external interrupt
//*****************************************************************************
void initExtInterrupt(void){
  
   bFM3_GPIO_DDR5_P0 = 0;
   bFM3_GPIO_DDR5_P1 = 0;
      
   //Set Port as function
   bFM3_GPIO_PFR5_P0 = 0x1;     //Taster 1
   bFM3_GPIO_PFR5_P1 = 0x1;     //Taster 2  
       
   FM3_GPIO->EPFR06 |= 0x0003e800;  // Use Port as interrupt input
   
   FM3_EXTI->ENIR = 0;
   
   bFM3_EXTI_ELVR_LA0 = 1;
   bFM3_EXTI_ELVR_LB0 = 1;      // Taster 1, falling edge
   
   bFM3_EXTI_ELVR_LA1 = 1;
   bFM3_EXTI_ELVR_LB1 = 1;      // Taster 2, falling edge
   
   FM3_EXTI->EICL=0;
   FM3_EXTI->ENIR=0x00003;
}

//*****************************************************************************
//  IRQ-Handler for the speed changing
//*****************************************************************************
void INT0_7_Handler(void)
{
//    unsigned int count = 0;          // counter for LED displays
//    const unsigned short LEDPATTERN[10] = {0xC000, 0xF900, 0xA400, 0xB000, 0x9900, 0x9200, 0x8200, 0xF800, 0x8000, 0x9000};

    if(bFM3_INTREQ_IRQ04MON_EXTINT1==1)
    {
       //STOP
       //Button 1 pressed
      if (poti_rpm < MAX_RPM)
        poti_rpm=poti_rpm+30;
    }
    
    if(bFM3_INTREQ_IRQ04MON_EXTINT0==1)
    {
      //Button 2 pressed
      //if (poti_rpm > 150)
        poti_rpm=poti_rpm-30;
          
    }
  
//    count = 380 -  FM3_MFT1_OCU->OCCP0;                         //
//    FM3_GPIO->PDOR1 = LEDPATTERN[((count/100) - ((count / 1000) * 1000))];                   // set tens' place 
//    FM3_GPIO->PDOR3 = LEDPATTERN[((count) - ((count / 100) * 100))/10];  // set ones' place
  
    FM3_EXTI->EICL=0;
}
