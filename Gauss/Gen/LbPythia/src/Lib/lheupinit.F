C $Id: lheupinit.F,v 1.1 2006-10-04 14:16:51 ibelyaev Exp $
C ----------------------------------------------------------------------
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
C ----------------------------------------------------------------------
C $Log: not supported by cvs2svn $ 
C ----------------------------------------------------------------------
      SUBROUTINE lheupinit
C-----------------------------------------------------------------------
C  Purpose   : UPINIT routine for reading "new" LHE (xml-like) format
C  Arguments : 
C  
C  The code is copied from http://www.thep.lu.se/~thorbjorn/lhef 
C  It will be the part of Pythia 6.4
C
C  @author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  @date      : 2006-10-03
C-----------------------------------------------------------------------
C...UPINIT
C...Reads initialization information from a Les Houches Events File. 
C      SUBROUTINE UPINIT
      
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      
C...PYTHIA commonblock: only used to provide read unit MSTP(161).
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/

C...User process initialization commonblock.
#include "HepXUP.inc"      
      
C...Lines to read in assumed never longer than 200 characters. 
C: V.B.      PARAMETER (MAXLEN=200)
C...Lines to read in assumed never longer than 256 characters. 
      PARAMETER (MAXLEN=256)
      CHARACTER*(MAXLEN) STRING
      
      LOGICAL FIRST
      DATA    FIRST/.TRUE./
      SAVE    FIRST
      
C...Format for reading lines.
      CHARACTER*6 STRFMT
      STRFMT='(A000)'
      WRITE(STRFMT(3:5),'(I3)') MAXLEN
      
      IF ( FIRST ) THEN
        WRITE(MSTU(11),*)' LHEUPINIT is  invoked '
      ELSE 
        CALL LHERESTORE
        RETURN
      ENDIF 
      
C...Loop until finds line beginning with "<init>" or "<init ". 
 100  READ(MSTP(161),STRFMT,END=130,ERR=130) STRING
      IBEG=0
 110  IBEG=IBEG+1
C...Allow indentation.
      IF(STRING(IBEG:IBEG).EQ.' '.AND.IBEG.LT.MAXLEN-5) GOTO 110 
      IF(STRING(IBEG:IBEG+5).NE.'<init>'.AND.
     &  STRING(IBEG:IBEG+5).NE.'<init ') GOTO 100
      
C...Read first line of initialization info.
      READ(MSTP(161),*,END=130,ERR=130) IDBMUP(1),IDBMUP(2),EBMUP(1),
     &  EBMUP(2),PDFGUP(1),PDFGUP(2),PDFSUP(1),PDFSUP(2),IDWTUP,NPRUP
      
C...Read NPRUP subsequent lines with information on each process.
      DO 120 IPR=1,NPRUP
        READ(MSTP(161),*,END=130,ERR=130) XSECUP(IPR),XERRUP(IPR),
     &    XMAXUP(IPR),LPRUP(IPR)
 120  CONTINUE
      
      CALL LHESAVE 
      FIRST = .FALSE.
      
      RETURN
      
C...Error exit: give up if initalization does not work.
 130  WRITE(*,*) ' Failed to read LHEF initialization information.'
      WRITE(*,*) ' Event generation will be stopped.'
      
      STOP  
      
 999  RETURN
      END
      
C-----------------------------------------------------------------------
C The END 
C-----------------------------------------------------------------------
