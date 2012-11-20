      subroutine hvq_init_processes
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_flg.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      include 'src/Lib/hvq/pwhg_st.h'
      include 'src/Lib/hvq/pwhg_par.h'
      include 'src/Lib/hvq/LesHouches.h'
      logical debug
      parameter (debug=.false.)
      integer i,ihvq
      character *3 flav(-5:5)
      data (flav(i),i=-5,5) 
     #     /'b~','c~','s~','u~','d~','g','d','u','s','c','b'/
      real * 8 hvq_powheginput,qmass
      par_isrtinycsi = 1d-8
      par_isrtinyy = 1d-8
      par_fsrtinycsi = 1d-8
      par_fsrtinyy = 1d-8
c flag to do importance sampling in x variable in collinear remnants
      flg_collremnsamp=.true.      
c     number of light flavors
      qmass=hvq_powheginput("qmass")
      if(qmass.lt.3) then         
         st_nlight = 3
      elseif(qmass.lt.10) then
         st_nlight = 4
      else
         st_nlight = 5
      endif
      kn_masses(1)=0
      kn_masses(2)=0
      kn_masses(3)=qmass
      kn_masses(4)=qmass
      kn_masses(5)=0
      kn_minmass=2*qmass
      ihvq=st_nlight+1
      lprup(1)=1000+ihvq
c     index of the first light coloured particle in the final state
c     (all subsequent particles are coloured)
      flst_lightpart=5

      flst_nborn=1
      flst_nreal=1

      flst_born(1,flst_nborn)=0
      flst_born(2,flst_nborn)=0
      flst_born(3,flst_nborn)=ihvq
      flst_born(4,flst_nborn)=-ihvq

      flst_real(1,flst_nreal)=0
      flst_real(2,flst_nreal)=0
      flst_real(3,flst_nreal)=ihvq
      flst_real(4,flst_nreal)=-ihvq
      flst_real(5,flst_nreal)=0

      do i=-st_nlight,st_nlight
         if(i.ne.0) then
            flst_nborn=flst_nborn+1
            flst_born(1,flst_nborn)=i
            flst_born(2,flst_nborn)=-i
            flst_born(3,flst_nborn)=ihvq
            flst_born(4,flst_nborn)=-ihvq
            
            flst_nreal=flst_nreal+1
            flst_real(1,flst_nreal)=i
            flst_real(2,flst_nreal)=-i
            flst_real(3,flst_nreal)=ihvq
            flst_real(4,flst_nreal)=-ihvq
            flst_real(5,flst_nreal)=0
            
            flst_nreal=flst_nreal+1
            flst_real(1,flst_nreal)=i
            flst_real(2,flst_nreal)=0
            flst_real(3,flst_nreal)=ihvq
            flst_real(4,flst_nreal)=-ihvq
            flst_real(5,flst_nreal)=i
            
            flst_nreal=flst_nreal+1
            flst_real(1,flst_nreal)=0
            flst_real(2,flst_nreal)=i
            flst_real(3,flst_nreal)=ihvq
            flst_real(4,flst_nreal)=-ihvq
            flst_real(5,flst_nreal)=i
         endif
      enddo
      call hvq_init_top_dec(ihvq)
      end


      subroutine hvq_init_top_dec(nhvq)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_rad.h'      
      include 'src/Lib/hvq/LesHouches.h'      
      integer nhvq
      integer itdec
      integer iwp1,iwp2,iwm1,iwm2
      real * 8 mdecwp1,mdecwp2,mdecwm1,mdecwm2,totbr
      real * 8 hvq_powheginput
      external hvq_powheginput
      if(nhvq.eq.6) then
         itdec=hvq_powheginput('#topdecaymode')
         if(itdec.eq.-1000000) itdec=0
         if(itdec.ne.0) then
            lprup(1)=300000+itdec
c first call to pickwdecay, to initialize and get back the branching fraction
            call  hvq_pickwdecays(iwp1,mdecwp1,iwp2,mdecwp2,
     #                 iwm1,mdecwm1,iwm2,mdecwm2,totbr)
            rad_branching=totbr
         endif
      endif
      end




