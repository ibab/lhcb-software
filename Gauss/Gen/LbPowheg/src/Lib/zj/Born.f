      subroutine zj_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      integer j,k,mu,nu
c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_i j=B*(C_i+C_j-C_k)/2,
c where k#i,j
      call zj_compborn(p,bflav,born,bbmunu)
      do j=1,nlegs
         if(abs(bflav(j)).le.6) then
            if(bflav(j).eq.0) then
               do mu=0,3
                  do nu=0,3
                     bmunu(mu,nu,j)=bbmunu(mu,nu)
                  enddo
               enddo
            endif
            do k=j+1,nlegs
               if(abs(bflav(k)).le.6) then
                  if(bflav(j).ne.0.and.bflav(k).ne.0) then
                     colcf=(2*cf-ca)/2
                  else
                     colcf=ca/2
                  endif
                  bornjk(j,k)=born*colcf
                  bornjk(k,j)=bornjk(j,k)
               endif
            enddo
         endif
      enddo
      end


c     Example
c     q q~ -> e- e+ g  
      subroutine zj_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
c -*- Fortran -*-
c      character *2 flav(-5:5)
      real * 8 charge(-5:5)
c      data (charge(ijkh),ijkh=-5,5) 
c      data (flav(ijkh),ijkh=-5,5) 
c      data flav
c     #     /'b~','c~','s~','u~','d~','g','d','u','s','c','b'/
      data charge
     #     / 0.33333333333333333333d0, !   1d0/3
     #      -0.66666666666666666667d0, !  -2d0/3
     #       0.33333333333333333333d0, !   1d0/3 
     #      -0.66666666666666666667d0, !   -2d0/3
     #       0.33333333333333333333d0, !   1d0/3 
     #       0d0,                      !   0d0   
     #      -0.33333333333333333333d0, !   -1d0/3
     #       0.66666666666666666667d0, !   2d0/3   
     #      -0.33333333333333333333d0, !   -1d0/3
     #       0.66666666666666666667d0, !   2d0/3 
     #      -0.33333333333333333333d0/ !   -1d0/3
c      include 'src/Lib/zj/QuarkFlavs.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer bflav(nleg)
      real * 8 amp2,born,bmunu(0:3,0:3)
      integer ferm_type(nleg)
      integer i,j,k
      real * 8 ferm_charge(nleg)

      if (abs(bflav(3)).le.6.or.abs(bflav(4)).le.6) then
         write(*,*) 'born_ampsq: ERROR in flavor assignement'
         stop
      endif

c     lepton-antilepton from Z decay
      ferm_type(3) = +1
      ferm_charge(3) = -1
      ferm_type(4) = -ferm_type(3)
      ferm_charge(4) = -ferm_charge(3)
      
c     i is the flavour index of first incoming parton
c     j is the flavour index of second incoming parton
c     k is the flavour of outgoing partons in the order particle,antiparticle,gluon
c     with the convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      
      i = bflav(1)
      j = bflav(2)
      k = bflav(5)
      ferm_charge(1) = charge(i)
      ferm_charge(2) = charge(j)
      ferm_charge(5) = charge(k)

      if (i.eq.0) then
         ferm_type(1) = 0
      else 
         ferm_type(1) = i/abs(i)
      endif 
      if (j.eq.0) then
         ferm_type(2) = 0
      else 
         ferm_type(2) = j/abs(j)
      endif   
      if (k.eq.0) then
         ferm_type(5) = 0
      else 
         ferm_type(5) = k/abs(k)
      endif   

      if ((i.eq.(-j)).and.(k.eq.0).and.(i.ne.0)) then
c     q q~ -> g 
         call zj_q_aq_to_l_al_g(p,ferm_type,ferm_charge,amp2,bmunu)         
      elseif ((i.eq.k).and.(j.eq.0).and.(i.ne.0)) then
c     q g -> q 
         call zj_q_g_to_l_al_q(p,ferm_type,ferm_charge,amp2,bmunu)
      elseif ((j.eq.k).and.(i.eq.0).and.(j.ne.0)) then
c     g q -> q g  
         call zj_g_q_to_l_al_q(p,ferm_type,ferm_charge,amp2,bmunu)
      else
        amp2 = 0d0
c        write(*,*) 'WARNING: returning 0 amplitude!'
      endif

      born=amp2
      end



c this subroutine compute the Born amplitude for the process
c q(p1) aq(p2) -> Z(p3+p4) g(p5)  con Z -> l-(p3) l+(p4) 
c NUMERICALLY, with the zj_bra/zj_ket formalism, not by squaring the analytic 
c amplitude
c   INPUT:  matrix pphys with all the momenta 
c   OUTPUT: the amplitude squared (amp2) averaged over initial 
c           polarization     
c
c         q  --->----------->------ g
c                     |
c                     |            l
c                     |          /  
c         aq ---<-----//////
c                       Z/gamma c                                 lbar
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine zj_q_aq_to_l_al_g(pphy,fermion_type,fermion_charge,
     #     amp2,bmunu)
      implicit none
c the nleg 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      complex *16 unit_I
      parameter (unit_I=(0,1))
      real * 8 p1(0:3),p2(0:3)
c      include 'src/Lib/zj/couplings.h'
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/PhysPars.h'
      real * 8 p34,zj_dotp,tmp
      complex * 16 zj_ccdotp
      real * 8 epsg(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)     
      complex*16 jlep(0:3,-1:1),jquark(0:3,-1:1),Jlep_dot_Jquark,
     #     Jlep_dot_Jquark_vec(2,-1:1,-1:1)
      integer i,pol,hel_lep,hel_quark
      real * 8 p(0:3,nleg),ferm_charge(nleg)
      integer ferm_type(nleg),utype_q, utype_l
      real * 8 q_q,v_q,a_q,q_l,v_l,a_l,Zcoup_q(-1:1),Zcoup_l(-1:1)
      complex * 16 prop34V, prop34gamma
      integer mu,nu,pol1,pol2

     
      if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with Z decay'
         stop
      endif
     
c     copy to local variables
      do i=1,nleg
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo

c     exchance particle 1 and 2
      if (ferm_type(1).eq.-1) then
        if (ferm_type(2).eq.1) then
            call zj_exchange_momenta(p(0,1),p(0,2))
            tmp = ferm_charge(1)
            ferm_charge(1)=-ferm_charge(2)
            ferm_charge(2)=-tmp
            tmp = ferm_type(1)
            ferm_type(1)=ferm_type(2)
            ferm_type(2)=tmp
         else
            write(*,*) 'Error in the type of the quark 1-2'
            stop
         endif
      endif

c utype = +1 if this is an up-type quark (u,c,ubar,cbar)
c utype = -1 otherwise
      if (abs(ferm_charge(1)).eq.2d0/3) then
         utype_q = +1
         q_q = 2d0/3
      elseif (abs(ferm_charge(1)).eq.1d0/3) then
         utype_q = -1
         q_q = -1d0/3
      else
         write(*,*) 'What charge is this??', ferm_charge(1)
         stop
      endif

      if (abs(ferm_charge(3)).eq.1d0) then
         utype_l = -1
         q_l = -1d0
      elseif (abs(ferm_charge(3)).eq.0d0) then
         utype_l = +1
         q_l = 0d0
      else
         write(*,*) 'What charge is this??',ferm_charge(4)
         stop
      endif      

      v_q = utype_q*1.d0/2 - 2*q_q*ph_sthw**2 
      a_q = utype_q*1.d0/2
      Zcoup_q(-1) = v_q + a_q
      Zcoup_q(+1) = v_q - a_q

      v_l = utype_l*1.d0/2 - 2*q_l*ph_sthw**2 
      a_l = utype_l*1.d0/2     
      Zcoup_l(-1) = v_l + a_l
      Zcoup_l(+1) = v_l - a_l

      do mu=0,3
         p1(mu) = ferm_type(1)*p(mu,1)
         p2(mu) = ferm_type(2)*p(mu,2)
      enddo

      p34=zj_dotp(p(0,3),p(0,4))
c     Z exchange in the s channel         
      prop34V = 1d0/dcmplx(2*p34-ph_Zmass2,ph_ZmZw) 
      prop34gamma = 1d0/(2*p34)

c     q
      call zj_ket(p(0,1),ferm_type(1),psi1)
c     aq
      call zj_bra(p(0,2),ferm_type(2),psi2)
c     em
      call zj_bra(p(0,3),ferm_type(3),psi3)
c     ep
      call zj_ket(p(0,4),ferm_type(4),psi4)
      
c     leptonic currents
      call zj_bra_gamma_ket(psi3,psi4,-1,jlep(0,-1))
      call zj_bra_gamma_ket(psi3,psi4,+1,jlep(0,+1))
      
      amp2=0d0
      do pol=1,2
         call zj_POLVEC(p(0,5),pol,epsg(0,pol))         
         do hel_lep=-1,1,2             
            do hel_quark=-1,1,2               
               Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark)=0d0
               call zj_bra_gamma_ket_gluon(psi2,psi1,hel_quark,p2,p1,
     #              p(0,5),epsg(0,pol),jquark(0,hel_quark))                  
               Jlep_dot_Jquark = 
     #              zj_ccdotp(jlep(0,hel_lep),jquark(0,hel_quark))               
               Jlep_dot_Jquark = Jlep_dot_Jquark*(
c     gamma exchange
     #             q_q*q_l*prop34gamma + 
c     Z exchange
     #             Zcoup_q(hel_quark)*Zcoup_l(hel_lep)/
     #              (2*ph_sthw*ph_cthw)**2*
     #             prop34V)
               Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark) = 
     #              Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark) +
     #              Jlep_dot_Jquark
               amp2 = amp2 + Jlep_dot_Jquark*DCONJG(Jlep_dot_Jquark)    
            enddo      
         enddo         
      enddo

      do mu=0,3
      do nu=0,3
         bmunu(mu,nu) = 0d0
         do pol1=1,2
         do pol2=1,2
         do hel_lep=-1,1,2             
         do hel_quark=-1,1,2               
c     we don't conjugate epsg(mu,pol1) since it is real
            bmunu(mu,nu) = bmunu(mu,nu) +                  
     #       Jlep_dot_Jquark_vec(pol1,hel_lep,hel_quark)
     #           *DCONJG(Jlep_dot_Jquark_vec(pol2,hel_lep,hel_quark))
     #           * epsg(mu,pol1) * epsg(nu,pol2)
         enddo
         enddo
         enddo
         enddo
      enddo
      enddo
      amp2 = amp2 * ph_unit_e**4 * (4*pi*st_alpha)
c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors 
      amp2=  amp2*CF/4/nc 

      do mu=0,3
         do nu=0,3
            bmunu(mu,nu) = bmunu(mu,nu) * ph_unit_e**4 *(4*pi*st_alpha)*
     #           CF/4/nc
         enddo
      enddo
      end


c this subroutine compute the tree-level amplitude for the process
c q(p1) g(p2) -> Z(p3+p4) q(p5)  con Z -> l-(p3) l+(p4) 
c NUMERICALLY, with the zj_bra/zj_ket formalism, not by squaring the analytic 
c amplitude
c   INPUT:  matrix pphys with all the momenta 
c   OUTPUT: the amplitude squared (born) averaged over initial 
c           polarization     
c                                l  
c                               /  
c         q --->----//////
c                   |  Z/gamma c                   |            lbar
c                   |   
c         g  --->------->---- q
c                     
c                 
c     ferm_type = 1 fermion
c     ferm_type = -1 antfermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine zj_q_g_to_l_al_q(pphy,fermion_type,fermion_charge,
     #     amp2,bmunu)   
      implicit none
c the nleg 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      complex *16 unit_I
      parameter (unit_I=(0,1))
      real * 8 p1(0:3),p2(0:3),p5(0:3)
c      include 'src/Lib/zj/couplings.h'
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/PhysPars.h'
      real * 8 p34
      real * 8 zj_dotp,tmp
      complex * 16 zj_ccdotp
      integer temp
      real * 8 epsg(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1),
     #     psi5(2,-1:1)
      complex*16 jlep(0:3,-1:1),Jquark(0:3,-1:1)
      complex*16 Jlep_dot_Jquark,Jlep_dot_Jquark_vec(2,-1:1,-1:1)
      integer mu,nu,i,pol,hel_lep,hel_quark,pol1,pol2
      real * 8 p(0:3,nleg)
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)
      integer utype_q, utype_l
      real * 8 q_q,v_q,a_q,q_l,v_l,a_l
      real*8 Zcoup_q(-1:1),Zcoup_l(-1:1)
      complex * 16 prop34V, prop34gamma
      
      if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with Z decay'
         stop
      endif

     
     
c  copy of local variables
      do i=1,5
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo

c check initial configuration qg or gq
      if ((ferm_type(1).eq.0).and.(ferm_charge(1).eq.0d0)) then
c exchance particle 1 and 2
         call zj_exchange_momenta(p(0,1),p(0,2))
         tmp = ferm_charge(1)
         ferm_charge(1)=-ferm_charge(2)
         ferm_charge(2)=-tmp
         temp = ferm_type(1)
         ferm_type(1)=ferm_type(2)
         ferm_type(2)=temp
      elseif ((ferm_type(2).ne.0).or.(ferm_charge(2).ne.0d0)) then
         write(*,*) 'Error in the initial configuration'
         stop
      endif
      
c exchance particle 1 and 5
      if (ferm_type(1).eq.-1) then
        if (ferm_type(5).eq.-1) then
            call zj_exchange_momenta(p(0,1),p(0,5))
            tmp = ferm_charge(1)
            ferm_charge(1)=-ferm_charge(5)
            ferm_charge(5)=-tmp
            temp = ferm_type(1)
            ferm_type(1)=ferm_type(5)
            ferm_type(5)=temp
         else
            write(*,*) 'Error in the type of the quark 1-5'
            stop
         endif
      endif

c utype = +1 if this is an up-type quark (u,c,ubar,cbar)
c utype = -1 otherwise
      if (abs(ferm_charge(1)).eq.2d0/3) then
         utype_q = +1
         q_q = 2d0/3
      elseif (abs(ferm_charge(1)).eq.1d0/3) then
         utype_q = -1
         q_q = -1d0/3
      else
         write(*,*) 'What charge is this??', ferm_charge(1)
         stop
      endif

            
      if (abs(ferm_charge(3)).eq.1d0) then
         utype_l = -1
         q_l = -1d0
      elseif (abs(ferm_charge(3)).eq.0d0) then
         utype_l = +1
         q_l = 0d0
      else
         write(*,*) 'What charge is this??',ferm_charge(3)
         stop
      endif
      
      v_q = utype_q*1.d0/2 - 2*q_q*ph_sthw**2 
      a_q = utype_q*1.d0/2
      Zcoup_q(-1) = v_q + a_q
      Zcoup_q(+1) = v_q - a_q
      
      v_l = utype_l*1.d0/2 - 2*q_l*ph_sthw**2 
      a_l = utype_l*1.d0/2
      Zcoup_l(-1) = v_l + a_l
      Zcoup_l(+1) = v_l - a_l

      do mu=0,3
         p1(mu) = ferm_type(1)*p(mu,1)
         p2(mu) = -p(mu,2)
         p5(mu) = ferm_type(5)*p(mu,5)         
      enddo

      p34=zj_dotp(p(0,3),p(0,4))
c     Z exchange in the s channel         
      prop34V = 1d0/dcmplx(2*p34-ph_Zmass2,ph_ZmZw) 
      prop34gamma = 1d0/(2*p34)
     
c     q
      call zj_ket(p(0,1),ferm_type(1),psi1)
c     qp
      call zj_bra(p(0,5),ferm_type(5),psi5)
c     em
      call zj_bra(p(0,3),ferm_type(3),psi3)
c     ep
      call zj_ket(p(0,4),ferm_type(4),psi4)
      
c     leptonic currents
      call zj_bra_gamma_ket(psi3,psi4,-1,jlep(0,-1))
      call zj_bra_gamma_ket(psi3,psi4,+1,jlep(0,+1))
      
      amp2=0d0

      do  pol=1,2        
         call zj_POLVEC(p(0,2),pol,epsg(0,pol))         
         do hel_lep=-1,1,2            
            do hel_quark=-1,1,2 
               Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark)=0d0
               call zj_bra_gamma_ket_gluon(psi5,psi1,hel_quark,p5,p1,p2,
     #              epsg(0,pol),Jquark(0,hel_quark))                
               Jlep_dot_Jquark = 
     #              zj_ccdotp(jlep(0,hel_lep),Jquark(0,hel_quark))
               Jlep_dot_Jquark = Jlep_dot_Jquark*(
c     gamma exchange
     #              q_q*q_l*prop34gamma + 
c     Z exchange
     #             Zcoup_q(hel_quark)*Zcoup_l(hel_lep)/
     #              (2*ph_sthw*ph_cthw)**2*
     #             prop34V)                       
               Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark) = 
     #              Jlep_dot_Jquark_vec(pol,hel_lep,hel_quark) +
     #              Jlep_dot_Jquark
               amp2 = amp2 + Jlep_dot_Jquark*DCONJG(Jlep_dot_Jquark)
            enddo      
         enddo
      enddo
      
      do mu=0,3
      do nu=0,3
         bmunu(mu,nu) = 0d0
         do pol1=1,2
         do pol2=1,2
         do hel_lep=-1,1,2             
         do hel_quark=-1,1,2               
c     we don't conjugate epsg(mu,pol1) since it is real
            bmunu(mu,nu) = bmunu(mu,nu) +                  
     #       Jlep_dot_Jquark_vec(pol1,hel_lep,hel_quark)
     #           *DCONJG(Jlep_dot_Jquark_vec(pol2,hel_lep,hel_quark))
     #           * epsg(mu,pol1) * epsg(nu,pol2)
         enddo
         enddo
         enddo
         enddo
      enddo
      enddo

      amp2 = amp2 * ph_unit_e**4 * (4*pi*st_alpha) 
c     1/4 from average over initial-state polarization
c     1/nc/(nc^2-1) * nc from average over initial-state colors and sum over   
c     quark colors
      amp2=  amp2*CF/4/(nc**2-1) 

      do mu=0,3
         do nu=0,3
            bmunu(mu,nu) = bmunu(mu,nu) * ph_unit_e**4*(4*pi*st_alpha)*
     #           CF/4/(nc**2-1)
         enddo
      enddo
      end



      subroutine zj_g_q_to_l_al_q(pphy,fermion_type,fermion_charge,
     #     amp2,bmunu)   
      implicit none
c the nleg 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      call zj_q_g_to_l_al_q(pphy,fermion_type,fermion_charge,amp2,bmunu)   
      end


      subroutine zj_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      include 'src/Lib/zj/LesHouches.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
c colours of incoming quarks, antiquarks
      integer icolqi(2),icolai(2),icolgi(2),
     #        icolqf(2),icolaf(2),icolgf(2)
      data icolqi/ 501, 0   /
      data icolai/ 0  , 502 /
      data icolgi/ 502, 501 /
      data icolqf/ 502, 0   /
      data icolaf/ 0  , 501 /
      data icolgf/ 501, 502 /
      save icolqi,icolai,icolgi,icolqf,icolaf,icolgf
c neutral particles
      icolup(1,3)=0
      icolup(2,3)=0
      icolup(1,4)=0
      icolup(2,4)=0
      do j=1,nlegborn
         if(j.eq.3.or.j.eq.4) then
            icolup(1,j)=0
            icolup(2,j)=0
         else
            if(idup(j).gt.0.and.idup(j).le.5) then
               if(j.le.2) then
                  icolup(1,j)=icolqi(1)
                  icolup(2,j)=icolqi(2)
               else
                  icolup(1,j)=icolqf(1)
                  icolup(2,j)=icolqf(2)
               endif
            elseif(idup(j).lt.0.and.idup(j).ge.-5) then
               if(j.le.2) then
                  icolup(1,j)=icolai(1)
                  icolup(2,j)=icolai(2)
               else
                  icolup(1,j)=icolaf(1)
                  icolup(2,j)=icolaf(2)
               endif
            elseif(idup(j).eq.21) then
               if(j.le.2) then
                  icolup(1,j)=icolgi(1)
                  icolup(2,j)=icolgi(2)
               else
                  icolup(1,j)=icolgf(1)
                  icolup(2,j)=icolgf(2)
               endif
            else
               write(*,*) ' invalid flavour'
               stop
            endif
         endif
      enddo
      end

      subroutine zj_finalize_lh
      implicit none
c     Set up the resonance whose mass must be preserved
c     on the Les Houches interface.
c     resonance Z -> e-(3) e+(4)
      call zj_add_resonance(23,3,4)
c     give zj_masses to final-state products
      call zj_lhefinitemasses
      end
