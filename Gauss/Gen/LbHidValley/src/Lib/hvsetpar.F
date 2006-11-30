C $Id: hvsetpar.F,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $ , version  $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C
C ======================================================================
      DOUBLE PRECISION FUNCTION hvsetpar ( VALUE , TITLE ) 
C ======================================================================
C  Purpose   : set parameters from Hidden Valley production
C  Arguments : 
C
C  An Hidden valley production
C  The actual lines are extracted from HV0.403 package 
C  written by Matt Strassler <strasslr@noether.phys.washington.edu>"
C
C  Matt Strassler <strasslr@noether.phys.washington.edu>:
C
C  "This program simulates q qbar -> Z' -> v-quarks, followed by
C  v-showering and v-hadronization to v-pions.  The current program
C  makes various approximations and is not suitable for precision 
C  studies, but should be good at the factor-of-two level.
C  In particular, the v-hadronization code uses Pythias QCD
C  hadronization code, scaled up in energy, and this is not a perfect 
C  simulation of the real model. Also, the Z' charges are not precisely 
C  those discussed in ref [1]; but in any case the model of [1] was 
C  just an example, and many other models exist.  Updates to this 
C  program will gradually correct these issues."
C
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2006-10-22
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
#include "HidValley.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      DOUBLE PRECISION VALUE 
      CHARACTER*(*)    TITLE 
      INTEGER          LENOCC 
      EXTERNAL         LENOCC 
C ======================================================================
      
      HVSETPAR = -100000 
      
      IF     ( 'ZPMASS' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        ZPMASS   = VALUE 
        HVSETPAR = ZPMASS 
      ELSEIF ( 'PIMASS' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        PIMASS   = VALUE 
        HVSETPAR = PIMASS 
      ELSEIF ( 'PIZLIF' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        PIZLIF   = VALUE 
        HVSETPAR = PIZLIF 
      ELSEIF ( 'PIPLIF' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        PIPLIF   = VALUE 
        HVSETPAR = PIPLIF 
      ELSE 
        WRITE(*,*) 'INVALID TITLE:"' , TITLE(1:LENOCC(TITLE)) , '"'
      ENDIF 
      

  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
