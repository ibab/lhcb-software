

	SUBROUTINE AR3JET(S,X1,X3,JL)
	IMPLICIT REAL*8(A-H,O-Z)
C     
	COMMON/HIPARNT/HIPR1(100),IHPR2(50),HINT1(100),IHNT2(50)
	SAVE  /HIPARNT/
	COMMON/HILUJETS/N,NPAD,K(9000,5),P(9000,5),V(9000,5)
	SAVE  /HILUJETS/
C     
	C=1.d0/3.d0
      	IF(K(JL,2).NE.21 .AND. K(JL+1,2).NE.21) C=8./27.
      	EXP1=3
      	EXP3=3
      	IF(K(JL,2).NE.21) EXP1=2
      	IF(K(JL+1,2).NE.21) EXP3=2
      	A=0.24d0**2/S
      	YMA=LOG(.5d0/SQRT(A)+SQRT(.25d0/A-1.d0))
      	D=4.d0*C*YMA
      	SM1=P(JL,5)**2/S
      	SM3=P(JL+1,5)**2/S
      	XT2M=(1.d0-2.d0*SQRT(SM1)+SM1-SM3)*(1.d0-2.d0*SQRT(SM3)-SM1+SM3)
      	XT2M=MIN(.25d0,XT2M)
      	NTRY=0
1     	IF(NTRY.EQ.5000) THEN
	        X1=.5d0*(2.d0*SQRT(SM1)+1.d0+SM1-SM3)
		X3=.5d0*(2.d0*SQRT(SM3)+1.d0-SM1+SM3)
		RETURN
      	ENDIF
      	NTRY=NTRY+1
     
      	XT2=A*(XT2M/A)**(hijing_RAN(0)**(1.d0/D))
     
      	YMAX=LOG(.5d0/SQRT(XT2)+SQRT(.25d0/XT2-1.d0))
      	Y=(2.d0*hijing_RAN(0)-1.)*YMAX
      	X1=1.d0-SQRT(XT2)*EXP(Y)
      	X3=1.d0-SQRT(XT2)*EXP(-Y)
      	X2=2.d0-X1-X3
      	NEG=0
      	IF(K(JL,2).NE.21 .OR. K(JL+1,2).NE.21) THEN
        IF((1.d0-X1)*(1.d0-X2)*(1.d0-X3)-X2*SM1*(1.d0-X1)-X2*SM3*
     &  (1.d0-X3).
     &  LE.0.d0.OR.X1.LE.2.d0*SQRT(SM1)-SM1+SM3.OR.X3.LE.2.d0*SQRT(SM3)
     &  -SM3+SM1) NEG=1
        X1=X1+SM1-SM3
        X3=X3-SM1+SM3
     	ENDIF
      	IF(NEG.EQ.1) GOTO 1
     
      	FG=2.d0*YMAX*C*(X1**EXP1+X3**EXP3)/D
      	XT2M=XT2
      	IF(FG.LT.hijing_RAN(0)) GOTO 1
     
      	RETURN
      	END
