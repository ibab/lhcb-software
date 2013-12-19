      
      SUBROUTINE ONIAPAIRS_UPEVNT 
      
C...  No more this implicit trash
      IMPLICIT NONE
      
C...  Include Files
      include "pythia.inc"
      include "oniapairs.inc"

C...  Local arrays and parameters.
      LOGICAL FIRST
      DATA FIRST /.TRUE./
      SAVE FIRST 
C...  Local arrays and parameters.
      LOGICAL IREJ
      INTEGER I,J
      DOUBLE PRECISION AEM,ALPS,Q2
      DOUBLE PRECISION BETA,SINTHE,COSTHE,PABS,GAMM
      DOUBLE PRECISION pe3,pe4,pz3,pz4,phi
      DOUBLE PRECISION XPP1(-25:25),XPP2(-25:25)
      DOUBLE PRECISION XMIN,X,Y,X1,X2,SHAT,THAT,UHAT
      DOUBLE PRECISION UHTMIN,UHTMAX
      DOUBLE PRECISION MPSI,m1,m2,PSI0,Kk
      DOUBLE PRECISION PHSPV, DSIGDT
      DOUBLE PRECISION PI,CONV, Pp, Qq, ptmin
      DATA CONV/0.3894D9/
      
C-----------------------------------------------------------------
      
      PI=PARU(1)
      
      IF ( FIRST ) THEN 
         WRITE(*,'(A,A)') '                       ',
     &        'ONIAPAIRS_UPEVNT: First call'
         FIRST = .FALSE.
      ENDIF         
      
C-----------------------------------------------------------------
      
C...  Default return value.
      XWGTUP=0D0
      
C...  Default flag for event rejection.
      IREJ=.FALSE.
      
      if (IDPRUP.GE.1.and.IDPRUP.LE.3) then 
C     Psi system
         Pfct=1.4D0
         Efct=0.3D0
      elseif (IDPRUP.GE.11) then
C     Upsilon system
         Pfct=1.8D0
         Efct=0.18
      endif

C-----------------------------------------------------------------

C...  First step
C...  Set up the partonic process that is selected.
      
C...  Define number of partons - two incoming and two outgoing.
      NUP=4
      
C...  Flavour codes for entries. 
      IDUP(1)=21
      IDUP(2)=21
      if (IDPRUP.EQ.1) then 
c     psi-psi
         IDUP(3)=443
         IDUP(4)=443
         PSI0 = PSI01S
      elseif (IDPRUP.EQ.2) then
c     psi2s-psi2s
         IDUP(3)=100443
         IDUP(4)=100443
         PSI0 = PSI02S
      elseif (IDPRUP.EQ.3) then
c     psi-psi2s
         IDUP(3)=443
         IDUP(4)=100443
         PSI0 = DSQRT(PSI01S*PSI02S)
      elseif (IDPRUP.EQ.11) then
c     ups-ups
         IDUP(3)=553
         IDUP(4)=553
         PSI0 = UPS01S  
      elseif (IDPRUP.EQ.12) then
c     ups2s-ups2s
         IDUP(3)=100553
         IDUP(4)=100553
         PSI0 = UPS02S  
      elseif (IDPRUP.EQ.13) then
c     ups3s-ups3s
         IDUP(3)=200553
         IDUP(4)=200553
         PSI0 = UPS03S  
      elseif (IDPRUP.EQ.14) then
c     ups-ups2s
         IDUP(3)=553
         IDUP(4)=100553
         PSI0 = DSQRT(UPS01S*UPS02S)
      elseif (IDPRUP.EQ.15) then
c     ups-ups3s
         IDUP(3)=553
         IDUP(4)=200553
         PSI0 = DSQRT(UPS01S*UPS03S)
      elseif (IDPRUP.EQ.16) then
c     ups2s-ups3s
         IDUP(3)=100553
         IDUP(4)=200553
         PSI0 = DSQRT(UPS02S*UPS03S)
      endif
      
      IF (IDUP(3).eq.IDUP(4)) THEN
         Kk=Kfct(IDPRUP)
      ELSE
         Kk=2D0*Kfct(IDPRUP)
      ENDIF
      
C...  Status codes.
      ISTUP(1)=-1
      ISTUP(2)=-1
      ISTUP(3)=1
      ISTUP(4)=1
      
C...  Mother codes.
      MOTHUP(1,1)=0
      MOTHUP(2,1)=0
      MOTHUP(1,2)=0
      MOTHUP(2,2)=0
      MOTHUP(1,3)=1
      MOTHUP(2,3)=2
      MOTHUP(1,4)=1
      MOTHUP(2,4)=2
      
C...  Colour flow.
      ICOLUP(1,1)=501
      ICOLUP(2,1)=502
      ICOLUP(1,2)=502
      ICOLUP(2,2)=501
      ICOLUP(1,3)=0
      ICOLUP(2,3)=0
      ICOLUP(1,4)=0
      ICOLUP(2,4)=0
      
C-----------------------------------------------------------------
      
C...The second step is to decide how the phase space should be sampled.
      
      m1 = PYMASS(IDUP(3))
      m2 = PYMASS(IDUP(4))
      
      PTMIN = 0D0

      XMIN=(m1**2 + m2**2 + 2D0*PTMIN**2 
     &     + 2D0*DSQRT(m1**2 + PTMIN**2)*DSQRT(m2**2 + PTMIN**2))/ECM**2  

      IF(.NOT.INIT) THEN
C...  Normally select a phase space point at random
         X=(XMIN**(-Pfct)+PYR(0)*(1D0 - XMIN**(-Pfct)))**(-1D0/Pfct)
         Y= -0.5*DLOG(X) + PYR(0)*DLOG(X)
         
         X1=DSQRT(X)*DEXP(Y)
         X2=DSQRT(X)*DEXP(-Y)
         
         SHAT=X*ECM**2
      ELSE
C...  For special initialization call pick lowest point as 
C...  likely to represent maximum cross section.
         X=XMIN
         Y=0
         
         X1=DSQRT(XMIN)
         X2=DSQRT(XMIN)
         
         SHAT=XMIN*ECM**2
      ENDIF

C...  Absolute momentum of the outgoing particles in 
C...  the subsystem frame.
      PABS=0.5D0*DSQRT(MAX(0D0,(m1**2-m2**2)**2/shat +
     &     shat - 2D0*m1**2 - 2D0*m2**2))
      
      UHTMIN=(M1**2-M2**2)**2/(4D0*SHAT) - 0.25D0*SHAT - PABS**2
     &     - DSQRT(SHAT*(PABS**2-PTMIN**2))
      UHTMAX=(M1**2-M2**2)**2/(4D0*SHAT) - 0.25D0*SHAT - PABS**2
     &     + DSQRT(SHAT*(PABS**2-PTMIN**2))

      IF(.NOT.INIT) THEN
         UHAT=UHTMIN+(UHTMAX-UHTMIN)*PYR(0)
      ELSE
         UHAT=0.5D0*(m1**2+m2**2-SHAT)
      ENDIF

      THAT=M1**2 + M2**2 - SHAT - UHAT

C...  By decoupling the x_1, x_2 and u-hat variables we overestimate
C...  the phase space region. We therefore have to reject events 
C...  outside allowed phase space by putting the cross section = 0.
C...  However, to keep bookkeeping correct, one still needs to set
C...  variables as if there is an event, e.g. picking the same
C...  point as for the initialization call.
C...  Actually this should never happen now.
      IF((.NOT.INIT).AND.(X1.GT.1.OR.X2.GT.1.OR.THAT.LT.UHTMIN
     &     .OR.THAT.GT.UHTMAX)) THEN
         IREJ=.TRUE.
         
         X=XMIN
         Y=0
         
         X1=DSQRT(XMIN)
         X2=DSQRT(XMIN)

         SHAT=(M1 + M2)**2
         
         UHTMIN=0.25D0*((M1-M2)**2 - SHAT)
         UHTMAX=0.25D0*((M1-M2)**2 - SHAT)
         
         UHAT=UHTMIN
         THAT=UHTMAX
         
         WRITE(*,'(A,A,A)') 'LbOniaPairsProduction: ONIAPAIRS_UPEVNT:',
     &        ' Warning: unexpected rejection of event.',
     &        ' Generation can be continued normaly.'
      ENDIF
      
C...  The selection strategy for the phase space point relies on
C...  a phase space volume V inside which points are sampled. The
C...  expression sigma = Integral dV d(sigma)/dV =
C...  = < d(sigma)/dV > * Integral dV
C...  (with < ... > denoting (Monte Carlo sampled) mean value over V)
      IF(.NOT.INIT) THEN
         PHSPV=(X**Pfct)/Pfct*(XMIN**(-Pfct)-1)*
     &        (-DLOG(X))*(UHTMAX-UHTMIN)
      ELSE
         PHSPV=(X**Pfct)/Pfct*(XMIN**(-Pfct)-1)*
     &        (-DLOG(X))*(M1 + M2)**2*Efct
      ENDIF
C...  Note x1*x2=X factor from the PYPDFU and
C...  correction factors from our choice of phase space sampling.
      
C-----------------------------------------------------------------
      
C...  The third step is to evaluate the cross section in the 
C...  phase space point selected above.
      
      MPSI = 0.5D0*(m1 + m2)
      
C...  Pick Q2 scale (which involves some arbitrariness) 
      Q2=(MPSI**2-THAT)*(MPSI**2-UHAT)/SHAT

C...  and evaluate alpha_s and alpha_em
      ALPS=PYALPS(ScfAlpS**2*Q2) 
      AEM=PYALEM(Q2)
 
C...  Evaluate matrix element d(sigma-hat)/d(t-hat)
      DSIGDT=(8*ALPS**4*PI**3*PSI0**4*(335*SHAT**12 + 
     &     972*(THAT - UHAT)**12 + 274*SHAT**11*(THAT + UHAT) + 
     &     3780*SHAT*(THAT - UHAT)**10*(THAT + UHAT) + SHAT**10*
     &     (-3393*THAT**2 + 6782*THAT*UHAT - 3393*UHAT**2) + 
     &     21*SHAT**2*(THAT - UHAT)**8*(223*THAT**2 + 398*THAT*UHAT + 
     &     223*UHAT**2) + SHAT**9*(-7124*THAT**3 + 7204*THAT**2*UHAT + 
     &     7204*THAT*UHAT**2 - 7124*UHAT**3) - 16*SHAT**5*(THAT - 
     &     UHAT)**4*(778*THAT**3 - 821*THAT**2*UHAT - 821*THAT*UHAT**2 +
     &     778*UHAT**3) - 2*SHAT**3*(THAT - UHAT)**6*(2647*THAT**3 - 
     &     1375*THAT**2*UHAT - 1375*THAT*UHAT**2 + 2647*UHAT**3) + 
     &     4*SHAT**7*(THAT - UHAT)**2*(5459*THAT**3 - 4631*
     &     (THAT**2*UHAT + THAT*UHAT**2) + 5459*UHAT**3) + 
     &     SHAT**8*(3290*THAT**4 - 24440*THAT**3*UHAT + 
     &     42684*THAT**2*UHAT**2 - 24440*THAT*UHAT**3 + 3290*UHAT**4) + 
     &     2*SHAT**6*(THAT - UHAT)**2*(7771*THAT**4 - 1016*THAT**3*UHAT 
     &     - 13126*THAT**2*UHAT**2 - 1016*THAT*UHAT**3 + 7771*UHAT**4) - 
     &     SHAT**4*(THAT - UHAT)**4*(20405*THAT**4 - 18628*THAT**3*UHAT 
     &     - 3938*THAT**2*UHAT**2 - 18628*THAT*UHAT**3 + 
     &     20405*UHAT**4)))/(1296.*MPSI**2*SHAT**8*
     &     (MPSI**2 - THAT)**4*(MPSI**2 - UHAT)**4)
      
      DSIGDT = Kk*DSIGDT
      
      IF (DSIGDT.LT.0) THEN
         WRITE(*,'(A,A,A)') 'LbOniaPairsProduction: ONIAPAIRS_UPEVNT:',
     &        ' Error: negative cross section value. Event rejected.',
     &        ' Generation can be continued normaly.'
         IREJ=.TRUE.
      ENDIF

C...  Evaluate parton distributions (multiplied by x) of protons
      CALL PYPDFU(2212,X1,ScfPDF**2*Q2,XPP1)
      CALL PYPDFU(2212,X2,ScfPDF**2*Q2,XPP2)
            
C...  Now possible to define the XWGTUP return value.
C...  The CONV factor converts from GeV**(-2) to pb.
      IF (.NOT.IREJ) XWGTUP=CONV*PHSPV*DSIGDT*XPP1(21)*XPP2(21)

C...  Done when initializing for maximum cross section.
      IF (INIT) RETURN
      
      Nexec(IDPRUP)=Nexec(IDPRUP)+1
      CSsum(IDPRUP)=CSsum(IDPRUP)+XWGTUP
      
      IF(XWGTUP.GT.Wtmax(IDPRUP)) THEN
         WRITE(*,'(A,A,F7.3,A,A)') 
     &        'LbOniaPairsProduction: ONIAPAIRS_UPEVNT: ',
     &        'Warning: XWGTUP ', XWGTUP/Wtmax(IDPRUP), 
     &        ' times greater than XMAXUP. ',
     &        'Bad it it happens frequently.'
         XWGTUP = Wtmax(IDPRUP)
         Nexcs(IDPRUP) = Nexcs(IDPRUP) + 1
      ENDIF

C...  Store scale choice etc.
      SCALUP=ScfShwr*DSQRT(Q2)
      AQEDUP=AEM
      AQCDUP=ALPS
      
C-----------------------------------------------------------------
      
C...  The forth step is to set the kinematical variables
      
C...  Reset momenta to zero.
      DO 130 I=1,8
         DO 120 J=1,5
            PUP(J,I) = 0D0
 120     CONTINUE
 130  CONTINUE
      
C...  Masses of final state entries; initial assumed massless.
c...  Particles 3 and 4 are massive - two quarkonia
      PUP(5,3) = M1
      PUP(5,4) = M2
      
C...  Four-momenta of the incoming partons simple.
      PUP(4,1)=0.5D0*X1*ECM
      PUP(3,1)=PUP(4,1)
      PUP(4,2)=0.5D0*X2*ECM
      PUP(3,2)=-PUP(4,2)
     
C...  Subsystem scattering angle
      IF (PABS.ne.0) COSTHE=(that-m1**2)/(dsqrt(shat)*pabs) 
     &     + dsqrt(1d0+(m1/pabs)**2)
      SINTHE=DSQRT(MAX(0D0,1D0-COSTHE**2))
      
C...  Azimuthal angle at random.
      PHI=2D0*PI*PYR(0)
      
C...  Energies of the outgoing particles in the subsystem frame.
      PE3=DSQRT(PUP(5,3)**2+PABS**2)
      PE4=DSQRT(PUP(5,4)**2+PABS**2)
      
C...  Momenta of outgoing particles in the subsystem frame.
      PUP(1,3)=PABS*SINTHE*COS(PHI)
      PUP(2,3)=PABS*SINTHE*SIN(PHI)
      PZ3=PABS*COSTHE
      PUP(1,4)=-PUP(1,3)
      PUP(2,4)=-PUP(2,3)
      PZ4=-PZ3
      
C...  Longitudinal boost of outgoing particles to cm frame.
      BETA=(X1-X2)/(X1+X2)
      GAMM=0.5D0*(X1+X2)/DSQRT(X1*X2)
      PUP(3,3)=GAMM*(PZ3+BETA*PE3)
      PUP(4,3)=GAMM*(PE3+BETA*PZ3)
      PUP(3,4)=GAMM*(PZ4+BETA*PE4)
      PUP(4,4)=GAMM*(PE4+BETA*PZ4)
      
C...  Done.
      RETURN
      END
      
      
