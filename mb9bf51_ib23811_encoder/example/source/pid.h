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

#define POS_KP_DEF 25000                // PID coefficients (conventional form) for position loop
#define POS_KI_DEF 0                    // PID coefficients (conventional form) for position loop
#define POS_KD_DEF 0                    // PID coefficients (conventional form) for position loop

// (POS_ANTIHUNT_THRESHOLD1 - POS_ANTIHUNT_THRESHOLD2) should be a power of 2 !!!
#define POS_ANTIHUNT_THRESHOLD1 36      // liniear position loop gain reduction below this position error
#define POS_ANTIHUNT_THRESHOLD2 2       // position loop gain = 0 below this position error
#define SP_REF_MAX_INCREMENT 50

#define ADC_SP_REG_INTERVAL 1           // run speed loop every n adc (current loop) cycles

#define DEF_Iq_MAX 500                  // upper limit for iq limit
#define DEF_Vq_MAX 28000
//#define DEF_Vq_MAX 18000
#define DEF_Vd_MAX 15000
//#define DEF_Vd_MAX 10000

#define SP_DEF_P         6000            // PID coefficients
#define SP_DEF_I         30         
#define SP_DEF_D         0          

#define IQ_DEF_P         230            // PID coefficients
//#define IQ_DEF_P         100            // PID coefficients
#define IQ_DEF_I         5     
#define IQ_DEF_D         0              

#define ID_DEF_P         230            // PID coefficients
//#define ID_DEF_P         100            // PID coefficients
#define ID_DEF_I         5  
#define ID_DEF_D         0               

extern unsigned short int pos_kp;
extern signed short int sp_max;

void init_all_pids(void);
void pid_control_iq (int ctrl_error);
void pid_control_id (int ctrl_error);
void pid_control_sp (int ctrl_error);

signed short int reg_pos(signed long int des_pos, signed long int act_pos);