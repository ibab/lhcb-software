* vacpol.f ==========================================================
* By A. Shamov
* Vaccum polarization correction calculation from Berends's gg_mmggen
* generator, it's not the exact polarization operator 
*====================================================================
      double precision function VacPolDelta(t1)
      double precision t1
      double precision VacPolDeltaLept,REPIMlpair
      double precision MTAU,ME,MU
      parameter (MTAU=1777.96d-3)
      parameter (ME=0.000511D0)
      parameter (MU=0.105658387D0)
      VacPolDelta=
     &     VacPolDeltaLept(t1,ME)+
     &     VacPolDeltaLept(t1,MU)+
     &     VacPolDeltaLept(t1,MTAU)+
     &     2.D0*REPIMlpair(T1)
      end
c
      double precision function VacPolDeltaLept(t1,MTAU)
      implicit none
      double precision t1,MTAU
      double precision PI,ALFA
      parameter (PI=3.14159265359D0,ALFA=1.D0/137.036D0)
      double precision ATAUK,DETOT5
c
c Correction for vacuum polarization by muon and tau loops
c is obtained by expanding the above formulae for large A.
c DETOT=-2*ALFA/PI*(-4/15*1/A**2-16/105/A**4.........)
c
      ATAUK=1.D0-4.D0*MTAU*MTAU/T1
      IF (ATAUK.LT.10.D0) GOTO 134
      DETOT5=2.D0*ALFA/PI*(4.D0/(15.D0*ATAUK)+16.D0/(105.D0*ATAUK*
     .       ATAUK))
      GOTO 135
  134 CONTINUE
      ATAUK = DSQRT(ATAUK)
      DETOT5=-2.D0*ALFA/PI*(8.D0/9.D0-ATAUK*ATAUK/3.D0+ATAUK*
     .       (0.5D0-ATAUK*ATAUK/6.D0)*DLOG((ATAUK-1.D0)/(ATAUK+1.D0)))
  135 CONTINUE
c
      VacPolDeltaLept=DETOT5
c
      end

      FUNCTION REPIMlpair(S)
C-------------------------------------------- REMARKS ------------------
C VACUUM POLARIZATION IN QED.
C THE HADRONIC CONTRIBUTION IS OBTAINED USING A DISPERSION INTEGRAL
C OVER THE KNOWN HADRONIC CROSS SECTION.
C THE RESULT USED HERE IS A PARAMETRIZATION GIVEN BY  
C H.BURKHARDT, TASSO NOTE 192(1981).   
C-----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
C-------------------------------------------- CHECK FOR S VALUES -------
      DATA I/0/
      IF(I.EQ.1) GOTO 2
      I=1
      IF(S.GT.0.D0.AND.S.LT.100.D0) WRITE(2,1) S
    1 FORMAT(3H0S=,F6.3,7H GEV**2/
     . 46H VACUUM POLARIZATION MAY BE BADLY APPROXIMATED)
    2 CONTINUE    
      X=DABS(S)   
C-------------------------------------------- HADRONIC PART -----------
      IF(X.LT.1.D0)
     . REPIM= - 2.302D-03*DLOG(1.D0+4.091D0*X)
      IF(X.GE.1.D0.AND.X.LT.64.D0)
     . REPIM= - 2.822D-03*DLOG(1.D0+1.218D0*X) - 1.512D-03
      IF(X.GE.64.D0)
     . REPIM= - 3.068D-03*DLOG(1.D0 +       X) - 1.1344D-03
      RETURN
      END
