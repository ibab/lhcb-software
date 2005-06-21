C $Id: upinit.F,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $ 
C*********************************************************************
 
C...UPINIT
C... User process initialization routine

      SUBROUTINE UPINIT( IISET )
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...User process initialization commonblock.
      INTEGER MAXPUP, IISET
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &LPRUP(MAXPUP)
      SAVE /HEPRUP/

      INTEGER PYTHIAGAUSSPROCESS 
      COMMON/GAUSSUSERPROCESS/ PYTHIAGAUSSPROCESS

      IF ( PYTHIAGAUSSPROCESS .EQ. 0 ) THEN
        GOTO 999
      ELSEIF ( PYTHIAGAUSSPROCESS .EQ. 1 ) THEN
        CALL ACERMC_UPINIT( IISET )  
        GOTO 999
      ENDIF
 999  RETURN
      END 

      SUBROUTINE ACERMC_UPINIT( IISET ) 
      INTEGER IISET
      PRINT*, 'WARNING... Dummy Routine !'
      END


      
