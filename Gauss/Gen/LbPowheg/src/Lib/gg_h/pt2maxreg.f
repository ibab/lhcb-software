      function gg_h_pt2max_regular()
      real * 8 gg_h_pt2max_regular
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/pwhg_rad.h'
      include 'src/Lib/gg_h/PhysPars.h'
      integer choiche
      parameter (choiche=1)
      logical debug
      parameter (debug=.false.) 

      if(choiche.eq.1) then
         gg_h_pt2max_regular=(kn_sreal/4)*(1-kn_y**2)*kn_csi**2
C     Maximum pt as in the ISR case, since we adopted ISR
C     parametrization for phase space of regular contributions.
         if(debug) then
            if ((sqrt(kn_cmpreal(1,4)**2+kn_cmpreal(2,4)**2)/gg_h_pt2max_regular 
     $           - 1d0).gt.1d-8) then
               write(*,*) 'ERROR: wrong gg_h_pt2max_regular'
               call exit(1)
            endif
         endif
      elseif(choiche.eq.2) then
         gg_h_pt2max_regular=ph_topmass
C     hard scale of the process
      else
         write(*,*) 'ERROR: wrong choihce in gg_h_pt2max_regular'
         call exit(1)
      endif
      if (gg_h_pt2max_regular.lt.rad_ptsqmin) then
         write(*,*) '****************************************'
         write(*,*) 'WARNING in gg_h_pt2max_regular'
         write(*,*) 'gg_h_pt2max_regular < rad_ptsqmin ',
     #        gg_h_pt2max_regular,' < ',rad_ptsqmin
         write(*,*) (flst_regular(i,rad_realreg),i=1,nlegreal)
         write(*,*) 'To generate this event, use the following seeds'
         call gg_h_printcurrentrandom
         write(*,*) '****************************************'
      endif
      end
