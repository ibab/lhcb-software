C $Id: pythiaoutput_utils.F,v 1.3 2005-11-04 10:55:41 robbep Exp $
      SUBROUTINE pythiaoutput_init( IVAL )
C-----------------------------------------------------------------------
C  Purpose   : Initialize Pythia output (suppress output)
C  Arguments : IVAL : 0 -- Keep output   1 -- Suppress output
C
C  Author    : Patrick Robbe
C  Created   : 2005-02-21
C-----------------------------------------------------------------------
      IMPLICIT NONE
C- Include Files
C- Local Variables
C-----------------------------------------------------------------------
      INTEGER IVAL
      INTEGER MSTU, MSTJ
      DOUBLE PRECISION PARU, PARJ
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)

      IF ( IVAL.EQ.0 ) THEN
        IF (MSTU(12).NE.12345) MSTU( 12 ) = 1
        MSTU( 13 ) = 1
        MSTU( 25 ) = 1
        MSTP( 122 ) = 1
      ELSEIF ( IVAL.EQ.1 ) THEN
        MSTU( 12 ) = 12345
        MSTU( 13 ) = 0
        MSTU( 25 ) = 0
        MSTP( 122 ) = 0
      ENDIF
      
  999 RETURN
      END

