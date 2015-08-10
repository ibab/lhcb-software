*CMZ :          29/01/08            by  Adrey Shamov
*  Vaccum polarization correction as in Berends's GG_MMGGEN
*CMZ :  1.00/00 23/11/92  16.42.01  by  Olaf Duenger
*CMZ :  2.00/00 23/11/92  16.26.49  by  Olaf Duenger
*CMZ :  1.00/00 02/04/92  19.17.32  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.26  by  O. Duenger
*CMZ :  0.00/00 10/01/92  15.43.01  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      FUNCTION F(X)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C---->
C---- OUR MODIFICATION / ADDITION
C----

c++++
      double precision function VacPolDelta
      integer mVacPol
      common /vacpol_cb/ mVacPol
      data mVacPol/0/           ! no correction by default
c++++

*KEEP,INPU.
      REAL*8       ME,MU,MP,MX,S,SQ,PE,PP,EE,EP,CONST,PI, my
      COMMON /INPU/ME,MU,MP,MX,S,SQ,PE,PP,EE,EP,CONST,PI, my

*KEEP,BEAM.
      INTEGER          INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,IPAIR,NQUARK
      REAL*8           INPE,INPP
      COMMON /BEAM/    INPE,INPP,INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,
     &                 IPAIR,NQUARK

*KEEP,CUTS.
      INTEGER      MODCUT
      REAL*4       THMAX,THMIN,MXMN,MXMX,Q2MN,Q2MX
      REAL*8       COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,QP2MIN,QP2MAX
      COMMON /CUTS/COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,
     &             THMAX,THMIN,QP2MIN,QP2MAX,MODCUT,MXMN,MXMX,Q2MN,Q2MX

*KEEP,XQCOM.
      INTEGER IUSEDF
      DOUBLE PRECISION XQ,EQ,PQ,MQ,
     &                 QSCALE,XDENS(-6:2),
     &                 PSEA,PVALD
      COMMON /XQCOM/   XQ,EQ,PQ,MQ,
     &                 QSCALE,XDENS,
     &                 PSEA,PVALD,IUSEDF

*KEEP,LTCOM.
      REAL*8          GAMMA,BETGAM
      COMMON /LTCOM/  GAMMA,BETGAM

*KEEP,VEGPAR.
      INTEGER          NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND
      COMMON /VEGPAR/  NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND

*KEND.
      COMMON /VARIAB/ E,E1,E2,E3,E4,E5,P,P3,P4,P5,CT3,ST3,CT4,ST4,CT5,
     &                ST5,CP3,SP3,CP5,SP5
      COMMON /VARIAD/ E6,E7,P6,P7,CT6,ST6,CT7,ST7,CP6,SP6,CP7,SP7,W
      COMMON /EXTRA/  S1,S2,T1,T2
C
      REAL*8  WX,WXMIN,WXMAX,MX2,MD2, wy
C PARAMETER FOR PDFLIB <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      REAL*8          XMIN,XMAX,Q2MIN,Q2MAX
      COMMON /W50513/ XMIN,XMAX,Q2MIN,Q2MAX

      REAL*8    MPI
      PARAMETER(MPI=0.1396D0)

      LOGICAL LCUT,LMU1,LMU2

      DIMENSION X(10)
C
      REAL*4 ULMASS
C
      DATA NCALL/0/,NDETCT/0/,NMXCUT/0/,NQ2CUT/0/,NPDFC/0/
      DATA NEXTW/1/
C
      NCALL=NCALL+1
C
      do i=1,NDIM
         if(x(i).gt.1.or.x(1).lt.0) print *,'F: I,X',i,x(i)
      enddo
c
      dwx=1.  ! weight factor for PMOD > 2
      mx=mp   ! default for MY
      dwy=1.  ! weight factor for EMOD > 2
      my=me   ! default for MY
c
      IF (PMOD .GE. 100) THEN
       IF (NQUARK.EQ.12) THEN
         MQ=DBLE(ULMASS(2))
       ELSE
         MQ=DBLE(ULMASS(NQUARK))
       ENDIF
       XQMIN =      4.0 * MU * MU / (S - MP * MP)
       XQMAX = (S - 2. * SQ * MP) / (S - MP * MP)
       CALL MAPXQ(XQ,X(8),XQMIN,XQMAX,DXQ)
       PQ  = XQ * PP
       EQ = DSQRT(PQ*PQ + MQ*MQ)
       ETOT  = EQ + EE
       PTOT  = PQ - PE
C
       SSQ = DSQRT(MQ * MQ + ME * ME + 2. * (EE * EQ + PE * PQ))
C
C  COMPUTING MAT.EL. FOR Q E -> Q E MU MU   <=========================
       CALL GAMGAM(SSQ,MQ,ME,MQ,ME,MU,MU,0.D+00,SQ,DJ,0,X,1)
C
      ELSEIF (PMOD .LE. 2 .and. EMOD .le.2 ) THEN
       ETOT  = EP + EE
       PTOT  = PP - PE
       SSQ = DSQRT(MP * MP + ME * ME + 2. * (EE * EP + PE * PP))
C
C  COMPUTING MAT.EL. FOR P E -> P E MU MU   <=========================
       CALL GAMGAM(SSQ,MP,ME,MP,ME,MU,MU,0.D+00,SQ,DJ,0,X,1)
C
      ELSEIF (PMOD .ge. 11 .and. PMOD .le. 17) THEN
       ETOT  = EP + EE
       PTOT  = PP - PE
       SSQ = DSQRT(MP * MP + ME * ME + 2. * (EE * EP + PE * PP))
       WXMIN = DMAX1((MP + MPI)**2,MXMIN2)
       WXMAX = DMIN1((SSQ - ME - 2*MU)**2,MXMAX2)
       CALL MAPWX(WX,X(8),WXMIN,WXMAX,DWX)
       MX=DSQRT(WX)
       if(EMOD .ge. 11 .and. EMOD .le. 17) then
         CALL MAPWX(WY,X(9),WXMIN,WXMAX,DWY)
         MY=DSQRT(WY)
       endif
C
C  COMPUTING MAT.EL. FOR P E -> X Y MU MU   <=========================
       CALL GAMGAM(SSQ,MP,ME,MX,MY,MU,MU,0.D+00,SQ,DJ,0,X,1)
C
      ELSEIF (EMOD .ge. 11 .and. EMOD .le. 17) THEN
       ETOT  = EP + EE
       PTOT  = PP - PE
       SSQ = DSQRT(MP * MP + ME * ME + 2. * (EE * EP + PE * PP))
       WXMIN = DMAX1((ME + MPI)**2,MXMIN2)
       WXMAX = DMIN1((SSQ - MP - 2*MU)**2,MXMAX2)
       CALL MAPWX(WY,X(8),WXMIN,WXMAX,DWY)
       MY=DSQRT(WY)
C
C  COMPUTING MAT.EL. FOR P E -> P Y MU MU   <=========================
       CALL GAMGAM(SSQ,MP,ME,MP,MY,MU,MU,0.D+00,SQ,DJ,0,X,1)
C
      ELSE
       WRITE(6,*) ' F(X) : WRONG PROTON MODE PMOD =',PMOD
       STOP
      ENDIF
      IF (DJ .EQ. 0D0) THEN
        F=0D0
        RETURN
      ENDIF
C
C  PARAMETER FOR LORENTZ TRANSFORMATION <===========================
       GAMMA = ETOT / SSQ
       BETGAM = PTOT / SSQ
C
C  COMPUTING SOME KIN. PARAMETER ONLY FOR THE CUTS     <=========
C
C  COMPUTING PT, PZ AND E OF THE MUON PAIR   <=========
      PT6  = P6 * ST6
      PT7  = P7 * ST7
      PZ6  = BETGAM* E6 + GAMMA * P6 * CT6
      PZ7  = BETGAM* E7 + GAMMA * P7 * CT7
      E6LAB= GAMMA * E6 + BETGAM* P6 * CT6
      E7LAB= GAMMA * E7 + BETGAM* P7 * CT7
C
C   STANDART CUT
      IF (MODCUT.EQ.2 .OR. MODCUT.EQ.3) THEN
C  COMPUTING COT(THETA) OF MUON PAIR OR SINGLE MUON  <=============
       COTT6= PZ6/PT6
       COTT7= PZ7/PT7
C
C  CUT IN THETA, PT AND E OF THE MUON PAIR <=====================
       IF (NCALL .EQ. 1)
     &  WRITE(6,*)'F : COTTH1 =',COTTH1,'  COTTH2 =',COTTH2,
     &           '  PTCUT =',PTCUT,'  ECUT =',ECUT
       LMU1 = (COTT6 .GE. COTTH1) .AND. (COTT6 .LE. COTTH2) .AND.
     &        (PT6 .GE. PTCUT) .AND.
     &        (E6LAB .GE. ECUT)
       LMU2 = (COTT7 .GE. COTTH1) .AND. (COTT7 .LE. COTTH2) .AND.
     &        (PT7 .GE. PTCUT) .AND.
     &        (E7LAB .GE. ECUT) 
       IF (MODCUT .EQ. 2) THEN
         LCUT = LMU1 .AND. LMU2
       ELSE
         LCUT = LMU1 .OR. LMU2
       ENDIF
C
      ELSEIF (MODCUT .EQ. 1) THEN
C   VERMASEREN HYPOTETICAL DETECTOR CUTS
       COST6=PZ6/DSQRT(PZ6**2+PT6**2)
       COST7=PZ7/DSQRT(PZ7**2+PT7**2)
       LCUT=(((ABS(COST6) .LE. 0.75D0) .AND. (PT6 .GE. 1D0))
     &       .OR.
     &       ((ABS(COST6) .LE. 0.95D0) .AND. (ABS(COST6) .GT. 0.75D0)
     &                            .AND. (ABS(PZ6) .GT. 1D0)))
     &     .AND.
     &      (((ABS(COST7) .LE. 0.75D0) .AND. (PT7 .GE. 1D0))
     &       .OR.
     &       ((ABS(COST7) .LE. 0.95D0) .AND. (ABS(COST7) .GT. 0.75D0)
     &                            .AND. (ABS(PZ7) .GT. 1D0)))
      ELSEIF (MODCUT .EQ. 0) THEN
       LCUT=.TRUE.
      elseif (MODCUT .eq. 10) then
          ptS=(Pt6*cp6+Pt7*cp7)**2+(Pt6*sp6+Pt7*sp7)**2
          Smass=(e6lab+e7lab)**2-(pz6+pz7)**2-ptS
          LCUT=Smass.gt.(2.*PTCUT)**2
      ELSE
       WRITE(6,*) ' F(X) : ILLEGAL CUT MODE ; MODCUT =',MODCUT
       STOP
      ENDIF
      IF (LCUT) NDETCT=NDETCT+1
CC
      IF (PMOD .GE. 100) THEN
        md2=mp**2+mq**2+2d0*pp*pq-2d0*ep*eq
        mx2=mq**2+md2+2d0*dsqrt((md2+(pp-pq)**2)*(mq**2+p3**2))
     &    -2d0*(pp-pq)*ct3*p3
        if (mx2 .gt. 0d0) then
          mx=dsqrt(mx2)
        else
          write(6,*)
     &    ' F: mx2 less then 0 ; mx2 =',mx2,' at call #',ncall
          mx=0d0
        endif
      ENDIF
C
C CUT ON MASS OFF FINAL HADRONIC SYSTEMs  (MX,my)
      IF ((PMOD .GT. 2) .AND.
     &    (MX*MX.LT.MXMIN2 .OR. MX*MX.GT.MXMAX2)) LCUT=.FALSE.
      IF ((EMOD .GT. 2) .AND.
     &    (MY*MY.LT.MXMIN2 .OR. MY*MY.GT.MXMAX2)) LCUT=.FALSE.
      IF (LCUT) NMXCUT=NMXCUT+1
C
C CUT ON THE PROTON Q**2 (T1)
      IF (T1.LT.QP2MAX .OR. T1.GT.QP2MIN) LCUT=.FALSE.
C
      IF (LCUT) THEN
       NQ2CUT=NQ2CUT+1
       IF (PMOD .GE. 100) THEN
C
C   GET QUARK CONTENT OF THE PROTON AND WEIGHT THE FUNCTION <==========
C
        QSCALE=-T1
        IF((QSCALE .LE. Q2MAX) .AND. (QSCALE .GE. Q2MIN).AND.
     &     (    XQ .LE.  XMAX) .AND. (    XQ .GE.  XMIN)) THEN

         NPDFC=NPDFC+1
C
         CALL PDF2PDG(XQ,DSQRT(QSCALE),XDENS)
         IF (NQUARK.EQ.2 .AND. PMOD.EQ.103) THEN
           FORMF = (XDENS(2)+XDENS(-2)*2D0)*4D0/9D0/XQ
           PVALD=0.0D0
           PSEA=XDENS(-2)/(XDENS(2)*0.5D0+XDENS(-2))
         ELSEIF (NQUARK.EQ.1 .AND. PMOD.EQ.103) THEN
           FORMF = (XDENS(1)+XDENS(-1)*2D0)/9D0/XQ
           PVALD=1.0D0
           PSEA=XDENS(-1)/(XDENS(1)*0.5D0+XDENS(-1))
         ELSEIF (NQUARK.EQ.12 .AND. PMOD.EQ.103) THEN
           FORMF = (XDENS(1)+XDENS(2)*4D0+XDENS(-1)*10D0)/9D0/XQ
           PVALD=XDENS(1)/(XDENS(2)*4.0D0+XDENS(1))
           PSEA=XDENS(-1)*10.0D0
     &            /(XDENS(2)*4.0D0+XDENS(1)+XDENS(-1)*10.0D0)
         ELSEIF (NQUARK.EQ.2 .AND. PMOD.EQ.101) THEN
           FORMF = XDENS(2)*4D0/9D0/XQ
           PVALD=0.0D0
           PSEA=0.0D0
         ELSEIF (NQUARK.EQ.1 .AND. PMOD.EQ.101) THEN
           FORMF = XDENS(1)/9D0/XQ
           PVALD=1.0D0
           PSEA=0.0D0
         ELSEIF (NQUARK.EQ.12 .AND. PMOD.EQ.101) THEN
           FORMF = (XDENS(1)+XDENS(2)*4D0)/9D0/XQ
           PVALD=XDENS(1)/(XDENS(2)*4.0D0+XDENS(1))
           PSEA=0.0D0
         ELSEIF ((NQUARK.EQ.1 .OR. NQUARK.EQ.3 .OR. NQUARK.EQ.5)
     &       .AND. PMOD.EQ.102) THEN
           FORMF = (XDENS(-NQUARK)*2D0)/9D0/XQ
           PVALD=0.0D0
           PSEA=1.0D0
         ELSEIF ((NQUARK.EQ.2 .OR. NQUARK.EQ.4).AND. PMOD.EQ.102) THEN
           FORMF = (XDENS(-NQUARK)*8D0)/9D0/XQ
           PVALD=0.0D0
           PSEA=1.0D0
         ELSE
           WRITE(6,*) 'F : WRONG QUARK NUMBER ; QPDF =',NQUARK,
     &     ' OR WRONG PROTON MODE FOR GIVEN QUARK NUMBER ; PMOD =',PMOD,
     &     '   PROGRAM STOPS !!!!!!'
           STOP
         ENDIF
        ELSE
         F=0D0
         RETURN
        ENDIF
C
        F = CONST * DJ * PERIPP(INTGP,INTGE) * DXQ * FORMF
C
       ELSEIF (PMOD .LE. 2 .and. EMOD .le. 2) THEN
C   IF PMOD=2,EMOD=2: ONLY USE THE FORMFACTOR IN PRIPP
        F = CONST * DJ * PERIPP(INTGP,INTGE)
       ELSE
C   IF PMOD or EMOD = 11:17
C   ONLY USE THE FORMFACTOR IN PRIPP AND DWX
        F = CONST * DJ * PERIPP(INTGP,INTGE) * DWX * dwy
       ENDIF
C
      ELSE
       F=0D0
      ENDIF
C
C   END CUT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
C
      IF (NCALL .GE. NEXTW) THEN
       WRITE(6,*) ' F(X) : NUMBER OF CALLS IS ',NCALL,
     &  '   CUTS SUCCEEDED :',NDETCT,NMXCUT,NQ2CUT,'(DET MX Q2)',
     &  '   PDF CALLED :',NPDFC
       NEXTW=NEXTW*2
      ENDIF
c++++
      if(mVacPol.ne.0) then
         F=F*(1.d0+VacPolDelta(t1)+VacPolDelta(t2))
      endif
c++++
      RETURN
      END
