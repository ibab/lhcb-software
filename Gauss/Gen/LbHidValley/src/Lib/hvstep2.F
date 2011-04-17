C $Id: hvstep2.F,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C 
C ======================================================================
      INTEGER FUNCTION hvstep2 ( IDUMMY ) 
C ======================================================================
C  Purpose   : perform the second step pof Hidden Valley production
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
C  Created   : 2006-10-23
C ======================================================================
      IMPLICIT NONE 
C ======================================================================
C- Include Files
C ======================================================================
#include "Pythia.inc"
C =====================================================================
C Local 
C =====================================================================
#include "HidValley.inc"
C =====================================================================
C- Local Variables
C ======================================================================
      INTEGER IDUMMY,I, IREPEAT
      DOUBLE  PRECISION ECMHAT,ECM,VQSCALE  
C ======================================================================
      INTEGER  MDCYBACK(500)
C ======================================================================
      DOUBLE  PRECISION HVACTION
      EXTERNAL HVACTION 
C ======================================================================
      INTEGER  HVMAKE 
      EXTERNAL HVMAKE 
C ======================================================================
      
C...Scale factor for converting from QCD to vQCD is the ratio
C... of the V-pion mass PIMASS to the ordinary pion mass 135 MeV

      VQSCALE=PIMASS/0.135D0
      
      ECMHAT = HVACTION ( IDUMMY )
                                ! ECMHAT is Z'* mass
                                ! WARNING: if FSR is not off 
                                ! this may not be e+e- invt mass!!!
      
C - Set up "brute-force" low-mass q-qbar system and hadronize it.
      ECM = ECMHAT/VQSCALE      ! v-quark pair invt mass is Z'* mass
                                ! scale the energy down by VQSCALE

C*    modify the decay:
      DO I = 1, 500
        
        MDCYBACK(I)=MDCY(I,1)   ! back-up the initial values 
        
        IF      ( 0       .EQ. MDCY(I,1) 
     &    .AND. 0         .LT. PMAS(I,2)
     &    .AND. 0.5*ECM   .GE. PMAS(I,1) 
     &    .AND. 0         .NE. MDCY(I,2) 
     &    .AND. 0         .NE. MDCY(I,3) ) THEN 
          MDCY(I,1)=1
        ELSE IF ( 1       .EQ. MDCY(I,1) 
     &      .AND. 0.001   .LT. PMAS(I,4) ) THEN 
          MDCY(I,1)=0
        ENDIF 
      ENDDO 
      
      MDCY(PYCOMP(111),1)=0 
      MDCY(PYCOMP(211),1)=0 

      IREPEAT = 0 

 10   CONTINUE 
      
      CALL PY2ENT(-1,1,-1,ECM)  ! set up a pair of u-ubar quarks

      
      MINT(33) = 0 
      CALL PYSHOW(1,2,ECM)      ! shower them -- note QED radiation has not
                                ! yet been turned off, but it should be
      
      MSTP(112) = 0 
      CALL PYEXEC               ! hadronize them
      
C*    restore the original decay flags 
      DO I = 1, 500
        MDCY(I,1)=MDCYBACK(I)
      ENDDO 
      
C...Process the event to extract vpions, combine them with STAGE 1
C... initial stage and fake neutrinos into a consistent parton-level
C... event, stored in output file hH.lhf.dat in LHA-like format

      IF ( SUCCESS .NE. HVMAKE( IDUMMY )  ) THEN
        WRITE(*,*)'HVSTEP2: error from HVMAKE'
        GO TO 999
      ENDIF 

C ======================================================================
      hvstep2 = SUCCESS 
      RETURN
 999  CONTINUE 
      hvstep2 = FAILURE 
      RETURN 
      END
C ======================================================================
