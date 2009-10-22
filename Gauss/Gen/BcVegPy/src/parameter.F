CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C...THIS SUBROUTINE IS USED TO SET THE NECESSARY PARAMETERS FOR      C
C...THE INITIALIZATION FOR HADRONIC PRODUCTION OF Bc MESON.          C
C...TO USE THE PROGRAM YOU'D BETTER MAKE A DIRECTORY: (DATA) TO      C
C...SAVE ALL THE OBTAINED DATA-FILES.                                C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C   TO HAVE A BETTER UNDERSTANDING OF SETTING THE PARAMETERS         C
C   YOU MAY SEE THE README.DAT FILE TO GET MORE INFORMATION.         C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C Copyright (c) Z.X ZHANG, Chafik Driouich, Paula Eerola and X.G. Wu C
C reference: hep-ph/0309120                                          C
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      SUBROUTINE SETPARAMETER
C...Preamble: declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...PYTHIA common block.
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      
C...User process event common block.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      
C...Transform of running information.
      COMMON/CONFINE/PTCUT,ETACUT
C...PARAMETERS TRANSFORMTION.
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
      COMMON/FUNTRANS/NQ2,NPDFU
      COMMON/RCONST/PI
      COMMON/USERTRAN/ISHOWER,IDPP
      COMMON/VEGCROSS/VEGSEC,VEGERR,IVEGGRADE
C...TO transform THE SUBPROCESS CROSS-SECTION.
      COMMON/SUBOPEN/SUBFACTOR,SUBENERGY,ISUBONLY
C...transform of pdf information
      COMMON/OUTPDF/IOUTPDF,IPDFNUM
C...TRANSFORM THE SUBPROCESS INFORMATION
      COMMON/QQBAR/IQQBAR,IQCODE
C...transform of the VEGAS information
      COMMON/VEGASINF/NUMBER,NITMX
C...transform the events number and Bc state.
      COMMON/COUNTER/IBCSTATE,NEV
C...transform some variables
C...  IUSECURDIR added (20090723), to enable the usage of the grade files
C...  in the current directory (`pwd`), mainly for Grid.  
      COMMON/LOGGRADE/IEVNTDIS,IGENERATE,IVEGASOPEN,IGRADE,IUSECURDIR
C...IOCTET--WHETHER GETTING THE COLOR-OCTET COMPONENT CONTRIBUTIONS. 
C...HERE ONLY FOR gg->(c\bar{b})+b+~c, (c\bar{b}) IN color-octet 
c...S-WAEE STATES. COEOCT--COEFFICIENT FOR COLOR-OCTET
C...MATRIXMENT: RELATION BETWEEN THE COLOR-OCTET MATRIXMENT AND THE
C...COLOR SINGLET MATRIXMENT.
      COMMON/COLOCT/IOCTET
      COMMON/OCTMATRIX/COEOCT
C...TRANSFORM THE FIRST DERIVATIVE OF THE WAVEFUNCTION AT THE ZERO OR THE
C...WAVEFUNCTION AT THE ZERO.
      COMMON/WAVEZERO/FBC
c...XSECUP(8) RECORDS THE TOTAL DIFFERENTIAL CROSS-SECTIONS FOR DIFFERENT
C...STATES: 1---Singlet 1S0; 2---singlet 3s1; 7---octet 1s0; 8---octet 3s1;
C...3---Singlet 1p1; 4---Singlet 3p0; 5---Singlet 3p1; 6---Singlet 3p2.
      COMMON/MIXEVNT/XBCSEC(8),IMIX,IMIXTYPE

      PI = DACOS(-1.0D0)

      IF(IMIX.EQ.1)THEN
         IF(ISUBONLY.EQ.0 .AND. IOUTPDF.EQ.1 .AND. IPDFNUM.EQ.300)
     &        CALL SetCtq6(4)
         RETURN
      END IF
         
C...*********************************************************
C...The following works only for imix=0
C...*********************************************************

C...for 1S, R(0)=1.241  --->  \psi(0)=(0.35).
C...Modified by hejb for 7 & * /2006/03/17
      IF(IBCSTATE.EQ.1.OR.IBCSTATE.EQ.2.OR.
     &   IBCSTATE.EQ.7.OR.IBCSTATE.EQ.8) THEN
         FBC =1.241
         FBCC=DSQRT(3.0D0*FBC**2/PI/PMBC)
      ELSE
C...for 2P, R'(0)=0.44833  --->  \psi'(0)=(0.219); R'(0)**2=(0.201)
C...THE VALUE OF P-WAVE MATRIX ELEMENT <0|P|0>.
         FBC =0.44833
         FBCC=FBC**2*9.0D0/(2.0D0*PI)
      END IF

        
C...ERROR MESSAGE.

C...Commented by hejb, added already. 2006/03/17
C      IF((IBCSTATE.GT.6).OR.(IBCSTATE.LT.1)) THEN
C	 WRITE(*,*) 'ONLY S AND P-WAVE STATES (1-6) HAVE BEEN PROGRAMMED!'
C	 STOP
C      ENDIF

      IF((IBCSTATE.GT.2).AND.(IQQBAR.EQ.1)) THEN
       WRITE(*,*) 'P-WAVE STATES FOR q-\bar{q} HAS NOT BEEN PROGRAMMED!'
       STOP
      ENDIF

      IF((IQQBAR.EQ.1) .AND. IOCTET.EQ.1) THEN
	 WRITE(*,'(A)')
     &        'THE COLOR-OCTET CONTRIBUTION 
     &    FROM QUARK-ANTIQUARK ANNIHILATION'
         WRITE(*,'(A)')
     &        'IS SMALL AND HAS NOT BEEN PROGRAMMED
     &    FOR Bc PRODUCTION      !!!!!!!' 
         STOP  '-----STOP THE PROGRAM-------'
      END IF

      IF(((IBCSTATE.EQ.3).OR.(IBCSTATE.EQ.4).OR.(IBCSTATE.EQ.5).OR.
     &     (IBCSTATE.EQ.6)) .AND. (IOCTET.EQ.1)) THEN
	 WRITE(*,'(A)')
     &        'THE COLOR-OCTET CONTRIBUTION FROM P-WAVE STATES'
         WRITE(*,'(A)')
     &        'IS SMALL AND HAS NOT BEEN PROGRAMMED
     &    FOR Bc PRODUCTION      !!!!!!!' 
         STOP  '-----STOP THE PROGRAM-------'
      END IF

C...SETTING INITIAL VALUE FOR CTEQ6L.
      IF(ISUBONLY.EQ.0 .AND. IOUTPDF.EQ.1 .AND. IPDFNUM.EQ.300)
     &     CALL SetCtq6(4)

      CALL PARAMETERS()
      CALL DPARAMETERS()
      CALL COUPLING()
      
      RETURN
      END

C******************************************************
C...     SOME FREQUENTLY USED PARAMETERS.
C******************************************************
      SUBROUTINE PARAMETERS() 
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
      COMMON/RCONST/PI
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)

      hbcvm=PMBC
      hbcm=PMBC
      fmb=PMB
      fmc=PMC

      wwpi=PI
      hbcp=FBCC

      fb1=(3*hbcm**2)/(8*fmb*fmc)
      fb2=(3*(fmb-fmc)*hbcm)/(8*fmb*fmc)
      fb3=hbcm**2/(8*fmb*fmc)
      fb4=((fmb-fmc)*hbcm)/(8*fmb*fmc)

C...THE COLOR FACTOR \DELTA_{IJ}/3 OF THE BOUND STATE HAS
C...BEEN INCLUDED IN THE FOLLOWING DEFINIATION FOR
C...THE P-WAVE MATRIX ELEMENTS TIMES THE FACTOR FROM BOUND
C...STATE, <0|P|0>*C_P.
      cbc1p1=dsqrt(hbcp/(72.0D0*hbcm**3))
      cbcp0 =dsqrt(hbcp/(216.0D0*hbcm**3))
      cbcp1 =dsqrt(hbcp/(144.0D0*hbcm**3))
C...HERE FOR SIMPLICITY ONE (hbcm) HAS BEEN ABSORBED INTO
C...MATRIX ELEMENT.
      cbcp2 =dsqrt(hbcp/(72.0D0*hbcm))

      RETURN
      END

C************************************************
C...   THIS IS ONLY USED FOR P-WAVE GENERATION.
C...   SOME SIMPLIFIED PAREMTERS.
C************************************************
      SUBROUTINE DPARAMETERS()
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PPP/PP(4,40),guv(4)
      include 'inclcon.f'
 
      guv(1)= 1.0D0
      guv(2)=-1.0D0
      guv(3)=-1.0D0
      guv(4)=-1.0D0

      fmc2=fmc**2
      fmb2=fmb**2
      fmb3=fmb**3
      fmc3=fmc**3

      hbcvm2=hbcvm**2
      hbcm2 =hbcm**2
      hbcm3 =hbcm**3
      hbcm4 =hbcm**4
      hbcm5 =hbcm**5

      dhbcvm2=1.0d0/hbcvm2
      dhbcm2 =1.0d0/hbcm2

      ffmcfmb=fmc/(fmb+fmc)

      RETURN
      END

C***********************************************
C******** PARAMETERS FOR S-WAVE ****************
C***********************************************
      
      SUBROUTINE PARASWAVE(IBCO)
C...Preamble: declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      
      COMMON/RCONST/PI
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
      COMMON/WAVEZERO/FBC
      COMMON/COLOCT/IOCTET
      COMMON/OCTMATRIX/COEOCT

      IOCTET =0
      FBC =1.241D0
      FBCC=DSQRT(3.0D0*FBC**2/PI/PMBC)

C....FOR COLOR-OCTET STATES. 
      IF(IBCO.EQ.7.OR.IBCO.EQ.8) THEN
         IOCTET =1
         COEOCT =0.2D0          ! THE VALUE OF \delta_s(v^2)
                                ! DEFINED IN PRD71,074012(2005)
      END IF
       
      CALL PARAMETERS()
      CALL DPARAMETERS()
      CALL COUPLING()
	
      RETURN
      END

C***********************************************
C******** PARAMETERS FOR P-WAVE ****************
C***********************************************

      SUBROUTINE PARAPWAVE
C...Preamble: declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

      COMMON/RCONST/PI
      DOUBLE COMPLEX COLMAT,BUNDAMP
      COMMON/UPCOM/ECM,PMBC,PMB,PMC,FBCC,PMOMUP(5,8),
     &     COLMAT(10,64),BUNDAMP(4),PMOMZERO(5,8)
      COMMON/WAVEZERO/FBC
      COMMON/COLOCT/IOCTET
      COMMON/OCTMATRIX/COEOCT

      IOCTET   =0
      FBC =0.44833D0
      FBCC=FBC**2*9.0D0/(2.0D0*PI)
       
      CALL PARAMETERS()
      CALL DPARAMETERS()
      CALL COUPLING()
	
      RETURN
      END
