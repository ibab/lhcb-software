      subroutine ww_qqb_ww(p,msq)
      implicit none
C----Author R.K.Ellis December 1998
C----modified by JMC to include supplementary diagrams February 1999
c----Matrix element for WW production
c----in the notation of DKS
C----averaged over initial colours and spins
C----massless final state particles
c     q(-p1)+qbar(-p2)-->q'(p5)+bar{q'}(p6)+n(p3)+ebar(p4)
c--- note that non-leptonic W decays do not include scattering diagrams

      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      include 'src/Lib/ww/zcouple.h'
      include 'src/Lib/ww/wcouple.h'
      include 'src/Lib/ww/ewcharge.h'
      include 'src/Lib/ww/anomcoup.h'
      include 'src/Lib/ww/vvsettings.h'

      double precision msq(-nf:nf,-nf:nf),p(mxpart,4),qdks(mxpart,4)
      double complex prop12,prop34,prop56
      double complex AWW(2),ww_a6treea,A6b_1,A6b_2,A6b_3
      double complex Fa123456,Fa213456,Fb123456_z,Fb213456_z
      double complex Fa126543,Fa216543,Fb126543_z,Fb216543_z
      double complex Fb123456_g,Fb213456_g,Fb126543_g,Fb216543_g
      double complex Fa341256,Fa653421,Fa346521,Fa651243
      double complex Fa342156,Fa653412,Fa346512,Fa652143
      double complex cs_z(2,2),cs_g(2,2),cgamz(2,2),cz(2,2)
      double precision fac,xfac
      integer j,k,jk,tjk,minus,mplus
      data minus,mplus/1,2/
      double complex ww_cpropfac
      external ww_cpropfac

      
      fac=gw**8*aveqq*xn 
      do j=-nf,nf
      do k=-nf,nf
c--set msq=0 to initalize
      msq(j,k)=0d0
      enddo
      enddo
c     -- to compare with MadGraph and MCFM      
c      zerowidth =.false.

C----Change the momenta to DKS notation 
c   We have --- f(p1) + f'(p2)-->mu^-(p5)+nubar(p6)+e^+(p4)+nu(p3)
c   DKS have--- ubar(q1)+u(q2)-->mu^-(q3)+nubar(q4)+e^+(q5)+nu(q6)

      do j=1,4
      qdks(1,j)=p(1,j)
      qdks(2,j)=p(2,j)
      qdks(3,j)=p(5,j)
      qdks(4,j)=p(6,j)
      qdks(5,j)=p(4,j)
      qdks(6,j)=p(3,j)
      enddo

      call ww_spinoru(6,qdks,za,zb)
c--   s returned from sprod (common block) is 2*dot product

C     calculate propagators 
c      prop12=s(1,2)/dcmplx(s(1,2)-zmass**2,zmass*zwidth)
c      prop34=s(3,4)/dcmplx(s(3,4)-wmass**2,wmass*wwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-wmass**2,wmass*wwidth)
      prop12=ww_cpropfac(s(1,2),zmass,zwidth)
      prop34=ww_cpropfac(s(3,4),wmass,wwidth)
      prop56=ww_cpropfac(s(5,6),wmass,wwidth)

c-- couplings with or without photon pole
      do j=1,2
      cs_z(minus,j)=+mp(j)*l(j)*sin2w*prop12
      cs_z(mplus,j)=-mp(j)*2d0*Q(j)*xw*prop12
      cs_g(minus,j)=+mp(j)*2d0*Q(j)*xw
      cs_g(mplus,j)=+mp(j)*2d0*Q(j)*xw
      if (.not.dronly) then
      cz(minus,j)=2d0*xw*ln*L(j)*prop12
      cz(mplus,j)=2d0*xw*ln*R(j)*prop12
      cgamz(minus,j)=2d0*xw*(-Q(j)+le*L(j)*prop12)
      cgamz(mplus,j)=2d0*xw*(-Q(j)+le*R(j)*prop12)
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
      endif

      do j=-nf,nf
      k=-j
c--Exclude gluon-gluon initial state
      if (j.eq.0) go to 20
      jk=max(j,k)

c--assign values
c---Remember that base process is ub-u so this has the natural 123456 order
      if (j .gt. 0) then
          if         (tau(jk) .eq. +1d0) then
            AWW(minus)=(Fa213456+cs_z(minus,2)*Fb213456_z
     .                          +cs_g(minus,2)*Fb213456_g)*prop56*prop34
            AWW(mplus)=(cs_z(mplus,2)*Fb123456_z
     .                 +cs_g(mplus,2)*Fb123456_g)*prop56*prop34
          elseif     (tau(jk) .eq. -1d0) then
            AWW(minus)=(Fa216543+cs_z(minus,1)*Fb216543_z
     .                          +cs_g(minus,1)*Fb216543_g)*prop56*prop34
            AWW(mplus)=(cs_z(mplus,1)*Fb126543_z
     .                 +cs_g(mplus,1)*Fb126543_g)*prop56*prop34
          endif
      elseif (j .lt. 0) then
          if     (tau(jk) .eq. +1d0) then
            AWW(minus)=(Fa123456+cs_z(minus,2)*Fb123456_z
     .                          +cs_g(minus,2)*Fb123456_g)*prop56*prop34
            AWW(mplus)=(cs_z(mplus,2)*Fb213456_z
     .                 +cs_g(mplus,2)*Fb213456_g)*prop56*prop34
            AWW(mplus)=(cs_z(mplus,2)*Fb213456_z
     .                 +cs_g(mplus,2)*Fb213456_g)*prop56*prop34

          elseif (tau(jk) .eq. -1d0) then
            AWW(minus)=(Fa126543+cs_z(minus,1)*Fb126543_z
     .                          +cs_g(minus,1)*Fb126543_g)*prop56*prop34
            AWW(mplus)=(cs_z(mplus,1)*Fb216543_z
     .                 +cs_g(mplus,1)*Fb216543_g)*prop56*prop34
          endif
      endif

     
      if (.not.dronly) then
c---we need supplementary diagrams for gauge invariance.
C---tjk is equal to 2 (u,c) or 1 (d,s,b)
      tjk=2-mod(abs(jk),2)     
      if (j .gt. 0) then
          AWW(minus)=AWW(minus)
     .              +cgamz(minus,tjk)*(Fa342156*prop56+Fa653412*prop34)
     .                 +cz(minus,tjk)*(Fa346512*prop56+Fa652143*prop34)
          AWW(mplus)=AWW(mplus)
     .              +cgamz(mplus,tjk)*(Fa341256*prop56+Fa653421*prop34)
     .                 +cz(mplus,tjk)*(Fa346521*prop56+Fa651243*prop34)
      elseif (j .lt. 0) then
          AWW(minus)=AWW(minus)
     .             +cgamz(minus,tjk)*(Fa341256*prop56+Fa653421*prop34)
     .                +cz(minus,tjk)*(Fa346521*prop56+Fa651243*prop34)
          AWW(mplus)=AWW(mplus)
     .             +cgamz(mplus,tjk)*(Fa342156*prop56+Fa653412*prop34)
     .                +cz(mplus,tjk)*(Fa346512*prop56+Fa652143*prop34)
      endif
      endif

      msq(j,k)=fac*(abs(AWW(minus))**2+abs(AWW(mplus))**2)

 20   continue

      enddo
      return
      end


