C $Id: $
C ======================================================================
      DOUBLE PRECISION FUNCTION ONIAPAIRSGETPAR ( TITLE ) 
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
#include "oniapairs.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      CHARACTER*(*)    TITLE 
      INTEGER          LNBLNK 
C      EXTERNAL         LENOCCHV 
C ======================================================================
      
      IF     ( 'ECM'   .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = ECM 
c      ELSEIF ( 'NRUNS' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
c        oniapairsgetpar = NRUNS 
      ELSE 
        WRITE(*,*) 'INVALID NAME:"' , TITLE(1:LNBLNK(TITLE)), '"'
        oniapairsgetpar = 0        
      ENDIF 
      
  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
