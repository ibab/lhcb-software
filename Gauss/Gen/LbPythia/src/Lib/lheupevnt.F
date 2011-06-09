C $Id: lheupevnt.F,v 1.1 2006-10-04 14:16:51 ibelyaev Exp $
C ----------------------------------------------------------------------
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
C ----------------------------------------------------------------------
C $Log: not supported by cvs2svn $ 
C ----------------------------------------------------------------------
      SUBROUTINE lheupevnt
C-----------------------------------------------------------------------
C  Purpose   : UPINIT routine for reading "new" LHE (xml-like) format
C  Arguments : 
C
C  The code is copied from http://www.thep.lu.se/~thorbjorn/lhef 
C  It will be the part of Pythia 6.4
C
C  @author    : Vanya  BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
C...UPEVNT
C...Reads a parton-level event from a Les Houches Events File. 
 
C      SUBROUTINE UPEVNT
 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
 
C...PYTHIA commonblock: only used to provide read unit MSTP(162).
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/
 
C...User process event common block.
#include "HepXUP.inc"

C...Lines to read in assumed never longer than 200 characters. 
      PARAMETER (MAXLEN=200)
      CHARACTER*(MAXLEN) STRING
      
C...Format for reading lines.
      CHARACTER*6 STRFMT
      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN


      MSTI(51) = 0 
      
C...Loop until finds line beginning with "<event>" or "<event ". 
 100  READ(MSTP(162),STRFMT,END=130,ERR=130) STRING
      IBEG=0
 110  IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-6) GOTO 110 
      IF(STRING(IBEG:IBEG+6).NE.'<event>'.AND.
     &  STRING(IBEG:IBEG+6).NE.'<event ') GOTO 100
      
C...Read first line of event info.
      READ(MSTP(162),*,END=130,ERR=130) NUP,IDPRUP,XWGTUP,SCALUP,
     &  AQEDUP,AQCDUP
      
C...Read NUP subsequent lines with information on each particle.
      DO 120 I=1,NUP
        READ(MSTP(162),*,END=130,ERR=130) IDUP(I),ISTUP(I),
     &    MOTHUP(1,I),MOTHUP(2,I),ICOLUP(1,I),ICOLUP(2,I),
     &    (PUP(J,I),J=1,5),VTIMUP(I),SPINUP(I)
 120  CONTINUE
      
      RETURN
      
C...Error exit, typically when no more events.
 130  WRITE(*,*) ' Failed to read LHEF event information.'
      WRITE(*,*) ' Will assume end of file has been reached.'
      NUP=0
      MSTI(51)=1
      
      RETURN
      END
      
C-----------------------------------------------------------------------
C The END 
C-----------------------------------------------------------------------
      
