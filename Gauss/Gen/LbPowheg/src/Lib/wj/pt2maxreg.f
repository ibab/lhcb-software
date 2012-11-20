      function wj_pt2max_regular()
      real * 8 wj_pt2max_regular
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/PhysPars.h'
      integer choiche
      parameter (choiche=1)
      logical debug
      parameter (debug=.false.) 

      if(choiche.eq.1) then
         wj_pt2max_regular=(kn_sreal/4)*(1-kn_y**2)*kn_csi**2
C     Maximum pt as in the ISR case, since we adopted ISR
C     parametrization for phase space of regular contributions.
         if(debug) then
            if((dabs((kn_cmpreal(1,4)**2+kn_cmpreal(2,4)**2)/
     $           wj_pt2max_regular)- 1d0).gt.1d-8) then
               write(*,*) 'ERROR: wrong wj_pt2max_regular'
               call exit(1)
            endif
         endif
      elseif(choiche.eq.2) then
         wj_pt2max_regular=ph_topmass
C     hard wj_scale of the process
      else
         write(*,*) 'ERROR: wrong choihce in wj_pt2max_regular'
         call exit(1)
      endif
      if (wj_pt2max_regular.lt.rad_ptsqmin) then
         write(*,*) '****************************************'
         write(*,*) 'WARNING in wj_pt2max_regular'
         write(*,*) 'wj_pt2max_regular < rad_ptsqmin ',
     #        wj_pt2max_regular,' < ',rad_ptsqmin
         write(*,*) (flst_regular(i,rad_realreg),i=1,nlegreal)
         write(*,*) 'To generate this event, use the following seeds'
         call wj_printcurrentrandom
         write(*,*) '****************************************'
      endif
      end
