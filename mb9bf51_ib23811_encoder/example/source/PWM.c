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

#include <stdio.h>
#include "mcu.h"
#include "pwm.h"
#include "ClarkePark.h"
#include "svm.h"
#include "QPRC.h"

#include "pid.h"
#include "globals.h"



//*****************************************************************************
// INIT WFG, Channel 0
//*****************************************************************************
void initComplPwm(void)                       // configure MFT
  { 
    NVIC_DisableIRQ(FRTIM_IRQn);                // Disable Interrupts from FRT
    NVIC_DisableIRQ(WFG_IRQn);                  // Disable Interrupts from WFG
    
    FM3_MFT1_FRT->TCCP0 = PWM_MAX_VAL;          // set compare clear register of FRT0
    FM3_MFT1_FRT->TCSA0 = 0x2070;               // enable zero detect irq
                                                // Initialize FRT0, no compare clear match irq, up/down mode, full speed
    
    FM3_MFT1_OCU->OCFS10 = 0x00;                // Set all OCUs to use FRT0
    FM3_MFT1_OCU->OCFS32 = 0x00;
    FM3_MFT1_OCU->OCFS54 = 0x00;

    FM3_MFT1_OCU->OCCP0 = 0xffff;               // Output duty cycle is set by OCCPB1, 3, 5 only
    FM3_MFT1_OCU->OCCP2 = 0xffff;
    FM3_MFT1_OCU->OCCP4 = 0xffff;
    FM3_MFT1_OCU->OCCP1 = 0x0;                  // Output duty cycle is set by OCCPB1, 3, 5 only
    FM3_MFT1_OCU->OCCP3 = 0x0;
    FM3_MFT1_OCU->OCCP5 = 0x0;
  
    
    FM3_MFT1_OCU->OCSC = 0x0;                   // toggle mode for all output pins
    FM3_MFT1_OCU->OCSB10 = 0x70;                // Inititalize OCU: Buffer transfer at CPCLR, toggle on compare match
    FM3_MFT1_OCU->OCSB32 = 0x70;                // All RTO pins = waveform pins
    FM3_MFT1_OCU->OCSB54 = 0x70;                // channels starting with 0
    FM3_MFT1_OCU->OCSA10 = 0x3;                 // No compare match IRQs, enable compare buffers, enable compare operation for all registers
    FM3_MFT1_OCU->OCSA32 = 0x3;
    FM3_MFT1_OCU->OCSA54 = 0x3;
  
    FM3_MFT1_WFG->WFTM10 = MIN_DEADTIME;        // set dead time timer to about 500ns (enough for typical MOSFET power stages)
    FM3_MFT1_WFG->WFTM32 = MIN_DEADTIME;
    FM3_MFT1_WFG->WFTM54 = MIN_DEADTIME;
    FM3_MFT1_WFG->WFSA10 = 0x20;                // enable waveform generator to output non-overlapping waveform pairs for inverter control
    FM3_MFT1_WFG->WFSA32 = 0x20;
    FM3_MFT1_WFG->WFSA54 = 0x20;
  
    FM3_MFT1_ADCMP->ACCP0 = PWM_MAX_VAL;        // trigger on Peak detect
    FM3_MFT1_ADCMP->ACCP1 = PWM_MAX_VAL;
    FM3_MFT1_ADCMP->ACSA = 0x0015;              // Trigger ADC0+1 by ADTRG unit
    FM3_MFT1_ADCMP->ACSB = 0x0007;              //**Enables ADCMP ch.0. Connects FRT ch.0 to ADCMP ch.0.
  
    FM3_MFT1_ADCMP->ATSA=0x0000;                // Trigger ADC0+1 by ADTRG unit
  
    FM3_GPIO->DDR4 |= 0x003f;                 // Set Port as output
    FM3_GPIO->PFR4 |= 0x003f;                 // use as function  

    FM3_GPIO->EPFR02 |=0x0aaa;                // use EPFR02 WFG output relocate 01 
  
    FM3_MFT1_FRT->TCSA0 &= 0xffbf;              // start FRT1
    
  //************************
  // DTTI input
  //************************
  //FM3_GPIO->PFR4 |= 0x100;
  //FM3_GPIO->EPFR02 |= 0x00021000;           // enable DTTI input + PWM shutdown
  //FM3_MFT1_WFG->NZCL = 0x0009;              // External Fault detect
    
    }

//*****************************************************************************
//  Init WFG, for DTTI Interrupt for Fault handling
//*****************************************************************************
void MFT_WG_IRQHandler(void) 
    { 
        if (bFM3_MFT1_WFG_WFIR_DTIF)
        {
            NVIC_DisableIRQ(WFG_IRQn);                  // disable interrupts 
            //while (1);                                 // halt system
        }
    }
  

//*****************************************************************************
//  MFT FRT-Handler, sets the duty cycle according to the actual rotor position
//*****************************************************************************
void MFT_FRT_IRQHandler(void)
{ 
    int helper_phase_a;
    int helper_phase_b;
    int helper_phase_c;
    
    if (motor_state == STOPPED) 
    {
        FM3_MFT1_OCU->OCCP1 = 0;                // zero duty for all channels if motor is stopped
        FM3_MFT1_OCU->OCCP3 = 0;
        FM3_MFT1_OCU->OCCP5 = 0;
    }
    
    else if (motor_state >= STARTING) 
    {           
        get_rotor_angle();                          // update rotor angle
        park_inv();                                 // call inverse Park transform
    }
    
    else if (motor_state == ENC_INIT)                    // forced rotation to search for zero index at startup
    {  
        des_system_voltages.q = 0;
        des_system_voltages.d = startup_ampl;       // fixed ref. voltage during startup (no regulator active)
        park_inv();
    }


    if (modtype == INV_CLARKE)                      // inverse clarke transform can be used
    {                                               // to directly calculate the output duty cycles
        clarke_inv();                               // resulting in a 3ph sine output

        FM3_MFT1_OCU->OCCP1 = (int)((long long)( ((des_system_voltages.u * MAX_DUTY)>>16) + MAX_DUTY/2));   // generate the corresponding phase voltages and 
        FM3_MFT1_OCU->OCCP3 = (int)((long long)( ((des_system_voltages.v * MAX_DUTY)>>16) + MAX_DUTY/2));   // shift up by 1/2 amplitude (no neg. duty cycle...)
        FM3_MFT1_OCU->OCCP5 = (int)((long long)( ((des_system_voltages.w * MAX_DUTY)>>16) + MAX_DUTY/2));   // write values to OCU buffer regs
                         
        helper_phase_a = FM3_MFT1_OCU->OCCP1;
        helper_phase_b = FM3_MFT1_OCU->OCCP3;
        helper_phase_c = FM3_MFT1_OCU->OCCP5;

/*
        if (helper_phase_a > PWM_MAX_VAL)
          helper_phase_a = 1023;
        if (helper_phase_b > PWM_MAX_VAL)
          helper_phase_b = 1023;
        if (helper_phase_c > PWM_MAX_VAL)
          helper_phase_c = 1023;
*/
        // debug
       // printf ("phas a b c %d, %d, %d,\n", helper_phase_a, helper_phase_b, helper_phase_c);
        if ((helper_phase_a < 0)|(helper_phase_b < 0)|(helper_phase_c < 0))
        {
            FM3_MFT1_OCU->OCCP1 = 0;
            FM3_MFT1_OCU->OCCP3 = 0;
            FM3_MFT1_OCU->OCCP5 = 0;
            while (1);
                
        }  
            
        if ((helper_phase_a > PWM_MAX_VAL)|(helper_phase_b > PWM_MAX_VAL)|(helper_phase_c > PWM_MAX_VAL))
        {
            FM3_MFT1_OCU->OCCP1 = 0;
            FM3_MFT1_OCU->OCCP3 = 0;
            FM3_MFT1_OCU->OCCP5 = 0;
            while (1);
        } 
    }

    else if (modtype == SVM)                     // alternatively, SVM can be used, giving a higher output
    {                                        // voltage
            svm(des_system_voltages.alpha, des_system_voltages.beta);
    }
    
  
    bFM3_MFT1_FRT_TCSA0_IRQZF = 0x0;
    NVIC_ClearPendingIRQ(FRTIM_IRQn);             // clear interrupt flag
}



