*
      SUBROUTINE SetCDFFname(istat, decayNameValue, cdfFileName )

*
********************************************************************
*                                                                  *
* Subroutine SetCDFFname                                           *
*                                                                  *
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
#include "utio/ucdfile.inc"
*
*-----------------------------------------------------------------
* Input variables
      CHARACTER*4   decayNameValue
      CHARACTER*100 cdfFileName
* Output variables
      INTEGER ISTAT
* Externals
      INTEGER IUCOMC
* Local variables
      INTEGER NDET, NFILE
      INTEGER ISTEND
*-----------------------------------------------------------------
*
* Set initial values in common block as in input arguments
*
      ISTAT = 0
      NDET = IUCOMC(decayNameValue,CHDETE_F,N_DETE_F)
      IF( NDET.EQ.0 ) THEN
        NDET = IUCOMC('B0PP',CHDETE_F,N_DETE_F)
        IF( NDET.EQ.0 ) THEN
          ISTAT = 1
          GOTO 999
        ELSE
          N_DETE_F = N_DETE_F+1
          INDFILE_F(N_DETE_F) = INDFILE_F(NDET)
          CHDETE_F(N_DETE_F) = decayNameValue
        ENDIF
      ENDIF
      NFILE = INDFILE_F(NDET)
      ISTEND = INDEX(cdfFileName,'!')
      IF( ISTEND.LE.0 ) THEN
        ISTAT = 1
      ELSE
        CHFILE_F(NFILE) = cdfFileName(1:ISTEND-1)
      ENDIF
*
 999  RETURN
      END
*
* End of SetCDFFName
