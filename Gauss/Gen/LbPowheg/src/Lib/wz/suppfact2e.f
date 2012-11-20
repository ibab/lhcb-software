
      function wz_suppfact2e(pin,flav)
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/vvsettings.h'
      real * 8  wz_suppfact2e
      real * 8 pin(0:3,nlegborn)
      integer flav(nlegborn)
      include 'src/Lib/wz/PhysPars.h'
      real * 8 pcombz1(0:3),pcombz2(0:3),mz1,mz2
      real * 8 pcombw1(0:3),pcombw2(0:3),mw1,mw2
      real * 8 f1,f2
c     identical lepton 1, identical lepton 2, different lepton
      integer nu,wz_L1,wz_L2,dl
      real * 8 wz_dotp
      external wz_dotp
      if(.not.interference) then
         wz_suppfact2e=1
         return
      endif
      if(2*(flav(3)/2).eq.flav(3)) then
         nu=3
         wz_L1=4
      else
         nu=4
         wz_L1=3
      endif
      if(abs(flav(nu)+flav(wz_L1)).ne.1) then
         write(*,*) 'wz_suppfact2e: error: wrong flavour structure'
         call exit(-1)
      endif
      if(flav(5).eq.flav(wz_L1)) then
         wz_L2=5
         dl=6
      elseif(flav(6).eq.flav(wz_L1)) then
         wz_L2=6
         dl=5
      else
         wz_suppfact2e=1
         return
      endif
      if(flav(wz_L1).ne.flav(wz_L2).or.flav(dl)+flav(wz_L1).ne.0) then
         write(*,*) 'wz_suppfact2e: error: wrong flavour structure'
      endif
      pcombz1=pin(:,wz_L1)+pin(:,dl)
      pcombw1=pin(:,nu)+pin(:,wz_L2)
      pcombz2=pin(:,wz_L2)+pin(:,dl)
      pcombw2=pin(:,nu)+pin(:,wz_L1)
      mz1=wz_dotp(pcombz1,pcombz1)
      mz2=wz_dotp(pcombz2,pcombz2)
      mw1=wz_dotp(pcombw1,pcombw1)
      mw2=wz_dotp(pcombw2,pcombw2)
      f1=mz1*((mz1-ph_Zmass2)**2+ph_Zwidth**2*ph_Zmass2)
     1      *((mw1-ph_Wmass2)**2+ph_Wwidth**2*ph_Wmass2)
      f2=mz2*((mz2-ph_Zmass2)**2+ph_Zwidth**2*ph_Zmass2)
     1      *((mw2-ph_Wmass2)**2+ph_Wwidth**2*ph_Wmass2)
      wz_suppfact2e=2*f1**2/(f1**2+f2**2)
      end
