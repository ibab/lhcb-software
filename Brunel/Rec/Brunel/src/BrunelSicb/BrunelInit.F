      SUBROUTINE BRUNELINIT
*****************************************************************
*                                                               *
*   Subroutine BRUNELINIT                                       *
*                                                               *
*     FORTRAN initialisations for Brunel                        *
*        First initialisation must be done on first event       *
*        after SicbCnvSvc has done its initialisations...       *
*        Subsequent event initialisations most be done before   *
*        first call to any other SICB event processing          *
*                                                               *
*   called by:  Brunel.Initialisation.cpp -> execute()          *
*                                                               *
*****************************************************************
#ifndef DOC
      IMPLICIT NONE
#include "inc/gcflag.inc"
#include "inc/cbpass.inc"
#include "inc/atmc_bank.inc"
#include "inc/avmc_bank.inc"
#include "inc/utwindow.inc"

#include "BrunelZebra.inc"

	LOGICAL FIRST /.TRUE./
      INTEGER IRECO,IDEBG,IPRNT,IHIST
      SAVE    IRECO,IDEBG,IPRNT,IHIST
*
      INTEGER IUVERS,LUN

	INTEGER IS1, IS2, NHEAD
	SAVE    NHEAD
*----------------------------------------------------------------
*
*
	IF( FIRST ) THEN
		FIRST = .FALSE.

C Initialisation of PAW
		CALL HLIMIT(-NWPAW)

C SIDBDST initialisation step
		CALL UGINIT
C RECRUN
		IEVENT = 0
C REDIGI
	ENDIF
C RECRUN
	IEVENT = IEVENT + 1
C RECTRIG event initialisation 
C
C - set DEBUG flag
      IF (IEVENT.GE.IDEMIN .AND. IEVENT.LE.IDEMAX) THEN
         IDEBUG=1
         IF (ITEST.GT.0) THEN
           IF (MOD(IEVENT,ITEST).EQ.0) THEN
             CALL GRNDMQ(IS1,IS2,0,'G')
             WRITE(6,1000) IEVENT,IS1,IS2
 1000 FORMAT(//,' *** RECTRIG *** trigger ',I7,' Random Seeds = ',
     &        I10,2X,I10)
           ENDIF
         ENDIF
      ELSE
         IDEBUG=0
      ENDIF                  

C REVIN/REVINIT stuff is done by GAUDI, except:
	
	CALL UTPASS(2)

      END
#endif




