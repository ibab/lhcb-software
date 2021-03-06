    
C*********************************************************************  
    
      SUBROUTINE PYHIKCUT(MCUT)   
      IMPLICIT REAL*8(A-H,O-Z)
C...Dummy routine, which the user can replace in order to make cuts on  
C...the kinematics on the parton level before the matrix elements are   
C...evaluated and the event is generated. The cross-section estimates   
C...will automatically take these cuts into account, so the given   
C...values are for the allowed phase space region only. MCUT=0 means    
C...that the event has passed the cuts, MCUT=1 that it has failed.  
      COMMON/PYHIPARS/MSTP(200),PARP(200),MSTI(200),PARI(200) 
      SAVE /PYHIPARS/ 
    
      MCUT=0    
    
      RETURN    
      END   
