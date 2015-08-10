*CMZ :          17/01/08            by A. Shamov
* 1) add PHOTOS call for dimuon
*CMZ :          30/12/07            by A. Shamov
* 1) use real *8 internally
* 2) fill /HEPEVT/ common
*CMZ :          04/08/95  20.04.35  by  ZEUS Offline Group
*CMZ :  1.00/00 18/08/94  22.10.14  by  ZEUS Offline Group
*-- Author :    ZEUS Offline Group   18/08/94
      SUBROUTINE GMUFIL
C
C   THIS SUBROUTINE SHOULD FILL THE GTR-BANK
C
      implicit none
c
      real*8  PI2
      PARAMETER (PI2=2*3.1415926535898)
*KEEP,INPU.
      REAL*8       ME,MU,MP,MX,S,SQ,PE,PP,EE,EP,CONST,PI, my
      COMMON /INPU/ME,MU,MP,MX,S,SQ,PE,PP,EE,EP,CONST,PI, my
      save /inpu/

*KEND.
      REAL*8         E,E1,E2,E3,E4,E5,P1,
     &                P3,P4,P5,CT3,ST3,CT4,ST4,CT5,
     &                ST5,CP3,SP3,CP5,SP5
      COMMON /VARIAB/E,E1,E2,E3,E4,E5,P1,
     &                P3,P4,P5,CT3,ST3,CT4,ST4,CT5,
     &                ST5,CP3,SP3,CP5,SP5
      REAL*8          E6,E7,P6,P7,CT6,ST6,CT7,ST7,CP6,SP6,CP7,SP7,W
      COMMON /VARIAD/ E6,E7,P6,P7,CT6,ST6,CT7,ST7,CP6,SP6,CP7,SP7,W
      REAL*8          S1,S2,T1,T2
      COMMON /EXTRA/  S1,S2,T1,T2
*KEEP,LTCOM.
      REAL*8          GAMMA,BETGAM
      COMMON /LTCOM/  GAMMA,BETGAM
      save /variab/,/variad/,/extra/

*KEEP,XQCOM.
      INTEGER IUSEDF
      DOUBLE PRECISION XQ,EQ,PQ,MQ,
     &                 QSCALE,XDENS(-6:2),
     &                 PSEA,PVALD
      COMMON /XQCOM/   XQ,EQ,PQ,MQ,
     &                 QSCALE,XDENS,
     &                 PSEA,PVALD,IUSEDF

*KEEP,BEAM.
      INTEGER          INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,IPAIR,NQUARK
      REAL*8           INPE,INPP
      COMMON /BEAM/    INPE,INPP,INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,
     &                 IPAIR,NQUARK
      save /beam/

*KEEP,KINVAR.
      REAL*8           GMUX,GMUY,GMUW,GMUNU
      COMMON /KINVAR/  GMUX,GMUY,GMUW,GMUNU
      save /kinvar/
*KEND.
      INTEGER NLIMAX
      PARAMETER (NLIMAX=13)
      real*8 PL(4,NLIMAX),PMXDA(4)
C
C   LUND COMMON <===================================================
      REAL*4        P(4000,5),V(4000,5)
      INTEGER       N,K(4000,5)
      COMMON/LUJETS/N,K,P,V
      save /lujets/
C
      include 'lpair_hepevt.inc'
C
      INTEGER NLINES
      REAL*8 PLAB(4,9)
      REAL*4 H1RN
      real*8 SINPHI,COSPHI,RANY,RANUDQ,RANPHI
C  INFORMATION FOR JETSET PACKAGE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      integer NJOIN
      PARAMETER(NJOIN=2)
      INTEGER JLPSF(NJOIN),JLVAL(NJOIN),JLSEA1(NJOIN),JLSEA2(NJOIN)
      DATA    JLPSF/10,11/
      DATA    JLVAL/2,7/
      DATA    JLSEA1/4,9/
      DATA    JLSEA2/2,3/
      LOGICAL LSEA,LVAL
      save jlpsf,jlval,jlsea1,jlsea2
*KEEP,PARTIC.
      INTEGER I2STAT(13),I2PART(13),
     &        I2MO1(13),I2DA1(13),I2DA2(13),I2LINE
      COMMON /PARTIC/  I2LINE,I2STAT,I2PART,I2MO1,I2DA1,I2DA2
*KEND.
C
      real*8  I2MASS(NLIMAX)
      DATA    I2MASS/NLIMAX*-9999.9/
C
      integer NCALL,NFRAC3,NTERM3
      DATA NCALL/0/
      DATA NFRAC3/0/
      DATA NTERM3/0/

      integer NEXTW
      common /gmufilcb/ NEXTW
      save /gmufilcb/

      save ncall,nfrac3,nterm3
      DATA NEXTW/1/
C
      real ULMASS,GMUSELX
c
      integer PHOTOSon
      real PHOTomegaMin
      common /photosonoff/ PHOTOSon,PHOTomegaMin
      data PHOTOSon/0/,PHOTomegaMin/2.e-3/
C
      REAL*8 phALPHA,phXPHCUT
      COMMON/PHOCOP/phALPHA,phXPHCUT
C
      integer kPHOT,IMU1,IMU2
      save kPHOT,IMU1,IMU2
      integer Nreal(2,2)
      double precision Preal(4,2,2)
      double precision Pvirt(4,2),vQ2(2),vQ2old(2)
      double precision CosA,CosAmx
      real dEmin,dE
      integer MOg,IPHO,IF,IHEP,JHEP,ICMF,NHEPold,LPRINT
C
      INTEGER NPOUT,IPDQ,i,j,NINIT,NFINAL,NFRACS,IPQ
      real PMXP,RANMXP,RANMXT,DUMMY
      double precision ULMDQ,ULMQ
C     
      NFRACS=0
C
      NLINES=9
      NFINAL=0
      NINIT =0

      if(nextw.le.1) print *,'entering GMUFIL'
C
C LORENZ TRANFORMATION AND COMPUTING OF INTRMED PARTICLES <======
C PARTICLE 1 = "PROTON"    <==================
      PLAB(1,1)=0.0
      PLAB(2,1)=0.0
      PLAB(3,1)=GAMMA*P1 + BETGAM*E1
      PLAB(4,1)=GAMMA*E1 + BETGAM*P1
C PARTICLE 2 = ELEKTRON IN <================
      PLAB(1,2)=0.0
      PLAB(2,2)=0.0
      PLAB(3,2)= -GAMMA*P1 + BETGAM*E2
      PLAB(4,2) =  GAMMA*E2 - BETGAM*P1
C PARTICLE 9 = ELEKTRON OUT <===============
      PLAB(1,9)=P5*ST5*CP5
      PLAB(2,9)=P5*ST5*SP5
      PLAB(3,9)=GAMMA*CT5*P5 + BETGAM  *  E5
      PLAB(4,9) =GAMMA  *  E5 + BETGAM*CT5*P5
C PARTICLE 4 = GAMMA_E   <==================
      PLAB(1,4)=-PLAB(1,9)
      PLAB(2,4)=-PLAB(2,9)
      PLAB(3,4)=PLAB(3,2)-PLAB(3,9)
      PLAB(4,4) =PLAB(4,2)-PLAB(4,9)
C PARTICLE 5 = QUARK OUT <==================
      PLAB(1,5)=P3*ST3*CP3
      PLAB(2,5)=P3*ST3*SP3
      PLAB(3,5)=GAMMA*CT3*P3 + BETGAM  *  E3
      PLAB(4,5) =GAMMA  *  E3 + BETGAM*CT3*P3
C PARTICLE 3 = GAMMA_P   <==================
      PLAB(1,3)=-PLAB(1,5)
      PLAB(2,3)=-PLAB(2,5)
      PLAB(3,3)=PLAB(3,1)-PLAB(3,5)
      PLAB(4,3) =PLAB(4,1)-PLAB(4,5)
C PARTICLE 6 = MUON1    <==================
      PLAB(1,6)=P6*ST6*CP6
      PLAB(2,6)=P6*ST6*SP6
      PLAB(3,6)=GAMMA*CT6*P6 + BETGAM  *  E6
      PLAB(4,6) =GAMMA  *  E6 + BETGAM*CT6*P6
C PARTICLE 7 = MUON1->2 <==================
      PLAB(1,7)=PLAB(1,3)-PLAB(1,6)
      PLAB(2,7)=PLAB(2,3)-PLAB(2,6)
      PLAB(3,7)=PLAB(3,3)-PLAB(3,6)
      PLAB(4,7) =PLAB(4,3)-PLAB(4,6)
C PARTICLE 8 = MUON2     <==================
      PLAB(1,8)=P7*ST7*CP7
      PLAB(2,8)=P7*ST7*SP7
      PLAB(3,8)=GAMMA*CT7*P7 + BETGAM  *  E7
      PLAB(4,8) =GAMMA  *  E7 + BETGAM*CT7*P7
C====> SET KINEMATIC VARIABLES FOR GKI   <============
       GMUX= -T2 /(EP*PLAB(4,4)-PP*PLAB(3,4))/2.0D0
       GMUY= (EP*PLAB(4,4)-PP*PLAB(3,4))/
     &       (EE*PLAB(4,4)+PE*PLAB(3,4))
!       GMUW = (EP+PLAB(4,4))**2 - (PP+PLAB(3,4))**2
       gmuw =((EP-PP)+(PLAB(4,4)-PLAB(3,4)))*
     &                  ((EP+PP)+(PLAB(4,4)+PLAB(3,4)))
       IF (GMUW .GE. 0) THEN
        GMUW=SQRT(GMUW)
       ELSE
        WRITE(6,*) ' GMUFIL : NEGATIV W**2 COMPUTED : GENW**2 = ', GMUW,
     &             ' GENW IS SET TO 0 '
        print *,'plab(1)',(plab(j,1),j=1,4)
        print *,'plab(9)',(plab(j,9),j=1,4)
        print *,'gamma',gamma,betgam
        print *,'p,e,s,c',p5,e5,st5,cp5
        GMUW=0.0
       ENDIF
       GMUNU= GMUY*2.0*ULMASS(2212)/EP/EE
C===> RANDOM REFLECTION AT XZ-PLAIN <==================
      IF (H1RN(DUMMY)  .GE. 0.5) THEN
       RANY=-1.0
      ELSE
       RANY=1.0
      ENDIF
C===> RANDOM ROTATION AT Z-AXIS <=====================
      RANPHI=PI2*H1RN(DUMMY)
      SINPHI=SIN(RANPHI)
      COSPHI=COS(RANPHI)
C====> ROTATE, REFELECT AND TRANSFORM TO REAL*4 VALUES <=============
      DO 100 I=1,9
        PL(1,I) =  PLAB(1,I)*COSPHI + RANY*PLAB(2,I)*SINPHI
        PL(2,I) = -PLAB(1,I)*SINPHI + RANY*PLAB(2,I)*COSPHI
        PL(3,I) = SNGL(PLAB(3,I))
        PL(4,I) = SNGL(PLAB(4,I))
 100  CONTINUE
C===> RANDOM DISTRIBUTION OF LEPTON+ AND LEPTON- <===========
      IF (H1RN(0.0) .LT. 0.5) THEN
       I2PART(6) = IPAIR
       I2PART(7) =-IPAIR
       I2PART(8) =-IPAIR
      ELSE
       I2PART(6) =-IPAIR
       I2PART(7) = IPAIR
       I2PART(8) = IPAIR
      ENDIF
C===> SELECTION OF HADRON MODE IN PARTON MODEL <================
      LVAL=.FALSE.
      LSEA=.FALSE.
      IF (PMOD .EQ. 101) THEN
         LVAL=.TRUE.
      ELSEIF (PMOD .EQ. 102) THEN
         LSEA=.TRUE.
      ELSEIF (PMOD .EQ. 103) THEN
         IF (H1RN(DUMMY) .GT. PSEA) THEN
            LVAL=.TRUE.
         ELSE
            LSEA=.TRUE.
         ENDIF
      ENDIF
C===> add. Particles for Val.Quark scatering in Parton model <=====
      IF (LVAL) THEN
       NINIT =2
C===> ADD INITIAL PROTON <====
       I2STAT(10)=21
       I2PART(10)=2212
       I2MO1(10)=0
       I2DA1(10)=2
       I2DA2(10)=3
C====> CORRECT PART 1 <===
       I2MO1(1)=10
C====> ADD DIQUARK  <====
       I2STAT(11)=1
       I2MO1(11)=1
       I2DA1(11)=0
       I2DA2(11)=0
C===> RANDOM SELECTION OF U AND D QUARKS <==
       RANUDQ=H1RN(DUMMY)
       IF (RANUDQ .LT. PVALD) THEN
        I2PART(1)=1
        I2PART(11)=2203
        I2PART(5)=1
       ELSEIF (RANUDQ .LT. 0.5+0.5*PVALD) THEN
        I2PART(1)=2
        I2PART(11)=2101
        I2PART(5)=2
       ELSE
        I2PART(1)=2
        I2PART(11)=2103
        I2PART(5)=2
       ENDIF
       IUSEDF=I2PART(1)
C==> SET MASSES <=============
       I2MASS(1)=MQ
       I2MASS(11)=ULMASS(I2PART(11))
       I2MASS(10)=ULMASS(2212)
C==> SET MOMENTA <============
       PL(1,10)=0.0
       PL(2,10)=0.0
       PL(3,10)=SNGL(PP)
       PL(4,10)=SNGL(EP)
       PL(1,11)=0.0
       PL(2,11)=0.0
       PL(3,11)=SNGL(PP-PQ)
       PL(4,11)=PL(4,10)-PL(4,1)
C???> computed energy may be in conflict to mass !!!!  ????????
C
C===> add. Particles for SEA Quark scatering in Parton model <=====
      ELSEIF (LSEA) THEN
       NINIT =4
C===> ADD INITIAL PROTON <====
       I2STAT(10)=21
       I2PART(10)=2212
       I2MO1(10)=0
       I2DA1(10)=2
       I2DA2(10)=5
C====> ADD PART CODE FOR ADD PARTICLES <======
       DO 234 I=11,13
          I2DA1(I)=0
          I2DA2(I)=0
          I2MO1(I)=1
          I2STAT(I)=1
 234   CONTINUE
C====> CORRECT PART 1<===
       I2MO1(1)=1
C===> SET SCATTERED QUARK AND HIS ANTI QUARK <==
       IF (NQUARK .NE. 12) THEN
          I2PART(1)=NQUARK
       ELSE
          IF (H1RN(DUMMY) .LT. 0.2) THEN
             I2PART(1)=1
          ELSE
             I2PART(1)=2
          ENDIF
       ENDIF
       IUSEDF=I2PART(1)
       IF (H1RN(DUMMY) .LE. 0.5) THEN
          I2PART(12)=-I2PART(1)
       ELSE
          I2PART(12)=I2PART(1)
          I2PART(1)=-I2PART(1)
       ENDIF
       I2PART(5)=I2PART(1)
C===> ADD QUARK AND DIQUARK FROM P <==
       IF (I2PART(1) .LT. 0) THEN
         IPQ=13
         IPDQ=11
       ELSE
         IPQ=11
         IPDQ=13
       ENDIF
       RANUDQ=H1RN(DUMMY)
       IF (RANUDQ .LT. 1.0/3.0) THEN
        I2PART(IPQ)=1
        I2PART(IPDQ)=2203
       ELSEIF (RANUDQ .LT. 2.0/3.0) THEN
        I2PART(IPQ)=2
        I2PART(IPDQ)=2101
       ELSE
        I2PART(IPQ)=2
        I2PART(IPDQ)=2103
       ENDIF
C==> SET MASSES <=============
       I2MASS(1)=MQ
       I2MASS(5)=MQ
       I2MASS(12)=MQ
       I2MASS(13)=ULMASS(I2PART(13))
       I2MASS(11)=ULMASS(I2PART(11))
       I2MASS(10)=ULMASS(2212)
C==> SET MOMENTA <=============
       PL(1,10)=0.0
       PL(2,10)=0.0
       PL(3,10)=SNGL(PP)
       PL(4,10)=SNGL(EP)
       PL(1,12)=0.0
       PL(2,12)=0.0
       PL(3,12)=GMUSELX(-IABS(IUSEDF),QSCALE)*PP
       PL(4,12)=SQRT(PL(3,12)**2+I2MASS(12)**2)
       PL(1,IPQ)=0.0
       PL(2,IPQ)=0.0
       PL(3,IPQ)=GMUSELX(I2PART(IPQ),QSCALE)*PP
       PL(4,IPQ)=SQRT(PL(3,IPQ)**2+I2MASS(IPQ)**2)
       PL(1,IPDQ)=0.0
       PL(2,IPDQ)=0.0
       PL(3,IPDQ)=PL(3,10)-PL(3,1)-PL(3,IPQ)-PL(3,12)
       PL(4,IPDQ)=PL(4,10)-PL(4,1)-PL(4,IPQ)-PL(4,12)
      ENDIF
C
C    FOR INELASTIC MODE WITH STRUCTURE FUNCTIONS BUILD <========
C    HADRONIC SYSTEM USING LUND SHOWER MC.
C
      if (EMOD.GE.10 .AND. EMOD.LE.99) then
        I2MASS(9)=MY
      endif
C
      IF (PMOD.GE.10 .AND. PMOD.LE.99) THEN
       NFINAL=2
C====> INSERT THE MASS OF THE HADRONIC SYSTEM <==================
       I2MASS(5)=MX
C===> RANDOM SELECTION OF U , D AND DI QUARKS <===========
       RANUDQ=H1RN(DUMMY)
       IF (RANUDQ .LT. 1.0/9.0) THEN
        I2PART(10)=1
        I2PART(11)=2203
       ELSEIF (RANUDQ .LT. 5.0/9.0) THEN
        I2PART(10)=2
        I2PART(11)=2101
       ELSE
        I2PART(10)=2
        I2PART(11)=2103
       ENDIF
       ULMDQ=ULMASS(I2PART(11))
       ULMQ =ULMASS(I2PART(10))
C====> SET OF LUND CODES <====================================
       I2MO1(10)=5
       I2DA1(10)=0
       I2DA2(10)=0
       I2STAT(10)=1
       I2MO1(11)=5
       I2DA1(11)=0
       I2DA2(11)=0
       I2STAT(11)=1
C====> CHOOSE RANDOM DIRECTION IN MX FRAME <===================
       RANMXP=PI2*H1RN(DUMMY)
       RANMXT=ACOS(2.0*H1RN(DUMMY)-1.0)
C====> COMPUTE MOMENTUM OF DECAY PARTICLE FROM MX <=============
       PMXP=DSQRT((MX**2-ULMDQ**2+ULMQ**2)**2/4.0/MX/MX - ULMQ**2 )
C=====> BUILD 4-VECTORS AND BOOST DECAY PARTICLES <===============
       PMXDA(1)=SIN(RANMXT)*COS(RANMXP)*PMXP
       PMXDA(2)=SIN(RANMXT)*SIN(RANMXP)*PMXP
       PMXDA(3)=COS(RANMXT)*PMXP
       PMXDA(4)=SQRT(PMXP**2+ULMDQ**2)
       call LORENB8(I2MASS(5),PL(1,5),PMXDA(1),PL(1,11))
       PMXDA(1)=-PMXDA(1)
       PMXDA(2)=-PMXDA(2)
       PMXDA(3)=-PMXDA(3)
       PMXDA(4)=SQRT(PMXP**2+ULMQ**2)
*       WRITE(6,*) ' GMUFIL : E OF QUARK AND DIQUARK BEFOR LB:'
*     &   ,PMXDA(4)+EDQPR
       call LORENB8(I2MASS(5),PL(1,5),PMXDA(1),PL(1,10))
      ENDIF
C====> PREPARE THE LUNT COMMON <================================
 10   CONTINUE
      call LUNSET_HEP(NLINES+NINIT+NFINAL)
C ====> FILLING THE LUND COMMON <================================
      DO 200 I=1+NLINES,NINIT+NLINES
C SET MOTHER/DAUGTER VALUES, MARKING PARTICLES AS DECAYED <=======
        call LUKSET_HEP(I-NLINES,I2STAT(I),I2PART(I),
     &       I2MO1(I),I2DA1(I),I2DA2(I),0)
C SET PULS, ENERGY AND MASS OFF THE PARTICLES <==================
        call LUPSET_HEP(I-NLINES,PL(1,I),PL(2,I),PL(3,I),
     &       PL(4,I),I2MASS(I))
 200  CONTINUE
      DO 201 I=1,NLINES+NFINAL
C SET MOTHER/DAUGTER VALUES, MARKING PARTICLES AS DECAYED <=======
        call LUKSET_HEP(I+NINIT,I2STAT(I),I2PART(I),
     &       I2MO1(I),I2DA1(I),I2DA2(I),NINIT)
C SET PULS, ENERGY AND MASS OFF THE PARTICLES <==================
        call LUPSET_HEP(I+NINIT,PL(1,I),PL(2,I),PL(3,I),
     &       PL(4,I),I2MASS(I))
 201  CONTINUE
C PUTTING QUARK AND DIQUARK TO A COLOR SINGLET <======================
      IF (LVAL) CALL LUJOIN(NJOIN,JLVAL)
      IF (LSEA) THEN
         CALL LUJOIN(NJOIN,JLSEA1)
         CALL LUJOIN(NJOIN,JLSEA2)
      ENDIF
c
c Using PYTHIA's model or LPAIR's own model
      IF (PMOD.GE.10 .AND. PMOD.LE.19 .or.
     &          EMOD.GE.10 .AND. EMOD.LE.19) then
         N=9
         if(pmod.ge.10.and.pmod.le.19) then
            call ZMDIFF(5)
         endif
         if(emod.ge.10.and.emod.le.19) then
            call ZMDIFF(9)
         endif
      else
         if(pmod.gt.2.or.emod.gt.2) then
            CALL LUJOIN(NJOIN,JLPSF)
         endif
      endif
c
C EXECUTE LUND FRAGMENTATION PROGRAM  <==============================
      if(nextw.le.1) print *,'calling LUEXEC'
      CALL LUEXEC
C Check wether the Hadronic system is inelastic  <===================
      IF (PMOD.GE.10 .AND. PMOD.LE.99) THEN
        NPOUT=0
        DO 300 I=1,N
          IF (K(I,1) .EQ. 1) NPOUT=NPOUT+1
 300    CONTINUE
        NFRAC3=NFRAC3+1
        NFRACS=NFRACS+1
        IF (NPOUT .EQ. 4 .AND. NFRACS .LE. 1000) GOTO 10
        IF (NFRACS .GT. 1000) NTERM3=NTERM3+1
      ENDIF
      NCALL=NCALL+1
      if(PHOTOSon.lt.-10) then
         Lprint=1
      else
         Lprint=0
      endif
      IF (NCALL .GE. NEXTW) THEN
       Lprint=1
       IF (PMOD.GE.10 .AND. PMOD.LE.99) THEN
        WRITE(6,*) ' GMUFIL : NUMBER OF CALLS IS ',NCALL
     &  ,'  PMOD 10-99:  # FRAC TRY :',NFRAC3,'  # FRAC TERM :',NTERM3
       ELSE
        WRITE(6,*) ' GMUFIL : NUMBER OF CALLS IS ',NCALL
       ENDIF
       NEXTW=NEXTW*2
      ENDIF
c
c   call PHOTOS to simulate radiation off muons
      if(PHOTOSon.ne.0) then
         IF(PHOTOSon.gt.0) then
            print *,'------------------------- '
            print *,'PHOTOS initialization'
            call PHOINI
            print *,'Don''t bother about XPHCUT'
            print *,'--------------------------'
            kPHOT=mod(PHOTOSon,10)
            IMU1=6
            IMU2=8
            PHOTOSon=-PHOTOSon
         endif
         NHEPold=NHEP
c     PHOTOS for muons
         if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
            print *
            print *,'Event',NCALL,'-----------------------'
         endif
         if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
            print *,'mu-in ',IMU1,': P',(PHEP(j,IMU1),j=1,5)
            print *,'mu-in ',IMU2,': P',(PHEP(j,IMU2),j=1,5)
         endif
         if(kPHOT.ne.2) then
            NHEP=NHEP+1         ! photon CMS
            ICMF=NHEP
            do i=1,4
               PHEP(i,ICMF)=PHEP(i,IMU1)+PHEP(i,IMU2)
               VHEP(i,ICMF)=0
            enddo
            PHEP(5,ICMF)=sqrt((PHEP(4,ICMF)**2-
     &           PHEP(3,ICMF)**2)-(PHEP(1,ICMF)**2+
     &           PHEP(2,ICMF)**2))
            JMOHEP(1,ICMF)=6
            JMOHEP(2,ICMF)=8
            IDHEP(ICMF)=36
            ISTHEP(ICMF)=21
            JDAHEP(1,ICMF)=ICMF+1
            JDAHEP(2,ICMF)=ICMF+2
            if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
               print *,'cmf',ICMF,' mu',IMU1,IMU2
               print *,'cmf ',ICMF,': P',(PHEP(j,ICMF),j=1,5)
            endif
            do IHEP=IMU1,IMU2,IMU2-IMU1 ! muons
               K(IHEP,1)=21
               ISTHEP(IHEP)=21
               NHEP=NHEP+1
               do i=1,4
                  PHEP(i,NHEP)=PHEP(i,IHEP)
                  VHEP(i,NHEP)=0
               enddo
               PHEP(5,NHEP)=PHEP(5,IHEP)
               IDHEP(NHEP)=IDHEP(IHEP)
               ISTHEP(NHEP)=1
               JMOHEP(1,NHEP)=ICMF
               JMOHEP(2,NHEP)=0
               JDAHEP(1,NHEP)=0
               JDAHEP(2,NHEP)=0
            enddo
            phXPHCUT=PHOTomegaMin/max(PHEP(4,IMU1),PHEP(4,IMU2))
            call PHOTOS(-ICMF)
            if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
               do IHEP=ICMF+1,ICMF+2
                  print *,'mu-out',IHEP,': P',(PHEP(j,IHEP),j=1,5)
               enddo
               do IHEP=ICMF+3,NHEP
                  print *,'new',IHEP,' KF',IDHEP(IHEP),
     &                 ': P',(PHEP(j,IHEP),j=1,5)
               enddo
            endif
         endif
c      PHOTOS for protons (just check)
         if(kPHOT.ge.2) then
            do 9090 i=1,2       ! protons
               do IF=1,2        ! clear real photon data
                  Nreal(IF,i)=0
                  do j=1,4
                     Preal(j,IF,i)=0
                  enddo
               enddo
               ICMF=NHEP+1
               do j=1,5         ! initial proton -> antiproton
                  PHEP(j,ICMF+1)=PHEP(j,i)
               enddo
               PHEP(4,ICMF+1)=sqrt((PHEP(1,ICMF+1)**2+
     &              PHEP(2,ICMF+1)**2+PHEP(5,ICMF+1)**2)+
     &              PHEP(3,ICMF+1)**2)
               JMOHEP(1,ICMF+1)=ICMF
               JMOHEP(2,ICMF+1)=0
               JDAHEP(1,ICMF+1)=0
               JDAHEP(2,ICMF+1)=0
               IDHEP(ICMF+1)=-IDHEP(i)
               ISTHEP(ICMF+1)=1
               if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                  print *,'p-in ',ICMF+1,': P',
     &                 (PHEP(j,ICMF+1),j=1,5),
     &                 ' D',PHEP(4,ICMF+1)-abs(PHEP(3,ICMF+1))
               endif
c
               do j=1,5         ! final proton
                  PHEP(j,ICMF+2)=PHEP(j,JDAHEP(2,i))
               enddo
               PHEP(4,ICMF+2)=sqrt((PHEP(1,ICMF+2)**2+
     &              PHEP(2,ICMF+2)**2+PHEP(5,ICMF+2)**2)+
     &              PHEP(3,ICMF+2)**2)
               JMOHEP(1,ICMF+2)=ICMF
               JMOHEP(2,ICMF+2)=0
               JDAHEP(1,ICMF+2)=0
               JDAHEP(2,ICMF+2)=0
               IDHEP(ICMF+2)=IDHEP(JDAHEP(2,i))
               ISTHEP(ICMF+2)=1
               if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                  print *,'p-in ',ICMF+2,': P',
     &                 (PHEP(j,ICMF+2),j=1,5),
     &                 ' D',PHEP(4,ICMF+2)-abs(PHEP(3,ICMF+2))
               endif
c
               do j=1,4         ! CMF
                  PHEP(j,ICMF)=PHEP(j,ICMF+1)+
     &                 PHEP(j,ICMF+2)
               enddo
               PHEP(5,ICMF)=sqrt((PHEP(4,ICMF)-PHEP(3,ICMF))*
     *              (PHEP(4,ICMF)+PHEP(3,ICMF))-
     &              (PHEP(1,ICMF)**2+
     &              PHEP(2,ICMF)**2))
               JMOHEP(1,ICMF)=0
               JMOHEP(2,ICMF)=0
               JDAHEP(1,ICMF)=ICMF+1
               JDAHEP(2,ICMF)=ICMF+2
               IDHEP(ICMF)=36
               ISTHEP(ICMF)=21
               if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                  print *,'cmf  ',ICMF,': P',(PHEP(j,ICMF),j=1,5)
               endif
               NHEP=ICMF+2
c
               phXPHCUT=PHOTomegaMin/max(EE,EP)
               call PHOTOS(-ICMF)
               if(NHEP.LT.ICMF+3) then ! no photons
                  if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                     print *,'PHOTOS, nhep',NHEP,' icmf',ICMF
                  endif
                  NHEP=ICMF-1
                  goto 9090
               endif
               do IHEP=ICMF+1,NHEP ! check final particles
                  if(PHEP(1,IHEP).lt.0.d0) then
                  elseif(PHEP(1,IHEP).ge.0.d0) then
                  else          ! NaN
                     print *,'PHOTOS ERROR: IHEP',IHEP,' ID',
     &                    IDHEP(IHEP),' P',(PHEP(j,IHEP),j=1,4)
                     goto 9090
                  endif
               enddo
               if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                  print *,'PHOTOS, NHEP',NHEP
                  do IHEP=ICMF+1,ICMF+2
                     print *,'p-out',IHEP,': P',(PHEP(j,IHEP),j=1,5)
                  enddo
                  do IHEP=ICMF+3,NHEP
                     print *,'new',IHEP,' KF',IDHEP(IHEP),
     &                    ': P',(PHEP(j,IHEP),j=1,5)
                  enddo
               endif
               do JHEP=ICMF+3,NHEP ! check photons
                  dEmin=1.e10
                  CosAmx=-2.
                  do IHEP=ICMF+1,ICMF+2
                     ISTHEP(IHEP)=21
                     CosA=(PHEP(1,JHEP)*PHEP(1,IHEP)+
     &                    PHEP(2,JHEP)*PHEP(2,IHEP)+
     &                    PHEP(3,JHEP)*PHEP(3,IHEP))/
     &                    PHEP(4,JHEP)/sqrt(PHEP(1,IHEP)**2+
     &                    PHEP(2,IHEP)**2+PHEP(3,IHEP)**2)
                     dE=PHEP(4,JHEP)+PHEP(4,iHEP)-PHEP(4,i)
                     if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                        print *,'dE',dE,' CosA',CosA,' IJ',IHEP,JHEP
                     endif
                     if(CosAmx.lt.CosA) then
                        MOg=IHEP
                        CosAmx=CosA
                        dEmin=abs(dE)
                     endif
                  enddo         ! I/F photons
                  IF=MOg-ICMF   ! Initial/Final
                  if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                     print *,'emitter',MOg,' CosAmx',CosAmx,
     &                    ' I/F',IF
                  endif
                  Nreal(IF,i)=Nreal(IF,i)+1
                  do j=1,4
                     Preal(j,IF,i)=Preal(j,IF,i)+PHEP(j,JHEP)
                  enddo
               enddo            ! real photons
               if(Nreal(1,i).ne.0) then ! Initial state radiation
                  IHEP=JDAHEP(2,i) ! modify final proton momentum
                  do j=1,4
                     PHEP(j,IHEP)=PHEP(j,IHEP)-Preal(j,1,i)
                  enddo
                  do IHEP=IMU1,IMU2,IMU2-IMU1 ! modify muon momenta
                     do j=1,4
                        PHEP(j,IHEP)=PHEP(j,IHEP)-Preal(j,1,i)
                     enddo
                     if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                        print *,'mu-out',IHEP,': P',
     &                       (PHEP(j,IHEP),j=1,5)
                     endif
                  enddo
               endif
               if(Nreal(2,i).ne.0) then ! Final state radiation
                  IHEP=JDAHEP(2,i) ! modify final proton momentum
                  do j=1,4
                     PHEP(j,IHEP)=PHEP(j,IHEP)-Preal(j,IF,i)
                  enddo
               endif
               do j=1,4
                  Pvirt(j,i)=(PHEP(j,i)-Preal(j,1,i))-
     &                 (PHEP(j,JDAHEP(2,i))+Preal(j,2,i))
               enddo
               IPHO=2+i
               vQ2(i)=-((Pvirt(4,i)-Pvirt(3,i))*
     &              (Pvirt(4,i)+Pvirt(3,i))-
     &              (Pvirt(1,i)**2+Pvirt(2,i)**2))
               vQ2old(i)=-((PHEP(4,IPHO)-PHEP(3,IPHO))*
     &              (PHEP(4,IPHO)+PHEP(3,IPHO))-
     &              (PHEP(1,IPHO)**2+PHEP(2,IPHO)**2))
               if(PHOTOSon.lt.-10.and.Lprint.ne.0) then
                  print *,'Pvirt',(Pvirt(j,i),j=1,4),
     &                 ' D',Pvirt(4,i)-abs(Pvirt(3,i))
                  print *,'PvOld',(PHEP(j,IPHO),j=1,4),
     &                 ' D',PHEP(4,IPHO)-abs(PHEP(3,IPHO))
                  print *,'Q2',vQ2(i),' old',vQ2old(i),
     &                 ' IPHO',IPHO
               endif
 9090       enddo               ! i, proton
         endif
c      Update /LUJETS/
         do i=NHEPold+1,NHEP
            K(i,1)=ISTHEP(i)
            K(i,2)=IDHEP(i)
            K(i,3)=JMOHEP(1,i)
            K(i,4)=JDAHEP(1,i)
            K(i,5)=JDAHEP(2,i)
            do j=1,5
               V(i,j)=0
               P(i,j)=PHEP(j,i)
            enddo
            N=NHEP
         enddo
      endif
c
      if(Lprint.ne.0) CALL LULIST(1)
c
      END
