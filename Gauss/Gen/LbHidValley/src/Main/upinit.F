C $Id: upinit.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
      SUBROUTINE upinit
C-----------------------------------------------------------------------
C  Purpose   : 
C  Arguments : 
C
C  Author    : Ivan BELYAEV
C  Created   : 2006-10-23
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER IISET 
C-----------------------------------------------------------------------
      IISET  = 1 
C-----------------------------------------------------------------------
      CALL HIDVALLEY_UPINIT(IISET)
  999 RETURN
      END
