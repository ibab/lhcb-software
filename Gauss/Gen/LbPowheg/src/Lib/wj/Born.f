      subroutine wj_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      integer j,k,mu,nu
ccccccccccccccccccccccccccccc
c$$$      logical debug
c$$$      parameter(debug=.true.)
c$$$      include 'src/Lib/wj/pwhg_st.h'
c$$$      include 'src/Lib/wj/PhysPars.h'
c$$$      include 'src/Lib/wj/scale.h'
c$$$      include 'src/Lib/wj/qcdcouple.h'
c$$$      
c$$$      real * 8 pmcfm(12,1:4),bornmcfm
c$$$      double precision msq0(-5:5,-5:5)
c$$$      real *8 tiny
c$$$      parameter (tiny=1d-6)
c$$$      integer i
cccccccccccccccccccccccccccc


c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_i j=B*(C_i+C_j-C_k)/2,
c where k#i,j
      call wj_compborn(p,bflav,born,bbmunu)
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
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c$$$c Compare Born with MCFM (needed to evaluate MCFM virtual)
c$$$      if(debug) then
c$$$      call wj_virtual_initialize_MCFM
c$$$      wj_scale = sqrt(st_muren2)
c$$$      musq=st_muren2            ! renormalization wj_scale squared
c$$$      as = st_alpha
c$$$      call wj_mom_to_MCFM(p,pmcfm)
c$$$      call wj_qqb_w_g(pmcfm,msq0)
c$$$
c$$$      bornMCFM=msq0(bflav(1),bflav(2))
c$$$
c$$$      i = bflav(1)
c$$$      j = bflav(2)
c$$$      k = bflav(5)
c$$$      if (i.eq.0) i=abs(k)
c$$$      if (j.eq.0) j=abs(k)
c$$$      if(mod(abs(i),2).eq.0) then
c$$$         bornMCFM=bornMCFM*ph_CKM(abs(i)/2,(abs(j)+1)/2) **2
c$$$      elseif(mod(abs(i),2).eq.1) then   
c$$$         bornMCFM=bornMCFM*ph_CKM(abs(j)/2,(abs(i)+1)/2) **2
c$$$      endif
c$$$
c$$$      if  (abs(bornMCFM/born -1d0).gt.tiny) then 
c$$$         print *,bflav,'BORN  MUST BE EQUAL =====> ', bornMCFM, born,
c$$$     $        ' RATIO: ', bornMCFM/born
c$$$         stop
c$$$      endif
c$$$      
c$$$      if  ((abs(bornMCFM-born)/bornMCFM).gt.tiny) then 
c$$$         print *,bflav,' B0RN MUST BE 0 =====> ', abs(bornMCFM-born)
c$$$     $        /bornMCFM
c$$$         stop
c$$$      endif
c$$$      endif
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      end




      subroutine wj_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer bflav(nleg)
      real * 8 amp2,born,bmunu(0:3,0:3)
      integer ferm_type(nleg)
      integer i,j,k,mu,nu
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/wj_cvecbos/idvecbos,vdecaymode  
     

      if (abs(bflav(3)).le.6.or.abs(bflav(4)).le.6) then
         write(*,*) 'born_ampsq: ERROR in flavor assignement'
         stop
      endif


c     lepton-neutrino from W decay
      ferm_type(3) = bflav(3)/abs(bflav(3))
      ferm_type(4) = -ferm_type(3)

c     i is the flavour index of first incoming parton
c     j is the flavour index of second incoming parton
c     k is the flavour of outgoing parton in the order particle,antiparticle,gluon
c     with the convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      
      i = bflav(1)
      j = bflav(2)
      k = bflav(5)

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



      if(idvecbos.eq.24) then
         if (i.eq.0) then
c     g q -> W+ qp
            call wj_g_aqp_to_al_vl_aq(p,ferm_type,amp2,bmunu)         
         elseif ((i.ne.0).and.(j.ne.0)) then
c     q aqp -> W+ g
            call wj_q_aqp_to_al_vl_g(p,ferm_type,amp2,bmunu)
         elseif (j.eq.0) then
c     q g -> W+ qp
            call wj_q_g_to_al_vl_qp(p,ferm_type,amp2,bmunu)
         else
            write(*,*) 'Invalid bflav in wj_compborn'
            amp2 = 0d0
         endif
      elseif(idvecbos.eq.-24) then
         if (i.eq.0) then
c     g q -> W- qp
            call wj_g_aqp_to_l_avl_aq(p,ferm_type,amp2,bmunu)         
         elseif ((i.ne.0).and.(j.ne.0)) then
c     q aqp -> W- g
            call wj_q_aqp_to_l_avl_g(p,ferm_type,amp2,bmunu)
         elseif (j.eq.0) then
c     q g -> W- qp
            call wj_q_g_to_l_avl_qp(p,ferm_type,amp2,bmunu)
         else
            write(*,*) 'Invalid bflav in wj_compborn'
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
         do mu=0,3
            do nu=0,3
               bmunu(mu,nu)=bmunu(mu,nu)*ph_CKM(abs(i)/2,(abs(j)+1)/2)
     $              **2
            enddo
         enddo
      elseif(mod(abs(i),2).eq.1) then   
         amp2=amp2*ph_CKM(abs(j)/2,(abs(i)+1)/2)**2
         do mu=0,3
            do nu=0,3
               bmunu(mu,nu)=bmunu(mu,nu)*ph_CKM(abs(j)/2,(abs(i)+1)/2)
     $              **2
            enddo
         enddo
      endif

      born=amp2
      end


c this subroutine compute the Born amplitude for the process
c q(p1) qp(p2) -> W(p3+p4) g(p5)   con W -> l(p3) vl(p4)
c NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic
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


      subroutine wj_q_aqp_to_al_vl_g(pphy,fermion_type,amp2,bmunu)
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      complex *16 unit_I
      parameter (unit_I=(0,1))
      real * 8 p1(0:3),p2(0:3),pp1(0:3),pp2(0:3),pp5(0:3)
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/PhysPars.h'
      real * 8 p34
      real * 8 wj_dotp,tmp
      complex * 16 wj_ccdotp
      real * 8 epsg(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)
      complex*16 jlep(0:3),jqua(0:3)
      complex*16 Jlep_dot_Jquark,Jlep_dot_Jquark_vec(2)
      integer mu,nu,i,pol,pol1,pol2
      real * 8 p(0:3,nleg)
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
         ferm_type(i) = fermion_type(i)
      enddo

c     exchance particle 1 and 2
      if (ferm_type(1).eq.-1) then
         if (ferm_type(2).eq.1) then
            call wj_exchange_momenta(p(0,1),p(0,2))
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

      p34=wj_dotp(p(0,3),p(0,4))
      
c     W propagator
      prop34w=1d0/dcmplx(2d0*p34-ph_Wmass2,ph_WmWw)

c     wj_bra and wj_ket are built with physical momenta, but a check on positivity
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
         call wj_ket(pp1,ferm_type(1),psi1)
      else
         call wj_ket(p(0,1),ferm_type(1),psi1)
      endif
c     aq
      if (p(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -p(mu,2)
         enddo         
         call wj_bra(pp2,ferm_type(2),psi2)
      else
         call wj_bra(p(0,2),ferm_type(2),psi2)
      endif   
c     l
      call wj_ket(p(0,3),ferm_type(3),psi3)
c     vl
      call wj_bra(p(0,4),ferm_type(4),psi4)

c     leptonic current
      call wj_bra_gamma_ket(psi4,psi3,-1,jlep)
      

      amp2=0d0

      do  pol=1,2
         
         if (p(0,5).lt.0d0) then
            do mu=0,3
               pp5(mu) = -p(mu,5)
            enddo         
            call wj_POLVEC(pp5(0),pol,epsg(0,pol))
         else   
            call wj_POLVEC(p(0,5),pol,epsg(0,pol))
         endif
         Jlep_dot_Jquark_vec(pol)=0d0
         call wj_bra_gamma_ket_gluon(psi2,psi1,-1,p2,p1,
     $        p(0,5),epsg(0,pol),jqua)

         Jlep_dot_Jquark = wj_ccdotp(jlep,jqua)*prop34w
         Jlep_dot_Jquark_vec(pol) =
     $              Jlep_dot_Jquark_vec(pol) +
     $              Jlep_dot_Jquark
        
         amp2 = amp2 + Jlep_dot_Jquark *
     $     DCONJG(Jlep_dot_Jquark)  
      
      enddo     
      
      do mu=0,3
      do nu=0,3
         bmunu(mu,nu) = 0d0
         do pol1=1,2
         do pol2=1,2
c     we don't conjugate epsg(mu,pol1) since it is real
            bmunu(mu,nu) = bmunu(mu,nu) +                  
     $       Jlep_dot_Jquark_vec(pol1)
     $           *DCONJG(Jlep_dot_Jquark_vec(pol2))
     $           * epsg(mu,pol1) * epsg(nu,pol2)
         enddo
         enddo
      enddo
      enddo



      amp2 = amp2*((ph_unit_e/ph_sthw)**4)/4d0 *(4d0*pi*st_alpha)

c     1/4 from average over initial-state polarization
c     1/nc^2 * nc = 1/nc from average over initial-state colors and sum over 
c     quark colors 
      amp2=  amp2*CF/4d0/nc

      do mu=0,3
         do nu=0,3
            bmunu(mu,nu) = bmunu(mu,nu) * ((ph_unit_e/ph_sthw)**4)/4d0
     $           *(4*pi*st_alpha)*CF/4/nc
         enddo
      enddo

      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
C AMPLITUDES RELATES BY CROSSING:
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC

      subroutine wj_g_aqp_to_al_vl_aq(pphy,fermion_type,
     #     amp2,bmunu)
   
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      integer ferm_type(nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      integer mu,nu,i

      do i = 1,nleg
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_type(i) = fermion_type(i)
      enddo

      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,5) = -pphy(mu,1)
         pp(mu,1) = -pphy(mu,5)
      enddo

c no useful information is in ferm_type(1) 
c since it's the gluon, BEFORE the following exchange
      ferm_type(1) = -ferm_type(5)
c NOTE the MINUS sign     !!!
c     if the following two lines are missing 
      ferm_type(5)=0
c     ferm_type(5) doesn't contain
c     their correct values, but this does not affect 
c     the correct call of

       call wj_q_aqp_to_al_vl_g(pp,ferm_type,
     #     amp2,bmunu)

c     correct for color average
      amp2 = amp2 * 3d0/8d0
      do mu=0,3
         do nu=0,3
            bmunu(mu,nu)=bmunu(mu,nu)* 3d0/8d0
         enddo
      enddo
      
      end

ccccccccccccccccccccccccccccccccccccccccccccc

      subroutine wj_q_g_to_al_vl_qp(pphy,fermion_type,
     #     amp2,bmunu)
   
      implicit none
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      integer ferm_type(nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      integer mu,nu,i

      do i = 1,nleg
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_type(i) = fermion_type(i)
      enddo

      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,5) = -pphy(mu,2)
         pp(mu,2) = -pphy(mu,5)
      enddo

c no useful information is in ferm_type(2) , 
c since it's the gluon, BEFORE the following exchange
      ferm_type(2) = -ferm_type(5)
c NOTE the MINUS sign     !!!

c     if the following two lines are missing 
      ferm_type(5)=0
c     ferm_type(5)  doesn't contain
c     their correct values, but this does not affect 
c     the correct call of

       call wj_q_aqp_to_al_vl_g(pp,ferm_type,
     #     amp2,bmunu)

c     correct for color average
      amp2 = amp2 * 3d0/8d0
      do mu=0,3
         do nu=0,3
            bmunu(mu,nu)=bmunu(mu,nu)* 3d0/8d0
         enddo
      enddo
      
      end

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC


      subroutine wj_q_aqp_to_l_avl_g(pphy,fermion_type,
     #     amp2,bmunu)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_q_aqp_to_al_vl_g(pphy,ferm_type,
     #     amp2,bmunu)

      end

ccccccccccccccccccccccccccccccccccccccccccccc

       subroutine wj_g_aqp_to_l_avl_aq(pphy,fermion_type,
     #     amp2,bmunu)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_g_aqp_to_al_vl_aq(pphy,ferm_type,
     #     amp2,bmunu)

      end

ccccccccccccccccccccccccccccccccccccccccccccc

       subroutine wj_q_g_to_l_avl_qp(pphy,fermion_type,
     #     amp2,bmunu)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=5)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2,bmunu(0:3,0:3)
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_q_g_to_al_vl_qp(pphy,ferm_type,amp2,bmunu)

      end

ccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine wj_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding wj_cross section, for the
c kinematics defined in the Les Houches interface
      include 'src/Lib/wj/LesHouches.h'
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
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

      subroutine wj_finalize_lh
      implicit none
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/wj_cvecbos/idvecbos,vdecaymode
c     Set up the resonance whose mass must be preserved
c     on the Les Houches interface.
c     resonance W -> l(3) vl(4)
      call wj_add_resonance(idvecbos,3,4)
c     give wj_masses to final-state products
      call wj_lhefinitemasses
      end


