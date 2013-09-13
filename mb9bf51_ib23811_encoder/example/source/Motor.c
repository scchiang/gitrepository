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
#include "pid.h"
#include "motor.h"
#include "QPRC.h"


uint16_t startup_speed_ramp = 0;

//*****************************************************************************
//  Stop Motor
//*****************************************************************************
void StopMotor (void)
{
    desired_rpm = 0;
    motor_state = STOPPING;
    startup_speed_ramp = 0;                        // stop possible startup speed ramping
    //while (motor_state != STOPPED) //__wait_nop();   // wait until motor has stopped
    motor_state = STOPPED;
    {}
}

//*****************************************************************************
//  Start Motor
//*****************************************************************************
void start_motor(signed char direction) {
  if (motor_state != STOPPED) {
    StopMotor();
  }
  
  init_all_pids();
  dir = direction;
  motor_state = STARTING;   // will be set to 'RUNNING' as soon as target speed is reached
  startup_speed_ramp = 1;
}


//*****************************************************************************
// Scheduler, Task 1
// these function blocks are called periodically from the RLT0 ISR, 
// so do nothing blocking here !
//*****************************************************************************
void hprio_sched1(void)
{   
   int poti = poti_rpm; 
   
   
   
    if (act_dir == FORWARD) 
    {
        abs_rpm = act_rpm;
    }
    else 
    {
        abs_rpm = (-act_rpm);
    }
  
    
    if (motor_state == RUNNING) 
    {
        //desired_rpm  = poti * dir;                // if motor is running, use poti value as speed setpoint
    }
    else if (motor_state == STARTING) 
    {
        if ((abs_rpm >= poti) || (startup_speed_ramp >= poti))                        // when desired speed is reached
        {
            motor_state = RUNNING;                     // set 'running' state
            startup_speed_ramp = 0;                    // stop ramping speed up
        }
        else 
        {
            desired_rpm = startup_speed_ramp * dir;    // use speed from startup ramper at beginning
        }
    }
  
    else if (motor_state == STOPPING) 
    {
        if (abs_rpm < 100) 
        {
            motor_state = STOPPED;
            desired_rpm = 0;
        }
    }
   
/*   
    if (act_dir == FORWARD) 
    {
        abs_rpm = act_rpm;
    }
    else 
    {
        abs_rpm = (-act_rpm);
    }
  
    
    if (motor_state == RUNNING) 
    {
        desired_rpm  = poti * dir;                // if motor is running, use poti value as speed setpoint
    }
    else if (motor_state == STARTING) 
    {
        if ((abs_rpm >= poti) || (startup_speed_ramp >= poti))                        // when desired speed is reached
        {
            motor_state = RUNNING;                     // set 'running' state
            startup_speed_ramp = 0;                    // stop ramping speed up
        }
        else 
        {
            desired_rpm = startup_speed_ramp * dir;    // use speed from startup ramper at beginning
        }
    }
  
    else if (motor_state == STOPPING) 
    {
        if (abs_rpm < 100) 
        {
            motor_state = STOPPED;
            desired_rpm = 0;
        }
    }
*/
}


