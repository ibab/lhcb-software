      function wz_cpropfac(s,m,w)
      implicit none
      double complex wz_cpropfac
      real * 8 s,m,w
      logical ini,rw
      data ini/.true./
      save ini,rw
      real * 8 wz_powheginput
      external wz_powheginput
      if(ini) then
         if(wz_powheginput("#runningwidth").eq.1) then
            write(*,*) ' Using running width in propagators'
            rw=.true.
         else
            write(*,*) ' Using fixed width in propagators'
            rw=.false.
         endif
         ini=.false.
      endif
      if(rw) then
         wz_cpropfac=s/dcmplx(s-m**2,s*w/m)
      else
         wz_cpropfac=s/dcmplx(s-m**2,m*w)
      endif
      end
