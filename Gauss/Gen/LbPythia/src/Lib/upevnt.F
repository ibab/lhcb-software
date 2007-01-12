C $Id: upevnt.F,v 1.7 2007-01-12 15:21:18 ranjard Exp $  
C*********************************************************************

      BLOCK DATA GAUSSDATA
#include "GaussUserProcess.inc"
      SAVE /GAUSSUSERPROCESS/      
      DATA PYTHIAGAUSSPROCESS /0/
      END
 
C...UPEVNT
C...Dummy routine, to be replaced by a user implementing external
C...processes. Depending on cross section model chosen, it either has
C...to generate a process of the type IDPRUP requested, or pick a type
C...itself and generate this event. The event is to be stored in the
C...HEPEUP commonblock, including (often) an event weight.
      
      SUBROUTINE UPEVNT
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYDAT1/,/PYPARS/
 
C...User process event common block.
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/
      
#include "GaussUserProcess.inc"
      
      IF ( PYTHIAGAUSSPROCESS .EQ. LHAREADPROCESS ) THEN
C...Read info from file.
      	IF(MSTP(162).GT.0) THEN
          READ(MSTP(162),*,END=110,ERR=110) NUP,IDPRUP,XWGTUP,SCALUP,
     &      AQEDUP,AQCDUP
          DO 100 I=1,NUP
            READ(MSTP(162),*,END=110,ERR=110) IDUP(I),ISTUP(I),
     &        MOTHUP(1,I),MOTHUP(2,I),ICOLUP(1,I),ICOLUP(2,I),
     &        (PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I)
 100      CONTINUE
          RETURN
C...Special when reached end of file or other error.
 110      NUP=0
          
C...Else not implemented.
      	ELSE
          GOTO 999
      	ENDIF
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ACERMCPROCESS    ) THEN
        CALL ACERMC_UPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. BCVEGPYPROCESS   ) THEN
        CALL BCVEGPY_UPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. LHEREADPROCESS   ) THEN
        CALL LHEUPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. HIDVALLEYPROCESS ) THEN
        CALL HIDVALLEY_UPEVNT 
      ENDIF
      
 999  RETURN
      END 
      
      SUBROUTINE ACERMC_UPEVNT
      PRINT*, 'WARNING ..... Dummy Routine to redefine !'
      END

      SUBROUTINE BCVEGPY_UPEVNT
      PRINT*, 'WARNING ..... Dummy Routine to redefine !'
      END
      
      SUBROUTINE HIDVALLEY_UPEVNT
      PRINT*,
     &  'WARNING ..... Dummy Routine HIDVALLEY_UPEVNT to redefine !'
      END
