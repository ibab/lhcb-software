*-- Author :    A.Tsaregorodtsev   03/11/96
      SUBROUTINE UDINIT
*****************************************************************
*                                                               *
*   Subroutine UDINIT                                           *
*                                                               *
*   Subroutine to make initializations for DST run              *
*                                                               *
*   called by:                                                  *
*                                                               *
*   Author: A.Tsaregorodtsev, 03/11/96 19.03.04                 *
*   modified by: F.Ranjard 990929                               *
*                remove call to UBLIND already called in UTINIT *
*****************************************************************
      IMPLICIT NONE
#include "inc/cbfread.inc"
#include "inc/cbunit.inc"
      CHARACTER FNAME*80
      INTEGER LF, LNBLNK
      LOGICAL LEXIST
*-----!-----------------------------------------------------------------
*
      LUPRNT = 6
*
*   Open data cards file, STOP if not there
*
      CALL GETENVF ('SICBCARDS',FNAME)
      IF (FNAME.EQ.' ') FNAME = 'sicb.dat'
      LF = LNBLNK(FNAME)
      INQUIRE(FILE=FNAME(1:LF),EXIST=LEXIST)
      IF(LEXIST) THEN
        OPEN(UNIT=4,FILE=FNAME(1:LF),STATUS='OLD')
        CALL FFSET('LINP',4)
        WRITE(LUPRNT,*) ' ----- UDINIT: Data cards are taken from ',
     >                  FNAME(1:LF)
      ELSE
        WRITE(LUPRNT,*)' ----- UDINIT: ',FNAME(1:LF),
     >      ' cards file is not found. STOP'
        STOP
      ENDIF

*
      CALL UTDATA
      CALL FFGO
      CLOSE(4)
*
      LUDEBU = LUPRNT
*
*   Initialize the DATA MODEL
*
CMC      CALL UTINIT
	CALL UTIOPASAVX
	CALL UTSVERS
*
      END
 







