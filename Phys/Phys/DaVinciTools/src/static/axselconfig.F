*
      SUBROUTINE AXSelConfig(iSELModeValue,iSELLevValue,
     &                       iSELTagValue,iFLGPrntValue,iFLGHistValue,
     &                       decayNameValue)

*
********************************************************************
*                                                                  *
* Subroutine AXSelConfig                                           *
*                                                                  *
* Routine to pass the jobOptions configuration to th axsel package *
* Replaces axseldata.F used when running from SICBDST              *
* See documentation of AxSelAlgorithm for meaning of variables     *
*                                                                  *
* Called by: AxSelAlgorithm                                        *
*                                                                  *
* Author: G.Corti, CERN                                            *
* Date  : 08 May 2001                                              *
*                                                                  *
********************************************************************
*
      IMPLICIT NONE
*
#include "axselcard.inc"
*
*-----------------------------------------------------------------
* Input variables
      CHARACTER*4 decayNameValue
      INTEGER     iSelModeValue 
      INTEGER     iSelLevValue
      INTEGER     iSelTagValue 
      INTEGER     iFLGPrntValue   
      INTEGER     iFLGHistValue
*-----------------------------------------------------------------
*
* Set initial values in common block as in input arguments
*
      DecayName = decayNameValue
      ISELMode  = iSELModeValue
      ISELLev   = iSELLevValue
      ISELTag   = iSELTagValue
      IFLGPrnt  = iFLGPrntValue
      IFLGHist  = iFLGHistValue
* The follwoing is for debugging
C      WRITE(6,*) 'DecayName =',DecayName
C      WRITE(6,*) 'ISELMode =', ISELMode
C      WRITE(6,*) 'ISELLev =', ISELLev
C      WRITE(6,*) 'ISELTag =', ISELTag
C      WRITE(6,*) 'IFLGPrnt =', IFLGPrnt
C      WRITE(6,*) 'IFLGHist =', IFLGHist
*
      RETURN
      END
*
* End of AXSelData
