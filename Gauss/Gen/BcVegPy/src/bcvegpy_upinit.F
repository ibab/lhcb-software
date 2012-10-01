C...*************************************************************************
C...To do the initialization
C...Messages commented; XSECUP(1) value corrected; 2007-05-09
C...Modified by hejb, 2006-03-14
C...*************************************************************************

      SUBROUTINE BCVEGPY_UPINIT(IISET)

C...Preamble: declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...Three Pythia functions return integers, so need declaring.
      INTEGER PYK,PYCHGE,PYCOMP
C...EXTERNAL statement.
      EXTERNAL PYDATA,TOTFUN

      CHARACTER*472 BASENAME
      CHARACTER*472 ENERGYNAME      
      CHARACTER*472 FULLPATHNAME

C...PYTHIA common block.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      PARAMETER (MAXNUP=500)
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/

      PARAMETER (MAXPUP=100)
      INTEGER PDFGUP,PDFSUP,LPRUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

C...User process event common block.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     & 	COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
      COMMON/COUNTER/IBCSTATE,NEV
      COMMON/RCONST/PI
      COMMON/GRADE/XI(50,10)
      COMMON/VEGCROSS/VEGSEC,VEGERR,IVEGGRADE
      COMMON/COLFLOW/AMP2CF(10),SMATVAL

C...hejb: CROSSMAX needed for us
      COMMON/PTPASS/PTMIN,PTMAX,CROSSMAX,ETAMIN,ETAMAX,
     &SMIN,SMAX,YMIN,YMAX,PSETAMIN,PSETAMAX

C...Transform of running information.
      COMMON/CONFINE/PTCUT,ETACUT

C...GENERATE---SWITCH FOR FULL EVENTS. VEGASOPEN---SWITCH FOR USING
C...VEGAS.
      LOGICAL GENERATE,VEGASOPEN,USEGRADE
      COMMON/GENEFULL/GENERATE

C...GET THE APPROXIMATE TOTAL CROSS-SECTION.
      COMMON/TOTCROSS/APPCROSS

C...PARAMETERS TRANSFORMTION.
      COMMON/FUNTRANS/NQ2,NPDFU
      COMMON/USERTRAN/ISHOWER,IDPP

C...transform of the VEGAS information
      COMMON/VEGASINF/NUMBER,NITMX

C...TO GET THE SUBPROCESS CROSS-SECTION.
      COMMON/SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY

C...TO GET THE DISTRIBUTION OF AN EXTRA FACTOR Z=(2(k1+k2).P_BC)/SHAT.
      COMMON/EXTRAZ/ZFACTOR,ZMIN,ZMAX
      COMMON/OUTPDF/IOUTPDF,IPDFNUM

C...USED IN GRV98L
      COMMON/INTINIP/IINIP
      COMMON/INTINIF/IINIF
        
C...transform some variables
C...  IUSECURDIR added (20090723), to enable the usage of the grade files
C...  in the current directory (`pwd`), mainly for Grid.  
      COMMON/LOGGRADE/IEVNTDIS,IGENERATE,IVEGASOPEN,IGRADE,IUSECURDIR

C...FOR TRANSFORM THE SUBPROCESS INFORMATION, I.E.,  WHETHER USING
C...THE SUBPROCESS q\bar{q}->Bc+b+\bar{c} TO GENERATE EVENTS.
      COMMON/QQBAR/IQQBAR,IQCODE

      CHARACTER*8 BEGIN_TIME,END_TIME,ALPORDER,BLANK
      DIMENSION IHI(10),NFIN(20),ALAMIN(20)
      COMMON/COLOCT/IOCTET
      COMMON/OCTMATRIX/COEOCT
C...TRANSFORM THE FIRST DERIVATIVE OF THE WAVEFUNCTION AT THE ZERO OR THE
C...WAVEFUNCTION AT THE ZERO.
      COMMON/WAVEZERO/FBC

C...Data:Lambda and n_f values for parton distributions. This is obtained
C...from PYTHIA and used for vegas running.
      DATA ALAMIN/0.177D0,0.239D0,0.247D0,0.2322D0,0.248D0,0.248D0,
     &0.192D0,0.326D0,2*0.2D0,0.2D0,0.2D0,0.29D0,0.2D0,0.4D0,5*0.2D0/,
     &NFIN/20*4/

c...XSECUP(8) RECORDS THE TOTAL DIFFERENTIAL CROSS-SECTIONS FOR DIFFERENT
C...STATES: 1---Singlet 1S0; 2---singlet 3s1; 7---octet 1s0; 8---octet 3s1;
C...3---Singlet 1p1; 4---Singlet 3p0; 5---Singlet 3p1; 6---Singlet 3p2.
      COMMON/MIXEVNT/XBCSEC(8),IMIX,IMIXTYPE

C....To store CROSSMAX and XBCSUM, IBCLIMIT
      COMMON/CRMABC/BCCRMA(8),XBCSUM,IBCLIMIT

C-----------------------------------------------------------------------
C  initilization of beam parameters and external process
C-----------------------------------------------------------------------
      IF( IISET.EQ.1 ) THEN
C...Set up incoming beams. Tevotran
        IF(NPDFU.EQ.1) THEN
          IDBMUP(1) = 2212
          IDBMUP(2) = -2212
        END IF
        
C...Set up incoming beams. LHC
        IF(NPDFU.EQ.2) THEN
          IDBMUP(1) = 2212
          IDBMUP(2) = 2212
        END IF
        
        EBMUP(1)  = 0.5D0*ECM
        EBMUP(2)  = 0.5D0*ECM
        
C...Set up the external process.
        IDWTUP   = IDPP
        NPRUP    = 1
        LPRUP(1) = 1001
        IDPRUP   = LPRUP(1)
        
        GOTO 999
      END IF

C...Comented by hejb
C      CALL SETPARAMETER

C**************************************************************************************
C The following from evtinit
C**************************************************************************************


C...Switch off aspects: initial and final state
C...showers, multiple interactions, hadronization.
C...Commented by hejb, 2006/03/21
      IF(ISHOWER.EQ.0) THEN
         MSTP(61) =0
         MSTP(71) =0
         MSTP(81) =0
C..Commented by hejb, we need fragmentation here.
C         MSTP(111)=0
      END IF

C...Commented by hejb, it is not required here
C...Expanded event listing (required for histogramming).
C      MSTP(125)=2


C**************************************************************************************
C The following from main
C**************************************************************************************
      CALL GETENV( 'BCVEGPYDATAROOT' , BASENAME )

C...For reading grid files for different center of mass energy
      IF( ABS(ECM-14000).LE.1. )THEN 
        ENERGYNAME='14TeV/'
      ELSE IF( ABS(ECM-10000).LE.1. )THEN
        ENERGYNAME='10TeV/'
      ELSE IF( ABS(ECM-8000).LE.1. )THEN
        ENERGYNAME='8TeV/'
      ELSE IF( ABS(ECM-7000).LE.1. )THEN
        ENERGYNAME='7TeV/'        
      ELSE
        ENERGYNAME='***Not-Supported-Energy***/'
        WRITE(*,*) "***************************************************"
        WRITE(*,*) "The energy " ,ECM," GeV is not supported in BcVegPy"
        WRITE(*,*) "***************************************************"
      ENDIF 

C...Full path of the grid data files. Use the ones (should be uploaded
C...by the user himself) in the currect directory when IUSECURDIR=1
C...Use the ones in the standard releases when IUSECURDIR=0 (default) 
      IF(IUSECURDIR.EQ.1)THEN
        FULLPATHNAME=''
      ELSE
        FULLPATHNAME=BASENAME(1:len_trim(BASENAME))//'/data/'
     &    //ENERGYNAME(1:len_trim(ENERGYNAME))  
      ENDIF

CC...Commented 09/05/2007, initializations done sereral times, messages unwanted
CC      IF(IMIX.EQ.1) THEN

C...Commented by hejb, not wanted for this version
C     OPEN(UNIT=3,FILE
C     &   =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/mix.dat'
C     &   ,STATUS='UNKNOWN')
CC         WRITE(*,'(A)') 'GET THE MIXING RESULTS FOR GLUON-GLUON FUSION.'
C	WRITE(3,'(A)') 'GET THE MIXING RESULTS FOR GLUON-GLUON FUSION.'
CC         IF(IMIXTYPE.EQ.1) THEN
CC            WRITE(*,'(A)') 
CC     &   'STATES TO BE MIXED: 1S0,3S1,1P1,3P0,3P1,3P2,(1S0)_8,(3S1)_8.'
C	 WRITE(3,'(A)')
C     &	 'STATES TO BE MIXED: 1S0,3S1,1P1,3P0,3P1,3P2,(1S0)_8,(3S1)_8.'
CC         END IF
CC         IF(IMIXTYPE.EQ.2) THEN
CC            WRITE(*,'(A)') 'STATES TO BE MIXED: 1S0,3S1'
C	  WRITE(3,'(A)') 'STATES TO BE MIXED: 1S0,3S1'
CC         END IF
CC         IF(IMIXTYPE.EQ.3) THEN
CC            WRITE(*,'(A)') 
CC     &	  'STATES TO BE MIXED: 1P1,3P0,3P1,3P2,(1S0)_8,(3S1)_8.'
C	  WRITE(3,'(A)') 
C     &	  'STATES TO BE MIXED: 1P1,3P0,3P1,3P2,(1S0)_8,(3S1)_8.'
CC         END IF
CC      END IF                    !IF IMIX=1

C************************************************

      IF(IGENERATE.EQ.0)  GENERATE =.FALSE.
      IF(IGENERATE.EQ.1)  GENERATE =.TRUE.
      
      IF(IVEGASOPEN.EQ.0) VEGASOPEN=.FALSE.
      IF(IVEGASOPEN.EQ.1) VEGASOPEN=.TRUE.
      
      IF(ISUBONLY.EQ.1) THEN
         SUBFACTOR=SUBENERGY/ECM
      END IF

      IF(IGRADE.EQ.1) THEN
         USEGRADE=.TRUE.
      ELSE
         USEGRADE=.FALSE.
      END IF                    !IF IGRADE=1

C--------------------------------------------

C...FILE ABOUT THE GENERATED GRADE BY VEGAS.
      IF(IMIX.EQ.0)THEN
C....Added by hejb (for this IF-ENDIF unit)
        IF(IBCSTATE.EQ.1)
     &    OPEN(UNIT=36,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1s0.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.2)
     &    OPEN(UNIT=37,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3s1.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.3)
     &    OPEN(UNIT=38,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1p1.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.4)
     &    OPEN(UNIT=39,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p0.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.5)
     &    OPEN(UNIT=46,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p1.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.6)
     &    OPEN(UNIT=47,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p2.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.7)
     &    OPEN(UNIT=48,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade81s.dat'
     &    ,STATUS='UNKNOWN')
        IF(IBCSTATE.EQ.8)
     &    OPEN(UNIT=49,FILE
     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade83s.dat'
     &    ,STATUS='UNKNOWN')
      END IF                    !IMIX=0
      
C...Commented by hejb
C     &   OPEN(UNIT=11,FILE
C     &   =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade.dat'
C     &   ,STATUS='UNKNOWN')
 
      IF(IMIX.EQ.1) THEN
        IF(IMIXTYPE.EQ.1) THEN
          OPEN(UNIT=36,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1s0.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=37,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3s1.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=38,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1p1.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=39,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p0.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=46,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p1.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=47,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p2.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=48,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade81s.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=49,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade83s.dat'
     &      ,STATUS='UNKNOWN')
        END IF
        IF(IMIXTYPE.EQ.2) THEN
          OPEN(UNIT=36,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1s0.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=37,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3s1.dat'
     &      ,STATUS='UNKNOWN')
        END IF
        IF(IMIXTYPE.EQ.3) THEN
          OPEN(UNIT=38,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade1p1.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=39,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p0.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=46,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p1.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=47,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade3p2.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=48,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade81s.dat'
     &      ,STATUS='UNKNOWN')
          OPEN(UNIT=49,FILE
     &      =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'grade83s.dat'
     &      ,STATUS='UNKNOWN')
        END IF
      END IF                    !IMIX=0
      


C--------------------------------------------

C...OUTPUT FILES TO STORE THE RUNNING INFORMATION FOR THE INTEGRATED 
C...CROSS-SECTION. NOTE HERE THE P-WAVE STATES ARE ONLY FOR GLUON-
C...GLUON FUSION SUBPROCESS.

C...Commneted by hejb, not wanted for this version
C      IF(IMIX.EQ.0) THEN
C	 IF(IBCSTATE.EQ.1)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/1s0.dat'
C     &    ,STATUS='UNKNOWN')
C	 IF(IBCSTATE.EQ.2)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/3s1.dat'
C     &    ,STATUS='UNKNOWN')
C	 IF(IBCSTATE.EQ.3)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/1p1.dat'
C     &    ,STATUS='UNKNOWN')
C	 IF(IBCSTATE.EQ.4)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/3p0.dat'
C     &    ,STATUS='UNKNOWN')
C	 IF(IBCSTATE.EQ.5)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/3p1.dat'
C     &    ,STATUS='UNKNOWN')
C	 IF(IBCSTATE.EQ.6)
C     &	    OPEN(UNIT=3,FILE
C     &    =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/3p2.dat'
C     &    ,STATUS='UNKNOWN')
C      END IF !IF IMIX=0

C--------------------------------------------

C...REFERENCE LIGHT-LIKE MOMENTUM, WHICH CAN BE CHOOSING ARBITRARILY.
C...THIS CAN BE USED TO CHECK THT RIGHTNESS OF THE PROGRAM.
C...--used only for s-wave.
      PMOMUP(1,8)=3.0D0
      PMOMUP(2,8)=4.0D0
      PMOMUP(3,8)=5.0D0
      PMOMUP(4,8)=DSQRT(PMOMUP(1,8)**2+PMOMUP(2,8)**2+PMOMUP(3,8)**2)
      PMOMUP(5,8)=0.0D0

C--------------------------------------------

C...output.
CC      WRITE(*,'(A)') 
CC     &     '......................................................'
CC      WRITE(*,'(A)')
CC     &     '.............. INITIAL PARAMETERS ....................'
CC      WRITE(*,'(A)') 
CC     &     '......................................................'
C      WRITE(3,'(A)') 
C     &	'......................................................'
C	WRITE(3,'(A)') 
C     &	'.............. INITIAL PARAMETERS ....................'
C      WRITE(3,'(A)') 
C     &	'......................................................'



C...To print the information of the state generated
      IF(IMIX.EQ.1) GO TO 111
      
CC      IF(IBCSTATE.EQ.3) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 1^P_1.'
CC         WRITE(*,*)
C	  WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 1^P_1.'
CC      END IF        

CC      IF(IBCSTATE.EQ.4) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 3^P_0.'
CC         WRITE(*,*)
C	  WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 3^P_0.'
CC      END IF

CC      IF(IBCSTATE.EQ.5) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 3^P_1.'
CC         WRITE(*,*)
C	  WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 3^P_1.'
CC      END IF

CC      IF(IBCSTATE.EQ.6) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 3^P_2.'
CC         WRITE(*,*)
C         WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 3^P_2.'
CC      END IF
      
CC      IF (IBCSTATE.EQ.1) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 1^S_0.'
CC         WRITE(*,*)
C	  WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 1^S_0.'
CC      END IF

CC      IF (IBCSTATE.EQ.2) THEN
CC         WRITE(*,*)
CC         WRITE(*,'(A)') 'GET THE RESULT FOR Bc IN 3^S_1.'
CC         WRITE(*,*)
C	  WRITE(3,'(A)') 'GET THE RESULT FOR Bc IN 3^S_1.'
CC      END IF

CC      IF(IOCTET.EQ.0) THEN
CC         WRITE(*,'(A)') 'Bc IN COLOR-SINGLET STATE.'
C	  WRITE(3,'(A)') 'Bc IN COLOR-SINGLET STATE.'
CC      END IF
CC      IF(IOCTET.EQ.1) THEN 
CC         WRITE(*,'(A)') 'Bc IN COLOR-OCTET STATE.'
C	  WRITE(3,'(A)') 'Bc IN COLOR-OCTET STATE.'
CC      END IF

111   CONTINUE

CC      IF(NPDFU.EQ.1) THEN
C	  WRITE(3,41)'GENERATE EVNTS', NEV,'TEVA ENERGY(GEV)',ECM
CC         WRITE(*,41)'GENERATE EVNTS', NEV,'TEVA ENERGY(GEV)',ECM
CC      ELSE                      !NPDFU=2
C	  WRITE(3,41) 'GENERATE EVNTS', NEV,'LHC ENERGY(GEV)',ECM
CC         WRITE(*,41) 'GENERATE EVNTS',NEV,'LHC ENERGY(GEV)',ECM
CC      END IF

CC      WRITE(*,'(A)') '*************************************************'
C      WRITE(3,'(A)') '*************************************************'
CC      IF(IQQBAR.EQ.0) THEN
CC	 WRITE(*,'(A)')'*   USING SUBPROCESS: 
CC     &    g+g->Bc+b+bar{c}      *'
C	 WRITE(3,'(A)')'*   USING SUBPROCESS: g+g->Bc+b+\bar{c}         *'
CC      ELSE
CC	 IF(IQCODE.EQ.1) THEN
CC            IF(IOUTPDF.EQ.1) THEN
C...FOR OUTER PDF, CODE FOR u IS 1 , d IS 2 AND s IS 3.
CC               WRITE(*,'(A)')'*   USING SUBPROCESS: 
CC     &          u+~u->Bc+b+\bar{c}   *'
C	   WRITE(3,'(A)')'*   USING SUBPROCESS: u+~u->Bc+b+\bar{c}   *'
CC            END IF
C...IN PYTHIA, CODE FOR d IS 1, u IS 2 AND s IS 3.
CC            IF(IOUTPDF.EQ.0) THEN
CC               WRITE(*,'(A)')'*   USING SUBPROCESS:
CC     &          d+~d->Bc+b+\bar{c}   *'
C	   WRITE(3,'(A)')'*   USING SUBPROCESS: d+~d->Bc+b+\bar{c}   *'
CC            END IF
CC         END IF                 ! IQCODE=1
         
CC	 IF(IQCODE.EQ.2) THEN
C...FOR OUTER PDF, CODE FOR u IS 1, d IS 2 AND s IS 3.
CC            IF(IOUTPDF.EQ.1) THEN
CC               WRITE(*,'(A)')'*   USING SUBPROCESS:
CC     &          d+~d->Bc+b+\bar{c}   *'
C	   WRITE(3,'(A)')'*   USING SUBPROCESS: d+~d->Bc+b+\bar{c}   *'
CC            END IF
C...IN PYTHIA, CODE FOR d IS 1, u IS 2 AND s IS 3.
CC            IF(IOUTPDF.EQ.0) THEN
CC               WRITE(*,'(A)')'*   USING SUBPROCESS:
CC     &          u+~u->Bc+b+\bar{c}   *'
C	   WRITE(3,'(A)')'*   USING SUBPROCESS: u+~u->Bc+b+\bar{c}   *'
CC            END IF
CC	 END IF                 !IQCODE=2

CC	 IF(IQCODE.EQ.3) THEN
CC            WRITE(*,'(A)')'*   USING SUBPROCESS:
CC     &       s+~s->Bc+b+\bar{c}   *'
C	  WRITE(3,'(A)')'*   USING SUBPROCESS: s+~s->Bc+b+\bar{c}   *'
CC         END IF                 !IQCODE=3
CC      END IF                    !IQQBAR=0
CC      WRITE(*,'(A)') '*************************************************'
C      WRITE(3,'(A)') '*************************************************'

	
CC      IF(IMIX.EQ.0) THEN
C	 WRITE(3,32) PMBC,PMB,PMC,FBC
CC	 WRITE(*,32) PMBC,PMB,PMC,FBC
CC      ELSE
C	 WRITE(3,31) PMBC,PMB,PMC
CC	 WRITE(*,31) PMBC,PMB,PMC
CC      END IF

      IF(ISUBONLY.EQ.0) THEN
	 IF(MSTU(111).EQ.0) ALPORDER='CONST'
	 IF(MSTU(111).EQ.1) ALPORDER='LO'
	 IF(MSTU(111).EQ.2) ALPORDER='NLO'
CC	 WRITE(*,'(A,I2,3X,A,A)')'Q2 TYPE=',NQ2, 'ALPH ORDER=',ALPORDER
C         WRITE(3,'(A,I2,3X,A,A)')'Q2 TYPE=',NQ2, 'ALPH ORDER=',ALPORDER


C....Commented by Patrick Robbe
CCCCCC TAKEN CARE BY GAUSS PYTHIA INTERFACE
C         IOUTPDF = 0
C	 IF(IOUTPDF.EQ.0) THEN
C            WRITE(*,'(A)') 'THE INNTER PDFs WHICH ARE FROM PYTHIA'
C            WRITE(3,'(A)') 'THE INNTER PDFs WHICH ARE FROM PYTHIA'
C...YOU MAY ADD SOME MORE FROM PYTHIA MANUAL BOOK.
C            IF(MSTP(51).EQ.1) THEN
C               WRITE(*,'(A)')'PDF: CTEQ 3L'
C               WRITE(3,'(A)')'PDF: CTEQ 3L'
C            END IF
C            IF(MSTP(51).EQ.2) THEN
C               WRITE(*,'(A)')'PDF: CTEQ 3M'
C               WRITE(3,'(A)')'PDF: CTEQ 3M'
C            END IF
C            IF(MSTP(51).EQ.7) THEN
C               WRITE(*,'(A)')'PDF: CTEQ 5L'
C               WRITE(3,'(A)')'PDF: CTEQ 5L'
C            END IF
C            IF(MSTP(51).EQ.8) THEN
C               WRITE(*,'(A)')'PDF: CTEQ 5M1'
C               WRITE(3,'(A)')'PDF: CTEQ 5M1'
C            END IF
C            IF(MSTP(51).EQ.13) THEN
C               WRITE(*,'(A)')'PDF: EHLQ SET 2'
C               WRITE(3,'(A)')'PDF: EHLQ SET 2'
C            END IF
C	 END IF
C	 IF(IOUTPDF.EQ.1) THEN
C            WRITE(*,'(A)') 'THE NEW VERSION OF OUTER PDFs
C     &       WHICH ARE FROM WWW'
C            WRITE(3,'(A)') 'THE NEW VERSION OF OUTER PDFs
C     &       WHICH ARE FROM WWW'
C            IF(IPDFNUM.EQ.100) THEN
C               WRITE(*,'(A)')'PDF: GRV98LO; ALPH IN LO'
C               WRITE(3,'(A)')'PDF: GRV98LO; ALPH IN LO'
C            END IF
C            IF(IPDFNUM.EQ.200) THEN
C               WRITE(*,'(A)')'PDF: MSTR2001LO; ALPH IN LO'
C               WRITE(3,'(A)')'PDF: MSTR2001LO; ALPH IN LO'
C            END IF
C            IF(IPDFNUM.EQ.300) THEN
C               WRITE(*,'(A)')'PDF: CTEQ6L; ALPH IN LO'
C               WRITE(3,'(A)')'PDF: CTEQ6L; ALPH IN LO'
C            END IF
C	 END IF                 !IOUTPDF=1

      END IF                    !ISUBONLY=0

CC      IF(ISUBONLY.EQ.1) THEN
CC         WRITE(*,'(A)') 'ALPHA_S TAKE CONSTANT VALUE: 0.20'
CC      END IF

CC      WRITE(*,'(A,I3)') 'USING PYTHIA IDWTUP=',IDPP
C	WRITE(3,'(A,I3)') 'USING PYTHIA IDWTUP=',IDPP

CC      WRITE(*,'(A,G10.4,A)') 'PTCUT=',PTCUT,'GeV'
C	WRITE(3,'(A,G10.4,A)') 'PTCUT=',PTCUT,'GeV'

CC      IF(ETACUT.GT.1.0D+5) THEN
CC         WRITE(*,'(A)') 'NO RAPIDITY CUT'
C	   WRITE(3,'(A)') 'NO RAPIDITY CUT'
CC      ELSE
CC         WRITE(*,'(A,G10.4)') 'RAPIDITY CUT=',ETACUT
C	   WRITE(3,'(A,G10.4)') 'RAPIDITY CUT=',ETACUT
CC      END IF


C...Commented by hejb, taken care by Gauss Pythia interface 2006/03/17
C...SETTING THE ALPHAS.
C   MSTU(111)=0---FIXED; D=1---FIR ORDER; =2---SEC ORDER
C      MSTP(2)  =MSTU(111)

C---------------------------------------------------------


C...Commented by hejb
C...This should be taken care by LbPythia 2006/03/21
C...CHOOSE LAMBDA VALUE TO BE USED IN CASE OF USING VEGAS.
C      IF(VEGASOPEN) THEN
C         IF(IOUTPDF.EQ.0) THEN
CC...Added by hejb, since in LbPythia, MSTP(51)=10042 for CTEQ6L  
C            IF(MSTP(51).EQ.10042)THEN
C               PARU(112)=0.215D0 !CTEQ6L
C               MSTU(112)=4
C            END IF
CC..
C...THIS PART IS FROM THE INNER PART OF PYTHIA.
C            IF(MSTP(51).GE.1 .AND. MSTP(51).LE.20) THEN
C               PARU(112)=ALAMIN(MSTP(51))
C               MSTU(112)=NFIN(MSTP(51))
C            END IF
C         ELSE                   !IOUTPDF=0
C            IF(IPDFNUM.EQ.100) THEN
C               PARU(112)=0.1750D0 !GRV98L
C               MSTU(112)=4
C            END IF
C            IF(IPDFNUM.EQ.200) THEN
C               PARU(112)=0.220D0 !MSRT2001L
C               MSTU(112)=4
C            END IF
C            IF(IPDFNUM.EQ.300) THEN
C               PARU(112)=0.215D0 !CTEQ6L
C               MSTU(112)=4
C            END IF
C         END IF                 !IOUTPDF=0 
C      END IF                    !VEGASOPEN


C...*********************************************************************
C...USING VEGAS TO GET THE IMPORTANCE FUNCTION AND THEN THE 
C...SAMPLING STRATAGE: XI(), WHICH WILL BE STORED IN FILE GRADE.DAT.
C...*********************************************************************

      IF(VEGASOPEN) THEN
         WRITE(*,'(A,I9,A,I3)') 'USING VEGAS: NUMBER IN EACH ITER.='
     &        ,NUMBER,'  ITER.=',NITMX
C   	   WRITE(3,'(A,I9,A,I3)') 'USING VEGAS: NUMBER IN EACH ITER.='
C     &	   ,NUMBER,'  ITER.=',NITMX
         WRITE(*,'(A)') 
     &        '......................................................'
         WRITE(*,'(A)') 
     &        '.............. END OF INITIALIZATION .................'
         WRITE(*,'(A)') 
     &        '......................................................'
C	   WRITE(3,'(A)') 
C     &	   '......................................................'
C	   WRITE(3,'(A)') 
C     &	   '.............. END OF INITIALIZATION .................'
C	   WRITE(3,'(A)') 
C     &	   '......................................................'
         WRITE(*,'(A)') 'WAITING......VEGAS RUNNING......'
C	   WRITE(3,'(A)') 'WAITING......VEGAS RUNNING......'

         IF(ISUBONLY.EQ.0) THEN

C...commented by hejb, not inclued for this version
C            IF(IVEGGRADE.EQ.1) THEN
C              WRITE(*,'(A)')'USING EXISTED GRADE TO GENE. PRECISE GRADE.'
C	       WRITE(3,'(A)')'USING EXISTED GRADE TO GENE. PRECISE GRADE.'
C	       DO I=1,50
C	         READ(11,*) (XI(I,J),J=1,7)
C	       END DO
C	       OPEN(UNIT=29,FILE
C     &          =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/newgrade.dat'
C     &          ,STATUS='UNKNOWN')
C            END IF              !IVEGRADE=1	   


C....****************************************************************************
C....To generate and store the grade.dat of the wanted state for imix=0
C....Added by hejb
C....****************************************************************************
            IF(IMIX.EQ.0)THEN
               IF(IBCSTATE.LE.6)THEN
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section 
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
               ENDIF
C********************************
C********************************
               IF(IBCSTATE.EQ.1)THEN
                  XBCSEC(1)=VEGSEC
                  BCCRMA(1)=CROSSMAX                
                  WRITE(36,*) XBCSEC(1),BCCRMA(1)
                  DO I=1,50
                     WRITE(36,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(36)
               END IF
C********************************
               IF(IBCSTATE.EQ.2)THEN
                  XBCSEC(2)=VEGSEC
                  BCCRMA(2)=CROSSMAX
                  WRITE(37,*) XBCSEC(2),BCCRMA(2)
                  DO I=1,50
                     WRITE(37,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(37)
               END IF
C********************************  
               IF(IBCSTATE.EQ.3)THEN
                  XBCSEC(3)=VEGSEC
                  BCCRMA(3)=CROSSMAX
                  WRITE(38,*) XBCSEC(3),BCCRMA(3)
                  DO I=1,50
                     WRITE(38,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(38)
               END IF
C********************************
               IF(IBCSTATE.EQ.4)THEN
                  XBCSEC(4)=VEGSEC
                  BCCRMA(4)=CROSSMAX
                  WRITE(39,*) XBCSEC(4),BCCRMA(4)
                  DO I=1,50
                     WRITE(39,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(39)
               END IF
C********************************
               IF(IBCSTATE.EQ.5)THEN
                  XBCSEC(5)=VEGSEC
                  BCCRMA(5)=CROSSMAX
                  WRITE(46,*) XBCSEC(5),BCCRMA(5)
                  DO I=1,50
                     WRITE(46,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(46)
               END IF
C********************************
               IF(IBCSTATE.EQ.6)THEN
                  XBCSEC(6)=VEGSEC
                  BCCRMA(6)=CROSSMAX
                  WRITE(47,*) XBCSEC(6),BCCRMA(6)
                  DO I=1,50
                     WRITE(47,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(47)
               END IF
C***********************************************************************************
               IF(IBCSTATE.EQ.7)THEN
                  CALL PARASWAVE(IBCSTATE)
                  IBCSTATE=1    !IBCSTATE=7 <=> IBCSTATE=1 & IOCTET=1
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(7)=VEGSEC
                  BCCRMA(7)=CROSSMAX
                  WRITE(48,*) XBCSEC(7),BCCRMA(7)
                  DO I=1,50
                     WRITE(48,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(48)
               END IF
C********************************
               IF(IBCSTATE.EQ.8)THEN
                  CALL PARASWAVE(IBCSTATE)
                  IBCSTATE=2    !IBCSTATE=8 <=> IBCSTATE=2 & IOCTET=1
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(8)=VEGSEC
                  BCCRMA(8)=CROSSMAX
                  WRITE(49,*) XBCSEC(8),BCCRMA(8)
                  DO I=1,50
                     WRITE(49,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(49)
               END IF

            END IF              ! IMIX=0
       

C...***********************************************************************
C...To generate and store the grade.dat of each state for imix=1
C...cross section & max C-S added by hejb
C...**********************************************************************
            IF(IMIX.EQ.1) THEN
               IF(IMIXTYPE.EQ.1.OR.IMIXTYPE.EQ.2) THEN !s-wave
                  IBCSTATE=1
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &                 AND GRADE (1S0)...'
C             WRITE(3,'(A)') '...GENERATING CROSS-SEC AND GRADE (1S0)...'
                  CALL PARASWAVE(IBCSTATE)
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(1)=VEGSEC
                  BCCRMA(1)=CROSSMAX
                  WRITE(36,*) XBCSEC(1),BCCRMA(1)
                  DO I=1,50
                     WRITE(36,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(36)
C***********************************
                  IBCSTATE=2
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &                 AND GRADE (3S1)...'
C             WRITE(3,'(A)') '...GENERATING CROSS-SEC AND GRADE (3S1)...'
                  CALL PARASWAVE(IBCSTATE)
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(2)=VEGSEC
                  BCCRMA(2)=CROSSMAX
                  WRITE(37,*) XBCSEC(2),BCCRMA(2)
                  DO I=1,50
                     WRITE(37,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(37)
               END IF           !IMIXTPYE=1 OR IMIXTYPE=2
C********************************
               IF(IMIXTYPE.EQ.1.OR.IMIXTYPE.EQ.3) THEN ! p-wave
                  IBCSTATE=3
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &                 AND GRADE (1P1)...'
C             WRITE(3,'(A)') '...GENERATING CROSS-SEC AND GRADE (1P1)...'
                  CALL PARAPWAVE
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(3)=VEGSEC
                  BCCRMA(3)=CROSSMAX
                  WRITE(38,*) XBCSEC(3),BCCRMA(3)
                  DO I=1,50
                     WRITE(38,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(38)
C********************************
                  IBCSTATE=4
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &                 AND GRADE (3P0)...'
C             WRITE(3,'(A)') '...GENERATING CROSS-SEC AND GRADE (3P0)...'
                  CALL PARAPWAVE
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(4)=VEGSEC
                  BCCRMA(4)=CROSSMAX
                  WRITE(39,*) XBCSEC(4),BCCRMA(4)
                  DO I=1,50
                     WRITE(39,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(39)
C********************************
                  IBCSTATE=5
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &        AND GRADE (3P1)...'
C             WRITE(3,'(A)') '...GENERATING CROSS-SEC AND GRADE (3P1)...'
                  CALL PARAPWAVE
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(5)=VEGSEC
                  BCCRMA(5)=CROSSMAX
                  WRITE(46,*) XBCSEC(5),BCCRMA(5)
                  DO I=1,50
                     WRITE(46,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(46)
C********************************
                  IBCSTATE=6
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '...GENERATING CROSS-SEC
     &             AND GRADE (3P2)...'
C             WRITE(3,'(A)') '...GENERATINDO I=1,50G CROSS-SEC AND GRADE (3P2)...'
                  CALL PARAPWAVE
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(6)=VEGSEC
                  BCCRMA(6)=CROSSMAX
                  WRITE(47,*) XBCSEC(6),BCCRMA(6)
                  DO I=1,50
                     WRITE(47,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(47)
C********************************
                  IBCSTATE=7
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '.GENERATING CROSS-SEC
     &             AND GRADE (1S0)_8...'
C             WRITE(3,'(A)') '.GENERATING CROSS-SEC AND GRADE (1S0)_8...'
                  CALL PARASWAVE(IBCSTATE)
                  IBCSTATE=1    !RETURN TO ORIGINAL DEFINITION 
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(7)=VEGSEC
                  BCCRMA(7)=CROSSMAX
                  WRITE(48,*) XBCSEC(7),BCCRMA(7)
                  DO I=1,50
                     WRITE(48,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(48)
C********************************
                  IBCSTATE=8
                  CROSSMAX=0.0D0 !added by hejb, to record the true max cross section
                  WRITE(*,'(A)') '.GENERATING CROSS-SEC
     &             AND GRADE (3S1)_8...'
C             WRITE(3,'(A)') '.GENERATING CROSS-SEC AND GRADE (3S1)_8...'
                  CALL PARASWAVE(IBCSTATE)
                  IBCSTATE=2    !RETURN TO ORIGINAL DEFINITION 
                  CALL VEGAS(TOTFUN,7,NUMBER,NITMX,2)
                  XBCSEC(8)=VEGSEC
                  BCCRMA(8)=CROSSMAX
                  WRITE(49,*) XBCSEC(8),BCCRMA(8)
                  DO I=1,50
                     WRITE(49,*) (XI(I,J),J=1,7)
                  END DO
                  REWIND(49)
               END IF           !IMIXTPYE=1 OR IMIXTYPE=3
            END IF              !IF IMIX=1

C******************************************************

C...RECORD THE (NEW) GENERATED GRADE.
C...hejb, not included for this version
C	     IF(IMIX.EQ.0) THEN
C	      DO I=1,50
C	       IF(IVEGGRADE.EQ.1) THEN
C		 WRITE(29,*) (XI(I,J),J=1,7)
C	       ELSE
C	          WRITE(11,*) (XI(I,J),J=1,7)
C	       END IF !IVEGRADE=1
C	      END DO
C	     END IF !IMIX=0

         ELSE                   !ISUBONLY=0 hejb
            WRITE(*,'(A,A,1X,G12.5)')'GETTING SUBPROCESS INFO....',
     &		 'AT C.M. ENERGY(GEV)',SUBENERGY
C		 WRITE(3,'(A,A,1X,G12.5)')'GETTING SUBPROCESS INFO....',
C     &		 'AT C.M. ENERGY(GEV)',SUBENERGY
            WRITE(*,'(A)') 'THE INFO. OF HADRON COLLIDER IS NO USE!!!'
C		 WRITE(3,'(A)') 'THE INFO. OF HADRON COLLIDER IS NO USE!!!'

C...Commented by hejb, not included for this version
C           IF(IVEGGRADE.EQ.1) THEN
C	       WRITE(*,'(A)')'USING EXISTED GRADE TO GENE. PRECISE GRADE.'
C	       WRITE(3,'(A)')'USING EXISTED GRADE TO GENE. PRECISE GRADE.'
C	       DO I=1,50
C	          READ(11,*) (XI(I,J),J=1,7)
C	       END DO
C	       OPEN(UNIT=29,FILE
C     &          =FULLPATHNAME(1:len_trim(FULLPATHNAME))//'/data/newgrade.dat'
C     &          ,STATUS='UNKNOWN')
C	   END IF	   
C	     CALL VEGAS(TOTFUN,5,NUMBER,NITMX,2)
C...RECORD THE (NEW) GENERATED GRADE.
C	    DO I=1,50
C	       IF(IVEGGRADE.EQ.1) THEN
C                  WRITE(29,*) (XI(I,J),J=1,7)
C	       ELSE
C                  WRITE(11,*) (XI(I,J),J=1,7)
C              END IF
C            END DO
         END IF                 !hejb The end of ISUBONLY=0
         WRITE(*,'(A)') '...... END OF VEGAS, GRADE GENERATED ......'
C	   WRITE(3,'(A)') '...... END OF VEGAS, GRADE GENERATED ......'
         IF(IEVNTDIS.EQ.1) THEN
            WRITE(*,'(A)')'OK, TO GET THE EVNT NUMBER DISTRIBUTIONS'
C	     WRITE(3,'(A)')'OK, TO GET THE EVNT NUMBER DISTRIBUTIONS'
         ELSE
            WRITE(*,'(A)')'OK, TO GET THE DIFFERENTIAL DISTRIBUTIONS'
C	     WRITE(3,'(A)')'OK, TO GET THE DIFFERENTIAL DISTRIBUTIONS'
         END IF
      ELSE                      !VEGASOPEN=true
CC         WRITE(*,'(A)')'(EXISTED) GRADE HAS NOT BEEN FURTHER IMPROVED'
C	   WRITE(3,'(A)')'(EXISTED) GRADE HAS NOT BEEN FURTHER IMPROVED'
         IF(IEVNTDIS.EQ.1) THEN
CC            WRITE(*,'(A)')'OK, TO GET THE EVNT NUMBER DISTRIBUTIONS'
C	     WRITE(3,'(A)')'OK, TO GET THE EVNT NUMBER DISTRIBUTIONS'
         ELSE
CC            WRITE(*,'(A)')'OK, TO GET THE DIFFERENTIAL DISTRIBUTIONS'
C	     WRITE(3,'(A)')'OK, TO GET THE DIFFERENTIAL DISTRIBUTIONS'
         END IF
CC         WRITE(*,'(A)') 
CC     &        '.............. END OF INITIALIZATION ...............'
C	   WRITE(3,'(A)') 
C     &	   '.............. END OF INITIALIZATION ...............'
         IF(ISUBONLY.EQ.0) THEN
            NDIM=7
         ELSE
            NDIM=5
            WRITE(*,'(A)')'GETTING THE INFO. OF THE SUBPORCESS....'
C		 WRITE(3,'(A)')'GETTING THE INFO. OF THE SUBPORCESS....'
         END IF

         RC=1.0D0/50.0D0
         DO 77 J=1,NDIM
            XI(50,J)=1.0D0
            DR=0.0D0
            DO 77 I=1,49
               DR=DR+RC
               XI(I,J)=DR
 77         CONTINUE

      END IF                 !The end of VEGASOPEN=1


C...USING THE EXISTED GRADE. ONE THING SHOULD BE CARE HERE IS THAT
C...THE EXISTED GRADE SHOULD BE FORMED UNDER THE SAME PARAMETERS.
      IF(IMIX.EQ.0) THEN
        IF(USEGRADE) THEN
CC            WRITE(*,'(A)') 'USING THE EXISTED VEGAS GRADE.'
C            WRITE(3,'(A)') 'USING THE EXISTED VEGAS GRADE.'
          
          IF(IBCSTATE.EQ.1)THEN
            READ(36,*) XBCSEC(1),BCCRMA(1)
            DO I=1,50
              READ(36,*) (XI(I,J),J=1,7)
            END DO
            REWIND(36)
            XSECUP(1)=XBCSEC(1)*2.0 !added by hejb, not sure it is right for every IDWTUP value.
          END IF
          IF(IBCSTATE.EQ.2)THEN
            READ(37,*) XBCSEC(2),BCCRMA(2)
            DO I=1,50
              READ(37,*) (XI(I,J),J=1,7)
            END DO
            REWIND(37)
            XSECUP(1)=XBCSEC(2)*2.0
          END IF  
          IF(IBCSTATE.EQ.3)THEN
            READ(38,*) XBCSEC(3),BCCRMA(3)
            DO I=1,50
              READ(38,*) (XI(I,J),J=1,7)
            END DO
            REWIND(38)
            XSECUP(1)=XBCSEC(3)*2.0
          END IF
          IF(IBCSTATE.EQ.4)THEN
            READ(39,*) XBCSEC(4),BCCRMA(4)
            DO I=1,50
              READ(39,*) (XI(I,J),J=1,7)
            END DO
            REWIND(39)
            XSECUP(1)=XBCSEC(4)*2.0
          END IF
          IF(IBCSTATE.EQ.5)THEN
            READ(46,*) XBCSEC(5),BCCRMA(5)
            DO I=1,50
              READ(46,*) (XI(I,J),J=1,7)
            END DO
            REWIND(46)
            XSECUP(1)=XBCSEC(5)*2.0
          END IF
          IF(IBCSTATE.EQ.6)THEN
            READ(47,*) XBCSEC(6),BCCRMA(6)
            DO I=1,50
              READ(47,*) (XI(I,J),J=1,7)
            END DO
            REWIND(47)
            XSECUP(1)=XBCSEC(6)*2.0
          END IF
C***************************************
          IF(IBCSTATE.EQ.7) THEN
            CALL PARASWAVE(IBCSTATE)
            IBCSTATE=1          !IBCSTATE=7 <=> IBCSTATE=1 & IOCTET=1
            READ(48,*) XBCSEC(7),BCCRMA(7)
            DO I=1,50
              READ(48,*) (XI(I,J),J=1,7)
            END DO
            REWIND(48)
            XSECUP(1)=XBCSEC(7)*2.0
          END IF
          IF(IBCSTATE.EQ.8) THEN
            CALL PARASWAVE(IBCSTATE)
            IBCSTATE=2          !IBCSTATE=8 <=> IBCSTATE=2 & IOCTET=1
            READ(49,*) XBCSEC(8),BCCRMA(8)
            DO I=1,50
              READ(49,*) (XI(I,J),J=1,7)
            END DO
            REWIND(49)
            XSECUP(1)=XBCSEC(8)*2.0
          END IF
        END IF                  !USEGRADE
      END IF                    !The end of IMIX=0



C...***************************************************************************************
C...added by hejb, to read the recorded cross sections
      IF(USEGRADE.AND.IMIX.EQ.1)THEN
        IF(IMIXTYPE.EQ.1.OR.IMIXTYPE.EQ.2)THEN
          READ(36,*) XBCSEC(1),BCCRMA(1)
          REWIND(36)
          READ(37,*) XBCSEC(2),BCCRMA(2)
          REWIND(37)
        END IF
C***************************************
        IF(IMIXTYPE.EQ.1.OR.IMIXTYPE.EQ.3)THEN
          READ(38,*) XBCSEC(3),BCCRMA(3)
          REWIND(38)
          READ(39,*) XBCSEC(4),BCCRMA(4)
          REWIND(39)
          READ(46,*) XBCSEC(5),BCCRMA(5)
          REWIND(46)
          READ(47,*) XBCSEC(6),BCCRMA(6)
          REWIND(47)
          READ(48,*) XBCSEC(7),BCCRMA(7)
          REWIND(48)
          READ(49,*) XBCSEC(8),BCCRMA(8)
          REWIND(49)
        END IF
      END IF
      

C...Cut from bcgen_upevnt, hejb 2006/03/20
C...To get the sum of the cross section of the eight Bc states
      IF(IMIX.EQ.1) THEN
        IF(IMIXTYPE.EQ.1) THEN 
          XBCSUM=0.0D0
          DO I=1,8
            XBCSUM=XBCSUM+XBCSEC(I)
          END DO
          IBCLIMIT=8
C...To caluculate the average cross section and give it to XSECUP(1),added by hejb
          XBCSSQ=0.0D0
          DO I=1,8
            XBCSSQ=XBCSSQ+XBCSEC(I)*XBCSEC(I)
          END DO
CC            XSECUP(1)=XBCSSQ/XBCSUM
          XSECUP(1)=XBCSUM*2.0
        END IF
C....******************************************************************************
        IF(IMIXTYPE.EQ.2) THEN 
          XBCSUM=0.0D0
          DO I=1,2
            XBCSUM=XBCSUM+XBCSEC(I)
          END DO
          IBCLIMIT=2
C...To caluculate the average cross section and give it to XSECUP(1)
          XBCSSQ=0.0D0
          DO I=1,2
            XBCSSQ=XBCSSQ+XBCSEC(I)*XBCSEC(I)
          END DO
CC            XSECUP(1)=XBCSSQ/XBCSUM
          XSECUP(1)=XBCSUM*2.0
        END IF
C...******************************************************************************
        IF(IMIXTYPE.EQ.3) THEN 
          XBCSUM=0.0D0
          DO I=3,8
            XBCSUM=XBCSUM+XBCSEC(I)
          END DO
          IBCLIMIT=8
C...To caluculate the average cross section and give it to XSECUP(1)
          XBCSSQ=0.0D0
          DO I=3,8
            XBCSSQ=XBCSSQ+XBCSEC(I)*XBCSEC(I)
          END DO 
CC            XSECUP(1)=XBCSSQ/XBCSUM
          XSECUP(1)=XBCSUM*2.0  
        END IF
      END IF
              

C---------------------------------------------

C...INITIAL VALUE FOR THE APPROXIMATE TOTAL CROSS-SECTION, WHICH IS 
C...EQUAL TO THE AVERAGING THE SUM OF THE CROSS-SECTIONS IN EACH POINT.

      APPCROSS =0.0D0
      BLANK    ='    '
      
 31   FORMAT('M_{Bc}=',G11.5,'M_{B}=',G11.5,'M_{C}=',G11.5)
 32   FORMAT('M_{Bc}=',G11.5,'M_{B}=',G11.5,'M_{C}=',G11.5,
     &     'f_{Bc}=',G11.5)
 41   FORMAT(A,1X,I9,';',3X,A,G11.3,1X)
 51   FORMAT('PMOMUP(',I3,',',I3,')=',G10.4)

 
C...**************************************************************************************
C ...The above were cut from bcvegpy.for by hejb
C...**************************************************************************************


C...Set up g+g --> B_c + b + c~ : maximum cross section in pb. 
C...Using the default XMAXUP(1)=0 to make PYEVNT accept almost 
C...all the UPEVNT events. CROSSMAX is the maximum differential
C...cross-section.       
      IF(IDWTUP.EQ.1) THEN
         IF(GENERATE) THEN
            IF(IVEGASOPEN.EQ.1) THEN
               XMAXUP(1)=CROSSMAX
            ELSE
C               WRITE(*,'(A)') 
C     &              'WARNING: HERE SHOULD INPUT
C     &              A MAXIMUM DIFFERENTIAL CROSS-SEC'
C               WRITE(*,'(A)')
C     &              '!STOP HERE! INPUT A PROPER 
C     &              VALUE IN (SUBROUTINE UPINIT) !!!!'
C               STOP 'OR RUNNING VEGAS TO GET THE
C     &              CORRECT VALUE !PROGRAM STOP!'
               XMAXUP(1) = 200000. 
C...      XMAXUP(1)=! THE VALUE ADDED HERE! FIND THE VALUE IN OLD RUNS.
C...NOTE: THIS VALUE SHOULD BE THE ONE OBTAINED UNDER THE SAME CONDITION.
            END IF
         ELSE
            XMAXUP(1)=0.0D0
         END IF
      END IF
C...THE VALUE OF XSECUP(1) CAN BE GIVEN ARBITRARILY. IT IS ONLY
C...USED FOR PYSTAT(1) TO PRODUCE A SENSIBLE CROSS-SECTION TABLE.
C...THE ACTURAL TOTAL CROSS-SEC IS (APPCROSS).
C...Commented by hejb 2006/03/20
C      IF (IDWTUP.EQ.3) THEN
C         XSECUP(1)=VEGSEC       ! VEGAS VALUE FOR INITIALIZATION.
C         XMAXUP(1)=CROSSMAX
C      END IF

C********************************************
         
 999  RETURN
      END
 
