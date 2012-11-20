      subroutine gg_h_init_couplings
      implicit none
      include 'src/Lib/gg_h/PhysPars.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_math.h'    
      include 'src/Lib/gg_h/nlegborn.h'      
      include 'src/Lib/gg_h/pwhg_kn.h'      
      real * 8 masswindow_low,masswindow_high
      logical verbose
      parameter(verbose=.true.)
      integer i,j
      real *8 gg_h_powheginput
      external gg_h_powheginput
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_Hmass = gg_h_powheginput('hmass')
      ph_Hwidth = gg_h_powheginput('hwidth')


      ph_GF= gg_h_powheginput('#gfermi') 
      if (ph_GF.le.0d0) ph_GF  = 0.116639D-04     
      ph_topmass = gg_h_powheginput('#topmass')
      if (ph_topmass.le.0d0) ph_topmass  = 171.3d0
      ph_bmass = gg_h_powheginput('#bmass')
      if (ph_bmass.le.0d0) ph_bmass  = 4.55d0
      ph_alphaem = gg_h_powheginput("#alphaem")
      if (ph_alphaem.le.0d0) ph_alphaem = 1d0/137.035999679d0
      ph_Zmass = gg_h_powheginput("#Zmass")
      if (ph_Zmass.le.0d0) ph_Zmass  = 91.1876d0     
      ph_Zwidth = gg_h_powheginput("#Zwidth")
      if (ph_Zwidth.le.0d0) ph_Zwidth =  2.4952d0
      ph_Wmass = gg_h_powheginput("#Wmass")
      if (ph_Wmass.le.0d0) ph_Wmass  = 80.398d0     
      ph_Wwidth = gg_h_powheginput("#Wwidth")
      if (ph_Wwidth.le.0d0) ph_Wwidth =  2.141d0      
      ph_sthw2 = gg_h_powheginput("#sthw2")
      if (ph_sthw2.le.0d0) ph_sthw2 = abs(1d0-(ph_Wmass/ph_Zmass)**2)
c     number of light flavors
      st_nlight = 5


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      
      ph_Hmass2 = ph_Hmass**2

c     set gg_h_mass windows around H-gg_h_mass peak in unit of ph_Hwidth
c     It is used in the generation of the Born phase space
C     masswindow is an optonal  parameter passed by the user
C     the default vale is 10 
      masswindow_low = gg_h_powheginput("#masswindow_low")
      if(masswindow_low.lt.0d0) masswindow_low=10d0
      ph_Hmass2low=max(0d0,ph_Hmass-masswindow_low*ph_Hwidth)
      ph_Hmass2low= ph_Hmass2low**2
      masswindow_high = gg_h_powheginput("#masswindow_high")
      if(masswindow_high.lt.0d0) masswindow_high=10d0
      ph_Hmass2high=ph_Hmass+masswindow_high*ph_Hwidth
      ph_Hmass2high= min(kn_sbeams,ph_Hmass2high**2)
      ph_HmHw = ph_Hmass * ph_Hwidth


      ph_unit_e = sqrt(4*pi*ph_alphaem)

      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'H gg_h_mass = ',ph_Hmass
      write(*,*) 'H width = ',ph_Hwidth
       write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) 'GF = ',ph_GF
      write(*,*) 'top gg_h_mass = ',ph_topmass
      write(*,*) '*************************************'
      write(*,*)
      write(*,*) '*************************************'
      write(*,*) sqrt(ph_Hmass2low),' < M_H <',sqrt(ph_Hmass2high)
      write(*,*) '*************************************'
      endif
      end




