c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine ww_setvirtual(pin,vflav,virtual)
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_st.h'
      include 'src/Lib/ww/qcdcouple.h'
      include 'src/Lib/ww/constants.h'
      double precision msqB(-nf:nf,-nf:nf),msq(-nf:nf,-nf:nf)
      integer i
      double precision p(mxpart,4)

      real * 8 pin(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual
      real * 8 born,dummy(0:3,0:3)
      real *8 s,ww_dotp
      external ww_dotp

c --- set scale dependent QCD coupling 
      ason2pi = st_alpha/twopi

      do i=1,nlegborn
         p(i,4) = pin(0,i)
         p(i,1:3) = pin(1:3,i)
      enddo
      p(1,:)=-p(1,:)
      p(2,:)=-p(2,:)

      call ww_qqb_ww(p,msqB)
      call ww_qqb_ww_v(p,msq)

      ! -- divide out ason2pi
      virtual = msq(vflav(1),vflav(2))/ason2pi
 
      ! -- scheme change from dred 
      born=msqB(vflav(1),vflav(2))
      virtual = virtual + born*(-2d0*(cf/2d0)) 

      end

