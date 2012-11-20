      subroutine wbb_init_couplings
      implicit none
      include 'src/Lib/wbb/PhysPars.h'
      include 'src/Lib/wbb/pwhg_st.h'
      include 'src/Lib/wbb/pwhg_math.h'
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/pwhg_flg.h'
      include 'src/Lib/wbb/pwhg_rad.h'
      real * 8 masswindow
      logical ini
      data ini/.true./
      real * 8 wbb_powheginput
      external wbb_powheginput
      integer vdecaymode
      real * 8 bmass

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_Zmass  = 91.1876d0
      ph_Zwidth =  2.4952d0

      ph_Wmass  =  80.41d0
      ph_Wwidth =  2.141d0      
      
      bmass=wbb_powheginput('#bmass')
      if (bmass.lt.0d0) then
         ph_mb=4.62d0
      else
         ph_mb=bmass
      endif

      ph_mt=173.1d0
c     
      vdecaymode=wbb_powheginput('vdecaymode')
      if (vdecaymode.eq.0) then
c     decay into MASSLESS electron
         ph_lmass = 0
      elseif (vdecaymode.eq.1) then
c     decay into electron
         ph_lmass = 0.511d-3    ! electron mass
      elseif (vdecaymode.eq.2) then
c     decay into muon 
         ph_lmass = 105.65d-3   ! muon mass
      elseif (vdecaymode.eq.3) then
c     decay into tau
         ph_lmass = 1776.82d-3   ! muon mass
      endif

c     set branching ratio according to the choosen W decay channel
c     used only in LH event file
      rad_branching=wbb_powheginput('#Wbranching')
      if (rad_branching.lt.0d0) then
         rad_branching = 10.8d-2
      endif

c      ph_Hmass  = 100d0
c      ph_Hwidth = 0.0033d0
c      ph_Hmass  = 120d0
c      ph_Hwidth = 0.00437d0 
c      ph_Hmass  = 150d0
c      ph_Hwidth = 0.0182d0
c      ph_Hmass  = 300d0
c      ph_Hwidth = 8.4086d0
c      ph_Hmass  = 400d0
c      ph_Hwidth = 29.42d0 

      ph_Hmass  = 300d0
      ph_Hwidth = 8.445d0 


      ph_sthw2 = 0.223d0

c     gw^2 = 8 * mW^2 * G_F/sqrt(2)
c     e = gw * sin th_W

      ph_alphaem = 1d0/132.088832d0

c     number of light flavors
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c     if false, DO NOT perform the check that there are no coloured
c     light partons before flst_lightpart. 
      flg_lightpart_check=.false.
c      In this way we can treat the final-state b as massive (there are
c      no b in the initial-state and no initial-state gluons), so
c      st_nlight enters only in the evaluation of alpha_s, that is then
c      computed in the MSbar scheme with 5 light flavours.
      st_nlight = 5
      if (ini) then
         write(*,*) '**********************************************'
         write(*,*) '**********************************************'
         write(*,*) '  number of light flavours = ', st_nlight
         write(*,*) '**********************************************'
         write(*,*) '**********************************************'
         ini=.false.
      endif


c      ph_CKM(1,1)=0.9748 	
c      ph_CKM(1,2)=0.2225  	 
c      ph_CKM(1,3)=0.0036  	
c      ph_CKM(2,1)=0.2225  	
c      ph_CKM(2,2)=0.9740 	
c      ph_CKM(2,3)=0.041	
c      ph_CKM(3,1)=0.009    
c      ph_CKM(3,2)=0.0405   
c      ph_CKM(3,3)=0.9992

      ph_CKM(1,1)=0.974d0 	
      ph_CKM(1,2)=0.227d0  	 
      ph_CKM(1,3)=0d0
      ph_CKM(2,1)=0.227d0
      ph_CKM(2,2)=0.9740 	
      ph_CKM(2,3)=0d0	
      ph_CKM(3,1)=0d0    
      ph_CKM(3,2)=0d0
      ph_CKM(3,3)=1d0



c     wbb_initialize CKM with flavor indexes
      call wbb_inizialize_ph_CKM_matrix

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      ph_Zmass2 = ph_Zmass**2
      ph_Wmass2 = ph_Wmass**2
      ph_Hmass2 = ph_Hmass**2
      ph_ZmZw = ph_Zmass * ph_Zwidth
      ph_WmWw = ph_Wmass * ph_Wwidth
      ph_HmHw = ph_Hmass * ph_Hwidth


c     set mass windows around the resonance peak 
c     It is used in the generation of the Born phase space
      masswindow = 30
c      ph_Zmass2low=(ph_Zmass-masswindow*ph_Zwidth)**2
c      ph_Zmass2high=(ph_Zmass+masswindow*ph_Zwidth)**2
c      ph_Hmass2low=max(0d0,ph_Hmass-masswindow*ph_Hwidth)
c      ph_Hmass2low=ph_Hmass2low**2
c      ph_Hmass2high=(ph_Hmass+masswindow*ph_Hwidth)**2

      ph_Wmass2low=max(0d0,ph_Wmass-masswindow*ph_Wwidth)
      ph_Wmass2low=ph_Wmass2low**2
      ph_Wmass2high=(ph_Wmass+masswindow*ph_Wwidth)**2


c      ph_Hmass2low=0d0
c      ph_Hmass2high=kn_sbeams/4
    
      ph_unit_e = sqrt(4*pi*ph_alphaem)
      end


      subroutine wbb_inizialize_ph_CKM_matrix
      implicit none     
      include 'src/Lib/wbb/PhysPars.h'  
      integer i,j
      do i=1,6
         do j=1,6
            ph_CKM_matrix(i,j) = 0
         enddo
      enddo
      ph_CKM_matrix(1,2) = ph_CKM(1,1)
      ph_CKM_matrix(1,4) = ph_CKM(2,1)
      ph_CKM_matrix(1,6) = ph_CKM(3,1)
      ph_CKM_matrix(2,3) = ph_CKM(1,2)
      ph_CKM_matrix(2,5) = ph_CKM(1,3)
      ph_CKM_matrix(3,4) = ph_CKM(2,2)
      ph_CKM_matrix(3,6) = ph_CKM(3,2)
      ph_CKM_matrix(4,5) = ph_CKM(2,3)
      ph_CKM_matrix(5,6) = ph_CKM(3,3)
      do i=1,6
         do j=i+1,6
            ph_CKM_matrix(j,i) = ph_CKM_matrix(i,j)
         enddo
      enddo
      end


