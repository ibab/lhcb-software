C $Id: upevnt.F,v 1.1 2008-03-16 20:20:15 robbep Exp $  
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
#include "HepXUP.inc"
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
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ACERMCPROCESS     ) THEN
        CALL ACERMC_UPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. BCVEGPYPROCESS    ) THEN
        CALL BCVEGPY_UPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. LHEREADPROCESS    ) THEN
        CALL LHEUPEVNT
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. HIDVALLEYPROCESS  ) THEN
        CALL HIDVALLEY_UPEVNT 
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ALPGENREADPROCESS ) THEN
        CALL ALPGEN_UPEVNT 
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ONIAPAIRSPROCESS  ) THEN
        CALL ONIAPAIRS_UPEVNT 
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. GENXICCPROCESS    ) THEN
        CALL GENXICC_UPEVNT
      ENDIF
      
 999  RETURN
      END 
