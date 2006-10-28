C $Id: hvaction.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
C ======================================================================
C $Log: not supported by cvs2svn $ 
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
      
      DO I = 5,9                ! skip the initial protons and the parent partons
        
        QEE(I-4) = PYP(I,4)     ! store four-momenta
        QPX(I-4) = PYP(I,1)     ! 
        QPY(I-4) = PYP(I,2)
        QPZ(I-4) = PYP(I,3)
        QMS(I-4) = PYP(I,5)     ! and mass [need the Z'* mass]
        IQTYP(I-4) = K(I,2)     ! and particle type
        
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
