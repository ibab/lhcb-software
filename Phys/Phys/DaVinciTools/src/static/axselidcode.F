*
      SUBROUTINE AXSelIDCode(decayNameValue,decCode)

*
********************************************************************
*                                                                  *
* Subroutine AXSelIDCode                                           *
*                                                                  *
* Routine to retrieve the decay code as set in axmakelist.F and    *
* stored in axdecaylist.inc                                        *
*                                                                  *
* Called by: AxSelAlgorithm                                        *
*                                                                  *
* Author: G.Corti, CERN                                            *
* Date  : 19 June 2001                                             *
*                                                                  *
********************************************************************
*
      IMPLICIT NONE
*
#include "axdecaylist.inc"
*
*-----------------------------------------------------------------
* Input variables
      CHARACTER*4 decayNameValue
* Output variable 
      INTEGER decCode
* External
      INTEGER IUCOMC
* Local variables
      INTEGER NSize, IDEC
*-----------------------------------------------------------------
*
* Set initial values in common block as in input arguments
*
      NSize = NDecays
      IDEC = 0
      IDEC = IUCOMC(decayNameValue,DECAYList,NSize)
      IF( IDEC.NE.0 ) THEN
        decCode = DECAYCode(IDEC)
      ELSE
        decCode = 0
      ENDIF
*
      RETURN
      END
*
* End of AXSelIDCode
