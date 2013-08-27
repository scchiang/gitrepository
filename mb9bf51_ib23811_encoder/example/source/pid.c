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
#include "pid.h"
#include "motor.h"
#include "globals.h"

// The PID control loops 

uint16_t pos_kp = POS_KP_DEF;
int16_t sp_max = MAX_RPM;


//*****************************************************************************
//  Init PID regulator
//*****************************************************************************
void init_all_pids(void) {

  //Proportional Values
  pid_params_sp.p = SP_DEF_P;          
  pid_params_iq.p = IQ_DEF_P;
  pid_params_id.p = ID_DEF_P;
  
  pid_params_sp.i = SP_DEF_I;          
  pid_params_iq.i = IQ_DEF_I;
  pid_params_id.i = ID_DEF_I;
  
  pid_params_sp.d = SP_DEF_D;          
  pid_params_iq.d = IQ_DEF_D;
  pid_params_id.d = ID_DEF_D;
  
  pid_params_sp.limit_output = DEF_Iq_MAX;
  pid_params_iq.limit_output = DEF_Vq_MAX;
  pid_params_id.limit_output = DEF_Vd_MAX;
  
  pid_params_sp.error_sum = 0;
  pid_params_iq.error_sum = 0;
  pid_params_id.error_sum = 0;
  
  pid_params_sp.e_n0 = 0;          
  pid_params_iq.e_n0 = 0;
  pid_params_id.e_n0 = 0;
  
  pid_params_sp.e_n1 = 0;          
  pid_params_iq.e_n1 = 0;
  pid_params_id.e_n1 = 0;
   
  pid_params_sp.last_output = 0;          
  pid_params_iq.last_output = 0;
  pid_params_id.last_output = 0;
  
}

//*****************************************************************************
//  Speed regulator
//*****************************************************************************
void pid_control_sp (int32_t ctrl_error) 
{
    int32_t tmp_output_sp;
    int32_t tmp_output_p;
    int32_t tmp_output_i;
    int32_t tmp_output_d;
    int32_t tmp_output_error_sum;
    int32_t output_sp;
    
    //tmp_output_error_sum = pid_params_sp.error_sum + ctrl_error;         // The error is only updated if the control-error is not limited
    tmp_output_error_sum = ctrl_error + pid_params_sp.e_n0 + pid_params_sp.e_n1;         // The error is only updated if the control-error is not limited
    
    // Anti Windup effekt
    if (tmp_output_error_sum > pid_params_sp.limit_output) 
        pid_params_sp.error_sum = pid_params_sp.limit_output;
    else if (tmp_output_error_sum < -pid_params_sp.limit_output) 
        pid_params_sp.error_sum = -pid_params_sp.limit_output;
    else
        pid_params_sp.error_sum = tmp_output_error_sum;     
         
    // PID Regulator
    tmp_output_p = (int32_t)(       (long long)  ((pid_params_sp.p * ctrl_error)>>15));
    tmp_output_i = (int32_t)(       (long long)  ((pid_params_sp.i * pid_params_sp.error_sum)>>15));
    tmp_output_d = 0;                   
    
    tmp_output_sp =  pid_params_sp.last_output + ((tmp_output_p + tmp_output_i + tmp_output_d));
    
    // Limit the output of the PID Regulator
    if (tmp_output_sp < (-pid_params_sp.limit_output))                   // limit output values
    {   
        output_sp = (-pid_params_sp.limit_output);
        pid_params_sp.last_output = output_sp;    // store new output value and shift control errors 
    }

    else if (tmp_output_sp > pid_params_sp.limit_output) 
    {
        output_sp = pid_params_sp.limit_output;
        pid_params_sp.last_output = output_sp;    // store new output value and shift control errors 
    }

    else
    {
        output_sp = tmp_output_sp;
        pid_params_sp.last_output = output_sp;    // store new output value and shift control errors (only if no saturation) 
    }
    
    pid_params_sp.e_n1 = pid_params_sp.e_n0;      // Store error
    pid_params_sp.e_n0 = ctrl_error;              // Store error
    
    pid_params_sp.counter = tmp_output_sp;
 //   ref_input.q = output_sp;//checkerSpeed; //
      ref_input.q = 600;//checkerSpeed; //
}

//*****************************************************************************
//  Iq Regulator
//*****************************************************************************
void pid_control_iq (int32_t ctrl_error) 
{
    int32_t tmp_output_iq;
    int32_t output_iq;
    int32_t tmp_output_p;
    int32_t tmp_output_i;
    int32_t tmp_output_d;
    int32_t tmp_output_error_sum;
        
    //tmp_output_error_sum = pid_params_iq.error_sum + ctrl_error;         // The error is only updated if the control-error is not limited
    tmp_output_error_sum = ctrl_error + pid_params_iq.e_n0 + pid_params_iq.e_n1; 
    
    // Anti Windup effekt
    if (tmp_output_error_sum > pid_params_iq.limit_output) 
        pid_params_iq.error_sum = pid_params_iq.limit_output;
    else if (tmp_output_error_sum < -pid_params_iq.limit_output) 
        pid_params_iq.error_sum = -pid_params_iq.limit_output;
    else
        pid_params_iq.error_sum = tmp_output_error_sum;    
    
    // PID Regulator
//    tmp_output_p = (int32_t)(       (long long)(  (pid_params_iq.p * ctrl_error)>>15)  );
// sc    tmp_output_i = (int32_t)(       (long long)(  (pid_params_iq.i * pid_params_iq.error_sum)>>15)  );
    tmp_output_p = (int32_t)((long long)(  (pid_params_iq.p * ctrl_error)>>12) );
    tmp_output_i = (int32_t)((long long)(  (pid_params_iq.i * pid_params_iq.error_sum)>>12) );
    tmp_output_d = 0;                   
    
    tmp_output_iq =  pid_params_iq.last_output + (tmp_output_p + tmp_output_i + tmp_output_d);
        
    // Limit the output of the PID Regulator
    if (tmp_output_iq < (-pid_params_iq.limit_output))                  // limit output values
    {   
        output_iq = (-pid_params_iq.limit_output);
        pid_params_iq.last_output = output_iq;                          // store new output value 
        
    }
    else if (tmp_output_iq > pid_params_iq.limit_output) 
    {
        output_iq = pid_params_iq.limit_output;
        pid_params_iq.last_output = output_iq;                          // store new output value 
        
    }
    else
    {
        output_iq = tmp_output_iq;
        pid_params_iq.last_output = output_iq;    // store new output value and shift control errors (only if no saturation) 
    }
    
   
    pid_params_iq.e_n1 = pid_params_iq.e_n0;      // Store error
    pid_params_iq.e_n0 = ctrl_error;              // Store error
    
    pid_params_iq.counter = tmp_output_iq;
        
//    des_system_voltages.q = output_iq;
    des_system_voltages.q = 4000;
}

//*****************************************************************************
//  Id Regulator
//*****************************************************************************
void pid_control_id (int32_t ctrl_error) 
{
    int32_t tmp_output_id;
    int32_t output_id;
    int32_t tmp_output_p;
    int32_t tmp_output_i;
    int32_t tmp_output_d;
    int32_t tmp_output_error_sum;
    
    tmp_output_error_sum = pid_params_id.error_sum + ctrl_error;         // The error is only updated if the control-error is not limited
    tmp_output_error_sum = ctrl_error + pid_params_id.e_n0 + pid_params_id.e_n1; 
    
    // Anti Windup effekt
    if (tmp_output_error_sum > pid_params_id.limit_output) 
        pid_params_id.error_sum = pid_params_id.limit_output;
    else if (tmp_output_error_sum < -pid_params_id.limit_output) 
        pid_params_id.error_sum = -pid_params_id.limit_output;
    else
        pid_params_id.error_sum = tmp_output_error_sum;    
    
    // PID Regulator
//    tmp_output_p = (int32_t)(       (long long)(    (pid_params_id.p * ctrl_error)>>15)       );
// sc    tmp_output_i = (int32_t)(       (long long)(    (pid_params_id.i * pid_params_id.error_sum)>>15)        );
    tmp_output_p = (int32_t)(       (long long)( (pid_params_id.p * ctrl_error)>>12) );
    tmp_output_i = (int32_t)(       (long long)( (pid_params_id.i * pid_params_id.error_sum)>>12) );
    tmp_output_d = 0; 
    
    tmp_output_id =  pid_params_id.last_output + ((tmp_output_p + tmp_output_i + tmp_output_d));
    
    
    // Limit the output of the PID Regulator
    if (tmp_output_id < (-pid_params_id.limit_output))                   // limit output values
    {   
        output_id = (-pid_params_id.limit_output);
        pid_params_id.last_output = output_id;    // store new output value and shift control errors 
        
    }
    else if (tmp_output_id > pid_params_id.limit_output) 
    {
        output_id = pid_params_id.limit_output;
        pid_params_id.last_output = output_id;    // store new output value and shift control errors 
    }
    else
    {
        output_id = tmp_output_id;
        pid_params_id.last_output = output_id;    // store new output value and shift control errors (only if no saturation) 
      
    }
    
   
    pid_params_id.e_n1 = pid_params_id.e_n0;      // Store error
    pid_params_id.e_n0 = ctrl_error;              // Store error
    
    pid_params_id.counter = tmp_output_id;
//    des_system_voltages.d = output_id;
    des_system_voltages.d = 0;

}


//*****************************************************************************
//  Regulator for position-mode
//*****************************************************************************
signed short int reg_pos(signed long int des_pos, signed long int act_pos) {
  signed long int reg_out_val, reg_pos_error;
  static signed long int reg_out_val_old;
  static int reg_out_val_I[4];
  static int reg_out_val_loop;
  signed int kp_eff, dyn_sp_max;
  unsigned long int abs_pos_error;
  
  reg_pos_error=(des_pos-act_pos);                                 // calculate actual speed error
  if (reg_pos_error < 0) abs_pos_error = -reg_pos_error;
  else abs_pos_error = reg_pos_error;

  if (abs_pos_error < POS_ANTIHUNT_THRESHOLD2) {
    kp_eff = 0;
  }
  else if (abs_pos_error < POS_ANTIHUNT_THRESHOLD1) {
    kp_eff = pos_kp * (abs_pos_error - POS_ANTIHUNT_THRESHOLD2) / (POS_ANTIHUNT_THRESHOLD1 - POS_ANTIHUNT_THRESHOLD2);
  }
  else {
    kp_eff=pos_kp;
  }

  if (reg_pos_error > 6000) reg_pos_error = 6000;           // saturate error to avoid overflow
  else if (reg_pos_error < -6000) reg_pos_error = -6000;    // speed cannot rise beyond max_rpm or sp_max anyway...

  reg_out_val = (reg_pos_error*kp_eff)/32768;           // calculate proportional term
  
  if (reg_out_val > (reg_out_val_old + SP_REF_MAX_INCREMENT))
      reg_out_val = reg_out_val + SP_REF_MAX_INCREMENT;
       
  if (reg_out_val < (reg_out_val_old - SP_REF_MAX_INCREMENT))
      reg_out_val = reg_out_val - SP_REF_MAX_INCREMENT;

  // if (abs_pos_error < 4*MAX_RPM) dyn_sp_max = abs_pos_error/4;
  // else dyn_sp_max = sp_max;
  dyn_sp_max = sp_max;
  
  if (reg_out_val < (-dyn_sp_max)) {reg_out_val = (-dyn_sp_max);} // do not add up error when output reaches the upper or lower limit
  if (reg_out_val > dyn_sp_max) {reg_out_val = dyn_sp_max;}
  
  reg_out_val_loop = reg_out_val_loop + 1;
  if (reg_out_val_loop == 3)
      reg_out_val_loop = 0;
  
  reg_out_val_I [reg_out_val_loop] = reg_out_val;
  
  reg_out_val = (reg_out_val / 2) + ((reg_out_val_I [0] + reg_out_val_I [1] + reg_out_val_I [2])/6);
  
  reg_out_val_old = reg_out_val;
  return (signed short int) reg_out_val;
}


