c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point

      subroutine wbb_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      include 'src/Lib/wbb/pwhg_st.h'
      include 'src/Lib/wbb/PhysPars.h'
      include 'src/Lib/wbb/pwhg_math.h'

      real * 8 p(0:3,nlegborn)
      integer vflav(nlegborn)      
      real * 8 virtual
c      integer nf
c      parameter (nf=5)
      real*8 amp2  

      call wbb_M2_virt_Wbb(p,amp2)
      virtual = amp2            !  NO st_alpha/(2*pi) factor

c     the prefactors are the same as for the Born
      virtual=virtual*ph_CKM_matrix(abs(vflav(1)),abs(vflav(2)))**2
c
      virtual=virtual*(ph_unit_e/ph_sthw)**2/8 * (4*pi*st_alpha)**2
c     color factor 
c     This is the overall color factor, which is the same of the Born.
c     The individual diagrams' color factors are included in wbb_M2_virt_Wbb
      virtual=virtual*(nc**2-1d0)/4d0
      end


c 
c subroutine that returns 2 Re(M_B * M_V)/(as/(2pi))
c
      subroutine wbb_M2_virt_Wbb(pphy,amp2)
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      include 'src/Lib/wbb/pwhg_st.h'
      include 'src/Lib/wbb/pwhg_math.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/PhysPars.h'
      
      integer nlegs
      parameter (nlegs=5)  
      real *8 pphy(0:3,nlegs)

      real*8 q1(4),q2(4),pb(4),pbb(4),pw(4)      
      real * 8 amp2
      real * 8 kern
      external wbb_dot_4,wbb_CSPEN
      real*8 wbb_dot_4
      complex*16 wbb_CSPEN
c
c various kinematic variables, wbb_constants, etc.
c
      integer j
      real*8 mb,mt,mw,mz,fac
      real*8 xpi,ry,alphas
      complex*16 igauss

      common/wbb_constants/xpi,igauss,ry
      common/wbb_masses/mb,mt,mw,mz

      real * 8 xmur
      real * 8 xmur1,xmuf1,s1,scalemur1,scalemuf1
      common/wbb_scale/xmur1,xmuf1,s1,scalemur1,scalemuf1

      real*8 jac,pqqb
c
c  sigma_born
c
      real*8 ampl2_wbb_lo_1,ampl2_wbb_lo_2,ampl2_wbb_lo,mat2_wbb_lo,
     - kern_b
      real*8 FF,CF0,Nf          !,nc,ca,cf
c
c sigma virtual
c
      integer dflag
      real*8 CFs1f,CFs1g,CV1ab,CV1nab,CV2ab,CV2nab,CB1,CB2,
     - CB3,CB3c,CP1,CP1c
      real*8 mm2_sme_v(75),mm2_sme_a(75)
      complex*16 beta,s2mtc,betab,s2mbc
      real*8 xbb,s2mt,s2mb,dsigma_wbb_s1_f,dsigma_wbb_s1_g,
     - dsigma_wbb_s1_f_ct_z3,dsigma_wbb_s1_g_ct_z3,
     - dsigma_wbb_s1_f_ren,dsigma_wbb_s1_g_ren,kern_s1
      real*8 x1w,x2w,cs2_1,cs2_2,
     - dsigma_wbb_s2,dsigma_wbb_s2_ct_z2,kern_s2
      real*8 ampl2_wbb_v1_1_ab,ampl2_wbb_v1_1_nab,ampl2_wbb_v1_2_ab,
     - ampl2_wbb_v1_2_nab,dsigma_wbb_v1,dsigma_wbb_v1_ct_z1,kern_v1
      real*8 ampl2_wbb_v2_1_ab,ampl2_wbb_v2_1_nab,ampl2_wbb_v2_2_ab,
     - ampl2_wbb_v2_2_nab,dsigma_wbb_v2,dsigma_wbb_v2_ct_z1,kern_v2
      real*8 ampl2_wbb_v3_1,ampl2_wbb_v3_2,dsigma_wbb_v3,
     - dsigma_wbb_v3_ct_zw,kern_v3
      real*8 ampl2_wbb_b1_1,ampl2_wbb_b1_2,dsigma_wbb_b1,kern_b1
      real*8 ampl2_wbb_b2,dsigma_wbb_b2,kern_b2
      real*8 ampl2_wbb_b3_1,dsigma_wbb_b3_1,
     - ampl2_wbb_b3_1c,dsigma_wbb_b3_1c,ampl2_wbb_b3_2,dsigma_wbb_b3_2,
     - ampl2_wbb_b3_2c,dsigma_wbb_b3_2c,kern_b3,
     - kern_b3_1,kern_b3_1c,kern_b3_2,kern_b3_2c
      real*8 ampl2_wbb_p1,dsigma_wbb_p1,kern_p1,ampl2_wbb_p1c,
     - dsigma_wbb_p1c,kern_p1c
      common/wbb_gramdet/dflag
      real*8 s,tau1,tau2,tau3,tau4,sbb,betabb,lambdabb,coeff_dp,
     $     coeff_sp,ll

c
c debug parameter for all virtuals
c
      logical*4 ldebug_virt
      parameter(ldebug_virt=.false.)
c     parameter(ldebug_virt=.true.)

      igauss = dcmplx(0.0d0,1.0d0)
      ry = 0.0000000001d0
      xpi=pi

c set to 1 all wbb_constants now provided by POWHEG
      jac=1d0
      fac=1d0
      pqqb=1d0
c     in this way the returned result (amp2) is as if divided by (as/2pi)
      alphas=2*pi
c input parameters
      mb=ph_mb
      mt=ph_mt
c      mw=ph_Wmass
      mw=kn_masses(3)             ! NOT necessarily equal to the W mass
      mz=0d0

c assign momenta for q(q1)+q'(q2)->W(pw)+b(pb)+bb(pbb)
      do j=1,3
         q1(j)=pphy(j,1)
         q2(j)=pphy(j,2)
         pw(j)=pphy(j,3)
         pb(j)=pphy(j,4)
         pbb(j)=pphy(j,5)
      enddo
      q1(4)=pphy(0,1)
      q2(4)=pphy(0,2)
      pw(4)=pphy(0,3)
      pb(4)=pphy(0,4)
      pbb(4)=pphy(0,5)

      xmur1=sqrt(st_muren2)
      xmur=xmur1
      xmuf1=0
      s1=0
      scalemur1=0
      scalemuf1=0


c Born cross section
      kern_b=0d0
c O(alphas^3) virtual cross section
      kern_s1=0d0
      kern_s2=0d0
      kern_v1=0d0
      kern_v2=0d0
      kern_v3=0d0
      kern_b1=0d0
      kern_b2=0d0
      kern_b3=0d0
      kern_b3_1=0d0
      kern_b3_1c=0d0
      kern_b3_2=0d0
      kern_b3_2c=0d0
      kern_p1=0d0
      kern_p1c=0d0
c
      dflag=0
      
c
c  some color/coupling factors
c  the conversion to pb/fb is done in "config"
c
      FF=1d0
      
      
c      Nc=3d0
c      Nf=5d0
      Nf=4d0
c qq'->Wbb
      CF0=1d0
c      CA=Nc
c      CF=Nc/2d0-1d0/2d0/Nc
      CFs1f=Nf/2d0
      CFs1g=Nc
      CV1ab=-1d0/2d0/Nc
      CV1nab=Nc/2d0
      CV2ab=-1d0/2d0/Nc
      CV2nab=Nc/2d0
      CB1=-1d0/2d0/Nc
      CB2=Nc/2d0
      CB3=(Nc**2-2d0)/2d0/Nc
      CB3c=(-1d0/Nc)
      CP1=(Nc**2-2d0)/2d0/Nc
      CP1c=(-1d0/Nc)

      
c
c call SME
      call wbb_mm2_sme(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a)
c
c  born cross section
c
      call wbb_amp2_wbb_lo_sme(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     -                     ampl2_wbb_lo_1,ampl2_wbb_lo_2)
      ampl2_wbb_lo=ampl2_wbb_lo_1+ampl2_wbb_lo_2
      mat2_wbb_lo=FF*CF0*pqqb*ampl2_wbb_lo*jac
      kern_b=fac*mat2_wbb_lo
c
      if (ldebug_virt) then
         print*,'born = ',kern_b
      endif

c
c  virtual cross section
c
c self-energy S1
c
      xbb=2d0*mb**2+2d0*wbb_dot_4(pb,pbb)
c top-quark loop
      beta=cdsqrt(dcmplx(1d0-4d0*mt**2/xbb))
      s2mtc=dcmplx((1d0-beta**2)+(1d0+(1d0-beta**2)/2d0)*beta*
     -(cdlog(dcmplx((1d0-beta)/(1d0+beta)))+igauss*pi))
      s2mt=dreal(s2mtc)
c bottom-quark loop
      betab=cdsqrt(dcmplx(1d0-4d0*mb**2/xbb))
      s2mbc=dcmplx((1d0-betab**2)+(1d0+(1d0-betab**2)/2d0)*betab*
     -(cdlog(dcmplx((1d0-betab)/(1d0+betab)))+igauss*pi))
      s2mb=dreal(s2mbc)
c top loop only
c      dsigma_wbb_s1_f = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
c     - (CFs1f*(-4d0/3d0)*(5d0/3d0+dlog(mb**2/xbb))+
c     - (1d0/2d0)*(-4d0/3d0)*(5d0/3d0+s2mt+dlog(mb**2/mt**2)))*
c     - ampl2_wbb_lo*jac
c      dsigma_wbb_s1_f_ct_z3 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
c     - ((4d0/3d0)*(CFs1f*dlog(mb**2/xmur**2)+1d0/2d0*dlog(mb**2/mt**2)))
c     - *ampl2_wbb_lo*jac
c bottom and top loop
      dsigma_wbb_s1_f = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     - (CFs1f*(-4d0/3d0)*(5d0/3d0+dlog(mb**2/xbb))+
     - (1d0/2d0)*(-4d0/3d0)*(2d0*5d0/3d0+s2mt+s2mb+dlog(mb**2/mt**2)))*
     - ampl2_wbb_lo*jac
      dsigma_wbb_s1_f_ct_z3 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     - ((4d0/3d0)*((CFs1f+1d0/2d0)*dlog(mb**2/xmur**2)+
     - 1d0/2d0*dlog(mb**2/mt**2)))*ampl2_wbb_lo*jac
      dsigma_wbb_s1_f_ren=dsigma_wbb_s1_f+dsigma_wbb_s1_f_ct_z3
c
      dsigma_wbb_s1_g = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     - (CFs1g*(5d0/3d0)*(31d0/15d0+dlog(mb**2/xbb)))*ampl2_wbb_lo*jac
      dsigma_wbb_s1_g_ct_z3 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     - (CFs1g*(-5d0/3d0)*dlog(mb**2/xmur**2))*ampl2_wbb_lo*jac
      dsigma_wbb_s1_g_ren=dsigma_wbb_s1_g+dsigma_wbb_s1_g_ct_z3
      kern_s1=fac*(dsigma_wbb_s1_f_ren+dsigma_wbb_s1_g_ren)
c
      if (ldebug_virt) then
         print*,'s1 = ',kern_s1/(alphas/2d0/pi)
      endif
c
c self-energy S2
c
      x1w=mw**2-2d0*wbb_dot_4(q1,pw)
      x2w=mw**2-2d0*wbb_dot_4(q2,pw)
      cs2_1=1d0+dlog(dabs(mb**2/x1w))
      cs2_2=1d0+dlog(dabs(mb**2/x2w))      
      dsigma_wbb_s2 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     -         (-CF)*(cs2_1*ampl2_wbb_lo_1+cs2_2*ampl2_wbb_lo_2)*jac      
      dsigma_wbb_s2_ct_z2 = 0d0
      kern_s2=fac*(dsigma_wbb_s2+dsigma_wbb_s2_ct_z2)
c
      if (ldebug_virt) then
         print*,'s2 = ',kern_s2/(alphas/2d0/pi)
      endif
c
c vertex V1
c
      call wbb_amp2_wbb_v1(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     - ampl2_wbb_v1_1_ab,ampl2_wbb_v1_1_nab,ampl2_wbb_v1_2_ab,
     - ampl2_wbb_v1_2_nab)
      dsigma_wbb_v1 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*(
     -          CV1ab*(ampl2_wbb_v1_1_ab+ampl2_wbb_v1_2_ab)+
     -          CV1nab*(ampl2_wbb_v1_1_nab+ampl2_wbb_v1_2_nab))*jac
      dsigma_wbb_v1_ct_z1 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     -         (-CA*dlog(mb**2/xmur**2))*ampl2_wbb_lo*jac
      kern_v1=fac*(dsigma_wbb_v1+dsigma_wbb_v1_ct_z1)
c
      if (ldebug_virt) then
         print*,'v1 = ',kern_v1/(alphas/2d0/pi)
      endif
c
c vertex V2
c
      call wbb_amp2_wbb_v2(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     -    ampl2_wbb_v2_1_ab,ampl2_wbb_v2_1_nab,ampl2_wbb_v2_2_ab,
     -    ampl2_wbb_v2_2_nab)
      dsigma_wbb_v2 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*(
     -          CV2ab*(ampl2_wbb_v2_1_ab+ampl2_wbb_v2_2_ab)+
     -          CV2nab*(ampl2_wbb_v2_1_nab+ampl2_wbb_v2_2_nab))*jac      
      dsigma_wbb_v2_ct_z1 = 2d0*FF*CF0*(alphas/4d0/pi)*pqqb*
     -   (-4d0*CF-CA*dlog(mb**2/xmur**2))*ampl2_wbb_lo*jac
      kern_v2=fac*(dsigma_wbb_v2+dsigma_wbb_v2_ct_z1)
c
      if (ldebug_virt) then
         print*,'v2 = ',kern_v2/(alphas/2d0/pi)
      endif
c
c vertex V3
c
      call wbb_amp2_wbb_v3(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     - ampl2_wbb_v3_1,ampl2_wbb_v3_2)
      dsigma_wbb_v3 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CF*
     -                 (ampl2_wbb_v3_1+ampl2_wbb_v3_2)*jac
      dsigma_wbb_v3_ct_zw = 0d0
      kern_v3=fac*(dsigma_wbb_v3+dsigma_wbb_v3_ct_zw)
c
      if (ldebug_virt) then
         print*,'v3 = ',kern_v3/(alphas/2d0/pi)
      endif
c
c box B1
c
      call wbb_amp2_wbb_b1(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     -                 ampl2_wbb_b1_1,ampl2_wbb_b1_2)
      if (dflag.eq.1) then
         print*,'dflag=1,b1'
         amp2=0
         return
      endif
      dsigma_wbb_b1 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB1*
     -                (ampl2_wbb_b1_1+ampl2_wbb_b1_2)*jac
      kern_b1=fac*dsigma_wbb_b1
c
      if (ldebug_virt) then
         print*,'b1 = ',kern_b1/(alphas/2d0/pi)
      endif
c
c box B2
c
      call wbb_amp2_wbb_b2(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,ampl2_wbb_b2)
      if (dflag.eq.1) then
         print*,'dflag=1,b2'
         amp2=0
         return
      endif
      dsigma_wbb_b2 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB2*
     -               ampl2_wbb_b2*jac
      kern_b2=fac*dsigma_wbb_b2
c
      if (ldebug_virt) then
         print*,'b2 = ',kern_b2/(alphas/2d0/pi)
      endif
c
c box B3
c
      call wbb_amp2_wbb_b3(q1,q2,pb,pbb,pw,mm2_sme_v,mm2_sme_a,
     - ampl2_wbb_b3_1,ampl2_wbb_b3_1c,ampl2_wbb_b3_2,ampl2_wbb_b3_2c)
      if (dflag.eq.1) then
c         print*,'dflag=1,b3'
         amp2=0
         return
      endif
      dsigma_wbb_b3_1 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB3*
     -               ampl2_wbb_b3_1*jac
      dsigma_wbb_b3_1c = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB3c*
     -               ampl2_wbb_b3_1c*jac
      dsigma_wbb_b3_2 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB3*
     -               ampl2_wbb_b3_2*jac
      dsigma_wbb_b3_2c = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CB3c*
     -               ampl2_wbb_b3_2c*jac
      kern_b3_1=fac*dsigma_wbb_b3_1
      kern_b3_1c=fac*dsigma_wbb_b3_1c
      kern_b3_2=fac*dsigma_wbb_b3_2
      kern_b3_2c=fac*dsigma_wbb_b3_2c
      kern_b3=fac*(dsigma_wbb_b3_1+dsigma_wbb_b3_1c+
     -             dsigma_wbb_b3_2+dsigma_wbb_b3_2c)
c
      if (ldebug_virt) then
         print*,'b3 = ',kern_b3/(alphas/2d0/pi)
         print*,'b3_1 = ',fac*dsigma_wbb_b3_1/(alphas/2d0/pi)
         print*,'b3_1c = ',fac*dsigma_wbb_b3_1c/(alphas/2d0/pi)
         print*,'b3_2 = ',fac*dsigma_wbb_b3_2/(alphas/2d0/pi)
         print*,'b3_2c = ',fac*dsigma_wbb_b3_2c/(alphas/2d0/pi)
      endif
c
c pentagon P1
c
      call wbb_amp2_wbb_p1(q1,q2,pb,pbb,pw,ampl2_wbb_p1,ampl2_wbb_p1c)
      if (dflag.eq.1) then
c         print*,'dflag=1,p1'
         amp2=0
         return
      endif
      dsigma_wbb_p1 = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CP1*
     -                ampl2_wbb_p1*jac
      dsigma_wbb_p1c = 2.d0*FF*CF0*(alphas/4.d0/pi)*pqqb*CP1c*
     -                ampl2_wbb_p1c*jac
      kern_p1=fac*dsigma_wbb_p1
      kern_p1c=fac*dsigma_wbb_p1c
c
      if (ldebug_virt) then
         print*,'p1 = ',kern_p1/(alphas/2d0/pi)
         print*,'p1c = ',kern_p1c/(alphas/2d0/pi)
      endif
c
c
      kern=kern_s1+kern_s2+kern_v1+kern_v2+kern_v3+kern_b1+
     -     kern_b2+kern_b3+kern_p1+kern_p1c
      if (ldebug_virt) then
         print*,'kern = ',kern/(alphas/2d0/pi)
      endif
c     
      if (kern.ne.0) then
         coeff_dp=-(Nc-1d0/Nc)
         sbb=2d0*wbb_dot_4(pb,pbb)+2d0*mb**2
         betabb=sqrt(abs(1d0-4d0*mb**2/sbb))
         lambdabb=log(abs((1d0+betabb)/(1d0-betabb)))
         s=wbb_dot_4(q1,q1)+wbb_dot_4(q2,q2)+2d0*wbb_dot_4(q1,q2)
         tau1=2d0*wbb_dot_4(q1,pb)
         tau2=2d0*wbb_dot_4(q2,pbb)
         tau3=2d0*wbb_dot_4(q2,pb)
         tau4=2d0*wbb_dot_4(q1,pbb)
         coeff_sp=Nc*(-5d0/2d0+log(tau1/mb**2)+log(tau2/mb**2))+
     -        1d0/Nc*(-log(s/mb**2)+5d0/2d0
     -        -(sbb-2d0*mb**2)/sbb/betabb*lambdabb
     -        -2d0*log(tau1*tau2/tau3/tau4))
         
         ll=log(xmur**2/mb**2)
         kern=kern+( 
     $        (1d0/2d0*ll**2 + pi**2/6)*coeff_dp + coeff_sp*ll)*kern_b
      endif

      amp2=kern

      end




      subroutine wbb_amp2_wbb_b1(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -                       amp2_b1_1,amp2_b1_2)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 amp2_b1_1,amp2_b1_2
      real*8 cf_b1_1_v(75),cf_b1_1_a(75),cf_b1_2_v(75),cf_b1_2_a(75),
     -       m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
      common/wbb_gramdet/dflag
c
c debug parameter
c
      logical*4 ldebugb1
      parameter(ldebugb1=.false.)
c      parameter(ldebugb1=.true.)
c
      if(ldebugb1) then
      print*,' ------  Debugging B1_1, B1_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_b1(q1,q2,pb,p1b,pw,mb,cf_b1_1_v,cf_b1_1_a,
     -              cf_b1_2_v,cf_b1_2_a)
      if (dflag.eq.1) then
         return
      endif
c
      amp2_b1_1 = 0d0
      amp2_b1_2 = 0d0
c
      do i=1,75
      amp2_b1_1 = amp2_b1_1 + cf_b1_1_v(i)*m2_sme_v(i) + 
     -                        cf_b1_1_a(i)*m2_sme_a(i)
      amp2_b1_2 = amp2_b1_2 + cf_b1_2_v(i)*m2_sme_v(i) + 
     -                        cf_b1_2_a(i)*m2_sme_a(i)
      enddo
c
      amp2_b1_1 = 1d0/4d0/9d0*amp2_b1_1
      amp2_b1_2 = 1d0/4d0/9d0*amp2_b1_2
c
      if(ldebugb1) then
        print*,'amp2_b1_1 = ',amp2_b1_1
        print*,'amp2_b1_2 = ',amp2_b1_2
      endif
c
      return
      end



      subroutine wbb_amp2_wbb_b2(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,amp2_b2)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 amp2_b2
      real*8 cf_b2_v(75),cf_b2_a(75),m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
      common/wbb_gramdet/dflag
c
c debug parameter
c
      logical*4 ldebugb2
      parameter(ldebugb2=.false.)
c      parameter(ldebugb2=.true.)
c
      if(ldebugb2) then
      print*,' ------  Debugging B2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_b2(q1,q2,pb,p1b,pw,mb,cf_b2_v,cf_b2_a)
      if (dflag.eq.1) then
         return
      endif
c
      amp2_b2 = 0d0
c
      do i=1,75
      amp2_b2 = amp2_b2 + cf_b2_v(i)*m2_sme_v(i) + 
     -                    cf_b2_a(i)*m2_sme_a(i)
      enddo
c
      amp2_b2 = 1d0/4d0/9d0*amp2_b2
c
      if(ldebugb2) then
        print*,'amp2_b2 = ',amp2_b2
        print*,'amp2_b2 = ',amp2_b2
      endif
c
      return
      end

      subroutine wbb_amp2_wbb_b3(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -               amp2_b3_1,amp2_b3_1c,amp2_b3_2,amp2_b3_2c)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 amp2_b3_1,amp2_b3_1c,amp2_b3_2,amp2_b3_2c
      real*8 cf_b3_1_v(75),cf_b3_1_a(75),cf_b3_1c_v(75),cf_b3_1c_a(75),
     -       cf_b3_2_v(75),cf_b3_2_a(75),cf_b3_2c_v(75),cf_b3_2c_a(75),
     -       m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
      common/wbb_gramdet/dflag
c
c debug parameter
c
      logical*4 ldebugb3
      parameter(ldebugb3=.false.)
c      parameter(ldebugb3=.true.)
c
      if(ldebugb3) then
      print*,' ------  Debugging B3  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_b3(q1,q2,pb,p1b,pw,mb,cf_b3_1_v,cf_b3_1_a,
     - cf_b3_1c_v,cf_b3_1c_a,cf_b3_2_v,cf_b3_2_a,cf_b3_2c_v,cf_b3_2c_a)
c
      if (dflag.eq.1) then
         return
      endif
c
      amp2_b3_1 = 0d0
      amp2_b3_1c = 0d0
      amp2_b3_2 = 0d0
      amp2_b3_2c = 0d0
c
      do i=1,75
      amp2_b3_1 = amp2_b3_1 + cf_b3_1_v(i)*m2_sme_v(i) + 
     -                        cf_b3_1_a(i)*m2_sme_a(i)
      amp2_b3_1c = amp2_b3_1c + cf_b3_1c_v(i)*m2_sme_v(i) + 
     -                          cf_b3_1c_a(i)*m2_sme_a(i)
      amp2_b3_2 = amp2_b3_2 + cf_b3_2_v(i)*m2_sme_v(i) + 
     -                        cf_b3_2_a(i)*m2_sme_a(i)
      amp2_b3_2c = amp2_b3_2c + cf_b3_2c_v(i)*m2_sme_v(i) + 
     -                          cf_b3_2c_a(i)*m2_sme_a(i)
      enddo
c
      amp2_b3_1 = 1d0/4d0/9d0*amp2_b3_1
      amp2_b3_1c = 1d0/4d0/9d0*amp2_b3_1c
      amp2_b3_2 = 1d0/4d0/9d0*amp2_b3_2
      amp2_b3_2c = 1d0/4d0/9d0*amp2_b3_2c
c
      if(ldebugb3) then
        print*,'amp2_b3_1 = ',amp2_b3_1
        print*,'amp2_b3_1c = ',amp2_b3_1c
        print*,'amp2_b3_2 = ',amp2_b3_2
        print*,'amp2_b3_2c = ',amp2_b3_2c
      endif
c
      return
      end

      subroutine wbb_amp2_wbb_lo(q1,q2,pb,p1b,pw,ampl2_wbb_lo)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 q1(4),q2(4),pb(4),p1b(4),pw(4),mb,mt,mw,mz
      real*8 CV,CA,den0x1,den0x2,den0x3,q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,
     -       pbDp1b,s1,s2,s3,s4,s5,s6,t0,ampl2_wbb_lo
      common/wbb_masses/mb,mt,mw,mz
c
      logical*4 ldebug
      parameter(ldebug=.false.)
c      parameter(ldebug=.true.)
c
      CV=1d0
      CA=-1d0
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
c
      s2 = CV**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb
     #*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp
     #1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b)
      s3 = CV**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b+384*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b+
     #384*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+384*q1Dq2*q2Dp1b*mb*
     #*2-256*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2-512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b-256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s1 = s2+s3
      s2 = s1+CV**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq
     #2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q
     #1Dq2*q1Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb
     #*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q
     #1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q
     #1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b**2*q2Dpb)
      s3 = s2
      s5 = CA**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb
     #*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp
     #1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b)
      s6 = CA**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b+384*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b+
     #384*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+384*q1Dq2*q2Dp1b*mb*
     #*2-256*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2-512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b-256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s4 = s5+s6
      t0 = s3+s4
      s2 = CV**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+128*q1Dpb*
     #q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1D
     #p1b*q2Dp1b*mb**2)
      s4 = CV**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-
     #128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb*
     #*2+768*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b+256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s5 = CV**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q
     #1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq
     #2*q1Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q1
     #Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dp
     #b*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*mb**2+128
     #*q1Dp1b**2*q2Dpb)
      s3 = s4+s5
      s1 = s2+s3
      s2 = s1+CA**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq
     #2*q2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q
     #1Dq2*q2Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb
     #*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+128*q1D
     #pb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1
     #Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*
     #q1Dp1b*q2Dp1b*mb**2)
      s3 = s2
      s5 = CA**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-
     #128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb*
     #*2+768*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b+256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s6 = CA**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q
     #1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq
     #2*q1Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q1
     #Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dp
     #b*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*mb**2+128
     #*q1Dp1b**2*q2Dpb)
      s4 = s5+s6
      t0 = s3+s4
c
      ampl2_wbb_lo=t0/4d0/9d0
c
      if (ldebug) then
         print*,'ampl2_wbb_lo = ',ampl2_wbb_lo
      endif
c
      return
      end
      subroutine wbb_amp2_wbb_lo_sme(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -                           amp2_lo_1,amp2_lo_2)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 CV,CA,den0x1,den0x2,den0x3
      real*8 amp2_lo_1,amp2_lo_2
      real*8 cf_0_1_v(75),cf_0_2_v(75),cf_0_1_a(75),cf_0_2_a(75),
     -       m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
c
c debug parameter
c
      logical*4 ldebuglo
      parameter(ldebuglo=.false.)
c      parameter(ldebuglo=.true.)
c
      if(ldebuglo) then
      print*,' ------  Debugging A0_1, A0_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
c      
      do i=1,75
         cf_0_1_v(i)=0d0
         cf_0_2_v(i)=0d0
         cf_0_1_a(i)=0d0
         cf_0_2_a(i)=0d0
      enddo
c
      cf_0_1_v(1)=0d0
      cf_0_1_v(2)=0d0
      cf_0_1_v(3)=CV*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_v(4)=CV*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_v(5)=CV*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_v(6)=0d0
      cf_0_1_v(7)=0d0
      cf_0_1_v(8)=0d0
      cf_0_1_v(9)=CV*den0x1**(-1)*den0x3**(-1)*( 2d0 )
      cf_0_1_v(10)=CV*den0x1**(-1)*den0x3**(-1)*( 2d0 )
      cf_0_1_v(11)=CV*den0x1**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_1_v(12)=CV*den0x1**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_1_v(13)=0d0
      cf_0_1_v(14)=0d0
      cf_0_1_v(15)=0d0
c
      cf_0_2_v(1)=0d0
      cf_0_2_v(2)=CV*den0x2**(-1)*den0x3**(-1)*( 2d0 )
      cf_0_2_v(3)=0d0
      cf_0_2_v(4)=0d0
      cf_0_2_v(5)=0d0
      cf_0_2_v(6)=0d0
      cf_0_2_v(7)=0d0
      cf_0_2_v(8)=0d0
      cf_0_2_v(9)=0d0
      cf_0_2_v(10)=0d0
      cf_0_2_v(11)=CV*den0x2**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_2_v(12)=CV*den0x2**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_2_v(13)=0d0
      cf_0_2_v(14)=0d0
      cf_0_2_v(15)=0d0
c
      cf_0_1_a(1)=0d0
      cf_0_1_a(2)=0d0
      cf_0_1_a(3)=CA*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_a(4)=CA*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_a(5)=CA*den0x1**(-1)*den0x3**(-1)*( - 2d0 )
      cf_0_1_a(6)=0d0
      cf_0_1_a(7)=0d0
      cf_0_1_a(8)=0d0
      cf_0_1_a(9)=CA*den0x1**(-1)*den0x3**(-1)*( 2d0 )
      cf_0_1_a(10)=CA*den0x1**(-1)*den0x3**(-1)*( 2d0 )
      cf_0_1_a(11)=CA*den0x1**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_1_a(12)=CA*den0x1**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_1_a(13)=0d0
      cf_0_1_a(14)=0d0
      cf_0_1_a(15)=0d0
c
      cf_0_2_a(1)=0d0
      cf_0_2_a(2)=CA*den0x2**(-1)*den0x3**(-1)* ( 2d0 )
      cf_0_2_a(3)=0d0
      cf_0_2_a(4)=0d0
      cf_0_2_a(5)=0d0
      cf_0_2_a(6)=0d0
      cf_0_2_a(7)=0d0
      cf_0_2_a(8)=0d0
      cf_0_2_a(9)=0d0
      cf_0_2_a(10)=0d0
      cf_0_2_a(11)=CA*den0x2**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_2_a(12)=CA*den0x2**(-1)*den0x3**(-1)*( 1d0 )
      cf_0_2_a(13)=0d0
      cf_0_2_a(14)=0d0
      cf_0_2_a(15)=0d0
c
      amp2_lo_1 = 0d0
      amp2_lo_2 = 0d0
c
      do i=1,75
      amp2_lo_1 = amp2_lo_1 + cf_0_1_v(i)*m2_sme_v(i) + 
     -                        cf_0_1_a(i)*m2_sme_a(i)
      amp2_lo_2 = amp2_lo_2 + cf_0_2_v(i)*m2_sme_v(i) +
     -                        cf_0_2_a(i)*m2_sme_a(i)
      enddo
c
      amp2_lo_1 = 1d0/4d0/9d0*amp2_lo_1
      amp2_lo_2 = 1d0/4d0/9d0*amp2_lo_2
c
      if(ldebuglo) then
        print*,'amp2_lo_1 = ',amp2_lo_1
        print*,'amp2_lo_2 = ',amp2_lo_2
      endif
c
      return
      end

      subroutine wbb_amp2_wbb_p1(q1,q2,pb,p1b,pw,amp2_p1,amp2_p1c)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd,wbb_pent1,wbb_pent1c
      real*8 wbb_dot_4,wbb_c0_sd,wbb_pent1,wbb_pent1c
c miscellanea
      integer i,dflag
      common/wbb_gramdet/dflag
c kinematics
      real*8 q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 p1(4),p2(4),p3(4),p4(4),p12(4),p23(4),p34(4),p123(4),
     - p234(4),s12,s23,s34,t123,t234,m1,m2,m3,m4,m5,xs1,xs2,xs12,xs3
      real*8 mb,mt,mw,mz
      common/wbb_masses/mb,mt,mw,mz
      real*8 amp2_p1,amp2_p1c
c scalar and tensor integrals
      real*8 D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     - A1d2_2345,A2d2_2345,A3d2_2345,A4d2_2345,A5d2_2345,A6d2_2345,
     - A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     - A1d2_1345,A2d2_1345,A3d2_1345,A4d2_1345,
     - A5d2_1345,A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     - A1d2_1245,A2d2_1245,A3d2_1245,A4d2_1245,
     - A5d2_1245,A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     - A1d2_1235,A2d2_1235,A3d2_1235,A4d2_1235,
     - A5d2_1235,A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     - A1d2_1234,A2d2_1234,A3d2_1234,A4d2_1234,
     - A5d2_1234,A6d2_1234,A7d2_1234,
     - E0,A1e1,A2e1,A3e1,A4e1
      real*8 C0234,A1c1234,A2c1234,!A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,!A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,!A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,!A1c2123,A2c2123,A3c2123,A4c2123,
     - B012,B013,B023!,B112,B113,B123
      real*8 D1_1,D1_2,D1_3,D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
c debug parameter
      logical*4 ldebugp1
      parameter(ldebugp1=.false.)
c      parameter(ldebugp1=.true.)
c
c ----------------------------- P1 ----------------------------------------
c
c 4PF's
c
c --> D0_2345,D1_2345,D2_2345
c
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pw(i)
         p3(i)=q2(i)-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=mb
c  ---> C0234
      xs1 = wbb_dot_4(p2,p2)
      xs2 = wbb_dot_4(p3,p3)
      xs12 = wbb_dot_4(p23,p23)
      C0234 = wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m_on(xs2,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p3,p3)
      call wbb_f4pf_p1(xs1,xs2,xs3,D0_2345)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_2345,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_2345=D1_1
         A2d1_2345=D1_2
         A3d1_2345=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_2345 = D2_11
         A2d2_2345 = D2_22
         A3d2_2345 = D2_33
         A4d2_2345 = D2_12
         A5d2_2345 = D2_13
         A6d2_2345 = D2_23
         A7d2_2345 = D2_0
c
c --> D0_1345,D1_1345,D2_1345
c
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pb(i)
         p3(i)=-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs3,D0_1345)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1345,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1345=D1_1
         A2d1_1345=D1_2
         A3d1_1345=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1345 = D2_11
         A2d2_1345 = D2_22
         A3d2_1345 = D2_33
         A4d2_1345 = D2_12
         A5d2_1345 = D2_13
         A6d2_1345 = D2_23
         A7d2_1345 = D2_0
c
c --> D0_1245,D1_1245,D2_1245
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-p1b(i)
         p3(i)=-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c     call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs3,D0_1245)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1245,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1245=D1_1
         A2d1_1245=D1_2
         A3d1_1245=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1245 = D2_11
         A2d2_1245 = D2_22
         A3d2_1245 = D2_33
         A4d2_1245 = D2_12
         A5d2_1245 = D2_13
         A6d2_1245 = D2_23
         A7d2_1245 = D2_0
c
c --> D0_1235,D1_1235,D2_1235
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=mb
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m_on(xs2,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p3,p3)
      call wbb_f4pf_p1(xs1,xs2,xs3,D0_1235)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1235,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1235=D1_1
         A2d1_1235=D1_2
         A3d1_1235=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1235 = D2_11
         A2d2_1235 = D2_22
         A3d2_1235 = D2_33
         A4d2_1235 = D2_12
         A5d2_1235 = D2_13
         A6d2_1235 = D2_23
         A7d2_1235 = D2_0
c
c --> D0_1234,D1_1234,D2_1234
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      call wbb_f3pf_2m(xs12,xs1,C0234)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
c      B123=0d0
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs12,xs1,C0134)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
c      B123=0d0
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      s12=wbb_dot_4(p12,p12)
      s23=wbb_dot_4(p23,p23)
      t123=wbb_dot_4(p123,p123)
      call wbb_f4pf_b2(s23,s12,mw**2,t123,D0_1234)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1234,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1234=D1_1
         A2d1_1234=D1_2
         A3d1_1234=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1234 = D2_11
         A2d2_1234 = D2_22
         A3d2_1234 = D2_33
         A4d2_1234 = D2_12
         A5d2_1234 = D2_13
         A6d2_1234 = D2_23
         A7d2_1234 = D2_0
c
c 5PF: E0
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)
         p4(i)=-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p34(i)=p3(i)+p4(i)
         p123(i)=p1(i)+p2(i)+p3(i)
         p234(i)=p2(i)+p3(i)+p4(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
      m5=mb
c
      s12=wbb_dot_4(p12,p12)
      s23=wbb_dot_4(p23,p23)
      s34=wbb_dot_4(p34,p34)
      t123=wbb_dot_4(p123,p123)
      t234=wbb_dot_4(p234,p234)
c
      call wbb_f5pf_p1(s12,s23,s34,t123,t234,D0_2345,D0_1345,D0_1245,
     -             D0_1235,D0_1234,E0)
c
      call wbb_e1_new(p1,p2,p3,p4,m1,m2,m3,m4,m5,
     - E0,D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,A1e1,A2e1,A3e1,A4e1)
      if (dflag.eq.1) then
         return
      endif
c
      if(ldebugp1) then
         print*,'A1e1 = ', A1e1
         print*,'A2e1 = ', A2e1
         print*,'A3e1 = ', A3e1
         print*,'A4e1 = ', A4e1
         print*,'-----------------'
      endif
c
      amp2_p1=wbb_pent1(q1,q2,pb,p1b,pw,mb,
     - E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     - A1d2_2345,A2d2_2345,A3d2_2345,A4d2_2345,A5d2_2345,
     - A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     - A1d2_1345,A2d2_1345,A3d2_1345,A4d2_1345,A5d2_1345,
     - A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     - A1d2_1245,A2d2_1245,A3d2_1245,A4d2_1245,A5d2_1245,
     - A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     - A1d2_1235,A2d2_1235,A3d2_1235,A4d2_1235,A5d2_1235,
     - A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     - A1d2_1234,A2d2_1234,A3d2_1234,A4d2_1234,A5d2_1234,
     - A6d2_1234,A7d2_1234)
c
c --------------------------- P1c -----------------------------------
c
c 4PF's
c
c --> D0_2345,D1_2345,D2_2345
c
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pw(i)
         p3(i)=q2(i)-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=mb
c  ---> C0234
      xs1 = wbb_dot_4(p2,p2)
      xs2 = wbb_dot_4(p3,p3)
      xs12 = wbb_dot_4(p23,p23)
      C0234 = wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m_on(xs2,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p3,p3)
      call wbb_f4pf_p1(xs1,xs2,xs3,D0_2345)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_2345,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_2345=D1_1
         A2d1_2345=D1_2
         A3d1_2345=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_2345 = D2_11
         A2d2_2345 = D2_22
         A3d2_2345 = D2_33
         A4d2_2345 = D2_12
         A5d2_2345 = D2_13
         A6d2_2345 = D2_23
         A7d2_2345 = D2_0
c
c --> D0_1345,D1_1345,D2_1345
c
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-p1b(i)
         p3(i)=-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs3,D0_1345)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1345,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1345=D1_1
         A2d1_1345=D1_2
         A3d1_1345=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1345 = D2_11
         A2d2_1345 = D2_22
         A3d2_1345 = D2_33
         A4d2_1345 = D2_12
         A5d2_1345 = D2_13
         A6d2_1345 = D2_23
         A7d2_1345 = D2_0
c
c --> D0_1245,D1_1245,D2_1245
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pb(i)
         p3(i)=-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs3,D0_1245)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1245,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1245=D1_1
         A2d1_1245=D1_2
         A3d1_1245=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1245 = D2_11
         A2d2_1245 = D2_22
         A3d2_1245 = D2_33
         A4d2_1245 = D2_12
         A5d2_1245 = D2_13
         A6d2_1245 = D2_23
         A7d2_1245 = D2_0
c
c --> D0_1235,D1_1235,D2_1235
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=mb
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m_on(xs2,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs3=wbb_dot_4(p3,p3)
      call wbb_f4pf_p1(xs1,xs2,xs3,D0_1235)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1235,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1235=D1_1
         A2d1_1235=D1_2
         A3d1_1235=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1235 = D2_11
         A2d2_1235 = D2_22
         A3d2_1235 = D2_33
         A4d2_1235 = D2_12
         A5d2_1235 = D2_13
         A6d2_1235 = D2_23
         A7d2_1235 = D2_0
c
c --> D0_1234,D1_1234,D2_1234
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
c  ---> C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      call wbb_f3pf_2m(xs12,xs1,C0234)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
c      B123=0d0
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
c      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
c     -            A4c2234,A1c2234,A2c2234,A3c2234)
c  ---> C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs12,xs1,C0134)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
c      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
c      B123=0d0
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
c      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
c     -            A4c2134,A1c2134,A2c2134,A3c2134)
c  ---> C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
c      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
c     -            A4c2124,A1c2124,A2c2124,A3c2124)
c  ---> C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
c      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
c      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
c      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
c      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
c     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
      s12=wbb_dot_4(p12,p12)
      s23=wbb_dot_4(p23,p23)
      t123=wbb_dot_4(p123,p123)
      call wbb_f4pf_b2(s23,s12,mw**2,t123,D0_1234)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1234,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
      if (dflag.eq.1) then
         return
      endif
c
         A1d1_1234=D1_1
         A2d1_1234=D1_2
         A3d1_1234=D1_3
c
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -   D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     -   C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,
     -   C0124,A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
      if (dflag.eq.1) then
         return
      endif
c
         A1d2_1234 = D2_11
         A2d2_1234 = D2_22
         A3d2_1234 = D2_33
         A4d2_1234 = D2_12
         A5d2_1234 = D2_13
         A6d2_1234 = D2_23
         A7d2_1234 = D2_0
c
c 5PF: E0
c
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pw(i)
         p3(i)=q1(i)
         p4(i)=-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p34(i)=p3(i)+p4(i)
         p123(i)=p1(i)+p2(i)+p3(i)
         p234(i)=p2(i)+p3(i)+p4(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
      m5=mb
c
      s12=wbb_dot_4(p12,p12)
      s23=wbb_dot_4(p23,p23)
      s34=wbb_dot_4(p34,p34)
      t123=wbb_dot_4(p123,p123)
      t234=wbb_dot_4(p234,p234)
c
      call wbb_f5pf_p1(s12,s23,s34,t123,t234,D0_2345,D0_1345,D0_1245,
     -             D0_1235,D0_1234,E0)
c
      call wbb_e1_new(p1,p2,p3,p4,m1,m2,m3,m4,m5,
     - E0,D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,A1e1,A2e1,A3e1,A4e1)
      if (dflag.eq.1) then
         return
      endif
c
      if(ldebugp1) then
         print*,'A1e1 = ', A1e1
         print*,'A2e1 = ', A2e1
         print*,'A3e1 = ', A3e1
         print*,'A4e1 = ', A4e1
         print*,'-----------------'
      endif
c
      amp2_p1c=wbb_pent1c(q1,q2,pb,p1b,pw,mb,
     - E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     - A1d2_2345,A2d2_2345,A3d2_2345,A4d2_2345,A5d2_2345,
     - A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     - A1d2_1345,A2d2_1345,A3d2_1345,A4d2_1345,A5d2_1345,
     - A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     - A1d2_1245,A2d2_1245,A3d2_1245,A4d2_1245,A5d2_1245,
     - A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     - A1d2_1235,A2d2_1235,A3d2_1235,A4d2_1235,A5d2_1235,
     - A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     - A1d2_1234,A2d2_1234,A3d2_1234,A4d2_1234,A5d2_1234,
     - A6d2_1234,A7d2_1234)
c
      return
      end
      subroutine wbb_amp2_wbb_v1(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -  amp2_v1_1_ab,amp2_v1_1_nab,amp2_v1_2_ab,amp2_v1_2_nab)
c
      implicit none
c
      external wbb_dot_4
      real * 8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i
      real * 8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real * 8 amp2_v1_1_ab,amp2_v1_1_nab,amp2_v1_2_ab,
     - amp2_v1_2_nab
      real * 8 cf_v1_1_ab_v(75),cf_v1_1_ab_a(75),
     - cf_v1_1_nab_v(75),cf_v1_1_nab_a(75),cf_v1_2_ab_v(75),
     - cf_v1_2_ab_a(75),cf_v1_2_nab_v(75),cf_v1_2_nab_a(75),
     - m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
c
c debug parameter
c
      logical ldebugv1
      parameter(ldebugv1=.false.)
c      parameter(ldebugv1=.true.)
c
      if(ldebugv1) then
      print*,' ------  Debugging V1_1, V1_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_v1(q1,q2,pb,p1b,pw,mb,cf_v1_1_ab_v,cf_v1_1_ab_a,
     -     cf_v1_1_nab_v,cf_v1_1_nab_a,cf_v1_2_ab_v,cf_v1_2_ab_a,
     -     cf_v1_2_nab_v,cf_v1_2_nab_a)
c
      amp2_v1_1_ab = 0d0
      amp2_v1_1_nab = 0d0
      amp2_v1_2_ab = 0d0
      amp2_v1_2_nab = 0d0
c
      do i=1,75
      amp2_v1_1_ab = amp2_v1_1_ab + cf_v1_1_ab_v(i)*m2_sme_v(i) + 
     -                              cf_v1_1_ab_a(i)*m2_sme_a(i)
      amp2_v1_1_nab = amp2_v1_1_nab + cf_v1_1_nab_v(i)*m2_sme_v(i) + 
     -                                cf_v1_1_nab_a(i)*m2_sme_a(i)
      amp2_v1_2_ab = amp2_v1_2_ab + cf_v1_2_ab_v(i)*m2_sme_v(i) + 
     -                              cf_v1_2_ab_a(i)*m2_sme_a(i)
      amp2_v1_2_nab = amp2_v1_2_nab + cf_v1_2_nab_v(i)*m2_sme_v(i) + 
     -                                cf_v1_2_nab_a(i)*m2_sme_a(i)
      enddo
c
      amp2_v1_1_ab = 1d0/4d0/9d0*amp2_v1_1_ab
      amp2_v1_1_nab = 1d0/4d0/9d0*amp2_v1_1_nab
      amp2_v1_2_ab = 1d0/4d0/9d0*amp2_v1_2_ab
      amp2_v1_2_nab = 1d0/4d0/9d0*amp2_v1_2_nab
c
      if(ldebugv1) then
        print*,'amp2_v1_1_ab = ',amp2_v1_1_ab
        print*,'amp2_v1_1_nab = ',amp2_v1_1_nab
        print*,'amp2_v1_2_ab = ',amp2_v1_2_ab
        print*,'amp2_v1_2_nab = ',amp2_v1_2_nab
      endif
c
      return
      end

      subroutine wbb_amp2_wbb_v2(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -    amp2_v2_1_ab,amp2_v2_1_nab,amp2_v2_2_ab,amp2_v2_2_nab)
c
      implicit none
c
      external wbb_dot_4
      real * 8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i
      real * 8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real * 8 amp2_v2_1_ab,amp2_v2_1_nab,amp2_v2_2_ab,
     - amp2_v2_2_nab
      real * 8 cf_v2_1_ab_v(75),cf_v2_1_ab_a(75),
     - cf_v2_1_nab_v(75),cf_v2_1_nab_a(75),cf_v2_2_ab_v(75),
     - cf_v2_2_ab_a(75),cf_v2_2_nab_v(75),cf_v2_2_nab_a(75),
     - m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
c
c debug parameter
c
      logical ldebugv2
      parameter(ldebugv2=.false.)
c      parameter(ldebugv2=.true.)
c
      if(ldebugv2) then
      print*,' ------  Debugging V2_1, V2_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_v2(q1,q2,pb,p1b,pw,mb,cf_v2_1_ab_v,cf_v2_1_ab_a,
     -     cf_v2_1_nab_v,cf_v2_1_nab_a,cf_v2_2_ab_v,cf_v2_2_ab_a,
     -     cf_v2_2_nab_v,cf_v2_2_nab_a)
c
      amp2_v2_1_ab = 0d0
      amp2_v2_1_nab = 0d0
      amp2_v2_2_ab = 0d0
      amp2_v2_2_nab = 0d0
c
      do i=1,75
      amp2_v2_1_ab = amp2_v2_1_ab + cf_v2_1_ab_v(i)*m2_sme_v(i) + 
     -                              cf_v2_1_ab_a(i)*m2_sme_a(i)
      amp2_v2_1_nab = amp2_v2_1_nab + cf_v2_1_nab_v(i)*m2_sme_v(i) + 
     -                                cf_v2_1_nab_a(i)*m2_sme_a(i)
      amp2_v2_2_ab = amp2_v2_2_ab + cf_v2_2_ab_v(i)*m2_sme_v(i) + 
     -                              cf_v2_2_ab_a(i)*m2_sme_a(i)
      amp2_v2_2_nab = amp2_v2_2_nab + cf_v2_2_nab_v(i)*m2_sme_v(i) + 
     -                                cf_v2_2_nab_a(i)*m2_sme_a(i)
      enddo
c
      amp2_v2_1_ab = 1d0/4d0/9d0*amp2_v2_1_ab
      amp2_v2_1_nab = 1d0/4d0/9d0*amp2_v2_1_nab
      amp2_v2_2_ab = 1d0/4d0/9d0*amp2_v2_2_ab
      amp2_v2_2_nab = 1d0/4d0/9d0*amp2_v2_2_nab
c
      if(ldebugv2) then
        print*,'amp2_v2_1_ab = ',amp2_v2_1_ab
        print*,'amp2_v2_1_nab = ',amp2_v2_1_nab
        print*,'amp2_v2_2_ab = ',amp2_v2_2_ab
        print*,'amp2_v2_2_nab = ',amp2_v2_2_nab
      endif
c
      return
      end

      subroutine wbb_amp2_wbb_v3(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a,
     -                       amp2_v3_1,amp2_v3_2)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 amp2_v3_1,amp2_v3_2
      real*8 cf_v3_1_v(75),cf_v3_1_a(75),cf_v3_2_v(75),cf_v3_2_a(75),
     -       m2_sme_v(75),m2_sme_a(75)
      common/wbb_masses/mb,mt,mw,mz
c
c debug parameter
c
      logical*4 ldebugv3
      parameter(ldebugv3=.false.)
c      parameter(ldebugv3=.true.)
c
      if(ldebugv3) then
      print*,' ------  Debugging V3_1, V3_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      endif
c
      call wbb_coeff_v3(q1,q2,pb,p1b,pw,mb,cf_v3_1_v,cf_v3_1_a,
     -     cf_v3_2_v,cf_v3_2_a)
c
      amp2_v3_1 = 0d0
      amp2_v3_2 = 0d0
c
      do i=1,75
      amp2_v3_1 = amp2_v3_1 + cf_v3_1_v(i)*m2_sme_v(i) + 
     -                        cf_v3_1_a(i)*m2_sme_a(i)
      amp2_v3_2 = amp2_v3_2 + cf_v3_2_v(i)*m2_sme_v(i) + 
     -                        cf_v3_2_a(i)*m2_sme_a(i)
      enddo
c
      amp2_v3_1 = 1d0/4d0/9d0*amp2_v3_1
      amp2_v3_2 = 1d0/4d0/9d0*amp2_v3_2
c
      if(ldebugv3) then
        print*,'amp2_v3_1 = ',amp2_v3_1
        print*,'amp2_v3_2 = ',amp2_v3_2
      endif
c
      return
      end

      subroutine wbb_coeff_b1(q1,q2,pb,p1b,pw,mb,cf_b1_1_v,cf_b1_1_a,
     -                    cf_b1_2_v,cf_b1_2_a)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd
      real*8 wbb_dot_4,wbb_c0_sd
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,m1,m2,m3,m4,q1(4),q2(4),pb(4),p1b(4),pw(4),p1(4),
     -       p2(4),p3(4),p12(4),p23(4),p123(4),xs1,xs2,xs12
      real*8 CV,CA,den0x1,den0x2,den0x3,
     -       q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,s1,s2,s3,s4,s5,t0
      real*8 AAi,B012,B112,B013,B113,B023,B123,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,A12d3,
     - A13d3,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real*8 cf_b1_1_v(75),cf_b1_1_a(75),cf_b1_2_v(75),cf_b1_2_a(75)
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
      common/wbb_gramdet/dflag
c
c debug parameter
c
      logical*4 ldebugb1
      parameter(ldebugb1=.false.)
c      parameter(ldebugb1=.true.)
c
      if(ldebugb1) then
      print*,' ------  Debugging coeff_b1_1, coeff_b1_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'pwx = ',pw(1),'pwy = ',pw(2),'pwz = ',pw(3),
     -       'Ew = ',pw(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for B1_1 ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pw(i)
         p3(i)=-pb(i)-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
C --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_1m(xs2,C0124)
      B012=0d0
      B112=0d0
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs1,xs2,C0134)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c --- D0
      call wbb_f4pf_b1(p1,p2,p3,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b1_1_v(i)=0d0
         cf_b1_1_a(i)=0d0
      enddo
c
c ---------------------- coefficients for B1_1_v
c
c --------cf_b1_1_1v
c
      cf_b1_1_v(1)=0d0
c
c --------cf_b1_1_2v
c
      t0 = CV/den0x3*(-8*mb**2*A2d1+8*mb**2*A3d1-8*mb**2*A3d2-8*mb**2*A4
     #d2+8*mb**2*A5d2+8*mb**2*A6d2-8*A7d2-8*A1d3+8*A2d3+8*q2Dpb*A2d1-8*q
     #2Dpb*A3d1+8*q2Dpb*A4d2-8*q2Dpb*A6d2+8*q2Dp1b*A2d1-8*q2Dp1b*A3d1+8*
     #q2Dp1b*A4d2-8*q2Dp1b*A6d2-8*pbDp1b*A2d1+8*pbDp1b*A3d1-8*pbDp1b*A3d
     #2-8*pbDp1b*A4d2+8*pbDp1b*A5d2+8*pbDp1b*A6d2)
c
      cf_b1_1_v(2)=t0
c
c --------cf_b1_1_3v
c
      s1 = CV
      s3 = 1/den0x3
      s5 = 8*q1Dq2*A2d1+8*q1Dp1b*A13d3-8*q2Dp1b*A3d1+8*mb**2*A3d1-16*q1D
     #pb*A4d2+8*q1Dq2*D0+8*q1Dp1b*A5d3-16*mb**2*A2d2-8*q2Dp1b*A10d3+8*q1
     #Dpb*A3d1+8*q2Dp1b*A5d3+8*q2Dpb*A5d3-16*q2Dp1b*A6d2-8*pbDp1b*A12d3+
     #8*q1Dp1b*A2d2+8*q2Dpb*A2d1-16*A2d3+24*mb**2*A6d2-16*pbDp1b*A2d2-16
     #*A7d2+24*pbDp1b*A6d2-8*q1Dp1b*A10d3+8*pbDp1b*A3d1-8*mb**2*A2d1-8*p
     #bDp1b*A3d2+8*q1Dq2*A9d3-16*q1Dp1b*A4d2
      s4 = -8*q2Dpb*A10d3-8*mb**2*A3d2+16*q1Dq2*A4d2-8*mb**2*A12d3-8*q1D
     #p1b*A9d3+8*q1Dq2*A1d1+16*q2Dp1b*A2d2-8*q2Dpb*A3d1-8*mb**2*A5d3+8*q
     #1Dp1b*A3d1+8*q1Dpb*A2d2-8*q1Dq2*A2d2+8*q1Dpb*A13d3+8*q1Dp1b*A5d2+8
     #*q1Dpb*A5d3-8*q1Dp1b*A2d1-8*q1Dq2*A5d3-8*q1Dpb*A9d3-8*pbDp1b*A2d1-
     #8*q1Dpb*A2d1-8*q1Dpb*A10d3+16*mb**2*A10d3-8*pbDp1b*A5d3-16*q2Dpb*A
     #6d2+8*q1Dpb*A5d2+16*q2Dpb*A2d2+16*pbDp1b*A10d3+8*q2Dp1b*A2d1+s5
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_v(3)=t0
c
c --------cf_b1_1_4v
c
      t0 = CV/den0x3*(-8*A7d2-8*A2d3+8*A3d3+8*q1Dq2*D0+8*q1Dq2*A1d1)
c
      cf_b1_1_v(4)=t0
c
c --------cf_b1_1_5v
c
      t0 = CV/den0x3*(-8*A7d2-8*A2d3+8*A3d3+8*q1Dq2*D0+8*q1Dq2*A1d1)
c
      cf_b1_1_v(5)=t0
c
c --------cf_b1_1_6v
c
      cf_b1_1_v(6)=0d0
c
c --------cf_b1_1_7v
c
      s1 = CV
      s3 = 1/den0x3
      s5 = 8*q2Dp1b*A3d1+16*mb**2*A10d3+16*q1Dq2*A9d3-8*mb**2*A5d3-8*q2D
     #p1b*A2d1-16*q2Dpb*A4d2-8*mb**2*A12d3-8*q2Dp1b*A9d3-8*q1Dpb*A2d2+8*
     #pbDp1b*A3d2-8*pbDp1b*A3d1+8*pbDp1b*A2d1-16*q2Dp1b*A4d2-8*q1Dq2*A1d
     #1+8*q1Dpb*A4d2+8*q1Dq2*A2d1+8*q1Dpb*A5d3+8*pbDp1b*A4d2-8*q1Dpb*A10
     #d3+16*q1Dpb*A13d3-8*q1Dp1b*A2d2+8*q1Dp1b*A4d2-8*mb**2*A3d1-8*q1Dp1
     #b*A5d2+8*q2Dpb*A3d1-8*mb**2*A6d2+8*q1Dp1b*A5d3-16*q1Dp1b*A9d3-8*q2
     #Dpb*A2d1-8*q1Dp1b*A10d3+16*q1Dp1b*A13d3+8*q2Dpb*A5d3+8*q2Dpb*A2d2-
     #8*q1Dp1b*A8d3-16*pbDp1b*A13d3-16*q1Dpb*A9d3
      s4 = s5+8*q2Dp1b*A2d2-8*q2Dpb*A10d3-8*q1Dpb*A5d2-16*A2d3+8*q2Dp1b*
     #A5d3-8*pbDp1b*A5d3-8*q2Dp1b*A10d3-8*pbDp1b*A12d3+16*pbDp1b*A10d3+8
     #*mb**2*A9d3+8*mb**2*A3d2+16*A1d3-8*mb**2*A5d2+8*mb**2*A11d3+8*mb**
     #2*A2d1-16*mb**2*A13d3-8*q1Dq2*A1d2+8*A7d2-8*q1Dq2*A7d3-8*q2Dpb*A9d
     #3+8*q2Dpb*A13d3+8*q2Dp1b*A5d2+8*q1Dpb*A6d2+8*q2Dp1b*A13d3+8*q1Dp1b
     #*A6d2+8*q1Dq2*A4d2+8*q1Dpb*A7d3+8*q1Dp1b*A7d3+8*q2Dpb*A5d2+8*pbDp1
     #b*A9d3+8*pbDp1b*A11d3-8*pbDp1b*A6d2-8*pbDp1b*A5d2-8*q1Dq2*A5d3-8*q
     #1Dpb*A8d3+8*mb**2*A4d2
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_v(7)=t0
c
c --------cf_b1_1_8v
c
      t0 = CV/den0x3*(8*mb**2*A2d1-8*mb**2*A3d1+8*mb**2*A2d2-8*mb**2*A6d
     #2+8*A7d2+8*A2d3-8*q1Dpb*A2d2+8*q1Dpb*A4d2-8*q1Dp1b*A2d2+8*q1Dp1b*A
     #4d2+8*pbDp1b*A2d1-8*pbDp1b*A3d1+8*pbDp1b*A2d2-8*pbDp1b*A6d2)
c
      cf_b1_1_v(8)=t0
c
c --------cf_b1_1_9v
c
      s1 = CV
      s3 = 1/den0x3
      s5 = -8*q2Dpb*A2d1-24*pbDp1b*A10d3+8*q2Dpb*A3d1-8*mb**2*A6d3-16*A3
     #d3-8*q2Dp1b*A5d3+8*mb**2*A5d3+8*q2Dp1b*A3d1-16*q1Dp1b*A13d3-16*mb*
     #*2*A6d2-8*q1Dpb*A5d3-16*pbDp1b*A6d2+16*A2d3+24*pbDp1b*A12d3+24*q2D
     #p1b*A6d2+8*q1Dq2*A5d3+16*q1Dpb*A10d3+8*mb**2*A2d2-8*q1Dq2*A9d3+8*q
     #1Dq2*A2d2-8*q1Dq2*A4d2+8*A7d2-8*q1Dp1b*A5d3-8*q1Dp1b*A12d3-16*q1Dp
     #b*A13d3-8*pbDp1b*A6d3+8*q1Dq2*A5d2
      s4 = s5-8*q2Dp1b*A3d2+8*q1Dq2*A13d3+8*pbDp1b*A5d3+8*q1Dp1b*A9d3-8*
     #q2Dp1b*A2d1+8*pbDp1b*A3d2+16*q1Dp1b*A10d3-8*q1Dq2*D0+24*mb**2*A12d
     #3-8*q1Dq2*A6d2+24*q2Dpb*A6d2-24*mb**2*A10d3-8*q2Dp1b*A12d3+8*q1Dp1
     #b*A11d3+8*q1Dpb*A11d3+8*pbDp1b*A2d2-8*q2Dpb*A12d3-8*q2Dpb*A3d2+8*m
     #b**2*A3d2-8*q1Dq2*A1d1-8*q1Dpb*A12d3+16*q2Dp1b*A10d3-16*q2Dpb*A2d2
     #+16*q2Dpb*A10d3-8*q2Dpb*A5d3-8*q1Dq2*A10d3-16*q2Dp1b*A2d2+8*q1Dpb*
     #A9d3
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_v(9)=t0
c
c --------cf_b1_1_10v
c
      s1 = CV
      s3 = 1/den0x3
      s5 = -8*q1Dq2*D0+8*q1Dpb*A9d3+16*A2d3+8*mb**2*A5d3-8*q2Dpb*A2d1+8*
     #mb**2*A3d2-8*q2Dp1b*A12d3-8*pbDp1b*A6d3+8*q1Dq2*A13d3-8*q1Dpb*A12d
     #3+8*mb**2*A2d2-8*q2Dpb*A3d2-8*q2Dpb*A12d3-8*q2Dp1b*A3d2-24*mb**2*A
     #10d3+8*q1Dpb*A11d3-8*q1Dq2*A10d3+8*q1Dp1b*A11d3-8*q1Dp1b*A12d3+8*q
     #1Dq2*A5d2-8*q1Dq2*A6d2+8*pbDp1b*A5d3-24*pbDp1b*A10d3+24*pbDp1b*A12
     #d3-16*A3d3+16*q2Dp1b*A10d3+8*pbDp1b*A2d2
      s4 = s5-16*q2Dpb*A2d2-8*q2Dp1b*A5d3+8*pbDp1b*A3d2-8*q2Dpb*A5d3+16*
     #q2Dpb*A10d3-16*q2Dp1b*A2d2+24*q2Dp1b*A6d2-8*q1Dp1b*A5d3+8*q2Dp1b*A
     #3d1+8*q1Dp1b*A9d3+16*q1Dp1b*A10d3-16*q1Dp1b*A13d3-16*q1Dpb*A13d3-8
     #*q2Dp1b*A2d1+8*q1Dq2*A2d2+8*q2Dpb*A3d1-8*q1Dpb*A5d3+8*A7d2+16*q1Dp
     #b*A10d3+8*q1Dq2*A5d3-8*q1Dq2*A9d3-8*q1Dq2*A4d2+24*q2Dpb*A6d2-16*pb
     #Dp1b*A6d2-8*mb**2*A6d3-8*q1Dq2*A1d1+24*mb**2*A12d3-16*mb**2*A6d2
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_v(10)=t0
c
c --------cf_b1_1_11v
c
      t0 = CV/den0x3*(-4*q2Dpb*A5d3-8*mb**2*A6d2-4*q2Dp1b*A12d3+12*mb**2
     #*A12d3-4*q2Dp1b*A3d2-4*q1Dq2*A10d3-4*pbDp1b*A6d3+4*q1Dq2*A13d3-4*q
     #1Dq2*D0-4*q2Dpb*A3d2-4*q2Dpb*A12d3+4*q1Dpb*A11d3-4*q1Dpb*A12d3+4*p
     #bDp1b*A3d2+4*q1Dp1b*A11d3+4*q1Dq2*A5d2-12*A3d3-4*q2Dp1b*A2d2-4*q1D
     #q2*A6d2-12*pbDp1b*A10d3-4*q2Dpb*A2d2-4*mb**2*A6d3+8*q2Dp1b*A10d3+4
     #*pbDp1b*A2d2+4*pbDp1b*A5d3+8*q2Dpb*A6d2-8*q1Dp1b*A13d3+8*q2Dpb*A10
     #d3+8*q1Dp1b*A10d3+8*q2Dp1b*A6d2+8*q1Dpb*A10d3+4*mb**2*A5d3-4*q1Dp1
     #b*A5d3+4*q1Dp1b*A9d3+4*q1Dq2*A5d3-4*q1Dq2*A9d3-4*q1Dpb*A5d3+4*mb**
     #2*A3d2+4*q1Dpb*A9d3-8*q1Dpb*A13d3-4*q2Dp1b*A5d3-4*q1Dp1b*A12d3+12*
     #pbDp1b*A12d3-8*pbDp1b*A6d2-4*q1Dq2*A1d1+4*A7d2+12*A2d3+4*mb**2*A2d
     #2-12*mb**2*A10d3)
c
      cf_b1_1_v(11)=t0
c
c --------cf_b1_1_12v
c
      t0 = CV/den0x3*(-4*q1Dq2*A1d1+8*q2Dpb*A6d2-4*mb**2*A6d3-12*pbDp1b*
     #A10d3+4*pbDp1b*A5d3-4*q1Dpb*A12d3+4*pbDp1b*A2d2-8*pbDp1b*A6d2+4*pb
     #Dp1b*A3d2+4*q1Dp1b*A11d3+8*q1Dpb*A10d3-4*q2Dp1b*A12d3-4*q2Dp1b*A5d
     #3+12*A2d3-4*q1Dq2*A9d3+4*q1Dq2*A5d3+4*q1Dp1b*A9d3-4*q2Dpb*A5d3-4*q
     #1Dp1b*A5d3+8*q1Dp1b*A10d3+4*mb**2*A5d3-4*q2Dpb*A12d3+4*A7d2-4*q2Dp
     #1b*A3d2-4*q1Dpb*A5d3-12*mb**2*A10d3-8*q1Dpb*A13d3-4*q2Dpb*A3d2+12*
     #pbDp1b*A12d3-4*q2Dpb*A2d2+4*q1Dpb*A9d3-4*pbDp1b*A6d3+8*q2Dpb*A10d3
     #+12*mb**2*A12d3+4*mb**2*A2d2-4*q1Dq2*A6d2+8*q2Dp1b*A10d3+4*q1Dq2*A
     #13d3+4*mb**2*A3d2-4*q1Dq2*D0-8*mb**2*A6d2-4*q1Dp1b*A12d3-4*q2Dp1b*
     #A2d2-8*q1Dp1b*A13d3+4*q1Dq2*A5d2-12*A3d3+4*q1Dpb*A11d3+8*q2Dp1b*A6
     #d2-4*q1Dq2*A10d3)
c
      cf_b1_1_v(12)=t0
c
c --------cf_b1_1_13v
c
      cf_b1_1_v(13)=0d0
c
c --------cf_b1_1_14v
c
      cf_b1_1_v(14)=0d0
c
c --------cf_b1_1_15v
c
      cf_b1_1_v(15)=0d0
c
c --------cf_b1_1_16v
c
      t0 = CV/den0x3*(8*A2d2-8*A4d2+8*A5d2-8*A6d2-8*A5d3-8*A7d3+8*A8d3+1
     #6*A9d3+8*A10d3-16*A13d3)
c
      cf_b1_1_v(16)=t0
c
c --------cf_b1_1_17v
c
      t0 = CV/den0x3*(-8*A5d2+8*A6d2-8*A5d3+8*A9d3+8*A10d3-8*A13d3)
c
      cf_b1_1_v(17)=t0
c
c --------cf_b1_1_18v
c
      t0 = CV/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_v(18)=t0
c
c --------cf_b1_1_19v
c
      t0 = CV/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_v(19)=t0
c
c --------cf_b1_1_20v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1-8*A2d2+16*A4d2-8*A5d2-8*A5d3+8*A9d3+
     #8*A10d3-8*A13d3)
c
      cf_b1_1_v(20)=t0
c
c --------cf_b1_1_21v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-16*A2d2+16*A6d2-8*A5d3+8*A10d3)
c
      cf_b1_1_v(21)=t0
c
c --------cf_b1_1_22v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_v(22)=t0
c
c --------cf_b1_1_23v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_v(23)=t0
c
c --------cf_b1_1_24v
c
      t0 = CV/den0x3*(8*A2d2-8*A4d2+8*A5d2-8*A6d2-8*A5d3-8*A7d3+8*A8d3+1
     #6*A9d3+8*A10d3-16*A13d3)
c
      cf_b1_1_v(24)=t0
c
c --------cf_b1_1_25v
c
      t0 = CV/den0x3*(-8*A5d2+8*A6d2-8*A5d3+8*A9d3+8*A10d3-8*A13d3)
c
      cf_b1_1_v(25)=t0
c
c --------cf_b1_1_26v
c
      t0 = CV/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_v(26)=t0
c
c --------cf_b1_1_27v
c
      t0 = CV/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_v(27)=t0
c
c --------cf_b1_1_28v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1-8*A2d2+16*A4d2-8*A5d2-8*A5d3+8*A9d3+
     #8*A10d3-8*A13d3)
c
      cf_b1_1_v(28)=t0
c
c --------cf_b1_1_29v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-16*A2d2+16*A6d2-8*A5d3+8*A10d3)
c
      cf_b1_1_v(29)=t0
c
c --------cf_b1_1_30v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_v(30)=t0
c
c --------cf_b1_1_31v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_v(31)=t0
c
c
c ---------------------- coefficients for B1_1_a
c
c --------cf_b1_1_1a
c
      cf_b1_1_a(1)=0d0
c
c --------cf_b1_1_2a
c
      t0 = CA/den0x3*(-8*mb**2*A2d1+8*mb**2*A3d1-8*mb**2*A3d2-8*mb**2*A4
     #d2+8*mb**2*A5d2+8*mb**2*A6d2-8*A7d2-8*A1d3+8*A2d3+8*q2Dpb*A2d1-8*q
     #2Dpb*A3d1+8*q2Dpb*A4d2-8*q2Dpb*A6d2+8*q2Dp1b*A2d1-8*q2Dp1b*A3d1+8*
     #q2Dp1b*A4d2-8*q2Dp1b*A6d2-8*pbDp1b*A2d1+8*pbDp1b*A3d1-8*pbDp1b*A3d
     #2-8*pbDp1b*A4d2+8*pbDp1b*A5d2+8*pbDp1b*A6d2)
c
      cf_b1_1_a(2)=t0
c
c --------cf_b1_1_3a
c
      s1 = CA
      s3 = 1/den0x3
      s5 = -8*q1Dq2*A2d2-8*q1Dp1b*A2d1-16*q2Dp1b*A6d2+8*q1Dq2*A2d1-8*mb*
     #*2*A2d1+24*mb**2*A6d2-16*q2Dpb*A6d2+16*q2Dpb*A2d2-8*q2Dp1b*A3d1-8*
     #mb**2*A3d2+8*q2Dpb*A2d1+8*q1Dpb*A5d3-16*A2d3+16*q2Dp1b*A2d2+8*q1Dp
     #1b*A5d2+16*pbDp1b*A10d3-8*q1Dp1b*A9d3+8*q1Dp1b*A5d3+8*q2Dpb*A5d3-8
     #*q1Dpb*A10d3-8*q1Dp1b*A10d3-16*q1Dp1b*A4d2+8*mb**2*A3d1-8*pbDp1b*A
     #5d3+24*pbDp1b*A6d2+16*q1Dq2*A4d2+8*q1Dpb*A13d3
      s4 = s5-8*q1Dpb*A2d1-8*mb**2*A5d3+8*q1Dpb*A3d1+8*q1Dpb*A5d2-8*q2Dp
     #1b*A10d3-8*pbDp1b*A2d1+8*q1Dpb*A2d2-8*pbDp1b*A3d2+8*q1Dp1b*A13d3-8
     #*mb**2*A12d3-8*q1Dq2*A5d3+8*q2Dp1b*A5d3-8*q2Dpb*A3d1+8*q1Dp1b*A2d2
     #+8*q2Dp1b*A2d1-8*q1Dpb*A9d3+8*q1Dp1b*A3d1-8*pbDp1b*A12d3-16*A7d2-1
     #6*pbDp1b*A2d2+8*q1Dq2*A1d1+8*q1Dq2*D0-8*q2Dpb*A10d3+8*pbDp1b*A3d1-
     #16*q1Dpb*A4d2+8*q1Dq2*A9d3-16*mb**2*A2d2+16*mb**2*A10d3
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_a(3)=t0
c
c --------cf_b1_1_4a
c
      t0 = CA/den0x3*(-8*A7d2-8*A2d3+8*A3d3+8*q1Dq2*D0+8*q1Dq2*A1d1)
c
      cf_b1_1_a(4)=t0
c
c --------cf_b1_1_5a
c
      t0 = CA/den0x3*(-8*A7d2-8*A2d3+8*A3d3+8*q1Dq2*D0+8*q1Dq2*A1d1)
c
      cf_b1_1_a(5)=t0
c
c --------cf_b1_1_6a
c
      cf_b1_1_a(6)=0d0
c
c --------cf_b1_1_7a
c
      s1 = CA
      s3 = 1/den0x3
      s5 = 8*q1Dp1b*A7d3-8*mb**2*A12d3-8*pbDp1b*A5d3+8*A7d2+16*q1Dq2*A9d
     #3+8*q2Dp1b*A3d1+8*q1Dpb*A4d2+8*q1Dpb*A5d3+8*q2Dpb*A2d2-16*q2Dp1b*A
     #4d2-8*q1Dp1b*A5d2-8*q1Dp1b*A10d3+8*q2Dp1b*A5d3+16*q1Dpb*A13d3-8*pb
     #Dp1b*A5d2-16*mb**2*A13d3+8*pbDp1b*A9d3+8*mb**2*A11d3+8*pbDp1b*A3d2
     #+8*q2Dpb*A5d2+8*pbDp1b*A11d3-16*q2Dpb*A4d2+16*A1d3-8*mb**2*A6d2+8*
     #mb**2*A3d2-8*q1Dpb*A8d3-8*q1Dp1b*A8d3+8*q1Dpb*A6d2-8*q1Dq2*A1d2-8*
     #q1Dq2*A5d3+8*pbDp1b*A2d1+8*q1Dp1b*A4d2-8*mb**2*A3d1+16*mb**2*A10d3
     #-8*q1Dp1b*A2d2-8*mb**2*A5d2
      s4 = s5-8*q2Dp1b*A2d1+8*pbDp1b*A4d2+8*q2Dp1b*A2d2+8*q2Dp1b*A13d3+8
     #*mb**2*A4d2-16*q1Dp1b*A9d3+8*q2Dpb*A5d3-8*pbDp1b*A12d3-8*q2Dpb*A10
     #d3-8*q1Dpb*A10d3-8*q1Dpb*A5d2-8*q2Dpb*A9d3-8*q1Dq2*A7d3-16*A2d3+8*
     #mb**2*A2d1+8*q2Dpb*A3d1+8*q2Dpb*A13d3+8*q1Dp1b*A6d2-8*q1Dq2*A1d1-1
     #6*q1Dpb*A9d3+16*pbDp1b*A10d3-16*pbDp1b*A13d3+8*q1Dq2*A4d2-8*pbDp1b
     #*A3d1-8*pbDp1b*A6d2-8*mb**2*A5d3-8*q2Dp1b*A9d3+8*q1Dp1b*A5d3+8*mb*
     #*2*A9d3-8*q1Dpb*A2d2+8*q1Dq2*A2d1-8*q2Dp1b*A10d3-8*q2Dpb*A2d1+16*q
     #1Dp1b*A13d3+8*q2Dp1b*A5d2+8*q1Dpb*A7d3
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_a(7)=t0
c
c --------cf_b1_1_8a
c
      t0 = CA/den0x3*(8*mb**2*A2d1-8*mb**2*A3d1+8*mb**2*A2d2-8*mb**2*A6d
     #2+8*A7d2+8*A2d3-8*q1Dpb*A2d2+8*q1Dpb*A4d2-8*q1Dp1b*A2d2+8*q1Dp1b*A
     #4d2+8*pbDp1b*A2d1-8*pbDp1b*A3d1+8*pbDp1b*A2d2-8*pbDp1b*A6d2)
c
      cf_b1_1_a(8)=t0
c
c --------cf_b1_1_9a
c
      s1 = CA
      s3 = 1/den0x3
      s5 = -16*pbDp1b*A6d2+8*pbDp1b*A2d2+8*pbDp1b*A3d2-8*q2Dpb*A3d2+16*q
     #1Dpb*A10d3-8*q2Dpb*A5d3+8*q2Dp1b*A3d1-8*q2Dpb*A2d1-8*q1Dq2*A1d1-8*
     #q2Dpb*A12d3+8*q1Dq2*A2d2+24*q2Dp1b*A6d2+8*pbDp1b*A5d3+8*q1Dpb*A9d3
     #-8*pbDp1b*A6d3+16*q2Dpb*A10d3-8*q1Dpb*A5d3+8*A7d2-24*mb**2*A10d3+1
     #6*A2d3-8*q2Dp1b*A2d1+8*mb**2*A3d2+8*q2Dpb*A3d1-8*q2Dp1b*A12d3-8*q2
     #Dp1b*A3d2-8*q1Dq2*A4d2+8*q1Dq2*A13d3
      s4 = s5-8*q1Dpb*A12d3-16*q1Dp1b*A13d3-8*q1Dp1b*A12d3-16*mb**2*A6d2
     #-8*q1Dq2*A6d2+16*q2Dp1b*A10d3+8*q1Dpb*A11d3-8*q1Dq2*A10d3-16*q2Dp1
     #b*A2d2-8*q1Dp1b*A5d3-16*A3d3+8*q1Dq2*A5d2-8*mb**2*A6d3-8*q1Dq2*D0+
     #8*q1Dp1b*A11d3-16*q2Dpb*A2d2+24*pbDp1b*A12d3+8*q1Dp1b*A9d3+16*q1Dp
     #1b*A10d3-24*pbDp1b*A10d3+8*mb**2*A2d2+24*mb**2*A12d3+8*mb**2*A5d3-
     #16*q1Dpb*A13d3-8*q1Dq2*A9d3-8*q2Dp1b*A5d3+8*q1Dq2*A5d3+24*q2Dpb*A6
     #d2
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_a(9)=t0
c
c --------cf_b1_1_10a
c
      s1 = CA
      s3 = 1/den0x3
      s5 = 8*pbDp1b*A2d2-8*q1Dp1b*A5d3+8*q1Dq2*A5d2+8*pbDp1b*A3d2+8*q1Dq
     #2*A2d2-16*q2Dpb*A2d2-8*q2Dp1b*A5d3+8*q1Dq2*A5d3-8*q1Dq2*A4d2+24*pb
     #Dp1b*A12d3-16*mb**2*A6d2-8*q1Dq2*A6d2-8*q1Dq2*A1d1-8*mb**2*A6d3+8*
     #mb**2*A2d2+16*q2Dpb*A10d3+16*q1Dp1b*A10d3+8*q1Dq2*A13d3+8*q1Dp1b*A
     #9d3-16*A3d3+24*q2Dp1b*A6d2+8*q2Dp1b*A3d1-16*q1Dpb*A13d3-8*q2Dp1b*A
     #3d2-8*q1Dp1b*A12d3-8*pbDp1b*A6d3-8*q2Dp1b*A12d3
      s4 = s5-16*q2Dp1b*A2d2-8*q1Dpb*A5d3+8*mb**2*A3d2-8*q1Dq2*A10d3-8*q
     #1Dq2*D0+16*q1Dpb*A10d3-8*q2Dpb*A2d1+8*pbDp1b*A5d3-8*q2Dp1b*A2d1-16
     #*q1Dp1b*A13d3-24*mb**2*A10d3+16*q2Dp1b*A10d3+8*q2Dpb*A3d1-8*q2Dpb*
     #A3d2+8*q1Dpb*A11d3+24*mb**2*A12d3+24*q2Dpb*A6d2-24*pbDp1b*A10d3-8*
     #q1Dpb*A12d3-8*q2Dpb*A5d3+8*q1Dpb*A9d3+8*mb**2*A5d3-16*pbDp1b*A6d2+
     #8*A7d2+16*A2d3+8*q1Dp1b*A11d3-8*q2Dpb*A12d3-8*q1Dq2*A9d3
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_1_a(10)=t0
c
c --------cf_b1_1_11a
c
      t0 = CA/den0x3*(-4*mb**2*A6d3+4*mb**2*A2d2+8*q2Dpb*A10d3+8*q2Dpb*A
     #6d2+4*q1Dp1b*A11d3+12*A2d3+12*pbDp1b*A12d3-4*q2Dp1b*A5d3-4*q2Dp1b*
     #A2d2-8*pbDp1b*A6d2+8*q2Dp1b*A10d3-4*q2Dpb*A12d3+4*q1Dq2*A13d3+4*A7
     #d2-4*q2Dpb*A2d2-8*q1Dp1b*A13d3+4*pbDp1b*A3d2+8*q1Dp1b*A10d3+8*q2Dp
     #1b*A6d2-4*q2Dp1b*A3d2-4*q2Dp1b*A12d3-4*q2Dpb*A5d3+4*q1Dp1b*A9d3-4*
     #pbDp1b*A6d3-12*mb**2*A10d3+8*q1Dpb*A10d3-4*q1Dp1b*A5d3-4*q2Dpb*A3d
     #2-8*q1Dpb*A13d3-8*mb**2*A6d2-4*q1Dpb*A5d3-4*q1Dp1b*A12d3-4*q1Dq2*A
     #1d1-4*q1Dq2*A9d3+4*q1Dq2*A5d3+4*mb**2*A3d2-12*pbDp1b*A10d3-4*q1Dq2
     #*D0+12*mb**2*A12d3+4*q1Dpb*A9d3+4*mb**2*A5d3-4*q1Dpb*A12d3+4*q1Dq2
     #*A5d2-4*q1Dq2*A6d2-4*q1Dq2*A10d3+4*pbDp1b*A5d3+4*q1Dpb*A11d3+4*pbD
     #p1b*A2d2-12*A3d3)
c
      cf_b1_1_a(11)=t0
c
c --------cf_b1_1_12a
c
      t0 = CA/den0x3*(4*q1Dpb*A9d3+12*mb**2*A12d3+4*mb**2*A5d3-4*q1Dq2*D
     #0-4*q1Dq2*A6d2+4*mb**2*A2d2+4*mb**2*A3d2+8*q2Dpb*A6d2+8*q2Dp1b*A6d
     #2-12*mb**2*A10d3+4*pbDp1b*A3d2-4*q2Dpb*A5d3-8*mb**2*A6d2+12*A2d3+8
     #*q2Dp1b*A10d3-4*q1Dq2*A1d1+4*q1Dq2*A13d3-4*q2Dpb*A3d2-4*q2Dp1b*A12
     #d3-4*pbDp1b*A6d3-12*A3d3+4*q1Dq2*A5d2+4*q1Dp1b*A9d3-4*q1Dq2*A10d3+
     #4*q1Dpb*A11d3-4*q1Dpb*A12d3+4*q1Dp1b*A11d3-4*q1Dp1b*A12d3-4*q2Dpb*
     #A12d3-4*q2Dp1b*A5d3+4*pbDp1b*A2d2+4*pbDp1b*A5d3-12*pbDp1b*A10d3+8*
     #q2Dpb*A10d3+12*pbDp1b*A12d3-4*mb**2*A6d3-8*q1Dp1b*A13d3-4*q2Dpb*A2
     #d2-4*q2Dp1b*A2d2-4*q1Dp1b*A5d3+4*A7d2+8*q1Dp1b*A10d3-8*q1Dpb*A13d3
     #-4*q2Dp1b*A3d2-8*pbDp1b*A6d2-4*q1Dq2*A9d3-4*q1Dpb*A5d3+8*q1Dpb*A10
     #d3+4*q1Dq2*A5d3)
c
      cf_b1_1_a(12)=t0
c
c --------cf_b1_1_13a
c
      cf_b1_1_a(13)=0d0
c
c --------cf_b1_1_14a
c
      cf_b1_1_a(14)=0d0
c
c --------cf_b1_1_15a
c
      cf_b1_1_a(15)=0d0
c
c --------cf_b1_1_16a
c
      t0 = CA/den0x3*(8*A2d2-8*A4d2+8*A5d2-8*A6d2-8*A5d3-8*A7d3+8*A8d3+1
     #6*A9d3+8*A10d3-16*A13d3)
c
      cf_b1_1_a(16)=t0
c
c --------cf_b1_1_17a
c
      t0 = CA/den0x3*(-8*A5d2+8*A6d2-8*A5d3+8*A9d3+8*A10d3-8*A13d3)
c
      cf_b1_1_a(17)=t0
c
c --------cf_b1_1_18a
c
      t0 = CA/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_a(18)=t0
c
c --------cf_b1_1_19a
c
      t0 = CA/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_a(19)=t0
c
c --------cf_b1_1_20a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1-8*A2d2+16*A4d2-8*A5d2-8*A5d3+8*A9d3+
     #8*A10d3-8*A13d3)
c
      cf_b1_1_a(20)=t0
c
c --------cf_b1_1_21a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-16*A2d2+16*A6d2-8*A5d3+8*A10d3)
c
      cf_b1_1_a(21)=t0
c
c --------cf_b1_1_22a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_a(22)=t0
c
c --------cf_b1_1_23a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_a(23)=t0
c
c --------cf_b1_1_24a
c
      t0 = CA/den0x3*(8*A2d2-8*A4d2+8*A5d2-8*A6d2-8*A5d3-8*A7d3+8*A8d3+1
     #6*A9d3+8*A10d3-16*A13d3)
c
      cf_b1_1_a(24)=t0
c
c --------cf_b1_1_25a
c
      t0 = CA/den0x3*(-8*A5d2+8*A6d2-8*A5d3+8*A9d3+8*A10d3-8*A13d3)
c
      cf_b1_1_a(25)=t0
c
c --------cf_b1_1_26a
c
      t0 = CA/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_a(26)=t0
c
c --------cf_b1_1_27a
c
      t0 = CA/den0x3*(8*A5d3-8*A9d3-16*A10d3-8*A11d3+8*A12d3+16*A13d3)
c
      cf_b1_1_a(27)=t0
c
c --------cf_b1_1_28a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1-8*A2d2+16*A4d2-8*A5d2-8*A5d3+8*A9d3+
     #8*A10d3-8*A13d3)
c
      cf_b1_1_a(28)=t0
c
c --------cf_b1_1_29a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-16*A2d2+16*A6d2-8*A5d3+8*A10d3)
c
      cf_b1_1_a(29)=t0
c
c --------cf_b1_1_30a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_a(30)=t0
c
c --------cf_b1_1_31a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A2d2+8*A3d2-24*A6d2+8*A5d3-16*A10
     #d3+8*A12d3)
c
      cf_b1_1_a(31)=t0
c
c
c --------------- integrals for B1_2 --------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pb(i)-p1b(i)
         p3(i)=-pw(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
C --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_1m(xs2,C0124)
      B012=0d0
      B112=0d0
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs1,xs2,C0134)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c --- C0234
      xs1 = wbb_dot_4(p2,p2)
      xs2 = wbb_dot_4(p3,p3)
      xs12 = wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c --- D0
      call wbb_f4pf_b1(p1,p2,p3,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b1_2_v(i)=0d0
         cf_b1_2_a(i)=0d0
      enddo
c
c ---------------------- coefficients for B1_2_v
c
c --------cf_b1_2_1v
c
      cf_b1_2_v(1)=0d0
c
c --------cf_b1_2_2v
c
      s1 = CV
      s3 = 1/den0x3
      s5 = -24*pbDp1b*A6d2+8*q2Dp1b*A2d1+8*q1Dpb*A8d3+16*q1Dp1b*A6d2-8*q
     #1Dp1b*A7d3+8*q2Dp1b*A13d3+8*pbDp1b*A9d3+8*pbDp1b*A11d3-16*pbDp1b*A
     #13d3-8*mb**2*A6d3-16*q1Dpb*A11d3-8*q1Dpb*A12d3-16*q1Dp1b*A11d3-8*q
     #1Dp1b*A12d3-8*q2Dpb*A3d2-8*q2Dpb*A12d3-8*q2Dp1b*A3d2-8*q2Dp1b*A12d
     #3-8*pbDp1b*A6d3+8*q1Dq2*A3d1+8*q1Dq2*A3d2-8*q1Dq2*A6d3-8*q1Dq2*A8d
     #3+8*q2Dpb*A6d3-8*q2Dpb*A11d3+8*q2Dp1b*A6d3-8*mb**2*A5d2-8*mb**2*A3
     #d1+16*A7d2+8*mb**2*A2d2-8*pbDp1b*A5d2-8*q2Dpb*A3d1+8*q1Dpb*A6d3+16
     #*q1Dq2*A11d3+16*pbDp1b*A12d3+8*q2Dp1b*A4d2+16*A1d3-16*q1Dp1b*A4d2
      s4 = s5+8*q1Dp1b*A3d1-8*q1Dp1b*A2d1+16*q1Dpb*A13d3-16*q1Dpb*A4d2+8
     #*q1Dpb*A3d1-8*q1Dpb*A2d1-16*q1Dq2*A1d1-16*q1Dpb*A3d2+16*q1Dp1b*A13
     #d3+16*q1Dp1b*A5d2+8*mb**2*A2d1+16*pbDp1b*A3d2-8*q1Dpb*A7d3-8*pbDp1
     #b*A10d3+8*q2Dpb*A4d2+8*pbDp1b*A2d2+8*q1Dp1b*A6d3+16*mb**2*A12d3-16
     #*q1Dp1b*A3d2-8*q2Dp1b*A11d3+8*q1Dp1b*A8d3+8*pbDp1b*A2d1+8*mb**2*A9
     #d3+8*pbDp1b*A4d2-8*pbDp1b*A3d1-16*A3d3-8*mb**2*A10d3+8*q2Dpb*A13d3
     #-8*q1Dq2*D0+8*mb**2*A11d3+16*mb**2*A3d2+8*q2Dpb*A2d1-8*q1Dq2*A1d2-
     #16*mb**2*A13d3+8*mb**2*A4d2+16*q1Dpb*A5d2-24*mb**2*A6d2+16*q1Dpb*A
     #6d2-8*q2Dp1b*A3d1
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_2_v(2)=t0
c
c --------cf_b1_2_3v
c
      t0 = CV/den0x3*(8*mb**2*A2d1-8*mb**2*A3d1+8*mb**2*A2d2-8*mb**2*A6d
     #2+8*A7d2+8*A3d3-8*q1Dpb*A2d1+8*q1Dpb*A3d1-8*q1Dpb*A4d2+8*q1Dpb*A6d
     #2-8*q1Dp1b*A2d1+8*q1Dp1b*A3d1-8*q1Dp1b*A4d2+8*q1Dp1b*A6d2+8*pbDp1b
     #*A2d1-8*pbDp1b*A3d1+8*pbDp1b*A2d2-8*pbDp1b*A6d2)
c
      cf_b1_2_v(3)=t0
c
c --------cf_b1_2_4v
c
      t0 = CV/den0x3*(8*q1Dq2*A13d3+8*q1Dq2*A6d3-8*q1Dq2*A11d3+8*q1Dpb*A
     #3d2-8*q1Dpb*A6d3-8*q2Dpb*A6d3-8*q2Dp1b*A6d3-8*q1Dq2*A12d3+16*q1Dpb
     #*A12d3-8*pbDp1b*A5d3-8*q2Dp1b*A10d3+8*pbDp1b*A6d3-8*pbDp1b*A3d2-8*
     #mb**2*A5d3+16*q2Dp1b*A12d3+8*mb**2*A6d3+24*mb**2*A10d3-8*q1Dq2*A5d
     #2+16*mb**2*A6d2+8*q1Dpb*A2d2-16*q1Dpb*A13d3-16*q1Dp1b*A6d2-8*pbDp1
     #b*A2d2+8*q1Dq2*A6d2-8*mb**2*A2d2+16*q2Dpb*A12d3-8*mb**2*A3d2-8*q1D
     #p1b*A6d3+16*q1Dp1b*A12d3+8*q1Dp1b*A11d3+8*q1Dp1b*A9d3+16*A3d3-16*q
     #1Dp1b*A13d3-8*q1Dp1b*A10d3-16*A2d3+8*q1Dp1b*A2d2+8*q1Dpb*A11d3+24*
     #pbDp1b*A10d3-24*pbDp1b*A12d3-8*q2Dpb*A10d3+8*q1Dp1b*A3d2-16*q1Dpb*
     #A6d2+16*pbDp1b*A6d2-24*mb**2*A12d3+8*q1Dpb*A9d3-8*q1Dpb*A10d3)
c
      cf_b1_2_v(4)=t0
c
c --------cf_b1_2_5v
c
      t0 = CV/den0x3*(16*pbDp1b*A6d2+8*q1Dq2*A6d2-8*q1Dq2*A5d2-8*q1Dq2*A
     #12d3-8*q1Dq2*A11d3+16*q2Dpb*A12d3+8*q1Dp1b*A2d2-8*pbDp1b*A2d2-16*q
     #1Dpb*A6d2+24*pbDp1b*A10d3-24*mb**2*A12d3-16*A2d3+8*q1Dpb*A3d2-8*pb
     #Dp1b*A3d2+8*q1Dq2*A13d3+8*q1Dp1b*A11d3+16*q1Dp1b*A12d3-8*q2Dpb*A10
     #d3+16*mb**2*A6d2-8*mb**2*A3d2-8*q2Dp1b*A10d3+16*A3d3-8*mb**2*A5d3-
     #8*q1Dp1b*A6d3-8*mb**2*A2d2-8*q2Dpb*A6d3-8*q1Dpb*A10d3+8*q1Dp1b*A9d
     #3+8*q1Dp1b*A3d2-16*q1Dp1b*A13d3-16*q1Dpb*A13d3+8*mb**2*A6d3-8*pbDp
     #1b*A5d3+8*q1Dq2*A6d3-8*q1Dpb*A6d3+16*q2Dp1b*A12d3+8*q1Dpb*A2d2+8*q
     #1Dpb*A9d3-16*q1Dp1b*A6d2-8*q2Dp1b*A6d3+8*q1Dpb*A11d3-8*q1Dp1b*A10d
     #3+24*mb**2*A10d3+8*pbDp1b*A6d3-24*pbDp1b*A12d3+16*q1Dpb*A12d3)
c
      cf_b1_2_v(5)=t0
c
c --------cf_b1_2_6v
c
      cf_b1_2_v(6)=0d0
c
c --------cf_b1_2_7v
c
      t0 = CV/den0x3*(-8*mb**2*A2d1+8*mb**2*A3d1-8*mb**2*A3d2-8*mb**2*A4
     #d2+8*mb**2*A5d2+8*mb**2*A6d2-8*A7d2-8*A1d3+8*A3d3+8*q2Dpb*A3d2-8*q
     #2Dpb*A5d2+8*q2Dp1b*A3d2-8*q2Dp1b*A5d2-8*pbDp1b*A2d1+8*pbDp1b*A3d1-
     #8*pbDp1b*A3d2-8*pbDp1b*A4d2+8*pbDp1b*A5d2+8*pbDp1b*A6d2)
c
      cf_b1_2_v(7)=t0
c
c --------cf_b1_2_8v
c
      t0 = CV/den0x3*(8*q2Dpb*A3d2-8*pbDp1b*A10d3-8*q1Dp1b*A11d3-8*q1Dp1
     #b*A12d3-8*q2Dpb*A12d3+8*q2Dp1b*A3d2-8*q2Dp1b*A12d3-8*pbDp1b*A6d3+8
     #*q1Dq2*A3d1-8*q1Dq2*A6d3+8*q1Dq2*A11d3-8*q1Dpb*A3d2+8*q1Dpb*A6d3+8
     #*q1Dp1b*A6d3+8*q2Dpb*A6d3+8*q2Dp1b*A6d3-8*q1Dpb*A11d3-16*A3d3-8*mb
     #**2*A2d2+16*mb**2*A12d3+8*q1Dq2*A5d2+16*pbDp1b*A12d3-8*q1Dp1b*A3d1
     #-8*q1Dpb*A12d3+8*q1Dp1b*A13d3-8*pbDp1b*A2d1-8*mb**2*A6d3+8*q1Dp1b*
     #A2d1-8*pbDp1b*A2d2+8*pbDp1b*A3d1+8*q1Dp1b*A4d2-8*mb**2*A2d1+8*pbDp
     #1b*A6d2-8*q1Dpb*A3d1-8*q1Dp1b*A3d2-8*mb**2*A10d3-8*A7d2+8*mb**2*A6
     #d2+8*q1Dpb*A13d3-8*q2Dp1b*A6d2+8*q1Dpb*A2d1+8*q1Dpb*A4d2+8*mb**2*A
     #3d1-8*q2Dpb*A6d2)
c
      cf_b1_2_v(8)=t0
c
c --------cf_b1_2_9v
c
      t0 = CV/den0x3*(8*A2d3-8*A3d3-8*q1Dq2*A3d2+8*q1Dq2*A5d2+8*q1Dpb*A2
     #d1-8*q1Dpb*A3d1+8*q1Dpb*A3d2+8*q1Dpb*A4d2-8*q1Dpb*A5d2-8*q1Dpb*A6d
     #2+8*q1Dp1b*A2d1-8*q1Dp1b*A3d1+8*q1Dp1b*A3d2+8*q1Dp1b*A4d2-8*q1Dp1b
     #*A5d2-8*q1Dp1b*A6d2)
c
      cf_b1_2_v(9)=t0
c
c --------cf_b1_2_10v
c
      t0 = CV/den0x3*(8*A2d3-8*A3d3-8*q1Dq2*A3d2+8*q1Dq2*A5d2+8*q1Dpb*A2
     #d1-8*q1Dpb*A3d1+8*q1Dpb*A3d2+8*q1Dpb*A4d2-8*q1Dpb*A5d2-8*q1Dpb*A6d
     #2+8*q1Dp1b*A2d1-8*q1Dp1b*A3d1+8*q1Dp1b*A3d2+8*q1Dp1b*A4d2-8*q1Dp1b
     #*A5d2-8*q1Dp1b*A6d2)
c
      cf_b1_2_v(10)=t0
c
c --------cf_b1_2_11v
c
      t0 = CV/den0x3*(-4*q1Dq2*A1d1-4*q1Dpb*A2d2+4*q1Dpb*A10d3+8*q1Dpb*A
     #13d3-4*q1Dp1b*A2d2-4*q1Dp1b*A9d3+4*q1Dp1b*A10d3+8*q1Dp1b*A13d3+4*q
     #2Dpb*A10d3+4*q2Dp1b*A10d3+4*pbDp1b*A2d2+4*pbDp1b*A5d3-12*pbDp1b*A1
     #0d3+12*pbDp1b*A12d3+8*q1Dpb*A6d2-12*A3d3+4*q1Dpb*A6d3+12*mb**2*A12
     #d3-12*mb**2*A10d3+4*mb**2*A5d3+4*q2Dp1b*A6d3-8*pbDp1b*A6d2+12*A2d3
     #-4*q1Dpb*A9d3-4*q1Dpb*A3d2+4*mb**2*A3d2+4*q1Dq2*A11d3+4*mb**2*A2d2
     #+4*A7d2+4*pbDp1b*A3d2-4*q1Dq2*D0+8*q1Dp1b*A6d2-8*mb**2*A6d2-4*mb**
     #2*A6d3+4*q1Dq2*A5d2-4*q1Dq2*A6d2-4*q1Dpb*A11d3-8*q1Dpb*A12d3-4*q1D
     #p1b*A11d3-8*q1Dp1b*A12d3-8*q2Dpb*A12d3-8*q2Dp1b*A12d3-4*pbDp1b*A6d
     #3-4*q1Dq2*A13d3-4*q1Dq2*A6d3-4*q1Dp1b*A3d2+4*q1Dp1b*A6d3+4*q2Dpb*A
     #6d3+4*q1Dq2*A12d3)
c
      cf_b1_2_v(11)=t0
c
c --------cf_b1_2_12v
c
      t0 = CV/den0x3*(4*q1Dpb*A10d3-4*q1Dp1b*A9d3+4*A7d2+12*A2d3+4*q2Dp1
     #b*A6d3+4*q2Dpb*A6d3+4*q1Dp1b*A6d3+4*q1Dq2*A12d3+4*mb**2*A3d2+8*q1D
     #p1b*A6d2-4*mb**2*A6d3-4*q1Dq2*D0+4*q2Dp1b*A10d3+4*pbDp1b*A5d3-4*q1
     #Dp1b*A3d2-8*mb**2*A6d2+12*pbDp1b*A12d3-8*pbDp1b*A6d2-4*q1Dpb*A11d3
     #-12*A3d3+8*q1Dpb*A6d2-12*mb**2*A10d3-4*q1Dpb*A3d2+4*mb**2*A5d3+4*p
     #bDp1b*A2d2+4*q1Dpb*A6d3+8*q1Dpb*A13d3-8*q2Dpb*A12d3+4*q1Dq2*A11d3-
     #4*q1Dq2*A6d3+4*mb**2*A2d2-4*q1Dpb*A2d2+12*mb**2*A12d3+4*q1Dp1b*A10
     #d3-4*q1Dq2*A13d3+8*q1Dp1b*A13d3-4*q1Dq2*A1d1+4*q1Dq2*A5d2-8*q2Dp1b
     #*A12d3-4*pbDp1b*A6d3+4*q2Dpb*A10d3-8*q1Dp1b*A12d3-4*q1Dq2*A6d2-4*q
     #1Dp1b*A2d2+4*pbDp1b*A3d2-4*q1Dp1b*A11d3-8*q1Dpb*A12d3-4*q1Dpb*A9d3
     #-12*pbDp1b*A10d3)
c
      cf_b1_2_v(12)=t0
c
c --------cf_b1_2_13v
c
      cf_b1_2_v(13)=0d0
c
c --------cf_b1_2_14v
c
      cf_b1_2_v(14)=0d0
c
c --------cf_b1_2_15v
c
      cf_b1_2_v(15)=0d0
c
c --------cf_b1_2_16v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A3d2+16*A4d2-16*A5d2-16*A6d2-8*A6
     #d3+8*A7d3-8*A8d3+16*A11d3+8*A12d3-16*A13d3)
c
      cf_b1_2_v(16)=t0
c
c --------cf_b1_2_17v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1+8*A3d2-8*A4d2-8*A6d3+8*A11d3+8*A12d
     #3-8*A13d3)
c
      cf_b1_2_v(17)=t0
c
c --------cf_b1_2_18v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_v(18)=t0
c
c --------cf_b1_2_19v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_v(19)=t0
c
c --------cf_b1_2_20v
c
      t0 = CV/den0x3*(8*A5d2-8*A6d2-8*A6d3+8*A11d3+8*A12d3-8*A13d3)
c
      cf_b1_2_v(20)=t0
c
c --------cf_b1_2_21v
c
      t0 = CV/den0x3*(-8*A3d2+8*A6d2-8*A6d3+8*A12d3)
c
      cf_b1_2_v(21)=t0
c
c --------cf_b1_2_22v
c
      t0 = CV/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_v(22)=t0
c
c --------cf_b1_2_23v
c
      t0 = CV/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_v(23)=t0
c
c --------cf_b1_2_24v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+16*A3d2+16*A4d2-16*A5d2-16*A6d2-8*A6
     #d3+8*A7d3-8*A8d3+16*A11d3+8*A12d3-16*A13d3)
c
      cf_b1_2_v(24)=t0
c
c --------cf_b1_2_25v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1+8*A3d2-8*A4d2-8*A6d3+8*A11d3+8*A12d
     #3-8*A13d3)
c
      cf_b1_2_v(25)=t0
c
c --------cf_b1_2_26v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_v(26)=t0
c
c --------cf_b1_2_27v
c
      t0 = CV/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_v(27)=t0
c
c --------cf_b1_2_28v
c
      t0 = CV/den0x3*(8*A5d2-8*A6d2-8*A6d3+8*A11d3+8*A12d3-8*A13d3)
c
      cf_b1_2_v(28)=t0
c
c --------cf_b1_2_29v
c
      t0 = CV/den0x3*(-8*A3d2+8*A6d2-8*A6d3+8*A12d3)
c
      cf_b1_2_v(29)=t0
c
c --------cf_b1_2_30v
c
      t0 = CV/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_v(30)=t0
c
c --------cf_b1_2_31v
c
      t0 = CV/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_v(31)=t0
c
c
c ---------------------- coefficients for B1_2_a
c
c --------cf_b1_2_1a
c
      cf_b1_2_a(1)=0d0
c
c --------cf_b1_2_2a
c
      s1 = CA
      s3 = 1/den0x3
      s5 = -8*q2Dpb*A11d3+8*q2Dpb*A6d3-8*q2Dpb*A12d3-8*q2Dp1b*A12d3-8*pb
     #Dp1b*A6d3+8*q1Dq2*A3d1+16*q1Dq2*A11d3-16*q1Dpb*A3d2+8*q1Dpb*A6d3-1
     #6*q1Dp1b*A3d2+8*q1Dp1b*A6d3-8*q1Dq2*A6d3-16*q1Dpb*A11d3-8*q1Dpb*A1
     #2d3-8*q1Dp1b*A12d3-8*q2Dpb*A3d2+8*q1Dpb*A8d3+16*q1Dp1b*A6d2-8*q1Dp
     #1b*A7d3+8*q1Dp1b*A8d3+8*q2Dpb*A13d3+8*q2Dp1b*A13d3+8*pbDp1b*A9d3+8
     #*pbDp1b*A11d3-16*pbDp1b*A13d3-8*mb**2*A6d3+8*mb**2*A11d3+8*q1Dq2*A
     #3d2-8*q1Dpb*A7d3+16*q1Dpb*A6d2-16*mb**2*A13d3-8*q1Dq2*A1d2-16*A3d3
     #+8*mb**2*A9d3-16*q1Dp1b*A4d2+16*q1Dp1b*A5d2+16*q1Dp1b*A13d3-8*pbDp
     #1b*A10d3
      s4 = 16*pbDp1b*A12d3-8*q1Dq2*A8d3+s5+8*q1Dp1b*A3d1+8*q1Dpb*A3d1-16
     #*q1Dpb*A4d2+16*q1Dpb*A13d3-8*q1Dp1b*A2d1+8*q2Dpb*A4d2-8*q1Dpb*A2d1
     #-16*q1Dq2*A1d1-8*q1Dq2*D0+16*pbDp1b*A3d2+8*pbDp1b*A4d2-8*pbDp1b*A5
     #d2+8*mb**2*A2d2-8*mb**2*A10d3-24*pbDp1b*A6d2+16*mb**2*A12d3+8*q2Dp
     #1b*A2d1+8*mb**2*A4d2-24*mb**2*A6d2+8*q2Dpb*A2d1-8*q2Dpb*A3d1-8*q2D
     #p1b*A3d1+8*q2Dp1b*A4d2+8*pbDp1b*A2d1-8*pbDp1b*A3d1+16*mb**2*A3d2+1
     #6*A7d2-8*mb**2*A3d1+16*A1d3+8*mb**2*A2d1-8*mb**2*A5d2-8*q2Dp1b*A3d
     #2+16*q1Dpb*A5d2+8*q2Dp1b*A6d3+8*pbDp1b*A2d2-16*q1Dp1b*A11d3-8*q2Dp
     #1b*A11d3
      s2 = s3*s4
      t0 = s1*s2
c
      cf_b1_2_a(2)=t0
c
c --------cf_b1_2_3a
c
      t0 = CA/den0x3*(8*mb**2*A2d1-8*mb**2*A3d1+8*mb**2*A2d2-8*mb**2*A6d
     #2+8*A7d2+8*A3d3-8*q1Dpb*A2d1+8*q1Dpb*A3d1-8*q1Dpb*A4d2+8*q1Dpb*A6d
     #2-8*q1Dp1b*A2d1+8*q1Dp1b*A3d1-8*q1Dp1b*A4d2+8*q1Dp1b*A6d2+8*pbDp1b
     #*A2d1-8*pbDp1b*A3d1+8*pbDp1b*A2d2-8*pbDp1b*A6d2)
c
      cf_b1_2_a(3)=t0
c
c --------cf_b1_2_4a
c
      t0 = CA/den0x3*(16*pbDp1b*A6d2-8*mb**2*A2d2-8*pbDp1b*A3d2-8*mb**2*
     #A5d3+8*mb**2*A6d3-16*q1Dp1b*A6d2+16*A3d3+16*mb**2*A6d2-16*A2d3+8*q
     #1Dp1b*A11d3+16*q1Dpb*A12d3+8*q1Dpb*A11d3+8*q1Dq2*A6d2-24*pbDp1b*A1
     #2d3-8*q1Dq2*A5d2+16*q2Dpb*A12d3-8*mb**2*A3d2+16*q1Dp1b*A12d3+16*q2
     #Dp1b*A12d3-8*pbDp1b*A5d3+24*pbDp1b*A10d3+8*pbDp1b*A6d3+8*q1Dq2*A13
     #d3+8*q1Dpb*A3d2-8*q1Dpb*A6d3-8*q1Dp1b*A6d3-8*pbDp1b*A2d2+8*q1Dq2*A
     #6d3-8*q2Dpb*A6d3-8*q2Dp1b*A6d3-8*q2Dp1b*A10d3-8*q1Dq2*A12d3-16*q1D
     #pb*A6d2-8*q2Dpb*A10d3-8*q1Dp1b*A10d3+8*q1Dp1b*A9d3-8*q1Dq2*A11d3+8
     #*q1Dpb*A9d3+8*q1Dp1b*A3d2-16*q1Dp1b*A13d3+8*q1Dp1b*A2d2-16*q1Dpb*A
     #13d3-8*q1Dpb*A10d3-24*mb**2*A12d3+8*q1Dpb*A2d2+24*mb**2*A10d3)
c
      cf_b1_2_a(4)=t0
c
c --------cf_b1_2_5a
c
      t0 = CA/den0x3*(-8*pbDp1b*A3d2-8*q1Dq2*A5d2+8*q1Dq2*A6d2+8*q1Dpb*A
     #11d3+16*q1Dpb*A12d3+8*q1Dp1b*A11d3+16*q1Dp1b*A12d3+16*q2Dpb*A12d3+
     #16*q2Dp1b*A12d3+8*pbDp1b*A6d3+8*q1Dq2*A13d3+8*q1Dq2*A6d3-8*q1Dq2*A
     #11d3+8*q1Dpb*A3d2-8*q1Dpb*A6d3+8*q1Dp1b*A3d2-8*q2Dpb*A6d3-8*q2Dp1b
     #*A6d3-8*q1Dq2*A12d3+8*q1Dpb*A9d3-8*mb**2*A2d2-8*q1Dp1b*A10d3+8*q1D
     #p1b*A9d3+8*q1Dp1b*A2d2-16*q1Dpb*A13d3-8*q1Dpb*A10d3+8*q1Dpb*A2d2-2
     #4*mb**2*A12d3+24*mb**2*A10d3-8*mb**2*A5d3-16*q1Dp1b*A13d3-8*q2Dpb*
     #A10d3-8*mb**2*A3d2-8*q2Dp1b*A10d3-8*q1Dp1b*A6d3-8*pbDp1b*A2d2+16*m
     #b**2*A6d2-8*pbDp1b*A5d3+24*pbDp1b*A10d3-24*pbDp1b*A12d3+16*A3d3-16
     #*q1Dpb*A6d2+8*mb**2*A6d3-16*q1Dp1b*A6d2-16*A2d3+16*pbDp1b*A6d2)
c
      cf_b1_2_a(5)=t0
c
c --------cf_b1_2_6a
c
      cf_b1_2_a(6)=0d0
c
c --------cf_b1_2_7a
c
      t0 = CA/den0x3*(-8*mb**2*A2d1+8*mb**2*A3d1-8*mb**2*A3d2-8*mb**2*A4
     #d2+8*mb**2*A5d2+8*mb**2*A6d2-8*A7d2-8*A1d3+8*A3d3+8*q2Dpb*A3d2-8*q
     #2Dpb*A5d2+8*q2Dp1b*A3d2-8*q2Dp1b*A5d2-8*pbDp1b*A2d1+8*pbDp1b*A3d1-
     #8*pbDp1b*A3d2-8*pbDp1b*A4d2+8*pbDp1b*A5d2+8*pbDp1b*A6d2)
c
      cf_b1_2_a(7)=t0
c
c --------cf_b1_2_8a
c
      t0 = CA/den0x3*(-8*q1Dp1b*A3d2-8*q1Dp1b*A3d1+8*q1Dp1b*A13d3-8*pbDp
     #1b*A2d2-8*pbDp1b*A10d3+16*pbDp1b*A12d3-8*mb**2*A6d3-16*A3d3+8*q1Dq
     #2*A5d2-8*q1Dpb*A11d3-8*q1Dpb*A12d3-8*q1Dp1b*A12d3+8*q2Dpb*A3d2-8*q
     #2Dpb*A12d3+8*q2Dp1b*A3d2-8*q2Dp1b*A12d3-8*pbDp1b*A6d3+8*q1Dq2*A3d1
     #-8*q1Dq2*A6d3+8*q1Dq2*A11d3-8*q1Dpb*A3d2+8*q1Dpb*A6d3+8*q1Dp1b*A6d
     #3+8*q2Dpb*A6d3+8*q2Dp1b*A6d3+8*q1Dp1b*A4d2+8*mb**2*A3d1-8*A7d2+8*m
     #b**2*A6d2-8*q2Dpb*A6d2-8*pbDp1b*A2d1+8*pbDp1b*A3d1+8*pbDp1b*A6d2-8
     #*mb**2*A2d2-8*mb**2*A10d3+16*mb**2*A12d3+8*q1Dpb*A2d1-8*q1Dpb*A3d1
     #+8*q1Dpb*A4d2+8*q1Dpb*A13d3+8*q1Dp1b*A2d1-8*q1Dp1b*A11d3-8*q2Dp1b*
     #A6d2-8*mb**2*A2d1)
c
      cf_b1_2_a(8)=t0
c
c --------cf_b1_2_9a
c
      t0 = CA/den0x3*(8*A2d3-8*A3d3-8*q1Dq2*A3d2+8*q1Dq2*A5d2+8*q1Dpb*A2
     #d1-8*q1Dpb*A3d1+8*q1Dpb*A3d2+8*q1Dpb*A4d2-8*q1Dpb*A5d2-8*q1Dpb*A6d
     #2+8*q1Dp1b*A2d1-8*q1Dp1b*A3d1+8*q1Dp1b*A3d2+8*q1Dp1b*A4d2-8*q1Dp1b
     #*A5d2-8*q1Dp1b*A6d2)
c
      cf_b1_2_a(9)=t0
c
c --------cf_b1_2_10a
c
      t0 = CA/den0x3*(8*A2d3-8*A3d3-8*q1Dq2*A3d2+8*q1Dq2*A5d2+8*q1Dpb*A2
     #d1-8*q1Dpb*A3d1+8*q1Dpb*A3d2+8*q1Dpb*A4d2-8*q1Dpb*A5d2-8*q1Dpb*A6d
     #2+8*q1Dp1b*A2d1-8*q1Dp1b*A3d1+8*q1Dp1b*A3d2+8*q1Dp1b*A4d2-8*q1Dp1b
     #*A5d2-8*q1Dp1b*A6d2)
c
      cf_b1_2_a(10)=t0
c
c --------cf_b1_2_11a
c
      t0 = CA/den0x3*(-8*mb**2*A6d2-8*pbDp1b*A6d2+4*mb**2*A2d2+4*mb**2*A
     #5d3-12*mb**2*A10d3+12*mb**2*A12d3-4*q1Dq2*D0-4*q1Dq2*A1d1-4*q1Dpb*
     #A2d2+4*q1Dpb*A10d3+8*q1Dpb*A13d3-4*q1Dp1b*A2d2-4*q1Dp1b*A9d3+4*q1D
     #p1b*A10d3+8*q1Dp1b*A13d3+4*q2Dpb*A10d3+4*q2Dp1b*A10d3+4*pbDp1b*A2d
     #2+4*pbDp1b*A5d3-12*pbDp1b*A10d3+12*pbDp1b*A12d3+8*q1Dpb*A6d2-12*A3
     #d3+8*q1Dp1b*A6d2-4*mb**2*A6d3+4*q1Dq2*A5d2-4*q1Dq2*A6d2-4*q1Dpb*A1
     #1d3-8*q1Dpb*A12d3-8*q1Dp1b*A12d3-8*q2Dpb*A12d3-8*q2Dp1b*A12d3-4*pb
     #Dp1b*A6d3-4*q1Dq2*A13d3-4*q1Dq2*A6d3+4*q1Dq2*A11d3-4*q1Dpb*A3d2+4*
     #q1Dpb*A6d3-4*q1Dp1b*A3d2+4*q1Dp1b*A6d3+4*q2Dpb*A6d3+4*q2Dp1b*A6d3+
     #4*q1Dq2*A12d3+4*mb**2*A3d2-4*q1Dpb*A9d3+4*pbDp1b*A3d2+12*A2d3-4*q1
     #Dp1b*A11d3+4*A7d2)
c
      cf_b1_2_a(11)=t0
c
c --------cf_b1_2_12a
c
      t0 = CA/den0x3*(4*mb**2*A3d2-4*q1Dpb*A9d3-12*mb**2*A10d3+12*mb**2*
     #A12d3-4*q1Dq2*D0-4*q1Dq2*A1d1-4*q1Dpb*A2d2+4*q1Dpb*A10d3+8*q1Dp1b*
     #A13d3+4*A7d2+4*pbDp1b*A3d2-8*pbDp1b*A6d2+12*A2d3+4*mb**2*A5d3+8*q1
     #Dpb*A13d3-4*q1Dp1b*A2d2-4*q1Dp1b*A9d3+4*q1Dp1b*A10d3+4*q2Dpb*A10d3
     #+4*q2Dp1b*A10d3+4*pbDp1b*A2d2+4*pbDp1b*A5d3-12*pbDp1b*A10d3+12*pbD
     #p1b*A12d3+8*q1Dp1b*A6d2-12*A3d3-4*mb**2*A6d3+4*q1Dq2*A5d2-4*q1Dq2*
     #A6d2-4*q1Dpb*A11d3-8*q1Dpb*A12d3-4*q1Dp1b*A11d3-8*q1Dp1b*A12d3-8*q
     #2Dpb*A12d3-8*q2Dp1b*A12d3-4*pbDp1b*A6d3-4*q1Dq2*A13d3-4*q1Dq2*A6d3
     #+4*q1Dq2*A11d3-4*q1Dpb*A3d2+4*q1Dpb*A6d3-4*q1Dp1b*A3d2+4*q1Dp1b*A6
     #d3+4*q2Dpb*A6d3+4*q2Dp1b*A6d3+4*q1Dq2*A12d3-8*mb**2*A6d2+4*mb**2*A
     #2d2+8*q1Dpb*A6d2)
c
      cf_b1_2_a(12)=t0
c
c --------cf_b1_2_13a
c
      cf_b1_2_a(13)=0d0
c
c --------cf_b1_2_14a
c
      cf_b1_2_a(14)=0d0
c
c --------cf_b1_2_15a
c
      cf_b1_2_a(15)=0d0
c
c --------cf_b1_2_16a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A3d2+16*A4d2-16*A5d2-16*A6d2-8*A6
     #d3+8*A7d3-8*A8d3+16*A11d3+8*A12d3-16*A13d3)
c
      cf_b1_2_a(16)=t0
c
c --------cf_b1_2_17a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1+8*A3d2-8*A4d2-8*A6d3+8*A11d3+8*A12d
     #3-8*A13d3)
c
      cf_b1_2_a(17)=t0
c
c --------cf_b1_2_18a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_a(18)=t0
c
c --------cf_b1_2_19a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_a(19)=t0
c
c --------cf_b1_2_20a
c
      t0 = CA/den0x3*(8*A5d2-8*A6d2-8*A6d3+8*A11d3+8*A12d3-8*A13d3)
c
      cf_b1_2_a(20)=t0
c
c --------cf_b1_2_21a
c
      t0 = CA/den0x3*(-8*A3d2+8*A6d2-8*A6d3+8*A12d3)
c
      cf_b1_2_a(21)=t0
c
c --------cf_b1_2_22a
c
      t0 = CA/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_a(22)=t0
c
c --------cf_b1_2_23a
c
      t0 = CA/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_a(23)=t0
c
c --------cf_b1_2_24a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+16*A3d2+16*A4d2-16*A5d2-16*A6d2-8*A6
     #d3+8*A7d3-8*A8d3+16*A11d3+8*A12d3-16*A13d3)
c
      cf_b1_2_a(24)=t0
c
c --------cf_b1_2_25a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1+8*A3d2-8*A4d2-8*A6d3+8*A11d3+8*A12d
     #3-8*A13d3)
c
      cf_b1_2_a(25)=t0
c
c --------cf_b1_2_26a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_a(26)=t0
c
c --------cf_b1_2_27a
c
      t0 = CA/den0x3*(-8*A2d1+8*A3d1-8*A2d2-16*A3d2-8*A4d2+8*A5d2+24*A6d
     #2+8*A6d3-8*A9d3+8*A10d3-8*A11d3-16*A12d3+16*A13d3)
c
      cf_b1_2_a(27)=t0
c
c --------cf_b1_2_28a
c
      t0 = CA/den0x3*(8*A5d2-8*A6d2-8*A6d3+8*A11d3+8*A12d3-8*A13d3)
c
      cf_b1_2_a(28)=t0
c
c --------cf_b1_2_29a
c
      t0 = CA/den0x3*(-8*A3d2+8*A6d2-8*A6d3+8*A12d3)
c
      cf_b1_2_a(29)=t0
c
c --------cf_b1_2_30a
c
      t0 = CA/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_a(30)=t0
c
c --------cf_b1_2_31a
c
      t0 = CA/den0x3*(8*A6d3+8*A10d3-16*A12d3)
c
      cf_b1_2_a(31)=t0
c
      return
      end


      subroutine wbb_coeff_b2(q1,q2,pb,p1b,pw,mb,cf_b2_v,cf_b2_a)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd
      real*8 wbb_dot_4,wbb_c0_sd
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,m1,m2,m3,m4,q1(4),q2(4),pb(4),p1b(4),pw(4),p1(4),
     -       p2(4),p3(4),p12(4),p23(4),p123(4),xs1,xs2,xs12,xm1,xm2
      real*8 CV,CA,den0x1,den0x2,den0x3,
     -       q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,t0
      real*8 AAi,B012,B112,B013,B113,B023,B123,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,A12d3,
     - A13d3,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real*8 cf_b2_v(75),cf_b2_a(75)
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
      common/wbb_gramdet/dflag
c
c debug parameter
c
      logical*4 ldebugb2
      parameter(ldebugb2=.false.)
c      parameter(ldebugb2=.true.)
c
      if(ldebugb2) then
      print*,' ------  Debugging wbb_coeff_b2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for B2 ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pb(i)-p1b(i)
         p3(i)=q2(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m(xs2,xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs1,xs12,C0134)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
      B123=0d0
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      call wbb_f3pf_2m(xs1,xs12,C0234)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      B023=0d0
      B123=0d0
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c --- D0
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xm1=wbb_dot_4(p2,p2)
      xm2=wbb_dot_4(p123,p123)
      call wbb_f4pf_b2(xs1,xs2,xm1,xm2,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b2_v(i)=0d0
         cf_b2_a(i)=0d0
      enddo
c
c ---------------------- coefficients for B2_v
c
c --------cf_b2_1v
c
      cf_b2_v(1)=0d0
c
c --------cf_b2_2v
c
      t0 = CV/den0x3*(-4*q1Dp1b*A1d1+4*q1Dq2*A5d2+4*pbDp1b*A9d3+8*
     #mb**2*A2d2+4*q1Dq2*A8d3+4*q1Dq2*A1d2+4*mb**2*A9d3+4*A1d3+4*mb**2*A
     #1d1+4*A7d2-4*q2Dp1b*A4d2-4*q1Dpb*A1d1-4*q1Dpb*A1d2-4*q1Dpb*A7d3-4*
     #q1Dp1b*A4d2-4*q1Dp1b*A7d3-4*q2Dpb*A1d1-8*q2Dpb*A2d1+8*q2Dpb*A3d1-4
     #*q2Dpb*A4d2+4*q2Dpb*A5d2-8*q2Dpb*A6d2-4*q2Dpb*A13d3-4*q2Dp1b*A1d1-
     #8*q2Dp1b*A2d1+8*q2Dp1b*A3d1+4*q2Dp1b*A5d2-8*q2Dp1b*A6d2-4*q2Dp1b*A
     #13d3+4*pbDp1b*A1d1+8*pbDp1b*A2d2-4*q1Dp1b*A1d2-4*q1Dpb*A4d2+4*q1Dq
     #2*A1d1)
c
      cf_b2_v(2)=t0
c
c --------cf_b2_3v
c
      t0 = CV/den0x3*(-4*q1Dp1b*A13d3-8*q1Dp1b*A3d1-8*mb**2*A2d1+8
     #*q1Dp1b*A1d1-4*q2Dp1b*A12d3+8*mb**2*A3d1-8*q1Dpb*A3d1-4*q2Dp1b*A6d
     #2+4*A3d3+8*q1Dpb*A1d1+8*pbDp1b*A6d2-8*q1Dpb*A5d2+4*mb**2*A10d3+8*p
     #bDp1b*A3d1+8*mb**2*A6d2-4*mb**2*A2d2+4*pbDp1b*A10d3+4*q1Dq2*A5d2+4
     #*q1Dpb*A4d2+4*q1Dp1b*A4d2-4*q2Dpb*A6d2+4*q1Dq2*A11d3-4*q2Dpb*A12d3
     #-4*q1Dpb*A13d3-8*q1Dp1b*A5d2-4*pbDp1b*A2d2-8*pbDp1b*A2d1)
c
      cf_b2_v(3)=t0
c
c --------cf_b2_4v
c
      t0 = CV/den0x3*(-8*mb**2*A2d1-12*mb**2*A2d2-4*mb**2*A5d3-8*A
     #7d2-4*A2d3-8*q1Dq2*A3d1-12*q1Dq2*A5d2-4*q1Dq2*A13d3+8*q1Dpb*A1d1+1
     #2*q1Dpb*A4d2+4*q1Dpb*A9d3+8*q1Dp1b*A1d1+12*q1Dp1b*A4d2+4*q1Dp1b*A9
     #d3+8*q2Dpb*A3d1+12*q2Dpb*A6d2+4*q2Dpb*A10d3+8*q2Dp1b*A3d1+12*q2Dp1
     #b*A6d2+4*q2Dp1b*A10d3-8*pbDp1b*A2d1-12*pbDp1b*A2d2-4*pbDp1b*A5d3)
c
      cf_b2_v(4)=t0
c
c --------cf_b2_5v
c
      t0 = CV/den0x3*(-8*mb**2*A2d1-12*mb**2*A2d2-4*mb**2*A5d3-8*A
     #7d2-4*A2d3-8*q1Dq2*A3d1-12*q1Dq2*A5d2-4*q1Dq2*A13d3+8*q1Dpb*A1d1+1
     #2*q1Dpb*A4d2+4*q1Dpb*A9d3+8*q1Dp1b*A1d1+12*q1Dp1b*A4d2+4*q1Dp1b*A9
     #d3+8*q2Dpb*A3d1+12*q2Dpb*A6d2+4*q2Dpb*A10d3+8*q2Dp1b*A3d1+12*q2Dp1
     #b*A6d2+4*q2Dp1b*A10d3-8*pbDp1b*A2d1-12*pbDp1b*A2d2-4*pbDp1b*A5d3)
c
      cf_b2_v(5)=t0
c
c --------cf_b2_6v
c
      cf_b2_v(6)=0d0
c
c --------cf_b2_7v
c
      t0 = CV/den0x3*(-4*mb**2*A2d2-4*mb**2*A9d3-8*A7d2-20*A1d3-4*
     #q1Dq2*A5d2-4*q1Dq2*A8d3+8*q1Dpb*A1d1+8*q1Dpb*A1d2+4*q1Dpb*A4d2+4*q
     #1Dpb*A7d3+8*q1Dp1b*A1d1+8*q1Dp1b*A1d2+4*q1Dp1b*A4d2+4*q1Dp1b*A7d3-
     #8*q2Dpb*A3d1-8*q2Dpb*A5d2+4*q2Dpb*A6d2+4*q2Dpb*A13d3-8*q2Dp1b*A3d1
     #-8*q2Dp1b*A5d2+4*q2Dp1b*A6d2+4*q2Dp1b*A13d3-4*pbDp1b*A2d2-4*pbDp1b
     #*A9d3)
c
      cf_b2_v(7)=t0
c
c --------cf_b2_8v
c
      t0 = CV/den0x3*(4*q2Dpb*A6d2+4*q2Dpb*A12d3-4*q1Dq2*A5d2-12*A
     #7d2+12*q1Dp1b*A3d1-4*q2Dpb*A3d1+4*q1Dpb*A13d3+12*q1Dpb*A3d1+12*q1D
     #pb*A5d2+4*q1Dp1b*A13d3-8*mb**2*A6d2-4*q2Dp1b*A3d1-4*mb**2*A10d3-4*
     #q1Dq2*A11d3-4*mb**2*A3d1-4*q1Dq2*A3d2+4*q2Dp1b*A6d2+12*q1Dp1b*A5d2
     #+4*q1Dp1b*A6d2-4*q2Dpb*A3d2-4*q2Dp1b*A3d2-4*q1Dq2*A3d1+4*q1Dpb*A6d
     #2-8*pbDp1b*A6d2-4*pbDp1b*A10d3-20*A3d3-4*pbDp1b*A3d1+4*q2Dp1b*A12d
     #3)
c
      cf_b2_v(8)=t0
c
c --------cf_b2_9v
c
      t0 = CV/den0x3*(8*mb**2*A2d1+12*mb**2*A2d2+4*mb**2*A5d3+16*A
     #7d2+20*A2d3+8*q1Dq2*A3d1+12*q1Dq2*A5d2+4*q1Dq2*A13d3-8*q1Dpb*A1d1-
     #8*q1Dpb*A2d1-20*q1Dpb*A4d2-4*q1Dpb*A9d3-8*q1Dp1b*A1d1-8*q1Dp1b*A2d
     #1-20*q1Dp1b*A4d2-4*q1Dp1b*A9d3-4*q2Dpb*A6d2-4*q2Dpb*A10d3-4*q2Dp1b
     #*A6d2-4*q2Dp1b*A10d3+8*pbDp1b*A2d1+12*pbDp1b*A2d2+4*pbDp1b*A5d3)
c
      cf_b2_v(9)=t0
c
c --------cf_b2_10v
c
      t0 = CV/den0x3*(8*mb**2*A2d1+12*mb**2*A2d2+4*mb**2*A5d3+16*
     #A7d2+20*A2d3+8*q1Dq2*A3d1+12*q1Dq2*A5d2+4*q1Dq2*A13d3-8*q1Dpb*A1d1
     #-8*q1Dpb*A2d1-20*q1Dpb*A4d2-4*q1Dpb*A9d3-8*q1Dp1b*A1d1-8*q1Dp1b*A2
     #d1-20*q1Dp1b*A4d2-4*q1Dp1b*A9d3-4*q2Dpb*A6d2-4*q2Dpb*A10d3-4*q2Dp1
     #b*A6d2-4*q2Dp1b*A10d3+8*pbDp1b*A2d1+12*pbDp1b*A2d2+4*pbDp1b*A5d3)
c
      cf_b2_v(10)=t0
c
c --------cf_b2_11v
c
      t0 = CV/den0x3*(2*q2Dp1b*A2d2-4*q2Dpb*A3d1+6*mb**2*A2d1+6*p
     #bDp1b*A2d1+8*q1Dq2*A3d1+2*q1Dpb*A2d2-2*q1Dq2*A4d2-2*q1Dp1b*A2d1+6*
     #mb**2*A2d2-4*q2Dp1b*A3d1-2*q1Dq2*A2d1-8*q2Dp1b*A6d2-2*q1Dq2*A6d2-4
     #*q1Dpb*A1d1+2*q2Dpb*A2d2+10*q1Dq2*A5d2-8*q1Dpb*A4d2+6*pbDp1b*A2d2+
     #2*q1Dp1b*A2d2-8*q1Dp1b*A4d2-4*q1Dp1b*A1d1-8*q2Dpb*A6d2-2*q1Dpb*A2d
     #1+6*A7d2-2*q2Dpb*A2d1-2*q2Dp1b*A2d1)
c
      cf_b2_v(11)=t0
c
c --------cf_b2_12v
c
      t0 = CV/den0x3*(-4*q2Dp1b*A3d1-4*q1Dpb*A1d1+2*q2Dpb*A2d2+2*
     #q2Dp1b*A2d2-2*q1Dq2*A4d2+2*q1Dp1b*A2d2+6*A7d2+2*q1Dpb*A2d2-2*q1Dq2
     #*A6d2-2*q2Dp1b*A2d1+6*pbDp1b*A2d2+6*pbDp1b*A2d1-4*q2Dpb*A3d1-4*q1D
     #p1b*A1d1-2*q2Dpb*A2d1-8*q2Dp1b*A6d2+6*mb**2*A2d2+8*q1Dq2*A3d1-8*q2
     #Dpb*A6d2-8*q1Dp1b*A4d2-2*q1Dq2*A2d1-2*q1Dp1b*A2d1-2*q1Dpb*A2d1-8*q
     #1Dpb*A4d2+10*q1Dq2*A5d2+6*mb**2*A2d1)
c
      cf_b2_v(12)=t0
c
c --------cf_b2_13v
c
      cf_b2_v(13)=0d0
c
c --------cf_b2_14v
c
      cf_b2_v(14)=0d0
c
c --------cf_b2_15v
c
      cf_b2_v(15)=0d0
c
c --------cf_b2_16v
c
      t0 = CV/den0x3*(8*A4d2+8*A7d3)
c
      cf_b2_v(16)=t0
c
c --------cf_b2_17v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+8*A4d2-8*A5d2+8*A6d2+8*A13d3)
c
      cf_b2_v(17)=t0
c
c --------cf_b2_18v
c
      t0 = CV/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_v(18)=t0
c
c --------cf_b2_19v
c
      t0 = CV/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_v(19)=t0
c
c --------cf_b2_20v
c
      t0 = CV/den0x3*(8*A3d1+16*A5d2+8*A13d3)
c
      cf_b2_v(20)=t0
c
c --------cf_b2_21v
c
      t0 = CV/den0x3*(8*A3d1+8*A3d2+8*A6d2+8*A12d3)
c
      cf_b2_v(21)=t0
c
c --------cf_b2_22v
c
      t0 = CV/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_v(22)=t0
c
c --------cf_b2_23v
c
      t0 = CV/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_v(23)=t0
c
c --------cf_b2_24v
c
      t0 = CV/den0x3*(8*A4d2+8*A7d3)
c
      cf_b2_v(24)=t0
c
c --------cf_b2_25v
c
      t0 = CV/den0x3*(8*A2d1-8*A3d1+8*A4d2-8*A5d2+8*A6d2+8*A13d3)
c
      cf_b2_v(25)=t0
c
c --------cf_b2_26v
c
      t0 = CV/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_v(26)=t0
c
c --------cf_b2_27v
c
      t0 = CV/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_v(27)=t0
c
c --------cf_b2_28v
c
      t0 = CV/den0x3*(8*A3d1+16*A5d2+8*A13d3)
c
      cf_b2_v(28)=t0
c
c --------cf_b2_29v
c
      t0 = CV/den0x3*(8*A3d1+8*A3d2+8*A6d2+8*A12d3)
c
      cf_b2_v(29)=t0
c
c --------cf_b2_30v
c
      t0 = CV/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_v(30)=t0
c
c --------cf_b2_31v
c
      t0 = CV/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_v(31)=t0
c
c
c ---------------------- coefficients for b2_a
c
c --------cf_b2_1a
c
      cf_b2_a(1)=0d0
c
c --------cf_b2_2a
c
      t0 = CA/den0x3*(-4*q1Dpb*A7d3+4*A7d2-4*q1Dpb*A1d2+4*q1Dq2*A1
     #d1-4*q1Dp1b*A7d3+4*q1Dq2*A1d2-4*q2Dpb*A1d1-4*q1Dpb*A1d1+4*mb**2*A1
     #d1-4*q1Dp1b*A4d2-4*q1Dpb*A4d2+4*A1d3+8*q2Dpb*A3d1-8*q2Dpb*A2d1+4*q
     #1Dq2*A8d3-4*q1Dp1b*A1d2+4*q1Dq2*A5d2-4*q2Dpb*A13d3-4*q2Dp1b*A1d1-8
     #*q2Dp1b*A2d1+8*q2Dp1b*A3d1-4*q2Dp1b*A4d2+4*q2Dp1b*A5d2-8*q2Dp1b*A6
     #d2-4*q2Dp1b*A13d3+4*pbDp1b*A1d1+8*pbDp1b*A2d2+4*pbDp1b*A9d3+4*mb**
     #2*A9d3-4*q2Dpb*A4d2-8*q2Dpb*A6d2+4*q2Dpb*A5d2-4*q1Dp1b*A1d1+8*mb**
     #2*A2d2)
c
      cf_b2_a(2)=t0
c
c --------cf_b2_3a
c
      t0 = CA/den0x3*(4*q1Dpb*A4d2-8*pbDp1b*A2d1-4*pbDp1b*A2d2-8*q
     #1Dpb*A3d1+8*q1Dpb*A1d1-8*q1Dpb*A5d2+4*A3d3+8*mb**2*A6d2+8*pbDp1b*A
     #3d1-4*mb**2*A2d2+8*pbDp1b*A6d2-4*q2Dpb*A6d2+8*q1Dp1b*A1d1-4*q2Dp1b
     #*A12d3+4*q1Dq2*A5d2+4*pbDp1b*A10d3-4*q2Dpb*A12d3+8*mb**2*A3d1-4*q1
     #Dpb*A13d3+4*mb**2*A10d3+4*q1Dp1b*A4d2+4*q1Dq2*A11d3-8*q1Dp1b*A5d2-
     #8*q1Dp1b*A3d1-8*mb**2*A2d1-4*q2Dp1b*A6d2-4*q1Dp1b*A13d3)
c
      cf_b2_a(3)=t0
c
c --------cf_b2_4a
c
      t0 = CA/den0x3*(-8*mb**2*A2d1-12*mb**2*A2d2-4*mb**2*A5d3-8*A
     #7d2-4*A2d3-8*q1Dq2*A3d1-12*q1Dq2*A5d2-4*q1Dq2*A13d3+8*q1Dpb*A1d1+1
     #2*q1Dpb*A4d2+4*q1Dpb*A9d3+8*q1Dp1b*A1d1+12*q1Dp1b*A4d2+4*q1Dp1b*A9
     #d3+8*q2Dpb*A3d1+12*q2Dpb*A6d2+4*q2Dpb*A10d3+8*q2Dp1b*A3d1+12*q2Dp1
     #b*A6d2+4*q2Dp1b*A10d3-8*pbDp1b*A2d1-12*pbDp1b*A2d2-4*pbDp1b*A5d3)
c
      cf_b2_a(4)=t0
c
c --------cf_b2_5a
c
      t0 = CA/den0x3*(-8*mb**2*A2d1-12*mb**2*A2d2-4*mb**2*A5d3-8*A
     #7d2-4*A2d3-8*q1Dq2*A3d1-12*q1Dq2*A5d2-4*q1Dq2*A13d3+8*q1Dpb*A1d1+1
     #2*q1Dpb*A4d2+4*q1Dpb*A9d3+8*q1Dp1b*A1d1+12*q1Dp1b*A4d2+4*q1Dp1b*A9
     #d3+8*q2Dpb*A3d1+12*q2Dpb*A6d2+4*q2Dpb*A10d3+8*q2Dp1b*A3d1+12*q2Dp1
     #b*A6d2+4*q2Dp1b*A10d3-8*pbDp1b*A2d1-12*pbDp1b*A2d2-4*pbDp1b*A5d3)
c
      cf_b2_a(5)=t0
c
c --------cf_b2_6a
c
      cf_b2_a(6)=0d0
c
c --------cf_b2_7a
c
      t0 = CA/den0x3*(-4*mb**2*A2d2-4*mb**2*A9d3-8*A7d2-20*A1d3-4*
     #q1Dq2*A5d2-4*q1Dq2*A8d3+8*q1Dpb*A1d1+8*q1Dpb*A1d2+4*q1Dpb*A4d2+4*q
     #1Dpb*A7d3+8*q1Dp1b*A1d1+8*q1Dp1b*A1d2+4*q1Dp1b*A4d2+4*q1Dp1b*A7d3-
     #8*q2Dpb*A3d1-8*q2Dpb*A5d2+4*q2Dpb*A6d2+4*q2Dpb*A13d3-8*q2Dp1b*A3d1
     #-8*q2Dp1b*A5d2+4*q2Dp1b*A6d2+4*q2Dp1b*A13d3-4*pbDp1b*A2d2-4*pbDp1b
     #*A9d3)
c
      cf_b2_a(7)=t0
c
c --------cf_b2_8a
c
      t0 = CA/den0x3*(-4*q2Dp1b*A3d1+4*q2Dpb*A6d2-12*A7d2-4*q2Dpb*
     #A3d1-4*q2Dp1b*A3d2-4*q1Dq2*A5d2-8*mb**2*A6d2+4*q2Dpb*A12d3+4*q1Dpb
     #*A6d2+4*q1Dp1b*A6d2-4*q2Dpb*A3d2-8*pbDp1b*A6d2-4*pbDp1b*A10d3-4*q1
     #Dq2*A3d1-4*q1Dq2*A3d2+12*q1Dp1b*A3d1+12*q1Dp1b*A5d2+4*q1Dp1b*A13d3
     #+4*q2Dp1b*A12d3-20*A3d3-4*pbDp1b*A3d1+12*q1Dpb*A5d2+4*q1Dpb*A13d3-
     #4*q1Dq2*A11d3+12*q1Dpb*A3d1+4*q2Dp1b*A6d2-4*mb**2*A3d1-4*mb**2*A10
     #d3)
c
      cf_b2_a(8)=t0
c
c --------cf_b2_9a
c
      t0 = CA/den0x3*(8*mb**2*A2d1+12*mb**2*A2d2+4*mb**2*A5d3+16*A
     #7d2+20*A2d3+8*q1Dq2*A3d1+12*q1Dq2*A5d2+4*q1Dq2*A13d3-8*q1Dpb*A1d1-
     #8*q1Dpb*A2d1-20*q1Dpb*A4d2-4*q1Dpb*A9d3-8*q1Dp1b*A1d1-8*q1Dp1b*A2d
     #1-20*q1Dp1b*A4d2-4*q1Dp1b*A9d3-4*q2Dpb*A6d2-4*q2Dpb*A10d3-4*q2Dp1b
     #*A6d2-4*q2Dp1b*A10d3+8*pbDp1b*A2d1+12*pbDp1b*A2d2+4*pbDp1b*A5d3)
c
      cf_b2_a(9)=t0
c
c --------cf_b2_10a
c
      t0 = CA/den0x3*(8*mb**2*A2d1+12*mb**2*A2d2+4*mb**2*A5d3+16*
     #A7d2+20*A2d3+8*q1Dq2*A3d1+12*q1Dq2*A5d2+4*q1Dq2*A13d3-8*q1Dpb*A1d1
     #-8*q1Dpb*A2d1-20*q1Dpb*A4d2-4*q1Dpb*A9d3-8*q1Dp1b*A1d1-8*q1Dp1b*A2
     #d1-20*q1Dp1b*A4d2-4*q1Dp1b*A9d3-4*q2Dpb*A6d2-4*q2Dpb*A10d3-4*q2Dp1
     #b*A6d2-4*q2Dp1b*A10d3+8*pbDp1b*A2d1+12*pbDp1b*A2d2+4*pbDp1b*A5d3)
c
      cf_b2_a(10)=t0
c
c --------cf_b2_11a
c
      t0 = CA/den0x3*(-2*q1Dq2*A6d2+2*q2Dp1b*A2d2-2*q1Dq2*A4d2-2*
     #q2Dp1b*A2d1-4*q2Dpb*A3d1-4*q1Dp1b*A1d1-2*q1Dp1b*A2d1-4*q1Dpb*A1d1+
     #6*pbDp1b*A2d2-2*q1Dpb*A2d1+6*mb**2*A2d1+6*pbDp1b*A2d1-4*q2Dp1b*A3d
     #1-8*q2Dp1b*A6d2+6*A7d2-2*q2Dpb*A2d1+8*q1Dq2*A3d1+10*q1Dq2*A5d2+2*q
     #1Dp1b*A2d2+6*mb**2*A2d2+2*q1Dpb*A2d2-8*q1Dpb*A4d2-8*q1Dp1b*A4d2-8*
     #q2Dpb*A6d2-2*q1Dq2*A2d1+2*q2Dpb*A2d2)
c
      cf_b2_a(11)=t0
c
c --------cf_b2_12a
c
      t0 = CA/den0x3*(-2*q1Dq2*A2d1-2*q1Dpb*A2d1-2*q2Dpb*A2d1+2*q
     #1Dp1b*A2d2-4*q1Dp1b*A1d1+6*pbDp1b*A2d2-2*q1Dq2*A4d2+6*pbDp1b*A2d1+
     #6*A7d2-4*q2Dp1b*A3d1-4*q2Dpb*A3d1+10*q1Dq2*A5d2+6*mb**2*A2d2-8*q2D
     #pb*A6d2-2*q1Dq2*A6d2-8*q1Dp1b*A4d2+2*q1Dpb*A2d2+8*q1Dq2*A3d1-8*q1D
     #pb*A4d2-4*q1Dpb*A1d1-8*q2Dp1b*A6d2+2*q2Dpb*A2d2-2*q2Dp1b*A2d1+6*mb
     #**2*A2d1-2*q1Dp1b*A2d1+2*q2Dp1b*A2d2)
c
      cf_b2_a(12)=t0
c
c --------cf_b2_13a
c
      cf_b2_a(13)=0d0
c
c --------cf_b2_14a
c
      cf_b2_a(14)=0d0
c
c --------cf_b2_15a
c
      cf_b2_a(15)=0d0
c
c --------cf_b2_16a
c
      t0 = CA/den0x3*(8*A4d2+8*A7d3)
c
      cf_b2_a(16)=t0
c
c --------cf_b2_17a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+8*A4d2-8*A5d2+8*A6d2+8*A13d3)
c
      cf_b2_a(17)=t0
c
c --------cf_b2_18a
c
      t0 = CA/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_a(18)=t0
c
c --------cf_b2_19a
c
      t0 = CA/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_a(19)=t0
c
c --------cf_b2_20a
c
      t0 = CA/den0x3*(8*A3d1+16*A5d2+8*A13d3)
c
      cf_b2_a(20)=t0
c
c --------cf_b2_21a
c
      t0 = CA/den0x3*(8*A3d1+8*A3d2+8*A6d2+8*A12d3)
c
      cf_b2_a(21)=t0
c
c --------cf_b2_22a
c
      t0 = CA/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_a(22)=t0
c
c --------cf_b2_23a
c
      t0 = CA/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_a(23)=t0
c
c --------cf_b2_24a
c
      t0 = CA/den0x3*(8*A4d2+8*A7d3)
c
      cf_b2_a(24)=t0
c
c --------cf_b2_25a
c
      t0 = CA/den0x3*(8*A2d1-8*A3d1+8*A4d2-8*A5d2+8*A6d2+8*A13d3)
c
      cf_b2_a(25)=t0
c
c --------cf_b2_26a
c
      t0 = CA/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_a(26)=t0
c
c --------cf_b2_27a
c
      t0 = CA/den0x3*(-8*A2d2-8*A9d3)
c
      cf_b2_a(27)=t0
c
c --------cf_b2_28a
c
      t0 = CA/den0x3*(8*A3d1+16*A5d2+8*A13d3)
c
      cf_b2_a(28)=t0
c
c --------cf_b2_29a
c
      t0 = CA/den0x3*(8*A3d1+8*A3d2+8*A6d2+8*A12d3)
c
      cf_b2_a(29)=t0
c
c --------cf_b2_30a
c
      t0 = CA/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_a(30)=t0
c
c --------cf_b2_31a
c
      t0 = CA/den0x3*(-8*A3d1-16*A6d2-8*A10d3)
c
      cf_b2_a(31)=t0
c
      return
      end

      subroutine wbb_coeff_b3(q1,q2,pb,p1b,pw,mb,cf_b3_1_v,cf_b3_1_a,
     -  cf_b3_1c_v,cf_b3_1c_a,cf_b3_2_v,cf_b3_2_a,cf_b3_2c_v,cf_b3_2c_a)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd
      real*8 wbb_dot_4,wbb_c0_sd
c
c external wbb_masses and momenta
c
      integer i,dflag
      real*8 mb,m1,m2,m3,m4,q1(4),q2(4),pb(4),p1b(4),pw(4),p1(4),
     -       p2(4),p3(4),p12(4),p23(4),p123(4),xs1,xs2,xs12
      real*8 CV,CA,den0x1,den0x2,den0x3,
     -       q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,t0
      real*8 AAi,B012,B112,B013,B113,B023,B123,
c     - A1c1,A2c1,A1c2,A2c2,A3c2,A4c2,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,A12d3,
     - A13d3,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real*8 cf_b3_1_v(75),cf_b3_1_a(75),cf_b3_1c_v(75),cf_b3_1c_a(75),
     - cf_b3_2_v(75),cf_b3_2_a(75),cf_b3_2c_v(75),cf_b3_2c_a(75)
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
      common/wbb_gramdet/dflag
c
c test paramters
c
c
c debug parameter
c
      logical*4 ldebugb3
      parameter(ldebugb3=.false.)
c      parameter(ldebugb3=.true.)
c
      if(ldebugb3) then
      print*,' ------  Debugging wbb_coeff_b3  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for B3_1 ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-p1b(i)
         p3(i)=-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
c --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c
c --- D0 
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs12,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b3_1_v(i)=0d0
         cf_b3_1_a(i)=0d0
      enddo
c
c ---------------------- coefficients for B3_1_v
c
c --------cf_b3_1_1v
c
      t0 = CV/den0x1*(4*mb**3*A6d2+4*mb**3*A3d2+8*mb**3*A3d1-8*q2Dpb*mb*
     #A3d2-8*q2Dpb*mb*A3d1-8*q2Dp1b*mb*A6d2-8*q2Dp1b*mb*A3d1+8*pbDp1b*mb
     #*A6d2+8*pbDp1b*mb*A3d1)
c
      cf_b3_1_v(1)=t0
c
c --------cf_b3_1_3v
c
      t0 = CV/den0x1*(-32*A7d2-8*mb**2*A5d2-8*mb**2*A2d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+16*q2Dpb*A5d2+8*q2Dpb*A3d1+16*q2Dp1b*A4d2+8*q2Dp1b*A2d1
     #+8*q2Dp1b*A1d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1_v(3)=t0
c
c --------cf_b3_1_4v
c
      t0 = CV/den0x1*(-32*A7d2+8*mb**2*A6d2-8*mb**2*A2d2+4*mb**2*A3d1-4*
     #mb**2*A2d1-16*q2Dp1b*A6d2+16*q2Dp1b*A4d2-8*q2Dp1b*A3d1+8*q2Dp1b*A2
     #d1+8*q2Dp1b*A1d1)
c
      cf_b3_1_v(4)=t0
c
c --------cf_b3_1_5v
c
      t0 = CV/den0x1*(-32*A7d2-8*mb**2*A6d2-8*mb**2*A2d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+16*q2Dpb*A6d2+8*q2Dpb*A3d1+16*q2Dp1b*A4d2+8*q2Dp1b*A2d1
     #+8*q2Dp1b*A1d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1_v(5)=t0
c
c --------cf_b3_1_9v
c
      t0 = CV/den0x1*(40*A7d2-8*mb**2*A6d2+8*mb**2*A2d2-4*mb**2*A3d1+4*m
     #b**2*A2d1+16*q2Dp1b*A6d2-16*q2Dp1b*A4d2+8*q2Dp1b*A3d1-8*q2Dp1b*A2d
     #1-8*q2Dp1b*A1d1)
c
      cf_b3_1_v(9)=t0
c
c --------cf_b3_1_10v
c
      t0 = CV/den0x1*(40*A7d2+8*mb**2*A6d2+8*mb**2*A2d2+4*mb**2*A3d1+4*m
     #b**2*A2d1-16*q2Dpb*A6d2-8*q2Dpb*A3d1-16*q2Dp1b*A4d2-8*q2Dp1b*A2d1-
     #8*q2Dp1b*A1d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_1_v(10)=t0
c
c --------cf_b3_1_11v
c
      t0 = CV/den0x1*(8*A7d2-4*mb**2*A6d2+4*mb**2*A2d2-2*mb**2*A3d1+2*mb
     #**2*A2d1+8*q2Dp1b*A6d2-8*q2Dp1b*A4d2+4*q2Dp1b*A3d1-4*q2Dp1b*A2d1-4
     #*q2Dp1b*A1d1)
c
      cf_b3_1_v(11)=t0
c
c --------cf_b3_1_12v
c
      t0 = CV/den0x1*(8*A7d2+4*mb**2*A6d2+4*mb**2*A2d2+2*mb**2*A3d1+2*mb
     #**2*A2d1-8*q2Dpb*A6d2-4*q2Dpb*A3d1-8*q2Dp1b*A4d2-4*q2Dp1b*A2d1-4*q
     #2Dp1b*A1d1+8*pbDp1b*A6d2+4*pbDp1b*A3d1)
c
      cf_b3_1_v(12)=t0
c
c --------cf_b3_1_13v
c
      t0 = CV/den0x1*(-4*mb*A6d2+4*mb*A3d2)
c
      cf_b3_1_v(13)=t0
c
c --------cf_b3_1_14v
c
      t0 = CV/den0x1*(24*mb*A6d2-16*mb*A3d2-8*mb*A2d2)
c
      cf_b3_1_v(14)=t0
c
c --------cf_b3_1_15v
c
      t0 = CV/den0x1*(-24*mb*A6d2+16*mb*A3d2+8*mb*A2d2)
c
      cf_b3_1_v(15)=t0
c
c --------cf_b3_1_23v
c
      t0 = CV/den0x1*(16*A6d2-24*A5d2+8*A4d2-8*A3d1+8*A2d1)
c
      cf_b3_1_v(23)=t0
c
c --------cf_b3_1_30v
c
      t0 = CV/den0x1*(-16*A6d2+24*A5d2-8*A4d2+8*A3d1-8*A2d1)
c
      cf_b3_1_v(30)=t0
c
c --------cf_b3_1_32v
c
      t0 = CV/den0x1*(-8*mb*A6d2-8*mb*A5d2+8*mb*A4d2+8*mb*A3d2-4*mb*A3d1
     #+4*mb*A2d1)
c
      cf_b3_1_v(32)=t0
c
c --------cf_b3_1_33v
c
      t0 = CV/den0x1*(8*mb*A6d2-8*mb*A5d2+8*mb*A4d2-8*mb*A2d2-4*mb*A3d1+
     #4*mb*A2d1)
c
      cf_b3_1_v(33)=t0
c
c --------cf_b3_1_34v
c
      t0 = CV/den0x1*(8*mb*A6d2+8*mb*A5d2-8*mb*A4d2-8*mb*A3d2+4*mb*A3d1-
     #4*mb*A2d1)
c
      cf_b3_1_v(34)=t0
c
c --------cf_b3_1_35v
c
      t0 = CV/den0x1*(-8*mb*A6d2+8*mb*A5d2-8*mb*A4d2+8*mb*A2d2+4*mb*A3d1
     #-4*mb*A2d1)
c
      cf_b3_1_v(35)=t0
c
c --------cf_b3_1_36v
c
      t0 = CV/den0x1*(4*mb**3*A3d2-4*mb**3*A2d2+8*q2Dpb*mb*A6d2-8*q2Dpb*
     #mb*A3d2-8*q2Dp1b*mb*A6d2+8*q2Dp1b*mb*A2d2+8*pbDp1b*mb*A6d2-8*pbDp1
     #b*mb*A2d2)
c
      cf_b3_1_v(36)=t0
c
c --------cf_b3_1_37v
c
      t0 = CV/den0x1*(-24*A7d2-4*mb**2*A5d2-4*mb**2*A4d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+8*q2Dpb*A5d2+8*q2Dpb*A3d1+8*q2Dp1b*A4d2+8*q2Dp1b*A2d1-8
     #*pbDp1b*A4d2-8*pbDp1b*A2d1)
c
      cf_b3_1_v(37)=t0
c
c --------cf_b3_1_38v
c
      t0 = CV/den0x1*(8*A6d2-8*A5d2)
c
      cf_b3_1_v(38)=t0
c
c --------cf_b3_1_39v
c
      t0 = CV/den0x1*(-8*mb*A6d2+4*mb*A3d2+4*mb*A2d2)
c
      cf_b3_1_v(39)=t0
c
c --------cf_b3_1_40v
c
      t0 = CV/den0x1*(-4*A5d2+4*A4d2-4*A3d1+4*A2d1)
c
      cf_b3_1_v(40)=t0
c
c --------cf_b3_1_41v
c
      t0 = 6*CV/den0x1*A7d2
c
      cf_b3_1_v(41)=t0
c
c --------cf_b3_1_43v
c
      t0 = CV/den0x1*(4*mb*A6d2-2*mb*A3d2-2*mb*A2d2)
c
      cf_b3_1_v(43)=t0
c
c --------cf_b3_1_45v
c
      t0 = CV/den0x1*(2*A5d2-2*A4d2+2*A3d1-2*A2d1)
c
      cf_b3_1_v(45)=t0
c
c --------cf_b3_1_46v
c
      t0 = 6*CV/den0x1*A7d2
c
      cf_b3_1_v(46)=t0
c
c --------cf_b3_1_47v
c
      t0 = CV/den0x1*(-4*mb*A6d2+2*mb*A3d2+2*mb*A2d2)
c
      cf_b3_1_v(47)=t0
c
c --------cf_b3_1_49v
c
      t0 = CV/den0x1*(-2*A5d2+2*A4d2-2*A3d1+2*A2d1)
c
      cf_b3_1_v(49)=t0
c
c --------cf_b3_1_51v
c
      t0 = CV/den0x1*(4*mb*A6d2+4*mb*A5d2-4*mb*A4d2-4*mb*A3d2+2*mb*A3d1-
     #2*mb*A2d1)
c
      cf_b3_1_v(51)=t0
c
c --------cf_b3_1_52v
c
      t0 = CV/den0x1*(-4*mb*A6d2+4*mb*A5d2-4*mb*A4d2+4*mb*A2d2+2*mb*A3d1
     #-2*mb*A2d1)
c
      cf_b3_1_v(52)=t0
c
c --------cf_b3_1_53v
c
      t0 = CV/den0x1*(-mb**3*A3d2+mb**3*A2d2-2*q2Dpb*mb*A6d2+2*q2Dpb*mb*
     #A3d2+2*q2Dp1b*mb*A6d2-2*q2Dp1b*mb*A2d2-2*pbDp1b*mb*A6d2+2*pbDp1b*m
     #b*A2d2)
c
      cf_b3_1_v(53)=t0
c
c --------cf_b3_1_54v
c
      t0 = CV/den0x1*(6*A7d2+mb**2*A5d2+mb**2*A4d2+mb**2*A3d1+mb**2*A2d1
     #-2*q2Dpb*A5d2-2*q2Dpb*A3d1-2*q2Dp1b*A4d2-2*q2Dp1b*A2d1+2*pbDp1b*A4
     #d2+2*pbDp1b*A2d1)
c
      cf_b3_1_v(54)=t0
c
c --------cf_b3_1_55v
c
      t0 = -2*CV/den0x1*A7d2
c
      cf_b3_1_v(55)=t0
c
c --------cf_b3_1_56v
c
      t0 = -2*CV/den0x1*A7d2
c
      cf_b3_1_v(56)=t0
c
c --------cf_b3_1_57v
c
      t0 = CV/den0x1*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_1_v(57)=t0
c
c --------cf_b3_1_58v
c
      t0 = CV/den0x1*(A5d2-A4d2+A3d1-A2d1)
c
      cf_b3_1_v(58)=t0
c
c --------cf_b3_1_59v
c
      t0 = -CV/den0x1*A7d2
c
      cf_b3_1_v(59)=t0
c
c --------cf_b3_1_60v
c
      t0 = -CV/den0x1*A7d2
c
      cf_b3_1_v(60)=t0
c
c
c ---------------------- coefficients for B3_1_1_a
c
c --------cf_b3_1_1a
c
      t0 = CA/den0x1*(4*mb**3*A6d2+4*mb**3*A3d2+8*mb**3*A3d1-8*q2Dpb*mb*
     #A3d2-8*q2Dpb*mb*A3d1-8*q2Dp1b*mb*A6d2-8*q2Dp1b*mb*A3d1+8*pbDp1b*mb
     #*A6d2+8*pbDp1b*mb*A3d1)
c
      cf_b3_1_a(1)=t0
c
c --------cf_b3_1_3a
c
      t0 = CA/den0x1*(-32*A7d2-8*mb**2*A5d2-8*mb**2*A2d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+16*q2Dpb*A5d2+8*q2Dpb*A3d1+16*q2Dp1b*A4d2+8*q2Dp1b*A2d1
     #+8*q2Dp1b*A1d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1_a(3)=t0
c
c --------cf_b3_1_4a
c
      t0 = CA/den0x1*(-32*A7d2+8*mb**2*A6d2-8*mb**2*A2d2+4*mb**2*A3d1-4*
     #mb**2*A2d1-16*q2Dp1b*A6d2+16*q2Dp1b*A4d2-8*q2Dp1b*A3d1+8*q2Dp1b*A2
     #d1+8*q2Dp1b*A1d1)
c
      cf_b3_1_a(4)=t0
c
c --------cf_b3_1_5a
c
      t0 = CA/den0x1*(-32*A7d2-8*mb**2*A6d2-8*mb**2*A2d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+16*q2Dpb*A6d2+8*q2Dpb*A3d1+16*q2Dp1b*A4d2+8*q2Dp1b*A2d1
     #+8*q2Dp1b*A1d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1_a(5)=t0
c
c --------cf_b3_1_9a
c
      t0 = CA/den0x1*(40*A7d2-8*mb**2*A6d2+8*mb**2*A2d2-4*mb**2*A3d1+4*m
     #b**2*A2d1+16*q2Dp1b*A6d2-16*q2Dp1b*A4d2+8*q2Dp1b*A3d1-8*q2Dp1b*A2d
     #1-8*q2Dp1b*A1d1)
c
      cf_b3_1_a(9)=t0
c
c --------cf_b3_1_10a
c
      t0 = CA/den0x1*(40*A7d2+8*mb**2*A6d2+8*mb**2*A2d2+4*mb**2*A3d1+4*m
     #b**2*A2d1-16*q2Dpb*A6d2-8*q2Dpb*A3d1-16*q2Dp1b*A4d2-8*q2Dp1b*A2d1-
     #8*q2Dp1b*A1d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_1_a(10)=t0
c
c --------cf_b3_1_11a
c
      t0 = CA/den0x1*(8*A7d2-4*mb**2*A6d2+4*mb**2*A2d2-2*mb**2*A3d1+2*mb
     #**2*A2d1+8*q2Dp1b*A6d2-8*q2Dp1b*A4d2+4*q2Dp1b*A3d1-4*q2Dp1b*A2d1-4
     #*q2Dp1b*A1d1)
c
      cf_b3_1_a(11)=t0
c
c --------cf_b3_1_12a
c
      t0 = CA/den0x1*(8*A7d2+4*mb**2*A6d2+4*mb**2*A2d2+2*mb**2*A3d1+2*mb
     #**2*A2d1-8*q2Dpb*A6d2-4*q2Dpb*A3d1-8*q2Dp1b*A4d2-4*q2Dp1b*A2d1-4*q
     #2Dp1b*A1d1+8*pbDp1b*A6d2+4*pbDp1b*A3d1)
c
      cf_b3_1_a(12)=t0
c
c --------cf_b3_1_13a
c
      t0 = CA/den0x1*(-4*mb*A6d2+4*mb*A3d2)
c
      cf_b3_1_a(13)=t0
c
c --------cf_b3_1_14a
c
      t0 = CA/den0x1*(24*mb*A6d2-16*mb*A3d2-8*mb*A2d2)
c
      cf_b3_1_a(14)=t0
c
c --------cf_b3_1_15a
c
      t0 = CA/den0x1*(-24*mb*A6d2+16*mb*A3d2+8*mb*A2d2)
c
      cf_b3_1_a(15)=t0
c
c --------cf_b3_1_23a
c
      t0 = CA/den0x1*(16*A6d2-24*A5d2+8*A4d2-8*A3d1+8*A2d1)
c
      cf_b3_1_a(23)=t0
c
c --------cf_b3_1_30a
c
      t0 = CA/den0x1*(-16*A6d2+24*A5d2-8*A4d2+8*A3d1-8*A2d1)
c
      cf_b3_1_a(30)=t0
c
c --------cf_b3_1_32a
c
      t0 = CA/den0x1*(-8*mb*A6d2-8*mb*A5d2+8*mb*A4d2+8*mb*A3d2-4*mb*A3d1
     #+4*mb*A2d1)
c
      cf_b3_1_a(32)=t0
c
c --------cf_b3_1_33a
c
      t0 = CA/den0x1*(8*mb*A6d2-8*mb*A5d2+8*mb*A4d2-8*mb*A2d2-4*mb*A3d1+
     #4*mb*A2d1)
c
      cf_b3_1_a(33)=t0
c
c --------cf_b3_1_34a
c
      t0 = CA/den0x1*(8*mb*A6d2+8*mb*A5d2-8*mb*A4d2-8*mb*A3d2+4*mb*A3d1-
     #4*mb*A2d1)
c
      cf_b3_1_a(34)=t0
c
c --------cf_b3_1_35a
c
      t0 = CA/den0x1*(-8*mb*A6d2+8*mb*A5d2-8*mb*A4d2+8*mb*A2d2+4*mb*A3d1
     #-4*mb*A2d1)
c
      cf_b3_1_a(35)=t0
c
c --------cf_b3_1_36a
c
      t0 = CA/den0x1*(4*mb**3*A3d2-4*mb**3*A2d2+8*q2Dpb*mb*A6d2-8*q2Dpb*
     #mb*A3d2-8*q2Dp1b*mb*A6d2+8*q2Dp1b*mb*A2d2+8*pbDp1b*mb*A6d2-8*pbDp1
     #b*mb*A2d2)
c
      cf_b3_1_a(36)=t0
c
c --------cf_b3_1_37a
c
      t0 = CA/den0x1*(-24*A7d2-4*mb**2*A5d2-4*mb**2*A4d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+8*q2Dpb*A5d2+8*q2Dpb*A3d1+8*q2Dp1b*A4d2+8*q2Dp1b*A2d1-8
     #*pbDp1b*A4d2-8*pbDp1b*A2d1)
c
      cf_b3_1_a(37)=t0
c
c --------cf_b3_1_38a
c
      t0 = CA/den0x1*(8*A6d2-8*A5d2)
c
      cf_b3_1_a(38)=t0
c
c --------cf_b3_1_39a
c
      t0 = CA/den0x1*(-8*mb*A6d2+4*mb*A3d2+4*mb*A2d2)
c
      cf_b3_1_a(39)=t0
c
c --------cf_b3_1_40a
c
      t0 = CA/den0x1*(-4*A5d2+4*A4d2-4*A3d1+4*A2d1)
c
      cf_b3_1_a(40)=t0
c
c --------cf_b3_1_41a
c
      t0 = 6*CA/den0x1*A7d2
c
      cf_b3_1_a(41)=t0
c
c --------cf_b3_1_43a
c
      t0 = CA/den0x1*(4*mb*A6d2-2*mb*A3d2-2*mb*A2d2)
c
      cf_b3_1_a(43)=t0
c
c --------cf_b3_1_45a
c
      t0 = CA/den0x1*(2*A5d2-2*A4d2+2*A3d1-2*A2d1)
c
      cf_b3_1_a(45)=t0
c
c --------cf_b3_1_46a
c
      t0 = 6*CA/den0x1*A7d2
c
      cf_b3_1_a(46)=t0
c
c --------cf_b3_1_47a
c
      t0 = CA/den0x1*(-4*mb*A6d2+2*mb*A3d2+2*mb*A2d2)
c
      cf_b3_1_a(47)=t0
c
c --------cf_b3_1_49a
c
      t0 = CA/den0x1*(-2*A5d2+2*A4d2-2*A3d1+2*A2d1)
c
      cf_b3_1_a(49)=t0
c
c --------cf_b3_1_51a
c
      t0 = CA/den0x1*(4*mb*A6d2+4*mb*A5d2-4*mb*A4d2-4*mb*A3d2+2*mb*A3d1-
     #2*mb*A2d1)
c
      cf_b3_1_a(51)=t0
c
c --------cf_b3_1_52a
c
      t0 = CA/den0x1*(-4*mb*A6d2+4*mb*A5d2-4*mb*A4d2+4*mb*A2d2+2*mb*A3d1
     #-2*mb*A2d1)
c
      cf_b3_1_a(52)=t0
c
c --------cf_b3_1_53a
c
      t0 = CA/den0x1*(-mb**3*A3d2+mb**3*A2d2-2*q2Dpb*mb*A6d2+2*q2Dpb*mb*
     #A3d2+2*q2Dp1b*mb*A6d2-2*q2Dp1b*mb*A2d2-2*pbDp1b*mb*A6d2+2*pbDp1b*m
     #b*A2d2)
c
      cf_b3_1_a(53)=t0
c
c --------cf_b3_1_54a
c
      t0 = CA/den0x1*(6*A7d2+mb**2*A5d2+mb**2*A4d2+mb**2*A3d1+mb**2*A2d1
     #-2*q2Dpb*A5d2-2*q2Dpb*A3d1-2*q2Dp1b*A4d2-2*q2Dp1b*A2d1+2*pbDp1b*A4
     #d2+2*pbDp1b*A2d1)
c
      cf_b3_1_a(54)=t0
c
c --------cf_b3_1_55a
c
      t0 = -2*CA/den0x1*A7d2
c
      cf_b3_1_a(55)=t0
c
c --------cf_b3_1_56a
c
      t0 = -2*CA/den0x1*A7d2
c
      cf_b3_1_a(56)=t0
c
c --------cf_b3_1_57a
c
      t0 = CA/den0x1*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_1_a(57)=t0
c
c --------cf_b3_1_58a
c
      t0 = CA/den0x1*(A5d2-A4d2+A3d1-A2d1)
c
      cf_b3_1_a(58)=t0
c
c --------cf_b3_1_59a
c
      t0 = -CA/den0x1*A7d2
c
      cf_b3_1_a(59)=t0
c
c --------cf_b3_1_60a
c
      t0 = -CA/den0x1*A7d2
c
      cf_b3_1_a(60)=t0
c
c
c
c --------------- integrals for B3_1c  ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q2(i)
         p2(i)=-pb(i)
         p3(i)=-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c
c --- D0 
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs12,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b3_1c_v(i)=0d0
         cf_b3_1c_a(i)=0d0
      enddo
c
c ---------------------- coefficients for B3_1c_v
c
c --------cf_b3_1c_1v
c
      t0 = CV/den0x1*(4*mb**3*A6d2+4*mb**3*A3d2+8*mb**3*A3d1-8*q2Dpb*mb*
     #A6d2-8*q2Dpb*mb*A3d1-8*q2Dp1b*mb*A3d2-8*q2Dp1b*mb*A3d1+8*pbDp1b*mb
     #*A3d2+8*pbDp1b*mb*A3d1)
c
      cf_b3_1c_v(1)=t0
c
c --------cf_b3_1c_3v
c
      t0 = CV/den0x1*(8*A7d2+4*mb**2*A5d2-4*mb**2*A4d2+8*mb**2*A2d2-8*q2
     #Dpb*A4d2-8*q2Dpb*A1d1-8*q2Dp1b*A5d2+8*pbDp1b*A5d2)
c
      cf_b3_1c_v(3)=t0
c
c --------cf_b3_1c_4v
c
      t0 = CV/den0x1*(8*A7d2+8*mb**2*A6d2+8*mb**2*A2d2+4*mb**2*A3d1+4*mb
     #**2*A2d1-16*q2Dpb*A4d2-8*q2Dpb*A2d1-8*q2Dpb*A1d1-16*q2Dp1b*A6d2-8*
     #q2Dp1b*A3d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_1c_v(4)=t0
c
c --------cf_b3_1c_5v
c
      t0 = CV/den0x1*(8*A7d2-8*mb**2*A6d2+8*mb**2*A2d2-4*mb**2*A3d1+4*mb
     #**2*A2d1+16*q2Dpb*A6d2-16*q2Dpb*A4d2+8*q2Dpb*A3d1-8*q2Dpb*A2d1-8*q
     #2Dpb*A1d1)
c
      cf_b3_1c_v(5)=t0
c
c --------cf_b3_1c_9v
c
      t0 = CV/den0x1*(-8*mb**2*A6d2-8*mb**2*A2d2-4*mb**2*A3d1-4*mb**2*A2
     #d1+16*q2Dpb*A4d2+8*q2Dpb*A2d1+8*q2Dpb*A1d1+16*q2Dp1b*A6d2+8*q2Dp1b
     #*A3d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1c_v(9)=t0
c
c --------cf_b3_1c_10v
c
      t0 = CV/den0x1*(8*mb**2*A6d2-8*mb**2*A2d2+4*mb**2*A3d1-4*mb**2*A2d
     #1-16*q2Dpb*A6d2+16*q2Dpb*A4d2-8*q2Dpb*A3d1+8*q2Dpb*A2d1+8*q2Dpb*A1
     #d1)
c
      cf_b3_1c_v(10)=t0
c
c --------cf_b3_1c_11v
c
      t0 = CV/den0x1*(-12*A7d2-4*mb**2*A6d2-4*mb**2*A2d2-2*mb**2*A3d1-2*
     #mb**2*A2d1+8*q2Dpb*A4d2+4*q2Dpb*A2d1+4*q2Dpb*A1d1+8*q2Dp1b*A6d2+4*
     #q2Dp1b*A3d1-8*pbDp1b*A6d2-4*pbDp1b*A3d1)
c
      cf_b3_1c_v(11)=t0
c
c --------cf_b3_1c_12v
c
      t0 = CV/den0x1*(-12*A7d2+4*mb**2*A6d2-4*mb**2*A2d2+2*mb**2*A3d1-2*
     #mb**2*A2d1-8*q2Dpb*A6d2+8*q2Dpb*A4d2-4*q2Dpb*A3d1+4*q2Dpb*A2d1+4*q
     #2Dpb*A1d1)
c
      cf_b3_1c_v(12)=t0
c
c --------cf_b3_1c_13v
c
      t0 = CV/den0x1*(4*mb*A6d2-4*mb*A3d2)
c
      cf_b3_1c_v(13)=t0
c
c --------cf_b3_1c_14v
c
      t0 = CV/den0x1*(8*mb*A6d2-8*mb*A2d2)
c
      cf_b3_1c_v(14)=t0
c
c --------cf_b3_1c_15v
c
      t0 = CV/den0x1*(-8*mb*A6d2+8*mb*A2d2)
c
      cf_b3_1c_v(15)=t0
c
c --------cf_b3_1c_23v
c
      t0 = CV/den0x1*(16*A6d2-16*A4d2+16*A3d1-16*A2d1)
c
      cf_b3_1c_v(23)=t0
c
c --------cf_b3_1c_30v
c
      t0 = CV/den0x1*(-16*A6d2+16*A4d2-16*A3d1+16*A2d1)
c
      cf_b3_1c_v(30)=t0
c
c --------cf_b3_1c_32v
c
      t0 = CV/den0x1*(-8*mb*A6d2+8*mb*A5d2-8*mb*A4d2+8*mb*A2d2+4*mb*A3d1
     #-4*mb*A2d1)
c
      cf_b3_1c_v(32)=t0
c
c --------cf_b3_1c_33v
c
      t0 = CV/den0x1*(8*mb*A6d2+8*mb*A5d2-8*mb*A4d2-8*mb*A3d2+4*mb*A3d1-
     #4*mb*A2d1)
c
      cf_b3_1c_v(33)=t0
c
c --------cf_b3_1c_34v
c
      t0 = CV/den0x1*(8*mb*A6d2-8*mb*A5d2+8*mb*A4d2-8*mb*A2d2-4*mb*A3d1+
     #4*mb*A2d1)
c
      cf_b3_1c_v(34)=t0
c
c --------cf_b3_1c_35v
c
      t0 = CV/den0x1*(-8*mb*A6d2-8*mb*A5d2+8*mb*A4d2+8*mb*A3d2-4*mb*A3d1
     #+4*mb*A2d1)
c
      cf_b3_1c_v(35)=t0
c
c --------cf_b3_1c_36v
c
      t0 = CV/den0x1*(-4*mb**3*A3d2+4*mb**3*A2d2+8*q2Dpb*mb*A6d2-8*q2Dpb
     #*mb*A2d2-8*q2Dp1b*mb*A6d2+8*q2Dp1b*mb*A3d2+8*pbDp1b*mb*A6d2-8*pbDp
     #1b*mb*A3d2)
c
      cf_b3_1c_v(36)=t0
c
c --------cf_b3_1c_37v
c
      t0 = CV/den0x1*(-24*A7d2-4*mb**2*A5d2-4*mb**2*A4d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+8*q2Dpb*A4d2+8*q2Dpb*A2d1+8*q2Dp1b*A5d2+8*q2Dp1b*A3d1-8
     #*pbDp1b*A5d2-8*pbDp1b*A3d1)
c
      cf_b3_1c_v(37)=t0
c
c --------cf_b3_1c_38v
c
      t0 = CV/den0x1*(8*A6d2-4*A5d2-4*A4d2+4*A3d1-4*A2d1)
c
      cf_b3_1c_v(38)=t0
c
c --------cf_b3_1c_39v
c
      t0 = CV/den0x1*(-8*mb*A6d2+4*mb*A3d2+4*mb*A2d2)
c
      cf_b3_1c_v(39)=t0
c
c --------cf_b3_1c_40v
c
      t0 = CV/den0x1*(4*A5d2-4*A4d2+4*A3d1-4*A2d1)
c
      cf_b3_1c_v(40)=t0
c
c --------cf_b3_1c_41v
c
      t0 = 6*CV/den0x1*A7d2
c
      cf_b3_1c_v(41)=t0
c
c --------cf_b3_1c_43v
c
      t0 = CV/den0x1*(4*mb*A6d2-2*mb*A3d2-2*mb*A2d2)
c
      cf_b3_1c_v(43)=t0
c
c --------cf_b3_1c_45v
c
      t0 = CV/den0x1*(-2*A5d2+2*A4d2-2*A3d1+2*A2d1)
c
      cf_b3_1c_v(45)=t0
c
c --------cf_b3_1c_46v
c
      t0 = 6*CV/den0x1*A7d2
c
      cf_b3_1c_v(46)=t0
c
c --------cf_b3_1c_47v
c
      t0 = CV/den0x1*(-4*mb*A6d2+2*mb*A3d2+2*mb*A2d2)
c
      cf_b3_1c_v(47)=t0
c
c --------cf_b3_1c_49v
c
      t0 = CV/den0x1*(2*A5d2-2*A4d2+2*A3d1-2*A2d1)
c
      cf_b3_1c_v(49)=t0
c
c --------cf_b3_1c_51v
c
      t0 = CV/den0x1*(4*mb*A6d2-4*mb*A5d2+4*mb*A4d2-4*mb*A2d2-2*mb*A3d1+
     #2*mb*A2d1)
c
      cf_b3_1c_v(51)=t0
c
c --------cf_b3_1c_52v
c
      t0 = CV/den0x1*(-4*mb*A6d2-4*mb*A5d2+4*mb*A4d2+4*mb*A3d2-2*mb*A3d1
     #+2*mb*A2d1)
c
      cf_b3_1c_v(52)=t0
c
c --------cf_b3_1c_53v
c
      t0 = CV/den0x1*(mb**3*A3d2-mb**3*A2d2-2*q2Dpb*mb*A6d2+2*q2Dpb*mb*A
     #2d2+2*q2Dp1b*mb*A6d2-2*q2Dp1b*mb*A3d2-2*pbDp1b*mb*A6d2+2*pbDp1b*mb
     #*A3d2)
c
      cf_b3_1c_v(53)=t0
c
c --------cf_b3_1c_54v
c
      t0 = CV/den0x1*(6*A7d2+mb**2*A5d2+mb**2*A4d2+mb**2*A3d1+mb**2*A2d1
     #-2*q2Dpb*A4d2-2*q2Dpb*A2d1-2*q2Dp1b*A5d2-2*q2Dp1b*A3d1+2*pbDp1b*A5
     #d2+2*pbDp1b*A3d1)
c
      cf_b3_1c_v(54)=t0
c
c --------cf_b3_1c_55v
c
      t0 = -2*CV/den0x1*A7d2
c
      cf_b3_1c_v(55)=t0
c
c --------cf_b3_1c_56v
c
      t0 = -2*CV/den0x1*A7d2
c
      cf_b3_1c_v(56)=t0
c
c --------cf_b3_1c_57v
c
      t0 = CV/den0x1*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_1c_v(57)=t0
c
c --------cf_b3_1c_58v
c
      t0 = CV/den0x1*(-A5d2+A4d2-A3d1+A2d1)
c
      cf_b3_1c_v(58)=t0
c
c --------cf_b3_1c_59v
c
      t0 = -CV/den0x1*A7d2
c
      cf_b3_1c_v(59)=t0
c
c --------cf_b3_1c_60v
c
      t0 = -CV/den0x1*A7d2
c
      cf_b3_1c_v(60)=t0
c
c
c ---------------------- coefficients for b3_1c_1_a
c
c --------cf_b3_1c_1a
c
      t0 = CA/den0x1*(4*mb**3*A6d2+4*mb**3*A3d2+8*mb**3*A3d1-8*q2Dpb*mb*
     #A6d2-8*q2Dpb*mb*A3d1-8*q2Dp1b*mb*A3d2-8*q2Dp1b*mb*A3d1+8*pbDp1b*mb
     #*A3d2+8*pbDp1b*mb*A3d1)
c
      cf_b3_1c_a(1)=t0
c
c --------cf_b3_1c_3a
c
      t0 = CA/den0x1*(8*A7d2+4*mb**2*A5d2-4*mb**2*A4d2+8*mb**2*A2d2-8*q2
     #Dpb*A4d2-8*q2Dpb*A1d1-8*q2Dp1b*A5d2+8*pbDp1b*A5d2)
c
      cf_b3_1c_a(3)=t0
c
c --------cf_b3_1c_4a
c
      t0 = CA/den0x1*(8*A7d2+8*mb**2*A6d2+8*mb**2*A2d2+4*mb**2*A3d1+4*mb
     #**2*A2d1-16*q2Dpb*A4d2-8*q2Dpb*A2d1-8*q2Dpb*A1d1-16*q2Dp1b*A6d2-8*
     #q2Dp1b*A3d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_1c_a(4)=t0
c
c --------cf_b3_1c_5a
c
      t0 = CA/den0x1*(8*A7d2-8*mb**2*A6d2+8*mb**2*A2d2-4*mb**2*A3d1+4*mb
     #**2*A2d1+16*q2Dpb*A6d2-16*q2Dpb*A4d2+8*q2Dpb*A3d1-8*q2Dpb*A2d1-8*q
     #2Dpb*A1d1)
c
      cf_b3_1c_a(5)=t0
c
c --------cf_b3_1c_9a
c
      t0 = CA/den0x1*(-8*mb**2*A6d2-8*mb**2*A2d2-4*mb**2*A3d1-4*mb**2*A2
     #d1+16*q2Dpb*A4d2+8*q2Dpb*A2d1+8*q2Dpb*A1d1+16*q2Dp1b*A6d2+8*q2Dp1b
     #*A3d1-16*pbDp1b*A6d2-8*pbDp1b*A3d1)
c
      cf_b3_1c_a(9)=t0
c
c --------cf_b3_1c_10a
c
      t0 = CA/den0x1*(8*mb**2*A6d2-8*mb**2*A2d2+4*mb**2*A3d1-4*mb**2*A2d
     #1-16*q2Dpb*A6d2+16*q2Dpb*A4d2-8*q2Dpb*A3d1+8*q2Dpb*A2d1+8*q2Dpb*A1
     #d1)
c
      cf_b3_1c_a(10)=t0
c
c --------cf_b3_1c_11a
c
      t0 = CA/den0x1*(-12*A7d2-4*mb**2*A6d2-4*mb**2*A2d2-2*mb**2*A3d1-2*
     #mb**2*A2d1+8*q2Dpb*A4d2+4*q2Dpb*A2d1+4*q2Dpb*A1d1+8*q2Dp1b*A6d2+4*
     #q2Dp1b*A3d1-8*pbDp1b*A6d2-4*pbDp1b*A3d1)
c
      cf_b3_1c_a(11)=t0
c
c --------cf_b3_1c_12a
c
      t0 = CA/den0x1*(-12*A7d2+4*mb**2*A6d2-4*mb**2*A2d2+2*mb**2*A3d1-2*
     #mb**2*A2d1-8*q2Dpb*A6d2+8*q2Dpb*A4d2-4*q2Dpb*A3d1+4*q2Dpb*A2d1+4*q
     #2Dpb*A1d1)
c
      cf_b3_1c_a(12)=t0
c
c --------cf_b3_1c_13a
c
      t0 = CA/den0x1*(4*mb*A6d2-4*mb*A3d2)
c
      cf_b3_1c_a(13)=t0
c
c --------cf_b3_1c_14a
c
      t0 = CA/den0x1*(8*mb*A6d2-8*mb*A2d2)
c
      cf_b3_1c_a(14)=t0
c
c --------cf_b3_1c_15a
c
      t0 = CA/den0x1*(-8*mb*A6d2+8*mb*A2d2)
c
      cf_b3_1c_a(15)=t0
c
c --------cf_b3_1c_23a
c
      t0 = CA/den0x1*(16*A6d2-16*A4d2+16*A3d1-16*A2d1)
c
      cf_b3_1c_a(23)=t0
c
c --------cf_b3_1c_30a
c
      t0 = CA/den0x1*(-16*A6d2+16*A4d2-16*A3d1+16*A2d1)
c
      cf_b3_1c_a(30)=t0
c
c --------cf_b3_1c_32a
c
      t0 = CA/den0x1*(-8*mb*A6d2+8*mb*A5d2-8*mb*A4d2+8*mb*A2d2+4*mb*A3d1
     #-4*mb*A2d1)
c
      cf_b3_1c_a(32)=t0
c
c --------cf_b3_1c_33a
c
      t0 = CA/den0x1*(8*mb*A6d2+8*mb*A5d2-8*mb*A4d2-8*mb*A3d2+4*mb*A3d1-
     #4*mb*A2d1)
c
      cf_b3_1c_a(33)=t0
c
c --------cf_b3_1c_34a
c
      t0 = CA/den0x1*(8*mb*A6d2-8*mb*A5d2+8*mb*A4d2-8*mb*A2d2-4*mb*A3d1+
     #4*mb*A2d1)
c
      cf_b3_1c_a(34)=t0
c
c --------cf_b3_1c_35a
c
      t0 = CA/den0x1*(-8*mb*A6d2-8*mb*A5d2+8*mb*A4d2+8*mb*A3d2-4*mb*A3d1
     #+4*mb*A2d1)
c
      cf_b3_1c_a(35)=t0
c
c --------cf_b3_1c_36a
c
      t0 = CA/den0x1*(-4*mb**3*A3d2+4*mb**3*A2d2+8*q2Dpb*mb*A6d2-8*q2Dpb
     #*mb*A2d2-8*q2Dp1b*mb*A6d2+8*q2Dp1b*mb*A3d2+8*pbDp1b*mb*A6d2-8*pbDp
     #1b*mb*A3d2)
c
      cf_b3_1c_a(36)=t0
c
c --------cf_b3_1c_37a
c
      t0 = CA/den0x1*(-24*A7d2-4*mb**2*A5d2-4*mb**2*A4d2-4*mb**2*A3d1-4*
     #mb**2*A2d1+8*q2Dpb*A4d2+8*q2Dpb*A2d1+8*q2Dp1b*A5d2+8*q2Dp1b*A3d1-8
     #*pbDp1b*A5d2-8*pbDp1b*A3d1)
c
      cf_b3_1c_a(37)=t0
c
c --------cf_b3_1c_38a
c
      t0 = CA/den0x1*(8*A6d2-4*A5d2-4*A4d2+4*A3d1-4*A2d1)
c
      cf_b3_1c_a(38)=t0
c
c --------cf_b3_1c_39a
c
      t0 = CA/den0x1*(-8*mb*A6d2+4*mb*A3d2+4*mb*A2d2)
c
      cf_b3_1c_a(39)=t0
c
c --------cf_b3_1c_40a
c
      t0 = CA/den0x1*(4*A5d2-4*A4d2+4*A3d1-4*A2d1)
c
      cf_b3_1c_a(40)=t0
c
c --------cf_b3_1c_41a
c
      t0 = 6*CA/den0x1*A7d2
c
      cf_b3_1c_a(41)=t0
c
c --------cf_b3_1c_43a
c
      t0 = CA/den0x1*(4*mb*A6d2-2*mb*A3d2-2*mb*A2d2)
c
      cf_b3_1c_a(43)=t0
c
c --------cf_b3_1c_45a
c
      t0 = CA/den0x1*(-2*A5d2+2*A4d2-2*A3d1+2*A2d1)
c
      cf_b3_1c_a(45)=t0
c
c --------cf_b3_1c_46a
c
      t0 = 6*CA/den0x1*A7d2
c
      cf_b3_1c_a(46)=t0
c
c --------cf_b3_1c_47a
c
      t0 = CA/den0x1*(-4*mb*A6d2+2*mb*A3d2+2*mb*A2d2)
c
      cf_b3_1c_a(47)=t0
c
c --------cf_b3_1c_49a
c
      t0 = CA/den0x1*(2*A5d2-2*A4d2+2*A3d1-2*A2d1)
c
      cf_b3_1c_a(49)=t0
c
c --------cf_b3_1c_51a
c
      t0 = CA/den0x1*(4*mb*A6d2-4*mb*A5d2+4*mb*A4d2-4*mb*A2d2-2*mb*A3d1+
     #2*mb*A2d1)
c
      cf_b3_1c_a(51)=t0
c
c --------cf_b3_1c_52a
c
      t0 = CA/den0x1*(-4*mb*A6d2-4*mb*A5d2+4*mb*A4d2+4*mb*A3d2-2*mb*A3d1
     #+2*mb*A2d1)
c
      cf_b3_1c_a(52)=t0
c
c --------cf_b3_1c_53a
c
      t0 = CA/den0x1*(mb**3*A3d2-mb**3*A2d2-2*q2Dpb*mb*A6d2+2*q2Dpb*mb*A
     #2d2+2*q2Dp1b*mb*A6d2-2*q2Dp1b*mb*A3d2-2*pbDp1b*mb*A6d2+2*pbDp1b*mb
     #*A3d2)
c
      cf_b3_1c_a(53)=t0
c
c --------cf_b3_1c_54a
c
      t0 = CA/den0x1*(6*A7d2+mb**2*A5d2+mb**2*A4d2+mb**2*A3d1+mb**2*A2d1
     #-2*q2Dpb*A4d2-2*q2Dpb*A2d1-2*q2Dp1b*A5d2-2*q2Dp1b*A3d1+2*pbDp1b*A5
     #d2+2*pbDp1b*A3d1)
c
      cf_b3_1c_a(54)=t0
c
c --------cf_b3_1c_55a
c
      t0 = -2*CA/den0x1*A7d2
c
      cf_b3_1c_a(55)=t0
c
c --------cf_b3_1c_56a
c
      t0 = -2*CA/den0x1*A7d2
c
      cf_b3_1c_a(56)=t0
c
c --------cf_b3_1c_57a
c
      t0 = CA/den0x1*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_1c_a(57)=t0
c
c --------cf_b3_1c_58a
c
      t0 = CA/den0x1*(-A5d2+A4d2-A3d1+A2d1)
c
      cf_b3_1c_a(58)=t0
c
c --------cf_b3_1c_59a
c
      t0 = -CA/den0x1*A7d2
c
      cf_b3_1c_a(59)=t0
c
c --------cf_b3_1c_60a
c
      t0 = -CA/den0x1*A7d2
c
      cf_b3_1c_a(60)=t0
c

c
c
c --------------- integrals for B3_2 ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pb(i)
         p3(i)=-p1b(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
c --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c
c --- D0 
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs12,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b3_2_v(i)=0d0
         cf_b3_2_a(i)=0d0
      enddo
c
c ---------------------- coefficients for b3_2_v
c
c --------cf_b3_2_1v
c
      t0 = CV/den0x2*(-24*mb*A7d2+4*mb**3*A6d2-12*mb**3*A2d2+8*q1Dpb*mb*
     #A2d2+8*q1Dp1b*mb*A6d2+8*q1Dp1b*mb*A3d1-8*q1Dp1b*mb*A2d1-8*pbDp1b*m
     #b*A2d2)
c
      cf_b3_2_v(1)=t0
c
c --------cf_b3_2_2v
c
      t0 = CV/den0x2*(32*A7d2+8*mb**2*A4d2+8*mb**2*A3d2+4*mb**2*A3d1+4*m
     #b**2*A2d1-16*q1Dpb*A4d2-8*q1Dpb*A2d1-8*q1Dpb*A1d1-16*q1Dp1b*A5d2-8
     #*q1Dp1b*A3d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_2_v(2)=t0
c
c --------cf_b3_2_11v
c
      t0 = CV/den0x2*(32*A7d2-4*mb**2*A6d2+4*mb**2*A3d2+8*mb**2*A2d2+2*m
     #b**2*A3d1+2*mb**2*A2d1-8*q1Dpb*A4d2-4*q1Dpb*A2d1-4*q1Dpb*A1d1-8*q1
     #Dp1b*A6d2-8*q1Dp1b*A5d2+8*q1Dp1b*A4d2-12*q1Dp1b*A3d1+8*q1Dp1b*A2d1
     #+8*pbDp1b*A6d2+4*pbDp1b*A3d1)
c
      cf_b3_2_v(11)=t0
c
c --------cf_b3_2_12v
c
      t0 = CV/den0x2*(8*A7d2+4*mb**2*A6d2-4*mb**2*A3d2-2*mb**2*A3d1+2*mb
     #**2*A2d1+8*q1Dpb*A6d2-8*q1Dpb*A4d2+4*q1Dpb*A3d1-4*q1Dpb*A2d1-4*q1D
     #pb*A1d1)
c
      cf_b3_2_v(12)=t0
c
c --------cf_b3_2_13v
c
      t0 = CV/den0x2*(-12*mb*A6d2+8*mb*A3d2+4*mb*A2d2)
c
      cf_b3_2_v(13)=t0
c
c --------cf_b3_2_53v
c
      t0 = CV/den0x2*(-mb**3*A3d2+mb**3*A2d2+2*q1Dpb*mb*A6d2-2*q1Dpb*mb*
     #A2d2-2*q1Dp1b*mb*A6d2+2*q1Dp1b*mb*A3d2-2*pbDp1b*mb*A6d2+2*pbDp1b*m
     #b*A2d2)
c
      cf_b3_2_v(53)=t0
c
c --------cf_b3_2_57v
c
      t0 = CV/den0x2*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_2_v(57)=t0
c
c --------cf_b3_2_59v
c
      t0 = -CV/den0x2*A7d2
c
      cf_b3_2_v(59)=t0
c
c --------cf_b3_2_60v
c
      t0 = -CV/den0x2*A7d2
c
      cf_b3_2_v(60)=t0
c
c --------cf_b3_2_66v
c
      t0 = CV/den0x2*(-8*A6d2+8*A5d2)
c
      cf_b3_2_v(66)=t0
c
c --------cf_b3_2_72v
c
      t0 = CV/den0x2*(-4*mb*A6d2+4*mb*A2d2-2*mb*A3d1+2*mb*A2d1)
c
      cf_b3_2_v(72)=t0
c
c --------cf_b3_2_73v
c
      t0 = CV/den0x2*(4*mb*A6d2-4*mb*A3d2-2*mb*A3d1+2*mb*A2d1)
c
      cf_b3_2_v(73)=t0
c
c --------cf_b3_2_74v
c
      t0 = CV/den0x2*(-6*A7d2-mb**2*A5d2-mb**2*A4d2-mb**2*A3d1-mb**2*A2d
     #1+2*q1Dpb*A4d2+2*q1Dpb*A2d1+2*q1Dp1b*A5d2+2*q1Dp1b*A3d1-2*pbDp1b*A
     #4d2-2*pbDp1b*A2d1)
c
      cf_b3_2_v(74)=t0
c
c --------cf_b3_2_75v
c
      t0 = CV/den0x2*(-A5d2+A4d2-A3d1+A2d1)
c
      cf_b3_2_v(75)=t0
c
c
c ---------------------- coefficients for b3_2_1_a
c
c --------cf_b3_2_1a
c
      t0 = CA/den0x2*(-24*mb*A7d2+4*mb**3*A6d2-12*mb**3*A2d2+8*q1Dpb*mb*
     #A2d2+8*q1Dp1b*mb*A6d2+8*q1Dp1b*mb*A3d1-8*q1Dp1b*mb*A2d1-8*pbDp1b*m
     #b*A2d2)
c
      cf_b3_2_a(1)=t0
c
c --------cf_b3_2_2a
c
      t0 = CA/den0x2*(32*A7d2+8*mb**2*A4d2+8*mb**2*A3d2+4*mb**2*A3d1+4*m
     #b**2*A2d1-16*q1Dpb*A4d2-8*q1Dpb*A2d1-8*q1Dpb*A1d1-16*q1Dp1b*A5d2-8
     #*q1Dp1b*A3d1+16*pbDp1b*A6d2+8*pbDp1b*A3d1)
c
      cf_b3_2_a(2)=t0
c
c --------cf_b3_2_11a
c
      t0 = CA/den0x2*(32*A7d2-4*mb**2*A6d2+4*mb**2*A3d2+8*mb**2*A2d2+2*m
     #b**2*A3d1+2*mb**2*A2d1-8*q1Dpb*A4d2-4*q1Dpb*A2d1-4*q1Dpb*A1d1-8*q1
     #Dp1b*A6d2-8*q1Dp1b*A5d2+8*q1Dp1b*A4d2-12*q1Dp1b*A3d1+8*q1Dp1b*A2d1
     #+8*pbDp1b*A6d2+4*pbDp1b*A3d1)
c
      cf_b3_2_a(11)=t0
c
c --------cf_b3_2_12a
c
      t0 = CA/den0x2*(8*A7d2+4*mb**2*A6d2-4*mb**2*A3d2-2*mb**2*A3d1+2*mb
     #**2*A2d1+8*q1Dpb*A6d2-8*q1Dpb*A4d2+4*q1Dpb*A3d1-4*q1Dpb*A2d1-4*q1D
     #pb*A1d1)
c
      cf_b3_2_a(12)=t0
c
c --------cf_b3_2_13a
c
      t0 = CA/den0x2*(-12*mb*A6d2+8*mb*A3d2+4*mb*A2d2)
c
      cf_b3_2_a(13)=t0
c
c --------cf_b3_2_53a
c
      t0 = CA/den0x2*(-mb**3*A3d2+mb**3*A2d2+2*q1Dpb*mb*A6d2-2*q1Dpb*mb*
     #A2d2-2*q1Dp1b*mb*A6d2+2*q1Dp1b*mb*A3d2-2*pbDp1b*mb*A6d2+2*pbDp1b*m
     #b*A2d2)
c
      cf_b3_2_a(53)=t0
c
c --------cf_b3_2_57a
c
      t0 = CA/den0x2*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_2_a(57)=t0
c
c --------cf_b3_2_59a
c
      t0 = -CA/den0x2*A7d2
c
      cf_b3_2_a(59)=t0
c
c --------cf_b3_2_60a
c
      t0 = -CA/den0x2*A7d2
c
      cf_b3_2_a(60)=t0
c
c --------cf_b3_2_66a
c
      t0 = CA/den0x2*(-8*A6d2+8*A5d2)
c
      cf_b3_2_a(66)=t0
c
c --------cf_b3_2_72a
c
      t0 = CA/den0x2*(-4*mb*A6d2+4*mb*A2d2-2*mb*A3d1+2*mb*A2d1)
c
      cf_b3_2_a(72)=t0
c
c --------cf_b3_2_73a
c
      t0 = CA/den0x2*(4*mb*A6d2-4*mb*A3d2-2*mb*A3d1+2*mb*A2d1)
c
      cf_b3_2_a(73)=t0
c
c --------cf_b3_2_74a
c
      t0 = CA/den0x2*(-6*A7d2-mb**2*A5d2-mb**2*A4d2-mb**2*A3d1-mb**2*A2d
     #1+2*q1Dpb*A4d2+2*q1Dpb*A2d1+2*q1Dp1b*A5d2+2*q1Dp1b*A3d1-2*pbDp1b*A
     #4d2-2*pbDp1b*A2d1)
c
      cf_b3_2_a(74)=t0
c
c --------cf_b3_2_75a
c
      t0 = CA/den0x2*(-A5d2+A4d2-A3d1+A2d1)
c
      cf_b3_2_a(75)=t0
c
c
c
c --------------- integrals for B3_2c ---------------------------------
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-p1b(i)
         p3(i)=-pb(i)
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
         p123(i)=p1(i)+p2(i)+p3(i)
      enddo
      m1=0d0
      m2=0d0
      m3=mb
      m4=0d0
c --- C0123
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      call wbb_f3pf_2m_on(xs12,C0123)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,A1c1123,A2c1123)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0123,B012,B013,B023,B112,B113,B123,
     -            A4c2123,A1c2123,A2c2123,A3c2123)
c
c --- C0124
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f3pf_2m(xs2,xs12,C0124)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,A1c1124,A2c1124)
      call wbb_c2_new(p1,p23,m1,m2,m4,C0124,B012,B013,B023,B112,B113,B123,
     -            A4c2124,A1c2124,A2c2124,A3c2124)
c
c --- C0134
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p123,p123)
      C0134=wbb_c0_sd(xs1,xs2,xs12,m1,m3,m4)
      call wbb_bfunc(dabs(xs1),m1,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,A1c1134,A2c1134)
      call wbb_c2_new(p12,p3,m1,m3,m4,C0134,B012,B013,B023,B112,B113,B123,
     -            A4c2134,A1c2134,A2c2134,A3c2134)
c
c --- C0234
      xs1=wbb_dot_4(p2,p2)
      xs2=wbb_dot_4(p3,p3)
      xs12=wbb_dot_4(p23,p23)
      C0234=wbb_c0_sd(xs1,xs2,xs12,m2,m3,m4)
      call wbb_bfunc(dabs(xs1),m2,m3,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m2,m4,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m3,m4,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_c1_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,A1c1234,A2c1234)
      call wbb_c2_new(p2,p3,m2,m3,m4,C0234,B012,B013,B023,B112,B113,B123,
     -            A4c2234,A1c2234,A2c2234,A3c2234)
c
c --- D0 
      xs1=wbb_dot_4(p12,p12)
      xs2=wbb_dot_4(p23,p23)
      xs12=wbb_dot_4(p123,p123)
      call wbb_f4pf_b3(xs1,xs2,xs12,D0)
      call wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,C0234,C0134,C0124,C0123,
     -               A1d1,A2d1,A3d1)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,D0,A1d1,A2d1,A3d1,
     - C0234,A1c1234,A2c1234,C0134,A1c1134,A2c1134,C0124,
     - A1c1124,A2c1124,C0123,A1c1123,A2c1123,
     - A7d2,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2)
      if (dflag.eq.1) then
         return
      endif
      call wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1234,A2c1234,A1c2234,A2c2234,A3c2234,A4c2234,
     - C0134,A1c1134,A2c1134,A1c2134,A2c2134,A3c2134,A4c2134,
     - C0124,A1c1124,A2c1124,A1c2124,A2c2124,A3c2124,A4c2124,
     - C0123,A1c1123,A2c1123,A1c2123,A2c2123,A3c2123,A4c2123,
     - A1d3,A2d3,A3d3,A4d3,A5d3,A6d3,A7d3,A8d3,A9d3,A10d3,A11d3,
     - A12d3,A13d3)
      if (dflag.eq.1) then
         return
      endif
c      
      do i=1,75
         cf_b3_2c_v(i)=0d0
         cf_b3_2c_a(i)=0d0
      enddo
c
c ---------------------- coefficients for b3_2c_v
c
c --------cf_b3_2c_1v
c
      t0 = CV/den0x2*(-24*mb*A7d2+4*mb**3*A6d2-8*mb**3*A3d2-4*mb**3*A2d2
     #+8*q1Dpb*mb*A6d2+8*q1Dp1b*mb*A2d2-8*q1Dp1b*mb*A3d1+8*q1Dp1b*mb*A2d
     #1-8*pbDp1b*mb*A6d2)
c
      cf_b3_2c_v(1)=t0
c
c --------cf_b3_2c_2v
c
      t0 = CV/den0x2*(-8*A7d2+4*mb**2*A5d2-4*mb**2*A4d2-8*mb**2*A3d2+8*q
     #1Dpb*A5d2+8*q1Dp1b*A4d2+8*q1Dp1b*A1d1-8*pbDp1b*A5d2)
c
      cf_b3_2c_v(2)=t0
c
c --------cf_b3_2c_11v
c
      t0 = CV/den0x2*(12*A7d2-4*mb**2*A6d2+4*mb**2*A3d2+2*mb**2*A3d1-2*m
     #b**2*A2d1-8*q1Dp1b*A6d2+8*q1Dp1b*A5d2+4*q1Dp1b*A3d1-4*q1Dp1b*A2d1+
     #4*q1Dp1b*A1d1)
c
      cf_b3_2c_v(11)=t0
c
c --------cf_b3_2c_12v
c
      t0 = CV/den0x2*(-12*A7d2+4*mb**2*A6d2-4*mb**2*A3d2-8*mb**2*A2d2-2*
     #mb**2*A3d1-2*mb**2*A2d1+8*q1Dpb*A6d2+4*q1Dpb*A3d1+8*q1Dp1b*A4d2+4*
     #q1Dp1b*A2d1+4*q1Dp1b*A1d1-8*pbDp1b*A6d2-4*pbDp1b*A3d1)
c
      cf_b3_2c_v(12)=t0
c
c --------cf_b3_2c_13v
c
      t0 = CV/den0x2*(-4*mb*A6d2+4*mb*A2d2)
c
      cf_b3_2c_v(13)=t0
c
c --------cf_b3_2c_53v
c
      t0 = CV/den0x2*(mb**3*A3d2-mb**3*A2d2+2*q1Dpb*mb*A6d2-2*q1Dpb*mb*A
     #3d2-2*q1Dp1b*mb*A6d2+2*q1Dp1b*mb*A2d2-2*pbDp1b*mb*A6d2+2*pbDp1b*mb
     #*A3d2)
c
      cf_b3_2c_v(53)=t0
c
c --------cf_b3_2c_57v
c
      t0 = CV/den0x2*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_2c_v(57)=t0
c
c --------cf_b3_2c_59v
c
      t0 = -CV/den0x2*A7d2
c
      cf_b3_2c_v(59)=t0
c
c --------cf_b3_2c_60v
c
      t0 = -CV/den0x2*A7d2
c
      cf_b3_2c_v(60)=t0
c
c --------cf_b3_2c_66v
c
      t0 = CV/den0x2*(-8*A6d2+4*A5d2+4*A4d2-4*A3d1+4*A2d1)
c
      cf_b3_2c_v(66)=t0
c
c --------cf_b3_2c_72v
c
      t0 = CV/den0x2*(-4*mb*A6d2+4*mb*A3d2+2*mb*A3d1-2*mb*A2d1)
c
      cf_b3_2c_v(72)=t0
c
c --------cf_b3_2c_73v
c
      t0 = CV/den0x2*(4*mb*A6d2-4*mb*A2d2+2*mb*A3d1-2*mb*A2d1)
c
      cf_b3_2c_v(73)=t0
c
c --------cf_b3_2c_74v
c
      t0 = CV/den0x2*(-6*A7d2-mb**2*A5d2-mb**2*A4d2-mb**2*A3d1-mb**2*A2d
     #1+2*q1Dpb*A5d2+2*q1Dpb*A3d1+2*q1Dp1b*A4d2+2*q1Dp1b*A2d1-2*pbDp1b*A
     #5d2-2*pbDp1b*A3d1)
c
      cf_b3_2c_v(74)=t0
c
c --------cf_b3_2c_75v
c
      t0 = CV/den0x2*(A5d2-A4d2+A3d1-A2d1)
c
      cf_b3_2c_v(75)=t0
c
c
c ---------------------- coefficients for b3_2c_1_a
c
c --------cf_b3_2c_1a
c
      t0 = CA/den0x2*(-24*mb*A7d2+4*mb**3*A6d2-8*mb**3*A3d2-4*mb**3*A2d2
     #+8*q1Dpb*mb*A6d2+8*q1Dp1b*mb*A2d2-8*q1Dp1b*mb*A3d1+8*q1Dp1b*mb*A2d
     #1-8*pbDp1b*mb*A6d2)
c
      cf_b3_2c_a(1)=t0
c
c --------cf_b3_2c_2a
c
      t0 = CA/den0x2*(-8*A7d2+4*mb**2*A5d2-4*mb**2*A4d2-8*mb**2*A3d2+8*q
     #1Dpb*A5d2+8*q1Dp1b*A4d2+8*q1Dp1b*A1d1-8*pbDp1b*A5d2)
c
      cf_b3_2c_a(2)=t0
c
c --------cf_b3_2c_11a
c
      t0 = CA/den0x2*(12*A7d2-4*mb**2*A6d2+4*mb**2*A3d2+2*mb**2*A3d1-2*m
     #b**2*A2d1-8*q1Dp1b*A6d2+8*q1Dp1b*A5d2+4*q1Dp1b*A3d1-4*q1Dp1b*A2d1+
     #4*q1Dp1b*A1d1)
c
      cf_b3_2c_a(11)=t0
c
c --------cf_b3_2c_12a
c
      t0 = CA/den0x2*(-12*A7d2+4*mb**2*A6d2-4*mb**2*A3d2-8*mb**2*A2d2-2*
     #mb**2*A3d1-2*mb**2*A2d1+8*q1Dpb*A6d2+4*q1Dpb*A3d1+8*q1Dp1b*A4d2+4*
     #q1Dp1b*A2d1+4*q1Dp1b*A1d1-8*pbDp1b*A6d2-4*pbDp1b*A3d1)
c
      cf_b3_2c_a(12)=t0
c
c --------cf_b3_2c_13a
c
      t0 = CA/den0x2*(-4*mb*A6d2+4*mb*A2d2)
c
      cf_b3_2c_a(13)=t0
c
c --------cf_b3_2c_53a
c
      t0 = CA/den0x2*(mb**3*A3d2-mb**3*A2d2+2*q1Dpb*mb*A6d2-2*q1Dpb*mb*A
     #3d2-2*q1Dp1b*mb*A6d2+2*q1Dp1b*mb*A2d2-2*pbDp1b*mb*A6d2+2*pbDp1b*mb
     #*A3d2)
c
      cf_b3_2c_a(53)=t0
c
c --------cf_b3_2c_57a
c
      t0 = CA/den0x2*(2*mb*A6d2-mb*A3d2-mb*A2d2)
c
      cf_b3_2c_a(57)=t0
c
c --------cf_b3_2c_59a
c
      t0 = -CA/den0x2*A7d2
c
      cf_b3_2c_a(59)=t0
c
c --------cf_b3_2c_60a
c
      t0 = -CA/den0x2*A7d2
c
      cf_b3_2c_a(60)=t0
c
c --------cf_b3_2c_66a
c
      t0 = CA/den0x2*(-8*A6d2+4*A5d2+4*A4d2-4*A3d1+4*A2d1)
c
      cf_b3_2c_a(66)=t0
c
c --------cf_b3_2c_72a
c
      t0 = CA/den0x2*(-4*mb*A6d2+4*mb*A3d2+2*mb*A3d1-2*mb*A2d1)
c
      cf_b3_2c_a(72)=t0
c
c --------cf_b3_2c_73a
c
      t0 = CA/den0x2*(4*mb*A6d2-4*mb*A2d2+2*mb*A3d1-2*mb*A2d1)
c
      cf_b3_2c_a(73)=t0
c
c --------cf_b3_2c_74a
c
      t0 = CA/den0x2*(-6*A7d2-mb**2*A5d2-mb**2*A4d2-mb**2*A3d1-mb**2*A2d
     #1+2*q1Dpb*A5d2+2*q1Dpb*A3d1+2*q1Dp1b*A4d2+2*q1Dp1b*A2d1-2*pbDp1b*A
     #5d2-2*pbDp1b*A3d1)
c
      cf_b3_2c_a(74)=t0
c
c --------cf_b3_2c_75a
c
      t0 = CA/den0x2*(A5d2-A4d2+A3d1-A2d1)
c
      cf_b3_2c_a(75)=t0
c
c
      return
      end

      subroutine wbb_coeff_v1(q1,q2,pb,p1b,pw,mb,cf_v1_1_ab_v,cf_v1_1_ab_a,
     -           cf_v1_1_nab_v,cf_v1_1_nab_a,cf_v1_2_ab_v,cf_v1_2_ab_a,
     -           cf_v1_2_nab_v,cf_v1_2_nab_a)
c
      implicit none
c
      external wbb_dot_4
      real * 8 wbb_dot_4
c
c external wbb_masses and momenta
c
      integer i
      real * 8 mb,m1,m2,m3,q1(4),q2(4),pb(4),p1b(4),pw(4),
     - p1(4),p2(4),p12(4),xs1,xs2,xs12
      real * 8 CV,CA,den0x1,den0x2,den0x3,q1Dq2,q1Dpb,q1Dp1b,
     - q2Dpb,q2Dp1b,pbDp1b,t0
      real * 8 AAi,B0v1,B1v1,C0v1,A1c1v1,A2c1v1,A1c2v1,A2c2v1,
     - A3c2v1,A4c2v1,B012,B112,B013,B113,B023,B123
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real * 8 cf_v1_1_ab_v(75),cf_v1_1_ab_a(75),
     -  cf_v1_1_nab_v(75),cf_v1_1_nab_a(75),cf_v1_2_ab_v(75),
     -  cf_v1_2_ab_a(75),cf_v1_2_nab_v(75),cf_v1_2_nab_a(75)
c tests
c      real * 8 B012_ff,B112_ff,B013_ff,B113_ff,B023_ff,B123_ff,
c     - B112l,B2012l,B2112l,B113l,B2013l,B2113l,B123l,B2023l,B2123l,
c     - A01,wbb_a02,A03
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
c
c debug parameter
c
      logical ldebugv1
      parameter(ldebugv1=.false.)
c      parameter(ldebugv1=.true.)
c
      if(ldebugv1) then
      print*,' ------  Debugging coeff_v1_1, coeff_v1_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for V1_1 (both "ab" and "nab") 
c in the notation used for V_ab and V_nab
c         p1(i)=q1(i)-pw(i)
c         p2(i)=-q2(i)
c         q(i)=p2(i)-p1(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)-pw(i)
         p2(i)=-pb(i)-p1b(i)
         p12(i)=p1(i)+p2(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
c
      call wbb_f3pf_2m(xs1,xs2,C0v1)
      call wbb_bfunc(dabs(xs1),m1,m2,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v1=B023
      B1v1=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v1,B012,B013,B023,A1c1v1,A2c1v1)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v1,B012,B013,B023,B112,B113,B123,
     -            A4c2v1,A1c2v1,A2c2v1,A3c2v1)
c
      do i=1,75
         cf_v1_1_ab_v(i)=0d0
         cf_v1_1_nab_v(i)=0d0
         cf_v1_1_ab_a(i)=0d0
         cf_v1_1_nab_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v1_1_ab_v
c
c --------cf_v1_1_ab_1v
c
      cf_v1_1_ab_v(1)=0d0
c
c --------cf_v1_1_ab_2v
c
      cf_v1_1_ab_v(2)=0d0
c
c --------cf_v1_1_ab_3v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*mb**2+8.
     #D0*C0v1*pbDp1b+8.D0*A1c1v1*mb**2+8.D0*A1c1v1*pbDp1b+8.D0*A1c2v1*mb
     #**2-8.D0*A1c2v1*q2Dpb-8.D0*A1c2v1*q2Dp1b+8.D0*A1c2v1*pbDp1b-8.D0*A
     #3c2v1*mb**2+8.D0*A3c2v1*q2Dpb+8.D0*A3c2v1*q2Dp1b-8.D0*A3c2v1*pbDp1
     #b+8.D0*A4c2v1)
c
      cf_v1_1_ab_v(3)=t0
c
c --------cf_v1_1_ab_4v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*q2Dpb+8.
     #D0*C0v1*q2Dp1b-8.D0*A1c1v1*mb**2+16.D0*A1c1v1*q2Dpb+16.D0*A1c1v1*q
     #2Dp1b-8.D0*A1c1v1*pbDp1b+8.D0*A2c1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*
     #A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b+8.D0*A4c2v1)
c
      cf_v1_1_ab_v(4)=t0
c
c --------cf_v1_1_ab_5v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*q2Dpb+8.
     #D0*C0v1*q2Dp1b-8.D0*A1c1v1*mb**2+16.D0*A1c1v1*q2Dpb+16.D0*A1c1v1*q
     #2Dp1b-8.D0*A1c1v1*pbDp1b+8.D0*A2c1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*
     #A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b+8.D0*A4c2v1)
c
      cf_v1_1_ab_v(5)=t0
c
c --------cf_v1_1_ab_6v
c
      cf_v1_1_ab_v(6)=0d0
c
c --------cf_v1_1_ab_7v
c
      cf_v1_1_ab_v(7)=0d0
c
c --------cf_v1_1_ab_8v
c
      cf_v1_1_ab_v(8)=0d0
c
c --------cf_v1_1_ab_9v
c
      t0 = CV/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*q2Dpb-8
     #.D0*C0v1*q2Dp1b+8.D0*A1c1v1*mb**2-16.D0*A1c1v1*q2Dpb-16.D0*A1c1v1*
     #q2Dp1b+8.D0*A1c1v1*pbDp1b-8.D0*A2c1v1*mb**2+8.D0*A2c1v1*q2Dpb+8.D0
     #*A2c1v1*q2Dp1b-8.D0*A2c1v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_ab_v(9)=t0
c
c --------cf_v1_1_ab_10v
c
      t0 = CV/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*q2Dpb-
     #8.D0*C0v1*q2Dp1b+8.D0*A1c1v1*mb**2-16.D0*A1c1v1*q2Dpb-16.D0*A1c1v1
     #*q2Dp1b+8.D0*A1c1v1*pbDp1b-8.D0*A2c1v1*mb**2+8.D0*A2c1v1*q2Dpb+8.D
     #0*A2c1v1*q2Dp1b-8.D0*A2c1v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_ab_v(10)=t0
c
c --------cf_v1_1_ab_11v
c
      t0 = CV/den0x1/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q2Dpb-4.D0*
     #C0v1*q2Dp1b+4.D0*A1c1v1*mb**2-8.D0*A1c1v1*q2Dpb-8.D0*A1c1v1*q2Dp1b
     #+4.D0*A1c1v1*pbDp1b-4.D0*A2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v
     #1*q2Dp1b-4.D0*A2c1v1*pbDp1b-4.D0*A4c2v1)
c
      cf_v1_1_ab_v(11)=t0
c
c --------cf_v1_1_ab_12v
c
      t0 = CV/den0x1/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q2Dpb-4.D0*
     #C0v1*q2Dp1b+4.D0*A1c1v1*mb**2-8.D0*A1c1v1*q2Dpb-8.D0*A1c1v1*q2Dp1b
     #+4.D0*A1c1v1*pbDp1b-4.D0*A2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v
     #1*q2Dp1b-4.D0*A2c1v1*pbDp1b-4.D0*A4c2v1)
c
      cf_v1_1_ab_v(12)=t0
c
c --------cf_v1_1_ab_13v
c
      cf_v1_1_ab_v(13)=0d0
c
c --------cf_v1_1_ab_14v
c
      cf_v1_1_ab_v(14)=0d0
c
c --------cf_v1_1_ab_15v
c
      cf_v1_1_ab_v(15)=0d0
c
c --------cf_v1_1_ab_16v
c
      cf_v1_1_ab_v(16)=0d0
c
c --------cf_v1_1_ab_17v
c
      cf_v1_1_ab_v(17)=0d0
c
c --------cf_v1_1_ab_18v
c
      cf_v1_1_ab_v(18)=0d0
c
c --------cf_v1_1_ab_19v
c
      cf_v1_1_ab_v(19)=0d0
c
c --------cf_v1_1_ab_20v
c
      cf_v1_1_ab_v(20)=0d0
c
c --------cf_v1_1_ab_21v
c
      cf_v1_1_ab_v(21)=0d0
c
c --------cf_v1_1_ab_22v
c
      cf_v1_1_ab_v(22)=0d0
c
c --------cf_v1_1_ab_23v
c
      cf_v1_1_ab_v(23)=0d0
c
c --------cf_v1_1_ab_24v
c
      cf_v1_1_ab_v(24)=0d0
c
c --------cf_v1_1_ab_25v
c
      cf_v1_1_ab_v(25)=0d0
c
c --------cf_v1_1_ab_26v
c
      cf_v1_1_ab_v(26)=0d0
c
c --------cf_v1_1_ab_27v
c
      cf_v1_1_ab_v(27)=0d0
c
c --------cf_v1_1_ab_28v
c
      cf_v1_1_ab_v(28)=0d0
c
c --------cf_v1_1_ab_29v
c
      cf_v1_1_ab_v(29)=0d0
c
c --------cf_v1_1_ab_30v
c
      cf_v1_1_ab_v(30)=0d0
c
c --------cf_v1_1_ab_31v
c
      cf_v1_1_ab_v(31)=0d0

c
c
c ---------------------- coefficients for v1_1_ab_a
c
c --------cf_v1_1_ab_1a
c
      cf_v1_1_ab_a(1)=0d0
c
c --------cf_v1_1_ab_2a
c
      cf_v1_1_ab_a(2)=0d0
c
c --------cf_v1_1_ab_3a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*mb**2+8.
     #D0*C0v1*pbDp1b+8.D0*A1c1v1*mb**2+8.D0*A1c1v1*pbDp1b+8.D0*A1c2v1*mb
     #**2-8.D0*A1c2v1*q2Dpb-8.D0*A1c2v1*q2Dp1b+8.D0*A1c2v1*pbDp1b-8.D0*A
     #3c2v1*mb**2+8.D0*A3c2v1*q2Dpb+8.D0*A3c2v1*q2Dp1b-8.D0*A3c2v1*pbDp1
     #b+8.D0*A4c2v1)
c
      cf_v1_1_ab_a(3)=t0
c
c --------cf_v1_1_ab_4a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*q2Dpb+8.
     #D0*C0v1*q2Dp1b-8.D0*A1c1v1*mb**2+16.D0*A1c1v1*q2Dpb+16.D0*A1c1v1*q
     #2Dp1b-8.D0*A1c1v1*pbDp1b+8.D0*A2c1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*
     #A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b+8.D0*A4c2v1)
c
      cf_v1_1_ab_a(4)=t0
c
c --------cf_v1_1_ab_5a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1+8.D0*C0v1*q2Dpb+8.
     #D0*C0v1*q2Dp1b-8.D0*A1c1v1*mb**2+16.D0*A1c1v1*q2Dpb+16.D0*A1c1v1*q
     #2Dp1b-8.D0*A1c1v1*pbDp1b+8.D0*A2c1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*
     #A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b+8.D0*A4c2v1)
c
      cf_v1_1_ab_a(5)=t0
c
c --------cf_v1_1_ab_6a
c
      cf_v1_1_ab_a(6)=0d0
c
c --------cf_v1_1_ab_7a
c
      cf_v1_1_ab_a(7)=0d0
c
c --------cf_v1_1_ab_8a
c
      cf_v1_1_ab_a(8)=0d0
c
c --------cf_v1_1_ab_9a
c
      t0 = CA/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*q2Dpb-8
     #.D0*C0v1*q2Dp1b+8.D0*A1c1v1*mb**2-16.D0*A1c1v1*q2Dpb-16.D0*A1c1v1*
     #q2Dp1b+8.D0*A1c1v1*pbDp1b-8.D0*A2c1v1*mb**2+8.D0*A2c1v1*q2Dpb+8.D0
     #*A2c1v1*q2Dp1b-8.D0*A2c1v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_ab_a(9)=t0
c
c --------cf_v1_1_ab_10a
c
      t0 = CA/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*q2Dpb-
     #8.D0*C0v1*q2Dp1b+8.D0*A1c1v1*mb**2-16.D0*A1c1v1*q2Dpb-16.D0*A1c1v1
     #*q2Dp1b+8.D0*A1c1v1*pbDp1b-8.D0*A2c1v1*mb**2+8.D0*A2c1v1*q2Dpb+8.D
     #0*A2c1v1*q2Dp1b-8.D0*A2c1v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_ab_a(10)=t0
c
c --------cf_v1_1_ab_11a
c
      t0 = CA/den0x1/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q2Dpb-4.D0*
     #C0v1*q2Dp1b+4.D0*A1c1v1*mb**2-8.D0*A1c1v1*q2Dpb-8.D0*A1c1v1*q2Dp1b
     #+4.D0*A1c1v1*pbDp1b-4.D0*A2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v
     #1*q2Dp1b-4.D0*A2c1v1*pbDp1b-4.D0*A4c2v1)
c
      cf_v1_1_ab_a(11)=t0
c
c --------cf_v1_1_ab_12a
c
      t0 = CA/den0x1/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q2Dpb-4.D0*
     #C0v1*q2Dp1b+4.D0*A1c1v1*mb**2-8.D0*A1c1v1*q2Dpb-8.D0*A1c1v1*q2Dp1b
     #+4.D0*A1c1v1*pbDp1b-4.D0*A2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v
     #1*q2Dp1b-4.D0*A2c1v1*pbDp1b-4.D0*A4c2v1)
c
      cf_v1_1_ab_a(12)=t0
c
c --------cf_v1_1_ab_13a
c
      cf_v1_1_ab_a(13)=0d0
c
c --------cf_v1_1_ab_14a
c
      cf_v1_1_ab_a(14)=0d0
c
c --------cf_v1_1_ab_15a
c
      cf_v1_1_ab_a(15)=0d0
c
c --------cf_v1_1_ab_16a
c
      cf_v1_1_ab_a(16)=0d0
c
c --------cf_v1_1_ab_17a
c
      cf_v1_1_ab_a(17)=0d0
c
c --------cf_v1_1_ab_18a
c
      cf_v1_1_ab_a(18)=0d0
c
c --------cf_v1_1_ab_19a
c
      cf_v1_1_ab_a(19)=0d0
c
c --------cf_v1_1_ab_20a
c
      cf_v1_1_ab_a(20)=0d0
c
c --------cf_v1_1_ab_21a
c
      cf_v1_1_ab_a(21)=0d0
c
c --------cf_v1_1_ab_22a
c
      cf_v1_1_ab_a(22)=0d0
c
c --------cf_v1_1_ab_23a
c
      cf_v1_1_ab_a(23)=0d0
c
c --------cf_v1_1_ab_24a
c
      cf_v1_1_ab_a(24)=0d0
c
c --------cf_v1_1_ab_25a
c
      cf_v1_1_ab_a(25)=0d0
c
c --------cf_v1_1_ab_26a
c
      cf_v1_1_ab_a(26)=0d0
c
c --------cf_v1_1_ab_27a
c
      cf_v1_1_ab_a(27)=0d0
c
c --------cf_v1_1_ab_28a
c
      cf_v1_1_ab_a(28)=0d0
c
c --------cf_v1_1_ab_29a
c
      cf_v1_1_ab_a(29)=0d0
c
c --------cf_v1_1_ab_30a
c
      cf_v1_1_ab_a(30)=0d0
c
c --------cf_v1_1_ab_31a
c
      cf_v1_1_ab_a(31)=0d0
c
c
c ---------------------- coefficients for v1_1_nab_v
c
c --------cf_v1_1_nab_1v
c
      cf_v1_1_nab_v(1)=0d0
c
c --------cf_v1_1_nab_2v
c
      cf_v1_1_nab_v(2)=0d0
c
c --------cf_v1_1_nab_3v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+8.D0*A2c
     #1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b-
     #8.D0*A1c2v1*mb**2+8.D0*A1c2v1*q2Dpb+8.D0*A1c2v1*q2Dp1b-8.D0*A1c2v1
     #*pbDp1b+8.D0*A3c2v1*mb**2-8.D0*A3c2v1*q2Dpb-8.D0*A3c2v1*q2Dp1b+8.D
     #0*A3c2v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_nab_v(3)=t0
c
c --------cf_v1_1_nab_4v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+4.D0*A2c
     #1v1*mb**2-4.D0*A2c1v1*q2Dpb-4.D0*A2c1v1*q2Dp1b+4.D0*A2c1v1*pbDp1b-
     #8.D0*A4c2v1)
c
      cf_v1_1_nab_v(4)=t0
c
c --------cf_v1_1_nab_5v
c
      t0 = CV/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+4.D0*A2c
     #1v1*mb**2-4.D0*A2c1v1*q2Dpb-4.D0*A2c1v1*q2Dp1b+4.D0*A2c1v1*pbDp1b-
     #8.D0*A4c2v1)
c
      cf_v1_1_nab_v(5)=t0
c
c --------cf_v1_1_nab_6v
c
      cf_v1_1_nab_v(6)=0d0
c
c --------cf_v1_1_nab_7v
c
      cf_v1_1_nab_v(7)=0d0
c
c --------cf_v1_1_nab_8v
c
      cf_v1_1_nab_v(8)=0d0
c
c --------cf_v1_1_nab_9v
c
      t0 = CV/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1+4.D0*A1c1v1*mb**2
     #-12.D0*A1c1v1*q2Dpb-12.D0*A1c1v1*q2Dp1b+4.D0*A1c1v1*pbDp1b-4.D0*A2
     #c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v1*q2Dp1b-4.D0*A2c1v1*pbDp1b
     #+8.D0*A4c2v1)
c
      cf_v1_1_nab_v(9)=t0
c
c --------cf_v1_1_nab_10v
c
      t0 = CV/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1+4.D0*A1c1v1*mb**
     #2-12.D0*A1c1v1*q2Dpb-12.D0*A1c1v1*q2Dp1b+4.D0*A1c1v1*pbDp1b-4.D0*A
     #2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v1*q2Dp1b-4.D0*A2c1v1*pbDp1
     #b+8.D0*A4c2v1)
c
      cf_v1_1_nab_v(10)=t0
c
c --------cf_v1_1_nab_11v
c
      t0 = CV/den0x1/den0x3*(-AAi+2.D0*B0v1+2.D0*A1c1v1*mb**2-6.D
     #0*A1c1v1*q2Dpb-6.D0*A1c1v1*q2Dp1b+2.D0*A1c1v1*pbDp1b-2.D0*A2c1v1*m
     #b**2+2.D0*A2c1v1*q2Dpb+2.D0*A2c1v1*q2Dp1b-2.D0*A2c1v1*pbDp1b+4.D0*
     #A4c2v1)
c
      cf_v1_1_nab_v(11)=t0
c
c --------cf_v1_1_nab_12v
c
      t0 = CV/den0x1/den0x3*(-AAi+2.D0*B0v1+2.D0*A1c1v1*mb**2-6.D
     #0*A1c1v1*q2Dpb-6.D0*A1c1v1*q2Dp1b+2.D0*A1c1v1*pbDp1b-2.D0*A2c1v1*m
     #b**2+2.D0*A2c1v1*q2Dpb+2.D0*A2c1v1*q2Dp1b-2.D0*A2c1v1*pbDp1b+4.D0*
     #A4c2v1)
c
      cf_v1_1_nab_v(12)=t0
c
c --------cf_v1_1_nab_13v
c
      cf_v1_1_nab_v(13)=0d0
c
c --------cf_v1_1_nab_14v
c
      cf_v1_1_nab_v(14)=0d0
c
c --------cf_v1_1_nab_15v
c
      cf_v1_1_nab_v(15)=0d0
c
c --------cf_v1_1_nab_16v
c
      cf_v1_1_nab_v(16)=0d0
c
c --------cf_v1_1_nab_17v
c
      cf_v1_1_nab_v(17)=0d0
c
c --------cf_v1_1_nab_18v
c
      cf_v1_1_nab_v(18)=0d0
c
c --------cf_v1_1_nab_19v
c
      cf_v1_1_nab_v(19)=0d0
c
c --------cf_v1_1_nab_20v
c
      cf_v1_1_nab_v(20)=0d0
c
c --------cf_v1_1_nab_21v
c
      cf_v1_1_nab_v(21)=0d0
c
c --------cf_v1_1_nab_22v
c
      cf_v1_1_nab_v(22)=0d0
c
c --------cf_v1_1_nab_23v
c
      cf_v1_1_nab_v(23)=0d0
c
c --------cf_v1_1_nab_24v
c
      cf_v1_1_nab_v(24)=0d0
c
c --------cf_v1_1_nab_25v
c
      cf_v1_1_nab_v(25)=0d0
c
c --------cf_v1_1_nab_26v
c
      cf_v1_1_nab_v(26)=0d0
c
c --------cf_v1_1_nab_27v
c
      cf_v1_1_nab_v(27)=0d0
c
c --------cf_v1_1_nab_28v
c
      cf_v1_1_nab_v(28)=0d0
c
c --------cf_v1_1_nab_29v
c
      cf_v1_1_nab_v(29)=0d0
c
c --------cf_v1_1_nab_30v
c
      cf_v1_1_nab_v(30)=0d0
c
c --------cf_v1_1_nab_31v
c
      cf_v1_1_nab_v(31)=0d0
c
c
c ---------------------- coefficients for v1_1_nab_a
c
c --------cf_v1_1_nab_1a
c
      cf_v1_1_nab_a(1)=0d0
c
c --------cf_v1_1_nab_2a
c
      cf_v1_1_nab_a(2)=0d0
c
c --------cf_v1_1_nab_3a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+8.D0*A2c
     #1v1*mb**2-8.D0*A2c1v1*q2Dpb-8.D0*A2c1v1*q2Dp1b+8.D0*A2c1v1*pbDp1b-
     #8.D0*A1c2v1*mb**2+8.D0*A1c2v1*q2Dpb+8.D0*A1c2v1*q2Dp1b-8.D0*A1c2v1
     #*pbDp1b+8.D0*A3c2v1*mb**2-8.D0*A3c2v1*q2Dpb-8.D0*A3c2v1*q2Dp1b+8.D
     #0*A3c2v1*pbDp1b-8.D0*A4c2v1)
c
      cf_v1_1_nab_a(3)=t0
c
c --------cf_v1_1_nab_4a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+4.D0*A2c
     #1v1*mb**2-4.D0*A2c1v1*q2Dpb-4.D0*A2c1v1*q2Dp1b+4.D0*A2c1v1*pbDp1b-
     #8.D0*A4c2v1)
c
      cf_v1_1_nab_a(4)=t0
c
c --------cf_v1_1_nab_5a
c
      t0 = CA/den0x1/den0x3*(2.D0*AAi-4.D0*B0v1-4.D0*A1c1v1*mb**2+
     #12.D0*A1c1v1*q2Dpb+12.D0*A1c1v1*q2Dp1b-4.D0*A1c1v1*pbDp1b+4.D0*A2c
     #1v1*mb**2-4.D0*A2c1v1*q2Dpb-4.D0*A2c1v1*q2Dp1b+4.D0*A2c1v1*pbDp1b-
     #8.D0*A4c2v1)
c
      cf_v1_1_nab_a(5)=t0
c
c --------cf_v1_1_nab_6a
c
      cf_v1_1_nab_a(6)=0d0
c
c --------cf_v1_1_nab_7a
c
      cf_v1_1_nab_a(7)=0d0
c
c --------cf_v1_1_nab_8a
c
      cf_v1_1_nab_a(8)=0d0
c
c --------cf_v1_1_nab_9a
c
      t0 = CA/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1+4.D0*A1c1v1*mb**2
     #-12.D0*A1c1v1*q2Dpb-12.D0*A1c1v1*q2Dp1b+4.D0*A1c1v1*pbDp1b-4.D0*A2
     #c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v1*q2Dp1b-4.D0*A2c1v1*pbDp1b
     #+8.D0*A4c2v1)
c
      cf_v1_1_nab_a(9)=t0
c
c --------cf_v1_1_nab_10a
c
      t0 = CA/den0x1/den0x3*(-2.D0*AAi+4.D0*B0v1+4.D0*A1c1v1*mb**
     #2-12.D0*A1c1v1*q2Dpb-12.D0*A1c1v1*q2Dp1b+4.D0*A1c1v1*pbDp1b-4.D0*A
     #2c1v1*mb**2+4.D0*A2c1v1*q2Dpb+4.D0*A2c1v1*q2Dp1b-4.D0*A2c1v1*pbDp1
     #b+8.D0*A4c2v1)
c
      cf_v1_1_nab_a(10)=t0
c
c --------cf_v1_1_nab_11a
c
      t0 = CA/den0x1/den0x3*(-AAi+2.D0*B0v1+2.D0*A1c1v1*mb**2-6.D
     #0*A1c1v1*q2Dpb-6.D0*A1c1v1*q2Dp1b+2.D0*A1c1v1*pbDp1b-2.D0*A2c1v1*m
     #b**2+2.D0*A2c1v1*q2Dpb+2.D0*A2c1v1*q2Dp1b-2.D0*A2c1v1*pbDp1b+4.D0*
     #A4c2v1)
c
      cf_v1_1_nab_a(11)=t0
c
c --------cf_v1_1_nab_12a
c
      t0 = CA/den0x1/den0x3*(-AAi+2.D0*B0v1+2.D0*A1c1v1*mb**2-6.D
     #0*A1c1v1*q2Dpb-6.D0*A1c1v1*q2Dp1b+2.D0*A1c1v1*pbDp1b-2.D0*A2c1v1*m
     #b**2+2.D0*A2c1v1*q2Dpb+2.D0*A2c1v1*q2Dp1b-2.D0*A2c1v1*pbDp1b+4.D0*
     #A4c2v1)
c
      cf_v1_1_nab_a(12)=t0
c
c --------cf_v1_1_nab_13a
c
      cf_v1_1_nab_a(13)=0d0
c
c --------cf_v1_1_nab_14a
c
      cf_v1_1_nab_a(14)=0d0
c
c --------cf_v1_1_nab_15a
c
      cf_v1_1_nab_a(15)=0d0
c
c --------cf_v1_1_nab_16a
c
      cf_v1_1_nab_a(16)=0d0
c
c --------cf_v1_1_nab_17a
c
      cf_v1_1_nab_a(17)=0d0
c
c --------cf_v1_1_nab_18a
c
      cf_v1_1_nab_a(18)=0d0
c
c --------cf_v1_1_nab_19a
c
      cf_v1_1_nab_a(19)=0d0
c
c --------cf_v1_1_nab_20a
c
      cf_v1_1_nab_a(20)=0d0
c
c --------cf_v1_1_nab_21a
c
      cf_v1_1_nab_a(21)=0d0
c
c --------cf_v1_1_nab_22a
c
      cf_v1_1_nab_a(22)=0d0
c
c --------cf_v1_1_nab_23a
c
      cf_v1_1_nab_a(23)=0d0
c
c --------cf_v1_1_nab_24a
c
      cf_v1_1_nab_a(24)=0d0
c
c --------cf_v1_1_nab_25a
c
      cf_v1_1_nab_a(25)=0d0
c
c --------cf_v1_1_nab_26a
c
      cf_v1_1_nab_a(26)=0d0
c
c --------cf_v1_1_nab_27a
c
      cf_v1_1_nab_a(27)=0d0
c
c --------cf_v1_1_nab_28a
c
      cf_v1_1_nab_a(28)=0d0
c
c --------cf_v1_1_nab_29a
c
      cf_v1_1_nab_a(29)=0d0
c
c --------cf_v1_1_nab_30a
c
      cf_v1_1_nab_a(30)=0d0
c
c --------cf_v1_1_nab_31a
c
      cf_v1_1_nab_a(31)=0d0
c
c
c
c --------------- integrals for V1_2 (both "ab" and "nab") 
c in the notation used for V_ab and V_nab
c         p1(i)=q1(i)
c         p2(i)=-q2(i)+pw(i)
c         q(i)=p2(i)-p1(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pb(i)-p1b(i)
         p12(i)=p1(i)+p2(i)
      enddo
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
      m1=0d0
      m2=0d0
      m3=0d0
c
      call wbb_f3pf_2m(xs2,xs12,C0v1)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v1=B023
      B1v1=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v1,B012,B013,B023,A1c1v1,A2c1v1)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v1,B012,B013,B023,B112,B113,B123,
     -            A4c2v1,A1c2v1,A2c2v1,A3c2v1)
c
      do i=1,75
         cf_v1_2_ab_v(i)=0d0
         cf_v1_2_nab_v(i)=0d0
         cf_v1_2_ab_a(i)=0d0
         cf_v1_2_nab_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v1_2_ab_v
c
c --------cf_v1_2_ab_1v
c
      cf_v1_2_ab_v(1)=0d0
c
c --------cf_v1_2_ab_2v
c
      t0 = CV/den0x2/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*mb**2-8
     #.D0*C0v1*pbDp1b-8.D0*A1c1v1*mb**2-8.D0*A1c1v1*pbDp1b-8.D0*A3c2v1*m
     #b**2+8.D0*A3c2v1*q1Dpb+8.D0*A3c2v1*q1Dp1b-8.D0*A3c2v1*pbDp1b-8.D0*
     #A4c2v1)
c
      cf_v1_2_ab_v(2)=t0
c
c --------cf_v1_2_ab_3v
c
      cf_v1_2_ab_v(3)=0d0
c
c --------cf_v1_2_ab_4v
c
      cf_v1_2_ab_v(4)=0d0
c
c --------cf_v1_2_ab_5v
c
      cf_v1_2_ab_v(5)=0d0
c
c --------cf_v1_2_ab_6v
c
      cf_v1_2_ab_v(6)=0d0
c
c --------cf_v1_2_ab_7v
c
      cf_v1_2_ab_v(7)=0d0
c
c --------cf_v1_2_ab_8v
c
      cf_v1_2_ab_v(8)=0d0
c
c --------cf_v1_2_ab_9v
c
      cf_v1_2_ab_v(9)=0d0
c
c --------cf_v1_2_ab_10v
c
      cf_v1_2_ab_v(10)=0d0
c
c --------cf_v1_2_ab_11v
c
      t0 = CV/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q1Dpb-4.D0*
     #C0v1*q1Dp1b-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b+4.D0*A2c1v1*mb**2
     #-4.D0*A2c1v1*q1Dpb-4.D0*A2c1v1*q1Dp1b+4.D0*A2c1v1*pbDp1b-4.D0*A4c2
     #v1)
c
      cf_v1_2_ab_v(11)=t0
c
c --------cf_v1_2_ab_12v
c
      t0 = CV/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q1Dpb-4.D0*
     #C0v1*q1Dp1b-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b+4.D0*A2c1v1*mb**2
     #-4.D0*A2c1v1*q1Dpb-4.D0*A2c1v1*q1Dp1b+4.D0*A2c1v1*pbDp1b-4.D0*A4c2
     #v1)
c
      cf_v1_2_ab_v(12)=t0
c
c --------cf_v1_2_ab_13v
c
      cf_v1_2_ab_v(13)=0d0
c
c --------cf_v1_2_ab_14v
c
      cf_v1_2_ab_v(14)=0d0
c
c --------cf_v1_2_ab_15v
c
      cf_v1_2_ab_v(15)=0d0
c
c --------cf_v1_2_ab_16v
c
      cf_v1_2_ab_v(16)=0d0
c
c --------cf_v1_2_ab_17v
c
      cf_v1_2_ab_v(17)=0d0
c
c --------cf_v1_2_ab_18v
c
      cf_v1_2_ab_v(18)=0d0
c
c --------cf_v1_2_ab_19v
c
      cf_v1_2_ab_v(19)=0d0
c
c --------cf_v1_2_ab_20v
c
      cf_v1_2_ab_v(20)=0d0
c
c --------cf_v1_2_ab_21v
c
      cf_v1_2_ab_v(21)=0d0
c
c --------cf_v1_2_ab_22v
c
      cf_v1_2_ab_v(22)=0d0
c
c --------cf_v1_2_ab_23v
c
      cf_v1_2_ab_v(23)=0d0
c
c --------cf_v1_2_ab_24v
c
      cf_v1_2_ab_v(24)=0d0
c
c --------cf_v1_2_ab_25v
c
      cf_v1_2_ab_v(25)=0d0
c
c --------cf_v1_2_ab_26v
c
      cf_v1_2_ab_v(26)=0d0
c
c --------cf_v1_2_ab_27v
c
      cf_v1_2_ab_v(27)=0d0
c
c --------cf_v1_2_ab_28v
c
      cf_v1_2_ab_v(28)=0d0
c
c --------cf_v1_2_ab_29v
c
      cf_v1_2_ab_v(29)=0d0
c
c --------cf_v1_2_ab_30v
c
      cf_v1_2_ab_v(30)=0d0
c
c --------cf_v1_2_ab_31v
c
      cf_v1_2_ab_v(31)=0d0
c
c
c ---------------------- coefficients for v1_2_ab_a
c
c --------cf_v1_2_ab_1a
c
      cf_v1_2_ab_a(1)=0d0
c
c --------cf_v1_2_ab_2a
c
      t0 = CA/den0x2/den0x3*(-2.D0*AAi+4.D0*B0v1-8.D0*C0v1*mb**2-8
     #.D0*C0v1*pbDp1b-8.D0*A1c1v1*mb**2-8.D0*A1c1v1*pbDp1b-8.D0*A3c2v1*m
     #b**2+8.D0*A3c2v1*q1Dpb+8.D0*A3c2v1*q1Dp1b-8.D0*A3c2v1*pbDp1b-8.D0*
     #A4c2v1)
c
      cf_v1_2_ab_a(2)=t0
c
c --------cf_v1_2_ab_3a
c
      cf_v1_2_ab_a(3)=0d0
c
c --------cf_v1_2_ab_4a
c
      cf_v1_2_ab_a(4)=0d0
c
c --------cf_v1_2_ab_5a
c
      cf_v1_2_ab_a(5)=0d0
c
c --------cf_v1_2_ab_6a
c
      cf_v1_2_ab_a(6)=0d0
c
c --------cf_v1_2_ab_7a
c
      cf_v1_2_ab_a(7)=0d0
c
c --------cf_v1_2_ab_8a
c
      cf_v1_2_ab_a(8)=0d0
c
c --------cf_v1_2_ab_9a
c
      cf_v1_2_ab_a(9)=0d0
c
c --------cf_v1_2_ab_10a
c
      cf_v1_2_ab_a(10)=0d0
c
c --------cf_v1_2_ab_11a
c
      t0 = CA/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q1Dpb-4.D0*
     #C0v1*q1Dp1b-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b+4.D0*A2c1v1*mb**2
     #-4.D0*A2c1v1*q1Dpb-4.D0*A2c1v1*q1Dp1b+4.D0*A2c1v1*pbDp1b-4.D0*A4c2
     #v1)
c
      cf_v1_2_ab_a(11)=t0
c
c --------cf_v1_2_ab_12a
c
      t0 = CA/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*C0v1*q1Dpb-4.D0*
     #C0v1*q1Dp1b-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b+4.D0*A2c1v1*mb**2
     #-4.D0*A2c1v1*q1Dpb-4.D0*A2c1v1*q1Dp1b+4.D0*A2c1v1*pbDp1b-4.D0*A4c2
     #v1)
c
      cf_v1_2_ab_a(12)=t0
c
c --------cf_v1_2_ab_13a
c
      cf_v1_2_ab_a(13)=0d0
c
c --------cf_v1_2_ab_14a
c
      cf_v1_2_ab_a(14)=0d0
c
c --------cf_v1_2_ab_15a
c
      cf_v1_2_ab_a(15)=0d0
c
c --------cf_v1_2_ab_16a
c
      cf_v1_2_ab_a(16)=0d0
c
c --------cf_v1_2_ab_17a
c
      cf_v1_2_ab_a(17)=0d0
c
c --------cf_v1_2_ab_18a
c
      cf_v1_2_ab_a(18)=0d0
c
c --------cf_v1_2_ab_19a
c
      cf_v1_2_ab_a(19)=0d0
c
c --------cf_v1_2_ab_20a
c
      cf_v1_2_ab_a(20)=0d0
c
c --------cf_v1_2_ab_21a
c
      cf_v1_2_ab_a(21)=0d0
c
c --------cf_v1_2_ab_22a
c
      cf_v1_2_ab_a(22)=0d0
c
c --------cf_v1_2_ab_23a
c
      cf_v1_2_ab_a(23)=0d0
c
c --------cf_v1_2_ab_24a
c
      cf_v1_2_ab_a(24)=0d0
c
c --------cf_v1_2_ab_25a
c
      cf_v1_2_ab_a(25)=0d0
c
c --------cf_v1_2_ab_26a
c
      cf_v1_2_ab_a(26)=0d0
c
c --------cf_v1_2_ab_27a
c
      cf_v1_2_ab_a(27)=0d0
c
c --------cf_v1_2_ab_28a
c
      cf_v1_2_ab_a(28)=0d0
c
c --------cf_v1_2_ab_29a
c
      cf_v1_2_ab_a(29)=0d0
c
c --------cf_v1_2_ab_30a
c
      cf_v1_2_ab_a(30)=0d0
c
c --------cf_v1_2_ab_31a
c
      cf_v1_2_ab_a(31)=0d0
c
c
c ---------------------- coefficients for v1_2_nab_v
c
c --------cf_v1_2_nab_1v
c
      cf_v1_2_nab_v(1)=0d0
c
c --------cf_v1_2_nab_2v
c
      t0 = CV/den0x2/den0x3*(-2.D0*AAi+4.D0*B0v1-4.D0*A1c1v1*mb**2
     #-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b-4.D0*A1c1v1*pbDp1b+8.D0*A2c1
     #v1*mb**2-8.D0*A2c1v1*q1Dpb-8.D0*A2c1v1*q1Dp1b+8.D0*A2c1v1*pbDp1b+8
     #.D0*A3c2v1*mb**2-8.D0*A3c2v1*q1Dpb-8.D0*A3c2v1*q1Dp1b+8.D0*A3c2v1*
     #pbDp1b+8.D0*A4c2v1)
c
      cf_v1_2_nab_v(2)=t0
c
c --------cf_v1_2_nab_3v
c
      cf_v1_2_nab_v(3)=0d0
c
c --------cf_v1_2_nab_4v
c
      cf_v1_2_nab_v(4)=0d0
c
c --------cf_v1_2_nab_5v
c
      cf_v1_2_nab_v(5)=0d0
c
c --------cf_v1_2_nab_6v
c
      cf_v1_2_nab_v(6)=0d0
c
c --------cf_v1_2_nab_7v
c
      cf_v1_2_nab_v(7)=0d0
c
c --------cf_v1_2_nab_8v
c
      cf_v1_2_nab_v(8)=0d0
c
c --------cf_v1_2_nab_9v
c
      cf_v1_2_nab_v(9)=0d0
c
c --------cf_v1_2_nab_10v
c
      cf_v1_2_nab_v(10)=0d0
c
c --------cf_v1_2_nab_11v
c
      t0 = CV/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*A1c1v1*q1Dpb-4.D
     #0*A1c1v1*q1Dp1b+2.D0*A2c1v1*mb**2-2.D0*A2c1v1*q1Dpb-2.D0*A2c1v1*q1
     #Dp1b+2.D0*A2c1v1*pbDp1b+4.D0*A4c2v1)
c
      cf_v1_2_nab_v(11)=t0
c
c --------cf_v1_2_nab_12v
c
      t0 = CV/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*A1c1v1*q1Dpb-4.D
     #0*A1c1v1*q1Dp1b+2.D0*A2c1v1*mb**2-2.D0*A2c1v1*q1Dpb-2.D0*A2c1v1*q1
     #Dp1b+2.D0*A2c1v1*pbDp1b+4.D0*A4c2v1)
c
      cf_v1_2_nab_v(12)=t0
c
c --------cf_v1_2_nab_13v
c
      cf_v1_2_nab_v(13)=0d0
c
c --------cf_v1_2_nab_14v
c
      cf_v1_2_nab_v(14)=0d0
c
c --------cf_v1_2_nab_15v
c
      cf_v1_2_nab_v(15)=0d0
c
c --------cf_v1_2_nab_16v
c
      cf_v1_2_nab_v(16)=0d0
c
c --------cf_v1_2_nab_17v
c
      cf_v1_2_nab_v(17)=0d0
c
c --------cf_v1_2_nab_18v
c
      cf_v1_2_nab_v(18)=0d0
c
c --------cf_v1_2_nab_19v
c
      cf_v1_2_nab_v(19)=0d0
c
c --------cf_v1_2_nab_20v
c
      cf_v1_2_nab_v(20)=0d0
c
c --------cf_v1_2_nab_21v
c
      cf_v1_2_nab_v(21)=0d0
c
c --------cf_v1_2_nab_22v
c
      cf_v1_2_nab_v(22)=0d0
c
c --------cf_v1_2_nab_23v
c
      cf_v1_2_nab_v(23)=0d0
c
c --------cf_v1_2_nab_24v
c
      cf_v1_2_nab_v(24)=0d0
c
c --------cf_v1_2_nab_25v
c
      cf_v1_2_nab_v(25)=0d0
c
c --------cf_v1_2_nab_26v
c
      cf_v1_2_nab_v(26)=0d0
c
c --------cf_v1_2_nab_27v
c
      cf_v1_2_nab_v(27)=0d0
c
c --------cf_v1_2_nab_28v
c
      cf_v1_2_nab_v(28)=0d0
c
c --------cf_v1_2_nab_29v
c
      cf_v1_2_nab_v(29)=0d0
c
c --------cf_v1_2_nab_30v
c
      cf_v1_2_nab_v(30)=0d0
c
c --------cf_v1_2_nab_31v
c
      cf_v1_2_nab_v(31)=0d0
c
c
c ---------------------- coefficients for v1_2_nab_a
c
c --------cf_v1_2_nab_1a
c
      cf_v1_2_nab_a(1)=0d0
c
c --------cf_v1_2_nab_2a
c
      t0 = CA/den0x2/den0x3*(-2.D0*AAi+4.D0*B0v1-4.D0*A1c1v1*mb**2
     #-4.D0*A1c1v1*q1Dpb-4.D0*A1c1v1*q1Dp1b-4.D0*A1c1v1*pbDp1b+8.D0*A2c1
     #v1*mb**2-8.D0*A2c1v1*q1Dpb-8.D0*A2c1v1*q1Dp1b+8.D0*A2c1v1*pbDp1b+8
     #.D0*A3c2v1*mb**2-8.D0*A3c2v1*q1Dpb-8.D0*A3c2v1*q1Dp1b+8.D0*A3c2v1*
     #pbDp1b+8.D0*A4c2v1)
c
      cf_v1_2_nab_a(2)=t0
c
c --------cf_v1_2_nab_3a
c
      cf_v1_2_nab_a(3)=0d0
c
c --------cf_v1_2_nab_4a
c
      cf_v1_2_nab_a(4)=0d0
c
c --------cf_v1_2_nab_5a
c
      cf_v1_2_nab_a(5)=0d0
c
c --------cf_v1_2_nab_6a
c
      cf_v1_2_nab_a(6)=0d0
c
c --------cf_v1_2_nab_7a
c
      cf_v1_2_nab_a(7)=0d0
c
c --------cf_v1_2_nab_8a
c
      cf_v1_2_nab_a(8)=0d0
c
c --------cf_v1_2_nab_9a
c
      cf_v1_2_nab_a(9)=0d0
c
c --------cf_v1_2_nab_10a
c
      cf_v1_2_nab_a(10)=0d0
c
c --------cf_v1_2_nab_11a
c
      t0 = CA/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*A1c1v1*q1Dpb-4.D
     #0*A1c1v1*q1Dp1b+2.D0*A2c1v1*mb**2-2.D0*A2c1v1*q1Dpb-2.D0*A2c1v1*q1
     #Dp1b+2.D0*A2c1v1*pbDp1b+4.D0*A4c2v1)
c
      cf_v1_2_nab_a(11)=t0
c
c --------cf_v1_2_nab_12a
c
      t0 = CA/den0x2/den0x3*(-AAi+2.D0*B0v1-4.D0*A1c1v1*q1Dpb-4.D
     #0*A1c1v1*q1Dp1b+2.D0*A2c1v1*mb**2-2.D0*A2c1v1*q1Dpb-2.D0*A2c1v1*q1
     #Dp1b+2.D0*A2c1v1*pbDp1b+4.D0*A4c2v1)
c
      cf_v1_2_nab_a(12)=t0
c
c --------cf_v1_2_nab_13a
c
      cf_v1_2_nab_a(13)=0d0
c
c --------cf_v1_2_nab_14a
c
      cf_v1_2_nab_a(14)=0d0
c
c --------cf_v1_2_nab_15a
c
      cf_v1_2_nab_a(15)=0d0
c
c --------cf_v1_2_nab_16a
c
      cf_v1_2_nab_a(16)=0d0
c
c --------cf_v1_2_nab_17a
c
      cf_v1_2_nab_a(17)=0d0
c
c --------cf_v1_2_nab_18a
c
      cf_v1_2_nab_a(18)=0d0
c
c --------cf_v1_2_nab_19a
c
      cf_v1_2_nab_a(19)=0d0
c
c --------cf_v1_2_nab_20a
c
      cf_v1_2_nab_a(20)=0d0
c
c --------cf_v1_2_nab_21a
c
      cf_v1_2_nab_a(21)=0d0
c
c --------cf_v1_2_nab_22a
c
      cf_v1_2_nab_a(22)=0d0
c
c --------cf_v1_2_nab_23a
c
      cf_v1_2_nab_a(23)=0d0
c
c --------cf_v1_2_nab_24a
c
      cf_v1_2_nab_a(24)=0d0
c
c --------cf_v1_2_nab_25a
c
      cf_v1_2_nab_a(25)=0d0
c
c --------cf_v1_2_nab_26a
c
      cf_v1_2_nab_a(26)=0d0
c
c --------cf_v1_2_nab_27a
c
      cf_v1_2_nab_a(27)=0d0
c
c --------cf_v1_2_nab_28a
c
      cf_v1_2_nab_a(28)=0d0
c
c --------cf_v1_2_nab_29a
c
      cf_v1_2_nab_a(29)=0d0
c
c --------cf_v1_2_nab_30a
c
      cf_v1_2_nab_a(30)=0d0
c
c --------cf_v1_2_nab_31a
c
      cf_v1_2_nab_a(31)=0d0
c
      return
      end
c
      subroutine wbb_coeff_v2(q1,q2,pb,p1b,pw,mb,cf_v2_1_ab_v,cf_v2_1_ab_a,
     -           cf_v2_1_nab_v,cf_v2_1_nab_a,cf_v2_2_ab_v,cf_v2_2_ab_a,
     -           cf_v2_2_nab_v,cf_v2_2_nab_a)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd
      real * 8 wbb_dot_4,wbb_c0_sd
c
c external wbb_masses and momenta
c
      integer i
      real * 8 mb,m1,m2,m3,q1(4),q2(4),pb(4),p1b(4),pw(4),p1(4),
     - p2(4),p12(4),xs1,xs2,xs12
      real * 8 CV,CA,den0x1,den0x2,den0x3,
     - q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,t0
      real * 8 AAi,B0v2ab,B1v2ab,C0v2ab,A1c1v2ab,A2c1v2ab,
     - A1c2v2ab,A2c2v2ab,A3c2v2ab,A4c2v2ab,
     - B0v2nab,B1v2nab,C0v2nab,A1c1v2nab,A2c1v2nab,A1c2v2nab,
     - A2c2v2nab,A3c2v2nab,A4c2v2nab,
     - B012,B013,B023,B112,B113,B123!,C0,A1c1,A2c1,A1c2,A2c2,A3c2,A4c2
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real * 8 cf_v2_1_ab_v(75),cf_v2_1_ab_a(75),
     - cf_v2_1_nab_v(75),cf_v2_1_nab_a(75),cf_v2_2_ab_v(75),
     - cf_v2_2_ab_a(75),cf_v2_2_nab_v(75),cf_v2_2_nab_a(75)
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
c
c debug parameter
c
      logical ldebugv2
      parameter(ldebugv2=.false.)
c      parameter(ldebugv2=.true.)
c
      if(ldebugv2) then
      print*,' ------  Debugging coeff_v2_1, coeff_v2_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for V2_1_ab and V2_2_ab
c in the notation used for V_ab and V_nab
c         p1(i)=-p1b(i)
c         p2(i)=pb(i)
c         q(i)=p2(i)-p1(i)=pb(i)+p1b(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=-p1b(i)
         p2(i)=pb(i)+p1b(i)
         p12(i)=p1(i)+p2(i)
      enddo
      m1=0d0
      m2=mb
      m3=mb
      xs1=wbb_dot_4(p1,p1)      
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
c
      call wbb_f3pf_v2(xs2,C0v2ab)
c
      call wbb_bfunc(dabs(xs1),m1,m2,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v2ab=B023
      B1v2ab=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v2ab,B012,B013,B023,A1c1v2ab,
     -            A2c1v2ab)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v2ab,B012,B013,B023,B112,B113,B123,
     -            A4c2v2ab,A1c2v2ab,A2c2v2ab,A3c2v2ab)
c
      do i=1,75
         cf_v2_1_ab_v(i)=0d0
         cf_v2_2_ab_v(i)=0d0
         cf_v2_1_ab_a(i)=0d0
         cf_v2_2_ab_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v2_1_ab_v
c
c --------cf_v2_1_ab_1v
c
      t0 = CV/den0x1/den0x3*(-4.D0*mb**3*A1c1v2ab+8.D0*mb**3*A2c1v
     #2ab+4.D0*mb**3*A1c2v2ab-8.D0*mb**3*A3c2v2ab+8.D0*q2Dpb*mb*A1c1v2ab
     #-8.D0*q2Dpb*mb*A2c1v2ab+8.D0*q2Dpb*mb*A3c2v2ab-8.D0*q2Dp1b*mb*A2c1
     #v2ab-8.D0*q2Dp1b*mb*A1c2v2ab+8.D0*q2Dp1b*mb*A3c2v2ab+8.D0*pbDp1b*m
     #b*A2c1v2ab+8.D0*pbDp1b*mb*A1c2v2ab-8.D0*pbDp1b*mb*A3c2v2ab)
c
      cf_v2_1_ab_v(1)=t0
c
c --------cf_v2_1_ab_3v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(3)=t0
c
c --------cf_v2_1_ab_4v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(4)=t0
c
c --------cf_v2_1_ab_5v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(5)=t0
c
c --------cf_v2_1_ab_9v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0v
     #2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(9)=t0
c
c --------cf_v2_1_ab_10v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0
     #v2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(10)=t0
c
c --------cf_v2_1_ab_11v
c
      t0 = CV/den0x1/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(11)=t0
c
c --------cf_v2_1_ab_12v
c
      t0 = CV/den0x1/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_v(12)=t0
c
c --------cf_v2_1_ab_13v
c
      t0 = CV/den0x1/den0x3*(-4.D0*mb*A1c1v2ab-4.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_v(13)=t0
c
c --------cf_v2_1_ab_14v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb*A1c1v2ab+8.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_v(14)=t0
c
c --------cf_v2_1_ab_15v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb*A1c1v2ab-8.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_v(15)=t0
c
c
c ---------------------- coefficients for v2_1_ab_a
c
c --------cf_v2_1_ab_1a
c
      t0 = CA/den0x1/den0x3*(-4.D0*mb**3*A1c1v2ab+8.D0*mb**3*A2c1v
     #2ab+4.D0*mb**3*A1c2v2ab-8.D0*mb**3*A3c2v2ab+8.D0*q2Dpb*mb*A1c1v2ab
     #-8.D0*q2Dpb*mb*A2c1v2ab+8.D0*q2Dpb*mb*A3c2v2ab-8.D0*q2Dp1b*mb*A2c1
     #v2ab-8.D0*q2Dp1b*mb*A1c2v2ab+8.D0*q2Dp1b*mb*A3c2v2ab+8.D0*pbDp1b*m
     #b*A2c1v2ab+8.D0*pbDp1b*mb*A1c2v2ab-8.D0*pbDp1b*mb*A3c2v2ab)
c
      cf_v2_1_ab_a(1)=t0
c
c --------cf_v2_1_ab_3a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(3)=t0
c
c --------cf_v2_1_ab_4a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(4)=t0
c
c --------cf_v2_1_ab_5a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb**2*A1c1v2ab+2.D0*AAi-4.D0*B0
     #v2ab+8.D0*A4c2v2ab+8.D0*pbDp1b*C0v2ab+8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(5)=t0
c
c --------cf_v2_1_ab_9a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0v
     #2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(9)=t0
c
c --------cf_v2_1_ab_10a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0
     #v2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(10)=t0
c
c --------cf_v2_1_ab_11a
c
      t0 = CA/den0x1/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(11)=t0
c
c --------cf_v2_1_ab_12a
c
      t0 = CA/den0x1/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_1_ab_a(12)=t0
c
c --------cf_v2_1_ab_13a
c
      t0 = CA/den0x1/den0x3*(-4.D0*mb*A1c1v2ab-4.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_a(13)=t0
c
c --------cf_v2_1_ab_14a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb*A1c1v2ab+8.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_a(14)=t0
c
c --------cf_v2_1_ab_15a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb*A1c1v2ab-8.D0*mb*A1c2v2ab)
c
      cf_v2_1_ab_a(15)=t0
c
c ---------------------- coefficients for v2_2_ab_v
c
c --------cf_v2_2_ab_1v
c
      t0 = CV/den0x2/den0x3*(4.D0*mb**3*A1c1v2ab-8.D0*mb**3*A2c1v2
     #ab-4.D0*mb**3*A1c2v2ab+8.D0*mb**3*A3c2v2ab-8.D0*q1Dpb*mb*A1c1v2ab+
     #8.D0*q1Dpb*mb*A2c1v2ab-8.D0*q1Dpb*mb*A3c2v2ab+8.D0*q1Dp1b*mb*A2c1v
     #2ab+8.D0*q1Dp1b*mb*A1c2v2ab-8.D0*q1Dp1b*mb*A3c2v2ab+8.D0*pbDp1b*mb
     #*A1c1v2ab-8.D0*pbDp1b*mb*A2c1v2ab+8.D0*pbDp1b*mb*A3c2v2ab)
c
      cf_v2_2_ab_v(1)=t0
c
c --------cf_v2_2_ab_2v
c
      t0 = CV/den0x2/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0v
     #2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_v(2)=t0
c
c --------cf_v2_2_ab_11v
c
      t0 = CV/den0x2/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_v(11)=t0
c
c --------cf_v2_2_ab_12v
c
      t0 = CV/den0x2/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_v(12)=t0
c
c --------cf_v2_2_ab_13v
c
      t0 = CV/den0x2/den0x3*(-4.D0*mb*A1c1v2ab-4.D0*mb*A1c2v2ab)
c
      cf_v2_2_ab_v(13)=t0
c
c
c ---------------------- coefficients for v2_2_ab_a
c
c --------cf_v2_2_ab_1a
c
      t0 = CA/den0x2/den0x3*(4.D0*mb**3*A1c1v2ab-8.D0*mb**3*A2c1v2
     #ab-4.D0*mb**3*A1c2v2ab+8.D0*mb**3*A3c2v2ab-8.D0*q1Dpb*mb*A1c1v2ab+
     #8.D0*q1Dpb*mb*A2c1v2ab-8.D0*q1Dpb*mb*A3c2v2ab+8.D0*q1Dp1b*mb*A2c1v
     #2ab+8.D0*q1Dp1b*mb*A1c2v2ab-8.D0*q1Dp1b*mb*A3c2v2ab+8.D0*pbDp1b*mb
     #*A1c1v2ab-8.D0*pbDp1b*mb*A2c1v2ab+8.D0*pbDp1b*mb*A3c2v2ab)
c
      cf_v2_2_ab_a(1)=t0
c
c --------cf_v2_2_ab_2a
c
      t0 = CA/den0x2/den0x3*(8.D0*mb**2*A1c1v2ab-2.D0*AAi+4.D0*B0v
     #2ab-8.D0*A4c2v2ab-8.D0*pbDp1b*C0v2ab-8.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_a(2)=t0
c
c --------cf_v2_2_ab_11a
c
      t0 = CA/den0x2/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_a(11)=t0
c
c --------cf_v2_2_ab_12a
c
      t0 = CA/den0x2/den0x3*(4.D0*mb**2*A1c1v2ab-AAi+2.D0*B0v2ab-
     #4.D0*A4c2v2ab-4.D0*pbDp1b*C0v2ab-4.D0*pbDp1b*A1c1v2ab)
c
      cf_v2_2_ab_a(12)=t0
c
c --------cf_v2_2_ab_13a
c
      t0 = CA/den0x2/den0x3*(-4.D0*mb*A1c1v2ab-4.D0*mb*A1c2v2ab)
c
      cf_v2_2_ab_a(13)=t0
c
c
c --------------- integrals for V2_1_nab and V2_2_nab
c in the notation used for V_ab and V_nab
c         p1(i)=-p1b(i)
c         p2(i)=pb(i)
c         q(i)=p2(i)-p1(i)=pb(i)+p1b(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=-p1b(i)
         p2(i)=pb(i)+p1b(i)
         p12(i)=p1(i)+p2(i)
      enddo
      m1=mb
      m2=0d0
      m3=0d0
      xs1 = wbb_dot_4(p1,p1)
      xs2 = wbb_dot_4(p2,p2)
      xs12 = wbb_dot_4(p12,p12)
c
      C0v2nab=wbb_c0_sd(xs1,xs2,xs12,m1,m2,m3)
c
      call wbb_bfunc(dabs(xs1),m1,m2,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v2nab=B023
      B1v2nab=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v2nab,B012,B013,B023,A1c1v2nab,
     -            A2c1v2nab)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v2nab,B012,B013,B023,B112,B113,B123,
     -            A4c2v2nab,A1c2v2nab,A2c2v2nab,A3c2v2nab)
c
      do i=1,75
         cf_v2_1_nab_v(i)=0d0
         cf_v2_2_nab_v(i)=0d0
         cf_v2_1_nab_a(i)=0d0
         cf_v2_2_nab_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v2_1_nab_v
c
c --------cf_v2_1_nab_1v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb**3*A1c1v2nab+16.D0*mb**3*A2c
     #1v2nab-4.D0*mb**3*A1c2v2nab+8.D0*mb**3*A3c2v2nab+4.D0*q2Dpb*mb*A1c
     #1v2nab-16.D0*q2Dpb*mb*A2c1v2nab-8.D0*q2Dpb*mb*A3c2v2nab+12.D0*q2Dp
     #1b*mb*A1c1v2nab-16.D0*q2Dp1b*mb*A2c1v2nab+8.D0*q2Dp1b*mb*A1c2v2nab
     #-8.D0*q2Dp1b*mb*A3c2v2nab-12.D0*pbDp1b*mb*A1c1v2nab+16.D0*pbDp1b*m
     #b*A2c1v2nab-8.D0*pbDp1b*mb*A1c2v2nab+8.D0*pbDp1b*mb*A3c2v2nab)
c
      cf_v2_1_nab_v(1)=t0
c
c --------cf_v2_1_nab_3v
c
      t0 = CV/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(3)=t0
c
c --------cf_v2_1_nab_4v
c
      t0 = CV/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(4)=t0
c
c --------cf_v2_1_nab_5v
c
      t0 = CV/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(5)=t0
c
c --------cf_v2_1_nab_9v
c
      t0 = CV/den0x1/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v2
     #nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(9)=t0
c
c --------cf_v2_1_nab_10v
c
      t0 = CV/den0x1/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v
     #2nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(10)=t0
c
c --------cf_v2_1_nab_11v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(11)=t0
c
c --------cf_v2_1_nab_12v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_v(12)=t0
c
c --------cf_v2_1_nab_13v
c
      t0 = CV/den0x1/den0x3*(4.D0*mb*A1c1v2nab+4.D0*mb*A1c2v2nab)
c
      cf_v2_1_nab_v(13)=t0
c
c --------cf_v2_1_nab_14v
c
      t0 = CV/den0x1/den0x3*(-8.D0*mb*A1c1v2nab-8.D0*mb*A1c2v2nab
     #)
c
      cf_v2_1_nab_v(14)=t0
c
c --------cf_v2_1_nab_15v
c
      t0 = CV/den0x1/den0x3*(8.D0*mb*A1c1v2nab+8.D0*mb*A1c2v2nab)
c
      cf_v2_1_nab_v(15)=t0
c
c
c ---------------------- coefficients for v2_1_nab_a
c
c --------cf_v2_1_nab_1a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb**3*A1c1v2nab+16.D0*mb**3*A2c
     #1v2nab-4.D0*mb**3*A1c2v2nab+8.D0*mb**3*A3c2v2nab+4.D0*q2Dpb*mb*A1c
     #1v2nab-16.D0*q2Dpb*mb*A2c1v2nab-8.D0*q2Dpb*mb*A3c2v2nab+12.D0*q2Dp
     #1b*mb*A1c1v2nab-16.D0*q2Dp1b*mb*A2c1v2nab+8.D0*q2Dp1b*mb*A1c2v2nab
     #-8.D0*q2Dp1b*mb*A3c2v2nab-12.D0*pbDp1b*mb*A1c1v2nab+16.D0*pbDp1b*m
     #b*A2c1v2nab-8.D0*pbDp1b*mb*A1c2v2nab+8.D0*pbDp1b*mb*A3c2v2nab)
c
      cf_v2_1_nab_a(1)=t0
c
c --------cf_v2_1_nab_3a
c
      t0 = CA/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(3)=t0
c
c --------cf_v2_1_nab_4a
c
      t0 = CA/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(4)=t0
c
c --------cf_v2_1_nab_5a
c
      t0 = CA/den0x1/den0x3*(-16.D0*mb**2*C0v2nab-8.D0*mb**2*A1c1v
     #2nab+2.D0*AAi-4.D0*B0v2nab-8.D0*A4c2v2nab+8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(5)=t0
c
c --------cf_v2_1_nab_9a
c
      t0 = CA/den0x1/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v2
     #nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(9)=t0
c
c --------cf_v2_1_nab_10a
c
      t0 = CA/den0x1/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v
     #2nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(10)=t0
c
c --------cf_v2_1_nab_11a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(11)=t0
c
c --------cf_v2_1_nab_12a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_1_nab_a(12)=t0
c
c --------cf_v2_1_nab_13a
c
      t0 = CA/den0x1/den0x3*(4.D0*mb*A1c1v2nab+4.D0*mb*A1c2v2nab)
c
      cf_v2_1_nab_a(13)=t0
c
c --------cf_v2_1_nab_14a
c
      t0 = CA/den0x1/den0x3*(-8.D0*mb*A1c1v2nab-8.D0*mb*A1c2v2nab
     #)
c
      cf_v2_1_nab_a(14)=t0
c
c --------cf_v2_1_nab_15a
c
      t0 = CA/den0x1/den0x3*(8.D0*mb*A1c1v2nab+8.D0*mb*A1c2v2nab)
c
      cf_v2_1_nab_a(15)=t0
c
c ---------------------- coefficients for v2_2_nab_v
c
c --------cf_v2_2_nab_1v
c
      t0 = CV/den0x2/den0x3*(8.D0*mb**3*A1c1v2nab-16.D0*mb**3*A2c1
     #v2nab+4.D0*mb**3*A1c2v2nab-8.D0*mb**3*A3c2v2nab-4.D0*q1Dpb*mb*A1c1
     #v2nab+16.D0*q1Dpb*mb*A2c1v2nab+8.D0*q1Dpb*mb*A3c2v2nab-12.D0*q1Dp1
     #b*mb*A1c1v2nab+16.D0*q1Dp1b*mb*A2c1v2nab-8.D0*q1Dp1b*mb*A1c2v2nab+
     #8.D0*q1Dp1b*mb*A3c2v2nab+4.D0*pbDp1b*mb*A1c1v2nab-16.D0*pbDp1b*mb*
     #A2c1v2nab-8.D0*pbDp1b*mb*A3c2v2nab)
c
      cf_v2_2_nab_v(1)=t0
c
c --------cf_v2_2_nab_2v
c
      t0 = CV/den0x2/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v2
     #nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_v(2)=t0
c
c --------cf_v2_2_nab_11v
c
      t0 = CV/den0x2/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_v(11)=t0
c
c --------cf_v2_2_nab_12v
c
      t0 = CV/den0x2/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_v(12)=t0
c
c --------cf_v2_2_nab_13v
c
      t0 = CV/den0x2/den0x3*(4.D0*mb*A1c1v2nab+4.D0*mb*A1c2v2nab)
c
      cf_v2_2_nab_v(13)=t0
c
c
c ---------------------- coefficients for v2_2_nab_a
c
c --------cf_v2_2_nab_1a
c
      t0 = CA/den0x2/den0x3*(8.D0*mb**3*A1c1v2nab-16.D0*mb**3*A2c1
     #v2nab+4.D0*mb**3*A1c2v2nab-8.D0*mb**3*A3c2v2nab-4.D0*q1Dpb*mb*A1c1
     #v2nab+16.D0*q1Dpb*mb*A2c1v2nab+8.D0*q1Dpb*mb*A3c2v2nab-12.D0*q1Dp1
     #b*mb*A1c1v2nab+16.D0*q1Dp1b*mb*A2c1v2nab-8.D0*q1Dp1b*mb*A1c2v2nab+
     #8.D0*q1Dp1b*mb*A3c2v2nab+4.D0*pbDp1b*mb*A1c1v2nab-16.D0*pbDp1b*mb*
     #A2c1v2nab-8.D0*pbDp1b*mb*A3c2v2nab)
c
      cf_v2_2_nab_a(1)=t0
c
c --------cf_v2_2_nab_2a
c
      t0 = CA/den0x2/den0x3*(16.D0*mb**2*C0v2nab+8.D0*mb**2*A1c1v2
     #nab-2.D0*AAi+4.D0*B0v2nab+8.D0*A4c2v2nab-8.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_a(2)=t0
c
c --------cf_v2_2_nab_11a
c
      t0 = CA/den0x2/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_a(11)=t0
c
c --------cf_v2_2_nab_12a
c
      t0 = CA/den0x2/den0x3*(8.D0*mb**2*C0v2nab+4.D0*mb**2*A1c1v2
     #nab-AAi+2.D0*B0v2nab+4.D0*A4c2v2nab-4.D0*pbDp1b*A1c1v2nab)
c
      cf_v2_2_nab_a(12)=t0
c
c --------cf_v2_2_nab_13a
c
      t0 = CA/den0x2/den0x3*(4.D0*mb*A1c1v2nab+4.D0*mb*A1c2v2nab)
c
      cf_v2_2_nab_a(13)=t0
c
      return
      end
      subroutine wbb_coeff_v3(q1,q2,pb,p1b,pw,mb,cf_v3_1_v,cf_v3_1_a,
     -                    cf_v3_2_v,cf_v3_2_a)
c
      implicit none
c
      external wbb_dot_4,wbb_c0_sd
      real*8 wbb_dot_4,wbb_c0_sd
c
c external wbb_masses and momenta
c
      integer i
      real*8 mb,m1,m2,m3,q1(4),q2(4),pb(4),p1b(4),pw(4),p1(4),p2(4),
     -       p12(4),xs1,xs2,xs12
      real*8 CV,CA,den0x1,den0x2,den0x3,
     -       q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,t0
      real*8 AAi,B0v3,B1v3,C0v3,A1c1v3,A2c1v3,A1c2v3,A2c2v3,A3c2v3,
     -  A4c2v3,
     - B012,B013,B023,B112,B113,B123!,A1c1,A2c1,A1c2,A2c2,A3c2,A4c2
      complex * 16 B012d,B112d,B2012d,B013d,B113d,B2013d,B023d,B123d,
     - B2023d
      real*8 cf_v3_1_v(75),cf_v3_1_a(75),cf_v3_2_v(75),cf_v3_2_a(75)
      real * 8 mur,muf,s,scalemur,scalemuf
      common/wbb_scale/mur,muf,s,scalemur,scalemuf
c
c debug parameter
c
      logical*4 ldebugv3
      parameter(ldebugv3=.false.)
c      parameter(ldebugv3=.true.)
c checks
c
      if(ldebugv3) then
      print*,' ------  Debugging coeff_v3_1, coeff_v3_2  ------ '
      print*,'q1x = ',q1(1),'q1y = ',q1(2),'q1z = ',q1(3),'E1 = ',q1(4)
      print*,'q2x = ',q2(1),'q2y = ',q2(2),'q2z = ',q2(3),'E2 = ',q2(4)
      print*,'pbx = ',pb(1),'pby = ',pb(2),'ptz = ',pb(3),'Eb = ',pb(4)
      print*,'p1bx = ',p1b(1),'p1by = ',p1b(2),'p1tz = ',p1b(3),
     -       'E1b = ',p1b(4)
      print*,'             '
      endif
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
      AAi=1d0
c
c --------------- integrals for V3_1 ---------------------------------
c in the notation used for V_W
c         p1(i)=q1(i)
c         p2(i)=q1(i)-pw(i)
c         q(i)=p2(i)-p1(i)=-pw(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=q1(i)
         p2(i)=-pw(i)
         p12(i)=p1(i)+p2(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
c
      call wbb_f3pf_2m(xs2,xs12,C0v3)
      B012=0d0
      B112=0d0
      call wbb_bfunc(dabs(xs12),m1,m3,B013d,B113d,B2013d)
      B013=dreal(B013d)-dlog(mur**2/mb**2)
      B113=dreal(B113d)+1d0/2d0*dlog(mur**2/mb**2)
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v3=B023
      B1v3=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v3,B012,B013,B023,A1c1v3,A2c1v3)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v3,B012,B013,B023,B112,B113,B123,
     -            A4c2v3,A1c2v3,A2c2v3,A3c2v3)
c
      do i=1,75
         cf_v3_1_v(i)=0d0
         cf_v3_1_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v3_1_v
c
c --------cf_v3_1_1v
c
      cf_v3_1_v(1)=0d0
c
c --------cf_v3_1_2v
c
      cf_v3_1_v(2)=0d0
c
c --------cf_v3_1_3v
c
      t0 = CV/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_v(3)=t0
c
c --------cf_v3_1_4v
c
      t0 = CV/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_v(4)=t0
c
c --------cf_v3_1_5v
c
      t0 = CV/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_v(5)=t0
c
c --------cf_v3_1_6v
c
      cf_v3_1_v(6)=0d0
c
c --------cf_v3_1_7v
c
      t0 = CV/den0x1/den0x3*(-8*C0v3*mb**2+8*C0v3*q2Dpb+8*C0v3*q2Dp1b-8*
     #C0v3*pbDp1b-8*A1c1v3*mb**2+8*A1c1v3*q2Dpb+8*A1c1v3*q2Dp1b-8*A1c1v3
     #*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q2Dpb-8*A2c2v3*q2Dp1b+8*A2c2v3*pbD
     #p1b-8*A3c2v3*mb**2+8*A3c2v3*q2Dpb+8*A3c2v3*q2Dp1b-8*A3c2v3*pbDp1b)
c
      cf_v3_1_v(7)=t0
c
c --------cf_v3_1_8v
c
      t0 = CV/den0x1/den0x3*(8*A2c1v3*mb**2-8*A2c1v3*q2Dpb-8*A2c1v3*q2Dp
     #1b+8*A2c1v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q2Dpb-8*A2c2v3*q2Dp1b+8
     #*A2c2v3*pbDp1b)
c
      cf_v3_1_v(8)=t0
c
c --------cf_v3_1_9v
c
      t0 = CV/den0x1/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q1Dpb+8*C
     #0v3*q1Dp1b-8*A1c1v3*q1Dq2+8*A1c1v3*q1Dpb+8*A1c1v3*q1Dp1b-8*A2c2v3*
     #mb**2+8*A2c2v3*q2Dpb+8*A2c2v3*q2Dp1b-8*A2c2v3*pbDp1b-8*A4c2v3)
c
      cf_v3_1_v(9)=t0
c
c --------cf_v3_1_10v
c
      t0 = CV/den0x1/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q1Dpb+8*C
     #0v3*q1Dp1b-8*A1c1v3*q1Dq2+8*A1c1v3*q1Dpb+8*A1c1v3*q1Dp1b-8*A2c2v3*
     #mb**2+8*A2c2v3*q2Dpb+8*A2c2v3*q2Dp1b-8*A2c2v3*pbDp1b-8*A4c2v3)
c
      cf_v3_1_v(10)=t0
c
c --------cf_v3_1_11v
c
      t0 = CV/den0x1/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q1Dpb+4*C0v
     #3*q1Dp1b-4*A1c1v3*q1Dq2+4*A1c1v3*q1Dpb+4*A1c1v3*q1Dp1b+4*A2c1v3*mb
     #**2-4*A2c1v3*q2Dpb-4*A2c1v3*q2Dp1b+4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_1_v(11)=t0
c
c --------cf_v3_1_12v
c
      t0 = CV/den0x1/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q1Dpb+4*C0v
     #3*q1Dp1b-4*A1c1v3*q1Dq2+4*A1c1v3*q1Dpb+4*A1c1v3*q1Dp1b+4*A2c1v3*mb
     #**2-4*A2c1v3*q2Dpb-4*A2c1v3*q2Dp1b+4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_1_v(12)=t0
c
c --------cf_v3_1_13v
c
      cf_v3_1_v(13)=0d0
c
c --------cf_v3_1_14v
c
      cf_v3_1_v(14)=0d0
c
c --------cf_v3_1_15v
c
      cf_v3_1_v(15)=0d0
c
c --------cf_v3_1_16v
c
      cf_v3_1_v(16)=0d0
c
c --------cf_v3_1_17v
c
      cf_v3_1_v(17)=0d0
c
c --------cf_v3_1_18v
c
      cf_v3_1_v(18)=0d0
c
c --------cf_v3_1_19v
c
      cf_v3_1_v(19)=0d0
c
c --------cf_v3_1_20v
c
      cf_v3_1_v(20)=0d0
c
c --------cf_v3_1_21v
c
      cf_v3_1_v(21)=0d0
c
c --------cf_v3_1_22v
c
      cf_v3_1_v(22)=0d0
c
c --------cf_v3_1_23v
c
      cf_v3_1_v(23)=0d0
c
c --------cf_v3_1_24v
c
      cf_v3_1_v(24)=0d0
c
c --------cf_v3_1_25v
c
      cf_v3_1_v(25)=0d0
c
c --------cf_v3_1_26v
c
      cf_v3_1_v(26)=0d0
c
c --------cf_v3_1_27v
c
      cf_v3_1_v(27)=0d0
c
c --------cf_v3_1_28v
c
      cf_v3_1_v(28)=0d0
c
c --------cf_v3_1_29v
c
      cf_v3_1_v(29)=0d0
c
c --------cf_v3_1_30v
c
      cf_v3_1_v(30)=0d0
c
c --------cf_v3_1_31v
c
      cf_v3_1_v(31)=0d0
c
c
c ---------------------- coefficients for v3_1_a
c
c --------cf_v3_1_1a
c
      cf_v3_1_a(1)=0d0
c
c --------cf_v3_1_2a
c
      cf_v3_1_a(2)=0d0
c
c --------cf_v3_1_3a
c
      t0 = CA/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_a(3)=t0
c
c --------cf_v3_1_4a
c
      t0 = CA/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_a(4)=t0
c
c --------cf_v3_1_5a
c
      t0 = CA/den0x1/den0x3*(2*AAi-4*B0v3+8*C0v3*q1Dq2-8*C0v3*q1Dpb-8*C0
     #v3*q1Dp1b+8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b-8*A2c1v3*m
     #b**2+8*A2c1v3*q2Dpb+8*A2c1v3*q2Dp1b-8*A2c1v3*pbDp1b+8*A4c2v3)
c
      cf_v3_1_a(5)=t0
c
c --------cf_v3_1_6a
c
      cf_v3_1_a(6)=0d0
c
c --------cf_v3_1_7a
c
      t0 = CA/den0x1/den0x3*(-8*C0v3*mb**2+8*C0v3*q2Dpb+8*C0v3*q2Dp1b-8*
     #C0v3*pbDp1b-8*A1c1v3*mb**2+8*A1c1v3*q2Dpb+8*A1c1v3*q2Dp1b-8*A1c1v3
     #*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q2Dpb-8*A2c2v3*q2Dp1b+8*A2c2v3*pbD
     #p1b-8*A3c2v3*mb**2+8*A3c2v3*q2Dpb+8*A3c2v3*q2Dp1b-8*A3c2v3*pbDp1b)
c
      cf_v3_1_a(7)=t0
c
c --------cf_v3_1_8a
c
      t0 = CA/den0x1/den0x3*(8*A2c1v3*mb**2-8*A2c1v3*q2Dpb-8*A2c1v3*q2Dp
     #1b+8*A2c1v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q2Dpb-8*A2c2v3*q2Dp1b+8
     #*A2c2v3*pbDp1b)
c
      cf_v3_1_a(8)=t0
c
c --------cf_v3_1_9a
c
      t0 = CA/den0x1/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q1Dpb+8*C
     #0v3*q1Dp1b-8*A1c1v3*q1Dq2+8*A1c1v3*q1Dpb+8*A1c1v3*q1Dp1b-8*A2c2v3*
     #mb**2+8*A2c2v3*q2Dpb+8*A2c2v3*q2Dp1b-8*A2c2v3*pbDp1b-8*A4c2v3)
c
      cf_v3_1_a(9)=t0
c
c --------cf_v3_1_10a
c
      t0 = CA/den0x1/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q1Dpb+8*C
     #0v3*q1Dp1b-8*A1c1v3*q1Dq2+8*A1c1v3*q1Dpb+8*A1c1v3*q1Dp1b-8*A2c2v3*
     #mb**2+8*A2c2v3*q2Dpb+8*A2c2v3*q2Dp1b-8*A2c2v3*pbDp1b-8*A4c2v3)
c
      cf_v3_1_a(10)=t0
c
c --------cf_v3_1_11a
c
      t0 = CA/den0x1/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q1Dpb+4*C0v
     #3*q1Dp1b-4*A1c1v3*q1Dq2+4*A1c1v3*q1Dpb+4*A1c1v3*q1Dp1b+4*A2c1v3*mb
     #**2-4*A2c1v3*q2Dpb-4*A2c1v3*q2Dp1b+4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_1_a(11)=t0
c
c --------cf_v3_1_12a
c
      t0 = CA/den0x1/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q1Dpb+4*C0v
     #3*q1Dp1b-4*A1c1v3*q1Dq2+4*A1c1v3*q1Dpb+4*A1c1v3*q1Dp1b+4*A2c1v3*mb
     #**2-4*A2c1v3*q2Dpb-4*A2c1v3*q2Dp1b+4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_1_a(12)=t0
c
c --------cf_v3_1_13a
c
      cf_v3_1_a(13)=0d0
c
c --------cf_v3_1_14a
c
      cf_v3_1_a(14)=0d0
c
c --------cf_v3_1_15a
c
      cf_v3_1_a(15)=0d0
c
c --------cf_v3_1_16a
c
      cf_v3_1_a(16)=0d0
c
c --------cf_v3_1_17a
c
      cf_v3_1_a(17)=0d0
c
c --------cf_v3_1_18a
c
      cf_v3_1_a(18)=0d0
c
c --------cf_v3_1_19a
c
      cf_v3_1_a(19)=0d0
c
c --------cf_v3_1_20a
c
      cf_v3_1_a(20)=0d0
c
c --------cf_v3_1_21a
c
      cf_v3_1_a(21)=0d0
c
c --------cf_v3_1_22a
c
      cf_v3_1_a(22)=0d0
c
c --------cf_v3_1_23a
c
      cf_v3_1_a(23)=0d0
c
c --------cf_v3_1_24a
c
      cf_v3_1_a(24)=0d0
c
c --------cf_v3_1_25a
c
      cf_v3_1_a(25)=0d0
c
c --------cf_v3_1_26a
c
      cf_v3_1_a(26)=0d0
c
c --------cf_v3_1_27a
c
      cf_v3_1_a(27)=0d0
c
c --------cf_v3_1_28a
c
      cf_v3_1_a(28)=0d0
c
c --------cf_v3_1_29a
c
      cf_v3_1_a(29)=0d0
c
c --------cf_v3_1_30a
c
      cf_v3_1_a(30)=0d0
c
c --------cf_v3_1_31a
c
      cf_v3_1_a(31)=0d0
c
c
c --------------- integrals for V3_2 --------------------------------
c in the notation used for V_W
c         p1(i)=-q2(i)+pw(i)
c         p2(i)=-q2(i)
c         q(i)=p2(i)-p1(i)=-pw(i)
c in the notation used for the integrals
      do i=1,4
         p1(i)=-q2(i)+pw(i)
         p2(i)=-pw(i)
         p12(i)=p1(i)+p2(i)
      enddo
      m1=0d0
      m2=0d0
      m3=0d0
      xs1=wbb_dot_4(p1,p1)
      xs2=wbb_dot_4(p2,p2)
      xs12=wbb_dot_4(p12,p12)
c
      call wbb_f3pf_2m(xs1,xs2,C0v3)
      call wbb_bfunc(dabs(xs1),m1,m2,B012d,B112d,B2012d)
      B012=dreal(B012d)-dlog(mur**2/mb**2)
      B112=dreal(B112d)+1d0/2d0*dlog(mur**2/mb**2)
      B013=0d0
      B113=0d0
      call wbb_bfunc(dabs(xs2),m2,m3,B023d,B123d,B2023d)
      B023=dreal(B023d)-dlog(mur**2/mb**2)
      B123=dreal(B123d)+1d0/2d0*dlog(mur**2/mb**2)
      B0v3=B023
      B1v3=B123
      call wbb_c1_new(p1,p2,m1,m2,m3,C0v3,B012,B013,B023,A1c1v3,A2c1v3)
      call wbb_c2_new(p1,p2,m1,m2,m3,C0v3,B012,B013,B023,B112,B113,B123,
     -            A4c2v3,A1c2v3,A2c2v3,A3c2v3)
c
      do i=1,75
         cf_v3_2_v(i)=0d0
         cf_v3_2_a(i)=0d0
      enddo
c
c ---------------------- coefficients for v3_2_v
c
c --------cf_v3_2_1v
c
      cf_v3_2_v(1)=0d0
c
c --------cf_v3_2_2v
c
      t0 = CV/den0x2/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q2Dpb+8*C
     #0v3*q2Dp1b+8*A1c1v3*mb**2-8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q
     #1Dp1b+8*A1c1v3*q2Dpb+8*A1c1v3*q2Dp1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**
     #2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8*A2c1v3*pbDp1b-8*A4c2v3)
c
      cf_v3_2_v(2)=t0
c
c --------cf_v3_2_3v
c
      cf_v3_2_v(3)=0d0
c
c --------cf_v3_2_4v
c
      cf_v3_2_v(4)=0d0
c
c --------cf_v3_2_5v
c
      cf_v3_2_v(5)=0d0
c
c --------cf_v3_2_6v
c
      cf_v3_2_v(6)=0d0
c
c --------cf_v3_2_7v
c
      t0 = CV/den0x2/den0x3*(-8*A1c1v3*mb**2+8*A1c1v3*q1Dpb+8*A1c1v3*q1D
     #p1b-8*A1c1v3*pbDp1b+8*A2c1v3*mb**2-8*A2c1v3*q1Dpb-8*A2c1v3*q1Dp1b+
     #8*A2c1v3*pbDp1b-8*A1c2v3*mb**2+8*A1c2v3*q1Dpb+8*A1c2v3*q1Dp1b-8*A1
     #c2v3*pbDp1b-8*A2c2v3*mb**2+8*A2c2v3*q1Dpb+8*A2c2v3*q1Dp1b-8*A2c2v3
     #*pbDp1b+16*A3c2v3*mb**2-16*A3c2v3*q1Dpb-16*A3c2v3*q1Dp1b+16*A3c2v3
     #*pbDp1b)
c
      cf_v3_2_v(7)=t0
c
c --------cf_v3_2_8v
c
      t0 = CV/den0x2/den0x3*(8*C0v3*mb**2-8*C0v3*q1Dpb-8*C0v3*q1Dp1b+8*C
     #0v3*pbDp1b+8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b+8*A1c1v3*
     #pbDp1b-8*A2c2v3*mb**2+8*A2c2v3*q1Dpb+8*A2c2v3*q1Dp1b-8*A2c2v3*pbDp
     #1b+8*A3c2v3*mb**2-8*A3c2v3*q1Dpb-8*A3c2v3*q1Dp1b+8*A3c2v3*pbDp1b)
c
      cf_v3_2_v(8)=t0
c
c --------cf_v3_2_9v
c
      t0 = CV/den0x2/den0x3*(8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp
     #1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8
     #*A2c1v3*pbDp1b+8*A1c2v3*mb**2-8*A1c2v3*q1Dpb-8*A1c2v3*q1Dp1b+8*A1c
     #2v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q1Dpb-8*A2c2v3*q1Dp1b+8*A2c2v3*
     #pbDp1b-16*A3c2v3*mb**2+16*A3c2v3*q1Dpb+16*A3c2v3*q1Dp1b-16*A3c2v3*
     #pbDp1b)
c
      cf_v3_2_v(9)=t0
c
c --------cf_v3_2_10v
c
      t0 = CV/den0x2/den0x3*(8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp
     #1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8
     #*A2c1v3*pbDp1b+8*A1c2v3*mb**2-8*A1c2v3*q1Dpb-8*A1c2v3*q1Dp1b+8*A1c
     #2v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q1Dpb-8*A2c2v3*q1Dp1b+8*A2c2v3*
     #pbDp1b-16*A3c2v3*mb**2+16*A3c2v3*q1Dpb+16*A3c2v3*q1Dp1b-16*A3c2v3*
     #pbDp1b)
c
      cf_v3_2_v(10)=t0
c
c --------cf_v3_2_11v
c
      t0 = CV/den0x2/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q2Dpb+4*C0v
     #3*q2Dp1b+4*A1c1v3*mb**2-4*A1c1v3*q1Dq2-4*A1c1v3*q1Dpb-4*A1c1v3*q1D
     #p1b+4*A1c1v3*q2Dpb+4*A1c1v3*q2Dp1b+4*A1c1v3*pbDp1b-4*A2c1v3*mb**2+
     #4*A2c1v3*q1Dpb+4*A2c1v3*q1Dp1b-4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_2_v(11)=t0
c
c --------cf_v3_2_12v
c
      t0 = CV/den0x2/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q2Dpb+4*C0v
     #3*q2Dp1b+4*A1c1v3*mb**2-4*A1c1v3*q1Dq2-4*A1c1v3*q1Dpb-4*A1c1v3*q1D
     #p1b+4*A1c1v3*q2Dpb+4*A1c1v3*q2Dp1b+4*A1c1v3*pbDp1b-4*A2c1v3*mb**2+
     #4*A2c1v3*q1Dpb+4*A2c1v3*q1Dp1b-4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_2_v(12)=t0
c
c --------cf_v3_2_13v
c
      cf_v3_2_v(13)=0d0
c
c --------cf_v3_2_14v
c
      cf_v3_2_v(14)=0d0
c
c --------cf_v3_2_15v
c
      cf_v3_2_v(15)=0d0
c
c --------cf_v3_2_16v
c
      cf_v3_2_v(16)=0d0
c
c --------cf_v3_2_17v
c
      cf_v3_2_v(17)=0d0
c
c --------cf_v3_2_18v
c
      cf_v3_2_v(18)=0d0
c
c --------cf_v3_2_19v
c
      cf_v3_2_v(19)=0d0
c
c --------cf_v3_2_20v
c
      cf_v3_2_v(20)=0d0
c
c --------cf_v3_2_21v
c
      cf_v3_2_v(21)=0d0
c
c --------cf_v3_2_22v
c
      cf_v3_2_v(22)=0d0
c
c --------cf_v3_2_23v
c
      cf_v3_2_v(23)=0d0
c
c --------cf_v3_2_24v
c
      cf_v3_2_v(24)=0d0
c
c --------cf_v3_2_25v
c
      cf_v3_2_v(25)=0d0
c
c --------cf_v3_2_26v
c
      cf_v3_2_v(26)=0d0
c
c --------cf_v3_2_27v
c
      cf_v3_2_v(27)=0d0
c
c --------cf_v3_2_28v
c
      cf_v3_2_v(28)=0d0
c
c --------cf_v3_2_29v
c
      cf_v3_2_v(29)=0d0
c
c --------cf_v3_2_30v
c
      cf_v3_2_v(30)=0d0
c
c --------cf_v3_2_31v
c
      cf_v3_2_v(31)=0d0
c
c
c ---------------------- coefficients for v3_2_a
c
c --------cf_v3_2_1a
c
      cf_v3_2_a(1)=0d0
c
c --------cf_v3_2_2a
c
      t0 = CA/den0x2/den0x3*(-2*AAi+4*B0v3-8*C0v3*q1Dq2+8*C0v3*q2Dpb+8*C
     #0v3*q2Dp1b+8*A1c1v3*mb**2-8*A1c1v3*q1Dq2-8*A1c1v3*q1Dpb-8*A1c1v3*q
     #1Dp1b+8*A1c1v3*q2Dpb+8*A1c1v3*q2Dp1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**
     #2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8*A2c1v3*pbDp1b-8*A4c2v3)
c
      cf_v3_2_a(2)=t0
c
c --------cf_v3_2_3a
c
      cf_v3_2_a(3)=0d0
c
c --------cf_v3_2_4a
c
      cf_v3_2_a(4)=0d0
c
c --------cf_v3_2_5a
c
      cf_v3_2_a(5)=0d0
c
c --------cf_v3_2_6a
c
      cf_v3_2_a(6)=0d0
c
c --------cf_v3_2_7a
c
      t0 = CA/den0x2/den0x3*(-8*A1c1v3*mb**2+8*A1c1v3*q1Dpb+8*A1c1v3*q1D
     #p1b-8*A1c1v3*pbDp1b+8*A2c1v3*mb**2-8*A2c1v3*q1Dpb-8*A2c1v3*q1Dp1b+
     #8*A2c1v3*pbDp1b-8*A1c2v3*mb**2+8*A1c2v3*q1Dpb+8*A1c2v3*q1Dp1b-8*A1
     #c2v3*pbDp1b-8*A2c2v3*mb**2+8*A2c2v3*q1Dpb+8*A2c2v3*q1Dp1b-8*A2c2v3
     #*pbDp1b+16*A3c2v3*mb**2-16*A3c2v3*q1Dpb-16*A3c2v3*q1Dp1b+16*A3c2v3
     #*pbDp1b)
c
      cf_v3_2_a(7)=t0
c
c --------cf_v3_2_8a
c
      t0 = CA/den0x2/den0x3*(8*C0v3*mb**2-8*C0v3*q1Dpb-8*C0v3*q1Dp1b+8*C
     #0v3*pbDp1b+8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp1b+8*A1c1v3*
     #pbDp1b-8*A2c2v3*mb**2+8*A2c2v3*q1Dpb+8*A2c2v3*q1Dp1b-8*A2c2v3*pbDp
     #1b+8*A3c2v3*mb**2-8*A3c2v3*q1Dpb-8*A3c2v3*q1Dp1b+8*A3c2v3*pbDp1b)
c
      cf_v3_2_a(8)=t0
c
c --------cf_v3_2_9a
c
      t0 = CA/den0x2/den0x3*(8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp
     #1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8
     #*A2c1v3*pbDp1b+8*A1c2v3*mb**2-8*A1c2v3*q1Dpb-8*A1c2v3*q1Dp1b+8*A1c
     #2v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q1Dpb-8*A2c2v3*q1Dp1b+8*A2c2v3*
     #pbDp1b-16*A3c2v3*mb**2+16*A3c2v3*q1Dpb+16*A3c2v3*q1Dp1b-16*A3c2v3*
     #pbDp1b)
c
      cf_v3_2_a(9)=t0
c
c --------cf_v3_2_10a
c
      t0 = CA/den0x2/den0x3*(8*A1c1v3*mb**2-8*A1c1v3*q1Dpb-8*A1c1v3*q1Dp
     #1b+8*A1c1v3*pbDp1b-8*A2c1v3*mb**2+8*A2c1v3*q1Dpb+8*A2c1v3*q1Dp1b-8
     #*A2c1v3*pbDp1b+8*A1c2v3*mb**2-8*A1c2v3*q1Dpb-8*A1c2v3*q1Dp1b+8*A1c
     #2v3*pbDp1b+8*A2c2v3*mb**2-8*A2c2v3*q1Dpb-8*A2c2v3*q1Dp1b+8*A2c2v3*
     #pbDp1b-16*A3c2v3*mb**2+16*A3c2v3*q1Dpb+16*A3c2v3*q1Dp1b-16*A3c2v3*
     #pbDp1b)
c
      cf_v3_2_a(10)=t0
c
c --------cf_v3_2_11a
c
      t0 = CA/den0x2/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q2Dpb+4*C0v
     #3*q2Dp1b+4*A1c1v3*mb**2-4*A1c1v3*q1Dq2-4*A1c1v3*q1Dpb-4*A1c1v3*q1D
     #p1b+4*A1c1v3*q2Dpb+4*A1c1v3*q2Dp1b+4*A1c1v3*pbDp1b-4*A2c1v3*mb**2+
     #4*A2c1v3*q1Dpb+4*A2c1v3*q1Dp1b-4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_2_a(11)=t0
c
c --------cf_v3_2_12a
c
      t0 = CA/den0x2/den0x3*(-AAi+2*B0v3-4*C0v3*q1Dq2+4*C0v3*q2Dpb+4*C0v
     #3*q2Dp1b+4*A1c1v3*mb**2-4*A1c1v3*q1Dq2-4*A1c1v3*q1Dpb-4*A1c1v3*q1D
     #p1b+4*A1c1v3*q2Dpb+4*A1c1v3*q2Dp1b+4*A1c1v3*pbDp1b-4*A2c1v3*mb**2+
     #4*A2c1v3*q1Dpb+4*A2c1v3*q1Dp1b-4*A2c1v3*pbDp1b-4*A4c2v3)
c
      cf_v3_2_a(12)=t0
c
c --------cf_v3_2_13a
c
      cf_v3_2_a(13)=0d0
c
c --------cf_v3_2_14a
c
      cf_v3_2_a(14)=0d0
c
c --------cf_v3_2_15a
c
      cf_v3_2_a(15)=0d0
c
c --------cf_v3_2_16a
c
      cf_v3_2_a(16)=0d0
c
c --------cf_v3_2_17a
c
      cf_v3_2_a(17)=0d0
c
c --------cf_v3_2_18a
c
      cf_v3_2_a(18)=0d0
c
c --------cf_v3_2_19a
c
      cf_v3_2_a(19)=0d0
c
c --------cf_v3_2_20a
c
      cf_v3_2_a(20)=0d0
c
c --------cf_v3_2_21a
c
      cf_v3_2_a(21)=0d0
c
c --------cf_v3_2_22a
c
      cf_v3_2_a(22)=0d0
c
c --------cf_v3_2_23a
c
      cf_v3_2_a(23)=0d0
c
c --------cf_v3_2_24a
c
      cf_v3_2_a(24)=0d0
c
c --------cf_v3_2_25a
c
      cf_v3_2_a(25)=0d0
c
c --------cf_v3_2_26a
c
      cf_v3_2_a(26)=0d0
c
c --------cf_v3_2_27a
c
      cf_v3_2_a(27)=0d0
c
c --------cf_v3_2_28a
c
      cf_v3_2_a(28)=0d0
c
c --------cf_v3_2_29a
c
      cf_v3_2_a(29)=0d0
c
c --------cf_v3_2_30a
c
      cf_v3_2_a(30)=0d0
c
c --------cf_v3_2_31a
c
      cf_v3_2_a(31)=0d0
c
      return
      end
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      real * 8 function wbb_c0_sd(cq1,cq2,cq3,mm1,mm2,mm3)
***********************************************************************
      implicit real*8(m,q,c)
      implicit complex*16(z)
      complex*16 y(3,3),wbb_spence_sd,cywur
      real*8 cj(3)
*
      j = 3
      cj(1) = -1d0
      cj(2) = 1d0
      cj(3) = -1d0
      m1 = mm1*mm1
      m2 = mm2*mm2
      m3 = mm3*mm3
      q1 = cq1
      q2 = cq2
      q3 = cq3
      if (q2 .eq. 0d0) then
         q2 = cq1
         q1 = cq2
         m3 = mm1*mm1
         m1 = mm3*mm3
      end if
      if (q1 .eq. 0d0) then
         j = 2
      end if
      c1 = 1d0+(q1-q3)/q2
      if (c1*c1-4d0*q1/q2 .lt. 0d0) then
         write(6,*) ' neg. determinante !'
      end if
      cwurz = dsqrt(c1*c1-4d0*q1/q2)
      calphs = (c1-cwurz)/2d0
      c0 = 0d0
      ca = q1
      cb = q2
      cc = q3-q2-q1
      cd = m2-m1-q1
      ce = q1-q3+m3-m2
      cf = m1
      cnum = -(cd+ce*calphs)
      cdenom = cc+2d0*calphs*cb
      y(1,3) = dcmplx((cnum-2d0*ca-cc*calphs)/cdenom,0d0)
      cy0 = -cc-ce
      cywur = cdsqrt(dcmplx(cy0*cy0-4d0*cb*(ca+cd+cf),1d-20))
      y(1,1) = (cy0+cywur)/2d0/cb
      y(1,2) = (cy0-cywur)/2d0/cb
      cy0 = -ce-cd
      cywur = cdsqrt(dcmplx(cy0*cy0-4d0*cf*(ca+cb+cc),1d-20))
      y(2,3) = dcmplx(cnum/cdenom/(1d0-calphs),0d0)
      y(2,1) = (cy0+cywur)/2d0/(ca+cb+cc)
      y(2,2) = (cy0-cywur)/2d0/(ca+cb+cc)
      cywur = cdsqrt(dcmplx(cd*cd-4d0*ca*cf,1d-20))
      if (j .eq. 3) then
         y(3,3) = dcmplx(-cnum/calphs/cdenom,0d0)
         y(3,1) = (-cd+cywur)/2d0/ca
         y(3,2) = (-cd-cywur)/2d0/ca
      end if
      do 100 i=1,j
         c0 = c0+cj(i)*dreal(wbb_spence_sd(y(i,3)/(y(i,3)-y(i,1)))
     1        -wbb_spence_sd((y(i,3)-1d0)/(y(i,3)-y(i,1)))
     2        +wbb_spence_sd(y(i,3)/(y(i,3)-y(i,2)))
     3        -wbb_spence_sd((y(i,3)-1d0)/(y(i,3)-y(i,2))))
 100  continue
      wbb_c0_sd = c0/cdenom
      return
      end
***********************************************************************
      complex*16 function wbb_spence_sd(xx)
c  the dilogarithm for general complex argument.
c  not allowed: real(xx) gt 1 with imag(xx)=0.
      implicit real*8(a-z)
      integer n
      complex*16 xx,x,z,d,p,cdlog
      dimension a(19)
      pi=4d0*datan(1d0)
      x=xx
      xr=dreal(x)
      xi=dimag(x)
      if(xr.ne.1.) goto 111
      if(xi.eq.0.) goto 20
111   continue
c    projection into the convergence radius
      vor=1.d0
      p=dcmplx(0.d0,0.d0)
      r=dreal(x)
      if (r .le. 0.5d0) goto 1
      p=pi*pi/6.d0- cdlog(x)*cdlog(1.d0-x)
      vor=-1.d0
      x=1.d0-x
    1 continue
      b=cdabs(x)
      if (b .lt. 1.d0) goto 2
      p=p - (pi*pi/6.d0+ cdlog(-x)*cdlog(-x)/2.d0)*vor
      vor=vor*(-1.d0)
      x=1.d0/x
    2 continue
c    calculation of the wbb_spence_sd function
      a(1)=1.d0
      a(2)=-0.5d0
      a(3)=1.d0/6.d0
      a(5)=-1.d0/30.d0
      a(7)=1.d0/42.d0
      a(9)=-1.d0/30.d0
      a(11)=5.d0/66.d0
      a(13)=-691.d0/2730.d0
      a(15)=7.d0/6.d0
      a(17)=-3617.d0/510.d0
      a(19)=43867.d0/798.d0
      do 5 n=2,9,1
      a(2*n)=0.d0
    5 continue
      z=(-1.d0)*cdlog(1.d0-x)
      d=dcmplx(a(19),0.d0)
      do 10 n=1,18,1
      d=d*z/(20.d0-n) + a(19-n)
   10 continue
      d=d*z
      wbb_spence_sd=d*vor + p
      goto 30
   20 continue
      wbb_spence_sd=pi*pi/6.d0
   30 continue
      return
      end
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        FUNCTION wbb_CSPENS(Z)                                              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        COMPLEX*16 wbb_CSPENS,W,SUM,Z,U                                     
        REAL*8 RZ,AZ,A1                                                
        REAL*8 B(9)
        data B/                                                   
     1   0.1666666666666666666666666667D0,                             
     2  -0.0333333333333333333333333333D0,                             
     3   0.0238095238095238095238095238D0,                             
     4  -0.0333333333333333333333333333D0,                             
     5   0.0757575757575757575757575758D0,                             
     6  -0.2531135531135531135531135531D0,                             
     7   1.1666666666666666666666666667D0,                             
     8  -7.09215686274509804D0         ,                               
     9  54.97117794486215539D0         /                               
C                                                                      
      Z =Z*DCMPLX(1D0)                                                 
      RZ=DREAL(Z)                                                      
      AZ=CDABS(Z)                                                      
      A1=CDABS(1D0-Z)                                                  
C     IF((SNGL(RZ) .EQ. 0.0) .AND. (SNGL(DIMAG(Z)) .EQ. 0.0)) THEN     
      IF(AZ .LT. 1D-20) THEN                                           
        wbb_CSPENS=-CDLOG(1D0-Z)                                            
        RETURN                                                         
      END IF                                                           
c      IF((SNGL(RZ) .EQ. 1.0) .AND. (SNGL(DIMAG(Z)) .EQ. 0.0)) THEN     
       IF( (ABS(RZ-1D0).LT.1D-18) .AND. (ABS(DIMAG(Z)).LT.1D-18) ) THEN     
        wbb_CSPENS=1.64493406684822643D0                                    
        RETURN                                                         
      END IF                                                           
      IF(RZ.GT.5D-1) GOTO 20                                           
      IF(AZ.GT.1D0) GOTO 10                                            
      W=-CDLOG(1D0-Z)                                                  
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 2                                     
      DO 1 K=1,9                                                       
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 2                            
      SUM=SUM+U*B(K)                                                   
 1    CONTINUE                                                         
 2    wbb_CSPENS=SUM                                                        
      RETURN                                                           
10    W=-CDLOG(1D0-1D0/Z)                                              
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 12                                    
                                                                       
      DO 11 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(B(K)*U/SUM).LT.1D-20) GOTO 12                           
      SUM=SUM+U*B(K)                                                   
11    CONTINUE                                                         
12    wbb_CSPENS=-SUM-1.64493406684822643D0-.5D0*CDLOG(-Z)**2               
      RETURN                                                           
20    IF(A1.GT.1D0) GOTO 30                                            
      W=-CDLOG(Z)                                                      
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 22                                    
      DO 21 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 22                           
      SUM=SUM+U*B(K)                                                   
21    CONTINUE                                                         
22    wbb_CSPENS=-SUM+1.64493406684822643D0-CDLOG(Z)*CDLOG(1D0-Z)           
      RETURN                                                           
30    W=CDLOG(1D0-1D0/Z)                                               
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 32                                    
      DO 31 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 32                           
      SUM=SUM+U*B(K)                                                   
31    CONTINUE                                                         
32    wbb_CSPENS=SUM+3.28986813369645287D0                                  
     *               +.5D0*CDLOG(Z-1D0)**2-CDLOG(Z)*CDLOG(1D0-Z)       
50    CONTINUE                                                         
        END                                                            
***********************************************************************
        FUNCTION wbb_ETASS(C1,C2)                                            
***********************************************************************
        IMPLICIT   LOGICAL(A-Z)                                        
        COMPLEX*16 wbb_ETASS,C1,C2                                           
        REAL*8     PI,IM1,IM2,IM12                                     
                                                                       
        PI     = 4D0*DATAN(1D0)                                        
        IM1    = DIMAG(C1)                                             
        IM2    = DIMAG(C2)                                             
        IM12   = DIMAG(C1*C2)                                          

	if (((IM1.eq.0d0).and.(DREAL(C1).lt.0d0)).or.
     &	    ((IM2.eq.0d0).and.(DREAL(C2).lt.0d0)).or.
     &	    ((IM12.eq.0d0).and.(DREAL(C1*C2).lt.0d0))) then
	  write(*,*) 'wbb_ETASS function on cut !!!'
	  write(*,*) 'C1    = ',C1
	  write(*,*) 'C2    = ',C2
	  write(*,*) 'C1*C2 = ',C1*C2
	  stop
	endif
                                                                       
        IF(IM1.LT.0D0.AND.IM2.LT.0D0.AND.IM12.GT.0D0) THEN             
            wbb_ETASS = DCMPLX(0D0,2D0*PI)                                   
        ELSE IF (IM1.GT.0D0.AND.IM2.GT.0D0.AND.IM12.LT.0D0) THEN       
            wbb_ETASS = DCMPLX(0D0,-2D0*PI)                                  
        ELSE                                                           
            wbb_ETASS = DCMPLX(0D0)                                          
        END IF                                                         
        END                                                            

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        FUNCTION wbb_CSPEN(Z)                                              
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
        COMPLEX*16 wbb_CSPEN,W,SUM,Z,U                                     
        REAL*8 RZ,AZ,A1                                                
        REAL*8 B(9)
        data B/                                                   
     1   0.1666666666666666666666666667D0,                             
     2  -0.0333333333333333333333333333D0,                             
     3   0.0238095238095238095238095238D0,                             
     4  -0.0333333333333333333333333333D0,                             
     5   0.0757575757575757575757575758D0,                             
     6  -0.2531135531135531135531135531D0,                             
     7   1.1666666666666666666666666667D0,                             
     8  -7.09215686274509804D0         ,                               
     9  54.97117794486215539D0         /                               
C                                                                      
      Z =Z*DCMPLX(1D0)                                                 
      RZ=DREAL(Z)                                                      
      AZ=CDABS(Z)                                                      
      A1=CDABS(1D0-Z)                                                  
C     IF((SNGL(RZ) .EQ. 0.0) .AND. (SNGL(DIMAG(Z)) .EQ. 0.0)) THEN     
      IF(AZ .LT. 1D-20) THEN                                           
        wbb_CSPEN=-CDLOG(1D0-Z)                                            
        RETURN                                                         
      END IF                                                           
c      IF((SNGL(RZ) .EQ. 1.0) .AND. (SNGL(DIMAG(Z)) .EQ. 0.0)) THEN     
       IF( (ABS(RZ-1D0).LT.1D-18) .AND. (ABS(DIMAG(Z)).LT.1D-18) ) THEN     
        wbb_CSPEN=1.64493406684822643D0                                    
        RETURN                                                         
      END IF                                                           
      IF(RZ.GT.5D-1) GOTO 20                                           
      IF(AZ.GT.1D0) GOTO 10                                            
      W=-CDLOG(1D0-Z)                                                  
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 2                                     
      DO 1 K=1,9                                                       
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 2                            
      SUM=SUM+U*B(K)                                                   
 1    CONTINUE                                                         
 2    wbb_CSPEN=SUM                                                        
      RETURN                                                           
10    W=-CDLOG(1D0-1D0/Z)                                              
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 12                                    
                                                                       
      DO 11 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(B(K)*U/SUM).LT.1D-20) GOTO 12                           
      SUM=SUM+U*B(K)                                                   
11    CONTINUE                                                         
12    wbb_CSPEN=-SUM-1.64493406684822643D0-.5D0*CDLOG(-Z)**2               
      RETURN                                                           
20    IF(A1.GT.1D0) GOTO 30                                            
      W=-CDLOG(Z)                                                      
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 22                                    
      DO 21 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 22                           
      SUM=SUM+U*B(K)                                                   
21    CONTINUE                                                         
22    wbb_CSPEN=-SUM+1.64493406684822643D0-CDLOG(Z)*CDLOG(1D0-Z)           
      RETURN                                                           
30    W=CDLOG(1D0-1D0/Z)                                               
      SUM=W-0.25D0*W*W                                                 
      U=W                                                              
      IF(CDABS(U).LT.1D-10) GOTO 32                                    
      DO 31 K=1,9                                                      
      U=U*W*W/DFLOAT(2*K*(2*K+1))                                      
      IF(CDABS(U*B(K)/SUM).LT.1D-20) GOTO 32                           
      SUM=SUM+U*B(K)                                                   
31    CONTINUE                                                         
32    wbb_CSPEN=SUM+3.28986813369645287D0                                  
     *               +.5D0*CDLOG(Z-1D0)**2-CDLOG(Z)*CDLOG(1D0-Z)       
50    CONTINUE                                                         
        END                                                            
c
      subroutine wbb_c1_new(p1,p2,m1,m2,m3,C0,B012,B013,B023,C1_1,C1_2)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer i
      real*8 m1,m2,m3,p1(4),p2(4),p1p2(4),f1,f2
      real*8 t0,p1Dp1,p1Dp2,p2Dp2,wbb_gramdet
      real*8 B012,B013,B023,C0!,A1c1,A2c1
      real*8 I1,I2,C1_1,C1_2
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p2dp2=wbb_dot(p2,p2)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
c     
      do i=1,4
         p1p2(i)=p1(i)+p2(i)
      enddo
c
      I1=1d0/2d0*(B013-B023+f1*C0)
      I2=1d0/2d0*(B012-B013+f2*C0)
c Gram Determinant
      wbb_gramdet = p1dp1*p2dp2-p1dp2**2
c C1_1
      t0 = p2dp2*I1-p1dp2*I2
      C1_1=t0/wbb_gramdet
c C1_2
      t0 = -p1dp2*I1+p1dp1*I2
      C1_2=t0/wbb_gramdet
c
      return
      end
c
c
c
      subroutine wbb_c2_new(p1,p2,m1,m2,m3,C0,B012,B013,B023,
     -                  B112,B113,B123,C2_0,C2_11,C2_22,C2_12)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer i
      real*8 m1,m2,m3,p1(4),p2(4),p1p2(4),f1,f2
      real*8 t0,p1Dp1,p1Dp2,p2Dp2,wbb_gramdet
      real*8 B012,B013,B023,B112,B113,B123,C0,B023_uv
c      real*8 A1c1,A2c1,A1c2,A2c2,A3c2,A4c2
      real*8 I11,I12,I21,I22,C1_1,C1_2,C2_0,C2_11,C2_22,C2_12
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p2dp2=wbb_dot(p2,p2)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
c     
      do i=1,4
         p1p2(i)=p1(i)+p2(i)
      enddo
c
      call wbb_c1_new(p1,p2,m1,m2,m3,C0,B012,B013,B023,C1_1,C1_2)
c
      I11=1d0/2d0*(B113+B023+f1*C1_1)
      I12=1d0/2d0*(B113-B123+f1*C1_2)
      I21=1d0/2d0*(B112-B113+f2*C1_1)
      I22=1d0/2d0*(-B113+f2*C1_2)
c
      B023_uv=1d0
      C2_0=1d0/2d0*(1d0/2d0*(B023+B023_uv)+m1**2*C0-
     -              1d0/2d0*(f1*C1_1+f2*C1_2))
      I11=I11-C2_0
      I22=I22-C2_0
c
c Gram Determinant
      wbb_gramdet = p1Dp1*p2Dp2-p1Dp2**2
c C2_11
      t0 = p2Dp2*I11-p1Dp2*I21
      C2_11=t0/wbb_gramdet
c C2_22
      t0 = -p1Dp2*I12+p1Dp1*I22
      C2_22=t0/wbb_gramdet
c C2_12
      t0 = p2Dp2*I12-p1Dp2*I22
      C2_12=t0/wbb_gramdet
c
      return
      end
      real * 8 function wbb_pent1c(q1,q2,pb,p1b,pw,mb,
     - E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     - A1d2_2345,A2d2_2345,A3d2_2345,A4d2_2345,A5d2_2345,
     - A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     - A1d2_1345,A2d2_1345,A3d2_1345,A4d2_1345,A5d2_1345,
     - A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     - A1d2_1245,A2d2_1245,A3d2_1245,A4d2_1245,A5d2_1245,
     - A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     - A1d2_1235,A2d2_1235,A3d2_1235,A4d2_1235,A5d2_1235,
     - A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     - A1d2_1234,A2d2_1234,A3d2_1234,A4d2_1234,A5d2_1234,
     - A6d2_1234,A7d2_1234)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 q1,q2,pb,p1b,pw,mb,den0x1,den0x2,den0x3
      real *8 E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,A1d2_2345,A2d2_2345,
     - A3d2_2345,A4d2_2345,A5d2_2345,A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,A1d2_1345,A2d2_1345,
     - A3d2_1345,A4d2_1345,A5d2_1345,A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,A1d2_1245,A2d2_1245,
     - A3d2_1245,A4d2_1245,A5d2_1245,A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,A1d2_1235,A2d2_1235,
     - A3d2_1235,A4d2_1235,A5d2_1235,A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,A1d2_1234,A2d2_1234,
     - A3d2_1234,A4d2_1234,A5d2_1234,A6d2_1234,A7d2_1234
c       
      real*8 q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b
      real*8 s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,
     -  s17,t0,CA,CV
c
      CV=1d0
      CA=-1d0
c
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
c
      s6 = CA**2/den0x1/den0x3*A5d2_1235*(128*q1Dpb*q2Dp1b**2-128*q1Dp1b
     #*q2Dpb*q2Dp1b+128*q2Dpb*q2Dp1b*mb**2-128*q2Dp1b**2*pbDp1b)+CA**2/d
     #en0x2/den0x3*A3d2_1345*(-128*q1Dpb*q1Dp1b*mb**2-256*q1Dpb*pbDp1b*m
     #b**2-128*q1Dpb*mb**4+128*q1Dpb**2*mb**2+128*q1Dp1b*mb**4)
      s7 = s6+CA**2/den0x1/den0x3*A3d1_1235*(128*q1Dq2*q1Dp1b*mb**2+128*
     #q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-256*q1Dq2*q1Dp1b*q2Dpb-
     #128*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dq2*pbDp1b*mb**2+64*q1Dpb*q2Dp1b*mb**
     #2+256*q1Dq2*q2Dpb*mb**2-512*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbD
     #p1b+448*q2Dpb*pbDp1b*mb**2-64*q2Dp1b*pbDp1b*mb**2-320*q1Dpb*q2Dpb*
     #mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2D
     #pb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dpb*q2Dp1b*pbDp1b+320*q2Dpb*mb
     #**4-128*q2Dpb*q2Dp1b*pbDp1b+64*q2Dp1b*mb**4-128*q2Dpb**2*mb**2+256
     #*q1Dp1b**2*q2Dpb+128*q2Dpb*pbDp1b**2-128*q1Dq2*mb**4-128*q2Dp1b*pb
     #Dp1b**2)
      s5 = s7+CV**2/den0x2/den0x3*A3d1_1345*(-128*q1Dq2*q1Dpb*pbDp1b-128
     #*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*mb**2-64*q1Dq2*pbDp1b*mb**2-64
     #*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb-64*q1Dpb*q1Dp1b*mb**2+256*q1Dp
     #b*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-576*q1D
     #pb*pbDp1b*mb**2-256*q1Dpb*pbDp1b**2-320*q1Dpb*mb**4+128*q1Dpb**2*q
     #1Dp1b+128*q1Dpb**2*q2Dp1b+256*q1Dpb**2*pbDp1b+320*q1Dpb**2*mb**2-6
     #4*q1Dp1b*q2Dpb*mb**2+192*q1Dp1b*pbDp1b*mb**2+192*q1Dp1b*mb**4-128*
     #q1Dp1b**2*mb**2)+CV**2/den0x1/den0x3*A2d2_1245*(128*q2Dpb*pbDp1b**
     #2-128*q2Dp1b*pbDp1b*mb**2)
      s7 = s5
      s9 = CA**2/den0x1/den0x3*A2d2_2345*(-256*q1Dq2*q1Dp1b*mb**2+128*q1
     #Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-256*q1Dq2*q1Dp1b*pbDp1b-1
     #28*q1Dq2*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp
     #1b+256*q1Dq2*pbDp1b*mb**2-256*q1Dp1b*pbDp1b*mb**2-256*q1Dpb*pbDp1b
     #*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dq2*q2D
     #p1b*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbDp1b+128*q1Dp1
     #b*q2Dp1b*pbDp1b+128*q1Dq2**2*mb**2-128*q1Dq2*q1Dpb*mb**2+256*q1Dp1
     #b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2+128*q1Dp
     #b*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*pbDp1b-128
     #*q1Dp1b*pbDp1b**2+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2Dp1b+128*q1Dp
     #1b**2*pbDp1b+128*q1Dq2**2*pbDp1b-128*q1Dpb*mb**4+128*q1Dp1b**2*mb*
     #*2-128*q1Dp1b*mb**4-128*q1Dpb*pbDp1b**2+128*q1Dq2*mb**4)
      s10 = CV**2/den0x2/den0x3*D0_1235*(128*q1Dq2*q1Dpb*q2Dpb+64*q1Dq2*
     #q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2*q1Dp1b*mb**2+128*q1Dq2
     #*q2Dpb*pbDp1b+256*q1Dq2*q2Dpb*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+128*q1D
     #pb*q2Dpb*pbDp1b+64*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dpb**2+64*q1Dpb*q
     #2Dp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+64*q1Dp1b*q2
     #Dp1b*mb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dpb*pbDp1b*mb**2+128*q2Dpb
     #*mb**4-128*q2Dpb**2*pbDp1b-256*q2Dpb**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s4 = s6+CA**2/den0x1/den0x3*A6d2_1245*(256*q2Dpb*pbDp1b*mb**2-128*
     #q2Dp1b*pbDp1b**2-128*q2Dp1b*mb**4)+CA**2/den0x1/den0x3*A5d2_1345*(
     #128*q1Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp
     #b-128*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*mb**2-128*q1Dpb**2*q2Dp
     #1b)+CA**2/den0x1/den0x3*A1d1_1234*(-128*q1Dq2*q2Dpb*q2Dp1b-64*q1Dq
     #2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-192*q1Dq2*q2Dp1b*mb**2+128*q
     #1Dq2*q2Dp1b**2+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dpb*q2Dp1b**2-128*q1Dp
     #1b*q2Dpb**2-128*q1Dp1b*q2Dp1b**2)
      s7 = s4
      s9 = CA**2/den0x2/den0x3*A2d1_1234*(-256*q1Dq2*q1Dpb*q1Dp1b-256*q1
     #Dq2*q1Dpb*q2Dp1b+128*q1Dq2*q1Dpb*pbDp1b-64*q1Dq2*q1Dpb*mb**2-192*q
     #1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-320*q1Dq2*q2Dpb*mb**2-192
     #*q1Dq2*q2Dp1b*mb**2+640*q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2+512
     #*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1
     #Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+128
     #*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+12
     #8*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b*mb
     #**2-128*q1Dp1b**2*q2Dpb)
      s10 = CA**2/den0x1/den0x3*A4d2_2345*(128*q1Dq2*q1Dpb*q2Dp1b+128*q1
     #Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb+256*
     #q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q1Dp1b*mb**2-128*q1Dq2**2*pbDp1b-128
     #*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b-128
     #*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*mb**2-128*q1Dpb**2*q2Dp1b-12
     #8*q1Dp1b**2*q2Dpb-128*q1Dp1b**2*pbDp1b-128*q1Dp1b**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s5 = s6+CV**2/den0x2/den0x3*A1d1_1234*(256*q1Dq2*q1Dpb*q2Dp1b+128*
     #q1Dq2*q2Dpb*pbDp1b+320*q1Dq2*q2Dpb*mb**2+192*q1Dq2*q2Dp1b*mb**2)+C
     #A**2/den0x1/den0x3*A2d2_1245*(128*q2Dpb*pbDp1b**2-128*q2Dp1b*pbDp1
     #b*mb**2)+CV**2/den0x2/den0x3*A4d2_2345*(-256*q1Dq2*q1Dpb*q1Dp1b-12
     #8*q1Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+
     #256*q1Dpb*q1Dp1b**2+256*q1Dpb**2*q1Dp1b-128*q1Dpb**2*q2Dp1b+256*q1
     #Dpb**2*pbDp1b+256*q1Dpb**2*mb**2)
      s7 = s5
      s9 = CV**2/den0x2/den0x3*A6d2_1345*(256*q1Dpb*q1Dp1b*mb**2-128*q1D
     #pb*pbDp1b*mb**2-256*q1Dpb*pbDp1b**2-128*q1Dpb*mb**4-128*q1Dpb**2*m
     #b**2+128*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*mb**4-128*q1Dp1b**2*mb**2)
      s10 = CA**2/den0x1/den0x3*A6d2_2345*(128*q1Dq2*q1Dp1b*mb**2-512*q1
     #Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*q2Dpb-12
     #8*q1Dpb*q1Dp1b*mb**2-384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*pbDp1b*mb**
     #2+384*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q1Dp1b*q
     #2Dpb+256*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dp
     #b*mb**2+512*q1Dq2*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*pbDp1b-128*q1Dpb*q2
     #Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2**2*mb**2+128*q1Dq2*q1
     #Dpb*mb**2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dp1b-256*q1Dq2
     #*pbDp1b**2-256*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dp
     #b*q1Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b-128*q1Dq2
     #**2*pbDp1b+128*q1Dpb*mb**4+256*q1Dpb*pbDp1b**2-128*q1Dq2*mb**4)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x2/den0x3*D0_1245*(256*q1Dq2*q1Dp1b*q2Dpb-128*q1
     #Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2+256*q1Dpb*q1Dp1b*q2Dpb-128*
     #q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-1
     #28*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb**2-
     #128*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*pbDp1b*mb**2-128*q2Dpb*mb**4-64*q
     #2Dpb**2*mb**2-64*q2Dp1b**2*mb**2)
      s3 = s7+CV**2/den0x1/den0x3*A3d2_1345*(-128*q1Dpb*q2Dpb*mb**2+128*
     #q1Dpb*q2Dp1b*mb**2+128*q1Dpb*pbDp1b*mb**2+128*q1Dpb*pbDp1b**2)+CA*
     #*2/den0x1/den0x3*A7d2_1345*(-128*q1Dq2*pbDp1b-128*q1Dq2*mb**2+128*
     #q1Dpb*q2Dp1b+128*q1Dp1b*q2Dpb+128*q1Dp1b*pbDp1b+128*q1Dp1b*mb**2)+
     #CV**2/den0x1/den0x3*A1e1*(-256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*
     #q1Dpb*q2Dpb*mb**2+128*q1Dq2*q1Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dp1b*q2
     #Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb
     #**2+256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+256*q1Dq2*q2Dpb*q2Dp1b*mb**2-128
     #*q1Dq2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**4-256*q1Dp1b*q2Dpb
     #*q2Dp1b*pbDp1b-256*q1Dp1b*q2Dpb*q2Dp1b*mb**2)
      s6 = s3+CA**2/den0x1/den0x3*A4e1*(-256*q1Dq2*q2Dpb*pbDp1b*mb**2-25
     #6*q1Dq2*q2Dpb*mb**4+128*q1Dq2*pbDp1b*mb**4+128*q1Dq2*mb**6+256*q1D
     #pb*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q1Dp
     #1b*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb*pbDp1b*mb**2+256*q1Dp1b*q2Dpb*mb*
     #*4+256*q1Dp1b**2*q2Dpb*q2Dp1b-128*q1Dp1b**2*q2Dpb*mb**2-128*q1Dp1b
     #**2*q2Dp1b*mb**2)
      s7 = s6+CA**2/den0x2/den0x3*A2d2_2345*(128*q1Dq2*q1Dp1b*mb**2+128*
     #q1Dpb*q2Dpb*q2Dp1b+256*q1Dq2*q1Dpb*q1Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+
     #128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2
     #Dp1b-256*q1Dq2*pbDp1b*mb**2+512*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1
     #b*mb**2-256*q1Dpb*q1Dp1b*q2Dpb-256*q1Dpb**2*q1Dp1b+128*q1Dq2*q2Dpb
     #*pbDp1b+128*q1Dq2*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q2D
     #pb*pbDp1b+256*q1Dq2*q1Dpb*pbDp1b-256*q1Dpb*q2Dpb*mb**2-128*q1Dq2**
     #2*mb**2+256*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb**2+128*q1Dq2*q1Dpb*
     #q2Dp1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1
     #b**2-128*q1Dp1b**2*q2Dpb-128*q1Dq2**2*pbDp1b+256*q1Dpb*mb**4+256*q
     #1Dpb*pbDp1b**2-128*q1Dq2*mb**4-256*q1Dpb**2*pbDp1b-256*q1Dpb**2*mb
     #**2)
      s5 = s7+CV**2/den0x1/den0x3*A1d1_1235*(-128*q1Dq2*q2Dpb*q2Dp1b+64*
     #q1Dq2*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b-256*q2Dpb*q2Dp1b*mb**2+
     #256*q2Dpb**2*q2Dp1b-128*q2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp
     #1b**2*mb**2)+CA**2/den0x2/den0x3*A2d1_1345*(-128*q1Dq2*q1Dp1b*mb**
     #2-64*q1Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4+192*q1Dpb*q1Dp1b*mb**2+128*
     #q1Dpb*q1Dp1b**2+128*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1D
     #pb*pbDp1b*mb**2-64*q1Dpb*mb**4+128*q1Dpb**2*mb**2+192*q1Dp1b*q2Dpb
     #*mb**2+128*q1Dp1b*q2Dp1b*mb**2-320*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*
     #pbDp1b**2-192*q1Dp1b*mb**4+64*q1Dp1b**2*mb**2)
      s6 = s5+CV**2/den0x2/den0x3*A3e1*(-128*q1Dq2*q1Dp1b*pbDp1b*mb**2-1
     #28*q1Dq2*q1Dp1b*mb**4)+CA**2/den0x2/den0x3*A3d1_1234*(256*q1Dq2*q1
     #Dpb*q1Dp1b+128*q1Dq2*q1Dpb*pbDp1b+320*q1Dq2*q1Dpb*mb**2+192*q1Dq2*
     #q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1
     #Dpb**2*q2Dp1b+128*q1Dp1b**2*q2Dpb)
      s7 = s6+CA**2/den0x1/den0x3*A1d1_1345*(128*q1Dq2*q1Dpb*pbDp1b+128*
     #q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2-2
     #56*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dpb**2*mb**2-128*q1Dp1b**2*pbDp1b-192*
     #q1Dp1b**2*mb**2)
      s4 = s7+CV**2/den0x1/den0x3*A6d2_1235*(-256*q1Dpb*q1Dp1b*q2Dp1b+12
     #8*q1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2+1
     #28*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dp1b*q2Dpb*pbDp1b-512*q1Dp1b*q2Dpb*mb
     #**2+256*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b*mb**2+256*q1Dp1b**2
     #*q2Dpb-128*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbDp1b*mb**2+256*q2Dpb*mb*
     #*4-256*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-128*q2Dp1b*mb**4+1
     #28*q2Dp1b**2*pbDp1b)+CV**2/den0x1/den0x3*A3d2_1235*(128*q1Dpb*q1Dp
     #1b*q2Dp1b-128*q1Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b
     #*q2Dp1b*pbDp1b-128*q1Dp1b**2*q2Dpb-128*q2Dpb*mb**4+128*q2Dp1b*pbDp
     #1b*mb**2)+CA**2/den0x1/den0x3*A3d2_1235*(128*q1Dpb*q1Dp1b*q2Dp1b-1
     #28*q1Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbD
     #p1b-128*q1Dp1b**2*q2Dpb-128*q2Dpb*mb**4+128*q2Dp1b*pbDp1b*mb**2)
      s7 = s4
      s9 = CA**2/den0x2/den0x3*A3d1_1245*(-128*q1Dq2*q1Dp1b*pbDp1b+192*q
     #1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2+64*q1Dq2*mb**4+128*q1Dpb*q2D
     #pb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dpb*pbDp1b-192*q1Dp1
     #b*q2Dpb*mb**2-64*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb+320*q2Dpb
     #*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+64*q2Dpb*mb**4+192*q2Dp1b*pbDp1b
     #*mb**2+64*q2Dp1b*mb**4)
      s11 = CV**2/den0x1
      s13 = 1/den0x3
      s15 = E0
      s17 = 512*q1Dq2*q1Dp1b*q2Dpb**2-256*q1Dpb*q1Dp1b*q2Dpb**2+128*q1Dp
     #b**2*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb**2*mb**2+
     #128*q1Dpb*q2Dpb**2*mb**2+128*q1Dq2*q2Dpb*mb**4-128*q1Dp1b*q2Dpb*mb
     #**4-128*q1Dp1b*q2Dp1b*mb**4+256*q1Dp1b*q2Dpb*q2Dp1b**2-128*q1Dq2*q
     #2Dp1b*mb**4-128*q1Dpb*q2Dpb*mb**4-128*q1Dp1b**2*q2Dp1b*mb**2+128*q
     #1Dpb**2*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2*mb**2-256*q1Dpb**2*q2Dpb*
     #q2Dp1b+256*q1Dq2*q2Dpb*pbDp1b**2+128*q1Dp1b*q2Dp1b**2*mb**2
      s16 = s17+256*q1Dq2*q1Dp1b*mb**4+128*q1Dp1b**2*q2Dpb*mb**2-256*q1D
     #p1b*q2Dpb**3+256*q1Dp1b**2*q2Dpb*pbDp1b-768*q1Dp1b**2*q2Dpb**2+384
     #*q1Dq2*q2Dpb*pbDp1b*mb**2+256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dpb*
     #q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*pbDp1b*mb**2+256*q1Dq2*q1Dp1b*
     #pbDp1b*mb**2-512*q1Dq2*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp1b
     #*mb**2-512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-256*q1Dp1b*q2Dpb*q2Dp1b*mb**
     #2-256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q
     #1Dpb*q2Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*pbDp1b*mb**2-128*q1Dp1b*q
     #2Dp1b*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*A3d1_1245*(128*q1Dq2*q2Dpb*mb**2-64*q1
     #Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4-128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dp1b*
     #q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2-128*q2
     #Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2+448*q2Dpb*pbDp1b*mb**2+12
     #8*q2Dpb*pbDp1b**2+320*q2Dpb*mb**4-128*q2Dpb**2*mb**2-64*q2Dp1b*pbD
     #p1b*mb**2-128*q2Dp1b*pbDp1b**2+64*q2Dp1b*mb**4+128*q2Dp1b**2*pbDp1
     #b)
      s5 = s7+CA**2/den0x1/den0x3*A2d1_1245*(128*q1Dq2*q2Dpb*pbDp1b-64*q
     #1Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4-128*q1Dpb*q2Dpb*q2Dp1b+64*q1Dpb*q
     #2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dp1
     #b*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2+448*
     #q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+320*q2Dpb*mb**4-128*q2Dpb**
     #2*pbDp1b-64*q2Dp1b*pbDp1b*mb**2-64*q2Dp1b*mb**4+128*q2Dp1b**2*mb**
     #2)+CV**2/den0x1/den0x3*A4e1*(-256*q1Dq2*q2Dpb*pbDp1b*mb**2-256*q1D
     #q2*q2Dpb*mb**4+128*q1Dq2*pbDp1b*mb**4+128*q1Dq2*mb**6+256*q1Dpb*q1
     #Dp1b*q2Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2
     #Dp1b*mb**2+256*q1Dp1b*q2Dpb*pbDp1b*mb**2+256*q1Dp1b*q2Dpb*mb**4+25
     #6*q1Dp1b**2*q2Dpb*q2Dp1b-128*q1Dp1b**2*q2Dpb*mb**2-128*q1Dp1b**2*q
     #2Dp1b*mb**2)
      s7 = s5+CA**2/den0x2/den0x3*A4e1*(128*q1Dq2*pbDp1b*mb**4+128*q1Dq2
     #*mb**6+256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+384*q1Dpb*q1Dp1b*q2Dpb*mb**2+
     #128*q1Dpb*q1Dp1b*q2Dp1b*mb**2-256*q1Dpb*q1Dp1b**2*q2Dpb+256*q1Dp1b
     #**2*q2Dpb*pbDp1b+384*q1Dp1b**2*q2Dpb*mb**2+128*q1Dp1b**2*q2Dp1b*mb
     #**2-256*q1Dp1b**3*q2Dpb)
      s8 = s7
      s9 = CA**2/den0x1/den0x3*A3d2_1345*(-128*q1Dpb*q2Dpb*mb**2+128*q1D
     #pb*q2Dp1b*mb**2+128*q1Dpb*pbDp1b*mb**2+128*q1Dpb*pbDp1b**2)+CA**2/
     #den0x2/den0x3*D0_2345*(-576*q1Dq2*q1Dp1b*mb**2+320*q1Dp1b*q2Dp1b*m
     #b**2+256*q1Dq2*q1Dp1b*q2Dp1b+384*q1Dq2*q1Dpb*q1Dp1b-128*q1Dp1b*q2D
     #pb*pbDp1b-384*q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q1Dp1b*q2Dpb+128*q1Dpb
     #*q1Dp1b*mb**2-384*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dq2*pbDp1b*mb**2-128*q1
     #Dp1b*pbDp1b*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128
     #*q1Dpb**2*q1Dp1b-128*q1Dq2*q2Dpb*mb**2+192*q1Dp1b*q2Dpb*mb**2+192*
     #q1Dpb*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b**2+64
     #*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb**2+128*q1Dpb*q1Dp1b**2+256*q1D
     #pb*q1Dp1b*pbDp1b+256*q1Dp1b**2*q2Dpb-256*q1Dq2**2*q1Dp1b-64*q1Dp1b
     #**2*mb**2-128*q1Dp1b*mb**4-64*q1Dq2*mb**4-64*q1Dpb**2*mb**2+128*q1
     #Dq2*q1Dp1b**2)
      s6 = s8+s9
      s8 = s6
      s10 = CA**2/den0x2
      s12 = 1/den0x3
      s14 = A2d1_2345
      s15 = 64*q1Dq2*q1Dp1b*mb**2+64*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb
     #*q2Dp1b-128*q1Dq2*q1Dpb*q1Dp1b+256*q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q
     #1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b-192*q1Dq2*pbDp1b*mb**2+128*q1D
     #p1b*pbDp1b*mb**2+640*q1Dpb*pbDp1b*mb**2-64*q1Dpb*q2Dp1b*mb**2+128*
     #q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q1Dp1b+128*q1Dq2*q2Dpb*pbDp1b-64*q
     #1Dq2*q2Dpb*mb**2+192*q1Dp1b*q2Dpb*mb**2-384*q1Dpb*q2Dpb*pbDp1b+384
     #*q1Dq2*q1Dpb*pbDp1b-320*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b
     #-256*q1Dq2**2*mb**2+512*q1Dq2*q1Dpb*mb**2-256*q1Dp1b*q2Dpb**2-128*
     #q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**2-128
     #*q1Dpb*q1Dp1b**2-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2-128*
     #q1Dp1b**2*q2Dpb-128*q1Dpb**2*q2Dp1b-256*q1Dq2**2*pbDp1b+384*q1Dpb*
     #mb**4+256*q1Dp1b**2*mb**2+256*q1Dpb*pbDp1b**2-64*q1Dq2*mb**4-128*q
     #1Dpb**2*mb**2
      s13 = s14*s15
      s11 = s12*s13
      s9 = s10*s11
      s7 = s8+s9
      s2 = s7+CA**2/den0x1/den0x3*A7d2_1245*(128*q2Dpb*mb**2-128*q2Dp1b*
     #pbDp1b)+CA**2/den0x1/den0x3*A3d2_2345*(-256*q1Dq2*q2Dpb*q2Dp1b-128
     #*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*pbDp1b*mb**2+
     #128*q1Dq2*pbDp1b**2+256*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*mb**2+1
     #28*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*pbDp1b**2)
     #+CA**2/den0x2/den0x3*A4d2_1345*(128*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q
     #1Dp1b**2+128*q1Dpb**2*mb**2)
      s5 = CA**2/den0x1/den0x3*A7d2_2345*(128*q1Dq2*pbDp1b+128*q1Dq2*mb*
     #*2-128*q1Dpb*q2Dp1b-128*q1Dp1b*q2Dpb-128*q1Dp1b*pbDp1b-128*q1Dp1b*
     #mb**2)+CV**2/den0x2/den0x3*A7d2_2345*(256*q1Dpb*q1Dp1b+256*q1Dpb*p
     #bDp1b+128*q1Dpb*mb**2-128*q1Dp1b*mb**2)+CA**2/den0x2/den0x3*A2d2_1
     #345*(-128*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dp1b*pbD
     #p1b*mb**2+128*q1Dp1b**2*mb**2)+CA**2/den0x1/den0x3*A3d1_1234*(-128
     #*q1Dq2*q1Dpb*q2Dp1b-64*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-1
     #28*q1Dq2*q1Dp1b*pbDp1b-192*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2D
     #p1b-128*q1Dp1b**2*q2Dp1b)+CV**2/den0x2/den0x3*A1d1_1235*(64*q1Dq2*
     #q2Dp1b*mb**2-256*q1Dpb*q2Dpb*q2Dp1b-256*q2Dpb*q2Dp1b*mb**2-128*q2D
     #pb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp1b**2*mb**2)
      s6 = s5+CA**2/den0x2/den0x3*A3e1*(-128*q1Dq2*q1Dp1b*pbDp1b*mb**2-1
     #28*q1Dq2*q1Dp1b*mb**4)+CA**2/den0x2/den0x3*A1d1_1345*(128*q1Dpb*q1
     #Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b**2-64*q1Dpb**2
     #*mb**2-192*q1Dp1b**2*mb**2)
      s7 = s6+CV**2/den0x1/den0x3*D0_1235*(-128*q1Dq2*q1Dp1b*mb**2+128*q
     #1Dp1b*q2Dp1b*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+256*q1Dq2*q1Dpb*q2Dpb+12
     #8*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q1Dp1b*q2Dpb-512*q1Dq2*q2Dpb*pbDp1
     #b-704*q1Dq2*q2Dpb*mb**2+384*q1Dp1b*q2Dpb*mb**2+128*q2Dpb*q2Dp1b*mb
     #**2+256*q1Dpb*q2Dpb*pbDp1b+384*q1Dq2*q2Dpb*q2Dp1b-384*q2Dpb*pbDp1b
     #*mb**2+384*q1Dpb*q2Dpb*mb**2+256*q1Dp1b*q2Dpb**2+64*q1Dq2*q2Dp1b*m
     #b**2-256*q1Dq2**2*q2Dpb-256*q2Dpb*mb**4+256*q2Dpb*q2Dp1b*pbDp1b-64
     #*q2Dpb**2*mb**2-128*q1Dp1b**2*q2Dpb-128*q2Dpb*pbDp1b**2-64*q2Dp1b*
     #*2*mb**2+128*q2Dpb**2*q2Dp1b+128*q1Dq2*q2Dpb**2-128*q1Dpb**2*q2Dpb
     #-128*q2Dpb*q2Dp1b**2)
      s8 = s7+CV**2/den0x1/den0x3*D0_1234*(128*q1Dq2*q1Dp1b*mb**2-128*q1
     #Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b-2
     #56*q1Dq2*q1Dpb*q2Dpb+128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q2Dp
     #b+128*q1Dpb*q1Dp1b*q2Dp1b-384*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*
     #mb**2+128*q1Dpb*q1Dp1b*q2Dpb+384*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dp
     #1b*pbDp1b+384*q1Dq2*q2Dpb*mb**2-384*q1Dq2*q2Dpb*q2Dp1b-128*q1Dp1b*
     #q2Dp1b*pbDp1b-128*q1Dq2**2*mb**2-256*q1Dp1b*q2Dpb**2+128*q1Dq2*q1D
     #pb*q2Dp1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2
     #Dp1b*mb**2+256*q1Dq2**2*q2Dpb+128*q1Dp1b**2*q2Dp1b-128*q1Dq2**2*pb
     #Dp1b-128*q1Dq2*q2Dpb**2-256*q1Dq2*mb**4+128*q1Dpb**2*q2Dpb)
      s4 = s8+CV**2/den0x2/den0x3*A1d1_2345*(64*q1Dq2*q1Dpb*mb**2-128*q1
     #Dq2*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*mb**2+2
     #56*q1Dpb*q1Dp1b**2-64*q1Dpb**2*mb**2-192*q1Dp1b**2*mb**2)+CA**2/de
     #n0x2/den0x3*A1d1_1234*(256*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*q2Dpb*pbDp
     #1b+320*q1Dq2*q2Dpb*mb**2+192*q1Dq2*q2Dp1b*mb**2)
      s6 = s4+CA**2/den0x2/den0x3*A2d1_1235*(128*q1Dq2*q1Dp1b*mb**2+128*
     #q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q1Dp1b+
     #128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b-320*q1Dq2*pbDp1b*m
     #b**2+128*q1Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb*mb**2+256*q2Dpb*q2Dp1
     #b*mb**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*pbDp1b-512*q2Dpb*pb
     #Dp1b*mb**2-256*q2Dp1b*pbDp1b*mb**2+384*q1Dpb*q2Dpb*mb**2+192*q1Dq2
     #*q1Dpb*mb**2-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*
     #q2Dp1b*mb**2-384*q2Dpb*mb**4-256*q2Dp1b*mb**4+320*q2Dpb**2*mb**2-1
     #28*q1Dp1b**2*q2Dpb-128*q2Dpb*pbDp1b**2+128*q1Dpb**2*q2Dp1b+64*q2Dp
     #1b**2*mb**2+128*q2Dpb**2*pbDp1b-192*q1Dq2*mb**4)+CV**2/den0x1/den0
     #x3*A7d2_1235*(-128*q2Dpb*mb**2+128*q2Dp1b*pbDp1b)
      s5 = s6+CA**2/den0x2/den0x3*A7d2_1345*(-256*q1Dpb*q1Dp1b-256*q1Dpb
     #*pbDp1b-128*q1Dpb*mb**2+128*q1Dp1b*mb**2)+CV**2/den0x1/den0x3*A3e1
     #*(256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q
     #1Dq2*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q1Dp1b*mb**4-256*q1Dp1b**2*q2Dp
     #b*pbDp1b-256*q1Dp1b**2*q2Dpb*mb**2)+CV**2/den0x1/den0x3*A4d2_2345*
     #(128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb*
     #*2+128*q1Dq2*q1Dp1b*q2Dpb+256*q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q1Dp1b
     #*mb**2-128*q1Dq2**2*pbDp1b-128*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2D
     #pb-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q1Dp1
     #b*mb**2-128*q1Dpb**2*q2Dp1b-128*q1Dp1b**2*q2Dpb-128*q1Dp1b**2*pbDp
     #1b-128*q1Dp1b**2*mb**2)
      s6 = s5+CV**2/den0x1/den0x3*A2d2_1345*(128*q1Dp1b*q2Dpb*mb**2-128*
     #q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4)+CV**
     #2/den0x2/den0x3*A3d1_1245*(-128*q1Dq2*q1Dp1b*pbDp1b+192*q1Dq2*pbDp
     #1b*mb**2+128*q1Dq2*pbDp1b**2+64*q1Dq2*mb**4+128*q1Dpb*q2Dpb*mb**2+
     #128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dpb*pbDp1b-192*q1Dp1b*q2Dpb*m
     #b**2-64*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb+320*q2Dpb*pbDp1b*m
     #b**2+128*q2Dpb*pbDp1b**2+64*q2Dpb*mb**4+192*q2Dp1b*pbDp1b*mb**2+64
     #*q2Dp1b*mb**4)
      s7 = s6+CA**2/den0x1/den0x3*D0_1245*(-256*q1Dq2*q2Dpb*q2Dp1b+128*q
     #1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbDp1b-64*
     #q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2-256*
     #q1Dp1b*q2Dpb**2-64*q1Dp1b*q2Dp1b*mb**2-256*q2Dpb*q2Dp1b*pbDp1b-384
     #*q2Dpb*q2Dp1b*mb**2+128*q2Dpb*q2Dp1b**2+384*q2Dpb*pbDp1b*mb**2+128
     #*q2Dpb*pbDp1b**2+256*q2Dpb*mb**4+128*q2Dpb**2*q2Dp1b-128*q2Dpb**2*
     #pbDp1b-256*q2Dpb**2*mb**2)
      s8 = s7+CA**2/den0x1/den0x3*A2d1_1345*(-128*q1Dq2*q2Dpb*mb**2-64*q
     #1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2+64*q1Dq2*mb**4+64*q1Dpb*q1Dp
     #1b*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1Dpb*pbDp1b*mb**2+64*q1Dpb*mb**
     #4+128*q1Dp1b*q2Dpb*pbDp1b+64*q1Dp1b*q2Dpb*mb**2-192*q1Dp1b*pbDp1b*
     #mb**2-64*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb+64*q1Dp1b**2*mb**2)
      s3 = s8+CA**2/den0x1/den0x3*A1d1_1235*(-128*q1Dq2*q2Dpb*q2Dp1b+64*
     #q1Dq2*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b-256*q2Dpb*q2Dp1b*mb**2+
     #256*q2Dpb**2*q2Dp1b-128*q2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp
     #1b**2*mb**2)+CA**2/den0x2/den0x3*A3d2_2345*(128*q1Dq2*q1Dpb*q2Dp1b
     #+128*q1Dq2*q1Dpb*pbDp1b+256*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2D
     #pb-128*q1Dq2*q1Dp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb*m
     #b**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dq2**2*pbDp1b-12
     #8*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*mb**2+128
     #*q1Dpb*q2Dpb*q2Dp1b-256*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-1
     #28*q1Dpb*q2Dp1b*mb**2+256*q1Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4-128*q
     #1Dpb**2*q2Dp1b-128*q1Dpb**2*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1
     #b*q2Dpb**2-128*q1Dp1b*mb**4)
      s5 = s3+CV**2/den0x2/den0x3*A3d2_1345*(-128*q1Dpb*q1Dp1b*mb**2-256
     #*q1Dpb*pbDp1b*mb**2-128*q1Dpb*mb**4+128*q1Dpb**2*mb**2+128*q1Dp1b*
     #mb**4)+CA**2/den0x2/den0x3*D0_1245*(256*q1Dq2*q1Dp1b*q2Dpb-128*q1D
     #q2*q2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2+256*q1Dpb*q1Dp1b*q2Dpb-128*q
     #1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-12
     #8*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb**2-1
     #28*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*pbDp1b*mb**2-128*q2Dpb*mb**4-64*q2
     #Dpb**2*mb**2-64*q2Dp1b**2*mb**2)+CV**2/den0x2/den0x3*A5d2_1345*(-1
     #28*q1Dpb*q1Dp1b*mb**2+256*q1Dpb**2*q1Dp1b+256*q1Dpb**2*pbDp1b+128*
     #q1Dpb**2*mb**2)+CA**2/den0x2/den0x3*A5d2_1345*(-128*q1Dpb*q1Dp1b*m
     #b**2+256*q1Dpb**2*q1Dp1b+256*q1Dpb**2*pbDp1b+128*q1Dpb**2*mb**2)
      s7 = s5
      s9 = CA**2/den0x2/den0x3*A3d1_1235*(128*q1Dq2*q1Dpb*q1Dp1b-128*q1D
     #q2*q1Dpb*pbDp1b-192*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b+192*
     #q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2-128*q1Dpb*q1Dp1b*q2Dpb-128*
     #q1Dpb*q2Dpb*pbDp1b-192*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b-1
     #92*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-
     #256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb
     #+320*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+64*q2Dpb*mb**4+192*q2D
     #p1b*pbDp1b*mb**2+64*q2Dp1b*mb**4)
      s11 = CA**2/den0x1
      s13 = 1/den0x3
      s15 = A2e1
      s17 = 128*q1Dpb**2*q2Dpb*mb**2-128*q1Dq2*pbDp1b**2*mb**2+256*q1Dq2
     #*q2Dpb*mb**4-256*q1Dp1b*q2Dpb*mb**4+128*q1Dq2*q2Dp1b*mb**4+128*q1D
     #p1b**2*q2Dp1b*mb**2-256*q1Dp1b**2*q2Dpb*q2Dp1b+128*q1Dpb**2*q2Dp1b
     #*mb**2-256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dq2*pbDp1b*mb**4+256*q1Dq2*
     #q2Dpb*pbDp1b**2-256*q1Dp1b*q2Dpb*pbDp1b**2+128*q1Dq2*q1Dp1b*mb**4+
     #384*q1Dp1b**2*q2Dpb*mb**2+256*q1Dp1b**2*q2Dpb*pbDp1b-128*q1Dq2*mb*
     #*6+512*q1Dq2*q2Dpb*pbDp1b*mb**2
      s16 = s17-512*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+256*q1Dpb*q1Dp1b*q2Dpb*mb*
     #*2+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-512*q1Dp1b*q2Dpb*pbDp1b*mb**2-256
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-384*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*
     #q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q
     #2Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb*q2D
     #p1b-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2-256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-25
     #6*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2+256*q1Dp1
     #b*q2Dpb*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dpb*q2Dp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*A4d2_2345*(-256*q1Dq2*q1Dpb*q1Dp1b-128
     #*q1Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+2
     #56*q1Dpb*q1Dp1b**2+256*q1Dpb**2*q1Dp1b-128*q1Dpb**2*q2Dp1b+256*q1D
     #pb**2*pbDp1b+256*q1Dpb**2*mb**2)
      s8 = s7+CV**2/den0x2/den0x3*A6d2_2345*(-256*q1Dpb*q2Dpb*q2Dp1b-256
     #*q1Dq2*q1Dpb*q1Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dq2*q1Dp1b*pbDp1
     #b-256*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q
     #2Dp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2-640*q1Dpb*pbD
     #p1b*mb**2+256*q1Dpb*q2Dp1b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb+256*q1Dpb*
     #*2*q1Dp1b-256*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*mb**2-384*q1Dp1b*
     #q2Dpb*mb**2+512*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*q1Dpb*pbDp1b+384*q1Dp
     #b*q2Dpb*mb**2+256*q1Dq2**2*mb**2-512*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*
     #q2Dpb**2-256*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2+128*q1Dpb*q2Dp
     #1b*pbDp1b+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2Dp1b+256*q1Dq2**2*pbD
     #p1b-384*q1Dpb*mb**4-128*q1Dp1b**2*mb**2+128*q1Dp1b*mb**4-256*q1Dpb
     #*pbDp1b**2+256*q1Dq2*mb**4+256*q1Dpb**2*pbDp1b+256*q1Dpb**2*mb**2)
      s4 = s8+CV**2/den0x1/den0x3*A6d2_1245*(256*q2Dpb*pbDp1b*mb**2-128*
     #q2Dp1b*pbDp1b**2-128*q2Dp1b*mb**4)+CV**2/den0x1/den0x3*A1d1_1234*(
     #-128*q1Dq2*q2Dpb*q2Dp1b-64*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp
     #1b-192*q1Dq2*q2Dp1b*mb**2+128*q1Dq2*q2Dp1b**2+128*q1Dpb*q2Dpb*q2Dp
     #1b-128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb**2-128*q1Dp1b*q2Dp1b**2)
      s6 = s4+CV**2/den0x2/den0x3*A4d2_1345*(128*q1Dpb*q1Dp1b*mb**2+256*
     #q1Dpb*q1Dp1b**2+128*q1Dpb**2*mb**2)+CA**2/den0x1/den0x3*A5d2_2345*
     #(-128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb
     #**2-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1
     #b*mb**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1b*q2
     #Dpb+128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb**2*q2
     #Dp1b)
      s7 = s6+CA**2/den0x1/den0x3*A1d1_1245*(-256*q2Dpb*q2Dp1b*mb**2+256
     #*q2Dpb**2*q2Dp1b-128*q2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp1b*
     #*2*mb**2)
      s5 = s7+CV**2/den0x1/den0x3*A4d2_1245*(-128*q2Dpb*q2Dp1b*pbDp1b+12
     #8*q2Dp1b**2*mb**2)+CA**2/den0x1/den0x3*D0_1235*(-128*q1Dq2*q1Dp1b*
     #mb**2+128*q1Dp1b*q2Dp1b*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+256*q1Dq2*q1D
     #pb*q2Dpb+128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q1Dp1b*q2Dpb-512*q1Dq2*
     #q2Dpb*pbDp1b-704*q1Dq2*q2Dpb*mb**2+384*q1Dp1b*q2Dpb*mb**2+128*q2Dp
     #b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*pbDp1b+384*q1Dq2*q2Dpb*q2Dp1b-384*q
     #2Dpb*pbDp1b*mb**2+384*q1Dpb*q2Dpb*mb**2+256*q1Dp1b*q2Dpb**2+64*q1D
     #q2*q2Dp1b*mb**2-256*q1Dq2**2*q2Dpb-256*q2Dpb*mb**4+256*q2Dpb*q2Dp1
     #b*pbDp1b-64*q2Dpb**2*mb**2-128*q1Dp1b**2*q2Dpb-128*q2Dpb*pbDp1b**2
     #-64*q2Dp1b**2*mb**2+128*q2Dpb**2*q2Dp1b+128*q1Dq2*q2Dpb**2-128*q1D
     #pb**2*q2Dpb-128*q2Dpb*q2Dp1b**2)
      s7 = s5+CA**2/den0x1/den0x3*A2d1_1235*(-128*q1Dq2*q1Dp1b*mb**2-128
     #*q1Dp1b*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q1Dp1b*q2Dp
     #b+128*q1Dpb*q1Dp1b*q2Dp1b+192*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*
     #mb**2-256*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*mb**2+512*q1Dp1b*q2Dp
     #b*mb**2+384*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dq2*q2
     #Dpb*q2Dp1b-896*q2Dpb*pbDp1b*mb**2+128*q2Dp1b*pbDp1b*mb**2+256*q1Dp
     #b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*mb**2-128*q1
     #Dp1b*q2Dpb**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q2Dp1b*mb**2-640*q2
     #Dpb*mb**4+128*q2Dpb*q2Dp1b*pbDp1b+448*q2Dpb**2*mb**2-256*q1Dp1b**2
     #*q2Dpb-256*q2Dpb*pbDp1b**2+64*q2Dp1b**2*mb**2-256*q2Dpb**2*q2Dp1b+
     #256*q2Dpb**2*pbDp1b+192*q1Dq2*mb**4+128*q2Dp1b*pbDp1b**2)
      s8 = s7
      s10 = CV**2/den0x1/den0x3*A2d2_1235*(128*q1Dpb*q1Dp1b*q2Dp1b-128*q
     #1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-128*
     #q1Dp1b*q2Dpb*q2Dp1b+256*q1Dp1b*q2Dpb*pbDp1b+256*q1Dp1b*q2Dpb*mb**2
     #-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b**2*q2
     #Dpb+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*pbDp1
     #b*mb**2-128*q2Dpb*pbDp1b**2-128*q2Dpb*mb**4+256*q2Dp1b*pbDp1b*mb**
     #2+128*q2Dp1b*pbDp1b**2+128*q2Dp1b*mb**4-128*q2Dp1b**2*pbDp1b-128*q
     #2Dp1b**2*mb**2)
      s11 = CA**2/den0x2/den0x3*D0_1345*(-256*q1Dq2*q1Dpb*q1Dp1b+128*q1D
     #q2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128
     #*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*mb**
     #2+128*q1Dpb*q1Dp1b**2-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb**2*q1Dp1b-12
     #8*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b*mb
     #**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4-256*q1Dp1b**2*q2Dpb-1
     #28*q1Dp1b**2*mb**2)
      s9 = s10+s11
      s6 = s8+s9
      s7 = s6+CA**2/den0x1/den0x3*A2d2_1235*(128*q1Dpb*q1Dp1b*q2Dp1b-128
     #*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-12
     #8*q1Dp1b*q2Dpb*q2Dp1b+256*q1Dp1b*q2Dpb*pbDp1b+256*q1Dp1b*q2Dpb*mb*
     #*2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b**2*
     #q2Dpb+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*pbD
     #p1b*mb**2-128*q2Dpb*pbDp1b**2-128*q2Dpb*mb**4+256*q2Dp1b*pbDp1b*mb
     #**2+128*q2Dp1b*pbDp1b**2+128*q2Dp1b*mb**4-128*q2Dp1b**2*pbDp1b-128
     #*q2Dp1b**2*mb**2)
      s1 = s7+CV**2/den0x2/den0x3*A1e1*(256*q1Dq2*q1Dpb*q1Dp1b*q2Dpb-256
     #*q1Dq2*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q1
     #Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-384*q1Dq2*q1Dp1b*q2
     #Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b**2*q2Dpb-
     #128*q1Dq2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**4)+CV**2/den0x2
     #/den0x3*A2d1_1345*(-128*q1Dq2*q1Dp1b*mb**2-64*q1Dq2*pbDp1b*mb**2-6
     #4*q1Dq2*mb**4+192*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b**2+128*q1Dpb
     #*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1Dpb*pbDp1b*mb**2-64*q1Dpb*
     #mb**4+128*q1Dpb**2*mb**2+192*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*
     #mb**2-320*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*pbDp1b**2-192*q1Dp1b*mb**
     #4+64*q1Dp1b**2*mb**2)+s2
      s6 = s1+CA**2/den0x2/den0x3*A3d1_1345*(-128*q1Dq2*q1Dpb*pbDp1b-128
     #*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*mb**2-64*q1Dq2*pbDp1b*mb**2-64
     #*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb-64*q1Dpb*q1Dp1b*mb**2+256*q1Dp
     #b*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-576*q1D
     #pb*pbDp1b*mb**2-256*q1Dpb*pbDp1b**2-320*q1Dpb*mb**4+128*q1Dpb**2*q
     #1Dp1b+128*q1Dpb**2*q2Dp1b+256*q1Dpb**2*pbDp1b+320*q1Dpb**2*mb**2-6
     #4*q1Dp1b*q2Dpb*mb**2+192*q1Dp1b*pbDp1b*mb**2+192*q1Dp1b*mb**4-128*
     #q1Dp1b**2*mb**2)
      s7 = s6+CA**2/den0x1/den0x3*A6d2_1235*(-256*q1Dpb*q1Dp1b*q2Dp1b+12
     #8*q1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2+1
     #28*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dp1b*q2Dpb*pbDp1b-512*q1Dp1b*q2Dpb*mb
     #**2+256*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b*mb**2+256*q1Dp1b**2
     #*q2Dpb-128*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbDp1b*mb**2+256*q2Dpb*mb*
     #*4-256*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-128*q2Dp1b*mb**4+1
     #28*q2Dp1b**2*pbDp1b)
      s5 = s7+CV**2/den0x2/den0x3*A3d2_2345*(128*q1Dq2*q1Dpb*q2Dp1b+128*
     #q1Dq2*q1Dpb*pbDp1b+256*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb-12
     #8*q1Dq2*q1Dp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb*mb**2-
     #128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dq2**2*pbDp1b-128*q1D
     #q2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*mb**2+128*q1Dp
     #b*q2Dpb*q2Dp1b-256*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-128*q1
     #Dpb*q2Dp1b*mb**2+256*q1Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4-128*q1Dpb*
     #*2*q2Dp1b-128*q1Dpb**2*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2D
     #pb**2-128*q1Dp1b*mb**4)+CV**2/den0x1/den0x3*A1d1_1345*(128*q1Dq2*q
     #1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq
     #2*q1Dp1b*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dpb**2*mb**2-128*q1Dp1
     #b**2*pbDp1b-192*q1Dp1b**2*mb**2)
      s7 = s5
      s9 = CV**2/den0x2/den0x3*A2d2_2345*(128*q1Dq2*q1Dp1b*mb**2+128*q1D
     #pb*q2Dpb*q2Dp1b+256*q1Dq2*q1Dpb*q1Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+128
     #*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1
     #b-256*q1Dq2*pbDp1b*mb**2+512*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*m
     #b**2-256*q1Dpb*q1Dp1b*q2Dpb-256*q1Dpb**2*q1Dp1b+128*q1Dq2*q2Dpb*pb
     #Dp1b+128*q1Dq2*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q2Dpb*
     #pbDp1b+256*q1Dq2*q1Dpb*pbDp1b-256*q1Dpb*q2Dpb*mb**2-128*q1Dq2**2*m
     #b**2+256*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb**2+128*q1Dq2*q1Dpb*q2D
     #p1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1b**
     #2-128*q1Dp1b**2*q2Dpb-128*q1Dq2**2*pbDp1b+256*q1Dpb*mb**4+256*q1Dp
     #b*pbDp1b**2-128*q1Dq2*mb**4-256*q1Dpb**2*pbDp1b-256*q1Dpb**2*mb**2
     #)
      s10 = CV**2/den0x1/den0x3*D0_2345*(256*q1Dq2*q1Dp1b*mb**2-256*q1Dp
     #1b*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b+12
     #8*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*mb**
     #2-256*q1Dpb*q1Dp1b*q2Dp1b+192*q1Dq2*pbDp1b*mb**2+256*q1Dp1b*pbDp1b
     #*mb**2+128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q2D
     #pb*pbDp1b-128*q1Dq2*q2Dp1b*pbDp1b-64*q1Dq2*q2Dpb*mb**2-384*q1Dp1b*
     #q2Dpb*mb**2-128*q1Dpb*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp
     #1b*q2Dpb**2+128*q1Dq2*pbDp1b**2-64*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**
     #2*q2Dpb-128*q1Dp1b**2*q2Dp1b-128*q1Dp1b**2*pbDp1b+128*q1Dpb*mb**4-
     #256*q1Dp1b**2*mb**2+256*q1Dp1b*mb**4+64*q1Dq2*mb**4)
      s8 = s9+s10
      s6 = s7+s8
      s8 = s6
      s10 = CV**2/den0x1
      s12 = 1/den0x3
      s14 = A2e1
      s16 = 128*q1Dpb**2*q2Dpb*mb**2-128*q1Dq2*pbDp1b**2*mb**2+256*q1Dq2
     #*q2Dpb*mb**4-256*q1Dp1b*q2Dpb*mb**4+128*q1Dq2*q2Dp1b*mb**4+128*q1D
     #p1b**2*q2Dp1b*mb**2-256*q1Dp1b**2*q2Dpb*q2Dp1b+128*q1Dpb**2*q2Dp1b
     #*mb**2-256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dq2*pbDp1b*mb**4+256*q1Dq2*
     #q2Dpb*pbDp1b**2-256*q1Dp1b*q2Dpb*pbDp1b**2+128*q1Dq2*q1Dp1b*mb**4+
     #384*q1Dp1b**2*q2Dpb*mb**2+256*q1Dp1b**2*q2Dpb*pbDp1b-128*q1Dq2*mb*
     #*6+512*q1Dq2*q2Dpb*pbDp1b*mb**2
      s15 = s16-512*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+256*q1Dpb*q1Dp1b*q2Dpb*mb*
     #*2+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-512*q1Dp1b*q2Dpb*pbDp1b*mb**2-256
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-384*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*
     #q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q
     #2Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb*q2D
     #p1b-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2-256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-25
     #6*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2+256*q1Dp1
     #b*q2Dpb*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dpb*q2Dp1b*mb**2
      s13 = s14*s15
      s11 = s12*s13
      s9 = s10*s11
      s7 = s8+s9
      s8 = s7
      s10 = CA**2/den0x1/den0x3*D0_1234*(128*q1Dq2*q1Dp1b*mb**2-128*q1Dp
     #1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b-256
     #*q1Dq2*q1Dpb*q2Dpb+128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q2Dpb+
     #128*q1Dpb*q1Dp1b*q2Dp1b-384*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb
     #**2+128*q1Dpb*q1Dp1b*q2Dpb+384*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dp1b
     #*pbDp1b+384*q1Dq2*q2Dpb*mb**2-384*q1Dq2*q2Dpb*q2Dp1b-128*q1Dp1b*q2
     #Dp1b*pbDp1b-128*q1Dq2**2*mb**2-256*q1Dp1b*q2Dpb**2+128*q1Dq2*q1Dpb
     #*q2Dp1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp
     #1b*mb**2+256*q1Dq2**2*q2Dpb+128*q1Dp1b**2*q2Dp1b-128*q1Dq2**2*pbDp
     #1b-128*q1Dq2*q2Dpb**2-256*q1Dq2*mb**4+128*q1Dpb**2*q2Dpb)
      s11 = CA**2/den0x1/den0x3*A1e1*(-256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*
     #q1Dq2*q1Dpb*q2Dpb*mb**2+128*q1Dq2*q1Dpb*q2Dp1b*mb**2-256*q1Dq2*q1D
     #p1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2D
     #p1b*mb**2+256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+256*q1Dq2*q2Dpb*q2Dp1b*mb*
     #*2-128*q1Dq2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**4-256*q1Dp1b
     #*q2Dpb*q2Dp1b*pbDp1b-256*q1Dp1b*q2Dpb*q2Dp1b*mb**2)
      s9 = s10+s11
      s4 = s8+s9
      s6 = s4+CV**2/den0x1/den0x3*A6d2_1345*(128*q1Dpb*q2Dpb*mb**2-128*q
     #1Dpb*q2Dp1b*mb**2+128*q1Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4-128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*
     #q1Dp1b*pbDp1b**2)+CV**2/den0x2/den0x3*A4e1*(128*q1Dq2*pbDp1b*mb**4
     #+128*q1Dq2*mb**6+256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+384*q1Dpb*q1Dp1b*q2
     #Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b*mb**2-256*q1Dpb*q1Dp1b**2*q2Dpb+
     #256*q1Dp1b**2*q2Dpb*pbDp1b+384*q1Dp1b**2*q2Dpb*mb**2+128*q1Dp1b**2
     #*q2Dp1b*mb**2-256*q1Dp1b**3*q2Dpb)
      s7 = s6+CV**2/den0x1/den0x3*A3d2_2345*(-256*q1Dq2*q2Dpb*q2Dp1b-128
     #*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*pbDp1b*mb**2+
     #128*q1Dq2*pbDp1b**2+256*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*mb**2+1
     #28*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*pbDp1b**2)
      s8 = s7
      s11 = CA**2/den0x1
      s13 = 1/den0x3
      s15 = E0
      s17 = 512*q1Dq2*q1Dp1b*q2Dpb**2-256*q1Dpb*q1Dp1b*q2Dpb**2+128*q1Dp
     #b**2*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb**2*mb**2+
     #128*q1Dpb*q2Dpb**2*mb**2+128*q1Dq2*q2Dpb*mb**4-128*q1Dp1b*q2Dpb*mb
     #**4-128*q1Dp1b*q2Dp1b*mb**4+256*q1Dp1b*q2Dpb*q2Dp1b**2-128*q1Dq2*q
     #2Dp1b*mb**4-128*q1Dpb*q2Dpb*mb**4-128*q1Dp1b**2*q2Dp1b*mb**2+128*q
     #1Dpb**2*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2*mb**2-256*q1Dpb**2*q2Dpb*
     #q2Dp1b+256*q1Dq2*q2Dpb*pbDp1b**2+128*q1Dp1b*q2Dp1b**2*mb**2
      s16 = s17+256*q1Dq2*q1Dp1b*mb**4+128*q1Dp1b**2*q2Dpb*mb**2-256*q1D
     #p1b*q2Dpb**3+256*q1Dp1b**2*q2Dpb*pbDp1b-768*q1Dp1b**2*q2Dpb**2+384
     #*q1Dq2*q2Dpb*pbDp1b*mb**2+256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dpb*
     #q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*pbDp1b*mb**2+256*q1Dq2*q1Dp1b*
     #pbDp1b*mb**2-512*q1Dq2*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp1b
     #*mb**2-512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-256*q1Dp1b*q2Dpb*q2Dp1b*mb**
     #2-256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q
     #1Dpb*q2Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*pbDp1b*mb**2-128*q1Dp1b*q
     #2Dp1b*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s11 = CV**2/den0x1/den0x3*A3d1_1345*(-128*q1Dq2*q2Dpb*pbDp1b-64*q1
     #Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb+64*q1Dpb*q1
     #Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb
     #*q2Dp1b*pbDp1b+192*q1Dpb*q2Dp1b*mb**2-192*q1Dpb*pbDp1b*mb**2-320*q
     #1Dpb*mb**4+64*q1Dpb**2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b-192*q1Dp1b*q2
     #Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*mb**2+192*q1Dp1b*p
     #bDp1b*mb**2+128*q1Dp1b*pbDp1b**2-64*q1Dp1b*mb**4)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x2/den0x3*A1d1_1245*(-256*q1Dpb*q2Dpb*q2Dp1b-256
     #*q2Dpb*q2Dp1b*mb**2-128*q2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp
     #1b**2*mb**2)+CV**2/den0x1/den0x3*A2d1_1245*(128*q1Dq2*q2Dpb*pbDp1b
     #-64*q1Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4-128*q1Dpb*q2Dpb*q2Dp1b+64*q1
     #Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb*pbDp1b+128*
     #q1Dp1b*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2
     #+448*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+320*q2Dpb*mb**4-128*q2
     #Dpb**2*pbDp1b-64*q2Dp1b*pbDp1b*mb**2-64*q2Dp1b*mb**4+128*q2Dp1b**2
     #*mb**2)
      s7 = s6+CV**2/den0x1/den0x3*A7d2_1345*(-128*q1Dq2*pbDp1b-128*q1Dq2
     #*mb**2+128*q1Dpb*q2Dp1b+128*q1Dp1b*q2Dpb+128*q1Dp1b*pbDp1b+128*q1D
     #p1b*mb**2)
      s9 = s7
      s11 = CA**2/den0x2
      s13 = 1/den0x3
      s15 = E0
      s17 = 256*q1Dpb*q1Dp1b*q2Dpb**2+512*q1Dq2*q1Dp1b**2*q2Dpb-256*q1Dp
     #b**2*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*mb**4-256*q1Dpb*q2Dpb**2*mb**2+5
     #12*q1Dpb*q1Dp1b**2*q2Dpb+256*q1Dq2*q2Dpb*mb**4+128*q1Dp1b*q2Dpb*mb
     #**4-128*q1Dp1b*q2Dp1b*mb**4-128*q1Dpb*q2Dpb*mb**4-256*q1Dpb**2*q2D
     #pb*pbDp1b+512*q1Dpb**2*q1Dp1b*q2Dpb-256*q1Dp1b**2*q2Dpb*q2Dp1b+256
     #*q1Dp1b*q2Dpb*pbDp1b**2-128*q1Dq2*q1Dp1b*mb**4-128*q1Dq2*q1Dpb*mb*
     #*4-256*q1Dp1b**2*q2Dpb*mb**2
      s16 = s17+256*q1Dp1b*q2Dpb**2*pbDp1b-256*q1Dp1b**2*q2Dpb*pbDp1b-76
     #8*q1Dp1b**2*q2Dpb**2-512*q1Dq2*q1Dpb*q1Dp1b*q2Dpb+256*q1Dq2*q2Dpb*
     #pbDp1b*mb**2+768*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-1024*q1Dpb*q1Dp1b*q2Dpb
     #*mb**2+384*q1Dp1b*q2Dpb*pbDp1b*mb**2-128*q1Dq2*q1Dp1b*pbDp1b*mb**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-512*q1Dp1b*q2Dpb*q2Dp1b*mb**2-1024
     #*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q1Dpb*
     #q2Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*pbDp1b*mb**2-128*q1Dp1b*q2Dp1b
     #*pbDp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s9 = s8
      s11 = CV**2/den0x2/den0x3*A1d1_1345*(128*q1Dpb*q1Dp1b*pbDp1b-128*q
     #1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b**2-64*q1Dpb**2*mb**2-192*q1Dp1b
     #**2*mb**2)
      s12 = CA**2/den0x2/den0x3*D0_1234*(256*q1Dq2*q1Dp1b*mb**2-128*q1Dp
     #1b*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-256*q1Dq2*q1Dp1b*q2Dp1b-384
     #*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q1Dpb*q2Dpb+256*q1Dp1b*q2Dpb*pbDp1b+
     #256*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q2Dpb+256*q1Dpb*q1Dp1b*q2
     #Dp1b+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb
     #*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbDp1b+256*q1Dq2*q1D
     #pb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1
     #b*q2Dp1b**2-128*q1Dq2**2*mb**2+256*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2
     #Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dp1b-256*q1Dp1b**2*q2Dpb+256*q1Dq2**2
     #*q1Dp1b+128*q1Dpb*q2Dpb**2-128*q1Dq2**2*pbDp1b+128*q1Dq2*mb**4-128
     #*q1Dq2*q1Dp1b**2)
      s10 = s11+s12
      s3 = s9+s10
      s6 = s3+CA**2/den0x1/den0x3*A3e1*(256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+25
     #6*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b*mb**2-128*q1Dq2
     #*q1Dp1b*mb**4-256*q1Dp1b**2*q2Dpb*pbDp1b-256*q1Dp1b**2*q2Dpb*mb**2
     #)
      s7 = s6+CV**2/den0x1/den0x3*A3d2_1245*(128*q2Dpb*mb**4-128*q2Dp1b*
     #pbDp1b*mb**2)
      s8 = s7
      s10 = CA**2/den0x2/den0x3*A2e1*(-256*q1Dq2*q1Dp1b**2*q2Dpb-384*q1D
     #pb**2*q2Dpb*mb**2-128*q1Dq2*pbDp1b**2*mb**2+512*q1Dpb*q1Dp1b**2*q2
     #Dpb+128*q1Dq2*q2Dp1b*mb**4-128*q1Dp1b**2*q2Dp1b*mb**2-256*q1Dpb**2
     #*q2Dpb*pbDp1b+256*q1Dpb**2*q1Dp1b*q2Dpb+256*q1Dp1b**3*q2Dpb-128*q1
     #Dpb**2*q2Dp1b*mb**2-256*q1Dq2*pbDp1b*mb**4+128*q1Dq2*q1Dp1b*mb**4-
     #384*q1Dp1b**2*q2Dpb*mb**2-256*q1Dp1b**2*q2Dpb*pbDp1b-256*q1Dq2*q1D
     #pb*q1Dp1b*q2Dpb-128*q1Dq2*mb**6-768*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q
     #1Dpb*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+384*q1Dq2*q
     #1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*pbDp1b*mb**2+384*q1Dq2*q1Dpb*q2
     #Dpb*mb**2+128*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb*
     #*2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2-512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+256
     #*q1Dq2*q1Dpb*q2Dpb*pbDp1b)
      s11 = CV**2/den0x1/den0x3*A2d1_1235*(-128*q1Dq2*q1Dp1b*mb**2-128*q
     #1Dp1b*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q1Dp1b*q2Dpb+
     #128*q1Dpb*q1Dp1b*q2Dp1b+192*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb
     #**2-256*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*mb**2+512*q1Dp1b*q2Dpb*
     #mb**2+384*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dq2*q2Dp
     #b*q2Dp1b-896*q2Dpb*pbDp1b*mb**2+128*q2Dp1b*pbDp1b*mb**2+256*q1Dpb*
     #q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*mb**2-128*q1Dp
     #1b*q2Dpb**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q2Dp1b*mb**2-640*q2Dp
     #b*mb**4+128*q2Dpb*q2Dp1b*pbDp1b+448*q2Dpb**2*mb**2-256*q1Dp1b**2*q
     #2Dpb-256*q2Dpb*pbDp1b**2+64*q2Dp1b**2*mb**2-256*q2Dpb**2*q2Dp1b+25
     #6*q2Dpb**2*pbDp1b+192*q1Dq2*mb**4+128*q2Dp1b*pbDp1b**2)
      s9 = s10+s11
      s5 = s8+s9
      s7 = s5
      s9 = CV**2/den0x1/den0x3*A7d2_2345*(128*q1Dq2*pbDp1b+128*q1Dq2*mb*
     #*2-128*q1Dpb*q2Dp1b-128*q1Dp1b*q2Dpb-128*q1Dp1b*pbDp1b-128*q1Dp1b*
     #mb**2)
      s11 = CV**2/den0x2
      s13 = 1/den0x3
      s15 = E0
      s17 = 256*q1Dpb*q1Dp1b*q2Dpb**2+512*q1Dq2*q1Dp1b**2*q2Dpb-256*q1Dp
     #b**2*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*mb**4-256*q1Dpb*q2Dpb**2*mb**2+5
     #12*q1Dpb*q1Dp1b**2*q2Dpb+256*q1Dq2*q2Dpb*mb**4+128*q1Dp1b*q2Dpb*mb
     #**4-128*q1Dp1b*q2Dp1b*mb**4-128*q1Dpb*q2Dpb*mb**4-256*q1Dpb**2*q2D
     #pb*pbDp1b+512*q1Dpb**2*q1Dp1b*q2Dpb-256*q1Dp1b**2*q2Dpb*q2Dp1b+256
     #*q1Dp1b*q2Dpb*pbDp1b**2-128*q1Dq2*q1Dp1b*mb**4-128*q1Dq2*q1Dpb*mb*
     #*4-256*q1Dp1b**2*q2Dpb*mb**2
      s16 = s17+256*q1Dp1b*q2Dpb**2*pbDp1b-256*q1Dp1b**2*q2Dpb*pbDp1b-76
     #8*q1Dp1b**2*q2Dpb**2-512*q1Dq2*q1Dpb*q1Dp1b*q2Dpb+256*q1Dq2*q2Dpb*
     #pbDp1b*mb**2+768*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-1024*q1Dpb*q1Dp1b*q2Dpb
     #*mb**2+384*q1Dp1b*q2Dpb*pbDp1b*mb**2-128*q1Dq2*q1Dp1b*pbDp1b*mb**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-512*q1Dp1b*q2Dpb*q2Dp1b*mb**2-1024
     #*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q1Dpb*
     #q2Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*pbDp1b*mb**2-128*q1Dp1b*q2Dp1b
     #*pbDp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x1/den0x3*A4d2_1235*(-128*q1Dpb*q2Dp1b**2+128*q1
     #Dp1b*q2Dpb*q2Dp1b-128*q2Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2+1
     #28*q2Dp1b**2*pbDp1b+128*q2Dp1b**2*mb**2)
      s8 = s7+CV**2/den0x1/den0x3*A7d2_1245*(128*q2Dpb*mb**2-128*q2Dp1b*
     #pbDp1b)
      s9 = s8
      s11 = CV**2/den0x1/den0x3*D0_1245*(-256*q1Dq2*q2Dpb*q2Dp1b+128*q1D
     #q2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbDp1b-64*q1
     #Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2-256*q1
     #Dp1b*q2Dpb**2-64*q1Dp1b*q2Dp1b*mb**2-256*q2Dpb*q2Dp1b*pbDp1b-384*q
     #2Dpb*q2Dp1b*mb**2+128*q2Dpb*q2Dp1b**2+384*q2Dpb*pbDp1b*mb**2+128*q
     #2Dpb*pbDp1b**2+256*q2Dpb*mb**4+128*q2Dpb**2*q2Dp1b-128*q2Dpb**2*pb
     #Dp1b-256*q2Dpb**2*mb**2)
      s12 = CV**2/den0x2/den0x3*A2d1_1235*(128*q1Dq2*q1Dp1b*mb**2+128*q1
     #Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q1Dp1b+12
     #8*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b-320*q1Dq2*pbDp1b*mb*
     #*2+128*q1Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb*mb**2+256*q2Dpb*q2Dp1b*
     #mb**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*pbDp1b-512*q2Dpb*pbDp
     #1b*mb**2-256*q2Dp1b*pbDp1b*mb**2+384*q1Dpb*q2Dpb*mb**2+192*q1Dq2*q
     #1Dpb*mb**2-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q2
     #Dp1b*mb**2-384*q2Dpb*mb**4-256*q2Dp1b*mb**4+320*q2Dpb**2*mb**2-128
     #*q1Dp1b**2*q2Dpb-128*q2Dpb*pbDp1b**2+128*q1Dpb**2*q2Dp1b+64*q2Dp1b
     #**2*mb**2+128*q2Dpb**2*pbDp1b-192*q1Dq2*mb**4)
      s10 = s11+s12
      s4 = s9+s10
      s6 = s4+CA**2/den0x2/den0x3*A2d1_1245*(-128*q1Dq2*q1Dp1b*mb**2+192
     #*q1Dq2*pbDp1b*mb**2+192*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb-192*q1D
     #pb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2+192*q2Dpb*pbDp1b*mb**2+320*q2
     #Dpb*mb**4+64*q2Dp1b*pbDp1b*mb**2+192*q2Dp1b*mb**4)+CV**2/den0x2/de
     #n0x3*A2d2_1345*(-128*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128
     #*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b**2*mb**2)
      s7 = s6+CV**2/den0x2/den0x3*D0_1345*(-256*q1Dq2*q1Dpb*q1Dp1b+128*q
     #1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+1
     #28*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*mb
     #**2+128*q1Dpb*q1Dp1b**2-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb**2*q1Dp1b-
     #128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b*
     #mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4-256*q1Dp1b**2*q2Dpb
     #-128*q1Dp1b**2*mb**2)
      s8 = s7
      s10 = CV**2/den0x1/den0x3*A2d1_1234*(192*q1Dq2*q1Dp1b*mb**2-128*q1
     #Dp1b*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b-1
     #28*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*q2
     #Dp1b-384*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q2Dp1
     #b*pbDp1b+64*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*mb**2+128*q1Dpb*q2D
     #pb*pbDp1b+128*q1Dq2*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*
     #q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b**2+64*q1Dq2*q1Dpb*mb**2+128*q1Dp1b
     #*q2Dpb**2+128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q2D
     #p1b*pbDp1b+192*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dp1b-128*q1Dq2*q
     #2Dp1b**2+128*q1Dpb*q2Dp1b**2-256*q1Dq2*mb**4)
      s11 = CV**2/den0x2/den0x3*D0_2345*(-576*q1Dq2*q1Dp1b*mb**2+320*q1D
     #p1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dp1b+384*q1Dq2*q1Dpb*q1Dp1b-12
     #8*q1Dp1b*q2Dpb*pbDp1b-384*q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q1Dp1b*q2D
     #pb+128*q1Dpb*q1Dp1b*mb**2-384*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dq2*pbDp1b*
     #mb**2-128*q1Dp1b*pbDp1b*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q1Dp
     #1b*q2Dpb-128*q1Dpb**2*q1Dp1b-128*q1Dq2*q2Dpb*mb**2+192*q1Dp1b*q2Dp
     #b*mb**2+192*q1Dpb*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*
     #q2Dp1b**2+64*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb**2+128*q1Dpb*q1Dp1
     #b**2+256*q1Dpb*q1Dp1b*pbDp1b+256*q1Dp1b**2*q2Dpb-256*q1Dq2**2*q1Dp
     #1b-64*q1Dp1b**2*mb**2-128*q1Dp1b*mb**4-64*q1Dq2*mb**4-64*q1Dpb**2*
     #mb**2+128*q1Dq2*q1Dp1b**2)
      s9 = s10+s11
      s5 = s8+s9
      s7 = s5+CA**2/den0x2/den0x3*A6d2_2345*(-256*q1Dpb*q2Dpb*q2Dp1b-256
     #*q1Dq2*q1Dpb*q1Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dq2*q1Dp1b*pbDp1
     #b-256*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q
     #2Dp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2-640*q1Dpb*pbD
     #p1b*mb**2+256*q1Dpb*q2Dp1b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb+256*q1Dpb*
     #*2*q1Dp1b-256*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*mb**2-384*q1Dp1b*
     #q2Dpb*mb**2+512*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*q1Dpb*pbDp1b+384*q1Dp
     #b*q2Dpb*mb**2+256*q1Dq2**2*mb**2-512*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*
     #q2Dpb**2-256*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2+128*q1Dpb*q2Dp
     #1b*pbDp1b+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2Dp1b+256*q1Dq2**2*pbD
     #p1b-384*q1Dpb*mb**4-128*q1Dp1b**2*mb**2+128*q1Dp1b*mb**4-256*q1Dpb
     #*pbDp1b**2+256*q1Dq2*mb**4+256*q1Dpb**2*pbDp1b+256*q1Dpb**2*mb**2)
      s8 = s7
      s10 = CV**2/den0x2/den0x3*A2e1*(-256*q1Dq2*q1Dp1b**2*q2Dpb-384*q1D
     #pb**2*q2Dpb*mb**2-128*q1Dq2*pbDp1b**2*mb**2+512*q1Dpb*q1Dp1b**2*q2
     #Dpb+128*q1Dq2*q2Dp1b*mb**4-128*q1Dp1b**2*q2Dp1b*mb**2-256*q1Dpb**2
     #*q2Dpb*pbDp1b+256*q1Dpb**2*q1Dp1b*q2Dpb+256*q1Dp1b**3*q2Dpb-128*q1
     #Dpb**2*q2Dp1b*mb**2-256*q1Dq2*pbDp1b*mb**4+128*q1Dq2*q1Dp1b*mb**4-
     #384*q1Dp1b**2*q2Dpb*mb**2-256*q1Dp1b**2*q2Dpb*pbDp1b-256*q1Dq2*q1D
     #pb*q1Dp1b*q2Dpb-128*q1Dq2*mb**6-768*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q
     #1Dpb*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+384*q1Dq2*q
     #1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*pbDp1b*mb**2+384*q1Dq2*q1Dpb*q2
     #Dpb*mb**2+128*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb*
     #*2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2-512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+256
     #*q1Dq2*q1Dpb*q2Dpb*pbDp1b)
      s11 = CV**2/den0x1/den0x3*D0_1345*(128*q1Dq2*q1Dpb*pbDp1b+128*q1Dq
     #2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dpb-256*q1Dq2*pbDp1b*mb**2-128*q1
     #Dq2*pbDp1b**2-128*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1D
     #p1b*mb**2+64*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*pbD
     #p1b*mb**2-128*q1Dpb*mb**4+64*q1Dpb**2*mb**2+128*q1Dp1b*q2Dpb*q2Dp1
     #b+64*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb**2+64*q1Dp1b*q2Dp1b*mb**2
     #-256*q1Dp1b*pbDp1b*mb**2-256*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb+64*q
     #1Dp1b**2*mb**2)
      s9 = s10+s11
      s6 = s8+s9
      s7 = s6+CV**2/den0x1/den0x3*A4d2_1345*(128*q1Dq2*q1Dp1b*pbDp1b+128
     #*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dp1b**2*q2Dpb-12
     #8*q1Dp1b**2*pbDp1b-128*q1Dp1b**2*mb**2)
      s8 = s7+CA**2/den0x1/den0x3*A4d2_1345*(128*q1Dq2*q1Dp1b*pbDp1b+128
     #*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dp1b**2*q2Dpb-12
     #8*q1Dp1b**2*pbDp1b-128*q1Dp1b**2*mb**2)
      s2 = s8+CV**2/den0x2/den0x3*A3d1_1235*(128*q1Dq2*q1Dpb*q1Dp1b-128*
     #q1Dq2*q1Dpb*pbDp1b-192*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b+1
     #92*q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2-128*q1Dpb*q1Dp1b*q2Dpb-1
     #28*q1Dpb*q2Dpb*pbDp1b-192*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1
     #b-192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp
     #1b-256*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b**2*q2
     #Dpb+320*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+64*q2Dpb*mb**4+192*
     #q2Dp1b*pbDp1b*mb**2+64*q2Dp1b*mb**4)+CA**2/den0x2/den0x3*A1d1_2345
     #*(64*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*pb
     #Dp1b-128*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b**2-64*q1Dpb**2*mb**2-
     #192*q1Dp1b**2*mb**2)
      s7 = CA**2/den0x2/den0x3*A3d1_2345*(192*q1Dq2*q1Dp1b*mb**2-192*q1D
     #p1b*q2Dp1b*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q1Dp1b+128
     #*q1Dp1b*q2Dpb*pbDp1b-128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q2Dp
     #b+64*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b+320*q1Dq2*pbDp1b*m
     #b**2+192*q1Dp1b*pbDp1b*mb**2-576*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q1Dp
     #1b*q2Dpb-128*q1Dpb**2*q1Dp1b-128*q1Dq2*q2Dpb*pbDp1b+64*q1Dq2*q2Dpb
     #*mb**2-256*q1Dp1b*q2Dpb*mb**2+384*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*q1D
     #pb*pbDp1b+64*q1Dpb*q2Dpb*mb**2+256*q1Dq2**2*mb**2-448*q1Dq2*q1Dpb*
     #mb**2+256*q1Dp1b*q2Dpb**2+128*q1Dq2*pbDp1b**2+128*q1Dpb*q2Dp1b*pbD
     #p1b+256*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2Dp1b
     #+256*q1Dq2**2*pbDp1b-320*q1Dpb*mb**4-128*q1Dp1b**2*mb**2+192*q1Dp1
     #b*mb**4-256*q1Dpb*pbDp1b**2+256*q1Dq2*mb**4+192*q1Dpb**2*mb**2)
      s8 = CV**2/den0x1/den0x3*A5d2_1245*(-128*q2Dpb*q2Dp1b*mb**2+128*q2
     #Dp1b**2*pbDp1b)
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*A1e1*(256*q1Dq2*q1Dpb*q1Dp1b*q2Dpb-256
     #*q1Dq2*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q1
     #Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-384*q1Dq2*q1Dp1b*q2
     #Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b**2*q2Dpb-
     #128*q1Dq2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**4)
      s5 = s7+CA**2/den0x1/den0x3*A3d1_1345*(-128*q1Dq2*q2Dpb*pbDp1b-64*
     #q1Dq2*pbDp1b*mb**2-64*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb+64*q1Dpb*
     #q1Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+128*q1D
     #pb*q2Dp1b*pbDp1b+192*q1Dpb*q2Dp1b*mb**2-192*q1Dpb*pbDp1b*mb**2-320
     #*q1Dpb*mb**4+64*q1Dpb**2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b-192*q1Dp1b*
     #q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*mb**2+192*q1Dp1b
     #*pbDp1b*mb**2+128*q1Dp1b*pbDp1b**2-64*q1Dp1b*mb**4)+CV**2/den0x1/d
     #en0x3*A1d1_1245*(-256*q2Dpb*q2Dp1b*mb**2+256*q2Dpb**2*q2Dp1b-128*q
     #2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp1b**2*mb**2)
      s6 = s5+CV**2/den0x2/den0x3*A2d1_1245*(-128*q1Dq2*q1Dp1b*mb**2+192
     #*q1Dq2*pbDp1b*mb**2+192*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb-192*q1D
     #pb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2+192*q2Dpb*pbDp1b*mb**2+320*q2
     #Dpb*mb**4+64*q2Dp1b*pbDp1b*mb**2+192*q2Dp1b*mb**4)+CV**2/den0x2/de
     #n0x3*A3d1_1234*(256*q1Dq2*q1Dpb*q1Dp1b+128*q1Dq2*q1Dpb*pbDp1b+320*
     #q1Dq2*q1Dpb*mb**2+192*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-12
     #8*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb**2*q2Dp1b+128*q1Dp1b**2*q2Dpb)
      s7 = s6+CA**2/den0x2/den0x3*A7d2_2345*(256*q1Dpb*q1Dp1b+256*q1Dpb*
     #pbDp1b+128*q1Dpb*mb**2-128*q1Dp1b*mb**2)
      s8 = s7+CA**2/den0x1/den0x3*A2d1_1234*(192*q1Dq2*q1Dp1b*mb**2-128*
     #q1Dp1b*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b
     #-128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*
     #q2Dp1b-384*q1Dq2*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q2D
     #p1b*pbDp1b+64*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dpb*mb**2+128*q1Dpb*q
     #2Dpb*pbDp1b+128*q1Dq2*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*mb**2-128*q1Dp1
     #b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b**2+64*q1Dq2*q1Dpb*mb**2+128*q1Dp
     #1b*q2Dpb**2+128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dq2*pbDp1b**2-128*q1Dpb*q
     #2Dp1b*pbDp1b+192*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dp1b-128*q1Dq2
     #*q2Dp1b**2+128*q1Dpb*q2Dp1b**2-256*q1Dq2*mb**4)
      s9 = s8
      s11 = CA**2/den0x2/den0x3*D0_1235*(128*q1Dq2*q1Dpb*q2Dpb+64*q1Dq2*
     #q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2*q1Dp1b*mb**2+128*q1Dq2
     #*q2Dpb*pbDp1b+256*q1Dq2*q2Dpb*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+128*q1D
     #pb*q2Dpb*pbDp1b+64*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dpb**2+64*q1Dpb*q
     #2Dp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+64*q1Dp1b*q2
     #Dp1b*mb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dpb*pbDp1b*mb**2+128*q2Dpb
     #*mb**4-128*q2Dpb**2*pbDp1b-256*q2Dpb**2*mb**2)
      s12 = CV**2/den0x1/den0x3*A3d1_1235*(128*q1Dq2*q1Dp1b*mb**2+128*q1
     #Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-256*q1Dq2*q1Dp1b*q2Dpb-12
     #8*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dq2*pbDp1b*mb**2+64*q1Dpb*q2Dp1b*mb**2+
     #256*q1Dq2*q2Dpb*mb**2-512*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbDp1
     #b+448*q2Dpb*pbDp1b*mb**2-64*q2Dp1b*pbDp1b*mb**2-320*q1Dpb*q2Dpb*mb
     #**2+128*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb
     #*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dpb*q2Dp1b*pbDp1b+320*q2Dpb*mb**
     #4-128*q2Dpb*q2Dp1b*pbDp1b+64*q2Dp1b*mb**4-128*q2Dpb**2*mb**2+256*q
     #1Dp1b**2*q2Dpb+128*q2Dpb*pbDp1b**2-128*q1Dq2*mb**4-128*q2Dp1b*pbDp
     #1b**2)
      s10 = s11+s12
      s4 = s9+s10
      s7 = s4
      s9 = CA**2/den0x2/den0x3*A5d2_2345*(256*q1Dq2*q1Dpb*q1Dp1b+128*q1D
     #q2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*q
     #1Dpb*q1Dp1b*mb**2-256*q1Dpb**2*q1Dp1b+128*q1Dpb**2*q2Dp1b-256*q1Dp
     #b**2*pbDp1b-128*q1Dpb**2*mb**2)
      s10 = CA**2/den0x1/den0x3*D0_2345*(256*q1Dq2*q1Dp1b*mb**2-256*q1Dp
     #1b*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b+12
     #8*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*mb**
     #2-256*q1Dpb*q1Dp1b*q2Dp1b+192*q1Dq2*pbDp1b*mb**2+256*q1Dp1b*pbDp1b
     #*mb**2+128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q2D
     #pb*pbDp1b-128*q1Dq2*q2Dp1b*pbDp1b-64*q1Dq2*q2Dpb*mb**2-384*q1Dp1b*
     #q2Dpb*mb**2-128*q1Dpb*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp
     #1b*q2Dpb**2+128*q1Dq2*pbDp1b**2-64*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**
     #2*q2Dpb-128*q1Dp1b**2*q2Dp1b-128*q1Dp1b**2*pbDp1b+128*q1Dpb*mb**4-
     #256*q1Dp1b**2*mb**2+256*q1Dp1b*mb**4+64*q1Dq2*mb**4)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*A7d2_1235*(-128*q2Dpb*mb**2+128*q2Dp1b
     #*pbDp1b)
      s8 = s7
      s10 = CV**2/den0x2/den0x3*D0_1234*(256*q1Dq2*q1Dp1b*mb**2-128*q1Dp
     #1b*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-256*q1Dq2*q1Dp1b*q2Dp1b-384
     #*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q1Dpb*q2Dpb+256*q1Dp1b*q2Dpb*pbDp1b+
     #256*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q2Dpb+256*q1Dpb*q1Dp1b*q2
     #Dp1b+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dpb
     #*mb**2+256*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q2Dpb*pbDp1b+256*q1Dq2*q1D
     #pb*pbDp1b-128*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1
     #b*q2Dp1b**2-128*q1Dq2**2*mb**2+256*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2
     #Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dp1b-256*q1Dp1b**2*q2Dpb+256*q1Dq2**2
     #*q1Dp1b+128*q1Dpb*q2Dpb**2-128*q1Dq2**2*pbDp1b+128*q1Dq2*mb**4-128
     #*q1Dq2*q1Dp1b**2)
      s11 = CV**2/den0x2/den0x3*A5d2_2345*(256*q1Dq2*q1Dpb*q1Dp1b+128*q1
     #Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*
     #q1Dpb*q1Dp1b*mb**2-256*q1Dpb**2*q1Dp1b+128*q1Dpb**2*q2Dp1b-256*q1D
     #pb**2*pbDp1b-128*q1Dpb**2*mb**2)
      s9 = s10+s11
      s5 = s8+s9
      s7 = s5
      s9 = CA**2/den0x1/den0x3*A4d2_1235*(-128*q1Dpb*q2Dp1b**2+128*q1Dp1
     #b*q2Dpb*q2Dp1b-128*q2Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2+128*
     #q2Dp1b**2*pbDp1b+128*q2Dp1b**2*mb**2)
      s10 = CV**2/den0x1/den0x3*A2d1_2345*(192*q1Dq2*q1Dp1b*mb**2-192*q1
     #Dp1b*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b+1
     #28*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*mb
     #**2+128*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dq2*pbDp1b*mb**2+256*q1Dpb*pbDp1b
     #*mb**2-320*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-192*q1Dq2*q2D
     #pb*mb**2+64*q1Dp1b*q2Dpb*mb**2-256*q1Dq2*q2Dpb*q2Dp1b-64*q1Dpb*q2D
     #pb*mb**2+128*q1Dp1b*q2Dp1b**2+128*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2D
     #pb*q2Dp1b-128*q1Dp1b*q2Dpb**2+128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbD
     #p1b**2-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dpb*q1
     #Dp1b*pbDp1b-128*q1Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b+128*q1Dp1b**2
     #*q2Dp1b-128*q1Dq2*q2Dp1b**2+256*q1Dpb*mb**4+128*q1Dp1b*mb**4+128*q
     #1Dpb*q2Dp1b**2-64*q1Dq2*mb**4-128*q1Dpb**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x2/den0x3*A2d1_1234*(-256*q1Dq2*q1Dpb*q1Dp1b-256
     #*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*q1Dpb*pbDp1b-64*q1Dq2*q1Dpb*mb**2-19
     #2*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-320*q1Dq2*q2Dpb*mb**2-
     #192*q1Dq2*q2Dp1b*mb**2+640*q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2+
     #512*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b-128
     #*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+
     #128*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b+128*q1Dp1b*q2Dpb*pbDp1b
     #+128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dp1b*q2Dp1b
     #*mb**2-128*q1Dp1b**2*q2Dpb)
      s3 = s7+CV**2/den0x2/den0x3*A1d1_1245*(-256*q1Dpb*q2Dpb*q2Dp1b-256
     #*q2Dpb*q2Dp1b*mb**2-128*q2Dpb**2*pbDp1b-320*q2Dpb**2*mb**2-64*q2Dp
     #1b**2*mb**2)+CA**2/den0x1/den0x3*A5d2_1245*(-128*q2Dpb*q2Dp1b*mb**
     #2+128*q2Dp1b**2*pbDp1b)+CA**2/den0x1/den0x3*A4d2_1245*(-128*q2Dpb*
     #q2Dp1b*pbDp1b+128*q2Dp1b**2*mb**2)
      s7 = s3
      s9 = CA**2/den0x1/den0x3*A3d2_1245*(128*q2Dpb*mb**4-128*q2Dp1b*pbD
     #p1b*mb**2)
      s10 = CV**2/den0x1/den0x3*A3d1_2345*(-192*q1Dq2*q1Dp1b*mb**2-64*q1
     #Dp1b*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-1
     #28*q1Dq2*q1Dp1b*pbDp1b+192*q1Dpb*q1Dp1b*mb**2+64*q1Dq2*pbDp1b*mb**
     #2+192*q1Dp1b*pbDp1b*mb**2-192*q1Dpb*pbDp1b*mb**2+128*q1Dpb*q2Dp1b*
     #mb**2+128*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*pbDp1b+320*q1Dq2*q2D
     #pb*mb**2+256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbDp1b+64*q1Dpb*q2
     #Dpb*mb**2-128*q1Dp1b*q2Dp1b**2+128*q1Dq2**2*mb**2-192*q1Dq2*q1Dpb*
     #mb**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2-128*q1Dq2*q1Dpb*
     #q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+128*q1Dpb*q1
     #Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2+128*q1Dpb**2*q2Dp1b+128*q1Dq2*q2D
     #p1b**2+128*q1Dq2**2*pbDp1b-320*q1Dpb*mb**4-64*q1Dp1b*mb**4-128*q1D
     #pb*q2Dp1b**2+128*q1Dq2*mb**4+64*q1Dpb**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*A6d2_1345*(128*q1Dpb*q2Dpb*mb**2-128*q
     #1Dpb*q2Dp1b*mb**2+128*q1Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4-128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*
     #q1Dp1b*pbDp1b**2)
      s8 = s7
      s11 = CV**2/den0x2
      s13 = 1/den0x3
      s15 = A2d1_2345
      s16 = 64*q1Dq2*q1Dp1b*mb**2+64*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb
     #*q2Dp1b-128*q1Dq2*q1Dpb*q1Dp1b+256*q1Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q
     #1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b-192*q1Dq2*pbDp1b*mb**2+128*q1D
     #p1b*pbDp1b*mb**2+640*q1Dpb*pbDp1b*mb**2-64*q1Dpb*q2Dp1b*mb**2+128*
     #q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q1Dp1b+128*q1Dq2*q2Dpb*pbDp1b-64*q
     #1Dq2*q2Dpb*mb**2+192*q1Dp1b*q2Dpb*mb**2-384*q1Dpb*q2Dpb*pbDp1b+384
     #*q1Dq2*q1Dpb*pbDp1b-320*q1Dpb*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b
     #-256*q1Dq2**2*mb**2+512*q1Dq2*q1Dpb*mb**2-256*q1Dp1b*q2Dpb**2-128*
     #q1Dq2*pbDp1b**2-128*q1Dpb*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**2-128
     #*q1Dpb*q1Dp1b**2-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2-128*
     #q1Dp1b**2*q2Dpb-128*q1Dpb**2*q2Dp1b-256*q1Dq2**2*pbDp1b+384*q1Dpb*
     #mb**4+256*q1Dp1b**2*mb**2+256*q1Dpb*pbDp1b**2-64*q1Dq2*mb**4-128*q
     #1Dpb**2*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s11 = CA**2/den0x1/den0x3*A2d2_1345*(128*q1Dp1b*q2Dpb*mb**2-128*q1
     #Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x1/den0x3*D0_1345*(128*q1Dq2*q1Dpb*pbDp1b+128*q1
     #Dq2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dpb-256*q1Dq2*pbDp1b*mb**2-128*
     #q1Dq2*pbDp1b**2-128*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q
     #1Dp1b*mb**2+64*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*p
     #bDp1b*mb**2-128*q1Dpb*mb**4+64*q1Dpb**2*mb**2+128*q1Dp1b*q2Dpb*q2D
     #p1b+64*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb**2+64*q1Dp1b*q2Dp1b*mb*
     #*2-256*q1Dp1b*pbDp1b*mb**2-256*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb+64
     #*q1Dp1b**2*mb**2)+CV**2/den0x1/den0x3*A3d1_1234*(-128*q1Dq2*q1Dpb*
     #q2Dp1b-64*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-128*q1Dq2*q1Dp
     #1b*pbDp1b-192*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dp1
     #b**2*q2Dp1b)
      s7 = s6+CA**2/den0x1/den0x3*A3d1_2345*(-192*q1Dq2*q1Dp1b*mb**2-64*
     #q1Dp1b*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b
     #-128*q1Dq2*q1Dp1b*pbDp1b+192*q1Dpb*q1Dp1b*mb**2+64*q1Dq2*pbDp1b*mb
     #**2+192*q1Dp1b*pbDp1b*mb**2-192*q1Dpb*pbDp1b*mb**2+128*q1Dpb*q2Dp1
     #b*mb**2+128*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*pbDp1b+320*q1Dq2*q
     #2Dpb*mb**2+256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbDp1b+64*q1Dpb*
     #q2Dpb*mb**2-128*q1Dp1b*q2Dp1b**2+128*q1Dq2**2*mb**2-192*q1Dq2*q1Dp
     #b*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2-128*q1Dq2*q1Dp
     #b*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+128*q1Dpb*
     #q1Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2+128*q1Dpb**2*q2Dp1b+128*q1Dq2*q
     #2Dp1b**2+128*q1Dq2**2*pbDp1b-320*q1Dpb*mb**4-64*q1Dp1b*mb**4-128*q
     #1Dpb*q2Dp1b**2+128*q1Dq2*mb**4+64*q1Dpb**2*mb**2)
      s8 = s7+CV**2/den0x1/den0x3*A2d2_2345*(-256*q1Dq2*q1Dp1b*mb**2+128
     #*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-256*q1Dq2*q1Dp1b*pbDp1
     #b-128*q1Dq2*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q
     #2Dp1b+256*q1Dq2*pbDp1b*mb**2-256*q1Dp1b*pbDp1b*mb**2-256*q1Dpb*pbD
     #p1b*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dq2*
     #q2Dp1b*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbDp1b+128*q1
     #Dp1b*q2Dp1b*pbDp1b+128*q1Dq2**2*mb**2-128*q1Dq2*q1Dpb*mb**2+256*q1
     #Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2+128*q
     #1Dpb*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*pbDp1b-
     #128*q1Dp1b*pbDp1b**2+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2Dp1b+128*q
     #1Dp1b**2*pbDp1b+128*q1Dq2**2*pbDp1b-128*q1Dpb*mb**4+128*q1Dp1b**2*
     #mb**2-128*q1Dp1b*mb**4-128*q1Dpb*pbDp1b**2+128*q1Dq2*mb**4)
      s9 = s8
      s11 = CA**2/den0x1/den0x3*A2d1_2345*(192*q1Dq2*q1Dp1b*mb**2-192*q1
     #Dp1b*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dp1b+1
     #28*q1Dp1b*q2Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*mb
     #**2+128*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dq2*pbDp1b*mb**2+256*q1Dpb*pbDp1b
     #*mb**2-320*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-192*q1Dq2*q2D
     #pb*mb**2+64*q1Dp1b*q2Dpb*mb**2-256*q1Dq2*q2Dpb*q2Dp1b-64*q1Dpb*q2D
     #pb*mb**2+128*q1Dp1b*q2Dp1b**2+128*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2D
     #pb*q2Dp1b-128*q1Dp1b*q2Dpb**2+128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*pbD
     #p1b**2-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dpb*q1
     #Dp1b*pbDp1b-128*q1Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b+128*q1Dp1b**2
     #*q2Dp1b-128*q1Dq2*q2Dp1b**2+256*q1Dpb*mb**4+128*q1Dp1b*mb**4+128*q
     #1Dpb*q2Dp1b**2-64*q1Dq2*mb**4-128*q1Dpb**2*mb**2)
      s12 = CA**2/den0x1/den0x3*A1d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+64*q1D
     #q2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128
     #*q1Dq2*q1Dp1b*mb**2-128*q1Dq2**2*pbDp1b-128*q1Dq2**2*mb**2-256*q1D
     #pb*q1Dp1b*q2Dp1b+64*q1Dpb**2*mb**2-128*q1Dp1b**2*q2Dp1b-128*q1Dp1b
     #**2*pbDp1b-192*q1Dp1b**2*mb**2)
      s10 = s11+s12
      s4 = s9+s10
      s6 = s4+CA**2/den0x2/den0x3*A1d1_1235*(64*q1Dq2*q2Dp1b*mb**2-256*q
     #1Dpb*q2Dpb*q2Dp1b-256*q2Dpb*q2Dp1b*mb**2-128*q2Dpb**2*pbDp1b-320*q
     #2Dpb**2*mb**2-64*q2Dp1b**2*mb**2)+CA**2/den0x2/den0x3*A6d2_1345*(2
     #56*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-256*q1Dpb*pbDp1b**2-1
     #28*q1Dpb*mb**4-128*q1Dpb**2*mb**2+128*q1Dp1b*pbDp1b*mb**2-128*q1Dp
     #1b*mb**4-128*q1Dp1b**2*mb**2)
      s5 = s6+CV**2/den0x1/den0x3*A5d2_1235*(128*q1Dpb*q2Dp1b**2-128*q1D
     #p1b*q2Dpb*q2Dp1b+128*q2Dpb*q2Dp1b*mb**2-128*q2Dp1b**2*pbDp1b)+CV**
     #2/den0x1/den0x3*A3d1_1245*(128*q1Dq2*q2Dpb*mb**2-64*q1Dq2*pbDp1b*m
     #b**2-64*q1Dq2*mb**4-128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb*mb**2+
     #128*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b*p
     #bDp1b-128*q2Dpb*q2Dp1b*mb**2+448*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp
     #1b**2+320*q2Dpb*mb**4-128*q2Dpb**2*mb**2-64*q2Dp1b*pbDp1b*mb**2-12
     #8*q2Dp1b*pbDp1b**2+64*q2Dp1b*mb**4+128*q2Dp1b**2*pbDp1b)+CV**2/den
     #0x1/den0x3*A5d2_1345*(128*q1Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2
     #-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*m
     #b**2-128*q1Dpb**2*q2Dp1b)
      s7 = s5+CV**2/den0x1/den0x3*A2d1_1345*(-128*q1Dq2*q2Dpb*mb**2-64*q
     #1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2+64*q1Dq2*mb**4+64*q1Dpb*q1Dp
     #1b*mb**2+64*q1Dpb*q2Dp1b*mb**2-64*q1Dpb*pbDp1b*mb**2+64*q1Dpb*mb**
     #4+128*q1Dp1b*q2Dpb*pbDp1b+64*q1Dp1b*q2Dpb*mb**2-192*q1Dp1b*pbDp1b*
     #mb**2-64*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb+64*q1Dp1b**2*mb**2)
      s8 = s7
      s10 = CV**2/den0x1/den0x3*A6d2_2345*(128*q1Dq2*q1Dp1b*mb**2-512*q1
     #Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*q2Dpb-12
     #8*q1Dpb*q1Dp1b*mb**2-384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*pbDp1b*mb**
     #2+384*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q1Dp1b*q
     #2Dpb+256*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dp
     #b*mb**2+512*q1Dq2*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*pbDp1b-128*q1Dpb*q2
     #Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2**2*mb**2+128*q1Dq2*q1
     #Dpb*mb**2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dp1b-256*q1Dq2
     #*pbDp1b**2-256*q1Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dp
     #b*q1Dp1b*pbDp1b+128*q1Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b-128*q1Dq2
     #**2*pbDp1b+128*q1Dpb*mb**4+256*q1Dpb*pbDp1b**2-128*q1Dq2*mb**4)
      s11 = CV**2/den0x1/den0x3*A5d2_2345*(-128*q1Dq2*q1Dpb*q2Dp1b-128*q
     #1Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128
     #*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dq2**2*pbDp1b+12
     #8*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*pbDp1b+12
     #8*q1Dpb*q1Dp1b*mb**2+128*q1Dpb**2*q2Dp1b)
      s9 = s10+s11
      s6 = s8+s9
      s7 = s6+CV**2/den0x1/den0x3*A1d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+64*q
     #1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+1
     #28*q1Dq2*q1Dp1b*mb**2-128*q1Dq2**2*pbDp1b-128*q1Dq2**2*mb**2-256*q
     #1Dpb*q1Dp1b*q2Dp1b+64*q1Dpb**2*mb**2-128*q1Dp1b**2*q2Dp1b-128*q1Dp
     #1b**2*pbDp1b-192*q1Dp1b**2*mb**2)
      s8 = s7+CV**2/den0x2/den0x3*A7d2_1345*(-256*q1Dpb*q1Dp1b-256*q1Dpb
     #*pbDp1b-128*q1Dpb*mb**2+128*q1Dp1b*mb**2)
      t0 = s8+CV**2/den0x2/den0x3*A3d1_2345*(192*q1Dq2*q1Dp1b*mb**2-192*
     #q1Dp1b*q2Dp1b*mb**2-256*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q1Dp1b+
     #128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*q
     #2Dpb+64*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b+320*q1Dq2*pbDp1
     #b*mb**2+192*q1Dp1b*pbDp1b*mb**2-576*q1Dpb*pbDp1b*mb**2-128*q1Dpb*q
     #1Dp1b*q2Dpb-128*q1Dpb**2*q1Dp1b-128*q1Dq2*q2Dpb*pbDp1b+64*q1Dq2*q2
     #Dpb*mb**2-256*q1Dp1b*q2Dpb*mb**2+384*q1Dpb*q2Dpb*pbDp1b-384*q1Dq2*
     #q1Dpb*pbDp1b+64*q1Dpb*q2Dpb*mb**2+256*q1Dq2**2*mb**2-448*q1Dq2*q1D
     #pb*mb**2+256*q1Dp1b*q2Dpb**2+128*q1Dq2*pbDp1b**2+128*q1Dpb*q2Dp1b*
     #pbDp1b+256*q1Dq2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb+128*q1Dpb**2*q2D
     #p1b+256*q1Dq2**2*pbDp1b-320*q1Dpb*mb**4-128*q1Dp1b**2*mb**2+192*q1
     #Dp1b*mb**4-256*q1Dpb*pbDp1b**2+256*q1Dq2*mb**4+192*q1Dpb**2*mb**2)
     #+s2
c
      wbb_pent1c=1d0/4d0/9d0*t0
c
      return
      end
c
      real * 8 function wbb_pent1(q1,q2,pb,p1b,pw,mb,
     - E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     - A1d2_2345,A2d2_2345,A3d2_2345,A4d2_2345,A5d2_2345,
     - A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     - A1d2_1345,A2d2_1345,A3d2_1345,A4d2_1345,A5d2_1345,
     - A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     - A1d2_1245,A2d2_1245,A3d2_1245,A4d2_1245,A5d2_1245,
     - A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     - A1d2_1235,A2d2_1235,A3d2_1235,A4d2_1235,A5d2_1235,
     - A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     - A1d2_1234,A2d2_1234,A3d2_1234,A4d2_1234,A5d2_1234,
     - A6d2_1234,A7d2_1234)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 q1,q2,pb,p1b,pw,mb,den0x1,den0x2,den0x3
      real *8 E0,A1e1,A2e1,A3e1,A4e1,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,A1d2_2345,A2d2_2345,
     - A3d2_2345,A4d2_2345,A5d2_2345,A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,A1d2_1345,A2d2_1345,
     - A3d2_1345,A4d2_1345,A5d2_1345,A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,A1d2_1245,A2d2_1245,
     - A3d2_1245,A4d2_1245,A5d2_1245,A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,A1d2_1235,A2d2_1235,
     - A3d2_1235,A4d2_1235,A5d2_1235,A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,A1d2_1234,A2d2_1234,
     - A3d2_1234,A4d2_1234,A5d2_1234,A6d2_1234,A7d2_1234
c       
      real*8 q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b
      real*8 s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16,
     -  s17,t0,CA,CV
c
      CV=1d0
      CA=-1d0
c
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
c
      s7 = CA**2/den0x2/den0x3*A1e1*(-256*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b+128*
     #q1Dq2*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q1
     #Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dpb**2*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*
     #mb**2+256*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*q2Dp1b*mb**2
     #+128*q1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4)
      s8 = CV**2/den0x2/den0x3*A2e1*(384*q1Dp1b**2*q2Dp1b*mb**2+256*q1Dp
     #b**2*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1b**2*q2Dp1b+256*q1Dq2*q1Dpb**2*q
     #2Dp1b+256*q1Dp1b**2*q2Dp1b*pbDp1b-128*q1Dq2*q1Dpb*mb**4-512*q1Dpb*
     #*2*q1Dp1b*q2Dp1b+128*q1Dq2*pbDp1b**2*mb**2+128*q1Dp1b**2*q2Dpb*mb*
     #*2+256*q1Dq2*pbDp1b*mb**4+384*q1Dpb**2*q2Dp1b*mb**2-128*q1Dq2*q2Dp
     #b*mb**4+128*q1Dpb**2*q2Dpb*mb**2+256*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b+256
     #*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb**3*q2Dp1b+128*q1Dq2*mb**6+768*
     #q1Dpb*q1Dp1b*q2Dp1b*mb**2-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-384*q1Dq2*
     #q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b*mb**2-128*q1Dq2*q1Dpb*q2
     #Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb*mb**2-384*q1Dq2*q1Dp1b*q2Dp1b*mb*
     #*2-128*q1Dq2*q2Dpb*pbDp1b*mb**2+512*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-256
     #*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b)
      s6 = s7+s8
      s7 = s6+CV**2/den0x2/den0x3*A4e1*(-128*q1Dq2*pbDp1b*mb**4-128*q1Dq
     #2*mb**6-128*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1
     #b-384*q1Dpb*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb**2*q1Dp1b*q2Dp1b-128*q1D
     #pb**2*q2Dpb*mb**2-256*q1Dpb**2*q2Dp1b*pbDp1b-384*q1Dpb**2*q2Dp1b*m
     #b**2+256*q1Dpb**3*q2Dp1b)
      s5 = s7+CA**2/den0x1/den0x3*D0_1235*(-256*q1Dq2*q1Dpb*q2Dp1b-256*q
     #2Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dpb*mb**2-6
     #4*q1Dq2*q2Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dp1b+512*q1Dq2*q2Dp1b*pbDp1
     #b-384*q1Dp1b*q2Dp1b*mb**2+384*q2Dp1b*pbDp1b*mb**2+704*q1Dq2*q2Dp1b
     #*mb**2-128*q1Dq2*q2Dp1b**2+128*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*q2Dpb*
     #q2Dp1b-384*q1Dpb*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dp1b*pb
     #Dp1b**2+128*q1Dpb**2*q2Dp1b+64*q2Dp1b**2*mb**2-256*q1Dp1b*q2Dp1b*p
     #bDp1b-384*q1Dq2*q2Dpb*q2Dp1b+256*q2Dp1b*mb**4+64*q2Dpb**2*mb**2+25
     #6*q1Dq2**2*q2Dp1b+128*q1Dp1b**2*q2Dp1b-128*q2Dpb*q2Dp1b**2+128*q2D
     #pb**2*q2Dp1b-256*q1Dpb*q2Dp1b**2)+CV**2/den0x1/den0x3*A2d2_1245*(1
     #28*q2Dpb*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2)
      s7 = s5
      s9 = CA**2/den0x2/den0x3*A6d2_1345*(-256*q1Dpb*q1Dp1b*mb**2-128*q1
     #Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4+128*q1Dpb**2*mb**2+128*q1Dp1b*pbD
     #p1b*mb**2+256*q1Dp1b*pbDp1b**2+128*q1Dp1b*mb**4+128*q1Dp1b**2*mb**
     #2)
      s10 = CA**2/den0x2/den0x3*D0_2345*(384*q1Dq2*q1Dpb*pbDp1b-256*q1Dq
     #2*q1Dpb*q2Dp1b-256*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b-128
     #*q1Dpb*q1Dp1b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb-320*q1Dpb*q2Dpb*mb**2+1
     #28*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*pbDp1b*mb**2-192*q1Dp1b*q2Dp1b*mb
     #**2+128*q1Dq2*q2Dp1b*mb**2+576*q1Dq2*q1Dpb*mb**2-64*q1Dq2*q1Dp1b*m
     #b**2-256*q1Dq2*q1Dpb*q2Dpb-192*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*
     #*2-384*q1Dq2*q1Dpb*q1Dp1b-256*q1Dpb**2*q2Dp1b-128*q1Dpb*q2Dpb*pbDp
     #1b+256*q1Dq2**2*q1Dpb+64*q1Dq2*pbDp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+
     #64*q1Dpb**2*mb**2+64*q1Dp1b**2*mb**2-128*q1Dpb**2*q1Dp1b-128*q1Dq2
     #*q1Dpb**2+128*q1Dpb*q1Dp1b**2+128*q1Dpb*q2Dp1b**2+64*q1Dq2*mb**4+1
     #28*q1Dpb*mb**4)
      s8 = s9+s10
      s6 = s7+s8
      s4 = s6+CV**2/den0x2/den0x3*A1d1_1345*(-128*q1Dpb*q1Dp1b*pbDp1b+12
     #8*q1Dpb*q1Dp1b*mb**2-256*q1Dpb**2*q1Dp1b+192*q1Dpb**2*mb**2+64*q1D
     #p1b**2*mb**2)+CA**2/den0x2/den0x3*A2d1_1245*(128*q1Dq2*q1Dpb*mb**2
     #-192*q1Dq2*pbDp1b*mb**2-192*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dp1b-64
     #*q1Dp1b*q2Dpb*mb**2+192*q1Dp1b*q2Dp1b*mb**2-64*q2Dpb*pbDp1b*mb**2-
     #192*q2Dpb*mb**4-192*q2Dp1b*pbDp1b*mb**2-320*q2Dp1b*mb**4)+CV**2/de
     #n0x1/den0x3*A3e1*(-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-256*q1Dq2*q1Dpb*q
     #2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q1Dpb*mb**4+256
     #*q1Dpb**2*q2Dp1b*pbDp1b+256*q1Dpb**2*q2Dp1b*mb**2)
      s7 = s4
      s8 = CV**2/den0x1/den0x3*A4e1*(256*q1Dq2*q2Dp1b*pbDp1b*mb**2+256*q
     #1Dq2*q2Dp1b*mb**4-128*q1Dq2*pbDp1b*mb**4-128*q1Dq2*mb**6-256*q1Dpb
     #*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dpb*q1Dp1b*q2Dpb*mb**2+128*q1Dpb*q1Dp1b
     #*q2Dp1b*mb**2-256*q1Dpb*q2Dp1b*pbDp1b*mb**2-256*q1Dpb*q2Dp1b*mb**4
     #-256*q1Dpb**2*q2Dpb*q2Dp1b+128*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**2*q
     #2Dp1b*mb**2)+CA**2/den0x2/den0x3*A3d1_1345*(-128*q1Dq2*q1Dpb*mb**2
     #+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2+64*q1Dq2*pbDp1b*mb
     #**2+64*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dpb*q1Dp1b*mb**2-1
     #28*q1Dpb*q1Dp1b**2+64*q1Dpb*q2Dp1b*mb**2-192*q1Dpb*pbDp1b*mb**2-19
     #2*q1Dpb*mb**4+128*q1Dpb**2*mb**2-64*q1Dp1b*q2Dpb*mb**2-256*q1Dp1b*
     #q2Dp1b*pbDp1b-256*q1Dp1b*q2Dp1b*mb**2+576*q1Dp1b*pbDp1b*mb**2+256*
     #q1Dp1b*pbDp1b**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dp1b**
     #2*pbDp1b-320*q1Dp1b**2*mb**2)
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*D0_1345*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*
     #q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*mb**2
     #-128*q1Dpb*q1Dp1b**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*mb**2+
     #128*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*mb**4-128*
     #q1Dpb**2*q1Dp1b+256*q1Dpb**2*q2Dp1b+128*q1Dpb**2*mb**2+128*q1Dp1b*
     #q2Dp1b*mb**2)
      s8 = s7
      s10 = CA**2/den0x2/den0x3*D0_1234*(-256*q1Dq2*q1Dpb*pbDp1b+256*q1D
     #q2*q1Dpb*q2Dp1b-256*q1Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*q2Dpb+128
     #*q1Dpb*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp1b
     #+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q2Dp1b*m
     #b**2-256*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dq2*q1Dp1
     #b*mb**2+256*q1Dq2*q1Dpb*q2Dpb-256*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2D
     #pb*q2Dp1b-256*q1Dq2*q1Dp1b*pbDp1b-128*q1Dpb*q2Dpb**2+384*q1Dq2*q1D
     #pb*q1Dp1b+256*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-256*q1Dq2**2*
     #q1Dpb-128*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dq2*q1
     #Dpb**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2-128*q1Dp1b*q2Dp1b**2
     #-128*q1Dq2*mb**4)
      s11 = CA**2/den0x1/den0x3*A3d2_1235*(-128*q1Dpb*q1Dp1b*q2Dpb+128*q
     #1Dpb*q2Dpb*pbDp1b-256*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q
     #1Dp1b*q2Dpb*mb**2-128*q2Dpb*pbDp1b*mb**2+128*q2Dp1b*mb**4)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x1/den0x3*A3e1*(-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-2
     #56*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q1Dq2
     #*q1Dpb*mb**4+256*q1Dpb**2*q2Dp1b*pbDp1b+256*q1Dpb**2*q2Dp1b*mb**2)
     #+CV**2/den0x2/den0x3*A7d2_1345*(256*q1Dpb*q1Dp1b-128*q1Dpb*mb**2+2
     #56*q1Dp1b*pbDp1b+128*q1Dp1b*mb**2)
      s7 = s6+CV**2/den0x1/den0x3*A6d2_1235*(256*q1Dpb*q1Dp1b*q2Dpb-128*
     #q1Dpb*q2Dpb*q2Dp1b-256*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2+25
     #6*q1Dpb*q2Dp1b*pbDp1b+512*q1Dpb*q2Dp1b*mb**2-256*q1Dpb**2*q2Dp1b-1
     #28*q1Dp1b*q2Dpb*pbDp1b-256*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+
     #128*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+
     #128*q2Dpb*mb**4-128*q2Dpb**2*pbDp1b-256*q2Dp1b*pbDp1b*mb**2-256*q2
     #Dp1b*mb**4)
      s8 = s7+CA**2/den0x1/den0x3*A1d1_1245*(256*q2Dpb*q2Dp1b*mb**2-256*
     #q2Dpb*q2Dp1b**2+64*q2Dpb**2*mb**2+128*q2Dp1b**2*pbDp1b+320*q2Dp1b*
     #*2*mb**2)
      s3 = s8+CA**2/den0x1/den0x3*A3d1_1235*(256*q1Dq2*q1Dpb*q2Dp1b+128*
     #q2Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb
     #+320*q1Dp1b*q2Dp1b*mb**2-448*q2Dp1b*pbDp1b*mb**2-256*q1Dq2*q2Dp1b*
     #mb**2-128*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dq2*q1Dp1
     #b*mb**2+512*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b+64*q2Dpb*pbD
     #p1b*mb**2-128*q2Dp1b*pbDp1b**2-256*q1Dpb**2*q2Dp1b-128*q1Dpb*q2Dpb
     #*pbDp1b+64*q1Dq2*pbDp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+128*q2Dp1b**2*
     #mb**2-128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dp1b*q2Dp1b*pbDp1b-64*q2Dpb*mb
     #**4-320*q2Dp1b*mb**4+128*q2Dpb*pbDp1b**2-128*q1Dpb*q2Dp1b**2+128*q
     #1Dq2*mb**4)+CV**2/den0x1/den0x3*A7d2_1345*(128*q1Dq2*pbDp1b+128*q1
     #Dq2*mb**2-128*q1Dpb*q2Dp1b-128*q1Dpb*pbDp1b-128*q1Dpb*mb**2-128*q1
     #Dp1b*q2Dpb)
      s6 = s3+CV**2/den0x2/den0x3*A6d2_1345*(-256*q1Dpb*q1Dp1b*mb**2-128
     #*q1Dpb*pbDp1b*mb**2+128*q1Dpb*mb**4+128*q1Dpb**2*mb**2+128*q1Dp1b*
     #pbDp1b*mb**2+256*q1Dp1b*pbDp1b**2+128*q1Dp1b*mb**4+128*q1Dp1b**2*m
     #b**2)
      s7 = s6+CV**2/den0x2/den0x3*D0_1245*(-256*q1Dq2*q1Dpb*q2Dp1b+128*q
     #1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b+
     #128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*mb
     #**2+128*q1Dpb*q2Dp1b**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b
     #*mb**2+128*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2Dp1b*pbDp1b*
     #mb**2+128*q2Dp1b*mb**4+64*q2Dp1b**2*mb**2)
      s8 = s7
      s10 = CA**2/den0x2/den0x3*A3d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+256*q1
     #Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dpb*q1Dp1b*mb**2-128
     #*q1Dpb*q1Dp1b*q2Dpb+192*q1Dpb*q2Dpb*mb**2-256*q1Dq2*q2Dpb*mb**2+12
     #8*q1Dpb*q1Dp1b*q2Dp1b-192*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp
     #1b-64*q1Dp1b*q2Dp1b*mb**2+576*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*q2Dp1b*
     #mb**2-192*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*q2Dpb*q2Dp1b+448*q1Dq2*q1Dp
     #1b*mb**2+256*q1Dpb*q2Dp1b*mb**2+384*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*
     #q1Dpb*q1Dp1b-128*q1Dpb**2*q2Dp1b-320*q1Dq2*pbDp1b*mb**2+128*q1Dpb*
     #*2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b-384*q1Dp1b*q2Dp1b*pbDp1b-192*q1Dp
     #1b**2*mb**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dq2**2*pbDp
     #1b+256*q1Dp1b*pbDp1b**2-256*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1b**2-256
     #*q1Dpb*q2Dp1b**2-256*q1Dq2*mb**4-128*q1Dq2*pbDp1b**2-192*q1Dpb*mb*
     #*4)
      s11 = CA**2/den0x1/den0x3*A1d1_2345*(-128*q1Dq2*q1Dpb*q2Dpb-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-64*
     #q1Dq2*q1Dp1b*mb**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+256*q1Dp
     #b*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dpb+128*q1Dpb**2*pbDp1b+192*q1Dpb**2
     #*mb**2-64*q1Dp1b**2*mb**2)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x1/den0x3*A3d1_1234*(-128*q1Dq2*q1Dpb*q2Dpb+128*
     #q1Dq2*q1Dpb*pbDp1b+192*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb+64
     #*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dpb)+CA*
     #*2/den0x1/den0x3*A5d2_1345*(-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1D
     #p1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dp
     #b*q1Dp1b*mb**2+128*q1Dp1b**2*q2Dpb)
      s7 = s6+CA**2/den0x1/den0x3*A3d1_1245*(-128*q1Dq2*q2Dp1b*mb**2+64*
     #q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dpb
     #*q2Dpb*pbDp1b-64*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2+128*q2Dpb
     #*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2+64*q2Dpb*pbDp1b*mb**2+128*q2
     #Dpb*pbDp1b**2-64*q2Dpb*mb**4-128*q2Dpb**2*pbDp1b-448*q2Dp1b*pbDp1b
     #*mb**2-128*q2Dp1b*pbDp1b**2-320*q2Dp1b*mb**4+128*q2Dp1b**2*mb**2)
      s8 = s7+CA**2/den0x2/den0x3*A2d1_1234*(256*q1Dq2*q1Dpb*q1Dp1b+192*
     #q1Dq2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b+6
     #4*q1Dq2*q1Dp1b*mb**2+192*q1Dq2*q2Dpb*mb**2+128*q1Dq2*q2Dp1b*pbDp1b
     #+320*q1Dq2*q2Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2
     #-512*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+12
     #8*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*pbDp1b
     #-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1
     #b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1
     #b*mb**2+128*q1Dp1b**2*q2Dpb)
      s9 = s8
      s11 = CV**2/den0x2/den0x3*A4d2_2345*(256*q1Dq2*q1Dpb*q1Dp1b+128*q1
     #Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-2
     #56*q1Dpb*q1Dp1b**2-256*q1Dpb**2*q1Dp1b+128*q1Dp1b**2*q2Dpb-256*q1D
     #p1b**2*pbDp1b-256*q1Dp1b**2*mb**2)
      s12 = CA**2/den0x1/den0x3*A6d2_2345*(-128*q1Dq2*q1Dpb*pbDp1b-256*q
     #1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+1
     #28*q1Dpb*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dp1
     #b-128*q1Dpb*pbDp1b*mb**2+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b*
     #q2Dpb-384*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**2-128*q1Dq2*q1D
     #pb*mb**2+512*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*
     #q2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*pbDp1b+128*q1
     #Dpb*q2Dpb*pbDp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2+256
     #*q1Dp1b*q2Dpb*pbDp1b-512*q1Dq2*q2Dpb*q2Dp1b-128*q1Dpb*pbDp1b**2-12
     #8*q1Dp1b*mb**4+128*q1Dp1b**2*q2Dpb+128*q1Dq2**2*pbDp1b-256*q1Dp1b*
     #pbDp1b**2+128*q1Dq2**2*mb**2+128*q1Dq2*mb**4+256*q1Dq2*pbDp1b**2)
      s10 = s11+s12
      s4 = s9+s10
      s7 = s4
      s9 = CA**2/den0x1/den0x3*A6d2_1245*(128*q2Dpb*pbDp1b**2+128*q2Dpb*
     #mb**4-256*q2Dp1b*pbDp1b*mb**2)
      s10 = CA**2/den0x1/den0x3*A3d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+128*q1
     #Dq2*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b-1
     #92*q1Dpb*q1Dp1b*mb**2+64*q1Dpb*q2Dpb*mb**2+128*q1Dq2*q2Dpb*mb**2-1
     #92*q1Dpb*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-64*q1Dp1b*q2Dp1b*mb*
     #*2+128*q1Dq2*q1Dp1b*q2Dpb+192*q1Dp1b*pbDp1b*mb**2-320*q1Dq2*q2Dp1b
     #*mb**2+192*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b+192*q1Dq2*q1D
     #p1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dpb
     #*q2Dpb**2-64*q1Dq2*pbDp1b*mb**2-128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*
     #q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b-64*q1Dp1b**2*mb**2-128*q1Dq2*q
     #2Dpb**2-128*q1Dpb*pbDp1b**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-1
     #28*q1Dq2**2*pbDp1b-128*q1Dq2**2*mb**2-128*q1Dpb*q2Dp1b**2+128*q1Dp
     #1b*q2Dpb**2-128*q1Dq2*mb**4+64*q1Dpb*mb**4)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*A2d1_1235*(-256*q1Dq2*q1Dpb*q2Dp1b-128
     #*q2Dpb*q2Dp1b*pbDp1b-384*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*q2Dp
     #b+128*q1Dpb*q2Dpb*mb**2+64*q1Dq2*q2Dpb*mb**2+256*q1Dq2*q2Dp1b*pbDp
     #1b-256*q1Dp1b*q2Dp1b*mb**2+896*q2Dp1b*pbDp1b*mb**2+256*q1Dq2*q2Dp1
     #b*mb**2+128*q1Dq2*q1Dpb*mb**2+64*q1Dq2*q1Dp1b*mb**2-512*q1Dpb*q2Dp
     #1b*mb**2-384*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*pbDp1b*mb**2+256*q2Dp1b*
     #pbDp1b**2+256*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-192*q1Dq2*pbD
     #p1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-448*q2Dp1b**2*mb**2+128*q1Dp1b*q2
     #Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q2Dpb*q2Dp1b+640*q2D
     #p1b*mb**4-64*q2Dpb**2*mb**2-128*q2Dpb*pbDp1b**2-256*q2Dp1b**2*pbDp
     #1b+256*q2Dpb*q2Dp1b**2+128*q1Dpb*q2Dp1b**2-192*q1Dq2*mb**4)
      s5 = s7+CV**2/den0x2/den0x3*A2d1_1234*(256*q1Dq2*q1Dpb*q1Dp1b+192*
     #q1Dq2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b+6
     #4*q1Dq2*q1Dp1b*mb**2+192*q1Dq2*q2Dpb*mb**2+128*q1Dq2*q2Dp1b*pbDp1b
     #+320*q1Dq2*q2Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2
     #-512*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+12
     #8*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*pbDp1b
     #-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1
     #b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1
     #b*mb**2+128*q1Dp1b**2*q2Dpb)+CA**2/den0x1/den0x3*A3d2_1245*(128*q2
     #Dpb*pbDp1b*mb**2-128*q2Dp1b*mb**4)
      s7 = s5+CA**2/den0x2/den0x3*D0_1245*(-256*q1Dq2*q1Dpb*q2Dp1b+128*q
     #1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b+
     #128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*mb
     #**2+128*q1Dpb*q2Dp1b**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b
     #*mb**2+128*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2Dp1b*pbDp1b*
     #mb**2+128*q2Dp1b*mb**4+64*q2Dp1b**2*mb**2)
      s6 = s7+CV**2/den0x1/den0x3*A2d1_1345*(128*q1Dq2*q2Dp1b*mb**2+64*q
     #1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2-64*q1Dq2*mb**4-64*q1Dpb*q1Dp
     #1b*mb**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dpb*q2Dp1b*mb**2+192*q1Dpb*p
     #bDp1b*mb**2+64*q1Dpb*mb**4+128*q1Dpb**2*q2Dp1b-64*q1Dpb**2*mb**2-6
     #4*q1Dp1b*q2Dpb*mb**2+64*q1Dp1b*pbDp1b*mb**2-64*q1Dp1b*mb**4)+CA**2
     #/den0x1/den0x3*A7d2_1345*(128*q1Dq2*pbDp1b+128*q1Dq2*mb**2-128*q1D
     #pb*q2Dp1b-128*q1Dpb*pbDp1b-128*q1Dpb*mb**2-128*q1Dp1b*q2Dpb)
      s8 = s6
      s10 = CV**2/den0x1
      s12 = 1/den0x3
      s14 = E0
      s16 = 128*q1Dp1b*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb*mb**4-128*q1Dp1b**2
     #*q2Dp1b*mb**2-256*q1Dpb**2*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*mb**4-128
     #*q1Dq2*q2Dp1b*mb**4-512*q1Dq2*q1Dpb*q2Dp1b**2-256*q1Dq2*q1Dpb*mb**
     #4-128*q1Dpb*q2Dp1b**2*mb**2+128*q1Dpb*q2Dp1b*mb**4-128*q1Dp1b*q2Dp
     #b**2*mb**2+768*q1Dpb**2*q2Dp1b**2-128*q1Dp1b*q2Dp1b**2*mb**2+256*q
     #1Dp1b**2*q2Dpb*q2Dp1b-128*q1Dpb*q2Dpb**2*mb**2-256*q1Dpb*q2Dpb**2*
     #q2Dp1b+256*q1Dpb*q2Dp1b**3-128*q1Dp1b**2*q2Dpb*mb**2
      s15 = s16-256*q1Dq2*q2Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b*mb**2+128
     #*q1Dq2*q2Dpb*mb**4+256*q1Dpb*q1Dp1b*q2Dp1b**2+128*q1Dpb**2*q2Dpb*m
     #b**2-384*q1Dq2*q2Dp1b*pbDp1b*mb**2-256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+2
     #56*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b*pbDp1b*mb**2-256*q1D
     #q2*q1Dpb*pbDp1b*mb**2+512*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+512*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b
     #*mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*pbDp1b*mb**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*pbDp1b*mb**2+128*q
     #1Dp1b*q2Dp1b*pbDp1b*mb**2
      s13 = s14*s15
      s11 = s12*s13
      s9 = s10*s11
      s7 = s8+s9
      s2 = s7+CA**2/den0x2/den0x3*A7d2_1345*(256*q1Dpb*q1Dp1b-128*q1Dpb*
     #mb**2+256*q1Dp1b*pbDp1b+128*q1Dp1b*mb**2)+CA**2/den0x2/den0x3*A7d2
     #_2345*(-256*q1Dpb*q1Dp1b+128*q1Dpb*mb**2-256*q1Dp1b*pbDp1b-128*q1D
     #p1b*mb**2)+CA**2/den0x1/den0x3*A4d2_1245*(128*q2Dpb*q2Dp1b*pbDp1b-
     #128*q2Dpb**2*mb**2)
      s7 = CV**2/den0x2/den0x3*D0_2345*(384*q1Dq2*q1Dpb*pbDp1b-256*q1Dq2
     #*q1Dpb*q2Dp1b-256*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b-128*
     #q1Dpb*q1Dp1b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb-320*q1Dpb*q2Dpb*mb**2+12
     #8*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*pbDp1b*mb**2-192*q1Dp1b*q2Dp1b*mb*
     #*2+128*q1Dq2*q2Dp1b*mb**2+576*q1Dq2*q1Dpb*mb**2-64*q1Dq2*q1Dp1b*mb
     #**2-256*q1Dq2*q1Dpb*q2Dpb-192*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb**
     #2-384*q1Dq2*q1Dpb*q1Dp1b-256*q1Dpb**2*q2Dp1b-128*q1Dpb*q2Dpb*pbDp1
     #b+256*q1Dq2**2*q1Dpb+64*q1Dq2*pbDp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+6
     #4*q1Dpb**2*mb**2+64*q1Dp1b**2*mb**2-128*q1Dpb**2*q1Dp1b-128*q1Dq2*
     #q1Dpb**2+128*q1Dpb*q1Dp1b**2+128*q1Dpb*q2Dp1b**2+64*q1Dq2*mb**4+12
     #8*q1Dpb*mb**4)
      s8 = CA**2/den0x1/den0x3*A4d2_1235*(-128*q1Dpb*q2Dpb*q2Dp1b+128*q1
     #Dp1b*q2Dpb**2+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2-128*q
     #2Dpb**2*pbDp1b-128*q2Dpb**2*mb**2)
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*A5d2_1345*(128*q1Dpb*q1Dp1b*mb**2-256*
     #q1Dpb*q1Dp1b**2-256*q1Dp1b**2*pbDp1b-128*q1Dp1b**2*mb**2)
      s8 = s7
      s10 = CA**2/den0x2/den0x3*A3e1*(128*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q
     #1Dq2*q1Dpb*mb**4)
      s12 = CA**2/den0x2
      s14 = 1/den0x3
      s16 = A2d1_2345
      s17 = -256*q1Dq2*q1Dpb*pbDp1b-256*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q1D
     #p1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb-64*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q
     #2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*pbDp1b*mb**2-128*q1Dq
     #2*q2Dp1b*pbDp1b+320*q1Dp1b*q2Dp1b*mb**2-640*q1Dp1b*pbDp1b*mb**2+64
     #*q1Dq2*q2Dp1b*mb**2-64*q1Dq2*q1Dpb*mb**2-256*q1Dp1b*q2Dpb*q2Dp1b-5
     #12*q1Dq2*q1Dp1b*mb**2-192*q1Dpb*q2Dp1b*mb**2-384*q1Dq2*q1Dp1b*pbDp
     #1b+128*q1Dq2*q1Dpb*q1Dp1b+128*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp
     #1b+192*q1Dq2*pbDp1b*mb**2+64*q1Dp1b*q2Dpb*mb**2-256*q1Dpb**2*mb**2
     #+128*q1Dp1b*q2Dpb*pbDp1b+384*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b**2*mb
     #**2-128*q1Dpb*pbDp1b**2+128*q1Dpb**2*q1Dp1b-384*q1Dp1b*mb**4+128*q
     #1Dp1b**2*q2Dpb+256*q1Dq2**2*pbDp1b-256*q1Dp1b*pbDp1b**2+256*q1Dq2*
     #*2*mb**2-128*q1Dpb*q1Dp1b**2+256*q1Dpb*q2Dp1b**2+64*q1Dq2*mb**4+12
     #8*q1Dq2*pbDp1b**2
      s15 = s16*s17
      s13 = s14*s15
      s11 = s12*s13
      s9 = s10+s11
      s5 = s8+s9
      s7 = s5
      s9 = CA**2/den0x2/den0x3*A6d2_2345*(128*q1Dq2*q1Dpb*pbDp1b+256*q1D
     #q2*q1Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2-128
     #*q1Dpb*q1Dp1b*q2Dpb-384*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*pbDp1b*mb**2
     #+256*q1Dq2*q2Dp1b*pbDp1b-384*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*
     #q2Dpb+640*q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q2Dp1b*mb**2+256*q1Dp1b*q2
     #Dpb*q2Dp1b+512*q1Dq2*q1Dp1b*mb**2+384*q1Dpb*q2Dp1b*mb**2+384*q1Dq2
     #*q1Dp1b*pbDp1b+256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dpb**2*q2Dp1b-384*q1Dq
     #2*pbDp1b*mb**2-256*q1Dp1b*q2Dpb*mb**2+128*q1Dpb**2*mb**2-128*q1Dp1
     #b*q2Dpb*pbDp1b-512*q1Dp1b*q2Dp1b*pbDp1b-256*q1Dp1b**2*mb**2+384*q1
     #Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dq2**2*pbDp1b+256*q1Dp1b*pbDp
     #1b**2-256*q1Dq2**2*mb**2-256*q1Dpb*q1Dp1b**2-256*q1Dpb*q2Dp1b**2-2
     #56*q1Dq2*mb**4-128*q1Dq2*pbDp1b**2-128*q1Dpb*mb**4-256*q1Dp1b**2*p
     #bDp1b)
      s10 = CV**2/den0x2/den0x3*A1d1_1235*(-64*q1Dq2*q2Dpb*mb**2+256*q1D
     #p1b*q2Dpb*q2Dp1b+256*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2Dp
     #1b**2*pbDp1b+320*q2Dp1b**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*D0_1345*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q
     #1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+256*q1Dq2*pbDp1b*mb**2+1
     #28*q1Dq2*pbDp1b**2+128*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1D
     #pb*q1Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-64*q1Dpb*q2Dpb*mb**2-64*q1D
     #pb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+256*q1Dpb*pbDp1b*mb**2+256*q1D
     #pb*mb**4+128*q1Dpb**2*q2Dp1b-64*q1Dpb**2*mb**2-64*q1Dp1b*q2Dpb*mb*
     #*2-64*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4
     #-64*q1Dp1b**2*mb**2)
      s4 = s7+CA**2/den0x2/den0x3*A4e1*(-128*q1Dq2*pbDp1b*mb**4-128*q1Dq
     #2*mb**6-128*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1
     #b-384*q1Dpb*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb**2*q1Dp1b*q2Dp1b-128*q1D
     #pb**2*q2Dpb*mb**2-256*q1Dpb**2*q2Dp1b*pbDp1b-384*q1Dpb**2*q2Dp1b*m
     #b**2+256*q1Dpb**3*q2Dp1b)+CV**2/den0x1/den0x3*A7d2_1245*(128*q2Dpb
     #*pbDp1b-128*q2Dp1b*mb**2)+CV**2/den0x2/den0x3*D0_1235*(128*q1Dq2*q
     #1Dpb*q2Dp1b-64*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b-64*q1Dq2*
     #q1Dp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**2-64*q1D
     #pb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2+256*q1Dp1
     #b*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-64*q
     #1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*q2Dp1b**2+128*q2Dpb*q2Dp1b*mb**2-128
     #*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*mb**4+128*q2Dp1b**2*pbDp1b+256*q2D
     #p1b**2*mb**2)
      s6 = s4+CV**2/den0x2/den0x3*A1d1_2345*(128*q1Dq2*q1Dpb*pbDp1b-64*q
     #1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2-2
     #56*q1Dpb**2*q1Dp1b+192*q1Dpb**2*mb**2+64*q1Dp1b**2*mb**2)+CV**2/de
     #n0x1/den0x3*A1d1_2345*(-128*q1Dq2*q1Dpb*q2Dpb-128*q1Dq2*q1Dpb*pbDp
     #1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-64*q1Dq2*q1Dp1b*m
     #b**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+256*q1Dpb*q1Dp1b*q2Dpb
     #+128*q1Dpb**2*q2Dpb+128*q1Dpb**2*pbDp1b+192*q1Dpb**2*mb**2-64*q1Dp
     #1b**2*mb**2)
      s7 = s6+CA**2/den0x2/den0x3*A1d1_1245*(256*q1Dp1b*q2Dpb*q2Dp1b+256
     #*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2Dp1b**2*pbDp1b+320*q2D
     #p1b**2*mb**2)
      s8 = s7
      s10 = CV**2/den0x1/den0x3*A3d1_1235*(256*q1Dq2*q1Dpb*q2Dp1b+128*q2
     #Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb+3
     #20*q1Dp1b*q2Dp1b*mb**2-448*q2Dp1b*pbDp1b*mb**2-256*q1Dq2*q2Dp1b*mb
     #**2-128*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dq2*q1Dp1b*
     #mb**2+512*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b+64*q2Dpb*pbDp1
     #b*mb**2-128*q2Dp1b*pbDp1b**2-256*q1Dpb**2*q2Dp1b-128*q1Dpb*q2Dpb*p
     #bDp1b+64*q1Dq2*pbDp1b*mb**2-64*q1Dp1b*q2Dpb*mb**2+128*q2Dp1b**2*mb
     #**2-128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dp1b*q2Dp1b*pbDp1b-64*q2Dpb*mb**
     #4-320*q2Dp1b*mb**4+128*q2Dpb*pbDp1b**2-128*q1Dpb*q2Dp1b**2+128*q1D
     #q2*mb**4)
      s11 = CA**2/den0x1/den0x3*A4d2_2345*(-128*q1Dq2*q1Dpb*q2Dp1b-256*q
     #1Dq2*q1Dpb*pbDp1b-256*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128
     #*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dq2**2*pbDp1b+12
     #8*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b+12
     #8*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+1
     #28*q1Dpb**2*pbDp1b+128*q1Dpb**2*mb**2+128*q1Dp1b**2*q2Dpb)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x1/den0x3*A3d2_2345*(256*q1Dq2*q2Dpb*q2Dp1b+128*
     #q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-1
     #28*q1Dq2*pbDp1b**2-256*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b
     #-128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*pbDp1b
     #**2)+CA**2/den0x2/den0x3*A4d2_1345*(-128*q1Dpb*q1Dp1b*mb**2-256*q1
     #Dpb**2*q1Dp1b-128*q1Dp1b**2*mb**2)
      s7 = s6+CA**2/den0x1/den0x3*A2d2_2345*(256*q1Dq2*q1Dpb*pbDp1b+128*
     #q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-
     #128*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q2Dpb*mb**
     #2+128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+256*q1Dpb*pbDp1b*m
     #b**2+128*q1Dq2*q1Dp1b*q2Dpb+256*q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q1Dp
     #b*mb**2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*mb**2-256*q1Dpb*q
     #2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dpb**2*pbDp1b-128*q1Dpb*
     #*2*q2Dp1b-128*q1Dpb*q2Dpb*pbDp1b-256*q1Dq2*pbDp1b*mb**2-128*q1Dp1b
     #*q2Dpb*mb**2-128*q1Dpb**2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*
     #q2Dpb*q2Dp1b+128*q1Dpb*pbDp1b**2+128*q1Dp1b*mb**4-128*q1Dp1b**2*q2
     #Dpb-128*q1Dq2**2*pbDp1b+128*q1Dp1b*pbDp1b**2-128*q1Dq2**2*mb**2-12
     #8*q1Dq2*mb**4-128*q1Dq2*pbDp1b**2+128*q1Dpb*mb**4)
      s8 = s7+CA**2/den0x2/den0x3*A2d2_1345*(128*q1Dpb*q1Dp1b*mb**2+128*
     #q1Dpb*pbDp1b*mb**2-128*q1Dpb**2*mb**2+128*q1Dp1b*pbDp1b*mb**2)
      s3 = s8+CV**2/den0x1/den0x3*A5d2_2345*(128*q1Dq2*q1Dpb*q2Dp1b+128*
     #q1Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb+12
     #8*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2**2*pbDp1b-1
     #28*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-
     #128*q1Dpb*q1Dp1b*mb**2-128*q1Dp1b**2*q2Dpb)+CV**2/den0x1/den0x3*A4
     #d2_2345*(-128*q1Dq2*q1Dpb*q2Dp1b-256*q1Dq2*q1Dpb*pbDp1b-256*q1Dq2*
     #q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b-128*q1D
     #q2*q1Dp1b*mb**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+128*q1Dpb*q
     #1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+128*q1
     #Dpb*q1Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dpb**2*pbDp1b+128*q1Dpb
     #**2*mb**2+128*q1Dp1b**2*q2Dpb)
      s6 = s3+CA**2/den0x2/den0x3*A1d1_1235*(-64*q1Dq2*q2Dpb*mb**2+256*q
     #1Dp1b*q2Dpb*q2Dp1b+256*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2
     #Dp1b**2*pbDp1b+320*q2Dp1b**2*mb**2)
      s5 = s6+CV**2/den0x1/den0x3*A5d2_1245*(128*q2Dpb*q2Dp1b*mb**2-128*
     #q2Dpb**2*pbDp1b)+CA**2/den0x1/den0x3*A4d2_1345*(-128*q1Dq2*q1Dpb*p
     #bDp1b-128*q1Dq2*q1Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2
     #Dp1b+128*q1Dpb**2*pbDp1b+128*q1Dpb**2*mb**2)+CA**2/den0x2/den0x3*A
     #3d1_1235*(-128*q1Dq2*q1Dpb*q1Dp1b+128*q1Dq2*q1Dpb*pbDp1b+128*q1Dq2
     #*q1Dp1b*pbDp1b+192*q1Dq2*q1Dp1b*mb**2-192*q1Dq2*pbDp1b*mb**2-128*q
     #1Dq2*pbDp1b**2+128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*mb**2+128*q
     #1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b-128*
     #q1Dp1b*q2Dpb*pbDp1b+192*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1
     #b+192*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb-192*q2Dpb*pbDp1b*mb*
     #*2-64*q2Dpb*mb**4-320*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-64*
     #q2Dp1b*mb**4)
      s6 = s5+CV**2/den0x2/den0x3*A7d2_2345*(-256*q1Dpb*q1Dp1b+128*q1Dpb
     #*mb**2-256*q1Dp1b*pbDp1b-128*q1Dp1b*mb**2)+CV**2/den0x2/den0x3*A3d
     #2_2345*(-128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1
     #Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq2*q1Dp1b*mb**2-128*q1Dq
     #2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*
     #q1Dq2*mb**4+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1
     #b*q2Dp1b-128*q1Dpb*q1Dp1b*mb**2-256*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q
     #2Dp1b**2+128*q1Dpb*mb**4-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb*
     #mb**2+256*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dp1b*mb**2-256*q1Dp1b*
     #pbDp1b*mb**2-128*q1Dp1b*mb**4+128*q1Dp1b**2*q2Dpb+128*q1Dp1b**2*mb
     #**2)
      s4 = s6+CV**2/den0x1/den0x3*A7d2_2345*(-128*q1Dq2*pbDp1b-128*q1Dq2
     #*mb**2+128*q1Dpb*q2Dp1b+128*q1Dpb*pbDp1b+128*q1Dpb*mb**2+128*q1Dp1
     #b*q2Dpb)+CA**2/den0x1/den0x3*A6d2_1345*(-128*q1Dpb*q2Dpb*mb**2+128
     #*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*pbDp1b**2+128
     #*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**
     #2-128*q1Dp1b*mb**4)+CV**2/den0x2/den0x3*A5d2_1345*(128*q1Dpb*q1Dp1
     #b*mb**2-256*q1Dpb*q1Dp1b**2-256*q1Dp1b**2*pbDp1b-128*q1Dp1b**2*mb*
     #*2)+CA**2/den0x2/den0x3*A5d2_2345*(-256*q1Dq2*q1Dpb*q1Dp1b-128*q1D
     #q2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b-12
     #8*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b**2-128*q1Dp1b**2*q2Dpb+256*q
     #1Dp1b**2*pbDp1b+128*q1Dp1b**2*mb**2)
      s6 = s4+CA**2/den0x2/den0x3*A2d1_1345*(128*q1Dq2*q1Dpb*mb**2+64*q1
     #Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4-192*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*q
     #2Dpb*mb**2-192*q1Dpb*q2Dp1b*mb**2+320*q1Dpb*pbDp1b*mb**2+128*q1Dpb
     #*pbDp1b**2+192*q1Dpb*mb**4-128*q1Dpb**2*q1Dp1b-64*q1Dpb**2*mb**2-6
     #4*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2+64*q1Dp1b*pbDp1b*mb**
     #2+64*q1Dp1b*mb**4-128*q1Dp1b**2*mb**2)+CV**2/den0x1/den0x3*A1d1_12
     #45*(256*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*q2Dp1b**2+64*q2Dpb**2*mb**2+1
     #28*q2Dp1b**2*pbDp1b+320*q2Dp1b**2*mb**2)
      s5 = s6+CA**2/den0x1/den0x3*A7d2_2345*(-128*q1Dq2*pbDp1b-128*q1Dq2
     #*mb**2+128*q1Dpb*q2Dp1b+128*q1Dpb*pbDp1b+128*q1Dpb*mb**2+128*q1Dp1
     #b*q2Dpb)+CA**2/den0x1/den0x3*A1d1_1235*(128*q1Dq2*q2Dpb*q2Dp1b-64*
     #q1Dq2*q2Dpb*mb**2-128*q1Dpb*q2Dpb*q2Dp1b+256*q2Dpb*q2Dp1b*mb**2-25
     #6*q2Dpb*q2Dp1b**2+64*q2Dpb**2*mb**2+128*q2Dp1b**2*pbDp1b+320*q2Dp1
     #b**2*mb**2)+CV**2/den0x1/den0x3*A5d2_1235*(128*q1Dpb*q2Dpb*q2Dp1b-
     #128*q1Dp1b*q2Dpb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dpb**2*pbDp1b)
      s7 = s5+CV**2/den0x2/den0x3*D0_1234*(-256*q1Dq2*q1Dpb*pbDp1b+256*q
     #1Dq2*q1Dpb*q2Dp1b-256*q1Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1b*q2Dpb+1
     #28*q1Dpb*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp
     #1b+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q2Dp1b
     #*mb**2-256*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dq2*q1D
     #p1b*mb**2+256*q1Dq2*q1Dpb*q2Dpb-256*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q
     #2Dpb*q2Dp1b-256*q1Dq2*q1Dp1b*pbDp1b-128*q1Dpb*q2Dpb**2+384*q1Dq2*q
     #1Dpb*q1Dp1b+256*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-256*q1Dq2**
     #2*q1Dpb-128*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dq2*
     #q1Dpb**2+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2-128*q1Dp1b*q2Dp1b*
     #*2-128*q1Dq2*mb**4)
      s8 = s7
      s10 = CA**2/den0x1/den0x3*A2d1_1234*(-128*q1Dq2*q1Dpb*pbDp1b-128*q
     #1Dq2*q2Dpb*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*q2Dpb+1
     #28*q1Dpb*q2Dpb*mb**2-192*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2
     #-128*q1Dq2*q1Dp1b*q2Dpb-64*q1Dq2*q2Dp1b*mb**2-192*q1Dq2*q1Dpb*mb**
     #2+128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dq2*q1Dp1b*mb**2+128*q1Dq2*q1Dpb*q2
     #Dpb+128*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dpb**2+128*q1Dpb*q2Dpb*pbDp
     #1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-128*q1Dpb**2*q2Dp
     #b+128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q2Dpb
     #*q2Dp1b+128*q1Dq2*q2Dpb**2-128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb**2
     #+256*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s11 = CA**2/den0x1/den0x3*D0_2345*(-128*q1Dq2*q1Dpb*pbDp1b+128*q1D
     #q2*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128
     #*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b*q2Dpb+256*q1Dpb*q2Dpb*mb**2+6
     #4*q1Dq2*q2Dpb*mb**2-256*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b
     #+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**2+64*q1Dq2*q2Dp1b*m
     #b**2-256*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dpb+384*q1Dpb*q2Dp1b*
     #mb**2-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb**2*pbDp1b-128*q1Dpb**2*q2Dp
     #1b-192*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2+128*q1Dpb**2*q2Dp
     #b+256*q1Dpb**2*mb**2-128*q1Dp1b*mb**4-128*q1Dpb*q2Dp1b**2-64*q1Dq2
     #*mb**4-128*q1Dq2*pbDp1b**2-256*q1Dpb*mb**4)
      s9 = s10+s11
      s6 = s8+s9
      s7 = s6+CA**2/den0x1/den0x3*D0_1245*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1
     #Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+64*q1Dpb*q2Dpb*mb**2+64*q
     #1Dpb*q2Dp1b*mb**2+256*q1Dpb*q2Dp1b**2-64*q1Dp1b*q2Dpb*mb**2+128*q1
     #Dp1b*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2+256*q2Dpb*q2Dp1b*pbDp1b+
     #384*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b**2-128*q2Dpb**2*q2Dp1b-384
     #*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-256*q2Dp1b*mb**4+128*q2D
     #p1b**2*pbDp1b+256*q2Dp1b**2*mb**2)
      s1 = s7+CV**2/den0x2/den0x3*A5d2_2345*(-256*q1Dq2*q1Dpb*q1Dp1b-128
     #*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1
     #b-128*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b**2-128*q1Dp1b**2*q2Dpb+2
     #56*q1Dp1b**2*pbDp1b+128*q1Dp1b**2*mb**2)+CV**2/den0x1/den0x3*A1d1_
     #1235*(128*q1Dq2*q2Dpb*q2Dp1b-64*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q2Dpb*
     #q2Dp1b+256*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*q2Dp1b**2+64*q2Dpb**2*mb**
     #2+128*q2Dp1b**2*pbDp1b+320*q2Dp1b**2*mb**2)+s2
      s6 = s1+CA**2/den0x2/den0x3*A3d2_1345*(128*q1Dpb*q1Dp1b*mb**2-128*
     #q1Dpb*mb**4+256*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4-128*q1Dp1b**2
     #*mb**2)
      s7 = s6+CA**2/den0x1/den0x3*A4e1*(256*q1Dq2*q2Dp1b*pbDp1b*mb**2+25
     #6*q1Dq2*q2Dp1b*mb**4-128*q1Dq2*pbDp1b*mb**4-128*q1Dq2*mb**6-256*q1
     #Dpb*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dpb*q1Dp1b*q2Dpb*mb**2+128*q1Dpb*q1D
     #p1b*q2Dp1b*mb**2-256*q1Dpb*q2Dp1b*pbDp1b*mb**2-256*q1Dpb*q2Dp1b*mb
     #**4-256*q1Dpb**2*q2Dpb*q2Dp1b+128*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**
     #2*q2Dp1b*mb**2)
      s5 = s7+CV**2/den0x1/den0x3*D0_1345*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q
     #1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+256*q1Dq2*pbDp1b*mb**2+1
     #28*q1Dq2*pbDp1b**2+128*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1D
     #pb*q1Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-64*q1Dpb*q2Dpb*mb**2-64*q1D
     #pb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+256*q1Dpb*pbDp1b*mb**2+256*q1D
     #pb*mb**4+128*q1Dpb**2*q2Dp1b-64*q1Dpb**2*mb**2-64*q1Dp1b*q2Dpb*mb*
     #*2-64*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4
     #-64*q1Dp1b**2*mb**2)+CV**2/den0x1/den0x3*A3d2_2345*(256*q1Dq2*q2Dp
     #b*q2Dp1b+128*q1Dq2*q2Dpb*pbDp1b+128*q1Dq2*q2Dp1b*mb**2-128*q1Dq2*p
     #bDp1b*mb**2-128*q1Dq2*pbDp1b**2-256*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b
     #*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*pbDp1b*mb**2+128*
     #q1Dp1b*pbDp1b**2)
      s6 = s5+CV**2/den0x1/den0x3*A5d2_1345*(-128*q1Dq2*q1Dp1b*pbDp1b-12
     #8*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp
     #1b+128*q1Dpb*q1Dp1b*mb**2+128*q1Dp1b**2*q2Dpb)+CV**2/den0x1/den0x3
     #*A3d1_1234*(-128*q1Dq2*q1Dpb*q2Dpb+128*q1Dq2*q1Dpb*pbDp1b+192*q1Dq
     #2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2*q1Dp1b*mb**2+128*q1D
     #pb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dpb)
      s4 = s6+CV**2/den0x2/den0x3*A2d1_1345*(128*q1Dq2*q1Dpb*mb**2+64*q1
     #Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4-192*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*q
     #2Dpb*mb**2-192*q1Dpb*q2Dp1b*mb**2+320*q1Dpb*pbDp1b*mb**2+128*q1Dpb
     #*pbDp1b**2+192*q1Dpb*mb**4-128*q1Dpb**2*q1Dp1b-64*q1Dpb**2*mb**2-6
     #4*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2+64*q1Dp1b*pbDp1b*mb**
     #2+64*q1Dp1b*mb**4-128*q1Dp1b**2*mb**2)+CV**2/den0x2/den0x3*A3e1*(1
     #28*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q1Dpb*mb**4)+CA**2/den0x1/de
     #n0x3*A2d2_1345*(128*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*mb**2-128*q
     #1Dpb*pbDp1b*mb**2-128*q1Dpb*mb**4)
      s6 = s4+CV**2/den0x2/den0x3*A1d1_1245*(256*q1Dp1b*q2Dpb*q2Dp1b+256
     #*q2Dpb*q2Dp1b*mb**2+64*q2Dpb**2*mb**2+128*q2Dp1b**2*pbDp1b+320*q2D
     #p1b**2*mb**2)+CV**2/den0x1/den0x3*A2d2_1235*(-128*q1Dpb*q1Dp1b*q2D
     #pb+128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*m
     #b**2-256*q1Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q
     #2Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2
     #Dpb**2-128*q2Dpb*q2Dp1b*pbDp1b-128*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*pb
     #Dp1b*mb**2-128*q2Dpb*pbDp1b**2-128*q2Dpb*mb**4+128*q2Dpb**2*pbDp1b
     #+128*q2Dpb**2*mb**2+256*q2Dp1b*pbDp1b*mb**2+128*q2Dp1b*pbDp1b**2+1
     #28*q2Dp1b*mb**4)
      s8 = s6
      s10 = CV**2/den0x1
      s12 = 1/den0x3
      s14 = A2e1
      s16 = -128*q1Dp1b**2*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1
     #Dpb**2*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**4+256*q1Dpb*q2Dp1b*pbDp1
     #b**2-128*q1Dq2*q1Dpb*mb**4+256*q1Dpb*q2Dp1b*mb**4+256*q1Dp1b**2*q2
     #Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2*mb**2-128*q1Dp1b**2*q2Dpb*mb**2+256
     #*q1Dq2*pbDp1b*mb**4-256*q1Dq2*q2Dp1b*pbDp1b**2-384*q1Dpb**2*q2Dp1b
     #*mb**2-128*q1Dq2*q2Dpb*mb**4-128*q1Dpb**2*q2Dpb*mb**2-256*q1Dpb*q1
     #Dp1b*q2Dpb*mb**2+128*q1Dq2*mb**6
      s15 = s16-512*q1Dq2*q2Dp1b*pbDp1b*mb**2+512*q1Dpb*q1Dp1b*q2Dpb*q2D
     #p1b-256*q1Dpb*q1Dp1b*q2Dp1b*mb**2+512*q1Dpb*q2Dp1b*pbDp1b*mb**2+25
     #6*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q1Dpb*q2Dp1b*mb**2-128*q1Dq2
     #*q1Dpb*pbDp1b*mb**2-256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q
     #2Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*mb
     #**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+25
     #6*q1Dq2*q2Dpb*q2Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b*mb**2-256*q1Dpb*
     #q2Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2
      s13 = s14*s15
      s11 = s12*s13
      s9 = s10*s11
      s7 = s8+s9
      s5 = s7+CA**2/den0x2/den0x3*D0_1235*(128*q1Dq2*q1Dpb*q2Dp1b-64*q1D
     #q2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b-64*q1Dq2*q1Dp1b*mb**2-128*q
     #1Dq2*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**2-64*q1Dpb*q2Dpb*mb**2+64*
     #q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2+256*q1Dp1b*q2Dpb*q2Dp1b-64*
     #q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b-64*q1Dp1b*q2Dp1b*mb**2
     #+128*q1Dp1b*q2Dp1b**2+128*q2Dpb*q2Dp1b*mb**2-128*q2Dp1b*pbDp1b*mb*
     #*2-128*q2Dp1b*mb**4+128*q2Dp1b**2*pbDp1b+256*q2Dp1b**2*mb**2)+CV**
     #2/den0x1/den0x3*A2d2_1345*(128*q1Dpb*q2Dpb*mb**2-128*q1Dpb*q2Dp1b*
     #mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*mb**4)
      s7 = s5
      s9 = CV**2/den0x1/den0x3*A6d2_2345*(-128*q1Dq2*q1Dpb*pbDp1b-256*q1
     #Dq2*q2Dpb*pbDp1b-128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+12
     #8*q1Dpb*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b
     #-128*q1Dpb*pbDp1b*mb**2+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b*q
     #2Dpb-384*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*mb**2-128*q1Dq2*q1Dp
     #b*mb**2+512*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dpb*q
     #2Dp1b*mb**2+256*q1Dpb*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*pbDp1b+128*q1D
     #pb*q2Dpb*pbDp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2+256*
     #q1Dp1b*q2Dpb*pbDp1b-512*q1Dq2*q2Dpb*q2Dp1b-128*q1Dpb*pbDp1b**2-128
     #*q1Dp1b*mb**4+128*q1Dp1b**2*q2Dpb+128*q1Dq2**2*pbDp1b-256*q1Dp1b*p
     #bDp1b**2+128*q1Dq2**2*mb**2+128*q1Dq2*mb**4+256*q1Dq2*pbDp1b**2)
      s10 = CA**2/den0x2/den0x3*A1d1_2345*(128*q1Dq2*q1Dpb*pbDp1b-64*q1D
     #q2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2-256
     #*q1Dpb**2*q1Dp1b+192*q1Dpb**2*mb**2+64*q1Dp1b**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x2/den0x3*A3d1_1235*(-128*q1Dq2*q1Dpb*q1Dp1b+128
     #*q1Dq2*q1Dpb*pbDp1b+128*q1Dq2*q1Dp1b*pbDp1b+192*q1Dq2*q1Dp1b*mb**2
     #-192*q1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2+128*q1Dpb*q1Dp1b*q2Dp1
     #b+128*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dp1b*m
     #b**2-128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b+192*q1Dp1b*q2Dpb*
     #mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+192*q1Dp1b*q2Dp1b*mb**2+128*q1Dp1b*
     #*2*q2Dpb-192*q2Dpb*pbDp1b*mb**2-64*q2Dpb*mb**4-320*q2Dp1b*pbDp1b*m
     #b**2-128*q2Dp1b*pbDp1b**2-64*q2Dp1b*mb**4)
      s8 = s7+CV**2/den0x2/den0x3*A2d1_1245*(128*q1Dq2*q1Dpb*mb**2-192*q
     #1Dq2*pbDp1b*mb**2-192*q1Dq2*mb**4-128*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dp1
     #b*q2Dpb*mb**2+192*q1Dp1b*q2Dp1b*mb**2-64*q2Dpb*pbDp1b*mb**2-192*q2
     #Dpb*mb**4-192*q2Dp1b*pbDp1b*mb**2-320*q2Dp1b*mb**4)
      s9 = s8
      s11 = CV**2/den0x1/den0x3*A2d2_2345*(256*q1Dq2*q1Dpb*pbDp1b+128*q1
     #Dq2*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-12
     #8*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dpb*q2Dpb*mb**2+
     #128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+256*q1Dpb*pbDp1b*mb*
     #*2+128*q1Dq2*q1Dp1b*q2Dpb+256*q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q1Dpb*
     #mb**2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*mb**2-256*q1Dpb*q2D
     #pb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dpb**2*pbDp1b-128*q1Dpb**2
     #*q2Dp1b-128*q1Dpb*q2Dpb*pbDp1b-256*q1Dq2*pbDp1b*mb**2-128*q1Dp1b*q
     #2Dpb*mb**2-128*q1Dpb**2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q2
     #Dpb*q2Dp1b+128*q1Dpb*pbDp1b**2+128*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dp
     #b-128*q1Dq2**2*pbDp1b+128*q1Dp1b*pbDp1b**2-128*q1Dq2**2*mb**2-128*
     #q1Dq2*mb**4-128*q1Dq2*pbDp1b**2+128*q1Dpb*mb**4)
      s12 = CV**2/den0x1/den0x3*D0_1245*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq
     #2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**2+64*q1Dpb*q2Dpb*mb**2+64*q1D
     #pb*q2Dp1b*mb**2+256*q1Dpb*q2Dp1b**2-64*q1Dp1b*q2Dpb*mb**2+128*q1Dp
     #1b*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2+256*q2Dpb*q2Dp1b*pbDp1b+38
     #4*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b**2-128*q2Dpb**2*q2Dp1b-384*q
     #2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-256*q2Dp1b*mb**4+128*q2Dp1
     #b**2*pbDp1b+256*q2Dp1b**2*mb**2)
      s10 = s11+s12
      s3 = s9+s10
      s7 = s3
      s9 = CA**2/den0x2
      s11 = 1/den0x3
      s13 = E0
      s15 = 128*q1Dp1b*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb*mb**4+256*q1Dp1b**2
     #*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dpb*q2Dp1b**2*pbDp1b
     #+256*q1Dpb**2*q2Dp1b*pbDp1b-512*q1Dpb*q1Dp1b**2*q2Dp1b+128*q1Dpb*q
     #2Dpb*mb**4-256*q1Dq2*q2Dp1b*mb**4-512*q1Dq2*q1Dpb**2*q2Dp1b+256*q1
     #Dp1b**2*q2Dp1b*pbDp1b-256*q1Dpb*q2Dp1b*pbDp1b**2+128*q1Dq2*q1Dpb*m
     #b**4-128*q1Dpb*q2Dp1b*mb**4+768*q1Dpb**2*q2Dp1b**2+256*q1Dp1b*q2Dp
     #1b**2*mb**2+128*q1Dq2*q1Dp1b*mb**4
      s14 = s15-512*q1Dpb**2*q1Dp1b*q2Dp1b+256*q1Dpb**2*q2Dp1b*mb**2-256
     #*q1Dpb*q1Dp1b*q2Dp1b**2+512*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b-256*q1Dq2*q2
     #Dp1b*pbDp1b*mb**2-768*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+1024*q1Dpb*q1Dp1b*
     #q2Dp1b*mb**2-384*q1Dpb*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*
     #mb**2+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+512*q1Dpb*q2Dpb*q2Dp1b*mb**2+1
     #024*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*pbDp1b*mb**2+256*q1
     #Dp1b*q2Dpb*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*pbDp1b*mb**2+128*q1Dp1b*q
     #2Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dp1b*pbDp1b*mb**2
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9*s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*A2e1*(384*q1Dp1b**2*q2Dp1b*mb**2+256*q
     #1Dpb**2*q2Dp1b*pbDp1b-256*q1Dpb*q1Dp1b**2*q2Dp1b+256*q1Dq2*q1Dpb**
     #2*q2Dp1b+256*q1Dp1b**2*q2Dp1b*pbDp1b-128*q1Dq2*q1Dpb*mb**4-512*q1D
     #pb**2*q1Dp1b*q2Dp1b+128*q1Dq2*pbDp1b**2*mb**2+128*q1Dp1b**2*q2Dpb*
     #mb**2+256*q1Dq2*pbDp1b*mb**4+384*q1Dpb**2*q2Dp1b*mb**2-128*q1Dq2*q
     #2Dpb*mb**4+128*q1Dpb**2*q2Dpb*mb**2+256*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b+
     #256*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb**3*q2Dp1b+128*q1Dq2*mb**6+7
     #68*q1Dpb*q1Dp1b*q2Dp1b*mb**2-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-384*q1D
     #q2*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b*mb**2-128*q1Dq2*q1Dpb
     #*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb*mb**2-384*q1Dq2*q1Dp1b*q2Dp1b*
     #mb**2-128*q1Dq2*q2Dpb*pbDp1b*mb**2+512*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-
     #256*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b)
      s5 = s7+CV**2/den0x1/den0x3*A4d2_1345*(-128*q1Dq2*q1Dpb*pbDp1b-128
     #*q1Dq2*q1Dpb*mb**2+128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dp1b+128*
     #q1Dpb**2*pbDp1b+128*q1Dpb**2*mb**2)+CA**2/den0x2/den0x3*A4d2_2345*
     #(256*q1Dq2*q1Dpb*q1Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*m
     #b**2-128*q1Dpb*q1Dp1b*q2Dp1b-256*q1Dpb*q1Dp1b**2-256*q1Dpb**2*q1Dp
     #1b+128*q1Dp1b**2*q2Dpb-256*q1Dp1b**2*pbDp1b-256*q1Dp1b**2*mb**2)
      s7 = s5
      s9 = CA**2/den0x2/den0x3*A2d1_1235*(-128*q1Dq2*q1Dpb*pbDp1b-128*q1
     #Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dpb*mb**2+64*q1Dq2*q2Dpb*mb**2-384*q
     #1Dp1b*q2Dp1b*mb**2+512*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q1Dpb*mb**2-2
     #56*q1Dp1b*q2Dpb*q2Dp1b-192*q1Dq2*q1Dp1b*mb**2-256*q1Dpb*q2Dp1b*mb*
     #*2-128*q1Dq2*q1Dp1b*pbDp1b-256*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbDp1b
     #*mb**2+128*q1Dq2*q1Dpb*q1Dp1b+128*q2Dp1b*pbDp1b**2+128*q1Dpb**2*q2
     #Dp1b+320*q1Dq2*pbDp1b*mb**2-128*q1Dp1b*q2Dpb*mb**2-320*q2Dp1b**2*m
     #b**2+128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b+256*q2Dpb*mb
     #**4+384*q2Dp1b*mb**4-64*q2Dpb**2*mb**2-128*q2Dp1b**2*pbDp1b-128*q1
     #Dp1b**2*q2Dpb+192*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s10 = CA**2/den0x1/den0x3*D0_1234*(-128*q1Dq2*q1Dpb*pbDp1b-128*q1D
     #q2*q2Dpb*pbDp1b+256*q1Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*q2Dpb+128*
     #q1Dpb*q2Dpb*mb**2-128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+25
     #6*q1Dq2*q1Dp1b*q2Dp1b-384*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2D
     #pb-384*q1Dq2*q2Dp1b*mb**2+128*q1Dq2*q2Dp1b**2-128*q1Dq2*q1Dpb*mb**
     #2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dpb+128*q1Dpb*q2Dpb*pb
     #Dp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-128*q1Dpb**2*q2
     #Dpb+128*q1Dp1b*q2Dpb*pbDp1b+384*q1Dq2*q2Dpb*q2Dp1b-256*q1Dq2**2*q2
     #Dp1b-128*q1Dp1b**2*q2Dp1b+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+2
     #56*q1Dpb*q2Dp1b**2+256*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x2/den0x3*A1d1_1345*(-128*q1Dpb*q1Dp1b*pbDp1b+12
     #8*q1Dpb*q1Dp1b*mb**2-256*q1Dpb**2*q1Dp1b+192*q1Dpb**2*mb**2+64*q1D
     #p1b**2*mb**2)
      s8 = s7+CV**2/den0x1/den0x3*A3d2_1235*(-128*q1Dpb*q1Dp1b*q2Dpb+128
     #*q1Dpb*q2Dpb*pbDp1b-256*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128
     #*q1Dp1b*q2Dpb*mb**2-128*q2Dpb*pbDp1b*mb**2+128*q2Dp1b*mb**4)
      s9 = s8
      s11 = CV**2/den0x2/den0x3*A3d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+256*q1
     #Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dpb*q1Dp1b*mb**2-128
     #*q1Dpb*q1Dp1b*q2Dpb+192*q1Dpb*q2Dpb*mb**2-256*q1Dq2*q2Dpb*mb**2+12
     #8*q1Dpb*q1Dp1b*q2Dp1b-192*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp
     #1b-64*q1Dp1b*q2Dp1b*mb**2+576*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*q2Dp1b*
     #mb**2-192*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*q2Dpb*q2Dp1b+448*q1Dq2*q1Dp
     #1b*mb**2+256*q1Dpb*q2Dp1b*mb**2+384*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*
     #q1Dpb*q1Dp1b-128*q1Dpb**2*q2Dp1b-320*q1Dq2*pbDp1b*mb**2+128*q1Dpb*
     #*2*mb**2-128*q1Dp1b*q2Dpb*pbDp1b-384*q1Dp1b*q2Dp1b*pbDp1b-192*q1Dp
     #1b**2*mb**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dq2**2*pbDp
     #1b+256*q1Dp1b*pbDp1b**2-256*q1Dq2**2*mb**2+128*q1Dpb*q1Dp1b**2-256
     #*q1Dpb*q2Dp1b**2-256*q1Dq2*mb**4-128*q1Dq2*pbDp1b**2-192*q1Dpb*mb*
     #*4)
      s12 = CV**2/den0x1/den0x3*A6d2_1345*(-128*q1Dpb*q2Dpb*mb**2+128*q1
     #Dpb*q2Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*pbDp1b**2+128*q1
     #Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**2-1
     #28*q1Dp1b*mb**4)
      s10 = s11+s12
      s4 = s9+s10
      s7 = s4
      s9 = CV**2/den0x2/den0x3*D0_1345*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2
     #*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1D
     #pb*q1Dp1b*q2Dp1b+128*q1Dpb*q1Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*mb**2-12
     #8*q1Dpb*q1Dp1b**2+128*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*mb**2+128
     #*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*pbDp1b*mb**2-128*q1Dpb*mb**4-128*q1D
     #pb**2*q1Dp1b+256*q1Dpb**2*q2Dp1b+128*q1Dpb**2*mb**2+128*q1Dp1b*q2D
     #p1b*mb**2)
      s11 = CA**2/den0x1
      s13 = 1/den0x3
      s15 = E0
      s17 = 128*q1Dp1b*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb*mb**4-128*q1Dp1b**2
     #*q2Dp1b*mb**2-256*q1Dpb**2*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*mb**4-128
     #*q1Dq2*q2Dp1b*mb**4-512*q1Dq2*q1Dpb*q2Dp1b**2-256*q1Dq2*q1Dpb*mb**
     #4-128*q1Dpb*q2Dp1b**2*mb**2+128*q1Dpb*q2Dp1b*mb**4-128*q1Dp1b*q2Dp
     #b**2*mb**2+768*q1Dpb**2*q2Dp1b**2-128*q1Dp1b*q2Dp1b**2*mb**2+256*q
     #1Dp1b**2*q2Dpb*q2Dp1b-128*q1Dpb*q2Dpb**2*mb**2-256*q1Dpb*q2Dpb**2*
     #q2Dp1b+256*q1Dpb*q2Dp1b**3-128*q1Dp1b**2*q2Dpb*mb**2
      s16 = s17-256*q1Dq2*q2Dp1b*pbDp1b**2-128*q1Dpb**2*q2Dp1b*mb**2+128
     #*q1Dq2*q2Dpb*mb**4+256*q1Dpb*q1Dp1b*q2Dp1b**2+128*q1Dpb**2*q2Dpb*m
     #b**2-384*q1Dq2*q2Dp1b*pbDp1b*mb**2-256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+2
     #56*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b*pbDp1b*mb**2-256*q1D
     #q2*q1Dpb*pbDp1b*mb**2+512*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+512*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b
     #*mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*pbDp1b*mb**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*pbDp1b*mb**2+128*q
     #1Dp1b*q2Dp1b*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x2/den0x3*A6d2_2345*(128*q1Dq2*q1Dpb*pbDp1b+256*
     #q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*mb**2-
     #128*q1Dpb*q1Dp1b*q2Dpb-384*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*pbDp1b*mb
     #**2+256*q1Dq2*q2Dp1b*pbDp1b-384*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q1Dp
     #1b*q2Dpb+640*q1Dp1b*pbDp1b*mb**2+256*q1Dq2*q2Dp1b*mb**2+256*q1Dp1b
     #*q2Dpb*q2Dp1b+512*q1Dq2*q1Dp1b*mb**2+384*q1Dpb*q2Dp1b*mb**2+384*q1
     #Dq2*q1Dp1b*pbDp1b+256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dpb**2*q2Dp1b-384*q
     #1Dq2*pbDp1b*mb**2-256*q1Dp1b*q2Dpb*mb**2+128*q1Dpb**2*mb**2-128*q1
     #Dp1b*q2Dpb*pbDp1b-512*q1Dp1b*q2Dp1b*pbDp1b-256*q1Dp1b**2*mb**2+384
     #*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dq2**2*pbDp1b+256*q1Dp1b*p
     #bDp1b**2-256*q1Dq2**2*mb**2-256*q1Dpb*q1Dp1b**2-256*q1Dpb*q2Dp1b**
     #2-256*q1Dq2*mb**4-128*q1Dq2*pbDp1b**2-128*q1Dpb*mb**4-256*q1Dp1b**
     #2*pbDp1b)
      s5 = s7+CV**2/den0x1/den0x3*A1e1*(256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b-128
     #*q1Dq2*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1
     #Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2
     #Dp1b*mb**2-256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b*mb
     #**2+128*q1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+256*q1Dpb*q
     #2Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b*mb**2)+CV**2/den0x2/den0
     #x3*A2d2_1345*(128*q1Dpb*q1Dp1b*mb**2+128*q1Dpb*pbDp1b*mb**2-128*q1
     #Dpb**2*mb**2+128*q1Dp1b*pbDp1b*mb**2)
      s7 = s5+CV**2/den0x2/den0x3*A3d1_1245*(128*q1Dq2*q1Dpb*pbDp1b-192*
     #q1Dq2*pbDp1b*mb**2-128*q1Dq2*pbDp1b**2-64*q1Dq2*mb**4+64*q1Dpb*q2D
     #pb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*
     #*2*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*mb**2-192*q2Dp
     #b*pbDp1b*mb**2-64*q2Dpb*mb**4-320*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*p
     #bDp1b**2-64*q2Dp1b*mb**4)
      s6 = s7+CV**2/den0x2/den0x3*A1d1_1234*(-256*q1Dq2*q1Dp1b*q2Dpb-192
     #*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-320*q1Dq2*q2Dp1b*mb**2)
     #+CV**2/den0x1/den0x3*A2d1_1234*(-128*q1Dq2*q1Dpb*pbDp1b-128*q1Dq2*
     #q2Dpb*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*q2Dpb+128*q1
     #Dpb*q2Dpb*mb**2-192*q1Dq2*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*mb**2-128*
     #q1Dq2*q1Dp1b*q2Dpb-64*q1Dq2*q2Dp1b*mb**2-192*q1Dq2*q1Dpb*mb**2+128
     #*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dq2*q1Dp1b*mb**2+128*q1Dq2*q1Dpb*q2Dpb+1
     #28*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dpb**2+128*q1Dpb*q2Dpb*pbDp1b+38
     #4*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-128*q1Dpb**2*q2Dpb+128
     #*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q2Dpb*q2Dp
     #1b+128*q1Dq2*q2Dpb**2-128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb**2+256*
     #q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s2 = s6+CA**2/den0x1/den0x3*A1d1_1234*(128*q1Dq2*q2Dpb*q2Dp1b+128*
     #q1Dq2*q2Dpb*pbDp1b+192*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dpb**2+64*q1D
     #q2*q2Dp1b*mb**2+128*q1Dpb*q2Dpb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*
     #q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2)+CV**2/den0x2/den0x3*A4d2_1345*(-
     #128*q1Dpb*q1Dp1b*mb**2-256*q1Dpb**2*q1Dp1b-128*q1Dp1b**2*mb**2)+CA
     #**2/den0x1/den0x3*A7d2_1235*(-128*q2Dpb*pbDp1b+128*q2Dp1b*mb**2)+C
     #V**2/den0x1/den0x3*A1d1_1234*(128*q1Dq2*q2Dpb*q2Dp1b+128*q1Dq2*q2D
     #pb*pbDp1b+192*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dpb**2+64*q1Dq2*q2Dp1b
     #*mb**2+128*q1Dpb*q2Dpb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2D
     #p1b+128*q1Dp1b*q2Dpb**2)
      s6 = CA**2/den0x1/den0x3*A6d2_1235*(256*q1Dpb*q1Dp1b*q2Dpb-128*q1D
     #pb*q2Dpb*q2Dp1b-256*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dpb*mb**2+256*q
     #1Dpb*q2Dp1b*pbDp1b+512*q1Dpb*q2Dp1b*mb**2-256*q1Dpb**2*q2Dp1b-128*
     #q1Dp1b*q2Dpb*pbDp1b-256*q1Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128
     #*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbDp1b*mb**2+128*q2Dpb*pbDp1b**2+128
     #*q2Dpb*mb**4-128*q2Dpb**2*pbDp1b-256*q2Dp1b*pbDp1b*mb**2-256*q2Dp1
     #b*mb**4)+CV**2/den0x1/den0x3*A3d1_1245*(-128*q1Dq2*q2Dp1b*mb**2+64
     #*q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp
     #b*q2Dpb*pbDp1b-64*q1Dpb*q2Dpb*mb**2+64*q1Dpb*q2Dp1b*mb**2+128*q2Dp
     #b*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2+64*q2Dpb*pbDp1b*mb**2+128*q
     #2Dpb*pbDp1b**2-64*q2Dpb*mb**4-128*q2Dpb**2*pbDp1b-448*q2Dp1b*pbDp1
     #b*mb**2-128*q2Dp1b*pbDp1b**2-320*q2Dp1b*mb**4+128*q2Dp1b**2*mb**2)
      s5 = s6+CV**2/den0x1/den0x3*A3d2_1345*(-128*q1Dp1b*q2Dpb*mb**2+128
     #*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*pbDp1b**2)
     #+CA**2/den0x1/den0x3*A5d2_1235*(128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*
     #q2Dpb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dpb**2*pbDp1b)+CA**2/den0x1/
     #den0x3*A2d2_1235*(-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q2Dpb*q2Dp1b+1
     #28*q1Dpb*q2Dpb*pbDp1b+128*q1Dpb*q2Dpb*mb**2-256*q1Dpb*q2Dp1b*pbDp1
     #b-256*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1b*q2Dpb*pbDp
     #1b+128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dpb**2-128*q2Dpb*q2Dp1b*pbD
     #p1b-128*q2Dpb*q2Dp1b*mb**2-256*q2Dpb*pbDp1b*mb**2-128*q2Dpb*pbDp1b
     #**2-128*q2Dpb*mb**4+128*q2Dpb**2*pbDp1b+128*q2Dpb**2*mb**2+256*q2D
     #p1b*pbDp1b*mb**2+128*q2Dp1b*pbDp1b**2+128*q2Dp1b*mb**4)
      s6 = s5+CA**2/den0x1/den0x3*A1e1*(256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b-128
     #*q1Dq2*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1
     #Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2
     #Dp1b*mb**2-256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b*mb
     #**2+128*q1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+256*q1Dpb*q
     #2Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q2Dpb*q2Dp1b*mb**2)+CV**2/den0x2/den0
     #x3*A1e1*(-256*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dq2*q1Dpb*q2Dpb*mb**
     #2+256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q1Dpb*q2Dp1b*mb**2-256*q
     #1Dq2*q1Dpb**2*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*mb**2+256*q1Dq2*q1Dp1b
     #*q2Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*q2Dp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1
     #b*mb**2+128*q1Dq2*q2Dpb*mb**4)
      s7 = s6+CA**2/den0x1/den0x3*A7d2_1245*(128*q2Dpb*pbDp1b-128*q2Dp1b
     #*mb**2)
      s8 = s7+CV**2/den0x1/den0x3*A3d1_2345*(128*q1Dq2*q1Dpb*pbDp1b+128*
     #q1Dq2*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*pbDp1b+128*q1Dpb*q2Dp1b*pbDp1b
     #-192*q1Dpb*q1Dp1b*mb**2+64*q1Dpb*q2Dpb*mb**2+128*q1Dq2*q2Dpb*mb**2
     #-192*q1Dpb*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-64*q1Dp1b*q2Dp1b*m
     #b**2+128*q1Dq2*q1Dp1b*q2Dpb+192*q1Dp1b*pbDp1b*mb**2-320*q1Dq2*q2Dp
     #1b*mb**2+192*q1Dq2*q1Dpb*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b+192*q1Dq2*q
     #1Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*pbDp1b+128*q1D
     #pb*q2Dpb**2-64*q1Dq2*pbDp1b*mb**2-128*q1Dp1b*q2Dpb*mb**2-128*q1Dp1
     #b*q2Dpb*pbDp1b-256*q1Dq2*q2Dpb*q2Dp1b-64*q1Dp1b**2*mb**2-128*q1Dq2
     #*q2Dpb**2-128*q1Dpb*pbDp1b**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb
     #-128*q1Dq2**2*pbDp1b-128*q1Dq2**2*mb**2-128*q1Dpb*q2Dp1b**2+128*q1
     #Dp1b*q2Dpb**2-128*q1Dq2*mb**4+64*q1Dpb*mb**4)
      s4 = s8+CA**2/den0x1/den0x3*A5d2_1245*(128*q2Dpb*q2Dp1b*mb**2-128*
     #q2Dpb**2*pbDp1b)+CA**2/den0x1/den0x3*A2d2_1245*(128*q2Dpb*pbDp1b*m
     #b**2-128*q2Dp1b*pbDp1b**2)
      s7 = s4
      s9 = CV**2/den0x2/den0x3*A2d2_2345*(-128*q1Dq2*q1Dpb*pbDp1b-128*q1
     #Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb+25
     #6*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*mb
     #**2-128*q1Dq2*q1Dp1b*q2Dpb-512*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1
     #b*mb**2-128*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dq2*q1
     #Dp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dp1b*pbDp1b-256*q1Dq
     #2*q1Dpb*q1Dp1b+128*q1Dpb**2*q2Dp1b+256*q1Dq2*pbDp1b*mb**2+128*q1Dp
     #1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dp1b*q2Dp1b*pbDp1b+25
     #6*q1Dp1b**2*mb**2+256*q1Dpb**2*q1Dp1b-256*q1Dp1b*mb**4+128*q1Dq2**
     #2*pbDp1b-256*q1Dp1b*pbDp1b**2+128*q1Dq2**2*mb**2+256*q1Dpb*q1Dp1b*
     #*2+128*q1Dpb*q2Dp1b**2+128*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2+256*q1D
     #p1b**2*pbDp1b)
      s10 = CV**2/den0x1/den0x3*A1d1_1345*(-128*q1Dq2*q1Dpb*pbDp1b-128*q
     #1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2+25
     #6*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*pbDp1b+192*q1Dpb**2*mb**2-64*q1D
     #p1b**2*mb**2)
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CA**2/den0x1/den0x3*A2d1_1245*(-128*q1Dq2*q2Dp1b*pbDp1b+64
     #*q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb
     #*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb*mb**2-64*q1
     #Dp1b*q2Dp1b*mb**2+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2+6
     #4*q2Dpb*pbDp1b*mb**2+64*q2Dpb*mb**4-128*q2Dpb**2*mb**2-448*q2Dp1b*
     #pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-320*q2Dp1b*mb**4+128*q2Dp1b**2*p
     #bDp1b)
      s8 = s7
      s10 = CA**2/den0x1/den0x3*A2d1_1345*(128*q1Dq2*q2Dp1b*mb**2+64*q1D
     #q2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2-64*q1Dq2*mb**4-64*q1Dpb*q1Dp1b
     #*mb**2-128*q1Dpb*q2Dp1b*pbDp1b-64*q1Dpb*q2Dp1b*mb**2+192*q1Dpb*pbD
     #p1b*mb**2+64*q1Dpb*mb**4+128*q1Dpb**2*q2Dp1b-64*q1Dpb**2*mb**2-64*
     #q1Dp1b*q2Dpb*mb**2+64*q1Dp1b*pbDp1b*mb**2-64*q1Dp1b*mb**4)
      s11 = CV**2/den0x1/den0x3*D0_2345*(-128*q1Dq2*q1Dpb*pbDp1b+128*q1D
     #q2*q2Dpb*pbDp1b+128*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q2Dp1b*pbDp1b+128
     #*q1Dpb*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b*q2Dpb+256*q1Dpb*q2Dpb*mb**2+6
     #4*q1Dq2*q2Dpb*mb**2-256*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b
     #+128*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**2+64*q1Dq2*q2Dp1b*m
     #b**2-256*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dpb*q2Dpb+384*q1Dpb*q2Dp1b*
     #mb**2-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb**2*pbDp1b-128*q1Dpb**2*q2Dp
     #1b-192*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2+128*q1Dpb**2*q2Dp
     #b+256*q1Dpb**2*mb**2-128*q1Dp1b*mb**4-128*q1Dpb*q2Dp1b**2-64*q1Dq2
     #*mb**4-128*q1Dq2*pbDp1b**2-256*q1Dpb*mb**4)
      s9 = s10+s11
      s5 = s8+s9
      s6 = s5+CA**2/den0x2/den0x3*A1d1_1234*(-256*q1Dq2*q1Dp1b*q2Dpb-192
     #*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-320*q1Dq2*q2Dp1b*mb**2)
     #+CA**2/den0x2/den0x3*A3d1_1245*(128*q1Dq2*q1Dpb*pbDp1b-192*q1Dq2*p
     #bDp1b*mb**2-128*q1Dq2*pbDp1b**2-64*q1Dq2*mb**4+64*q1Dpb*q2Dpb*mb**
     #2+128*q1Dpb*q2Dp1b*pbDp1b+192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp
     #1b-128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*mb**2-192*q2Dpb*pbDp1
     #b*mb**2-64*q2Dpb*mb**4-320*q2Dp1b*pbDp1b*mb**2-128*q2Dp1b*pbDp1b**
     #2-64*q2Dp1b*mb**4)
      s7 = s6+CA**2/den0x1/den0x3*A3d2_1345*(-128*q1Dp1b*q2Dpb*mb**2+128
     #*q1Dp1b*q2Dp1b*mb**2-128*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*pbDp1b**2)
      s8 = s7+CA**2/den0x1/den0x3*A3d1_1345*(128*q1Dq2*q2Dp1b*pbDp1b+64*
     #q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dpb
     #*q1Dp1b*mb**2-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+192*q1
     #Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2-192*q1Dpb*pbDp1b*mb**2-128*q1
     #Dpb*pbDp1b**2+64*q1Dpb*mb**4+128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2
     #Dpb*pbDp1b-192*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b*pbDp1b+192*q1D
     #p1b*pbDp1b*mb**2+320*q1Dp1b*mb**4-64*q1Dp1b**2*mb**2)
      s3 = s8+CV**2/den0x2/den0x3*A3d1_1345*(-128*q1Dq2*q1Dpb*mb**2+128*
     #q1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2+64*q1Dq2*pbDp1b*mb**2+6
     #4*q1Dq2*mb**4+128*q1Dpb*q1Dp1b*q2Dp1b+64*q1Dpb*q1Dp1b*mb**2-128*q1
     #Dpb*q1Dp1b**2+64*q1Dpb*q2Dp1b*mb**2-192*q1Dpb*pbDp1b*mb**2-192*q1D
     #pb*mb**4+128*q1Dpb**2*mb**2-64*q1Dp1b*q2Dpb*mb**2-256*q1Dp1b*q2Dp1
     #b*pbDp1b-256*q1Dp1b*q2Dp1b*mb**2+576*q1Dp1b*pbDp1b*mb**2+256*q1Dp1
     #b*pbDp1b**2+320*q1Dp1b*mb**4-128*q1Dp1b**2*q2Dpb-256*q1Dp1b**2*pbD
     #p1b-320*q1Dp1b**2*mb**2)+CA**2/den0x1/den0x3*A1d1_1345*(-128*q1Dq2
     #*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1
     #Dq2*q1Dp1b*mb**2+256*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*pbDp1b+192*q1
     #Dpb**2*mb**2-64*q1Dp1b**2*mb**2)
      s7 = s3
      s9 = CA**2/den0x1/den0x3*A5d2_2345*(128*q1Dq2*q1Dpb*q2Dp1b+128*q1D
     #q2*q1Dpb*pbDp1b+128*q1Dq2*q1Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dpb+128*q
     #1Dq2*q1Dp1b*pbDp1b+128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2**2*pbDp1b-128*
     #q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*pbDp1b-128
     #*q1Dpb*q1Dp1b*mb**2-128*q1Dp1b**2*q2Dpb)
      s11 = CV**2/den0x2
      s13 = 1/den0x3
      s15 = E0
      s17 = 128*q1Dp1b*q2Dp1b*mb**4+128*q1Dp1b*q2Dpb*mb**4+256*q1Dp1b**2
     #*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dpb*q2Dp1b**2*pbDp1b
     #+256*q1Dpb**2*q2Dp1b*pbDp1b-512*q1Dpb*q1Dp1b**2*q2Dp1b+128*q1Dpb*q
     #2Dpb*mb**4-256*q1Dq2*q2Dp1b*mb**4-512*q1Dq2*q1Dpb**2*q2Dp1b+256*q1
     #Dp1b**2*q2Dp1b*pbDp1b-256*q1Dpb*q2Dp1b*pbDp1b**2+128*q1Dq2*q1Dpb*m
     #b**4-128*q1Dpb*q2Dp1b*mb**4+768*q1Dpb**2*q2Dp1b**2+256*q1Dp1b*q2Dp
     #1b**2*mb**2+128*q1Dq2*q1Dp1b*mb**4
      s16 = s17-512*q1Dpb**2*q1Dp1b*q2Dp1b+256*q1Dpb**2*q2Dp1b*mb**2-256
     #*q1Dpb*q1Dp1b*q2Dp1b**2+512*q1Dq2*q1Dpb*q1Dp1b*q2Dp1b-256*q1Dq2*q2
     #Dp1b*pbDp1b*mb**2-768*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+1024*q1Dpb*q1Dp1b*
     #q2Dp1b*mb**2-384*q1Dpb*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*
     #mb**2+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+512*q1Dpb*q2Dpb*q2Dp1b*mb**2+1
     #024*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dpb*q2Dpb*pbDp1b*mb**2+256*q1
     #Dp1b*q2Dpb*q2Dp1b*mb**2+128*q1Dp1b*q2Dpb*pbDp1b*mb**2+128*q1Dp1b*q
     #2Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dp1b*pbDp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x1/den0x3*A2d1_1245*(-128*q1Dq2*q2Dp1b*pbDp1b+64
     #*q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4-128*q1Dpb*q2Dpb*mb**2+128*q1Dpb
     #*q2Dp1b*pbDp1b+128*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb*mb**2-64*q1
     #Dp1b*q2Dp1b*mb**2+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2+6
     #4*q2Dpb*pbDp1b*mb**2+64*q2Dpb*mb**4-128*q2Dpb**2*mb**2-448*q2Dp1b*
     #pbDp1b*mb**2-128*q2Dp1b*pbDp1b**2-320*q2Dp1b*mb**4+128*q2Dp1b**2*p
     #bDp1b)
      s8 = s7
      s10 = CA**2/den0x1/den0x3*A2d1_2345*(-128*q1Dq2*q1Dpb*pbDp1b+128*q
     #1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*mb**2-
     #128*q1Dpb*q1Dp1b*q2Dpb+192*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q2Dpb*mb**2
     #+128*q1Dq2*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b*q
     #2Dpb-256*q1Dp1b*pbDp1b*mb**2+192*q1Dq2*q2Dp1b*mb**2-192*q1Dq2*q1Dp
     #b*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dq2*q
     #1Dpb*q2Dpb-64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dpb*
     #q2Dpb**2-64*q1Dq2*pbDp1b*mb**2+320*q1Dp1b*q2Dpb*mb**2-128*q1Dpb**2
     #*q2Dpb+128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q2Dpb*q2Dp1b+128*q1Dp1b**
     #2*mb**2+128*q1Dq2*q2Dpb**2+128*q1Dpb*pbDp1b**2-256*q1Dp1b*mb**4+12
     #8*q1Dp1b**2*q2Dpb+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb**2+64*q1Dq2
     #*mb**4-128*q1Dq2*pbDp1b**2-128*q1Dpb*mb**4)
      s11 = CV**2/den0x2/den0x3*A3d2_1345*(128*q1Dpb*q1Dp1b*mb**2-128*q1
     #Dpb*mb**4+256*q1Dp1b*pbDp1b*mb**2+128*q1Dp1b*mb**4-128*q1Dp1b**2*m
     #b**2)
      s9 = s10+s11
      s5 = s8+s9
      s7 = s5
      s9 = CV**2/den0x1/den0x3*A4d2_1235*(-128*q1Dpb*q2Dpb*q2Dp1b+128*q1
     #Dp1b*q2Dpb**2+128*q2Dpb*q2Dp1b*pbDp1b+128*q2Dpb*q2Dp1b*mb**2-128*q
     #2Dpb**2*pbDp1b-128*q2Dpb**2*mb**2)
      s11 = CA**2/den0x1
      s13 = 1/den0x3
      s15 = A2e1
      s17 = -128*q1Dp1b**2*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*q2Dp1b-256*q1
     #Dpb**2*q2Dp1b*pbDp1b-256*q1Dq2*q2Dp1b*mb**4+256*q1Dpb*q2Dp1b*pbDp1
     #b**2-128*q1Dq2*q1Dpb*mb**4+256*q1Dpb*q2Dp1b*mb**4+256*q1Dp1b**2*q2
     #Dpb*q2Dp1b+128*q1Dq2*pbDp1b**2*mb**2-128*q1Dp1b**2*q2Dpb*mb**2+256
     #*q1Dq2*pbDp1b*mb**4-256*q1Dq2*q2Dp1b*pbDp1b**2-384*q1Dpb**2*q2Dp1b
     #*mb**2-128*q1Dq2*q2Dpb*mb**4-128*q1Dpb**2*q2Dpb*mb**2-256*q1Dpb*q1
     #Dp1b*q2Dpb*mb**2+128*q1Dq2*mb**6
      s16 = s17-512*q1Dq2*q2Dp1b*pbDp1b*mb**2+512*q1Dpb*q1Dp1b*q2Dpb*q2D
     #p1b-256*q1Dpb*q1Dp1b*q2Dp1b*mb**2+512*q1Dpb*q2Dp1b*pbDp1b*mb**2+25
     #6*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q1Dpb*q2Dp1b*mb**2-128*q1Dq2
     #*q1Dpb*pbDp1b*mb**2-256*q1Dq2*q1Dpb*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q
     #2Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dp1b*q2Dpb*mb
     #**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+25
     #6*q1Dq2*q2Dpb*q2Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b*mb**2-256*q1Dpb*
     #q2Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2
      s14 = s15*s16
      s12 = s13*s14
      s10 = s11*s12
      s8 = s9+s10
      s6 = s7+s8
      s7 = s6+CV**2/den0x1/den0x3*A4d2_1245*(128*q2Dpb*q2Dp1b*pbDp1b-128
     #*q2Dpb**2*mb**2)
      s8 = s7+CA**2/den0x2/den0x3*A3d1_1234*(-256*q1Dq2*q1Dpb*q1Dp1b-192
     #*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-320*q1Dq2*q1Dp1b*mb**2+
     #128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb**2*q2Dp1b
     #-128*q1Dp1b**2*q2Dpb)
      s9 = s8
      s11 = CA**2/den0x2/den0x3*A3d2_2345*(-128*q1Dq2*q1Dpb*q2Dp1b+128*q
     #1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb-128*q1Dq2*q1Dp1b*pbDp1b-25
     #6*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb**
     #2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4+128*q1Dq2**2*pbDp1b+128*q
     #1Dq2**2*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*q1Dp1b*mb**2-256*q
     #1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+128*q1Dpb*mb**4-128*q1Dp1b*q
     #2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb*mb**2+256*q1Dp1b*q2Dp1b*pbDp1b+128*q1
     #Dp1b*q2Dp1b*mb**2-256*q1Dp1b*pbDp1b*mb**2-128*q1Dp1b*mb**4+128*q1D
     #p1b**2*q2Dpb+128*q1Dp1b**2*mb**2)
      s12 = CA**2/den0x2/den0x3*A2d2_2345*(-128*q1Dq2*q1Dpb*pbDp1b-128*q
     #1Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q2Dp1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb+2
     #56*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dq2*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*m
     #b**2-128*q1Dq2*q1Dp1b*q2Dpb-512*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp
     #1b*mb**2-128*q1Dq2*q1Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dq2*q
     #1Dp1b*mb**2-128*q1Dpb*q2Dp1b*mb**2-256*q1Dq2*q1Dp1b*pbDp1b-256*q1D
     #q2*q1Dpb*q1Dp1b+128*q1Dpb**2*q2Dp1b+256*q1Dq2*pbDp1b*mb**2+128*q1D
     #p1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dp1b*q2Dp1b*pbDp1b+2
     #56*q1Dp1b**2*mb**2+256*q1Dpb**2*q1Dp1b-256*q1Dp1b*mb**4+128*q1Dq2*
     #*2*pbDp1b-256*q1Dp1b*pbDp1b**2+128*q1Dq2**2*mb**2+256*q1Dpb*q1Dp1b
     #**2+128*q1Dpb*q2Dp1b**2+128*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2+256*q1
     #Dp1b**2*pbDp1b)
      s10 = s11+s12
      s4 = s9+s10
      s6 = s4+CV**2/den0x1/den0x3*A7d2_1235*(-128*q2Dpb*pbDp1b+128*q2Dp1
     #b*mb**2)+CV**2/den0x1/den0x3*A2d1_1235*(-256*q1Dq2*q1Dpb*q2Dp1b-12
     #8*q2Dpb*q2Dp1b*pbDp1b-384*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q1Dp1b*q2D
     #pb+128*q1Dpb*q2Dpb*mb**2+64*q1Dq2*q2Dpb*mb**2+256*q1Dq2*q2Dp1b*pbD
     #p1b-256*q1Dp1b*q2Dp1b*mb**2+896*q2Dp1b*pbDp1b*mb**2+256*q1Dq2*q2Dp
     #1b*mb**2+128*q1Dq2*q1Dpb*mb**2+64*q1Dq2*q1Dp1b*mb**2-512*q1Dpb*q2D
     #p1b*mb**2-384*q2Dpb*q2Dp1b*mb**2-128*q2Dpb*pbDp1b*mb**2+256*q2Dp1b
     #*pbDp1b**2+256*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-192*q1Dq2*pb
     #Dp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-448*q2Dp1b**2*mb**2+128*q1Dp1b*q
     #2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q2Dpb*q2Dp1b+640*q2
     #Dp1b*mb**4-64*q2Dpb**2*mb**2-128*q2Dpb*pbDp1b**2-256*q2Dp1b**2*pbD
     #p1b+256*q2Dpb*q2Dp1b**2+128*q1Dpb*q2Dp1b**2-192*q1Dq2*mb**4)
      s5 = s6+CV**2/den0x1/den0x3*A6d2_1245*(128*q2Dpb*pbDp1b**2+128*q2D
     #pb*mb**4-256*q2Dp1b*pbDp1b*mb**2)+CV**2/den0x1/den0x3*A3d2_1245*(1
     #28*q2Dpb*pbDp1b*mb**2-128*q2Dp1b*mb**4)+CV**2/den0x1/den0x3*A3d1_1
     #345*(128*q1Dq2*q2Dp1b*pbDp1b+64*q1Dq2*pbDp1b*mb**2+64*q1Dq2*mb**4+
     #128*q1Dpb*q1Dp1b*q2Dp1b-64*q1Dpb*q1Dp1b*mb**2-128*q1Dpb*q2Dpb*mb**
     #2+128*q1Dpb*q2Dp1b*pbDp1b+192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b*
     #*2-192*q1Dpb*pbDp1b*mb**2-128*q1Dpb*pbDp1b**2+64*q1Dpb*mb**4+128*q
     #1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-192*q1Dp1b*q2Dpb*mb**2-
     #128*q1Dp1b*q2Dp1b*pbDp1b+192*q1Dp1b*pbDp1b*mb**2+320*q1Dp1b*mb**4-
     #64*q1Dp1b**2*mb**2)
      s7 = s5+CV**2/den0x1/den0x3*A2d1_2345*(-128*q1Dq2*q1Dpb*pbDp1b+128
     #*q1Dpb*q1Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*mb**
     #2-128*q1Dpb*q1Dp1b*q2Dpb+192*q1Dpb*q2Dpb*mb**2-128*q1Dq2*q2Dpb*mb*
     #*2+128*q1Dq2*q2Dp1b*pbDp1b+64*q1Dp1b*q2Dp1b*mb**2-128*q1Dq2*q1Dp1b
     #*q2Dpb-256*q1Dp1b*pbDp1b*mb**2+192*q1Dq2*q2Dp1b*mb**2-192*q1Dq2*q1
     #Dpb*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dq2*q1Dp1b*mb**2+128*q1Dq2
     #*q1Dpb*q2Dpb-64*q1Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp
     #b*q2Dpb**2-64*q1Dq2*pbDp1b*mb**2+320*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*
     #*2*q2Dpb+128*q1Dp1b*q2Dpb*pbDp1b+256*q1Dq2*q2Dpb*q2Dp1b+128*q1Dp1b
     #**2*mb**2+128*q1Dq2*q2Dpb**2+128*q1Dpb*pbDp1b**2-256*q1Dp1b*mb**4+
     #128*q1Dp1b**2*q2Dpb+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb**2+64*q1D
     #q2*mb**4-128*q1Dq2*pbDp1b**2-128*q1Dpb*mb**4)
      s8 = s7
      s10 = CV**2/den0x1/den0x3*D0_1234*(-128*q1Dq2*q1Dpb*pbDp1b-128*q1D
     #q2*q2Dpb*pbDp1b+256*q1Dq2*q1Dpb*q2Dp1b-128*q1Dpb*q1Dp1b*q2Dpb+128*
     #q1Dpb*q2Dpb*mb**2-128*q1Dq2*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+25
     #6*q1Dq2*q1Dp1b*q2Dp1b-384*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2D
     #pb-384*q1Dq2*q2Dp1b*mb**2+128*q1Dq2*q2Dp1b**2-128*q1Dq2*q1Dpb*mb**
     #2-256*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dq2*q1Dpb*q2Dpb+128*q1Dpb*q2Dpb*pb
     #Dp1b+384*q1Dq2*pbDp1b*mb**2+128*q1Dp1b*q2Dpb*mb**2-128*q1Dpb**2*q2
     #Dpb+128*q1Dp1b*q2Dpb*pbDp1b+384*q1Dq2*q2Dpb*q2Dp1b-256*q1Dq2**2*q2
     #Dp1b-128*q1Dp1b**2*q2Dp1b+128*q1Dq2**2*pbDp1b+128*q1Dq2**2*mb**2+2
     #56*q1Dpb*q2Dp1b**2+256*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s11 = CV**2/den0x1/den0x3*D0_1235*(-256*q1Dq2*q1Dpb*q2Dp1b-256*q2D
     #pb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dpb*mb**2-64*
     #q1Dq2*q2Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dp1b+512*q1Dq2*q2Dp1b*pbDp1b-
     #384*q1Dp1b*q2Dp1b*mb**2+384*q2Dp1b*pbDp1b*mb**2+704*q1Dq2*q2Dp1b*m
     #b**2-128*q1Dq2*q2Dp1b**2+128*q1Dq2*q1Dpb*mb**2+256*q1Dp1b*q2Dpb*q2
     #Dp1b-384*q1Dpb*q2Dp1b*mb**2-128*q2Dpb*q2Dp1b*mb**2+128*q2Dp1b*pbDp
     #1b**2+128*q1Dpb**2*q2Dp1b+64*q2Dp1b**2*mb**2-256*q1Dp1b*q2Dp1b*pbD
     #p1b-384*q1Dq2*q2Dpb*q2Dp1b+256*q2Dp1b*mb**4+64*q2Dpb**2*mb**2+256*
     #q1Dq2**2*q2Dp1b+128*q1Dp1b**2*q2Dp1b-128*q2Dpb*q2Dp1b**2+128*q2Dpb
     #**2*q2Dp1b-256*q1Dpb*q2Dp1b**2)
      s9 = s10+s11
      s6 = s8+s9
      s7 = s6+CV**2/den0x2/den0x3*A3d1_1234*(-256*q1Dq2*q1Dpb*q1Dp1b-192
     #*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-320*q1Dq2*q1Dp1b*mb**2+
     #128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb**2*q2Dp1b
     #-128*q1Dp1b**2*q2Dpb)
      s8 = s7+CV**2/den0x2/den0x3*A2d1_1235*(-128*q1Dq2*q1Dpb*pbDp1b-128
     #*q1Dpb*q2Dp1b*pbDp1b-128*q1Dpb*q2Dpb*mb**2+64*q1Dq2*q2Dpb*mb**2-38
     #4*q1Dp1b*q2Dp1b*mb**2+512*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q1Dpb*mb**
     #2-256*q1Dp1b*q2Dpb*q2Dp1b-192*q1Dq2*q1Dp1b*mb**2-256*q1Dpb*q2Dp1b*
     #mb**2-128*q1Dq2*q1Dp1b*pbDp1b-256*q2Dpb*q2Dp1b*mb**2+256*q2Dpb*pbD
     #p1b*mb**2+128*q1Dq2*q1Dpb*q1Dp1b+128*q2Dp1b*pbDp1b**2+128*q1Dpb**2
     #*q2Dp1b+320*q1Dq2*pbDp1b*mb**2-128*q1Dp1b*q2Dpb*mb**2-320*q2Dp1b**
     #2*mb**2+128*q1Dp1b*q2Dpb*pbDp1b-128*q1Dp1b*q2Dp1b*pbDp1b+256*q2Dpb
     #*mb**4+384*q2Dp1b*mb**4-64*q2Dpb**2*mb**2-128*q2Dp1b**2*pbDp1b-128
     #*q1Dp1b**2*q2Dpb+192*q1Dq2*mb**4+128*q1Dq2*pbDp1b**2)
      s9 = s8
      s12 = CV**2/den0x2
      s14 = 1/den0x3
      s16 = A2d1_2345
      s17 = -256*q1Dq2*q1Dpb*pbDp1b-256*q1Dq2*q1Dpb*q2Dp1b+128*q1Dpb*q1D
     #p1b*pbDp1b+128*q1Dpb*q1Dp1b*q2Dpb-64*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q
     #2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b-128*q1Dpb*pbDp1b*mb**2-128*q1Dq
     #2*q2Dp1b*pbDp1b+320*q1Dp1b*q2Dp1b*mb**2-640*q1Dp1b*pbDp1b*mb**2+64
     #*q1Dq2*q2Dp1b*mb**2-64*q1Dq2*q1Dpb*mb**2-256*q1Dp1b*q2Dpb*q2Dp1b-5
     #12*q1Dq2*q1Dp1b*mb**2-192*q1Dpb*q2Dp1b*mb**2-384*q1Dq2*q1Dp1b*pbDp
     #1b+128*q1Dq2*q1Dpb*q1Dp1b+128*q1Dpb**2*q2Dp1b+128*q1Dpb*q2Dpb*pbDp
     #1b+192*q1Dq2*pbDp1b*mb**2+64*q1Dp1b*q2Dpb*mb**2-256*q1Dpb**2*mb**2
     #+128*q1Dp1b*q2Dpb*pbDp1b+384*q1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b**2*mb
     #**2-128*q1Dpb*pbDp1b**2+128*q1Dpb**2*q1Dp1b-384*q1Dp1b*mb**4+128*q
     #1Dp1b**2*q2Dpb+256*q1Dq2**2*pbDp1b-256*q1Dp1b*pbDp1b**2+256*q1Dq2*
     #*2*mb**2-128*q1Dpb*q1Dp1b**2+256*q1Dpb*q2Dp1b**2+64*q1Dq2*mb**4+12
     #8*q1Dq2*pbDp1b**2
      s15 = s16*s17
      s13 = s14*s15
      s11 = s12*s13
      s12 = s2
      s10 = s11+s12
      t0 = s9+s10
c
      wbb_pent1=1d0/4d0/9d0*t0
c
      return
      end
c
c
      subroutine wbb_f3pf_2m(s1,s2,C0r)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 s1,s2,mb,mt,mw,mz,C0r
      complex * 16 C0
      common/wbb_masses/mb,mt,mw,mz
c
      logical*4 ldebug3pfv1
      parameter(ldebug3pfv1=.false.)
c      parameter(ldebug3pfv1=.true.)
c
      C0=1d0/(s1-s2)*(1d0/2d0*(cdlog(dcmplx(-s1/mb**2))**2-
     -                         cdlog(dcmplx(-s2/mb**2))**2))
      C0r=dreal(C0)
c
      if (ldebug3pfv1) then 
         print*,'C0_2m = ',C0
      endif
c
      return
      end
c
c-------------------------------------------------------------------------
c
      subroutine wbb_f3pf_1m(s,C0r)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 pi,ry,s,mb,mt,mw,mz,C0r
      complex * 16 igauss,C0
      common/wbb_masses/mb,mt,mw,mz
      common/wbb_constants/pi,igauss,ry
c
      logical*4 ldebug3pfv1
      parameter(ldebug3pfv1=.false.)
c      parameter(ldebug3pfv1=.true.)
c
      C0=1d0/s*(1d0/2d0*cdlog(dcmplx(-s/mb**2))**2-pi**2/6d0)
      C0r=dreal(C0)
c
      if (ldebug3pfv1) then 
         print*,'C0_1m = ',C0
      endif
c
      return
      end
c--------------------------------------------------------------------------
c
      subroutine wbb_f3pf_2m_on(s,C0r)
c
      implicit none
c
      external wbb_dot,wbb_CSPEN
c
      real * 8 wbb_dot
      complex * 16 wbb_CSPEN
      real * 8 s,tau
      complex * 16 C0
      real * 8 C0r
c
      real * 8 mb,mt,mw,mz
      common/wbb_masses/mb,mt,mw,mz
c
      tau = mb**2-s
c
      C0 = 1d0/tau*(-1d0/2d0*cdlog(dcmplx(tau/mb**2))**2+
     -  wbb_CSPEN(dcmplx(1d0-mb**2/tau)))
      C0r = dreal(C0)
c
      return
      end
c -------------------------------------------------------------------
c
      subroutine wbb_f3pf_v2(s,C0v2r)
c
      implicit none
c
      external i2,wbb_dot
c
      real*8 wbb_dot,pi
      complex*16 i2,beta,C0v2,C0v2_pole
      real*8 mb,mt,mw,s,mz
      real*8 C0v2r,C0v2r_pole
c
      common/wbb_masses/mb,mt,mw,mz
c
      pi=2d0*dasin(1d0)
c
      beta = cdsqrt(dcmplx(1d0-4d0*mb**2/s))
c
      C0v2 = 1d0/s/beta*(
     - -cdlog(dcmplx(s/mb**2))*
     -  cdlog(dcmplx((1d0-beta)/(1d0+beta)))
     - -1d0/2d0*i2(beta)-pi**2)
      C0v2_pole=1d0/s/beta*cdlog(dcmplx((1d0-beta)/(1d0+beta)))
      C0v2r=dreal(C0v2)
      C0v2r_pole=dreal(C0v2_pole)
c
      return
      end
c
c
      complex * 16 function i2(beta)
c
      implicit none
c
      external wbb_CSPEN
c
      complex*16 wbb_CSPEN,beta,yp,ym
c      
      yp = 1d0/2d0*(1d0+beta)
      ym = 1d0/2d0*(1d0-beta)
c
      i2 = -cdlog(dcmplx(yp))**2+cdlog(dcmplx(-ym))**2+
     -  2d0*cdlog(dcmplx(beta))*cdlog(dcmplx(-ym/yp))-
     -  2d0*wbb_CSPEN(dcmplx(-ym/beta))+2d0*wbb_CSPEN(dcmplx(yp/beta))
c
      return
      end
c -------------------------------------------------------------------
c
      subroutine wbb_f4pf_b1(p1,p2,p3,D0r)
c
      implicit none
c
      external wbb_dot_4,wbb_CSPEN
      real*8 wbb_dot_4
      complex * 16 wbb_CSPEN
c
      integer i
      real*8 pi,ry,p1(4),p2(4),p3(4),p12(4),p23(4),s,t,m3,m4,mb,mt,mw,
     - mz,D0r
      complex * 16 D0,igauss
      common/wbb_masses/mb,mt,mw,mz
      common/wbb_constants/pi,igauss,ry
c
      logical*4 ldebug4pfb1
      parameter(ldebug4pfb1=.false.)
c      parameter(ldebug4pfb1=.true.)
c
      do i=1,4
         p12(i)=p1(i)+p2(i)
         p23(i)=p2(i)+p3(i)
      enddo
c
      s=wbb_dot_4(p23,p23)
      t=wbb_dot_4(p12,p12)
      m3=dsqrt(wbb_dot_4(p3,p3))
      m4=dsqrt(wbb_dot_4(p2,p2))
c
      D0= 1d0/s/t*(cdlog(dcmplx(-s/mb**2))**2+cdlog(dcmplx(-t/mb**2))**2
     -  -cdlog(dcmplx(-m3**2/mb**2))**2-cdlog(dcmplx(-m4**2/mb**2))**2
     -  +1d0/2d0*cdlog(dcmplx(-m3**2*m4**2/s/mb**2))**2
     -  -2d0*wbb_CSPEN(dcmplx(1d0-m3**2/t))-2d0*wbb_CSPEN(dcmplx(1d0-m4**2/t))
     -  -cdlog(dcmplx(s/t))**2-pi**2/6d0)
      D0r=dreal(D0)
c
      if (ldebug4pfb1) then 
         print*,'=D0 = ',D0
      endif
c
      return
      end
c
c -------------------------------------------------------------------
c
      subroutine wbb_f4pf_b2(xs1,xs2,xm1,xm2,D0r)
c
      implicit none
c
      external wbb_CSPEN
      complex * 16 wbb_CSPEN
c
      real*8 xs1,xs2,xm1,xm2,mb,mt,mw,mz,D0r
      complex * 16 D0
      common/wbb_masses/mb,mt,mw,mz
c
      logical*4 ldebug4pfb2
      parameter(ldebug4pfb2=.false.)
c      parameter(ldebug4pfb2=.true.)
c
      D0= 2d0/(xs1*xs2-xm1*xm2)*(
     -  1d0/2d0*(
     -    cdlog(dcmplx(-xs1/mb**2))**2+cdlog(dcmplx(-xs2/mb**2))**2
     -   -cdlog(dcmplx(-xm1/mb**2))**2-cdlog(dcmplx(-xm2/mb**2))**2)
     -   +wbb_CSPEN(dcmplx(1d0-xm1*xm2/xs1/xs2))
     -   -wbb_CSPEN(dcmplx(1d0-xm1/xs1))-wbb_CSPEN(dcmplx(1d0-xm1/xs2))
     -   -wbb_CSPEN(dcmplx(1d0-xm2/xs1))-wbb_CSPEN(dcmplx(1d0-xm2/xs2))
     -   -1d0/2d0*cdlog(dcmplx(xs1/xs2))**2)
      D0r=dreal(D0)
c
      if (ldebug4pfb2) then 
         print*,'=D0 = ',D0
      endif
c
      return
      end
c
c -------------------------------------------------------------------
c
      subroutine wbb_f4pf_b3(xs1,xs2,xs3,D0r)
c
      implicit none
c
      external wbb_CSPEN
      complex * 16 wbb_CSPEN
c
      real*8 xs1,xs2,xs3,s12,s23,t123,sigma,tau,omega,mb,mt,mw,mz,D0r
      complex * 16 D0,igauss
      common/wbb_masses/mb,mt,mw,mz
      real*8 pi,ry
      common/wbb_constants/pi,igauss,ry
c
      logical*4 ldebug4pfb3
      parameter(ldebug4pfb3=.false.)
c      parameter(ldebug4pfb3=.true.)
c
      s12=xs1
      s23=xs2
      t123=xs3
      tau=mb**2-s12
      sigma=-t123
      omega=-s23
c
      D0= 1d0/tau/omega*(
     - +cdlog(dcmplx(tau/sigma))**2
     - +2d0*cdlog(dcmplx(sigma/mb**2))*
     -      cdlog(dcmplx((sigma-omega)/sigma))
     - -2d0*cdlog(dcmplx(tau/sigma))*cdlog(dcmplx((sigma-tau)/omega))
     - +2d0*cdlog(dcmplx(tau/sigma))*cdlog(dcmplx((sigma-tau)/sigma))
     - -2d0*cdlog(dcmplx(tau/(sigma-omega)))*
     -      cdlog(dcmplx((sigma-omega-tau)/(sigma-omega)))
     - +cdlog(dcmplx(mb**2/omega))**2
     - +2d0*cdlog(dcmplx((sigma-omega)/tau))*cdlog(dcmplx(mb**2/omega))
     - +pi**2/2d0
     - -2d0*wbb_CSPEN(dcmplx(1d0+(tau-sigma)/omega))
     - +2d0*wbb_CSPEN(dcmplx((-sigma**2+tau*sigma+sigma*omega)/tau/omega))
     - +2d0*wbb_CSPEN(dcmplx(tau/sigma))
     - -2d0*wbb_CSPEN(dcmplx(tau/(sigma-omega)))
     - -2d0*wbb_CSPEN(dcmplx(1d0-tau/(sigma-omega)))
     - -2d0*wbb_CSPEN(dcmplx(1d0-(sigma-omega)/tau)))
      D0r=dreal(D0)
c
      if (ldebug4pfb3) then 
         print*,'=D0 = ',D0
      endif
c
      return
      end
c
c -------------------------------------------------------------------
c
      subroutine wbb_f4pf_p1(xs1,xs2,xs3,D0r)
c
      implicit none
c
      external wbb_CSPEN
      complex * 16 wbb_CSPEN
c
      real*8 xs1,xs2,xs3,tau1,tau2,tau3,mb,mt,mw,mz,D0r
      complex * 16 D0,igauss,mw2
      common/wbb_masses/mb,mt,mw,mz
      real*8 pi,ry
      common/wbb_constants/pi,igauss,ry
c
      logical*4 ldebug4pfb3
      parameter(ldebug4pfb3=.false.)
c      parameter(ldebug4pfb3=.true.)
c
      tau1=-xs1
      tau2=mb**2-xs2
      tau3=mb**2-xs3
      mw2=-(mw**2+igauss*ry)      
c
      D0 = 1d0/tau1/tau2*(
c -- int4_a
     - -cdlog(dcmplx(mw2/mb**2))**2+cdlog(dcmplx(tau2/mb**2))**2
     - +2d0*cdlog(dcmplx(mw2/mb**2))*
     -      cdlog(dcmplx((tau1-mw2)*(tau2-mw2)/tau1/tau2))
     - -2d0*cdlog(dcmplx(tau2/mb**2))*
     -      cdlog(dcmplx((-tau2+mw2)/tau1))
     - +cdlog(dcmplx(tau3/mb**2))*
     -  cdlog(dcmplx(mw2*(tau3-mb**2)/(tau2*tau3-mb**2*mw2)))
     - -cdlog(dcmplx(tau3/mb**2))*
     -  cdlog(dcmplx((-tau1+mw2)*(tau3-mb**2)/
     -                (tau2*tau3-mb**2*(-tau1+mw2))))
     - +2d0*wbb_CSPEN(dcmplx(1d0-(tau1-mw2)*(tau2-mw2)/tau1/tau2))
     - -2d0*wbb_CSPEN(dcmplx(1d0-(-tau2+mw2)/tau1))
     - -wbb_CSPEN(dcmplx(1d0-tau2/mw2))
     - +wbb_CSPEN(dcmplx(1d0+tau2/(tau1-mw2)))
     - -wbb_CSPEN(dcmplx(1d0-tau2*(tau3-mb**2)/(tau2*tau3-mb**2*mw2)))
     - +wbb_CSPEN(dcmplx(1d0-mw2*(tau3-mb**2)/(tau2*tau3-mb**2*mw2)))
     - +wbb_CSPEN(dcmplx(1d0-tau2*(tau3-mb**2)/
     -               (tau2*tau3-mb**2*(-tau1+mw2))))
     - -wbb_CSPEN(dcmplx(1d0-(mw2-tau1)*(tau3-mb**2)/
     -               (tau2*tau3-mb**2*(-tau1+mw2))))
c -- int4_b 
     - +cdlog(dcmplx(tau1/mb**2))**2
     - -2d0*cdlog(dcmplx(tau1/mb**2))*
     -      cdlog(dcmplx((-tau1+mw2)/tau2))
     - +cdlog(dcmplx(tau3/mb**2))*
     -  cdlog(dcmplx((tau1-mw2)*(mb**2-tau3)/
     -               (tau2*tau3+mb**2*(tau1-mw2))))
     - +pi**2/3d0
     - -wbb_CSPEN(dcmplx(1d0+tau2/(tau1-mw2)))
     - +wbb_CSPEN(dcmplx(1d0-tau3/mb**2))
     - +wbb_CSPEN(dcmplx(1d0-(tau1-mw2)*(mb**2-tau3)/
     -               (tau2*tau3+mb**2*(tau1-mw2))))
     - -wbb_CSPEN(dcmplx(1d0+tau2*(mb**2-tau3)/
     -              (tau2*tau3+mb**2*(tau1-mw2))))
     - -2d0*wbb_CSPEN(dcmplx(1d0+(tau1-mw2)/tau2))
     - )
      D0r = dreal(D0)
c
      return
      end
c
c -------------------------------------------------------------------
c
      subroutine wbb_f5pf_p1(s12,s23,s34,t123,t234,
     -                   D0_1,D0_2,D0_3,D0_4,D0_5,E0)
c
      implicit none
c
      real*8 s12,s23,s34,t123,t234,mb,mt,mw,mz
      real*8 c1,c2,c3,c4,c5,D0_1,D0_2,D0_3,D0_4,D0_5,E0
      common/wbb_masses/mb,mt,mw,mz
c
      logical*4 ldebug5pf
      parameter(ldebug5pf=.false.)
c      parameter(ldebug5pf=.true.)
c
c -- c1
      c1 = -(2*t123*mw**2*t234*mb**2-t123*t234*mb**2*s23-2*t123*mw**2*t2
     #34*s34+t123*t234*s34*s23-2*t123*mw**2*mb**4+t123*mw**2*s23*mb**2+2
     #*t123*mw**2*mb**2*s34-2*t123*mw**4*mb**2+t123*s23*mb**2*s34-t123*s
     #23*s34**2+t123*mw**2*s23*s34-s23*s12*t234*mb**2+s23*s12*t234*s34+s
     #23**2*mb**4+s23*s12*mb**4-2*s23**2*mb**2*s34-s23*s12*mb**2*s34-s23
     #**2*s12*mb**2+2*mw**2*s23*s12*mb**2+s23**2*s34**2-s23**2*s12*s34)/
     #(-t123*mw**2*mb**2-mb**4*t123-t234*t123*s34+t234*t123*mb**2+mb**2*
     #t123*s34+s12*s23*mb**2)/(t123*mw**2-s12*s23)
c -- c2
      c2 = -(t123**2*t234*mb**2-t123**2*t234*s34-t123**2*mw**2*mb**2-t12
     #3**2*mb**2*s34+t123**2*s34**2-t123**2*mw**2*s34+s12*t234*t123*s34-
     #s12*t234*t123*mb**2+s12*mb**4*t123-t123*s23*mb**4+2*t123*mw**2*s12
     #*mb**2+t123*s12*s23*mb**2-s12*mb**2*t123*s34+2*t123*s23*mb**2*s34-
     #t123*s23*s34**2+t123*s12*s23*s34-2*s12**2*s23*mb**2)/(-t123*mw**2*
     #mb**2-mb**4*t123-t234*t123*s34+t234*t123*mb**2+mb**2*t123*s34+s12*
     #s23*mb**2)/(t123*mw**2-s12*s23)

c -- c3      
      c3 = -(t123**2*t234**2-t123**2*t234*mb**2-t123**2*t234*mw**2-t123*
     #*2*t234*s34+t123**2*mb**2*s34-t123**2*mw**2*mb**2-s12*t123*t234**2
     #-t123*t234*mb**2*s23+2*s12*t234*t123*mb**2+t123*t234*s34*s23+t123*
     #s23*s12*t234-s12*mb**4*t123+t123*s23*mb**4+2*t123*mw**2*s23*mb**2+
     #t123*s12*s23*mb**2-t123*s23*mb**2*s34-2*s23**2*s12*mb**2)/(-t123*m
     #w**2*mb**2-mb**4*t123-t234*t123*s34+t234*t123*mb**2+mb**2*t123*s34
     #+s12*s23*mb**2)/(t123*mw**2-s12*s23)

c -- c4      
      c4 = (s12*t123*t234**2-s12*t234*t123*mb**2-2*t123*mw**2*t234*mb**2
     #-t123*t234*mw**2*s12+2*t123*mw**2*t234*s34-s12*t234*t123*s34+2*t12
     #3*mw**2*mb**4-2*t123*mw**2*mb**2*s34+2*t123*mw**4*mb**2+s12*mb**2*
     #t123*s34-t123*mw**2*s12*mb**2-s12**2*t234**2+2*s12**2*t234*mb**2+s
     #23*s12*t234*mb**2-s23*s12*t234*s34+t234*s12**2*s23-s12**2*mb**4-s2
     #3*s12*mb**4-2*mw**2*s23*s12*mb**2+s12**2*s23*mb**2+s23*s12*mb**2*s
     #34)/(-t123*mw**2*mb**2-mb**4*t123-t234*t123*s34+t234*t123*mb**2+mb
     #**2*t123*s34+s12*s23*mb**2)/(t123*mw**2-s12*s23)
c -- c5      
      c5 = (s23*mb**2-s23*s34-2*t123*mb**2+t123*s34+t123*t234+s12*mb**2-
     #s12*t234-t123*mw**2+s12*s23)/(-t123*mw**2*mb**2-mb**4*t123-t234*t1
     #23*s34+t234*t123*mb**2+mb**2*t123*s34+s12*s23*mb**2)
c
      E0=-1d0/2d0*(c1*D0_1+c2*D0_2+c3*D0_3+c4*D0_4+c5*D0_5)
c
      if (ldebug5pf) then 
         print*,'=E0 = ',E0
      endif
c
      return
      end

      subroutine wbb_mm2_sme(q1,q2,pb,p1b,pw,m2_sme_v,m2_sme_a)
c
      implicit none
c
      external wbb_dot_4
      real*8 wbb_dot_4
c
      real*8 mb,mt,mw,mz,q1(4),q2(4),pb(4),p1b(4),pw(4)
      real*8 m2_sme_v(75),m2_sme_a(75)
      real*8 CV,CA,den0x1,den0x2,den0x3,
     -       q1Dq2,q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,pbDp1b,s1,s2,t0
      common/wbb_masses/mb,mt,mw,mz
c
      CV=1d0
      CA=-1d0
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
c
c
c ----------m2_sme_v(1) ---------
c
      t0 = CV/den0x1/den0x3*(64.D0*q1Dq2*q2Dpb*mb-64.D0*q1Dq2*q2Dp1b*mb+
     #64.D0*q1Dpb*q2Dp1b*mb-64.D0*q1Dp1b*q2Dpb*mb)+CV/den0x2/den0x3*(-64
     #.D0*q1Dq2*q1Dpb*mb+64.D0*q1Dq2*q1Dp1b*mb+64.D0*q1Dpb*q2Dp1b*mb-64.
     #D0*q1Dp1b*q2Dpb*mb)
c
      m2_sme_v(1) = t0
c
c ----------m2_sme_v(2) ---------
c
      t0 = CV/den0x1/den0x3*(-64.D0*q1Dq2*q1Dpb*q2Dp1b-32.D0*q1Dq2*q1Dpb
     #*pbDp1b-32.D0*q1Dq2*q1Dpb*mb**2-64.D0*q1Dq2*q1Dp1b*q2Dpb-32.D0*q1D
     #q2*q1Dp1b*pbDp1b-32.D0*q1Dq2*q1Dp1b*mb**2+64.D0*q1Dq2**2*pbDp1b+64
     #.D0*q1Dq2**2*mb**2-32.D0*q1Dpb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2D
     #p1b+32.D0*q1Dpb**2*q2Dp1b+32.D0*q1Dp1b**2*q2Dpb)+CV/den0x2/den0x3*
     #(128.D0*q1Dq2*q1Dpb*q1Dp1b-32.D0*q1Dq2*q1Dpb*pbDp1b-32.D0*q1Dq2*q1
     #Dpb*mb**2-32.D0*q1Dq2*q1Dp1b*pbDp1b-32.D0*q1Dq2*q1Dp1b*mb**2-32.D0
     #*q1Dpb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb**2*q2Dp1
     #b+32.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(2) = t0
c
c ----------m2_sme_v(3) ---------
c
      t0 = CV/den0x1/den0x3*(-128.D0*q1Dq2*q2Dpb*q2Dp1b+32.D0*q1Dq2*q2Dp
     #b*pbDp1b+32.D0*q1Dq2*q2Dpb*mb**2+32.D0*q1Dq2*q2Dp1b*pbDp1b+32.D0*q
     #1Dq2*q2Dp1b*mb**2+32.D0*q1Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+3
     #2.D0*q1Dp1b*q2Dpb*q2Dp1b-32.D0*q1Dp1b*q2Dpb**2)+CV/den0x2/den0x3*(
     #64.D0*q1Dq2*q1Dpb*q2Dp1b+64.D0*q1Dq2*q1Dp1b*q2Dpb+32.D0*q1Dq2*q2Dp
     #b*pbDp1b+32.D0*q1Dq2*q2Dpb*mb**2+32.D0*q1Dq2*q2Dp1b*pbDp1b+32.D0*q
     #1Dq2*q2Dp1b*mb**2-64.D0*q1Dq2**2*pbDp1b-64.D0*q1Dq2**2*mb**2+32.D0
     #*q1Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+32.D0*q1Dp1b*q2Dpb*q2Dp1
     #b-32.D0*q1Dp1b*q2Dpb**2)
c
      m2_sme_v(3) = t0
c
c ----------m2_sme_v(4) ---------
c
      t0 = CV/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-32.D0*q1Dq2*q2Dp1
     #b*mb**2+96.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*pbDp1b**2+64.D0*q1Dq2
     #*mb**4+96.D0*q1Dpb*q2Dpb*q2Dp1b-64.D0*q1Dpb*q2Dpb*pbDp1b-128.D0*q1
     #Dpb*q2Dpb*mb**2-32.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*q1Dpb*q2Dp1b*mb**2
     #-32.D0*q1Dp1b*q2Dpb*pbDp1b-64.D0*q1Dp1b*q2Dpb*mb**2+32.D0*q1Dp1b*q
     #2Dpb**2)+CV/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+32.D0*q1Dq2*q1
     #Dp1b*mb**2-96.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*pbDp1b**2-64.D0*q1
     #Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*q2Dpb+64.D0*q1Dpb*q2Dpb*pbDp1b+128.D0
     #*q1Dpb*q2Dpb*mb**2+32.D0*q1Dpb*q2Dp1b*pbDp1b+64.D0*q1Dpb*q2Dp1b*mb
     #**2-32.D0*q1Dpb**2*q2Dp1b+32.D0*q1Dp1b*q2Dpb*pbDp1b+64.D0*q1Dp1b*q
     #2Dpb*mb**2)
c
      m2_sme_v(4) = t0
c
c ----------m2_sme_v(5) ---------
c
      t0 = CV/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*mb**2-32.D0*q1Dq2*q2Dp1b
     #*pbDp1b+96.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*pbDp1b**2+64.D0*q1Dq2
     #*mb**4-32.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*q1Dpb*q2Dp1b*mb**2+32.D0*q1
     #Dpb*q2Dp1b**2+96.D0*q1Dp1b*q2Dpb*q2Dp1b-32.D0*q1Dp1b*q2Dpb*pbDp1b-
     #64.D0*q1Dp1b*q2Dpb*mb**2-64.D0*q1Dp1b*q2Dp1b*pbDp1b-128.D0*q1Dp1b*
     #q2Dp1b*mb**2)+CV/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*mb**2+32.D0*q1Dq
     #2*q1Dp1b*pbDp1b-96.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*pbDp1b**2-64.
     #D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb*q2Dp1b*pbDp1b
     #+64.D0*q1Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dpb*pbDp1b+64.D0*q1Dp1b*q
     #2Dpb*mb**2+64.D0*q1Dp1b*q2Dp1b*pbDp1b+128.D0*q1Dp1b*q2Dp1b*mb**2-3
     #2.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(5) = t0
c
c ----------m2_sme_v(6) ---------
c
      t0 = 0.D0
c
      m2_sme_v(6) = t0
c
c ----------m2_sme_v(7) ---------
c
      t0 = CV/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+96.D0*q1Dq2*q1Dpb*
     #mb**2+32.D0*q1Dq2*q1Dp1b*pbDp1b+96.D0*q1Dq2*q1Dp1b*mb**2-32.D0*q1D
     #pb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb**2*q2Dp1b+32
     #.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(7) = t0
c
c ----------m2_sme_v(8) ---------
c
      t0 = CV/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-96.D0*q1Dq2*q2Dpb
     #*mb**2-32.D0*q1Dq2*q2Dp1b*pbDp1b-96.D0*q1Dq2*q2Dp1b*mb**2+32.D0*q1
     #Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+32.D0*q1Dp1b*q2Dpb*q2Dp1b-3
     #2.D0*q1Dp1b*q2Dpb**2)
c
      m2_sme_v(8) = t0
c
c ----------m2_sme_v(9) ---------
c
      t0 = CV/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-32.D0*q1Dq2*q2Dp1
     #b*mb**2-32.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*mb**4+96.D0*q1Dpb*q2D
     #pb*q2Dp1b-32.D0*q1Dpb*q2Dpb*mb**2-64.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*
     #q1Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dpb**2+32.D0*q1Dp1b*q2Dp1b*mb**2
     #)+CV/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+32.D0*q1Dq2*q1Dp1b*mb
     #**2+32.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*
     #q2Dpb+32.D0*q1Dpb*q2Dpb*mb**2-32.D0*q1Dpb**2*q2Dp1b+64.D0*q1Dp1b*q
     #2Dpb*pbDp1b+64.D0*q1Dp1b*q2Dpb*mb**2-32.D0*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_v(9) = t0
c
c ----------m2_sme_v(10) ---------
c
      t0 = CV/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*mb**2-32.D0*q1Dq2*q2Dp1b
     #*pbDp1b-32.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*mb**4+32.D0*q1Dpb*q2D
     #pb*mb**2+32.D0*q1Dpb*q2Dp1b**2+96.D0*q1Dp1b*q2Dpb*q2Dp1b-64.D0*q1D
     #p1b*q2Dpb*pbDp1b-64.D0*q1Dp1b*q2Dpb*mb**2-32.D0*q1Dp1b*q2Dp1b*mb**
     #2)+CV/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*mb**2+32.D0*q1Dq2*q1Dp1b*pb
     #Dp1b+32.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b
     #*q2Dp1b-32.D0*q1Dpb*q2Dpb*mb**2+64.D0*q1Dpb*q2Dp1b*pbDp1b+64.D0*q1
     #Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dp1b*mb**2-32.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(10) = t0
c
c ----------m2_sme_v(11) ---------
c
      t0 = CV/den0x1/den0x3*(-64.D0*q1Dq2*q2Dpb*pbDp1b-64.D0*q1Dq2*q2Dp1
     #b*mb**2+192.D0*q1Dq2*pbDp1b*mb**2+64.D0*q1Dq2*pbDp1b**2+128.D0*q1D
     #q2*mb**4-64.D0*q1Dpb*q2Dpb*q2Dp1b+64.D0*q1Dpb*q2Dp1b*pbDp1b+128.D0
     #*q1Dpb*q2Dp1b*mb**2-64.D0*q1Dp1b*q2Dpb*pbDp1b-128.D0*q1Dp1b*q2Dpb*
     #mb**2+64.D0*q1Dp1b*q2Dpb**2)+CV/den0x2/den0x3*(-64.D0*q1Dq2*q1Dpb*
     #pbDp1b-128.D0*q1Dq2*q1Dpb*mb**2+64.D0*q1Dq2*q1Dp1b*mb**2-64.D0*q1D
     #q2*pbDp1b*mb**2-64.D0*q1Dq2*mb**4-64.D0*q1Dpb*q1Dp1b*q2Dpb+128.D0*
     #q1Dpb*q2Dpb*pbDp1b+192.D0*q1Dpb*q2Dpb*mb**2+128.D0*q1Dpb*q2Dp1b*mb
     #**2+64.D0*q1Dpb**2*q2Dp1b+64.D0*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_v(11) = t0
c
c ----------m2_sme_v(12) ---------
c
      t0 = CV/den0x1/den0x3*(-64.D0*q1Dq2*q2Dpb*mb**2-64.D0*q1Dq2*q2Dp1b
     #*pbDp1b+192.D0*q1Dq2*pbDp1b*mb**2+64.D0*q1Dq2*pbDp1b**2+128.D0*q1D
     #q2*mb**4-64.D0*q1Dpb*q2Dp1b*pbDp1b-128.D0*q1Dpb*q2Dp1b*mb**2+64.D0
     #*q1Dpb*q2Dp1b**2-64.D0*q1Dp1b*q2Dpb*q2Dp1b+64.D0*q1Dp1b*q2Dpb*pbDp
     #1b+128.D0*q1Dp1b*q2Dpb*mb**2)+CV/den0x2/den0x3*(64.D0*q1Dq2*q1Dpb*
     #mb**2-64.D0*q1Dq2*q1Dp1b*pbDp1b-128.D0*q1Dq2*q1Dp1b*mb**2-64.D0*q1
     #Dq2*pbDp1b*mb**2-64.D0*q1Dq2*mb**4-64.D0*q1Dpb*q1Dp1b*q2Dp1b+64.D0
     #*q1Dpb*q2Dpb*mb**2+128.D0*q1Dp1b*q2Dpb*mb**2+128.D0*q1Dp1b*q2Dp1b*
     #pbDp1b+192.D0*q1Dp1b*q2Dp1b*mb**2+64.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(12) = t0
c
c ----------m2_sme_v(13) ---------
c
      t0 = CV/den0x1/den0x3*(64.D0*q1Dq2*q2Dpb*pbDp1b*mb-64.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb-64.D0*q1Dq2*pbDp1b**2*mb+64.D0*q1Dq2*mb**5-64.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+64.D0*q1Dpb*q2Dp1b*pbDp1b*mb+64.D0*q1Dpb*q2Dp1b
     #**2*mb-64.D0*q1Dp1b*q2Dpb*q2Dp1b*mb-64.D0*q1Dp1b*q2Dpb*pbDp1b*mb+6
     #4.D0*q1Dp1b*q2Dpb**2*mb)+CV/den0x2/den0x3*(-64.D0*q1Dq2*q1Dpb*pbDp
     #1b*mb+64.D0*q1Dq2*q1Dp1b*pbDp1b*mb+64.D0*q1Dq2*pbDp1b**2*mb-64.D0*
     #q1Dq2*mb**5-64.D0*q1Dpb*q1Dp1b*q2Dpb*mb-64.D0*q1Dpb*q1Dp1b*q2Dp1b*
     #mb+128.D0*q1Dpb*q2Dpb*mb**3-64.D0*q1Dpb*q2Dp1b*pbDp1b*mb+64.D0*q1D
     #pb**2*q2Dp1b*mb-192.D0*q1Dp1b*q2Dpb*pbDp1b*mb+128.D0*q1Dp1b*q2Dp1b
     #*mb**3+64.D0*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_v(13) = t0
c
c ----------m2_sme_v(14) ---------
c
      t0 = CV/den0x1/den0x3*(32.D0*q1Dq2*q2Dpb*pbDp1b*mb-32.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb-32.D0*q1Dq2*pbDp1b**2*mb+32.D0*q1Dq2*mb**5-32.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+32.D0*q1Dpb*q2Dp1b**2*mb+32.D0*q1Dpb*q2Dp1b*pbD
     #p1b*mb-32.D0*q1Dp1b*q2Dpb**2*mb+32.D0*q1Dp1b*q2Dpb*q2Dp1b*mb+32.D0
     #*q1Dp1b*q2Dpb*pbDp1b*mb-64.D0*q1Dp1b*q2Dp1b*mb**3)+CV/den0x2/den0x
     #3*(-32.D0*q1Dq2*q1Dpb*pbDp1b*mb+32.D0*q1Dq2*q1Dp1b*pbDp1b*mb+32.D0
     #*q1Dq2*pbDp1b**2*mb-32.D0*q1Dq2*mb**5+32.D0*q1Dpb**2*q2Dp1b*mb+32.
     #D0*q1Dpb*q1Dp1b*q2Dpb*mb-32.D0*q1Dpb*q1Dp1b*q2Dp1b*mb-32.D0*q1Dpb*
     #q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b**2*q2Dpb*mb-32.D0*q1Dp1b*q2Dpb*pbDp1
     #b*mb+64.D0*q1Dp1b*q2Dp1b*mb**3)
c
      m2_sme_v(14) = t0
c
c ----------m2_sme_v(15) ---------
c
      t0 = CV/den0x1/den0x3*(32.D0*q1Dq2*q2Dpb*pbDp1b*mb-32.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb+32.D0*q1Dq2*pbDp1b**2*mb-32.D0*q1Dq2*mb**5-32.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+64.D0*q1Dpb*q2Dpb*mb**3+32.D0*q1Dpb*q2Dp1b**2*m
     #b-32.D0*q1Dpb*q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b*q2Dpb**2*mb+32.D0*q1Dp
     #1b*q2Dpb*q2Dp1b*mb-32.D0*q1Dp1b*q2Dpb*pbDp1b*mb)+CV/den0x2/den0x3*
     #(-32.D0*q1Dq2*q1Dpb*pbDp1b*mb+32.D0*q1Dq2*q1Dp1b*pbDp1b*mb-32.D0*q
     #1Dq2*pbDp1b**2*mb+32.D0*q1Dq2*mb**5+32.D0*q1Dpb**2*q2Dp1b*mb+32.D0
     #*q1Dpb*q1Dp1b*q2Dpb*mb-32.D0*q1Dpb*q1Dp1b*q2Dp1b*mb-64.D0*q1Dpb*q2
     #Dpb*mb**3+32.D0*q1Dpb*q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b**2*q2Dpb*mb+32
     #.D0*q1Dp1b*q2Dpb*pbDp1b*mb)
c
      m2_sme_v(15) = t0
c
c ----------m2_sme_v(16) ---------
c
      t0 = CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-32*q1Dq2*q1Dpb**2*pbDp1b-32*q1Dq2*q1Dp1b**2*mb**2+32
     #*q1Dpb*q1Dp1b**2*q2Dpb-32*q1Dpb**2*q1Dp1b*q2Dpb-32*q1Dpb**2*q1Dp1b
     #*q2Dp1b+32*q1Dpb**3*q2Dp1b)
c
      m2_sme_v(16) = t0
c
c ----------m2_sme_v(17) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dpb*pbDp1b-64*q1Dq2*q1Dpb
     #*q2Dpb*mb**2-32*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*q2Dp1b*mb
     #**2-64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1
     #Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*pbDp1
     #b**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp1
     #b*q2Dpb**2+32*q1Dpb**2*q2Dpb*q2Dp1b+32*q1Dp1b**2*q2Dpb**2)
c
      m2_sme_v(17) = t0
c
c
c ----------m2_sme_v(18) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*pbDp1b-64*q1Dq2*q1Dpb
     #*q2Dpb*mb**2-32*q1Dq2*q1Dpb*q2Dp1b*mb**2+48*q1Dq2*q1Dpb*pbDp1b*mb*
     #*2+32*q1Dq2*q1Dpb*pbDp1b**2+16*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*q
     #2Dpb*mb**2-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q
     #1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*pbDp1
     #b-16*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2Dpb**2+16*q1Dpb*q1
     #Dp1b*q2Dp1b*mb**2+64*q1Dpb**2*q2Dpb*q2Dp1b-32*q1Dpb**2*q2Dpb*mb**2
     #-32*q1Dpb**2*q2Dp1b*pbDp1b-48*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp1b**2*q
     #2Dpb*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*mb**2-16*q1Dq2*q1Dpb*
     #pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*
     #q1Dq2*q1Dp1b*mb**4+64*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+48*q1Dpb*q1Dp1b*q2
     #Dpb*mb**2-16*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb**2*q1Dp1b*q2Dpb+3
     #2*q1Dpb**2*q2Dpb*mb**2+16*q1Dpb**2*q2Dp1b*mb**2-16*q1Dp1b**2*q2Dpb
     #*mb**2)
      t0 = s1+s2
c
      m2_sme_v(18) = t0
c
c ----------m2_sme_v(19) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2-16*q1Dq2*q1Dpb*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-64
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2+48*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2+48*q1Dq2*q1Dp1b*mb**4+32*q1Dq2**2*pbDp1b*mb**2+3
     #2*q1Dq2**2*pbDp1b**2+64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dpb*q1Dp1b*
     #q2Dpb*pbDp1b-80*q1Dpb*q1Dp1b*q2Dpb*mb**2-16*q1Dpb*q1Dp1b*q2Dp1b*mb
     #**2+32*q1Dpb**2*q2Dpb*mb**2+16*q1Dpb**2*q2Dp1b*mb**2+32*q1Dpb**2*q
     #2Dp1b**2-16*q1Dp1b**2*q2Dpb*mb**2+32*q1Dp1b**2*q2Dpb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-32*q1Dq2*q1Dpb*pbDp1b**2+16*q1Dq2*q1Dpb*mb**4-16*q1D
     #q2*q1Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q1Dpb*q1Dp1b*q2Dpb
     #*pbDp1b+48*q1Dpb*q1Dp1b*q2Dpb*mb**2+16*q1Dpb*q1Dp1b*q2Dp1b*mb**2-6
     #4*q1Dpb*q1Dp1b**2*q2Dpb-64*q1Dpb**2*q1Dp1b*q2Dp1b-32*q1Dpb**2*q2Dp
     #b*mb**2+32*q1Dpb**2*q2Dp1b*pbDp1b+16*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp
     #1b**2*q2Dpb*mb**2)
      t0 = s1+s2
c
      m2_sme_v(19) = t0
c
c ----------m2_sme_v(20) ---------
c
      t0 = CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dpb*pbDp1b+64*q1Dq2*q1Dpb*
     #q2Dpb*mb**2+64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb*q2Dp1b*mb*
     #*2+32*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+64*q1Dq2*q1Dp1b*q2Dpb*mb**2-32*q1D
     #q2*q1Dp1b*q2Dp1b*mb**2-64*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*pbDp1b
     #**2-32*q1Dq2**2*mb**4+32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp1b
     #*q2Dpb**2+32*q1Dpb**2*q2Dpb*q2Dp1b-32*q1Dpb**2*q2Dp1b**2)
c
      m2_sme_v(20) = t0
c
c ----------m2_sme_v(21) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q2Dp
     #b*q2Dp1b*mb**2+32*q1Dq2*q2Dpb**2*pbDp1b+32*q1Dq2*q2Dp1b**2*mb**2-3
     #2*q1Dpb*q2Dpb*q2Dp1b**2+32*q1Dpb*q2Dpb**2*q2Dp1b+32*q1Dp1b*q2Dpb**
     #2*q2Dp1b-32*q1Dp1b*q2Dpb**3)
c
      m2_sme_v(21) = t0
c
c ----------m2_sme_v(22) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*mb**2+16*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16
     #*q1Dq2*q2Dp1b*mb**4-64*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-48*q1Dpb*q2Dpb*q2
     #Dp1b*mb**2+128*q1Dpb*q2Dpb**2*q2Dp1b-32*q1Dpb*q2Dpb**2*mb**2+16*q1
     #Dpb*q2Dp1b**2*mb**2+16*q1Dp1b*q2Dpb*q2Dp1b*mb**2-16*q1Dp1b*q2Dpb**
     #2*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dpb*pbDp1b+64*q1Dq2*q1Dpb*
     #q2Dpb*mb**2+32*q1Dq2*q1Dpb*q2Dp1b*mb**2+32*q1Dq2*q1Dp1b*q2Dpb*mb**
     #2-48*q1Dq2*q2Dpb*pbDp1b*mb**2-32*q1Dq2*q2Dpb*pbDp1b**2-16*q1Dq2*q2
     #Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1
     #Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*mb**4-64*q1Dpb*q1Dp1b*q2Dpb**2+32*
     #q1Dpb*q2Dpb*q2Dp1b*pbDp1b+16*q1Dpb*q2Dpb*q2Dp1b*mb**2+32*q1Dpb*q2D
     #pb**2*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-64*q1Dpb**2*q2Dpb*q2Dp1b-16*q
     #1Dp1b*q2Dpb*q2Dp1b*mb**2+32*q1Dp1b*q2Dpb**2*pbDp1b+48*q1Dp1b*q2Dpb
     #**2*mb**2)
      t0 = s1+s2
c
      m2_sme_v(22) = t0
c
c ----------m2_sme_v(23) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+16*q1Dq2*q2Dp
     #b*pbDp1b*mb**2+32*q1Dq2*q2Dpb*pbDp1b**2-16*q1Dq2*q2Dpb*mb**4+16*q1
     #Dq2*q2Dp1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1Dpb*q2Dpb*q2Dp1
     #b*pbDp1b-48*q1Dpb*q2Dpb*q2Dp1b*mb**2+64*q1Dpb*q2Dpb*q2Dp1b**2+32*q
     #1Dpb*q2Dpb**2*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-16*q1Dp1b*q2Dpb*q2Dp1
     #b*mb**2+64*q1Dp1b*q2Dpb**2*q2Dp1b-32*q1Dp1b*q2Dpb**2*pbDp1b-16*q1D
     #p1b*q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+32*q1Dq2*q1Dp1b*q2Dpb*m
     #b**2+16*q1Dq2*q2Dpb*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4-48*q1Dq2*q2D
     #p1b*pbDp1b*mb**2-48*q1Dq2*q2Dp1b*mb**4-32*q1Dq2**2*pbDp1b*mb**2-32
     #*q1Dq2**2*pbDp1b**2-64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+64*q1Dpb*q2Dpb*q2
     #Dp1b*pbDp1b+80*q1Dpb*q2Dpb*q2Dp1b*mb**2-32*q1Dpb*q2Dpb**2*mb**2+16
     #*q1Dpb*q2Dp1b**2*mb**2-32*q1Dpb**2*q2Dp1b**2+16*q1Dp1b*q2Dpb*q2Dp1
     #b*mb**2-16*q1Dp1b*q2Dpb**2*mb**2-32*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_v(23) = t0
c
c ----------m2_sme_v(24) ---------
c
      t0 = CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-32*q1Dq2*q1Dpb**2*mb**2-32*q1Dq2*q1Dp1b**2*pbDp1b-32
     #*q1Dpb*q1Dp1b**2*q2Dpb-32*q1Dpb*q1Dp1b**2*q2Dp1b+32*q1Dpb**2*q1Dp1
     #b*q2Dp1b+32*q1Dp1b**3*q2Dpb)
c
      m2_sme_v(24) = t0
c
c ----------m2_sme_v(25) ---------
c
      t0 = CV/den0x1/den0x3*(32*q1Dq2*q1Dpb*q2Dpb*mb**2-64*q1Dq2*q1Dpb*q
     #2Dp1b*pbDp1b-32*q1Dq2*q1Dpb*q2Dp1b*mb**2-32*q1Dq2*q1Dp1b*q2Dpb*pbD
     #p1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-32*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-64*q
     #1Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*pbDp
     #1b**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp
     #1b*q2Dp1b**2+32*q1Dpb**2*q2Dp1b**2+32*q1Dp1b**2*q2Dpb*q2Dp1b)
c
      m2_sme_v(25) = t0
c
c ----------m2_sme_v(26) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2+48*q1Dq2*q1Dpb*pbDp1b*mb**2+48*q1Dq2*q1Dpb*mb**4-64
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2-16*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q1Dq2**2*pbDp1b*mb**2+3
     #2*q1Dq2**2*pbDp1b**2+64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-16*q1Dpb*q1Dp1b*
     #q2Dpb*mb**2-64*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-80*q1Dpb*q1Dp1b*q2Dp1b*m
     #b**2-16*q1Dpb**2*q2Dp1b*mb**2+32*q1Dpb**2*q2Dp1b**2+16*q1Dp1b**2*q
     #2Dpb*mb**2+32*q1Dp1b**2*q2Dpb**2+32*q1Dp1b**2*q2Dp1b*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-32
     #*q1Dq2*q1Dp1b*pbDp1b**2+16*q1Dq2*q1Dp1b*mb**4+16*q1Dpb*q1Dp1b*q2Dp
     #b*mb**2+32*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+48*q1Dpb*q1Dp1b*q2Dp1b*mb**2
     #-64*q1Dpb*q1Dp1b**2*q2Dpb-64*q1Dpb**2*q1Dp1b*q2Dp1b+16*q1Dpb**2*q2
     #Dp1b*mb**2+32*q1Dp1b**2*q2Dpb*pbDp1b+16*q1Dp1b**2*q2Dpb*mb**2-32*q
     #1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(26) = t0
c
c ----------m2_sme_v(27) ---------
c
      s1 = CV/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb**2-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*q2Dpb*mb**2-64*
     #q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dp1b*q2Dp1b*mb**2+48*q1Dq2*q
     #1Dp1b*pbDp1b*mb**2+32*q1Dq2*q1Dp1b*pbDp1b**2+16*q1Dq2*q1Dp1b*mb**4
     #+32*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*mb**4+16*q1Dpb*q1Dp1b*q2Dpb*
     #mb**2-32*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-16*q1Dpb*q1Dp1b*q2Dp1b*mb**2+6
     #4*q1Dpb*q1Dp1b*q2Dp1b**2+16*q1Dpb**2*q2Dp1b*mb**2+64*q1Dp1b**2*q2D
     #pb*q2Dp1b-32*q1Dp1b**2*q2Dpb*pbDp1b-48*q1Dp1b**2*q2Dpb*mb**2-32*q1
     #Dp1b**2*q2Dp1b*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*mb**2-16*q1Dq2*q1Dpb*
     #pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*
     #q1Dq2*q1Dp1b*mb**4-16*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2D
     #p1b*pbDp1b+48*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb*q1Dp1b**2*q2Dp1b
     #-16*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp1b**2*q2Dpb*mb**2+32*q1Dp1b**2*q2
     #Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(27) = t0
c
c ----------m2_sme_v(28) ---------
c
      t0 = CV/den0x2/den0x3*(-32*q1Dq2*q1Dpb*q2Dpb*mb**2+32*q1Dq2*q1Dpb*
     #q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pb
     #Dp1b+32*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+64*
     #q1Dq2*q1Dp1b*q2Dp1b*mb**2-64*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*pbD
     #p1b**2-32*q1Dq2**2*mb**4+32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1D
     #p1b*q2Dp1b**2+32*q1Dp1b**2*q2Dpb*q2Dp1b-32*q1Dp1b**2*q2Dpb**2)
c
      m2_sme_v(28) = t0
c
c ----------m2_sme_v(29) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q2Dp
     #b*q2Dp1b*mb**2+32*q1Dq2*q2Dpb**2*mb**2+32*q1Dq2*q2Dp1b**2*pbDp1b+3
     #2*q1Dpb*q2Dpb*q2Dp1b**2-32*q1Dpb*q2Dp1b**3+32*q1Dp1b*q2Dpb*q2Dp1b*
     #*2-32*q1Dp1b*q2Dpb**2*q2Dp1b)
c
      m2_sme_v(29) = t0
c
c ----------m2_sme_v(30) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+16*q1Dq2*q2Dp
     #b*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+3
     #2*q1Dq2*q2Dp1b*pbDp1b**2-16*q1Dq2*q2Dp1b*mb**4-16*q1Dpb*q2Dpb*q2Dp
     #1b*mb**2+64*q1Dpb*q2Dpb*q2Dp1b**2-32*q1Dpb*q2Dp1b**2*pbDp1b-16*q1D
     #pb*q2Dp1b**2*mb**2-32*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-48*q1Dp1b*q2Dpb*q
     #2Dp1b*mb**2+64*q1Dp1b*q2Dpb**2*q2Dp1b-16*q1Dp1b*q2Dpb**2*mb**2+32*
     #q1Dp1b*q2Dp1b**2*mb**2)
      s2 = CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+32*q1Dq2*q1Dp1b*q2Dpb*m
     #b**2-48*q1Dq2*q2Dpb*pbDp1b*mb**2-48*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2D
     #p1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1Dq2**2*pbDp1b*mb**2-32
     #*q1Dq2**2*pbDp1b**2-64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+16*q1Dpb*q2Dpb*q2
     #Dp1b*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-32*q1Dpb**2*q2Dp1b**2+64*q1Dp1
     #b*q2Dpb*q2Dp1b*pbDp1b+80*q1Dp1b*q2Dpb*q2Dp1b*mb**2+16*q1Dp1b*q2Dpb
     #**2*mb**2-32*q1Dp1b*q2Dp1b**2*mb**2-32*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_v(30) = t0
c
c ----------m2_sme_v(31) ---------
c
      s1 = CV/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*mb**2+16*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16
     #*q1Dq2*q2Dp1b*mb**4+16*q1Dpb*q2Dpb*q2Dp1b*mb**2-16*q1Dpb*q2Dp1b**2
     #*mb**2-64*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-48*q1Dp1b*q2Dpb*q2Dp1b*mb**2+
     #128*q1Dp1b*q2Dpb*q2Dp1b**2+16*q1Dp1b*q2Dpb**2*mb**2-32*q1Dp1b*q2Dp
     #1b**2*mb**2)
      s2 = CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dp1b*mb**2+32*q1Dq2*q1Dp1b
     #*q2Dpb*mb**2+64*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dp1b*q2Dp1b*
     #mb**2+16*q1Dq2*q2Dpb*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4-48*q1Dq2*q2
     #Dp1b*pbDp1b*mb**2-32*q1Dq2*q2Dp1b*pbDp1b**2-16*q1Dq2*q2Dp1b*mb**4-
     #32*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*mb**4-64*q1Dpb*q1Dp1b*q2Dp1b*
     #*2-16*q1Dpb*q2Dpb*q2Dp1b*mb**2+32*q1Dpb*q2Dp1b**2*pbDp1b+48*q1Dpb*
     #q2Dp1b**2*mb**2+32*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+16*q1Dp1b*q2Dpb*q2Dp
     #1b*mb**2-16*q1Dp1b*q2Dpb**2*mb**2+32*q1Dp1b*q2Dp1b**2*mb**2-64*q1D
     #p1b**2*q2Dpb*q2Dp1b)
      t0 = s1+s2
c
      m2_sme_v(31) = t0

c
c ----------m2_sme_v(32) ---------
c
      t0 = CV/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb-32*q1Dq2*q2Dpb*mb*
     #*3-32*q1Dq2*q2Dp1b*pbDp1b*mb-96*q1Dq2*q2Dp1b*mb**3+160*q1Dpb*q2Dpb
     #*q2Dp1b*mb+64*q1Dpb*q2Dpb**2*mb+32*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2
     #Dpb*q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb)+CV/den0x2/den0x3*(64*q1Dq2*q1
     #Dpb*q2Dpb*mb+32*q1Dq2*q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb+32*
     #q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb**3+32*q1Dq2*q2Dp1b*pbDp1b*
     #mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3-9
     #6*q1Dpb*q2Dpb*q2Dp1b*mb-64*q1Dpb*q2Dpb**2*mb-32*q1Dpb*q2Dp1b**2*mb
     #-32*q1Dp1b*q2Dpb*q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb)
c
      m2_sme_v(32) = t0
c
c ----------m2_sme_v(33) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb
     #**3-96*q1Dq2*q2Dp1b*pbDp1b*mb-32*q1Dq2*q2Dp1b*mb**3+32*q1Dpb*q2Dpb
     #*q2Dp1b*mb+96*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2Dpb*q2Dp1b*mb+32*q1Dp
     #1b*q2Dpb**2*mb+64*q1Dp1b*q2Dp1b**2*mb)+CV/den0x2/den0x3*(32*q1Dq2*
     #q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb+64*q1Dq2*q1Dp1b*q2Dp1b*mb
     #+32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb**3+32*q1Dq2*q2Dp1b*pbD
     #p1b*mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb*
     #*3-32*q1Dpb*q2Dpb*q2Dp1b*mb-32*q1Dpb*q2Dp1b**2*mb-96*q1Dp1b*q2Dpb*
     #q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb-64*q1Dp1b*q2Dp1b**2*mb)
c
      m2_sme_v(33) = t0
c
c ----------m2_sme_v(34) ---------
c
      t0 = CV/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb*
     #*3+32*q1Dq2*q2Dp1b*pbDp1b*mb+32*q1Dq2*q2Dp1b*mb**3+32*q1Dpb*q2Dpb*
     #q2Dp1b*mb+32*q1Dpb*q2Dp1b**2*mb-32*q1Dp1b*q2Dpb*q2Dp1b*mb-32*q1Dp1
     #b*q2Dpb**2*mb)+CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dpb*mb+32*q1Dq2*
     #q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb-64*q1Dq2*q2Dpb*pbDp1b*mb-
     #64*q1Dq2*q2Dpb*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3)
c
      m2_sme_v(34) = t0
c
c ----------m2_sme_v(35) ---------
c
      t0 = CV/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb*
     #*3+32*q1Dq2*q2Dp1b*pbDp1b*mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dpb*q2Dpb*
     #q2Dp1b*mb-32*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2Dpb*q2Dp1b*mb+32*q1Dp1
     #b*q2Dpb**2*mb)+CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dp1b*mb+32*q1Dq2
     #*q1Dp1b*q2Dpb*mb+64*q1Dq2*q1Dp1b*q2Dp1b*mb-64*q1Dq2*q2Dp1b*pbDp1b*
     #mb-64*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3)
c
      m2_sme_v(35) = t0
c
c ----------m2_sme_v(36) ---------
c
      t0 = CV/den0x1/den0x3*(64*q1Dq2*q2Dpb*mb-64*q1Dq2*q2Dp1b*mb-128*q1
     #Dq2*pbDp1b*mb-128*q1Dq2*mb**3+64*q1Dpb*q2Dpb*mb+128*q1Dpb*q2Dp1b*m
     #b+64*q1Dp1b*q2Dp1b*mb)+CV/den0x2/den0x3*(-64*q1Dq2*q1Dpb*mb+64*q1D
     #q2*q1Dp1b*mb+128*q1Dq2*pbDp1b*mb+128*q1Dq2*mb**3-64*q1Dpb*q2Dpb*mb
     #-128*q1Dp1b*q2Dpb*mb-64*q1Dp1b*q2Dp1b*mb)
c
      m2_sme_v(36) = t0
c
c ----------m2_sme_v(37) ---------
c
      t0 = CV/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b+64*q1Dq2*q2Dpb*mb**
     #2+128*q1Dq2*q2Dp1b*pbDp1b+192*q1Dq2*q2Dp1b*mb**2)+CV/den0x2/den0x3
     #*(64*q1Dq2*q1Dpb*q2Dp1b+64*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2*q2Dpb*pbDp1
     #b+64*q1Dq2*q2Dpb*mb**2-64*q1Dq2**2*pbDp1b-64*q1Dq2**2*mb**2+64*q1D
     #pb*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb**2)
c
      m2_sme_v(37) = t0
c
c ----------m2_sme_v(38) ---------
c
      s1 = CV/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q2D
     #pb*pbDp1b*mb**2+32*q1Dq2*q2Dpb*mb**4+32*q1Dq2*q2Dp1b*pbDp1b*mb**2+
     #64*q1Dq2*q2Dp1b*pbDp1b**2-32*q1Dq2*q2Dp1b*mb**4-32*q1Dpb*q2Dpb*q2D
     #p1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b**2-64*q1Dpb*q2Dp1b**2*pbDp1b-32*q
     #1Dpb*q2Dp1b**2*mb**2+64*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+32*q1Dp1b*q2Dpb
     #*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*q2Dp1b+32*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+64*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2-64*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1Dq2*q2Dpb*pbDp1b*m
     #b**2+64*q1Dq2*q2Dpb*pbDp1b**2-32*q1Dq2*q2Dpb*mb**4+32*q1Dq2*q2Dp1b
     #*pbDp1b*mb**2+32*q1Dq2*q2Dp1b*mb**4-64*q1Dq2**2*pbDp1b*mb**2-64*q1
     #Dq2**2*pbDp1b**2-64*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-96*q1Dpb*q2Dpb*q2Dp1
     #b*mb**2+64*q1Dpb*q2Dpb**2*mb**2-32*q1Dpb*q2Dp1b**2*mb**2-64*q1Dpb*
     #*2*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+96*q1Dp1b*q2Dpb*q2Dp1b
     #*mb**2-64*q1Dp1b*q2Dpb**2*pbDp1b+32*q1Dp1b*q2Dpb**2*mb**2-64*q1Dp1
     #b*q2Dp1b**2*mb**2+64*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_v(38) = t0
c
c ----------m2_sme_v(39) ---------
c
      s1 = CV/den0x1/den0x3*(64*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*pb
     #Dp1b*mb-128*q1Dq2*pbDp1b*mb**3-192*q1Dq2*pbDp1b**2*mb+64*q1Dq2*mb*
     #*5-64*q1Dpb*q2Dpb*q2Dp1b*mb-64*q1Dpb*q2Dpb*mb**3+192*q1Dpb*q2Dp1b*
     #pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb-64*q1Dp1b*q2
     #Dpb*q2Dp1b*mb-64*q1Dp1b*q2Dpb*pbDp1b*mb-64*q1Dp1b*q2Dpb*mb**3+64*q
     #1Dp1b*q2Dpb**2*mb+128*q1Dp1b*q2Dp1b*pbDp1b*mb+64*q1Dp1b*q2Dp1b*mb*
     #*3)
      s2 = CV/den0x2/den0x3*(-128*q1Dq2*q1Dpb*pbDp1b*mb-64*q1Dq2*q1Dpb*m
     #b**3+128*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1Dq2*q1Dp1b*mb**3+128*q1Dq2*pb
     #Dp1b*mb**3+192*q1Dq2*pbDp1b**2*mb-64*q1Dq2*mb**5-128*q1Dpb*q1Dp1b*
     #q2Dpb*mb+192*q1Dpb*q2Dpb*mb**3-192*q1Dpb*q2Dp1b*pbDp1b*mb-64*q1Dpb
     #*q2Dp1b*mb**3+128*q1Dpb**2*q2Dp1b*mb-192*q1Dp1b*q2Dpb*pbDp1b*mb+64
     #*q1Dp1b*q2Dpb*mb**3-128*q1Dp1b*q2Dp1b*pbDp1b*mb+64*q1Dp1b*q2Dp1b*m
     #b**3)
      t0 = s1+s2
c
      m2_sme_v(39) = t0
c
c ----------m2_sme_v(40) ---------
c
      s1 = CV/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+64*q1Dq2*q2D
     #pb*pbDp1b*mb**2+192*q1Dq2*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbD
     #p1b**2-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b**2-128*q
     #1Dpb*q2Dp1b**2*pbDp1b-192*q1Dpb*q2Dp1b**2*mb**2+128*q1Dp1b*q2Dpb*q
     #2Dp1b*pbDp1b+192*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*q2D
     #p1b+64*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dpb
     #*pbDp1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+
     #64*q1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b**2-64*q1Dq2*q2D
     #pb*mb**4-64*q1Dq2**2*pbDp1b*mb**2-128*q1Dq2**2*pbDp1b**2+64*q1Dq2*
     #*2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b*pbDp
     #1b-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb**2*mb**2-128*q1Dpb*
     #*2*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*pbD
     #p1b+64*q1Dp1b*q2Dpb**2*mb**2)
      t0 = s1+s2
c
      m2_sme_v(40) = t0
c
c ----------m2_sme_v(41) ---------
c
      t0 = CV/den0x1/den0x3*(-256*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*mb
     #**2+576*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+320*q1Dq2*mb**4-256
     #*q1Dpb*q2Dpb*pbDp1b-384*q1Dpb*q2Dpb*mb**2+256*q1Dpb*q2Dp1b*pbDp1b+
     #320*q1Dpb*q2Dp1b*mb**2-256*q1Dp1b*q2Dpb*pbDp1b-448*q1Dp1b*q2Dpb*mb
     #**2+256*q1Dp1b*q2Dpb**2)+CV/den0x2/den0x3*(-128*q1Dq2*q1Dpb*pbDp1b
     #-256*q1Dq2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*mb**2-320*q1Dq2*pbDp1b*mb*
     #*2-128*q1Dq2*pbDp1b**2-192*q1Dq2*mb**4-384*q1Dpb*q1Dp1b*q2Dpb+512*
     #q1Dpb*q2Dpb*pbDp1b+640*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+3
     #20*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+
     #64*q1Dp1b*q2Dpb*mb**2)
c
      m2_sme_v(41) = t0
c
c ----------m2_sme_v(42) ---------
c
      t0 = CV/den0x1/den0x3*(192*q1Dq2*q2Dpb*mb**3-64*q1Dq2*q2Dp1b*mb**3
     #+64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*mb**5+128*q1Dpb*q2Dpb*q2Dp1b*mb+38
     #4*q1Dpb*q2Dpb*pbDp1b*mb-384*q1Dpb*q2Dpb**2*mb+256*q1Dpb*q2Dp1b*pbD
     #p1b*mb-256*q1Dp1b*q2Dpb*mb**3-128*q1Dp1b*q2Dp1b*mb**3)+CV/den0x2/d
     #en0x3*(-128*q1Dq2*q1Dpb*pbDp1b*mb-192*q1Dq2*q1Dpb*mb**3+192*q1Dq2*
     #q1Dp1b*mb**3-64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*mb**5-256*q1Dpb*q1Dp1b
     #*q2Dpb*mb-384*q1Dpb*q2Dpb*pbDp1b*mb-128*q1Dpb*q2Dpb*mb**3+128*q1Dp
     #b*q2Dp1b*mb**3+384*q1Dpb**2*q2Dpb*mb+128*q1Dpb**2*q2Dp1b*mb+128*q1
     #Dp1b*q2Dpb*mb**3)
c
      m2_sme_v(42) = t0
c
c ----------m2_sme_v(43) ---------
c
      t0 = CV/den0x1/den0x3*(192*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*p
     #bDp1b*mb+64*q1Dq2*pbDp1b*mb**3+192*q1Dq2*pbDp1b**2*mb-128*q1Dq2*mb
     #**5-192*q1Dpb*q2Dpb*q2Dp1b*mb+384*q1Dpb*q2Dpb*mb**3-192*q1Dpb*q2Dp
     #1b*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b
     #*q2Dpb*q2Dp1b*mb-192*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**3-
     #192*q1Dp1b*q2Dpb**2*mb+128*q1Dp1b*q2Dp1b*mb**3)+CV/den0x2/den0x3*(
     #-192*q1Dq2*q1Dpb*pbDp1b*mb-128*q1Dq2*q1Dpb*mb**3+192*q1Dq2*q1Dp1b*
     #pbDp1b*mb-64*q1Dq2*pbDp1b*mb**3-192*q1Dq2*pbDp1b**2*mb+128*q1Dq2*m
     #b**5+192*q1Dpb*q1Dp1b*q2Dpb*mb+64*q1Dpb*q1Dp1b*q2Dp1b*mb-256*q1Dpb
     #*q2Dpb*mb**3-64*q1Dpb*q2Dp1b*pbDp1b*mb-192*q1Dpb*q2Dp1b*mb**3+192*
     #q1Dpb**2*q2Dp1b*mb+192*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**
     #3-192*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_v(43) = t0
c
c ----------m2_sme_v(44) ---------
c
      s1 = CV/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2D
     #pb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**2
     #+64*q1Dq2*q2Dp1b*mb**4-256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q2D
     #pb*q2Dp1b*mb**2+768*q1Dpb*q2Dpb**2*q2Dp1b-256*q1Dpb*q2Dpb**2*mb**2
     #+64*q1Dpb*q2Dp1b**2*mb**2+64*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*
     #q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(384*q1Dq2*q1Dpb*q2Dpb*pbDp1b+384*q1Dq2*q1Dp
     #b*q2Dpb*mb**2+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+192*q1Dq2*q1Dp1b*q2Dpb*
     #mb**2-320*q1Dq2*q2Dpb*pbDp1b*mb**2-256*q1Dq2*q2Dpb*pbDp1b**2-64*q1
     #Dq2*q2Dpb*mb**4+128*q1Dq2*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*mb*
     #*4-192*q1Dq2**2*pbDp1b*mb**2-192*q1Dq2**2*mb**4-384*q1Dpb*q1Dp1b*q
     #2Dpb**2+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+64*q1Dpb*q2Dpb*q2Dp1b*mb**2+
     #128*q1Dpb*q2Dpb**2*mb**2-128*q1Dpb*q2Dp1b**2*mb**2-384*q1Dpb**2*q2
     #Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb**2*pbDp1
     #b+320*q1Dp1b*q2Dpb**2*mb**2)
      t0 = s1+s2
c
      m2_sme_v(44) = t0
c
c ----------m2_sme_v(45) ---------
c
      s1 = CV/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+256*q1Dq2*q2Dpb*pbDp1b**2-128*q1Dq2*q2Dpb*mb**4+6
     #4*q1Dq2*q2Dp1b*pbDp1b*mb**2+64*q1Dq2*q2Dp1b*mb**4-256*q1Dpb*q2Dpb*
     #q2Dp1b*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2+384*q1Dpb*q2Dpb*q2Dp1b*
     #*2+256*q1Dpb*q2Dpb**2*mb**2-64*q1Dpb*q2Dp1b**2*mb**2-64*q1Dp1b*q2D
     #pb*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb**2*q2Dp1b-256*q1Dp1b*q2Dpb**2*pbD
     #p1b-128*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2+512*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+512*q1Dq2*q1Dp1b*q2Dp
     #b*pbDp1b+192*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**
     #2+64*q1Dq2*q2Dpb*pbDp1b*mb**2+64*q1Dq2*q2Dpb*mb**4-256*q1Dq2*q2Dp1
     #b*pbDp1b*mb**2-256*q1Dq2*q2Dp1b*mb**4-192*q1Dq2**2*pbDp1b*mb**2-25
     #6*q1Dq2**2*pbDp1b**2+64*q1Dq2**2*mb**4-256*q1Dpb*q1Dp1b*q2Dpb*q2Dp
     #1b+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+448*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*
     #q1Dpb*q2Dpb**2*mb**2+128*q1Dpb*q2Dp1b**2*mb**2-256*q1Dpb**2*q2Dp1b
     #**2+128*q1Dp1b*q2Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb**2*mb**2-256*q1D
     #p1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_v(45) = t0
c
c ----------m2_sme_v(46) ---------
c
      t0 = CV/den0x1/den0x3*(-256*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbD
     #p1b+576*q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2+448*q1Dq2*mb**4-128
     #*q1Dpb*q2Dp1b*pbDp1b-320*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+12
     #8*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-64*q1Dp1b*q2Dpb*mb**
     #2-128*q1Dp1b*q2Dp1b*mb**2)+CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*mb**2
     #-256*q1Dq2*q1Dp1b*mb**2-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4-256
     #*q1Dpb*q1Dp1b*q2Dp1b+256*q1Dpb*q2Dpb*mb**2+192*q1Dpb*q2Dp1b*mb**2+
     #448*q1Dp1b*q2Dpb*mb**2+256*q1Dp1b*q2Dp1b*pbDp1b+640*q1Dp1b*q2Dp1b*
     #mb**2)
c
      m2_sme_v(46) = t0
c
c ----------m2_sme_v(47) ---------
c
      t0 = CV/den0x1/den0x3*(192*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*p
     #bDp1b*mb+64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*pbDp1b**2*mb+128*q1Dq2*mb*
     #*5-192*q1Dpb*q2Dpb*q2Dp1b*mb+128*q1Dpb*q2Dpb*mb**3+64*q1Dpb*q2Dp1b
     #*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b*q
     #2Dpb*q2Dp1b*mb+64*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**3-192
     #*q1Dp1b*q2Dpb**2*mb-128*q1Dp1b*q2Dp1b*mb**3)+CV/den0x2/den0x3*(-64
     #*q1Dq2*q1Dpb*pbDp1b*mb+64*q1Dq2*q1Dp1b*pbDp1b*mb-128*q1Dq2*q1Dp1b*
     #mb**3-64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*pbDp1b**2*mb-128*q1Dq2*mb**5+
     #320*q1Dpb*q1Dp1b*q2Dpb*mb-64*q1Dpb*q1Dp1b*q2Dp1b*mb-64*q1Dpb*q2Dp1
     #b*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb**2*q2Dp1b*mb-320*q1Dp1b
     #*q2Dpb*pbDp1b*mb-192*q1Dp1b*q2Dpb*mb**3+256*q1Dp1b*q2Dp1b*mb**3-64
     #*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_v(47) = t0
c
c ----------m2_sme_v(48) ---------
c
      t0 = CV/den0x1/den0x3*(192*q1Dq2*q2Dpb*mb**3-64*q1Dq2*q2Dp1b*mb**3
     #+64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*mb**5-128*q1Dpb*q2Dpb*mb**3-384*q1
     #Dp1b*q2Dpb*q2Dp1b*mb+256*q1Dp1b*q2Dpb*pbDp1b*mb+256*q1Dp1b*q2Dpb*m
     #b**3-128*q1Dp1b*q2Dp1b*pbDp1b*mb+128*q1Dp1b*q2Dp1b**2*mb)+CV/den0x
     #2/den0x3*(-64*q1Dq2*q1Dpb*mb**3-128*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1Dq
     #2*q1Dp1b*mb**3-64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*mb**5+256*q1Dpb*q1Dp
     #1b*q2Dp1b*mb-128*q1Dpb*q2Dp1b*mb**3-128*q1Dp1b*q2Dpb*mb**3+128*q1D
     #p1b*q2Dp1b*pbDp1b*mb-128*q1Dp1b*q2Dp1b*mb**3+128*q1Dp1b**2*q2Dpb*m
     #b-128*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_v(48) = t0
c
c ----------m2_sme_v(49) ---------
c
      s1 = CV/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**
     #2+128*q1Dq2*q2Dp1b*pbDp1b**2-64*q1Dq2*q2Dp1b*mb**4-128*q1Dpb*q2Dpb
     #*q2Dp1b*mb**2+384*q1Dpb*q2Dpb*q2Dp1b**2-128*q1Dpb*q2Dp1b**2*pbDp1b
     #-64*q1Dpb*q2Dp1b**2*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-320*q1Dp1
     #b*q2Dpb*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb**
     #2*mb**2+128*q1Dp1b*q2Dp1b**2*mb**2)
      s2 = CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q1Dpb
     #*q2Dp1b*pbDp1b+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb
     #*pbDp1b+192*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2
     #-320*q1Dq2*q2Dpb*pbDp1b*mb**2-320*q1Dq2*q2Dpb*mb**4+128*q1Dq2*q2Dp
     #1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*mb**4-192*q1Dq2**2*pbDp1b*mb**2-1
     #28*q1Dq2**2*pbDp1b**2-64*q1Dq2**2*mb**4-512*q1Dpb*q1Dp1b*q2Dpb*q2D
     #p1b+64*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2*mb**2-128*q1Dp
     #b**2*q2Dp1b**2+512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+512*q1Dp1b*q2Dpb*q2D
     #p1b*mb**2+64*q1Dp1b*q2Dpb**2*mb**2-256*q1Dp1b*q2Dp1b**2*mb**2-128*
     #q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_v(49) = t0
c
c ----------m2_sme_v(50) ---------
c
      s1 = CV/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2D
     #pb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**2
     #+64*q1Dq2*q2Dp1b*mb**4+128*q1Dpb*q2Dpb*q2Dp1b*mb**2-64*q1Dpb*q2Dp1
     #b**2*mb**2-512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-320*q1Dp1b*q2Dpb*q2Dp1b*
     #mb**2+768*q1Dp1b*q2Dpb*q2Dp1b**2+128*q1Dp1b*q2Dpb**2*mb**2-128*q1D
     #p1b*q2Dp1b**2*mb**2)
      s2 = CV/den0x2/den0x3*(192*q1Dq2*q1Dpb*q2Dp1b*mb**2+192*q1Dq2*q1Dp
     #1b*q2Dpb*mb**2+384*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*q2D
     #p1b*mb**2+64*q1Dq2*q2Dpb*pbDp1b*mb**2+64*q1Dq2*q2Dpb*mb**4-256*q1D
     #q2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b**2-128*q1Dq2*q2Dp1b
     #*mb**4-192*q1Dq2**2*pbDp1b*mb**2-192*q1Dq2**2*mb**4-384*q1Dpb*q1Dp
     #1b*q2Dp1b**2-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2*pbDp1
     #b+256*q1Dpb*q2Dp1b**2*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+128*q1D
     #p1b*q2Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb**2*mb**2+256*q1Dp1b*q2Dp1b*
     #*2*mb**2-384*q1Dp1b**2*q2Dpb*q2Dp1b)
      t0 = s1+s2
c
      m2_sme_v(50) = t0
c
c ----------m2_sme_v(51) ---------
c
      t0 = CV/den0x1/den0x3*(64*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dpb*mb*
     #*3-64*q1Dq2*q2Dp1b*pbDp1b*mb-192*q1Dq2*q2Dp1b*mb**3+64*q1Dpb*q2Dpb
     #*q2Dp1b*mb-64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q1Dp
     #1b*q2Dpb**2*mb)+CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb+64*q1Dq
     #2*q1Dpb*q2Dp1b*mb+64*q1Dq2*q1Dp1b*q2Dpb*mb+64*q1Dq2*q2Dpb*pbDp1b*m
     #b+64*q1Dq2*q2Dpb*mb**3+64*q1Dq2*q2Dp1b*pbDp1b*mb+64*q1Dq2*q2Dp1b*m
     #b**3-64*q1Dq2**2*pbDp1b*mb-64*q1Dq2**2*mb**3-192*q1Dpb*q2Dpb*q2Dp1
     #b*mb-128*q1Dpb*q2Dpb**2*mb-64*q1Dpb*q2Dp1b**2*mb-64*q1Dp1b*q2Dpb*q
     #2Dp1b*mb-64*q1Dp1b*q2Dpb**2*mb)
c
      m2_sme_v(51) = t0
c
c ----------m2_sme_v(52) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q2Dpb*pbDp1b*mb+64*q1Dq2*q2Dpb*mb
     #**3-192*q1Dq2*q2Dp1b*pbDp1b*mb-64*q1Dq2*q2Dp1b*mb**3+64*q1Dpb*q2Dp
     #b*q2Dp1b*mb+192*q1Dpb*q2Dp1b**2*mb-192*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q
     #1Dp1b*q2Dpb**2*mb)+CV/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*mb+64*q
     #1Dq2*q1Dp1b*q2Dpb*mb+128*q1Dq2*q1Dp1b*q2Dp1b*mb+64*q1Dq2*q2Dpb*pbD
     #p1b*mb+64*q1Dq2*q2Dpb*mb**3+64*q1Dq2*q2Dp1b*pbDp1b*mb+64*q1Dq2*q2D
     #p1b*mb**3-64*q1Dq2**2*pbDp1b*mb-64*q1Dq2**2*mb**3-64*q1Dpb*q2Dpb*q
     #2Dp1b*mb-64*q1Dpb*q2Dp1b**2*mb-192*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q1Dp1
     #b*q2Dpb**2*mb-128*q1Dp1b*q2Dp1b**2*mb)
c
      m2_sme_v(52) = t0
c
c ----------m2_sme_v(53) ---------
c
      t0 = CV/den0x1/den0x3*(384*q1Dq2*q2Dpb*mb-128*q1Dq2*q2Dp1b*mb-384*
     #q1Dq2*pbDp1b*mb-384*q1Dq2*mb**3+256*q1Dpb*q2Dp1b*mb-256*q1Dp1b*q2D
     #pb*mb)+CV/den0x2/den0x3*(-128*q1Dq2*q1Dpb*mb+384*q1Dq2*q1Dp1b*mb+1
     #28*q1Dq2*pbDp1b*mb+128*q1Dq2*mb**3-256*q1Dpb*q2Dpb*mb-512*q1Dp1b*q
     #2Dpb*mb-256*q1Dp1b*q2Dp1b*mb)
c
      m2_sme_v(53) = t0
c
c ----------m2_sme_v(54) ---------
c
      t0 = CV/den0x1/den0x3*(-768*q1Dq2*q2Dpb*q2Dp1b+128*q1Dq2*q2Dpb*pbD
     #p1b+384*q1Dq2*q2Dpb*mb**2+512*q1Dq2*q2Dp1b*pbDp1b+768*q1Dq2*q2Dp1b
     #*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb**2)+CV/den0x2/den0x
     #3*(384*q1Dq2*q1Dpb*q2Dp1b+384*q1Dq2*q1Dp1b*q2Dpb+256*q1Dq2*q2Dpb*p
     #bDp1b+256*q1Dq2*q2Dpb*mb**2+128*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp
     #1b*mb**2-384*q1Dq2**2*pbDp1b-384*q1Dq2**2*mb**2+256*q1Dpb*q2Dpb*q2
     #Dp1b-128*q1Dpb*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dp1b*q2Dpb*
     #*2)
c
      m2_sme_v(54) = t0
c
c ----------m2_sme_v(55) ---------
c
      t0 = CV/den0x1/den0x3*(-512*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*mb
     #**2-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4+768*q1Dpb*q2Dpb*q2Dp1b-
     #512*q1Dpb*q2Dpb*mb**2-256*q1Dpb*q2Dp1b*pbDp1b-448*q1Dpb*q2Dp1b*mb*
     #*2-192*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*mb
     #**2)+CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*pbDp1b+512*q1Dq2*q1Dp1b*mb*
     #*2+320*q1Dq2*pbDp1b*mb**2+320*q1Dq2*mb**4-1152*q1Dpb*q1Dp1b*q2Dpb+
     #128*q1Dpb*q2Dpb*mb**2-192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b+1
     #024*q1Dp1b*q2Dpb*pbDp1b+832*q1Dp1b*q2Dpb*mb**2-512*q1Dp1b*q2Dp1b*m
     #b**2)
c
      m2_sme_v(55) = t0
c
c ----------m2_sme_v(56) ---------
c
      t0 = CV/den0x1/den0x3*(-512*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbD
     #p1b-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4+512*q1Dpb*q2Dpb*mb**2+1
     #92*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+1152*q1Dp1b*q2Dpb*q2Dp1b
     #-1024*q1Dp1b*q2Dpb*pbDp1b-832*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b
     #*mb**2)+CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*mb**2+512*q1Dq2*q1Dp1b*p
     #bDp1b+320*q1Dq2*pbDp1b*mb**2+320*q1Dq2*mb**4-768*q1Dpb*q1Dp1b*q2Dp
     #1b-128*q1Dpb*q2Dpb*mb**2+256*q1Dpb*q2Dp1b*pbDp1b+448*q1Dpb*q2Dp1b*
     #mb**2+192*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dp1b*mb**2-512*q1Dp1b**2
     #*q2Dpb)
c
      m2_sme_v(56) = t0
c
c ----------m2_sme_v(57) ---------
c
      s1 = CV/den0x1/den0x3*(384*q1Dq2*q2Dpb*pbDp1b*mb-128*q1Dq2*q2Dp1b*
     #pbDp1b*mb-384*q1Dq2*pbDp1b*mb**3-640*q1Dq2*pbDp1b**2*mb+256*q1Dq2*
     #mb**5-384*q1Dpb*q2Dpb*q2Dp1b*mb+640*q1Dpb*q2Dp1b*pbDp1b*mb+384*q1D
     #pb*q2Dp1b*mb**3+128*q1Dpb*q2Dp1b**2*mb-128*q1Dp1b*q2Dpb*q2Dp1b*mb-
     #640*q1Dp1b*q2Dpb*pbDp1b*mb-384*q1Dp1b*q2Dpb*mb**3+384*q1Dp1b*q2Dpb
     #**2*mb)
      s2 = CV/den0x2/den0x3*(-384*q1Dq2*q1Dpb*pbDp1b*mb-256*q1Dq2*q1Dpb*
     #mb**3+640*q1Dq2*q1Dp1b*pbDp1b*mb+256*q1Dq2*q1Dp1b*mb**3+128*q1Dq2*
     #pbDp1b*mb**3+384*q1Dq2*pbDp1b**2*mb-256*q1Dq2*mb**5-384*q1Dpb*q1Dp
     #1b*q2Dpb*mb-128*q1Dpb*q1Dp1b*q2Dp1b*mb+768*q1Dpb*q2Dpb*mb**3-384*q
     #1Dpb*q2Dp1b*pbDp1b*mb+128*q1Dpb*q2Dp1b*mb**3+384*q1Dpb**2*q2Dp1b*m
     #b-1152*q1Dp1b*q2Dpb*pbDp1b*mb-128*q1Dp1b*q2Dpb*mb**3-512*q1Dp1b*q2
     #Dp1b*pbDp1b*mb+256*q1Dp1b*q2Dp1b*mb**3+128*q1Dp1b**2*q2Dpb*mb)
      t0 = s1+s2
c
      m2_sme_v(57) = t0
c
c ----------m2_sme_v(58) ---------
c
      s1 = CV/den0x1/den0x3*(-768*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+768*q1Dq2*q2Dp1b*pbDp1b*mb*
     #*2+512*q1Dq2*q2Dp1b*pbDp1b**2-384*q1Dpb*q2Dpb*q2Dp1b*mb**2+768*q1D
     #pb*q2Dpb*q2Dp1b**2-512*q1Dpb*q2Dp1b**2*pbDp1b-768*q1Dpb*q2Dp1b**2*
     #mb**2+512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+768*q1Dp1b*q2Dpb*q2Dp1b*mb**2
     #-768*q1Dp1b*q2Dpb**2*q2Dp1b+384*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CV/den0x2/den0x3*(-512*q1Dpb**2*q2Dp1b**2+128*q1Dq2**2*mb**4-
     #512*q1Dq2**2*pbDp1b**2-128*q1Dpb*q2Dp1b**2*mb**2+256*q1Dp1b*q2Dpb*
     #*2*mb**2+128*q1Dq2*q2Dp1b*mb**4-384*q1Dq2**2*pbDp1b*mb**2-512*q1Dp
     #1b*q2Dpb**2*pbDp1b+512*q1Dq2*q2Dpb*pbDp1b**2-512*q1Dpb*q2Dpb*q2Dp1
     #b*pbDp1b-256*q1Dq2*q1Dpb*q2Dpb*mb**2+384*q1Dq2*q1Dpb*q2Dp1b*mb**2-
     #384*q1Dq2*q1Dp1b*q2Dpb*mb**2+1024*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+256*q1
     #Dq2*q1Dp1b*q2Dpb*pbDp1b-256*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q1
     #Dp1b*q2Dpb*q2Dp1b+512*q1Dpb*q2Dpb**2*mb**2-256*q1Dp1b*q2Dp1b**2*mb
     #**2+256*q1Dp1b**2*q2Dpb**2-256*q1Dq2*q2Dpb*mb**4+256*q1Dq2*q2Dpb*p
     #bDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2-512*q1Dpb*q2Dpb*q2Dp1b*m
     #b**2+512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+640*q1Dp1b*q2Dpb*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(58) = t0
c
c ----------m2_sme_v(59) ---------
c
      t0 = CV/den0x1/den0x3*(-1024*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dp1b*m
     #b**2+1920*q1Dq2*pbDp1b*mb**2+1024*q1Dq2*pbDp1b**2+896*q1Dq2*mb**4-
     #1024*q1Dpb*q2Dpb*q2Dp1b+1024*q1Dpb*q2Dp1b*pbDp1b+1664*q1Dpb*q2Dp1b
     #*mb**2-1024*q1Dp1b*q2Dpb*pbDp1b-1664*q1Dp1b*q2Dpb*mb**2+1024*q1Dp1
     #b*q2Dpb**2)+CV/den0x2/den0x3*(-1024*q1Dq2*q1Dpb*pbDp1b-1280*q1Dq2*
     #q1Dpb*mb**2+1024*q1Dq2*q1Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-640*q1D
     #q2*mb**4-1024*q1Dpb*q1Dp1b*q2Dpb+2048*q1Dpb*q2Dpb*pbDp1b+2304*q1Dp
     #b*q2Dpb*mb**2+896*q1Dpb*q2Dp1b*mb**2+1024*q1Dpb**2*q2Dp1b-384*q1Dp
     #1b*q2Dpb*mb**2+256*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_v(59) = t0
c
c ----------m2_sme_v(60) ---------
c
      t0 = CV/den0x1/den0x3*(-1024*q1Dq2*q2Dpb*mb**2-256*q1Dq2*q2Dp1b*pb
     #Dp1b+1920*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+1664*q1Dq2*mb**4-
     #256*q1Dpb*q2Dp1b*pbDp1b-896*q1Dpb*q2Dp1b*mb**2+256*q1Dpb*q2Dp1b**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b+256*q1Dp1b*q2Dpb*pbDp1b+896*q1Dp1b*q2Dpb*
     #mb**2)+CV/den0x2/den0x3*(256*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*pb
     #Dp1b-1280*q1Dq2*q1Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-640*q1Dq2*mb**
     #4-256*q1Dpb*q1Dp1b*q2Dp1b+1024*q1Dpb*q2Dpb*mb**2+384*q1Dpb*q2Dp1b*
     #mb**2+1664*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dp1b*pbDp1b+1536*q1Dp1b
     #*q2Dp1b*mb**2+256*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(60) = t0
c
c ----------m2_sme_v(61) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*mb-32*q1Dq2*q1Dpb*q2D
     #p1b*mb-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp
     #1b*q2Dpb*mb-32*q1Dq2*q1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q1D
     #q2**2*pbDp1b*mb+32*q1Dq2**2*mb**3+96*q1Dpb*q1Dp1b*q2Dpb*mb+32*q1Dp
     #b*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dpb*mb+32*q1Dpb**2*q2Dp1b*mb+32*q
     #1Dp1b**2*q2Dpb*mb)+CV/den0x2/den0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb+32*
     #q1Dq2*q1Dpb*mb**3+32*q1Dq2*q1Dp1b*pbDp1b*mb+96*q1Dq2*q1Dp1b*mb**3-
     #160*q1Dpb*q1Dp1b*q2Dpb*mb-32*q1Dpb*q1Dp1b*q2Dp1b*mb-64*q1Dpb**2*q2
     #Dpb*mb+32*q1Dpb**2*q2Dp1b*mb-32*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_v(61) = t0
c
c ----------m2_sme_v(62) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb-32*q1Dq2*q1Dpb*pb
     #Dp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp1b*q2Dpb*mb-64*q1Dq2*q1D
     #p1b*q2Dp1b*mb-32*q1Dq2*q1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q
     #1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3+32*q1Dpb*q1Dp1b*q2Dpb*mb+96*q1
     #Dpb*q1Dp1b*q2Dp1b*mb+32*q1Dpb**2*q2Dp1b*mb+32*q1Dp1b**2*q2Dpb*mb+6
     #4*q1Dp1b**2*q2Dp1b*mb)+CV/den0x2/den0x3*(32*q1Dq2*q1Dpb*pbDp1b*mb-
     #32*q1Dq2*q1Dpb*mb**3+96*q1Dq2*q1Dp1b*pbDp1b*mb+32*q1Dq2*q1Dp1b*mb*
     #*3-32*q1Dpb*q1Dp1b*q2Dpb*mb-32*q1Dpb*q1Dp1b*q2Dp1b*mb-32*q1Dpb**2*
     #q2Dp1b*mb-96*q1Dp1b**2*q2Dpb*mb-64*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_v(62) = t0
c
c ----------m2_sme_v(63) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*mb-32*q1Dq2*q1Dpb*q2D
     #p1b*mb+64*q1Dq2*q1Dpb*pbDp1b*mb+64*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp
     #1b*q2Dpb*mb+32*q1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3)+CV/den0x2/den
     #0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp1
     #b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3-32*q1Dpb*q1Dp1b*q2Dpb*mb+32*q1Dp
     #b*q1Dp1b*q2Dp1b*mb+32*q1Dpb**2*q2Dp1b*mb-32*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_v(63) = t0
c
c ----------m2_sme_v(64) ---------
c
      t0 = CV/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb-32*q1Dq2*q1Dp1b*q
     #2Dpb*mb-64*q1Dq2*q1Dp1b*q2Dp1b*mb+64*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1D
     #q2*q1Dp1b*mb**3+32*q1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3)+CV/den0x2
     #/den0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q
     #1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q1Dpb*q1Dp1b*q2Dpb*mb-32*
     #q1Dpb*q1Dp1b*q2Dp1b*mb-32*q1Dpb**2*q2Dp1b*mb+32*q1Dp1b**2*q2Dpb*mb
     #)
c
      m2_sme_v(64) = t0
c
c ----------m2_sme_v(65) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b-64*q1Dq2*q1Dpb*pbDp1
     #b-64*q1Dq2*q1Dpb*mb**2-64*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2**2*pbDp1b+64
     #*q1Dq2**2*mb**2-64*q1Dpb*q1Dp1b*q2Dpb+64*q1Dpb**2*q2Dp1b)+CV/den0x
     #2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b-64*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1
     #Dp1b*pbDp1b-192*q1Dq2*q1Dp1b*mb**2)
c
      m2_sme_v(65) = t0
c
c ----------m2_sme_v(66) ---------
c
      s1 = CV/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1D
     #pb*q2Dp1b*mb**2+96*q1Dq2*q1Dpb*pbDp1b*mb**2+96*q1Dq2*q1Dpb*mb**4+6
     #4*q1Dq2*q1Dp1b*q2Dpb*mb**2-160*q1Dq2*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*
     #q1Dp1b*pbDp1b**2-96*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64
     #*q1Dq2**2*pbDp1b**2+32*q1Dpb*q1Dp1b*q2Dpb*mb**2-64*q1Dpb*q1Dp1b*q2
     #Dp1b*pbDp1b-32*q1Dpb*q1Dp1b*q2Dp1b*mb**2-32*q1Dpb**2*q2Dp1b*mb**2+
     #64*q1Dpb**2*q2Dp1b**2+64*q1Dp1b**2*q2Dpb*pbDp1b+32*q1Dp1b**2*q2Dpb
     #*mb**2-64*q1Dp1b**2*q2Dpb**2)
      s2 = CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-32*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*pbDp1b*mb**2-6
     #4*q1Dq2*q1Dp1b*pbDp1b**2+32*q1Dq2*q1Dp1b*mb**4-128*q1Dpb*q1Dp1b*q2
     #Dpb*pbDp1b-96*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2Dp1b*pbDp
     #1b+96*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b**2*q2Dpb-128*q1Dp
     #b**2*q1Dp1b*q2Dp1b+64*q1Dpb**2*q2Dpb*mb**2+32*q1Dpb**2*q2Dp1b*mb**
     #2+64*q1Dp1b**2*q2Dpb*pbDp1b-32*q1Dp1b**2*q2Dpb*mb**2-64*q1Dp1b**2*
     #q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(66) = t0
c
c ----------m2_sme_v(67) ---------
c
      s1 = CV/den0x1/den0x3*(-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1D
     #pb*q2Dp1b*mb**2+256*q1Dq2*q1Dpb*pbDp1b*mb**2+256*q1Dq2*q1Dpb*mb**4
     #+64*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2-320*q1D
     #q2*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q1Dp1b*pbDp1b**2-192*q1Dq2*q1Dp1b
     #*mb**4+64*q1Dq2**2*pbDp1b*mb**2+128*q1Dq2**2*pbDp1b**2-64*q1Dq2**2
     #*mb**4+128*q1Dpb*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b
     #-192*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b*mb**2+128*q1Dpb
     #**2*q2Dp1b**2+128*q1Dp1b**2*q2Dpb*pbDp1b+192*q1Dp1b**2*q2Dpb*mb**2
     #-128*q1Dp1b**2*q2Dpb**2)
      s2 = CV/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-128*q1Dq2*q1D
     #pb*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4-128*q1Dq2*q1Dp1b*pbDp1b*mb**2
     #-128*q1Dq2*q1Dp1b*pbDp1b**2+64*q1Dq2*q1Dp1b*mb**4-128*q1Dq2*q1Dp1b
     #**2*mb**2-256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*q2Dpb*mb*
     #*2+128*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2+25
     #6*q1Dpb*q1Dp1b**2*q2Dpb-256*q1Dpb**2*q1Dp1b*q2Dp1b+128*q1Dpb**2*q2
     #Dpb*mb**2+128*q1Dpb**2*q2Dp1b*mb**2-128*q1Dp1b**2*q2Dpb*pbDp1b-128
     #*q1Dp1b**2*q2Dpb*mb**2)
      t0 = s1+s2
c
      m2_sme_v(67) = t0
c
c ----------m2_sme_v(68) ---------
c
      t0 = CV/den0x1/den0x3*(-384*q1Dq2*q1Dpb*q2Dpb*pbDp1b-128*q1Dq2*q1D
     #pb*q2Dpb*mb**2-64*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*
     #*2-128*q1Dq2*q1Dpb*mb**4-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-64*q1Dq2*q1Dp
     #1b*pbDp1b*mb**2-64*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64*
     #q1Dq2**2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*q2Dp
     #b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb**2+64*q1Dpb*q1Dp1b*q2Dp1b*mb**2+384
     #*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dpb**2*q2Dpb*mb**2-128*q1Dpb**2*q2Dp1
     #b*pbDp1b-256*q1Dpb**2*q2Dp1b*mb**2+64*q1Dp1b**2*q2Dpb*mb**2)+CV/de
     #n0x2/den0x3*(384*q1Dq2*q1Dpb*q1Dp1b*mb**2+64*q1Dq2*q1Dpb*pbDp1b*mb
     #**2+64*q1Dq2*q1Dpb*mb**4+512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+448*q1Dpb*q
     #1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-768*q1Dpb**2*q1Dp1
     #b*q2Dpb+128*q1Dpb**2*q2Dpb*mb**2-64*q1Dpb**2*q2Dp1b*mb**2)
c
      m2_sme_v(68) = t0
c
c ----------m2_sme_v(69) ---------
c
      s1 = CV/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2-256*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b
     #*mb**2-128*q1Dq2*q1Dpb*mb**4-256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-64*q1Dq
     #2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b
     #*pbDp1b*mb**2+64*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+128*q
     #1Dq2**2*pbDp1b**2-64*q1Dq2**2*mb**4+512*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-
     #512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-512*q1Dpb*q1Dp1b*q2Dpb*mb**2-64*q1Dp
     #b*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**2*q2Dp1b
     #*mb**2+128*q1Dpb**2*q2Dp1b**2-64*q1Dp1b**2*q2Dpb*mb**2+128*q1Dp1b*
     #*2*q2Dpb**2)
      s2 = CV/den0x2/den0x3*(384*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+64*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b**2+192*q1Dq2*q1Dpb*mb**4+128
     #*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+192*q1Dpb*q1Dp1b*q2Dpb*mb**2+256*q1Dpb*
     #q1Dp1b*q2Dp1b*mb**2-384*q1Dpb*q1Dp1b**2*q2Dpb-384*q1Dpb**2*q1Dp1b*
     #q2Dp1b-128*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**2*q2Dp1b*pbDp1b+192*q1D
     #pb**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(69) = t0
c
c ----------m2_sme_v(70) ---------
c
      s1 = CV/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2-64*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q1Dpb*mb**4-2
     #56*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2
     #*q1Dp1b*q2Dp1b*mb**2-192*q1Dq2*q1Dp1b*pbDp1b*mb**2-192*q1Dq2*q1Dp1
     #b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64*q1Dq2**2*mb**4+512*q1Dpb*q1Dp1
     #b*q2Dpb*q2Dp1b-256*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1
     #b*pbDp1b-320*q1Dpb*q1Dp1b*q2Dp1b*mb**2-64*q1Dp1b**2*q2Dpb*mb**2+25
     #6*q1Dp1b**2*q2Dpb**2+128*q1Dp1b**2*q2Dp1b*mb**2)
      s2 = CV/den0x2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-64*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4+128*q1Dq2*q1Dp1b*pbDp1b*mb**2+
     #128*q1Dq2*q1Dp1b*mb**4+256*q1Dq2*q1Dp1b**2*mb**2+64*q1Dpb*q1Dp1b*q
     #2Dpb*mb**2-640*q1Dpb*q1Dp1b**2*q2Dpb-128*q1Dpb**2*q1Dp1b*q2Dp1b+64
     #*q1Dpb**2*q2Dp1b*mb**2+512*q1Dp1b**2*q2Dpb*pbDp1b+384*q1Dp1b**2*q2
     #Dpb*mb**2-256*q1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(70) = t0
c
c ----------m2_sme_v(71) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*mb**2-320*q1Dq2*q1Dp
     #1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2D
     #p1b*mb**2-64*q1Dq2*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*q1Dp1b*mb**4+64*q1
     #Dq2**2*pbDp1b*mb**2+64*q1Dq2**2*mb**4+256*q1Dpb*q1Dp1b*q2Dpb*mb**2
     #+64*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b**2+640*q1Dp1
     #b**2*q2Dpb*q2Dp1b-512*q1Dp1b**2*q2Dpb*pbDp1b-448*q1Dp1b**2*q2Dpb*m
     #b**2-128*q1Dp1b**2*q2Dp1b*mb**2)+CV/den0x2/den0x3*(128*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-64*q1Dq2*q1Dpb*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4+128
     #*q1Dq2*q1Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dp1b*mb**4+256*q1Dq2*q1Dp1b
     #**2*pbDp1b-64*q1Dpb*q1Dp1b*q2Dpb*mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbD
     #p1b+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-512*q1Dpb*q1Dp1b**2*q2Dp1b-64*q1
     #Dpb**2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb*mb**2+256*q1Dp1b**2*q2Dp1b
     #*mb**2-256*q1Dp1b**3*q2Dpb)
c
      m2_sme_v(71) = t0
c
c ----------m2_sme_v(72) ---------
c
      t0 = CV/den0x1/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb-64*q1Dq2*q1Dpb*q2D
     #p1b*mb-192*q1Dq2*q1Dpb*pbDp1b*mb-192*q1Dq2*q1Dpb*mb**3-64*q1Dq2*q1
     #Dp1b*q2Dpb*mb-64*q1Dq2*q1Dp1b*pbDp1b*mb-64*q1Dq2*q1Dp1b*mb**3+64*q
     #1Dq2**2*pbDp1b*mb+64*q1Dq2**2*mb**3-64*q1Dpb*q1Dp1b*q2Dpb*mb-64*q1
     #Dpb*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dp1b*mb+64*q1Dp1b**2*q2Dpb*mb)+
     #CV/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b*mb-256*q1Dpb*q1Dp1b*q2Dpb
     #*mb-128*q1Dpb*q1Dp1b*q2Dp1b*mb-128*q1Dpb**2*q2Dpb*mb)
c
      m2_sme_v(72) = t0
c
c ----------m2_sme_v(73) ---------
c
      t0 = CV/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*mb-64*q1Dq2*q1Dpb*pb
     #Dp1b*mb-64*q1Dq2*q1Dpb*mb**3+192*q1Dq2*q1Dp1b*q2Dpb*mb-128*q1Dq2*q
     #1Dp1b*q2Dp1b*mb-192*q1Dq2*q1Dp1b*pbDp1b*mb-192*q1Dq2*q1Dp1b*mb**3+
     #64*q1Dq2**2*pbDp1b*mb+64*q1Dq2**2*mb**3-64*q1Dpb*q1Dp1b*q2Dpb*mb+1
     #92*q1Dpb*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dp1b*mb-192*q1Dp1b**2*q2Dp
     #b*mb)+CV/den0x2/den0x3*(256*q1Dq2*q1Dp1b**2*mb-128*q1Dpb*q1Dp1b*q2
     #Dpb*mb-256*q1Dp1b**2*q2Dpb*mb-128*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_v(73) = t0
c
c ----------m2_sme_v(74) ---------
c
      t0 = CV/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbD
     #p1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb+128*q1Dq2**2*pbDp
     #1b+128*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dp1b)+
     #CV/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q1Dp1b*pbDp1b-1
     #28*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dp1b**2*q2Dpb)
c
      m2_sme_v(74) = t0
c
c ----------m2_sme_v(75) ---------
c
      s1 = CV/den0x1/den0x3*(-256*q1Dq2*q1Dpb*q2Dpb*mb**2-512*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+896*q1Dq2*q1Dpb*pbDp1
     #b*mb**2+896*q1Dq2*q1Dpb*mb**4+256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+128*q1
     #Dq2*q1Dp1b*q2Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dp1b*mb**2-1024*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2-512*q1Dq2*q1Dp1b*pbDp1b**2-512*q1Dq2*q1Dp1b*mb**
     #4+128*q1Dq2**2*pbDp1b*mb**2+256*q1Dq2**2*pbDp1b**2-128*q1Dq2**2*mb
     #**4+256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+384*q1Dpb*q1Dp1b*q2Dpb*mb**2-512
     #*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-768*q1Dpb*q1Dp1b*q2Dp1b*mb**2-384*q1Dp
     #b**2*q2Dp1b*mb**2+256*q1Dpb**2*q2Dp1b**2+512*q1Dp1b**2*q2Dpb*pbDp1
     #b+768*q1Dp1b**2*q2Dpb*mb**2-512*q1Dp1b**2*q2Dpb**2)
      s2 = CV/den0x2/den0x3*(768*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-256*q1Dq2*q1D
     #pb*pbDp1b*mb**2-256*q1Dq2*q1Dpb*mb**4+128*q1Dq2*q1Dp1b*pbDp1b*mb**
     #2-256*q1Dq2*q1Dp1b*pbDp1b**2+384*q1Dq2*q1Dp1b*mb**4-512*q1Dq2*q1Dp
     #1b**2*mb**2-1024*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-512*q1Dpb*q1Dp1b*q2Dpb*
     #mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+384*q1Dpb*q1Dp1b*q2Dp1b*mb**2
     #+768*q1Dpb*q1Dp1b**2*q2Dpb-768*q1Dpb**2*q1Dp1b*q2Dp1b+512*q1Dpb**2
     #*q2Dpb*mb**2+256*q1Dpb**2*q2Dp1b*mb**2+256*q1Dp1b**2*q2Dpb*pbDp1b+
     #128*q1Dp1b**2*q2Dpb*mb**2-256*q1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_v(75) = t0
c
c
c
c ----------m2_sme_a(1) ---------
c
      t0 = CA/den0x1/den0x3*(64.D0*q1Dq2*q2Dpb*mb-64.D0*q1Dq2*q2Dp1b*mb+
     #64.D0*q1Dpb*q2Dp1b*mb-64.D0*q1Dp1b*q2Dpb*mb)+CA/den0x2/den0x3*(-64
     #.D0*q1Dq2*q1Dpb*mb+64.D0*q1Dq2*q1Dp1b*mb+64.D0*q1Dpb*q2Dp1b*mb-64.
     #D0*q1Dp1b*q2Dpb*mb)
c
      m2_sme_a(1) = t0
c
c ----------m2_sme_a(2) ---------
c
      t0 = CA/den0x1/den0x3*(-64.D0*q1Dq2*q1Dpb*q2Dp1b-32.D0*q1Dq2*q1Dpb
     #*pbDp1b-32.D0*q1Dq2*q1Dpb*mb**2-64.D0*q1Dq2*q1Dp1b*q2Dpb-32.D0*q1D
     #q2*q1Dp1b*pbDp1b-32.D0*q1Dq2*q1Dp1b*mb**2+64.D0*q1Dq2**2*pbDp1b+64
     #.D0*q1Dq2**2*mb**2-32.D0*q1Dpb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2D
     #p1b+32.D0*q1Dpb**2*q2Dp1b+32.D0*q1Dp1b**2*q2Dpb)+CA/den0x2/den0x3*
     #(128.D0*q1Dq2*q1Dpb*q1Dp1b-32.D0*q1Dq2*q1Dpb*pbDp1b-32.D0*q1Dq2*q1
     #Dpb*mb**2-32.D0*q1Dq2*q1Dp1b*pbDp1b-32.D0*q1Dq2*q1Dp1b*mb**2-32.D0
     #*q1Dpb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb**2*q2Dp1
     #b+32.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(2) = t0
c
c ----------m2_sme_a(3) ---------
c
      t0 = CA/den0x1/den0x3*(-128.D0*q1Dq2*q2Dpb*q2Dp1b+32.D0*q1Dq2*q2Dp
     #b*pbDp1b+32.D0*q1Dq2*q2Dpb*mb**2+32.D0*q1Dq2*q2Dp1b*pbDp1b+32.D0*q
     #1Dq2*q2Dp1b*mb**2+32.D0*q1Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+3
     #2.D0*q1Dp1b*q2Dpb*q2Dp1b-32.D0*q1Dp1b*q2Dpb**2)+CA/den0x2/den0x3*(
     #64.D0*q1Dq2*q1Dpb*q2Dp1b+64.D0*q1Dq2*q1Dp1b*q2Dpb+32.D0*q1Dq2*q2Dp
     #b*pbDp1b+32.D0*q1Dq2*q2Dpb*mb**2+32.D0*q1Dq2*q2Dp1b*pbDp1b+32.D0*q
     #1Dq2*q2Dp1b*mb**2-64.D0*q1Dq2**2*pbDp1b-64.D0*q1Dq2**2*mb**2+32.D0
     #*q1Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+32.D0*q1Dp1b*q2Dpb*q2Dp1
     #b-32.D0*q1Dp1b*q2Dpb**2)
c
      m2_sme_a(3) = t0
c
c ----------m2_sme_a(4) ---------
c
      t0 = CA/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-32.D0*q1Dq2*q2Dp1
     #b*mb**2+96.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*pbDp1b**2+64.D0*q1Dq2
     #*mb**4+96.D0*q1Dpb*q2Dpb*q2Dp1b-64.D0*q1Dpb*q2Dpb*pbDp1b-128.D0*q1
     #Dpb*q2Dpb*mb**2-32.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*q1Dpb*q2Dp1b*mb**2
     #-32.D0*q1Dp1b*q2Dpb*pbDp1b-64.D0*q1Dp1b*q2Dpb*mb**2+32.D0*q1Dp1b*q
     #2Dpb**2)+CA/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+32.D0*q1Dq2*q1
     #Dp1b*mb**2-96.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*pbDp1b**2-64.D0*q1
     #Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*q2Dpb+64.D0*q1Dpb*q2Dpb*pbDp1b+128.D0
     #*q1Dpb*q2Dpb*mb**2+32.D0*q1Dpb*q2Dp1b*pbDp1b+64.D0*q1Dpb*q2Dp1b*mb
     #**2-32.D0*q1Dpb**2*q2Dp1b+32.D0*q1Dp1b*q2Dpb*pbDp1b+64.D0*q1Dp1b*q
     #2Dpb*mb**2)
c
      m2_sme_a(4) = t0
c
c ----------m2_sme_a(5) ---------
c
      t0 = CA/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*mb**2-32.D0*q1Dq2*q2Dp1b
     #*pbDp1b+96.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*pbDp1b**2+64.D0*q1Dq2
     #*mb**4-32.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*q1Dpb*q2Dp1b*mb**2+32.D0*q1
     #Dpb*q2Dp1b**2+96.D0*q1Dp1b*q2Dpb*q2Dp1b-32.D0*q1Dp1b*q2Dpb*pbDp1b-
     #64.D0*q1Dp1b*q2Dpb*mb**2-64.D0*q1Dp1b*q2Dp1b*pbDp1b-128.D0*q1Dp1b*
     #q2Dp1b*mb**2)+CA/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*mb**2+32.D0*q1Dq
     #2*q1Dp1b*pbDp1b-96.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*pbDp1b**2-64.
     #D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb*q2Dp1b*pbDp1b
     #+64.D0*q1Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dpb*pbDp1b+64.D0*q1Dp1b*q
     #2Dpb*mb**2+64.D0*q1Dp1b*q2Dp1b*pbDp1b+128.D0*q1Dp1b*q2Dp1b*mb**2-3
     #2.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(5) = t0
c
c ----------m2_sme_a(6) ---------
c
      t0 = 0.D0
c
      m2_sme_a(6) = t0
c
c ----------m2_sme_a(7) ---------
c
      t0 = CA/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+96.D0*q1Dq2*q1Dpb*
     #mb**2+32.D0*q1Dq2*q1Dp1b*pbDp1b+96.D0*q1Dq2*q1Dp1b*mb**2-32.D0*q1D
     #pb*q1Dp1b*q2Dpb-32.D0*q1Dpb*q1Dp1b*q2Dp1b+32.D0*q1Dpb**2*q2Dp1b+32
     #.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(7) = t0
c
c ----------m2_sme_a(8) ---------
c
      t0 = CA/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-96.D0*q1Dq2*q2Dpb
     #*mb**2-32.D0*q1Dq2*q2Dp1b*pbDp1b-96.D0*q1Dq2*q2Dp1b*mb**2+32.D0*q1
     #Dpb*q2Dpb*q2Dp1b-32.D0*q1Dpb*q2Dp1b**2+32.D0*q1Dp1b*q2Dpb*q2Dp1b-3
     #2.D0*q1Dp1b*q2Dpb**2)
c
      m2_sme_a(8) = t0
c
c ----------m2_sme_a(9) ---------
c
      t0 = CA/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*pbDp1b-32.D0*q1Dq2*q2Dp1
     #b*mb**2-32.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*mb**4+96.D0*q1Dpb*q2D
     #pb*q2Dp1b-32.D0*q1Dpb*q2Dpb*mb**2-64.D0*q1Dpb*q2Dp1b*pbDp1b-64.D0*
     #q1Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dpb**2+32.D0*q1Dp1b*q2Dp1b*mb**2
     #)+CA/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*pbDp1b+32.D0*q1Dq2*q1Dp1b*mb
     #**2+32.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b*
     #q2Dpb+32.D0*q1Dpb*q2Dpb*mb**2-32.D0*q1Dpb**2*q2Dp1b+64.D0*q1Dp1b*q
     #2Dpb*pbDp1b+64.D0*q1Dp1b*q2Dpb*mb**2-32.D0*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_a(9) = t0
c
c ----------m2_sme_a(10) ---------
c
      t0 = CA/den0x1/den0x3*(-32.D0*q1Dq2*q2Dpb*mb**2-32.D0*q1Dq2*q2Dp1b
     #*pbDp1b-32.D0*q1Dq2*pbDp1b*mb**2-32.D0*q1Dq2*mb**4+32.D0*q1Dpb*q2D
     #pb*mb**2+32.D0*q1Dpb*q2Dp1b**2+96.D0*q1Dp1b*q2Dpb*q2Dp1b-64.D0*q1D
     #p1b*q2Dpb*pbDp1b-64.D0*q1Dp1b*q2Dpb*mb**2-32.D0*q1Dp1b*q2Dp1b*mb**
     #2)+CA/den0x2/den0x3*(32.D0*q1Dq2*q1Dpb*mb**2+32.D0*q1Dq2*q1Dp1b*pb
     #Dp1b+32.D0*q1Dq2*pbDp1b*mb**2+32.D0*q1Dq2*mb**4-96.D0*q1Dpb*q1Dp1b
     #*q2Dp1b-32.D0*q1Dpb*q2Dpb*mb**2+64.D0*q1Dpb*q2Dp1b*pbDp1b+64.D0*q1
     #Dpb*q2Dp1b*mb**2+32.D0*q1Dp1b*q2Dp1b*mb**2-32.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(10) = t0
c
c ----------m2_sme_a(11) ---------
c
      t0 = CA/den0x1/den0x3*(-64.D0*q1Dq2*q2Dpb*pbDp1b-64.D0*q1Dq2*q2Dp1
     #b*mb**2+192.D0*q1Dq2*pbDp1b*mb**2+64.D0*q1Dq2*pbDp1b**2+128.D0*q1D
     #q2*mb**4-64.D0*q1Dpb*q2Dpb*q2Dp1b+64.D0*q1Dpb*q2Dp1b*pbDp1b+128.D0
     #*q1Dpb*q2Dp1b*mb**2-64.D0*q1Dp1b*q2Dpb*pbDp1b-128.D0*q1Dp1b*q2Dpb*
     #mb**2+64.D0*q1Dp1b*q2Dpb**2)+CA/den0x2/den0x3*(-64.D0*q1Dq2*q1Dpb*
     #pbDp1b-128.D0*q1Dq2*q1Dpb*mb**2+64.D0*q1Dq2*q1Dp1b*mb**2-64.D0*q1D
     #q2*pbDp1b*mb**2-64.D0*q1Dq2*mb**4-64.D0*q1Dpb*q1Dp1b*q2Dpb+128.D0*
     #q1Dpb*q2Dpb*pbDp1b+192.D0*q1Dpb*q2Dpb*mb**2+128.D0*q1Dpb*q2Dp1b*mb
     #**2+64.D0*q1Dpb**2*q2Dp1b+64.D0*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_a(11) = t0
c
c ----------m2_sme_a(12) ---------
c
      t0 = CA/den0x1/den0x3*(-64.D0*q1Dq2*q2Dpb*mb**2-64.D0*q1Dq2*q2Dp1b
     #*pbDp1b+192.D0*q1Dq2*pbDp1b*mb**2+64.D0*q1Dq2*pbDp1b**2+128.D0*q1D
     #q2*mb**4-64.D0*q1Dpb*q2Dp1b*pbDp1b-128.D0*q1Dpb*q2Dp1b*mb**2+64.D0
     #*q1Dpb*q2Dp1b**2-64.D0*q1Dp1b*q2Dpb*q2Dp1b+64.D0*q1Dp1b*q2Dpb*pbDp
     #1b+128.D0*q1Dp1b*q2Dpb*mb**2)+CA/den0x2/den0x3*(64.D0*q1Dq2*q1Dpb*
     #mb**2-64.D0*q1Dq2*q1Dp1b*pbDp1b-128.D0*q1Dq2*q1Dp1b*mb**2-64.D0*q1
     #Dq2*pbDp1b*mb**2-64.D0*q1Dq2*mb**4-64.D0*q1Dpb*q1Dp1b*q2Dp1b+64.D0
     #*q1Dpb*q2Dpb*mb**2+128.D0*q1Dp1b*q2Dpb*mb**2+128.D0*q1Dp1b*q2Dp1b*
     #pbDp1b+192.D0*q1Dp1b*q2Dp1b*mb**2+64.D0*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(12) = t0
c
c ----------m2_sme_a(13) ---------
c
      t0 = CA/den0x1/den0x3*(64.D0*q1Dq2*q2Dpb*pbDp1b*mb-64.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb-64.D0*q1Dq2*pbDp1b**2*mb+64.D0*q1Dq2*mb**5-64.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+64.D0*q1Dpb*q2Dp1b*pbDp1b*mb+64.D0*q1Dpb*q2Dp1b
     #**2*mb-64.D0*q1Dp1b*q2Dpb*q2Dp1b*mb-64.D0*q1Dp1b*q2Dpb*pbDp1b*mb+6
     #4.D0*q1Dp1b*q2Dpb**2*mb)+CA/den0x2/den0x3*(-64.D0*q1Dq2*q1Dpb*pbDp
     #1b*mb+64.D0*q1Dq2*q1Dp1b*pbDp1b*mb+64.D0*q1Dq2*pbDp1b**2*mb-64.D0*
     #q1Dq2*mb**5-64.D0*q1Dpb*q1Dp1b*q2Dpb*mb-64.D0*q1Dpb*q1Dp1b*q2Dp1b*
     #mb+128.D0*q1Dpb*q2Dpb*mb**3-64.D0*q1Dpb*q2Dp1b*pbDp1b*mb+64.D0*q1D
     #pb**2*q2Dp1b*mb-192.D0*q1Dp1b*q2Dpb*pbDp1b*mb+128.D0*q1Dp1b*q2Dp1b
     #*mb**3+64.D0*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_a(13) = t0
c
c ----------m2_sme_a(14) ---------
c
      t0 = CA/den0x1/den0x3*(32.D0*q1Dq2*q2Dpb*pbDp1b*mb-32.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb-32.D0*q1Dq2*pbDp1b**2*mb+32.D0*q1Dq2*mb**5-32.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+32.D0*q1Dpb*q2Dp1b**2*mb+32.D0*q1Dpb*q2Dp1b*pbD
     #p1b*mb-32.D0*q1Dp1b*q2Dpb**2*mb+32.D0*q1Dp1b*q2Dpb*q2Dp1b*mb+32.D0
     #*q1Dp1b*q2Dpb*pbDp1b*mb-64.D0*q1Dp1b*q2Dp1b*mb**3)+CA/den0x2/den0x
     #3*(-32.D0*q1Dq2*q1Dpb*pbDp1b*mb+32.D0*q1Dq2*q1Dp1b*pbDp1b*mb+32.D0
     #*q1Dq2*pbDp1b**2*mb-32.D0*q1Dq2*mb**5+32.D0*q1Dpb**2*q2Dp1b*mb+32.
     #D0*q1Dpb*q1Dp1b*q2Dpb*mb-32.D0*q1Dpb*q1Dp1b*q2Dp1b*mb-32.D0*q1Dpb*
     #q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b**2*q2Dpb*mb-32.D0*q1Dp1b*q2Dpb*pbDp1
     #b*mb+64.D0*q1Dp1b*q2Dp1b*mb**3)
c
      m2_sme_a(14) = t0
c
c ----------m2_sme_a(15) ---------
c
      t0 = CA/den0x1/den0x3*(32.D0*q1Dq2*q2Dpb*pbDp1b*mb-32.D0*q1Dq2*q2D
     #p1b*pbDp1b*mb+32.D0*q1Dq2*pbDp1b**2*mb-32.D0*q1Dq2*mb**5-32.D0*q1D
     #pb*q2Dpb*q2Dp1b*mb+64.D0*q1Dpb*q2Dpb*mb**3+32.D0*q1Dpb*q2Dp1b**2*m
     #b-32.D0*q1Dpb*q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b*q2Dpb**2*mb+32.D0*q1Dp
     #1b*q2Dpb*q2Dp1b*mb-32.D0*q1Dp1b*q2Dpb*pbDp1b*mb)+CA/den0x2/den0x3*
     #(-32.D0*q1Dq2*q1Dpb*pbDp1b*mb+32.D0*q1Dq2*q1Dp1b*pbDp1b*mb-32.D0*q
     #1Dq2*pbDp1b**2*mb+32.D0*q1Dq2*mb**5+32.D0*q1Dpb**2*q2Dp1b*mb+32.D0
     #*q1Dpb*q1Dp1b*q2Dpb*mb-32.D0*q1Dpb*q1Dp1b*q2Dp1b*mb-64.D0*q1Dpb*q2
     #Dpb*mb**3+32.D0*q1Dpb*q2Dp1b*pbDp1b*mb-32.D0*q1Dp1b**2*q2Dpb*mb+32
     #.D0*q1Dp1b*q2Dpb*pbDp1b*mb)
c
      m2_sme_a(15) = t0
c
c ----------m2_sme_a(16) ---------
c
      t0 = CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-32*q1Dq2*q1Dpb**2*pbDp1b-32*q1Dq2*q1Dp1b**2*mb**2+32
     #*q1Dpb*q1Dp1b**2*q2Dpb-32*q1Dpb**2*q1Dp1b*q2Dpb-32*q1Dpb**2*q1Dp1b
     #*q2Dp1b+32*q1Dpb**3*q2Dp1b)
c
      m2_sme_a(16) = t0
c
c ----------m2_sme_a(17) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dpb*pbDp1b-64*q1Dq2*q1Dpb
     #*q2Dpb*mb**2-32*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*q2Dp1b*mb
     #**2-64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1
     #Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*pbDp1
     #b**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp1
     #b*q2Dpb**2+32*q1Dpb**2*q2Dpb*q2Dp1b+32*q1Dp1b**2*q2Dpb**2)
c
      m2_sme_a(17) = t0
c
c ----------m2_sme_a(18) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*pbDp1b-64*q1Dq2*q1Dpb
     #*q2Dpb*mb**2-32*q1Dq2*q1Dpb*q2Dp1b*mb**2+48*q1Dq2*q1Dpb*pbDp1b*mb*
     #*2+32*q1Dq2*q1Dpb*pbDp1b**2+16*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*q
     #2Dpb*mb**2-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q
     #1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*pbDp1
     #b-16*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2Dpb**2+16*q1Dpb*q1
     #Dp1b*q2Dp1b*mb**2+64*q1Dpb**2*q2Dpb*q2Dp1b-32*q1Dpb**2*q2Dpb*mb**2
     #-32*q1Dpb**2*q2Dp1b*pbDp1b-48*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp1b**2*q
     #2Dpb*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*mb**2-16*q1Dq2*q1Dpb*
     #pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*
     #q1Dq2*q1Dp1b*mb**4+64*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+48*q1Dpb*q1Dp1b*q2
     #Dpb*mb**2-16*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb**2*q1Dp1b*q2Dpb+3
     #2*q1Dpb**2*q2Dpb*mb**2+16*q1Dpb**2*q2Dp1b*mb**2-16*q1Dp1b**2*q2Dpb
     #*mb**2)
      t0 = s1+s2
c
      m2_sme_a(18) = t0
c
c ----------m2_sme_a(19) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2-16*q1Dq2*q1Dpb*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-64
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2+48*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2+48*q1Dq2*q1Dp1b*mb**4+32*q1Dq2**2*pbDp1b*mb**2+3
     #2*q1Dq2**2*pbDp1b**2+64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-64*q1Dpb*q1Dp1b*
     #q2Dpb*pbDp1b-80*q1Dpb*q1Dp1b*q2Dpb*mb**2-16*q1Dpb*q1Dp1b*q2Dp1b*mb
     #**2+32*q1Dpb**2*q2Dpb*mb**2+16*q1Dpb**2*q2Dp1b*mb**2+32*q1Dpb**2*q
     #2Dp1b**2-16*q1Dp1b**2*q2Dpb*mb**2+32*q1Dp1b**2*q2Dpb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-32*q1Dq2*q1Dpb*pbDp1b**2+16*q1Dq2*q1Dpb*mb**4-16*q1D
     #q2*q1Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q1Dpb*q1Dp1b*q2Dpb
     #*pbDp1b+48*q1Dpb*q1Dp1b*q2Dpb*mb**2+16*q1Dpb*q1Dp1b*q2Dp1b*mb**2-6
     #4*q1Dpb*q1Dp1b**2*q2Dpb-64*q1Dpb**2*q1Dp1b*q2Dp1b-32*q1Dpb**2*q2Dp
     #b*mb**2+32*q1Dpb**2*q2Dp1b*pbDp1b+16*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp
     #1b**2*q2Dpb*mb**2)
      t0 = s1+s2
c
      m2_sme_a(19) = t0
c
c ----------m2_sme_a(20) ---------
c
      t0 = CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dpb*pbDp1b+64*q1Dq2*q1Dpb*
     #q2Dpb*mb**2+64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb*q2Dp1b*mb*
     #*2+32*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+64*q1Dq2*q1Dp1b*q2Dpb*mb**2-32*q1D
     #q2*q1Dp1b*q2Dp1b*mb**2-64*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*pbDp1b
     #**2-32*q1Dq2**2*mb**4+32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp1b
     #*q2Dpb**2+32*q1Dpb**2*q2Dpb*q2Dp1b-32*q1Dpb**2*q2Dp1b**2)
c
      m2_sme_a(20) = t0
c
c ----------m2_sme_a(21) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q2Dp
     #b*q2Dp1b*mb**2+32*q1Dq2*q2Dpb**2*pbDp1b+32*q1Dq2*q2Dp1b**2*mb**2-3
     #2*q1Dpb*q2Dpb*q2Dp1b**2+32*q1Dpb*q2Dpb**2*q2Dp1b+32*q1Dp1b*q2Dpb**
     #2*q2Dp1b-32*q1Dp1b*q2Dpb**3)
c
      m2_sme_a(21) = t0
c
c ----------m2_sme_a(22) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*mb**2+16*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16
     #*q1Dq2*q2Dp1b*mb**4-64*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-48*q1Dpb*q2Dpb*q2
     #Dp1b*mb**2+128*q1Dpb*q2Dpb**2*q2Dp1b-32*q1Dpb*q2Dpb**2*mb**2+16*q1
     #Dpb*q2Dp1b**2*mb**2+16*q1Dp1b*q2Dpb*q2Dp1b*mb**2-16*q1Dp1b*q2Dpb**
     #2*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dpb*pbDp1b+64*q1Dq2*q1Dpb*
     #q2Dpb*mb**2+32*q1Dq2*q1Dpb*q2Dp1b*mb**2+32*q1Dq2*q1Dp1b*q2Dpb*mb**
     #2-48*q1Dq2*q2Dpb*pbDp1b*mb**2-32*q1Dq2*q2Dpb*pbDp1b**2-16*q1Dq2*q2
     #Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1
     #Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*mb**4-64*q1Dpb*q1Dp1b*q2Dpb**2+32*
     #q1Dpb*q2Dpb*q2Dp1b*pbDp1b+16*q1Dpb*q2Dpb*q2Dp1b*mb**2+32*q1Dpb*q2D
     #pb**2*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-64*q1Dpb**2*q2Dpb*q2Dp1b-16*q
     #1Dp1b*q2Dpb*q2Dp1b*mb**2+32*q1Dp1b*q2Dpb**2*pbDp1b+48*q1Dp1b*q2Dpb
     #**2*mb**2)
      t0 = s1+s2
c
      m2_sme_a(22) = t0
c
c ----------m2_sme_a(23) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+16*q1Dq2*q2Dp
     #b*pbDp1b*mb**2+32*q1Dq2*q2Dpb*pbDp1b**2-16*q1Dq2*q2Dpb*mb**4+16*q1
     #Dq2*q2Dp1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1Dpb*q2Dpb*q2Dp1
     #b*pbDp1b-48*q1Dpb*q2Dpb*q2Dp1b*mb**2+64*q1Dpb*q2Dpb*q2Dp1b**2+32*q
     #1Dpb*q2Dpb**2*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-16*q1Dp1b*q2Dpb*q2Dp1
     #b*mb**2+64*q1Dp1b*q2Dpb**2*q2Dp1b-32*q1Dp1b*q2Dpb**2*pbDp1b-16*q1D
     #p1b*q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+32*q1Dq2*q1Dp1b*q2Dpb*m
     #b**2+16*q1Dq2*q2Dpb*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4-48*q1Dq2*q2D
     #p1b*pbDp1b*mb**2-48*q1Dq2*q2Dp1b*mb**4-32*q1Dq2**2*pbDp1b*mb**2-32
     #*q1Dq2**2*pbDp1b**2-64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+64*q1Dpb*q2Dpb*q2
     #Dp1b*pbDp1b+80*q1Dpb*q2Dpb*q2Dp1b*mb**2-32*q1Dpb*q2Dpb**2*mb**2+16
     #*q1Dpb*q2Dp1b**2*mb**2-32*q1Dpb**2*q2Dp1b**2+16*q1Dp1b*q2Dpb*q2Dp1
     #b*mb**2-16*q1Dp1b*q2Dpb**2*mb**2-32*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_a(23) = t0
c
c ----------m2_sme_a(24) ---------
c
      t0 = CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-32*q1Dq2*q1Dpb**2*mb**2-32*q1Dq2*q1Dp1b**2*pbDp1b-32
     #*q1Dpb*q1Dp1b**2*q2Dpb-32*q1Dpb*q1Dp1b**2*q2Dp1b+32*q1Dpb**2*q1Dp1
     #b*q2Dp1b+32*q1Dp1b**3*q2Dpb)
c
      m2_sme_a(24) = t0
c
c ----------m2_sme_a(25) ---------
c
      t0 = CA/den0x1/den0x3*(32*q1Dq2*q1Dpb*q2Dpb*mb**2-64*q1Dq2*q1Dpb*q
     #2Dp1b*pbDp1b-32*q1Dq2*q1Dpb*q2Dp1b*mb**2-32*q1Dq2*q1Dp1b*q2Dpb*pbD
     #p1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-32*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-64*q
     #1Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*pbDp
     #1b**2+32*q1Dq2**2*mb**4-32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1Dp
     #1b*q2Dp1b**2+32*q1Dpb**2*q2Dp1b**2+32*q1Dp1b**2*q2Dpb*q2Dp1b)
c
      m2_sme_a(25) = t0
c
c ----------m2_sme_a(26) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2+48*q1Dq2*q1Dpb*pbDp1b*mb**2+48*q1Dq2*q1Dpb*mb**4-64
     #*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-32*q1Dq2*q1Dp1b*q2Dpb*mb**2-16*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2-16*q1Dq2*q1Dp1b*mb**4+32*q1Dq2**2*pbDp1b*mb**2+3
     #2*q1Dq2**2*pbDp1b**2+64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-16*q1Dpb*q1Dp1b*
     #q2Dpb*mb**2-64*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-80*q1Dpb*q1Dp1b*q2Dp1b*m
     #b**2-16*q1Dpb**2*q2Dp1b*mb**2+32*q1Dpb**2*q2Dp1b**2+16*q1Dp1b**2*q
     #2Dpb*mb**2+32*q1Dp1b**2*q2Dpb**2+32*q1Dp1b**2*q2Dp1b*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-32
     #*q1Dq2*q1Dp1b*pbDp1b**2+16*q1Dq2*q1Dp1b*mb**4+16*q1Dpb*q1Dp1b*q2Dp
     #b*mb**2+32*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+48*q1Dpb*q1Dp1b*q2Dp1b*mb**2
     #-64*q1Dpb*q1Dp1b**2*q2Dpb-64*q1Dpb**2*q1Dp1b*q2Dp1b+16*q1Dpb**2*q2
     #Dp1b*mb**2+32*q1Dp1b**2*q2Dpb*pbDp1b+16*q1Dp1b**2*q2Dpb*mb**2-32*q
     #1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(26) = t0
c
c ----------m2_sme_a(27) ---------
c
      s1 = CA/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb**2-16*q1Dq2*q1Dpb
     #*pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*q2Dpb*mb**2-64*
     #q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dp1b*q2Dp1b*mb**2+48*q1Dq2*q
     #1Dp1b*pbDp1b*mb**2+32*q1Dq2*q1Dp1b*pbDp1b**2+16*q1Dq2*q1Dp1b*mb**4
     #+32*q1Dq2**2*pbDp1b*mb**2+32*q1Dq2**2*mb**4+16*q1Dpb*q1Dp1b*q2Dpb*
     #mb**2-32*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-16*q1Dpb*q1Dp1b*q2Dp1b*mb**2+6
     #4*q1Dpb*q1Dp1b*q2Dp1b**2+16*q1Dpb**2*q2Dp1b*mb**2+64*q1Dp1b**2*q2D
     #pb*q2Dp1b-32*q1Dp1b**2*q2Dpb*pbDp1b-48*q1Dp1b**2*q2Dpb*mb**2-32*q1
     #Dp1b**2*q2Dp1b*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q1Dp1b*mb**2-16*q1Dq2*q1Dpb*
     #pbDp1b*mb**2-16*q1Dq2*q1Dpb*mb**4-16*q1Dq2*q1Dp1b*pbDp1b*mb**2-16*
     #q1Dq2*q1Dp1b*mb**4-16*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2D
     #p1b*pbDp1b+48*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb*q1Dp1b**2*q2Dp1b
     #-16*q1Dpb**2*q2Dp1b*mb**2+16*q1Dp1b**2*q2Dpb*mb**2+32*q1Dp1b**2*q2
     #Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(27) = t0
c
c ----------m2_sme_a(28) ---------
c
      t0 = CA/den0x2/den0x3*(-32*q1Dq2*q1Dpb*q2Dpb*mb**2+32*q1Dq2*q1Dpb*
     #q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pb
     #Dp1b+32*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+64*
     #q1Dq2*q1Dp1b*q2Dp1b*mb**2-64*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*pbD
     #p1b**2-32*q1Dq2**2*mb**4+32*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-32*q1Dpb*q1D
     #p1b*q2Dp1b**2+32*q1Dp1b**2*q2Dpb*q2Dp1b-32*q1Dp1b**2*q2Dpb**2)
c
      m2_sme_a(28) = t0
c
c ----------m2_sme_a(29) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q2Dpb*q2Dp1b*pbDp1b-32*q1Dq2*q2Dp
     #b*q2Dp1b*mb**2+32*q1Dq2*q2Dpb**2*mb**2+32*q1Dq2*q2Dp1b**2*pbDp1b+3
     #2*q1Dpb*q2Dpb*q2Dp1b**2-32*q1Dpb*q2Dp1b**3+32*q1Dp1b*q2Dpb*q2Dp1b*
     #*2-32*q1Dp1b*q2Dpb**2*q2Dp1b)
c
      m2_sme_a(29) = t0
c
c ----------m2_sme_a(30) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+16*q1Dq2*q2Dp
     #b*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+3
     #2*q1Dq2*q2Dp1b*pbDp1b**2-16*q1Dq2*q2Dp1b*mb**4-16*q1Dpb*q2Dpb*q2Dp
     #1b*mb**2+64*q1Dpb*q2Dpb*q2Dp1b**2-32*q1Dpb*q2Dp1b**2*pbDp1b-16*q1D
     #pb*q2Dp1b**2*mb**2-32*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-48*q1Dp1b*q2Dpb*q
     #2Dp1b*mb**2+64*q1Dp1b*q2Dpb**2*q2Dp1b-16*q1Dp1b*q2Dpb**2*mb**2+32*
     #q1Dp1b*q2Dp1b**2*mb**2)
      s2 = CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+32*q1Dq2*q1Dp1b*q2Dpb*m
     #b**2-48*q1Dq2*q2Dpb*pbDp1b*mb**2-48*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2D
     #p1b*pbDp1b*mb**2+16*q1Dq2*q2Dp1b*mb**4-32*q1Dq2**2*pbDp1b*mb**2-32
     #*q1Dq2**2*pbDp1b**2-64*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+16*q1Dpb*q2Dpb*q2
     #Dp1b*mb**2-16*q1Dpb*q2Dp1b**2*mb**2-32*q1Dpb**2*q2Dp1b**2+64*q1Dp1
     #b*q2Dpb*q2Dp1b*pbDp1b+80*q1Dp1b*q2Dpb*q2Dp1b*mb**2+16*q1Dp1b*q2Dpb
     #**2*mb**2-32*q1Dp1b*q2Dp1b**2*mb**2-32*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_a(30) = t0
c
c ----------m2_sme_a(31) ---------
c
      s1 = CA/den0x1/den0x3*(-64*q1Dq2*q2Dpb*q2Dp1b*mb**2+16*q1Dq2*q2Dpb
     #*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4+16*q1Dq2*q2Dp1b*pbDp1b*mb**2+16
     #*q1Dq2*q2Dp1b*mb**4+16*q1Dpb*q2Dpb*q2Dp1b*mb**2-16*q1Dpb*q2Dp1b**2
     #*mb**2-64*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-48*q1Dp1b*q2Dpb*q2Dp1b*mb**2+
     #128*q1Dp1b*q2Dpb*q2Dp1b**2+16*q1Dp1b*q2Dpb**2*mb**2-32*q1Dp1b*q2Dp
     #1b**2*mb**2)
      s2 = CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dp1b*mb**2+32*q1Dq2*q1Dp1b
     #*q2Dpb*mb**2+64*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dp1b*q2Dp1b*
     #mb**2+16*q1Dq2*q2Dpb*pbDp1b*mb**2+16*q1Dq2*q2Dpb*mb**4-48*q1Dq2*q2
     #Dp1b*pbDp1b*mb**2-32*q1Dq2*q2Dp1b*pbDp1b**2-16*q1Dq2*q2Dp1b*mb**4-
     #32*q1Dq2**2*pbDp1b*mb**2-32*q1Dq2**2*mb**4-64*q1Dpb*q1Dp1b*q2Dp1b*
     #*2-16*q1Dpb*q2Dpb*q2Dp1b*mb**2+32*q1Dpb*q2Dp1b**2*pbDp1b+48*q1Dpb*
     #q2Dp1b**2*mb**2+32*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+16*q1Dp1b*q2Dpb*q2Dp
     #1b*mb**2-16*q1Dp1b*q2Dpb**2*mb**2+32*q1Dp1b*q2Dp1b**2*mb**2-64*q1D
     #p1b**2*q2Dpb*q2Dp1b)
      t0 = s1+s2
c
      m2_sme_a(31) = t0
c
c ----------m2_sme_a(32) ---------
c
      t0 = CA/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb-32*q1Dq2*q2Dpb*mb*
     #*3-32*q1Dq2*q2Dp1b*pbDp1b*mb-96*q1Dq2*q2Dp1b*mb**3+160*q1Dpb*q2Dpb
     #*q2Dp1b*mb+64*q1Dpb*q2Dpb**2*mb+32*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2
     #Dpb*q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb)+CA/den0x2/den0x3*(64*q1Dq2*q1
     #Dpb*q2Dpb*mb+32*q1Dq2*q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb+32*
     #q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb**3+32*q1Dq2*q2Dp1b*pbDp1b*
     #mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3-9
     #6*q1Dpb*q2Dpb*q2Dp1b*mb-64*q1Dpb*q2Dpb**2*mb-32*q1Dpb*q2Dp1b**2*mb
     #-32*q1Dp1b*q2Dpb*q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb)
c
      m2_sme_a(32) = t0
c
c ----------m2_sme_a(33) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb
     #**3-96*q1Dq2*q2Dp1b*pbDp1b*mb-32*q1Dq2*q2Dp1b*mb**3+32*q1Dpb*q2Dpb
     #*q2Dp1b*mb+96*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2Dpb*q2Dp1b*mb+32*q1Dp
     #1b*q2Dpb**2*mb+64*q1Dp1b*q2Dp1b**2*mb)+CA/den0x2/den0x3*(32*q1Dq2*
     #q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb+64*q1Dq2*q1Dp1b*q2Dp1b*mb
     #+32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb**3+32*q1Dq2*q2Dp1b*pbD
     #p1b*mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb*
     #*3-32*q1Dpb*q2Dpb*q2Dp1b*mb-32*q1Dpb*q2Dp1b**2*mb-96*q1Dp1b*q2Dpb*
     #q2Dp1b*mb-32*q1Dp1b*q2Dpb**2*mb-64*q1Dp1b*q2Dp1b**2*mb)
c
      m2_sme_a(33) = t0
c
c ----------m2_sme_a(34) ---------
c
      t0 = CA/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb*
     #*3+32*q1Dq2*q2Dp1b*pbDp1b*mb+32*q1Dq2*q2Dp1b*mb**3+32*q1Dpb*q2Dpb*
     #q2Dp1b*mb+32*q1Dpb*q2Dp1b**2*mb-32*q1Dp1b*q2Dpb*q2Dp1b*mb-32*q1Dp1
     #b*q2Dpb**2*mb)+CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dpb*mb+32*q1Dq2*
     #q1Dpb*q2Dp1b*mb+32*q1Dq2*q1Dp1b*q2Dpb*mb-64*q1Dq2*q2Dpb*pbDp1b*mb-
     #64*q1Dq2*q2Dpb*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3)
c
      m2_sme_a(34) = t0
c
c ----------m2_sme_a(35) ---------
c
      t0 = CA/den0x1/den0x3*(32*q1Dq2*q2Dpb*pbDp1b*mb+32*q1Dq2*q2Dpb*mb*
     #*3+32*q1Dq2*q2Dp1b*pbDp1b*mb+32*q1Dq2*q2Dp1b*mb**3-32*q1Dpb*q2Dpb*
     #q2Dp1b*mb-32*q1Dpb*q2Dp1b**2*mb+32*q1Dp1b*q2Dpb*q2Dp1b*mb+32*q1Dp1
     #b*q2Dpb**2*mb)+CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*q2Dp1b*mb+32*q1Dq2
     #*q1Dp1b*q2Dpb*mb+64*q1Dq2*q1Dp1b*q2Dp1b*mb-64*q1Dq2*q2Dp1b*pbDp1b*
     #mb-64*q1Dq2*q2Dp1b*mb**3-32*q1Dq2**2*pbDp1b*mb-32*q1Dq2**2*mb**3)
c
      m2_sme_a(35) = t0
c
c ----------m2_sme_a(36) ---------
c
      t0 = CA/den0x1/den0x3*(64*q1Dq2*q2Dpb*mb-64*q1Dq2*q2Dp1b*mb-128*q1
     #Dq2*pbDp1b*mb-128*q1Dq2*mb**3+64*q1Dpb*q2Dpb*mb+128*q1Dpb*q2Dp1b*m
     #b+64*q1Dp1b*q2Dp1b*mb)+CA/den0x2/den0x3*(-64*q1Dq2*q1Dpb*mb+64*q1D
     #q2*q1Dp1b*mb+128*q1Dq2*pbDp1b*mb+128*q1Dq2*mb**3-64*q1Dpb*q2Dpb*mb
     #-128*q1Dp1b*q2Dpb*mb-64*q1Dp1b*q2Dp1b*mb)
c
      m2_sme_a(36) = t0
c
c ----------m2_sme_a(37) ---------
c
      t0 = CA/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b+64*q1Dq2*q2Dpb*mb**
     #2+128*q1Dq2*q2Dp1b*pbDp1b+192*q1Dq2*q2Dp1b*mb**2)+CA/den0x2/den0x3
     #*(64*q1Dq2*q1Dpb*q2Dp1b+64*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2*q2Dpb*pbDp1
     #b+64*q1Dq2*q2Dpb*mb**2-64*q1Dq2**2*pbDp1b-64*q1Dq2**2*mb**2+64*q1D
     #pb*q2Dpb*q2Dp1b-64*q1Dp1b*q2Dpb**2)
c
      m2_sme_a(37) = t0
c
c ----------m2_sme_a(38) ---------
c
      s1 = CA/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+32*q1Dq2*q2D
     #pb*pbDp1b*mb**2+32*q1Dq2*q2Dpb*mb**4+32*q1Dq2*q2Dp1b*pbDp1b*mb**2+
     #64*q1Dq2*q2Dp1b*pbDp1b**2-32*q1Dq2*q2Dp1b*mb**4-32*q1Dpb*q2Dpb*q2D
     #p1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b**2-64*q1Dpb*q2Dp1b**2*pbDp1b-32*q
     #1Dpb*q2Dp1b**2*mb**2+64*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+32*q1Dp1b*q2Dpb
     #*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*q2Dp1b+32*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+64*q1Dq2*q1Dp
     #b*q2Dp1b*mb**2-64*q1Dq2*q1Dp1b*q2Dpb*mb**2+32*q1Dq2*q2Dpb*pbDp1b*m
     #b**2+64*q1Dq2*q2Dpb*pbDp1b**2-32*q1Dq2*q2Dpb*mb**4+32*q1Dq2*q2Dp1b
     #*pbDp1b*mb**2+32*q1Dq2*q2Dp1b*mb**4-64*q1Dq2**2*pbDp1b*mb**2-64*q1
     #Dq2**2*pbDp1b**2-64*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-96*q1Dpb*q2Dpb*q2Dp1
     #b*mb**2+64*q1Dpb*q2Dpb**2*mb**2-32*q1Dpb*q2Dp1b**2*mb**2-64*q1Dpb*
     #*2*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+96*q1Dp1b*q2Dpb*q2Dp1b
     #*mb**2-64*q1Dp1b*q2Dpb**2*pbDp1b+32*q1Dp1b*q2Dpb**2*mb**2-64*q1Dp1
     #b*q2Dp1b**2*mb**2+64*q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_a(38) = t0
c
c ----------m2_sme_a(39) ---------
c
      s1 = CA/den0x1/den0x3*(64*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*pb
     #Dp1b*mb-128*q1Dq2*pbDp1b*mb**3-192*q1Dq2*pbDp1b**2*mb+64*q1Dq2*mb*
     #*5-64*q1Dpb*q2Dpb*q2Dp1b*mb-64*q1Dpb*q2Dpb*mb**3+192*q1Dpb*q2Dp1b*
     #pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb-64*q1Dp1b*q2
     #Dpb*q2Dp1b*mb-64*q1Dp1b*q2Dpb*pbDp1b*mb-64*q1Dp1b*q2Dpb*mb**3+64*q
     #1Dp1b*q2Dpb**2*mb+128*q1Dp1b*q2Dp1b*pbDp1b*mb+64*q1Dp1b*q2Dp1b*mb*
     #*3)
      s2 = CA/den0x2/den0x3*(-128*q1Dq2*q1Dpb*pbDp1b*mb-64*q1Dq2*q1Dpb*m
     #b**3+128*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1Dq2*q1Dp1b*mb**3+128*q1Dq2*pb
     #Dp1b*mb**3+192*q1Dq2*pbDp1b**2*mb-64*q1Dq2*mb**5-128*q1Dpb*q1Dp1b*
     #q2Dpb*mb+192*q1Dpb*q2Dpb*mb**3-192*q1Dpb*q2Dp1b*pbDp1b*mb-64*q1Dpb
     #*q2Dp1b*mb**3+128*q1Dpb**2*q2Dp1b*mb-192*q1Dp1b*q2Dpb*pbDp1b*mb+64
     #*q1Dp1b*q2Dpb*mb**3-128*q1Dp1b*q2Dp1b*pbDp1b*mb+64*q1Dp1b*q2Dp1b*m
     #b**3)
      t0 = s1+s2
c
      m2_sme_a(39) = t0
c
c ----------m2_sme_a(40) ---------
c
      s1 = CA/den0x1/den0x3*(-128*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+64*q1Dq2*q2D
     #pb*pbDp1b*mb**2+192*q1Dq2*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbD
     #p1b**2-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb*q2Dp1b**2-128*q
     #1Dpb*q2Dp1b**2*pbDp1b-192*q1Dpb*q2Dp1b**2*mb**2+128*q1Dp1b*q2Dpb*q
     #2Dp1b*pbDp1b+192*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*q2D
     #p1b+64*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b+64*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dp1b*q2Dpb
     #*pbDp1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+
     #64*q1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*pbDp1b**2-64*q1Dq2*q2D
     #pb*mb**4-64*q1Dq2**2*pbDp1b*mb**2-128*q1Dq2**2*pbDp1b**2+64*q1Dq2*
     #*2*mb**4+128*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b*pbDp
     #1b-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dpb**2*mb**2-128*q1Dpb*
     #*2*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*q2Dpb**2*pbD
     #p1b+64*q1Dp1b*q2Dpb**2*mb**2)
      t0 = s1+s2
c
      m2_sme_a(40) = t0
c
c ----------m2_sme_a(41) ---------
c
      t0 = CA/den0x1/den0x3*(-256*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*mb
     #**2+576*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+320*q1Dq2*mb**4-256
     #*q1Dpb*q2Dpb*pbDp1b-384*q1Dpb*q2Dpb*mb**2+256*q1Dpb*q2Dp1b*pbDp1b+
     #320*q1Dpb*q2Dp1b*mb**2-256*q1Dp1b*q2Dpb*pbDp1b-448*q1Dp1b*q2Dpb*mb
     #**2+256*q1Dp1b*q2Dpb**2)+CA/den0x2/den0x3*(-128*q1Dq2*q1Dpb*pbDp1b
     #-256*q1Dq2*q1Dpb*mb**2+256*q1Dq2*q1Dp1b*mb**2-320*q1Dq2*pbDp1b*mb*
     #*2-128*q1Dq2*pbDp1b**2-192*q1Dq2*mb**4-384*q1Dpb*q1Dp1b*q2Dpb+512*
     #q1Dpb*q2Dpb*pbDp1b+640*q1Dpb*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*pbDp1b+3
     #20*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1b*q2Dpb*pbDp1b+
     #64*q1Dp1b*q2Dpb*mb**2)
c
      m2_sme_a(41) = t0
c
c ----------m2_sme_a(42) ---------
c
      t0 = CA/den0x1/den0x3*(192*q1Dq2*q2Dpb*mb**3-64*q1Dq2*q2Dp1b*mb**3
     #+64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*mb**5+128*q1Dpb*q2Dpb*q2Dp1b*mb+38
     #4*q1Dpb*q2Dpb*pbDp1b*mb-384*q1Dpb*q2Dpb**2*mb+256*q1Dpb*q2Dp1b*pbD
     #p1b*mb-256*q1Dp1b*q2Dpb*mb**3-128*q1Dp1b*q2Dp1b*mb**3)+CA/den0x2/d
     #en0x3*(-128*q1Dq2*q1Dpb*pbDp1b*mb-192*q1Dq2*q1Dpb*mb**3+192*q1Dq2*
     #q1Dp1b*mb**3-64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*mb**5-256*q1Dpb*q1Dp1b
     #*q2Dpb*mb-384*q1Dpb*q2Dpb*pbDp1b*mb-128*q1Dpb*q2Dpb*mb**3+128*q1Dp
     #b*q2Dp1b*mb**3+384*q1Dpb**2*q2Dpb*mb+128*q1Dpb**2*q2Dp1b*mb+128*q1
     #Dp1b*q2Dpb*mb**3)
c
      m2_sme_a(42) = t0
c
c ----------m2_sme_a(43) ---------
c
      t0 = CA/den0x1/den0x3*(192*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*p
     #bDp1b*mb+64*q1Dq2*pbDp1b*mb**3+192*q1Dq2*pbDp1b**2*mb-128*q1Dq2*mb
     #**5-192*q1Dpb*q2Dpb*q2Dp1b*mb+384*q1Dpb*q2Dpb*mb**3-192*q1Dpb*q2Dp
     #1b*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b
     #*q2Dpb*q2Dp1b*mb-192*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**3-
     #192*q1Dp1b*q2Dpb**2*mb+128*q1Dp1b*q2Dp1b*mb**3)+CA/den0x2/den0x3*(
     #-192*q1Dq2*q1Dpb*pbDp1b*mb-128*q1Dq2*q1Dpb*mb**3+192*q1Dq2*q1Dp1b*
     #pbDp1b*mb-64*q1Dq2*pbDp1b*mb**3-192*q1Dq2*pbDp1b**2*mb+128*q1Dq2*m
     #b**5+192*q1Dpb*q1Dp1b*q2Dpb*mb+64*q1Dpb*q1Dp1b*q2Dp1b*mb-256*q1Dpb
     #*q2Dpb*mb**3-64*q1Dpb*q2Dp1b*pbDp1b*mb-192*q1Dpb*q2Dp1b*mb**3+192*
     #q1Dpb**2*q2Dp1b*mb+192*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**
     #3-192*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_a(43) = t0
c
c ----------m2_sme_a(44) ---------
c
      s1 = CA/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2D
     #pb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**2
     #+64*q1Dq2*q2Dp1b*mb**4-256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b-256*q1Dpb*q2D
     #pb*q2Dp1b*mb**2+768*q1Dpb*q2Dpb**2*q2Dp1b-256*q1Dpb*q2Dpb**2*mb**2
     #+64*q1Dpb*q2Dp1b**2*mb**2+64*q1Dp1b*q2Dpb*q2Dp1b*mb**2-128*q1Dp1b*
     #q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(384*q1Dq2*q1Dpb*q2Dpb*pbDp1b+384*q1Dq2*q1Dp
     #b*q2Dpb*mb**2+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+192*q1Dq2*q1Dp1b*q2Dpb*
     #mb**2-320*q1Dq2*q2Dpb*pbDp1b*mb**2-256*q1Dq2*q2Dpb*pbDp1b**2-64*q1
     #Dq2*q2Dpb*mb**4+128*q1Dq2*q2Dp1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*mb*
     #*4-192*q1Dq2**2*pbDp1b*mb**2-192*q1Dq2**2*mb**4-384*q1Dpb*q1Dp1b*q
     #2Dpb**2+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+64*q1Dpb*q2Dpb*q2Dp1b*mb**2+
     #128*q1Dpb*q2Dpb**2*mb**2-128*q1Dpb*q2Dp1b**2*mb**2-384*q1Dpb**2*q2
     #Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b*mb**2+256*q1Dp1b*q2Dpb**2*pbDp1
     #b+320*q1Dp1b*q2Dpb**2*mb**2)
      t0 = s1+s2
c
      m2_sme_a(44) = t0
c
c ----------m2_sme_a(45) ---------
c
      s1 = CA/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+256*q1Dq2*q2Dpb*pbDp1b**2-128*q1Dq2*q2Dpb*mb**4+6
     #4*q1Dq2*q2Dp1b*pbDp1b*mb**2+64*q1Dq2*q2Dp1b*mb**4-256*q1Dpb*q2Dpb*
     #q2Dp1b*pbDp1b-256*q1Dpb*q2Dpb*q2Dp1b*mb**2+384*q1Dpb*q2Dpb*q2Dp1b*
     #*2+256*q1Dpb*q2Dpb**2*mb**2-64*q1Dpb*q2Dp1b**2*mb**2-64*q1Dp1b*q2D
     #pb*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb**2*q2Dp1b-256*q1Dp1b*q2Dpb**2*pbD
     #p1b-128*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2+512*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+512*q1Dq2*q1Dp1b*q2Dp
     #b*pbDp1b+192*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*mb**
     #2+64*q1Dq2*q2Dpb*pbDp1b*mb**2+64*q1Dq2*q2Dpb*mb**4-256*q1Dq2*q2Dp1
     #b*pbDp1b*mb**2-256*q1Dq2*q2Dp1b*mb**4-192*q1Dq2**2*pbDp1b*mb**2-25
     #6*q1Dq2**2*pbDp1b**2+64*q1Dq2**2*mb**4-256*q1Dpb*q1Dp1b*q2Dpb*q2Dp
     #1b+256*q1Dpb*q2Dpb*q2Dp1b*pbDp1b+448*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*
     #q1Dpb*q2Dpb**2*mb**2+128*q1Dpb*q2Dp1b**2*mb**2-256*q1Dpb**2*q2Dp1b
     #**2+128*q1Dp1b*q2Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb**2*mb**2-256*q1D
     #p1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_a(45) = t0
c
c ----------m2_sme_a(46) ---------
c
      t0 = CA/den0x1/den0x3*(-256*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbD
     #p1b+576*q1Dq2*pbDp1b*mb**2+128*q1Dq2*pbDp1b**2+448*q1Dq2*mb**4-128
     #*q1Dpb*q2Dp1b*pbDp1b-320*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+12
     #8*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*pbDp1b-64*q1Dp1b*q2Dpb*mb**
     #2-128*q1Dp1b*q2Dp1b*mb**2)+CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*mb**2
     #-256*q1Dq2*q1Dp1b*mb**2-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4-256
     #*q1Dpb*q1Dp1b*q2Dp1b+256*q1Dpb*q2Dpb*mb**2+192*q1Dpb*q2Dp1b*mb**2+
     #448*q1Dp1b*q2Dpb*mb**2+256*q1Dp1b*q2Dp1b*pbDp1b+640*q1Dp1b*q2Dp1b*
     #mb**2)
c
      m2_sme_a(46) = t0
c
c ----------m2_sme_a(47) ---------
c
      t0 = CA/den0x1/den0x3*(192*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dp1b*p
     #bDp1b*mb+64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*pbDp1b**2*mb+128*q1Dq2*mb*
     #*5-192*q1Dpb*q2Dpb*q2Dp1b*mb+128*q1Dpb*q2Dpb*mb**3+64*q1Dpb*q2Dp1b
     #*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b*q
     #2Dpb*q2Dp1b*mb+64*q1Dp1b*q2Dpb*pbDp1b*mb+64*q1Dp1b*q2Dpb*mb**3-192
     #*q1Dp1b*q2Dpb**2*mb-128*q1Dp1b*q2Dp1b*mb**3)+CA/den0x2/den0x3*(-64
     #*q1Dq2*q1Dpb*pbDp1b*mb+64*q1Dq2*q1Dp1b*pbDp1b*mb-128*q1Dq2*q1Dp1b*
     #mb**3-64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*pbDp1b**2*mb-128*q1Dq2*mb**5+
     #320*q1Dpb*q1Dp1b*q2Dpb*mb-64*q1Dpb*q1Dp1b*q2Dp1b*mb-64*q1Dpb*q2Dp1
     #b*pbDp1b*mb+64*q1Dpb*q2Dp1b*mb**3+64*q1Dpb**2*q2Dp1b*mb-320*q1Dp1b
     #*q2Dpb*pbDp1b*mb-192*q1Dp1b*q2Dpb*mb**3+256*q1Dp1b*q2Dp1b*mb**3-64
     #*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_a(47) = t0
c
c ----------m2_sme_a(48) ---------
c
      t0 = CA/den0x1/den0x3*(192*q1Dq2*q2Dpb*mb**3-64*q1Dq2*q2Dp1b*mb**3
     #+64*q1Dq2*pbDp1b*mb**3+64*q1Dq2*mb**5-128*q1Dpb*q2Dpb*mb**3-384*q1
     #Dp1b*q2Dpb*q2Dp1b*mb+256*q1Dp1b*q2Dpb*pbDp1b*mb+256*q1Dp1b*q2Dpb*m
     #b**3-128*q1Dp1b*q2Dp1b*pbDp1b*mb+128*q1Dp1b*q2Dp1b**2*mb)+CA/den0x
     #2/den0x3*(-64*q1Dq2*q1Dpb*mb**3-128*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1Dq
     #2*q1Dp1b*mb**3-64*q1Dq2*pbDp1b*mb**3-64*q1Dq2*mb**5+256*q1Dpb*q1Dp
     #1b*q2Dp1b*mb-128*q1Dpb*q2Dp1b*mb**3-128*q1Dp1b*q2Dpb*mb**3+128*q1D
     #p1b*q2Dp1b*pbDp1b*mb-128*q1Dp1b*q2Dp1b*mb**3+128*q1Dp1b**2*q2Dpb*m
     #b-128*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_a(48) = t0
c
c ----------m2_sme_a(49) ---------
c
      s1 = CA/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+128*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**
     #2+128*q1Dq2*q2Dp1b*pbDp1b**2-64*q1Dq2*q2Dp1b*mb**4-128*q1Dpb*q2Dpb
     #*q2Dp1b*mb**2+384*q1Dpb*q2Dpb*q2Dp1b**2-128*q1Dpb*q2Dp1b**2*pbDp1b
     #-64*q1Dpb*q2Dp1b**2*mb**2-128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-320*q1Dp1
     #b*q2Dpb*q2Dp1b*mb**2+384*q1Dp1b*q2Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb**
     #2*mb**2+128*q1Dp1b*q2Dp1b**2*mb**2)
      s2 = CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb**2+256*q1Dq2*q1Dpb
     #*q2Dp1b*pbDp1b+192*q1Dq2*q1Dpb*q2Dp1b*mb**2+256*q1Dq2*q1Dp1b*q2Dpb
     #*pbDp1b+192*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2
     #-320*q1Dq2*q2Dpb*pbDp1b*mb**2-320*q1Dq2*q2Dpb*mb**4+128*q1Dq2*q2Dp
     #1b*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*mb**4-192*q1Dq2**2*pbDp1b*mb**2-1
     #28*q1Dq2**2*pbDp1b**2-64*q1Dq2**2*mb**4-512*q1Dpb*q1Dp1b*q2Dpb*q2D
     #p1b+64*q1Dpb*q2Dpb*q2Dp1b*mb**2-128*q1Dpb*q2Dp1b**2*mb**2-128*q1Dp
     #b**2*q2Dp1b**2+512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+512*q1Dp1b*q2Dpb*q2D
     #p1b*mb**2+64*q1Dp1b*q2Dpb**2*mb**2-256*q1Dp1b*q2Dp1b**2*mb**2-128*
     #q1Dp1b**2*q2Dpb**2)
      t0 = s1+s2
c
      m2_sme_a(49) = t0
c
c ----------m2_sme_a(50) ---------
c
      s1 = CA/den0x1/den0x3*(-384*q1Dq2*q2Dpb*q2Dp1b*mb**2+128*q1Dq2*q2D
     #pb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+64*q1Dq2*q2Dp1b*pbDp1b*mb**2
     #+64*q1Dq2*q2Dp1b*mb**4+128*q1Dpb*q2Dpb*q2Dp1b*mb**2-64*q1Dpb*q2Dp1
     #b**2*mb**2-512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b-320*q1Dp1b*q2Dpb*q2Dp1b*
     #mb**2+768*q1Dp1b*q2Dpb*q2Dp1b**2+128*q1Dp1b*q2Dpb**2*mb**2-128*q1D
     #p1b*q2Dp1b**2*mb**2)
      s2 = CA/den0x2/den0x3*(192*q1Dq2*q1Dpb*q2Dp1b*mb**2+192*q1Dq2*q1Dp
     #1b*q2Dpb*mb**2+384*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*q2D
     #p1b*mb**2+64*q1Dq2*q2Dpb*pbDp1b*mb**2+64*q1Dq2*q2Dpb*mb**4-256*q1D
     #q2*q2Dp1b*pbDp1b*mb**2-128*q1Dq2*q2Dp1b*pbDp1b**2-128*q1Dq2*q2Dp1b
     #*mb**4-192*q1Dq2**2*pbDp1b*mb**2-192*q1Dq2**2*mb**4-384*q1Dpb*q1Dp
     #1b*q2Dp1b**2-64*q1Dpb*q2Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2*pbDp1
     #b+256*q1Dpb*q2Dp1b**2*mb**2+128*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+128*q1D
     #p1b*q2Dpb*q2Dp1b*mb**2-64*q1Dp1b*q2Dpb**2*mb**2+256*q1Dp1b*q2Dp1b*
     #*2*mb**2-384*q1Dp1b**2*q2Dpb*q2Dp1b)
      t0 = s1+s2
c
      m2_sme_a(50) = t0
c
c ----------m2_sme_a(51) ---------
c
      t0 = CA/den0x1/den0x3*(64*q1Dq2*q2Dpb*pbDp1b*mb-64*q1Dq2*q2Dpb*mb*
     #*3-64*q1Dq2*q2Dp1b*pbDp1b*mb-192*q1Dq2*q2Dp1b*mb**3+64*q1Dpb*q2Dpb
     #*q2Dp1b*mb-64*q1Dpb*q2Dp1b**2*mb+64*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q1Dp
     #1b*q2Dpb**2*mb)+CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb+64*q1Dq
     #2*q1Dpb*q2Dp1b*mb+64*q1Dq2*q1Dp1b*q2Dpb*mb+64*q1Dq2*q2Dpb*pbDp1b*m
     #b+64*q1Dq2*q2Dpb*mb**3+64*q1Dq2*q2Dp1b*pbDp1b*mb+64*q1Dq2*q2Dp1b*m
     #b**3-64*q1Dq2**2*pbDp1b*mb-64*q1Dq2**2*mb**3-192*q1Dpb*q2Dpb*q2Dp1
     #b*mb-128*q1Dpb*q2Dpb**2*mb-64*q1Dpb*q2Dp1b**2*mb-64*q1Dp1b*q2Dpb*q
     #2Dp1b*mb-64*q1Dp1b*q2Dpb**2*mb)
c
      m2_sme_a(51) = t0
c
c ----------m2_sme_a(52) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q2Dpb*pbDp1b*mb+64*q1Dq2*q2Dpb*mb
     #**3-192*q1Dq2*q2Dp1b*pbDp1b*mb-64*q1Dq2*q2Dp1b*mb**3+64*q1Dpb*q2Dp
     #b*q2Dp1b*mb+192*q1Dpb*q2Dp1b**2*mb-192*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q
     #1Dp1b*q2Dpb**2*mb)+CA/den0x2/den0x3*(64*q1Dq2*q1Dpb*q2Dp1b*mb+64*q
     #1Dq2*q1Dp1b*q2Dpb*mb+128*q1Dq2*q1Dp1b*q2Dp1b*mb+64*q1Dq2*q2Dpb*pbD
     #p1b*mb+64*q1Dq2*q2Dpb*mb**3+64*q1Dq2*q2Dp1b*pbDp1b*mb+64*q1Dq2*q2D
     #p1b*mb**3-64*q1Dq2**2*pbDp1b*mb-64*q1Dq2**2*mb**3-64*q1Dpb*q2Dpb*q
     #2Dp1b*mb-64*q1Dpb*q2Dp1b**2*mb-192*q1Dp1b*q2Dpb*q2Dp1b*mb-64*q1Dp1
     #b*q2Dpb**2*mb-128*q1Dp1b*q2Dp1b**2*mb)
c
      m2_sme_a(52) = t0
c
c ----------m2_sme_a(53) ---------
c
      t0 = CA/den0x1/den0x3*(384*q1Dq2*q2Dpb*mb-128*q1Dq2*q2Dp1b*mb-384*
     #q1Dq2*pbDp1b*mb-384*q1Dq2*mb**3+256*q1Dpb*q2Dp1b*mb-256*q1Dp1b*q2D
     #pb*mb)+CA/den0x2/den0x3*(-128*q1Dq2*q1Dpb*mb+384*q1Dq2*q1Dp1b*mb+1
     #28*q1Dq2*pbDp1b*mb+128*q1Dq2*mb**3-256*q1Dpb*q2Dpb*mb-512*q1Dp1b*q
     #2Dpb*mb-256*q1Dp1b*q2Dp1b*mb)
c
      m2_sme_a(53) = t0
c
c ----------m2_sme_a(54) ---------
c
      t0 = CA/den0x1/den0x3*(-768*q1Dq2*q2Dpb*q2Dp1b+128*q1Dq2*q2Dpb*pbD
     #p1b+384*q1Dq2*q2Dpb*mb**2+512*q1Dq2*q2Dp1b*pbDp1b+768*q1Dq2*q2Dp1b
     #*mb**2+128*q1Dpb*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb**2)+CA/den0x2/den0x
     #3*(384*q1Dq2*q1Dpb*q2Dp1b+384*q1Dq2*q1Dp1b*q2Dpb+256*q1Dq2*q2Dpb*p
     #bDp1b+256*q1Dq2*q2Dpb*mb**2+128*q1Dq2*q2Dp1b*pbDp1b+128*q1Dq2*q2Dp
     #1b*mb**2-384*q1Dq2**2*pbDp1b-384*q1Dq2**2*mb**2+256*q1Dpb*q2Dpb*q2
     #Dp1b-128*q1Dpb*q2Dp1b**2+128*q1Dp1b*q2Dpb*q2Dp1b-256*q1Dp1b*q2Dpb*
     #*2)
c
      m2_sme_a(54) = t0
c
c ----------m2_sme_a(55) ---------
c
      t0 = CA/den0x1/den0x3*(-512*q1Dq2*q2Dpb*pbDp1b-128*q1Dq2*q2Dp1b*mb
     #**2-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4+768*q1Dpb*q2Dpb*q2Dp1b-
     #512*q1Dpb*q2Dpb*mb**2-256*q1Dpb*q2Dp1b*pbDp1b-448*q1Dpb*q2Dp1b*mb*
     #*2-192*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*mb
     #**2)+CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*pbDp1b+512*q1Dq2*q1Dp1b*mb*
     #*2+320*q1Dq2*pbDp1b*mb**2+320*q1Dq2*mb**4-1152*q1Dpb*q1Dp1b*q2Dpb+
     #128*q1Dpb*q2Dpb*mb**2-192*q1Dpb*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b+1
     #024*q1Dp1b*q2Dpb*pbDp1b+832*q1Dp1b*q2Dpb*mb**2-512*q1Dp1b*q2Dp1b*m
     #b**2)
c
      m2_sme_a(55) = t0
c
c ----------m2_sme_a(56) ---------
c
      t0 = CA/den0x1/den0x3*(-512*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbD
     #p1b-320*q1Dq2*pbDp1b*mb**2-320*q1Dq2*mb**4+512*q1Dpb*q2Dpb*mb**2+1
     #92*q1Dpb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2+1152*q1Dp1b*q2Dpb*q2Dp1b
     #-1024*q1Dp1b*q2Dpb*pbDp1b-832*q1Dp1b*q2Dpb*mb**2-128*q1Dp1b*q2Dp1b
     #*mb**2)+CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*mb**2+512*q1Dq2*q1Dp1b*p
     #bDp1b+320*q1Dq2*pbDp1b*mb**2+320*q1Dq2*mb**4-768*q1Dpb*q1Dp1b*q2Dp
     #1b-128*q1Dpb*q2Dpb*mb**2+256*q1Dpb*q2Dp1b*pbDp1b+448*q1Dpb*q2Dp1b*
     #mb**2+192*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dp1b*mb**2-512*q1Dp1b**2
     #*q2Dpb)
c
      m2_sme_a(56) = t0
c
c ----------m2_sme_a(57) ---------
c
      s1 = CA/den0x1/den0x3*(384*q1Dq2*q2Dpb*pbDp1b*mb-128*q1Dq2*q2Dp1b*
     #pbDp1b*mb-384*q1Dq2*pbDp1b*mb**3-640*q1Dq2*pbDp1b**2*mb+256*q1Dq2*
     #mb**5-384*q1Dpb*q2Dpb*q2Dp1b*mb+640*q1Dpb*q2Dp1b*pbDp1b*mb+384*q1D
     #pb*q2Dp1b*mb**3+128*q1Dpb*q2Dp1b**2*mb-128*q1Dp1b*q2Dpb*q2Dp1b*mb-
     #640*q1Dp1b*q2Dpb*pbDp1b*mb-384*q1Dp1b*q2Dpb*mb**3+384*q1Dp1b*q2Dpb
     #**2*mb)
      s2 = CA/den0x2/den0x3*(-384*q1Dq2*q1Dpb*pbDp1b*mb-256*q1Dq2*q1Dpb*
     #mb**3+640*q1Dq2*q1Dp1b*pbDp1b*mb+256*q1Dq2*q1Dp1b*mb**3+128*q1Dq2*
     #pbDp1b*mb**3+384*q1Dq2*pbDp1b**2*mb-256*q1Dq2*mb**5-384*q1Dpb*q1Dp
     #1b*q2Dpb*mb-128*q1Dpb*q1Dp1b*q2Dp1b*mb+768*q1Dpb*q2Dpb*mb**3-384*q
     #1Dpb*q2Dp1b*pbDp1b*mb+128*q1Dpb*q2Dp1b*mb**3+384*q1Dpb**2*q2Dp1b*m
     #b-1152*q1Dp1b*q2Dpb*pbDp1b*mb-128*q1Dp1b*q2Dpb*mb**3-512*q1Dp1b*q2
     #Dp1b*pbDp1b*mb+256*q1Dp1b*q2Dp1b*mb**3+128*q1Dp1b**2*q2Dpb*mb)
      t0 = s1+s2
c
      m2_sme_a(57) = t0
c
c ----------m2_sme_a(58) ---------
c
      s1 = CA/den0x1/den0x3*(-768*q1Dq2*q2Dpb*q2Dp1b*pbDp1b+384*q1Dq2*q2
     #Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dpb*mb**4+768*q1Dq2*q2Dp1b*pbDp1b*mb*
     #*2+512*q1Dq2*q2Dp1b*pbDp1b**2-384*q1Dpb*q2Dpb*q2Dp1b*mb**2+768*q1D
     #pb*q2Dpb*q2Dp1b**2-512*q1Dpb*q2Dp1b**2*pbDp1b-768*q1Dpb*q2Dp1b**2*
     #mb**2+512*q1Dp1b*q2Dpb*q2Dp1b*pbDp1b+768*q1Dp1b*q2Dpb*q2Dp1b*mb**2
     #-768*q1Dp1b*q2Dpb**2*q2Dp1b+384*q1Dp1b*q2Dpb**2*mb**2)
      s2 = CA/den0x2/den0x3*(-512*q1Dq2**2*pbDp1b**2+512*q1Dp1b*q2Dpb*q2
     #Dp1b*pbDp1b+640*q1Dp1b*q2Dpb*q2Dp1b*mb**2-512*q1Dpb*q2Dpb*q2Dp1b*p
     #bDp1b-256*q1Dq2*q1Dpb*q2Dpb*mb**2+384*q1Dq2*q1Dpb*q2Dp1b*mb**2-384
     #*q1Dq2*q1Dp1b*q2Dpb*mb**2+1024*q1Dq2*q1Dpb*q2Dp1b*pbDp1b+256*q1Dq2
     #*q1Dp1b*q2Dpb*pbDp1b-256*q1Dq2*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb*q1Dp1
     #b*q2Dpb*q2Dp1b+256*q1Dp1b**2*q2Dpb**2-512*q1Dpb**2*q2Dp1b**2+256*q
     #1Dq2*q2Dpb*pbDp1b*mb**2+128*q1Dq2*q2Dp1b*pbDp1b*mb**2-512*q1Dpb*q2
     #Dpb*q2Dp1b*mb**2+128*q1Dq2**2*mb**4+512*q1Dq2*q2Dpb*pbDp1b**2+512*
     #q1Dpb*q2Dpb**2*mb**2-256*q1Dp1b*q2Dp1b**2*mb**2-256*q1Dq2*q2Dpb*mb
     #**4-128*q1Dpb*q2Dp1b**2*mb**2+256*q1Dp1b*q2Dpb**2*mb**2+128*q1Dq2*
     #q2Dp1b*mb**4-384*q1Dq2**2*pbDp1b*mb**2-512*q1Dp1b*q2Dpb**2*pbDp1b)
      t0 = s1+s2
c
      m2_sme_a(58) = t0
c
c ----------m2_sme_a(59) ---------
c
      t0 = CA/den0x1/den0x3*(-1024*q1Dq2*q2Dpb*pbDp1b-256*q1Dq2*q2Dp1b*m
     #b**2+1920*q1Dq2*pbDp1b*mb**2+1024*q1Dq2*pbDp1b**2+896*q1Dq2*mb**4-
     #1024*q1Dpb*q2Dpb*q2Dp1b+1024*q1Dpb*q2Dp1b*pbDp1b+1664*q1Dpb*q2Dp1b
     #*mb**2-1024*q1Dp1b*q2Dpb*pbDp1b-1664*q1Dp1b*q2Dpb*mb**2+1024*q1Dp1
     #b*q2Dpb**2)+CA/den0x2/den0x3*(-1024*q1Dq2*q1Dpb*pbDp1b-1280*q1Dq2*
     #q1Dpb*mb**2+1024*q1Dq2*q1Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-640*q1D
     #q2*mb**4-1024*q1Dpb*q1Dp1b*q2Dpb+2048*q1Dpb*q2Dpb*pbDp1b+2304*q1Dp
     #b*q2Dpb*mb**2+896*q1Dpb*q2Dp1b*mb**2+1024*q1Dpb**2*q2Dp1b-384*q1Dp
     #1b*q2Dpb*mb**2+256*q1Dp1b*q2Dp1b*mb**2)
c
      m2_sme_a(59) = t0
c
c ----------m2_sme_a(60) ---------
c
      t0 = CA/den0x1/den0x3*(-1024*q1Dq2*q2Dpb*mb**2-256*q1Dq2*q2Dp1b*pb
     #Dp1b+1920*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+1664*q1Dq2*mb**4-
     #256*q1Dpb*q2Dp1b*pbDp1b-896*q1Dpb*q2Dp1b*mb**2+256*q1Dpb*q2Dp1b**2
     #-256*q1Dp1b*q2Dpb*q2Dp1b+256*q1Dp1b*q2Dpb*pbDp1b+896*q1Dp1b*q2Dpb*
     #mb**2)+CA/den0x2/den0x3*(256*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*pb
     #Dp1b-1280*q1Dq2*q1Dp1b*mb**2-640*q1Dq2*pbDp1b*mb**2-640*q1Dq2*mb**
     #4-256*q1Dpb*q1Dp1b*q2Dp1b+1024*q1Dpb*q2Dpb*mb**2+384*q1Dpb*q2Dp1b*
     #mb**2+1664*q1Dp1b*q2Dpb*mb**2+512*q1Dp1b*q2Dp1b*pbDp1b+1536*q1Dp1b
     #*q2Dp1b*mb**2+256*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(60) = t0
c
c ----------m2_sme_a(61) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*mb-32*q1Dq2*q1Dpb*q2D
     #p1b*mb-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp
     #1b*q2Dpb*mb-32*q1Dq2*q1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q1D
     #q2**2*pbDp1b*mb+32*q1Dq2**2*mb**3+96*q1Dpb*q1Dp1b*q2Dpb*mb+32*q1Dp
     #b*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dpb*mb+32*q1Dpb**2*q2Dp1b*mb+32*q
     #1Dp1b**2*q2Dpb*mb)+CA/den0x2/den0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb+32*
     #q1Dq2*q1Dpb*mb**3+32*q1Dq2*q1Dp1b*pbDp1b*mb+96*q1Dq2*q1Dp1b*mb**3-
     #160*q1Dpb*q1Dp1b*q2Dpb*mb-32*q1Dpb*q1Dp1b*q2Dp1b*mb-64*q1Dpb**2*q2
     #Dpb*mb+32*q1Dpb**2*q2Dp1b*mb-32*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_a(61) = t0
c
c ----------m2_sme_a(62) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb-32*q1Dq2*q1Dpb*pb
     #Dp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp1b*q2Dpb*mb-64*q1Dq2*q1D
     #p1b*q2Dp1b*mb-32*q1Dq2*q1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q
     #1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3+32*q1Dpb*q1Dp1b*q2Dpb*mb+96*q1
     #Dpb*q1Dp1b*q2Dp1b*mb+32*q1Dpb**2*q2Dp1b*mb+32*q1Dp1b**2*q2Dpb*mb+6
     #4*q1Dp1b**2*q2Dp1b*mb)+CA/den0x2/den0x3*(32*q1Dq2*q1Dpb*pbDp1b*mb-
     #32*q1Dq2*q1Dpb*mb**3+96*q1Dq2*q1Dp1b*pbDp1b*mb+32*q1Dq2*q1Dp1b*mb*
     #*3-32*q1Dpb*q1Dp1b*q2Dpb*mb-32*q1Dpb*q1Dp1b*q2Dp1b*mb-32*q1Dpb**2*
     #q2Dp1b*mb-96*q1Dp1b**2*q2Dpb*mb-64*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_a(62) = t0
c
c ----------m2_sme_a(63) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dpb*mb-32*q1Dq2*q1Dpb*q2D
     #p1b*mb+64*q1Dq2*q1Dpb*pbDp1b*mb+64*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp
     #1b*q2Dpb*mb+32*q1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3)+CA/den0x2/den
     #0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q1Dp1
     #b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3-32*q1Dpb*q1Dp1b*q2Dpb*mb+32*q1Dp
     #b*q1Dp1b*q2Dp1b*mb+32*q1Dpb**2*q2Dp1b*mb-32*q1Dp1b**2*q2Dpb*mb)
c
      m2_sme_a(63) = t0
c
c ----------m2_sme_a(64) ---------
c
      t0 = CA/den0x1/den0x3*(-32*q1Dq2*q1Dpb*q2Dp1b*mb-32*q1Dq2*q1Dp1b*q
     #2Dpb*mb-64*q1Dq2*q1Dp1b*q2Dp1b*mb+64*q1Dq2*q1Dp1b*pbDp1b*mb+64*q1D
     #q2*q1Dp1b*mb**3+32*q1Dq2**2*pbDp1b*mb+32*q1Dq2**2*mb**3)+CA/den0x2
     #/den0x3*(-32*q1Dq2*q1Dpb*pbDp1b*mb-32*q1Dq2*q1Dpb*mb**3-32*q1Dq2*q
     #1Dp1b*pbDp1b*mb-32*q1Dq2*q1Dp1b*mb**3+32*q1Dpb*q1Dp1b*q2Dpb*mb-32*
     #q1Dpb*q1Dp1b*q2Dp1b*mb-32*q1Dpb**2*q2Dp1b*mb+32*q1Dp1b**2*q2Dpb*mb
     #)
c
      m2_sme_a(64) = t0
c
c ----------m2_sme_a(65) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b-64*q1Dq2*q1Dpb*pbDp1
     #b-64*q1Dq2*q1Dpb*mb**2-64*q1Dq2*q1Dp1b*q2Dpb+64*q1Dq2**2*pbDp1b+64
     #*q1Dq2**2*mb**2-64*q1Dpb*q1Dp1b*q2Dpb+64*q1Dpb**2*q2Dp1b)+CA/den0x
     #2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b-64*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1
     #Dp1b*pbDp1b-192*q1Dq2*q1Dp1b*mb**2)
c
      m2_sme_a(65) = t0
c
c ----------m2_sme_a(66) ---------
c
      s1 = CA/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1D
     #pb*q2Dp1b*mb**2+96*q1Dq2*q1Dpb*pbDp1b*mb**2+96*q1Dq2*q1Dpb*mb**4+6
     #4*q1Dq2*q1Dp1b*q2Dpb*mb**2-160*q1Dq2*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*
     #q1Dp1b*pbDp1b**2-96*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64
     #*q1Dq2**2*pbDp1b**2+32*q1Dpb*q1Dp1b*q2Dpb*mb**2-64*q1Dpb*q1Dp1b*q2
     #Dp1b*pbDp1b-32*q1Dpb*q1Dp1b*q2Dp1b*mb**2-32*q1Dpb**2*q2Dp1b*mb**2+
     #64*q1Dpb**2*q2Dp1b**2+64*q1Dp1b**2*q2Dpb*pbDp1b+32*q1Dp1b**2*q2Dpb
     #*mb**2-64*q1Dp1b**2*q2Dpb**2)
      s2 = CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-32*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-32*q1Dq2*q1Dpb*mb**4-32*q1Dq2*q1Dp1b*pbDp1b*mb**2-6
     #4*q1Dq2*q1Dp1b*pbDp1b**2+32*q1Dq2*q1Dp1b*mb**4-128*q1Dpb*q1Dp1b*q2
     #Dpb*pbDp1b-96*q1Dpb*q1Dp1b*q2Dpb*mb**2+64*q1Dpb*q1Dp1b*q2Dp1b*pbDp
     #1b+96*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b**2*q2Dpb-128*q1Dp
     #b**2*q1Dp1b*q2Dp1b+64*q1Dpb**2*q2Dpb*mb**2+32*q1Dpb**2*q2Dp1b*mb**
     #2+64*q1Dp1b**2*q2Dpb*pbDp1b-32*q1Dp1b**2*q2Dpb*mb**2-64*q1Dp1b**2*
     #q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(66) = t0
c
c ----------m2_sme_a(67) ---------
c
      s1 = CA/den0x1/den0x3*(-256*q1Dq2*q1Dpb*q2Dp1b*pbDp1b-64*q1Dq2*q1D
     #pb*q2Dp1b*mb**2+256*q1Dq2*q1Dpb*pbDp1b*mb**2+256*q1Dq2*q1Dpb*mb**4
     #+64*q1Dq2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2-320*q1D
     #q2*q1Dp1b*pbDp1b*mb**2-128*q1Dq2*q1Dp1b*pbDp1b**2-192*q1Dq2*q1Dp1b
     #*mb**4+64*q1Dq2**2*pbDp1b*mb**2+128*q1Dq2**2*pbDp1b**2-64*q1Dq2**2
     #*mb**4+128*q1Dpb*q1Dp1b*q2Dpb*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b
     #-192*q1Dpb*q1Dp1b*q2Dp1b*mb**2-128*q1Dpb**2*q2Dp1b*mb**2+128*q1Dpb
     #**2*q2Dp1b**2+128*q1Dp1b**2*q2Dpb*pbDp1b+192*q1Dp1b**2*q2Dpb*mb**2
     #-128*q1Dp1b**2*q2Dpb**2)
      s2 = CA/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-128*q1Dq2*q1D
     #pb*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4-128*q1Dq2*q1Dp1b*pbDp1b*mb**2
     #-128*q1Dq2*q1Dp1b*pbDp1b**2+64*q1Dq2*q1Dp1b*mb**4-128*q1Dq2*q1Dp1b
     #**2*mb**2-256*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*q2Dpb*mb*
     #*2+128*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2+25
     #6*q1Dpb*q1Dp1b**2*q2Dpb-256*q1Dpb**2*q1Dp1b*q2Dp1b+128*q1Dpb**2*q2
     #Dpb*mb**2+128*q1Dpb**2*q2Dp1b*mb**2-128*q1Dp1b**2*q2Dpb*pbDp1b-128
     #*q1Dp1b**2*q2Dpb*mb**2)
      t0 = s1+s2
c
      m2_sme_a(67) = t0
c
c ----------m2_sme_a(68) ---------
c
      t0 = CA/den0x1/den0x3*(-384*q1Dq2*q1Dpb*q2Dpb*pbDp1b-128*q1Dq2*q1D
     #pb*q2Dpb*mb**2-64*q1Dq2*q1Dpb*q2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*
     #*2-128*q1Dq2*q1Dpb*mb**4-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-64*q1Dq2*q1Dp
     #1b*pbDp1b*mb**2-64*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64*
     #q1Dq2**2*mb**4-128*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-128*q1Dpb*q1Dp1b*q2Dp
     #b*mb**2+384*q1Dpb*q1Dp1b*q2Dpb**2+64*q1Dpb*q1Dp1b*q2Dp1b*mb**2+384
     #*q1Dpb**2*q2Dpb*q2Dp1b-256*q1Dpb**2*q2Dpb*mb**2-128*q1Dpb**2*q2Dp1
     #b*pbDp1b-256*q1Dpb**2*q2Dp1b*mb**2+64*q1Dp1b**2*q2Dpb*mb**2)+CA/de
     #n0x2/den0x3*(384*q1Dq2*q1Dpb*q1Dp1b*mb**2+64*q1Dq2*q1Dpb*pbDp1b*mb
     #**2+64*q1Dq2*q1Dpb*mb**4+512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+448*q1Dpb*q
     #1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-768*q1Dpb**2*q1Dp1
     #b*q2Dpb+128*q1Dpb**2*q2Dpb*mb**2-64*q1Dpb**2*q2Dp1b*mb**2)
c
      m2_sme_a(68) = t0
c
c ----------m2_sme_a(69) ---------
c
      s1 = CA/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2-256*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b-64*q1Dq2*q1Dpb*q2Dp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b
     #*mb**2-128*q1Dq2*q1Dpb*mb**4-256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-64*q1Dq
     #2*q1Dp1b*q2Dpb*mb**2+128*q1Dq2*q1Dp1b*q2Dp1b*mb**2+64*q1Dq2*q1Dp1b
     #*pbDp1b*mb**2+64*q1Dq2*q1Dp1b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+128*q
     #1Dq2**2*pbDp1b**2-64*q1Dq2**2*mb**4+512*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b-
     #512*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-512*q1Dpb*q1Dp1b*q2Dpb*mb**2-64*q1Dp
     #b*q1Dp1b*q2Dp1b*mb**2+256*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**2*q2Dp1b
     #*mb**2+128*q1Dpb**2*q2Dp1b**2-64*q1Dp1b**2*q2Dpb*mb**2+128*q1Dp1b*
     #*2*q2Dpb**2)
      s2 = CA/den0x2/den0x3*(384*q1Dq2*q1Dpb*q1Dp1b*pbDp1b+64*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-128*q1Dq2*q1Dpb*pbDp1b**2+192*q1Dq2*q1Dpb*mb**4+128
     #*q1Dpb*q1Dp1b*q2Dpb*pbDp1b+192*q1Dpb*q1Dp1b*q2Dpb*mb**2+256*q1Dpb*
     #q1Dp1b*q2Dp1b*mb**2-384*q1Dpb*q1Dp1b**2*q2Dpb-384*q1Dpb**2*q1Dp1b*
     #q2Dp1b-128*q1Dpb**2*q2Dpb*mb**2+128*q1Dpb**2*q2Dp1b*pbDp1b+192*q1D
     #pb**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(69) = t0
c
c ----------m2_sme_a(70) ---------
c
      s1 = CA/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dpb*mb**2-64*q1Dq2*q1Dpb
     #*q2Dp1b*mb**2+128*q1Dq2*q1Dpb*pbDp1b*mb**2+128*q1Dq2*q1Dpb*mb**4-2
     #56*q1Dq2*q1Dp1b*q2Dpb*pbDp1b-64*q1Dq2*q1Dp1b*q2Dpb*mb**2-128*q1Dq2
     #*q1Dp1b*q2Dp1b*mb**2-192*q1Dq2*q1Dp1b*pbDp1b*mb**2-192*q1Dq2*q1Dp1
     #b*mb**4+64*q1Dq2**2*pbDp1b*mb**2+64*q1Dq2**2*mb**4+512*q1Dpb*q1Dp1
     #b*q2Dpb*q2Dp1b-256*q1Dpb*q1Dp1b*q2Dpb*mb**2-256*q1Dpb*q1Dp1b*q2Dp1
     #b*pbDp1b-320*q1Dpb*q1Dp1b*q2Dp1b*mb**2-64*q1Dp1b**2*q2Dpb*mb**2+25
     #6*q1Dp1b**2*q2Dpb**2+128*q1Dp1b**2*q2Dp1b*mb**2)
      s2 = CA/den0x2/den0x3*(128*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-64*q1Dq2*q1Dp
     #b*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4+128*q1Dq2*q1Dp1b*pbDp1b*mb**2+
     #128*q1Dq2*q1Dp1b*mb**4+256*q1Dq2*q1Dp1b**2*mb**2+64*q1Dpb*q1Dp1b*q
     #2Dpb*mb**2-640*q1Dpb*q1Dp1b**2*q2Dpb-128*q1Dpb**2*q1Dp1b*q2Dp1b+64
     #*q1Dpb**2*q2Dp1b*mb**2+512*q1Dp1b**2*q2Dpb*pbDp1b+384*q1Dp1b**2*q2
     #Dpb*mb**2-256*q1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(70) = t0
c
c ----------m2_sme_a(71) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*mb**2-320*q1Dq2*q1Dp
     #1b*q2Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*q2D
     #p1b*mb**2-64*q1Dq2*q1Dp1b*pbDp1b*mb**2-64*q1Dq2*q1Dp1b*mb**4+64*q1
     #Dq2**2*pbDp1b*mb**2+64*q1Dq2**2*mb**4+256*q1Dpb*q1Dp1b*q2Dpb*mb**2
     #+64*q1Dpb*q1Dp1b*q2Dp1b*mb**2+128*q1Dpb*q1Dp1b*q2Dp1b**2+640*q1Dp1
     #b**2*q2Dpb*q2Dp1b-512*q1Dp1b**2*q2Dpb*pbDp1b-448*q1Dp1b**2*q2Dpb*m
     #b**2-128*q1Dp1b**2*q2Dp1b*mb**2)+CA/den0x2/den0x3*(128*q1Dq2*q1Dpb
     #*q1Dp1b*mb**2-64*q1Dq2*q1Dpb*pbDp1b*mb**2-64*q1Dq2*q1Dpb*mb**4+128
     #*q1Dq2*q1Dp1b*pbDp1b*mb**2+128*q1Dq2*q1Dp1b*mb**4+256*q1Dq2*q1Dp1b
     #**2*pbDp1b-64*q1Dpb*q1Dp1b*q2Dpb*mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbD
     #p1b+256*q1Dpb*q1Dp1b*q2Dp1b*mb**2-512*q1Dpb*q1Dp1b**2*q2Dp1b-64*q1
     #Dpb**2*q2Dp1b*mb**2+128*q1Dp1b**2*q2Dpb*mb**2+256*q1Dp1b**2*q2Dp1b
     #*mb**2-256*q1Dp1b**3*q2Dpb)
c
      m2_sme_a(71) = t0
c
c ----------m2_sme_a(72) ---------
c
      t0 = CA/den0x1/den0x3*(128*q1Dq2*q1Dpb*q2Dpb*mb-64*q1Dq2*q1Dpb*q2D
     #p1b*mb-192*q1Dq2*q1Dpb*pbDp1b*mb-192*q1Dq2*q1Dpb*mb**3-64*q1Dq2*q1
     #Dp1b*q2Dpb*mb-64*q1Dq2*q1Dp1b*pbDp1b*mb-64*q1Dq2*q1Dp1b*mb**3+64*q
     #1Dq2**2*pbDp1b*mb+64*q1Dq2**2*mb**3-64*q1Dpb*q1Dp1b*q2Dpb*mb-64*q1
     #Dpb*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dp1b*mb+64*q1Dp1b**2*q2Dpb*mb)+
     #CA/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b*mb-256*q1Dpb*q1Dp1b*q2Dpb
     #*mb-128*q1Dpb*q1Dp1b*q2Dp1b*mb-128*q1Dpb**2*q2Dpb*mb)
c
      m2_sme_a(72) = t0
c
c ----------m2_sme_a(73) ---------
c
      t0 = CA/den0x1/den0x3*(-64*q1Dq2*q1Dpb*q2Dp1b*mb-64*q1Dq2*q1Dpb*pb
     #Dp1b*mb-64*q1Dq2*q1Dpb*mb**3+192*q1Dq2*q1Dp1b*q2Dpb*mb-128*q1Dq2*q
     #1Dp1b*q2Dp1b*mb-192*q1Dq2*q1Dp1b*pbDp1b*mb-192*q1Dq2*q1Dp1b*mb**3+
     #64*q1Dq2**2*pbDp1b*mb+64*q1Dq2**2*mb**3-64*q1Dpb*q1Dp1b*q2Dpb*mb+1
     #92*q1Dpb*q1Dp1b*q2Dp1b*mb+64*q1Dpb**2*q2Dp1b*mb-192*q1Dp1b**2*q2Dp
     #b*mb)+CA/den0x2/den0x3*(256*q1Dq2*q1Dp1b**2*mb-128*q1Dpb*q1Dp1b*q2
     #Dpb*mb-256*q1Dp1b**2*q2Dpb*mb-128*q1Dp1b**2*q2Dp1b*mb)
c
      m2_sme_a(73) = t0
c
c ----------m2_sme_a(74) ---------
c
      t0 = CA/den0x1/den0x3*(-128*q1Dq2*q1Dpb*q2Dp1b-128*q1Dq2*q1Dpb*pbD
     #p1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*q2Dpb+128*q1Dq2**2*pbDp
     #1b+128*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb+128*q1Dpb**2*q2Dp1b)+
     #CA/den0x2/den0x3*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q1Dp1b*pbDp1b-1
     #28*q1Dq2*q1Dp1b*mb**2-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dp1b**2*q2Dpb)
c
      m2_sme_a(74) = t0
c
c ----------m2_sme_a(75) ---------
c
      s1 = CA/den0x1/den0x3*(-256*q1Dq2*q1Dpb*q2Dpb*mb**2-512*q1Dq2*q1Dp
     #b*q2Dp1b*pbDp1b-128*q1Dq2*q1Dpb*q2Dp1b*mb**2+896*q1Dq2*q1Dpb*pbDp1
     #b*mb**2+896*q1Dq2*q1Dpb*mb**4+256*q1Dq2*q1Dp1b*q2Dpb*pbDp1b+128*q1
     #Dq2*q1Dp1b*q2Dpb*mb**2+256*q1Dq2*q1Dp1b*q2Dp1b*mb**2-1024*q1Dq2*q1
     #Dp1b*pbDp1b*mb**2-512*q1Dq2*q1Dp1b*pbDp1b**2-512*q1Dq2*q1Dp1b*mb**
     #4+128*q1Dq2**2*pbDp1b*mb**2+256*q1Dq2**2*pbDp1b**2-128*q1Dq2**2*mb
     #**4+256*q1Dpb*q1Dp1b*q2Dpb*q2Dp1b+384*q1Dpb*q1Dp1b*q2Dpb*mb**2-512
     #*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b-768*q1Dpb*q1Dp1b*q2Dp1b*mb**2-384*q1Dp
     #b**2*q2Dp1b*mb**2+256*q1Dpb**2*q2Dp1b**2+512*q1Dp1b**2*q2Dpb*pbDp1
     #b+768*q1Dp1b**2*q2Dpb*mb**2-512*q1Dp1b**2*q2Dpb**2)
      s2 = CA/den0x2/den0x3*(768*q1Dq2*q1Dpb*q1Dp1b*pbDp1b-256*q1Dq2*q1D
     #pb*pbDp1b*mb**2-256*q1Dq2*q1Dpb*mb**4+128*q1Dq2*q1Dp1b*pbDp1b*mb**
     #2-256*q1Dq2*q1Dp1b*pbDp1b**2+384*q1Dq2*q1Dp1b*mb**4-512*q1Dq2*q1Dp
     #1b**2*mb**2-1024*q1Dpb*q1Dp1b*q2Dpb*pbDp1b-512*q1Dpb*q1Dp1b*q2Dpb*
     #mb**2+256*q1Dpb*q1Dp1b*q2Dp1b*pbDp1b+384*q1Dpb*q1Dp1b*q2Dp1b*mb**2
     #+768*q1Dpb*q1Dp1b**2*q2Dpb-768*q1Dpb**2*q1Dp1b*q2Dp1b+512*q1Dpb**2
     #*q2Dpb*mb**2+256*q1Dpb**2*q2Dp1b*mb**2+256*q1Dp1b**2*q2Dpb*pbDp1b+
     #128*q1Dp1b**2*q2Dpb*mb**2-256*q1Dp1b**2*q2Dp1b*mb**2)
      t0 = s1+s2
c
      m2_sme_a(75) = t0
c
      return
      end
      subroutine wbb_d1_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -  D0,C0234,C0134,C0124,C0123,D1_1,D1_2,D1_3)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer dflag
      real*8 m1,m2,m3,m4,p1(4),p2(4),p3(4),f1,f2,f3
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p2Dp2,p2Dp3,p3Dp3,wbb_gramdet
      real*8 C0234,C0134,C0124,C0123,D0
      real*8 I1,I2,I3,D1_1,D1_2,D1_3
      common/wbb_gramdet/dflag
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p3dp3=wbb_dot(p3,p3)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
c
      I1=1d0/2d0*(C0134-C0234+f1*D0)
      I2=1d0/2d0*(C0124-C0134+f2*D0)
      I3=1d0/2d0*(C0123-C0124+f3*D0)
c Gram Determinant
      wbb_gramdet = 
     #p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+
     #2.D0*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2
      if (dabs(wbb_gramdet).lt.1D-01) then
         dflag=1
         return
      endif
c D1_1
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I1+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I2+
     #(p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I3
      D1_1= t0/wbb_gramdet
c D1_2
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I1+(p1Dp1*p3Dp3-p1Dp3**2)*I2+
     #(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I3
      D1_2=t0/wbb_gramdet
c D1_3
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I1+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I2+
     #(p1Dp1*p2Dp2-p1Dp2**2)*I3
      D1_3=t0/wbb_gramdet
c
      return
      end
c
c
c
      subroutine wbb_d2_ir_new(p1,p2,p3,m1,m2,m3,m4,
     -  D0,A1d1,A2d1,A3d1,
     -  C0234,A1c1_234,A2c1_234,C0134,A1c1_134,A2c1_134,
     -  C0124,A1c1_124,A2c1_124,C0123,A1c1_123,A2c1_123,
     -  D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer dflag
      real*8 m1,m2,m3,m4,p1(4),p2(4),p3(4),f1,f2,f3
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p2Dp2,p2Dp3,p3Dp3,wbb_gramdet
      real*8 C0234,C0134,C0124,C0123,D0
      real*8 A1c1_234,A2c1_234,A1c1_134,A2c1_134,
     -   A1c1_124,A2c1_124,A1c1_123,A2c1_123,A1d1,A2d1,A3d1
      real*8 I0,I11,I12,I13,I21,I22,I23,I31,I32,I33,
     -   D2_0,D2_11,D2_22,D2_33,D2_12,D2_13,D2_23
      common/wbb_gramdet/dflag
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p3dp3=wbb_dot(p3,p3)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
c
      I0=C0234+m1**2*D0
      I11=1d0/2d0*(A1c1_134+C0234+f1*A1d1)
      I12=1d0/2d0*(A1c1_134-A1c1_234+f1*A2d1)
      I13=1d0/2d0*(A2c1_134-A2c1_234+f1*A3d1)
      I21=1d0/2d0*(A1c1_124-A1c1_134+f2*A1d1)
      I22=1d0/2d0*(A2c1_124-A1c1_134+f2*A2d1)
      I23=1d0/2d0*(A2c1_124-A2c1_134+f2*A3d1)
      I31=1d0/2d0*(A1c1_123-A1c1_124+f3*A1d1)
      I32=1d0/2d0*(A2c1_123-A2c1_124+f3*A2d1)
      I33=1d0/2d0*(-A2c1_124+f3*A3d1)
c
      D2_0=I0-I11-I22-I33
      I11=I11-D2_0
      I22=I22-D2_0
      I33=I33-D2_0
c
c Gram Determinant
      wbb_gramdet = 
     #p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+
     #2.D0*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2
      if (dabs(wbb_gramdet).lt.1D-01) then
         dflag=1
         return
      endif
c D2_11
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I11+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I21+(p1
     #Dp2*p2Dp3-p1Dp3*p2Dp2)*I31
      D2_11=t0/wbb_gramdet
c D2_22
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I12+(p1Dp1*p3Dp3-p1Dp3**2)*I22+(-p
     #1Dp1*p2Dp3+p1Dp2*p1Dp3)*I32
      D2_22=t0/wbb_gramdet
c D2_33
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I13+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I23+
     #(p1Dp1*p2Dp2-p1Dp2**2)*I33
      D2_33=t0/wbb_gramdet
c D2_12
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I12+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I22+(p1
     #Dp2*p2Dp3-p1Dp3*p2Dp2)*I32
      D2_12=t0/wbb_gramdet
c D2_13
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I13+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I23+(p1
     #Dp2*p2Dp3-p1Dp3*p2Dp2)*I33
      D2_13=t0/wbb_gramdet
c D2_23
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I13+(p1Dp1*p3Dp3-p1Dp3**2)*I23+(-p
     #1Dp1*p2Dp3+p1Dp2*p1Dp3)*I33
      D2_23=t0/wbb_gramdet
c
      return
      end
c
c
c
      subroutine wbb_d3_ir_new(p1,p2,p3,m1,m2,m3,m4,
     - D0,A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2,
     - C0234,A1c1_234,A2c1_234,A1c2_234,A2c2_234,A3c2_234,A4c2_234,
     - C0134,A1c1_134,A2c1_134,A1c2_134,A2c2_134,A3c2_134,A4c2_134,
     - C0124,A1c1_124,A2c1_124,A1c2_124,A2c2_124,A3c2_124,A4c2_124,
     - C0123,A1c1_123,A2c1_123,A1c2_123,A2c2_123,A3c2_123,A4c2_123,
     - D3_001,D3_002,D3_003,D3_111,D3_222,D3_333,
     - D3_112,D3_113,D3_221,D3_223,D3_331,D3_332,D3_123)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer dflag
      real*8 m1,m2,m3,m4,p1(4),p2(4),p3(4),f1,f2,f3
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p2Dp2,p2Dp3,p3Dp3,wbb_gramdet
      real*8 C0234,C0134,C0124,C0123,D0
      real*8 A1c1_234,A2c1_234,A1c2_234,A2c2_234,A3c2_234,A4c2_234,
     -   A1c1_134,A2c1_134,A1c2_134,A2c2_134,A3c2_134,A4c2_134,
     -   A1c1_124,A2c1_124,A1c2_124,A2c2_124,A3c2_124,A4c2_124,
     -   A1c1_123,A2c1_123,A1c2_123,A2c2_123,A3c2_123,A4c2_123,
     -   A1d1,A2d1,A3d1,A1d2,A2d2,A3d2,A4d2,A5d2,A6d2,A7d2
      real*8 I100,I200,I300,I111,I211,I311,I122,I222,I322,I133,I233,
     -   I333,I112,I212,I312,I113,I213,I313,I123,I223,I323,
     -   D3_001,D3_002,D3_003,D3_111,D3_222,D3_333,D3_112,D3_113,
     -   D3_221,D3_223,D3_331,D3_332,D3_123
      common/wbb_gramdet/dflag
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p3dp3=wbb_dot(p3,p3)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
c
      I100=1d0/2d0*(A4c2_134-A4c2_234+f1*A7d2)
      I111=1d0/2d0*(A1c2_134-C0234+f1*A1d2)
      I122=1d0/2d0*(A1c2_134-A1c2_234+f1*A2d2)
      I133=1d0/2d0*(A2c2_134-A2c2_234+f1*A3d2)
      I112=1d0/2d0*(A1c2_134+A1c1_234+f1*A4d2)
      I113=1d0/2d0*(A3c2_134+A2c1_234+f1*A5d2)
      I123=1d0/2d0*(A3c2_134-A3c2_234+f1*A6d2)
c
      I200=1d0/2d0*(A4c2_124-A4c2_134+f2*A7d2)
      I211=1d0/2d0*(A1c2_124-A1c2_134+f2*A1d2)
      I222=1d0/2d0*(A2c2_124-A1c2_134+f2*A2d2)
      I233=1d0/2d0*(A2c2_124-A2c2_134+f2*A3d2)
      I212=1d0/2d0*(A3c2_124-A1c2_134+f2*A4d2)
      I213=1d0/2d0*(A3c2_124-A3c2_134+f2*A5d2)
      I223=1d0/2d0*(A2c2_124-A3c2_134+f2*A6d2)
c
      I300=1d0/2d0*(A4c2_123-A4c2_124+f3*A7d2)
      I311=1d0/2d0*(A1c2_123-A1c2_124+f3*A1d2)
      I322=1d0/2d0*(A2c2_123-A2c2_124+f3*A2d2)
      I333=1d0/2d0*(-A2c2_124+f3*A3d2)
      I312=1d0/2d0*(A3c2_123-A3c2_124+f3*A4d2)
      I313=1d0/2d0*(-A3c2_124+f3*A5d2)
      I323=1d0/2d0*(-A2c2_124+f3*A6d2)
c
c Gram Determinant
      wbb_gramdet = 
     #p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+
     #2.D0*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2
      if (dabs(wbb_gramdet).lt.1D-01) then
         dflag=1
         return
      endif
c D3_001
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I100+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I200+(
     #p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I300
      D3_001=t0/wbb_gramdet
c D3_002
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I100+(p1Dp1*p3Dp3-p1Dp3**2)*I200+(
     #-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I300
      D3_002=t0/wbb_gramdet
c D3_003
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I100+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I20
     #0+(p1Dp1*p2Dp2-p1Dp2**2)*I300
      D3_003=t0/wbb_gramdet
c
      I111=I111-2d0*D3_001
      I112=I112-D3_002
      I113=I113-D3_003
      I222=I222-2d0*D3_002
      I212=I212-D3_001
      I223=I223-D3_003
      I333=I333-2d0*D3_003
      I313=I313-D3_001
      I323=I323-D3_002
c
c D3_111
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I111+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I211+(
     #p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I311
      D3_111=t0/wbb_gramdet
c D3_112
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I111+(p1Dp1*p3Dp3-p1Dp3**2)*I211+(
     #-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I311
      D3_112=t0/wbb_gramdet
c D3_113
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I111+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I21
     #1+(p1Dp1*p2Dp2-p1Dp2**2)*I311
      D3_113=t0/wbb_gramdet
c D3_221
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I122+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I222+(
     #p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I322
      D3_221=t0/wbb_gramdet
c D3_222
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I122+(p1Dp1*p3Dp3-p1Dp3**2)*I222+(
     #-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I322
      D3_222=t0/wbb_gramdet
c D3_223
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I122+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I22
     #2+(p1Dp1*p2Dp2-p1Dp2**2)*I322
      D3_223=t0/wbb_gramdet
c D3_331
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I133+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I233+(
     #p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I333
      D3_331=t0/wbb_gramdet
c D3_332
      t0 = (-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I133+(p1Dp1*p3Dp3-p1Dp3**2)*I233+(
     #-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I333
      D3_332=t0/wbb_gramdet
c D3_333
      t0 = (p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I133+(-p1Dp1*p2Dp3+p1Dp2*p1Dp3)*I23
     #3+(p1Dp1*p2Dp2-p1Dp2**2)*I333
      D3_333=t0/wbb_gramdet
c D3_123
      t0 = (p2Dp2*p3Dp3-p2Dp3**2)*I123+(-p1Dp2*p3Dp3+p1Dp3*p2Dp3)*I223+(
     #p1Dp2*p2Dp3-p1Dp3*p2Dp2)*I323
      D3_123=t0/wbb_gramdet
c
      return
      end
      subroutine wbb_e1_new(p1,p2,p3,p4,m1,m2,m3,m4,m5,
     - E0,D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,E1_1,E1_2,E1_3,E1_4)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      integer dflag
      real*8 m1,m2,m3,m4,m5,p1(4),p2(4),p3(4),p4(4),f1,f2,f3,f4
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p1Dp4,p2Dp2,p2Dp3,p2Dp4,p3Dp3,p3Dp4,
     -       p4Dp4,wbb_gramdet
      real*8 D0_2345,D0_1345,D0_1245,D0_1235,D0_1234,E0
      real*8 I1,I2,I3,I4,E1_1,E1_2,E1_3,E1_4
      common/wbb_gramdet/dflag
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p1dp4=wbb_dot(p1,p4)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p2dp4=wbb_dot(p2,p4)
      p3dp3=wbb_dot(p3,p3)
      p3dp4=wbb_dot(p3,p4)
      p4dp4=wbb_dot(p4,p4)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
      f4=-wbb_dot(p4,p4)-2d0*wbb_dot(p1,p4)-2d0*wbb_dot(p2,p4)-2d0*wbb_dot(p3,p4)
     -   +m5**2-m4**2
c
      I1=1d0/2d0*(D0_1345-D0_2345+f1*E0)
      I2=1d0/2d0*(D0_1245-D0_1345+f2*E0)
      I3=1d0/2d0*(D0_1235-D0_1245+f3*E0)
      I4=1d0/2d0*(D0_1234-D0_1235+f4*E0)
c
c Gram Determinant
      t0 = p1Dp1*p2Dp2*p3Dp3*p4Dp4-p1Dp1*p2Dp2*p3Dp4**2-p1Dp1*p2Dp3**2*p
     #4Dp4+2.D0*p1Dp1*p2Dp3*p2Dp4*p3Dp4-p1Dp1*p2Dp4**2*p3Dp3-p1Dp2**2*p3
     #Dp3*p4Dp4+p1Dp2**2*p3Dp4**2+2.D0*p1Dp2*p2Dp3*p1Dp3*p4Dp4-2.D0*p1Dp
     #2*p2Dp3*p1Dp4*p3Dp4-2.D0*p1Dp2*p2Dp4*p1Dp3*p3Dp4+2.D0*p1Dp2*p2Dp4*
     #p1Dp4*p3Dp3-p2Dp2*p1Dp3**2*p4Dp4+2.D0*p1Dp3*p2Dp2*p1Dp4*p3Dp4+p1Dp
     #3**2*p2Dp4**2-2.D0*p1Dp3*p2Dp4*p1Dp4*p2Dp3-p2Dp2*p1Dp4**2*p3Dp3+p1
     #Dp4**2*p2Dp3**2
      wbb_gramdet = t0
      if (dabs(wbb_gramdet).lt.1D-01) then
         dflag=1
         return
      endif
c E1_1
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I1+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p
     #2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p
     #3Dp3)*I2+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2
     #Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I3+(-p1Dp2*p2Dp3
     #*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3-p2Dp3
     #*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I4
      E1_1=t0/wbb_gramdet
c E1_2
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I1+(p1Dp1*p3Dp3*p4D
     #p4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2*p
     #3Dp3)*I2+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p
     #1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I3+(p1Dp1*p2Dp3
     #*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1
     #Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I4
      E1_2=t0/wbb_gramdet
c E1_3
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I1+(-p1Dp1*p2Dp3*p4D
     #p4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1D
     #p2*p3Dp4+p1Dp4**2*p2Dp3)*I2+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4-p1D
     #p2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I3+(-p1Dp1*p2Dp
     #2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p
     #1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I4
      E1_3=t0/wbb_gramdet
c E1_4
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I1+(p1Dp1*p2Dp3*p3D
     #p4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp2*
     #p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I2+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+
     #p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2D
     #p2)*I3+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0*p1Dp2
     #*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I4
      E1_4=t0/wbb_gramdet
c
      return
      end
c
c
      subroutine wbb_e2_new(p1,p2,p3,p4,m1,m2,m3,m4,m5,
     -  E0,e1_1,e1_2,e1_3,e1_4,
     -  D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     -  D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     -  D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     -  D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     -  D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,
     -  E2_11,E2_22,E2_33,E2_44,E2_12,E2_13,E2_14,E2_23,E2_24,E2_34)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      real*8 m1,m2,m3,m4,m5,p1(4),p2(4),p3(4),p4(4),f1,f2,f3,f4
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p1Dp4,p2Dp2,p2Dp3,p2Dp4,p3Dp3,p3Dp4,
     -       p4Dp4,wbb_gramdet
      real*8 E0,e1_1,e1_2,e1_3,e1_4,
     -  D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,
     -  D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,
     -  D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,
     -  D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,
     -  D0_1234,A1d1_1234,A2d1_1234,A3d1_1234
      real*8 I11,I12,I13,I14,I21,I22,I23,I24,I31,I32,I33,I34,I41,I42,
     -  I43,I44,
     -  E2_11,E2_22,E2_33,E2_44,E2_12,E2_13,E2_14,E2_23,E2_24,E2_34
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p1dp4=wbb_dot(p1,p4)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p2dp4=wbb_dot(p2,p4)
      p3dp3=wbb_dot(p3,p3)
      p3dp4=wbb_dot(p3,p4)
      p4dp4=wbb_dot(p4,p4)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
      f4=-wbb_dot(p4,p4)-2d0*wbb_dot(p1,p4)-2d0*wbb_dot(p2,p4)-2d0*wbb_dot(p3,p4)
     -   +m5**2-m4**2
c
      I11=1d0/2d0*(A1d1_1345+D0_2345+f1*e1_1)
      I12=1d0/2d0*(A1d1_1345-A1d1_2345+f1*e1_2)
      I13=1d0/2d0*(A2d1_1345-A2d1_2345+f1*e1_3)
      I14=1d0/2d0*(A3d1_1345-A3d1_2345+f1*e1_4)
      I21=1d0/2d0*(A1d1_1245-A1d1_1345+f2*e1_1)
      I22=1d0/2d0*(A2d1_1245-A1d1_1345+f2*e1_2)
      I23=1d0/2d0*(A2d1_1245-A2d1_1345+f2*e1_3)
      I24=1d0/2d0*(A3d1_1245-A3d1_1345+f2*e1_4)
      I31=1d0/2d0*(A1d1_1235-A1d1_1245+f3*e1_1)
      I32=1d0/2d0*(A2d1_1235-A2d1_1245+f3*e1_2)
      I33=1d0/2d0*(A3d1_1235-A2d1_1245+f3*e1_3)
      I34=1d0/2d0*(A3d1_1235-A3d1_1245+f3*e1_4)
      I41=1d0/2d0*(A1d1_1234-A1d1_1235+f4*e1_1)
      I42=1d0/2d0*(A2d1_1234-A2d1_1235+f4*e1_2)
      I43=1d0/2d0*(A3d1_1234-A3d1_1235+f4*e1_3)
      I44=1d0/2d0*(         -A3d1_1235+f4*e1_4)
c
c Gram Determinant
      t0 = p1Dp1*p2Dp2*p3Dp3*p4Dp4-p1Dp1*p2Dp2*p3Dp4**2-p1Dp1*p2Dp3**2*p
     #4Dp4+2.D0*p1Dp1*p2Dp3*p2Dp4*p3Dp4-p1Dp1*p2Dp4**2*p3Dp3-p1Dp2**2*p3
     #Dp3*p4Dp4+p1Dp2**2*p3Dp4**2+2.D0*p1Dp2*p2Dp3*p1Dp3*p4Dp4-2.D0*p1Dp
     #2*p2Dp3*p1Dp4*p3Dp4-2.D0*p1Dp2*p2Dp4*p1Dp3*p3Dp4+2.D0*p1Dp2*p2Dp4*
     #p1Dp4*p3Dp3-p2Dp2*p1Dp3**2*p4Dp4+2.D0*p1Dp3*p2Dp2*p1Dp4*p3Dp4+p1Dp
     #3**2*p2Dp4**2-2.D0*p1Dp3*p2Dp4*p1Dp4*p2Dp3-p2Dp2*p1Dp4**2*p3Dp3+p1
     #Dp4**2*p2Dp3**2
      wbb_gramdet = t0
c E2_11
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I11+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+
     #p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*
     #p3Dp3)*I21+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+
     #p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I31+(-p1Dp2*p2
     #Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3-p2
     #Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I41
      E2_11=t0/wbb_gramdet
c E2_22
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I12+(p1Dp1*p3Dp3*p4
     #Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2*
     #p3Dp3)*I22+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4
     #-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I32+(p1Dp1*p2
     #Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4
     #*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I42
      E2_22=t0/wbb_gramdet
c E2_33
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I13+(-p1Dp1*p2Dp3*p4
     #Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1
     #Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I23+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4-p
     #1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I33+(-p1Dp1*p
     #2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp
     #4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I43
      E2_33=t0/wbb_gramdet
c E2_44
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I14+(p1Dp1*p2Dp3*p3
     #Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp2
     #*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I24+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp
     #3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p
     #2Dp2)*I34+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0*p1
     #Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I44
      E2_44=t0/wbb_gramdet
c E2_12
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I12+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+
     #p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*
     #p3Dp3)*I22+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+
     #p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I32+(-p1Dp2*p2
     #Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3-p2
     #Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I42
      E2_12=t0/wbb_gramdet
c E2_13
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I13+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+
     #p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*
     #p3Dp3)*I23+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+
     #p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I33+(-p1Dp2*p2
     #Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3-p2
     #Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I43
      E2_13=t0/wbb_gramdet
c E2_14
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I14+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+
     #p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*
     #p3Dp3)*I24+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+
     #p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I34+(-p1Dp2*p2
     #Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3-p2
     #Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I44
      E2_14=t0/wbb_gramdet
c E2_23
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I13+(p1Dp1*p3Dp3*p4
     #Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2*
     #p3Dp3)*I23+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4
     #-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I33+(p1Dp1*p2
     #Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4
     #*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I43
      E2_23=t0/wbb_gramdet
c E2_24
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I14+(p1Dp1*p3Dp3*p4
     #Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2*
     #p3Dp3)*I24+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4
     #-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I34+(p1Dp1*p2
     #Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4
     #*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I44
      E2_24=t0/wbb_gramdet
c E2_34
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I14+(-p1Dp1*p2Dp3*p4
     #Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1
     #Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I24+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4-p
     #1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I34+(-p1Dp1*p
     #2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp
     #4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I44
      E2_34=t0/wbb_gramdet
c
      return
      end
c
c
      subroutine wbb_e3_new(p1,p2,p3,p4,m1,m2,m3,m4,m5,
     - E0,e1_1,e1_2,e1_3,e1_4,e2_11,e2_22,e2_33,e2_44,e2_12,e2_13,
     - e2_14,e2_23,e2_24,e2_34,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,A1d2_2345,A2d2_2345,
     - A3d2_2345,A4d2_2345,A5d2_2345,A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,A1d2_1345,A2d2_1345,
     - A3d2_1345,A4d2_1345,A5d2_1345,A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,A1d2_1245,A2d2_1245,
     - A3d2_1245,A4d2_1245,A5d2_1245,A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,A1d2_1235,A2d2_1235,
     - A3d2_1235,A4d2_1235,A5d2_1235,A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,A1d2_1234,A2d2_1234,
     - A3d2_1234,A4d2_1234,A5d2_1234,A6d2_1234,A7d2_1234,
     - E3_111,E3_222,E3_333,E3_444,E3_112,E3_113,E3_114,
     - E3_221,E3_223,E3_224,E3_331,E3_332,E3_334,E3_441,E3_442,E3_443,
     - E3_123,E3_124,E3_134,E3_234,
     - E3_112_test,E3_113_test,E3_114_test,E3_221_test,E3_223_test,
     - E3_224_test,E3_331_test,E3_332_test,E3_334_test,E3_441_test,
     - E3_442_test,E3_443_test,E3_123_test,E3_124_test,E3_134_test,
     - E3_234_test)
c
      implicit none
c
      external wbb_dot
      real*8 wbb_dot
c
      real*8 m1,m2,m3,m4,m5,p1(4),p2(4),p3(4),p4(4),f1,f2,f3,f4
      real*8 t0,p1Dp1,p1Dp2,p1Dp3,p1Dp4,p2Dp2,p2Dp3,p2Dp4,p3Dp3,p3Dp4,
     -       p4Dp4,wbb_gramdet,z11,z22,z33,z44,z12,z13,z14,z23,z24,z34
      real*8 E0,e1_1,e1_2,e1_3,e1_4,e2_11,e2_22,e2_33,e2_44,
     - e2_12,e2_13,e2_14,e2_23,e2_24,e2_34,
     - D0_2345,A1d1_2345,A2d1_2345,A3d1_2345,A1d2_2345,A2d2_2345,
     - A3d2_2345,A4d2_2345,A5d2_2345,A6d2_2345,A7d2_2345,
     - D0_1345,A1d1_1345,A2d1_1345,A3d1_1345,A1d2_1345,A2d2_1345,
     - A3d2_1345,A4d2_1345,A5d2_1345,A6d2_1345,A7d2_1345,
     - D0_1245,A1d1_1245,A2d1_1245,A3d1_1245,A1d2_1245,A2d2_1245,
     - A3d2_1245,A4d2_1245,A5d2_1245,A6d2_1245,A7d2_1245,
     - D0_1235,A1d1_1235,A2d1_1235,A3d1_1235,A1d2_1235,A2d2_1235,
     - A3d2_1235,A4d2_1235,A5d2_1235,A6d2_1235,A7d2_1235,
     - D0_1234,A1d1_1234,A2d1_1234,A3d1_1234,A1d2_1234,A2d2_1234,
     - A3d2_1234,A4d2_1234,A5d2_1234,A6d2_1234,A7d2_1234
      real*8 I10,I111,I122,I133,I144,I112,I113,I114,I123,I124,I134,
     - I20,I211,I222,I233,I244,I212,I213,I214,I223,I224,I234,
     - I30,I311,I322,I333,I344,I312,I313,I314,I323,I324,I334,
     - I40,I411,I422,I433,I444,I412,I413,I414,I423,I424,I434, 
     - E3_111,E3_222,E3_333,E3_444,E3_112,E3_113,E3_114,
     - E3_221,E3_223,E3_224,E3_331,E3_332,E3_334,E3_441,E3_442,E3_443,
     - E3_123,E3_124,E3_134,E3_234,
     - E3_112_test,E3_113_test,E3_114_test,E3_221_test,E3_223_test,
     - E3_224_test,E3_331_test,E3_332_test,E3_334_test,E3_441_test,
     - E3_442_test,E3_443_test,E3_123_test,E3_124_test,E3_134_test,
     - E3_234_test
c
      p1dp1=wbb_dot(p1,p1)
      p1dp2=wbb_dot(p1,p2)
      p1dp3=wbb_dot(p1,p3)
      p1dp4=wbb_dot(p1,p4)
      p2dp2=wbb_dot(p2,p2)
      p2dp3=wbb_dot(p2,p3)
      p2dp4=wbb_dot(p2,p4)
      p3dp3=wbb_dot(p3,p3)
      p3dp4=wbb_dot(p3,p4)
      p4dp4=wbb_dot(p4,p4)
      f1=-wbb_dot(p1,p1)+m2**2-m1**2
      f2=-wbb_dot(p2,p2)-2d0*wbb_dot(p1,p2)+m3**2-m2**2
      f3=-wbb_dot(p3,p3)-2d0*wbb_dot(p1,p3)-2d0*wbb_dot(p2,p3)+m4**2-m3**2
      f4=-wbb_dot(p4,p4)-2d0*wbb_dot(p1,p4)-2d0*wbb_dot(p2,p4)-2d0*wbb_dot(p3,p4)
     -   +m5**2-m4**2
c
c Gram Determinant
      t0 = p1Dp1*p2Dp2*p3Dp3*p4Dp4-p1Dp1*p2Dp2*p3Dp4**2-p1Dp1*p2Dp3**2*p
     #4Dp4+2.D0*p1Dp1*p2Dp3*p2Dp4*p3Dp4-p1Dp1*p2Dp4**2*p3Dp3-p1Dp2**2*p3
     #Dp3*p4Dp4+p1Dp2**2*p3Dp4**2+2.D0*p1Dp2*p2Dp3*p1Dp3*p4Dp4-2.D0*p1Dp
     #2*p2Dp3*p1Dp4*p3Dp4-2.D0*p1Dp2*p2Dp4*p1Dp3*p3Dp4+2.D0*p1Dp2*p2Dp4*
     #p1Dp4*p3Dp3-p2Dp2*p1Dp3**2*p4Dp4+2.D0*p1Dp3*p2Dp2*p1Dp4*p3Dp4+p1Dp
     #3**2*p2Dp4**2-2.D0*p1Dp3*p2Dp4*p1Dp4*p2Dp3-p2Dp2*p1Dp4**2*p3Dp3+p1
     #Dp4**2*p2Dp3**2
      wbb_gramdet = t0
c z11=[Z^(-1)]_11
      t0 = p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p2
     #Dp4*p3Dp4-p2Dp4**2*p3Dp3
      z11=t0/wbb_gramdet
c z22=[Z^(-1)]_22
      t0 = p1Dp1*p3Dp3*p4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1
     #Dp4*p3Dp4-p1Dp4**2*p3Dp3
      z22=t0/wbb_gramdet
c z33=[Z^(-1)]_33
      t0 = p1Dp1*p2Dp2*p4Dp4-p1Dp1*p2Dp4**2-p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1
     #Dp4*p2Dp4-p1Dp4**2*p2Dp2
      z33=t0/wbb_gramdet
c z44=[Z^(-1)]_44
      t0 = p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0*p1Dp2*p1
     #Dp3*p2Dp3-p1Dp3**2*p2Dp2
      z44=t0/wbb_gramdet
c z12=[Z^(-1)]_12
      t0 = -p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1D
     #p4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3
      z12=t0/wbb_gramdet
c z13=[Z^(-1)]_13
      t0 = p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*p
     #1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3
      z13=t0/wbb_gramdet
c z14=[Z^(-1)]_14
      t0 = -p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*
     #p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2
      z14=t0/wbb_gramdet
c z23=[Z^(-1)]_23
      t0 = -p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*
     #p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3
      z23=t0/wbb_gramdet
c z24=[Z^(-1)]_24
      t0 = p1Dp1*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**
     #2*p2Dp4+p1Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3
      z24=t0/wbb_gramdet
c z34=[Z^(-1)]_34
      t0 = -p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1D
     #p3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2
      z34=t0/wbb_gramdet
c
      I10=1d0/2d0*(A7d2_1345-A7d2_2345)
      I111=1d0/2d0*(A1d2_1345-D0_2345+f1*e2_11)+z11*I10
      I122=1d0/2d0*(A1d2_1345-A1d2_2345+f1*e2_22)+z22*I10
      I133=1d0/2d0*(A2d2_1345-A2d2_2345+f1*e2_33)+z33*I10
      I144=1d0/2d0*(A3d2_1345-A3d2_2345+f1*e2_44)+z44*I10
      I112=1d0/2d0*(A1d2_1345+A1d1_2345+f1*e2_12)+z12*I10
      I113=1d0/2d0*(A4d2_1345+A2d1_2345+f1*e2_13)+z13*I10
      I114=1d0/2d0*(A5d2_1345+A3d1_2345+f1*e2_14)+z14*I10
      I123=1d0/2d0*(A4d2_1345-A4d2_2345+f1*e2_23)+z23*I10
      I124=1d0/2d0*(A5d2_1345-A5d2_2345+f1*e2_24)+z24*I10
      I134=1d0/2d0*(A6d2_1345-A6d2_2345+f1*e2_34)+z34*I10
c
      I20=1d0/2d0*(A7d2_1245-A7d2_1345)
      I211=1d0/2d0*(A1d2_1245-A1d2_1345+f2*e2_11)+z11*I20
      I222=1d0/2d0*(A2d2_1245-A1d2_1345+f2*e2_22)+z22*I20
      I233=1d0/2d0*(A2d2_1245-A2d2_1345+f2*e2_33)+z33*I20
      I244=1d0/2d0*(A3d2_1245-A3d2_1345+f2*e2_44)+z44*I20
      I212=1d0/2d0*(A4d2_1245-A1d2_1345+f2*e2_12)+z12*I20
      I213=1d0/2d0*(A4d2_1245-A4d2_1345+f2*e2_13)+z13*I20
      I214=1d0/2d0*(A5d2_1245-A5d2_1345+f2*e2_14)+z14*I20
      I223=1d0/2d0*(A2d2_1245-A4d2_1345+f2*e2_23)+z23*I20
      I224=1d0/2d0*(A6d2_1245-A5d2_1345+f2*e2_24)+z24*I20
      I234=1d0/2d0*(A6d2_1245-A6d2_1345+f2*e2_34)+z34*I20
c
      I30=1d0/2d0*(A7d2_1235-A7d2_1245)
      I311=1d0/2d0*(A1d2_1235-A1d2_1245+f3*e2_11)+z11*I30
      I322=1d0/2d0*(A2d2_1235-A2d2_1245+f3*e2_22)+z22*I30
      I333=1d0/2d0*(A3d2_1235-A2d2_1245+f3*e2_33)+z33*I30
      I344=1d0/2d0*(A3d2_1235-A3d2_1245+f3*e2_44)+z44*I30
      I312=1d0/2d0*(A4d2_1235-A4d2_1245+f3*e2_12)+z12*I30
      I313=1d0/2d0*(A5d2_1235-A4d2_1245+f3*e2_13)+z13*I30
      I314=1d0/2d0*(A5d2_1235-A5d2_1245+f3*e2_14)+z14*I30
      I323=1d0/2d0*(A6d2_1235-A2d2_1245+f3*e2_23)+z23*I30
      I324=1d0/2d0*(A6d2_1235-A6d2_1245+f3*e2_24)+z24*I30
      I334=1d0/2d0*(A3d2_1235-A6d2_1245+f3*e2_34)+z34*I30
c
      I40=1d0/2d0*(A7d2_1234-A7d2_1235)
      I411=1d0/2d0*(A1d2_1234-A1d2_1235+f4*e2_11)+z11*I40
      I422=1d0/2d0*(A2d2_1234-A2d2_1235+f4*e2_22)+z22*I40
      I433=1d0/2d0*(A3d2_1234-A3d2_1235+f4*e2_33)+z33*I40
      I444=1d0/2d0*(         -A3d2_1235+f4*e2_44)+z44*I40
      I412=1d0/2d0*(A4d2_1234-A4d2_1235+f4*e2_12)+z12*I40
      I413=1d0/2d0*(A5d2_1234-A5d2_1235+f4*e2_13)+z13*I40
      I414=1d0/2d0*(         -A5d2_1235+f4*e2_14)+z14*I40
      I423=1d0/2d0*(A6d2_1234-A6d2_1235+f4*e2_23)+z23*I40
      I424=1d0/2d0*(         -A6d2_1235+f4*e2_24)+z24*I40
      I434=1d0/2d0*(         -A3d2_1235+f4*e2_34)+z34*I40
c
c E3_111
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I111+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I211+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I311+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I411
      E3_111=t0/wbb_gramdet
c E3_222
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I122+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I222+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I322+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I422
      E3_222=t0/wbb_gramdet
c E3_333
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I133+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I233+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4
     #-p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I333+(-p1Dp
     #1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p
     #1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I433
      E3_333=t0/wbb_gramdet
c E3_444
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I144+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I244+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I344+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I444
      E3_444=t0/wbb_gramdet
c E3_112
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I111+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I211+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I311+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I411
      E3_112=t0/wbb_gramdet
c E3_113
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I111+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I211+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4
     #-p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I311+(-p1Dp
     #1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p
     #1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I411
      E3_113=t0/wbb_gramdet
c E3_114
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I111+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I211+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I311+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I411
      E3_114=t0/wbb_gramdet
c E3_221
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I122+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I222+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I322+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I422
      E3_221=t0/wbb_gramdet
c E3_223
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I122+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I222+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4
     #-p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I322+(-p1Dp
     #1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p
     #1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I422
      E3_223=t0/wbb_gramdet
c E3_224
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I122+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I222+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I322+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I422
      E3_224=t0/wbb_gramdet
c E3_331
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I133+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I233+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I333+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I433
      E3_331=t0/wbb_gramdet
c E3_332
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I133+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I233+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I333+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I433
      E3_332=t0/wbb_gramdet
c E3_334
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I133+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I233+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I333+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I433
      E3_334=t0/wbb_gramdet
c E3_441
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I144+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I244+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I344+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I444
      E3_441=t0/wbb_gramdet
c E3_442
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I144+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I244+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I344+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I444
      E3_442=t0/wbb_gramdet
c E3_443
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I144+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I244+(-p1Dp1*p2Dp4**2+p1Dp1*p2Dp2*p4Dp4
     #-p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I344+(-p1Dp
     #1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p
     #1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I444
      E3_443=t0/wbb_gramdet
c E3_123
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I123+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I223+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I323+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I423
      E3_123=t0/wbb_gramdet
c E3_124
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I124+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I224+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I324+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I424
      E3_124=t0/wbb_gramdet
c E3_134
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I134+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I234+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I334+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I434
      E3_134=t0/wbb_gramdet
c E3_234
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I134+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I234+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I334+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I434
      E3_234=t0/wbb_gramdet
c E3_112_test
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I112+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I212+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I312+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I412
      E3_112_test=t0/wbb_gramdet
c E3_113_test
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I113+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I213+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I313+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I413
      E3_113_test=t0/wbb_gramdet
c E3_114_test
      t0 = (p2Dp2*p3Dp3*p4Dp4-p2Dp2*p3Dp4**2-p2Dp3**2*p4Dp4+2.D0*p2Dp3*p
     #2Dp4*p3Dp4-p2Dp4**2*p3Dp3)*I114+(-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2
     #+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4
     #*p3Dp3)*I214+(p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp
     #4+p2Dp2*p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I314+(-p1Dp2
     #*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2*p1Dp4*p3Dp3
     #-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I414
      E3_114_test=t0/wbb_gramdet
c E3_221_test
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I112+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I212+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I312+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I412
      E3_221_test=t0/wbb_gramdet
c E3_223_test
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I123+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I223+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I323+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I423
      E3_223_test=t0/wbb_gramdet
c E3_224_test
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I124+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I224+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I324+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I424
      E3_224_test=t0/wbb_gramdet
c E3_331_test
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I113+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I213+(p1Dp1*p2Dp2*p4Dp4-p1Dp1*p2Dp4**2-
     #p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I313+(-p1Dp1
     #*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1
     #Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I413
      E3_331_test=t0/wbb_gramdet
c E3_332_test
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I123+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I223+(p1Dp1*p2Dp2*p4Dp4-p1Dp1*p2Dp4**2-
     #p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I323+(-p1Dp1
     #*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1
     #Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I423
      E3_332_test=t0/wbb_gramdet
c E3_334_test
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I134+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I234+(p1Dp1*p2Dp2*p4Dp4-p1Dp1*p2Dp4**2-
     #p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I334+(-p1Dp1
     #*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1
     #Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I434
      E3_334_test=t0/wbb_gramdet
c E3_441_test
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I114+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I214+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I314+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I414
      E3_441_test=t0/wbb_gramdet
c E3_442_test
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I124+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I224+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I324+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I424
      E3_442_test=t0/wbb_gramdet
c E3_443_test
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I134+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I234+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I334+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I434
      E3_443_test=t0/wbb_gramdet
c E3_123_test
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I113+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I213+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I313+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I413
      E3_123_test=t0/wbb_gramdet
c E3_124_test
      t0 = (-p1Dp2*p3Dp3*p4Dp4+p1Dp2*p3Dp4**2+p2Dp3*p1Dp3*p4Dp4-p2Dp3*p1
     #Dp4*p3Dp4-p2Dp4*p1Dp3*p3Dp4+p2Dp4*p1Dp4*p3Dp3)*I114+(p1Dp1*p3Dp3*p
     #4Dp4-p1Dp1*p3Dp4**2-p1Dp3**2*p4Dp4+2.D0*p1Dp3*p1Dp4*p3Dp4-p1Dp4**2
     #*p3Dp3)*I214+(-p1Dp1*p2Dp3*p4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4D
     #p4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I314+(p1Dp1
     #*p2Dp3*p3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1
     #Dp4*p1Dp2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I414
      E3_124_test=t0/wbb_gramdet
c E3_134_test
      t0 = (p1Dp2*p2Dp3*p4Dp4-p1Dp2*p2Dp4*p3Dp4-p2Dp2*p1Dp3*p4Dp4+p2Dp2*
     #p1Dp4*p3Dp4+p1Dp3*p2Dp4**2-p2Dp4*p1Dp4*p2Dp3)*I114+(-p1Dp1*p2Dp3*p
     #4Dp4+p1Dp1*p2Dp4*p3Dp4+p1Dp3*p1Dp2*p4Dp4-p1Dp3*p1Dp4*p2Dp4-p1Dp4*p
     #1Dp2*p3Dp4+p1Dp4**2*p2Dp3)*I214+(p1Dp1*p2Dp2*p4Dp4-p1Dp1*p2Dp4**2-
     #p1Dp2**2*p4Dp4+2.D0*p1Dp2*p1Dp4*p2Dp4-p1Dp4**2*p2Dp2)*I314+(-p1Dp1
     #*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1
     #Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3*p2Dp2)*I414
      E3_134_test=t0/wbb_gramdet
c E3_234_test
      t0 = (-p1Dp2*p2Dp3*p3Dp4+p1Dp2*p2Dp4*p3Dp3+p2Dp2*p1Dp3*p3Dp4-p2Dp2
     #*p1Dp4*p3Dp3-p2Dp3*p1Dp3*p2Dp4+p1Dp4*p2Dp3**2)*I123+(p1Dp1*p2Dp3*p
     #3Dp4-p1Dp1*p2Dp4*p3Dp3-p1Dp3*p1Dp2*p3Dp4+p1Dp3**2*p2Dp4+p1Dp4*p1Dp
     #2*p3Dp3-p1Dp4*p1Dp3*p2Dp3)*I223+(-p1Dp1*p2Dp2*p3Dp4+p1Dp1*p2Dp4*p2
     #Dp3+p1Dp2**2*p3Dp4-p1Dp2*p1Dp3*p2Dp4-p1Dp4*p1Dp2*p2Dp3+p1Dp4*p1Dp3
     #*p2Dp2)*I323+(p1Dp1*p2Dp2*p3Dp3-p1Dp1*p2Dp3**2-p1Dp2**2*p3Dp3+2.D0
     #*p1Dp2*p1Dp3*p2Dp3-p1Dp3**2*p2Dp2)*I423
      E3_234_test=t0/wbb_gramdet
      return
      end
c---------------------------------------------------------------------
c     checked version of "integrallib.f"
c---------------------------------------------------------------------
c--------------------------------------------------------------------
      subroutine wbb_fint(s,m1,m2,f)
c--------------------------------------------------------------------
c     the f-function as specified in : boehm et.al., 
c     fortschr.phys.34 ; and in : hollik, fortschr.phys. 38 
c--------------------------------------------------------------------
      implicit real*8(a-z)
      parameter (eps=1d-6)
      complex*16 f    
      pi = 4d0*datan(1d0)
      x1 = m1**2
      x2 = m2**2
      p = s-(dabs(m1)+dabs(m2))**2
      m = s-(dabs(m1)-dabs(m2))**2
      imagf = 0d0
      rest = 0d0
      if (abs(s).lt.eps) then
         realf = 0d0
         goto 999
      endif
c
      if (x1.lt.eps) then
         if (x2.lt.eps) then
            realf = 0d0
         else
            if (s.gt.x2+eps) then
               realf = 1d0+(1d0-x2/s)*dlog(1d0/(s/x2-1d0))
               imagf = pi*(1d0-x2/s)
            else if (s.lt.x2-eps) then
               realf = 1d0+(1d0-x2/s)*dlog(1d0/(1d0-s/x2))
            else
               realf = 1d0
            endif 
         endif
         goto 999
      else if (x2.lt.eps) then
         if (x1.lt.eps) then
            realf = 0d0
         else
            if (s.gt.x1+eps) then
               realf = 1d0+(1d0-x1/s)*dlog(1d0/(s/x1-1d0))
               imagf = pi*(1d0-x1/s)
            else if (s.lt.x1-eps) then
               realf = 1d0+(1d0-x1/s)*dlog(1d0/(1d0-s/x1))
            else
               realf = 1d0
            endif 
         endif
         goto 999
      endif
c
      if ((abs(s).lt.x1/1d3).or.(abs(s).lt.x2/1d3)) then
         if (dabs(dabs(m1)-dabs(m2)).lt.eps) then
            realf = s/6d0/x1
         else
            realf = s/(x1-x2)**2*((x1+x2)/2d0
     $           -x1*x2/(x1-x2)*dlog(x1/x2))
         endif
         goto 999
      endif
c
      if (dabs(dabs(m1)-dabs(m2)).lt.eps) then
         rest = 2d0
      else
         rest = 1d0-((x1-x2)/s-(x1+x2)/(x1-x2))*dlog(x1/x2)/2d0
      endif
c
      if (m.lt.0d0) then
         realf = dsqrt(p*m)*dlog((dsqrt(-p)+dsqrt(-m))**2
     $        /(4d0*dsqrt(x1*x2)))/s
      else if (p.lt.0d0) then
         realf = -2d0*dsqrt(-p*m)*datan(dsqrt(-m/p))/s
      else
         realf = -dsqrt(p*m)*dlog((dsqrt(p)+dsqrt(m))**2
     $        /(4d0*dsqrt(x1*x2)))/s
         imagf = dsqrt(p*m)/s*pi
      endif
 999  continue
      f = dcmplx(rest+realf,imagf)
      end
c---------------------------------------------------------------------
c---------------------------------------------------------------------
      subroutine wbb_afunc(m,a0)
      implicit real*8(a-z)
      parameter (eps=1d-4)
c      common / wbb_dimreg / muedr
      common/wbb_scale/muedr,muedf,xs,scalemur,scalemuf
      if (m**2.lt.eps) then 
         a0 = 0d0
      else
         a0 = m**2*(1d0-dlog(m**2/muedr**2))
      endif
      end
c---------------------------------------------------------------------
      subroutine wbb_bfunc(s,m1,m2,b0,b1,b20)
c---------------------------------------------------------------------
c     b0 and b1 are the b0"quer" and b1"quer"-quantities defined 
c     in : Hollik, Fortschr.Phys. 38 (but with log(mu)-terms incl.)
c     b20 is the g_{mu}.{nu} coefficient of the 
c     tensor-2-point-integral (log(mu)-terms incl, too)
c     muedr is the renormalization wbb_scale
c---------------------------------------------------------------------
      implicit real*8(a-z)
      parameter (eps=1d-4)
      complex*16 b0,b1,b20,ff
c      common / wbb_dimreg / muedr
      common/wbb_scale/muedr,muedf,xs,scalemur,scalemuf
      pi = 4d0*datan(1d0)
      x1 = m1**2
      x2 = m2**2
      if (dabs(s).lt.eps) then
         call wbb_bnull(m1,m2,realb0)
         call wbb_beins(m1,m2,realb1)
         call wbb_bzwnull(m1,m2,realb20)
         b0 = dcmplx(realb0,0d0)
         b1 = dcmplx(realb1,0d0)
         b20 = dcmplx(realb20,0d0)
         goto 999
      endif
      if ((x1.lt.eps).and.(x2.lt.eps)) goto 47 
      call wbb_fint(s,m1,m2,ff)
 47   continue
      if (x1.lt.eps) then
         if (x2.lt.eps) then
            b0 = 2d0-dlog(s/muedr**2)+dcmplx(0d0,1d0)*pi
            b1 = -b0/2d0
         else
            b0 = 1d0+ff-dlog(x2/muedr**2)
            b1 = -(1d0+(1d0-x2/s)*ff-dlog(x2/muedr**2))/2d0
         endif
         goto 666
      endif
      if (x2.lt.eps) then
         if (x1.lt.eps) then
            b0 = 2d0-dlog(s/muedr**2)+dcmplx(0d0,1d0)*pi
            b1 = -b0/2d0
         else
            b0 = 1d0+ff-dlog(x1/muedr**2)
            b1 = -(1d0+(1d0+x1/s)*ff-dlog(x1/muedr**2))/2d0
         endif
         goto 666
      endif
      if (dabs(x1-x2).lt.1d3*eps) then
         b0 = ff
         b1 = -ff/2d0
      else
         b0 = 1d0-(x1+x2)/(x1-x2)*dlog(x1/x2)/2d0+ff
         b1 = -1d0/2d0+x1/(x1-x2)*dlog(x1/x2)/2d0-(s+x1-x2)/2d0/s*ff
      endif
      b0 = b0 - dlog(x1*x2/muedr**4)/2d0
      b1 = b1 + dlog(x2/muedr**2)/2d0 
c     
 666  continue
      call wbb_afunc(m2,a0)
      b20 = a0/6d0+x1/3d0*b0
     $     +(s+x1-x2)/6d0*b1+(x1+x2-s/3d0)/6d0
 999  continue
      end
c--------------------------------------------------------------------
c     the x"null" subroutines calculate the b0-, b1-,
c     b20-integrals at zero external momenta  
c--------------------------------------------------------------------
      subroutine wbb_bnull(a,b,b0)
      implicit real*8(a-z)
      parameter (eps=1d-8)      
c      common / wbb_dimreg / muedr
      common/wbb_scale/muedr,muedf,xs,scalemur,scalemuf
      xa = a**2
      xb = b**2
      x = xa+xb
      if (x.lt.eps) then
c        write(6,*)'all args zero is not allowed for b0-function !'
         b0 = 0d0
         goto 2
      endif
      if (xa*xb.eq.0d0) then
         zw = 1d0-dlog(x/muedr**2)
      else
         zw = -dlog(xa*xb/muedr**4)/2d0
         if (dabs(dabs(a)-dabs(b)).gt.eps) then
            zw = zw+1d0-(xa+xb)/(xa-xb)*dlog(xa/xb)/2d0
         endif
      endif
      b0 = zw
 2    continue
      end
c-----------------------------------------------------------------     
      subroutine wbb_beins(a,b,b1)
      implicit real*8(a-z)
      parameter (eps=1d-8)
c      common / wbb_dimreg / muedr
      common/wbb_scale/muedr,muedf,xs,scalemur,scalemuf
      xa = a**2
      xb = b**2
      x = xa+xb
      if (x.lt.eps) then
c        write(6,*)'all args zero is not allowed for b1-function !'
         b1 = 0d0
         goto 2
      endif      
      if (xa.eq.0d0) then
         zw = -(1d0/2d0-dlog(xb/muedr**2))/2d0
      else if (xb.eq.0d0) then
         zw = -(3d0/2d0-dlog(xa/muedr**2))/2d0
      else
         zw = dlog(xb/muedr**2)/2d0
         if (dabs(dabs(a)-dabs(b)).gt.eps) then
            zw = zw -(xa+xb)/(xa-xb)/4d0-1d0/2d0
     $           +xa/(xa-xb)*dlog(xa/xb)/2d0
     $           +xa*xb/(xa-xb)**2*dlog(xa/xb)/2d0
         endif
      endif
      b1 = zw
 2    continue
      end
c-----------------------------------------------------------------     
      subroutine wbb_bzwnull(a,b,b20)
      implicit real*8(a-z)
c      common / wbb_dimreg / muedr
      common/wbb_scale/muedr,muedf,xs,scalemur,scalemuf
      xa = a**2
      xb = b**2
      call wbb_bnull(a,b,b0)
      call wbb_beins(a,b,b1)
      zw = xa*b0/3d0+(xa-xb)*b1/6d0+(xa+xb)/6d0
      if (xb.ne.0d0) then 
         zw = zw+xb*(1d0-dlog(xb/muedr**2))/6d0
      endif
      b20 = zw
      end
c
      real*8 function wbb_dot(a,b)
      implicit none
      real*8 a(4), b(4)
      wbb_dot = - a(1)*b(1) - a(2)*b(2) - a(3)*b(3) + a(4)*b(4)
      end
