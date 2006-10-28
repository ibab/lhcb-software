C $Id: hvevent.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
C ======================================================================
C $Log: not supported by cvs2svn $ 
C ======================================================================
      INTEGER FUNCTION hvevent ( IDUMMY ) 
C ======================================================================
C  Purpose   : generate oen Hidden Valley event
C  Arguments : 
C
C  An Hidden valley production
C  The actual lines are extracted from HV0.403 package 
C  written by Matt Strassler <strasslr@noether.phys.washington.edu>"
C
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2006-10-23
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
C Pythia 
C ======================================================================
#include "Pythia.inc"
C ======================================================================
C Local 
C ======================================================================
#include "HidValley.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      INTEGER  IDUMMY
C ======================================================================
      INTEGER  HVSTEP1 
      EXTERNAL HVSTEP1 
      INTEGER  HVSTEP2 
      EXTERNAL HVSTEP2 
      INTEGER  HVSTEP3 
      EXTERNAL HVSTEP3 
C ======================================================================
      
      
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC STAGE ONE : MAKE PARTON-LEVEL q qbar --> Z'(*) --> e+ e- EVENTS
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      IF ( SUCCESS .NE. HVSTEP1( IDUMMY ) ) THEN 
        WRITE(*,*)'HVEVENT: error from HVSTEP1'
        GO TO 999
      ENDIF 
      
C - Set up "brute-force" low-mass q-qbar system and hadronize it.
C...Process the event to extract vpions, combine them with STAGE 1
C... initial stage and fake neutrinos into a consistent parton-level
C... event, stored in output file hH.lhf.dat in LHA-like format
      IF ( SUCCESS .NE. HVSTEP2( IDUMMY )  ) THEN
        WRITE(*,*)'HVEVENT: error from HVSTEP2 '
        GO TO 999
      ENDIF 
      
C... run the actual generation of the prepared partonic event 
      IF ( SUCCESS .NE. HVSTEP3( IDUMMY ) ) THEN
        WRITE(*,*)'HVEVENT: error from HVSTEP3:'
        GO TO 999
      ENDIF 
      
C ======================================================================
      hvevent = SUCCESS 
      RETURN
 999  CONTINUE 
      hvevent = FAILURE 
      RETURN 
      END
C ======================================================================
      
      
C ======================================================================
C The END
C ======================================================================
