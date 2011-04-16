C $Id: hvupinit.F,v 1.3 2008-06-24 10:49:13 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.2  2006/11/30 12:37:01  ibelyaev
C  v1r1: veryfy all opts-files and selection
C
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C 
C ======================================================================
      SUBROUTINE HVUPINIT(IIISET)
C ======================================================================
C  Purpose   : 
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
C Pythia/GENSER 
C ======================================================================
#include "HepXUP.inc"
C ======================================================================
C Local 
C ======================================================================
#include "HidValley.inc"
C ======================================================================
      INTEGER IIISET 
      LOGICAL FIRST
      DATA    FIRST /.TRUE./
      SAVE    FIRST 
C ======================================================================
      IF ( FIRST  ) THEN 
        WRITE(*,*)'LbHidValley: HIDDEN VALLEY UPINIT'
        FIRST = .FALSE.
      ENDIF         
C ======================================================================
C 
      
      IDBMUP (1) = 2212 
      IDBMUP (2) = 2212 
      
C      EBMUP  (1) = 7.0E+03 
C      EBMUP  (2) = 7.0E+03 
      
      EBMUP  (1) = BEAMENE 
      EBMUP  (2) = BEAMENE 
      
      PDFGUP (1) =  4 
      PDFGUP (2) =  4 
      PDFSUP (1) = 46 
      PDFSUP (2) = 46
      IDWTUP     =  1 
      NPRUP      =  1 
      
      XSECUP (1) = 1.0
      XERRUP (1) = 1.0 
      XMAXUP (1) = 1.0
      LPRUP  (1) = 661 

C ======================================================================
  999 RETURN
      END
C ======================================================================


C ======================================================================
C The END 
C ======================================================================
      
