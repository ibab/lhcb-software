    
C***********************************************************************    
    
      FUNCTION PYHIW2AU(EPS,IREIM)    
      IMPLICIT REAL*8(A-H,O-Z)
C...Calculates real and imaginary parts of the auxiliary function W2;   
C...see R. K. Ellis, I. Hinchliffe, M. Soldate and J. J. van der Bij,   
C...FERMILAB-Pub-87/100-T, LBL-23504, June, 1987    
      COMMON/HILUDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200) 
      SAVE /HILUDAT1/ 
    
      ASINH(X)=DLOG(X+DSQRT(X**2+1.)) 
      ACOSH(X)=DLOG(X+DSQRT(X**2-1.)) 
    
      IF(EPS.LT.0.) THEN    
        W2RE=4.*(ASINH(DSQRT(-1./EPS)))**2   
        W2IM=0. 
      ELSEIF(EPS.LT.1.) THEN    
        W2RE=4.*(ACOSH(DSQRT(1./EPS)))**2-PARU(1)**2 
        W2IM=-4.*PARU(1)*ACOSH(DSQRT(1./EPS))    
      ELSE  
        W2RE=-4.*(ASIN(DSQRT(1./EPS)))**2    
        W2IM=0. 
      ENDIF 
    
      IF(IREIM.EQ.1) PYHIW2AU=W2RE    
      IF(IREIM.EQ.2) PYHIW2AU=W2IM    
    
      RETURN    
      END   
