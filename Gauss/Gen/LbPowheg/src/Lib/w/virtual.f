c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine w_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_st.h'
      include 'src/Lib/w/pwhg_math.h'
      real * 8 p(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual
      real * 8 born,dummy(0:3,0:3)
      real *8 s,w_dotp
      external w_dotp

      s=2d0*w_dotp(p(0,1),p(0,2))
      call w_compborn(p,vflav,born,dummy)
      virtual=pi**2 - 8 - 3*log(st_muren2/s) -log(st_muren2/s)**2
      virtual=virtual *cf *born

      end


c$$$c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c$$$c     where M_B is the Born amplitude and 
c$$$c     M_V is the finite part of the virtual amplitude
c$$$c     The as/(2pi) factor is attached at a later point
c$$$
c$$$c     Like w_setvirtual, but it calculates only the independent
c$$$c     virtual contributions and fills directly the array 'virt_arr'.
c$$$c     
c$$$c     At the moment (revision 12), no calls to setvirtual_fast are present
c$$$c     in the main code. Therefore, this routine is left dummy.
c$$$
c$$$      subroutine setvirtual_fast(virt_arr)
c$$$      implicit none
c$$$      include 'src/Lib/w/nlegborn.h'
c$$$      include 'src/Lib/w/pwhg_flst.h'
c$$$      real * 8 virt_arr(flst_nborn)
c$$$
c$$$      write(*,*) 'Error: setvirtual_fast is not implemented yet'
c$$$      stop
c$$$
c$$$      end
