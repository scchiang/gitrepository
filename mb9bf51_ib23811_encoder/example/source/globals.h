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

void wait(uint32_t ms);

#define TRUE 1
#define FALSE 0
#define FORWARD 1
#define BACKWARD -1
#define STOPPED 0
#define ENC_INIT 1
#define STOPPING 2
#define STARTING 3
#define RUNNING 4
#define ACTIVE 1
#define INACTIVE 0
#define FALSE 0
#define TRUE 1

//#define SINTAB_SIZE 500             // must fit to sine table size used in ClarkePark.c !
#define SINTAB_SIZE 2048             //sc must fit to sine table size used in ClarkePark.c !
#define HALF_PI_SINTAB_SIZE (SINTAB_SIZE / 4)

#define ONE_OVER_SQRT3_Q15 0x49E6    // 1/sqrt(3) in Q15 format (=1/sqrt(3) * 2^15)
#define ONE_OVER_SQRT3_Q12 0x93c    // 1/sqrt(3) in Q12 format (=1/sqrt(3) * 2^12)

#define SQRT3HALF_Q15 0x6ED9         // sqrt(3)/2 in Q15 format
#define SQRT3HALF_Q12 0xddb        // sqrt(3)/2 in Q12 format

#define CURRENT_SCALE 1              // 32 scale from ADC value to Q15 format
#define PWM_MAX_VAL 0x3ff         // compare clear value for frt0; @32MHz PLL: pwm freq.=32MHz/(2*PWM_MAX_VAL) (up/down), e.g. 0x03ff = ca. 15kHz
#define MAX_DUTY (PWM_MAX_VAL-5) // can be used to limit duty cycle (should not be higher than PWM_MAX_VAL)
                                  // this can be necessary depending on the circuit, to ensure bootstrap reload or ADC sampling
#define MIN_DEADTIME 0x001f       //1f minimum reload
#define INV_CLARKE 1
#define SVM 2


//Variables for Scheduler
extern volatile int32_t msTicks;                // timebase ticks for system scheduler
extern volatile uint16_t sched1_ticks, sched2_ticks, sched3_ticks;//, msec, seconds, minutes; // used in rlt0 isr
extern volatile uint16_t hprio_sched1_ticks, hprio_sched2_ticks, hprio_sched3_ticks;       // used in rlt0 isr
extern volatile uint16_t delay_counter;

//Helper Variables
extern int32_t SinAngle;
extern int32_t CosAngle;
extern uint64_t bt0val32;
extern volatile int8_t bt0count_valid;
extern int16_t startup_ampl;
extern volatile int16_t poti_rpm; 
extern uint16_t phase_current_offset_a, phase_current_offset_b;

//Motor Variables
extern volatile int8_t act_dir; 
extern volatile int16_t abs_rpm;
extern volatile int8_t dir; 
extern volatile int8_t motor_state;
extern volatile int16_t rotor_angle;
extern volatile int8_t modtype;
extern volatile int16_t act_rpm;
extern volatile int16_t desired_rpm; 
extern volatile int16_t fixed_angle;
extern unsigned char position_mode;
extern int64_t desired_position, abs_position;
extern int32_t PotiVal;



// Structures for PID Regulator
typedef struct {
  int32_t p;                        // PID coefficients: P
  int32_t i;                        //                   I
  int32_t d;                        //                   D
  int32_t limit_output;             // limit of the pid regulators
  int32_t e_n0;                     // last control error
  int32_t e_n1;                     // old control error
  int32_t error_sum;                // error of last cycle
  int32_t counter;                  // Virtual output of the regulator
  int32_t last_output;              // Last output

} pid_params_t;


extern pid_params_t pid_params_id;
extern pid_params_t pid_params_iq;
extern pid_params_t pid_params_sp;

typedef struct {                // stator phase currents in the rotating dq-system attached to the rotor
  int32_t d;                      // Isd_act, des_id
  int32_t q;                      // isq_act, des_iq
} rotsys_current_t;


typedef struct {                // output reference voltages from the regulators
  int32_t d;                      // Ud_ref  output from current loops
  int32_t q;                      // Uq_ref
  int32_t alpha;                  // Ualpha_ref 2-axis components after inv. park transform 
  int32_t beta;                   // Ubeta_ref
  int32_t u;                      // vref_a     3-phase representation, e.g. after inv. clarke transform
  int32_t v;                      // vref_b
  int32_t w;                      // vref_c
} statsys_rotsys_variables_t;


extern statsys_rotsys_variables_t act_system_currents;
extern statsys_rotsys_variables_t des_system_voltages;
extern rotsys_current_t ref_input;

extern int32_t checkerSpeed;

