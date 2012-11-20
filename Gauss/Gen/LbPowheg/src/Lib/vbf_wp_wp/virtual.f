      subroutine vbf_wp_wp_setvirtual(p,vflav,virtual)

      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'

      real * 8 p(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual

      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),vbf_wp_wp_born,colcf
      real *8 vbf_wp_wp_powheginput
      external vbf_wp_wp_powheginput 

      logical, save :: firsttime = .true. 

      integer fakevirt
      save fakevirt 

      virtual = 0d0

      if (firsttime) then
      fakevirt=vbf_wp_wp_powheginput("#fakevirt")
      if (fakevirt == 1) write(*,*) 'WARNING: Using fakevirt !'
      firsttime = .false.
      endif

      if(fakevirt.eq.1) then  
 
      call vbf_wp_wp_compute_tensors(p)     
      call vbf_wp_wp_compborn_ew(p,vflav,vbf_wp_wp_born,bbmunu) 

      virtual = 0.2d0*vbf_wp_wp_born
      
      else

c numbering of momenta is q(1) q(2) -> l1(3)v1(4) l2(5)v2(6) q(7)q(8)
c
      call vbf_wp_wp_compute_tensors(p) 
      call vbf_wp_wp_compvirt_ew(p,vflav,virtual) 

c     cancel as/(2pi) associated with amp2. It will be put back by vbf_wp_wp_real_ampsq
      virtual = virtual/(st_alpha/(2d0*pi))

      endif

      return 
      end
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      subroutine vbf_wp_wp_compvirt_ew(pin,bflav,virtual)
      implicit none
c
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/cvecbos.h'
c
      integer nlegs,nf
      parameter (nlegs=nlegborn)
      real*8 pin(0:3,nlegs)  
      integer bflav(nlegs)
      real*8 virtual
c
c vbfnlo stuff:
      include 'src/Lib/vbf_wp_wp/global.inc'
      integer bos,nlo
      real*8 p(0:3,np), v(0:3,nv)
      real*8 pbar(0:3,4+nv), polcol
      real*8 res,tri,box,pent

      real*8 N ! color factors
      parameter(N=3d0)

      complex*16 zero
      parameter (zero=(0d0,0d0))
c
c declare local variables
c
      integer i,j,mu,nu
      integer FSIGN(4+nv),physToDiag(4)
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      bos = 33
      nlo = 1 ! NLO
      polcol =1d0/(4d0*N**2)
      
      do mu = 0,3
         p(mu,1) = pin(mu,1)
         p(mu,2) = pin(mu,2)   
         do i = 1,4
            v(mu,i) = pin(mu,i+2)
         enddo !i     
         p(mu,3) = pin(mu,7)
         p(mu,4) = pin(mu,8) 
         p(mu,5) = 0d0   
      enddo ! mu

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
      
      if (bflav(1).gt.0.and.bflav(2).gt.0) then

C*******************  q1 q3 ---> q2 q4  W W  **********************

c   physToDiag(ext.momentum label) = Feynman diagram label

      physToDiag(1)=1    !physToDiag(1/2) are labels of incoming quarks
      physToDiag(2)=3
      physToDiag(3)=2    !physToDiag(3/4) are labels of outgoing quarks.
      physToDiag(4)=4

      fsign(1) = 1
      fsign(2) = 1
      fsign(3) = 1
      fsign(4) = 1

      elseif (bflav(1).gt.0.and.bflav(2).lt.0) then

C******************* q1 qb4 ---> q2 qb3 W  W  **********************
c      
      physToDiag(1)=1    !physToDiag(1/2) are labels of incoming quarks
      physToDiag(2)=4
      physToDiag(3)=2    !physToDiag(3/4) are labels of outgoing quarks.
      physToDiag(4)=3
c
      fsign(1) =  1
      fsign(2) =  1
      fsign(3) = -1
      fsign(4) = -1      
      

      elseif (bflav(1).lt.0.and.bflav(2).gt.0) then

C******************* qbar2 q3 ---> qbar1 q4 W W  **********************
      
      physToDiag(1)=2    !physToDiag(1/2) are labels of incoming quarks
      physToDiag(2)=3
      physToDiag(3)=1    !physToDiag(3/4) are labels of outgoing quarks.
      physToDiag(4)=4
c
      fsign(1) = -1
      fsign(2) = -1
      fsign(3) =  1
      fsign(4) =  1


      elseif (bflav(1).lt.0.and.bflav(2).lt.0) then

C*******************  qbar2 qb4 ---> qbar1 qb3 W W  **********************

      physToDiag(1)=2    !physToDiag(1/2) are labels of incoming quarks
      physToDiag(2)=4
      physToDiag(3)=1    !physToDiag(3/4) are labels of outgoing quarks.
      physToDiag(4)=3
c
      fsign(1) = -1
      fsign(2) = -1
      fsign(3) = -1
      fsign(4) = -1      

      elseif (bflav(1).lt.0.and.bflav(2).lt.0) then
         
         write(*,*) 'wrong value of bflav(1) and bflav(2)'
         write(*,*) 'bflav(1) = ',bflav(1)
         write(*,*) 'bflav(2) = ',bflav(2)

      endif
            
C*****************  end of process evaluation  **********************

c get the amplitude squared:
      do mu = 0,3
         do i = 1,4
            pbar(mu,physToDiag(i))=p(mu,i)
         enddo
      enddo
	 
c triangles:
      call vbf_wp_wp_qqwppqq_vonly(pbar,fsign,4,1,bos,tri) 
c boxes:
      call vbf_wp_wp_qqwppqq_vonly(pbar,fsign,-4,1,bos,box) 
c pentagons:
      call vbf_wp_wp_qqwppqq_vonly(pbar,fsign,-5,1,bos,pent)

      res = tri+box+pent

      virtual = res*polcol  
c symmetry factor for leptons:
      virtual = virtual*wsymfact
     
      return
      end
