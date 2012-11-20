c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine zz_setvirtual(pin,vflav,virtual)
      implicit none
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_st.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/qcdcouple.h'
c     vector boson id and decay
      include 'src/Lib/zz/cvecbos.h'
      integer mxpart
      parameter (mxpart=12)
      double precision msqB(-5:5,-5:5),msq(-5:5,-5:5)
      double precision p(mxpart,4)
      real * 8 pin(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual
      real * 8 born,dummy(0:3,0:3)
      real *8 s,zz_dotp
      external zz_dotp
      integer i
      real * 8 zz_suppfact4e
      external zz_suppfact4e

      ason2pi = st_alpha/2d0/pi

      do i=1,nlegborn
         p(i,4) = pin(0,i)
         p(i,1:3) = pin(1:3,i)
      enddo
      p(1,:)=-p(1,:)
      p(2,:)=-p(2,:)

      call zz_qqb_zz(p,msqB)
      born=msqB(vflav(1),vflav(2))
      call zz_qqb_zz_v(p,msq)

      ! -- divide out ason2pi
      virtual = msq(vflav(1),vflav(2))/ason2pi

      ! -- scheme change from dred 
      virtual = virtual + born*(-2d0*(cf/2d0)) 

c phase space suppression of (36)(45) singularities
      virtual = zz_suppfact4e(pin,vflav) * virtual

      end

