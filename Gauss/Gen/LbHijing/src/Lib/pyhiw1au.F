    
C***********************************************************************    
    
      FUNCTION PYHIW1AU(EPS,IREIM)    
      IMPLICIT REAL*8(A-H,O-Z)
C...Calculates real and imaginary parts of the auxiliary function W1;   
C...see R. K. Ellis, I. Hinchliffe, M. Soldate and J. J. van der Bij,   
C...FERMILAB-Pub-87/100-T, LBL-23504, June, 1987    
      COMMON/HILUDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200) 
      SAVE /HILUDAT1/ 
    
      ASINH(X)=DLOG(X+DSQRT(X**2+1.)) 
      ACOSH(X)=DLOG(X+DSQRT(X**2-1.)) 
    
      IF(EPS.LT.0.) THEN    
        W1RE=2.*DSQRT(1.-EPS)*ASINH(DSQRT(-1./EPS))   
        W1IM=0. 
      ELSEIF(EPS.LT.1.) THEN    
        W1RE=2.*DSQRT(1.-EPS)*ACOSH(DSQRT(1./EPS))    
        W1IM=-PARU(1)*DSQRT(1.-EPS)  
      ELSE  
        W1RE=2.*DSQRT(EPS-1.)*DASIN(DSQRT(1./EPS)) 
        W1IM=0. 
      ENDIF 
    
      IF(IREIM.EQ.1) PYHIW1AU=W1RE    
      IF(IREIM.EQ.2) PYHIW1AU=W1IM    
    
      RETURN    
      END   
