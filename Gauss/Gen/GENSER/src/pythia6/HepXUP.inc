C $Id: HepXUP.inc,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C $Revision: 1.2 $
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
