C $Id: photos_utils.F,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
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
