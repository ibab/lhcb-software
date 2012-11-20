      subroutine ww_qqb_ww_v(p,msqv)
      implicit none
C----Author R.K.Ellis December 1998
C----modified by JMC to include supplementary diagrams February 1999
c----Matrix element for WW production
c----in the notation of DKS
C----averaged over initial colours and spins
C----massless b quarks
c     q(-p1)+qbar(-p2)-->q'(p5)+bar{q'}(p6)+n(p3)+ebar(p4)
      include 'src/Lib/ww/qcdcouple.h'
      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      include 'src/Lib/ww/zcouple.h'
      include 'src/Lib/ww/wcouple.h'
      include 'src/Lib/ww/ewcharge.h'
      include 'src/Lib/ww/anomcoup.h'
      include 'src/Lib/ww/pwhg_st.h'
      include 'src/Lib/ww/vvsettings.h'
      double precision msq(-nf:nf,-nf:nf),msqv(-nf:nf,-nf:nf),
     . p(mxpart,4),qdks(mxpart,4),facnlo
      double complex AWWM,AWWP,BWWM,BWWP
      double complex prop12,prop34,prop56
      double complex ww_a6treea,ww_a6loopa,A6b_1,A6b_2,A6b_3
      double complex ww_Vpole,Vpole12,suppl
      double complex Fa123456,Fa213456,Fb123456_z,Fb213456_z
      double complex Fa126543,Fa216543,Fb126543_z,Fb216543_z
      double complex Fb123456_g,Fb213456_g,Fb126543_g,Fb216543_g
      double complex La123456,La213456,Lb123456_z,Lb213456_z
      double complex La126543,La216543,Lb126543_z,Lb216543_z
      double complex Lb123456_g,Lb213456_g,Lb126543_g,Lb216543_g
      double complex Fa341256,Fa653421,Fa346521,Fa651243
      double complex Fa342156,Fa653412,Fa346512,Fa652143
      double complex cl_z(2),cr_z(2),cl_g(2),cr_g(2)
      double complex clgamz(2),crgamz(2),clz(2),crz(2)
      double precision FAC,xfac
      integer j,k,jk
      double complex ww_cpropfac
      external ww_cpropfac

      fac=gw**8*aveqq*xn
      facnlo=ason2pi*cf
c--- set msqv=0 to initalize
      do j=-nf,nf
      do k=-nf,nf
      msqv(j,k)=0d0
      enddo
      enddo
      
c--- calculate the lowest order matrix element
      call ww_qqb_ww(p,msq)

c--- Change the momenta to DKS notation 
c    We have --- f(p1) + f'(p2)-->mu^-(p5)+nubar(p6)+e^+(p4)+nu(p3)
c    DKS have--- ubar(q1)+u(q2)-->mu^-(q3)+nubar(q4)+e^+(q5)+nu(q6)

      do j=1,4
      qdks(1,j)=p(1,j)
      qdks(2,j)=p(2,j)
      qdks(3,j)=p(5,j)
      qdks(4,j)=p(6,j)
      qdks(5,j)=p(4,j)
      qdks(6,j)=p(3,j)
      enddo

c-- s returned from sprod (common block) is 2*dot product
      call ww_spinoru(6,qdks,za,zb)

c--   calculate propagators
c      prop12=s(1,2)/dcmplx(s(1,2)-zmass**2,zmass*zwidth)
c      prop34=s(3,4)/dcmplx(s(3,4)-wmass**2,wmass*wwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-wmass**2,wmass*wwidth)
      prop12=ww_cpropfac(s(1,2),zmass,zwidth)
      prop34=ww_cpropfac(s(3,4),wmass,wwidth)
      prop56=ww_cpropfac(s(5,6),wmass,wwidth)
      
c-- couplings with or without photon pole
      do j=1,2
      cl_z(j)=+mp(j)*l(j)*sin2w*prop12
      cr_z(j)=-mp(j)*2d0*Q(j)*xw*prop12
      cl_g(j)=+mp(j)*2d0*Q(j)*xw
      cr_g(j)=+mp(j)*2d0*Q(j)*xw
      if (.not.dronly) then
      clgamz(j)=two*xw*(-Q(j)+le*L(j)*prop12)
      crgamz(j)=two*xw*(-Q(j)+le*R(j)*prop12)
      clz(j)=two*xw*ln*L(j)*prop12
      crz(j)=two*xw*ln*R(j)*prop12
      endif
      enddo


c--- apply a dipole form factor to anomalous couplings
      xfac=1d0/(1d0+s(1,2)/(tevscale*1d3)**2)**2
      xdelg1_z=xfac*delg1_z
      xdelg1_g=xfac*delg1_g
      xdelk_z=xfac*delk_z
      xdelk_g=xfac*delk_g
      xlambda_z=xfac*lambda_z
      xlambda_g=xfac*lambda_g
      
c---case dbar-d and d-dbar
      Fa126543=ww_a6treea(1,2,6,5,4,3,za,zb)
      Fa216543=ww_a6treea(2,1,6,5,4,3,za,zb)
      Fa123456=ww_a6treea(1,2,3,4,5,6,za,zb)
      Fa213456=ww_a6treea(2,1,3,4,5,6,za,zb)

      call ww_a6treeb_anom(1,2,3,4,5,6,za,zb,A6b_1,A6b_2,A6b_3)
      Fb123456_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_2*(2d0*(1d0+xdelg1_z))
     .          +A6b_3*(xlambda_z/wmass**2)
      Fb123456_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_2*(2d0*(1d0+xdelg1_g))
     .          +A6b_3*(xlambda_g/wmass**2)
      Fb126543_z=-Fb123456_z
      Fb126543_g=-Fb123456_g
      call ww_a6treeb_anom(2,1,3,4,5,6,za,zb,A6b_1,A6b_2,A6b_3)
      Fb213456_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_2*(2d0*(1d0+xdelg1_z))
     .          +A6b_3*(xlambda_z/wmass**2)
      Fb213456_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_2*(2d0*(1d0+xdelg1_g))
     .          +A6b_3*(xlambda_g/wmass**2)
      Fb216543_z=-Fb213456_z
      Fb216543_g=-Fb213456_g
     
c      Fb123456=ww_a6treeb(1,2,3,4,5,6,za,zb)
c      Fb126543=-Fb123456
c      Fb213456=ww_a6treeb(2,1,3,4,5,6,za,zb)
c      Fb216543=-Fb213456

      La126543=ww_a6loopa(1,2,6,5,4,3,za,zb)
      La216543=ww_a6loopa(2,1,6,5,4,3,za,zb)
      La123456=ww_a6loopa(1,2,3,4,5,6,za,zb)
      La213456=ww_a6loopa(2,1,3,4,5,6,za,zb)

c--- loop for a6b is simply tree*ww_Vpole
      Vpole12=ww_Vpole(s(1,2))
      Lb123456_z=Vpole12*Fb123456_z
      Lb213456_z=Vpole12*Fb213456_z
      Lb126543_z=Vpole12*Fb126543_z
      Lb216543_z=Vpole12*Fb216543_z
      Lb123456_g=Vpole12*Fb123456_g
      Lb213456_g=Vpole12*Fb213456_g
      Lb126543_g=Vpole12*Fb126543_g
      Lb216543_g=Vpole12*Fb216543_g

c      Lb123456=A6loopb(1,2,3,4,5,6,za,zb)
c      Lb213456=A6loopb(2,1,3,4,5,6,za,zb)
c      Lb126543=A6loopb(1,2,6,5,4,3,za,zb)
c      Lb216543=A6loopb(2,1,6,5,4,3,za,zb)

      if (.not.dronly) then
c---for supplementary diagrams.
      Fa341256=ww_a6treea(3,4,1,2,5,6,za,zb)
      Fa653421=ww_a6treea(6,5,3,4,2,1,za,zb)
      Fa346521=ww_a6treea(3,4,6,5,2,1,za,zb)
      Fa651243=ww_a6treea(6,5,1,2,4,3,za,zb)
      Fa342156=ww_a6treea(3,4,2,1,5,6,za,zb)
      Fa653412=ww_a6treea(6,5,3,4,1,2,za,zb)
      Fa346512=ww_a6treea(3,4,6,5,1,2,za,zb)
      Fa652143=ww_a6treea(6,5,2,1,4,3,za,zb)

c---loop diagrams just tree*ww_Vpole since they're all triangle-type
      Vpole12=ww_Vpole(s(1,2))
      endif

      do j=-nf,nf
      k=-j
      if (j.eq.0) go to 20
      jk=max(j,k)

c--assign values
      if (j .gt. 0) then
         if (tau(jk) .eq. +1d0) then
           AWWM=(Fa213456+cl_z(2)*Fb213456_z
     .                   +cl_g(2)*Fb213456_g)*prop56*prop34
           AWWP=(cr_z(2)*Fb123456_z+cr_g(2)*Fb123456_g)*prop56*prop34
           BWWM=(La213456+cl_z(2)*Lb213456_z
     .                   +cl_g(2)*Lb213456_g)*prop56*prop34
           BWWP=(cr_z(2)*Lb123456_z+cr_g(2)*Lb123456_g)*prop56*prop34
         elseif (tau(jk) .eq. -1d0) then
           AWWM=(Fa216543+cl_z(1)*Fb216543_z
     .                   +cl_g(1)*Fb216543_g)*prop56*prop34
           AWWP=(cr_z(1)*Fb126543_z+cr_g(1)*Fb126543_g)*prop56*prop34
           BWWM=(La216543+cl_z(1)*Lb216543_z
     .                   +cl_g(1)*Lb216543_g)*prop56*prop34
           BWWP=(cr_z(1)*Lb126543_z+cr_g(1)*Lb126543_g)*prop56*prop34
         endif
      elseif (j .lt. 0) then
         if (tau(jk) .eq. +1d0) then
C---remember that this is the basic process, M refers to the helicity
c---of the outgoing quark
           AWWM=(Fa123456+cl_z(2)*Fb123456_z
     .                   +cl_g(2)*Fb123456_g)*prop56*prop34
           AWWP=(cr_z(2)*Fb213456_z+cr_g(2)*Fb213456_g)*prop56*prop34
           BWWM=(La123456+cl_z(2)*Lb123456_z
     .                   +cl_g(2)*Lb123456_g)*prop56*prop34
           BWWP=(cr_z(2)*Lb213456_z+cr_g(2)*Lb213456_g)*prop56*prop34
         elseif (tau(jk) .eq. -1d0) then
           AWWM=(Fa126543+cl_z(1)*Fb126543_z
     .                   +cl_g(1)*Fb126543_g)*prop56*prop34
           AWWP=(cr_z(1)*Fb216543_z+cr_g(1)*Fb216543_g)*prop56*prop34
           BWWM=(La126543+cl_z(1)*Lb126543_z
     .                   +cl_g(1)*Lb126543_g)*prop56*prop34
           BWWP=(cr_z(1)*Lb216543_z+cr_g(1)*Lb216543_g)*prop56*prop34
         endif
      endif
      if (.not.dronly) then
c---we need supplementary diagrams for gauge invariance.
      if     (j .gt. 0) then
          if     (tau(jk) .eq. +1d0) then
                   suppl=clgamz(2)*(Fa342156*prop56+Fa653412*prop34)
     .                  +clz(2)*(Fa346512*prop56+Fa652143*prop34)
                   AWWM=AWWM+suppl
                   BWWM=BWWM+suppl*Vpole12
                   suppl=crgamz(2)*(Fa341256*prop56+Fa653421*prop34)
     .                  +crz(2)*(Fa346521*prop56+Fa651243*prop34)
                   AWWP=AWWP+suppl
                   BWWP=BWWP+suppl*Vpole12
          elseif (tau(jk) .eq. -1d0) then
                   suppl=clgamz(1)*(Fa342156*prop56+Fa653412*prop34)
     .                  +clz(1)*(Fa346512*prop56+Fa652143*prop34)
c                   suppl=clgamz(1)*(Fa652143*prop34+Fa346512*prop56)
c     .                  +clz(1)*(Fa653412*prop34+Fa342156*prop56)
                   AWWM=AWWM+suppl
                   BWWM=BWWM+suppl*Vpole12
                   suppl=crgamz(1)*(Fa341256*prop56+Fa653421*prop34)
     .                  +crz(1)*(Fa346521*prop56+Fa651243*prop34)
c                   suppl=crgamz(1)*(Fa651243*prop34+Fa346521*prop56)
c     .                  +crz(1)*(Fa653421*prop34+Fa341256*prop56)
                   AWWP=AWWP+suppl
                   BWWP=BWWP+suppl*Vpole12
         endif
      elseif (j .lt. 0) then
          if     (tau(jk) .eq. +1d0) then
                   suppl=clgamz(2)*(Fa341256*prop56+Fa653421*prop34)
     .                  +clz(2)*(Fa346521*prop56+Fa651243*prop34)
                   AWWM=AWWM+suppl
                   BWWM=BWWM+suppl*Vpole12
                   suppl=crgamz(2)*(Fa342156*prop56+Fa653412*prop34)
     .                  +crz(2)*(Fa346512*prop56+Fa652143*prop34)
                   AWWP=AWWP+suppl
                   BWWP=BWWP+suppl*Vpole12
          elseif (tau(jk) .eq. -1d0) then
                   suppl=clgamz(1)*(Fa341256*prop56+Fa653421*prop34)
     .                  +clz(1)*(Fa346521*prop56+Fa651243*prop34)
c                   suppl=clgamz(1)*(Fa651243*prop34+Fa346521*prop56)
c     .                  +clz(1)*(Fa653421*prop34+Fa341256*prop56)
                   AWWM=AWWM+suppl
                   BWWM=BWWM+suppl*Vpole12
                   suppl=crgamz(1)*(Fa342156*prop56+Fa653412*prop34)
     .                  +crz(1)*(Fa346512*prop56+Fa652143*prop34)
c                   suppl=crgamz(1)*(Fa652143*prop34+Fa346512*prop56)
c     .                  +crz(1)*(Fa653412*prop34+Fa342156*prop56)
                   AWWP=AWWP+suppl
                   BWWP=BWWP+suppl*Vpole12
          endif
      endif
      endif


      msqv(j,k)=facnlo*fac*two*dble(dconjg(AWWM)*BWWM+dconjg(AWWP)*BWWP)


 20   continue
      enddo
        
      return
      end
