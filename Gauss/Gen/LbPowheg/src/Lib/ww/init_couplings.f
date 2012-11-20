      subroutine ww_init_couplings
      implicit none
      include 'src/Lib/ww/PhysPars.h'
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/zcouple.h'  
      include 'src/Lib/ww/wcouple.h'  
      include 'src/Lib/ww/ewcharge.h'
      include 'src/Lib/ww/qcdcouple.h'
      include 'src/Lib/ww/anomcoup.h'
      include 'src/Lib/ww/pwhg_st.h'
      include 'src/Lib/ww/pwhg_physpar.h'
      logical verbose
      parameter(verbose=.true.)
      physpar_ml(1)=0.511d-3
      physpar_ml(2)=0.1057d0
      physpar_ml(3)=1.777d0
      physpar_mq(1)=0.33d0     ! up
      physpar_mq(2)=0.33d0     ! down
      physpar_mq(3)=0.50d0     ! strange
      physpar_mq(4)=1.50d0     ! charm
      physpar_mq(5)=4.80d0     ! bottom

c     number of light flavors used in evolution of alpha and in PDFs
      st_nlight = 5

      call ww_smcouplings


      ! -- EW couplings
      esq     = ph_unit_e**2
      gw      = ph_unit_e/ph_sthw
      xw      = ph_sthw2

      ! -- masses 
      zmass  = ph_Zmass
      zwidth = ph_Zwidth
      wmass  = ph_Wmass
      wwidth = ph_Wwidth

      
      mp=(/-1d0,+1d0,-1d0,+1d0/)

      Q=(/-2d0,1d0,-2d0,1d0,0d0,-1d0,2d0,-1d0,2d0/)
      Q = Q/3d0 

      tau=(/-1d0,1d0,-1d0,1d0,0d0,-1d0,1d0,-1d0,1d0/)
      
c --- set W/Z couplings
      call ww_couplzw(ph_sthw2)

c --- set anomalous couplings
      call ww_setanomcoup

      if (anomtgc) then
         write(*,*) 'Anomalous couplings used:'
         write(*,*) 'Delta_g1(Z)', delg1_z
         write(*,*) 'Delta_g1(Gamma)', delg1_g
         write(*,*) 'Lambda(Z)', lambda_z
         write(*,*) 'Lambda(Gamma)', lambda_g
         write(*,*) 'Delta_K(Gamma)', delk_g
         write(*,*) 'Delta_K(Z)', delk_z
      endif

      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'Z mass = ',ph_Zmass
      write(*,*) 'Z width = ',ph_Zwidth
      write(*,*) 'W mass = ',ph_Wmass
      write(*,*) 'W width = ',ph_Wwidth
      write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) 'e**2  = ',ph_unit_e**2
      write(*,*) '*************************************'
      endif

      end



