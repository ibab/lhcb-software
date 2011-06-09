C $Id: $
C ----------------------------------------------------------------------
      SUBROUTINE lhesave
C-----------------------------------------------------------------------
C  Purpose   : 
C  Arguments : 
C
C  Author    : Vanya Belyaev
C  Created   : 2011-06-09
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
#include "HepXUP.inc"
#include "HepRupSave.inc"
C- Local Variables
      INTEGER IPR 
C-----------------------------------------------------------------------
      
      SIDBMUP ( 1 ) = IDBMUP ( 1 ) 
      SIDBMUP ( 2 ) = IDBMUP ( 2 ) 
      
      SEBMUP  ( 1 ) = EBMUP  ( 1 ) 
      SEBMUP  ( 2 ) = EBMUP  ( 2 ) 
      
      SPDFGUP ( 1 ) = PDFGUP ( 1 ) 
      SPDFGUP ( 2 ) = PDFGUP ( 2 ) 
      
      SPDFSUP ( 1 ) = PDFSUP ( 1 ) 
      SPDFSUP ( 2 ) = PDFSUP ( 2 ) 
      
      SIDWTUP       = IDWTUP 
      SNPRUP        = NPRUP 
      
      DO 100 IPR=1,NPRUP
        SXSECUP ( IPR ) = XSECUP ( IPR )
        SXERRUP ( IPR ) = XERRUP ( IPR )
        SXMAXUP ( IPR ) = XMAXUP ( IPR )
        SLPRUP  ( IPR ) = LPRUP  ( IPR )
 100  CONTINUE
      
  999 RETURN
      END
