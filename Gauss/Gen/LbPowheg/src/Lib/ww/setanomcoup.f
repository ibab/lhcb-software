      subroutine ww_setanomcoup
c --- reads in anomalous couplings from powheg.input
      implicit none
      include 'src/Lib/ww/anomcoup.h'
      real * 8 ww_powheginput
      external ww_powheginput

      delg1_z = ww_powheginput("#delg1_z")
      delg1_g =ww_powheginput("#delg1_g")
      lambda_g = ww_powheginput("#lambda_g")
      lambda_z = ww_powheginput("#lambda_z")
      delk_g = ww_powheginput("#delk_g")
      delk_z = ww_powheginput("#delk_z")
      tevscale = ww_powheginput("#tevscale")

      if(delg1_z.eq.-1d6) delg1_z=0
      if(delg1_g.eq.-1d6) delg1_g=0
      if(lambda_g.eq.-1d6) lambda_g=0
      if(lambda_z.eq.-1d6) lambda_z=0
      if(delk_g.eq.-1d6) delk_g=0
      if(delk_z.eq.-1d6) delk_z=0
      if(tevscale.eq.-1d6) tevscale = 1d10
      

      if ((delg1_z .ne. 0d0) .or. (delg1_g .ne. 0d0) .or. 
     &     (lambda_z .ne. 0d0) .or. (lambda_g .ne. 0d0) .or.
     &     (delk_g .ne. 0d0) .or. (delk_z .ne. 0d0)) then
         anomtgc = .true.
      else
         anomtgc = .false.
      endif

      return 
      end

