C $Id: hvupevnt.F,v 1.3 2008-06-24 10:49:13 ibelyaev Exp $
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
      SUBROUTINE HVUPEVNT
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
C Include files 
C ======================================================================
C Pythia/GENSER 
C ======================================================================
#include "Pythia.inc"
#include "HepXUP.inc"
C ======================================================================
C Local 
C ======================================================================
#include "HidValley.inc"
C ======================================================================
      INTEGER I
      LOGICAL FIRST
      DATA FIRST /.TRUE./
      SAVE FIRST 

C ======================================================================
      IF ( FIRST  ) THEN 
        WRITE(*,*)'LbHidValley: HIDDEN VALLEY UPEVENT'
        FIRST = .FALSE.
      ENDIF         
C ======================================================================

      NUP          = NCNTR + 2 
      IDPRUP       = 661 
      XWGTUP       = 1.0 
      SCALUP       = PIMASS 
      AQEDUP       = 1.82E-03
      AQCDUP       = 1.16E-01
      
C*    (1) and (2) are initial partons 
      IDUP   (1)   = IQTYP(1) 
      ISTUP  (1)   =   -1 
      MOTHUP (1,1) =    0  
      MOTHUP (2,1) =    0
      
      PUP    (1,1) =  QPX(1)
      PUP    (2,1) =  QPY(1)
      PUP    (3,1) =  QPZ(1)
      PUP    (4,1) =  QEE(1)
      PUP    (5,1) =   0.0
      VTIMUP (1)   = 0.0 
      SPINUP (1)   = 9.0 
      
      IDUP   (2)   = IQTYP(2) 
      ISTUP  (2)   =   -1 
      MOTHUP (1,2) =    0  
      MOTHUP (2,2) =    0
      
      PUP    (1,2) =  QPX(2)
      PUP    (2,2) =  QPY(2)
      PUP    (3,2) =  QPZ(2)
      PUP    (4,2) =  QEE(2)
      PUP    (5,2) =   0.0
      VTIMUP (1)   = 0.0 
      SPINUP (1)   = 9.0 
      
C*    make sure quarks get color not anticolor 
      IF ( IQTYP(1) .GT. 0 ) THEN
        ICOLUP (1,1) =  501 
        ICOLUP (2,1) =    0
        ICOLUP (1,2) =    0
        ICOLUP (2,2) =  501
      ELSE
        ICOLUP (1,1) =    0 
        ICOLUP (2,1) =  501
        ICOLUP (1,2) =  501 
        ICOLUP (2,2) =    0       
      ENDIF 
      
C*    v-pions 
      DO I=3,NCNTR 
        IDUP   (I)   = NTYP(I) 
        ISTUP  (I)   = 1 
        MOTHUP (1,I) = 1 
        MOTHUP (2,I) = 2 
        ICOLUP (1,I) = 0 
        ICOLUP (2,I) = 0 
        PUP    (1,I) = NPX(I) 
        PUP    (2,I) = NPY(I) 
        PUP    (3,I) = NPZ(I) 
        PUP    (4,I) = NEE(I) 
        PUP    (5,I) = PMAS(NTYP(I),1)
        VTIMUP (I)   = 0.0 
        SPINUP (I)   = 0.0 
      ENDDO 
      
C*    neutrinos 
      DO I=NCNTR+1,NCNTR+2
        IDUP   (I)   = NTYP(I) 
        ISTUP  (I)   = 1 
        MOTHUP (1,I) = 1 
        MOTHUP (2,I) = 2 
        ICOLUP (1,I) = 0 
        ICOLUP (2,I) = 0 
        PUP    (1,I) = NPX(I) 
        PUP    (2,I) = NPY(I) 
        PUP    (3,I) = NPZ(I) 
        PUP    (4,I) = NEE(I) 
        PUP    (5,I) = 0.0 
        VTIMUP (I)   = 0.0 
        SPINUP (I)   = 0.0 
      ENDDO 
        
C ======================================================================
  999 RETURN
      END
C ======================================================================

C ======================================================================
C The END 
C ======================================================================
