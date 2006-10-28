C $Id: hvdecay.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
C ======================================================================
C $Log: not supported by cvs2svn $ 
C ======================================================================
      INTEGER FUNCTION hvdecay ( KF , ID1 , ID2 , ID3 , ID4 , ID5 ) 
C ======================================================================
C  Purpose   : turn off all decays of the particle KF 
C  Arguments : 
C
C  An Hidden valley production
C  The actual lines are extracted from HV0.403 package 
C  written by Matt Strassler <strasslr@noether.phys.washington.edu>"
C
C  Author    : Vanya BELYAEV ibelyaev@physics.syr.edu
C  Created   : 2006-10-24
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
C ======================================================================
#include "Pythia.inc"
C ======================================================================
C Arguments 
C ======================================================================
      INTEGER KF,ID1,ID2,ID3,ID4,ID5
C ======================================================================
C- Local Variables
C ======================================================================
      INTEGER  I,IC,KC,I1,I2,I3,I4,I5
      INTEGER  FIRST,LAST 
C ======================================================================
      hvdecay = 0 
      
      KC=PYCOMP(KF) 
      IF ( 0 .EQ. KC ) THEN 
        WRITE(*,*)'HVDECAY: invalid KF code:',KF
        GOTO 999
      ENDIF 
      
      FIRST = MDCY(KC,2) 
      IF ( 0 .GE. FIRST ) THEN
        WRITE(*,*)'HVDECAY: invalid FIRST line for KF:',KF
        GOTO 999
      ENDIF
      
      LAST = MDCY(KC,2)+MDCY(KC,3)-1
      IF ( 0 .GE. LAST) THEN
        WRITE(*,*)'HVDECAY: invalid LAST line for KF:',KF
        GOTO 999
      ENDIF
      
C === LOOP OVER ALL DECAY MODES 
      DO I=FIRST,LAST
        IF ( 1 .EQ. MDME(I,1) )  MDME(I,1)=0
        I1 = IABS(KFDP(I,1))
        I2 = IABS(KFDP(I,2))
        I3 = IABS(KFDP(I,3))
        I4 = IABS(KFDP(I,4))
        I5 = IABS(KFDP(I,5))
        IF (    ( I1 .EQ. IABS(ID1) ) .AND. ( I2 .EQ. IABS(ID2) ) ) THEN
          IF (  ( I3 .EQ. IABS(ID3) ) .AND. ( I4 .EQ. IABS(ID4) ) ) THEN
            IF (                            ( I5 .EQ. IABS(ID5) ) ) THEN
              MDME(I,1)=1
            ENDIF
          ENDIF
        ENDIF
        IF ( 1 .EQ. MDME(I,1) ) HVDECAY = HVDECAY + 1 
      ENDDO 
      
C ======================================================================
      RETURN 
C ======================================================================
 999  CONTINUE 
      hvdecay = -1 
      RETURN      
      END
C ======================================================================

C ======================================================================
C The END 
C ======================================================================
