ccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     compute leptonic tensors 
c
      subroutine vbf_wp_wp_compute_tensors_real(pin)
c
ccccccccccccccccccccccccccccccccccccccccccccccccc
c
      implicit none
c
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
c
      integer nlegs
      parameter (nlegs=nlegreal)
      real*8 pin(0:3,nlegs)
c
c vbfnlo stuff:
      include 'src/Lib/vbf_wp_wp/global.inc'
      include 'src/Lib/vbf_wp_wp/coupl.inc'
      include 'src/Lib/vbf_wp_wp/tensorww.inc'
      real*8 p(0:3,np,3), v(0:3,nv)
      real*8 q12(0:4,3), q34(0:4,3)

      complex*16 zero
      parameter (zero=(0d0,0d0))
c
c declare local variables
      integer i,j,mu
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c

      do j=2,3
         do i = 1,np
         do mu = 0,3
            p(mu,i,j) = 0d0
         enddo
         enddo
      enddo   
      do mu = 0,3
         p(mu,1,1) = pin(mu,1)
         p(mu,2,1) = pin(mu,2)   
         do i = 1,4
            v(mu,i) = pin(mu,i+2)
         enddo !i     
         p(mu,3,1) = pin(mu,7)
         p(mu,4,1) = pin(mu,8) 
         p(mu,5,1) = pin(mu,9)   
      enddo ! mu

c compute invariants:
      call vbf_wp_wp_calc_invariants(p,v,q12,q34,3)
c
c reset leptonic tensors:
      call vbf_wp_wp_vtowpm_reset
c compute leptonic tensors:
         CALL vbf_wp_wp_ixxxxx(v(0,1),ZERO , 1,-1,wel) !e+ 
         CALL vbf_wp_wp_oxxxxx(v(0,2),ZERO ,-1, 1,wve) !ve 
         CALL vbf_wp_wp_ixxxxx(v(0,3),ZERO , 1,-1,wmu) !mu+ 
         CALL vbf_wp_wp_oxxxxx(v(0,4),ZERO ,-1, 1,wvu) !vm 
         CALL vbf_wp_wp_jioxxx(wel,wve,GWF ,WMASS,WWIDTH,we)    !W+->e+ve
         CALL vbf_wp_wp_jioxxx(wmu,wvu,GWF ,WMASS,WWIDTH,wu)    !W+->mu+vm

         do mu = 0,3
            qe(mu)  = v(mu,1)+v(mu,2)
            qu(mu)  = v(mu,3)+v(mu,4)
            qww(mu) = qe(mu) + qu(mu)
         enddo
         qe(4)  = qe(0)**2-qe(1)**2-qe(2)**2-qe(3)**2
         qu(4)  = qu(0)**2-qu(1)**2-qu(2)**2-qu(3)**2
         qww(4) = qww(0)**2-qww(1)**2-qww(2)**2-qww(3)**2
c        
c	 
c leptonic tensors
C for W+W+->e+ve mu+vm
c         j1 = 1
         do j = 2,3

         call vbf_wp_wp_WPPTOWPP(q12(0,j),q34(0,j),v,wppwpp(0,0,j)) 
 
c t-channel processes:	 
C for V1 V2->e+ve
         call vbf_wp_wp_WPVtoWP(1,1,q34(0,j),v,awen(0,0,1,j),zwen(0,0,1,j)) !Wu on upper
         call vbf_wp_wp_WPVtoWP(1,2,q12(0,j),v,awen(0,0,2,j),zwen(0,0,2,j)) !Wu on lower
c        
         call vbf_wp_wp_WPVtoWP(1,1,q12(0,j),v,waen(0,0,2,j),wzen(0,0,2,j)) !Wu on lower
         call vbf_wp_wp_WPVtoWP(1,2,q34(0,j),v,waen(0,0,1,j),wzen(0,0,1,j)) !Wu on upper

c for V1 V2->mu+vm
         call vbf_wp_wp_WPVtoWP(2,1,q34(0,j),v,awmu(0,0,1,j),zwmu(0,0,1,j)) !We on upper
         call vbf_wp_wp_WPVtoWP(2,2,q12(0,j),v,awmu(0,0,2,j),zwmu(0,0,2,j)) !We on lower
c        
         call vbf_wp_wp_WPVtoWP(2,1,q12(0,j),v,wamu(0,0,2,j),wzmu(0,0,2,j)) !We on lower
         call vbf_wp_wp_WPVtoWP(2,2,q34(0,j),v,wamu(0,0,1,j),wzmu(0,0,1,j)) !We on upper

         
         enddo !j

      return
      end
