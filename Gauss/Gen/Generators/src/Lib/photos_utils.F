C $Id: photos_utils.F,v 1.2 2006-02-05 21:01:44 robbep Exp $
      SUBROUTINE photos_init( FILENAME ) 
C-----------------------------------------------------------------------
C  Purpose   : Initialize photos (suppress output)
C  Arguments : 
C
C  Author    : Patrick Robbe
C  Created   : 2005-02-21
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
      CHARACTER*(*) FILENAME
C-----------------------------------------------------------------------
      INTEGER PHLUN
      COMMON/PHOLUN/ PHLUN

      PHLUN = 73
C      OPEN( UNIT=73 , STATUS='SCRATCH' )  ! gives pb in lxbatch
      OPEN( UNIT=73 , FILE=FILENAME( 1:LEN(FILENAME) ) )
  999 RETURN
      END

      SUBROUTINE photos_end
      IMPLICIT NONE
      CLOSE ( UNIT=73 ) 
      RETURN
      END
