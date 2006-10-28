C $Id: hvgetpar.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $ , version  $Revision: 1.1.1.1 $
C ======================================================================
C $Log: not supported by cvs2svn $
C ======================================================================
      DOUBLE PRECISION FUNCTION hvgetpar ( TITLE ) 
C ======================================================================
C  Purpose   :  get parameters from Hidden Valley production
C  Arguments : 
C
C  An Hidden valley production
C  The actual lines are extracted from HV0.403 package 
C  written by Matt Strassler <strasslr@noether.phys.washington.edu>"
C
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2006-10-22
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
#include "HidValley.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      CHARACTER*(*)    TITLE 
      INTEGER          LENOCC 
      EXTERNAL         LENOCC 
C ======================================================================
      
      HVGETPAR = -100000 
      
      IF     ( 'ZPMASS' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        HVGETPAR = ZPMASS 
      ELSEIF ( 'PIMASS' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        HVGETPAR = PIMASS 
      ELSEIF ( 'PIZLIF' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        HVGETPAR = PIZLIF 
      ELSEIF ( 'PIPLIF' .EQ. TITLE(1:LENOCC(TITLE)) ) THEN 
        HVGETPAR = PIPLIF 
      ELSE 
        WRITE(*,*) 'INVALID NAME:"' , TITLE(1:LENOCC(TITLE)), '"'
      ENDIF 
      
  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
