C...The user decision is to be conveyed by the IVETO value.
C...IVETO = 0 : retain current event and generate in full;
C...      = 1 : abort generation of current event and move to next.
 
      SUBROUTINE UPVETO(IVETO)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)

C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYDAT1/,/PYPARS/

#include "GaussUserProcess.inc"
 
      IF ( PYTHIAGAUSSPROCESS .EQ. ALPGENREADPROCESS ) THEN
        CALL ALPGEN_UPVETO( IVETO )
        GOTO 999
      ELSE 
        WRITE(MSTU(11),5001) 
        STOP
      ENDIF
      
 5001 FORMAT(1X,'Error: UPVETO routine: ILLEGAL PYTHIAGAUSSPROCESS'/
     &  1X,'Execution stopped!')

 999  RETURN
      END
