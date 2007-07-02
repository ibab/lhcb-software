c-------------------------------------------------------------------
      subroutine AHspar
c     set list of parameters types and assign default values
c-------------------------------------------------------------------
      implicit none
      INCLUDE 'alpsho.inc'
c
      ih2=parval(2)
      ebeam=parval(3)
      ndns=parval(4)
      ickkw=parval(7)
      ihvy=parval(11)
      ihvy2=parval(12)
      nw=parval(13)
      nz=parval(14)
      nh=parval(15)
      nph=parval(16)
      ptjmin=parval(30)
      ptbmin=parval(31)
      ptcmin=parval(32)
      ptlmin=parval(33)
      metmin=parval(34)
      ptphmin=parval(35)
      etajmax=parval(40)
      etabmax=parval(41)
      etacmax=parval(42)
      etalmax=parval(43)
      etaphmax=parval(44)
      drjmin=parval(50)
      drbmin=parval(51)
      drcmin=parval(52)
      drlmin=parval(55)
      drphjmin=parval(56)
      drphlmin=parval(57)
      drphmin=parval(58)
      mllmin=parval(61)
      mllmax=parval(62)
      itopprc=parval(102)
c
      end

CDECK  ID>, STRNUM.
*CMZ :-        -13/02/02  07.20.46  by  Peter Richardson
*-- Author :    Michelangelo Mangano
C----------------------------------------------------------------------
      subroutine strnumH(string,num)
C----------------------------------------------------------------------
c- writes the number num on the string string starting at the blank
c- following the last non-blank character
C----------------------------------------------------------------------
      character * (*) string
      character * 20 tmp
      l = len(string)
      write(tmp,'(i15)')num
      j=1
      dowhile(tmp(j:j).eq.' ')
        j=j+1
      enddo
      ipos = istrlH(string)
      ito = ipos+1+(15-j)
      if(ito.gt.l) then
         write(*,*)'error, string too short'
         write(*,*) string
         stop
      endif
      string(ipos+1:ito)=tmp(j:)
      end

      function istrlH(string)
c returns the position of the last non-blank character in string
      character * (*) string
      i = len(string)
      dowhile(i.gt.0.and.string(i:i).eq.' ')
         i=i-1
      enddo
      istrlH= i
      end
CDECK  ID>, STRCATH.
*CMZ :-        -13/02/02  07.20.46  by  Peter Richardson
*-- Author :    Michelangelo Mangano
C----------------------------------------------------------------------
      subroutine strcatH(str1,str2,str)
C----------------------------------------------------------------------
c concatenates str1 and str2 into str. Ignores trailing blanks of str1,str2
C----------------------------------------------------------------------
      character *(*) str1,str2,str
      l1=istrlH(str1)
      l2=istrlH(str2)
      l =len(str)
      if(l.lt.l1+l2) then
        write(*,*) str1,str2
          write(*,*) 'error: l1+l2>l in strcatH'
          write(*,*) 'l1=',l1,' str1=',str1
          write(*,*) 'l2=',l2,' str2=',str2
          write(*,*) 'l=',l
          stop
      endif
      if(l1.ne.0) str(1:l1)=str1(1:l1)
      if(l2.ne.0) str(l1+1:l1+l2)=str2(1:l2)
      if(l1+l2+1.le.l) str(l1+l2+1:l)= ' '
      end
c-------------------------------------------------------------------
      subroutine pdfconvH(nin,nout,type)
c-------------------------------------------------------------------
c converts ALPHA convention for PDF namings to hvqpdf conventions
      implicit none
      integer nin,nout
      character*25 type
      character*25 pdftyp(20,2)
      data pdftyp/
c cteq sets
     $     'CTEQ4M ','CTEQ4L ','CTEQ4HJ',
     $     'CTEQ5M ','CTEQ5L ','CTEQ5HJ',
     $     'CTEQ6M ','CTEQ6L ',12*' ',
C MRST SETS
     $     'MRST99 ',
     $     'MRST01; as=0.119','MRST01; as=0.117','MRST01; as=0.121'
     $     ,'MRST01J; as=0.121','MRST02LO',14*' '/
      integer pdfmap(20,2)
      data pdfmap/
     $   81,83,88,   101,103, 104,   131,133, 12*0,
     $  111,  185,186,187,188,189,   14*0/
c
      nout=pdfmap(mod(nin ,100),1+nin /100)
      type=pdftyp(mod(nin ,100),1+nin /100)
      
      end

C-----------------------------------------------------------------------
C----Calorimeter simulation obtained from Frank Paige 23 March 1988-----
C
C          USE
C
C     CALL CALINI_M
C     CALL CALSIM_M
C
C          THEN TO FIND JETS WITH A SIMPLIFIED VERSION OF THE UA1 JET
C          ALGORITHM WITH JET RADIUS RJET AND MINIMUM SCALAR TRANSVERSE
C          ENERGY EJCUT
C            (RJET=1., EJCUT=5. FOR UA1)
C          USE
C
C     CALL GETJET_M(RJET,EJCUT)
C
C
C-----------------------------------------------------------------------
C 
C          ADDED BY MIKE SEYMOUR: PARTON-LEVEL CALORIMETER. ALL PARTONS
C          ARE CONSIDERED TO BE HADRONS, SO IN FACT RESEM IS IGNORED
C
C     CALL CALPAR
C
C          HARD PARTICLE CALORIMETER. ONLY USES THOSE PARTICLES WHICH
C          CAME FROM THE HARD PROCESS, AND NOT THE UNDERLYING EVENT
C
C     CALL CALHAR
C
C-----------------------------------------------------------------------
      SUBROUTINE CALINI_M
C                
C          INITIALIZE CALORIMETER FOR CALSIM_M AND GETJET_M.  NOTE THAT
C          BECAUSE THE INITIALIZATION IS SEPARATE, CALSIM_M CAN BE
C          CALLED MORE THAN ONCE TO SIMULATE PILEUP OF SEVERAL EVENTS.
C
      IMPLICIT NONE
      INTEGER NCY,NCPHI,NJMAX,IPHI,IY,JETNO,NCJET
      DOUBLE PRECISION YCMIN,YCMAX,PI,ET,DELPHI,PHIX,CPHCAL,SPHCAL,DELY,
     &  YX,THX,CTHCAL,STHCAL,PCJET,ETJET
      PARAMETER (NCY=100)
      PARAMETER (NCPHI=60,PI=3.141593D0)
      COMMON/CALOR_M/DELY,DELPHI,ET(NCY,NCPHI),
     $CTHCAL(NCY),STHCAL(NCY),CPHCAL(NCPHI),SPHCAL(NCPHI),YCMIN,YCMAX
      PARAMETER (NJMAX=500)
      COMMON/GETCOM_M/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(NCY,NCPHI),NCJET
      LOGICAL FSTCAL
      DATA FSTCAL/.TRUE./
C
C          INITIALIZE ET ARRAY.
      DO 100 IPHI=1,NCPHI
      DO 100 IY=1,NCY
100   ET(IY,IPHI)=0.
C
      IF (FSTCAL) THEN
C          CALCULATE TRIG. FUNCTIONS.
        DELPHI=2.*PI/FLOAT(NCPHI)
        DO 200 IPHI=1,NCPHI
        PHIX=DELPHI*(IPHI-.5)
        CPHCAL(IPHI)=COS(PHIX)
        SPHCAL(IPHI)=SIN(PHIX)
200     CONTINUE
        DELY=(YCMAX-YCMIN)/FLOAT(NCY)
        DO 300 IY=1,NCY
        YX=DELY*(IY-.5)+YCMIN
        THX=2.*ATAN(EXP(-YX))
        CTHCAL(IY)=COS(THX)
        STHCAL(IY)=SIN(THX)
300     CONTINUE
        FSTCAL=.FALSE.
      ENDIF
      END
C
      SUBROUTINE CALSIM_M
C                
C          SIMPLE CALORIMETER SIMULATION.  ASSUME UNIFORM Y AND PHI
C          BINS
C...HEPEVT commonblock.
      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      PARAMETER (NMXHEP=4000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
      INTEGER NCY,NCPHI,NJMAX,IHEP,ID,IY,IPHI,JETNO,NCJET
      DOUBLE PRECISION YCMIN,YCMAX,PI,YIP,PSERAP,
     &  PHIIP,DELY,DELPHI,EIP,ET,STHCAL,CTHCAL,CPHCAL,SPHCAL,
     &  PCJET,ETJET
      PARAMETER (NCY=100)
      PARAMETER (NCPHI=60,PI=3.141593D0)
      COMMON/CALOR_M/DELY,DELPHI,ET(NCY,NCPHI),
     $CTHCAL(NCY),STHCAL(NCY),CPHCAL(NCPHI),SPHCAL(NCPHI),YCMIN,YCMAX
      PARAMETER (NJMAX=500)
      COMMON/GETCOM_M/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(NCY,NCPHI),NCJET
C
C          FILL CALORIMETER
C
      DO 200 IHEP=1,NHEP
      IF (ISTHEP(IHEP).EQ.1) THEN
        YIP=PSERAP(PHEP(1,IHEP))
        IF(YIP.LT.YCMIN.OR.YIP.GT.YCMAX) GOTO 200
        ID=ABS(IDHEP(IHEP))
C---EXCLUDE TOP QUARK, LEPTONS, PROMPT PHOTONS
        IF ((ID.GE.11.AND.ID.LE.16).OR.ID.EQ.6.OR.ID.EQ.22) GOTO 200
C
        PHIIP=ATAN2(PHEP(2,IHEP),PHEP(1,IHEP))
        IF(PHIIP.LT.0.) PHIIP=PHIIP+2.*PI
        IY=INT((YIP-YCMIN)/DELY)+1
        IPHI=INT(PHIIP/DELPHI)+1
        EIP=PHEP(4,IHEP)
C            WEIGHT BY SIN(THETA)
        ET(IY,IPHI)=ET(IY,IPHI)+EIP*STHCAL(IY)
      ENDIF
  200 CONTINUE
  999 END
      SUBROUTINE GETJET_M(RJET,EJCUT,ETAJCUT)
C                
C          SIMPLE JET-FINDING ALGORITHM (SIMILAR TO UA1).
C
C     FIND HIGHEST REMAINING CELL > ETSTOP AND SUM SURROUNDING
C          CELLS WITH--
C            DELTA(Y)**2+DELTA(PHI)**2<RJET**2
C            ET>ECCUT.
C          KEEP JETS WITH ET>EJCUT AND ABS(ETA)<ETAJCUT
C          THE UA1 PARAMETERS ARE RJET=1.0 AND EJCUT=5.0
C                  
      IMPLICIT NONE
      INTEGER NCY,NCPHI,NJMAX,IPHI,IY,JETNO,J,K,NCJET,NPHI1,NPHI2,NY1,
     &  NY2,IPASS,IYMX,IPHIMX,ITLIS,IPHI1,IPHIX,IY1,IYX
      DOUBLE PRECISION YCMIN,YCMAX,PI,ETJET,PCJET,RJET,DELPHI,DELY,
     &  ETMAX,ET,ETSTOP,RR,ECCUT,PX,STHCAL,CPHCAL,SPHCAL,CTHCAL,EJCUT
      PARAMETER (NCY=100)
      PARAMETER (NCPHI=60,PI=3.141593D0)
      COMMON/CALOR_M/DELY,DELPHI,ET(NCY,NCPHI),
     &CTHCAL(NCY),STHCAL(NCY),CPHCAL(NCPHI),SPHCAL(NCPHI),YCMIN,YCMAX
      PARAMETER (NJMAX=500)
      COMMON/GETCOM_M/PCJET(4,NJMAX),ETJET(NJMAX),JETNO(NCY,NCPHI),NCJET
      DOUBLE PRECISION ETAJCUT,PSERAP
C
C          PARAMETERS
      DATA ECCUT/0.1D0/
      DATA ETSTOP/1.5D0/
      DATA ITLIS/6/
C
C          INITIALIZE
C
      DO 100 IPHI=1,NCPHI
      DO 100 IY=1,NCY
100   JETNO(IY,IPHI)=0
      DO 110 J=1,NJMAX
      ETJET(J)=0.
      DO 110 K=1,4
110   PCJET(K,J)=0.
      NCJET=0
      NPHI1=RJET/DELPHI
      NPHI2=2*NPHI1+1
      NY1=RJET/DELY
      NY2=2*NY1+1
      IPASS=0
C
C          FIND HIGHEST CELL REMAINING
C
1     ETMAX=0.
      DO 200 IPHI=1,NCPHI
      DO 210 IY=1,NCY
      IF(ET(IY,IPHI).LT.ETMAX) GOTO 210
      IF(JETNO(IY,IPHI).NE.0) GOTO 210
      ETMAX=ET(IY,IPHI)
      IYMX=IY
      IPHIMX=IPHI
210   CONTINUE
200   CONTINUE
      IF(ETMAX.LT.ETSTOP) RETURN
C
C          SUM CELLS
C
      IPASS=IPASS+1
      IF(IPASS.GT.NCY*NCPHI) THEN
        WRITE(ITLIS,8888) IPASS
8888    FORMAT(//' ERROR IN GETJET_M...IPASS > ',I6)
        RETURN
      ENDIF
      NCJET=NCJET+1
      IF(NCJET.GT.NJMAX) THEN
        WRITE(ITLIS,9999) NCJET
9999    FORMAT(//' ERROR IN GETJET_M...NCJET > ',I5)
        RETURN
      ENDIF
      DO 300 IPHI1=1,NPHI2
      IPHIX=IPHIMX-NPHI1-1+IPHI1
      IF(IPHIX.LE.0) IPHIX=IPHIX+NCPHI
      IF(IPHIX.GT.NCPHI) IPHIX=IPHIX-NCPHI
      DO 310 IY1=1,NY2
      IYX=IYMX-NY1-1+IY1
      IF(IYX.LE.0) GOTO 310
      IF(IYX.GT.NCY) GOTO 310
      IF(JETNO(IYX,IPHIX).NE.0) GOTO 310
      RR=(DELY*(IY1-NY1-1))**2+(DELPHI*(IPHI1-NPHI1-1))**2
      IF(RR.GT.RJET**2) GOTO 310
      IF(ET(IYX,IPHIX).LT.ECCUT) GOTO 310
      PX=ET(IYX,IPHIX)/STHCAL(IYX)
C          ADD CELL TO JET
      PCJET(1,NCJET)=PCJET(1,NCJET)+PX*STHCAL(IYX)*CPHCAL(IPHIX)
      PCJET(2,NCJET)=PCJET(2,NCJET)+PX*STHCAL(IYX)*SPHCAL(IPHIX)
      PCJET(3,NCJET)=PCJET(3,NCJET)+PX*CTHCAL(IYX)
      PCJET(4,NCJET)=PCJET(4,NCJET)+PX
      ETJET(NCJET)=ETJET(NCJET)+ET(IYX,IPHIX)
      JETNO(IYX,IPHIX)=NCJET
310   CONTINUE
300   CONTINUE
C
C          DISCARD JET IF ET < EJCUT.
C
      IF(ETJET(NCJET).GT.EJCUT.AND.ABS(PSERAP(PCJET(1,NCJET))).LT
     $     .ETAJCUT) GOTO 1
      ETJET(NCJET)=0.
      DO 400 K=1,4
400   PCJET(K,NCJET)=0.
      NCJET=NCJET-1
      GOTO 1
      END
C-----------------------------------------------------------------------
      SUBROUTINE CALDEL_M(ISTLO,ISTHI,ISTOP)
C     LABEL ALL PARTICLES WITH STATUS BETWEEN ISTLO AND ISTHI (UNTIL A
C     PARTICLE WITH STATUS ISTOP IS FOUND) AS FINAL-STATE, CALL CALSIM_M
C     AND THEN PUT LABELS BACK TO NORMAL
C
C     THIS VERSION LEAVES OUT PARTICLES THAT POINT BACK TO MOTHERS TO BE
C     LEFT OUT OF MATCHING
C-----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER MAXNUP
      PARAMETER(MAXNUP=500)
      INTEGER IEND,INORAD
      COMMON/SHVETO/IEND,INORAD(MAXNUP)
C...HEPEVT commonblock.
      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      PARAMETER (NMXHEP=4000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
      INTEGER ISTOLD(NMXHEP),IHEP,IST,ISTLO,ISTHI,ISTOP,IMO
      LOGICAL FOUND
c      write(3,*) 'new event',nevhep
      FOUND=.FALSE.
      DO 10 IHEP=1,NHEP
        IST=ISTHEP(IHEP)
        ISTOLD(IHEP)=IST
        IF (IST.EQ.ISTOP) FOUND=.TRUE.
        IF (IST.GE.ISTLO.AND.IST.LE.ISTHI.AND..NOT.FOUND) THEN
C     FOUND A RADIATED PARTON, CHECK MOTHER
          IMO=IHEP
 1        IMO=JMOHEP(1,IMO)
          IF(IMO.EQ.IEND) THEN
C     PARENTHOOD OK
            IST=1
c            write(3,*) ihep,ist
            GOTO 9
          ENDIF
          IF(INORAD(IMO).EQ.1) THEN
C     PARTON COMES FROM A VETOED MOTHER
            IST=0
            GOTO 9
          ELSE
C     CHECK GRANDMOTHER
            GOTO 1
          ENDIF
        ELSE
          IST=0
        ENDIF
 9      ISTHEP(IHEP)=IST
 10   CONTINUE
      CALL CALSIM_M
      DO 20 IHEP=1,NHEP
        ISTHEP(IHEP)=ISTOLD(IHEP)
 20   CONTINUE
      END
C-----------------------------------------------------------------------
      SUBROUTINE CALDEL_HVQ(ISTLO,ISTHI,ISTOP)
C     LABEL ALL PARTICLES WITH STATUS BETWEEN ISTLO AND ISTHI (UNTIL A
C     PARTICLE WITH STATUS ISTOP IS FOUND) AS FINAL-STATE, CALL CALSIM_M
C     AND THEN PUT LABELS BACK TO NORMAL
C
C     THIS VERSION KEEPS ONLY ALL IST=1 PARTICLES REJECTED BY CALDEL AS
C     DAUGHTERS OF VETOED HEAVY-QUARK MOTHERS: JETS COMPLEMENTARY TO
C     THOSE RECONSTRUCTED BY CALDEL
C-----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER MAXNUP
      PARAMETER(MAXNUP=500)
      INTEGER IEND,INORAD
      COMMON/SHVETO/IEND,INORAD(MAXNUP)
C...HEPEVT commonblock.
      INTEGER NMXHEP,NEVHEP,NHEP,ISTHEP,IDHEP,JMOHEP,JDAHEP
      PARAMETER (NMXHEP=4000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
      DOUBLE PRECISION PHEP,VHEP
      SAVE /HEPEVT/
      INTEGER ISTOLD(NMXHEP),IHEP,IST,ISTLO,ISTHI,ISTOP,IMO
      INTEGER IDMOTH,IDDAUG
      LOGICAL FOUND
      FOUND=.FALSE.
C TAG W-B FROM TOP DECAYS
      DO IHEP=1,NHEP
        IMO=JMOHEP(1,IHEP)
        IDMOTH=ABS(IDHEP(IMO))
        IDDAUG=ABS(IDHEP(IHEP))
        IF(IDMOTH.EQ.6.AND.(IDDAUG.EQ.5.OR.IDDAUG.EQ.24)) INORAD(IHEP)=2
      ENDDO
C
      DO 10 IHEP=1,NHEP
        IST=ISTHEP(IHEP)
        ISTOLD(IHEP)=IST
        IF (IST.EQ.ISTOP) FOUND=.TRUE.
        IF (IST.GE.ISTLO.AND.IST.LE.ISTHI.AND..NOT.FOUND) THEN
C     FOUND A RADIATED PARTON, CHECK MOTHER
          IMO=IHEP
 1        IMO=JMOHEP(1,IMO)
          IF(IMO.EQ.IEND) THEN
C     PARENTHOOD OK, VETO
            IST=0
            GOTO 9
          ENDIF
          IF(INORAD(IMO).EQ.1) THEN
            IDMOTH=ABS(IDHEP(IMO))
            IDDAUG=ABS(IDHEP(IHEP))
C     VERIFY IT'S A HEAVY QUARK -- LEAVE OUT GAUGE BOSON DECAYS
            IF(IDMOTH.GE.4.AND.IDMOTH.LE.6) THEN
C     PARTON COMES FROM A VETOED MOTHER, KEEP UNLESS IT IS THE EVOLVED
C     MOTHER
              IF(IDMOTH.NE.IDDAUG) THEN
                IST=1
                GOTO 9
              ELSE
                IST=0
              ENDIF
            ELSE
C     NOT A HEAVY QUARK MOTHER, LEAVE OUT OF JET RECONSUTRCTION
              IST=0
            ENDIF
          ELSEIF(INORAD(IMO).EQ.2) THEN
C     IT'S A TOP DECAY PRODUCT
            IST=0
          ELSE
C     GO CHECK GRANDMOTHER
            GOTO 1
          ENDIF
        ELSE
          IST=0
        ENDIF
 9      ISTHEP(IHEP)=IST
 10   CONTINUE
      CALL CALSIM_M
      DO 20 IHEP=1,NHEP
        ISTHEP(IHEP)=ISTOLD(IHEP)
 20   CONTINUE
      END
C-----------------------------------------------------------------------
      FUNCTION PSERAP(P)
C     PSEUDO-RAPIDITY (-LOG TAN THETA/2)
C-----------------------------------------------------------------------
      DOUBLE PRECISION PSERAP,P(3),PT,PL,TINY,THETA
      PARAMETER (TINY=1D-3)
      PT=SQRT(P(1)**2+P(2)**2)+TINY
      PL=P(3)
      THETA=ATAN2(PT,PL)
      PSERAP=-LOG(TAN(0.5*THETA))
      END
C-----------------------------------------------------------------------
C-----------------------------------------------------------------------
      SUBROUTINE ALPSOR(A,N,K,IOPT)
C-----------------------------------------------------------------------
C     Sort A(N) into ascending order
C     IOPT = 1 : return sorted A and index array K
C     IOPT = 2 : return index array K only
C-----------------------------------------------------------------------
      DOUBLE PRECISION A(N),B(5000)
      INTEGER N,I,J,IOPT,K(N),IL(5000),IR(5000)
      IF (N.GT.5000) then
        write(*,*) 'Too many entries to sort in alpsrt, stop'
        stop
      endif
      if(n.le.0) return
      IL(1)=0
      IR(1)=0
      DO 10 I=2,N
      IL(I)=0
      IR(I)=0
      J=1
   2  IF(A(I).GT.A(J)) GOTO 5
   3  IF(IL(J).EQ.0) GOTO 4
      J=IL(J)
      GOTO 2
   4  IR(I)=-J
      IL(J)=I
      GOTO 10
   5  IF(IR(J).LE.0) GOTO 6
      J=IR(J)
      GOTO 2
   6  IR(I)=IR(J)
      IR(J)=I
  10  CONTINUE
      I=1
      J=1
      GOTO 8
  20  J=IL(J)
   8  IF(IL(J).GT.0) GOTO 20
   9  K(I)=J
      B(I)=A(J)
      I=I+1
      IF(IR(J)) 12,30,13
  13  J=IR(J)
      GOTO 8
  12  J=-IR(J)
      GOTO 9
  30  IF(IOPT.EQ.2) RETURN
      DO 31 I=1,N
  31  A(I)=B(I)
 999  END
C-----------------------------------------------------------------------
