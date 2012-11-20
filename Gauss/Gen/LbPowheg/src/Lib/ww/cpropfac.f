      function ww_cpropfac(s,m,w)
      implicit none
      double complex ww_cpropfac
      real * 8 s,m,w
      logical ini,rw
      data ini/.true./
      save ini,rw
      real * 8 ww_powheginput
      external ww_powheginput
      if(ini) then
         if(ww_powheginput("#runningwidth").eq.1) then
            write(*,*) ' Using running width in propagators'
            rw=.true.
         else
            write(*,*) ' Using fixed width in propagators'
            rw=.false.
         endif
         ini=.false.
      endif
      if(rw) then
         ww_cpropfac=s/dcmplx(s-m**2,s*w/m)
      else
         ww_cpropfac=s/dcmplx(s-m**2,m*w)
      endif
      end
