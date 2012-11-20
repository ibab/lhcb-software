C********************************************************************
C
C Calculates the vbf_wp_wp_bra and vbf_wp_wp_ket vectors for external massless fermions as
C given in Hagiwara, Zeppenfeld  MAD/PH/402, Appendix A, Eq.A.9
C checked on May 24., 88
C
C Modification on Aug. 14, 1992: 
C
C     The omega factors, i.e. sqrt(2E) are multiplied in already at 
C     the Chi level and returned are both helicity Weyl spinors
C     S*sqrt(2E)*chi_{+} and S*sqrt(2E)*chi_{-} 
C
C     In addition the subroutine was transformed to real*8
C
C  INPUT:
C  ------
C
C  NF            Number of fermions to calculate bras or kets for.
C                Fermions with odd index generate kets, even indexed
C                ones generate bras.
C
C  PBAR(0:3,NF)  Physical momenta of the NF fermions. All fermions
C                are assumed to be massless.
C
C  SIGN(NF)      The sign factors distinguishing fermion (S=+1) from
C                antifermion (S=-1) for all NF fermions
C
C
C  OUTPUT:
C  -------
C
C  PSI(2,-1:1,NF) 2 dimensional complex vector containing the vbf_wp_wp_ket |i>
C                 for odd and the vbf_wp_wp_bra <i| for even numbered fermions
C                 time S*sqrt(2E)
C
C********************************************************************
C
      SUBROUTINE vbf_wp_wp_PSI0M( NF,PBAR,SIGN,PSI )
      IMPLICIT NONE
      INTEGER  IF, NF, SIGN(NF)
      DOUBLE COMPLEX  PSI(2,-1:1,NF)
      DOUBLE PRECISION  PBAR(0:3,NF), PABS, PBARX, PBARY, PBARZ, PAPZ
      DOUBLE PRECISION  NORMAL, NX, NY, NZ, EPS
      PARAMETER ( EPS=1.d-30 )
CC
      DO IF = 1,NF
        PABS  = PBAR(0,IF)
        PBARX = PBAR(1,IF)
        IF ( MOD(IF,2).EQ.1 ) THEN
          PBARY =  PBAR(2,IF)
        ELSE
          PBARY = -PBAR(2,IF)
        ENDIF
        PBARZ = PBAR(3,IF)
        IF ( PBARZ.GT.0.d0 ) THEN
           PAPZ = PABS + PBARZ
        ELSE
           PAPZ = (PBARX**2 + PBARY**2)/(PABS - PBARZ)
        ENDIF
c
c treat the pbar along negative z-axis case first
c
        IF ( PAPZ.LE.EPS*PABS ) THEN
           NORMAL = SIGN(IF)*SQRT(2.d0*PBAR(0,IF))
           PSI(1,-1,IF) = -NORMAL 
           PSI(2,-1,IF) = 0.d0
           PSI(1,1,IF)  = 0.d0
           PSI(2,1,IF)  = NORMAL
        ELSE
c
c and now the general case
c
           NORMAL = SIGN(IF)/SQRT(PAPZ)
           NX = NORMAL*PBARX
           NY = NORMAL*PBARY
           NZ = NORMAL*PAPZ
           PSI(1,-1,IF) = DCMPLX(-NX,NY)
           PSI(2,-1,IF) = NZ
           PSI(1,1,IF)  = NZ
           PSI(2,1,IF)  = DCMPLX(NX,NY)
        ENDIF
      ENDDO
ccc
      END
C
C************************************  vbf_wp_wp_curr  *************************
C
C  vbf_wp_wp_curr calculates the current 
C
C        PSIBAR gamm^mu PSI
C 
C  of Hagiwara, Zeppenfeld, Nucl.Phys.B313 (1989) 560, eq. 2.20 for 
C  the two possible polarizations of the Weyl spinors PSIBAR and PSI
C
C  INPUT:
C  ------
C
C    PSIBAR(2,-1:1)  two Weyl spinors for 2 helicity states each
C    PSI(2,-1:1)
C
C  OUTPUT:
C  -------
C
C  J(0:3,-1:1)       the current as defined in eq.2.27 for two possible
C                    helicity combinations for massless fermions
C
      subroutine vbf_wp_wp_curr( sigmax,psibar,psi,j )
      double complex  j(0:3,-1:1), psibar(2,-1:1), psi(2,-1:1), zj2
      double complex  z1, z2, z3, z4
      integer  sigmax,sig
cc
      do sig = -1,sigmax,2
         z1 = psibar(1,sig) * psi(1,sig)
         z2 = psibar(2,sig) * psi(2,sig)
         z3 = psibar(1,sig) * psi(2,sig)
         z4 = psibar(2,sig) * psi(1,sig)
         j(0,sig) = z1 + z2
         if (sig.eq.-1) then
            j(1,sig) = -(z3+z4)
            zj2 = z3-z4
            j(2,sig) = dcmplx(-dimag(zj2),dreal(zj2))
            j(3,sig) = z2-z1
         else
            j(1,sig) = z3+z4
            zj2 = z4-z3
            j(2,sig) = dcmplx(-dimag(zj2),dreal(zj2))
            j(3,sig) = z1-z2
         endif
      enddo
ccc
      end
C
C************************************  vbf_wp_wp_curr6  *************************
C
C  vbf_wp_wp_curr6 calculates the current 
C
C        PSIBAR gamm^mu PSI
C 
C  of Hagiwara, Zeppenfeld, Nucl.Phys.B313 (1989) 560, eq. 2.20 for 
C  the two possible polarizations of the Weyl spinors PSIBAR and PSI
c  and stores the current momentum in the last 2 complex entries of the 
c  current, as in HELAS
C
C  INPUT:
C  ------
C
C    PSIBAR(2,-1:1)  two Weyl spinors for 2 helicity states each
C    PSI(2,-1:1)
c    pb(0:3)         the outflowing momentum of the vbf_wp_wp_bra spinor psibar
c    p(0:3)          the inflowing momentum of the vbf_wp_wp_ket spinor psi
C
C  OUTPUT:
C  -------
C
C  J(0:3,-1:1)       the current as defined in eq.2.27 for two possible
C                    helicity combinations for massless fermions
C  J(4:5,-1:1)       the momentum q = p-pb of the current, flowing away from 
c                    the fermion line stored as
c                    J(4) = dcmplx(q0,qz), J(5) = dcmplx(qx,qy)
C
      subroutine vbf_wp_wp_curr6( sigmax,psibar,pb,psi,p,j )
      double complex  j(0:5,-1:1), psibar(2,-1:1), psi(2,-1:1), zj2
      double precision pb(0:3), p(0:3)
      double complex  z1, z2, z3, z4, q4, q5
      integer  sigmax,sig
cc
      q4 = dcmplx(p(0)-pb(0),p(3)-pb(3))
      q5 = dcmplx(p(1)-pb(1),p(2)-pb(2))
      do sig = -1,sigmax,2
         z1 = psibar(1,sig) * psi(1,sig)
         z2 = psibar(2,sig) * psi(2,sig)
         z3 = psibar(1,sig) * psi(2,sig)
         z4 = psibar(2,sig) * psi(1,sig)
         j(0,sig) = z1 + z2
         if (sig.eq.-1) then
            j(1,sig) = -(z3+z4)
            zj2 = z3-z4
            j(2,sig) = dcmplx(-dimag(zj2),dreal(zj2))
            j(3,sig) = z2-z1
         else
            j(1,sig) = z3+z4
            zj2 = z4-z3
            j(2,sig) = dcmplx(-dimag(zj2),dreal(zj2))
            j(3,sig) = z1-z2
         endif
         j(4,sig) = q4
         j(5,sig) = q5
      enddo
ccc
      end
C
C******************************** vbf_wp_wp_BRA2R, vbf_wp_wp_BRA2C ************************
C
C   Calculates the vbf_wp_wp_bra vector <i,k| as given in
C      MAD/PH/402, Appendix A, Eqn A.10, top eqn.
C      checked on May 26, 88
C
C   Modified to double precision and nonzero time component of 
C   polarization vector Aug. 92
C
C   INPUT:
C            chi      double complex array(2)
C                        vbf_wp_wp_bra <i| as given by eq. A.9 if chreal = .true.
C                        any vbf_wp_wp_bra <...| if chreal = .false.
C
C            chreal  logical
C                        .true. if one component of chi is real
C                        .false. otherwise
C
C            p      double precision  array(0:3)
C                        standard four momentum for fermion
C                        p(0:3) = [E, px, py, pz]
C
C            sigmap      integer
C                        chirality/helicity factor for fermion
C                        allowed values: +1,-1
C
C            k      double precision  array(0:4)
C                        standard four momentum for boson and mass**2
C                        k(0:4) = [E, kx, ky, kz, mass**2]
C
C            reps     double precision  array(0:3)
C            ceps     double complex array(0:3)
C                     real or complex polarisation vector of the boson.
C
C   OUTPUT:
C            result      double complex array(1:2)
C                        two component row vector <i,k| on Eqn. A.10
C
C            pplusk      double precision  array(0:4)
C                        four momentum p+k. The fourth component
C                        contains the square (p+k)**2
C
C
      SUBROUTINE vbf_wp_wp_BRA2R(CHI,CHREAL,P,SIGMAP,K,REPS,RESULT,PPLUSK)
c
c arguments
c
      DOUBLE COMPLEX  CHI(2), CEPS(0:3), RESULT(2)
      DOUBLE PRECISION  P(0:3), K(0:4), REPS(0:3), PPLUSK(0:4)
      INTEGER  SIGMAP
      LOGICAL  CHREAL
c
c local variables
c
      DOUBLE PRECISION  A0,A1,A2,A3,vbf_wp_wp_B0,B1,B2,B3,C,CR,CI,
     &                  D0,D1,D2,D3,CCR,CCI
      DOUBLE PRECISION  T1R,T1I,T2R,T2I
      DOUBLE PRECISION  PROP
      INTEGER  INDEX1, INDEX2
      LOGICAL  DEXIST
c
c Compute product of the three matrices in A.10 explicitly in terms 
C of components.
c If one component of Chi is real it is denoted by "c", the other is 
C complex and separated as cr + i*ci. If both are complex replace 
C c --> ccr + i*cci. The 2 possible helicity indices are combined by 
C using some Pauli matrix algebra.
c
c Computations are reduced by computing (chi*eps)*(p+k).
c The (chi*eps) temporary is stored in the row vector:
c   ( t1r + I*t1i, t2r + I*t2i )
c
c Since the 4-vector eps can be real or complex, the chi*eps part is 
C done with two different entries into the subroutine: vbf_wp_wp_BRA2R and vbf_wp_wp_BRA2C
c
      A0 = REPS(0)
      IF ( SIGMAP.EQ.1 ) THEN
         A1 = REPS(1)
         INDEX1 = 1
         INDEX2 = 2
      ELSE
         A1 = -REPS(1)
         INDEX1 = 2
         INDEX2 = 1
      ENDIF
      A2 = REPS(2)
      A3 = REPS(3)
      DEXIST = .FALSE.
      GOTO 1
c
c entry for complex polarization vector
c
      ENTRY vbf_wp_wp_BRA2C( CHI,CHREAL,P,SIGMAP,K,CEPS,RESULT,PPLUSK )
      A0 = DREAL(CEPS(0))
      D0 = DIMAG(CEPS(0))
      IF ( SIGMAP.EQ.1 ) THEN
         A1 = DREAL(CEPS(1))
         D1 = DIMAG(CEPS(1))
         INDEX1 = 1
         INDEX2 = 2
      ELSEIF ( SIGMAP.EQ.-1 ) THEN
         A1 = -DREAL(CEPS(1))
         D1 = -DIMAG(CEPS(1))
         INDEX1 = 2
         INDEX2 = 1
      ELSE
c
c unrecognised value of simgap
c
        WRITE (*,*) 'Invalid Sigmap in BRA2 : Sigmap = ',SIGMAP
        RESULT(1) = 0.d0
        RESULT(2) = 0.d0
        RETURN
      ENDIF
      A2 = DREAL(CEPS(2))
      D2 = DIMAG(CEPS(2))
      A3 = DREAL(CEPS(3))
      D3 = DIMAG(CEPS(3))
      DEXIST = .TRUE.

  1   CONTINUE
c
c compute the sum (p+k) and store in pplusk:
c   (p+k)(0:3) = pplusk(0:3), (p+k)**2 = pplusk(4)
c
      PPLUSK(0) = P(0) + K(0)
      PPLUSK(1) = P(1) + K(1)
      PPLUSK(2) = P(2) + K(2)
      PPLUSK(3) = P(3) + K(3)
      PPLUSK(4) = PPLUSK(0)**2 - PPLUSK(1)**2 - 
     &            PPLUSK(2)**2 - PPLUSK(3)**2
      PROP = 1.d0/PPLUSK(4)

      vbf_wp_wp_B0 = PPLUSK(0)
      IF ( SIGMAP.EQ.1 ) THEN
            B1 = PPLUSK(1)
      ELSE
            B1 = - PPLUSK(1)
      ENDIF
      B2 = PPLUSK(2)
      B3 = PPLUSK(3)
c
c now calculate Chi*eps*prop
c
      IF ( CHREAL ) THEN
         C  = DREAL(CHI(INDEX1))*PROP
         CR = DREAL(CHI(INDEX2))*PROP
         CI = DIMAG(CHI(INDEX2))*PROP

         IF ( DEXIST ) THEN
            T1R = -C*(A3-A0) + CR*(D2-A1) + CI*(A2+D1)
            T1I = -C*(D3-D0) + (D2-A1)*CI - (A2+D1)*CR
            T2R =  CR*(A3+A0) - C*(A1+D2) - CI*(D3+D0)
            T2I =  C*(A2-D1) + CI*(A3+A0) + CR*(D3+D0)
         ELSE
            T1R = -C*(A3-A0) - CR*A1 + CI*A2
            T1I = -A1*CI - A2*CR
            T2R = CR*(A3+A0) - C*A1
            T2I = C*A2 + CI*(A3+A0)
         ENDIF
      ELSE
         CCR = DREAL(CHI(INDEX1))*PROP
         CCI = DIMAG(CHI(INDEX1))*PROP
         CR  = DREAL(CHI(INDEX2))*PROP
         CI  = DIMAG(CHI(INDEX2))*PROP

         IF ( DEXIST ) THEN
            T1R = -CCR*(A3-A0) + CCI*(D3-D0) + CR*(D2-A1) + CI*(A2+D1)
            T1I = -CCR*(D3-D0) - CCI*(A3-A0) + (D2-A1)*CI - (A2+D1)*CR
            T2R =  CR*(A3+A0) - CCR*(A1+D2) - CCI*(A2-D1) - CI*(D3+D0)
            T2I = CCR*(A2-D1) - CCI*(A1+D2) + CI*(A3+A0) + CR*(D3+D0)
         ELSE
            T1R = -CCR*(A3-A0) - CR*A1 + CI*A2
            T1I = -CCI*(A3-A0) - A1*CI - A2*CR
            T2R = CR*(A3+A0) - CCR*A1 - CCI*A2
            T2I = CCR*A2 - CCI*A1 + CI*(A3+A0)
         ENDIF
      ENDIF

      RESULT(INDEX1) = DCMPLX( T1R*(vbf_wp_wp_B0+B3) + T2R*B1 - T2I*B2,
     &                         T1I*(vbf_wp_wp_B0+B3) + T2I*B1 + T2R*B2 )
      RESULT(INDEX2) = DCMPLX( T1R*B1 + T1I*B2 + T2R*(vbf_wp_wp_B0-B3),
     &                         T1I*B1 - T1R*B2 + T2I*(vbf_wp_wp_B0-B3) )
ccc
      END
C
C********************* vbf_wp_wp_KET2R, vbf_wp_wp_KET2C **********************************
C
C   Calculates the vbf_wp_wp_ket vector |k,i> as given in
C      MAD/PH/402, Appendix A, Eqn A.10, bottom eqn.
C      checked on May 26, 88
C
C   Modified to double precision and nonzero time component of 
C   polarization vector Aug. 92
C
C   INPUT:
C            chi      double complex array(2)
C                        vbf_wp_wp_ket |i> as given by eq. A.9 if chreal = .true.
C                        any vbf_wp_wp_ket |...> if chreal = .false.
C
C            chreal  logical
C                        .true. if one component of chi is real
C                        .false. otherwise
C
C            p      double precision  array(0:3)
C                        standard four momentum for fermion
C                        p(0:3) = [E, px, py, pz]
C
C            sigmap      integer
C                        chirality/helicity factor for fermion
C                        allowed values: +1,-1
C
C            k      double precision  array(0:4)
C                        standard four momentum for boson and mass**2
C                        k(0:4) = [E, kx, ky, kz, mass**2]
C
C            reps   double precision  array(0:3)
C            ceps   double complex array(0:3)
C                   real or complex polarisation vector of the boson. 
C
C   OUTPUT:
C            result      double complex array(1:2)
C                        two component column vector |k,i> on Eqn. A.10
C
C            pmink      double precision  array(0:4)
C                        four momentum p-k. The fourth component
C                        contains the square (p-k)**2
C
      SUBROUTINE vbf_wp_wp_KET2R( CHI,CHREAL,P,SIGMAP,K,REPS,RESULT,PMINK )
c
c arguments
c
      DOUBLE COMPLEX  CHI(2), CEPS(0:3), RESULT(2)
      DOUBLE PRECISION  P(0:3), K(0:4), REPS(0:3), PMINK(0:4)
      INTEGER  SIGMAP
      LOGICAL  CHREAL
c
c local variables
c
      DOUBLE PRECISION   A0,A1,A2,A3,vbf_wp_wp_B0,B1,B2,B3,C,CR,CI,
     &                   D0,D1,D2,D3,CCI,CCR
      DOUBLE PRECISION   T1R,T1I,T2R,T2I
      DOUBLE PRECISION   PROP
      INTEGER  INDEX1, INDEX2
      LOGICAL  DEXIST
c
c Compute product of the three matrices in A.10 explicitly in terms 
C of components.
c If one component of Chi is real it is denoted by "c", the other is 
C complex and separated as cr + i*ci. If both are complex replace 
C c --> ccr + i*cci. The 2 possible helicity indices are combined by 
C using some Pauli matrix algebra.
c
c Computations are reduced by computing (p-k)*(eps*chi).
c The (eps*chi) temporary is stored in the vector:
c     ( t1r + I*t1i, t2r + I*t2i )
c
c Since the 4-vector eps can be real or complex, the chi*eps part is 
C done with two different entries into the subroutine: vbf_wp_wp_BRA2R and vbf_wp_wp_BRA2C
c
      A0 = REPS(0)
      IF ( SIGMAP.EQ.1 ) THEN
         A1 = REPS(1)
         INDEX1 = 1
         INDEX2 = 2
      ELSE
         A1 = -REPS(1)
         INDEX1 = 2
         INDEX2 = 1
      ENDIF
      A2 = REPS(2)
      A3 = REPS(3)
      DEXIST = .FALSE.
      GOTO 1
c
c entry for complex polarization vector
c
      ENTRY vbf_wp_wp_KET2C( CHI,CHREAL,P,SIGMAP,K,CEPS,RESULT,PMINK )
      A0 = DREAL(CEPS(0))
      D0 = DIMAG(CEPS(0))
      IF ( SIGMAP.EQ.1 ) THEN
         A1 = DREAL(CEPS(1))
         D1 = DIMAG(CEPS(1))
         INDEX1 = 1
         INDEX2 = 2
      ELSEIF ( SIGMAP.EQ.-1 ) THEN
         A1 = -DREAL(CEPS(1))
         D1 = -DIMAG(CEPS(1))
         INDEX1 = 2
         INDEX2 = 1
      ELSE
c
c unrecognised value of simgap
c
         WRITE(6,*) 'Invalid Sigmap in BRA2 : Sigmap = ',SIGMAP
         RESULT(1) = 0.d0
         RESULT(2) = 0.d0
         RETURN
      ENDIF
      A2 = DREAL(CEPS(2))
      D2 = DIMAG(CEPS(2))
      A3 = DREAL(CEPS(3))
      D3 = DIMAG(CEPS(3))
      DEXIST = .TRUE.

  1   CONTINUE
c
c compute the sum (p-k) and store in pmink:
c   (p-k)(0:3) = pmink(0:3), (p-k)**2 = pmink(4)
c
      PMINK(0) = P(0) - K(0)
      PMINK(1) = P(1) - K(1)
      PMINK(2) = P(2) - K(2)
      PMINK(3) = P(3) - K(3)
      
      PMINK(4) = PMINK(0)**2 - PMINK(1)**2 - PMINK(2)**2 - PMINK(3)**2
      PROP = 1.d0/PMINK(4)

      vbf_wp_wp_B0 = PMINK(0)
      IF ( SIGMAP.EQ.1 ) THEN
            B1 = PMINK(1)
      ELSE
            B1 = -PMINK(1)
      ENDIF
      B2 = PMINK(2)
      B3 = PMINK(3)
c
c now calculate Chi*eps*prop
c
        IF ( CHREAL ) THEN
           C  = DREAL(CHI(INDEX1))*PROP
           CR = DREAL(CHI(INDEX2))*PROP
           CI = DIMAG(CHI(INDEX2))*PROP

           IF ( DEXIST ) THEN
              T1R = -C*(A3-A0) - CR*(D2+A1) - CI*(A2-D1)
              T1I = -C*(D3-D0) - CI*(D2+A1) + CR*(A2-D1)
              T2R =  C*(D2-A1) + CR*(A3+A0) - CI*(D3+D0)
              T2I = -C*(A2+D1) + CI*(A3+A0) + CR*(D3+D0)
           ELSE
              T1R = -C*(A3-A0) - CR*A1 - CI*A2
              T1I = -CI*A1 + CR*A2
              T2R = -C*A1 + CR*(A3+A0)
              T2I = -C*A2 + CI*(A3+A0)
           ENDIF
        ELSE
           CCR = DREAL(CHI(INDEX1))*PROP
           CCI = DIMAG(CHI(INDEX1))*PROP
           CR  = DREAL(CHI(INDEX2))*PROP
           CI  = DIMAG(CHI(INDEX2))*PROP

           IF ( DEXIST ) THEN
              T1R = -CCR*(A3-A0)+ CCI*(D3-D0) - CR*(D2+A1) - CI*(A2-D1)
              T1I = -CCR*(D3-D0)- CCI*(A3-A0) - CI*(D2+A1) + CR*(A2-D1)
              T2R =  CCR*(D2-A1)+ CCI*(A2+D1) + CR*(A3+A0) - CI*(D3+D0)
              T2I =  CCI*(D2-A1)- CCR*(A2+D1) + CI*(A3+A0) + CR*(D3+D0)
           ELSE
              T1R = -CCR*(A3-A0) - CR*A1 - CI*A2
              T1I = -CCI*(A3-A0) - CI*A1 + CR*A2
              T2R = -CCR*A1 + CCI*A2 + CR*(A3+A0)
              T2I = -CCR*A2 - CCI*A1 + CI*(A3+A0)
           ENDIF
        ENDIF

        RESULT(INDEX1) = DCMPLX( T1R*(vbf_wp_wp_B0+B3) + T2R*B1 + T2I*B2,
     &                           T1I*(vbf_wp_wp_B0+B3) + T2I*B1 - T2R*B2 )
        RESULT(INDEX2) = DCMPLX( T1R*B1 - T1I*B2 + T2R*(vbf_wp_wp_B0-B3),
     &                           T1I*B1 + T1R*B2 + T2I*(vbf_wp_wp_B0-B3) )
ccc
      END
C
C**********************  vbf_wp_wp_S1R,vbf_wp_wp_S1C  *************************************
C  checked on 31.5.88
C
C  vbf_wp_wp_S1R and vbf_wp_wp_S1C calculate the complex number 
C             <vbf_wp_wp_bra|(rvec-slash)_sigma|vbf_wp_wp_ket>
C  in the notation of HZ
C
C  INPUT:
C  ------
C
C  vbf_wp_wp_bra(2)           double complex vbf_wp_wp_bra vector
C
C  RVEC(0:3)        double precision 4-vector
C  CVEC(0:3)        double complex 4-vector
C
C  TIMEEX           logical variable:
C                      .true. if time component of RVEC/CVEC is nonzero
C                      .false. if time component of RVEC/CVEC vanishes
C
C  SIGMA            integer index of (rvec-slash)_sigma.
C                      Possible values: +1,-1
C
C  vbf_wp_wp_ket(2)           complex vbf_wp_wp_ket vector
C
C  OUTPUT:
C  -------
C
C  vbf_wp_wp_S1R,vbf_wp_wp_S1C          complex function value
C
      FUNCTION vbf_wp_wp_S1R( vbf_wp_wp_bra,RVEC,TIMEEX,SIGMA,vbf_wp_wp_ket )
      DOUBLE COMPLEX  vbf_wp_wp_bra(2), CVEC(0:3), vbf_wp_wp_ket(2), vbf_wp_wp_S1R, vbf_wp_wp_S1C
      DOUBLE PRECISION  RVEC(0:3)
      INTEGER  SIGMA
      LOGICAL  TIMEEX
c
c local variables
c
      DOUBLE PRECISION  A0,A1,A2,A3, vbf_wp_wp_B0,B1,B2,B3
      DOUBLE PRECISION  SL11R,SL11I,SL12R,SL12I,SL21R,SL21I, 
     &                  SL22R,SL22I
      DOUBLE PRECISION  K1R,K1I,K2R,K2I, KA1R,KA1I,KA2R,KA2I
cc
      K1R = DREAL(vbf_wp_wp_ket(1))
      K1I = DIMAG(vbf_wp_wp_ket(1))
      K2R = DREAL(vbf_wp_wp_ket(2))
      K2I = DIMAG(vbf_wp_wp_ket(2))

      A1 = RVEC(1)
      A2 = RVEC(2)
      A3 = RVEC(3)
      IF ( TIMEEX ) THEN
        A0 = RVEC(0)
        IF ( SIGMA.EQ.1 ) THEN
          SL11R =  A3 - A0
          SL22R = -A3 - A0
        ELSE
          SL11R =  A3 + A0
          SL22R = -A3 + A0
        ENDIF
      ELSE
        SL11R =  A3
        SL22R = -A3
      ENDIF

      KA1R = SL11R*K1R + A1*K2R + A2*K2I
      KA1I = SL11R*K1I - A2*K2R + A1*K2I
      KA2R = SL22R*K2R + A1*K1R - A2*K1I
      KA2I = SL22R*K2I + A1*K1I + A2*K1R

      K1R = DREAL(vbf_wp_wp_bra(1))
      K1I = DIMAG(vbf_wp_wp_bra(1))
      K2R = DREAL(vbf_wp_wp_bra(2))
      K2I = DIMAG(vbf_wp_wp_bra(2))

      IF (SIGMA.EQ.-1) THEN
        vbf_wp_wp_S1R = DCMPLX( K1R*KA1R - K1I*KA1I + K2R*KA2R - K2I*KA2I,
     &                K1R*KA1I + K1I*KA1R + K2R*KA2I + K2I*KA2R )
      ELSE
        vbf_wp_wp_S1R = DCMPLX( -K1R*KA1R + K1I*KA1I - K2R*KA2R + K2I*KA2I,
     &                -K1R*KA1I - K1I*KA1R - K2R*KA2I - K2I*KA2R )
      ENDIF

      RETURN
c
c entry for complex 4-vector
c
      ENTRY vbf_wp_wp_S1C( vbf_wp_wp_bra,CVEC,TIMEEX,SIGMA,vbf_wp_wp_ket )

      K1R = DREAL(vbf_wp_wp_ket(1))
      K1I = DIMAG(vbf_wp_wp_ket(1))
      K2R = DREAL(vbf_wp_wp_ket(2))
      K2I = DIMAG(vbf_wp_wp_ket(2))

      A1 = DREAL(CVEC(1))
      A2 = DREAL(CVEC(2))
      A3 = DREAL(CVEC(3))

      B1 = DIMAG(CVEC(1))
      B2 = DIMAG(CVEC(2))
      B3 = DIMAG(CVEC(3))

      IF ( TIMEEX ) THEN
        A0 = DREAL(CVEC(0))
        vbf_wp_wp_B0 = DIMAG(CVEC(0))
        IF ( SIGMA.EQ.1 ) THEN
          SL11R =  A3 - A0
          SL22R = -A3 - A0
          SL11I =  B3 - vbf_wp_wp_B0
          SL22I = -B3 - vbf_wp_wp_B0
        ELSE
          SL11R =  A3 + A0
          SL22R = -A3 + A0
          SL11I =  B3 + vbf_wp_wp_B0
          SL22I = -B3 + vbf_wp_wp_B0
        ENDIF
      ELSE
        SL11R =  A3
        SL22R = -A3
        SL11I =  B3
        SL22I = -B3
      ENDIF

      SL12R = A1 + B2
      SL12I = B1 - A2
      SL21R = A1 - B2
      SL21I = A2 + B1

      KA1R = SL11R*K1R - SL11I*K1I + SL12R*K2R - SL12I*K2I
      KA1I = SL11R*K1I + SL11I*K1R + SL12R*K2I + SL12I*K2R
      KA2R = SL21R*K1R - SL21I*K1I + SL22R*K2R - SL22I*K2I
      KA2I = SL21R*K1I + SL21I*K1R + SL22R*K2I + SL22I*K2R

      K1R = DREAL(vbf_wp_wp_bra(1))
      K1I = DIMAG(vbf_wp_wp_bra(1))
      K2R = DREAL(vbf_wp_wp_bra(2))
      K2I = DIMAG(vbf_wp_wp_bra(2))

      IF ( SIGMA.EQ.-1 ) THEN
        vbf_wp_wp_S1R = DCMPLX( K1R*KA1R - K1I*KA1I + K2R*KA2R - K2I*KA2I,
     &                K1R*KA1I + K1I*KA1R + K2R*KA2I + K2I*KA2R )
      ELSE
        vbf_wp_wp_S1R = DCMPLX( -K1R*KA1R + K1I*KA1I - K2R*KA2R + K2I*KA2I,
     &                -K1R*KA1I - K1I*KA1R - K2R*KA2I - K2I*KA2R )
      ENDIF
ccc
      RETURN
      END

