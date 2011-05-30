C $Id: $
C ======================================================================
      DOUBLE PRECISION FUNCTION ONIAPAIRSSETPAR ( VALUE , TITLE ) 
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
#include "oniapairs.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      DOUBLE PRECISION VALUE 
      CHARACTER*(*)    TITLE 
      INTEGER          LNBLNK 
C      EXTERNAL         LENOCCHV 
C ======================================================================
      
      IF     ( 'ECM' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        ECM = VALUE 
c      ELSEIF ( 'NRUNS' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
c        NRUNS = VALUE 
      ELSE 
        WRITE(*,*) 'INVALID TITLE:"' , TITLE(1:LNBLNK(TITLE)) , '"'
      ENDIF 

  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
