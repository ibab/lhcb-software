      subroutine z_setreal(p,rflav,amp2real)
      implicit none
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      include 'src/Lib/z/pwhg_math.h'
      include 'src/Lib/z/pwhg_st.h'
      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2real

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

      real *8 amp2,ploc(0:3,nlegreal)

      integer i,mu,ii(nlegreal)
      integer ferm_type(nlegreal)
      real * 8 ferm_charge(nlegreal)

c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/z_cvecbos/idvecbos,vdecaymode

      logical check,cond
      parameter (check=.false.)

      do i=1,nlegreal
         ii(i)=rflav(i)
         do mu=0,3
            ploc(mu,i)=p(mu,i)
         enddo
      enddo

      if(check) then
         do mu=0,3
            if(
     $dabs(ploc(mu,1)+ploc(mu,2)-ploc(mu,3)-ploc(mu,4)-ploc(mu,5))
     $.gt.1d-6) then
               write(*,*) 'momentum non conservation in z_setreal'
               stop
            endif
         enddo
         if(ii(3).le.0.or.ii(4).ge.0) then
            write(*,*) 'Error in z_setreal'
            stop
         endif
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
         write(*,*) 'Error in vdecaymode in z_setreal'
         stop
      endif

      if((ii(1)+ii(2).eq.0).and.(ii(1)*ii(2).ne.0)) then
c     q q -> e- e+ g
         cond=.false.
         if((ii(1).eq.1).or.(ii(1).eq.3).or.(ii(1).eq.5)) then
c     d d~ -> Z g
            ferm_charge(1) = charge(1)
            ferm_charge(2) = charge(-1)
            ferm_charge(5) = 0d0
            ferm_type(1) = 1
            ferm_type(2) = -1
            ferm_type(5) = 0
            cond=.true.
         elseif((ii(1).eq.-1).or.(ii(1).eq.-3).or.(ii(1).eq.-5)) then
c     d~ d -> Z g
            ferm_charge(1) = charge(-1)
            ferm_charge(2) = charge(1)
            ferm_charge(5) = 0d0
            ferm_type(1) = -1
            ferm_type(2) = 1
            ferm_type(5) = 0
            cond=.true.
         elseif((ii(1).eq.2).or.(ii(1).eq.4)) then
c     u u~ -> Z g
            ferm_charge(1) = charge(2)
            ferm_charge(2) = charge(-2)
            ferm_charge(5) = 0d0
            ferm_type(1) = 1
            ferm_type(2) = -1
            ferm_type(5) = 0
            cond=.true.
         elseif((ii(1).eq.-2).or.(ii(1).eq.-4)) then
c     u~ u -> Z g
            ferm_charge(1) = charge(-2)
            ferm_charge(2) = charge(2)
            ferm_charge(5) = 0d0
            ferm_type(1) = -1
            ferm_type(2) = 1
            ferm_type(5) = 0
            cond=.true.
         endif
         
         if(cond) then
            call z_q_aq_to_l_al_g(ploc,ferm_type,ferm_charge,amp2)         
         else
            write(*,*) 'Error in z_setreal'
            stop
         endif

      elseif((ii(1).eq.0).and.(ii(2).eq.ii(5))) then
c     g q -> e- e+ q
         cond=.false.
         if((ii(2).eq.1).or.(ii(2).eq.3).or.(ii(2).eq.5)) then
c     g d -> Z d
            ferm_charge(1) = 0d0
            ferm_charge(2) = charge(1)
            ferm_charge(5) = charge(1)
            ferm_type(1) = 0
            ferm_type(2) = 1
            ferm_type(5) = 1
            cond=.true.
         elseif((ii(2).eq.-1).or.(ii(2).eq.-3).or.(ii(2).eq.-5)) then
c     g d~ -> Z d~
            ferm_charge(1) = 0d0
            ferm_charge(2) = charge(-1)
            ferm_charge(5) = charge(-1)
            ferm_type(1) = 0
            ferm_type(2) = -1
            ferm_type(5) = -1   
            cond=.true.
         elseif((ii(2).eq.2).or.(ii(2).eq.4)) then
c     g u -> Z u
            ferm_charge(1) = 0d0
            ferm_charge(2) = charge(2)
            ferm_charge(5) = charge(2)
            ferm_type(1) = 0
            ferm_type(2) = 1
            ferm_type(5) = 1
            cond=.true.
         elseif((ii(2).eq.-2).or.(ii(2).eq.-4)) then
c     g u~ -> Z u~
            ferm_charge(1) = 0d0
            ferm_charge(2) = charge(-2)
            ferm_charge(5) = charge(-2)
            ferm_type(1) = 0
            ferm_type(2) = -1
            ferm_type(5) = -1            
            cond=.true.
         endif
         if(cond) then
            call z_g_q_to_l_al_q(ploc,ferm_type,ferm_charge,amp2)
         else
            write(*,*) 'Error in z_setreal'
            stop
         endif
         
      elseif((ii(2).eq.0).and.(ii(1).eq.ii(5))) then
c     q g -> e- e+ q
         cond=.false.
         if((ii(1).eq.1).or.(ii(1).eq.3).or.(ii(1).eq.5)) then
c     d g -> Z d
            ferm_charge(2) = 0d0
            ferm_charge(1) = charge(1)
            ferm_charge(5) = charge(1)
            ferm_type(2) = 0
            ferm_type(1) = 1
            ferm_type(5) = 1
            cond=.true.
         elseif((ii(1).eq.-1).or.(ii(1).eq.-3).or.(ii(1).eq.-5)) then
c     d~ g -> Z d~
            ferm_charge(2) = 0d0
            ferm_charge(1) = charge(-1)
            ferm_charge(5) = charge(-1)
            ferm_type(2) = 0
            ferm_type(1) = -1
            ferm_type(5) = -1
            cond=.true.
         elseif((ii(1).eq.2).or.(ii(1).eq.4)) then
c     u g  -> Z u
            ferm_charge(2) = 0d0
            ferm_charge(1) = charge(2)
            ferm_charge(5) = charge(2)
            ferm_type(2) = 0
            ferm_type(1) = 1
            ferm_type(5) = 1
            cond=.true.
         elseif((ii(1).eq.-2).or.(ii(1).eq.-4)) then
c     u~ g -> Z u~
            ferm_charge(2) = 0d0
            ferm_charge(1) = charge(-2)
            ferm_charge(5) = charge(-2)
            ferm_type(2) = 0
            ferm_type(1) = -1
            ferm_type(5) = -1   
            cond=.true.
         endif
         if(cond) then
            call z_q_g_to_l_al_q(ploc,ferm_type,ferm_charge,amp2)
         else
            write(*,*) 'Error in z_setreal'
            stop
         endif

      else
         write(*,*) 'Error in z_setreal'
         stop
      endif

      amp2real=amp2

c     cancel as/(2pi) associated with amp2. It will be put back by z_real_ampsq
      amp2real = amp2real/(st_alpha/(2*pi))

      end




c this subroutine compute the Born amplitude for the process
c q(p1) aq(p2) -> Z(p3+p4) g(p5)  con Z -> f(p3) af(p4) 
c NUMERICALLY, with the z_bra/z_ket formalism, not by squaring the analytic 
c amplitude
c It gets the matrix p with all the momenta and gives   
c the amplitude squared (amp2) averaged over initial 
c polarization     
c
c         q  --->----------->------ g
c                     |
c                     |            f
c                     |          /  
c         aq ---<-----/\/\/\/\/\/
c                       Z/gamma \
c                                \ af
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine z_q_aq_to_l_al_g(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the nleg 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      include 'src/Lib/z/PhysPars.h'
      include 'src/Lib/z/pwhg_math.h'  
      include 'src/Lib/z/pwhg_st.h'  
      real * 8 p1(0:3),p2(0:3),pp1(0:3),pp2(0:3),pp5(0:3)
      real * 8 p34
      real * 8 z_dotp,tmp
      complex * 16 z_ccdotp
      real * 8 epsg(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
      complex*16 jlep(0:3,-1:1),jqua(0:3,-1:1)
      complex*16 jlep_dot_jqua
      integer mu,i,pol,hel_lep,hel_qua
      real * 8 p(0:3,nleg)
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)
      integer utype_q,utype_f
      real * 8 q_q,v_q,a_q,q_f,v_f,a_f
      real*8 Zcoup_q(-1:1),Zcoup_f(-1:1)
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

c exchance particle 1 and 2
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
         utype_f = -1
         q_f = -1d0
      elseif (abs(ferm_charge(3)).eq.0d0) then
         utype_f = +1
         q_f = 0d0
      elseif (abs(ferm_charge(3)).eq.1d0/3) then
         utype_f = -1
         q_f = -1d0/3
      elseif (abs(ferm_charge(3)).eq.2d0/3) then
         utype_f = +1
         q_f = 2d0/3
      else
         write(*,*) 'What charge is this??',ferm_charge(3)
         stop
      endif
      
      v_q = utype_q*1.d0/2 - 2*q_q*ph_sthw2 
      a_q = utype_q*1.d0/2
      Zcoup_q(-1) = v_q + a_q
      Zcoup_q(+1) = v_q - a_q

      v_f = utype_f*1.d0/2 - 2*q_f*ph_sthw2 
      a_f = utype_f*1.d0/2     
      Zcoup_f(-1) = v_f + a_f
      Zcoup_f(+1) = v_f - a_f

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

      prop34zeta=teo_fact/dcmplx(2d0*p34-ph_Zmass2,ph_ZmZw)!*2d0*p34/mz2)
c     teo_fact is the square root of the ratio between the experimental
c     width of the Z boson and the theoretical one.  This last is
c     evaluated by summing over species and flavours the quantity
c     (g_a)^2+(g_v)^2, multiplying then for the appropriate factor.
c     In this way one can safely push Z width to zero, avoiding
c     the cross section to diverge

c     photon propagator
      prop34gamma=1d0/(2d0*p34)
     

c     z_bra and z_ket are made with actual momenta,but a check on positivity
c     of energy is needed when one uses this function to evaluate a
c     crossing related amplitudes. NOTE that this affect only the
c     construction of spinors or polarization vectors on external leg
c     that may be crossed, all other momenta are unchanged 
c
c     q
      if (p(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -p(mu,1)
         enddo         
         call z_ket(pp1,ferm_type(1),psi1)
      else
         call z_ket(p(0,1),ferm_type(1),psi1)
      endif
c     aq
      if (p(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -p(mu,2)
         enddo         
         call z_bra(pp2,ferm_type(2),psi2)
      else
         call z_bra(p(0,2),ferm_type(2),psi2)
      endif   
c     em
      call z_bra(p(0,3),ferm_type(3),psi3)
c     ep
      call z_ket(p(0,4),ferm_type(4),psi4)
      
c     leptonic currents
      call z_bra_gamma_ket(psi3,psi4,-1,jlep(0,-1))
      call z_bra_gamma_ket(psi3,psi4,+1,jlep(0,1))
      
      amp2=0d0

      do  pol=1,2

         if (p(0,5).lt.0d0) then
            do mu=0,3
               pp5(mu) = -p(mu,5)
            enddo         
            call z_POLVEC(pp5,pol,epsg(0,pol))
         else   
            call z_POLVEC(p(0,5),pol,epsg(0,pol))
         endif

                  
         do hel_lep=-1,1,2
             
            do hel_qua=-1,1,2
               
               call z_bra_gamma_ket_gluon(psi2,psi1,hel_qua,p2,p1,
     #              p(0,5),epsg(0,pol),jqua(0,hel_qua))
               
   
               jlep_dot_jqua = 
     #               z_ccdotp(jlep(0,hel_lep),jqua(0,hel_qua))

c to take account of the gamma/Z interference         

               jlep_dot_jqua = (
     #              q_q*q_f*prop34gamma +
     #              prop34zeta*    
     #              Zcoup_q(hel_qua)*Zcoup_f(hel_lep)/
     #              (2*ph_sthw*ph_cthw)**2 )
     #              * jlep_dot_jqua
     
               amp2 = amp2 + jlep_dot_jqua *
     #              DCONJG(jlep_dot_jqua)    
        
            enddo      
         enddo
         
      enddo
      amp2 = amp2* ph_unit_e**4 * (4*pi*st_alpha)
c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors 
      amp2=  amp2*CF/4d0/nc

      end



CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C AMPLITUDES RELATES BY CROSSING:
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

c this subroutine compute the born amplitude for the process
c q(p1) g(p2) -> Z(p3+p4) q(p5)  con Z -> l-(p3) l+(p4) 
c NUMERICALLY, with the z_bra/z_ket formalism, not by squaring the analytic 
c amplitude. This amplitude is obtained by crossing the formula for 
c z_q_aq_to_l_al_g
      
      subroutine z_q_g_to_l_al_q(pphy,fermion_type,fermion_charge,
     #     amp2)
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg), ferm_charge(nleg)
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i


      do i = 1,nleg
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo

      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,5) = -pphy(mu,2)
         pp(mu,2) = -pphy(mu,5)
      enddo
      
c no useful information is in ferm_type(2) or ferm_charge(2), 
c since it's the gluon, BEFORE the following exchange
      ferm_type(2) = -ferm_type(5)
c NOTE the MINUS sign     !!!
      ferm_charge(2) = -ferm_charge(5)

c     if the following two lines are missing 
      ferm_type(5)=0
      ferm_charge(5)=0d0 
c     ferm_type(5) and ferm_charge(5) don't contain
c     their correct values, but this does not affect 
c     the correct call of
      
      call z_q_aq_to_l_al_g(pp,ferm_type,ferm_charge,
     #     amp2)

c     correct for color average
      amp2 = amp2 * 3d0/8d0

      end


ccccccccccccccccccccccccccccccccccccccccccccc
c this subroutine compute the Born amplitude for the process
c g(p1) q(p2) -> Z(p3+p4) q(p5)   con Z -> l-(p3) l+(p4) 
c NUMERICALLY, with the z_bra/z_ket formalism, not by squaring the analytic 
c amplitude. This amplitude is obtained by crossing the formula for 
c z_q_aq_to_l_al_g


      subroutine z_g_q_to_l_al_q(pphy,fermion_type,fermion_charge,
     #     amp2)
      
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg), ferm_charge(nleg)
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i


      do i = 1,nleg
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_charge(i) = fermion_charge(i)
         ferm_type(i) = fermion_type(i)
      enddo

      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,5) = -pphy(mu,1)
         pp(mu,1) = -pphy(mu,5)
      enddo
      
c no useful information is in ferm_type(1) or ferm_charge(1), 
c since it's the gluon, BEFORE the following exchange
      ferm_type(1) = -ferm_type(5)
c NOTE the MINUS sign!!!
      ferm_charge(1) = -ferm_charge(5)

          
      call z_q_aq_to_l_al_g(pp,ferm_type,ferm_charge,
     #     amp2)
      
c     correct for color average
      amp2 = amp2 *3d0/8d0
     
      end
