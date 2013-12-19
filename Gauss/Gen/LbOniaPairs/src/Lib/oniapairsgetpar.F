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
C ======================================================================
      
      IF     ( 'ECM'   .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = ECM 
      ELSEIF ( 'PSI1S1S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(1)
      ELSEIF ( 'PSI2S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(2)
      ELSEIF ( 'PSI1S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(3)
      ELSEIF ( 'UPS1S1S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(11)
      ELSEIF ( 'UPS2S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(12)
      ELSEIF ( 'UPS3S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(13)
      ELSEIF ( 'UPS1S2S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(14)
      ELSEIF ( 'UPS1S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(15)
      ELSEIF ( 'UPS2S3S' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = Kfct(16)
      ELSEIF ( 'SCFALPS' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = ScfAlpS
      ELSEIF ( 'SCFPDF'  .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = ScfPDF
      ELSEIF ( 'SCFSHWR' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = ScfShwr
      ELSEIF ( 'MAXWGHTMULT' .EQ. TITLE(1:LNBLNK(TITLE)) ) THEN 
        oniapairsgetpar = MaxWghtMult
      ELSE 
        WRITE(*,*) 'INVALID NAME:"' , TITLE(1:LNBLNK(TITLE)), '"'
        oniapairsgetpar = 0        
      ENDIF 
      
  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
