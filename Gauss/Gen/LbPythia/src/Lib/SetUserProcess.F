C $Id: SetUserProcess.F,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
      SUBROUTINE SetUserProcess( INDEX )
C-----------------------------------------------------------------------
C  Purpose   : Set User Process in Pythia For Gauss
C  Arguments : Type of User process to set
C
C  Author    : Patrick Robbe
C  Created   : 2005-03-06
C-----------------------------------------------------------------------
      IMPLICIT NONE
      INTEGER INDEX, PYTHIAGAUSSPROCESS
      COMMON/GAUSSUSERPROCESS/ PYTHIAGAUSSPROCESS

      PYTHIAGAUSSPROCESS = INDEX
 
C- Include Files
C- Local Variables
C-----------------------------------------------------------------------
  999 RETURN
      END
