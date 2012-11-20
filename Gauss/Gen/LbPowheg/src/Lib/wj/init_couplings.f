      subroutine wj_init_couplings
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/PhysPars.h'
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/pwhg_physpar.h'      
      real * 8 masswindow_low,masswindow_high,wj_powheginput
      external wj_powheginput
      logical verbose
      parameter(verbose=.true.)
      integer i,j
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_Wmass = wj_powheginput("#wj_wmass")
      if (ph_Wmass.le.0d0) ph_Wmass  = 80.398d0     
      ph_Wwidth = wj_powheginput("#Wwidth")
      if (ph_Wwidth.le.0d0) ph_Wwidth =  2.141d0

      ph_alphaem = wj_powheginput("#alphaem")
      if (ph_alphaem.le.0d0) ph_alphaem = 1d0/128.89d0
      ph_Zmass = wj_powheginput("#wj_zmass")
      if (ph_Zmass.le.0d0) ph_Zmass  = 91.1876d0     
      ph_Zwidth = wj_powheginput("#Zwidth")
      if (ph_Zwidth.le.0d0) ph_Zwidth =  2.4952d0
      ph_sthw2 = wj_powheginput("#sthw2")
      if (ph_sthw2.le.0d0) ph_sthw2 = abs(1d0-(ph_Wmass/ph_Zmass)**2)

c     CAVEAT: 
      ph_CKM(1,1) = wj_powheginput("#CKM_Vud")
      if (ph_CKM(1,1).le.0d0) ph_CKM(1,1)=0.975d0 
      ph_CKM(1,2) = wj_powheginput("#CKM_Vus")
      if (ph_CKM(1,2).le.0d0) ph_CKM(1,2)=0.222d0 
      ph_CKM(1,3) = wj_powheginput("#CKM_Vub")
      if (ph_CKM(1,3).le.0d0) ph_CKM(1,3)=1d-10
      ph_CKM(2,1) = wj_powheginput("#CKM_Vcd")
      if (ph_CKM(2,1).le.0d0) ph_CKM(2,1)=0.222d0 
      ph_CKM(2,2) = wj_powheginput("#CKM_Vcs")
      if (ph_CKM(2,2).le.0d0)  ph_CKM(2,2)=0.975d0 
      ph_CKM(2,3) = wj_powheginput("#CKM_Vcb")
      if (ph_CKM(2,3).le.0d0) ph_CKM(2,3)=1d-10
      ph_CKM(3,1) = wj_powheginput("#CKM_Vtd")
      if (ph_CKM(3,1).le.0d0) ph_CKM(3,1)=1d-10
      ph_CKM(3,2) = wj_powheginput("#CKM_Vts")
      if (ph_CKM(3,2).le.0d0) ph_CKM(3,2)=1d-10
      ph_CKM(3,3) = wj_powheginput("#CKM_Vtb")
      if (ph_CKM(3,3).le.0d0) ph_CKM(3,3)=1d0

c     number of light flavors
      st_nlight = 5

c wj_masses of light leptons for W decays:
      physpar_ml(1)=0.51099891d-3
      physpar_ml(2)=0.1056583668d0
      physpar_ml(3)=1.77684d0     

      

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      ph_Zmass2 = ph_Zmass**2
      ph_Wmass2 = ph_Wmass**2



c     set mass windows around W-mass peak in unit of ph_Wwidth
c     It is used in the generation of the Born phase space
C     masswindow is an optonal  parameter passed by the user
C     the default vale is 30 
      masswindow_low = wj_powheginput("#masswindow_low")
      if(masswindow_low.lt.0d0) masswindow_low=30d0
      ph_Wmass2low=max(0d0,ph_Wmass-masswindow_low*ph_Wwidth)
      ph_Wmass2low= ph_Wmass2low**2
      masswindow_high = wj_powheginput("#masswindow_high")
      if(masswindow_high.lt.0d0) masswindow_high=30d0
      ph_Wmass2high=ph_Wmass+masswindow_high*ph_Wwidth
      ph_Wmass2high= min(kn_sbeams,ph_Wmass2high**2)
      ph_WmWw = ph_Wmass * ph_wwidth

      ph_unit_e = sqrt(4*pi*ph_alphaem)

      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'Z mass = ',ph_Zmass
      write(*,*) 'Z width = ',ph_Zwidth
      write(*,*) 'W mass = ',ph_Wmass
      write(*,*) 'W width = ',ph_Wwidth
      write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) '(unit_e)^2 = ',ph_unit_e**2   
      write(*,*) '(g_w)^2 = ',ph_unit_e*ph_unit_e/ph_sthw2   
      write(*,*) 'wj_CKM matrix' 
      do i=1,3
         write(*,*) (ph_CKM(i,j),j=1,3)
      enddo
      write(*,*) '*************************************'
      write(*,*)
      write(*,*) '*************************************'
      write(*,*) sqrt(ph_Wmass2low),'< M_W <',sqrt(ph_Wmass2high)
      write(*,*) '*************************************'
      endif
      end




