c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
c     Use MCFM subroutines
      subroutine wj_setvirtual(p,vflav,virtual)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/PhysPars.h'
c     MCFM include
      include 'src/Lib/wj/scale.h'
      include 'src/Lib/wj/qcdcouple.h'
      include 'src/Lib/wj/constants.h' 
      real * 8 p(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual
      real * 8 pmcfm(mxpart,1:4),pold(0:3,nlegborn)
      real * 8 virt(-nf:nf, -nf:nf)
      save virt,pold
      logical ini
      save ini
      data ini/.true./
      logical equalp
      integer i,j,k,mu


      if (ini) then
         call wj_virtual_initialize_MCFM
         do mu=0,3
            do j=1,nlegborn
               pold(mu,j)=0
            enddo
         enddo
         ini=.false.
      endif
      wj_scale = sqrt(st_muren2)
      musq=st_muren2            ! renormalization wj_scale squared
      as = st_alpha

c     MCFM fills an array with all processes.
      equalp=.true.
      do mu=0,3
         do j=1,nlegborn
            equalp=equalp.and.p(mu,j).eq.pold(mu,j)
            pold(mu,j)=p(mu,j)
         enddo
      enddo
      if(.not.equalp) then
         call wj_mom_to_MCFM(p,pmcfm)
         call wj_qqb_w1jet_v(pmcfm,virt)
      endif

c       print *,vflav,' ==> ',virt(vflav(1),vflav(2))

c Add wj_CKM matrix
c     i is the flavour index of first incoming parton
c     j is the flavour index of second incoming parton
c     k is the flavour of outgoing parton in the order particle,antiparticle,gluon
c     with the convention:
c     
c      -6  -5  -4  -3  -2  -1  0  1  2  3  4  5  6                    
c      t~  b~  c~  s~  u~  d~  g  d  u  s  c  b  t 
      i = vflav(1)
      j = vflav(2)
      k = vflav(5)
      if (i.eq.0) i=abs(k)
      if (j.eq.0) j=abs(k)
      if(mod(abs(i),2).eq.0) then
         virtual=virt(vflav(1),vflav(2))*ph_CKM(abs(i)/2,(abs(j)+1)/2)
     $        **2
      elseif(mod(abs(i),2).eq.1) then   
         virtual=virt(vflav(1),vflav(2))*ph_CKM(abs(j)/2,(abs(i)+1)/2)
     $        **2
      endif

     
c     Now select only the one needed and divide result by as/(2pi)
      virtual=virtual/(st_alpha/(2*pi))
      end




      subroutine wj_virtual_initialize_MCFM
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/ewcharge.h'
      include 'src/Lib/wj/ewcouple.h'
      include 'src/Lib/wj/masses.h'
      include 'src/Lib/wj/scale.h'
      include 'src/Lib/wj/qcdcouple.h'
c      include 'src/Lib/wj/zcouple.f'
      include 'src/Lib/wj/epinv.h'
      include 'src/Lib/wj/epinv2.h'
      include 'src/Lib/wj/nflav.h'
      include 'src/Lib/wj/PhysPars.h'
c      block data wsalam1
      data Q(-5)/+0.333333333333333d0/
      data Q(-4)/-0.666666666666667d0/
      data Q(-3)/+0.333333333333333d0/
      data Q(-2)/-0.666666666666667d0/
      data Q(-1)/+0.333333333333333d0/
      data Q(0)/+0d0/
      data Q(+1)/-0.333333333333333d0/
      data Q(+2)/+0.666666666666667d0/
      data Q(+3)/-0.333333333333333d0/
      data Q(+4)/+0.666666666666667d0/
      data Q(+5)/-0.333333333333333d0/
      data tau/1d0,-1d0,1d0,-1d0,1d0,0d0,-1d0,1d0,-1d0,1d0,-1d0/
c--- Number of active flavours in the initial state
      data wj_nflav/5/     
      real * 8 aemmz
      logical gmuscheme
c$$$      double precision Vud,Vus,Vub,Vcd,Vcs,Vcb,rtxw
c$$$c----
c$$$      common/wj_cabib/Vud,Vus,Vub,
c$$$     &             Vcd,Vcs,Vcb
      integer idvecbos,vdecaymode
      common/wj_cvecbos/idvecbos,vdecaymode

      wj_epinv = 0d0
      wj_epinv2 = 0d0
      wj_zmass = ph_Zmass          ! Z mass
      zwidth = ph_Zwidth        ! Z width

      wj_wmass = ph_Wmass          ! W mass
      wwidth = ph_Wwidth        ! W width
      
      gmuscheme = .false.

      if (gmuscheme) then
         Gf = 1.16639d-05
         wj_wmass  = 80.419d0
c--   derived
         xw  = One-(wj_wmass/wj_zmass)**2     ! sin^2(theta_W)
         aemmz  = Rt2*Gf*wj_wmass**2*xw/pi ! alpha_em
      else
         xw = ph_sthw2          ! sin^2(theta_W)
         aemmz = ph_alphaem     ! alpha_em
      endif      

c--- Now set up the other derived parameters
      gwsq=fourpi*aemmz/xw
      esq=gwsq*xw


c$$$c wj_CKM matrix elements
c$$$      Vud=ph_CKM(1,1)
c$$$      Vus=ph_CKM(1,2)
c$$$      Vub=ph_CKM(1,3)
c$$$      Vcd=ph_CKM(2,1)
c$$$      Vcs=ph_CKM(2,2)
c$$$      Vcb=ph_CKM(2,3)
c$$$
c$$$      call ckmfill(sign(1,idvecbos))

cccc      call couplz(xw)
ccc      q1=-1d0
ccc      wj_L1=le
ccc      r1=re
c     renormalization wj_scale squared
c      musq=st_muren2
c     alpha_s
c      as = 0.118d0

      end


      subroutine wj_mom_to_MCFM(cmpborn,pmcfm)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
c      include 'src/Lib/wj/pwhg_flst.h'
c      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/constants.h'
      real * 8 cmpborn(0:3,5)
      real * 8 pmcfm(mxpart,1:4)
      integer i,mu      
c--------
      integer idvecbos,vdecaymode
      common/wj_cvecbos/idvecbos,vdecaymode
      do i=1,2
         pmcfm(i,4)=-cmpborn(0,i)
         do mu=1,3
            pmcfm(i,mu)=-cmpborn(mu,i)
         enddo
      enddo
      if(idvecbos.eq.24) then
c     THIRD  momentum must be the neutrino in MCFM for W+
         pmcfm(3,4)=cmpborn(0,4)
         do mu=1,3
            pmcfm(3,mu)=cmpborn(mu,4)
         enddo
c     FOURTH momentum must be the antilepton in MCFM for W+
         pmcfm(4,4)=cmpborn(0,3)
         do mu=1,3
            pmcfm(4,mu)=cmpborn(mu,3)
         enddo

      elseif(idvecbos.eq.-24) then
c     THIRD  momentum must be the neutrino in MCFM for W-
         pmcfm(3,4)=cmpborn(0,3)
         do mu=1,3
            pmcfm(3,mu)=cmpborn(mu,3)
         enddo
c     FOURTH momentum must be the antilepton in MCFM for W-
         pmcfm(4,4)=cmpborn(0,4)
         do mu=1,3
            pmcfm(4,mu)=cmpborn(mu,4)
         enddo
      else
         print *,"ERROR: idvecbos"
         call exit(1)
      endif
      pmcfm(5,4)=cmpborn(0,5)
      do mu=1,3
         pmcfm(5,mu)=cmpborn(mu,5)
      enddo
      
      end



      subroutine wj_qqb_w1jet_v(p,msq)
      implicit none
c----Matrix element for W + jet production
c----in order alpha_s^2
C----averaged over initial colours and spins
c     q(-p1)+qbar(-p2)-->W^+(nu(p3)+e^+(p4))+g(p5)
c---
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/masses.h'
      include 'src/Lib/wj/qcdcouple.h'
      include 'src/Lib/wj/ewcouple.h'
      include 'src/Lib/wj/sprods_com.h'
      include 'src/Lib/wj/zprods_com.h'
      include 'src/Lib/wj/epinv.h'
      include 'src/Lib/wj/scheme.h'
      include 'src/Lib/wj/nflav.h'
      include 'src/Lib/wj/ckm.h'
      integer j,k,iqqbg(5),iqbqg(5),iqgq(5),igqq(5),
     . igqbqb(5),iqbgqb(5)
      double precision msq(-nf:nf,-nf:nf),msq0(-nf:nf,-nf:nf),
     . p(mxpart,4),fac,sw,prop,wj_virt5,subuv,
     . qqbWg,qbqWg,qgWq,gqWq,qbgWqb,gqbWqb

      data iqqbg/1,2,3,4,5/
      data iqgq/1,5,3,4,2/
      data igqq/2,5,3,4,1/

      data iqbqg/2,1,3,4,5/
      data iqbgqb/5,1,3,4,2/
      data igqbqb/5,2,3,4,1/

c--set msq=0 to initialize
      do j=-nf,nf
      do k=-nf,nf
      msq(j,k)=0d0
      enddo
      enddo
      wj_scheme='dred'

c-- if Gflag=.false. then only the endpoint contributions from the
c-- 4-quark diagrams are included, ie. no pole subtraction for this
c-- piece. Therefore return 0.
c      if (Gflag .eqv. .false.) return

c--calculate spinor and dot-products (using BDK type notation)
      call wj_spinoru(5,p,za,zb)

c---protect from wj_soft and collinear singularities
c      if ((abs(s(1,5)) .lt. cutoff).or.(abs(s(2,5)) .lt. cutoff)) return

c--- calculate lowest order
      call wj_qqb_w_g(p,msq0)
      
c$$$c--- UV counterterm contains the finite renormalization to arrive
c$$$c--- at MS bar wj_scheme.      
c$$$      subuv=ason2pi*xn*(wj_epinv*(11d0-2d0*dble(wj_nflav)/xn)-1d0)/6d0
c$$$      subuv=as/twopi*xn*(wj_epinv*(11d0-2d0*dble(wj_nflav)/xn)-1d0)/6d0

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     Use subuv to include finite renormalization to arrive
c     at MS bar wj_scheme, AND to convert finite part from DR to
c     CDR. 
c     subuv*born is SUBTRACTED from the finite part
c     1st term: as_dr -> as_msbar
c     2nd term: DR -> CDR
      subuv=-as/twopi*(
     $     + xn/6.
     $     - 2*cf/2. - ca/6.)
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      

c--- calculate propagator
      sw=s(3,4)
      prop=sw**2/((sw-wj_wmass**2)**2+(wj_wmass*wwidth)**2)

c!!!! fac=2d0*cf*xnsq*gwsq**2*gsq*prop
      fac=2d0*cf*xnsq*gwsq**2*fourpi*as*prop      
      
      qqbWg=aveqq*fac*wj_virt5(iqqbg,za,zb)
      qbqWg=aveqq*fac*wj_virt5(iqbqg,za,zb)
      gqWq=aveqg*fac*wj_virt5(igqq,za,zb)
      qgWq=aveqg*fac*wj_virt5(iqgq,za,zb)
      gqbWqb=aveqg*fac*wj_virt5(igqbqb,za,zb)
      qbgWqb=aveqg*fac*wj_virt5(iqbgqb,za,zb)
      
      do j=-nf,nf
      do k=-nf,nf

c$$$      if     ((j .gt. 0) .and. (k .lt. 0)) then
c$$$          msq(j,k)=Vsq(j,k)*qqbWg-subuv*msq0(j,k)
c$$$      elseif ((j .lt. 0) .and. (k .gt. 0)) then
c$$$          msq(j,k)=Vsq(j,k)*qbqWg-subuv*msq0(j,k)
c$$$      elseif ((j .gt. 0) .and. (k .eq. 0)) then
c$$$          msq(j,k)=
c$$$     &   (Vsq(j,-1)+Vsq(j,-2)+Vsq(j,-3)+Vsq(j,-4)+Vsq(j,-5))*qgWq
c$$$     &     -subuv*msq0(j,k)
c$$$      elseif ((j .lt. 0) .and. (k .eq. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(j,+1)+Vsq(j,+2)+Vsq(j,+3)+Vsq(j,+4)+Vsq(j,+5))*qbgWqb
c$$$     &     -subuv*msq0(j,k)
c$$$      elseif ((j .eq. 0) .and. (k .gt. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(-1,k)+Vsq(-2,k)+Vsq(-3,k)+Vsq(-4,k)+Vsq(-5,k))*gqWq
c$$$     &     -subuv*msq0(j,k)
c$$$      elseif ((j .eq. 0) .and. (k .lt. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(+1,k)+Vsq(+2,k)+Vsq(+3,k)+Vsq(+4,k)+Vsq(+5,k))*gqbWqb
c$$$     &     -subuv*msq0(j,k)
c$$$      endif

c Removed wj_CKM entries!
      if     ((j .gt. 0) .and. (k .lt. 0)) then
          msq(j,k)=qqbWg-subuv*msq0(j,k)
      elseif ((j .lt. 0) .and. (k .gt. 0)) then
          msq(j,k)=qbqWg-subuv*msq0(j,k)
      elseif ((j .gt. 0) .and. (k .eq. 0)) then
          msq(j,k)=
     &   qgWq
     &     -subuv*msq0(j,k)
      elseif ((j .lt. 0) .and. (k .eq. 0)) then
          msq(j,k)=
     &    qbgWqb
     &     -subuv*msq0(j,k)
      elseif ((j .eq. 0) .and. (k .gt. 0)) then
          msq(j,k)=
     &    gqWq
     &     -subuv*msq0(j,k)
      elseif ((j .eq. 0) .and. (k .lt. 0)) then
          msq(j,k)=
     &    gqbWqb
     &     -subuv*msq0(j,k)
      endif

      enddo
      enddo

      return
      end


 
      subroutine wj_qqb_w_g(p,msq)
      implicit none
c----Matrix element for W production
C----averaged over initial colours and spins
C for nwz=+1
c     u(-p1)+dbar(-p2)--> W^+(n(p3)+e^+(p4))   + g(p5)
C For nwz=-1
c     d(-p1)+ubar(-p2)--> W^-(e^-(p3)+nbar(p4))+ g(p5) 
c---
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/ckm.h'
      include 'src/Lib/wj/ewcouple.h'
      include 'src/Lib/wj/qcdcouple.h'
      include 'src/Lib/wj/sprods_com.h'
      integer j,k
      double precision msq(-nf:nf,-nf:nf),p(mxpart,4),fac
      double precision qqbWg,qbqWg,qgWq,qbgWqb,gqbWqb,gqWq,wj_w1jet
     

      do j=-nf,nf
      do k=-nf,nf
      msq(j,k)=0d0
      enddo
      enddo

      call wj_dotem(5,p,s)
c---calculate the propagator
c      fac=gwsq**2*gsq*V     
      fac=gwsq**2*fourpi*as*V


      qqbWg= +aveqq*fac*wj_w1jet(1,2,3,4,5)
      gqbWqb=-aveqg*fac*wj_w1jet(5,2,3,4,1)
      qgWq=  -aveqg*fac*wj_w1jet(1,5,3,4,2)
      
      qbqWg= +aveqq*fac*wj_w1jet(2,1,3,4,5)
      qbgWqb=-aveqg*fac*wj_w1jet(5,1,3,4,2)
      gqWq=  -aveqg*fac*wj_w1jet(2,5,3,4,1)

      do j=-nf,nf
      do k=-nf,nf
c$$$      if     ((j .gt. 0) .and. (k .lt. 0)) then
c$$$          msq(j,k)=Vsq(j,k)*qqbWg
c$$$      elseif ((j .lt. 0) .and. (k .gt. 0)) then
c$$$          msq(j,k)=Vsq(j,k)*qbqWg
c$$$      elseif ((j .gt. 0) .and. (k .eq. 0)) then
c$$$          msq(j,k)=
c$$$     &   (Vsq(j,-1)+Vsq(j,-2)+Vsq(j,-3)+Vsq(j,-4)+Vsq(j,-5))*qgWq
c$$$      elseif ((j .lt. 0) .and. (k .eq. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(j,+1)+Vsq(j,+2)+Vsq(j,+3)+Vsq(j,+4)+Vsq(j,+5))*qbgWqb
c$$$      elseif ((j .eq. 0) .and. (k .gt. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(-1,k)+Vsq(-2,k)+Vsq(-3,k)+Vsq(-4,k)+Vsq(-5,k))*gqWq
c$$$      elseif ((j .eq. 0) .and. (k .lt. 0)) then
c$$$          msq(j,k)=
c$$$     &    (Vsq(+1,k)+Vsq(+2,k)+Vsq(+3,k)+Vsq(+4,k)+Vsq(+5,k))*gqbWqb
c$$$      endif

c Removed wj_CKM entries !
      if     ((j .gt. 0) .and. (k .lt. 0)) then
          msq(j,k)=qqbWg
      elseif ((j .lt. 0) .and. (k .gt. 0)) then
          msq(j,k)=qbqWg
      elseif ((j .gt. 0) .and. (k .eq. 0)) then
          msq(j,k)=
     &   qgWq
      elseif ((j .lt. 0) .and. (k .eq. 0)) then
          msq(j,k)=
     &    qbgWqb
      elseif ((j .eq. 0) .and. (k .gt. 0)) then
          msq(j,k)=
     &    gqWq
      elseif ((j .eq. 0) .and. (k .lt. 0)) then
          msq(j,k)=
     &    gqbWqb
      endif

      enddo
      enddo
      return
      end
 
      double precision function wj_w1jet(j1,j2,j3,j4,j5)
      implicit none 
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/masses.h'
      include 'src/Lib/wj/sprods_com.h'
      integer j1,j2,j3,j4,j5
      double precision prop

      prop=((s(j3,j4)-wj_wmass**2)**2+(wj_wmass*wwidth)**2)

      wj_w1jet=(s(j1,j4)**2+s(j2,j3)**2)*s(j3,j4)/(s(j1,j5)*s(j2,j5)*prop)

      return
      end


      subroutine wj_dotem(N,p,s)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision p(mxpart,4),s(mxpart,mxpart)
      integer j,k,N
c---returns 2*piDpj for massless particles
      do j=1,N
      s(j,j)=0d0
      do k=j+1,N
      s(j,k)=2d0*
     & (p(j,4)*p(k,4)-p(j,1)*p(k,1)-p(j,2)*p(k,2)-p(j,3)*p(k,3))
      s(k,j)=s(j,k)
      enddo
      enddo
      return
      end
 

      subroutine wj_zgamps2(j1,j2,j3,j4,j5,za,zb,amps2)
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/zprods_decl.h'
      double complex amps(2,2,2)
      double precision amps2(2,2)
      integer hq,hl,hg,j1,j2,j3,j4,j5
c-- amplitude helicities are amps(quark,lepton,gluon)
      
      amps(1,1,1)=za(j2,j3)/za(j1,j5)/za(j2,j5)
     .             *(za(j2,j1)*zb(j4,j1)+za(j2,j5)*zb(j4,j5))
           
      amps(1,1,2)=zb(j4,j1)/zb(j1,j5)/zb(j2,j5)
     .             *(za(j2,j3)*zb(j2,j1)+za(j3,j5)*zb(j1,j5))
          
      amps(1,2,1)=za(j2,j4)/za(j1,j5)/za(j2,j5)
     .             *(za(j2,j1)*zb(j3,j1)+za(j2,j5)*zb(j3,j5))
     
      amps(1,2,2)=zb(j3,j1)/zb(j1,j5)/zb(j2,j5)
     .             *(za(j2,j4)*zb(j2,j1)+za(j4,j5)*zb(j1,j5))
     
      do hl=1,2
      do hg=1,2
        amps(2,hl,hg)=-dconjg(amps(1,3-hl,3-hg))
      enddo
      enddo
      
      do hq=1,2
      do hl=1,2
        amps2(hq,hl)=cdabs(amps(hq,hl,1))**2+cdabs(amps(hq,hl,2))**2
      enddo
      enddo
      
            
      return
      end
      


      double precision function wj_virt5(ip,za,zb)
      implicit none
************************************************************************ 
*     Author: R.K. Ellis                                               *
*     July, 1999.                                                      *
*   Given za and zb calculate the                                      *
*   the interference of the amplitude for the process                  *
*   0--> qb_R(1)+q_L(2)+l_L(3)+a_R(4)+g_L/R(5)                         *
*   at one loop with the corresponding lowest order amplitude          *
*   summed over the polarizations of the emitted gluon                 *
*   Virtual terms are in units of 
*   (as/4/pi) (4 pi)^ep Gamma(1+ep)*Gamma(1-ep)^2/Gamma(1-2*ep)
************************************************************************
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/qcdcouple.h'
      include 'src/Lib/wj/zprods_decl.h'
      integer ip(5)
      double complex A5LOm,A5NLOm,A5LOp,A5NLOp

c   0--> qb_R(1)+q_L(2)+l_L(3)+a_R(4)+g_L(5)
      call wj_A5NLO(ip(1),ip(2),ip(3),ip(4),ip(5),za,zb,A5LOm,A5NLOm)
c   0--> qb_R(1)+q_L(2)+l_L(3)+a_R(4)+g_R(5)
      call wj_A5NLO(ip(2),ip(1),ip(4),ip(3),ip(5),zb,za,A5LOp,A5NLOp)

c      wj_virt5=
c     . +ason2pi*(Dble(Dconjg(A5LOp)*A5NLOp)+Dble(Dconjg(A5LOm)*A5NLOm))
      wj_virt5=
     . +as/twopi*(Dble(Dconjg(A5LOp)*A5NLOp)+Dble(Dconjg(A5LOm)*A5NLOm))

      return
      end


      subroutine wj_A5NLO(j1,j2,j3,j4,j5,za,zb,A5LOm,A5NLOm)
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/zprods_decl.h'
      integer j1,j2,j3,j4,j5
      double complex wj_A51,wj_A52,A5NLOm,A5LOm

* As originally written, the functions wj_A51, wj_A52 correspond to 
* 0 --> q_R(1)+qb_L(3)+g_R(2)+ebar_L(4)+e_R(5)
* with all RH couplings
* However we want it in our 
* standard form 
*       0--> qb_R(1)+q_L(2)++e_L(3)+ebar_R(4)+g_L(5)
* with all LH couplings 

* so we have made the changes 
*
*                    'q+g+qb-'   (wj_A51)
*                   (1 ---> 2)
*                   (2 ---> 5)
*                   (3 ---> 1)
*                   (4 ---> 4)
*                   (5 ---> 3)

*                    'q+qb-g+'   (wj_A52)
*                   (1 ---> 2)
*                   (2 ---> 1)
*                   (3 ---> 5)
*                   (4 ---> 4)
*                   (5 ---> 3)

*  and also exchanged za and zb.

C--- corresponds to (1V.1) times minus i, with the (wj_A51) change
      A5LOm=-zb(j1,j4)**2/(zb(j2,j5)*zb(j5,j1)*zb(j4,j3))
      A5NLOm=wj_A51(j2,j5,j1,j4,j3,zb,za)+wj_A52(j2,j1,j5,j4,j3,zb,za)/xnsq

c      write(*,*) 'wj_A51 e wj_A52',wj_A51(j2,j5,j1,j4,j3,zb,za),
c     #      wj_A52(j2,j1,j5,j4,j3,zb,za)
      

      return
      end



      double complex function wj_A51(j1,j2,j3,j4,j5,za,zb)
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/zprods_decl.h'
      include 'src/Lib/wj/sprods_com.h'
      include 'src/Lib/wj/scale.h'
      include 'src/Lib/wj/epinv.h'
      integer j1,j2,j3,j4,j5
      double complex Vcc,Fcc,Vsc,Fsc,l12,l23,wj_L0,wj_L1,wj_Lsm1,A5lom
      double complex wj_Lnrat

C    -i * A5tree  
      A5lom =-za(j3,j4)**2/(za(j1,j2)*za(j2,j3)*za(j4,j5))
      l12=wj_Lnrat(musq,-s(j1,j2))
      l23=wj_Lnrat(musq,-s(j2,j3))

C--leading N
      Vcc=
     . -(wj_epinv**2+wj_epinv*l12+0.5d0*l12**2)
     . -(wj_epinv**2+wj_epinv*l23+0.5d0*l23**2)
     . -2d0*(wj_epinv+l23)-4d0      

      Fcc=za(j3,j4)**2/(za(j1,j2)*za(j2,j3)*za(j4,j5))
     . *(wj_Lsm1(-s(j1,j2),-s(j4,j5),-s(j2,j3),-s(j4,j5))
     . -2d0*za(j3,j1)*zb(j1,j5)*za(j5,j4)/za(j3,j4)
     .   *wj_L0(-s(j2,j3),-s(j4,j5))/s(j4,j5))

      Vsc =0.5d0*(wj_epinv+l23)+1d0
      Fsc =za(j3,j4)*za(j3,j1)*zb(j1,j5)*za(j5,j4)
     . /(za(j1,j2)*za(j2,j3)*za(j4,j5))*wj_L0(-s(j2,j3),-s(j4,j5))/s(j4,j5)
     . +0.5d0*(za(j3,j1)*zb(j1,j5))**2*za(j4,j5)
     . /(za(j1,j2)*za(j2,j3))*wj_L1(-s(j2,j3),-s(j4,j5))/s(j4,j5)**2

      wj_A51=(Vcc+Vsc)*A5Lom+Fcc+Fsc

c      write(*,*) 'A5Lom wj_A51',A5Lom

      return  
      end



      double complex function wj_A52(j1,j2,j3,j4,j5,za,zb)
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/zprods_decl.h'
      include 'src/Lib/wj/sprods_com.h'
      include 'src/Lib/wj/scale.h'
      include 'src/Lib/wj/epinv.h'
      integer j1,j2,j3,j4,j5
      double complex Vcc,Fcc,Vsc,Fsc,l12,l45,wj_L0,wj_L1,wj_Lsm1,A5lom
      double complex wj_Lnrat

      l12=wj_Lnrat(musq,-s(j1,j2))  
      l45=wj_Lnrat(musq,-s(j4,j5))  
C    -i * A5tree  
      A5lom=za(j2,j4)**2/(za(j2,j3)*za(j3,j1)*za(j4,j5))
      Vcc=-(wj_epinv**2+wj_epinv*l12+0.5d0*l12**2)
     . -2d0*(wj_epinv+l45)-4d0

C--subleading N
      Fcc=-za(j2,j4)**2/(za(j2,j3)*za(j3,j1)*za(j4,j5))
     . *wj_Lsm1(-s(j1,j2),-s(j4,j5),-s(j1,j3), -s(j4,j5))
     . +za(j2,j4)*(za(j1,j2)*za(j3,j4)-za(j1,j4)*za(j2,j3))
     .  /(za(j2,j3)*za(j1,j3)**2*za(j4,j5))
     . *wj_Lsm1(-s(j1,j2),-s(j4,j5),-s(j2,j3),-s(j4,j5))
     . +2d0*zb(j1,j3)*za(j1,j4)*za(j2,j4)/(za(j1,j3)*za(j4,j5))
     . *wj_L0(-s(j2,j3),-s(j4,j5))/s(j4,j5)

      Vsc=0.5d0*(wj_epinv+l45)+0.5d0
      Fsc=za(j1,j4)**2*za(j2,j3)/(za(j1,j3)**3*za(j4,j5))
     . *wj_Lsm1(-s(j1,j2),-s(j4,j5),-s(j2,j3),-s(j4,j5))
     . -0.5d0*(za(j4,j1)*zb(j1,j3))**2*za(j2,j3)/(za(j1,j3)*za(j4,j5))
     . *wj_L1(-s(j4,j5),-s(j2,j3))/s(j2,j3)**2
     . +za(j1,j4)**2*za(j2,j3)*zb(j3,j1)/(za(j1,j3)**2*za(j4,j5))
     . *wj_L0(-s(j4,j5),-s(j2,j3))/s(j2,j3)
     . -za(j2,j1)*zb(j1,j3)*za(j4,j3)*zb(j3,j5)/za(j1,j3)
     . *wj_L1(-s(j4,j5),-s(j1,j2))/s(j1,j2)**2
     . -za(j2,j1)*zb(j1,j3)*za(j3,j4)*za(j1,j4)/(za(j1,j3)**2*za(j4,j5))
     . *wj_L0(-s(j4,j5),-s(j1,j2))/s(j1,j2)
     . -0.5d0*zb(j3,j5)*(zb(j1,j3)*zb(j2,j5)+zb(j2,j3)*zb(j1,j5))
     . /(zb(j1,j2)*zb(j2,j3)*za(j1,j3)*zb(j4,j5))

      wj_A52=(Vcc+Vsc)*A5lom+Fcc+Fsc

c      write(*,*) 'A5Lom wj_A52',A5Lom

      return  
      end



      subroutine wj_spinoru(N,p,za,zb)
c---Calculate spinor products
c---extended to deal with negative energies ie with all momenta outgoing
c---Arbitrary conventions of Bern, Dixon, Kosower, Weinzierl, 
c---za(i,j)*zb(j,i)=s(i,j)
      implicit none
      include 'src/Lib/wj/constants.h'
      include 'src/Lib/wj/zprods_decl.h'
      include 'src/Lib/wj/sprods_com.h'
      double precision p(mxpart,4),rt(mxpart)
      double complex c23(mxpart),f(mxpart)
      integer i,j,N
      
c---if one of the vectors happens to be zero this routine fails.
      do j=1,N
         za(j,j)=czip
         zb(j,j)=za(j,j)

C-----positive energy case
         if (p(j,4) .gt. 0d0) then
            rt(j)=dsqrt(p(j,4)+p(j,1))
            c23(j)=dcmplx(p(j,3),-p(j,2))
            f(j)=cone
         else
C-----negative energy case
            rt(j)=dsqrt(-p(j,4)-p(j,1))
            c23(j)=dcmplx(-p(j,3),p(j,2))
            f(j)=im
         endif
      enddo
      do i=2,N
         do j=1,i-1
         s(i,j)=two*(p(i,4)*p(j,4)-p(i,1)*p(j,1)
     &              -p(i,2)*p(j,2)-p(i,3)*p(j,3))
         za(i,j)=f(i)*f(j)
     &   *(c23(i)*dcmplx(rt(j)/rt(i))-c23(j)*dcmplx(rt(i)/rt(j)))

         if (abs(s(i,j)).lt.1d-9) then
         zb(i,j)=-(f(i)*f(j))**2*dconjg(za(i,j))
         else
         zb(i,j)=-dcmplx(s(i,j))/za(i,j)
         endif
         za(j,i)=-za(i,j)
         zb(j,i)=-zb(i,j)
         s(j,i)=s(i,j)
         enddo
      enddo

      return
      end


c$$$      subroutine ckmfill(nwz)
c$$$      implicit none
c$$$      include 'src/Lib/wj/constants.h'
c$$$      include 'src/Lib/wj/ckm.h'
c$$$      integer nwz,j,k
c$$$      double precision Vud,Vus,Vub,Vcd,Vcs,Vcb,rtxw
c$$$      common/wj_cabib/Vud,Vus,Vub,
c$$$     &             Vcd,Vcs,Vcb
c$$$      
c$$$c---- initialize Vsq
c$$$      do j=-nf,nf
c$$$      do k=-nf,nf
c$$$      Vsq(j,k)=0d0
c$$$      enddo
c$$$      enddo
c$$$
c$$$C case W+
c$$$      if (nwz .eq. 1) then 
c$$$      
c$$$      Vsq(2,-1)=Vud**2
c$$$      Vsq(2,-3)=Vus**2
c$$$      Vsq(2,-5)=Vub**2
c$$$      Vsq(4,-1)=Vcd**2
c$$$      Vsq(4,-3)=Vcs**2
c$$$      Vsq(4,-5)=Vcb**2
c$$$
c$$$C case W-
c$$$      elseif (nwz .eq. -1) then 
c$$$
c$$$      Vsq(1,-2)=Vud**2
c$$$      Vsq(3,-2)=Vus**2
c$$$      Vsq(5,-2)=Vub**2
c$$$      Vsq(1,-4)=Vcd**2
c$$$      Vsq(3,-4)=Vcs**2
c$$$      Vsq(5,-4)=Vcb**2
c$$$
c$$$
c$$$      endif     
c$$$
c$$$      do j=-nf,nf
c$$$      do k=-nf,nf
c$$$      Vsq(j,k)=Vsq(k,j)
c$$$      enddo
c$$$      enddo
c$$$
c$$$      return
c$$$      end




************************************************************************
*     Author: R.K. Ellis                                               *
*     July, 1998/July 2005/June 2007                                   *
************************************************************************

      double complex function wj_L0(x,y)
      implicit none
      include 'src/Lib/wj/constants.h'
      double complex wj_Lnrat
      double precision x,y,denom
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      wj_L0=-cone-dcmplx(denom*(half+denom/3d0))
      else
      wj_L0=wj_Lnrat(x,y)/dcmplx(denom)
      endif
      return
      end

      double complex function wj_L1(x,y)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x,y,denom
      double complex wj_L0
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      wj_L1=-half*cone-dcmplx(denom/3d0*(one+0.75d0*denom))
      else
      wj_L1=(wj_L0(x,y)+cone)/dcmplx(denom)
      endif
      return
      end

      double complex function wj_L2(x,y)
      implicit none
      include 'src/Lib/wj/constants.h'
      double complex wj_Lnrat
      double precision x,y,r,denom
      r=x/y
      denom=one-r
      if (abs(denom) .lt. 1d-7) then
      wj_L2=(dcmplx(10d0)+denom*(dcmplx(15d0)+dcmplx(18d0)*denom))
     . /dcmplx(60d0)
      else
      wj_L2=(wj_Lnrat(x,y)-dcmplx(0.5d0*(r-1d0/r)))/dcmplx(denom)**3
      endif  
      return
      end

      double complex function wj_L0old(x,y)
      implicit none
      include 'src/Lib/wj/constants.h'
      double complex wj_Lnrat
      double precision x,y,denom
      denom=one-x/y
      wj_L0old=wj_Lnrat(x,y)/dcmplx(denom)
      return
      end


      double complex function wj_L1old(x,y)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x,y,denom
      double complex wj_L0old
      denom=one-x/y
      wj_L1old=(wj_L0old(x,y)+cone)/dcmplx(denom)
      return
      end

      double complex function wj_Ls0(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x1,x2,y1,y2,r1,r2
      double complex wj_Lsm1
      r1=x1/y1
      r2=x2/y2
      wj_Ls0=wj_Lsm1(x1,y1,x2,y2)/dcmplx(one-r1-r2)
      return
      end

      double complex function wj_Ls1(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x1,x2,y1,y2,r1,r2
      double complex wj_Ls0,wj_L0
      r1=x1/y1
      r2=x2/y2
      wj_Ls1=(wj_Ls0(x1,y1,x2,y2)+wj_L0(x1,y1)+wj_L0(x2,y2))/dcmplx(one-r1-r2)
      return
      end



      double complex function wj_Lsm1(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x1,x2,y1,y2,r1,r2,omr1,omr2,wj_DDILOG
      double complex dilog1,dilog2,wj_Lnrat
      r1=x1/y1
      r2=x2/y2
      omr1=one-r1
      omr2=one-r2
      if (omr1 .gt. one) then 
         dilog1=dcmplx(pisqo6-wj_DDILOG(r1))-wj_Lnrat(x1,y1)*dcmplx(log(omr1))
      else
         dilog1=dcmplx(wj_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
         dilog2=dcmplx(pisqo6-wj_DDILOG(r2))-wj_Lnrat(x2,y2)*dcmplx(log(omr2))
      else
          dilog2=dcmplx(wj_DDILOG(omr2))
      endif
      wj_Lsm1=dilog1+dilog2+wj_Lnrat(x1,y1)*wj_Lnrat(x2,y2)-dcmplx(pisqo6)
      return
      end

      double complex function wj_Lsm1_2mh(s,t,m1sq,m2sq)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision s,t,m1sq,m2sq
      double complex wj_Lsm1_2mht,wj_I3m
      wj_Lsm1_2mh=wj_Lsm1_2mht(s,t,m1sq,m2sq)
     & +(half*(s-m1sq-m2sq)+m1sq*m2sq/t)*wj_I3m(s,m1sq,m2sq)
      return
      end

      double complex function wj_Lsm1_2mht(s,t,m1sq,m2sq)
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision s,t,m1sq,m2sq,wj_DDILOG,r1,r2,omr1,omr2
      double complex wj_Lnrat,dilog1,dilog2
      r1=m1sq/t
      r2=m2sq/t
      omr1=one-r1
      omr2=one-r2

      if (omr1 .gt. one) then 
      dilog1=dcmplx(pisqo6-wj_DDILOG(r1))-wj_Lnrat(-m1sq,-t)*dcmplx(log(omr1))
      else
      dilog1=dcmplx(wj_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
      dilog2=dcmplx(pisqo6-wj_DDILOG(r2))-wj_Lnrat(-m2sq,-t)*dcmplx(log(omr2))
      else
      dilog2=dcmplx(wj_DDILOG(omr2))
      endif
      wj_Lsm1_2mht=-dilog1-dilog2
     & +half*(wj_Lnrat(-s,-m1sq)*wj_Lnrat(-s,-m2sq)-wj_Lnrat(-s,-t)**2)
      return
      end


      double complex function wj_Lsm1_2me(s,t,m1sq,m3sq)
      implicit none
c---- formula taken from G.~Duplancic and B~Nizic [arXiv:hep-ph/0006249 v2]
c---- Eq 71
c---- wj_Lsm1_2me notation follows from 
c----  Z.~Bern, L.~J.~Dixon and D.~A.~Kosower,
c----  %``Dimensionally regulated pentagon integrals,''
c----  Nucl. Phys. B {bf 412}, 751 (1994)
c----  [arXiv:hep-ph/9306240].
c----  %%CITATION = HEP-PH 9306240;%%
c----  Eqs. (I.13)
C---- analytic continuation has been checked by calculating numerically.
      include 'src/Lib/wj/constants.h'
      integer j
      double precision s,t,m1sq,m3sq,wj_DDILOG,arg(4),omarg(4),f2me,htheta
      double complex Li2(4),wlog(4)
C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
      htheta(s)=half+half*sign(one,s)

      f2me=(s+t-m1sq-m3sq)/(s*t-m1sq*m3sq)
     
      arg(1)=f2me*s
      arg(2)=f2me*t
      arg(3)=f2me*m1sq
      arg(4)=f2me*m3sq

      do j=1,4
         omarg(j)=one-arg(j)
         wlog(j)=log(abs(arg(j)))    
     .     +impi*dcmplx(htheta(-arg(j))*sign(one,f2me))
         if (omarg(j) .gt. one) then 
             Li2(j)=dcmplx(pisqo6-wj_DDILOG(arg(j)))
     .       -wlog(j)*dcmplx(log(omarg(j)))
          else
             Li2(j)=dcmplx(wj_DDILOG(omarg(j)))
         endif
      enddo
      wj_Lsm1_2me=Li2(1)+Li2(2)-Li2(3)-Li2(4)

      return
      end

c      double complex function wj_Lsm1_2me(s,t,m1sq,m3sq)
c      implicit none
c      include 'src/Lib/wj/constants.h'
c      integer j
c      double precision s,t,m1sq,m3sq,wj_DDILOG,arg(5),omarg(5)
c      double complex wj_Lnrat,Li2(5),wlog(5)
c      arg(1)=m1sq/s
c      wlog(1)=wj_Lnrat(-m1sq,-s)

c      arg(2)=m1sq/t
c      wlog(2)=wj_Lnrat(-m1sq,-t)

c      arg(3)=m3sq/s
c      wlog(3)=wj_Lnrat(-m3sq,-s)

c      arg(4)=m3sq/t
c      wlog(4)=wj_Lnrat(-m3sq,-t)

c      arg(5)=arg(1)*arg(4)
c      wlog(5)=wj_Lnrat(-m1sq,-s)+wj_Lnrat(-m3sq,-t)

c      do j=1,5
c         omarg(j)=one-arg(j)
c         if (omarg(j) .gt. one) then 
c             Li2(j)=dcmplx(pisqo6-wj_DDILOG(arg(j)))-wlog(j)*log(omarg(j))
c          else
c             Li2(j)=dcmplx(wj_DDILOG(omarg(j)))
c         endif
c      enddo
c      wj_Lsm1_2me=Li2(5)-Li2(1)-Li2(2)-Li2(3)-Li2(4)-half*wj_Lnrat(-s,-t)**2
c      return
c      end




      double complex function wj_Lnrat(x,y)
************************************************************************
*     Author: R.K. Ellis                                               *
*     August, 1998.                                                    *
c     wj_Lnrat(x,y)=log(x-i*ep)-log(y-i*ep)                               *
c     this function is hard-wired for sign of epsilon we must adjust   *
c     sign of x and y to get the right sign for epsilon                *
************************************************************************
      implicit none
      include 'src/Lib/wj/constants.h'
      double precision x,y,htheta
C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
      htheta(x)=half+half*sign(one,x)
      wj_Lnrat=dcmplx(dlog(abs(x/y)))-impi*dcmplx((htheta(-x)-htheta(-y)))
      return
      end


      double complex function wj_I3m(s1,s2,s3)
C     This is the function wj_I3m, a massless triangle with all three external 
C     lines offshell defined in BDK
C     %cite{Bern:1997sc}
C     bibitem{Bern:1997sc}
C     Z.~Bern, L.~J.~Dixon and D.~A.~Kosower,
C     %``One-loop amplitudes for e+ e- to four partons,''
C     Nucl. Phys. B {bf 513}, 3 (1998)
C     [arXiv:hep-ph/9708239].
C     %%CITATION = HEP-PH 9708239;%%
C     defined in their equation II.9
C     int da_1 da_2 da_3 /(-a_1*a_2*s1-a_2*a_3*s2-a_3*a_1*s3)
       implicit none
      include 'src/Lib/wj/constants.h'
      double precision s1,s2,s3,smax,smid,smin,del3,rtdel3
      double precision wj_I3m1a,flag
      double complex wj_I3m1b

      smax=max(s1,s2,s3)
      smin=min(s1,s2,s3)
      smid=s1+s2+s3-smax-smin
      del3=s1**2+s2**2+s3**2-two*(s1*s2+s2*s3+s3*s1)      

      if (del3 .gt. 0) then
      rtdel3=sqrt(del3)
         if (smax .lt. 0) then
c---case all negative
             flag=0d0
             wj_I3m=wj_I3m1b(smax,smid,smin,rtdel3,flag)
         elseif (smin .gt. 0) then
c---case all positive
             flag=0d0
             wj_I3m=-wj_I3m1b(-smin,-smid,-smax,rtdel3,flag)
         elseif ((smid .lt. 0) .and. (smin .lt. 0)) then
c---case two negative and one positive
             flag=+1d0
             wj_I3m=wj_I3m1b(smin,smid,smax,rtdel3,flag)
         elseif ((smax .gt. 0).and.(smid .gt. 0)) then
c---case two positive and one negative
             flag=-1d0
             wj_I3m=-wj_I3m1b(-smax,-smid,-smin,rtdel3,flag)
         endif
      elseif (del3 .lt. 0) then 
      rtdel3=sqrt(-del3)
         if (smax .lt. 0) then
c---case all negative
             wj_I3m=+dcmplx(wj_I3m1a(+s1,+s2,+s3,rtdel3))
         elseif (smin .gt. 0) then
c---case all positive
             wj_I3m=-dcmplx(wj_I3m1a(-s1,-s2,-s3,rtdel3))  
          endif
      endif

      return
      end     


      double precision function wj_I3m1a(s1,s2,s3,rtmdel)
      implicit none
C     symmetric form of Lu and Perez
C     %cite{Lu:1992ny}
c     bibitem{Lu:1992ny}
c     H.~J.~Lu and C.~A.~Perez,
c     %``Massless one loop scalar three point integral and associated Clausen,
c     %Glaisher and L functions,''
c     SLAC-PUB-5809
      include 'src/Lib/wj/constants.h'
      double precision s1,s2,s3,d1,d2,d3,rtmdel,arg1,arg2,arg3,wj_DCLAUS

      d1=s1-s2-s3
      d2=s2-s3-s1
      d3=s3-s1-s2
      
      arg1=two*datan(rtmdel/d1)
      arg2=two*datan(rtmdel/d2)
      arg3=two*datan(rtmdel/d3)
      wj_I3m1a=two/rtmdel*(wj_DCLAUS(arg1)+wj_DCLAUS(arg2)+wj_DCLAUS(arg3))

      end

      
      double complex function wj_I3m1b(s1,s2,s3,rtdel,flag)
      implicit none
C     form of Ussyukina and Davydychev
C %cite{Usyukina:1994iw}
C bibitem{Usyukina:1994iw}
C   N.~I.~Usyukina and A.~I.~Davydychev,
C   %``New results for two loop off-shell three point diagrams,''
C  Phys. Lett. B {bf 332}, 159 (1994)
C  [arXiv:hep-ph/9402223].
C  %%CITATION = HEP-PH 9402223;%%

      include 'src/Lib/wj/constants.h'
      double precision s1,s2,s3,d3,temp,wj_DDILOG,wj_xlog,ylog,rat
      double precision x,y,rho,rtdel,argx,argy,argdlx,argdly,flag
      d3=s3-s1-s2
      x=s1/s3
      y=s2/s3
      rat=0.5d0*(d3+rtdel)/s3
      if (abs(rat) .lt. 1d-3) rat=2d0*s1*s2/(s3*(d3-rtdel))
      rho=1d0/rat
      argx=rho*x
      argy=rho*y
      argdlx=-argx
      argdly=-argy

      if ((argdlx .gt. 1d0) .or. (argdly .gt. 1d0)) then
      write(6,*) 'problems with call of wj_I3m1b'
      write(6,*) 'argdlx',argdlx
      write(6,*) 'argdly',argdly
      stop
      endif

      wj_xlog=log(abs(argx))
      ylog=log(abs(argy))
      temp=wj_xlog*ylog+pisq/3d0+(ylog-wj_xlog)*log((one+argy)/(one+argx))
     & +two*(wj_DDILOG(argdlx)+wj_DDILOG(argdly))
      wj_I3m1b=Dcmplx(temp-abs(flag)*pisq)+impi*Dcmplx(flag*(wj_xlog+ylog))
      wj_I3m1b=-wj_I3m1b/Dcmplx(rtdel)
      end





*
* $Id: dclaus64.F,v 1.2 1996/04/02 16:23:45 mclareni Exp $
*
* $Log: dclaus64.F,v $
* Revision 1.2  1996/04/02 16:23:45  mclareni
* More precise dclaus64 (C326), test added and C344 removed from TESTALL
*
* Revision 1.1.1.1  1996/04/01 15:02:03  mclareni
* Mathlib wj_gen
*
*
      DOUBLE PRECISION FUNCTION wj_DCLAUS(X)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      DIMENSION A(0:8),B(0:13)
 
      PARAMETER (R1 = 1d0, HF =R1/2d0)
      PARAMETER (PI = 3.14159 26535 89793 24D0)
      PARAMETER (PI2 = 2d0*PI, PIH = PI/2d0, RPIH = 2d0/PI)
 
      DATA A( 0) / 0.02795 28319 73575 6613D0/
      DATA A( 1) / 0.00017 63088 74389 8116D0/
      DATA A( 2) / 0.00000 12662 74146 1157D0/
      DATA A( 3) / 0.00000 00117 17181 8134D0/
      DATA A( 4) / 0.00000 00001 23006 4129D0/
      DATA A( 5) / 0.00000 00000 01395 2729D0/
      DATA A( 6) / 0.00000 00000 00016 6908D0/
      DATA A( 7) / 0.00000 00000 00000 2076D0/
      DATA A( 8) / 0.00000 00000 00000 0027D0/
 
      DATA B( 0) / 0.63909 70888 57265 341D0/
      DATA B( 1) /-0.05498 05693 01851 716D0/
      DATA B( 2) /-0.00096 12619 45950 606D0/
      DATA B( 3) /-0.00003 20546 86822 550D0/
      DATA B( 4) /-0.00000 13294 61695 426D0/
      DATA B( 5) /-0.00000 00620 93601 824D0/
      DATA B( 6) /-0.00000 00031 29600 656D0/
      DATA B( 7) /-0.00000 00001 66351 954D0/
      DATA B( 8) /-0.00000 00000 09196 527D0/
      DATA B( 9) /-0.00000 00000 00524 004D0/
      DATA B(10) /-0.00000 00000 00030 580D0/
      DATA B(11) /-0.00000 00000 00001 820D0/
      DATA B(12) /-0.00000 00000 00000 110D0/
      DATA B(13) /-0.00000 00000 00000 007D0/
 
      V=MOD(ABS(X),PI2)
      S=SIGN(R1,X)
      IF(V .GT. PI) THEN
       V=PI2-V
       S=-S
      ENDIF
      IF(V .EQ. 0d0 .OR. V .EQ. PI) THEN
       H=0d0
      ELSEIF(V .LT. PIH) THEN
       U=RPIH*V
       H=2d0*U**2-1d0
       wj_alfa=H+H
       B1=0d0
       B2=0d0
       DO 1 I = 8,0,-1
       B0=A(I)+wj_alfa*B1-B2
       B2=B1
    1  B1=B0
       H=V*(1d0-LOG(V)+HF*V**2*(B0-H*B2))
      ELSE
       U=RPIH*V-2d0
       H=2d0*U**2-1d0
       wj_alfa=H+H
       B1=0d0
       B2=0d0
       DO 2 I = 13,0,-1
       B0=B(I)+wj_alfa*B1-B2
       B2=B1
    2  B1=B0
       H=(PI-V)*(B0-H*B2)
      ENDIF
      wj_DCLAUS=S*H
      RETURN
      END



      DOUBLE PRECISION FUNCTION wj_DDILOGMCFM(X)

      DOUBLE PRECISION X,Y,T,S,A,PI3,PI6,ZERO,ONE,HALF,MALF,MONE,MTWO
      DOUBLE PRECISION C(0:18),H,wj_alfa,B0,B1,B2

      DATA ZERO /0.0D0/, ONE /1.0D0/
      DATA HALF /0.5D0/, MALF /-0.5D0/, MONE /-1.0D0/, MTWO /-2.0D0/
      DATA PI3 /3.28986 81336 96453D0/, PI6 /1.64493 40668 48226D0/

      DATA C( 0) / 0.42996 69356 08137 0D0/
      DATA C( 1) / 0.40975 98753 30771 1D0/
      DATA C( 2) /-0.01858 84366 50146 0D0/
      DATA C( 3) / 0.00145 75108 40622 7D0/
      DATA C( 4) /-0.00014 30418 44423 4D0/
      DATA C( 5) / 0.00001 58841 55418 8D0/
      DATA C( 6) /-0.00000 19078 49593 9D0/
      DATA C( 7) / 0.00000 02419 51808 5D0/
      DATA C( 8) /-0.00000 00319 33412 7D0/
      DATA C( 9) / 0.00000 00043 45450 6D0/
      DATA C(10) /-0.00000 00006 05784 8D0/
      DATA C(11) / 0.00000 00000 86121 0D0/
      DATA C(12) /-0.00000 00000 12443 3D0/
      DATA C(13) / 0.00000 00000 01822 6D0/
      DATA C(14) /-0.00000 00000 00270 1D0/
      DATA C(15) / 0.00000 00000 00040 4D0/
      DATA C(16) /-0.00000 00000 00006 1D0/
      DATA C(17) / 0.00000 00000 00000 9D0/
      DATA C(18) /-0.00000 00000 00000 1D0/

      IF(X .EQ. ONE) THEN
       wj_DDILOGMCFM=PI6
       RETURN
      ELSE IF(X .EQ. MONE) THEN
       wj_DDILOGMCFM=MALF*PI6
       RETURN
      END IF
      T=-X
      IF(T .LE. MTWO) THEN
       Y=MONE/(ONE+T)
       S=ONE
       A=-PI3+HALF*(LOG(-T)**2-LOG(ONE+ONE/T)**2)
      ELSE IF(T .LT. MONE) THEN
       Y=MONE-T
       S=MONE
       A=LOG(-T)
       A=-PI6+A*(A+LOG(ONE+ONE/T))
      ELSE IF(T .LE. MALF) THEN
       Y=(MONE-T)/T
       S=ONE
       A=LOG(-T)
       A=-PI6+A*(MALF*A+LOG(ONE+T))
      ELSE IF(T .LT. ZERO) THEN
       Y=-T/(ONE+T)
       S=MONE
       A=HALF*LOG(ONE+T)**2
      ELSE IF(T .LE. ONE) THEN
       Y=T
       S=ONE
       A=ZERO
      ELSE
       Y=ONE/T
       S=MONE
       A=PI6+HALF*LOG(T)**2
      END IF

      H=Y+Y-ONE
      wj_alfa=H+H
      B1=ZERO
      B2=ZERO
      DO 1 I = 18,0,-1
      B0=C(I)+wj_alfa*B1-B2
      B2=B1
    1 B1=B0
      wj_DDILOGMCFM=-(S*(B0-H*B2)+A)
      RETURN
      END
