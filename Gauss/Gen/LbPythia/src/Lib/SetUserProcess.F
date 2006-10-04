C $Id: SetUserProcess.F,v 1.2 2006-10-04 14:16:51 ibelyaev Exp $
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
#include "LbPythia/GaussUserProcess.inc"      
C-----------------------------------------------------------------------
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER INDEX
C-----------------------------------------------------------------------
      
      PYTHIAGAUSSPROCESS = INDEX
      
C-----------------------------------------------------------------------
  999 RETURN
      END
