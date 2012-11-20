      subroutine vbf_h_init_couplings
      implicit none
      include 'src/Lib/vbf_h/PhysPars.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      real * 8 masswindow
      real * 8 vbf_h_powheginput
      external vbf_h_powheginput
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      
      ph_Hmass = vbf_h_powheginput("#hmass")
      ph_Hwidth = vbf_h_powheginput("#hwidth")
      
c     if one of two parameters is missing, use the default ones
      if ((ph_Hmass.lt.0d0).or.(ph_Hwidth.lt.0d0)) then
c     ph_Hmass  = 100d0
c     ph_Hwidth = 0.0033d0
c     ph_Hmass  = 120d0
c     ph_Hwidth = 0.00437d0 
         ph_Hmass  = 150d0
         ph_Hwidth = 0.0182d0
c     ph_Hmass  = 300d0
c     ph_Hwidth = 8.4086d0
c     ph_Hmass  = 400d0
c     ph_Hwidth = 29.42d0 
c     ph_Hmass  = 300d0
c     ph_Hwidth = 8.445d0 
c     ph_Hmass  = 600d0
c     ph_Hwidth = 122.6d0 
c     ph_Hmass  = 800d0
c     ph_Hwidth = 265.d0 
      endif

      write(*,*) '**************************************'
      write(*,*) '**************************************'
      write(*,*) 'Higgs boson vbf_h_mass  = ',ph_Hmass
      write(*,*) 'Higgs boson width = ',ph_Hwidth
      write(*,*) '**************************************'
      write(*,*) '**************************************'

      
      
      ph_Zmass  = 91.1876d0
      ph_Zwidth =  2.4952d0
      ph_Wmass  = 80.398d0
      ph_Wwidth =  2.141d0
      ph_alphaem = 1d0/128.930d0
c      ph_sthw2 = 0.2312d0
      ph_sthw2 = 0.23102d0

c     number of light flavors
      st_nlight = 5

      ph_CKM(1,1)=0.9748 	
      ph_CKM(1,2)=0.2225  	 
      ph_CKM(1,3)=0.0036  	
      ph_CKM(2,1)=0.2225  	
      ph_CKM(2,2)=0.9740 	
      ph_CKM(2,3)=0.041	
      ph_CKM(3,1)=0.009    
      ph_CKM(3,2)=0.0405   
      ph_CKM(3,3)=0.9992

c     initialize CKM with flavor indexes
      call vbf_h_inizialize_ph_CKM_matrix

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


c     set vbf_h_mass windows around the resonance peak 
c     It is used in the generation of the Born phase space
      masswindow = 30
c      ph_Zmass2low=(ph_Zmass-masswindow*ph_Zwidth)**2
c      ph_Zmass2high=(ph_Zmass+masswindow*ph_Zwidth)**2
      ph_Hmass2low=max(0d0,ph_Hmass-masswindow*ph_Hwidth)
      ph_Hmass2low=ph_Hmass2low**2
      ph_Hmass2high=(ph_Hmass+masswindow*ph_Hwidth)**2
c      ph_Hmass2low=0d0
c      ph_Hmass2high=kn_sbeams/4
    
      ph_unit_e = sqrt(4*pi*ph_alphaem)
      end


      subroutine vbf_h_inizialize_ph_CKM_matrix
      implicit none     
      include 'src/Lib/vbf_h/PhysPars.h'  
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


