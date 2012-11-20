c -*- Fortran -*-

c====================================================================
c
c  Define common block containing all coupling wbb_constants and wbb_masses
c  which are used in the HELAS routines.
c
c  These include wbb_masses, widths and real/complex couplings.
c
c  This file can be built automatically from particles.dat and
c  interactions.dat
c
c====================================================================
c
c     QCD 
c
      double complex       gg(2)
      double precision     g
      common /wbb_COUPL_QCD/   g,gg   
c
c    kinematical wbb_masses
c
      double precision     hmass, wmass, zmass, amass,
     &                     tmass, bmass, lmass, cmass
      common /wbb_COUPL_MASS/  hmass, wmass, zmass, amass,
     &                     tmass, bmass, lmass, cmass
c
c   widths
c
      double precision     hwidth, wwidth, zwidth, 
     &                     twidth, lwidth, awidth
      common /wbb_COUPL_WIDTH/ hwidth, wwidth, zwidth, 
     &                     twidth, lwidth, awidth
c
c  couplings in the feynman rules
c
      double complex       gal(2), gad(2), gau(2), gwf(2),
     &                     gzn(2), gzl(2), gzd(2), gzu(2)
      double precision     gw, gwwa, gwwz
      common /wbb_COUPL_GAUGE/ gal   , gad   , gau   , gwf   ,
     &                     gzn   , gzl   , gzd   , gzu   ,
     &                     gw, gwwa, gwwz
c
      double complex       gwfc(2),  gwfs(2), gwfm(2)
      common /wbb_coupl_ckm/   gwfc,     gwfs   , gwfm	
c
      double complex       gwwh, gzzh, gwwhh, gzzhh, ghhh, ghhhh
      common /wbb_COUPL_SCAL/  gwwh, gzzh, gwwhh, gzzhh, ghhh, ghhhh
c
      double complex       ghtop(2), ghbot(2), ghtau(2), ghcha(2)
      common /wbb_COUPL_YUK/   ghtop   , ghbot   , ghtau   , ghcha

