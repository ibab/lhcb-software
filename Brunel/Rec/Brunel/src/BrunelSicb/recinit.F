      SUBROUTINE RECINIT
*****************************************************************
*                                                               *
C! make some general initializations                            *
*                                                               *
*   called by:  UGINIT                                          *
*                                                               *
*   Author: F.Ranjard - 991101                                  *
*****************************************************************
#ifndef DOC
      IMPLICIT NONE
#include "inc/gcbank.inc"
#include "inc/cbflag.inc"
#include "inc/cbwpar.inc"
#include "inc/cblist.inc"
#include "inc/cbrung.inc"
#include "inc/cbpass.inc"
#include "inc/cbfread.inc"
#include "inc/cbunit.inc"
#include "utio/ucvers.inc"
C#include "util/version.inc"
*-----!-----------------------------------------------------------------
      INTEGER ISTAT,IDATE,ITIME,IVERSQ
      CHARACTER TITLE*28
*-----!-----------------------------------------------------------------
*
*  Get program version number  
*
      IODETM = 0
CMC NPILEUP is set inside GAUDI event selector
CMC      NPILEUP = 1
      CALL SICBVERS (IVERSQ,IDATE,ITIME)
      IPRVERS = IVERSQ
*
      ISETUPG = MSETLS(1)
      ISETUPS = MSETLS(2)
      NTVERSD = 0
*
*  Get the Header record if data are read in in the job
*
CMC Header record already read by Gaudi event selector 
CMC      IF(LUGETX.NE.0) THEN
CMC         CALL UTHEADER(ISTAT)
CMC         IF(ISTAT.NE.0) THEN
CMC            WRITE(LUPRNT,*) ' >>>>> RECINIT: No header record in the',
CMC     >           ' data sample specified in GETX stream'
CMC         ENDIF
CMC      ENDIF
*
*  Interpret SKIP data card
*
CMC SKIP card has no effect in Brunel - use jobOptions instead
CMC      CALL SKIPCARD
*
*  Detector initialization
*
      CALL DETINIT
*
*  Print Title
*
      CALL DATIME (IDATE,ITIME)
      TITLE = 'LHCb RECONSTRUCTION PROGRAM '
      WRITE(LUPRNT,801) TITLE,IVERSQ,IDATE,ITIME
 801  FORMAT(1H1,//20X,A28,3X,'VERSION',I8,'  DATE',I8,'  TIME',I5/)
*
      END
#endif




