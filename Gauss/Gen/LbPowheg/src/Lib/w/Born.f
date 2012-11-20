      subroutine w_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/w/pwhg_math.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      integer j,k,mu,nu
c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_ij=Cj * B,
c where i#j
      call w_compborn(p,bflav,born,bbmunu)
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


c     Example
c     q q'-> e+ ve~
      subroutine w_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
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
c      include 'src/Lib/w/QuarkFlavs.h'
      include 'src/Lib/w/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer bflav(nleg)
      real * 8 amp2,born,bmunu(0:3,0:3)
      integer ferm_type(nleg)
      integer i,j
      real * 8 ferm_charge(nleg)
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/w_cvecbos/idvecbos,vdecaymode   

      if (abs(bflav(3)).le.6.or.abs(bflav(4)).le.6) then
         write(*,*) 'born_ampsq: ERROR in flavor assignement'
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


c     antilepton-neutrino from W decay
      ferm_type(3) = bflav(3)/abs(bflav(3))
      ferm_charge(3) = ferm_type(3)*(-1d0)
      ferm_type(4) = -ferm_type(3)
      ferm_charge(4) = 0d0

      
      if(idvecbos.eq.24) then
         call w_q_aqp_to_al_vl(p,ferm_type,ferm_charge,
     $        amp2)
      elseif(idvecbos.eq.-24) then
         call w_q_aqp_to_l_avl(p,ferm_type,ferm_charge,
     $        amp2)
      else
         write(*,*) 'ERROR: this subroutine deals only with W+ or W- '
         call exit(1)
      endif
      
      
      if(mod(abs(i),2).eq.0) then
         born=amp2*ph_CKM(abs(i)/2,(abs(j)+1)/2)**2
      elseif(mod(abs(i),2).eq.1) then   
         born=amp2*ph_CKM(abs(j)/2,(abs(i)+1)/2)**2
      endif

      do i=0,3
         do j=0,3
            bmunu(i,j)=0d0
         enddo
      enddo

      end


c this subroutine compute the Born amplitude for the process
c q(p1) qp(p2) -> W(p3+p4)   con W -> l(p3) vl(p4) 
c NUMERICALLY, with the w_bra_X/w_ket_X formalism, not by squaring the analytic 
c amplitude
c It gets the matrix pphy with all the momenta and gives   
c the amplitude squared (amp2) averaged over initial 
c polarization     
c
c         q  --->-----
c                     |
c                     |            l
c                     |          /  
c         qp ---<-----/\/\/\/\/\/
c                        W      \
c                                \ vl
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine w_q_aqp_to_al_vl(pphy,fermion_type,fermion_charge,
     $     amp2)
   
      implicit none
c the nleg 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=4)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 p1(0:3),p2(0:3)
      include 'src/Lib/w/pwhg_st.h'
      include 'src/Lib/w/pwhg_math.h'
      include 'src/Lib/w/PhysPars.h'
      real * 8 p34
      real * 8 w_dotp,tmp
      complex * 16 w_ccdotp
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
      complex*16 jlep(0:3),jqua(0:3)
      complex*16 jlep_dot_jqua
      integer mu,i
      real * 8 p(0:3,nleg)
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)
      complex *16 prop34w      

      if ((fermion_type(3).ne.-1).and.(fermion_type(4).ne.1)) then
         write(*,*) 'ERROR: this subroutine deals only with W+ decay'
         stop
      endif
     

c  copy to local variables
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
            call w_exchange_momenta(p(0,1),p(0,2))
            tmp = ferm_charge(1)
            ferm_charge(1)=ferm_charge(2)
            ferm_charge(2)=tmp
            tmp = ferm_type(1)
            ferm_type(1)=ferm_type(2)
            ferm_type(2)=tmp
         else
            write(*,*) 'Error in the type of the quark 1-2'
            stop
         endif
      endif

      
c     fake momenta:
c     for bosons always outgoing
c     for fermions along fermionic current
      do mu=0,3
         p1(mu) = ferm_type(1)*p(mu,1)
         p2(mu) = ferm_type(2)*p(mu,2)
      enddo

      p34=w_dotp(p(0,3),p(0,4))
      
c     W propagator
      prop34w=1d0/dcmplx(2d0*p34-ph_Wmass2,ph_WmWw)
     
c     w_bra_X and w_ket_X are built with physical momenta 

c     q
      call w_ket_X(p(0,1),ferm_type(1),psi1)
c     qp
      call w_bra_X(p(0,2),ferm_type(2),psi2)
c     l
      call w_ket_X(p(0,3),ferm_type(3),psi3)
c     vl
      call w_bra_X(p(0,4),ferm_type(4),psi4)
      
c     leptonic current
      call w_bra_gamma_ket(psi4,psi3,-1,jlep)
      
c     quark current
      call w_bra_gamma_ket(psi2,psi1,-1,jqua)
      


      amp2=0d0

      
      jlep_dot_jqua = w_ccdotp(jlep,jqua)*prop34w
            
      amp2 = amp2 + jlep_dot_jqua *
     $     DCONJG(jlep_dot_jqua)       
            
            
                 
      amp2 = amp2*(ph_unit_e/ph_sthw)**4/4d0 
c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors 
      amp2=  amp2/4d0/nc 
      
      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      subroutine w_q_aqp_to_l_avl(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=4)
      integer fermion_type(nleg),i
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
      
         ferm_charge(i) = -fermion_charge(i)
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call w_q_aqp_to_al_vl(pphy,ferm_type,ferm_charge,
     #     amp2)

      end

      subroutine w_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      include 'src/Lib/w/LesHouches.h'
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
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

      subroutine w_finalize_lh
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
c     
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/w_cvecbos/idvecbos,vdecaymode
c     lepton masses
      real *8 lepmass(3),decmass
      common/w_clepmass/lepmass,decmass

      call w_add_resonance(idvecbos,3,4)
c     The following routine also performs the reshuffling of momenta if
c     a massive decay is chosen
      call w_momenta_reshuffle(3,4,5,decmass)
      end



c     i1<i2
      subroutine w_momenta_reshuffle(ires,i1,i2,decmass)
      implicit none
      include 'src/Lib/w/LesHouches.h'
      integer ires,i1,i2,j
      real * 8 ptemp(0:3),ptemp1(0:3),beta(3),betainv(3),modbeta,decmass
      if (i1.ge.i2) then
         write(*,*) 'wrong sequence in w_momenta_reshuffle'
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
c first decay product (massive)
      ptemp(0)=pup(4,i1)
      do j=1,3
         ptemp(j)=pup(j,i1)
      enddo
      call w_mboost(1,beta,modbeta,ptemp,ptemp)
      ptemp1(0)=0.5d0*(pup(5,ires)+(decmass**2)/pup(5,ires))
      do j=1,3
         ptemp1(j)=ptemp(j)/ptemp(0)*sqrt(ptemp1(0)**2 -decmass**2)
      enddo
      call w_mboost(1,betainv,modbeta,ptemp1,ptemp)
      do j=1,3
         pup(j,i1)=ptemp(j)
      enddo
      pup(4,i1)=ptemp(0)
      pup(5,i1)=sqrt(pup(4,i1)**2-pup(1,i1)**2
     $     -pup(2,i1)**2-pup(3,i1)**2)
      
c second decay product (massless)

      ptemp(0)=pup(4,i2)
      do j=1,3
         ptemp(j)=pup(j,i2)
      enddo
      call w_mboost(1,beta,modbeta,ptemp,ptemp)
      ptemp1(0)=0.5d0*(pup(5,ires)-(decmass**2)/pup(5,ires))
      do j=1,3
         ptemp1(j)=ptemp(j)/ptemp(0)*ptemp1(0)
      enddo
      call w_mboost(1,betainv,modbeta,ptemp1,ptemp)
      do j=1,3
         pup(j,i2)=ptemp(j)
      enddo
      pup(4,i2)=ptemp(0)
c abs to avoid tiny negative values
      pup(5,i2)=sqrt(abs(pup(4,i2)**2-pup(1,i2)**2
     $     -pup(2,i2)**2-pup(3,i2)**2))
cccccccccccccccccccccccccccccccccccccccc
      end

