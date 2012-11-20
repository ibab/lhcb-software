c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine gg_h_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      real * 8 p(0:3,nlegborn)
      integer vflav(nlegborn)      
      real * 8 virtual,dummyjk(nlegborn,nlegborn)
      real * 8 born,dummymunu(0:3,0:3,nlegborn)
      real *8 s,gg_h_dotp
      external gg_h_dotp
      
      s=2d0*gg_h_dotp(p(0,1),p(0,2))
      call gg_h_compborn(p,vflav,born,dummyjk,dummymunu)
C     see eq. 2.22 of arXiv: 0812.0578
      virtual=(11 + pi*pi*CA - CA*(log(st_muren2/s))**2)*born
      end

