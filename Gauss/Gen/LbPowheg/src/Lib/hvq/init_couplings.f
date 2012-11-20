      subroutine hvq_init_couplings
      implicit none
      include 'src/Lib/hvq/pwhg_st.h'
      include 'src/Lib/hvq/pwhg_math.h'
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      real * 8 masswindow,qmass
      real * 8 hvq_powheginput
      external hvq_powheginput
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c      ph_Wmass  = 80.398d0
c      ph_Wwidth =  2.141d0

c      ph_alphaem = 1d0/128.930d0
c      ph_sthw2 = 0.2312d0
c      ph_sthw2 = 0.23102d0

      end

