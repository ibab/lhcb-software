c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine st_tch_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/pwhg_st.h'
      include 'src/Lib/st_tch/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer vflav(nleg)
      real * 8 virtual

ccccccccccccccccccccc
      integer mu,ileg
      real *8 kblab(0:3,nleg)
      real *8 ewcoupl,s,t,u
      real *8 fvirt_du,fvirt_ud,fvirt_ddx,fvirt_dxd
      real *8 st_tch_dotp
      external st_tch_dotp

      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/

      real *8 ckm_b_t
ccccccccccccccccccccc

ccccccccccccccc
      include 'src/Lib/st_tch/pwhg_flst.h'
      integer i_fb
c     To pass the already subtracted virtuals to sigsoftvirt
      real *8 fksfinite(1000)
      common/st_tch_cfksfinite/fksfinite
ccccccccccccccc

      integer vflav_loc(nleg)

c     check
      if (abs(vflav(3)).ne.6) then
         write(*,*) 'st_tch_setvirtual: ERROR in flavor assignement'
         call exit(1)
      endif

ccccccccccccccccccccccccccccccccccccccc
c     charge conjugation
c     if ttype=-1, then bflav has been filled with tbar-production flavours.
c     Subroutines here work for t-production flavour assignment.
c     Therefore, invert the sign of local flavours.
      do ileg=1,nleg
         vflav_loc(ileg)= ttype *vflav(ileg)
      enddo
ccccccccccccccccccccccccccccccccccccccc

c     local copy of variables
      do ileg=1,nleg
         do mu=0,3
            kblab(mu,ileg)=p(mu,ileg)
         enddo
      enddo

c     ew coupling
      ewcoupl=4d0*pi*alphaem_pow/sthw2_pow

c     Mandelstam variables
      s=2d0*st_tch_dotp(kblab(0,1),kblab(0,2))
      t=st_tch_dotp(kblab(0,3),kblab(0,3)) - 2d0*st_tch_dotp(kblab(0,1),kblab(0,3)) 
      u=st_tch_dotp(kblab(0,3),kblab(0,3))-s-t

ccccccccccccccccccccccccccccccccccccccccccc
c     >>> T CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc

c     Evaluate finite part of virtual contribution
      call st_tch_virt_finite(s,t,u,st_tch_dotp(kblab(0,3),kblab(0,3)),
     #fvirt_du,fvirt_ud,fvirt_ddx,fvirt_dxd,st_muren2)


c     ddx
      if ((three_ch(vflav_loc(1)).eq.-1).and.
     #(three_ch(vflav_loc(2)).eq.1)) then
         virtual=fvirt_ddx
         ckm_b_t=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(3)))**2 
     #* CKM(abs(vflav_loc(2)),abs(vflav_loc(4)))**2 

c     dxd
      elseif ((three_ch(vflav_loc(1)).eq.1).and.
     #(three_ch(vflav_loc(2)).eq.-1)) then
         virtual=fvirt_dxd
         ckm_b_t=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(4)))**2 
     #* CKM(abs(vflav_loc(2)),abs(vflav_loc(3)))**2

c     ud
      elseif ((three_ch(vflav_loc(1)).eq.2).and.
     #(three_ch(vflav_loc(2)).eq.-1)) then
         virtual=fvirt_ud
         ckm_b_t=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(4)))**2 
     #* CKM(abs(vflav_loc(2)),abs(vflav_loc(3)))**2

c     du
      elseif ((three_ch(vflav_loc(1)).eq.-1).and.
     #(three_ch(vflav_loc(2)).eq.2)) then
         virtual=fvirt_du
         ckm_b_t=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(3)))**2 
     #* CKM(abs(vflav_loc(2)),abs(vflav_loc(4)))**2

      else
         write(*,*) 'Error in st_tch_setvirtual, (t)'
         call exit(1)
      endif


ccccccccccccccccccccccc
c     assign output
      virtual=virtual *ewcoupl**2 *ckm_b_t
cccccccccccccccccccccc
      end



c     Output the finite part of virtual contribution.
c     Finite parts for t-channel single-top.
c     Virtual contributions are taken from Laenen-Weinzierl-Sullivan...
c     paper, after the manipulation described in the note section.
c     Prefactor of FKS is different, so an expansion of
c     EulerGamma functions was needed.
c     Moreover, nontrivial logaritms arise because of
c     the presence of (mu^2/s) in front of the result.
      subroutine st_tch_virt_finite(s,t,u,m2,fvirt_du,fvirt_ud,fvirt_ddx,
     $     fvirt_dxd,mur2)
      implicit none
      real *8 s,t,u,m2,fvirt_du,fvirt_ud,fvirt_ddx,fvirt_dxd,mur2
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/PhysPars.h'

      real *8 lambda,q2,prop
      real *8 amp2_du,amp2_ud,amp2_ddx,amp2_dxd

      real *8 vcf
      parameter (vcf=4.d0/3.d0)

      real *8 st_tch_DDILOG,A0,A1

      real *8 cv0,cv,prop_mcfm
      double complex c1

      logical check_virt
      parameter (check_virt=.false.)

      if(dabs(m2/topmass_pow**2 -1.).gt.1d-6) then
         write(*,*) 'st_tch_virt_finite: problem with top offshelness'
         call exit(1)
      endif

      amp2_ddx=u*(u-topmass_pow**2)/(t-wmass_pow**2)**2/4
      amp2_dxd=t*(t-topmass_pow**2)/(u-wmass_pow**2)**2/4
      amp2_ud= s*(s-topmass_pow**2)/(u-wmass_pow**2)**2/4
      amp2_du= s*(s-topmass_pow**2)/(t-wmass_pow**2)**2/4

      if(s+t+u-m2.gt.1d-7) then
         print*, 'st_tch_virt_finite: problem with invariants'
         call exit(1)
      endif


cccccccccccccccccccccccc
c     'true' t-channel
cccccccccccccccccccccccc
      q2=t
      lambda=q2/(q2-m2)

cccccccccccccccccccccccccccccc
c     Result obtained from Laenen&al. paper.
      A0= -(log(-s/q2))**2 -3*log(-s/q2) -8 -pi**2/3 
     $     + (-0.5*log(s/m2)**2 -5./2.*log(s/m2) 
     $     - 2*log(1-lambda)*log(s/m2) -6
     $     - 1/lambda*log(1-lambda) -(log(1-lambda))**2
     $     - 2*log(1-lambda) +2*st_tch_DDILOG(lambda) -pi**2/3)
      A1= (-3. -2*log(-s/q2)) 
     $     + (-5./2. -2.*log(1-lambda) -log(s/m2))

      prop=
     $     A0 
     $     + pi**2/2.           !from 'EulerGamma prefactor'
     $     + A1*log(mur2/s)
     $     - 3./2.*(log(mur2/s))**2
ccccccccccccccccccccccccccccc

c$$$      if(check_virt) then
c$$$c     from Campbell-Ellis, MCFM
c$$$c$$$      if     (nwz .eq. +1) then
c$$$c$$$        ub=+fac*virtqqb(1,2,3,4,5,6)
c$$$c$$$        bu=+fac*virtqqb(2,1,3,4,5,6)
c$$$c$$$        bubar=+fac*virtqqb(6,1,3,4,5,2)
c$$$c$$$        ubarb=+fac*virtqqb(6,2,3,4,5,1)
c$$$c$$$      elseif (nwz .eq. -1) then
c$$$c$$$        ub=fac*virtqqb(6,2,4,3,5,1)
c$$$c$$$        bu=fac*virtqqb(6,1,4,3,5,2)
c$$$c$$$        bubar=fac*virtqqb(2,1,4,3,5,6)
c$$$c$$$        ubarb=fac*virtqqb(1,2,4,3,5,6)
c$$$c$$$      endif
c$$$         call coefs(t,m2,cv0,cv,c1)
c$$$         prop_mcfm=cv0+cv
c$$$
c$$$         if(dabs(prop/prop_mcfm-1).gt.1d-6) then
c$$$            write(*,*) 'POW/MCFM ',prop/prop_mcfm
c$$$            call exit(1)
c$$$         endif
c$$$      endif

      prop=prop*vcf
      fvirt_ddx=prop*amp2_ddx !2
      fvirt_du=prop*amp2_du   !1
      
c     add part not proportional to born
      fvirt_ddx=fvirt_ddx+vcf *m2*u*s/t *log(m2/(m2-t)) /4.
     $     * (t- wmass_pow**2)**(-2)
      fvirt_du=fvirt_du  +vcf *m2*s*u/t *log(m2/(m2-t)) /4.
     $     * (t- wmass_pow**2)**(-2)

cccccccccccccccccccccc
c     u channel
cccccccccccccccccccccc
      q2=u
      lambda=q2/(q2-m2)

cccccccccccccccccccccccccccccc
c     Result obtained from Laenen&al. paper.
      A0= -(log(-s/q2))**2 -3*log(-s/q2) -8 -pi**2/3 
     $     + (-0.5*log(s/m2)**2 -5./2.*log(s/m2) 
     $     - 2*log(1-lambda)*log(s/m2) -6
     $     - 1/lambda*log(1-lambda) -(log(1-lambda))**2
     $     - 2*log(1-lambda) +2*st_tch_DDILOG(lambda) -pi**2/3)
      A1= (-3. -2*log(-s/q2)) 
     $     + (-5./2. -2.*log(1-lambda) -log(s/m2))

      prop=
     $     A0 
     $     + pi**2/2.           !from 'EulerGamma prefactor'
     $     + A1*log(mur2/s)
     $     - 3./2.*(log(mur2/s))**2
ccccccccccccccccccccccccccccc

c$$$      if(check_virt) then
c$$$c     from Campbell-Ellis, MCFM
c$$$c$$$      if     (nwz .eq. +1) then
c$$$c$$$        ub=+fac*virtqqb(1,2,3,4,5,6)
c$$$c$$$        bu=+fac*virtqqb(2,1,3,4,5,6)
c$$$c$$$        bubar=+fac*virtqqb(6,1,3,4,5,2)
c$$$c$$$        ubarb=+fac*virtqqb(6,2,3,4,5,1)
c$$$c$$$      elseif (nwz .eq. -1) then
c$$$c$$$        ub=fac*virtqqb(6,2,4,3,5,1)
c$$$c$$$        bu=fac*virtqqb(6,1,4,3,5,2)
c$$$c$$$        bubar=fac*virtqqb(2,1,4,3,5,6)
c$$$c$$$        ubarb=fac*virtqqb(1,2,4,3,5,6)
c$$$c$$$      endif
c$$$         call coefs(u,m2,cv0,cv,c1)
c$$$         prop_mcfm=cv0+cv
c$$$
c$$$         if(dabs(prop/prop_mcfm-1).gt.1d-6) then
c$$$            write(*,*) 'POW/MCFM ',prop/prop_mcfm
c$$$            call exit(1)
c$$$         endif
c$$$      endif

      prop=prop*vcf
      fvirt_dxd=prop*amp2_dxd !4
      fvirt_ud=prop*amp2_ud   !3

c     add part not proportional to born
      fvirt_dxd=fvirt_dxd+vcf *m2*t*s/u *log(m2/(m2-u)) /4.
     #* (u- wmass_pow**2)**(-2)
      fvirt_ud=fvirt_ud  +vcf *m2*s*t/u *log(m2/(m2-u)) /4.
     #* (u- wmass_pow**2)**(-2)


      end



c$$$      subroutine coefs(s12,mtsq,cv0,cv,c1)
c$$$      implicit none
c$$$!:      include 'src/Lib/st_tch/constants.f'
c$$$      include 'src/Lib/st_tch/pwhg_math.h'
c$$$!:      include 'src/Lib/st_tch/epinv.f'
c$$$
c$$$!:      include 'src/Lib/st_tch/epinv2.f'
c$$$
c$$$!:      include 'src/Lib/st_tch/scale.f'
c$$$      include 'src/Lib/st_tch/pwhg_st.h'
c$$$
c$$$!:      include 'src/Lib/st_tch/scheme.f'
c$$$
c$$$      
c$$$      double precision cv,cv0,Li2la
c$$$      double precision s12,mtsq,taucs,st_tch_DDILOG,eta,la,oml
c$$$      double complex lnrat,logoml,logla,xl12,logsca,Kfun,c1
c$$$
c$$$cccccccccccccccccccccccccccccccccccccc
c$$$c     !:
c$$$      double precision epinv
c$$$      double precision epinv2
c$$$      real *8 musq
c$$$      double complex cone
c$$$      real *8 pisqo6
c$$$      character*4 scheme
c$$$      external lnrat
c$$$
c$$$
c$$$      cone=(1d0,0d0)
c$$$      pisqo6=pi**2/6.
c$$$      scheme='tH-V'    
c$$$      epinv=0d0
c$$$      epinv2=0d0
c$$$      musq=st_muren2
c$$$cccccccccccccccccccccccccccccccccccccc
c$$$
c$$$      if (scheme .eq.'dred') then
c$$$C------        eta=0 4d-hel
c$$$         eta=0d0
c$$$      elseif (scheme .eq. 'tH-V') then
c$$$C------       eta=1 t'Hooft Veltman
c$$$         eta=1d0
c$$$      endif
c$$$
c$$$C**********************************************************************
c$$$C   Massless case
c$$$C   Taken from
c$$$C   %\cite{Altarelli:1979ub}
c$$$C   \bibitem{Altarelli:1979ub}
c$$$C   G.~Altarelli, R.~K.~Ellis and G.~Martinelli,
c$$$C   %``Large Perturbative Corrections To The Drell-Yan Process In QCD,''
c$$$C   Nucl.\ Phys.\ B {\bf 157}, 461 (1979).
c$$$C   %%CITATION = NUPHA,B157,461;%%
c$$$C   Using Eqn(58) with normalization changed to 
c$$$C   as/2/pi*cf*(4*pi)^ep/Gamma(1-ep) 
c$$$C   Taking account that Gamma(1-ep)^2/Gamma(1-2*ep)=1-ep^2*pi^2/6
c$$$C**********************************************************************
c$$$      xl12=lnrat(-s12,musq) 
c$$$      cv0=-2d0*epinv*(epinv2-dble(xl12))-dble(xl12**2)
c$$$     .           -3d0*(epinv-dble(xl12))-7d0-eta
c$$$
c$$$
c$$$
c$$$C---- this routine has been constructed following closely 
c$$$C---- the notation of
c$$$C---- %\cite{Gottschalk:1980rv}
c$$$C---- \bibitem{Gottschalk:1980rv}
c$$$C---- T.~Gottschalk,
c$$$C---- %``Chromodynamic Corrections To Neutrino Production Of Heavy Quarks,''
c$$$C---- Phys.\ Rev.\ D {\bf 23}, 56 (1981).
c$$$C---- %%CITATION = PHRVA,D23,56;%%
c$$$C----- Adapted from Eqs.(A8,A9)
c$$$
c$$$      taucs=s12-mtsq
c$$$      la=-s12/(mtsq-s12)
c$$$      oml=1d0-la
c$$$C-----oml=mtsq/(mtsq-s12)
c$$$      logoml=-lnrat(-taucs,mtsq)
c$$$      logsca=lnrat(-taucs,musq)
c$$$      Kfun=dcmplx(oml/la)*logoml
c$$$
c$$$c--- Minus sign relative to Gottschalk since incoming b has momentum
c$$$c--- vector reversed for the t-channel process
c$$$c--- s-channel process follows by crossing
c$$$      c1=-dcmplx(2d0)*Kfun
c$$$
c$$$      if (la .lt. 1d0) then
c$$$      Li2la=st_tch_DDILOG(la)
c$$$      else
c$$$      logla=lnrat(-s12,-taucs)
c$$$      Li2la=pisqo6-st_tch_DDILOG(oml)-dble(logla*logoml)
c$$$      endif
c$$$      cv=-epinv*epinv2
c$$$     . -epinv*(2.5d0+dble(logoml-logsca))
c$$$     . -0.5d0*(11d0+eta)-pisqo6+2d0*Li2la-dble(Kfun)
c$$$     .  -0.5d0*dble(logoml*(cone-logoml))
c$$$     .  +2.5d0*dble(logsca)+dble(logsca*logoml)-0.5d0*dble(logsca**2)
c$$$
c$$$      return
c$$$      end
c$$$
c$$$
c$$$      double complex function Lnrat(x,y)
c$$$************************************************************************
c$$$*     Author: R.K. Ellis                                               *
c$$$*     August, 1998.                                                    *
c$$$c     Lnrat(x,y)=log(x-i*ep)-log(y-i*ep)                               *
c$$$c     this function is hard-wired for sign of epsilon we must adjust   *
c$$$c     sign of x and y to get the right sign for epsilon                *
c$$$************************************************************************
c$$$      implicit none
c$$$!:      include 'src/Lib/st_tch/constants.f'
c$$$      include 'src/Lib/st_tch/pwhg_math.h'
c$$$      double precision x,y,htheta
c$$$C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
c$$$      htheta(x)=0.5+0.5*sign(1d0,x)
c$$$
c$$$
c$$$c$$$      real *8 half,one
c$$$c$$$      one=1d0
c$$$c$$$      half=0.5d0
c$$$
c$$$      double complex impi
c$$$      impi=(0d0,1d0)*pi
c$$$
c$$$
c$$$      Lnrat=dcmplx(dlog(abs(x/y)))-impi*dcmplx((htheta(-x)-htheta(-y)))
c$$$      return
c$$$      end

