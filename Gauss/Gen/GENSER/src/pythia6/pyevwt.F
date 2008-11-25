C $Id: pyevwt.F,v 1.4 2008-11-25 14:30:51 robbep Exp $  
C*********************************************************************
 
C...PYEVWT
C...Dummy routine, which the user can replace in order to multiply the
C...standard PYTHIA differential cross-section by a process- and
C...kinematics-dependent factor WTXS. For MSTP(142)=1 this corresponds
C...to generation of weighted events, with weight 1/WTXS, while for
C...MSTP(142)=2 it corresponds to a modification of the underlying
C...physics.
 
      SUBROUTINE PYEVWT(WTXS)
 
C...Double precision and integer declarations.
      IMPLICIT NONE
cc      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
cc      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
      DOUBLE PRECISION PARP, PARI, VINT
      INTEGER MSTP, MSTI, MINT
      DOUBLE PRECISION WTXS, PT2, PT20, Q2
      DOUBLE PRECISION PYALPS
      INTEGER ISUB, ISTSB
C...Commonblocks.
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      SAVE /PYPARS/,/PYINT1/
      
C...Set default weight for WTXS.
      WTXS=1D0
 
C...Read out subprocess number.
      ISUB=MINT(1)
      IF ((ISUB.LT.421).OR.((ISUB.GT.439).AND.(ISUB.LT.461)).OR.
     +    (ISUB.GT.484)) THEN
        RETURN
      ENDIF
 
      PT2=VINT(48)
 
C...Modifications by user to be put here.
C GCorti - Introduce code from Marianne Bargiotti for re-weighting 
C...Copy form for pT0 as used in multiple interactions.
C      PT0=PARP(82)*(VINT(1)/PARP(89))**PARP(90)
C      PT20=PT0**2
      PT20 = (2.85D0)**2

C...Introduce dampening factor.
      WTXS=(PT2/(PT20+PT2))**2

C...Also dampen alpha_strong by using larger Q2 scale.
      Q2=VINT(52)
      WTXS=WTXS*((PYALPS(PT20+Q2)**2)/(PYALPS(Q2)**2))

      RETURN
      END
