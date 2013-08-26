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

#include "globals.h"
#include "ClarkePark.h"
#include "pid.h"
#include "motor.h"
#include "adc.h"
#include "QPRC.h"

uint16_t adcval0 = 0, adcval1 = 0;



//*****************************************************************************
//  Setup ADC1
//*****************************************************************************
void initAdc0(void) 
{
    FM3_GPIO->ADE |= 0x0006;      // AD input enable
    
    FM3_ADC0->ADST1 = 0x1F;       // Sampling Time 1
    FM3_ADC0->ADST0 = 0x1F;       // Sampling Time 0
        
    FM3_ADC0->ADSS3 = 0x00;       // Sampling Time Select AN31-AN24
    FM3_ADC0->ADSS2 = 0x00;       // Sampling Time Select AN23-AN16
    FM3_ADC0->ADSS1 = 0x00;       // Sampling Time Select AN15-AN08
    FM3_ADC0->ADSS0 = 0x00;       // Sampling Time Select AN07-AN00 -> Use Sampling Time 0 for AN0
    
    FM3_ADC0->ADCT  = 0x02;       // Comparison Time = 14 x (ADCT + 2) / HCLK
    
    FM3_ADC0->SFNS = 0x00;        // Set Fifo Stage Count Interrupt
    FM3_ADC0->ADCR = 0x08;        // Scan conversion interrupt request bit, Scan conversion Interrupt enable, 
    FM3_ADC0->SCCR = 0x12;        // Scan Conversion Control Register, Scan conversion timer start enable bit
    FM3_ADC0->SCTSL = 0x01;       // Starts scan conversion with the multifunction timer.
    FM3_ADC0->ADSR = 0x40;        // right aligned
    
    FM3_ADC0->SCIS3 = 0x00;       // Scan channel select AN31-AN24
    FM3_ADC0->SCIS2 = 0x00;       // Scan channel select AN23-AN16
    FM3_ADC0->SCIS1 = 0x00;       // Scan channel select AN15-AN08
    FM3_ADC0->SCIS0 = 0x02;       // AN02, Phase A (Motor 1)
      
    FM3_ADC0->ADCEN = 0x01;       // Enable ADC    
    while (3 != FM3_ADC0->ADCEN); // wait until ADC operation is enabled
}

//*****************************************************************************
//  Setup ADC1
//*****************************************************************************
void initAdc1(void) 
{
    FM3_ADC1->ADST1 = 0x1F;       // Sampling Time 1
    FM3_ADC1->ADST0 = 0x1F;       // Sampling Time 0
        
    FM3_ADC1->ADSS3 = 0x00;       // Sampling Time Select AN31-AN24
    FM3_ADC1->ADSS2 = 0x00;       // Sampling Time Select AN23-AN16
    FM3_ADC1->ADSS1 = 0x00;       // Sampling Time Select AN15-AN08
    FM3_ADC1->ADSS0 = 0x00;       // Sampling Time Select AN07-AN00 -> Use Sampling Time 0 for AN0
    
    FM3_ADC1->ADCT  = 0x02;       // Comparison Time = 14 x (ADCT + 2) / HCLK
    
    FM3_ADC1->SFNS = 0x00;        // Set Fifo Stage Count Interrupt
    FM3_ADC1->ADCR = 0x08;        // Scan conversion interrupt request bit, Scan conversion Interrupt enable, 
    FM3_ADC1->SCCR = 0x12;        // Scan Conversion Control Register, Scan conversion timer start enable bit
    FM3_ADC1->SCTSL = 0x01;       // Starts scan conversion with the multifunction timer.
    FM3_ADC1->ADSR = 0x40;        // right aligned
    
    FM3_ADC1->SCIS3 = 0x00;       // Scan channel select AN31-AN24
    FM3_ADC1->SCIS2 = 0x00;       // Scan channel select AN23-AN16
    FM3_ADC1->SCIS1 = 0x00;       // Scan channel select AN15-AN08
    FM3_ADC1->SCIS0 = 0x04;       // AN03, Phase A (Motor 1)
      
    FM3_ADC1->ADCEN = 0x01;       // Enable ADC    
    while (3 != FM3_ADC1->ADCEN); // wait until ADC operation is enabled
}

//*****************************************************************************
//  Setup ADC2
//*****************************************************************************
void initAdc2(void) 
{
    FM3_ADC2->ADST1 = 0x1F;       // Sampling Time 1
    FM3_ADC2->ADST0 = 0x1F;       // Sampling Time 0
        
    FM3_ADC2->ADSS3 = 0x00;       // Sampling Time Select AN31-AN24
    FM3_ADC2->ADSS2 = 0x00;       // Sampling Time Select AN23-AN16
    FM3_ADC2->ADSS1 = 0x00;       // Sampling Time Select AN15-AN08
    FM3_ADC2->ADSS0 = 0x00;       // Sampling Time Select AN07-AN00 -> Use Sampling Time 0 for AN0

    FM3_ADC2->ADCT  = 0x02;       // Comparison Time = 14 x (ADCT + 2) / HCLK
    
    FM3_ADC2->SFNS = 0x00;        // Set Fifo Stage Count Interrupt
    FM3_ADC2->ADCR = 0x08;        // Scan conversion interrupt request bit, Scan conversion Interrupt enable, 
    FM3_ADC2->SCCR = 0x12;        // Scan Conversion Control Register, Scan conversion timer start enable bit
    FM3_ADC2->SCTSL = 0x01;       // Starts scan conversion with the multifunction timer.
    FM3_ADC2->ADSR = 0x40;        // right aligned
    
    FM3_ADC2->SCIS3 = 0x40;       // Scan channel select AN31-AN24, Poti for Speed (Motor 1)
    FM3_ADC2->SCIS2 = 0x00;       // Scan channel select AN23-AN16
    FM3_ADC2->SCIS1 = 0x00;       // Scan channel select AN15-AN08
    FM3_ADC2->SCIS0 = 0x00;       // Scan channel select AN07-AN00
      
    FM3_ADC2->ADCEN = 0x01;       // Enable ADC    
    while (3 != FM3_ADC2->ADCEN); // wait until ADC operation is enabled
}


//*****************************************************************************
//  ADC Handler
//*****************************************************************************
void ADC0_IRQHandler(void) 
{                                         // current measurements (also uses ADC1 Ch0 Value!)
    static uint8_t adc_cycle_count;       // used to control after how many current controller cycles the speed controller should run
    static uint8_t avg_ptr;
    static int32_t adc0_avg_values[8];
    static int32_t adc1_avg_values[8];
    uint8_t i;
    int64_t tmp1, tmp2;
    int32_t desired_rpm_tmp;
    int32_t temp_speed_error;
    
    int32_t adcval0 = (FM3_ADC0->SCFDH);                  // Readout the FIFO for channel 0;
    int32_t adcval1 = (FM3_ADC1->SCFDH);                  // Readout the FIFO for channel 1
    //PotiVal = FM3_ADC2->SCFDH;
    //PotiVal = FM3_ADC2->SCFDH;
     
    act_system_currents.u = ((adcval0 - phase_current_offset_a));  // Current scale to get more accuracy 12bit shift 
    act_system_currents.v = ((adcval1 - phase_current_offset_b));  // of motor (and through the shunt) is negative
    act_system_currents.w = (-act_system_currents.u - act_system_currents.v);   //Not used in the firmware


    clarke_fwd();  // from act u,v,w calculate alpha and beta
    park_fwd();    //  from act alpha beta and current angle (encoder line) calculate act d and q 
    act_rpm = calc_rpm();   // calculate actual speed 

    //Limit measurement faults
    if (act_rpm > MAX_RPM)
        act_rpm = MAX_RPM;
    else if(act_rpm < -MAX_RPM)
        act_rpm = -MAX_RPM;
      
    if (motor_state >= STARTING) 
    {
        if (++adc_cycle_count >= ADC_SP_REG_INTERVAL) 
        { 
            if (position_mode) 
            {
                abs_position = get_abs_position();
                desired_rpm = reg_pos(desired_position, abs_position);
            }
    
            desired_rpm_tmp = desired_rpm;
            temp_speed_error = -(desired_rpm_tmp - act_rpm);//speed_average.average);             
            pid_control_sp (temp_speed_error);
            adc_cycle_count = 0;                                     // clear counter
        }
                  
        pid_control_id ((ref_input.d - act_system_currents.d)); // calculate actual Id error 
        pid_control_iq ((ref_input.q - act_system_currents.q)); // calculate actual Iq error
    } // if (motor_state >= STARTING)

    
    if (motor_state == STOPPED) 
    {
        des_system_voltages.d = 0;           // clear status variables if motor is not running
        des_system_voltages.q = 0; 
        des_system_voltages.alpha = 0;
        des_system_voltages.beta = 0;
         
    
        adc0_avg_values[avg_ptr] = adcval0;
        adc1_avg_values[avg_ptr] = adcval1;
        if ((++avg_ptr) >= 8) avg_ptr = 0;
        tmp1 = 0;
        tmp2 = 0;
        for (i=0; i<8; i++) 
        {
            tmp1 += adc0_avg_values[i];
            tmp2 += adc1_avg_values[i];
        }
        phase_current_offset_a = tmp1 >> 3;
        phase_current_offset_b = tmp2 >> 3;
    }                  
 
    FM3_ADC0->SCCR = 0x12;                      // clear FIFO status
    FM3_ADC1->SCCR = 0x12;
    FM3_ADC0->ADCR &= ~0x80;                    // clear IRQ flag
    NVIC_ClearPendingIRQ(ADC0_IRQn);
}

