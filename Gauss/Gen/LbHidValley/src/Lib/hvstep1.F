C $Id: hvstep1.F,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
C ======================================================================
C $Log: not supported by cvs2svn $ 
C ======================================================================
      INTEGER FUNCTION hvstep1 ( idummy ) 
C ======================================================================
C  Purpose   : perform the first step pof Hidden Valley production
C  Arguments : 
C
C  An Hidden valley production
C  The actual lines are extracted from HV0.403 package 
C  written by Matt Strassler <strasslr@noether.phys.washington.edu>"
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
      DOUBLE   PRECISION ECM 
      INTEGER  LUN,IDUMMY,I,ITMP1,ITMP2
      INTEGER  HVDECAY
      EXTERNAL HVDECAY 
      LOGICAL  FIRST 
      DATA     FIRST  /.TRUE./
      SAVE     FIRST 
C ======================================================================

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC STAGE ONE : MAKE PARTON-LEVEL q qbar --> Z'(*) --> e+ e- EVENTS
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

C setting up here to do q qbar -> Z' -> e+ e-
C store initial and final state short-distance partons
C with electrons currently as stand-ins for v-quarks
C so that we can recall the initial state later
C and so that we can hadronize the final state

      
C...Initialize for Z' production, 
C...Select Z' decays to e+e- only as standing for v-quarks
C... NOTE!!!!!!!!!  In future Z' couplings need to be set more carefully
C...                For now we are using the default Z'
C...
      MSEL=0

      DO I = 1,500
        MSUB(I) = 0             ! switch OFF *ALL* processes 
      ENDDO 
      MSUB(141)=1               ! drell yan to Z' with interference
      PMAS(32,1) = ZPMASS       ! set zprime mass
      CKIN(1) = ZPMASS/1.5D0    ! set minimum energy of d-y pair not too low
      
      
C*    turn off all gamma* decays, except for e+e- 
      IF ( 1 .NE. HVDECAY ( 22 , 11 , -11 , 0 , 0 , 0 ) ) THEN
        WRITE(*,*)'HVSTEP1: invalid decays for g* '
        GOTO 999
      ENDIF 
C*    turn off all Z* decays, except for e+e- 
      IF ( 1 .NE. HVDECAY ( 23 , 11 , -11 , 0 , 0 , 0 ) ) THEN
        WRITE(*,*)'HSTEP1: invalid decays for Z*   '
        GOTO 999
      ENDIF 
C*    turn off all Z'* decays, except for e+e- 
      IF ( 1 .NE. HVDECAY ( 32 , 11 , -11 , 0 , 0 , 0 ) ) THEN
        WRITE(*,*)'HSTEP1: invalid decays for Z''* '
        GOTO 999
      ENDIF 
      
C - arrange for proper v-pion masses, decays and lifetimes
C - (currently using h0 25 and H0 35 as v-pi-zero and v-pi-plus/minus)
C - will need to change this using QNUMBERS if possible
      
      PMAS(IDPIZ,1)=PIMASS
      PMAS(IDPIZ,4)=PIZLIF      
      PMAS(IDPIP,1)=PIMASS*0.1396D0/0.135D0
      
      IF ( PIPLIF .GE. 1000 ) THEN
        IF ( FIRST ) THEN 
          WRITE(*,*)'HVSTEP1: PIPLIF > 1 meter',PIPLIF
     &      ,' make it stable! '
        ENDIF   
        PIPLIF=-1
      ENDIF
        
      IF ( PIPLIF .GT. 0.D0 ) THEN
        PMAS(IDPIP,4)=PIPLIF
      ELSE
        MDCY(IDPIP,1)=0         ! if piplif < 0 don't let H0 decay
      ENDIF
      
C... Turn off showering/ISR/FSR/UE 
C... NOT IMPLEMENTED RIGHT NOW!!!!!!!!!!!!!!!!!!!!
C*****************
CCC WARNING: For reasons unknown, this approach seems to make the
CCC UE in stage 3, and maybe other aspects of the event simulation,
CCC inappropriate.  This may be due to a Pythia bug; consultations are
CCC ongoing 9/18/06...
C*****************

c      MSTJ(41)=0   ! NO SHOWER
c      MSTP(61)=0   ! NO ISR; BUT THIS MAY BE INCORRECT APPROACH!!!
c      MSTP(71)=0   ! NO FSR
      MSTP(61)=0                ! NO ISR  !!!!!See caution above
      MSTP(71)=0                ! NO FSR  !!!!!See caution above
c      MSTP(81)=0   ! NO UE
 
       ECM = 14000.D0 
      ITMP1 = MSTU(12)
      ITMP2 = MSTP(122) 
      MSTU(12)  = 12345
      MSTP(122) = 0
      CALL PYINIT('CMS','p','p',ECM)
      MSTU(12)  = ITMP1
      MSTP(122) = ITMP2

 
C...Final Les Houches Event File, obtained by combining above two.
      IF ( FIRST ) THEN 
        FIRST = .FALSE.  
        WRITE(*,*)" Hidden Valley Production : parameters to be used: "
        WRITE(*,*)" ZPMASS: " , ZPMASS
        WRITE(*,*)" PIMASS: " , PIMASS
        WRITE(*,*)" PIZLIF: " , PIZLIF
        WRITE(*,*)" PIPLIF: " , PIPLIF
        LUN = MSTP(163) 
        IF ( 0 .LT. LUN ) THEN 
          WRITE(LUN,'(A)',ERR=900) '<LesHouchesEvents version="1.0">'
          WRITE(LUN,'(A)',ERR=900) '<!--'
          WRITE(LUN,'(A,I1,A1,I3)') 'File generated with PYTHIA ',
     &      MSTP(181),'.',MSTP(182)
          WRITE(LUN,'(A)',ERR=900) '-->'       
          
          WRITE(LUN,'(A)',ERR=900) '<init>'       
          WRITE(LUN,'(A)',ERR=900)
     &      '2212 2212 7.0E+03 7.0E+03 4 4 46 46 1 1'
          WRITE(LUN,'(A)',ERR=900) '1.0 1.0 1.0 661'
          WRITE(LUN,'(A)',ERR=900) '</init>'       
        ENDIF 
      ENDIF 
      
C ======================================================================
      
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
CCCCCC STAGE ONE : MAKE PARTON-LEVEL q qbar --> Z'(*) --> e+ e- EVENTS
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      
      CALL PYEVNT 
      
      hvstep1 = SUCCESS 
      RETURN 
C ======================================================================
 900  CONTINUE 
      WRITE(*,*)'HVSTEP1: error in write to LUN:',LUN
 999  CONTINUE
      hvstep1 = FAILURE
      RETURN
      END
C ======================================================================


C ======================================================================
C The END 
C ======================================================================
