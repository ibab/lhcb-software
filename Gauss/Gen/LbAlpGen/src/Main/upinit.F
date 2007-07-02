C $Id: upinit.F,v 1.1.1.1 2007-07-02 09:25:04 ibelyaev Exp $
      SUBROUTINE upinit
C-----------------------------------------------------------------------
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2007-06-26
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
C-----------------------------------------------------------------------
      print *,'i am upINIT (MAIN) '
      CALL ALPGEN_UPINIT
  999 RETURN
      END
