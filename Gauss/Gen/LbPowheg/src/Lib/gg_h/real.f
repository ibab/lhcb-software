      subroutine gg_h_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/PhysPars.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      integer nlegs
      parameter (nlegs=nlegreal)
      real * 8 p(0:3,nlegs)
      integer rflav(nlegs)
      real * 8 amp2
      integer i,j
      
      amp2 = 0d0
      i=rflav(1)
      j=rflav(2)
      if((i.eq.j).and.(i.eq.0)) then
c     g g -> h g
         call gg_h_M2_gg_hg(p,amp2)
      elseif(((i*j.eq.0).and.(i.ne.j)).and.(i.ne.0)) then
c     q g -> h q
         call gg_h_M2_qg_hq(p,amp2)
      elseif(((i*j.eq.0).and.(i.ne.j)).and.(j.ne.0)) then
c     g q -> h q
         call gg_h_M2_gq_hq(p,amp2)
      elseif((i+j.eq.0).and.(i.ne.j)) then
c     q aq -> h g
         call gg_h_M2_qaq_hg(p,amp2)
      else
         write(*,*) 'ERROR gg_h_setreal: unammissible flavour!'
         write(*,*) rflav(1),' ',rflav(2),'->',rflav(3),' ',rflav(4)
         call exit(1)
      endif
      
      if(amp2.eq.0d0) then
         write(*,*) 'WARNING gg_h_setreal: returning 0 amplitude!'
         write(*,*) rflav(1),' ',rflav(2),'->',rflav(3),' ',rflav(4)
      endif

c     cancel as/(2pi) associated with amp2. It will be put back by gg_h_real_ampsq
      amp2 = amp2/(st_alpha/(2*pi))
      end
      
      
     
      subroutine gg_h_M2_gg_hg(pphy,amp2)
c Real matrix element times normalizations and averages.
c IMPORTANT the flux factor 1/2s is intentionally missing
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/PhysPars.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_br.h'
      real * 8 s,v2,t,u,tmp,xnorm
      parameter (v2=0.70710678118654757d0)
      real* 8 amp2
      integer nlegs
      parameter (nlegs=nlegreal)
      real *8 pphy(0:3,nlegs)
      real *8 m2
      
       s=(pphy(0,1)+pphy(0,2))**2-(pphy(1,1)+pphy(1,2))**2
     #    -(pphy(2,1)+pphy(2,2))**2 -(pphy(3,1)+pphy(3,2))**2
       
       t=(pphy(0,1)-pphy(0,3))**2-(pphy(1,1)-pphy(1,3))**2
     #   -(pphy(2,1)-pphy(2,3))**2 -(pphy(3,1)-pphy(3,3))**2
       
       u=(pphy(0,1)-pphy(0,4))**2-(pphy(1,1)-pphy(1,4))**2
     #   -(pphy(2,1)-pphy(2,4))**2 -(pphy(3,1)-pphy(3,4))**2
       
       m2=(pphy(0,3))**2-(pphy(1,3))**2
     #   -(pphy(2,3))**2 -(pphy(3,3))**2

c From eq.(3.18) of NPB359(91)283

      xnorm=32d0/(256.d0*3.d0*pi*v2)
      tmp=(m2**4+s**4+t**4+u**4)/(s*t*u)
c      print *,s,t,u
      amp2=xnorm*tmp*(st_alpha**3)*ph_GF
c      print *,amp2/br_bornjk(1,2,1)
      end
      
      
      subroutine gg_h_M2_qaq_hg(pphy,amp2)
c Real matrix element times normalizations and averages.
c     IMPORTANT the flux factor 1/2s is missing
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/PhysPars.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      real * 8 s,v2,t,u,tmp,xnorm
      parameter (v2=0.70710678118654757d0)
      real* 8 amp2
      integer nlegs
      parameter (nlegs=nlegreal)
      real *8 pphy(0:3,nlegs)
      
       s=(pphy(0,1)+pphy(0,2))**2-(pphy(1,1)+pphy(1,2))**2
     #    -(pphy(2,1)+pphy(2,2))**2 -(pphy(3,1)+pphy(3,2))**2
       
       t=(pphy(0,1)-pphy(0,3))**2-(pphy(1,1)-pphy(1,3))**2
     #   -(pphy(2,1)-pphy(2,3))**2 -(pphy(3,1)-pphy(3,3))**2
       
       u=(pphy(0,1)-pphy(0,4))**2-(pphy(1,1)-pphy(1,4))**2
     #   -(pphy(2,1)-pphy(2,4))**2 -(pphy(3,1)-pphy(3,4))**2
       
c From eq.(3.1) of NPB359(91)283

      xnorm=16/(36.d0*9.d0*pi*v2)
      tmp=(t**2+u**2)/s
      amp2=xnorm*tmp*(st_alpha**3)*ph_GF
      end

      
      
      subroutine gg_h_M2_qg_hq(pphy,amp2)
c Real matrix element times normalizations and averages.
c IMPORTANT the flux factor 1/2s is missing
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/PhysPars.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      real * 8 s,v2,t,u,tmp,xnorm
      parameter (v2=0.70710678118654757d0)
      real* 8 amp2
      integer nlegs
      parameter (nlegs=nlegreal)
      real *8 pphy(0:3,nlegs)
      
       s=(pphy(0,1)+pphy(0,2))**2-(pphy(1,1)+pphy(1,2))**2
     #    -(pphy(2,1)+pphy(2,2))**2 -(pphy(3,1)+pphy(3,2))**2
       
       t=(pphy(0,1)-pphy(0,3))**2-(pphy(1,1)-pphy(1,3))**2
     #   -(pphy(2,1)-pphy(2,3))**2 -(pphy(3,1)-pphy(3,3))**2
       
       u=(pphy(0,1)-pphy(0,4))**2-(pphy(1,1)-pphy(1,4))**2
     #   -(pphy(2,1)-pphy(2,4))**2 -(pphy(3,1)-pphy(3,4))**2
       
c From eq.(3.3) of NPB359(91)283

      xnorm=-16/(96.d0*9.d0*pi*v2)
      tmp=(s**2+t**2)/u
      amp2=xnorm*tmp*(st_alpha**3)*ph_GF
      end


      subroutine gg_h_M2_gq_hq(pphy,amp2)
c Real matrix element times normalizations and averages.
c IMPORTANT the flux factor 1/2s is missing
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/PhysPars.h' 
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      real * 8 s,v2,t,u,tmp,xnorm
      parameter (v2=0.70710678118654757d0)
      real* 8 amp2
      integer nlegs
      parameter (nlegs=nlegreal)
      real *8 pphy(0:3,nlegs)
      
       s=(pphy(0,1)+pphy(0,2))**2-(pphy(1,1)+pphy(1,2))**2
     #    -(pphy(2,1)+pphy(2,2))**2 -(pphy(3,1)+pphy(3,2))**2
       
       t=(pphy(0,1)-pphy(0,3))**2-(pphy(1,1)-pphy(1,3))**2
     #   -(pphy(2,1)-pphy(2,3))**2 -(pphy(3,1)-pphy(3,3))**2
       
       u=(pphy(0,1)-pphy(0,4))**2-(pphy(1,1)-pphy(1,4))**2
     #   -(pphy(2,1)-pphy(2,4))**2 -(pphy(3,1)-pphy(3,4))**2
       

c From eq.(3.3) of NPB359(91)283

      xnorm=-16/(96.d0*9.d0*pi*v2)
      tmp=(s**2+u**2)/t
      amp2=xnorm*tmp*(st_alpha**3)*ph_GF
      end
