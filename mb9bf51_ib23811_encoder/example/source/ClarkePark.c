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
#include "QPRC.h"


#if (SINTAB_SIZE == 500) 
  #include "sincostab.h"
#elif (SINTAB_SIZE == 1000)
  #include "sincostab1k.h"
#elif (SINTAB_SIZE == 2000)
  #include "sincostab2k.h"
#elif (SINTAB_SIZE == 2048)
  #include "sincostab2048.h"
#elif (SINTAB_SIZE == 4000)
  #include "sincostab4k.h"
#elif (SINTAB_SIZE == 160)
  #include "sincostab160.h"


#else
  #error No valid sinetable size defined!
#endif



//*****************************************************************************
// Forward Clarke Transform for two measurements of three phase currents:
// input:  pointer to stator current structure containing Ia and Ib (Ic is implicitly included by Ia+Ib+Ic=0)
// output: writes two-axis orthogonal components to stator current structure
//*****************************************************************************
void clarke_fwd()
{
     act_system_currents.alpha = act_system_currents.u;
     act_system_currents.beta = (int32_t)((int64_t)( (((act_system_currents.u + 2 * act_system_currents.v) * ONE_OVER_SQRT3_Q12) >> 12)));
//   act_statsys_currents.i_alpha = act_statsys_currents.i_u;
//    act_statsys_currents.i_beta = (long)(((act_statsys_currents.i_u + 2 * act_statsys_currents.i_v) * ONE_OVER_SQRT3_Q12) >> 12);
}
/*
void clarke_fwd()
{
    act_system_currents.alpha = act_system_currents.u;
    act_system_currents.beta = (int32_t)( (int64_t)((((act_system_currents.u + 2 * act_system_currents.v) * ONE_OVER_SQRT3_Q15) >> 15)));
}
*/
//*****************************************************************************
// inverse Clarke Transform
// input:  pointer to stator reference voltage structure containing the two-axis component vector
// output: writes symmetric 3-axis components to stator reference voltage structure
//*****************************************************************************
/*
void clarke_inv() 
{
    des_system_voltages.u =  des_system_voltages.alpha;
    des_system_voltages.v = -des_system_voltages.alpha / 2 + (int32_t)((int64_t)((des_system_voltages.beta * SQRT3HALF_Q15) >> 15) );
    des_system_voltages.w = -des_system_voltages.alpha / 2 - (int32_t)((int64_t)((des_system_voltages.beta * SQRT3HALF_Q15) >> 15) );
}
*/
void clarke_inv() 
{
    des_system_voltages.u =  des_system_voltages.alpha;
    des_system_voltages.v = -des_system_voltages.alpha / 2 + (int32_t)((int64_t)((des_system_voltages.beta * SQRT3HALF_Q12) >> 12) );
    des_system_voltages.w = -des_system_voltages.alpha / 2 - (int32_t)((int64_t)((des_system_voltages.beta * SQRT3HALF_Q12) >> 12) );

//    ref_voltages.v_a = ref_voltages.v_alpha;
//    ref_voltages.v_b = -ref_voltages.v_alpha / 2 + (long)((long long)(ref_voltages.v_beta * SQRT3HALF_Q12) >> 12 );
//    ref_voltages.v_c = -ref_voltages.v_alpha / 2 - (long)((long long)(ref_voltages.v_beta * SQRT3HALF_Q12) >> 12 );
}

//*****************************************************************************
// Forward Park transform, moves measured values from 2-axis stator to rotor coordinate system
// input: stator current structure, pointer to rotor current structure, rotor/d-axis angle
// output: writes to rotor current structure
//*****************************************************************************
void park_fwd() 
{
    int16_t rotor_angle_helper;

    get_rotor_angle();
 
    if (rotor_angle > 4095 )
      rotor_angle_helper = (rotor_angle -4095) >> 1;
    else
      rotor_angle_helper = rotor_angle >> 1; 

//     rotor_angle_helper = rotor_angle >> 2; 
//    int16_t SinCalc = sin_tableQ15[rotor_angle];
//    int16_t CosCalc = cos_tableQ15[rotor_angle];
    int16_t SinCalc = sin_tableQ15[rotor_angle_helper];
    int16_t CosCalc = cos_tableQ15[rotor_angle_helper];
    
      
 //   act_rotsys_currents.d = (long)(((long long)( act_statsys_currents.i_alpha * CosAngle) + (long long)(act_statsys_currents.i_beta * SinAngle))>>12);
 //   act_rotsys_currents.q = (long)(((long long)(-act_statsys_currents.i_alpha * SinAngle) + (long long)(act_statsys_currents.i_beta * CosAngle))>>12);
     
    act_system_currents.d = (int32_t)(    (int64_t)((( act_system_currents.alpha * CosCalc) + (int64_t)(act_system_currents.beta * SinCalc))>>12));
    act_system_currents.q = (int32_t)(    (int64_t)(((-act_system_currents.alpha * SinCalc) + (int64_t)(act_system_currents.beta * CosCalc))>>12));

}

//*****************************************************************************
// Inverse Park transform, moves reference values from rotor to stator 2-axis coordinate system
// input: stator current structure, pointer to rotor current structure, rotor/d-axis angle
// output: writes to rotor current structure
//*****************************************************************************
void park_inv() 
{
    int16_t rotor_angle_helper;
    int16_t SinCalc;
    int16_t CosCalc;
    
    get_rotor_angle();
    
    if (motor_state == ENC_INIT)
    {
        // rotor_angle_helper = fixed_angle;
    if (fixed_angle > 4095 )
      rotor_angle_helper = (fixed_angle -4095) >> 1;
    else
      rotor_angle_helper = fixed_angle >> 1; 
  
//       rotor_angle_helper = fixed_angle >>2; // SC map into 2048 table
        des_system_voltages.q = 0;
        des_system_voltages.d = startup_ampl;     // fixed ref. voltage during startup (no regulator active)
    }        
    
    else
    {
        //rotor_angle_helper = rotor_angle;       
    if (rotor_angle > 4095 )
      rotor_angle_helper = (rotor_angle -4095) >> 1;
    else
      rotor_angle_helper = rotor_angle >> 1; 

// sc     rotor_angle_helper = rotor_angle >>2;       // sc map into 2048 table
    }
    
    SinCalc = sin_tableQ15[rotor_angle_helper];
    CosCalc = cos_tableQ15[rotor_angle_helper];    
  
    //printf (" %d, %d, %d,\n", rotor_angle_helper, SinCalc, CosCalc);  
    
   des_system_voltages.alpha = (int32_t)((int64_t)(   ((des_system_voltages.d * CosCalc) - (des_system_voltages.q * SinCalc))>>12));
   des_system_voltages.beta  = (int32_t)((int64_t)(   ((des_system_voltages.d * SinCalc) + (des_system_voltages.q * CosCalc))>>12));
//    ref_voltages.alpha = (long)(((ref_voltages.v_d * CosAngle) - (ref_voltages.v_q * SinAngle))>>12);
//   ref_voltages.beta  = (long)(((ref_voltages.v_d * SinAngle) + (ref_voltages.v_q * CosAngle))>>12);
}
//   des_system_voltages.alpha = (int32_t)(       (int64_t)(   ((des_system_voltages.d * CosCalc) - (des_system_voltages.q * SinCalc))>>15));
//   des_system_voltages.beta  = (int32_t)(       (int64_t)(   ((des_system_voltages.d * SinCalc) + (des_system_voltages.q * CosCalc))>>15));
/*
void SinCalc (int angle)
{
    //int angleHelper;
    
    if ( angle >= 0 )
        {
        /* get rid of multiples of 360 deg *
        while ( angle > ( ( SINTAB_SIZE ) -1 ) )
            {
            angle -= ( ( SINTAB_SIZE ) -1 );
            }
        }   
    else
        {
        while ( angle < 0 )
            {
            angle += ( ( SINTAB_SIZE ) -1 );
            }
        }
    
    
    // evaluate look up table *
	if ( angle < HALF_PI_SINTAB_SIZE ) // first quadrant
        {
		SinAngle = sin_tableQ15[ angle ];
        }
	else if ( angle >= HALF_PI_SINTAB_SIZE && angle < ( 2 * HALF_PI_SINTAB_SIZE ) ) //* second quadrant 
        {
		SinAngle = sin_tableQ15[ ( ( 2 * HALF_PI_SINTAB_SIZE ) - 1 ) - angle ];
        }
	else if ( angle >= ( 2 * HALF_PI_SINTAB_SIZE ) && angle < ( 3 * HALF_PI_SINTAB_SIZE ) ) //* third quadrant 
        {
		SinAngle = -1 * sin_tableQ15[ angle - ( 2 * HALF_PI_SINTAB_SIZE ) ];
        }
	else if ( angle >= ( 3 * HALF_PI_SINTAB_SIZE ) && angle <= ( ( 4 * HALF_PI_SINTAB_SIZE ) - 1 ) ) /* fourth quadrant 
        {
		SinAngle = -1 * sin_tableQ15[ ( ( 4 * HALF_PI_SINTAB_SIZE ) - 1 ) - angle ];
        }
    
    
}

void CosCalc (int angle)
{
  angle += HALF_PI_SINTAB_SIZE; 
  
  if ( angle >= 0 )
        {
        /* get rid of multiples of 360 deg 
        while ( angle > ( ( SINTAB_SIZE ) -1 ) )
            {
            angle -= ( ( SINTAB_SIZE ) -1 );
            }
        }   
    else
        {
        while ( angle < 0 )
            {
            angle += ( ( SINTAB_SIZE ) -1 );
            }
        }
    
    
    /* evaluate look up table *
	if ( angle < HALF_PI_SINTAB_SIZE ) // first quadrant
        {
		CosAngle = sin_tableQ15[ angle ];
        }
	else if ( angle >= HALF_PI_SINTAB_SIZE && angle < ( 2 * HALF_PI_SINTAB_SIZE ) ) /* second quadrant *
        {
		CosAngle = sin_tableQ15[ ( ( 2 * HALF_PI_SINTAB_SIZE ) - 1 ) - angle ];
        }
	else if ( angle >= ( 2 * HALF_PI_SINTAB_SIZE ) && angle < ( 3 * HALF_PI_SINTAB_SIZE ) ) /* third quadrant *
        {
		CosAngle = -1 * sin_tableQ15[ angle - ( 2 * HALF_PI_SINTAB_SIZE ) ];
        }
	else if ( angle >= ( 3 * HALF_PI_SINTAB_SIZE ) && angle <= ( ( 4 * HALF_PI_SINTAB_SIZE ) - 1 ) ) /* fourth quadrant *
        {
		CosAngle = -1 * sin_tableQ15[ ( ( 4 * HALF_PI_SINTAB_SIZE ) - 1 ) - angle ];
        }
}
     

*/


