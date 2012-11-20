      subroutine zj_init_couplings
      implicit none
      include 'src/Lib/zj/PhysPars.h'
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/pwhg_flg.h'
      include 'src/Lib/zj/pwhg_physpar.h'
      real * 8 masswindow_low,masswindow_high
      flg_withdamp=.true.
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_Zmass  = 91.188d0     
      ph_Zwidth =  2.486d0

      ph_alphaem = 1d0/128.930d0
      ph_sthw2 = 0.2312d0

c     number of light flavors
      st_nlight = 5
c zj_masses of light leptons for Z decays:
      physpar_ml(1)=0.511d-3
      physpar_ml(2)=0.1057d0
      physpar_ml(3)=1.777d0

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      ph_Zmass2 = ph_Zmass**2

c     set mass windows around Z-mass peak in unit of ph_Zwidth
c     It is used in the generation of the Born phase space
      masswindow_low = 25
      masswindow_high = 35
      ph_Zmass2low=max(0d0,ph_Zmass-masswindow_low*ph_Zwidth)
      ph_Zmass2low=ph_Zmass2low**2
      ph_Zmass2high=(ph_Zmass+masswindow_high*ph_Zwidth)**2
      ph_ZmZw = ph_Zmass * ph_Zwidth
      ph_unit_e = sqrt(4*pi*ph_alphaem)

      end



