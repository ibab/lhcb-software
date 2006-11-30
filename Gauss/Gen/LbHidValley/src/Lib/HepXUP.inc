C $Id: HepXUP.inc,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C
C ======================================================================
C  Purpose : 
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
C
C  Author  : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created : 2006-10-22
C ======================================================================
C...User process event common block.
C ======================================================================
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &  ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &  VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/
C ======================================================================
C...User process initialization commonblock.
C ======================================================================
      INTEGER MAXPUP, IISET
      PARAMETER (MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON/HEPRUP/IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &  IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),XMAXUP(MAXPUP),
     &  LPRUP(MAXPUP)
      SAVE /HEPRUP/
C ======================================================================
      

C ======================================================================
C The END 
C ======================================================================
