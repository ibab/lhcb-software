c
      subroutine vbf_wp_wp_setreal(p,fermion_flav,amp2)
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'
      integer nleg
      parameter (nleg=nlegreal)
      real * 8 p(0:3,nleg)
      integer fermion_flav(nleg)
      real * 8 amp2

      call vbf_wp_wp_compute_tensors_real(p) 
      call vbf_wp_wp_compreal_ew(p,fermion_flav,amp2)

c     cancel as/(2pi) associated with amp2. It will be put back by vbf_wp_wp_real_ampsq
      amp2 = amp2/(st_alpha/(2d0*pi))

      return
      end
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      subroutine vbf_wp_wp_compreal_ew(pin,bflav,amp2)
      implicit none
c
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/cvecbos.h'
c
      integer nlegs
      parameter (nlegs=nlegreal)
      real*8 pin(0:3,nlegs)  
      integer bflav(nlegs)
      real*8 amp2 
c
c vbfnlo stuff:
      include 'src/Lib/vbf_wp_wp/global.inc'
      integer bos
      real*8 p(0:3,np), v(0:3,nv)
      real*8 pbar(0:3,4+nv),qbar(0:4)
      real*8 polcol,polcolq,polcolg
      real*8 res(3)

      real*8 N ! color factors
      parameter(N=3d0)

      complex*16 zero
      parameter (zero=(0d0,0d0))
c
c declare local variables
c
      integer i,mu
      integer FSIGN(4+nv),gsign,physToDiag(5)
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      bos = 33

      polcol = 0d0
      polcolq = 1d0/(4d0*N**2)
      polcolg = 1d0/(4d0*N*(N**2-1))
 
      do mu = 0,3
         p(mu,1) = pin(mu,1)
         p(mu,2) = pin(mu,2)   
         do i = 1,4
            v(mu,i) = pin(mu,i+2)
         enddo !i  

         if (bflav(1)*bflav(2).ne.0) then ! final-state gluon 
            if (bflav(9).eq.0) then
               p(mu,3) = pin(mu,7)
               p(mu,4) = pin(mu,8) 
               p(mu,5) = pin(mu,9)     
            elseif (bflav(7).eq.0) then
               p(mu,3) = pin(mu,8)
               p(mu,4) = pin(mu,9) 
               p(mu,5) = pin(mu,7)  
            elseif (bflav(8).eq.0) then
               p(mu,3) = pin(mu,7)
               p(mu,4) = pin(mu,9) 
               p(mu,5) = pin(mu,8)  
            endif
         else   ! initial-state gluon 
            p(mu,3) = pin(mu,7)
            p(mu,4) = pin(mu,8) 
            p(mu,5) = pin(mu,9) 
         endif ! fin/in state gluon
      enddo ! mu

      if (bflav(1).gt.0.and.bflav(2).gt.0) then

C*******************  q1 q3 ---> q2 q4 g W W   **********************

      physToDiag(1)=1    !physToDiag(1/2) are labels of incoming quarks
      physToDiag(2)=3
      physToDiag(3)=2    !physToDiag(3/4) are labels of outgoing quarks.
      physToDiag(4)=4
      physToDiag(5)=5   ! gluon

      fsign(1) = 1
      fsign(2) = 1
      fsign(3) = 1
      fsign(4) = 1
      gsign    = 1

      polcol = polcolq

      elseif (bflav(1).gt.0.and.bflav(2).lt.0) then
c            
C******************* q1 qb4 ---> q2 qb3 g W W   **********************
      
      physToDiag(1)=1    
      physToDiag(2)=4
      physToDiag(3)=2    
      physToDiag(4)=3
      physToDiag(5)=5   ! gluon
c
      fsign(1) =  1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) = -1
      gsign    =  1
      
      polcol = polcolq
      
      elseif (bflav(1).lt.0.and.bflav(2).gt.0) then
      
C******************* qbar2 q3 ---> qbar1 q4 g W W   **********************
      
      physToDiag(1)=2    
      physToDiag(2)=3
      physToDiag(3)=1    
      physToDiag(4)=4
      physToDiag(5)=5   ! gluon
c
      fsign(1) = -1
      fsign(2) = -1
      fsign(3) =  1
      fsign(4) =  1
      gsign    =  1
      
      polcol = polcolq

      elseif (bflav(1).lt.0.and.bflav(2).lt.0) then

C*******************  qbar2 qb4 ---> qbar1 qb3 g W W   **********************

      physToDiag(1)=2    
      physToDiag(2)=4
      physToDiag(3)=1    
      physToDiag(4)=3
      physToDiag(5)=5   ! gluon
c
      fsign(1) = -1
      fsign(2) = -1
      fsign(3) = -1
      fsign(4) = -1
      gsign    =  1
      
      polcol = polcolq
     
      elseif (bflav(1).eq.0.and.bflav(2).gt.0.and.bflav(7).gt.0) then
         
c*******************  g q ---> q q qb W W   **********************

      physToDiag(1)=5          
      physToDiag(2)=3           
      physToDiag(3)=2           
      physToDiag(4)=4
      physToDiag(5)=1
c
      fsign(1) = -1
      fsign(2) =  1
      fsign(3) =  1
      fsign(4) =  1
      gsign    = -1
      
      polcol = polcolg            

      elseif (bflav(1).eq.0.and.bflav(2).gt.0.and.bflav(7).lt.0) then
         
c*******************  g q ---> qb q q W W   **********************
c
      physToDiag(1)=5          
      physToDiag(2)=3           
      physToDiag(3)=1           
      physToDiag(4)=4
      physToDiag(5)=2
c
      fsign(1) = -1
      fsign(2) =  1
      fsign(3) =  1
      fsign(4) =  1
      gsign    = -1
      
      polcol = polcolg              

      elseif (bflav(1).gt.0.and.bflav(2).eq.0.
     &    and.bflav(7).gt.0.and.bflav(8).gt.0) then
      
C*******************  q g ---> q q qb W W   **********************
      
      physToDiag(1)=1             
      physToDiag(5)=3            
      physToDiag(3)=2             
      physToDiag(2)=5
      physToDiag(4)=4
c
      fsign(1) =  1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) =  1
      gsign    = -1

      polcol = polcolg

      elseif (bflav(1).gt.0.and.bflav(2).eq.0.and.
     &        bflav(7).gt.0.and.bflav(8).lt.0) then
      
C*******************  q g ---> q qb q W W   **********************
     
      physToDiag(1)=1             
      physToDiag(4)=3            
      physToDiag(3)=2             
      physToDiag(2)=5
      physToDiag(5)=4
c
      fsign(1) =  1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) =  1
      gsign    = -1

      polcol = polcolg

      elseif (bflav(1).eq.0.and.bflav(2).lt.0.and.bflav(7).gt.0) then
        
C*******************  g qbar ---> q qb qb W W  **********************

      physToDiag(1)=5
      physToDiag(2)=4
      physToDiag(3)=2
      physToDiag(4)=3
      physToDiag(5)=1
c
      fsign(1) = -1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) = -1
      gsign    = -1

      polcol = polcolg                 

      elseif (bflav(1).eq.0.and.bflav(2).lt.0.and.bflav(7).lt.0) then
        
C*******************  g qbar ---> qb qb q W W  **********************

      physToDiag(1)=5
      physToDiag(2)=4
      physToDiag(3)=1
      physToDiag(4)=3
      physToDiag(5)=2
c
      fsign(1) = -1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) = -1
      gsign    = -1

      polcol = polcolg

      elseif (bflav(1).lt.0.and.bflav(2).eq.0.and.bflav(7).lt.0) then
 
C*******************  qbar2 g ---> qbar1 qb3 q4 W W   **********************
c
      physToDiag(1)=2!1             
      physToDiag(5)=3            
      physToDiag(3)=1!2             
      physToDiag(2)=5
      physToDiag(4)=4

      fsign(1) = -1
      fsign(2) = -1
      fsign(3) = -1
      fsign(4) =  1
      gsign    = -1
      
      polcol = polcolg

      else
         
         print*,'this flav combination is not implemented'
         print*,'bflav=',bflav

      endif
         
C*****************  end of process evaluation  **********************

      do mu = 0,3
         do i = 1,5
            pbar(mu,physToDiag(i))=p(mu,i)
         enddo
         qbar(mu) = pbar(mu,5)
      enddo 
      qbar(4) = 0d0

c lepton momenta:
      do mu = 0,3             ! kinematics for 4-lepton final state
         pbar(mu,5) = v(mu,1) ! l+
         pbar(mu,6) = v(mu,2) ! nu
         pbar(mu,7) = v(mu,3) ! l'+
         pbar(mu,8) = v(mu,4) ! nu'
      enddo
      fsign(5) = -1
      fsign(6) = 1
      fsign(7) = -1
      fsign(8) = 1

      call vbf_wp_wp_qqwppqqj(pbar,fsign,qbar,gsign,bos,res(1))
      
      amp2 = res(1)*polcol
c symmetry factor for leptons:
      amp2 = amp2*wsymfact

      return
      end
