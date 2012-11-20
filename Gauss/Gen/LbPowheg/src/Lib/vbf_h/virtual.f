c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
c
c  The vertex correction depicted below is given by:
c                    -----
c                   /     \ <- virtual gluon
c                  /       \
c         q1 --->------------>------ q3
c                      |
c                      V q 
c                      |
c   q1^2 = q2^2 = 0         q = q1-q3  
c   q^2 < 0
c
c   V^nu = (4*Pi)^ep * Gamma(1+ep) * CF * as/(4*Pi) * 
c           (-2/ep^2+(-2*ln(r)-3)/ep-ln(r)^2-3*ln(r)+Pi^2/3-7)*B^nu
c         = (4*Pi)^ep / Gamma(1-ep) * CF * as/(4*Pi) * 
c           (-2/ep^2+(-2*ln(r)-3)/ep-ln(r)^2-3*ln(r)+Pi^2/3-7-Pi^2/3)*B^nu
c
c     where B^nu is the Born vertex and r = mu^2/(-q^2)
c     See my formula (2.11) in Phys.Rev.D68:073005,2003 [hep-ph/0306109] 
c
c     The factor  (4*Pi)^ep/Gamma(1-ep) IS NOT RETURNED by this subroutine
c     and it's thought as factorized in front of the real counterterms too.

      subroutine vbf_h_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer vflav(nleg)
      real * 8 virtual
      real * 8 ampborn2,bmunu(0:3,0:3)
      real * 8 cvirt
c     if (4*Pi)^ep * Gamma(1+ep) is collected in front then cvirt:
c      parameter (cvirt = pi**2/3 - 7)  in DR
c      parameter (cvirt = pi**2/3 - 8)  in CDR
c     if (4*Pi)^ep / Gamma(1-ep) is collected in front then cvirt:
      parameter (cvirt = -8d0)
      include 'src/Lib/vbf_h/PhysPars.h'
      real * 8 interf1,interf2,q2_up,q2_down,vbf_h_dotp,r,lr
      external vbf_h_dotp
      call vbf_h_compborn(p,vflav,ampborn2,bmunu)

c     squared momentum of the weak boson connected with the upper line
      q2_up = -2*vbf_h_dotp(p(0,1),p(0,4))
c     squared momentum of the weak boson connected with the lower line
      q2_down = -2*vbf_h_dotp(p(0,2),p(0,5))
      
c upper-line correction 
      r = st_muren2/(-q2_up)
      if (r.lt.0d0) then
         write(*,*) 'Error in vbf_h_setvirtual: q2_up should be < 0!!'
         stop
      endif      
      lr = log(r)
c     1/2 i front comes from (as/(4*Pi))/(as/(2*Pi))
      interf1 = 1d0/2 * CF * (-lr**2-3*lr+cvirt) * ampborn2
      
c lower-line correction 
      r = st_muren2/(-q2_down)
      if (r.lt.0d0) then
         write(*,*) 'Error in vbf_h_setvirtual: q2_down should be < 0!!'
         stop
      endif      
      lr = log(r)
c     1/2 i front comes from (as/(4*Pi))/(as/(2*Pi))
      interf2 = 1d0/2 * CF * (-lr**2-3*lr+cvirt) * ampborn2
    
c     2 * Re[M_V * M_B^*]
      virtual = 2 * (interf1 + interf2)
      end


