c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine st_sch_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/st_sch/nlegborn.h'
      include 'src/Lib/st_sch/pwhg_math.h'
      include 'src/Lib/st_sch/pwhg_st.h'
      include 'src/Lib/st_sch/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer vflav(nleg)
      real * 8 virtual

ccccccccccccccccccccc
      integer mu,ileg
      real *8 kblab(0:3,nleg)
      real *8 ewcoupl,s,t,u
      real *8 fvirt_udx,fvirt_dxu
      real *8 st_sch_dotp
      external st_sch_dotp

      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/

      real *8 ckm_b_s
ccccccccccccccccccccc

ccccccccccccccc
      include 'src/Lib/st_sch/pwhg_flst.h'
      integer i_fb
c     To pass the already subtracted virtuals to sigsoftvirt
      real *8 fksfinite(1000)
      common/st_sch_cfksfinite/fksfinite
ccccccccccccccc

      integer vflav_loc(nleg)

c     check
      if (abs(vflav(3)).ne.6) then
         write(*,*) 'st_sch_setvirtual: ERROR in flavor assignement'
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
      s=2d0*st_sch_dotp(kblab(0,1),kblab(0,2))
      t=st_sch_dotp(kblab(0,3),kblab(0,3)) - 2d0*st_sch_dotp(kblab(0,1),kblab(0,3))
      u=st_sch_dotp(kblab(0,3),kblab(0,3))-s-t

ccccccccccccccccccccccccccccccccccccccccccc
c     >>> S CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc

c     Evaluate finite part of virtual contribution
      call st_sch_virt_finite(s,t,u,st_sch_dotp(kblab(0,3),kblab(0,3)),
     #fvirt_udx,fvirt_dxu,st_muren2)

!      write(*,*) 'virtual: ',s,t,fvirt_udx,fvirt_dxu

c     udx
      if ((three_ch(vflav_loc(1)).eq.2).and.
     #(three_ch(vflav_loc(2)).eq.1)) then
         virtual=fvirt_udx
         ckm_b_s=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(2)))**2 
     #* CKM(abs(vflav_loc(3)),abs(vflav_loc(4)))**2 

c     dxu
      elseif ((three_ch(vflav_loc(1)).eq.1).and.
     #(three_ch(vflav_loc(2)).eq.2)) then
         virtual=fvirt_dxu
         ckm_b_s=
     #CKM(abs(vflav_loc(1)),abs(vflav_loc(2)))**2 
     #* CKM(abs(vflav_loc(3)),abs(vflav_loc(4)))**2

      else
         write(*,*) 'Error in st_sch_setvirtual, (s)'
         call exit(1)
      endif

ccccccccccccccccccccccc
c     assign output
      virtual=virtual *ewcoupl**2 *ckm_b_s
cccccccccccccccccccccc
      end



c     Output the finite part of virtual contribution.
c     Finite parts for s-channel single-top.
c     Virtual contributions are taken from Laenen-Weinzierl-Sullivan...
c     paper, after a manipulation analogous to the
c     one described in the t-channel note section.
c     An analytic continuation is also needed in this case.
c     Prefactor of FKS is different, so an expansion of
c     EulerGamma functions was needed.
c     Moreover, nontrivial logaritms arise because of
c     the presence of (mu^2/s) in front of the result.
      subroutine st_sch_virt_finite(s,t,u,m2,fvirt_udx,fvirt_dxu,mur2)
      implicit none
      real *8 s,t,u,m2,fvirt_udx,fvirt_dxu,mur2
      include 'src/Lib/st_sch/pwhg_math.h'
      include 'src/Lib/st_sch/PhysPars.h'

      real *8 lambda,q2,prop
      real *8 amp2_udx,amp2_dxu

      real *8 vcf
      parameter (vcf=4.d0/3.d0)

      real *8 st_sch_DDILOG,A0,A1

      real *8 A0h_s,A0l_s,A1h_s,A1l_s,A2h_s,A2l_s

      real *8 cv0,cv,prop_mcfm,prop_ztop
      double complex c1

      logical check_virt
      parameter (check_virt=.false.)

      if(dabs(m2/topmass_pow**2 -1.).gt.1d-6) then
         write(*,*) 'st_sch_virt_finite: problem with top offshelness'
         call exit(1)
      endif

      amp2_udx=u*(u-topmass_pow**2)/(s-wmass_pow**2)**2/4
      amp2_dxu=t*(t-topmass_pow**2)/(s-wmass_pow**2)**2/4

      if(s+t+u-m2.gt.1d-7) then
         print*, 'st_sch_virt_finite: problem with invariants'
         call exit(1)
      endif


cccccccccccccccccccccccc
c     s-channel
cccccccccccccccccccccccc
      lambda=s/(s-m2)


ccccccccccccccccccccccccccccc
c     This result has been obtained by crossing
c     the result in the paper by Laenen&al.
c     Several checks have been performed.
c      A2l=-2.
c      A1l=-3.
c      A0l=pi**2-8.-pi**2/3.
c     After having factorized the same prefactor
c     of Altarelli-Ellis-Martinelli, that corresponds
c     to 1/Ga[1-ep] up to order O(ep^3), we have
      A2l_s=-2.
      A1l_s=-3.
      A0l_s=(pi**2 -8. -pi**2/3.) 
     $     + pi**2/3.           !from 'EulerGamma prefactor'
c     A2h=-1.
c     A1h=-5./2. -2.*log(m2/(s-m2)) -log(s/m2)
c     A0h=....
c     After having factorized the same prefactor
c     of Altarelli-Ellis-Martinelli, that corresponds
c     to 1/Ga[1-ep] up to order O(ep^3), we have
      A2h_s=-1.
      A1h_s=-5./2. -2.*log(m2/(s-m2)) -log(s/m2)
      A0h_s=(
     $     -0.5*(log(s/m2))**2 -5./2.*log(s/m2) 
     $     - 2*log(m2/(s-m2))*log(s/m2) -6. 
     $     -(s-m2)/s*log(m2/(s-m2)) -(log(m2/(s-m2)))**2
     $     +pi**2 -2*log(m2/(s-m2))
     $     +2*st_sch_DDILOG(lambda) -pi**2/3.)
     $     +pi**2/6.            !from 'EulerGamma prefactor'

      prop= (A0l_s+A0h_s)
     $     +(A1l_s+A1h_s) *log(mur2/s)
     $     +(A2l_s+A2h_s) *0.5*(log(mur2/s))**2
ccccccccccccccccccccccccccccc

c$$$      if(check_virt) then
c$$$c     A0 from ZTOP, s-channel
c$$$         A0= 
c$$$     $        + (-0.5*(log(s/m2))**2 -5./2.*log(s/m2) 
c$$$     $        - 2*log(m2/(s-m2))*log(s/m2) -3.*log(m2/(s-m2))
c$$$     $        + m2/s*log(m2/(s-m2)) -(log(m2/(s-m2)))**2
c$$$     $        +2*st_sch_DDILOG(lambda))
c$$$     $        - 14. +pi**2 +pi**2/3.
c$$$c     A1, by crossing the Laenen&al result
c$$$         A1= -11./2. -2.*log(m2/(s-m2)) -log(s/m2)
c$$$c     from previous A0 and A1
c$$$         prop_ztop=
c$$$     $        A0 
c$$$     $        + pi**2/2.        !from different EulerGamma
c$$$     $        + A1*log(mur2/s)
c$$$     $        - 3./2.*(log(mur2/s))**2
c$$$
c$$$c     from Campbell-Ellis, MCFM
c$$$c     Notice that the use of lnrat is crucial in order
c$$$c     to get all pi's from analytical continuation
c$$$         call coefs(s,m2,cv0,cv,c1)
c$$$         prop_mcfm=cv0+cv
c$$$
c$$$         if(dabs(prop/prop_mcfm-1).gt.1d-6) then
c$$$            write(*,*) '>>>>> POW/MCFM, light ',
c$$$     $           (A0l_s+A1l_s*log(mur2/s)+A2l_s*0.5*(log(mur2/s))**2)/cv0
c$$$
c$$$            write(*,*) '>>>>> POW/MCFM, heavy ',
c$$$     $     (A0h_s+A1h_s*log(mur2/s)+A2h_s*0.5*(log(mur2/s))**2)/cv
c$$$
c$$$            write(*,*) 'POW/MCFM ',prop/prop_mcfm
c$$$            call exit(1)
c$$$         endif
c$$$         if(dabs(prop/prop_ztop-1).gt.1d-6) then
c$$$            write(*,*) 'POW/ZTOP ',prop/prop_ztop
c$$$            call exit(1)
c$$$         endif
c$$$      endif

      prop=prop*vcf
      fvirt_udx=prop*amp2_udx   !1
      fvirt_dxu=prop*amp2_dxu   !3

c     add part not proportional to born
      fvirt_udx=fvirt_udx  +vcf *m2*t*u/s *log((s-m2)/m2) /4.
     $     * (s- wmass_pow**2)**(-2)
      fvirt_dxu=fvirt_dxu  +vcf *m2*u*t/s *log((s-m2)/m2) /4.
     $     * (s- wmass_pow**2)**(-2)
      
      end



c$$$      subroutine coefs(s12,mtsq,cv0,cv,c1)
c$$$      implicit none
c$$$!:      include 'src/Lib/st_sch/constants.f'
c$$$      include 'src/Lib/st_sch/pwhg_math.h'
c$$$!:      include 'src/Lib/st_sch/epinv.f'
c$$$
c$$$!:      include 'src/Lib/st_sch/epinv2.f'
c$$$
c$$$!:      include 'src/Lib/st_sch/scale.f'
c$$$      include 'src/Lib/st_sch/pwhg_st.h'
c$$$
c$$$!:      include 'src/Lib/st_sch/scheme.f'
c$$$
c$$$      
c$$$      double precision cv,cv0,Li2la
c$$$      double precision s12,mtsq,taucs,st_sch_DDILOG,eta,la,oml
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
c$$$      Li2la=st_sch_DDILOG(la)
c$$$      else
c$$$      logla=lnrat(-s12,-taucs)
c$$$      Li2la=pisqo6-st_sch_DDILOG(oml)-dble(logla*logoml)
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
c$$$!:      include 'src/Lib/st_sch/constants.f'
c$$$      include 'src/Lib/st_sch/pwhg_math.h'
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
