 
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
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYINT1/MINT(400),VINT(400)
      COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
      SAVE /PYDAT1/,/PYPARS/,/PYINT1/,/PYINT2/
 
C...Set default weight for WTXS.
      WTXS=1D0
 
C...Read out subprocess number.
      ISUB=MINT(1)
      ISTSB=ISET(ISUB)
 
C...Read out tau, y*, cos(theta), tau' (where defined, else =0).
      TAU=VINT(21)
      YST=VINT(22)
      CTH=0D0
      IF(ISTSB.EQ.2.OR.ISTSB.EQ.4) CTH=VINT(23)
      TAUP=0D0
      IF(ISTSB.GE.3.AND.ISTSB.LE.5) TAUP=VINT(26)
 
C...Read out x_1, x_2, x_F, shat, that, uhat, p_T^2.
      X1=VINT(41)
      X2=VINT(42)
      XF=X1-X2
      SHAT=VINT(44)
      THAT=VINT(45)
      UHAT=VINT(46)
      PT2=VINT(48)
 
C...Modifications by user to be put here.
CGCorti - Introduce code from Marianne Bargiotti for re-weighting 
C...Copy form for pT0 as used in multiple interactions.
      PT0=PARP(82)*(VINT(1)/PARP(89))**PARP(90)
      PT20=PT0**2

C...Introduce dampening factor.
      WTXS=(PT2/(PT20+PT2))**2

C...Also dampen alpha_strong by using larger Q2 scale.
      Q2=VINT(52)
C      WTXS=WTXS*(PYALPS(PT20+Q2)/PYALPS(Q2))**3
      WTXS=WTXS*(PYALPS(PARP(82)**2+VINT(52))**2/PYALPS(VINT(52))**2)

 
C...Stop program if this routine is ever called.
CGCorti - Comment out the following
C...You should not copy these lines to your own routine.
C      WRITE(MSTU(11),5000)
C      CALL PYSTOP(4)
 
C...Format for error printout.
C 5000 FORMAT(1X,'Error: you did not link your PYEVWT routine ',
C     &'correctly.'/1X,'Dummy routine in PYTHIA file called instead.'/
C     &1X,'Execution stopped!')
 
      RETURN
      END
