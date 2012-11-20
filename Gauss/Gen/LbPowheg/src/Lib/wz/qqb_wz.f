      subroutine wz_qqb_wz(p,msq)
      implicit none
C----Author R.K.Ellis December 1998
C----modified by JMC to include supplementary diagrams February 1999
c----Matrix element for WZ production
c----in the notation of DKS
C----averaged over initial oolours and spins
C For wz_nwz=+1
c     u(-p1)+dbar(-p2)-->mu^-(p5)+mu^+(p6)+n(p3)+e^+(p4)
C For wz_nwz=-1
c     d(-p1)+ubar(-p2)-->mu^-(p5)+mu^+(p6)+e^-(p3)+nbar(p4)
c---
c     Notation to allow room for p3 --- gluon emission.
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/ckm.h'
      include 'src/Lib/wz/zwcouple.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      include 'src/Lib/wz/ewcharge.h'
      include 'src/Lib/wz/anomcoup.h'
      include 'src/Lib/wz/nwz.h'
      include 'src/Lib/wz/cvecbos.h'
      include 'src/Lib/wz/vvsettings.h'
      double precision msq(-nf:nf,-nf:nf),p(mxpart,4),qdks(mxpart,4)     
      double complex AWZM,AWZP,wz_a6treea
      double complex prop34,prop56,prop12
      double complex Fa123456,Fa126543,Fb123456_z,Fb123456_g
      double complex Fa123465,Fa125643,Fb123465_z,Fb123465_g
      double complex Fa213456,Fa216543,Fb213456_z,Fb213456_g
      double complex Fa213465,Fa215643,Fb213465_z,Fb213465_g
      double complex Fa346512,Fa342156,Fa652143
      double complex Fa345612,Fa342165,Fa653421
      double complex Fa346521,Fa341256,Fa651243
      double complex Fa345621,Fa341265,Fa653412
      double complex ZgL(-nf:nf),ZgR(-nf:nf),A6b_1,A6b_2,A6b_3,A6b_4
      double precision v2(2),cl1,cl2,en1,en2
      double precision ave,cotw,wwflag,xfac
      character*2 wz_plabel(mxpart)
      common/wz_plabel/wz_plabel
      double precision FAC,FACM
      integer j,k
      parameter(ave=0.25d0/xn)
      data cl1,cl2,en1,en2/4*1d0/

      integer iloop,nloop
      double complex AWZM_SAVE(-nf:nf,-nf:nf)
      double complex wz_cpropfac
      external wz_cpropfac

      FAC=-two*gwsq*esq


      if (wz_nwz.eq.1) then
      FACM=FAC
      elseif (wz_nwz.eq.-1) then
      FACM=-FAC
      else
      write(6,*) 'wz_nwz .ne. +1 or -1'
      stop
      endif 
      if     (wz_nwz.eq.-1) then
        cl1=1d0
        cl2=0d0
        en1=le
        en2=ln
      elseif (wz_nwz.eq.1) then
        cl1=0d0
        cl2=1d0
        en1=ln
        en2=le
      endif
      
c--- wwflag=1 for most cases, indicating presence of diagram with 2 W's
      wwflag=1d0
c--- but for Z -> bbbar this diagram contains |V_tb|**2 which we take 0
      if (wz_plabel(5) .eq. 'qb') then    
        wwflag=0d0
      endif
      
c-- if Z -> neutrinos, we need to switch c1 and c2
      if ((vdecaymodeZ.eq.12).or.(vdecaymodeZ.eq.14).or.
     .     (vdecaymodeZ.eq.16)) then
        cl1=1d0-cl1
        cl2=1d0-cl2
      endif
      
      v2(1)=wz_L1
      v2(2)=r1
      cotw=dsqrt((one-xw)/xw)

      do j=-nf,nf
      do k=-nf,nf
c--set msq=0 to initalize
      msq(j,k)=0d0
      AWZM_SAVE(j,k)=0d0
      enddo
      enddo

      if (interference) then
         nloop=2
      else
         nloop=1
      endif

      do iloop=1,nloop

C----Change the momenta to DKS notation 
c   We have --- d(-p1)+ubar(-p2)-->nu(p3)+e^+(p4)+mu^-(p5)+mu^+(p6)
c   DKS have--- u( q2)+dbar( q1)-->nu(q3)+e^+(q4)+mu^-(q6)+mu^+(q5)
      

      if (iloop.eq.1) then
         do j=1,4
            qdks(1,j)=p(1,j)
            qdks(2,j)=p(2,j)
            qdks(3,j)=p(3,j)
            qdks(4,j)=p(4,j)
            qdks(5,j)=p(6,j)
            qdks(6,j)=p(5,j)
         enddo
      elseif (iloop.eq.2) then
         if (wz_nwz.eq.1) then
         do j=1,4
            qdks(1,j)=p(1,j)
            qdks(2,j)=p(2,j)
            qdks(3,j)=p(3,j)
            qdks(4,j)=p(6,j)
            qdks(5,j)=p(4,j)
            qdks(6,j)=p(5,j)
         enddo
         elseif (wz_nwz.eq.-1) then
         do j=1,4
            qdks(1,j)=p(1,j)
            qdks(2,j)=p(2,j)
            qdks(3,j)=p(5,j)
            qdks(4,j)=p(4,j)
            qdks(5,j)=p(6,j)
            qdks(6,j)=p(3,j)
         enddo
         endif
      endif



      call wz_spinoru(6,qdks,za,zb)
      
c--   s returned from sprod (common block) is 2*dot product
c--   calculate propagators

c--   always use these propagators now
c      prop12=s(1,2)/dcmplx(s(1,2)-wmass**2,wmass*wwidth)
c      prop34=s(3,4)/dcmplx(s(3,4)-wmass**2,wmass*wwidth)
c      prop56=s(5,6)/dcmplx(s(5,6)-zmass**2,zmass*zwidth)
      prop12=wz_cpropfac(s(1,2),wmass,wwidth)
      prop34=wz_cpropfac(s(3,4),wmass,wwidth)
      prop56=wz_cpropfac(s(5,6),zmass,zwidth)

c--- apply a dipole form factor to anomalous couplings
      xfac=1d0/(1d0+s(1,2)/(tevscale*1d3)**2)**2
      xdelg1_z=xfac*delg1_z
      xdelg1_g=xfac*delg1_g
      xdelk_z=xfac*delk_z
      xdelk_g=xfac*delk_g
      xlambda_z=xfac*lambda_z
      xlambda_g=xfac*lambda_g


c---case dbar-u or ubar-d
      call wz_a6treeb_anom_wz(1,2,3,4,5,6,za,zb,A6b_1,A6b_2,A6b_3,A6b_4)
      Fb123456_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_3*2d0*(1d0+xdelg1_z) 
     .          +A6b_4*xlambda_z/wmass**2
      Fb123456_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_3*2d0*(1d0+xdelg1_g) 
     .          +A6b_4*xlambda_g/wmass**2
      Fa123456=wz_a6treea(1,2,3,4,5,6,za,zb)
      Fa126543=wz_a6treea(1,2,6,5,4,3,za,zb)

      call wz_a6treeb_anom_wz(1,2,3,4,6,5,za,zb,A6b_1,A6b_2,A6b_3,A6b_4)
      Fb123465_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_3*2d0*(1d0+xdelg1_z) 
     .          +A6b_4*xlambda_z/wmass**2
      Fb123465_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_3*2d0*(1d0+xdelg1_g) 
     .          +A6b_4*xlambda_g/wmass**2
      Fa123465=wz_a6treea(1,2,3,4,6,5,za,zb)
      Fa125643=wz_a6treea(1,2,5,6,4,3,za,zb)

c---case u-dbar or d-ubar
      call wz_a6treeb_anom_wz(2,1,3,4,5,6,za,zb,A6b_1,A6b_2,A6b_3,A6b_4)
      Fb213456_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_3*2d0*(1d0+xdelg1_z) 
     .          +A6b_4*xlambda_z/wmass**2
      Fb213456_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_3*2d0*(1d0+xdelg1_g) 
     .          +A6b_4*xlambda_g/wmass**2
      Fa213456=wz_a6treea(2,1,3,4,5,6,za,zb)
      Fa216543=wz_a6treea(2,1,6,5,4,3,za,zb)

      call wz_a6treeb_anom_wz(2,1,3,4,6,5,za,zb,A6b_1,A6b_2,A6b_3,A6b_4)
      Fb213465_z=A6b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A6b_3*2d0*(1d0+xdelg1_z) 
     .          +A6b_4*xlambda_z/wmass**2
      Fb213465_g=A6b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s(1,2)/wmass**2)
     .          +A6b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A6b_3*2d0*(1d0+xdelg1_g) 
     .          +A6b_4*xlambda_g/wmass**2
      Fa213465=wz_a6treea(2,1,3,4,6,5,za,zb)
      Fa215643=wz_a6treea(2,1,5,6,4,3,za,zb)

      if (.not.dronly) then
c---for supplementary diagrams.
      Fa346512=wz_a6treea(3,4,6,5,1,2,za,zb)
      Fa342156=wz_a6treea(3,4,2,1,5,6,za,zb)
      Fa652143=wz_a6treea(6,5,2,1,4,3,za,zb)
      Fa345612=wz_a6treea(3,4,5,6,1,2,za,zb)
      Fa342165=wz_a6treea(3,4,2,1,6,5,za,zb)
      Fa346521=wz_a6treea(3,4,6,5,2,1,za,zb)
      Fa341256=wz_a6treea(3,4,1,2,5,6,za,zb)
      Fa651243=wz_a6treea(6,5,1,2,4,3,za,zb)
      Fa345621=wz_a6treea(3,4,5,6,2,1,za,zb)
      Fa341265=wz_a6treea(3,4,1,2,6,5,za,zb)
      Fa653412=wz_a6treea(6,5,3,4,1,2,za,zb)
      Fa653421=wz_a6treea(6,5,3,4,2,1,za,zb)
      endif

c---set up left/right handed couplings for both Z and gamma*
c---note that L/R labels the LEPTON coupling v2, NOT the quarks (all L)
      do j=-nf,nf
        ZgL(j)=L(j)*v2(1)*prop56+Q(j)*q1           
        ZgR(j)=L(j)*v2(2)*prop56+Q(j)*q1           
      enddo
      
      do j=-nf,nf
      do k=-nf,nf
c--no point in wasting time if it gives zero anyway
      if (Vsq(j,k) .ne. 0d0) then
          if ((j .gt. 0) .and. (k .lt. 0)) then
            AWZM=(FAC*(ZgL(+j)*Fa213456+ZgL(-k)*Fa216543)
     .           +FACM*(v2(1)*cotw*prop56*Fb213456_z
     .                                +q1*Fb213456_g)*prop12)*prop34
            AWZP=(FAC*(ZgR(+j)*Fa213465+ZgR(-k)*Fa215643)
     .           +FACM*(v2(2)*cotw*prop56*Fb213465_z
     .                                +q1*Fb213465_g)*prop12)*prop34
          elseif ((j .lt. 0) .and. (k .gt. 0)) then
            AWZM=(FAC*(ZgL(+k)*Fa123456+ZgL(-j)*Fa126543)
     .           +FACM*(v2(1)*cotw*prop56*Fb123456_z
     .                                +q1*Fb123456_g)*prop12)*prop34
            AWZP=(FAC*(ZgR(+k)*Fa123465+ZgR(-j)*Fa125643)
     .           +FACM*(v2(2)*cotw*prop56*Fb123465_z
     .                                +q1*Fb123465_g)*prop12)*prop34
          endif
          if (.not.dronly) then
c---we need supplementary diagrams for gauge invariance.
c---now also assume that we have lepton decay products for W
c---so that v2(1)=le, v2(2)=re
c---1st term is diagram where Z couples to electron
c---2nd term is diagram where Z couples to neutrino
c---3rd term is diagram where gamma* couples to electron
c---4th term (l-h only) contains two W propagators
          if ((j .gt. 0) .and. (k .lt. 0)) then
             AWZM=AWZM+FAC*prop12*(
     .          (en1*Fa346512+en2*Fa342156)*v2(1)*prop56
     .          +q1*(-1d0)*(cl1*Fa346512+cl2*Fa342156)
     .          +wwflag*0.5d0/xw*prop34*(cl1*Fa652143+cl2*Fa653412))
            AWZP=AWZP+FAC*prop12*(
     .          (en1*Fa345612+en2*Fa342165)*v2(2)*prop56
     .          +q1*(-1d0)*(cl1*Fa345612+cl2*Fa342165))
          elseif ((j .lt. 0) .and. (k .gt. 0)) then
            AWZM=AWZM+FAC*prop12*(
     .          (en1*Fa346521+en2*Fa341256)*v2(1)*prop56
     .          +q1*(-1d0)*(cl1*Fa346521+cl2*Fa341256)
     .          +wwflag*0.5d0/xw*prop34*(cl1*Fa651243+cl2*Fa653421))
            AWZP=AWZP+FAC*prop12*(
     .          (en1*Fa345621+en2*Fa341265)*v2(2)*prop56
     .          +q1*(-1d0)*(cl1*Fa345621+cl2*Fa341265))
          endif
          endif

      msq(j,k)= msq(j,k)+Vsq(j,k)*ave*(abs(AWZM)**2+abs(AWZP)**2)


      if ((iloop.eq.1)) then
         if (interference) then
            AWZM_SAVE(j,k)=AWZM
         endif
      elseif (iloop.eq.2) then
         msq(j,k)=msq(j,k)-Vsq(j,k)*ave*(dconjg(AWZM)*AWZM_SAVE(j,k) +
     &           AWZM*dconjg(AWZM_SAVE(j,k)))
      endif


      endif

      enddo
      enddo


      enddo !-i loop 

      msq = msq*vsymfact

      return
      end
