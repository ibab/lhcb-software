      SUBROUTINE DaVinciInit
*****************************************************************
*                                                               *
*   Subroutine DaVinciInit                                      *
*                                                               *
*     FORTRAN initialisations for DaVinci                       *
*        First initialisation must be done on first event       *
*        after SicbCnvSvc has done its initialisations...       *
*        Subsequent event initialisations most be done before   *
*        first call to any other SICB event processing          *
*                                                               *
*   called by:  DaVinciInitialisation.cpp -> execute()          *
*                                                               *
*****************************************************************
      IMPLICIT NONE
#include "inc/gcflag.inc"
#include "inc/cbpass.inc"
*
#include "DaVinciZebra.inc"

      LOGICAL FIRST /.TRUE./
*
*----------------------------------------------------------------
C
      IF( FIRST ) THEN
        FIRST = .FALSE.
C
C Initialisation of PAW
        CALL HLIMIT(-NWPAW)
C Initialisation of vertex geometry :: it is necessary to have these parameters
        CALL VRPIGEOM
C Initialize event counter
        IEVENT = 0
      ENDIF
C
      IEVENT = IEVENT + 1
C REVIN/REVINIT stuff is done by GAUDI, except initialization
C of common blocks internally used by axreclib	
C
      CALL AXDSTINIT
C
      END
