      subroutine wbb_bornzerodamp(alr,r0,rc,rs,dampfac)
c given the R_alpha region (i.e. the alr) and the associated
c real contribution r (without pdf factor),
c returns in dampfac the damping factor to be applied to
c the real contribution to implement Born zero suppression
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/pwhg_flg.h'
      integer alr
      real * 8 r0,rc,rs,dampfac,h,wbb_pwhg_pt2,pt2,wbb_powheginput
      logical ini
      data ini/.true./
      save ini,h
      external wbb_pwhg_pt2,wbb_powheginput
      integer ig,ib,ibbar
      real * 8 par,dg,db,dbbar,Eg,Eb,Ebbar,y
      parameter (par=1d0)
      real * 8 wbb_dotp
      external wbb_dotp

      if(ini) then
c     for Wbb production set h > 0
         h = 1d0
c        h=wbb_powheginput("#hfact")
         if(h.gt.0) then
            write(*,*)'**********************************************'
            write(*,*)' Using the separation of the real contribution'
            write(*,*)'**********************************************'
         endif
         ini=.false.
      endif
      if(h.gt.0) then
c     compute damping factor only if radiated parton is a gluon
         if (flst_alr(nlegreal,alr).ne.0) then
            dampfac=1
         else
            ig = nlegreal
            ib = 4
            ibbar = 5
            Eg = kn_cmpreal(0,ig)
            Eb = kn_cmpreal(0,ib)
            Ebbar = kn_cmpreal(0,ibbar)
c     compute cos of the angle between radiated gluon and incoming beam plus direction
            y = 1 - wbb_dotp(kn_cmpreal(0,1),kn_cmpreal(0,ig))/
     $           (kn_cmpreal(0,1)*Eg)
            dg = Eg**2*(1-y**2)
            dg = dg**par
c     compute factor for b quark                        
            db = wbb_dotp(kn_cmpreal(0,ig),kn_cmpreal(0,ib))*
     $           Eg*Eb/(Eg+Eb)**2 * (1+kn_masses(ib)/Eg)**2
            db = db**par
c     compute factor for b antiquark                        
            dbbar = wbb_dotp(kn_cmpreal(0,ig),kn_cmpreal(0,ibbar))*
     $           Eg*Ebbar/(Eg+Ebbar)**2 * (1+kn_masses(ibbar)/Eg)**2
            dbbar = dbbar**par
            dampfac = 1/dg/(1/dg+1/db+1/dbbar)
         endif
      else
         dampfac=1
      endif

      if(r0.gt.5*rc.and.r0.gt.5*rs) then
         dampfac=0
      endif
c      write(*,*) ' wbb_bornzerodamp: r0=',r0,'  rc=',rc,'  rs=',rs,
c     # ' dampfac=',dampfac
      end
