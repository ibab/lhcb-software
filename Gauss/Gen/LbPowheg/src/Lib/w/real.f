      subroutine w_setreal(p,fermion_flav,amp2)
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      include 'src/Lib/w/pwhg_math.h'
      include 'src/Lib/w/pwhg_st.h'
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
      parameter (nleg=nlegreal)
      real * 8 p(0:3,nleg)
      integer fermion_flav(nleg)
      real * 8 amp2
      integer ferm_type(nleg)
      real * 8 ferm_charge(nleg)
      integer i,j,k,l,count,tmp_type
      real *8 tmp_charge
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/w_cvecbos/idvecbos,vdecaymode  

c     i is the flavour index of first incoming parton
c     j is the flavour index of second incoming parton
c     k is the flavour of outgoing parton in the order particle,antiparticle,gluon
c     with the convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      
      i = fermion_flav(1)
      j = fermion_flav(2)
      k = fermion_flav(5)
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

c     antilepton-neutrino from W decay
      ferm_type(3) = fermion_flav(3)/abs(fermion_flav(3))
      ferm_charge(3) = ferm_type(3)*(-1d0)
      ferm_type(4) = -ferm_type(3)
      ferm_charge(4) = 0d0

      if(idvecbos.eq.24) then
         if (i.eq.0) then
c     g q -> W+ qp
            call w_g_aqp_to_al_vl_aq(p,ferm_type,ferm_charge,amp2)         
         elseif ((i.ne.0).and.(j.ne.0)) then
c     q aqp -> W+ g
            call w_q_aqp_to_al_vl_g(p,ferm_type,ferm_charge,amp2)
         elseif (j.eq.0) then
c     q g -> W+ qp
            call w_q_g_to_al_vl_qp(p,ferm_type,ferm_charge,amp2)
         else
            amp2 = 0d0
         endif
      elseif(idvecbos.eq.-24) then
         if (i.eq.0) then
c     g q -> W- qp
            call w_g_aqp_to_l_avl_aq(p,ferm_type,ferm_charge,amp2)         
         elseif ((i.ne.0).and.(j.ne.0)) then
c     q aqp -> W- g
            call w_q_aqp_to_l_avl_g(p,ferm_type,ferm_charge,amp2)
         elseif (j.eq.0) then
c     q g -> W- qp
            call w_q_g_to_l_avl_qp(p,ferm_type,ferm_charge,amp2)
         else
            amp2 = 0d0
         endif

      else
         write(*,*) 'ERROR: this subroutine deals only with W+ or W- '
         call exit(1)
      endif

      if (i.eq.0) i=abs(k)
      if (j.eq.0) j=abs(k)
      if(mod(abs(i),2).eq.0) then
         amp2=amp2*ph_CKM(abs(i)/2,(abs(j)+1)/2)**2
      elseif(mod(abs(i),2).eq.1) then   
         amp2=amp2*ph_CKM(abs(j)/2,(abs(i)+1)/2)**2
      endif
c     cancel as/(2pi) associated with amp2. It will be put back by w_real_ampsq
      amp2 = amp2/(st_alpha/(2*pi))
      
      end

c this subroutine compute the Born amplitude for the process
c q(p1) qp(p2) -> W(p3+p4) g(p5)   con W -> l(p3) vl(p4) 
c NUMERICALLY, with the w_bra_X/w_ket_X formalism, not by squaring the analytic 
c amplitude
c It gets the matrix p with all the momenta and gives   
c the amplitude squared (amp2) averaged over initial 
c polarization 
c
c         q  --->----------->------ g
c                     |
c                     |            l
c                     |           /  
c         aqp ---<-----/\/\/\/\/\/
c                           W    \
c                                 \ vl 
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
c     fermion_charge = +2/3, -1/3, -2/3, +1/3

      subroutine w_q_aqp_to_al_vl_g(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 fermion_charge(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 p1(0:3),p2(0:3),pp1(0:3),pp2(0:3),pp5(0:3)
      include 'src/Lib/w/pwhg_st.h'
      include 'src/Lib/w/pwhg_math.h'
      include 'src/Lib/w/PhysPars.h'
      real * 8 p34
      real * 8 w_dotp,tmp
      complex * 16 w_ccdotp
      real * 8 epsg(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
      complex*16 jlep(0:3),jqua(0:3)
      complex*16 jlep_dot_jqua
      integer mu,i,pol
      real * 8 p(0:3,nleg)
      real * 8 ferm_charge(nleg)
      integer ferm_type(nleg)
      complex *16 prop34w 

      if ((fermion_type(3).ne.-1).and.(fermion_type(4).ne.1)) then
         write(*,*) 'ERROR: this subroutine deals only with W+ decay'
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

c     w_bra_X and w_ket_X are built with physical momenta, but a check on positivity
c     of energy is needed when one uses this function to evaluate a
c     crossing related amplitudes. NOTE that this affects only the
c     construction of spinors or polarization vectors on external legs
c     that may be crossed. All other momenta are unchanged 
c
c     q
      if (p(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -p(mu,1)
         enddo         
         call w_ket_X(pp1,ferm_type(1),psi1)
      else
         call w_ket_X(p(0,1),ferm_type(1),psi1)
      endif
c     aq
      if (p(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -p(mu,2)
         enddo         
         call w_bra_X(pp2,ferm_type(2),psi2)
      else
         call w_bra_X(p(0,2),ferm_type(2),psi2)
      endif   
c     l
      call w_ket_X(p(0,3),ferm_type(3),psi3)
c     vl
      call w_bra_X(p(0,4),ferm_type(4),psi4)

c     leptonic current
      call w_bra_gamma_ket(psi4,psi3,-1,jlep)
      

      amp2=0d0

      do  pol=1,2
         
         if (p(0,5).lt.0d0) then
            do mu=0,3
               pp5(mu) = -p(mu,5)
            enddo         
            call w_POLVEC(pp5,pol,epsg(0,pol))
         else   
            call w_POLVEC(p(0,5),pol,epsg(0,pol))
         endif
         
         call w_bra_gamma_ket_gluon(psi2,psi1,-1,p2,p1,
     #        p(0,5),epsg(0,pol),jqua)

         jlep_dot_jqua = w_ccdotp(jlep,jqua)*prop34w
         
         amp2 = amp2 + jlep_dot_jqua *
     #     DCONJG(jlep_dot_jqua)  
      
      enddo     
      
      amp2 = amp2*((ph_unit_e/ph_sthw)**4)/4d0 *(4d0*pi*st_alpha)

c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors 
      amp2=  amp2*CF/4d0/nc

      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C AMPLITUDES RELATES BY CROSSING:
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      subroutine w_g_aqp_to_al_vl_aq(pphy,fermion_type,fermion_charge,
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
c NOTE the MINUS sign     !!!
      ferm_charge(1) = -ferm_charge(5)

c     if the following two lines are missing 
      ferm_type(5)=0
      ferm_charge(5)=0d0 
c     ferm_type(5) and ferm_charge(5) don't contain
c     their correct values, but this does not affect 
c     the correct call of

       call w_q_aqp_to_al_vl_g(pp,ferm_type,ferm_charge,
     #     amp2)

c     correct for color average
      amp2 = amp2 * 3d0/8d0
      
      end

ccccccccccccccccccccccccccccccccccccccccccccc

      subroutine w_q_g_to_al_vl_qp(pphy,fermion_type,fermion_charge,
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

       call w_q_aqp_to_al_vl_g(pp,ferm_type,ferm_charge,
     #     amp2)

c     correct for color average
      amp2 = amp2 * 3d0/8d0
      
      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      subroutine w_q_aqp_to_l_avl_g(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
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
            
      
      call w_q_aqp_to_al_vl_g(pphy,ferm_type,ferm_charge,
     #     amp2)

      end

ccccccccccccccccccccccccccccccccccccccccccccc

       subroutine w_g_aqp_to_l_avl_aq(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
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
            
      
      call w_g_aqp_to_al_vl_aq(pphy,ferm_type,ferm_charge,
     #     amp2)

      end

ccccccccccccccccccccccccccccccccccccccccccccc

       subroutine w_q_g_to_l_avl_qp(pphy,fermion_type,fermion_charge,
     #     amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
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
            
      
      call w_q_g_to_al_vl_qp(pphy,ferm_type,ferm_charge,
     #     amp2)

      end
