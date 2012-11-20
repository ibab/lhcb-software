      subroutine zz_setreal(pin,rflav,amp2real)
      implicit none
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/pwhg_st.h'
      include 'src/Lib/zz/qcdcouple.h'
c     vector boson id and decay
      include 'src/Lib/zz/cvecbos.h'
      real * 8 pin(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2real

      integer i,mxpart
      parameter (mxpart=12)
      double precision p(mxpart,4),msq(-5:5,-5:5)
      real * 8 zz_suppfact4e
      external zz_suppfact4e
      ason2pi = st_alpha/2d0/pi

      do i=1,nlegreal
         p(i,4) = pin(0,i)
         p(i,1:3) = pin(1:3,i)
      enddo

      p(1,:)=-p(1,:)
      p(2,:)=-p(2,:)


      call zz_qqb_zz_g(p,msq)


      amp2real = msq(rflav(1),rflav(2))
      amp2real = amp2real/ason2pi

c phase space suppression of (36)(45) singularities
      amp2real = zz_suppfact4e(pin,rflav) * amp2real

      end
