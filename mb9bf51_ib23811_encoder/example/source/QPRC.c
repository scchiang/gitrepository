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
#include "QPRC.h"

uint64_t qprc_ctmm = 0, qprc_ctmm_nc = 0;


//*****************************************************************************
//  Init QPRC for speed and position measurement
//*****************************************************************************
void initQPRC(void)
{
    bFM3_GPIO_PFR3_P0 = 1;                  // Set Port function register for QPRC /A
    bFM3_GPIO_PFR3_P1 = 1;                  // Set Port function register for QPRC /B
        
    FM3_GPIO->EPFR09 |= 0x05;               // AIN0_0, BIN0_0 used for QPRC unit 0
    
    FM3_QPRC0->QCRL = 0x0a;                 // no Swap AB, Phase difference count mode (x4), RC Mode 2
    // 00 00 10 10 swap, rsel, cgsc,pstp, rcm1,rcm0, pcm1, pcm0 
    FM3_QPRC0->QCRH = 0x3c;                 // ZIN disable, AIN + BIN rising + falling edge select
    // 00 11 11 00 Z rise fall B reise A rise fall PCRM1 PCRM0  high byte
    FM3_QPRC0->QECR = 0;                    // No Interrupt from QPRC
    FM3_QPRC0->QICRL = 0x0;                 // No interrupt from QPRC 
    FM3_QPRC0->QICRH = 0x0;                 // No interrupt from QPRC 
    
    FM3_QPRC0->QMPR = ENCODER_COUNTS;
}

//*****************************************************************************
//  Encoder zero seach 
//*****************************************************************************

void EncoderZeroSearch(void) 
{
    fixed_angle = 0;
    motor_state = ENC_INIT;
    
    while (startup_ampl < ZS_ISQ_MAX) 
    {
        startup_ampl += ZS_ISQ_INCR;     // ramp up current for zero search
        wait(20);
    }
//    while (1){  
        for (fixed_angle = 0 ; fixed_angle < 8192 ;  (fixed_angle+= 1))   
        //for (fixed_angle = 0 ; fixed_angle < ENCODER_LINES ;  fixed_angle++)   
          {                               
             // wait(ZS_INCR_DELAY);
             wait(1);
          }
//    }
    bFM3_QPRC0_QCR_PSTP = 1;                    // Stop QPRC
    FM3_QPRC0->QPCR = 0;                        // Delete counter value
    bFM3_QPRC0_QCR_PSTP = 0;                    // Start QPRC    
 }

//*****************************************************************************
//  BT 0 1 for speed measurement
//*****************************************************************************
void initBt0Pwc(void) 
{
    FM3_GPIO->EPFR04 = 0x2a28;                // Uses TIOB0_0 at the input pin of BT-ch0-TIOB. [Initial value]
    FM3_BTIOSEL03->BTSEL0123 = 0x0001;        // BT0 is running in I/O mode 1, PWC Timer functionality
 
    FM3_BT0_PWC->TMCR=0;
    
    FM3_BT0_PWC->TMCR2 = 0x00;     
    FM3_BT1_PWC->TMCR2 = 0x00;     

    FM3_BT0_PWC->TMCR = 0x02c2;                // 32-bit, Cycle measurement between falling edges
    FM3_BT1_PWC->TMCR = 0x0242;                // Cycle measurement between falling edges
    
    FM3_BT1_PWC->STC = 0x00;                   // enable overflow interrupt
}

//*****************************************************************************
//  Get rotor angle 
//*****************************************************************************
void get_rotor_angle(void) 
{
     rotor_angle = FM3_QPRC0->QPCR;             // Position is directly acessable from the QPRC
}

//*****************************************************************************
//  Get abs position
//*****************************************************************************
signed long int get_abs_position (void) 
{
    signed long int abs_pos;
    signed short int rev_cnt, rev_cnt_rr;
    unsigned short int pos_cnt;
    unsigned char cnt;
  
    pos_cnt = FM3_QPRC0->QRCR;
    rev_cnt = FM3_QPRC0->QPCR;
    rev_cnt_rr = FM3_QPRC0->QRCR;
    
    if (rev_cnt != rev_cnt_rr) 
    {
        cnt = 3;
        do {
            rev_cnt = FM3_QPRC0->QRCR;
            pos_cnt = FM3_QPRC0->QPCR;
            rev_cnt_rr = FM3_QPRC0->QRCR;
            qprc_ctmm++;
            } while ((--cnt) && (rev_cnt != rev_cnt_rr));
    if (cnt == 0) qprc_ctmm_nc++;
    }
    abs_pos = ((signed long int)rev_cnt * ENCODER_COUNTS) + pos_cnt;
  
    return abs_pos;
}

//*****************************************************************************
//  Calculate the speed of the rotor
//*****************************************************************************
short int calc_rpm(void) 
{
    unsigned long int rpm=0;
    unsigned short int high;
    unsigned short int low;

    low = FM3_BT0_PWC->DTBF;
    high = FM3_BT1_PWC->DTBF;
    bt0val32 = (high << 16) | low;

    if (bFM3_QPRC0_QICR_DIRPC == 0) 
//    if (bFM3_QPRC0_QICR_DIRPC == 1) 
        act_dir = FORWARD;
    else 
        act_dir = BACKWARD;

    rpm = (BT0COUNTCLOCK * 60 / (ENCODER_LINES * POLEPAIRS)) / bt0val32  ;
  
    if (rpm < ZERO_SPEED_THRESHOLD) 
        rpm = 0;  // this can stabilize the speed loop in pos. mode
   
    return (short int) (act_dir * rpm);
    }


//*****************************************************************************
//  BT overflow and speed measurement
//*****************************************************************************
void BT_IRQHandler(void) 
{
    if (bFM3_BT2_PWC_STC_OVIR == 1) 
    {       
        // if it was an overflow of BT0
        bt0count_valid = FALSE;                 // motor is too slow for speed measurement
        act_rpm=0;                              // Set Speed to 0 RPM
        bFM3_BT2_PWC_STC_OVIR = 0;              // BT0STC_OVIR = 0;  clear interrupt flag
        // Place Error handler in this routine!!
        // StopMotor();
    }
}


//*****************************************************************************
//  QPRC Handler
//*****************************************************************************

void DT_Handler(void)
{
  bFM3_QPRC0_QICR_ZIIF=0;                       // Delete Interrupt request from QPRC
 
}




