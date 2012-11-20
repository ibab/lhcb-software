
c-------------------------------------------------------------------

      subroutine vbf_wp_wp_calc_invariants(p,v,q12,q34,Lmax)
      implicit none
c
c determine the momentum transfers of the scattering weak bosons 
c and define values for the factorization scales and alphas in vbfnlo format
c
c INPUT  p,v         external particle momenta in standard notation
c        Lmax        number of momentum configurations, L=1 for direct term
c                    L=2,...,Lmax for ptilde momenta for subtraction terms
c                    Lmax not in [1,3] prints initialization information
c
c OUTPUT q12(0:4,L)  momentum of weak boson attached to upper fermion line
c        q34(0:4,L)  momentum of weak boson attached to lower fermion line
c                    fourth component is abs(qij^2)
c
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/scales.inc'
      include 'src/Lib/vbf_wp_wp/global.inc'
c
      real*8 p(0:3,np,3), v(0:3,nv), q12(0:4,3), q34(0:4,3), qsq
      integer Lmax, L, mu
c
cccccccccccccccccccccccccccccccccccccccccccccc
c
      qsq = st_muren2
      do L = 1,3
         als(1,L) =  st_alpha
         als(2,L) =  st_alpha       
         mursq(1,L) = qsq
	 mursq(2,L) = qsq
      enddo         

      do mu = 0,3
         q12(mu,1) = p(mu,1,1)-p(mu,5,1)-p(mu,3,1)
         q34(mu,1) = p(mu,2,1)-p(mu,5,1)-p(mu,4,1)         
c adapted for POWHEG:
            q12(mu,2) = q12(mu,1)
            q12(mu,3) = p(mu,1,1)-p(mu,3,1)
            q34(mu,2) = p(mu,2,1)-p(mu,4,1)
            q34(mu,3) = q34(mu,1)
      enddo
      do L = 1,Lmax
         q12(4,L) = abs(q12(0,L)**2-q12(1,L)**2-q12(2,L)**2-q12(3,L)**2)
         q34(4,L) = abs(q34(0,L)**2-q34(1,L)**2-q34(2,L)**2-q34(3,L)**2)
         qmin(L) = sqrt( min( q12(4,L), q34(4,L) ) )
      enddo
      do L = Lmax+1,3
         qmin(L) = 1d10
      enddo

      end
