      SUBROUTINE SICBVERS(IVERSQ,IDATQQ,ITIMQQ)
#include "BrunelVersion.h"
      IMPLICIT NONE
      INTEGER iversq, idatqq, itimqq
C ---------------------------------------------------
C! return version number, dummy date and time
C - M.Cattaneo - 010514
C ----------------------------------------------------
C iversq = Brunel v *10000 + r * 100 + p * 100
      iversq = BrunelVersion
      idatqq = 999999
      itimqq = 999999
*
      end



