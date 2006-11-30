C $Id: hvmake.F,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C ======================================================================
C CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
C ======================================================================
C $Log: not supported by cvs2svn $
C Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
C New package: "Hidden Valley" production 
C
C ======================================================================
      INTEGER FUNCTION hvmake ( IDUMMY ) 
C ======================================================================
C  Purpose   : prepare "Hidden Valley" production
C  Arguments : none 
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
C  Created   : 2006-10-02
C ======================================================================
      IMPLICIT NONE 
C ======================================================================
C Include files 
C ======================================================================
C Pythia
C ======================================================================
#include "Pythia.inc"
C ======================================================================
C Local 
C ======================================================================
#include "HidValley.inc"
C ======================================================================
C Local variables 
C ======================================================================
      INTEGER  IDUMMY 
C ======================================================================
      DOUBLE   PRECISION PXTOT,PYTOT,PZTOT,EETOT,VQSCALE,DELTA
      DOUBLE   PRECISION EE,DX,DY,PX,PY,PZ,PT,ETA,PHI,PHILHCO,CC
      INTEGER  I,IN,LUN,JFLG
C ======================================================================

      hvmake = SUCCESS 
      
C -  set up to keep track of all the energy/momentum in the system
C -  these are the four-momenta of the initial state q-qbar 
      
      PXTOT=QPX(1)+QPX(2)
      PYTOT=QPY(1)+QPY(2)
      PZTOT=QPZ(1)+QPZ(2)
      EETOT=QEE(1)+QEE(2)
      
C - Presently the event is oriented with the q-qbar pair at rest 
C - and aligned along the z-axis
C - Need to rotate and boost so that it agrees with
C - the final state "electron/positron" pair
C - computed at the STAGE 1 
      
      CALL PYROBO(0,0,QQTHT,QQPHI,QQVX,QQVY,QQVZ)
   
C - Now event should be in correct frame
      
      NCNTR=1                   ! Object counter
      I=0                       ! Pythia line counter

C - cycle over pythia record
 400  I=I+1
      
ccccccccccccccccccccccccccccccccccccccccccccccccc
c   Here we are picking up the pions
ccccccccccccccccccccccccccccccccccccccccccccccccc
      
      VQSCALE = PIMASS/0.135D0 
      
c if a final state particle is a pizero (111) or a piplus (211)
c and was produced after the hard scatter 
c then call it a h0 (JFLG=1) or H0 (JFLG=2) for lhf output
      
C - reset flag that says "pizero=1, piplus/minus=2, other=0"
      JFLG=0
      IF ( K(I,1).EQ.1.AND.ABS(K(I,2)).EQ.211.AND.K(I,3).GT.0) THEN
        JFLG=2
      ELSEIF (K(I,1).EQ.1.AND.K(I,2).EQ.111.AND.K(I,3).GT.0) THEN
        JFLG=1
      ENDIF
      
      IF (JFLG.GT.0) THEN       ! If it is not a pion we will throw it away
        
        EE=PYP(I,4)*VQSCALE
        PX=PYP(I,1)*VQSCALE
        PY=PYP(I,2)*VQSCALE
        PZ=PYP(I,3)*VQSCALE
        PT=PYP(I,10)*VQSCALE
        ETA=PYP(I,19)
        PHI=PYP(I,19)           ! could be between -pi and pi
        PHILHCO=PHI
        IF (PHI.LT.0) PHILHCO=PHI+6.2832D0 ! shift to between 0 and 2pi
        
c keep track of total 4-momentum minus that of each hadron
        PXTOT=PXTOT-PX
        PYTOT=PYTOT-PY
        PZTOT=PZTOT-PZ
        EETOT=EETOT-EE
        
c store the momentum of this particle [shift its label by two
c to allow for the initial q qbar pair which sit in lines 1,2]
        NEE(NCNTR+2)=EE
        NPX(NCNTR+2)=PX
        NPY(NCNTR+2)=PY
        NPZ(NCNTR+2)=PZ

        IF ( 1 .EQ. JFLG ) THEN 
          NTYP(NCNTR+2)=IDPIZ   ! 25 for pizero, 35 for piplus
        ELSE 
          NTYP(NCNTR+2)=IDPIP   ! 25 for pizero, 35 for piplus
        ENDIF 
        
        NCNTR=NCNTR+1           ! update pion counter
        
      ENDIF  
      
      IF (I.LT.N) GOTO 400      ! if not at end of event record, go back

      DX=PYTOT
      DY=-PXTOT
      CC=EETOT*EETOT- PXTOT*PXTOT- PYTOT*PYTOT - PZTOT*PZTOT
      IF (CC.LT.0.D0) THEN   
        CC=0.D0                 ! usually this is just roundoff error
      ENDIF
      DELTA=DSQRT(CC/4/(PXTOT*PXTOT + PYTOT*PYTOT+.000000001D0) )
      
      
      PX=PXTOT/2.D0+DELTA*DX
      PY=PYTOT/2.D0+DELTA*DY
      PZ=PZTOT/2.D0
      EE=EETOT/2.D0
      
      NEE(NCNTR+2)=EE
      NPX(NCNTR+2)=PX
      NPY(NCNTR+2)=PY
      NPZ(NCNTR+2)=PZ
      NTYP(NCNTR+2)=12          ! It's a neutrino
      
      NCNTR=NCNTR+1
      
      PX=PXTOT/2.D0-DELTA*DX
      PY=PYTOT/2.D0-DELTA*DY
      PZ=PZTOT/2.D0
      EE=EETOT/2.D0
      
      NEE(NCNTR+2)=EE
      NPX(NCNTR+2)=PX
      NPY(NCNTR+2)=PY
      NPZ(NCNTR+2)=PZ
      NTYP(NCNTR+2)=-12         ! It's an antineutrino
      
C- Write LHE file 

      LUN = MSTP(163) 
      IF ( 0 .EQ. LUN ) RETURN  ! RETURN 
      
      WRITE(LUN,'(A)',ERR=999) '<event>'  
      
      WRITE(LUN,*,ERR=999) NCNTR+2,661,1.0,PIMASS, !scal0
     &  7.82E-03,1.16E-01       ! qed, qcd cplgs
      
      IF (IQTYP(1).GT.0) THEN
        WRITE(LUN,722,ERR=999) IQTYP(1),-1,0,0,501,0, ! make sure quarks
     &    QPX(1),QPY(1),QPZ(1),QEE(1),0.,0.,9. ! get color not 
        WRITE(LUN,722,ERR=999) IQTYP(2),-1,0,0,0,501, ! anticolor
     &    QPX(2),QPY(2),QPZ(2),QEE(2),0.,0.,9.
      ELSE
        WRITE(LUN,722,ERR=999) IQTYP(1),-1,0,0,0,501,            
     &    QPX(1),QPY(1),QPZ(1),QEE(1),0.,0.,9.
        WRITE(LUN,722,ERR=999) IQTYP(2),-1,0,0,501,0,
     &    QPX(2),QPY(2),QPZ(2),QEE(2),0.,0.,9.
      ENDIF
      DO IN=3,NCNTR
        WRITE(LUN,722,ERR=999) NTYP(IN),1,1,2,0,0, ! code,status,parents1 and 2,color,anticlr
     &    NPX(IN),NPY(IN),NPZ(IN),NEE(IN),
     &    PMAS(NTYP(IN),1),  
     &    0.,9.                 !???
      ENDDO
      
      DO IN=NCNTR+1,NCNTR+2
        WRITE(LUN,722,ERR=999) NTYP(IN),1,1,2,0,0, ! code,st,parents1,2,colors
     &    NPX(IN),NPY(IN),NPZ(IN),NEE(IN),0., ! neutrinos
     &    0.,9.                 !???
      ENDDO
      
      WRITE(LUN,'(A)') '</event>' 
      
 722  FORMAT(I7,3I4,2I4,5G18.10,2F3.0)
      
      hvmake = SUCCESS 
      RETURN 
      
 999  CONTINUE
      WRITE(*,*)'HVMAKE: error in writing to the file LUN=',LUN
      hvmake = FAILURE 
      RETURN 
      END
      

C ======================================================================
C THE END 
C ======================================================================
