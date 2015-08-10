*=======================================================================
* Decay of a massive barion according to PYTHIA. This routine replaces
* the original routine of ZLPAIR generator which is, probably,
* out-of-date.
* Thanks to T.Sjostrand et al.!
*=======================================================================
      subroutine zmdiff(Mother)
 
C...Handles diffractive and elastic scattering.
      COMMON/LUJETS/N,K(4000,5),P(4000,5),V(4000,5)
      COMMON/LUDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      SAVE /LUJETS/,/LUDAT1/
      SAVE /PYPARS/,/PYINT1/
      DOUBLE PRECISION DBETAZ

      common /ZMDIFFCUT/ dMmin
      data dMmin/1./ ! min mass difference for fragmetation

c   particle KF and mass
      KFH=K(Mother,2)
      PMMAS=P(Mother,5)
c
c   filling MINT,VINT...
      MINT(83)=Mother	! mother's line number
      vint(1)=p(Mother,4)
      Ps=0.
      do j=1,3
	Ps=Ps+P(Mother,j)**2
      enddo
      if(Ps.ne.0) then
        VINT(23)=P(Mother,3)/sqrt(Ps)                ! cos(theta)
        VINT(59)=sqrt((P(Mother,1)**2+P(Mother,2)**2)/Ps) ! sin(theta)
	if(P(Mother,1).ne.0.or.P(Mother,2).ne.0.) then
          VINT(24)=atan2(P(Mother,2),P(Mother,1))         ! phi
        else
	  VINT(24)=0.
	endif
      else
	VINT(23)=1.
	VINT(59)=0.
	VINT(24)=0.
      endif
C...Subprocess; kinematics.
      PZ=p(Mother,3)
      PE=p(Mother,4)

!      print *,'ZMDIFF',Mother,N,PMMAS

      do 200 jt=1,1

      I=MINT(83)	! mother's line number
!     PE=(VINT(2)+VINT(62+JT)-VINT(65-JT))/(2.*VINT(1))
 
C...Diffracted particle: low-mass system to two particles.
      if(PMMAS.lt.ULMASS(KFH)+dMmin) then
	do Ni=N+1,N+2
          do j=1,5
            K(Ni,j)=0
            P(Ni,j)=0.
            V(Ni,j)=0.
          enddo
        enddo
        N=N+2
        K(N-1,1)=1
        K(N,1)=1
        K(N-1,3)=I
        K(N,3)=I
        NTRY=0
  150   NTRY=NTRY+1
        IF(NTRY.LT.20) THEN
          MINT(105)=kfh
          MINT(109)=0
          CALL PYSPLI(KFH,21,KFL1,KFL2)
          CALL LUKFDI(KFL1,0,KFL3,KF1)
          IF(KF1.EQ.0) GOTO 150
          CALL LUKFDI(KFL2,-KFL3,KFLDUM,KF2)
          IF(KF2.EQ.0) GOTO 150
        ELSE
          KF1=KFH
          KF2=111
          if(ntry.gt.20) then
             PMMAS=PM1+PM2+PARJ(64)
             P(Mother,5)=PMMAS
             P(Mother,4)=sqrt(PMMAS**2+Ps)
          endif
        ENDIF
        PM1=ULMASS(KF1)
        PM2=ULMASS(KF2)
        IF(PM1+PM2+PARJ(64).GT.PMMAS) GOTO 150
        K(N-1,2)=KF1
        K(N,2)=KF2
        P(N-1,5)=PM1
        P(N,5)=PM2
        PZP=SQRT(MAX(0.,(PMMAS**2-PM1**2-PM2**2)**2-4.*PM1**2*PM2**2))/
     &  (2.*PMMAS)
        P(N-1,3)=PZP
        P(N,3)=-PZP
        P(N-1,4)=SQRT(PM1**2+PZP**2)
        P(N,4)=SQRT(PM2**2+PZP**2)
        
!        print *,'ZMDIFF',I,N

        CALL LUDBRB(N-1,N,ACOS(2.*RLU(0)-1.),PARU(2)*RLU(0),0D0,0D0,0D0)
        DBETAZ=DBLE(PZ)*(-1)**(JT+1)/SQRT(DBLE(PZ)**2+DBLE(PMMAS)**2)
        CALL LUDBRB(N-1,N,0.,0.,0D0,0D0,DBETAZ)
 
C...Diffracted particle: valence quark kicked out.
      elseif(Kqg.eq.0) then
	do Ni=N+1,N+2
          do j=1,5
            K(Ni,j)=0
            P(Ni,j)=0.
            V(Ni,j)=0.
          enddo
        enddo
        N=N+2
        K(N-1,1)=2
        K(N,1)=1
        K(N-1,3)=I+2
        K(N,3)=I+2
        MINT(105)=kfh
        MINT(109)=0
        CALL PYSPLI(KFH,21,K(N,2),K(N-1,2))
        P(N-1,5)=ULMASS(K(N-1,2))
        P(N,5)=ULMASS(K(N,2))
        SQLAM=(PMMAS**2-P(N-1,5)**2-P(N,5)**2)**2-
     &  4.*P(N-1,5)**2*P(N,5)**2
        P(N-1,3)=(PE*SQRT(SQLAM)+PZ*(PMMAS**2+P(N-1,5)**2-
     &  P(N,5)**2))/(2.*PMMAS**2)*(-1)**(JT+1)
        P(N-1,4)=SQRT(P(N-1,3)**2+P(N-1,5)**2)
        P(N,3)=PZ*(-1)**(JT+1)-P(N-1,3)
        P(N,4)=SQRT(P(N,3)**2+P(N,5)**2)
 
C...Diffracted particle: gluon kicked out.
      ELSE
	do Ni=N+1,N+3
          do j=1,5
            K(Ni,j)=0
            P(Ni,j)=0.
            V(Ni,j)=0.
          enddo
        enddo
        N=N+3
        K(N-2,1)=2
        K(N-1,1)=2
        K(N,1)=1
        K(N-2,3)=I+2
        K(N-1,3)=I+2
        K(N,3)=I+2
        MINT(105)=kfh
        MINT(109)=0
        CALL PYSPLI(KFH,21,K(N,2),K(N-2,2))
        K(N-1,2)=21
        P(N-2,5)=ULMASS(K(N-2,2))
        P(N-1,5)=0.
        P(N,5)=ULMASS(K(N,2))
C...Energy distribution for particle into two jets.
  160   IMB=1
        IF(MOD(KFH/1000,10).NE.0) IMB=2
        CHIK=PARP(92+2*IMB)
        IF(MSTP(92).LE.1) THEN
          IF(IMB.EQ.1) CHI=RLU(0)
          IF(IMB.EQ.2) CHI=1.-SQRT(RLU(0))
        ELSEIF(MSTP(92).EQ.2) THEN
          CHI=1.-RLU(0)**(1./(1.+CHIK))
        ELSEIF(MSTP(92).EQ.3) THEN
          CUT=2.*0.3/VINT(1)
  170     CHI=RLU(0)**2
          IF((CHI**2/(CHI**2+CUT**2))**0.25*(1.-CHI)**CHIK.LT.
     &    RLU(0)) GOTO 170
        ELSEIF(MSTP(92).EQ.4) THEN
          CUT=2.*0.3/VINT(1)
          CUTR=(1.+SQRT(1.+CUT**2))/CUT
  180     CHIR=CUT*CUTR**RLU(0)
          CHI=(CHIR**2-CUT**2)/(2.*CHIR)
          IF((1.-CHI)**CHIK.LT.RLU(0)) GOTO 180
        ELSE
          CUT=2.*0.3/VINT(1)
          CUTA=CUT**(1.-PARP(98))
          CUTB=(1.+CUT)**(1.-PARP(98))
  190     CHI=(CUTA+RLU(0)*(CUTB-CUTA))**(1./(1.-PARP(98)))
          IF(((CHI+CUT)**2/(2.*(CHI**2+CUT**2)))**
     &    (0.5*PARP(98))*(1.-CHI)**CHIK.LT.RLU(0)) GOTO 190
        ENDIF
        IF(CHI.LT.P(N,5)**2/PMMAS**2.OR.CHI.GT.1.-P(N-2,5)**2/
     &  PMMAS**2) GOTO 160
        SQM=P(N-2,5)**2/(1.-CHI)+P(N,5)**2/CHI
        IF((SQRT(SQM)+PARJ(32))**2.GE.PMMAS**2) GOTO 160
        PZI=(PE*(PMMAS**2-SQM)+PZ*(PMMAS**2+SQM))/
     &  (2.*PMMAS**2)
        PEI=SQRT(PZI**2+SQM)
        PQQP=(1.-CHI)*(PEI+PZI)
        P(N-2,3)=0.5*(PQQP-P(N-2,5)**2/PQQP)*(-1)**(JT+1)
        P(N-2,4)=SQRT(P(N-2,3)**2+P(N-2,5)**2)
        P(N-1,4)=0.5*(PMMAS**2-SQM)/(PEI+PZI)
        P(N-1,3)=P(N-1,4)*(-1)**JT
        P(N,3)=PZI*(-1)**(JT+1)-P(N-2,3)
        P(N,4)=SQRT(P(N,3)**2+P(N,5)**2)
      ENDIF
 
C...Documentation lines.
!      K(I+2,1)=21
!      IF(MINT(16+JT).EQ.0) K(I+2,2)=KFH
!      IF(MINT(16+JT).NE.0) K(I+2,2)=10*(KFH/10)
!      K(I+2,3)=I
!      P(I+2,3)=PZ*(-1)**(JT+1)
!      P(I+2,4)=PE
!      P(I+2,5)=SQRT(PMMAS**2)
  200 CONTINUE
 
C...Rotate outgoing partons/particles using cos(theta).
      IF(VINT(23).LT.0.9) THEN
        CALL LUDBRB(MINT(83)+1,N,ACOS(VINT(23)),VINT(24),0D0,0D0,0D0)
      ELSE
        CALL LUDBRB(MINT(83)+1,N,ASIN(VINT(59)),VINT(24),0D0,0D0,0D0)
      ENDIF
 
!      call LUEXEC
      END
