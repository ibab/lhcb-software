      SUBROUTINE ALSHCD(CSHO)
      CHARACTER*3 CSHO
      CSHO='PYT'
      END

      SUBROUTINE ALSHEN
      END

      SUBROUTINE ALSHER(I)
      INTEGER I
      I=0
      RETURN
      END
C----------------------------------------------------------------------
      SUBROUTINE ALSHIN(I)
C----------------------------------------------------------------------
C     subroutine to initialise the events
C----------------------------------------------------------------------
      IMPLICIT NONE
      INCLUDE 'alplib/alpsho.inc'
      INTEGER I
C--LOCAL VARIABLES
C     EASY-TEXT
      CHARACTER*10 CGIVE
      CHARACTER*12 CGIV2
      CHARACTER*32 CPASS
C LOCAL VARIABLES
      DOUBLE PRECISION MQ
      INTEGER IQ
C COMMUNICATION CODE
      I=661
C FILL IN PYTHIA SPECIFIC INPUTS
      IF(MC.NE.0) THEN
        IQ=4
        MQ=MC
        WRITE(CGIVE,'(I5)') IQ
        WRITE(CGIV2,'(D12.5)') MQ
        CPASS='PMAS('//CGIVE//',1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(IQ,1)=MQ
      ENDIF 
      IF(MB.NE.0) THEN
        IQ=5
        MQ=MB
        WRITE(CGIVE,'(I5)') IQ
        WRITE(CGIV2,'(D12.5)') MQ
        CPASS='PMAS('//CGIVE//',1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(IQ,1)=MQ
      ENDIF 
      IF(MT.NE.0) THEN
        IQ=6
        MQ=MT
        WRITE(CGIVE,'(I5)') IQ
        WRITE(CGIV2,'(D12.5)') MQ
        CPASS='PMAS('//CGIVE//',1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(IQ,1)=MQ
      ENDIF 
      IF(MW.NE.0) THEN
        WRITE(CGIV2,'(D12.5)') MW
        CPASS='PMAS(24,1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(24,1)=MW
      ENDIF
      IF(MZ.NE.0) THEN
        WRITE(CGIV2,'(D12.5)') MZ
        CPASS='PMAS(23,1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(23,1)=MZ
      ENDIF
      IF(MH.NE.0) THEN
        WRITE(CGIV2,'(D12.5)') MH
        CPASS='PMAS(25,1)='//CGIV2
        CALL PYGIVE(CPASS)
C     /**/         PMAS(25,1)=MH
      ENDIF
      END

C-----------------------------------------------------------------------
      SUBROUTINE ALSFIN
C-----------------------------------------------------------------------
C     PYTHIA END OF FILE TREATMENT
C-----------------------------------------------------------------------
C--   GUP Event common block
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &     IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &     ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &     SPINUP(MAXNUP)
      NUP=0
      END
