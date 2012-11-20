c# 1 "dilog64.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "dilog64.F"
*
* $Id: dilog64.F,v 1.1.1.1 1996/04/01 15:02:05 mclareni Exp $
*
* $Log: dilog64.F,v $
* Revision 1.1.1.1  1996/04/01 15:02:05  mclareni
* Mathlib wj_gen
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h" 1
























c# 40 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"

c# 57 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"



























































c# 10 "dilog64.F" 2

      FUNCTION wj_DDILOG(X)
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/imp64.inc" 1
*
* $Id: imp64.inc,v 1.1.1.1 1996/04/01 15:02:59 mclareni Exp $
*
* $Log: imp64.inc,v $
* Revision 1.1.1.1  1996/04/01 15:02:59  mclareni
* Mathlib wj_gen
*
*
* imp64.inc
*







      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

c# 13 "dilog64.F" 2




      DIMENSION C(0:19)

      PARAMETER (Z1 = 1, HF = Z1/2)
      PARAMETER (PI = 3.14159 26535 89793 24D0)
      PARAMETER (PI3 = PI**2/3, PI6 = PI**2/6, PI12 = PI**2/12)

      DATA C( 0) / 0.42996 69356 08136 97D0/
      DATA C( 1) / 0.40975 98753 30771 05D0/
      DATA C( 2) /-0.01858 84366 50145 92D0/
      DATA C( 3) / 0.00145 75108 40622 68D0/
      DATA C( 4) /-0.00014 30418 44423 40D0/
      DATA C( 5) / 0.00001 58841 55418 80D0/
      DATA C( 6) /-0.00000 19078 49593 87D0/
      DATA C( 7) / 0.00000 02419 51808 54D0/
      DATA C( 8) /-0.00000 00319 33412 74D0/
      DATA C( 9) / 0.00000 00043 45450 63D0/
      DATA C(10) /-0.00000 00006 05784 80D0/
      DATA C(11) / 0.00000 00000 86120 98D0/
      DATA C(12) /-0.00000 00000 12443 32D0/
      DATA C(13) / 0.00000 00000 01822 56D0/
      DATA C(14) /-0.00000 00000 00270 07D0/
      DATA C(15) / 0.00000 00000 00040 42D0/
      DATA C(16) /-0.00000 00000 00006 10D0/
      DATA C(17) / 0.00000 00000 00000 93D0/
      DATA C(18) /-0.00000 00000 00000 14D0/
      DATA C(19) /+0.00000 00000 00000 02D0/

      IF(X .EQ. 1) THEN
       H=PI6
      ELSEIF(X .EQ. -1) THEN
       H=-PI12
      ELSE
       T=-X
       IF(T .LE. -2) THEN
        Y=-1/(1+T)
        S=1
        A=-PI3+HF*(LOG(-T)**2-LOG(1+1/T)**2)
       ELSEIF(T .LT. -1) THEN
        Y=-1-T
        S=-1
        A=LOG(-T)
        A=-PI6+A*(A+LOG(1+1/T))
       ELSE IF(T .LE. -HF) THEN
        Y=-(1+T)/T
        S=1
        A=LOG(-T)
        A=-PI6+A*(-HF*A+LOG(1+T))
       ELSE IF(T .LT. 0) THEN
        Y=-T/(1+T)
        S=-1
        A=HF*LOG(1+T)**2
       ELSE IF(T .LE. 1) THEN
        Y=T
        S=1
        A=0
       ELSE
        Y=1/T
        S=-1
        A=PI6+HF*LOG(T)**2
       ENDIF
       H=Y+Y-1
       wj_alfa=H+H
       B1=0
       B2=0
       DO 1 I = 19,0,-1
       B0=C(I)+wj_alfa*B1-B2
       B2=B1
    1  B1=B0
       H=-(S*(B0-H*B2)+A)
      ENDIF

      wj_DDILOG=H




      RETURN
      END


*
* $Id: wj_DZERO.F,v 1.1.1.1 1996/02/15 17:49:07 mclareni Exp $
*
* $Log: wj_DZERO.F,v $
* Revision 1.1.1.1  1996/02/15 17:49:07  mclareni
* Kernlib
*










































      SUBROUTINE wj_DZERO(A,B,X0,R,EPS,MXF,F)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
*
* $Id: c205body.inc,v 1.1.1.1 1996/02/15 17:49:07 mclareni Exp $
*
* $Log: c205body.inc,v $
* Revision 1.1.1.1  1996/02/15 17:49:07  mclareni
* Kernlib
*
*


*
*
* c205body.inc
*
      LOGICAL MFLAG,RFLAG
      EXTERNAL F
 
      PARAMETER (ONE = 1, HALF = ONE/2)
 
      XA=MIN(A,B)
      XB=MAX(A,B)
      FA=F(XA,1)
      FB=F(XB,2)
      IF(FA*FB .GT. 0) GO TO 5
      MC=0
 
    1 X0=HALF*(XA+XB)
      R=X0-XA
      EE=EPS*(ABS(X0)+1)
      IF(R .LE. EE) GO TO 4
      F1=FA
      X1=XA
      F2=FB
      X2=XB
 
    2 FX=F(X0,2)
      MC=MC+1
      IF(MC .GT. MXF) GO TO 6
      IF(FX*FA .GT. 0) THEN
       XA=X0
       FA=FX
      ELSE
       XB=X0
       FB=FX
      END IF
 
    3 U1=F1-F2
      U2=X1-X2
      U3=F2-FX
      U4=X2-X0
      IF(U2 .EQ. 0 .OR. U4 .EQ. 0) GO TO 1
      F3=FX
      X3=X0
      U1=U1/U2
      U2=U3/U4
      CA=U1-U2
      CB=(X1+X2)*U2-(X2+X0)*U1
      CC=(X1-X0)*F1-X1*(CA*X1+CB)
      IF(CA .EQ. 0) THEN
       IF(CB .EQ. 0) GO TO 1
       X0=-CC/CB
      ELSE
       U3=CB/(2*CA)
       U4=U3*U3-CC/CA
       IF(U4 .LT. 0) GO TO 1
       X0=-U3+SIGN(SQRT(U4),X0+U3)
      END IF
      IF(X0 .LT. XA .OR. X0 .GT. XB) GO TO 1
 
      R=MIN(ABS(X0-X3),ABS(X0-X2))
      EE=EPS*(ABS(X0)+1)
      IF(R .GT. EE) THEN
       F1=F2
       X1=X2
       F2=F3
       X2=X3
       GO TO 2
      END IF
 
      FX=F(X0,2)
      IF(FX .EQ. 0) GO TO 4
      IF(FX*FA .LT. 0) THEN
       XX=X0-EE
       IF(XX .LE. XA) GO TO 4
       FF=F(XX,2)
       FB=FF
       XB=XX
      ELSE
       XX=X0+EE
       IF(XX .GE. XB) GO TO 4
       FF=F(XX,2)
       FA=FF
       XA=XX
      END IF
      IF(FX*FF .GT. 0) THEN
       MC=MC+2
       IF(MC .GT. MXF) GO TO 6
       F1=F3
       X1=X3
       F2=FX
       X2=X0
       X0=XX
       FX=FF
       GO TO 3
      END IF
 
    4 R=EE
      FF=F(X0,3)
      RETURN
    5 CALL wj_KERMTR('C205.1',LGFILE,MFLAG,RFLAG)
      IF(MFLAG) THEN
       IF(LGFILE .EQ. 0) WRITE(*,100)
       IF(LGFILE .NE. 0) WRITE(LGFILE,100)
      END IF
      IF(.NOT.RFLAG) CALL wj_ABEND
      R=-2*(XB-XA)
      X0=0
      RETURN
    6 CALL wj_KERMTR('C205.2',LGFILE,MFLAG,RFLAG)
      IF(MFLAG) THEN
       IF(LGFILE .EQ. 0) WRITE(*,101)
       IF(LGFILE .NE. 0) WRITE(LGFILE,101)
      END IF
      IF(.NOT.RFLAG) CALL wj_ABEND
      R=-HALF*ABS(XB-XA)
      X0=0
      RETURN


  100 FORMAT(1X,'***** CERN C205 wj_DZERO ... F(A) AND F(B)',
     1          ' HAVE THE SAME SIGN')
  101 FORMAT(1X,'***** CERN C205 wj_DZERO ... TOO MANY FUNCTION CALLS')
      END


c# 1 "wj_KERSET.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_KERSET.F"
*
* $Id: wj_KERSET.F,v 1.1.1.1 1996/02/15 17:48:35 mclareni Exp $
*
* $Log: wj_KERSET.F,v $
* Revision 1.1.1.1  1996/02/15 17:48:35  mclareni
* Kernlib
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/kernnum/pilot.h" 1
c# 21 "/usr/local/home/video/cernlib/2005/include/kernnum/pilot.h"

c# 33 "/usr/local/home/video/cernlib/2005/include/kernnum/pilot.h"

c# 10 "wj_KERSET.F" 2
          SUBROUTINE wj_KERSET(ERCODE,LGFILE,LIMITM,LIMITR)
                    PARAMETER(KOUNTE  =  27)
          CHARACTER*6         ERCODE,   CODE(KOUNTE)
          LOGICAL             MFLAG,    RFLAG
          INTEGER             KNTM(KOUNTE),       KNTR(KOUNTE)
          DATA      LOGF      /  0  /
          DATA      CODE(1), KNTM(1), KNTR(1)  / 'C204.1', 255, 255 /
          DATA      CODE(2), KNTM(2), KNTR(2)  / 'C204.2', 255, 255 /
          DATA      CODE(3), KNTM(3), KNTR(3)  / 'C204.3', 255, 255 /
          DATA      CODE(4), KNTM(4), KNTR(4)  / 'C205.1', 255, 255 /
          DATA      CODE(5), KNTM(5), KNTR(5)  / 'C205.2', 255, 255 /
          DATA      CODE(6), KNTM(6), KNTR(6)  / 'C305.1', 255, 255 /
          DATA      CODE(7), KNTM(7), KNTR(7)  / 'C308.1', 255, 255 /
          DATA      CODE(8), KNTM(8), KNTR(8)  / 'C312.1', 255, 255 /
          DATA      CODE(9), KNTM(9), KNTR(9)  / 'C313.1', 255, 255 /
          DATA      CODE(10),KNTM(10),KNTR(10) / 'C336.1', 255, 255 /
          DATA      CODE(11),KNTM(11),KNTR(11) / 'C337.1', 255, 255 /
          DATA      CODE(12),KNTM(12),KNTR(12) / 'C341.1', 255, 255 /
          DATA      CODE(13),KNTM(13),KNTR(13) / 'D103.1', 255, 255 /
          DATA      CODE(14),KNTM(14),KNTR(14) / 'D106.1', 255, 255 /
          DATA      CODE(15),KNTM(15),KNTR(15) / 'D209.1', 255, 255 /
          DATA      CODE(16),KNTM(16),KNTR(16) / 'D509.1', 255, 255 /
          DATA      CODE(17),KNTM(17),KNTR(17) / 'E100.1', 255, 255 /
          DATA      CODE(18),KNTM(18),KNTR(18) / 'E104.1', 255, 255 /
          DATA      CODE(19),KNTM(19),KNTR(19) / 'E105.1', 255, 255 /
          DATA      CODE(20),KNTM(20),KNTR(20) / 'E208.1', 255, 255 /
          DATA      CODE(21),KNTM(21),KNTR(21) / 'E208.2', 255, 255 /
          DATA      CODE(22),KNTM(22),KNTR(22) / 'F010.1', 255,   0 /
          DATA      CODE(23),KNTM(23),KNTR(23) / 'F011.1', 255,   0 /
          DATA      CODE(24),KNTM(24),KNTR(24) / 'F012.1', 255,   0 /
          DATA      CODE(25),KNTM(25),KNTR(25) / 'F406.1', 255,   0 /
          DATA      CODE(26),KNTM(26),KNTR(26) / 'G100.1', 255, 255 /
          DATA      CODE(27),KNTM(27),KNTR(27) / 'G100.2', 255, 255 /
          LOGF  =  LGFILE
             L  =  0
          IF(ERCODE .NE. ' ')  THEN
             DO 10  L = 1, 6
                IF(ERCODE(1:L) .EQ. ERCODE)  GOTO 12
  10            CONTINUE
  12         CONTINUE
          ENDIF
          DO 14     I  =  1, KOUNTE
             IF(L .EQ. 0)  GOTO 13
             IF(CODE(I)(1:L) .NE. ERCODE(1:L))  GOTO 14
  13         IF(LIMITM.GE.0) KNTM(I)  =  LIMITM
             IF(LIMITR.GE.0) KNTR(I)  =  LIMITR
  14         CONTINUE
          RETURN
          ENTRY wj_KERMTR(ERCODE,LOG,MFLAG,RFLAG)
          LOG  =  LOGF
          DO 20     I  =  1, KOUNTE
             IF(ERCODE .EQ. CODE(I))  GOTO 21
  20         CONTINUE
          WRITE(*,1000)  ERCODE
          CALL wj_ABEND
          RETURN
  21      RFLAG  =  KNTR(I) .GE. 1
          IF(RFLAG  .AND.  (KNTR(I) .LT. 255))  KNTR(I)  =  KNTR(I) - 1
          MFLAG  =  KNTM(I) .GE. 1
          IF(MFLAG  .AND.  (KNTM(I) .LT. 255))  KNTM(I)  =  KNTM(I) - 1
          IF(.NOT. RFLAG)  THEN
             IF(LOGF .LT. 1)  THEN
                WRITE(*,1001)  CODE(I)
             ELSE
                WRITE(LOGF,1001)  CODE(I)
             ENDIF
          ENDIF
          IF(MFLAG .AND. RFLAG)  THEN
             IF(LOGF .LT. 1)  THEN
                WRITE(*,1002)  CODE(I)
             ELSE
                WRITE(LOGF,1002)  CODE(I)
             ENDIF
          ENDIF
          RETURN
1000      FORMAT(' KERNLIB LIBRARY ERROR. ' /
     +           ' ERROR CODE ',A6,' NOT RECOGNIZED BY wj_KERMTR',
     +           ' ERROR MONITOR. RUN ABORTED.')
1001      FORMAT(/' ***** RUN TERMINATED BY CERN LIBRARY ERROR ',
     +           'CONDITION ',A6)
1002      FORMAT(/' ***** CERN LIBRARY ERROR CONDITION ',A6)
          END

c# 1 "wj_RM48.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_RM48.F"
*
* $Id: wj_RM48.F,v 1.2 1996/12/12 16:32:06 cernlib Exp $
*
* $Log: wj_RM48.F,v $
* Revision 1.2  1996/12/12 16:32:06  cernlib
* Variables ONE and ZERO added to SAVE statement, courtesy R.Veenhof
*
* Revision 1.1.1.1  1996/04/01 15:02:55  mclareni
* Mathlib wj_gen
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h" 1
























c# 40 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"

c# 57 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"



























































c# 13 "wj_RM48.F" 2
      SUBROUTINE wj_RM48(RVEC,LENV)
C     Double-precision version of
C Universal wj_random number generator proposed by Marsaglia and Zaman
C in report FSU-SCRI-87-50
C        based on RANMAR, modified by F. James, to generate vectors
C        of pseudorandom numbers RVEC of length LENV, where the numbers
C        in RVEC are numbers with at least 48-bit mantissas.
C   Input and output entry points: wj_RM48IN, RM48UT.
C!!! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C!!!  Calling sequences for wj_RM48:                                    ++
C!!!      CALL wj_RM48 (RVEC, LEN)     returns a vector RVEC of LEN     ++
C!!!                   64-bit wj_random floating point numbers between  ++
C!!!                   zero and one.                                 ++
C!!!      CALL wj_RM48IN(I1,N1,N2)   initializes the generator from one ++
C!!!                   64-bit integer I1, and number counts N1,N2    ++
C!!!                  (for initializing, set N1=N2=0, but to restart ++
C!!!                    a previously generated sequence, use values  ++ 
C!!!                    output by wj_RM48UT)                            ++ 
C!!!      CALL wj_RM48UT(I1,N1,N2)   outputs the value of the original  ++
C!!!                  wj_SEED and the two number counts, to be used     ++
C!!!                  for restarting by initializing to I1 and       ++  
C!!!                  skipping N2*100000000+N1 numbers.              ++
C!!! ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C for 32-bit machines, use IMPLICIT DOUBLE PRECISION
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION RVEC(*)
      COMMON/wj_R48ST1/U(97),C,I97,J97
      PARAMETER (MODCNS=1000000000)
      SAVE CD, CM, TWOM24, NTOT, NTOT2, IJKL,TWOM49, ONE, ZERO
      DATA NTOT,NTOT2,IJKL/-1,0,0/
C
      IF (NTOT .GE. 0)  GO TO 50
C
C        Default initialization. User has called wj_RM48 without RM48IN.
      IJKL = 54217137
      NTOT = 0
      NTOT2 = 0
      KALLED = 0
      GO TO 1
C
      ENTRY      wj_RM48IN(IJKLIN, NTOTIN,NTOT2N)
C         Initializing routine for wj_RM48, may be called before
C         generating pseudorandom numbers with wj_RM48.   The input
C         values should be in the ranges:  0<=IJKLIN<=900 OOO OOO
C                                          0<=NTOTIN<=999 999 999
C                                          0<=NTOT2N<<999 999 999!
C To get the standard values in Marsaglia's paper, IJKLIN=54217137
C                                            NTOTIN,NTOT2N=0
      IJKL = IJKLIN
      NTOT = MAX(NTOTIN,0)
      NTOT2= MAX(NTOT2N,0)
      KALLED = 1
C          always come here to initialize
    1 CONTINUE
      IJ = IJKL/30082
      KL = IJKL - 30082*IJ
      I = MOD(IJ/177, 177) + 2
      J = MOD(IJ, 177)     + 2
      K = MOD(KL/169, 178) + 1
      L = MOD(KL, 169)
      WRITE(6,'(A,I10,2X,2I10)') ' wj_RM48 INITIALIZED:',IJKL,NTOT,NTOT2
CCC      PRINT '(A,4I10)', '   I,J,K,L= ',I,J,K,L
      ONE = 1.
      HALF = 0.5
      ZERO = 0.
      DO 2 II= 1, 97
      S = 0.
      T = HALF
      DO 3 JJ= 1, 48
         M = MOD(MOD(I*J,179)*K, 179)
         I = J
         J = K
         K = M
         L = MOD(53*L+1, 169)
         IF (MOD(L*M,64) .GE. 32)  S = S+T
    3    T = HALF*T
    2 U(II) = S
      TWOM49 = T
      TWOM24 = ONE
      DO 4 I24= 1, 24
    4 TWOM24 = HALF*TWOM24
      C  =   362436.*TWOM24
      CD =  7654321.*TWOM24
      CM = 16777213.*TWOM24
      I97 = 97
      J97 = 33
C       Complete initialization by skipping
C            (NTOT2*MODCNS + NTOT) wj_random numbers
      DO 45 LOOP2= 1, NTOT2+1
      NOW = MODCNS
      IF (LOOP2 .EQ. NTOT2+1)  NOW=NTOT
      IF (NOW .GT. 0)  THEN
      WRITE(6,'(A,I15)') ' wj_RM48IN SKIPPING OVER ',NOW
          DO 40 IDUM = 1, NTOT
          UNI = U(I97)-U(J97)
          IF (UNI .LT. ZERO)  UNI=UNI+ONE
          U(I97) = UNI
          I97 = I97-1
          IF (I97 .EQ. 0)  I97=97
          J97 = J97-1
          IF (J97 .EQ. 0)  J97=97
          C = C - CD
          IF (C .LT. ZERO)  C=C+CM
   40     CONTINUE
      ENDIF
   45 CONTINUE
      IF (KALLED .EQ. 1)  RETURN
C
C          Normal entry to generate LENV wj_random numbers
   50 CONTINUE
      DO 100 IVEC= 1, LENV
      UNI = U(I97)-U(J97)
      IF (UNI .LT. ZERO)  UNI=UNI+ONE
      U(I97) = UNI
      I97 = I97-1
      IF (I97 .EQ. 0)  I97=97
      J97 = J97-1
      IF (J97 .EQ. 0)  J97=97
      C = C - CD
      IF (C .LT. ZERO)  C=C+CM
      UNI = UNI-C
      IF (UNI .LT. ZERO) UNI=UNI+ONE
      RVEC(IVEC) = UNI
C             Replace exact zeros by 2**-49
         IF (UNI .EQ. ZERO)  THEN
            RVEC(IVEC) = TWOM49
         ENDIF
  100 CONTINUE
      NTOT = NTOT + LENV
         IF (NTOT .GE. MODCNS)  THEN
         NTOT2 = NTOT2 + 1
         NTOT = NTOT - MODCNS
         ENDIF
      RETURN
C           Entry to output current status
      ENTRY wj_RM48UT(IJKLUT,NTOTUT,NTOT2T)
      IJKLUT = IJKL
      NTOTUT = NTOT
      NTOT2T = NTOT2
      RETURN
      END


c# 1 "wj_ABEND.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_ABEND.F"
*
* $Id: wj_ABEND.F,v 1.1.1.1 1996/02/15 17:50:37 mclareni Exp $
*
* $Log: wj_ABEND.F,v $
* Revision 1.1.1.1  1996/02/15 17:50:37  mclareni
* Kernlib
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h" 1
c# 94 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h"





c# 10 "wj_ABEND.F" 2





      SUBROUTINE wj_ABEND
C
C CERN PROGLIB# Z035    wj_ABEND           .VERSION KERNFOR  4.31  911111
C ORIG.  8/02/88  JZ
C

      STOP  7
      END
c# 1 "wj_LENOCC.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_LENOCC.F"
*
* $Id: wj_LENOCC.F,v 1.1.1.1 1996/02/15 17:49:49 mclareni Exp $
*
* $Log: wj_LENOCC.F,v $
* Revision 1.1.1.1  1996/02/15 17:49:49  mclareni
* Kernlib
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h" 1
c# 94 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h"





c# 10 "wj_LENOCC.F" 2
      FUNCTION wj_LENOCC (CHV)
C
C CERN PROGLIB# M507    wj_LENOCC          .VERSION KERNFOR  4.21  890323
C ORIG. March 85, A.Petrilli, re-write 21/02/89, JZ
C
C-    Find last non-blank character in CHV

      CHARACTER    CHV*(*)

      N = LEN(CHV)

      DO 17  JJ= N,1,-1
      IF (CHV(JJ:JJ).NE.' ') GO TO 99
   17 CONTINUE
      JJ = 0

   99 wj_LENOCC = JJ
      RETURN
      END
c# 1 "wj_MTLSET.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_MTLSET.F"
*
* $Id: wj_MTLSET.F,v 1.1.1.1 1996/04/01 15:02:53 mclareni Exp $
*
* $Log: wj_MTLSET.F,v $
* Revision 1.1.1.1  1996/04/01 15:02:53  mclareni
* Mathlib wj_gen
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h" 1
























c# 40 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"

c# 57 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"



























































c# 10 "wj_MTLSET.F" 2
      SUBROUTINE wj_MTLSET(ERC,NLG,MXM,MXR)

      PARAMETER (KTE = 132)
      CHARACTER*6 ERC,CODE(KTE)
      LOGICAL LMF,LRF
      DIMENSION KNTM(KTE),KNTR(KTE)

      DATA ILG /0/

C     renumber the data statements after putting new codes in Unix with:
C     awk -F'[()]' '{ printf"%s(%s)%s(%s)%s(%s)%s\n",$1,NR,$3,NR,$5,NR,$7 }'
C     and modify KTE to the number of lines below

      DATA CODE(1),KNTM(1),KNTR(1) / 'B100.1', 255, 255 /
      DATA CODE(2),KNTM(2),KNTR(2) / 'B300.1', 255, 255 /
      DATA CODE(3),KNTM(3),KNTR(3) / 'B300.2', 255, 255 /
      DATA CODE(4),KNTM(4),KNTR(4) / 'C200.0', 255, 255 /
      DATA CODE(5),KNTM(5),KNTR(5) / 'C200.1', 255, 255 /
      DATA CODE(6),KNTM(6),KNTR(6) / 'C200.2', 255, 255 /
      DATA CODE(7),KNTM(7),KNTR(7) / 'C200.3', 255, 255 /
      DATA CODE(8),KNTM(8),KNTR(8) / 'C201.0', 255, 255 /
      DATA CODE(9),KNTM(9),KNTR(9) / 'C202.0', 255, 255 /
      DATA CODE(10),KNTM(10),KNTR(10) / 'C202.1', 255, 255 /
      DATA CODE(11),KNTM(11),KNTR(11) / 'C202.2', 255, 255 /
      DATA CODE(12),KNTM(12),KNTR(12) / 'C205.1', 255, 255 /
      DATA CODE(13),KNTM(13),KNTR(13) / 'C205.2', 255, 255 /
      DATA CODE(14),KNTM(14),KNTR(14) / 'C207.0', 255, 255 /
      DATA CODE(15),KNTM(15),KNTR(15) / 'C208.0', 255, 255 /
      DATA CODE(16),KNTM(16),KNTR(16) / 'C209.0', 255, 255 /
      DATA CODE(17),KNTM(17),KNTR(17) / 'C209.1', 255, 255 /
      DATA CODE(18),KNTM(18),KNTR(18) / 'C209.2', 255, 255 /
      DATA CODE(19),KNTM(19),KNTR(19) / 'C209.3', 255, 255 /
      DATA CODE(20),KNTM(20),KNTR(20) / 'C210.1', 255, 255 /
      DATA CODE(21),KNTM(21),KNTR(21) / 'C302.1', 255, 255 /
      DATA CODE(22),KNTM(22),KNTR(22) / 'C303.1', 255, 255 /
      DATA CODE(23),KNTM(23),KNTR(23) / 'C304.1', 255, 255 /
      DATA CODE(24),KNTM(24),KNTR(24) / 'C305.1', 255, 255 /
      DATA CODE(25),KNTM(25),KNTR(25) / 'C306.1', 255, 255 /
      DATA CODE(26),KNTM(26),KNTR(26) / 'C307.1', 255, 255 /
      DATA CODE(27),KNTM(27),KNTR(27) / 'C312.1', 255, 255 /
      DATA CODE(28),KNTM(28),KNTR(28) / 'C313.1', 255, 255 /
      DATA CODE(29),KNTM(29),KNTR(29) / 'C315.1', 255, 255 /
      DATA CODE(30),KNTM(30),KNTR(30) / 'C316.1', 255, 255 /
      DATA CODE(31),KNTM(31),KNTR(31) / 'C316.2', 255, 255 /
      DATA CODE(32),KNTM(32),KNTR(32) / 'C320.1', 255, 255 /
      DATA CODE(33),KNTM(33),KNTR(33) / 'C321.1', 255, 255 /
      DATA CODE(34),KNTM(34),KNTR(34) / 'C323.1', 255, 255 /
      DATA CODE(35),KNTM(35),KNTR(35) / 'C327.1', 255, 255 /
      DATA CODE(36),KNTM(36),KNTR(36) / 'C328.1', 255, 255 /
      DATA CODE(37),KNTM(37),KNTR(37) / 'C328.2', 255, 255 /
      DATA CODE(38),KNTM(38),KNTR(38) / 'C328.3', 255, 255 /
      DATA CODE(39),KNTM(39),KNTR(39) / 'C330.1', 255, 255 /
      DATA CODE(40),KNTM(40),KNTR(40) / 'C330.2', 255, 255 /
      DATA CODE(41),KNTM(41),KNTR(41) / 'C330.3', 255, 255 /
      DATA CODE(42),KNTM(42),KNTR(42) / 'C331.1', 255, 255 /
      DATA CODE(43),KNTM(43),KNTR(43) / 'C331.2', 255, 255 /
      DATA CODE(44),KNTM(44),KNTR(44) / 'C334.1', 255, 255 /
      DATA CODE(45),KNTM(45),KNTR(45) / 'C334.2', 255, 255 /
      DATA CODE(46),KNTM(46),KNTR(46) / 'C334.3', 255, 255 /
      DATA CODE(47),KNTM(47),KNTR(47) / 'C334.4', 255, 255 /
      DATA CODE(48),KNTM(48),KNTR(48) / 'C334.5', 255, 255 /
      DATA CODE(49),KNTM(49),KNTR(49) / 'C334.6', 255, 255 /
      DATA CODE(50),KNTM(50),KNTR(50) / 'C336.1', 255, 255 /
      DATA CODE(51),KNTM(51),KNTR(51) / 'C337.1', 255, 255 /
      DATA CODE(52),KNTM(52),KNTR(52) / 'C338.1', 255, 255 /
      DATA CODE(53),KNTM(53),KNTR(53) / 'C340.1', 255, 255 /
      DATA CODE(54),KNTM(54),KNTR(54) / 'C343.1', 255, 255 /
      DATA CODE(55),KNTM(55),KNTR(55) / 'C343.2', 255, 255 /
      DATA CODE(56),KNTM(56),KNTR(56) / 'C343.3', 255, 255 /
      DATA CODE(57),KNTM(57),KNTR(57) / 'C343.4', 255, 255 /
      DATA CODE(58),KNTM(58),KNTR(58) / 'C344.1', 255, 255 /
      DATA CODE(59),KNTM(59),KNTR(59) / 'C344.2', 255, 255 /
      DATA CODE(60),KNTM(60),KNTR(60) / 'C344.3', 255, 255 /
      DATA CODE(61),KNTM(61),KNTR(61) / 'C344.4', 255, 255 /
      DATA CODE(62),KNTM(62),KNTR(62) / 'C345.1', 255, 255 /
      DATA CODE(63),KNTM(63),KNTR(63) / 'C346.1', 255, 255 /
      DATA CODE(64),KNTM(64),KNTR(64) / 'C346.2', 255, 255 /
      DATA CODE(65),KNTM(65),KNTR(65) / 'C346.3', 255, 255 /
      DATA CODE(66),KNTM(66),KNTR(66) / 'C347.1', 255, 255 /
      DATA CODE(67),KNTM(67),KNTR(67) / 'C347.2', 255, 255 /
      DATA CODE(68),KNTM(68),KNTR(68) / 'C347.3', 255, 255 /
      DATA CODE(69),KNTM(69),KNTR(69) / 'C347.4', 255, 255 /
      DATA CODE(70),KNTM(70),KNTR(70) / 'C347.5', 255, 255 /
      DATA CODE(71),KNTM(71),KNTR(71) / 'C347.6', 255, 255 /
      DATA CODE(72),KNTM(72),KNTR(72) / 'C348.1', 255, 255 /
      DATA CODE(73),KNTM(73),KNTR(73) / 'C349.1', 255, 255 /
      DATA CODE(74),KNTM(74),KNTR(74) / 'C349.2', 255, 255 /
      DATA CODE(75),KNTM(75),KNTR(75) / 'C349.3', 255, 255 /
      DATA CODE(76),KNTM(76),KNTR(76) / 'D101.1', 255, 255 /
      DATA CODE(77),KNTM(77),KNTR(77) / 'D103.1', 255, 255 /
      DATA CODE(78),KNTM(78),KNTR(78) / 'D104.1', 255, 255 /
      DATA CODE(79),KNTM(79),KNTR(79) / 'D104.2', 255, 255 /
      DATA CODE(80),KNTM(80),KNTR(80) / 'D105.1', 255, 255 /
      DATA CODE(81),KNTM(81),KNTR(81) / 'D105.2', 255, 255 /
      DATA CODE(82),KNTM(82),KNTR(82) / 'D107.1', 255, 255 /
      DATA CODE(83),KNTM(83),KNTR(83) / 'D110.0', 255, 255 /
      DATA CODE(84),KNTM(84),KNTR(84) / 'D110.1', 255, 255 /
      DATA CODE(85),KNTM(85),KNTR(85) / 'D110.2', 255, 255 /
      DATA CODE(86),KNTM(86),KNTR(86) / 'D110.3', 255, 255 /
      DATA CODE(87),KNTM(87),KNTR(87) / 'D110.4', 255, 255 /
      DATA CODE(88),KNTM(88),KNTR(88) / 'D110.5', 255, 255 /
      DATA CODE(89),KNTM(89),KNTR(89) / 'D110.6', 255, 255 /
      DATA CODE(90),KNTM(90),KNTR(90) / 'D113.1', 255, 255 /
      DATA CODE(91),KNTM(91),KNTR(91) / 'D201.1', 255, 255 /
      DATA CODE(92),KNTM(92),KNTR(92) / 'D202.1', 255, 255 /
      DATA CODE(93),KNTM(93),KNTR(93) / 'D401.1', 255, 255 /
      DATA CODE(94),KNTM(94),KNTR(94) / 'D601.1', 255, 255 /
      DATA CODE(95),KNTM(95),KNTR(95) / 'E210.1', 255, 255 /
      DATA CODE(96),KNTM(96),KNTR(96) / 'E210.2', 255, 255 /
      DATA CODE(97),KNTM(97),KNTR(97) / 'E210.3', 255, 255 /
      DATA CODE(98),KNTM(98),KNTR(98) / 'E210.4', 255, 255 /
      DATA CODE(99),KNTM(99),KNTR(99) / 'E210.5', 255, 255 /
      DATA CODE(100),KNTM(100),KNTR(100) / 'E210.6', 255, 255 /
      DATA CODE(101),KNTM(101),KNTR(101) / 'E210.7', 255, 255 /
      DATA CODE(102),KNTM(102),KNTR(102) / 'E211.0', 255, 255 /
      DATA CODE(103),KNTM(103),KNTR(103) / 'E211.1', 255, 255 /
      DATA CODE(104),KNTM(104),KNTR(104) / 'E211.2', 255, 255 /
      DATA CODE(105),KNTM(105),KNTR(105) / 'E211.3', 255, 255 /
      DATA CODE(106),KNTM(106),KNTR(106) / 'E211.4', 255, 255 /
      DATA CODE(107),KNTM(107),KNTR(107) / 'E406.0', 255, 255 /
      DATA CODE(108),KNTM(108),KNTR(108) / 'E406.1', 255, 255 /
      DATA CODE(109),KNTM(109),KNTR(109) / 'E407.0', 255, 255 /
      DATA CODE(110),KNTM(110),KNTR(110) / 'E408.0', 255, 255 /
      DATA CODE(111),KNTM(111),KNTR(111) / 'E408.1', 255, 255 /
      DATA CODE(112),KNTM(112),KNTR(112) / 'F500.0', 255, 255 /
      DATA CODE(113),KNTM(113),KNTR(113) / 'F500.1', 255, 255 /
      DATA CODE(114),KNTM(114),KNTR(114) / 'F500.2', 255, 255 /
      DATA CODE(115),KNTM(115),KNTR(115) / 'F500.3', 255, 255 /
      DATA CODE(116),KNTM(116),KNTR(116) / 'G100.1', 255, 255 /
      DATA CODE(117),KNTM(117),KNTR(117) / 'G100.2', 255, 255 /
      DATA CODE(118),KNTM(118),KNTR(118) / 'G101.1', 255, 255 /
      DATA CODE(119),KNTM(119),KNTR(119) / 'G101.2', 255, 255 /
      DATA CODE(120),KNTM(120),KNTR(120) / 'G105.1', 255, 255 /
      DATA CODE(121),KNTM(121),KNTR(121) / 'G106.1', 255, 255 /
      DATA CODE(122),KNTM(122),KNTR(122) / 'G106.2', 255, 255 /
      DATA CODE(123),KNTM(123),KNTR(123) / 'G116.1', 255, 255 /
      DATA CODE(124),KNTM(124),KNTR(124) / 'G116.2', 255, 255 /
      DATA CODE(125),KNTM(125),KNTR(125) / 'H101.0', 255, 255 /
      DATA CODE(126),KNTM(126),KNTR(126) / 'H101.1', 255, 255 /
      DATA CODE(127),KNTM(127),KNTR(127) / 'H101.2', 255, 255 /
      DATA CODE(128),KNTM(128),KNTR(128) / 'H301.1', 255, 255 /
      DATA CODE(129),KNTM(129),KNTR(129) / 'U501.1', 255, 255 /
      DATA CODE(130),KNTM(130),KNTR(130) / 'V202.1', 255, 255 /
      DATA CODE(131),KNTM(131),KNTR(131) / 'V202.2', 255, 255 /
      DATA CODE(132),KNTM(132),KNTR(132) / 'V202.3', 255, 255 /

c# 175 "wj_MTLSET.F"

      ILG=NLG
      L=0
      IF(ERC .NE. ' ') THEN
       DO 10 L = 1,6
       IF(ERC(1:L) .EQ. ERC) GOTO 12
   10  CONTINUE
   12  CONTINUE
      ENDIF
      DO 14 I = 1,KTE
      IF(L .EQ. 0 .OR. CODE(I)(1:L) .EQ. ERC(1:L)) THEN
       IF(MXM .GE. 0) KNTM(I)=MXM
       IF(MXR .GE. 0) KNTR(I)=MXR
      ENDIF
   14 CONTINUE
      RETURN

      ENTRY wj_MTLMTR(ERC,MLG,LMF,LRF)

      MLG=ILG
      DO 20 I = 1,KTE
      IF(ERC .EQ. CODE(I))  GOTO 21
   20 CONTINUE
      WRITE(*,100) ERC
      CALL wj_ABEND
      RETURN

   21 LMF=KNTM(I) .GE. 1
      LRF=KNTR(I) .GE. 1
      IF(LMF .AND. KNTM(I) .LT. 255)  KNTM(I)=KNTM(I)-1
      IF(LRF .AND. KNTR(I) .LT. 255)  KNTR(I)=KNTR(I)-1
      IF(.NOT.LRF) THEN
       IF(ILG .LT. 1) WRITE(  *,101) CODE(I)
       IF(ILG .GE. 1) WRITE(ILG,101) CODE(I)
      ENDIF
      RETURN
  100 FORMAT(7X,'***** CERN N002 wj_MTLSET ... ERROR N002: ',
     1'ERROR CODE ',A6,' NOT RECOGNIZED BY ERROR MONITOR. RUN ABORTED.')
  101 FORMAT(7X,'***** CERN N002 wj_MTLSET ... ERROR NOO2.1: ',
     1'RUN TERMINATED BY LIBRARY ERROR CONDITION ',A6)
      END
c# 1 "wj_MTLPRT.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_MTLPRT.F"
*
* $Id: wj_MTLPRT.F,v 1.1.1.1 1996/04/01 15:02:52 mclareni Exp $
*
* $Log: wj_MTLPRT.F,v $
* Revision 1.1.1.1  1996/04/01 15:02:52  mclareni
* Mathlib wj_gen
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h" 1
























c# 40 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"

c# 57 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"



























































c# 10 "wj_MTLPRT.F" 2
      SUBROUTINE wj_MTLPRT(NAME,ERC,TEXT)
      CHARACTER*(*) NAME,ERC,TEXT
      LOGICAL LMF,LRF

      IF(ERC(5:6).NE.'.0') THEN
        CALL wj_MTLMTR(ERC,MLG,LMF,LRF)
      ELSE
        LMF=.TRUE.
        LRF=.FALSE.
      ENDIF
      IF(LMF) THEN
        LT=wj_LENOCC(TEXT)
        IF(MLG .LT. 1) WRITE(  *,100) ERC(1:4),NAME,ERC,TEXT(1:LT)
        IF(MLG .GE. 1) WRITE(MLG,100) ERC(1:4),NAME,ERC,TEXT(1:LT)
      ENDIF
      IF(.NOT.LRF) CALL wj_ABEND
      RETURN
100   FORMAT(7X,'***** CERN ',A,1X,A,' ERROR ',A,': ',A)
      END
c# 1 "wj_PERMU.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_PERMU.F"
*
* $Id: wj_PERMU.F,v 1.1.1.1 1996/04/01 15:02:57 mclareni Exp $
*
* $Log: wj_PERMU.F,v $
* Revision 1.1.1.1  1996/04/01 15:02:57  mclareni
* Mathlib wj_gen
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h" 1
























c# 40 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"

c# 57 "/usr/local/home/video/cernlib/2005/include/wj_gen/pilot.h"



























































c# 10 "wj_PERMU.F" 2
      SUBROUTINE wj_PERMU(IA,N)
C
      CHARACTER*(*) NAME
      PARAMETER(NAME='wj_PERMUT')
C
      DIMENSION IA(*)
      PARAMETER (IFD = 12)
      DIMENSION IFCT(0:IFD),IV(IFD+1)
      CHARACTER*80 ERRTXT

      DATA (IFCT(I),I=0,IFD) /1,1,2,6,24,120,720,5040,40320,362880,
     1                        3628800,39916800,479001600/

      IF(N .LE. 0) RETURN
      IF(IA(1) .EQ. 0) THEN
       DO 11 I = 1,N
   11  IA(I)=I
       IF(N .EQ. 1) IA(1)=0
      ELSE
       DO 12 K1 = N,2,-1
       K=K1
       IF(IA(K-1) .LT. IA(K)) GO TO 14
   12  CONTINUE
       IA(1)=0
       RETURN
   14  KN=K+N
       DO 15 L = K,KN/2
       IB=IA(KN-L)
       IA(KN-L)=IA(L)
   15  IA(L)=IB
       DO 16 wj_L1 = K,N
       L=wj_L1
       IF(IA(L) .GT. IA(K-1)) GO TO 17
   16  CONTINUE
   17  IB=IA(K-1)
       IA(K-1)=IA(L)
       IA(L)=IB
      ENDIF
      RETURN

      ENTRY wj_PERMUT(NRP,N,IA)

      IF(N .LE. 0) RETURN
      IF(N .GT. IFD) THEN
       WRITE(ERRTXT,101) N
       CALL wj_MTLPRT(NAME,'V202.1',ERRTXT)
      ELSEIF(NRP .GT. IFCT(N)) THEN
       IA(1)=0
       CALL wj_MTLPRT(NAME,'V202.2',
     +             'PERMUTATION OUTSIDE LEXICON REQUESTED')
      ELSE
       DO 21 I = 1,N
   21  IV(I)=I
       IO=NRP-1
       DO 22 M = N-1,1,-1
       IN=IO/IFCT(M)+1
       IO=MOD(IO,IFCT(M))
       IA(N-M)=IV(IN)
       DO 23 I = IN,M
   23  IV(I)=IV(I+1)
   22  CONTINUE
       IA(N)=IV(1)
      END IF
      RETURN

      ENTRY wj_COMBI(IA,N,J)

      IF(N .LE. 0 .OR. J .LE. 0) RETURN
      IF(J .GT. N) THEN
       WRITE(ERRTXT,103) J,N
       CALL wj_MTLPRT(NAME,'V202.3',ERRTXT)
      ELSEIF(IA(1) .EQ. 0) THEN
       DO 31 I = 1,J
   31  IA(I)=I
       IA(J+1)=0
      ELSE
       DO 32 I1 = 1,N
       I=I1
       IF(IA(I+1) .NE. IA(I)+1) GO TO 33
   32  IA(I)=I
   33  IA(I)=IA(I)+1
       IF(IA(J) .EQ. N+1) IA(1)=0
      ENDIF
      RETURN
  101 FORMAT('N = ',I20,' TOO BIG')
  103 FORMAT('J = ',I5,' > ',I5,' = N IS NOT PERMITTED')
      END




c# 1 "wj_SORTZV.F"
c# 1 "<built-in>"
c# 1 "<command line>"
c# 1 "wj_SORTZV.F"
*
* $Id: wj_SORTZV.F,v 1.1.1.1 1996/02/15 17:49:50 mclareni Exp $
*
* $Log: wj_SORTZV.F,v $
* Revision 1.1.1.1  1996/02/15 17:49:50  mclareni
* Kernlib
*
*
c# 1 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h" 1
c# 94 "/usr/local/home/video/cernlib/2005/include/kerngen/pilot.h"





c# 10 "wj_SORTZV.F" 2
      SUBROUTINE wj_SORTZV (A,INDEX,N1,MODE,NWAY,NSORT)
C
C CERN PROGLIB# M101    wj_SORTZV          .VERSION KERNFOR  3.15  820113
C ORIG. 02/10/75
C
      DIMENSION A(N1),INDEX(N1)
C
C
      N = N1
      IF (N.LE.0)            RETURN
      IF (NSORT.NE.0) GO TO 2
      DO 1 I=1,N
    1 INDEX(I)=I
C
    2 IF (N.EQ.1)            RETURN
      IF (MODE)    10,20,30
   10 CALL wj_SORTTI (A,INDEX,N)
      GO TO 40
C
   20 CALL wj_SORTTC(A,INDEX,N)
      GO TO 40
C
   30 CALL wj_SORTTF (A,INDEX,N)
C
   40 IF (NWAY.EQ.0) GO TO 50
      N2 = N/2
      DO 41 I=1,N2
      ISWAP = INDEX(I)
      K = N+1-I
      INDEX(I) = INDEX(K)
   41 INDEX(K) = ISWAP
   50 RETURN
      END
*     ========================================
      SUBROUTINE wj_SORTTF (A,INDEX,N1)
C
      DIMENSION A(N1),INDEX(N1)
C
      N = N1
      DO 3 I1=2,N
      I3 = I1
      I33 = INDEX(I3)
      AI = A(I33)
    1 I2 = I3/2
      IF (I2) 3,3,2
    2 I22 = INDEX(I2)
      IF (AI.LE.A (I22)) GO TO 3
      INDEX (I3) = I22
      I3 = I2
      GO TO 1
    3 INDEX (I3) = I33
    4 I3 = INDEX (N)
      INDEX (N) = INDEX (1)
      AI = A(I3)
      N = N-1
      IF (N-1) 12,12,5
    5 I1 = 1
    6 I2 = I1 + I1
      IF (I2.LE.N) I22= INDEX(I2)
      IF (I2-N) 7,9,11
    7 I222 = INDEX (I2+1)
      IF (A(I22)-A(I222)) 8,9,9
    8 I2 = I2+1
      I22 = I222
    9 IF (AI-A(I22)) 10,11,11
   10 INDEX(I1) = I22
      I1 = I2
      GO TO 6
   11 INDEX (I1) = I3
      GO TO 4
   12 INDEX (1) = I3
      RETURN
      END
*     ========================================
      SUBROUTINE wj_SORTTI (A,INDEX,N1)
C
      INTEGER A,AI
      DIMENSION A(N1),INDEX(N1)
C
      N = N1
      DO 3 I1=2,N
      I3 = I1
      I33 = INDEX(I3)
      AI = A(I33)
    1 I2 = I3/2
      IF (I2) 3,3,2
    2 I22 = INDEX(I2)
      IF (AI.LE.A (I22)) GO TO 3
      INDEX (I3) = I22
      I3 = I2
      GO TO 1
    3 INDEX (I3) = I33
    4 I3 = INDEX (N)
      INDEX (N) = INDEX (1)
      AI = A(I3)
      N = N-1
      IF (N-1) 12,12,5
    5 I1 = 1
    6 I2 = I1 + I1
      IF (I2.LE.N) I22= INDEX(I2)
      IF (I2-N) 7,9,11
    7 I222 = INDEX (I2+1)
      IF (A(I22)-A(I222)) 8,9,9
    8 I2 = I2+1
      I22 = I222
    9 IF (AI-A(I22)) 10,11,11
   10 INDEX(I1) = I22
      I1 = I2
      GO TO 6
   11 INDEX (I1) = I3
      GO TO 4
   12 INDEX (1) = I3
      RETURN
      END
*     ========================================
      SUBROUTINE wj_SORTTC (A,INDEX,N1)
C
      INTEGER A,AI
      DIMENSION A(N1),INDEX(N1)
C
      N = N1
      DO 3 I1=2,N
      I3 = I1
      I33 = INDEX(I3)
      AI = A(I33)
    1 I2 = I3/2
      IF (I2) 3,3,2
    2 I22 = INDEX(I2)
      IF(wj_ICMPCH(AI,A(I22)))3,3,21
   21 INDEX (I3) = I22
      I3 = I2
      GO TO 1
    3 INDEX (I3) = I33
    4 I3 = INDEX (N)
      INDEX (N) = INDEX (1)
      AI = A(I3)
      N = N-1
      IF (N-1) 12,12,5
    5 I1 = 1
    6 I2 = I1 + I1
      IF (I2.LE.N) I22= INDEX(I2)
      IF (I2-N) 7,9,11
    7 I222 = INDEX (I2+1)
      IF (wj_ICMPCH(A(I22),A(I222))) 8,9,9
    8 I2 = I2+1
      I22 = I222
    9 IF (wj_ICMPCH(AI,A(I22))) 10,11,11
   10 INDEX(I1) = I22
      I1 = I2
      GO TO 6
   11 INDEX (I1) = I3
      GO TO 4
   12 INDEX (1) = I3
      RETURN
      END
*     ========================================
      FUNCTION wj_ICMPCH(IC1,IC2)
C     FUNCTION TO COMPARE TWO 4 CHARACTER EBCDIC STRINGS - IC1,IC2
C     wj_ICMPCH=-1 IF HEX VALUE OF IC1 IS LESS THAN IC2
C     wj_ICMPCH=0  IF HEX VALUES OF IC1 AND IC2 ARE THE SAME
C     wj_ICMPCH=+1 IF HEX VALUES OF IC1 IS GREATER THAN IC2
      I1=IC1
      I2=IC2
      IF(I1.GE.0.AND.I2.GE.0)GOTO 40
      IF(I1.GE.0)GOTO 60
      IF(I2.GE.0)GOTO 80
      I1=-I1
      I2=-I2
      IF(I1-I2)80,70,60
 40   IF(I1-I2)60,70,80
 60   wj_ICMPCH=-1
      RETURN
 70   wj_ICMPCH=0
      RETURN
 80   wj_ICMPCH=1
      RETURN
      END





c **********  THIS IS NOT A CERN ROUTINE!!  ***************
      logical function wj_pwhg_isfinite(x)
      implicit none
      real * 8 x
c According to ieee standards, a NaN is the only real not
c satisfying x.eq.x.
      if (.not.(x.eq.x)) then
         wj_pwhg_isfinite = .false.
         call wj_increasecnt('NaN exception')
         return
      endif
c Put constraint to avoid denormals
      if(x.gt.1.or.x.lt.-1) then
         if (1/x.eq.0) then
            wj_pwhg_isfinite = .false.
            call wj_increasecnt('Inf exception')
            return
         endif
      endif
      wj_pwhg_isfinite = .true.
      end


      subroutine wj_increasecnt(string)
      implicit none
      character *(*) string
      integer maxnum
      parameter (maxnum=100)
      character * 100 keywords(maxnum)
      real * 8 counters(maxnum)
      integer ncounters
      common/wj_ccounters/keywords,counters,ncounters
      integer ini,j
      data ini/0/
      save ini
      if(ini.eq.0) then
         ncounters=0
         ini=1
      endif
      do j=1,ncounters
         if(string.eq.keywords(j)) then
            counters(j)=counters(j)+1
            return
         endif
      enddo
c not found
      if(ncounters.eq.maxnum) then
         write(*,*) 'ERROR: wj_increasecnt too many counters requested'
         stop
      endif
      ncounters=ncounters+1
      keywords(ncounters)=string
      counters(ncounters)=1
      end

      subroutine wj_resetcnt(string)
      implicit none
      character *(*) string
      integer maxnum
      parameter (maxnum=100)
      character * 100 keywords(maxnum)
      real * 8 counters(maxnum)
      integer ncounters
      common/wj_ccounters/keywords,counters,ncounters
      integer ini,j
      do j=1,ncounters
         if(string.eq.keywords(j)) then
            counters(j)=0
            return
         endif
      enddo
c not found
      if(ncounters.eq.maxnum) then
         write(*,*) 'ERROR: wj_increasecnt too many counters requested'
         stop
      endif
      ncounters=ncounters+1
      keywords(ncounters)=string
      counters(ncounters)=0
      end

      subroutine wj_incrcntrs(string,k)
      implicit none
      integer k
      character *(*) string
      character * 20 line
      line=string
      line(17:)=' '
      write(line(17:),'(i4)') k
      call wj_increasecnt(line)
      end

      subroutine wj_incrcntrs2(string,j,k)
      implicit none
      integer j,k
      character *(*) string
      character * 20 line
      line=string
      line(17:)=' '
      write(line(13:),'(i4)') j
      write(line(17:),'(i4)') k
      call wj_increasecnt(line)
      end

      subroutine wj_printcnt(iun)
      implicit none
      integer iun
      integer maxnum
      parameter (maxnum=100)
      character * 100 keywords(maxnum)
      real * 8 counters(maxnum)
      integer ncounters
      common/wj_ccounters/keywords,counters,ncounters
      integer j,k
      if(ncounters.eq.0) return
      write(iun,*)
      write(iun,*) 'Value of counters at end of run:'
      do j=1,ncounters
         k=100
 1       if(keywords(j)(k:k).eq.' ') then
            k=k-1
            goto 1
         endif
         write(iun,*) keywords(j)(1:k),' = ',counters(j)
      enddo
      write(iun,*)
      end

      function wj_getcnt(string)
      implicit none
      real * 8 wj_getcnt
      character *(*) string
      integer maxnum
      parameter (maxnum=100)
      character * 100 keywords(maxnum)
      real * 8 counters(maxnum)
      integer ncounters
      common/wj_ccounters/keywords,counters,ncounters
      integer j
      do j=1,ncounters
         if(string.eq.keywords(j)) then
            wj_getcnt=counters(j)
            return
         endif
      enddo
      wj_getcnt=0
      end
