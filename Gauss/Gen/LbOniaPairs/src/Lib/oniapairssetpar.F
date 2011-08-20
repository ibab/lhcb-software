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
      ELSEIF ( 'PSI1S1S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(1) = (VALUE.gt.0D0)
        Kfct(1) = VALUE
      ELSEIF ( 'PSI2S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(2) = (VALUE.gt.0D0)
        Kfct(2) = VALUE
      ELSEIF ( 'PSI1S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(3) = (VALUE.gt.0D0)
        Kfct(3) = VALUE
      ELSEIF ( 'UPS1S1S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(11) = (VALUE.gt.0D0)
        Kfct(11) = VALUE
      ELSEIF ( 'UPS2S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(12) = (VALUE.gt.0D0)
        Kfct(12) = VALUE
      ELSEIF ( 'UPS3S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(13) = (VALUE.gt.0D0)
        Kfct(13) = VALUE
      ELSEIF ( 'UPS1S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(14) = (VALUE.gt.0D0)
        Kfct(14) = VALUE
      ELSEIF ( 'UPS1S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(15) = (VALUE.gt.0D0)
        Kfct(15) = VALUE
      ELSEIF ( 'UPS2S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        SwOn(16) = (VALUE.gt.0D0)
        Kfct(16) = VALUE
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
