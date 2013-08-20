==========================================================================
                 Template Project for MB9BF51xN
==========================================================================
                  (C) 2012 FUJITSU SEMICONDUCTOR LIMITED                
                                                  
   The following software deliverable is intended for and must only be  
   used for reference and in an evaluation laboratory environment.      
   It is provided on an as-is basis without charge and is subject to    
   alterations.                                                         
   It is the user's obligation to fully test the software in its        
   environment and to ensure proper functionality, qualification and    
   compliance with component specifications.                            
                                                  
   In the event the software deliverable includes the use of open       
   source components, the provisions of the governing open source       
   license agreement shall apply with respect to such software          
   deliverable.                                                         
   FUJITSU SEMICONDUCTOR LIMITED ("FSL")
   does not warrant that the deliverables do not infringe any       
   third party intellectual property right (IPR). In the event that     
   the deliverables infringe a third party IPR it is the sole           
   responsibility of the customer to obtain necessary licenses to       
   continue the usage of the deliverable.                               
                                                  
   To the maximum extent permitted by applicable law FSL disclaims all  
   warranties, whether express or implied, in particular, but not       
   limited to, warranties of merchantability and fitness for a          
   particular purpose for which the deliverable is not designated.      
                                                  
   To the maximum extent permitted by applicable law, FSL's liability   
   is restricted to intention and gross negligence.                     
   FSL is not liable for consequential damages.                         
 
   (V1.0)                                                    
==========================================================================
History
Date       Ver AE   Description
2011-11-25 1.0 EH   Initial
2012-02-24 1.1 EH   Updated IO definition
2012-04-25 1.2 EH   -Library Options of EWARM change to "Auto"
                    -Updated bit band alias of RTC in IO definition
2012-06-07 1.3 EH   Updated IO definition
                    Modified system_mb9bf51x.c in common folder
2012-08-23 1.4 EH   Updated IO definition
                    Changed flashloader of EWARM to IAR made
                    Added Workflash flashloader
2013-03-15 1.5 CCa  Project configuration added for Atollic
                    Updated IO definition


Supported toolchain versions

IAR        6.40.1
ARM        4.54
Atollic    3.2.0

==========================================================================


This is a project template for MB9BF51xN. It includes some
basic settings for e.g. Linker, C-Compiler.



Clock settings:
---------------
Crystal:  4 MHz
HCLK:   144 MHz
PCLK0:   72 MHz
PCLK1:   72 MHz
PCLK2:   72 MHz

