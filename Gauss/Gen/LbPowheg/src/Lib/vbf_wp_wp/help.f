c***************************************************************************
      function vbf_wp_wp_dotrr(p1,p2)
c***************************************************************************
c
c     vbf_wp_wp_dotrr(p1,p2) = p1.p2
c
c***************************************************************************
      implicit none

      double precision vbf_wp_wp_dotrr,p1(0:3),p2(0:3)

      vbf_wp_wp_dotrr = p1(0)*p2(0) - p1(1)*p2(1) - p1(2)*p2(2) - p1(3)*p2(3)

      end
      
c ------------------------------------------------------------

      double complex function vbf_wp_wp_dotcc(v1,v2)
      implicit none
      double complex v1(0:3), v2(0:3)
      vbf_wp_wp_dotcc = v1(0)*v2(0)-v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)
      end


      double complex function vbf_wp_wp_dotrc(v1,v2)
      implicit none
      double precision v1(0:3)
      double complex  v2(0:3)
      vbf_wp_wp_dotrc = v1(0)*v2(0)-v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)
      end


      double complex function vbf_wp_wp_dotqj(v2)
      implicit none
      double precision v1(0:3)
      double complex  v2(0:5)
      v1(0) = dreal(v2(4))
      v1(1) = dreal(v2(5))
      v1(2) = dimag(v2(5))
      v1(3) = dimag(v2(4))
      vbf_wp_wp_dotqj = v1(0)*v2(0)-v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)
      end


      double complex function vbf_wp_wp_contract_Tjj(T,j1,j2)
      implicit none
c contract complex rank 2 tensor T^{mu,nu} with two complex vectors j1 and j2
      complex*16 T(0:3,0:3), j1(0:3), j2(0:3), resv(0:3)
      integer mu
      do mu = 0,3
         resv(mu) = T(mu,0)*j2(0) - T(mu,1)*j2(1) 
     &            - T(mu,2)*j2(2) - T(mu,3)*j2(3)
      enddo
      vbf_wp_wp_contract_Tjj = resv(0)*j1(0) - resv(1)*j1(1) 
     &             - resv(2)*j1(2) - resv(3)*j1(3)
      return
      end

      subroutine vbf_wp_wp_contract_T1j(T,jc,jout)
      implicit none
c contract first index of complex rank 2 tensor T^{mu,nu} with complex vector jc
c jout(mu) = T^{nu,mu} jc(nu) 
      complex*16 T(0:3,0:3), jc(0:3), jout(0:3)
      integer mu
      do mu = 0,3
         jout(mu) = T(0,mu)*jc(0) - T(1,mu)*jc(1) 
     &            - T(2,mu)*jc(2) - T(3,mu)*jc(3)
      enddo
      end



      subroutine vbf_wp_wp_contract_T2j(T,jc,jout)
      implicit none
c contract second index of complex rank 2 tensor T^{mu,nu} with complex vector jc
c jout(mu) = T^{mu,nu} jc(nu) 
      complex*16 T(0:3,0:3), jc(0:3), jout(0:3)
      integer mu
      do mu = 0,3
         jout(mu) = T(mu,0)*jc(0) - T(mu,1)*jc(1) 
     &            - T(mu,2)*jc(2) - T(mu,3)*jc(3)
      enddo
      end
      
C-------------------------------------------------------------------

      FUNCTION vbf_wp_wp_SC3(CHII,A1,A2,A3,CHIF,ALPHA)
      IMPLICIT NONE 
      COMPLEX*16 ZI, vbf_wp_wp_SC3
      PARAMETER ( ZI=(0D0,1D0) )
      INTEGER  ALPHA, ALP, N, I
      COMPLEX*16  CHII(2), CHIF(2), CHIAUX(2), CHIDUM, ASLASH(2,2)
      COMPLEX*16  A1(0:3), A3(0:3), AUX(0:3,3)
      REAL*8  A2(0:3)
C
      N = 3
      DO I = 0,3
         AUX(I,3) = A3(I)
         AUX(I,2) = A2(I)
         AUX(I,1) = A1(I)
      ENDDO
      CHIAUX(1) = CHII(1)
      CHIAUX(2) = CHII(2)
      ALP = ALPHA
C
      DO 30 I = 1,N
         IF (ALP.GT.0) THEN
            ASLASH(1,1) = AUX(0,I) - AUX(3,I)
            ASLASH(1,2) = -AUX(1,I) + ZI*AUX(2,I)
            ASLASH(2,1) = -AUX(1,I) - ZI*AUX(2,I)
            ASLASH(2,2) = AUX(0,I) + AUX(3,I)
         ELSE
            ASLASH(1,1) = AUX(0,I) + AUX(3,I)
            ASLASH(1,2) = AUX(1,I) - ZI*AUX(2,I)
            ASLASH(2,1) = AUX(1,I) + ZI*AUX(2,I)
            ASLASH(2,2) = AUX(0,I) - AUX(3,I)
         ENDIF
         CHIDUM = CHIAUX(1)*ASLASH(1,1) + CHIAUX(2)*ASLASH(2,1)
         CHIAUX(2) = CHIAUX(1)*ASLASH(1,2) + CHIAUX(2)*ASLASH(2,2)
         CHIAUX(1) = CHIDUM
         ALP = - ALP
 30   CONTINUE
C
      vbf_wp_wp_SC3  = CHIAUX(1) * CHIF(1) + CHIAUX(2) * CHIF(2)
      RETURN
      END

c
c----------  vbf_wp_wp_boxlinec(isigmax, ... ) ----------------------------------------
c
      subroutine vbf_wp_wp_boxlinec(isigmax, psi1,psi2,k1,k2,
     1                    ld0calc, jext, ivmu, 
     2                    jborn,    jvirt)
      implicit none
      integer isigmax, ivmu
      logical ld0calc
      double precision k1(0:3),k2(0:3)
      double complex psi1(2,-1:1),psi2(2,-1:1), jext(0:5), 
     1               jborn(0:5,-1:1),jvirt(0:5,-1:1)
c
c  variant of the boxline routines which returns the virtual corrections 
c  without contraction of one of the external vbf_wp_wp_gauge bosons, i.e. in form of a
c  current. Also, results for both fermion chiralities are returned for isgmax = +1
c 
c  determine the finite virtual corrections to the fundamental box-type
c  graph, i.e. the sum of all virtual corrections to the vbf_wp_wp_born diagram
c
c    psi1(k1)    ---->---------->-------------->--  psi2(k2)
c                         S              S
c                         S              S
c                         S              S
c                     q1  V           q2 V
c                         S              S
c                         S              S
c
c                        mu           jext(q2)      for ivmu = 1 
c                     jext(q1)          mu          for ivmu = 2
c
c          ====       eps1(q1)        eps2(q2)
c
c   note: k1 = k2+q1+q2, k1.k1 = 0 = k2.k2, q1.q1,q2.q2 arbitrary
c
c  INPUT:      isigmax         -1: only Lefthanded fermions, +1: return L and R
c              psi1(2,-1:1)    the incoming Weyl spinor for both chiralities
c              psi2(2,-1:1)    the outgoing Weyl spinor, psi^+(k2)
c       k1(0:3),k2(0:3)        the corresponding momenta in direction of
c                              the fermion line
c       ld0calc                call vbf_wp_wp_BCD_fill_c if true
c       jext(0:3)              specified polarization vector/current" 
c                              corresponding to 1 external EW boson
c       jext(4:5)              outflowing momenta of this EW bosons
c                              q1(0:3) or q2(0:3) in DHELAS notation
c       jborn(0:5,-1:1)        the current of free index at ivmu for the 
c                              corresponding vbf_wp_wp_born diagram and 2 quark chiralities
c
c  OUTPUT:   jvirt(0:5,-1:1)   the amplitude for the finite part of all 
c                              virtual 1-loop QCD corrections for free index at ivmu
c
      logical ldebug
      parameter (ldebug=.false.)
common block for D_ij etc. information via vbf_wp_wp_BCD_fill_c
      double complex D0c, Dijc(3,13), Teps1c, Teps2c, Tbornc,Tg1c,Tg2c
      double precision sc, tc, q1sqc, q2sqc
      common /vbf_wp_wp_bcd_qqv_c/ sc,tc,q1sqc,q2sqc, D0c,Dijc, 
     1                   Teps1c,Teps2c,Tbornc,Tg1c,Tg2c
C   local variables
      integer mu, nu, isig
      double precision s,t,mom(0:3),vbf_wp_wp_dotp
      external vbf_wp_wp_dotp
      double precision q1(0:4),q2(0:4)
      double complex eps1(0:3),eps2(0:3),mborn,mvirt
      double complex me1, me2, mq, vbf_wp_wp_D0t, vbf_wp_wp_S1C, vbf_wp_wp_S1R, vbf_wp_wp_SC3, z
      external vbf_wp_wp_D0t, vbf_wp_wp_S1C, vbf_wp_wp_S1R, vbf_wp_wp_SC3
      double complex ce1,ce2,cq,cborn
      double complex d0, d11,d12,d13, d21,d22,d23,d24,d25,d26,d27
      double complex d31,d32,d33,d34,d35,d36,d37,d38,d39,d310,
     1               d311,d312,d313
      double complex eps1k1, eps1k2, eps1q2, eps1eps2, 
     1               eps2k1, eps2q1, eps2k2, eps1q1, eps2q2,
     2               gce1, gce2, gcq

c variables for monitoring vbf_wp_wp_gauge invariance:
      double complex eps1g(0:3),eps2g(0:3),mborng,mvirtg
      double complex me1g, me2g, mqg
      double complex ce1g,ce2g,cqg
      double complex eps1gk1, eps1gk2, eps1gq2, eps1geps2g, 
     1               eps2gk1, eps2gq1, eps2gk2, eps1gq1, eps2gq2,
     2               gce1g, gce2g, gcqg
c
      integer icountmax,icountmaxl
c      integer iterations1, iterations2, n2max1, n2max2
      integer lflavr(5:8)
c      character*50 gridname1, gridname2
c      common /vbf_wp_wp_bmainpr/ iterations1, iterations2, n2max1, n2max2,
c     2                 lflavr, gridname1, gridname2
      logical breset
      parameter(breset = .true.)
	
      logical gimprove
      parameter (gimprove =.true.)
      integer bcount
      data bcount / 0/
      integer gcount
      data gcount / 0/

      common/vbf_wp_wp_gauge_checks/icountmax

c transfer momentum information of EW bosons to q1,q2
      if (ivmu.eq.1) then     ! eps2 and q2 are specified, 
                              ! eps1 = mu = free index
         q2(0) = dreal(jext(4))
         q2(1) = dreal(jext(5))
         q2(2) = dimag(jext(5))
         q2(3) = dimag(jext(4))
         do mu = 0,3
            eps2(mu) = jext(mu)
            mom(mu) = k2(mu)+q2(mu)
            q1(mu) = k1(mu)-mom(mu)
         enddo
      elseif (ivmu.eq.2) then     ! eps1 and q1 are specified,  
                                  ! eps2 = mu = free index
         q1(0) = dreal(jext(4))
         q1(1) = dreal(jext(5))
         q1(2) = dimag(jext(5))
         q1(3) = dimag(jext(4))
         do mu = 0,3
            eps1(mu) = jext(mu)
            mom(mu) = k1(mu)-q1(mu)
            q2(mu) = mom(mu)-k2(mu)
         enddo
      endif
      q1(4) = q1(0)**2-q1(1)**2-q1(2)**2-q1(3)**2
      q2(4) = q2(0)**2-q2(1)**2-q2(2)**2-q2(3)**2
c
c for vbf_wp_wp_gauge test:  replace eps(q) by q
      if (ivmu.eq.2) then
	do mu = 0,3
	   eps1g(mu) = q1(mu)	
	   eps2g(mu) = eps2(mu)
	enddo 
      elseif (ivmu.eq.1) then	      
	do mu = 0,3
	   eps2g(mu) = q2(mu)
	   eps1g(mu) = eps1(mu)
	enddo 
      else
        print*,'invalid value for ivmu'	       
      endif	

      if (ld0calc) call vbf_wp_wp_BCD_fill_c(k1,k2,q1,q2)
      d0 = d0c

      d11 = dijc(1,1)
      d12 = dijc(1,2)
      d13 = dijc(1,3)
      
      d21 = dijc(2,1)
      d22 = dijc(2,2)
      d23 = dijc(2,3)
      d24 = dijc(2,4)
      d25 = dijc(2,5)
      d26 = dijc(2,6)
      d27 = dijc(2,7)

      d31 = dijc(3,1)
      d32 = dijc(3,2)
      d33 = dijc(3,3)
      d34 = dijc(3,4)
      d35 = dijc(3,5)
      d36 = dijc(3,6)
      d37 = dijc(3,7)
      d38 = dijc(3,8)
      d39 = dijc(3,9)
      d310 = dijc(3,10)
      d311 = dijc(3,11)
      d312 = dijc(3,12)
      d313 = dijc(3,13)
c
Check consistency of kinematics
      if (ldebug) then
         print*,' '
         print*,' comparison of input to BOXLINE for ivmu = ',ivmu
         print('(a,3f16.4)'),' q1sq = ',q1sqc,q1(4),q1sqc/q1(4)
         print('(a,3f16.4)'),' q2sq = ',q2sqc,q2(4),q2sqc/q2(4)
         s = -2*vbf_wp_wp_dotp(k1,k2)
         t =  2*vbf_wp_wp_dotp(k2,q2)+q2(4)
         print('(a,3f16.4)'),' s = ',sc,s,sc/s
         print('(a,3f16.4)'),' t = ',tc,t,tc/t
c         do mu = 0,3
c            if (abs(k1(mu)-ak1(mu,id)).gt.(1d-10*k1(0)) .or.
c     1          abs(k2(mu)-ak2(mu,id)).gt.(1d-10*k2(0)) .or.
c     1          abs(q1(mu)-aq1(mu,id)).gt.(1d-10*k2(0)) .or.
c     1          abs(q2(mu)-aq2(mu,id)).gt.(1d-10*k2(0)) ) then
c               print*,' WARNING: momentum mismatch in boxline, mu =',mu
c               print*,' k1: ',k1(mu)/(ak1(mu,id)+1d-80)
c               print*,' k2: ',k2(mu)/(ak2(mu,id)+1d-80)
c               print*,' q1: ',q1(mu)/(aq1(mu,id)+1d-80)
c               print*,' q2: ',q2(mu)/(aq2(mu,id)+1d-80)
c            endif
c         enddo
         print*,' input psi1 ',psi1
         print*,' input psi2 ',psi2
         print*,' input jext ',jext
      endif

c  get matrix elements 
c
c  me1 = psibar(2) eps1slash psi(1)
c  me2 = psibar(2) eps2slash psi(1)
c  mq  = psibar(2) (q1-q2)slash psi(1)
      do mu = 0,3
         if (ivmu.eq.1) then
            do nu = 0,3
               eps1(nu)  = 0d0
               eps1g(nu) = 0d0
            enddo
            eps1(mu)  = -1d0
            eps1g(mu) = -1d0
            if (mu.eq.0) then
	    	eps1(mu)  = 1d0
	    	eps1g(mu) = 1d0
	    endif	
         elseif (ivmu.eq.2) then
            do nu = 0,3
               eps2(nu)  = 0d0
               eps2g(nu) = 0d0
            enddo
            eps2(mu) =  -1d0
            eps2g(mu) = -1d0
            if (mu.eq.0) then
	        eps2(mu)  = 1d0
	        eps2g(mu) = 1d0
	    endif	
         endif
	 
         do isig = -1,isigmax,2
            
            me1 = vbf_wp_wp_S1C(psi2(1,isig),eps1,.true.,isig,psi1(1,isig))
            me2 = vbf_wp_wp_S1C(psi2(1,isig),eps2,.true.,isig,psi1(1,isig))
            mq  = 2*vbf_wp_wp_S1R(psi2(1,isig),q1,.true.,isig,psi1(1,isig))
            mborn = jborn(mu,isig)
            	    
            if (ldebug) then
               print*,' check amplitudes for mu =',mu,' isig =',isig
               print*,' check M_q: ',
     1         -mq/vbf_wp_wp_S1R(psi2(1,isig),q2,.true.,isig,psi1(1,isig)),
     1         ' = 2?'
               t = mom(0)**2-mom(1)**2-mom(2)**2-mom(3)**2
               z = vbf_wp_wp_SC3(psi2(1,isig),eps2,mom,eps1,psi1(1,isig),isig)/t
               print*,' M_born:    ',mborn/z,' = 1? Abs(ratio) = ',
     &              dreal(mborn/z),abs(mborn/z)
            endif
c
c prepare the coefficients
            s = sc
            t = tc

      eps1k1 = eps1(0)*k1(0)-eps1(1)*k1(1)-eps1(2)*k1(2)-eps1(3)*k1(3)
      eps1k2 = eps1(0)*k2(0)-eps1(1)*k2(1)-eps1(2)*k2(2)-eps1(3)*k2(3)
      eps1q2 = eps1(0)*q2(0)-eps1(1)*q2(1)-eps1(2)*q2(2)-eps1(3)*q2(3)
      eps1eps2 = eps1(0)*eps2(0)-eps1(1)*eps2(1)-
     1           eps1(2)*eps2(2)-eps1(3)*eps2(3)
      eps2k1 = eps2(0)*k1(0)-eps2(1)*k1(1)-eps2(2)*k1(2)-eps2(3)*k1(3)
      eps2k2 = eps2(0)*k2(0)-eps2(1)*k2(1)-eps2(2)*k2(2)-eps2(3)*k2(3)
      eps2q1 = eps2(0)*q1(0)-eps2(1)*q1(1)-eps2(2)*q1(2)-eps2(3)*q1(3)

      eps1q1 = eps1(0)*q1(0)-eps1(1)*q1(1)-eps1(2)*q1(2)-eps1(3)*q1(3)
      eps2q2 = eps2(0)*q2(0)-eps2(1)*q2(1)-eps2(2)*q2(2)-eps2(3)*q2(3)
c      
c
      ce1 = -2*(d12+d24)*eps2k2*(q1sqc+q2sqc-3*s-4*t) - 
     1      2*(d12-d24)*eps2q1*(q2sqc-t) 
     2  +4*(- d11*eps2k2*s - d12*eps2k1*t + d13*eps2k2*
     3  (q2sqc-s-t) + d13*eps2q1*q2sqc - d21*eps2k2*s 
     4 - d22*eps2k2*t - d22*eps2q1*q2sqc + d23*eps2k2*q1sqc
     5 + d25*eps2k2*(q2sqc-s-2*t) 
     6 - d26*eps2k2*(q2sqc-s-t)+d26*eps2q1*t+2*d27*eps2q1
     7 - d32*eps2k2*q2sqc - d34*eps2k2*(q2sqc-t) 
     8 + d35*eps2k2*(q2sqc-s-t)+d36*eps2k2*(2*q2sqc-t) 
     9 + d37*eps2k2*q1sqc + d38*eps2k2*(q1sqc+q2sqc-s)
     & - d39*eps2k2*q1sqc 
     1 - d310*eps2k2*(q1sqc+2*q2sqc-2*s-t)
     2 - 4*d311*eps2k2 + 6*d312*eps2k2 + 2*d313*eps2q1)

      ce2= -2*(d12+d24)*(eps1k2*(q1sqc+q2sqc-s-2*t)
     1  +eps1q2*(q2sqc-s-3*t)) + 
     2  4*(d13*eps1k2*(q1sqc-2*s-t) - 
     3  d13*eps1q2*(2*s+t) + d22*eps1k2*q2sqc
     4  + d22*eps1q2*q2sqc - d23*eps1k2*t + 
     5    d23*eps1q2*(q1sqc-t) - d24*eps1k1*q2sqc 
     6  + d25*eps1k2*q1sqc + d25*eps1k1*(q2sqc-2*s-t) 
     7  + d26*eps1k2*t - d26*eps1k1*(q1sqc-s) - 
     8  2*d27*eps1q2 + d33*eps1k2*q1sqc + d33*eps1q2*q1sqc
     9  + d37*eps1k1*(q2sqc-s-t) + d38*eps1k1*q2sqc
     &  - d39*eps1k1*(q1sqc+q2sqc-s) - d310*eps1k1*(q2sqc-t)
     1  +2*d311*eps1k2+2*d312*eps1q2-6*d313*eps1k1)

C NOTE: cq = 1/2 * cq1 in my reduce code
      cq = (d12+d24)*eps1eps2*s + 2*(4*d12*eps2k2*eps1k2 + 
     1 3*d12*eps2k2*eps1q2+d12*eps2q1*eps1k2-4*d13*eps2k2*eps1k2-
     2 2*d13*eps2k2*eps1q2-2*d13*eps2q1*eps1k2-d13*eps1eps2*s+
     3 2*d22*eps2k2*eps1q2-d22*eps1eps2*t-2*d23*eps2q1*eps1k2-
     4 2*d23*eps2q1*eps1q2-d23*eps1eps2*t+6*d24*eps2k2*eps1k2+
     5 3*d24*eps2k2*eps1q2+d24*eps2q1*eps1k2-6*d25*eps2k2*eps1k2-
     6 2*d25*eps2k2*eps1q2-2*d25*eps2q1*eps1k2-d25*eps1eps2*s-
     7 4*d26*eps2k2*eps1q2+4*d26*eps2q1*eps1k2+2*d26*eps2q1*eps1q2+ 
     8 d26*eps1eps2*(s+2*t)-d32*eps1eps2*q2sqc+
     9 d33*eps1eps2*q1sqc+2*d34*eps2k2*eps1k2-2*d35*eps2k2*eps1k2+
     & 2*d36*eps2k2*eps1q2+d36*eps1eps2*(q2sqc-t)-
     1 2*d37*eps2q1*eps1k2+d37*eps1eps2*(q2sqc-s-t)+
     2 2*d38*eps2q1*eps1q2+d38*eps1eps2*(q1sqc+2*q2sqc-s) -
     3 2*d39*eps2q1*eps1q2-d39*eps1eps2*(2*q1sqc+q2sqc-s) -
     4 2*d310*eps2k2*eps1q2+2*d310*eps2q1*eps1k2-d310*eps1eps2*
     5 (2*q2sqc-s-2*t)+4*d312*eps1eps2-4*d313*eps1eps2)

      cborn = -2*((d36+d37-2*d310)*(q2sqc-t) + 
     1    d38*(q1sqc+2*q2sqc) - d39*(2*q1sqc+q2sqc) )
     2    -2*(d0+d11+d12-2*d13+d24-d25+d26-d37-d38+d39+d310)*s
     3    -2*(-(d22+d23-2*d26)*t + 2*d27 - d32*q2sqc
     4    +d33*q1sqc+ 6*(d312-d313))

      gce1 = -2*eps2q2*( d12*(2*q1sqc+q2sqc-s-3*t) - 
     1       2*(d13-d22)*(q1sqc+q2sqc-s-t)
     2     - d24*(q2sqc-s-t)-2*d26*q1sqc-4*d27-4*d312)

      gce2 = 2*eps1q1*(d12*(q2sqc+t)-2*d13*(q2sqc+s) + 
     1      2*d22*q2sqc + 2*d23*(q1sqc+q2sqc-s-2*t)
     2     +d24*(q2sqc+t)+2*d25*(-q2sqc+s+t)-2*d26*(q2sqc-t)
     3     -4*d27+2*d33*q1sqc + 4*d313)

      gcq = 4*eps1q1*eps2q2*(d12-d13+d22-d23+d38-d39) +
     1        2*eps1q1*eps2k2*(d12-4*d23+d24+2*d26-2*d37+2*d310) -
     2        4*eps1q1*eps2q1*(d23-d26+d33-d39) +
     3        2*eps2q2*eps1k2*(3*d12-4*d13+4*d22+d24-2*d25-
     4                                     2*d26+2*d36-2*d310) +
     5        4*eps2q2*eps1q2*(d12-d13+2*d22-2*d26+d32-d38)
     
            ce1 = ce1 + gce1
            ce2 = ce2 + gce2
            cq = cq + gcq
c
c  now add the terms from the vertex and propagator corrections
            ce1 = ce1 + 2*eps2k2*Teps2c + 2*eps2q2*Tg2c
            ce2 = ce2 + 2*eps1k1*Teps1c - 2*eps1q1*Tg1c
            cborn = cborn*t - Tbornc
c  and the final result
            mvirt = me1*ce1 + me2*ce2 + mq*cq + mborn*cborn
c
c -------------------------------------------
c
c 'same' for vbf_wp_wp_gauge check: 
c
      if (gimprove) then      
     	
       t = mom(0)**2-mom(1)**2-mom(2)**2-mom(3)**2
     
       me1g = vbf_wp_wp_S1C(psi2(1,isig),eps1g,.true.,isig,psi1(1,isig))
       me2g = vbf_wp_wp_S1C(psi2(1,isig),eps2g,.true.,isig,psi1(1,isig))
       mqg = mq
       mborng = vbf_wp_wp_SC3(psi2(1,isig),eps2g,mom,eps1g,psi1(1,isig),isig)/t	       
      
       eps1gk1 = eps1g(0)*k1(0)-eps1g(1)*k1(1)-
     %           eps1g(2)*k1(2)-eps1g(3)*k1(3)
       eps1gk2 = eps1g(0)*k2(0)-eps1g(1)*k2(1)-
     %           eps1g(2)*k2(2)-eps1g(3)*k2(3)
       eps1gq2 = eps1g(0)*q2(0)-eps1g(1)*q2(1)-
     %           eps1g(2)*q2(2)-eps1g(3)*q2(3)
       eps1geps2g = eps1g(0)*eps2g(0)-eps1g(1)*eps2g(1)-
     %              eps1g(2)*eps2g(2)-eps1g(3)*eps2g(3)
       eps2gk1 = eps2g(0)*k1(0)-eps2g(1)*k1(1)-
     %           eps2g(2)*k1(2)-eps2g(3)*k1(3)
       eps2gk2 = eps2g(0)*k2(0)-eps2g(1)*k2(1)-
     %           eps2g(2)*k2(2)-eps2g(3)*k2(3)
       eps2gq1 = eps2g(0)*q1(0)-eps2g(1)*q1(1)-
     %           eps2g(2)*q1(2)-eps2g(3)*q1(3)

       eps1gq1 = eps1g(0)*q1(0)-eps1g(1)*q1(1)-
     %           eps1g(2)*q1(2)-eps1g(3)*q1(3)
       eps2gq2 = eps2g(0)*q2(0)-eps2g(1)*q2(1)-
     %           eps2g(2)*q2(2)-eps2g(3)*q2(3)
c
       ce1g = -2*(d12+d24)*eps2gk2*(q1sqc+q2sqc-3*s-4*t) - 
     1      2*(d12-d24)*eps2gq1*(q2sqc-t) 
     2  +4*(- d11*eps2gk2*s - d12*eps2gk1*t + d13*eps2gk2*
     3  (q2sqc-s-t) + d13*eps2gq1*q2sqc - d21*eps2gk2*s 
     4 - d22*eps2gk2*t - d22*eps2gq1*q2sqc + d23*eps2gk2*q1sqc
     5 + d25*eps2gk2*(q2sqc-s-2*t) 
     6 - d26*eps2gk2*(q2sqc-s-t)+d26*eps2gq1*t+2*d27*eps2gq1
     7 - d32*eps2gk2*q2sqc - d34*eps2gk2*(q2sqc-t) 
     8 + d35*eps2gk2*(q2sqc-s-t)+d36*eps2gk2*(2*q2sqc-t) 
     9 + d37*eps2gk2*q1sqc + d38*eps2gk2*(q1sqc+q2sqc-s)
     & - d39*eps2gk2*q1sqc 
     1 - d310*eps2gk2*(q1sqc+2*q2sqc-2*s-t)
     2 - 4*d311*eps2gk2 + 6*d312*eps2gk2 + 2*d313*eps2gq1)

       ce2g= -2*(d12+d24)*(eps1gk2*(q1sqc+q2sqc-s-2*t)
     1  +eps1gq2*(q2sqc-s-3*t)) + 
     2  4*(d13*eps1gk2*(q1sqc-2*s-t) - 
     3  d13*eps1gq2*(2*s+t) + d22*eps1gk2*q2sqc
     4  + d22*eps1gq2*q2sqc - d23*eps1gk2*t + 
     5    d23*eps1gq2*(q1sqc-t) - d24*eps1gk1*q2sqc 
     6  + d25*eps1gk2*q1sqc + d25*eps1gk1*(q2sqc-2*s-t) 
     7  + d26*eps1gk2*t - d26*eps1gk1*(q1sqc-s) - 
     8  2*d27*eps1gq2 + d33*eps1gk2*q1sqc + d33*eps1gq2*q1sqc
     9  + d37*eps1gk1*(q2sqc-s-t) + d38*eps1gk1*q2sqc
     &  - d39*eps1gk1*(q1sqc+q2sqc-s) - d310*eps1gk1*(q2sqc-t)
     1  +2*d311*eps1gk2+2*d312*eps1gq2-6*d313*eps1gk1)

C NOTE: cq = 1/2 * cq1 in my reduce code
       cqg = (d12+d24)*eps1geps2g*s + 2*(4*d12*eps2gk2*eps1gk2 + 
     1 3*d12*eps2gk2*eps1gq2+d12*eps2gq1*eps1gk2-4*d13*eps2gk2*eps1gk2-
     2 2*d13*eps2gk2*eps1gq2-2*d13*eps2gq1*eps1gk2-d13*eps1geps2g*s+
     3 2*d22*eps2gk2*eps1gq2-d22*eps1geps2g*t-2*d23*eps2gq1*eps1gk2-
     4 2*d23*eps2gq1*eps1gq2-d23*eps1geps2g*t+6*d24*eps2gk2*eps1gk2+
     5 3*d24*eps2gk2*eps1gq2+d24*eps2gq1*eps1gk2-6*d25*eps2gk2*eps1gk2-
     6 2*d25*eps2gk2*eps1gq2-2*d25*eps2gq1*eps1gk2-d25*eps1geps2g*s-
     7 4*d26*eps2gk2*eps1gq2+4*d26*eps2gq1*eps1gk2+2*d26*eps2gq1*eps1gq2+ 
     8 d26*eps1geps2g*(s+2*t)-d32*eps1geps2g*q2sqc+
     9 d33*eps1geps2g*q1sqc+2*d34*eps2gk2*eps1gk2-2*d35*eps2gk2*eps1gk2+
     & 2*d36*eps2gk2*eps1gq2+d36*eps1geps2g*(q2sqc-t)-
     1 2*d37*eps2gq1*eps1gk2+d37*eps1geps2g*(q2sqc-s-t)+
     2 2*d38*eps2gq1*eps1gq2+d38*eps1geps2g*(q1sqc+2*q2sqc-s) -
     3 2*d39*eps2gq1*eps1gq2-d39*eps1geps2g*(2*q1sqc+q2sqc-s) -
     4 2*d310*eps2gk2*eps1gq2+2*d310*eps2gq1*eps1gk2-d310*eps1geps2g*
     5 (2*q2sqc-s-2*t)+4*d312*eps1geps2g-4*d313*eps1geps2g)
      

       gce1g = -2*eps2gq2*( d12*(2*q1sqc+q2sqc-s-3*t) - 
     1       2*(d13-d22)*(q1sqc+q2sqc-s-t)
     2     - d24*(q2sqc-s-t)-2*d26*q1sqc-4*d27-4*d312)

       gce2g = 2*eps1gq1*(d12*(q2sqc+t)-2*d13*(q2sqc+s) + 
     1      2*d22*q2sqc + 2*d23*(q1sqc+q2sqc-s-2*t)
     2     +d24*(q2sqc+t)+2*d25*(-q2sqc+s+t)-2*d26*(q2sqc-t)
     3     -4*d27+2*d33*q1sqc + 4*d313)

       gcqg = 4*eps1gq1*eps2gq2*(d12-d13+d22-d23+d38-d39) +
     1        2*eps1gq1*eps2gk2*(d12-4*d23+d24+2*d26-2*d37+2*d310) -
     2        4*eps1gq1*eps2gq1*(d23-d26+d33-d39) +
     3        2*eps2gq2*eps1gk2*(3*d12-4*d13+4*d22+d24-2*d25-
     4                                     2*d26+2*d36-2*d310) +
     5        4*eps2gq2*eps1gq2*(d12-d13+2*d22-2*d26+d32-d38)
     
            ce1g = ce1g + gce1g
            ce2g = ce2g + gce2g
            cqg  = cqg + gcqg
c
c  now add the terms from the vertex and propagator corrections

            ce1g = ce1g + 2*eps2gk2*Teps2c  + 2*eps2gq2*Tg2c
            ce2g = ce2g + 2*eps1gk1*Teps1c  - 2*eps1gq1*Tg1c
c  and the final result
            mvirtg = me1g*ce1g + me2g*ce2g + mqg*cqg + mborng*cborn
	    
	endif !gimprove
	
c -------------------------------
	    
       if (gimprove) then
       bcount = bcount+1

       icountmaxl = 100*icountmax
       if(icountmaxl.lt.999) icountmaxl = 1000
	
        if (abs(mvirtg)/abs(mborn).ge.1e-3) then		
          if ((abs((mvirtg))/(abs(me1g*ce1g)+abs(mqg*cqg)+
     #  		     abs(me2g*ce2g)+abs(mborng*cborn)).ge.1e-1)
     #		.or.(abs(mvirtg).ge.1e3))	 then
        
              gcount = gcount+1 		      
              mvirt = 0d0  !disregard this contribution
          
          endif       
        endif

c        if (mod(bcount,icountmaxl).eq.0 ) then
c          print*,'box vbf_wp_wp_gauge check counting ratio:',real(gcount)/real(bcount)
c        endif
	
       if(breset) then
	 if (bcount .eq. 1000000000 ) then !integer range exceeded -> reset
c	     print*,'gcount,bcount = ', gcount,bcount,' are reset now'
	     bcount = 0
	     gcount = 0
	 endif
       endif	  

       endif !gimprove

c ---------------------------------------------------------	   
	
            jvirt(mu,isig) = mvirt
         enddo !isig
      enddo !mu
            
      return
      end


c---------------------------------------------------------------------

      subroutine  vbf_wp_wp_J_virtual_tri_box_pent1(psi,p,nq1,nq2,sign, musq,
c     input currents (any ORDERED couple among [J1,J2,J3]) in DHELAS format
c     They can be  jmu1,jmu2  or  jmu1,jmu3   or   jmu2,jmu3
     #     jal,jbe,
c     uncontracted external current. It can be 1, 2 or 3
     #     num_ret_current,
c     the corresponding vbf_wp_wp_born current
c           Jborn,
c     returned current
     #     Jret)

      implicit none
      complex * 16 psi(2,-1:1,4)
      real * 8 p(0:3,4),q(0:3),l(0:3),t(0:3)
      complex * 16 psi_loc(2,-1:1,4)
      real*8 ploc(0:3,4)
      real * 8 musq
      complex * 16 jal(0:5),jbe(0:5)
      integer num_ret_current, nq1,nq2,nq3,nq4, sign(4)
      complex * 16 jborn(0:3), jret(0:3)
      logical lpr
      common /vbf_wp_wp_e0print/lpr

      integer mu
c map input momenta onto incoming standard and make use of 
c DHELAS information in currents
      if (nq1.eq.1 .and. nq2.eq.2) then
         nq3 = 3
         nq4 = 4
      elseif (nq1.eq.3 .and. nq2.eq.4) then
         nq3 = 1
         nq4 = 2
      else
         print*,' check nq1 etc. in vbf_wp_wp_J_virtual_tri_box_pent1 call '
         print*,nq1,nq2,nq3,nq4
         stop
      endif
         
      if (num_ret_current.eq.1) then     ! p3=q+p4 for q incoming on 12 line
         do mu = 0,3
            q(mu) = p(mu,nq3)-p(mu,nq4)
         enddo
         l(0) = -dreal(jal(4))
         l(1) = -dreal(jal(5))
         l(2) = -dimag(jal(5))
         l(3) = -dimag(jal(4))
         t(0) = -dreal(jbe(4))
         t(1) = -dreal(jbe(5))
         t(2) = -dimag(jbe(5))
         t(3) = -dimag(jbe(4))
      elseif (num_ret_current.eq.2) then
         do mu = 0,3
            l(mu) = p(mu,nq3)-p(mu,nq4)
         enddo
         q(0) = -dreal(jal(4))
         q(1) = -dreal(jal(5))
         q(2) = -dimag(jal(5))
         q(3) = -dimag(jal(4))
         t(0) = -dreal(jbe(4))
         t(1) = -dreal(jbe(5))
         t(2) = -dimag(jbe(5))
         t(3) = -dimag(jbe(4))
      elseif (num_ret_current.eq.3) then
         do mu = 0,3
            t(mu) = p(mu,nq3)-p(mu,nq4)
         enddo
         q(0) = -dreal(jal(4))
         q(1) = -dreal(jal(5))
         q(2) = -dimag(jal(5))
         q(3) = -dimag(jal(4))
         l(0) = -dreal(jbe(4))
         l(1) = -dreal(jbe(5))
         l(2) = -dimag(jbe(5))
         l(3) = -dimag(jbe(4))
      endif

      do mu = 0,3
         jret(mu) = 0
      enddo

      call  vbf_wp_wp_J_virtual_tri_box_pent (
c     vbf_wp_wp_bra(p'), vbf_wp_wp_ket(p) and helicity 
     #     psi(1,-1,nq2),psi(1,-1,nq1),-1,
c     momenta and squared renormalization scale
     #     p(0,nq1),q,l,t,p(0,nq2),musq,
c     input currents (any ORDERED couple among [J1,J2,J3])
c     They can be  jmu1,jmu2  or  jmu1,jmu3   or   jmu2,jmu3
     #     jal,jbe,
c     uncontracted external current. It can be 1, 2 or 3
     #     num_ret_current,
c     returned current
     #     Jret)

      lpr = .false.
      if (lpr) then
         print*,' input momenta for above: p,q,l,t,pp '
         call write_mom(p(0,nq1))
         call write_mom(q)
         call write_mom(l)
         call write_mom(t)
         call write_mom(p(0,nq2))
         print*,' '
      endif

c check momentum conservation and subtract vbf_wp_wp_born contribution
      do mu = 0,3
         t(mu) = q(mu)+l(mu)+t(mu)+p(mu,nq1)-p(mu,nq2) 
         if (abs(t(mu)).gt.1d-8) then
            print*,' no momentum conservation in vbf_wp_wp_J_virtual_tri_box_pent'
            print*,' mom diff(',mu,') = ',t(mu)
         endif
      enddo
      return
      end



      subroutine vbf_wp_wp_jtest(jin,jout)
      implicit none
      double complex jin(0:5), jout(0:5), x, jq
      real*8 q(0:3), qq, mi, mo
      integer mu

      q(0) = dreal(jin(4))
      q(1) = dreal(jin(5))
      q(2) = dimag(jin(5))
      q(3) = dimag(jin(4))
      jq = jin(0)*q(0)+ jin(1)*q(1)+ jin(2)*q(2)+ jin(3)*q(3)
      qq = q(0)*q(0)+ q(1)*q(1)+ q(2)*q(2)+ q(3)*q(3)
      X = jq/qq
      mi = 0
      mo = 0
      do mu = 0,3
         jout(mu) = jin(mu) - x*q(mu)
         mi = max(mi,abs(jin(mu)))
         mo = max(mo,abs(jout(mu)))
         print*,' mu = ',mu,': ',jout(mu),abs(jout(mu)/jin(mu))
      enddo
      print*,' ratio = ',mo/mi
      jout(4) = jin(4)
      jout(5) = jin(5)

      end

c
c----------  vbf_wp_wp_boxline6(id, ... ) ----------------------------------------
c
      subroutine vbf_wp_wp_boxline6(id, psi1,psi2,k1,k2, isig,
     1                    eps1,eps2,
     2                    mborn,    mvirt)
      implicit none
      integer id, isig
      double precision k1(0:3),k2(0:3), q1(0:4),q2(0:4)
      double complex psi1(2),psi2(2),eps1(0:5),eps2(0:5),mborn,mvirt
c 
c  determine the finite virtual corrections to the fundamental box-type
c  graph, i.e. the sum of all virtual corrections to the vbf_wp_wp_born diagram
c
c    psi1(k1)    ---->---------->-------------->--  psi2(k2)
c                         S              S
c                         S              S
c                         S              S
c                     q1  V           q2 V
c                         S              S
c                         S              S
c
c                      eps1(q1)       eps2(q2)
c
c   note: k1 = k2+q1+q2, k1.k1 = 0 = k2.k2, q1.q1,q2.q2 arbitrary
c
c  INPUT:      id = 1...8      identifier for graph to determine 
c                              correct D_ij function etc., see vbf_wp_wp_BCD_fill
c              psi1(2)         the incoming Weyl spinor
c              psi2(2)         the outgoing Weyl spinor, psi^+(k2)
c       k1(0:3),k2(0:3)        the corresponding momenta in direction of
c                              the fermion line
c              isig            chirality of Weyl fermion (=tau in HZ notation)
c       eps1,eps2(0:3)         the "polarization vectors/currents" 
c                              corresponding to the 2 external EW bosons
c       eps1,eps2(4:5)         outflowing momenta of EW bosons =
c                              q1(0:3),q2(0:3) in DHELAS notation
c              mborn           the amplitude for the corresponding vbf_wp_wp_born diagram
c
c  OUTPUT:     mvirt           the amplitude for the finite part of all 
c                              virtual 1-loop QCD corrections
c
Common block for D_ij etc. information, calculated in vbf_wp_wp_BCD_fill
      double complex D0v(8), Dijv(3,13,8), Teps1(8), Teps2(8), Tborn(8),
     1               Tg1(8), Tg2(8)
      double precision ss(8), tt(8), q1sq(8), q2sq(8)
      double precision aq1(0:3,8), aq2(0:3,8), ak1(0:3,8), ak2(0:3,8)
c      common /vbf_wp_wp_bcd_qqv/ ss,tt,q1sq,q2sq, D0v,Dijv, Teps1,Teps2,Tborn,
c     1                                            Tg1,Tg2
      common /vbf_wp_wp_bcd_qqv_aux/ aq1,aq2,ak1,ak2
Common block vbf_wp_wp_bcd_qqv_aux is used for debugging purposes only
      logical ldebug, gaugecheck, loutput, lwarn
      parameter (ldebug=.false., loutput = .false., lwarn = .true.)
      character text
common block for debugging via vbf_wp_wp_BCD_fill_c
      double complex D0c, Dijc(3,13), Teps1c, Teps2c, Tbornc,Tg1c,Tg2c
      double precision sc, tc, q1sqc, q2sqc
      common /vbf_wp_wp_bcd_qqv_c/ sc,tc,q1sqc,q2sqc, D0c,Dijc, 
     1                   Teps1c,Teps2c,Tbornc,Tg1c,Tg2c
C   local variables
      integer mu
      double precision s,t,mom(0:3),vbf_wp_wp_dotp
      external vbf_wp_wp_dotp
      double complex me1, me2, mq, vbf_wp_wp_D0t, vbf_wp_wp_S1C, vbf_wp_wp_S1R, vbf_wp_wp_SC3, z
      external vbf_wp_wp_D0t, vbf_wp_wp_S1C, vbf_wp_wp_S1R, vbf_wp_wp_SC3
      double complex ce1,ce2,cq,cborn
      double complex d0, d11,d12,d13, d21,d22,d23,d24,d25,d26,d27
      double complex d31,d32,d33,d34,d35,d36,d37,d38,d39,d310,
     1               d311,d312,d313
      double complex eps1k1, eps1k2, eps1q2, eps1eps2, 
     1               eps2k1, eps2q1, eps2k2, eps1q1, eps2q2,
     2               gce1, gce2, gcq

c transfer momentum information of EW bosons to q1,q2

      q1(0) = dreal(eps1(4))
      q1(1) = dreal(eps1(5))
      q1(2) = dimag(eps1(5))
      q1(3) = dimag(eps1(4))
      q1(4) = q1(0)**2-q1(1)**2-q1(2)**2-q1(3)**2
      q2(0) = dreal(eps2(4))
      q2(1) = dreal(eps2(5))
      q2(2) = dimag(eps2(5))
      q2(3) = dimag(eps2(4))
      q2(4) = q2(0)**2-q2(1)**2-q2(2)**2-q2(3)**2
      if (id.eq.-1) then
         do mu = 0,3
            q1(mu) = - q1(mu)
            if(ldebug) print*,' mu = ',mu,(k2(mu)+q1(mu)+q2(mu)),(k1(mu)+1d-33)
         enddo
      elseif (id.eq.-2) then
         do mu = 0,3
            q2(mu) = - q2(mu)
            if(ldebug) print*,' mu = ',mu,(k2(mu)+q1(mu)+q2(mu)),(k1(mu)+1d-33)
         enddo
      endif
      do mu = 0,3
         if (abs(k2(mu)+q1(mu)+q2(mu)-k1(mu)).gt.1d-5) then
            print*,' mu = ',mu,':',k2(mu)+q1(mu)+q2(mu)-k1(mu)
            print*,k1(mu),k2(mu),q1(mu),q2(mu)
         endif
      enddo
      id = 1

      call vbf_wp_wp_BCD_fill_c(k1,k2,q1,q2)
      ss(id) = sc
      tt(id) = tc
      q1sq(id) = q1sqc
      q2sq(id) = q2sqc
      Teps1(id) =Teps1c
      Teps2(id) =Teps2c
      Tborn(id) =Tbornc
      Tg1(id) =  Tg1c 
      Tg2(id) =  Tg2c 

Check consistency of kinematics
      if (ldebug) then
         print*,' '
         print*,' comparison of input to BOXLINE for id = ',id
         print('(a,3f16.4)'),' q1sq = ',q1sq(id),q1(4),q1sq(id)/q1(4)
         print('(a,3f16.4)'),' q2sq = ',q2sq(id),q2(4),q2sq(id)/q2(4)
         s = -2*vbf_wp_wp_dotp(k1,k2)
         t =  2*vbf_wp_wp_dotp(k2,q2)+q2(4)
         print('(a,3f16.4)'),' s = ',ss(id),s,ss(id)/s
         print('(a,3f16.4)'),' t = ',tt(id),t,tt(id)/t
c         do mu = 0,3
c            if (abs(k1(mu)-ak1(mu,id)).gt.(1d-10*k1(0)) .or.
c     1          abs(k2(mu)-ak2(mu,id)).gt.(1d-10*k2(0)) .or.
c     1          abs(q1(mu)-aq1(mu,id)).gt.(1d-10*k2(0)) .or.
c     1          abs(q2(mu)-aq2(mu,id)).gt.(1d-10*k2(0)) ) then
c               print*,' WARNING: momentum mismatch in boxline, mu =',mu
c               print*,' k1: ',k1(mu)/(ak1(mu,id)+1d-80)
c               print*,' k2: ',k2(mu)/(ak2(mu,id)+1d-80)
c               print*,' q1: ',q1(mu)/(aq1(mu,id)+1d-80)
c               print*,' q2: ',q2(mu)/(aq2(mu,id)+1d-80)
c            endif
c         enddo
         print*,' input psi1 ',psi1
         print*,' input psi2 ',psi2
         print*,' input eps1 ',eps1
         print*,' input eps2 ',eps2
      endif
           

c  get matrix elements 
c
c  me1 = psibar(2) eps1slash psi(1)
c  me2 = psibar(2) eps2slash psi(1)
c  mq  = psibar(2) (q1-q2)slash psi(1)
      me1 = -vbf_wp_wp_S1C(psi2,eps1,.true.,isig,psi1)
      me2 = -vbf_wp_wp_S1C(psi2,eps2,.true.,isig,psi1)
      mq  = -2*vbf_wp_wp_S1R(psi2,q1,.true.,isig,psi1)
      if (ldebug) then
         print*,' check M_q: ',mq/vbf_wp_wp_S1R(psi2,q2,.true.,isig,psi1),
     1          ' = 2?'
         do mu = 0,3
            mom(mu) = k2(mu)+q2(mu)
         enddo
         t = mom(0)**2-mom(1)**2-mom(2)**2-mom(3)**2
         z = -vbf_wp_wp_SC3(psi2,eps2,mom,eps1,psi1,isig)/t
c         print*,' M_born: 1=?   ',mborn/z,' Re(ratio) = ',
c     &                     dreal(mborn/z)
         mborn = z
      endif
      do mu = 0,3
         mom(mu) = k2(mu)+q2(mu)
      enddo
      t = mom(0)**2-mom(1)**2-mom(2)**2-mom(3)**2
      mborn = -vbf_wp_wp_SC3(psi2,eps2,mom,eps1,psi1,isig)/t
c
c prepare the coefficients
      s = ss(id)
      t = tt(id)

      eps1k1 = eps1(0)*k1(0)-eps1(1)*k1(1)-eps1(2)*k1(2)-eps1(3)*k1(3)
      eps1k2 = eps1(0)*k2(0)-eps1(1)*k2(1)-eps1(2)*k2(2)-eps1(3)*k2(3)
      eps1q2 = eps1(0)*q2(0)-eps1(1)*q2(1)-eps1(2)*q2(2)-eps1(3)*q2(3)
      eps1eps2 = eps1(0)*eps2(0)-eps1(1)*eps2(1)-
     1           eps1(2)*eps2(2)-eps1(3)*eps2(3)
      eps2k1 = eps2(0)*k1(0)-eps2(1)*k1(1)-eps2(2)*k1(2)-eps2(3)*k1(3)
      eps2k2 = eps2(0)*k2(0)-eps2(1)*k2(1)-eps2(2)*k2(2)-eps2(3)*k2(3)
      eps2q1 = eps2(0)*q1(0)-eps2(1)*q1(1)-eps2(2)*q1(2)-eps2(3)*q1(3)

      eps1q1 = eps1(0)*q1(0)-eps1(1)*q1(1)-eps1(2)*q1(2)-eps1(3)*q1(3)
      eps2q2 = eps2(0)*q2(0)-eps2(1)*q2(1)-eps2(2)*q2(2)-eps2(3)*q2(3)

      gaugecheck = abs(eps1q1).gt.1d-10*abs(eps1q2) .or.
     1             abs(eps2q2).gt.1d-10*abs(eps2q1)
      if (ldebug) print*,' gaugecheck = ',gaugecheck
      gaugecheck = .true. !gaugecheck .or. ldebug
      
      d0 = d0c
      
      d11 = dijc(1,1)
      d12 = dijc(1,2)
      d13 = dijc(1,3)
         
      d21 = dijc(2,1)
      d22 = dijc(2,2)
      d23 = dijc(2,3)
      d24 = dijc(2,4)
      d25 = dijc(2,5)
      d26 = dijc(2,6)
      d27 = dijc(2,7)

      d31 = dijc(3,1)
      d32 = dijc(3,2)
      d33 = dijc(3,3)
      d34 = dijc(3,4)
      d35 = dijc(3,5)
      d36 = dijc(3,6)
      d37 = dijc(3,7)
      d38 = dijc(3,8)
      d39 = dijc(3,9)
      d310 = dijc(3,10)
      d311 = dijc(3,11)
      d312 = dijc(3,12)
      d313 = dijc(3,13)

      if (ldebug) then
         call vbf_wp_wp_BCD_fill_c(k1,k2,q1,q2)
         print*,' D0: ',abs(d0/d0c-1)
         print('(a,3g10.2)'),' D1j ',abs(d11/dijc(1,1)-1),
     1                               abs(d12/dijc(1,2)-1),
     2                               abs(d13/dijc(1,3)-1)
         print('(a,7g10.2)'),' D2j ',abs(d21/dijc(2,1)-1),
     1                               abs(d22/dijc(2,2)-1),
     1                               abs(d23/dijc(2,3)-1),
     1                               abs(d24/dijc(2,4)-1),
     1                               abs(d25/dijc(2,5)-1),
     1                               abs(d26/dijc(2,6)-1),
     1                               abs(d27/dijc(2,7)-1)
         print('(a,7g10.2)'),' D3j ',abs(d31/dijc(3,1)-1),
     1                               abs(d32/dijc(3,2)-1),
     1                               abs(d33/dijc(3,3)-1),
     1                               abs(d34/dijc(3,4)-1),
     1                               abs(d35/dijc(3,5)-1),
     1                               abs(d36/dijc(3,6)-1),
     1                               abs(d37/dijc(3,7)-1)
         print('(a,7g10.2)'),'     ',abs(d38/dijc(3,8)-1),
     1                               abs(d39/dijc(3,9)-1),
     1                               abs(d310/dijc(3,10)-1),
     1                               abs(d311/dijc(3,11)-1),
     1                               abs(d312/dijc(3,12)-1),
     1                               abs(d313/dijc(3,13)-1)
      endif
c
      ce1 = -2*(d12+d24)*eps2k2*(q1sq(id)+q2sq(id)-3*s-4*t) - 
     1      2*(d12-d24)*eps2q1*(q2sq(id)-t) 
     2  +4*(- d11*eps2k2*s - d12*eps2k1*t + d13*eps2k2*
     3  (q2sq(id)-s-t) + d13*eps2q1*q2sq(id) - d21*eps2k2*s 
     4 - d22*eps2k2*t - d22*eps2q1*q2sq(id) + d23*eps2k2*q1sq(id)
     5 + d25*eps2k2*(q2sq(id)-s-2*t) 
     6 - d26*eps2k2*(q2sq(id)-s-t)+d26*eps2q1*t+2*d27*eps2q1
     7 - d32*eps2k2*q2sq(id) - d34*eps2k2*(q2sq(id)-t) 
     8 + d35*eps2k2*(q2sq(id)-s-t)+d36*eps2k2*(2*q2sq(id)-t) 
     9 + d37*eps2k2*q1sq(id) + d38*eps2k2*(q1sq(id)+q2sq(id)-s)
     & - d39*eps2k2*q1sq(id) 
     1 - d310*eps2k2*(q1sq(id)+2*q2sq(id)-2*s-t)
     2 - 4*d311*eps2k2 + 6*d312*eps2k2 + 2*d313*eps2q1)

      ce2= -2*(d12+d24)*(eps1k2*(q1sq(id)+q2sq(id)-s-2*t)
     1  +eps1q2*(q2sq(id)-s-3*t)) + 
     2  4*(d13*eps1k2*(q1sq(id)-2*s-t) - 
     3  d13*eps1q2*(2*s+t) + d22*eps1k2*q2sq(id)
     4  + d22*eps1q2*q2sq(id) - d23*eps1k2*t + 
     5    d23*eps1q2*(q1sq(id)-t) - d24*eps1k1*q2sq(id) 
     6  + d25*eps1k2*q1sq(id) + d25*eps1k1*(q2sq(id)-2*s-t) 
     7  + d26*eps1k2*t - d26*eps1k1*(q1sq(id)-s) - 
     8  2*d27*eps1q2 + d33*eps1k2*q1sq(id) + d33*eps1q2*q1sq(id)
     9  + d37*eps1k1*(q2sq(id)-s-t) + d38*eps1k1*q2sq(id)
     &  - d39*eps1k1*(q1sq(id)+q2sq(id)-s) - d310*eps1k1*(q2sq(id)-t)
     1  +2*d311*eps1k2+2*d312*eps1q2-6*d313*eps1k1)

C NOTE: cq = 1/2 * cq1 in my reduce code
      cq = (d12+d24)*eps1eps2*s + 2*(4*d12*eps2k2*eps1k2 + 
     1 3*d12*eps2k2*eps1q2+d12*eps2q1*eps1k2-4*d13*eps2k2*eps1k2-
     2 2*d13*eps2k2*eps1q2-2*d13*eps2q1*eps1k2-d13*eps1eps2*s+
     3 2*d22*eps2k2*eps1q2-d22*eps1eps2*t-2*d23*eps2q1*eps1k2-
     4 2*d23*eps2q1*eps1q2-d23*eps1eps2*t+6*d24*eps2k2*eps1k2+
     5 3*d24*eps2k2*eps1q2+d24*eps2q1*eps1k2-6*d25*eps2k2*eps1k2-
     6 2*d25*eps2k2*eps1q2-2*d25*eps2q1*eps1k2-d25*eps1eps2*s-
     7 4*d26*eps2k2*eps1q2+4*d26*eps2q1*eps1k2+2*d26*eps2q1*eps1q2+ 
     8 d26*eps1eps2*(s+2*t)-d32*eps1eps2*q2sq(id)+
     9 d33*eps1eps2*q1sq(id)+2*d34*eps2k2*eps1k2-2*d35*eps2k2*eps1k2+
     & 2*d36*eps2k2*eps1q2+d36*eps1eps2*(q2sq(id)-t)-
     1 2*d37*eps2q1*eps1k2+d37*eps1eps2*(q2sq(id)-s-t)+
     2 2*d38*eps2q1*eps1q2+d38*eps1eps2*(q1sq(id)+2*q2sq(id)-s) -
     3 2*d39*eps2q1*eps1q2-d39*eps1eps2*(2*q1sq(id)+q2sq(id)-s) -
     4 2*d310*eps2k2*eps1q2+2*d310*eps2q1*eps1k2-d310*eps1eps2*
     5 (2*q2sq(id)-s-2*t)+4*d312*eps1eps2-4*d313*eps1eps2)

      cborn = -2*((d36+d37-2*d310)*(q2sq(id)-t) + 
     1    d38*(q1sq(id)+2*q2sq(id)) - d39*(2*q1sq(id)+q2sq(id)) )
     2    -2*(d0+d11+d12-2*d13+d24-d25+d26-d37-d38+d39+d310)*s
     3    -2*(-(d22+d23-2*d26)*t + 2*d27 - d32*q2sq(id)
     4    +d33*q1sq(id)+ 6*(d312-d313))

      if (gaugecheck) then
         gce1 = -2*eps2q2*( d12*(2*q1sq(id)+q2sq(id)-s-3*t) - 
     1       2*(d13-d22)*(q1sq(id)+q2sq(id)-s-t)
     2     - d24*(q2sq(id)-s-t)-2*d26*q1sq(id)-4*d27-4*d312)

         gce2 = 2*eps1q1*(d12*(q2sq(id)+t)-2*d13*(q2sq(id)+s) + 
     1      2*d22*q2sq(id) + 2*d23*(q1sq(id)+q2sq(id)-s-2*t)
     2     +d24*(q2sq(id)+t)+2*d25*(-q2sq(id)+s+t)-2*d26*(q2sq(id)-t)
     3     -4*d27+2*d33*q1sq(id) + 4*d313)

         gcq = 4*eps1q1*eps2q2*(d12-d13+d22-d23+d38-d39) +
     1        2*eps1q1*eps2k2*(d12-4*d23+d24+2*d26-2*d37+2*d310) -
     2        4*eps1q1*eps2q1*(d23-d26+d33-d39) +
     3        2*eps2q2*eps1k2*(3*d12-4*d13+4*d22+d24-2*d25-
     4                                     2*d26+2*d36-2*d310) +
     5        4*eps2q2*eps1q2*(d12-d13+2*d22-2*d26+d32-d38)
         ce1 = ce1 + gce1
         ce2 = ce2 + gce2
         cq = cq + gcq
      endif
c
c  now add the terms from the vertex and propagator corrections
      ce1 = ce1 + 2*eps2k2*Teps2(id)
      ce2 = ce2 + 2*eps1k1*Teps1(id)
      if (gaugecheck) then
         ce1 = ce1 + 2*eps2q2*Tg2(id)
         ce2 = ce2 - 2*eps1q1*Tg1(id)
      endif
      cborn = cborn*t - Tborn(id)
c  and the final result
      mvirt = me1*ce1 + me2*ce2 + mq*cq + mborn*cborn
      if (ldebug) then
         print*,' mvirt/mborn ',mvirt/mborn
         if (abs(eps1(0)/q1(0)-1).lt.1d-10) then
            print*,' mborn type ',(mborn*cborn+ me2*ce2)/mborn
            print*,' mq type    ',(me1*ce1+ mq*cq)/mborn
         else
            print*,' mborn type ',(mborn*cborn+ me1*ce1)/mborn
            print*,' mq type    ',(me2*ce2+ mq*cq)/mborn
         endif
         read(*,'(a)') text
      endif

      return
      end

c---------------------------------------------------------------------

      subroutine vbf_wp_wp_gauge_check_5(j5,qin,qsign,psi,p,nq1,nq2,
     1                         jal,jbe,cvirtc)
      implicit none
      integer qsign,nq1,nq2
      double complex j5(0:3,3), psi(2,-1:1,4), jal(0:5),jbe(0:5)
      double precision q(0:3), qal(0:3), qbe(0:3), qin(0:3), p(0:3,4)
c
      double complex mv5(3), mvc(3), mb(3), mbc(3), braket(2,2), jx(0:5)
      integer mu,j,id
      double precision momsum, dummy(0:4), cvirtc
      double complex vbf_wp_wp_S1C,vbf_wp_wp_dotrc
      external vbf_wp_wp_S1C, vbf_wp_wp_dotrc
c
	logical bad_gauge,bad_gauge_sin,bad_gauge_isig
	common /vbf_wp_wp_gauge / bad_gauge,bad_gauge_sin,bad_gauge_isig
	
	bad_gauge_sin = .false. ! set F at beginning of each run
	
********************************************	
	
      do mu = 0,3
         q(mu) = qsign*qin(mu)
      enddo
c momenta are outgoing
      qal(0) = dreal(jal(4))
      qal(1) = dreal(jal(5))
      qal(2) = dimag(jal(5))
      qal(3) = dimag(jal(4))
      qbe(0) = dreal(jbe(4))
      qbe(1) = dreal(jbe(5))
      qbe(2) = dimag(jbe(5))
      qbe(3) = dimag(jbe(4))
c compute vbf_wp_wp_born amplitudes contracted with momentum instead of other quark line      
      call vbf_wp_wp_KET2C(psi(1,-1,nq1),.true.,p(0,nq1),-1,
     1        qal,jal,braket(1,1),dummy)
      call vbf_wp_wp_BRA2C(psi(1,-1,nq2),.true.,p(0,nq2),-1,
     1        qbe,jbe,braket(1,2),dummy)
      mb(3) = vbf_wp_wp_S1C(psi(1,-1,nq2),jbe,.true.,-1,braket(1,1))
      mb(1) = -vbf_wp_wp_S1C(braket(1,2),jal,.true.,-1,psi(1,-1,nq1))
      mb(2) = -mb(1)-mb(3)
c
      do mu = 0,3
         jx(mu) = jal(mu)
      enddo
      jx(4) = jal(4)+dcmplx(q(0),q(3))
      jx(5) = jal(5)+dcmplx(q(1),q(2))
      id = 1
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,-1,nq1),psi(1,-1,nq2),p(0,nq1),p(0,nq2),-1,
     2     jx, jbe,
     3     mbc(1),   mvc(1))
c      mvc(1) = - mvc(1)     not needed: vbf_wp_wp_boxline6 alread return -amplitude
c      mbc(1) = - mbc(1)

      do mu = 0,3
         jx(mu) = jbe(mu)
      enddo
      jx(4) = jbe(4)+dcmplx(q(0),q(3))
      jx(5) = jbe(5)+dcmplx(q(1),q(2))
      id = 1
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,-1,nq1),psi(1,-1,nq2),p(0,nq1),p(0,nq2),-1,
     2     jal, jx,
     3     mbc(3),   mvc(3))
      mvc(3) = - mvc(3)
      mbc(3) = - mbc(3)

      mvc(2) = - mvc(1) - mvc(3)
      mbc(2) = - mbc(1) - mbc(3)

      if (.false.) then
         print*,' momentum check in vbf_wp_wp_gauge_check_5 '
         do mu = 0,3
            momsum = p(mu,nq1)-qal(mu)-qbe(mu)-q(mu)-p(mu,nq2)
            print*,' mu = ',mu,' momsum = ',momsum
         enddo
         do j = 1,3
            print*,' j = ',j,' vbf_wp_wp_tree ratio = ',mbc(j)/mb(j)
         enddo
      endif

      do j = 1,3
         mv5(j) = vbf_wp_wp_dotrc(q,j5(0,j))-cvirtc*mb(j)

	 if( abs((abs(mv5(j)/mvc(j))-1d0)).gt.1e-1 ) then
		bad_gauge_sin = .true.
	 endif
      enddo
      end
      
c---------------------------------------------------------------------

      subroutine vbf_wp_wp_gauge_check0_5(j5,qin,qsign,psi,p,nq1,nq2,
     1                          jal,jbe,cvirtc)
      implicit none
      integer qsign,nq1,nq2
      double complex j5(0:3), psi(2,-1:1,4), jal(0:5),jbe(0:5)
      double precision q(0:3), qal(0:3), qbe(0:3), qin(0:3), p(0:3,4)
c
      double complex mv5(3), mvc(3), mb(3), mbc(3), braket(2,2), jx(0:5)
      integer mu,j,id
      double precision momsum, dummy(0:4), cvirtc
      double complex vbf_wp_wp_S1C,vbf_wp_wp_dotrc
      external vbf_wp_wp_S1C, vbf_wp_wp_dotrc
c
      logical bad_gauge,bad_gauge_sin,bad_gauge_isig
      common /vbf_wp_wp_gauge / bad_gauge,bad_gauge_sin,bad_gauge_isig
      
      ! test only:
       double precision  qx(0:4),fqal(0:4,4)
       double complex psial(2,-1:1,4), jout0(0:5,-1:1,2),jvout(0:5,-1:1,2)
      
      double complex vbf_wp_wp_dotcc
      external vbf_wp_wp_dotcc
      

*************************************************************************

      bad_gauge_sin = .false. ! set F at beginning of each run
	
      do mu = 0,3
         q(mu) = qsign*qin(mu)
      enddo
c momenta are outgoing
      qal(0) = dreal(jal(4))
      qal(1) = dreal(jal(5))
      qal(2) = dimag(jal(5))
      qal(3) = dimag(jal(4))
      qbe(0) = dreal(jbe(4))
      qbe(1) = dreal(jbe(5))
      qbe(2) = dimag(jbe(5))
      qbe(3) = dimag(jbe(4))
c compute vbf_wp_wp_born amplitudes contracted with momentum instead of other quark line      
      call vbf_wp_wp_KET2C(psi(1,-1,nq1),.true.,p(0,nq1),-1,
     1        qal,jal,braket(1,1),dummy)
      call vbf_wp_wp_BRA2C(psi(1,-1,nq2),.true.,p(0,nq2),-1,
     1        qbe,jbe,braket(1,2),dummy)
      mb(3) = vbf_wp_wp_S1C(psi(1,-1,nq2),jbe,.true.,-1,braket(1,1))
      mb(1) = -vbf_wp_wp_S1C(braket(1,2),jal,.true.,-1,psi(1,-1,nq1))
      mb(2) = -mb(1)-mb(3)

      do mu = 0,3
         jx(mu) = jal(mu)
      enddo
      jx(4) = jal(4)+dcmplx(q(0),q(3))
      jx(5) = jal(5)+dcmplx(q(1),q(2))
      id = 1
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,-1,nq1),psi(1,-1,nq2),p(0,nq1),p(0,nq2),-1,
     2     jx, jbe,
     3     mbc(1),   mvc(1))

   
      do mu = 0,3
         jx(mu) = jbe(mu)
      enddo
      jx(4) = jbe(4)+dcmplx(q(0),q(3))
      jx(5) = jbe(5)+dcmplx(q(1),q(2))
      id = 1
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,-1,nq1),psi(1,-1,nq2),p(0,nq1),p(0,nq2),-1,
     2     jal, jx,
     3     mbc(3),   mvc(3))
 
      mvc(3) = - mvc(3)
      mbc(3) = - mbc(3)

      mvc(2) = - mvc(1) - mvc(3)
      mbc(2) = - mbc(1) - mbc(3)
      
      if (.false.) then
         print*,' momentum check in vbf_wp_wp_gauge_check_5 '
         do mu = 0,3
            momsum = p(mu,nq1)-qal(mu)-qbe(mu)-q(mu)-p(mu,nq2)
            print*,' mu = ',mu,' momsum = ',momsum
         enddo
         j=2
         print*,' j = ',j,' vbf_wp_wp_tree ratio = ',mbc(j)/mb(j)
      endif

         mv5(2) = vbf_wp_wp_dotrc(q,j5(0))-cvirtc*mb(2)
	 if( (abs((abs(mv5(2)/mvc(2))-1d0)).gt.1e-1).or.
     &	     ((abs((abs(dreal(mv5(2))/dreal(mvc(2))))-1d0)).gt.1e-1).or.
     &	     ((abs((abs(dimag(mv5(2))/dimag(mvc(2))))-1d0)).gt.1e-1)) then
     
		bad_gauge_sin = .true.
	 endif
	 
      end
      
c---------------------------------------------------------------------

      subroutine vbf_wp_wp_gauge_check_isig_5(j5,qin,qsign,psi,p,nq1,nq2,
     1                         jal,jbe,cvirtc)
      implicit none
      integer qsign,nq1,nq2
      double complex j5(0:3,-1:1,3), psi(2,-1:1,4), jal(0:5),jbe(0:5)
      double precision q(0:3), qal(0:3), qbe(0:3), qin(0:3), p(0:3,4)
c
      double complex mv5(3,-1:1),mvc(3,-1:1),mb(3,-1:1),mbc(3,-1:1),
     1 		     braket(2,-1:1,2),jx(0:5)
      integer mu,j,id,isig
      double precision momsum, dummy(0:4), cvirtc
      double complex vbf_wp_wp_S1C,vbf_wp_wp_dotrc
      external vbf_wp_wp_S1C, vbf_wp_wp_dotrc

	logical bad_gauge,bad_gauge_sin,bad_gauge_isig
	common /vbf_wp_wp_gauge / bad_gauge,bad_gauge_sin,bad_gauge_isig
	
	bad_gauge_isig = .false. ! set F at beginning of each run
	
      do mu = 0,3
         q(mu) = qsign*qin(mu)
      enddo
c momenta are outgoing
      qal(0) = dreal(jal(4))
      qal(1) = dreal(jal(5))
      qal(2) = dimag(jal(5))
      qal(3) = dimag(jal(4))
      qbe(0) = dreal(jbe(4))
      qbe(1) = dreal(jbe(5))
      qbe(2) = dimag(jbe(5))
      qbe(3) = dimag(jbe(4))
c compute vbf_wp_wp_born amplitudes contracted with momentum 
c (instead of other quark line)
      
      do isig = -1,1,2
      
      call vbf_wp_wp_KET2C(psi(1,isig,nq1),.true.,p(0,nq1),isig,
     1        qal,jal,braket(1,isig,1),dummy)
      call vbf_wp_wp_BRA2C(psi(1,isig,nq2),.true.,p(0,nq2),isig,
     1        qbe,jbe,braket(1,isig,2),dummy)
      mb(3,isig) =  vbf_wp_wp_S1C(psi(1,isig,nq2),jbe,.true.,isig,braket(1,isig,1))
      mb(1,isig) = -vbf_wp_wp_S1C(braket(1,isig,2),jal,.true.,isig,psi(1,isig,nq1))
      mb(2,isig) = -mb(1,isig)-mb(3,isig)
      
      enddo !isig
c
      do mu = 0,3
         jx(mu) = jal(mu)
      enddo
      jx(4) = jal(4)+dcmplx(q(0),q(3))
      jx(5) = jal(5)+dcmplx(q(1),q(2))
      id = 1
      do isig = -1,1,2
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,isig,nq1),psi(1,isig,nq2),p(0,nq1),p(0,nq2),isig,
     2     jx, jbe,
     3     mbc(1,isig),   mvc(1,isig))
      enddo !isig

      do mu = 0,3
         jx(mu) = jbe(mu)
      enddo
      jx(4) = jbe(4)+dcmplx(q(0),q(3))
      jx(5) = jbe(5)+dcmplx(q(1),q(2))
      id = 1
      do isig = -1,1,2
      call vbf_wp_wp_boxline6(id, 
     1     psi(1,isig,nq1),psi(1,isig,nq2),p(0,nq1),p(0,nq2),isig,
     2     jal, jx,
     3     mbc(3,isig),   mvc(3,isig))
      mvc(3,isig) = - mvc(3,isig)
      mbc(3,isig) = - mbc(3,isig)

      mvc(2,isig) = - mvc(1,isig) - mvc(3,isig)
      mbc(2,isig) = - mbc(1,isig) - mbc(3,isig)
      enddo !isig

      if (.false.) then
         print*,' momentum check in vbf_wp_wp_gauge_check_5 '
         do mu = 0,3
            momsum = p(mu,nq1)-qal(mu)-qbe(mu)-q(mu)-p(mu,nq2)
            print*,' mu = ',mu,' momsum = ',momsum
         enddo
         do j = 1,3
            print*,' j = ',j,' vbf_wp_wp_tree ratio for isig = -1: ',
     &           mbc(j,-1)/mb(j,-1)
         enddo
      endif

      do isig = -1,1,2
      do j = 1,3
         mv5(j,isig) = vbf_wp_wp_dotrc(q,j5(0,isig,j))-cvirtc*mb(j,isig)

 	 if( abs((abs(mv5(j,isig)/mvc(j,isig))-1d0)).gt.1e0 ) then
		bad_gauge_isig = .true.
 	 endif
   
      enddo
      enddo !isig

      end
      
c--------------------------------------------------------------       
c       
      subroutine vbf_wp_wp_revers(ans,rev)
C  
c	rev(ans) exchanges ans(2) and ans(3);
c	needed in call of mg routines with switched quark lines
c
c	in:   ans(3) ... 3 dim array
c	out:  rev(3) ... 3 dim array (=ans switched)	
c
C  
      IMPLICIT NONE
      REAL*8    ans(3),rev(3)

	
	rev(1) = ans(1)
	rev(2) = ans(3)
	rev(3) = ans(2)
	
      end	
