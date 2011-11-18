C--------------------------------------------------------------------------
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See BelEvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: EvtJetSetInit.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C    RS          October 28, 2002        copied and modified from Jetset
C
C------------------------------------------------------------------------
      SUBROUTINE EVTPYTHIAINIT(FNAME)
      IMPLICIT NONE
CCC      EXTERNAL PYDATA
      
      CHARACTER*(*) FNAME
      
CC      PRINT *,'PYUPDA : : ',FNAME
      OPEN(54,STATUS='OLD',FILE=FNAME)
      CALL PYUPDA(3,54)
      CLOSE(54)      
CC      WRITE (*,*) 'PYUPDA DONE'
      RETURN
      END
CCC      SUBROUTINE INITPYTHIA(GAGA)
CCC      IMPLICIT NONE
CCC      INTEGER PYCOMP
CCC      EXTERNAL PYDATA,PYCOMP
CCC      LOGICAL FIRST
CCC      SAVE FIRST
CCC      DATA FIRST / .TRUE. /
CCC      DOUBLE PRECISION MAXIMUM
CCC      COMMON/CBBEAM/MAXIMUM
CCC     INTEGER IDC,GAGA
C...Decay information.
CCC      INTEGER MDCY,MDME,KFDP
CCC      DOUBLE PRECISION BRAT
CCC      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      
CCC      CHARACTER CH1*16,CH2*16
      
CCC      INTEGER DC,CDC
C     order of DC is d u s c b t b' t' ...
C     ... e nu_e mu nu_mu tau nu_tau tau' nu_tau'
CCC      COMMON/DECAYCH/DC(18)
      
CCC      IF(FIRST) THEN
CCC         MAXIMUM=10.6
CCC         WRITE (*,*) "Decay of Z0/gamma to"
C...  Only allow Z0 decay to quarks (i.e. no leptonic final states).
C     RS veto also b quarks
CCC         DO IDC=MDCY(23,2),MDCY(23,2)+MDCY(23,3)-1
C     first switch channel off
CCC            MDME(IDC,1)=MIN(0,MDME(IDC,1))
CCC            CDC=IABS(KFDP(IDC,1))
C     then switch on again if requested
CCC            CALL PYNAME(CDC,CH1)
CCC            CALL PYNAME(-CDC,CH2)
CCC            IF(DC(CDC).EQ.1) THEN
CCC               MDME(IDC,1)=1
CCC               WRITE (*,*) "  ",ch1,"+ ",ch2," ALLOWED"
CCC            ELSE
CCC               WRITE (*,*) "  ",ch1,"+ ",ch2," DISABLED"
CCC            ENDIF
CCC         ENDDO
C     CALL PYSTAT(2)
         
CCC         IF(GAGA.EQ.0) THEN
CCC            CALL PYINIT('CMS','E+','E-',MAXIMUM)
CCC         ELSE
CCC            CALL PYINIT('CMS','GAMMA/E+','GAMMA/E-',MAXIMUM)
CCC         ENDIF
C     CALL PYLIST(0)
C         WRITE (*,*) 'done PYTHIA initialization'//
C     >        ' with varying beam energy'
C         WRITE (*,*) 'maximum allowed energy is ',MAXIMUM,' GeV'
CCC         FIRST=.FALSE.
CCC     ENDIF
CCC      RETURN
CCC      END
      
C--------------------------------------------------------------------------
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See BelEvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: continuum.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C     26-Sep-2002 - RS : changed to PYTHIA
C
C------------------------------------------------------------------------
CCC      SUBROUTINE PYCONTINUUM(ENERGY,NDAUG,KF,PX,PY,PZ,E)
CCC      IMPLICIT NONE
CCC      DOUBLE PRECISION MAXIMUM
CCC      COMMON/CBBEAM/MAXIMUM
      
CCC      DOUBLE PRECISION P,V
CCC      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      
CCC      INTEGER MSTU,MSTJ
CCC      DOUBLE PRECISION PARU,PARJ
CCC      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      
CCC      INTEGER MSTP,MSTI
CCC      DOUBLE PRECISION PARP,PARI
CCC      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
C...Decay information.
CCC      INTEGER MDCY,MDME,KFDP
CCC      DOUBLE PRECISION BRAT
CCC      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      
CCC      INTEGER N,NPAD,K,IMSTJ
      
CCC      REAL*8 PXSUM,PYSUM,PZSUM
      
CCC      REAL*8 ENERGY
      
CCC      INTEGER PYCOMP
CCC      EXTERNAL PYCOMP
      
CCC      INTEGER KF(100),I,NDAUG
CCC      REAL*8  PX(100),PY(100),PZ(100),E(100)

C     RS Particles should not decay at this stage
C     but remember the previous setting
CCC      IMSTJ=MSTJ(21)
CCC      MSTJ(21)=0
      
C      IF(FLAVOR.NE.LastProdFLav) THEN
C...  Only allow Z0 decay to decay to certain flavour
C         DO IDC=MDCY(23,2),MDCY(23,2)+MDCY(23,3)-1
C            IF(IABS(KFDP(IDC,1)).NE.Flavor)
C     >           MDME(IDC,1)=MIN(0,MDME(IDC,1))
C         ENDDO
C      ENDIF
      
CCC 4    PARP(171)=Energy/Maximum
CCC      CALL PYEVNT
      
C--- only primary quarks and stable particles remain in the event record
CCC      CALL PYEDIT(5)
      
C     RS and allow decays
CCC      MSTJ(21)=IMSTJ
      
CCC      NDAUG=0
C     sum to check that we preserve momentum
CCC      PXSUM=0.0
CCC      PYSUM=0.0
CCC      PZSUM=0.0
      
CCC      DO 1,I=1,N
CCC         NDAUG=NDAUG+1
CCC         KF(NDAUG)=K(I,2)
CCC         PX(NDAUG)=P(I,1)
CCC         PY(NDAUG)=P(I,2)
CCC         PZ(NDAUG)=P(I,3)
CCC         E(NDAUG) =P(I,4)
CCC         IF(K(I,1).LT.10)THEN
CCC            PXSUM=PXSUM+PX(NDAUG)
CCC            PYSUM=PYSUM+PY(NDAUG)
CCC            PZSUM=PZSUM+PZ(NDAUG)
CCC         ENDIF
CCC1     CONTINUE
      
CCC      IF (ABS(PXSUM).GT.0.001.OR.
CCC     +       ABS(PYSUM).GT.0.001.OR.
CCC     +       ABS(PZSUM).GT.0.001) THEN


CCC      PRINT *, 'Momentum not conserved in jetset fragmentation:'
CCC         PRINT *,'dPx:',pxsum,' dPy:',pysum,' dPz:',pzsum

CCC         CALL PYLIST(2)

CCC         GOTO 4
         
CCC      ENDIF
CCC      RETURN
CCC      END
C--------------------------------------------------------------------------
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See BelEvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: jetset1.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C     26-Sep-2002 - RS : changed to PYTHIA
C
C------------------------------------------------------------------------
      SUBROUTINE PYTHIADEC(IP,M,NDAUG,KF,KM,PX,PY,PZ,E)

C
C interface to JETSET 7.4 to have one particle decayed
C including possibly fragmentation, if the decay products include
C partons.
C     
      IMPLICIT NONE
      
      INTEGER MSTU,MSTJ
      DOUBLE PRECISION PARU,PARJ
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      SAVE /PYDAT1/
      DOUBLE PRECISION P,V
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      INTEGER MINT
      DOUBLE PRECISION VINT 
      COMMON/PYINT1/MINT(400),VINT(400)
      
      INTEGER N,NPAD,K
      
      INTEGER NDMAX
      PARAMETER (NDMAX=100)
      
      INTEGER PYCOMP
      EXTERNAL PYCOMP
      
      INTEGER IP,KF(NDMAX),I,NDAUG,KM(NDMAX)
      INTEGER KP,KID,IPART1,IPART
      REAL*8  M,PX(NDMAX),PY(NDMAX),PZ(NDMAX),E(NDMAX)
      
      INTEGER IP1,IMSTJ
      DOUBLE PRECISION QMAX
      
C      CALL PY1ENT(1,IP,M,0D0,0D0)
 
      K(1,1)=1
      K(1,2)=ip
      P(1,5)=m
      P(1,4)=m
      P(1,1)=0.0
      P(1,2)=0.0
      P(1,3)=0.0
      n=1
     
C     RS Particles should not decay at this stage
C     but remember the previous setting
      IMSTJ=MSTJ(21)
      MSTJ(21)=0
      
      CALL PYDECY(1)
      
C     RS and allow decays
      MSTJ(21)=IMSTJ




C code copied from LUEXEC to avoid error with shower
C switched on
C...Decay products may develop a shower.
        IF(MSTJ(92).GT.0) THEN
          IP1=MSTJ(92)
          QMAX=SQRT(MAX(0.D0,(P(IP1,4)+P(IP1+1,4))**2-(P(IP1,1)+P(IP1+1,
     &    1))**2-(P(IP1,2)+P(IP1+1,2))**2-(P(IP1,3)+P(IP1+1,3))**2))
	  MINT(33) = 0 
          CALL PYSHOW(IP1,IP1+1,QMAX)
          CALL PYPREP(IP1)
          MSTJ(92)=0
        ELSEIF(MSTJ(92).LT.0) THEN
          IP1=-MSTJ(92)
	  MINT(33)=0
          CALL PYSHOW(IP1,-3,P(1,5))
          CALL PYPREP(IP1)
          MSTJ(92)=0
        ENDIF

c
c for debugging:
c      call lulist(1)
c
      mstj(21)=0
      call pyexec
      mstj(21)=2
 
c find partons, delete secondary partons, set mother pointers

      ndaug = 0
      ipart1 = 1
      ipart = 1

      do 10 i=2,n
         kp = k(i,3)
         kid = k(i,2)
         if (abs(kid) .ge. 1 .and. abs(kid) .le. 8
     1        .or. kid .eq. 21
     2        .or. kid .ge. 91 .and. kid .le. 94)  then
            if (ipart1 .eq. 1) ipart1 = i
            ipart = i
            if (kp .ne. 1)  goto 10
            kp = 0
         else
            if (kp .gt. ipart)  then
               goto 10
            elseif (kp .ge. ipart1)  then
               kp = ipart1-1
            else
               kp = 0
            endif
         endif
         ndaug = ndaug + 1
         km(ndaug)=kp
         kf(ndaug)=kid
         px(ndaug)=p(i,1)
         py(ndaug)=p(i,2)
         pz(ndaug)=p(i,3)
         e(ndaug)=p(i,4)

C        print '( 2I5,I12,4F12.4 )',ndaug,km(ndaug),kf(ndaug),
C     1    px(ndaug),py(ndaug),pz(ndaug),e(ndaug)

 10   CONTINUE

C      NDAUG=0
C      DO I=2,N
C        NDAUG = NDAUG + 1
C        KM(NDAUG)=K(I,3)
C        KF(NDAUG)=K(I,2)
C        PX(NDAUG)=P(I,1)
C        PY(NDAUG)=P(I,2)
C        PZ(NDAUG)=P(I,3)
C        E(NDAUG)=P(I,4)
C      ENDDO


      RETURN
      END
