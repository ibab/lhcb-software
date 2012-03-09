C $Id: upinit.F,v 1.4 2009-09-28 15:44:45 robbep Exp $ 
C*********************************************************************
 
C...UPINIT
C...Dummy routine, to be replaced by a user implementing external
C...processes. Is supposed to fill the HEPRUP commonblock with info
C...on incoming beams and allowed processes.
 
      SUBROUTINE UPINIT( IISET )
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYDAT1/,/PYPARS/
 
#include "HepXUP.inc" 
#include "GaussUserProcess.inc" 
      
      IF ( PYTHIAGAUSSPROCESS .EQ. LHAREADPROCESS  ) THEN
C...Read info from file.
        IF(MSTP(161).GT.0) THEN
          READ(MSTP(161),*,END=110,ERR=110) IDBMUP(1),IDBMUP(2),EBMUP(1)
     &      ,EBMUP(2),PDFGUP(1),PDFGUP(2),PDFSUP(1),PDFSUP(2),IDWTUP
     &      ,NPRUP
          DO 100 IPR=1,NPRUP
            READ(MSTP(161),*,END=110,ERR=110) XSECUP(IPR),XERRUP(IPR),
     &        XMAXUP(IPR),LPRUP(IPR)
 100      CONTINUE
          RETURN
C...Error or prematurely reached end of file.
 110      WRITE(MSTU(11),5000)
          STOP
          
C...Else not implemented.
        ELSE
          GOTO 999
        ENDIF
        
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ACERMCPROCESS    ) THEN
        CALL ACERMC_UPINIT( IISET )  
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. BCVEGPYPROCESS   ) THEN
        CALL BCVEGPY_UPINIT( IISET ) 
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. LHEREADPROCESS   ) THEN
        IF ( IISET .EQ. 1 ) CALL LHEUPINIT
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. HIDVALLEYPROCESS ) THEN
        CALL HIDVALLEY_UPINIT ( IISET ) 
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ALPGENREADPROCESS ) THEN
        IF ( IISET .EQ. 1 ) CALL ALPGEN_UPINIT  
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. ONIAPAIRSPROCESS ) THEN
        CALL ONIAPAIRS_UPINIT ( IISET ) 
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. GENXICCPROCESS   ) THEN
        CALL GENXICC_UPINIT( IISET ) 
      ELSE 
        WRITE(MSTU(11),5001) 
      ENDIF
      
 5000 FORMAT(1X,'Error: UPINIT routine failed to read information'/
     &  1X,'Execution stopped!')
 5001 FORMAT(1X,'Error: UPINIT routine: ILLEGAL PYTHIAGAUSSPROCESS'/
     &  1X,'Execution stopped!')
      
 999  RETURN
      END 
      
