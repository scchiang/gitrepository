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
#include "pwm.h"
#include "svm.h"

uint64_t value_over_limit_count = 0;    // variables used for debugging only (to check number ranges)
uint64_t value_under_limit_count = 0;

int32_t t_v1, t_v2;
int32_t ta, tb, tc;
int8_t sector;
int8_t psector;
const int8_t psectable[]={0,2,6,1,4,3,5};

void limit_to_pwm_cycle (void) {
  if (ta > MAX_DUTY) {ta=MAX_DUTY; value_over_limit_count++;}
  else if (ta<0) {ta=0; value_under_limit_count++;}
  if (tb > MAX_DUTY) {tb=MAX_DUTY; value_over_limit_count++;}
  else if (tb<0) {tb=0; value_under_limit_count++;}
  if (tc > MAX_DUTY) {tc=MAX_DUTY; value_over_limit_count++;}
  else if (tc<0) {tc=0; value_under_limit_count++;}
}

//*****************************************************************************
//  SVM
//*****************************************************************************
void svm(signed int a, signed int b) {
  signed int tmp_v_a, tmp_v_b, tmp_v_c;

  tmp_v_a = b;                                     // inverse clarke transform (with swapped alpha, beta)
  tmp_v_b = -b/2 + (a * SQRT3HALF_Q15 >>15);       // used for sector identification
  tmp_v_c = -b/2 - (a * SQRT3HALF_Q15 >>15);

  psector = 0;
  if (tmp_v_a > 0) psector += 1;
  if (tmp_v_b > 0) psector += 2;
  if (tmp_v_c > 0) psector += 4;

  // sector identification:
  // 'pseudo'sector: 1 2 3 4 5 6
  //  'real' sector: 2 6 1 4 3 5

  sector = psectable[psector];                       // lookup 'real' sector from pseudo sector

  switch (sector) {

    case 1:                                          // 'real' sector 1, pseudo sector 3
            t_v1 = (tmp_v_b * MAX_DUTY) >> 15;       // vector 1 active time
            t_v2 = (tmp_v_a * MAX_DUTY) >> 15;       // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;       // shortest duty cycle (last switch on)
            tb = tc + t_v2;                          // switching time for active vector 1->2
            ta = tb + t_v1;                          // longest duty cycle (first switch on)
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = ta;                             // write calculated switching times
            FM3_MFT1_OCU->OCCP3 = tb;                             // to output compare buffer registers
            FM3_MFT1_OCU->OCCP5 = tc;                             // in the right sequence according to the sector
            break;
    case 2:                                          // 'real' sector 2, pseudo sector 1
            t_v1 = ((-tmp_v_c) * MAX_DUTY) >> 15;    // vector 1 active time
            t_v2 = ((-tmp_v_b) * MAX_DUTY) >> 15;    // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;
            tb = tc + t_v1;                          // different assignment of v1 and v2 in
            ta = tb + t_v2;                          // even sectors to keep pulse order (0on->1on->2on->3on)
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = tb;
            FM3_MFT1_OCU->OCCP3 = ta;
            FM3_MFT1_OCU->OCCP5 = tc;
            break;
    case 3:                                          // 'real' sector 3, pseudo sector 5
            t_v1 = (tmp_v_a * MAX_DUTY) >> 15;       // vector 1 active time
            t_v2 = (tmp_v_c * MAX_DUTY) >> 15;       // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;
            tb = tc + t_v2;
            ta = tb + t_v1;
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = tc;
            FM3_MFT1_OCU->OCCP3 = ta;
            FM3_MFT1_OCU->OCCP5 = tb;
            break;
    case 4:                                          // 'real' sector 4, pseudo sector 4
            t_v1 = ((-tmp_v_b) * MAX_DUTY) >> 15;    // vector 1 active time
            t_v2 = ((-tmp_v_a) * MAX_DUTY) >> 15;    // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;
            tb = tc + t_v1;
            ta = tb + t_v2;
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = tc;
            FM3_MFT1_OCU->OCCP3 = tb;
            FM3_MFT1_OCU->OCCP5 = ta;
            break;  
    case 5:                                          // 'real' sector 5, pseudo sector 6
            t_v1 = (tmp_v_c * MAX_DUTY) >> 15;       // vector 1 active time
            t_v2 = (tmp_v_b * MAX_DUTY) >> 15;       // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;
            tb = tc + t_v2;
            ta = tb + t_v1;
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = tb;
            FM3_MFT1_OCU->OCCP3 = tc;
            FM3_MFT1_OCU->OCCP5 = ta;
            break;
    case 6:                                          // 'real' sector 6, pseudo sector 2
            t_v1 = ((-tmp_v_a) * MAX_DUTY) >> 15;    // vector 1 active time
            t_v2 = ((-tmp_v_c) * MAX_DUTY) >> 15;    // vector 2 active time
            tc = (MAX_DUTY - t_v1 - t_v2) / 2;
            tb = tc + t_v1;
            ta = tb + t_v2;
//            limit_to_pwm_cycle();                    // for debugging only
            FM3_MFT1_OCU->OCCP1 = ta;
            FM3_MFT1_OCU->OCCP3 = tc;
            FM3_MFT1_OCU->OCCP5 = tb;
            break;
    default:                                         // if e.g. all values are zero
            FM3_MFT1_OCU->OCCP1 = 0;
            FM3_MFT1_OCU->OCCP3 = 0;
            FM3_MFT1_OCU->OCCP5 = 0;
            break;
  }
}
