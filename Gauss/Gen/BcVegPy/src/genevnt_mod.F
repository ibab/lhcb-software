      SUBROUTINE BCPYTHIA(IPARTIC)
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...PYTHIA common block.
      PARAMETER (MAXNUP=500)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/

C...The user's own transfer of information.
 	COMMON/COLFLOW/AMP2CF(10),SMATVAL

C...COLOR-OCTET.
      COMMON/COLOCT/IOCTET

C...Define number of partons - two incoming and three outgoing.
      NUP=5


      IF(IPARTIC.EQ.21) THEN
C...Set up flavour and history of g + g -> B_c + b + cbar. 
         IDUP(1)= 21            ! Gluon 1
         IDUP(2)= 21            ! Gluon 2
      ELSE
C...Set up flavour and history of q + \bar{q} -> B_c + b + cbar. 
         IF(IPARTIC.EQ.1) THEN
            IDUP(1)= 1          ! u 
            IDUP(2)= -1         ! \bar{u}
         END IF
         IF(IPARTIC.EQ.2) THEN
            IDUP(1)= 2          ! d 
            IDUP(2)= -2         ! \bar{d}
         END IF
         IF(IPARTIC.EQ.3) THEN
            IDUP(1)= 3          ! s 
            IDUP(2)= -3         ! \bar{s}
         END IF
      END IF
     
      IDUP(3)= 541              ! B_c+
      IDUP(4)= 5                ! b-quark
      IDUP(5)=-4                ! c-antiquark

C...Status codes.

      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)= 1
      ISTUP(4)= 1
      ISTUP(5)= 1

C...Mother codes.
      
      MOTHUP(1,1)=0             ! Gluon 1 or q <-- proton 1
      MOTHUP(2,1)=0      
      MOTHUP(1,2)=0             ! Gluon 2 or \bar{q} <-- proton 2
      MOTHUP(2,2)=0      
      MOTHUP(1,3)=1             ! Bc+       or Bc-(hejb)
      MOTHUP(2,3)=2      
      MOTHUP(1,4)=1             ! b         or bbar(hejb)
      MOTHUP(2,4)=2      
      MOTHUP(1,5)=1             !cbar       or c(hejb)
      MOTHUP(2,5)=2

C...SECLECT ONE OF THE THREE POSSIBLE TYPES OF COLOR FLOW AT RANDOM, 
C...ACCORDING TO ITS CONTRIBUTION TO THE SQUARE OF AMPLITUDE.
      RANMAT =SMATVAL*PYR(0)
      ICOLNUM=0

      IF(IPARTIC.EQ.21) THEN
         IUPLIMIT=3
      ELSE
         IUPLIMIT=2
      END IF

      IF(IOCTET.EQ.1) THEN
         IUPLIMIT=10
      END IF

110   ICOLNUM=ICOLNUM+1
      RANMAT =RANMAT-AMP2CF(ICOLNUM)
      IF(ICOLNUM.LT.IUPLIMIT .AND. RANMAT.GT.1.0D-16) GO TO 110

      IF(IPARTIC.EQ.21) THEN
C----------------
         IF(IOCTET.EQ.0) THEN
c...three color flow for color-singlet in the gluon-gluon fusion.
            IF(ICOLNUM.EQ.1) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF
            
            IF(ICOLNUM.EQ.2) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501 
            END IF
            
            IF(ICOLNUM.EQ.3) THEN
               ICOLUP(1,1)=501
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=503
            END IF
         END IF

C------the following is for color-octet and can be found in Ref.
C... hep-ph/0504017. However, in PYTHIA, at the present, there is
c... no color-octet meson, so practically, one might think the
c... color-octet meson will emit a soft gluon with 100% and then 
c... changes to a color singlet state. under such prescription,
c... one may observe that the ten color flow are correspond to
c... three independent flow.
         IF(IOCTET.EQ.1) THEN
c...ten color flow for color-OCTET in the gluon-gluon fusion.
            IF(ICOLNUM.EQ.1) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.2) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.3) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.4) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF
            
            IF(ICOLNUM.EQ.5) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.6) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF
            
            IF(ICOLNUM.EQ.7) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.8) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.9) THEN
               ICOLUP(1,1)=503
               ICOLUP(2,1)=502
               ICOLUP(1,2)=502
               ICOLUP(2,2)=501
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF

            IF(ICOLNUM.EQ.10) THEN
               ICOLUP(1,1)=502
               ICOLUP(2,1)=501
               ICOLUP(1,2)=503
               ICOLUP(2,2)=502
               ICOLUP(1,3)=0
               ICOLUP(2,3)=0        
               ICOLUP(1,4)=503
               ICOLUP(2,4)=0
               ICOLUP(1,5)=0
               ICOLUP(2,5)=501
            END IF
         END IF                 !IOCTET=1

C--------THE FOLLOWING IS THE THEORETICAL COLOR-FLOW FROM 
C--------HEP-PH/0504017.
C      IF(IOCTET.EQ.1) THEN
c...three color flow for color-OCTET in the gluon-gluon fusion.
C	 IF(ICOLNUM.EQ.1) THEN
C        ICOLUP(1,1)=503
C        ICOLUP(2,1)=502
C	  ICOLUP(1,2)=502
C	  ICOLUP(2,2)=501
C        ICOLUP(1,3)=504   ! Bc in color-octet state.
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.2) THEN
C        ICOLUP(1,1)=502
C        ICOLUP(2,1)=501
C	  ICOLUP(1,2)=503
C	  ICOLUP(2,2)=502
C        ICOLUP(1,3)=504
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.3) THEN
C        ICOLUP(1,1)=503
C        ICOLUP(2,1)=502
C	  ICOLUP(1,2)=501
C	  ICOLUP(2,2)=504
C        ICOLUP(1,3)=502
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.4) THEN
C        ICOLUP(1,1)=501
C        ICOLUP(2,1)=504
C	  ICOLUP(1,2)=503
C	  ICOLUP(2,2)=502
C        ICOLUP(1,3)=502
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.5) THEN
C        ICOLUP(1,1)=503
C        ICOLUP(2,1)=502
C	  ICOLUP(1,2)=502
C	  ICOLUP(2,2)=501
C        ICOLUP(1,3)=501
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=504
C       END IF

C	 IF(ICOLNUM.EQ.6) THEN
C        ICOLUP(1,1)=502
C        ICOLUP(2,1)=501
C	  ICOLUP(1,2)=503
C	  ICOLUP(2,2)=502
C        ICOLUP(1,3)=501
C	  ICOLUP(2,3)=503
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=504
C       END IF
C
C	 IF(ICOLNUM.EQ.7) THEN
C        ICOLUP(1,1)=504
C        ICOLUP(2,1)=503
C	  ICOLUP(1,2)=503
C	  ICOLUP(2,2)=502
C        ICOLUP(1,3)=502
C	  ICOLUP(2,3)=501
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.8) THEN
C        ICOLUP(1,1)=503
C        ICOLUP(2,1)=502
C	  ICOLUP(1,2)=504
C	  ICOLUP(2,2)=503
C        ICOLUP(1,3)=502
C	  ICOLUP(2,3)=501
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.9) THEN
C        ICOLUP(1,1)=502
C        ICOLUP(2,1)=501
C	  ICOLUP(1,2)=504
C	  ICOLUP(2,2)=503
C        ICOLUP(1,3)=503
C	  ICOLUP(2,3)=502
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C
C	 IF(ICOLNUM.EQ.10) THEN
C        ICOLUP(1,1)=504
C        ICOLUP(2,1)=503
C	  ICOLUP(1,2)=502
C	  ICOLUP(2,2)=501
C        ICOLUP(1,3)=503
C	  ICOLUP(2,3)=502
C	  ICOLUP(1,4)=504
C        ICOLUP(2,4)=0
C        ICOLUP(1,5)=0
C        ICOLUP(2,5)=501
C       END IF
C	END IF
C-------
      ELSE
c...two color flow for quark-antiquark annihilation.
	 IF(ICOLNUM.EQ.1) THEN
            ICOLUP(1,1)=501
            ICOLUP(2,1)=0
            ICOLUP(1,2)=0
            ICOLUP(2,2)=502
            ICOLUP(1,3)=0
            ICOLUP(2,3)=0
            ICOLUP(1,4)=501
            ICOLUP(2,4)=0
            ICOLUP(1,5)=0
            ICOLUP(2,5)=502
	 END IF

	 IF(ICOLNUM.EQ.2) THEN
            ICOLUP(1,1)=501
            ICOLUP(2,1)=0
            ICOLUP(1,2)=0
            ICOLUP(2,2)=501
            ICOLUP(1,3)=0
            ICOLUP(2,3)=0
            ICOLUP(1,4)=502
            ICOLUP(2,4)=0
            ICOLUP(1,5)=0
            ICOLUP(2,5)=502
	 END IF
      END IF

C.....******************************************************************
C.....To generate Bc-
C.....
C.....Added by hejb
C.....******************************************************************

      IF(PYR(1).GT.0.5D0)THEN
         IDUP(3)=-541           ! B_c-
         IDUP(4)= 4             ! c-quark
         IDUP(5)=-5             ! b-antiquark
      END IF    

      RETURN
      END

C**********************************************************************

      DOUBLE PRECISION FUNCTION TOTFUN(ZUP,PAPAWT)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      IMPLICIT INTEGER (I-N)

C...PYTHIA common block.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      PARAMETER (MAXNUP=500)
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/

C...USER'S TRANSFORMATION.
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
C...TRANSFORM THE BOUND STATE INFORMATION.
      COMMON/COUNTER/IBCSTATE,NEV
      COMMON/RCONST/PI
      COMMON/CONFINE/PTCUT,ETACUT
      COMMON/COLFLOW/AMP2CF(10),SMATVAL

C...PARAMETERS TRANSFORMTION USED IN TOTFUN()
      COMMON/FUNTRANS/NQ2,NPDFU

C...TO GET THE SUBPROCESS CROSS-SECTION.
      COMMON/SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY

C...GENERATE---SWITCH FOR FULL EVENTS.
      LOGICAL GENERATE
      COMMON/GENEFULL/GENERATE
      
C...TO GET THE DISTRIBUTION OF AN EXTRA FACTOR Z=(2(k1+k2).P_BC)/SHAT.
      COMMON/EXTRAZ/ZFACTOR,ZMIN,ZMAX
      COMMON/OUTPDF/IOUTPDF,IPDFNUM
      COMMON/INTINIP/IINIP
      COMMON/INTINIF/IINIF

C...FOR TRANSFORM THE SUBPROCESS INFORMATION, I.E.,  WHETHER USING
C...THE SUBPROCESS q\bar{q}->Bc+b+\bar{c} TO GENERATE EVENTS.
      COMMON/QQBAR/IQQBAR,IQCODE

      DIMENSION XPP(-25:25),XPPBAR(-25:25),ZUP(7),PBOO(4),PC(4),PL(4)
      DATA CONV/3.8938573D+8/ !pb
      COMMON /PPP/ PP(4,40),guv(4)
      INCLUDE 'inclcon.f'

C------------------------------------------------

      TOTFUN=0.0D0

C------------------------------------------------
      IF(ISUBONLY.EQ.1) THEN
         X1=SUBFACTOR
         X2=SUBFACTOR
      ELSE
         TAUMIN =((PMBC+PMB+PMC)/ECM)**2
         TAUMAX =1.0D0
         TAU=(TAUMAX-TAUMIN)*ZUP(6)+TAUMIN
         YYMIN= DLOG(DSQRT(TAU))
         YYMAX=-DLOG(DSQRT(TAU))
         YY   =(YYMAX-YYMIN)*ZUP(7)+YYMIN
         X1 =DSQRT(TAU)*EXP(YY)
         X2 =DSQRT(TAU)*EXP(-YY)
      END IF

C-------------------------------------------------

C... Gluon 1, in LAB
      PUP(1,1)= 0.0D0
      PUP(2,1)= 0.0D0
      PUP(3,1)= ECM*X1/2.0D0
      PUP(4,1)= ECM*X1/2.0D0
      PUP(5,1)= 0.0D0

C... Gluon 2, in LAB
      PUP(1,2)= 0.0D0
      PUP(2,2)= 0.0D0
      PUP(3,2)=-ECM*X2/2.0D0
      PUP(4,2)= ECM*X2/2.0D0
      PUP(5,2)= 0.0D0
  
C...CHANGE MOMTUMA OF THE FINAL PARTICALS INTO LAB COORDINATE SYSTEM.
C...THE ORIGINAL ONE GETTING FROM PHASE_GEN IS RESULT FOR C.M. SYSTEM.
      DO I=1,4
         PBOO(I)=PUP(I,1)+PUP(I,2)
      END DO

      DO 101, I=1,3
         DO J=1,4
            PC(J)=PMOMUP(J,I+2)
         END DO
         CALL LORENTZ(PBOO,PC,PL)
         DO J=1,4
            PMOMUP(J,I+2)=PL(J)
         END DO
 101  CONTINUE

C...Set up kinematics of the out going particles: Bc, b and c~.
      DO I=3,5
         DO J=1,5
	    PUP(J,I)=PMOMUP(J,I)
         END DO
      END DO

C...FOR S-WAVE BOUND STATE, TAKING NON-RELATIVISTIC APPROXIMATION.
      DO I=1,5
         PMOMUP(I,6)=PMB/(PMB+PMC)*PMOMUP(I,3)
         PMOMUP(I,7)=PMC/(PMB+PMC)*PMOMUP(I,3)
      END DO

C...INCOMING GLUON MOMENTA USED IN s_amp.for.
      DO I=1,2
         DO J=1,5
	    PMOMUP(J,I)=PUP(J,I)
         END DO
      END DO

C...THIS PART IS FROM THE INNER PART OF PYTHIA SUBROUTINE PYP().
      PTBC  =DSQRT(PUP(1,3)**2+PUP(2,3)**2)
      PR    =MAX(1.0D-16,PUP(5,3)**2+PUP(1,3)**2+PUP(2,3)**2)
      PRS   =MAX(1.0D-16,PUP(1,3)**2+PUP(2,3)**2)
      ETA   =SIGN(DLOG(MIN((DSQRT(PR+PUP(3,3)**2)+DABS(PUP(3,3)))
     &     /DSQRT(PR),1.0D+20)),PUP(3,3))
      PSETA =SIGN(DLOG(MIN((DSQRT(PRS+PUP(3,3)**2)+DABS(PUP(3,3)))
     &     /DSQRT(PRS),1.0D+20)),PUP(3,3))

C...OTHER CONFINEMENT CAN ALSO BE ADDED HERE.
      IF(PTBC.LT.PTCUT .OR. ABS(ETA).GT.ETACUT) THEN
         IF (GENERATE) THEN
	    DO II=1,10
               AMP2CF(II)=0.0D0
	    END DO
         END IF
         SMATVAL=0.0D0
         RETURN
      END IF

C...ENERGY SCALE.
      IF(NQ2.EQ.1) Q2 =X1*X2*ECM**2/4.0D0
      IF(NQ2.EQ.2) Q2 =X1*X2*ECM**2
      IF(NQ2.EQ.3.OR.NQ2.EQ.8) Q2 =PTBC**2.0D0+PUP(5,3)**2
      IF(NQ2.EQ.4) THEN
         Q=0.0D0
         DO I=3,5
	    Q=Q+DSQRT(PUP(1,I)**2+PUP(2,I)**2+PUP(5,I)**2)
         END DO
         Q2=Q**2
      END IF
      IF(NQ2.EQ.5) THEN
         Q=0.0D0
         DO I=3,5
	    Q=Q+DSQRT(PUP(1,I)**2.0D0+PUP(2,I)**2.0D0+PUP(5,I)**2.0D0)
         END DO
         Q2=(Q/3.0D0)**2.0D0
      END IF
      IF(NQ2.EQ.6.OR.NQ2.EQ.7) THEN
         Q2=PMB**2+PUP(1,4)**2+PUP(2,4)**2
      END IF
C...THIS IS THE ENERGY SCALE USED IN GOUZ'S PROGRAM
      IF(NQ2.EQ.9) THEN
         Q2=4.0D0*PMB**2
      END IF
	
C...GET THE VALUE OF ALPHAS. ALL ARE IN LEADING ORDER.
      IF(IOUTPDF.EQ.1) THEN
         IF(IPDFNUM.EQ.100) ALPS =ALPGRV(Q2,1)*4*PI  
C         IF(IPDFNUM.EQ.200) ALPS =ALPMSRT(DSQRT(Q2),0.220D0,0)
         IF(IPDFNUM.EQ.300) ALPS =ALPCTEQ(Q2,1)*4*PI
      ELSE   
         ALPS =PYALPS(Q2)
      END IF

C...TWO ENERGY SCALE FOR ALPHAS.
C...ALPHAS^4=ALPHAS^2(\mu_b**2)*ALPHAS^2(\mu_c**2).
      IF(NQ2.EQ.6.OR.NQ2.EQ.8) THEN
         ALPS1=ALPS
         IF(IOUTPDF.EQ.1) THEN
            Q2=4.0D0*PMC**2.0D0
            IF(IPDFNUM.EQ.100) ALPS2 =ALPGRV(Q2,1)*4*PI
C            IF(IPDFNUM.EQ.200) ALPS2 =ALPMSRT(DSQRT(Q2),0.22D0,0)
            IF(IPDFNUM.EQ.300) ALPS2 =ALPCTEQ(Q2,1)*4*PI
         ELSE
            ALPS2 =PYALPS(4.0D0*PMC**2.0D0)
         END IF
         ALPS =DSQRT(ALPS1*ALPS2)
      END IF

C...Store scale choice and alphaS.
      SCALUP=DSQRT(Q2)
      AQCDUP=ALPS
      
      IF(ISUBONLY.EQ.0) THEN
	 IF(IOUTPDF.EQ.0) THEN
C...TEVATRON
C...Evaluate Parton distribution for (g1<--p,g2<---p~)
            IF(NPDFU.EQ.1) THEN
               CALL PYPDFU(2212,X1,Q2,XPP)
               CALL PYPDFU(-2212,X2,Q2,XPPBAR)
            END IF
C...LHC
C...Evaluate Parton distribution for (g1<--p,g2<---p)
            IF(NPDFU.EQ.2) THEN
               CALL PYPDFU(2212,X1,Q2,XPP)
               CALL PYPDFU(2212,X2,Q2,XPPBAR)
            END IF
	 ELSE
            IF(IPDFNUM.EQ.100) THEN
               CALL GRV98PA(1, X1, Q2, UV, DV, US, DS, SS, GL1)
               CALL GRV98PA(1, X2, Q2, UV, DV, US, DS, SS, GL2)
               IF(IQQBAR.EQ.0) THEN
                  XPP(21)=GL1
                  XPPBAR(21)=GL2
               ELSE
                  IF(IQCODE.EQ.1) THEN
                     IF(NPDFU.EQ.1) THEN !TEVATRON
                        XPP(IQCODE)=UV+US
                        XPP(-IQCODE)=US
                        XPPBAR(IQCODE)=UV+US
                        XPPBAR(-IQCODE)=US
		     END IF
                     IF(NPDFU.EQ.2) THEN !LHC
                        XPP(IQCODE)=UV+US
                        XPP(-IQCODE)=US
                        XPPBAR(IQCODE)=US
                        XPPBAR(-IQCODE)=UV+US
		     END IF
                  END IF
                  IF(IQCODE.EQ.2) THEN
                     IF(NPDFU.EQ.1) THEN !TEVATRON (u-P,~u-~P AND u-~P,~u-p)
                        XPP(IQCODE)=DV+DS
                        XPP(-IQCODE)=DS
                        XPPBAR(IQCODE)=DV+DS
                        XPPBAR(-IQCODE)=DS
		     END IF
                     IF(NPDFU.EQ.2) THEN !LHC (u-P,~u-P AND u-P,~u-p)
                        XPP(IQCODE)=DV+DS
                        XPP(-IQCODE)=DS
                        XPPBAR(IQCODE)=DS
                        XPPBAR(-IQCODE)=DV+DS
		     END IF
                  END IF
                  IF(IQCODE.EQ.3) THEN ! THE SAME FOR TEVATRON OR LHC
		     XPP(3)=2.0D0*SS
		     XPPBAR(3)=2.0D0*SS
                  END IF
               END IF
            END IF
            IF(IPDFNUM.EQ.300) THEN
               QQ=DSQRT(Q2)
C...  CTEQ6L.
               IF(IQQBAR.EQ.0) THEN
                  XPP(21)   =Ctq6Pdf(0,X1,QQ)
                  XPPBAR(21)=Ctq6Pdf(0,X2,QQ)
               ELSE
                  IF(NPDFU.EQ.1) THEN !TEVATRON
		     XPP(IQCODE)=Ctq6Pdf(IQCODE,X1,QQ)
		     XPP(-IQCODE)=Ctq6Pdf(-IQCODE,X1,QQ)
		     XPPBAR(IQCODE)=Ctq6Pdf(IQCODE,X2,QQ)
		     XPPBAR(-IQCODE)=Ctq6Pdf(-IQCODE,X2,QQ)
                  END IF
                  IF(NPDFU.EQ.2) THEN !LHC
		     XPP(IQCODE)=Ctq6Pdf(IQCODE,X1,QQ)
		     XPP(-IQCODE)=Ctq6Pdf(-IQCODE,X1,QQ)
		     XPPBAR(IQCODE)=Ctq6Pdf(-IQCODE,X2,QQ)
		     XPPBAR(-IQCODE)=Ctq6Pdf(IQCODE,X2,QQ)
                  END IF
               END IF
            END IF
	 END IF
      END IF	

C...THIS ENSURE THE RIGHTNESS OF THE EXTRAPOLATION OF THE PDF.
c...(BY USING THE PDFs, SOMETIMES IT WILL GET NEGATIVE VALUE)
      IF(XPP(21).LT.1.0D-16) XPP(21)=0.0D0
      IF(XPPBAR(21).LT.1.0D-16) XPPBAR(21)=0.0D0
      
      IF(IQQBAR.EQ.1) THEN
	 IF(XPP(IQCODE).LT.1.0D-16) XPP(IQCODE)=0.0D0
	 IF(XPPBAR(IQCODE).LT.1.0D-16) XPPBAR(IQCODE)=0.0D0
	 IF(XPP(-IQCODE).LT.1.0D-16) XPP(-IQCODE)=0.0D0
	 IF(XPPBAR(-IQCODE).LT.1.0D-16) XPPBAR(-IQCODE)=0.0D0
      END IF  

C...FOR THE SUB-PROCESS, TAKING THE CONSTANT ALPHAS VALUE.
      IF(ISUBONLY.EQ.1) ALPS=0.20D0
      
C...IF NOT GENERATE S-WAVE, GO TO THE PART FOR P-WAVE.
      IF(IBCSTATE.EQ.3) GOTO 1005
      IF(IBCSTATE.EQ.4) GOTO 1005
      IF(IBCSTATE.EQ.5) GOTO 1005
      IF(IBCSTATE.EQ.6) GOTO 1005

C...COMMON FACTOR FOR S-WAVE STATES.
      PHASE =PAPAWT*ALPS**4.0D0/(2.0D0**11*PI*3.0D0*DOTUP(1,2))

C...FIRST TO GET THE SQUARE OF THE AMPLITUDE.
C...NOTE 1) THE MOMENTA INPUTED INTO THIS SUBROUTINE IS PMOMUP(J,I):
C...Bc+: I=3, b: I=4, ~c: I=5; 
C...J=1: p_x; J=2: p_y; J=3: p_z; J=4: E; J=5: mass;	2) ALL THE MOMENTA
C...NOW ARE IN THE LAB SYSTEM, YOU MAY DIRECTLY GET THE MOMENTA IN C.M.S
C...OF GLUON-GLUON SUBSYSTEM BEFORE RUNNING LORENTZ TRANSFORMATION USED
C...ABOVE. THE CROSS-SECTION WILL NOT CHANGE UNDER THE CORDINATE 
C...TRANSFORMATION. 3) SIGSCL AND SIGVCT, AFTER CALLING THIS SUBROUTINE,
C...IS NOT THE FINAL CROSS-SECTION, WE NEED TO ADD SOME COEFFICIENTS.
C...THERE WE DON'T NEED AN EXTRA Q IN Xsection_BCY, BECAUSE 4) THE MOMENTA
C...ARE NOW ALSO STORED IN PUP(J,I), WHICH IS TRANSFORMED ACCORDING TO 
C...A PYTHIA COMMON BLOCK.
      CALL XSECTION(SIGSCL,SIGVCT)


C...GET THE RIGHT PHASE FOR THE SUBPROCESS: q+~q->Bc+b+~c FROM
C...THE SAME PRECEDURE OF SUBPROCESS g+g->Bc+b+~c.
      IF(IQQBAR.EQ.1) THEN
         PHASE=PHASE*(2.0D0**6/3.0D0**2)
      END IF

C...THE CORRECT CROSS-SECTION FOR THE SUBPROCESS TO A PARTICULAR PARTICLE
C...MOMENTA. 
      SIGSCL=CONV*PHASE*SIGSCL
      SIGVCT=CONV*PHASE*SIGVCT
      
      IF(IBCSTATE.EQ.1) SIGCROSS=SIGSCL
      IF(IBCSTATE.EQ.2) SIGCROSS=SIGVCT

C...GET THE CROSS-SECTION FOR S-WAVE.	
      IF(ISUBONLY.EQ.0) THEN
         IF(IQQBAR.EQ.0) THEN
            TOTFUN =SIGCROSS*XPP(21)*XPPBAR(21)/X1/X2
            IF(IOUTPDF.EQ.1 .AND. IPDFNUM.EQ.300) THEN
               TOTFUN=SIGCROSS*XPP(21)*XPPBAR(21)
            END IF
         ELSE
            TOTFUN =SIGCROSS*(XPP(IQCODE)*XPPBAR(IQCODE)+
     &           XPP(-IQCODE)*XPPBAR(-IQCODE))/X1/X2
            IF(IOUTPDF.EQ.1 .AND. IPDFNUM.EQ.300) THEN
               TOTFUN =SIGCROSS*XPP(IQCODE)*XPPBAR(IQCODE)
	    END IF
         END IF
      ELSE
         TOTFUN =SIGCROSS
      END IF

C...GETTING AN EXTRA DISTRIBUTION ABOUT Z=(2(k1+k2).P_BC)/SHAT.
      ZFACTOR=2.0D0*(DOTUP(1,3)+DOTUP(2,3))/(X1*X2*ECM**2.0D0)
      
C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES.
      IF(TOTFUN.LT.1.0D-16) TOTFUN=1.0D-16
      
      RETURN

C************************************************
C...THE FOLLOWING IS FOR THE P-WAVE STATES.
C************************************************

 1005 CONTINUE

      IF(IBCSTATE.NE.1.AND.IBCSTATE.NE.2) THEN
C...THE P-WAVE PART IS ADOPTED FROM FDC PROGRAM AND HERE
C...IS TO MAKE ALL THE VARIABLE CONSISTENT TO FDC.
C...IN THE LAB SYSTEM. 
C...PP(1,I)=E=PUP(4,I); 
C...PP((2,3,4),I)=(P_X,P_Y,P_Z)=PUP((1,2,3),I).
C...PP(4)=PUP(5)--B QUARK; PP(5)=PUP(4)--\BAR{C} QUARK.
	 DO I=1,3
            PP(2,I)=PUP(1,I)
            PP(3,I)=PUP(2,I)
            PP(4,I)=PUP(3,I)
            PP(1,I)=PUP(4,I)
	 END DO

	 PP(2,4)=PUP(1,5)
	 PP(3,4)=PUP(2,5)
	 PP(4,4)=PUP(3,5)
	 PP(1,4)=PUP(4,5)
         
	 PP(2,5)=PUP(1,4)
	 PP(3,5)=PUP(2,4)
	 PP(4,5)=PUP(3,4)
	 PP(1,5)=PUP(4,4)

C...THIS IS AN OVERALL COLOR FACTOR, WHERE THE AVERAGE OVER
C...THE INITIAL GLUONS HAVE BEEN INCLUDED, I.E. THE FACTOR
C...(1/2**6) HAS BEEN INCLUDED INTO CFACT, I.E.
C...   cfact=(33.0d0/2.0d0)/(2.0d0**6)
	 CFACT=0.2578125D0

C...THIS IS TO GET CONSTANTs FOR THE GLUON COUPLING CONSTANT G.
	 G3=DSQRT(4.0D0*PI*ALPS)

	 IF(IBCSTATE.EQ.3) THEN
	    AMPOFPW=amps2_1p1()
	    PHASE=PAPAWT*CFACT*G3**8/(2.0D0**9*PI**5*DOTUP(1,2))
	 END IF
	 IF(IBCSTATE.EQ.4) THEN
	    AMPOFPW=amps2_3p0()
	    PHASE=PAPAWT*CFACT*G3**8/(2.0D0**9*PI**5*DOTUP(1,2))
	 END IF
	 IF(IBCSTATE.EQ.5) THEN
	    AMPOFPW=amps2_3p1()
	    PHASE=PAPAWT*CFACT*G3**8/(2.0D0**9*PI**5*DOTUP(1,2))
	 END IF
	 IF(IBCSTATE.EQ.6) THEN
	    AMPOFPW=amps2_3p2()
	    PHASE=PAPAWT*CFACT*G3**8/(2.0D0**9*PI**5*DOTUP(1,2))
	 END IF
C...GET THE CROSS-SECTIN FOR P-WAVE.
	 IF(ISUBONLY.EQ.1) THEN
	   TOTFUN=CONV*PHASE*AMPOFPW
	 ELSE
            TOTFUN =CONV*PHASE*AMPOFPW*XPP(21)*XPPBAR(21)/X1/X2
            IF(IOUTPDF.EQ.1 .AND. IPDFNUM.EQ.300) THEN
               TOTFUN=CONV*PHASE*AMPOFPW*XPP(21)*XPPBAR(21)
            END IF
	 END IF
      END IF

C...GETTING AN EXTRA DISTRIBUTION ABOUT Z=(2(k1+k2).P_BC)/SHAT.
      ZFACTOR=2.0D0*(DOTUP(1,3)+DOTUP(2,3))/(X1*X2*ECM**2.0D0)

C...THE FOLLOWING IS ONLY TO ELIMINATE THE NUMERICAL UNCERNTAINTY,
C...WHICH IN PRINCIPLE DOES NOT NEEDED. HOWEVER WE ADDED HERE 
C...TO AVOID SOME VERY PARTICULAR CASES.
      IF(TOTFUN.LT.1.0D-16) TOTFUN=1.0D-16

      RETURN
      END

C*********************************************************************

C...GENERATE ALLOWED PHASE_SPACE AND RIGHT KINEMATICAL RANGES(TO ENSURE
C...THE SUBPROCESS HAS ENOUGH ENERGY).
      SUBROUTINE PHPOINT(ZUP,WT)
C...Preamble: declarations.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...PYTHIA COMMON BLOCK.
      PARAMETER (MAXNUP=500)
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/


      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)

C...TO GET THE SUBPROCESS CROSS-SECTION.
      COMMON/SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY

      DIMENSION ZUP(7),ZZUP(5)

C...INITIAL VALUE.
      WT=0.0D0

C--------------------------------------------
C****CONSTRAINTS FROM THE KINEMATICAL REGION.

      TAUMIN =((PMBC+PMB+PMC)/ECM)**2
      TAUMAX =1.0D0
      
C------------------------------------------

      IF(ISUBONLY.EQ.1) THEN
         X1=SUBFACTOR
         X2=SUBFACTOR
         IF((X1*X2).LT.TAUMIN) THEN
            WRITE(*,'(A)') 'ENERGY NOT HIGH ENOUGH!'
            STOP
         END IF
      ELSE
         TAU  = (TAUMAX-TAUMIN)*ZUP(6)+TAUMIN
         YYMIN= DLOG(DSQRT(TAU))
         YYMAX=-DLOG(DSQRT(TAU))
         YY   = (YYMAX-YYMIN)*ZUP(7)+YYMIN
         X1=DSQRT(TAU)*EXP(YY)
         X2=DSQRT(TAU)*EXP(-YY)
      END IF

C...IN THE C.M. SYSTEM.
      PUP(1,1) = 0.0D0
      PUP(2,1) = 0.0D0
      PUP(3,1) = DSQRT(X1*X2)*ECM/2.0D0
      PUP(4,1) = DSQRT(X1*X2)*ECM/2.0D0
      PUP(5,1) = 0.0D0

      PUP(1,2) = 0.0D0
      PUP(2,2) = 0.0D0
      PUP(3,2) =-DSQRT(X1*X2)*ECM/2.0D0
      PUP(4,2) = DSQRT(X1*X2)*ECM/2.0D0
      PUP(5,2) = 0.0D0

C----------------------------------------------

      DO I=1,5
         ZZUP(I)=ZUP(I)
      END DO
      
C...CALCULATE THE JACOBIAN OF THE FINAL PARTICLES IN C.M. FRAME.
      ET =PUP(4,1)+PUP(4,2)

      IZERO=0

C...INCREASE THE PHASE-SPACE EFFICIENCY.
 321  CALL PHASE_GEN(ZZUP,ET,WT)
      IF((WT .LT. 1.0D-16).AND.(IZERO.LT.100000))THEN
         IZERO=IZERO+1
         GOTO 321
      END IF

C--------------------------------------------

      IF(ISUBONLY.EQ.1) THEN
         RETURN
      ELSE
         WT=(TAUMAX-TAUMIN)*(YYMAX-YYMIN)*WT
         RETURN
      END IF

C...AFTER CALLING PHASE_GEN ALL THE PARTICLE'S MOMENTA ARE 
C...CONSTRUCTED EFFECTIVELY, WHICH IS IN THE C.M.S.
      
      END
      
