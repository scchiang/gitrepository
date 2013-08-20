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


#define MAX_RPM 3600              // nominal RPM of the motor (BG40x25)
#define MIN_RPM 200
//#define POLEPAIRS 4

#define POLEPAIRS 3 // for testing MCG motor


// #define ENCODER_LINES 125        // number of lines per electrical round
#define ENCODER_LINES 2731        // number of lines per electrical round
#define ENCODER_COUNTS (ENCODER_LINES * POLEPAIRS)-1 // Pulses per Round = lines per round x2 pulses per line x2 channels

//#define ZS_ISQ_MAX 5000           // value used for zero search forced commutation
#define ZS_ISQ_MAX 500          // value used for zero search forced commutation
//#define ZS_ISQ_INCR 500           // value used for zero search forced commutation
#define ZS_ISQ_INCR 50           // value used for zero search forced commutation
#define ZS_INCR_DELAY 2          // delay between two angle steps during zero search


#define ZERO_SPEED_THRESHOLD 20   // speed below this RPM will be measured as zero; this can stabilize the loops
#define STARTUP_SPEED_INCREMENT 8 // startup speed increment in RPM per ms

extern uint16_t startup_speed_ramp;

void StopMotor (void);
void start_motor(signed char direction);
void hprio_sched1(void);

