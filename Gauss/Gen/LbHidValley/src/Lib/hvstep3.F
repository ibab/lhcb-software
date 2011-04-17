C $Id: hvstep3.F,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C 
C ======================================================================
      INTEGER FUNCTION hvstep3 ( IDUMMY ) 
C ======================================================================
C  Purpose   : perform the third step of Hidden Valley production
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
#include "HepXUP.inc"
#include "Pythia.inc"
C =====================================================================
C Local 
C =====================================================================
#include "HidValley.inc"
C =====================================================================
C- Local Variables
C ======================================================================
      INTEGER IDUMMY,ITMP1,ITMP2
C ======================================================================
      INTEGER  HVDECAY
      EXTERNAL HVDECAY
C ======================================================================
      
C - prepare for event through upinit and upevnt
C - turn off other processes so that user process 661 is the only one
      MSUB(1)=0                 !drell yan OFF
      MSUB(141)=0               !drell yan to Z' with interference OFF
      
C - arrange for proper v-pion masses, decays and lifetimes
C - (currently using h0 25 and H0 35 as v-pi-zero and v-pi-plus/minus)
C - will need to change this using QNUMBERS if possible
      
C - turn off all but fermion decays for h0
      IF ( 1 .NE. HVDECAY ( IDPIZ , 5 , -5 , 0 , 0 , 0 ) ) THEN 
        WRITE(*,*)'HVPHASE: invalid decays for:',IDPIZ 
        GO TO 999
      ENDIF
      
C - turn off decay (if PIPLIF<0) 
C - or turn off all but fermion decays for H0
      IF ( 1 .NE. HVDECAY ( IDPIP , 5 , -5 , 0 , 0 , 0 ) ) THEN 
        WRITE(*,*)'HVPHASE: invalid decays for:',IDPIP 
        GO TO 999
      ENDIF
      
      IDBMUP (1) = 11 
      IDBMUP (2) = 11
      
C...Set PDF's of incoming beams: CTEQ 5L.
C...Note that Pythia will not look at PDFG and PDFS.  
C... !!!!!!!!!!!!!!!!!!!! This Needs Checking!!!
      PDFGUP(1) = 4
      PDFSUP(1) = 46
      PDFGUP(2) = PDFGUP(1)
      PDFSUP(2) = PDFSUP(1)
      
C...Decide on weighting strategy: unweighted on input.
      IDWTUP=1
C...Number of external processes. 
      NPRUP=1
C...Set up 
      XSECUP(1)=0.6D0
      XMAXUP(1)=1.0D0
      LPRUP(1)=661
C...All these settings will be used by upinit
      
      ITMP1 = MSTU(12)
      ITMP2 = MSTP(122) 
      MSTU(12)  = 12345
      MSTP(122) = 0
      CALL PYINIT ( 'USER' , ' ' , ' ' , 0d0 ) !  see upinit,upevnt in pythia
      MSTU(12)  = ITMP1 
      MSTP(122) = ITMP2 
      
      CALL PYEVNT

C ======================================================================
      hvstep3 = SUCCESS 
      RETURN
 999  CONTINUE 
      hvstep3 = FAILURE 
      RETURN 
      END
C ======================================================================
      
C ======================================================================
C The END 
C ======================================================================
