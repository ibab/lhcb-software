C...*************************************************************************
C...To do the initialization
C...Modified by hejb, 2006-03-14
C...*************************************************************************

      SUBROUTINE BCVEGPY_UPEVNT
 
C...All real arithmetic in double precision.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...User process event common block.
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

      CHARACTER*472 BASENAME
      CHARACTER*472 ENERGYNAME      
      CHARACTER*472 FULLPATHNAME
      
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     & 	COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
      
      COMMON/GRADE/XI(50,10)
      COMMON/COUNTER/IBCSTATE,NEV
      LOGICAL GENERATE,VEGASOPEN,USEGRADE

      COMMON/GENEFULL/GENERATE

C...GET THE APPROXIMATE TOTAL CORSS-SECTION.
      COMMON/TOTCROSS/APPCROSS

C...TO GET THE DISTRIBUTION OF AN EXTRA FACTOR Z=(2(k1+k2).P_BC)/SHAT.
      COMMON/EXTRAZ/ZFACTOR,ZMIN,ZMAX

C...TO GET THE SUBPROCESS CROSS-SECTION.
      COMMON/SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY

C...FOR TRANSFORM THE SUBPROCESS INFORMATION, I.E.,  WHETHER USING
C...THE SUBPROCESS q\bar{q}->Bc+b+\bar{c} TO GENERATE EVENTS.
      COMMON/QQBAR/IQQBAR,IQCODE

      DIMENSION X(10),IA(10)

C.... common block for deciding whether using existing grade
C...  IUSECURDIR added (20090723), to enable the usage of the grade files
C...  in the current directory (`pwd`), mainly for Grid.  
      COMMON/LOGGRADE/IEVNTDIS,IGENERATE,IVEGASOPEN,IGRADE,IUSECURDIR

c...XSECUP(8) RECORDS THE TOTAL DIFFERENTIAL CROSS-SECTIONS FOR DIFFERENT
C...STATES: 1---Singlet 1S0; 2---singlet 3s1; 7---octet 1s0; 8---octet 3s1;
C...3---Singlet 1p1; 4---Singlet 3p0; 5---Singlet 3p1; 6---Singlet 3p2.
      COMMON/MIXEVNT/XBCSEC(8),IMIX,IMIXTYPE

C...hejb:CROSSMAX needed for us 
      COMMON/PTPASS/PTMIN,PTMAX,CROSSMAX,ETAMIN,ETAMAX,
     &SMIN,SMAX,YMIN,YMAX,PSETAMIN,PSETAMAX

C....To store CROSSMAX of each Bc state
      COMMON/CRMABC/BCCRMA(8),XBCSUM,IBCLIMIT

C...THERE LIST SOME TYPICAL WAYS FOR RECORDING THE $BCVEGPYDATAROOT/DATA. USERS MAY USE
C...THEIR OWN CONVENIENT WAY TO DO SO.
      
  
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

C      WRITE(*,*) " FULLPATHNAME", FULLPATHNAME

C.the following from bcvegy
C.*******************************************************

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
      END IF                    !IMIX=1       
      

      IF(IGRADE.EQ.1) THEN
         USEGRADE=.TRUE.
      ELSE
         USEGRADE=.FALSE.
      END IF

       
C**********************************************************
      IF(IMIX.EQ.1) THEN
         IF(IMIXTYPE.EQ.1.OR.IMIXTYPE.EQ.2) IBCSTATE=0
         IF(IMIXTYPE.EQ.3) IBCSTATE=2
         RANBC=XBCSUM*PYR(0)
 55      IBCSTATE=IBCSTATE+1
         RANBC =RANBC-XBCSEC(IBCSTATE)
         IF(IBCSTATE.LT.IBCLIMIT .AND. RANBC.GT.1.0D-16) GO TO 55

C**************************************
         IF(IBCSTATE.EQ.1) THEN
            CALL PARASWAVE(IBCSTATE)
            IF(USEGRADE) THEN
               READ(36,*) XBCSEC(1),BCCRMA(1)
               DO I=1,50
                  READ(36,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(36)
         END IF
C***************************************
         IF(IBCSTATE.EQ.2) THEN
            CALL PARASWAVE(IBCSTATE)
            IF(USEGRADE) THEN
               READ(37,*) XBCSEC(2),BCCRMA(2)
               DO I=1,50
                  READ(37,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(37)
         END IF
C***************************************
         IF(IBCSTATE.EQ.3) THEN
            CALL PARAPWAVE
            IF(USEGRADE) THEN
               READ(38,*) XBCSEC(3),BCCRMA(3)
               DO I=1,50
                  READ(38,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(38)
         END IF
C***************************************
         IF(IBCSTATE.EQ.4) THEN
            CALL PARAPWAVE
            IF(USEGRADE) THEN
               READ(39,*) XBCSEC(4),BCCRMA(4)
               DO I=1,50
                  READ(39,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(39)
         END IF
C***************************************
         IF(IBCSTATE.EQ.5) THEN
            CALL PARAPWAVE
            IF(USEGRADE) THEN
               READ(46,*) XBCSEC(5),BCCRMA(5)
               DO I=1,50
                  READ(46,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(46)
         END IF
C***************************************
         IF(IBCSTATE.EQ.6) THEN
            CALL PARAPWAVE
            IF(USEGRADE) THEN
               READ(47,*) XBCSEC(6),BCCRMA(6)
               DO I=1,50
                  READ(47,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(47)
         END IF
C***************************************
         IF(IBCSTATE.EQ.7) THEN
            CALL PARASWAVE(IBCSTATE)
            IBCSTATE=1          !RETURN TO ORIGINAL DEFINITION
            IF(USEGRADE) THEN
               READ(48,*) XBCSEC(7),BCCRMA(7)
               DO I=1,50
                  READ(48,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(48)
         END IF
C***************************************
         IF(IBCSTATE.EQ.8) THEN
            CALL PARASWAVE(IBCSTATE)
            IBCSTATE=2          !RETURN TO ORIGINAL DEFINITION
            IF(USEGRADE) THEN
               READ(49,*) XBCSEC(8),BCCRMA(8)
               DO I=1,50
                  READ(49,*) (XI(I,J),J=1,7)
               END DO
            END IF
            REWIND(49)
         END IF

      END IF                    ! END OF IF--(IMIX=1)

C**********************************************************
C. The above from bcvegpy
C**********************************************************


C...Call the respective routine to generate event.
      IF(IDPRUP.EQ.1001) THEN
         XND  =5.0D+1
         IF(ISUBONLY.EQ.0) THEN
            NDIM =7
         ELSE  
            NDIM =5
         END IF

C...USING THE ALREADY GENERATED GRADE TO GENERATE THE EVENTS POINTS.
         CALL GENERAND(NDIM,XND,X,IA,WGT)
         CALL PHPOINT(X,WT)
         IF(WT.LT.1.0D-16) THEN
            XWGTUP=0.0D0
         ELSE
            XWGTUP=TOTFUN(X,WT)*WGT
         END IF
         
         IF(IDWTUP.EQ.1.AND.GENERATE) THEN
* a dirty trick: limiting the diff xsection to current XWGTUP
* The current XWGTUP is high enough, so the total xsection 
* does not deviate much. this trick is from Yu Gouz's program
            IF(XWGTUP.GT.XMAXUP(1)) THEN
               XWGTUP = XMAXUP(1)*0.9999999D0
            END IF
         END IF

C...CALCULATE APPROXIMATE CROSSECTION.
         APPCROSS=APPCROSS+XWGTUP/NEV*1.0D-3 !nb

C...RECORD THE MAXIMUM DIFFERENTIAL CROSS-SECTION.
         IF(XWGTUP.GT.CROSSMAX) THEN
            CROSSMAX=XWGTUP
         END IF
c...gluon-gluon fusion. for all the s- and p- wave states.
         IF(IQQBAR.EQ.0) THEN
            CALL BCPYTHIA(21)
         END IF
c...quark-antiquark annihilation, only for s-wave.
         IF(IQQBAR.EQ.1.AND.(IBCSTATE.EQ.1.OR.IBCSTATE.EQ.2)) THEN
            CALL BCPYTHIA(IQCODE)
         END IF
      ELSE
         WRITE(*,*) 'Fatal error! Unknown process',IDPRUP
         STOP
      END IF

      RETURN
      END

