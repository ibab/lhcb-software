C $Id: hvaction.F,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C 
C ======================================================================
      DOUBLE PRECISION FUNCTION hvaction ( IDUMMY ) 
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
C  Created   : 2006-10-02
C ======================================================================
      IMPLICIT NONE 
C ======================================================================
C  Include Files
C ======================================================================
C  Pythia/GENSER:
C ======================================================================
#include "Pythia.inc"
C ======================================================================
C Local 
C ======================================================================
#include "HidValley.inc"
C ======================================================================
C Arguments & Local Variables 
C ======================================================================
      INTEGER I,IDUMMY
C ======================================================================
      
      DO I = 5,9  ! skip the initial protons and the parent partons
        
        QEE   ( I-4 ) = PYP(I,4)   ! store four-momenta
        QPX   ( I-4 ) = PYP(I,1)   ! 
        QPY   ( I-4 ) = PYP(I,2)
        QPZ   ( I-4 ) = PYP(I,3)
        QMS   ( I-4 ) = PYP(I,5)   ! and mass [need the Z'* mass]
        IQTYP ( I-4 ) = K(I,2)     ! and particle type
        
      ENDDO 
            
C - At this point compute boost and rotation of the final state
      
      QQVX = QPX(3)/QEE(3)
      QQVY = QPY(3)/QEE(3)
      QQVZ = QPZ(3)/QEE(3)
      
      CALL PYROBO(0,0,0.D0,0.D0,-QQVX,-QQVY,-QQVZ)
      
C - Now the event should be in its rest frame 
C - Now want to rotate final state onto z-axis
C - Currently we have no reason to care about phi versus pi-phi
C - SO WE WILL NOT BE CAREFUL AT PRESENT!!!
      
      QQPHI = PYP(8,15) 
      QQTHT = PYP(8,13) 
      
      HVACTION = QMS(3)         ! ECM in this mode is the Z'* mass
                                ! It is passed back to the main program
                                ! and stored in the array ECMHAT
      
 999  RETURN
      END

C ======================================================================
C  The END 
C ======================================================================
