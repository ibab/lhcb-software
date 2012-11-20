      subroutine wbb_initialize
!******************************************************************
!     sets up wbb_masses and coupling wbb_constants for Helas
!******************************************************************
      implicit none
      include 'src/Lib/wbb/PhysPars.h'
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_kn.h'      
      include "coupl.inc"
c
c     local
c
      double precision  v,lambda
      double precision  ee, ee2, ez, ey, sw, cw, sc2, sin2w, wm
      double precision  alpha,gfermi
c
c wbb_constants
c
      double complex  ci
      parameter( ci = ( 0.0d0, 1.0d0 ) )
      double precision  Zero, One, Two, Three, Four, Half, Rt2
      parameter( Zero = 0.0d0, One = 1.0d0, Two = 2.0d0 )
      parameter( Three = 3.0d0, Four = 4.0d0, Half = 0.5d0 )
      parameter( Rt2   = 1.41421356237309504880168872421d0 )
      double precision  Pi, Fourpi
      parameter( Pi = 3.14159265358979323846d0 )
      parameter( Fourpi = Four * Pi )
c     
!  QCD couplings
c  in "couplings.f" (Madgraph4) G = dsqrt(4d0*pi*alphas)
c  but we keep the multiplication by the strong coupling separate
c
      G = 1d0
      GG(1)=-g
      GG(2)=-g
c
! wbb_masses
c read in the powheg values (included via PhysPars.h)
c
      hmass=ph_Hmass
      wmass=ph_Wmass
      zmass=ph_Zmass
      amass=0d0
      tmass=ph_mt
      bmass=ph_mb
      lmass=ph_lmass
      cmass=0d0
c
! widths
c read in the powheg values (included via PhysPars.h)
c      
      hwidth=ph_Hwidth
      wwidth=ph_Wwidth
      zwidth=ph_Zwidth
      twidth=1.50833649E+00 ! value in (MyProcDir/Cards/param_card.dat)
      lwidth=0d0
      awidth=0d0

      sw  = ph_sthw
      alpha=ph_alphaem

c
! auxiliary local values
c
      ee2 = alpha * Fourpi
      ee  = sqrt( ee2 )
      gfermi=1.16639000E-05  ! value in (MyProcDir/Cards/param_card.dat)  
      wm = sqrt(zmass**2/Two+
     $     sqrt(zmass**4/Four-Pi/Rt2*alpha/gfermi*zmass**2))
      sin2w  = sw**2
      cw  = sqrt( One - sin2w )
      sc2 = sin2w*( One - sin2w )
      ee2 = ee**2
      ez  = ee/(sw*cw)
      ey  = ee*(sw/cw)
      v   = Two*wm*sw/ee   ! the wmass is used to calculate v
      lambda = hmass**2 / (Two * v**2)
c
! vector boson couplings
c
      gw   = ee/sw
      gwwa = ee
      gwwz = ee*cw/sw
c
! gauge & higgs boson coupling wbb_constants
c
      gwwh  = dcmplx( ee2/sin2w*Half*v, Zero )
      gzzh  = dcmplx( ee2/sc2*Half*v, Zero )
      ghhh  = dcmplx( -hmass**2/v*Three, Zero )
      gwwhh = dcmplx( ee2/sin2w*Half, Zero )
      gzzhh = dcmplx( ee2/sc2*Half, Zero)
      ghhhh = ghhh/v
c
! fermion-fermion-vector couplings
c
      gal(1) = dcmplx(  ee          , Zero )
      gal(2) = dcmplx(  ee          , Zero )
      gau(1) = dcmplx( -ee*Two/Three, Zero )
      gau(2) = dcmplx( -ee*Two/Three, Zero )
      gad(1) = dcmplx(  ee/Three    , Zero )
      gad(2) = dcmplx(  ee/Three    , Zero )

      gwf(1) = dcmplx( -ee/sqrt(Two*sin2w), Zero )
      gwf(2) = dcmplx(  Zero              , Zero )

      gzn(1) = dcmplx( -ez*Half                     , Zero )
      gzn(2) = dcmplx(  Zero                        , Zero )
      gzl(1) = dcmplx( -ez*(-Half + sin2w)          , Zero )
      gzl(2) = dcmplx( -ey                          , Zero )
      gzu(1) = dcmplx( -ez*( Half - sin2w*Two/Three), Zero )
      gzu(2) = dcmplx(  ey*Two/Three                , Zero )
      gzd(1) = dcmplx( -ez*(-Half + sin2w/Three)    , Zero )
      gzd(2) = dcmplx( -ey/Three                    , Zero )
c      
      return
      end


