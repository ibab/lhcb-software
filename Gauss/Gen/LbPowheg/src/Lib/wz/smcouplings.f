      subroutine wz_smcouplings
      implicit none
      include 'src/Lib/wz/PhysPars.h'
      include 'src/Lib/wz/pwhg_math.h'
      include 'src/Lib/wz/pwhg_st.h'
      real * 8 deltas,asmzopi,asmwopi
      real * 8 wz_pwhg_alphas
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c PDG 7-7-2011
      ph_Zmass  = 91.1876d0
c PDG 7-7-2011
      ph_Wmass  = 80.399d0     
c PDG 7-7-2011, SM Review (Langaker)
      ph_gmu    = 1.166364d-5

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_Zmass2 = ph_Zmass**2
      ph_Wmass2 = ph_Wmass**2

      ph_cthw = ph_Wmass/ph_Zmass
      ph_cthw2 = ph_cthw**2
      ph_sthw2 = abs(1d0-ph_cthw2)
      ph_sthw = sqrt(ph_sthw2)

      ph_alphaem = sqrt(2d0)*ph_gmu*ph_Wmass2/pi*ph_sthw2

c alphas(Mz) from PDG review 7-7-2011      
      asmzopi=wz_pwhg_alphas(ph_Zmass2,st_lambda5MSB,5)/pi
      asmwopi=wz_pwhg_alphas(ph_Wmass2,st_lambda5MSB,5)/pi

c from PDG, Langaker review 7-7-2011
c with contributions up to as**4 then ph_Wwidth = 2.099, ph_Zwidth = 2.511
c      deltas=asmzopi+1.409*asmzopi**2-12.77*asmzopi**3-80*asmzopi**4
c with contributions up to as**1 then ph_Wwidth = 2.098, ph_Zwidth = 2.509
      deltas=asmwopi

c     ph_Wwidth =  2.085 +- 0.042, PDG 7-7-2011
c 2.0995 with this formula, 0.7% difference, 0.34 standard deviations
      ph_Wwidth = ph_gmu*ph_Wmass**3/(6*sqrt(2d0)*pi)
     1 *(3+3*2*(1+deltas))

      deltas=asmzopi
c     ph_Zwidth = 2.4952 +- 0.0023, PDG 7-7-2011
c 2.5106 with this formula, 0.6% differemnce, 6.7 standard deviations
      ph_Zwidth = ph_gmu*ph_Zmass**3/(6*sqrt(2d0)*pi)*(
c gv=t3L-2q sin^2 thetaW, ga=t3L
c neutrinos
     1     3*((0.5d0)**2+(0.5d0)**2)  +
c leptons
     2     3*((-0.5d0+2*ph_sthw2)**2+(-0.5d0)**2) +
     3    (
c up quarks
     4     3*2*((0.5d0-2*2d0/3d0*ph_sthw2)**2+(0.5d0)**2) +
c down quarks
     5     3*3*((-0.5d0+2*1d0/3d0*ph_sthw2)**2+(-0.5d0)**2) 
     6    )*(1+deltas) )
c
      write(*,*) ' 1/alpha',1/ph_alphaem
      write(*,*) ' W width',ph_Wwidth
      write(*,*) ' Z width',ph_Zwidth

      ph_ZmZw = ph_Zmass * ph_Zwidth
      ph_unit_e = sqrt(4*pi*ph_alphaem)

      ph_WmWw = ph_Wmass * ph_Wwidth

      end
