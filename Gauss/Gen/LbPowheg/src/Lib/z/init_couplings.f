      subroutine z_init_couplings
      implicit none
      include 'src/Lib/z/PhysPars.h'
      include 'src/Lib/z/pwhg_st.h'
      include 'src/Lib/z/pwhg_math.h'
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_kn.h'
      real * 8 masswindow_low,masswindow_high
      real * 8 z_powheginput
      external z_powheginput
      logical verbose
      parameter(verbose=.true.)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_alphaem = z_powheginput("#alphaem")
      if (ph_alphaem.le.0d0) ph_alphaem = 1d0/128.89d0
      ph_Zmass = z_powheginput("#Zmass")
      if (ph_Zmass.le.0d0) ph_Zmass  = 91.1876d0     
      ph_Zwidth = z_powheginput("#Zwidth")
      if (ph_Zwidth.le.0d0) ph_Zwidth =  2.4952d0
     
      ph_Wmass = z_powheginput("#Wmass")
      if (ph_Wmass.le.0d0) ph_Wmass  = 80.398d0     
      ph_Wwidth = z_powheginput("#Wwidth")
      if (ph_Wwidth.le.0d0) ph_Wwidth =  2.141d0

      ph_sthw2 = z_powheginput("#sthw2")
      if (ph_sthw2.le.0d0) ph_sthw2 = abs(1d0-(ph_Wmass/ph_Zmass)**2)


c     number of light flavors
      st_nlight = 5

c     mass window
      masswindow_low = z_powheginput("#masswindow_low")
      if (masswindow_low.le.0d0) masswindow_low=30d0
      masswindow_high = z_powheginput("#masswindow_high")
      if (masswindow_high.le.0d0) masswindow_high=30d0

      
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      ph_Zmass2 = ph_Zmass**2

c     set mass windows around Z-mass peak in unit of ph_Zwidth
c     It is used in the generation of the Born phase space
      ph_Zmass2low=max(0d0,ph_Zmass-masswindow_low*ph_Zwidth)
      if (ph_Zmass2low.lt.1d0) then
         write(*,*) '*************************************'
         write(*,*) 'WARNING: Z virtuality cutoff at 1 GeV'
         write(*,*) '         to avoid the photon pole    '
         write(*,*) '*************************************'
         ph_Zmass2low=1d0
      endif
      ph_Zmass2low=ph_Zmass2low**2
      ph_Zmass2high=ph_Zmass+masswindow_high*ph_Zwidth
      ph_Zmass2high=min(kn_sbeams,ph_Zmass2high**2)
      ph_ZmZw = ph_Zmass * ph_Zwidth
      ph_unit_e = sqrt(4*pi*ph_alphaem)

      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'Z mass = ',ph_Zmass
      write(*,*) 'Z width = ',ph_Zwidth
      write(*,*) 'W mass = ',ph_Wmass
      write(*,*) 'W width = ',ph_Wwidth
      write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) '*************************************'
      write(*,*)
      write(*,*) '*************************************'
      write(*,*) sqrt(ph_Zmass2low),'< M_Z <',sqrt(ph_Zmass2high)
      write(*,*) '*************************************'
      endif

      end



