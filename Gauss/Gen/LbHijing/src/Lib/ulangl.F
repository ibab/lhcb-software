    
C*********************************************************************  
    
      FUNCTION ULANGL(X,Y)  
      IMPLICIT REAL*8(A-H,O-Z)
C...Purpose: to reconstruct an angle from given x and y coordinates.    
      COMMON/HILUDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200) 
      SAVE /HILUDAT1/ 
    
      ULANGL=0. 
      R=DSQRT(X**2+Y**2) 
      IF(R.LT.1E-20) RETURN 
      IF(DABS(X)/R.LT.0.8) THEN  
        ULANGL=SIGN(ACOS(X/R),Y)    
      ELSE  
        ULANGL=DASIN(Y/R)    
        IF(X.LT.0..AND.ULANGL.GE.0.) THEN   
          ULANGL=PARU(1)-ULANGL 
        ELSEIF(X.LT.0.) THEN    
          ULANGL=-PARU(1)-ULANGL    
        ENDIF   
      ENDIF 
    
      RETURN    
      END   
