C $Id: pdflib_utils.F,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
      SUBROUTINE pdflib_init
C-----------------------------------------------------------------------
C  Purpose   : Initialize pdflib (suppress output)
C  Arguments : 
C
C  Author    : Patrick Robbe
C  Created   : 2005-02-21
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER N6
      COMMON/W50517/ N6

      N6 = 73
C      OPEN( UNIT=73 , STATUS='SCRATCH' )  ! gives pb in lxbatch
      OPEN( UNIT=73 , FILE='pdflib_init.tmp' )  
  999 RETURN
      END

      SUBROUTINE pdflib_end
      IMPLICIT NONE
      CLOSE ( UNIT=73 ) 
      RETURN
      END
