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

//#include "mcu.h"

/**
 ******************************************************************************
 ** \brief  Main function of project for MB9B510 series.
 **
 ** \param  none
 ** \return uint32_t return value, if needed
 ******************************************************************************/
/*
int32_t main(void)
{
  int i;
  bFM3_GPIO_DDR3_PC=1;
  while(1){
    for(i=0;i<2000000;i++) bFM3_GPIO_PDOR3_PC=1;
    for(i=0;i<2000000;i++) bFM3_GPIO_PDOR3_PC=0;
  }
  
}
*/


#include "mcu.h"
#include <stdio.h>
#include "motor.h"
#include "Interrupt.h"
#include "QPRC.h"
#include "pwm.h"
#include "globals.h"
#include "adc.h"
#include "Ext_INT.h"

#define SCHED1_INTERVAL 100             // These are the intervals for the polling scheduler tasks. Since they have lowest priority
#define SCHED2_INTERVAL 5               // (Main loop), the effective interval can vary with overall system load.
#define SCHED3_INTERVAL 5000


//Variables for Scheduler
volatile int32_t msTicks;                   // timebase ticks for system scheduler
volatile uint16_t sched1_ticks=0, sched2_ticks=0, sched3_ticks=0; // used in rlt0 isr
volatile uint16_t delay_counter;

//Helper Variables
int32_t SinAngle;
int32_t CosAngle;
uint64_t bt0val32;
int16_t startup_ampl = 0;
int32_t PotiVal = 0;
uint16_t phase_current_offset_a, phase_current_offset_b;
volatile int8_t bt0count_valid = TRUE;
volatile int16_t poti_rpm = 0; 

//Motor Variables
volatile int8_t    act_dir;
volatile int8_t    dir = FORWARD; 
volatile int8_t    motor_state = STOPPED;
volatile int16_t   rotor_angle;
volatile int8_t    modtype = INV_CLARKE;//SVM;//                // SVM or INV_CLARKE
//volatile int8_t    modtype = SVM;//                // SVM or INV_CLARKE
volatile int16_t   act_rpm = 0;
volatile int16_t   abs_rpm = 0;
volatile int16_t   desired_rpm = 0; 
volatile int16_t   fixed_angle;
uint8_t position_mode = 0;
int64_t desired_position = 0, abs_position = 0;

pid_params_t pid_params_id;
pid_params_t pid_params_iq;
pid_params_t pid_params_sp;

statsys_rotsys_variables_t act_system_currents;
statsys_rotsys_variables_t des_system_voltages;
rotsys_current_t ref_input;


int32_t main(void)
{
    SysTick_Config(80000000/1000);      // 1000ticks (1000us) per interrupt @80MHz
        
    printf("program start \n");
    initAdc0();                         // initialize ADC1 (12-bit, current measurement)
    initAdc1();                         // initialize ADC2 (12-bit, current measurement)
    initAdc2();                         // initialize ADC2 (12-bit, current measurement)
    initComplPwm();                     // initialize Multi-Function Timer + Waveform generator
    initBt0Pwc();                       // initialize base timer 0+1 in 32-bit PWC mode for speed measurement
    initQPRC();                         // Init QPRC for speed & position Sensing
    initIrqLevels();                    // Setup interrupts
    initExtInterrupt();
    initEnable();                       // Enable interrupts

 //   FM3_GPIO->ADE  = 0x00FF; // No Analog Inputs
 //   FM3_GPIO->DDR1 = 0xFF00;  // P18-P1F: LED-SEG1 output
 //   FM3_GPIO->DDR3 = 0xFF00;  // P38-P3F: LED-SEG2 output
   
 //   FM3_GPIO->PFR1 = 0x0000;  // P10-P1F: LED-SEG1 GPIO
 //   FM3_GPIO->PFR3 = 0x0000;  // P30-P3F: LED-SEG2 GPIO
  
 //   FM3_GPIO->PDOR1 = 0xA400;  //2
    StopMotor();                        // Stop motor
    wait (200);
//    poti_rpm = 400;
    poti_rpm = 140;
    ref_input.d = 0;                    // no field wakening
    
    EncoderZeroSearch();                // Calibrate the motor
    start_motor(FORWARD);
//    start_motor(BACKWARD);
        
    while(1) 
    {   // **************************** MAIN LOOP ************************************************
          
    }
  
}

void wait(unsigned int ms)
{
    if (delay_counter) return;            
    delay_counter = ms;
    while (delay_counter);
}

//*****************************************************************************
//  Scheduler, Task 2
//*****************************************************************************
void hprio_sched2(void)
{
    
}

//*****************************************************************************
//  Scheduler, Task 2
//*****************************************************************************
void hprio_sched3(void)
{
  
}


  
void SysTick_Handler (void)
{
    sched1_ticks++;                  // increment timebase for polling scheduler 1
    sched2_ticks++;                  // increment timebase for polling scheduler 2
    sched3_ticks++;                  // increment timebase for polling scheduler 3

    if ((sched1_ticks)>=SCHED1_INTERVAL) {sched1_ticks=0;hprio_sched1();} // increment timebase for schedulers
    if ((sched2_ticks)>=SCHED2_INTERVAL) {sched2_ticks=0;hprio_sched2();} // and call them if necessairy
    if ((sched3_ticks)>=SCHED3_INTERVAL) {sched3_ticks=0;hprio_sched3();}

    if (delay_counter) delay_counter--;  // delay counter used by wait loop
    if (startup_speed_ramp) startup_speed_ramp += STARTUP_SPEED_INCREMENT;   // used during startup speed ramp
    
    NVIC_ClearPendingIRQ(SysTick_IRQn);
}


