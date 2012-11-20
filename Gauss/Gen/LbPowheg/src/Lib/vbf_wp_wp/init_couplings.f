      subroutine vbf_wp_wp_init_couplings
      implicit none
      include 'src/Lib/vbf_wp_wp/PhysPars.h'
      include 'src/Lib/vbf_wp_wp/pwhg_st.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/pwhg_physpar.h'
      include 'src/Lib/vbf_wp_wp/PhysPars_Higgs.h'
      real * 8 masswindow
      logical verbose
      parameter(verbose=.true.)
      integer i,j

      real*8 gfermi,sthw,cthw,g2
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   INDEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c fermion masses:
      physpar_ml(1)=0.511d-3 ! e
      physpar_ml(2)=0.1057d0 ! mu
      physpar_ml(3)=1.777d0  ! tau
      physpar_mq(1)=0.33d0   ! up
      physpar_mq(2)=0.33d0   ! down
      physpar_mq(3)=0.50d0   ! strange
      physpar_mq(4)=1.50d0   ! charm
      physpar_mq(5)=4.80d0   ! bottom
      
c Higgs parameters: 
      ph_Hmass  = 120d0
      ph_Hwidth = 0.00441d0 
C
C vbf_wp_wp_gauge boson parameters:
      ph_Zmass   = 91.188d0     
      ph_Zwidth  =  2.51d0
      ph_Wmass   = 80.419d0     
      ph_Wwidth  =  2.099d0

c EW parameters:
      gfermi = 1.16639d-5
      ph_sthw2 = 1d0 - (ph_Wmass/ph_Zmass)**2
      sthw = SQRT(ph_sthw2)
      cthw = SQRT(1.d0 -ph_sthw2 )
      G2 = SQRT(8.d0*GFERMI/SQRT(2.d0))*ph_Zmass*cthw
      ph_alphaem = g2**2*ph_sthw2/(4.d0*PI)

c     DIAGONAL CKM 
      ph_CKM(1,1)=1d0 
      ph_CKM(1,2)=0d0 
      ph_CKM(1,3)=0d0
      ph_CKM(2,1)=0d0 
      ph_CKM(2,2)=1d0 
      ph_CKM(2,3)=0d0
      ph_CKM(3,1)=0d0
      ph_CKM(3,2)=0d0
      ph_CKM(3,3)=1d0

c     number of light flavors
      st_nlight = 5

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccc   DEPENDENT QUANTITIES       
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      ph_sthw = sqrt(ph_sthw2)
      ph_cthw = sqrt(1-ph_sthw2)
      ph_Zmass2 = ph_Zmass**2
      ph_Wmass2 = ph_Wmass**2
      ph_Hmass2 = ph_Hmass**2

c     set mass windows around Z-mass peak in units of ph_Zwidth
c     It is used in the generation of the vbf_wp_wp_born phase space
      masswindow = 30
      ph_Zmass2low=(ph_Zmass-masswindow*ph_Zwidth)**2
      ph_Zmass2high=(ph_Zmass+masswindow*ph_Zwidth)**2
      ph_ZmZw = ph_Zmass * ph_Zwidth

c     set mass window around W-mass peak in units of ph_Wwidth
c     It is used in the generation of the vbf_wp_wp_born phase space
      masswindow = 30
      ph_Wmass2low=(ph_Wmass-masswindow*ph_Wwidth)**2
      ph_Wmass2high=(ph_Wmass+masswindow*ph_Wwidth)**2
      ph_WmWw = ph_Wmass * ph_Wwidth

c Higgs parameters (not used)
      masswindow = 30
      ph_Hmass2low=(ph_Hmass-masswindow*ph_Hwidth)**2
      ph_Hmass2high=(ph_Hmass+masswindow*ph_Hwidth)**2
      ph_HmHw = ph_Hmass * ph_Hwidth
    
      ph_unit_e = sqrt(4*pi*ph_alphaem)

      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'Z mass = ',ph_Zmass
      write(*,*) 'Z width = ',ph_Zwidth
      write(*,*) 'W mass = ',ph_Wmass
      write(*,*) 'W width = ',ph_Wwidth
      write(*,*) 'H mass = ',ph_Hmass
      write(*,*) 'H width = ',ph_Hwidth
      write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) '(unit_e)^2 = ',ph_unit_e**2   
      write(*,*) '(g_w)^2 = ',ph_unit_e*ph_unit_e/ph_sthw2   
      write(*,*) 'CKM matrix' 
      do i=1,3
         write(*,*) (ph_CKM(i,j),j=1,3)
      enddo
      write(*,*) '*************************************'
      endif      

c convert couplings into format needed by EW matrixelements:
      call vbf_wp_wp_COUP_POWHEG_TO_VBFNLO

      end


      
