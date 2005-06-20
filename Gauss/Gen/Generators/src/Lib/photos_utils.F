C $Id: photos_utils.F,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
      SUBROUTINE photos_init
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
C-----------------------------------------------------------------------
      INTEGER PHLUN
      COMMON/PHOLUN/ PHLUN

      PHLUN = 73
C      OPEN( UNIT=73 , STATUS='SCRATCH' )  ! gives pb in lxbatch
      OPEN( UNIT=73 , FILE='photos_init.tmp' )  
  999 RETURN
      END

      SUBROUTINE photos_end
      IMPLICIT NONE
      CLOSE ( UNIT=73 ) 
      RETURN
      END
