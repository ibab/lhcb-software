      subroutine wj_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/pwhg_st.h'
      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal),rflavs(nlegreal)
      real * 8 amp2
      real * 8 ps(0:3,nlegreal)
      integer j,mu
cccccccccccccccccccc
c$$$      logical debug,ini
c$$$      data ini/.true./
c$$$      save ini
c$$$      parameter(debug=.true.)
c$$$      real *8 kr_mad(0:3,nlegreal),amp2mad
c$$$      integer ileg
c$$$      real *8 tiny
c$$$      parameter (tiny=1d-3)
ccccccccccccccccccc

c     if present, the gluon is assumed to be the last particle
      call wj_real_ampsq_g_last(p,rflav,amp2)
c     if the amplitude is zero, flip last two momenta to have gluon in the 
c     last position
      if(amp2.eq.0) then
         do j=1,6
            do mu=0,3
               ps(mu,j)=p(mu,j)
            enddo
            rflavs(j)=rflav(j)
         enddo
         rflavs(5)=rflav(6)
         rflavs(6)=rflav(5)
         do mu=0,3
            ps(mu,5)=p(mu,6)
            ps(mu,6)=p(mu,5)
         enddo
         call wj_real_ampsq_g_last(ps,rflavs,amp2)
      endif

CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
c$$$c compare with madgraph
c$$$      if (debug) then
c$$$         if(ini) then 
c$$$c     setting madgraph parameters (needed for madgraph subroutines)
c$$$            call set_madgraph_parameters
c$$$            ini=.false.
c$$$         endif
c$$$c     set madgraph parameters that can change on an event-by-event basis
c$$$      call mad_setparam
c$$$
c$$$      do ileg=1,nlegreal
c$$$         do mu=0,3
c$$$            kr_mad(mu,ileg)=p(mu,ileg)
c$$$         enddo
c$$$      enddo
c$$$c     to avoid bugs in HELAS, restore exact masslessness of  incoming partons 
c$$$      kr_mad(0,1)=dabs(kr_mad(3,1))
c$$$      kr_mad(0,2)=dabs(kr_mad(3,2))
c$$$      call compreal(kr_mad,rflav,amp2mad)
c$$$
c$$$      if  (abs(amp2mad/amp2 -1d0).gt.tiny) then 
c$$$         print *,rflav,' MUST BE EQUAL =====> ', amp2mad, amp2,
c$$$     $        ' RATIO: ', amp2mad/amp2, '\n P:',kr_mad
c$$$        stop
c$$$      endif
c$$$
c$$$      if  ((abs(amp2mad-amp2)/amp2mad).gt.tiny) then 
c$$$         print *,rflav,' MUST BE 0 =====> ', abs(amp2mad-amp2)/amp2mad
c$$$         stop
c$$$      endif
c$$$      endif
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
      
c     cancel as/(2pi) associated with amp2. It will be put back by wj_real_ampsq
      amp2 = amp2/(st_alpha/(2*pi))     
      
      end



c     NB The final-state gluon, if present, must be the last particle in 
c     the list
c     Example
c     q qp~ -> e+ ve g g 
      subroutine wj_real_ampsq_g_last(p,realflav,amp2)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      real * 8 p(0:3,nlegreal)
      integer realflav(nlegreal)
      real * 8 amp2
      integer ferm_type(nlegreal)
      integer i,j,k,l
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/wj_cvecbos/idvecbos,vdecaymode
      integer idx1,idx2
      real *8 wj_CKM_sq
      external wj_CKM_sq
c     i is the flavour index of first incoming parton, j is the flavour
c     index of second incoming parton k,l are the flavours of outgoing
c     partons in the order particle,antiparticle,gluon with the
c     convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t                    
      
      i = realflav(1)
      j = realflav(2)
      k = realflav(5)
      l = realflav(6)      
     
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
      if (l.eq.0) then
         ferm_type(6) = 0
      else 
         ferm_type(6) = l/abs(l)
      endif   

c     W decay products
      ferm_type(3) = realflav(3)/abs(realflav(3))
      ferm_type(4) = -ferm_type(3)
      
      if(idvecbos.eq.24) then
         if ((k.eq.0).and.(l.eq.0)) then
c     q aqp -> W+ g g
            call wj_q_aqp_to_al_vl_g_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(i,j)
         elseif ((i.eq.0).and.(l.eq.0)) then
c     g q -> W+ qp g
            call wj_g_q_to_al_vl_qp_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(j,k)
         elseif ((j.eq.0).and.(l.eq.0)) then
c     q g -> W+ qp g
            call wj_q_g_to_al_vl_qp_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(i,k)
         elseif ((i.eq.0).and.(j.eq.0)) then
c     g g -> W+ q aqp
            call wj_g_g_to_al_vl_q_aqp(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(k,l)
         elseif(i*j*k*l.ne.0) then
c     4 quarks
            call wj_find_diff_flavours(abs(i),abs(j),abs(k),abs(l),idx1
     $           ,idx2)
            if(idx1.ne.0) then
               if(idx2.ne.0) then
c     2 equal flavours, 2 different
                  call wj_cross_q_qp_to_al_vl_q_qpp(p,realflav,idx1,idx2
     $                 ,amp2)
               else
c     3 equal flavours, 1 different
                  call wj_cross_q_qp_to_al_vl_q_q(p,realflav,idx1,amp2)
               endif
            else
               print*, 'error'
               stop
            endif
         else
            amp2 = 0d0
         endif
      elseif(idvecbos.eq.-24) then
         if ((k.eq.0).and.(l.eq.0)) then
c     q aqp -> W- g g
            call wj_q_aqp_to_l_avl_g_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(i,j)
         elseif ((i.eq.0).and.(l.eq.0)) then
c     g q -> W- qp g
            call wj_g_q_to_l_avl_qp_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(j,k)
         elseif ((j.eq.0).and.(l.eq.0)) then
c     q g -> W- qp g
            call wj_q_g_to_l_avl_qp_g(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(i,k)
         elseif ((i.eq.0).and.(j.eq.0)) then
c     g g -> W- q aqp
            call wj_g_g_to_l_avl_q_aqp(p,ferm_type,amp2)
            amp2=amp2*wj_CKM_sq(k,l)
         elseif(i*j*k*l.ne.0) then
c     4 quarks
            call wj_find_diff_flavours(abs(i),abs(j),abs(k),abs(l),idx1
     $           ,idx2)
            if(idx1.ne.0) then
               if(idx2.ne.0) then
c     2 equal flavours, 2 different
                  call wj_cross_q_qp_to_l_avl_q_qpp(p,realflav,idx1,idx2
     $                 ,amp2)
               else
c     3 equal flavours, 1 different
                  call wj_cross_q_qp_to_l_avl_q_q(p,realflav,idx1,amp2)
               endif
            else
               print*, 'error'
               stop
            endif
         else
            amp2 = 0d0
         endif
      endif
      end

      function wj_CKM_sq(i,j) 
      implicit none
      include 'src/Lib/wj/PhysPars.h'
      integer i,j
      real *8 wj_CKM_sq
      if(mod(abs(i),2).eq.0) then
         wj_CKM_sq=ph_CKM(abs(i)/2,(abs(j)+1)/2)**2
      elseif(mod(abs(i),2).eq.1) then   
         wj_CKM_sq=ph_CKM(abs(j)/2,(abs(i)+1)/2)**2
      endif
      end

c     return the position of partons with different flavours in the
c     flavour array : if there are 3 equal flavours return the position
c     of the 4th different one, otherwise return the index of the 2
c     different flavours    
      subroutine wj_find_diff_flavours(i,j,k,l,idx1,idx2)
      implicit none
      integer i,j,k,l
      integer idx1,idx2
      idx1=0
      idx2=0
      if((i.eq.j).and.(i.eq.k)) then
         idx1=6
      elseif((i.eq.j).and.(i.eq.l)) then
         idx1=5
      elseif((i.eq.k).and.(i.eq.l)) then
         idx1=2
      elseif((j.eq.k).and.(j.eq.l)) then
         idx1=1
      elseif((i.eq.j).and.(k.ne.l).and.(k.ne.i)) then
         idx1=5
         idx2=6
      elseif((k.eq.l).and.(i.ne.j).and.(k.ne.i)) then
         idx1=1
         idx2=2   
      elseif((i.eq.k).and.(j.ne.l).and.(j.ne.i)) then
         idx1=2
         idx2=6
      elseif((j.eq.l).and.(i.ne.k).and.(j.ne.i)) then
         idx1=1
         idx2=5
      elseif((k.eq.j).and.(i.ne.l).and.(j.ne.i)) then
         idx1=1
         idx2=6   
      elseif((i.eq.l).and.(j.ne.i).and.(j.ne.k)) then
         idx1=2
         idx2=5
      else
         print*,'error'
         stop
      endif
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c this subroutine compute the Born amplitude for the process
c q(p1) aqp(p2) -> W+(p3+p4) g(p5) g(p6)  con W+ -> l+(p3) vl(p4) 
c NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c amplitude
c                              / g 
c                             / 
c         q  --->------->----<
c                     |       \  
c                     |        \ g 
c                     |
c                     |           
c                     |        /  l+
c         aq ---<-----/\/\/\/\/
c                        W+   \
c                              \  vl
c
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion

      subroutine wj_q_aqp_to_al_vl_g_g(pphy,fermion_type,amp2)      
      implicit none
c     the 5 4-momentum vectors
c     p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 p1(0:3),p2(0:3)
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/PhysPars.h'
      real * 8 p34
      real * 8 wj_dotp,tmp
      complex * 16 wj_ccdotp     
      real * 8 epsg5(0:3,1:2),epsg6(0:3,1:2)
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1)           
      complex*16 jlep(0:3),jqua(0:3)
      complex*16 amp_Ta_Tb
      complex*16 amp_Tb_Ta
      complex*16 amp_3gv
      integer mu,i,pol5,pol6
      real * 8 p(0:3,nleg),pp1(0:3),pp2(0:3),pp5(0:3),pp6(0:3)
      integer ferm_type(nleg)
      complex * 16 prop34w

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

cc     fake momenta:
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

c     vl
      call wj_bra(p(0,4),ferm_type(4),psi4)
c     al
      call wj_ket(p(0,3),ferm_type(3),psi3)
      
c     leptonic current
      call wj_bra_gamma_ket(psi4,psi3,-1,jlep)
      
      amp2=0.d0
      
c     loop over gluon polarization
      do  pol5=1,2
         do  pol6=1,2         
            if (p(0,5).lt.0d0) then
               do mu=0,3
                  pp5(mu) = -p(mu,5)
               enddo         
               call wj_POLVEC(pp5(0),pol5,epsg5(0,pol5))
            else   
               call wj_POLVEC(p(0,5),pol5,epsg5(0,pol5))
            endif
            if (p(0,6).lt.0d0) then
               do mu=0,3
                  pp6(mu) = -p(mu,6)
               enddo         
               call wj_POLVEC(pp6(0),pol6,epsg6(0,pol6))
            else
               call wj_POLVEC(p(0,6),pol6,epsg6(0,pol6))
            endif
         
c     first colour order: 5 before 6             
            call wj_bra_slash_ket_g1_g2(psi2,psi1,-1,p2,p1,
     #           p(0,5),epsg5(0,pol5),p(0,6),epsg6(0,pol6),jlep,
     #           amp_Ta_Tb)
c     second colour order: 6 before 5         
            call wj_bra_slash_ket_g1_g2(psi2,psi1,-1,p2,p1,
     #           p(0,6),epsg6(0,pol6),p(0,5),epsg5(0,pol5),jlep,
     #           amp_Tb_Ta)
c     third configuration (triple gluon vertex)             
            call wj_bra_gamma_ket_3gv(psi2,psi1,-1,p2,p1,p(0,5),
     #           epsg5(0,pol5),p(0,6),epsg6(0,pol6),jqua)
            
            amp_3gv=wj_ccdotp(jlep,jqua)

            
            amp2=amp2 + CF*CF*nc* (amp_Ta_Tb* DCONJG(amp_Ta_Tb) +
     $           amp_Tb_Ta* DCONJG(amp_Tb_Ta)) - CF/2d0* ( amp_Ta_Tb
     $           * DCONJG(amp_Tb_Ta) + amp_Tb_Ta* DCONJG(amp_Ta_Tb)) +
     $           CF*CA*nc* ( amp_3gv* DCONJG(amp_3gv)) + CF*CA/2d0*nc
     $           * (amp_3gv* DCONJG(amp_Ta_Tb - amp_Tb_Ta) + (amp_Ta_Tb
     $           - amp_Tb_Ta) * DCONJG(amp_3gv))
      enddo
      enddo

      amp2 = amp2*((ph_unit_e/ph_sthw)**4)/4d0 * (4*pi*st_alpha)**2 
     $     *prop34w*DCONJG(prop34w)
      amp2=  amp2/4/nc/nc/2
      end
      
ccccccccccccccccccccccccccccccccccccccccccccc
c     this subroutine compute the born amplitude for the process
c     q(p1) g(p2) -> W+(p3+p4) qp(p5) g(p6)  con W+ -> l+(p3) vl(p4) 
c     NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c     amplitude. This amplitude is obtained by crossing the formula for 
c     wj_q_aqp_to_al_vl_g_g
      subroutine wj_q_g_to_al_vl_qp_g(pphy,fermion_type,amp2)      
      implicit none
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i

      do i = 1,6
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
      
c     no useful information is in ferm_type(2) 
c     since it's the gluon, BEFORE the following exchange
      ferm_type(2) = -ferm_type(5)
c     NOTE the MINUS sign!!!
      
c     if the following two lines are missing 
      ferm_type(5)=0
c     ferm_type(5)  doesn't contain
c     their correct values, but this does not affect 
c     the correct call of

      call wj_q_aqp_to_al_vl_g_g(pp,ferm_type,amp2)
      
c     correct for color average
      amp2 = amp2 *2* 3d0/8
      end
      
      
      
ccccccccccccccccccccccccccccccccccccccccccccc
c     this subroutine compute the born amplitude for the process
c     g(p1) q(p2) -> W+(p3+p4) qp(p5) g(p6)  con W+ -> l+(p3) vl(p4) 
c     NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c     amplitude. This amplitude is obtained by crossing the formula for 
c     wj_q_aqp_to_al_vl_g_g
      subroutine wj_g_q_to_al_vl_qp_g(pphy,fermion_type,amp2
     $     )      
      implicit none
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i

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
      
c     no useful information is in ferm_type(1) 
c     since it's the gluon, BEFORE the following exchange
      ferm_type(1) = -ferm_type(5)
c     NOTE the MINUS sign!!!
      
c     if the following two lines are missing 
      ferm_type(5)=0
c     ferm_type(5) doesn't contain
c     their correct values, but this does not affect 
c     the correct call of
      call wj_q_aqp_to_al_vl_g_g(pp,ferm_type,amp2)
c     correct for color average
      amp2 = amp2 *2* 3d0/8
      end
      
ccccccccccccccccccccccccccccccccccccccccccccc
c     this subroutine compute the born amplitude for the process
c     g(p1) g(p2) -> W+(p3+p4) q(p5) aqp(p6)  con W+ -> l+(p3) vl(p4) 
c     NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c     amplitude. This amplitude is obtained by crossing the formula for 
c     wj_q_aqp_to_al_vl_g_g
      subroutine wj_g_g_to_al_vl_q_aqp(pphy,fermion_type,amp2)      
      implicit none
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i
     
      do i = 1,6
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         ferm_type(i) = fermion_type(i)
      enddo

      do mu=0,3
c     exchange initial gluon <-> final quark
         pp(mu,5) = -pphy(mu,1)
         pp(mu,1) = -pphy(mu,5)
         pp(mu,6) = -pphy(mu,2)
         pp(mu,2) = -pphy(mu,6)         
      enddo
      
c     no useful information is in ferm_type(1) 
c     since it's the gluon, BEFORE the following exchange
      ferm_type(1) = -ferm_type(5)
      ferm_type(2) = -ferm_type(6)
c     NOTE the MINUS sign!!!

c     if the following lines are missing 
      ferm_type(5)=0
      ferm_type(6)=0
c     ferm_type(5) and ferm_type(6) don't contain
c     their correct values, but this does not affect 
c     the correct call of

      call wj_q_aqp_to_al_vl_g_g(pp,ferm_type,amp2)
c     correct for color average
      amp2 = amp2*2d0*3d0/8*3d0/8
      end
      

      
c     this subroutine compute the Born amplitude for the process
c     q(p1) qp(p2) -> W+(p3+p4) q(p5) q(p6) con W+ -> l+(p3) vl(p4) 
c     NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c     amplitude
c     !!!!!!!!!! SAME FLAVOURS !!!!!!!!!!
c     
c         q  --->----------->---- q
c                     |         
c                   g |           
c                     |        
c         qp --->----------->----- q
c                       \
c                     W+ \  / l+ 
c                         \/ 
c                          \
c                           \ vl
c 
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
      subroutine wj_q_qp_to_al_vl_q_q(pphy,fermion_type,amp2)   
      implicit none
c     the 5 4-momentum vectors
c     p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 p1(0:3),p2(0:3),p5(0:3),p6(0:3)       
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/PhysPars.h'
      real * 8 p34
      real * 8 wj_dotp
      complex * 16 wj_ccdotp
      complex *16 z1,z2
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1),
     #     psi5(2,-1:1),psi6(2,-1:1)      
      complex*16 jlep(0:3),jqua15(0:3,-1:1)
      complex*16 jqua16(0:3,-1:1)
      complex*16 jtemp(0:3)
      complex*16 amp_ljj(2,-1:1)
      integer mu,i,hel_qua
      real * 8 p(0:3,nleg),px1(0:3,nleg),px2(0:3,nleg)
      real *8 pp1(0:3),pp2(0:3),pp5(0:3),pp6(0:3)
      integer ferm_type(nleg)
      real * 8 pcurr15(0:3)
      real * 8 pcurr16(0:3)
      complex * 16 prop34w
      
      if ((fermion_type(3).ne.-1).and.(fermion_type(4).ne.1)) then
         write(*,*) 'ERROR: this subroutine deals only with W+ decay'
         stop
      endif
      
c     copy of local variables
      do i=1,6
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
         ferm_type(i) = fermion_type(i)
      enddo
      
              
c     exchance particle 1 and 6
      if (ferm_type(1).eq.-1) then
         if (ferm_type(6).eq.-1) then
            call wj_exchange_mom(p,1,6,6,px2)
         else
            write(*,*) 'Error in the type of the quark 1-6'
            stop
         endif
      else  
         call wj_exchange_mom(p,1,1,6,px2)   
      endif
      
c     exchance particle 2 and 5
      if (ferm_type(2).eq.-1) then
         if (ferm_type(5).eq.-1) then
            call wj_exchange_mom(px2,2,5,6,px2)
         else
            write(*,*) 'Error in the type of the quark 2-5'
            stop
         endif
      endif
      
c     exchance particle 1 and 5
      if (ferm_type(1).eq.-1) then
         if (ferm_type(5).eq.-1) then
            call wj_exchange_mom(p,1,5,6,px1)
         else
            write(*,*) 'Error in the type of the quark 1-5'
            stop
         endif
      else  
         call wj_exchange_mom(p,1,1,6,px1)    
      endif
      
c     exchance particle 2 and 6
      if (ferm_type(2).eq.-1) then
         if (ferm_type(6).eq.-1) then
            call wj_exchange_mom(px1,2,6,6,px1)
         else
            write(*,*) 'Error in the type of the quark 2-6'
            stop
         endif
      endif
      
      p34=wj_dotp(p(0,3),p(0,4))
c     W propagator
      prop34w=1d0/dcmplx(2d0*p34-ph_Wmass2,ph_WmWw)
      
      amp2=0d0      

c     fake momenta:
c     for bosons always outgoing
c     for fermions along fermionic current
      do mu=0,3
         p1(mu) = ferm_type(1)*px1(mu,1)
         p2(mu) = ferm_type(2)*px1(mu,2)
         p5(mu) = ferm_type(5)*px1(mu,5)
         p6(mu) = ferm_type(6)*px1(mu,6)
c     current momenta are built from physical vectors always outgoing         
         pcurr15(mu) = p5(mu)-p1(mu)
      enddo               
      
c     wj_bra and wj_ket are built with physical momenta, but a check on positivity
c     of energy is needed when one uses this function to evaluate a
c     crossing related amplitudes. NOTE that this affects only the
c     construction of spinors or polarization vectors on external legs
c     that may be crossed. All other momenta are unchanged 
c     
c     q
      if (px1(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -px1(mu,1)
         enddo         
         call wj_ket(pp1,ferm_type(1),psi1)
      else
         call wj_ket(px1(0,1),ferm_type(1),psi1)
      endif                
c     q
      if (px1(0,5).lt.0d0) then
         do mu=0,3
            pp5(mu) = -px1(mu,5)
         enddo         
         call wj_bra(pp5,ferm_type(5),psi5)
      else
         call wj_bra(px1(0,5),ferm_type(5),psi5)
      endif
c     vl
      call wj_bra(px1(0,4),ferm_type(4),psi4)
c     al
      call wj_ket(px1(0,3),ferm_type(3),psi3)
c     q
      if (px1(0,6).lt.0d0) then
         do mu=0,3
            pp6(mu) = -px1(mu,6)
         enddo         
         call wj_bra(pp6,ferm_type(6),psi6)
      else
         call wj_bra(px1(0,6),ferm_type(6),psi6)
      endif
c     qp
      if (px1(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -px1(mu,2)
         enddo         
         call wj_ket(pp2,ferm_type(2),psi2)
      else
         call wj_ket(px1(0,2),ferm_type(2),psi2)
      endif
c     currents
      call wj_bra_gamma_ket(psi4,psi3,-1,jlep)
      do i=-1,1,2
         call wj_bra_gamma_ket(psi5,psi1,i,jqua15(0,i))         
      enddo
      
      do hel_qua=-1,1,2       
         call wj_bra_gamma_ket_curr(psi6,psi2,-1,p6,p2,pcurr15,
     #        jqua15(0,hel_qua),jtemp)
         
         amp_ljj(1,hel_qua) = wj_ccdotp(jlep,jtemp)
         
      enddo

c     fake momenta:
c     for bosons always outgoing
c     for fermions along fermionic current    
c     momenti fittizi 
c     (fermioni-> linea fermionica, bosoni-> outgoing)  
c     quindi +1 * il gluone outgoing
      do mu=0,3
         p1(mu) = ferm_type(1)*px2(mu,1)
         p2(mu) = ferm_type(2)*px2(mu,2)
         p5(mu) = ferm_type(5)*px2(mu,5)
         p6(mu) = ferm_type(6)*px2(mu,6)
c     current momenta are built from physical vectors always outgoing         
         pcurr16(mu) = p6(mu)-p1(mu)
       enddo               
c     wj_bra and wj_ket are built with physical momenta, but a check on positivity
c     of energy is needed when one uses this function to evaluate a
c     crossing related amplitudes. NOTE that this affects only the
c     construction of spinors or polarization vectors on external legs
c     that may be crossed. All other momenta are unchanged 
c
c     q
      if (px2(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -px2(mu,1)
         enddo         
         call wj_ket(pp1,ferm_type(1),psi1)
      else
         call wj_ket(px2(0,1),ferm_type(1),psi1)
      endif               
c     q        
      if (px2(0,5).lt.0d0) then
         do mu=0,3
            pp5(mu) = -px2(mu,5)
         enddo         
         call wj_bra(pp5,ferm_type(5),psi5)
      else
         call wj_bra(px2(0,5),ferm_type(5),psi5)
      endif
c     vl
      call wj_bra(px2(0,4),ferm_type(4),psi4)
c     al
      call wj_ket(px2(0,3),ferm_type(3),psi3)
c     q
      if (px2(0,6).lt.0d0) then
         do mu=0,3
            pp6(mu) = -px2(mu,6)
         enddo         
         call wj_bra(pp6,ferm_type(6),psi6)
      else
         call wj_bra(px2(0,6),ferm_type(6),psi6)
      endif
c     qp 
      if (px2(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -px2(mu,2)
         enddo         
         call wj_ket(pp2,ferm_type(2),psi2)
      else
         call wj_ket(px2(0,2),ferm_type(2),psi2)
      endif
c     correnti
      call wj_bra_gamma_ket(psi4,psi3,-1,jlep)
      do i=-1,1,2
         call wj_bra_gamma_ket(psi6,psi1,i,jqua16(0,i))
      enddo
         
      do hel_qua=-1,1,2      
         call wj_bra_gamma_ket_curr(psi5,psi2,-1,p5,p2,pcurr16,
     #        jqua16(0,hel_qua),jtemp)
         
         amp_ljj(2,hel_qua) = wj_ccdotp(jlep,jtemp)
      enddo
         
c     final coherent sum   
c     minus sign for fermion statistic
      do hel_qua=-1,1,2      
         z1=amp_ljj(1,hel_qua)
         z2=-amp_ljj(2,hel_qua)
         amp2 = amp2 + 
     #        CF*nc/2*(z1*DCONJG(z1)+z2*DCONJG(z2)) 
         
         if (hel_qua.eq.-1) then
            amp2 = amp2
     #           -CF/2*(z1*DCONJG(z2)+z2*DCONJG(z1))               
         endif               
      enddo
c     coupling costants and averaging factors 
      amp2 = amp2*((ph_unit_e/ph_sthw)**4)/4d0 * (4*pi*st_alpha)**2 
     $     *prop34w*DCONJG(prop34w)
      amp2=  amp2/nc/nc/4/2
      end

      
ccccccccccc

c     this subroutine compute the Born amplitude for the process
c     q(p1) qp(p2) -> W+(p3+p4) q(p5) q(p6) con W+ -> l+(p3) vl(p4) 
c     NUMERICALLY, with the wj_bra/wj_ket formalism, not by squaring the analytic 
c     amplitude
c     !!!!!!!!!! DIFFERENT FLAVOURS !!!!!!!!!!
c     
c         q  --->----------->----- q
c                     |         
c                   g |           
c                     |        
c         qp --->----------->----- qpp
c                       \
c                     W+ \  /  l+ 
c                         \/  
c                          \ 
c                           \ vl
c 
c     ferm_type = 1 fermion
c     ferm_type = -1 antifermion
      subroutine wj_q_qp_to_al_vl_q_qpp(pphy,fermion_type,amp2)   
      implicit none
c     the 5 4-momentum vectors
c     p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      real * 8 p1(0:3),p2(0:3),p5(0:3),p6(0:3)       
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/pwhg_math.h'
      include 'src/Lib/wj/PhysPars.h'
      real * 8 p34
      real * 8 wj_dotp
      complex * 16 wj_ccdotp
      complex *16 z1,z2
      complex*16 psi1(2,-1:1),psi2(2,-1:1),psi3(2,-1:1),psi4(2,-1:1),
     #     psi5(2,-1:1),psi6(2,-1:1)      
      complex*16 jlep(0:3),jqua15(0:3,-1:1)
      complex*16 jtemp(0:3)
      complex*16 amp_ljj(-1:1)
      integer mu,i,hel_qua
      real * 8 p(0:3,nleg),px1(0:3,nleg),px2(0:3,nleg)
      real *8 pp1(0:3),pp2(0:3),pp5(0:3),pp6(0:3)
      integer ferm_type(nleg)
      real * 8 pcurr15(0:3)
      real * 8 pcurr16(0:3)
      complex * 16 prop34w,wj_cdotp
      real *8 ptemp(0:3)
      
      if ((fermion_type(3).ne.-1).and.(fermion_type(4).ne.1)) then
         write(*,*) 'ERROR: this subroutine deals only with W+ decay'
         stop
      endif
      
c     copy of local variables
      do i=1,6
         do mu=0,3
            p(mu,i) = pphy(mu,i)
         enddo
         ferm_type(i) = fermion_type(i)
      enddo
      
              
c     exchance particle 1 and 5
      if (ferm_type(1).eq.-1) then
         if (ferm_type(5).eq.-1) then
            call wj_exchange_mom(p,1,5,6,px1)
         else
            write(*,*) 'Error in the type of the quark 1-5'
            stop
         endif
      else  
         call wj_exchange_mom(p,1,1,6,px1)    
      endif
      
c     exchance particle 2 and 6
      if (ferm_type(2).eq.-1) then
         if (ferm_type(6).eq.-1) then
            call wj_exchange_mom(px1,2,6,6,px1)
         else
            write(*,*) 'Error in the type of the quark 2-6'
            stop
         endif
      endif
      
      p34=wj_dotp(p(0,3),p(0,4))
c     W propagator
      prop34w=1d0/dcmplx(2d0*p34-ph_Wmass2,ph_WmWw)
      
      amp2=0d0      

c     fake momenta:
c     for bosons always outgoing
c     for fermions along fermionic current
      do mu=0,3
         p1(mu) = ferm_type(1)*px1(mu,1)
         p2(mu) = ferm_type(2)*px1(mu,2)
         p5(mu) = ferm_type(5)*px1(mu,5)
         p6(mu) = ferm_type(6)*px1(mu,6)
c     current momenta are built from physical vectors always outgoing         
         pcurr15(mu) = p5(mu)-p1(mu)
      enddo               
      
c     wj_bra and wj_ket are built with physical momenta, but a check on positivity
c     of energy is needed when one uses this function to evaluate a
c     crossing related amplitudes. NOTE that this affects only the
c     construction of spinors or polarization vectors on external legs
c     that may be crossed. All other momenta are unchanged 
c     
c     q
      if (px1(0,1).lt.0d0) then
         do mu=0,3
            pp1(mu) = -px1(mu,1)
         enddo         
         call wj_ket(pp1,ferm_type(1),psi1)
      else
         call wj_ket(px1(0,1),ferm_type(1),psi1)
      endif                
c     q
      if (px1(0,5).lt.0d0) then
         do mu=0,3
            pp5(mu) = -px1(mu,5)
         enddo         
         call wj_bra(pp5,ferm_type(5),psi5)
      else
         call wj_bra(px1(0,5),ferm_type(5),psi5)
      endif
c     vl
      call wj_bra(px1(0,4),ferm_type(4),psi4)
c     al
      call wj_ket(px1(0,3),ferm_type(3),psi3)
c     q
      if (px1(0,6).lt.0d0) then
         do mu=0,3
            pp6(mu) = -px1(mu,6)
         enddo         
         call wj_bra(pp6,ferm_type(6),psi6)
      else
         call wj_bra(px1(0,6),ferm_type(6),psi6)
      endif
c     qp
      if (px1(0,2).lt.0d0) then
         do mu=0,3
            pp2(mu) = -px1(mu,2)
         enddo         
         call wj_ket(pp2,ferm_type(2),psi2)
      else
         call wj_ket(px1(0,2),ferm_type(2),psi2)
      endif
c     currents
      call wj_bra_gamma_ket(psi4,psi3,-1,jlep)
      do i=-1,1,2
         call wj_bra_gamma_ket(psi5,psi1,i,jqua15(0,i))         
      enddo
      
      do hel_qua=-1,1,2       
         call wj_bra_gamma_ket_curr(psi6,psi2,-1,p6,p2,pcurr15,
     #        jqua15(0,hel_qua),jtemp)
         
         amp_ljj(hel_qua) = wj_ccdotp(jlep,jtemp)
      enddo

     

         
c     final coherent sum   
c     minus sign for fermion statistic
      do hel_qua=-1,1,2      
         z1= amp_ljj(hel_qua)
         amp2 = amp2 + 
     #        CF*nc/2*(z1*DCONJG(z1)) 
      enddo
c     coupling costants and averaging factors 
      amp2 = amp2*((ph_unit_e/ph_sthw)**4)/4d0 * (4*pi*st_alpha)**2 
     $     *prop34w*DCONJG(prop34w)
      amp2=  amp2/nc/nc/4
      end


c     idx is the position of qp in the flavour string
      subroutine wj_cross_q_qp_to_al_vl_q_q(pphy,rflav,idx,amp2)
      implicit none
      integer nleg
      parameter (nleg=6)
      integer rflav(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg),rtemp
      integer flav(nleg),itemp
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i,idx
      real *8 wj_CKM_sq
      external wj_CKM_sq

      do i = 1,6
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         flav(i) = rflav(i)
         ferm_type(i)=isign(1,flav(i))
      enddo


c     first permute to get idx in position 2


      if(idx.eq.2) then
c     already OK
         continue
      elseif(idx.eq.1) then
c     crossing not needed, just relabelling
         call wj_cross(pp,ferm_type,1,5,6,pp,ferm_type)
         call wj_cross(pp,ferm_type,2,5,6,pp,ferm_type)
c         print *,((pp(mu,i),mu=0,3),i=1,6)
c         call wj_exchange_mom(pp,1,2,6,pp)
c         call wj_swap(ferm_type,1,2,6,ferm_type)
      elseif(idx.eq.5) then
c     'real crossing': wj_cross 2<->5, changing signs.
         call wj_cross(pp,ferm_type,2,5,6,pp,ferm_type)
      elseif(idx.eq.6) then
c     'real crossing': wj_cross 2<->6, changing signs.
         call wj_cross(pp,ferm_type,2,5,6,pp,ferm_type) 
      endif

c Now the different flavour is in the 2nd position 
C wj_cross again to obtain either only quarks,
c either only anti-quarks
c      print *,ferm_type
c      print *,rflav
      if(ferm_type(1).ne.ferm_type(2)) then
         if (ferm_type(5).eq.ferm_type(1)) then
c           wj_cross 1-5
            call wj_cross(pp,ferm_type,1,5,6,pp,ferm_type)
         elseif (ferm_type(6).eq.ferm_type(1))  then
c           wj_cross 1-6
            call wj_cross(pp,ferm_type,1,6,6,pp,ferm_type)
         else
            print *,"Error on ferm_type(1)"
            stop
         endif
      endif

c     call to fundamental routine
      call wj_q_qp_to_al_vl_q_q(pp,ferm_type,amp2)

c     add wj_CKM
      i=1
      if (idx.eq.1) i=2
      amp2=amp2*wj_CKM_sq(flav(idx),flav(i))
      
c     corrections for average and identical particles
      if (rflav(5).ne.rflav(6)) then
         amp2=amp2*2
      endif
      end


c     idx1 and idx2 is the position of qp and qpp in the
c     flavour string
      subroutine wj_cross_q_qp_to_al_vl_q_qpp(pphy,rflav,idx1,idx2,amp2)
      implicit none
      integer nleg
      parameter (nleg=6)
      integer rflav(nleg)
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg),rtemp
      integer flav(nleg),itemp
      integer ferm_type(nleg)
      real * 8 amp2
      integer mu,i,idx1,idx2
      real *8 wj_CKM_sq
      external wj_CKM_sq

      do i = 1,6
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         flav(i) = rflav(i)
         ferm_type(i)=isign(1,flav(i))
      enddo

c     wj_swap momenta and ferm_type in order to have
c     qp as 2nd and qpp as 6th before calling the
c     fundamental routine
      if((idx1.eq.2).and.(idx2.eq.6)) then
c     already OK
         continue
      elseif((idx1.eq.2).and.(idx2.eq.5)) then
c     crossing not needed, just relabelling
         call wj_exchange_mom(pp,5,6,6,pp)
         call wj_swap(ferm_type,5,6,6,ferm_type)
      elseif((idx1.eq.1).and.(idx2.eq.6)) then
c     crossing not needed, just relabelling
         call wj_exchange_mom(pp,1,2,6,pp)
         call wj_swap(ferm_type,1,2,6,ferm_type)
      elseif((idx1.eq.1).and.(idx2.eq.5))then
c     crossing not needed, just relabelling
         call wj_exchange_mom(pp,1,2,6,pp)
         call wj_swap(ferm_type,1,2,6,ferm_type)    
         call wj_exchange_mom(pp,5,6,6,pp)
         call wj_swap(ferm_type,5,6,6,ferm_type)
      elseif((idx1.eq.1).and.(idx2.eq.2)) then
c     'real crossing': wj_cross 1<->6, changing signs.
c     The handling of quarks and antiquarks is done internally
c     in the fundamental routine
         do mu=0,3
            rtemp=pp(mu,1)
            pp(mu,1)=-pp(mu,6)
            pp(mu,6)=-rtemp
         enddo
         itemp=ferm_type(1)
         ferm_type(1)=-ferm_type(6)
         ferm_type(6)=-itemp
      elseif((idx1.eq.5).and.(idx2.eq.6)) then
c     'real crossing': wj_cross 2<->5, changing signs.
c     The handling of quarks and antiquarks is done internally
c     in the fundamental routine
         do mu=0,3
            rtemp=pp(mu,2)
            pp(mu,2)=-pp(mu,5)
            pp(mu,5)=-rtemp
         enddo
         itemp=ferm_type(2)
         ferm_type(2)=-ferm_type(5)
         ferm_type(5)=-itemp
      else
         write(*,*) 'not defined action in wj_cross_q_qp_to_al_vl_q_qpp'
      endif
      
c     call to fundamental routine
      call wj_q_qp_to_al_vl_q_qpp(pp,ferm_type,amp2)

c     add wj_CKM
      amp2=amp2*wj_CKM_sq(flav(idx1),flav(idx2))
      
      end

      subroutine wj_cross(p,array,i,j,dim,pnew,arraynew)
      implicit none
      integer array(*),arraynew(*)
      real * 8 p(0:3,*),pnew(0:3,*)
      integer i,j,dim,k,tmp,mu
      real * 8 ptmp(0:3)
      if (((i*j).le.4).or.((i*j).ge.25)) then
         write (*,*) 'error: crossing only between initial-final'
         call exit(1)
      endif
      do k=1,dim
         arraynew(k)=array(k)
         do mu=0,3
            pnew(mu,k) = p(mu,k)
         enddo
      enddo
      tmp=array(i)
      arraynew(i)=-array(j)
      arraynew(j)=-tmp
      do mu=0,3
         ptmp(mu) = p(mu,j)
         pnew(mu,j) = -p(mu,i)
         pnew(mu,i) = -ptmp(mu)
      enddo
      end

      subroutine wj_swap(array,i,j,dim,arraynew)
      implicit none
      integer array(*),arraynew(*)
      integer i,j,dim,k,tmp
      do k=1,dim
         arraynew(k)=array(k)
      enddo
      tmp=array(i)
      arraynew(i)=array(j)
      arraynew(j)=tmp
      end

ccccccccccccccccccccccccccccccccccccccccccc
      subroutine wj_q_aqp_to_l_avl_g_g(pphy,fermion_type,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_q_aqp_to_al_vl_g_g(pphy,ferm_type,amp2)

      end

ccccccccccccccccccccccccccccccccccccccccccccc

      subroutine wj_q_g_to_l_avl_qp_g(pphy,fermion_type,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg     
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_q_g_to_al_vl_qp_g(pphy,ferm_type,amp2)

      end

cccccccccccccccccccccccccccccccccccccc

      subroutine wj_g_q_to_l_avl_qp_g(pphy,fermion_type,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_g_q_to_al_vl_qp_g(pphy,ferm_type,amp2)

      end
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine wj_g_g_to_l_avl_q_aqp(pphy,fermion_type,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer fermion_type(nleg),i
      real * 8 pphy(0:3,nleg)
      real * 8 amp2
      integer ferm_type(nleg)

       if ((fermion_type(3).ne.1).and.(fermion_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         ferm_type(i) = -fermion_type(i)
      enddo
            
      
      call wj_g_g_to_al_vl_q_aqp(pphy,ferm_type, amp2)

      end
cccccccccccccccccccccccccccccccccccccccccccccccccc


      subroutine wj_cross_q_qp_to_l_avl_q_qpp(pphy,rflav,idx1,idx2,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer rflav(nleg)
      integer i,mu,idx1,idx2
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      real * 8 amp2
      integer flav(nleg)
      integer ferm_type(nleg)


      do i = 1,6
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         flav(i) = rflav(i)
         ferm_type(i)=isign(1,flav(i))
      enddo
      
       if ((ferm_type(3).ne.1).and.(ferm_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
        flav(i) = -rflav(i)
      enddo
            
      
      call wj_cross_q_qp_to_al_vl_q_qpp(pp,flav,idx1,idx2,amp2)

      end
ccccccccccccccccccccccccccccccccccccccccccccccccc

      subroutine wj_cross_q_qp_to_l_avl_q_q(pphy,rflav,idx,amp2)
   
      implicit none
c the 5 4-momentum vectors
c p(i,1) is the i-th component of vector p1...   
      integer nleg
      parameter (nleg=6)
      integer rflav(nleg)
      integer i,mu,idx
      real * 8 pphy(0:3,nleg)
      real * 8 pp(0:3,nleg)
      real * 8 amp2
      integer ferm_type(nleg)
      integer flav(nleg)
      
      do i = 1,6
         do mu=0,3
            pp(mu,i) = pphy(mu,i)
         enddo
         flav(i) = rflav(i)
         ferm_type(i)=isign(1,flav(i))
      enddo
    
       if ((ferm_type(3).ne.1).and.(ferm_type(4).ne.-1)) then
         write(*,*) 'ERROR: this subroutine deals only with W- decay'
         stop
      endif

      do i=1,nleg
         flav(i) = -rflav(i)
      enddo
            

      call wj_cross_q_qp_to_al_vl_q_q(pphy,flav,idx,amp2)

      end
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
