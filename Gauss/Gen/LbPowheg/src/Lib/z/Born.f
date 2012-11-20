      subroutine z_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/z/pwhg_math.h'
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      integer j,k,mu,nu
c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_i j=B* C_i
      call z_compborn(p,bflav,born,bbmunu)
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
                  colcf=cf
               endif
               bornjk(j,k)=born*colcf
               bornjk(k,j)=bornjk(j,k)
            enddo
         endif
      enddo
      end


      subroutine z_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      real * 8 p(0:3,nlegborn)
      integer bflav(nlegborn)
      real * 8 born,bmunu(0:3,0:3)

      real * 8 charge(-5:5)
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

      real *8 amp2

      integer ferm_type(nlegborn)
      integer i,j
      real * 8 ferm_charge(nlegborn)

c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/z_cvecbos/idvecbos,vdecaymode

      if (bflav(3).le.0.or.bflav(4).ge.0) then
         write(*,*) 'Error in z_compborn'
         stop
      endif
      ferm_type(3) = +1
      ferm_type(4) = -1

      if(mod(vdecaymode,2).eq.1) then
         ferm_charge(3) = -1d0
         ferm_charge(4) = +1d0
      elseif(mod(vdecaymode,2).eq.0) then
         ferm_charge(3) = 0d0
         ferm_charge(4) = 0d0
      else
         write(*,*) 'Error in vdecaymode in z_compborn'
         stop
      endif

      
c     i is the flavour index of first incoming parton
c     j is the flavour index of second incoming parton
c     with the convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      
      i = bflav(1)
      j = bflav(2)
      ferm_charge(1) = charge(i)
      ferm_charge(2) = charge(j)
      ferm_type(1) = i/abs(i)
      ferm_type(2) = j/abs(j)

      call z_q_aq_to_l_al(p,ferm_type,ferm_charge,amp2) 
      born=amp2

c     spin projected born amplitudes not needed, since no gluons in the Born
c     process
      do i=0,3
         do j=0,3
            bmunu(i,j)=0d0
         enddo
      enddo


      end


      subroutine z_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      include 'src/Lib/z/LesHouches.h'
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
c     neutral particles
      icolup(1,3)=0
      icolup(2,3)=0
      icolup(1,4)=0
      icolup(2,4)=0
c     colored particles
      if((idup(1).gt.0).and.(idup(2).lt.0)) then
         icolup(1,1)=501
         icolup(2,1)=0
         icolup(1,2)=0
         icolup(2,2)=501
      elseif((idup(1).lt.0).and.(idup(2).gt.0)) then
         icolup(1,1)=0
         icolup(2,1)=501
         icolup(1,2)=501
         icolup(2,2)=0
      else
         write(*,*) ' invalid flavour'
         stop
      endif
      end


      subroutine z_finalize_lh
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
c     
c     lepton masses
      real *8 lepmass(3),decmass
      common/z_clepmass/lepmass,decmass
c     Resonance Z -> e-(3) e+(4)
      call z_add_resonance(23,3,4)
c     The following routine also performs the reshuffling of momenta if
c     a massive decay is chosen
      call z_momenta_reshuffle(3,4,5,decmass)
      end



c     i1<i2
      subroutine z_momenta_reshuffle(ires,i1,i2,decmass)
      implicit none
      include 'src/Lib/z/LesHouches.h'
      integer ires,i1,i2,j
      real * 8 ptemp(0:3),ptemp1(0:3),beta(3),betainv(3),modbeta,decmass
      if (i1.ge.i2) then
         write(*,*) 'wrong sequence in z_momenta_reshuffle'
         stop
      endif
cccccccccccccccccccccccccccccc
c construct boosts from/to vector boson rest frame 
      do j=1,3
         beta(j)=-pup(j,ires)/pup(4,ires)
      enddo
      modbeta=sqrt(beta(1)**2+beta(2)**2+beta(3)**2)
      do j=1,3
         beta(j)=beta(j)/modbeta
         betainv(j)=-beta(j)
      enddo
cccccccccccccccccccccccccccccccccccccccc
c first decay product 
      ptemp(0)=pup(4,i1)
      do j=1,3
         ptemp(j)=pup(j,i1)
      enddo
      call z_mboost(1,beta,modbeta,ptemp,ptemp)
      ptemp1(0)=0.5d0*pup(5,ires)
      do j=1,3
         ptemp1(j)=ptemp(j)/ptemp(0)*sqrt(ptemp1(0)**2 -decmass**2)
      enddo
      call z_mboost(1,betainv,modbeta,ptemp1,ptemp)
      do j=1,3
         pup(j,i1)=ptemp(j)
      enddo
      pup(4,i1)=ptemp(0)
c abs to avoid tiny negative values in case of neutrinos
      pup(5,i1)=sqrt(abs(pup(4,i1)**2-pup(1,i1)**2
     $     -pup(2,i1)**2-pup(3,i1)**2))
      
c second decay product 

      ptemp(0)=pup(4,i2)
      do j=1,3
         ptemp(j)=pup(j,i2)
      enddo
      call z_mboost(1,beta,modbeta,ptemp,ptemp)
      ptemp1(0)=0.5d0*pup(5,ires)
      do j=1,3
         ptemp1(j)=ptemp(j)/ptemp(0)*sqrt(ptemp1(0)**2 -decmass**2)
      enddo
      call z_mboost(1,betainv,modbeta,ptemp1,ptemp)
      do j=1,3
         pup(j,i2)=ptemp(j)
      enddo
      pup(4,i2)=ptemp(0)
c abs to avoid tiny negative values in case of neutrinos
      pup(5,i2)=sqrt(abs(pup(4,i2)**2-pup(1,i2)**2
     $     -pup(2,i2)**2-pup(3,i2)**2))
cccccccccccccccccccccccccccccccccccccccc
      end





c this subroutine compute the Born amplitude for the process
c q(p1) aq(p2) -> Z(p3+p4)   con Z -> l-(p3) l+(p4) 
c NUMERICALLY, with the z_bra/z_ket formalism, not by squaring the analytic 
c amplitude
c It gets the matrix p with all the momenta and gives   
c the amplitude squared (amp2) averaged over initial 
c polarization     
c
c         q  --->-----
c                     |
c                     |            l-
c                     |          /  
c         aq ---<-----/\/\/\/\/\/
c                       Z/gamma \
c                                \ l+
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine z_q_aq_to_l_al(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=4)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      include 'src/Lib/z/PhysPars.h'
      include 'src/Lib/z/pwhg_math.h'      
      real * 8 p1(0:3),p2(0:3)
      real * 8 p34
      real * 8 z_dotp,tmp
      complex * 16 z_ccdotp
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
      complex*16 jlep(0:3,-1:1),jqua(0:3,-1:1)
      complex*16 jlep_dot_jqua
      integer mu,i,hel_lep,hel_qua
      real * 8 p(0:3,nleg)
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)
      integer utype_q,utype_l
      real * 8 q_q,v_q,a_q,q_l,v_l,a_l
      real*8 Zcoup_q(-1:1),Zcoup_l(-1:1)
      complex *16 prop34zeta,prop34gamma

      real * 8 teo_fact
      
      if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with Z decay'
         stop
      endif
     

c  copy of local variables
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
            call z_exchange_momenta(p(0,1),p(0,2))
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
      
      v_q = utype_q*1.d0/2 - 2*q_q*ph_sthw2 
      a_q = utype_q*1.d0/2
      Zcoup_q(-1) = v_q + a_q
      Zcoup_q(+1) = v_q - a_q

      v_l = utype_l*1.d0/2 - 2*q_l*ph_sthw2 
      a_l = utype_l*1.d0/2     
      Zcoup_l(-1) = v_l + a_l
      Zcoup_l(+1) = v_l - a_l
      
c     fake momenta:
c     for bosons always outgoing
c     for fermions along fermionic current
      do mu=0,3
         p1(mu) = ferm_type(1)*p(mu,1)
         p2(mu) = ferm_type(2)*p(mu,2)
      enddo

      p34=z_dotp(p(0,3),p(0,4))
      
c     Z propagator (see eq 2.13 in Nason-Webber,Nucl.Phys B421 pp.473-517)

c     Here, simply use teo_fact=1
      teo_fact=1

      prop34zeta=teo_fact/dcmplx(2d0*p34-ph_Zmass2,ph_ZmZw)!*2d0*p34/ph_Zmass2)
c     teo_fact is the square root of the ratio between the experimental
c     width of the Z boson and the theoretical one.  This last is
c     evaluated by summing over species and flavours the quantity
c     (g_a)^2+(g_v)^2, multiplying then for the appropriate factor.
c     In this way one can safely push Z width to zero, avoiding
c     the cross section to diverge

c     photon propagator
      prop34gamma=1d0/(2d0*p34)
     
           
c     z_bra and z_ket are made with actual momenta 

c     q
      call z_ket(p(0,1),ferm_type(1),psi1)
c     aq
      call z_bra(p(0,2),ferm_type(2),psi2)
c     em
      call z_bra(p(0,3),ferm_type(3),psi3)
c     ep
      call z_ket(p(0,4),ferm_type(4),psi4)
      
c     leptonic currents
      call z_bra_gamma_ket(psi3,psi4,-1,jlep(0,-1))
      call z_bra_gamma_ket(psi3,psi4,+1,jlep(0,1))
      
c     quark currents
      call z_bra_gamma_ket(psi2,psi1,-1,jqua(0,-1))
      call z_bra_gamma_ket(psi2,psi1,+1,jqua(0,1))

      amp2=0d0

      do hel_lep=-1,1,2
             
         do hel_qua=-1,1,2
                                
            jlep_dot_jqua = 
     #           z_ccdotp(jlep(0,hel_lep),jqua(0,hel_qua))

c     to take account of the gamma/Z interference         
            
    
            jlep_dot_jqua = (
     #           q_q*q_l*prop34gamma +
     #           prop34zeta*    
     #           Zcoup_q(hel_qua)*Zcoup_l(hel_lep)/
     #           (2*ph_sthw*ph_cthw)**2 )
     #           * jlep_dot_jqua
            
            amp2 = amp2 + jlep_dot_jqua *
     #           DCONJG(jlep_dot_jqua)       
            
            
                 
         enddo      
      enddo
      
      
      amp2 = amp2*ph_unit_e**4 
c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors
      amp2=  amp2/4d0/nc 
      
      end
