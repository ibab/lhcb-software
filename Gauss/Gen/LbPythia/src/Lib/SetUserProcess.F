C $Id: SetUserProcess.F,v 1.3 2007-01-12 15:21:18 ranjard Exp $
      SUBROUTINE SetUserProcess( INDEX )
C-----------------------------------------------------------------------
C  Purpose   : Set User Process in Pythia For Gauss
C  Arguments : Type of User process to set
C
C  Author    : Patrick Robbe
C  Created   : 2005-03-06
C-----------------------------------------------------------------------
      IMPLICIT NONE
C-----------------------------------------------------------------------
C- Include Files
C-----------------------------------------------------------------------
#include "GaussUserProcess.inc"      
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER INDEX
C-----------------------------------------------------------------------
      
      PYTHIAGAUSSPROCESS = INDEX
      
C-----------------------------------------------------------------------
  999 RETURN
      END
