      function zz_cpropfac(s,m,w)
      implicit none
      double complex zz_cpropfac
      real * 8 s,m,w
      logical ini,rw
      data ini/.true./
      save ini,rw
      real * 8 zz_powheginput
      external zz_powheginput
      if(ini) then
         if(zz_powheginput("#runningwidth").eq.1) then
            write(*,*) ' Using running width in propagators'
            rw=.true.
         else
            write(*,*) ' Using fixed width in propagators'
            rw=.false.
         endif
         ini=.false.
      endif
      if(rw) then
         zz_cpropfac=s/dcmplx(s-m**2,s*w/m)
      else
         zz_cpropfac=s/dcmplx(s-m**2,m*w)
      endif
      end
