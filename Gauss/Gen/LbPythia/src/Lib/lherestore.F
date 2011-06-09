C $Id: $
      SUBROUTINE lherestore 
C-----------------------------------------------------------------------
C  Purpose   : 
C  Arguments : 
C
C  Author    : Vanya Belyaev
C  Created   : 2011-06-09
C-----------------------------------------------------------------------
      IMPLICIT NONE
      
      INTEGER MSTU, MSTJ
      DOUBLE  PRECISION PARU,PARJ 
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      
C- Include Files
#include "HepXUP.inc"
#include "HepRupSave.inc"
C- Local Variables
      INTEGER IPR 
      LOGICAL FIRST 
      DATA    FIRST /.TRUE./
      SAVE    FIRST 
C-----------------------------------------------------------------------
      
      IDBMUP ( 1 ) = SIDBMUP ( 1 ) 
      IDBMUP ( 2 ) = SIDBMUP ( 2 ) 
      
      EBMUP  ( 1 ) = SEBMUP  ( 1 ) 
      EBMUP  ( 2 ) = SEBMUP  ( 2 ) 
      
      PDFGUP ( 1 ) = SPDFGUP ( 1 ) 
      PDFGUP ( 2 ) = SPDFGUP ( 2 ) 
      
      PDFSUP ( 1 ) = SPDFSUP ( 1 ) 
      PDFSUP ( 2 ) = SPDFSUP ( 2 ) 
      
      IDWTUP       = SIDWTUP 
      NPRUP        = SNPRUP 
      
      DO 100 IPR=1,NPRUP
        XSECUP ( IPR ) = SXSECUP ( IPR )
        XERRUP ( IPR ) = SXERRUP ( IPR )
        XMAXUP ( IPR ) = SXMAXUP ( IPR )
        LPRUP  ( IPR ) = SLPRUP  ( IPR )
 100  CONTINUE
      
      if ( FIRST ) THEN 
        FIRST = .FALSE. 
        WRITE(MSTU(11),*) 'LHERESTORE: restore LHE-data:'
        WRITE(MSTU(11),*) ' IDBMUP       ',IDBMUP(1),'/',IDBMUP(2)
        WRITE(MSTU(11),*) ' EBMUP        ', EBMUP(1),'/', EBMUP(2)
        WRITE(MSTU(11),*) ' PDFGUP       ',PDFGUP(1),'/',PDFGUP(2)
        WRITE(MSTU(11),*) ' PDFSUP       ',PDFSUP(1),'/',PDFSUP(2)
        WRITE(MSTU(11),*) ' IDWTUP/NPRUP ',IDWTUP   ,'/', NPRUP
       ENDIF 
             
  999 RETURN
      END
