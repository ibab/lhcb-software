      subroutine wz_qqb_wz_v(p,msqv)
C----Author R.K.Ellis December 1998
C----modified by JMC to include supplementary diagrams February 1999
c---  calculate the virtual matrix element squared and endpoint 
c     subtraction terms for the process
C For wz_nwz=+1
c     u(-p1)+dbar(-p2)-->mu^-(p5)+mu^+(p6)+n(p3)+e^+(p4)
C For wz_nwz=-1
c     d(-p1)+ubar(-p2)-->mu^-(p5)+mu^+(p6)+e^-(p3)+nbar(p4)
c---  averaged(summed) over initial(final) colours and spins
      implicit none
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/qcdcouple.h'
      include 'src/Lib/wz/zwcouple.h'
      include 'src/Lib/wz/ckm.h'
      include 'src/Lib/wz/zprods_decl.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/ewcharge.h'
      include 'src/Lib/wz/anomcoup.h'
      include 'src/Lib/wz/nwz.h'
      include 'src/Lib/wz/pwhg_st.h'
      include 'src/Lib/wz/pwhg_math.h'
      include 'src/Lib/wz/vvsettings.h'
      integer j,k
      double precision p(mxpart,4),qdks(mxpart,4)
      double precision msqv(-nf:nf,-nf:nf),msq(-nf:nf,-nf:nf)
      double precision facnlo,ave,cotw
      double precision FAC,FACM
      double complex AWZM,AWZP,BWZM,BWZP,wz_Vpole,Vpole12,suppl
      double complex prop12,prop34,prop56
      double complex props,propw,propz,A6b_1,A6b_2,A6b_3,A6b_4
      double complex wz_a6treea,wz_a6loopa
      double complex Fa123456,Fa126543,Fb123456_z,Fb123456_g
      double complex Fa123465,Fa125643,Fb123465_z,Fb123465_g
      double complex Fa213456,Fa216543,Fb213456_z,Fb213456_g
      double complex Fa213465,Fa215643,Fb213465_z,Fb213465_g
      double complex La123456,La126543,Lb123456_z,Lb123456_g
      double complex La123465,La125643,Lb123465_z,Lb123465_g
      double complex La213456,La216543,Lb213456_z,Lb213456_g
      double complex La213465,La215643,Lb213465_z,Lb213465_g
      double complex Fa346512,Fa342156,Fa652143
      double complex Fa345612,Fa342165,Fa653421
      double complex Fa346521,Fa341256,Fa651243
      double complex Fa345621,Fa341265,Fa653412
c      double complex Fa561243,Fa562143
      double precision v2(2),cl1,cl2,en1,en2,xfac
      double complex ZgL(-nf:nf),ZgR(-nf:nf)
      parameter(ave=0.25d0/xn)
      data cl1,cl2,en1,en2/4*1d0/
      include 'src/Lib/wz/cvecbos.h'

      integer iloop,nloop
      double complex AWZM_SAVE(-nf:nf,-nf:nf)
      double complex BWZM_SAVE(-nf:nf,-nf:nf)
      double complex wz_cpropfac
      external wz_cpropfac

!      scheme='dred'

      !TM make sure this is set here
      ason2pi = st_alpha/2d0/pi

      FAC=-2D0*gwsq*esq
      FACNLO=ason2pi*cf

      if (wz_nwz.eq.1) then
      FACM=FAC
      elseif (wz_nwz.eq.-1) then
      FACM=-FAC
      else
      write(6,*) 'wz_nwz ne +1 or -1'
      stop
      endif 
      if     (wz_nwz.eq.-1) then
        cl1=1d0
        cl2=0d0
        en1=le
        en2=ln
      elseif (wz_nwz.eq.+1) then
        cl1=0d0
        cl2=1d0
        en1=ln
        en2=le
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
      msqv(j,k)=0d0
      AWZM_SAVE(j,k) = 0d0
      BWZM_SAVE(j,k) = 0d0
      enddo
      enddo

c---calculate the lowest order matrix element
!      call wz_qqb_wz(p,msq)


      if (interference) then
         nloop=2
      else
         nloop=1
      endif

      do iloop=1,nloop

c---Change the momenta to DKS notation 
c   We have --- d(-p1)+ubar(-p2)-->nu(p6)+e^+(p7)+mu^-(p4)+mu^+(p5)
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

c---case dbar-u
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

c--- loop for a6b is simply tree*wz_Vpole
      Vpole12=wz_Vpole(s(1,2))

      Lb123456_z=Vpole12*Fb123456_z
      Lb123456_g=Vpole12*Fb123456_g
      La123456=wz_a6loopa(1,2,3,4,5,6,za,zb)
      La126543=wz_a6loopa(1,2,6,5,4,3,za,zb)

      Lb123465_z=Vpole12*Fb123465_z
      Lb123465_g=Vpole12*Fb123465_g
      La123465=wz_a6loopa(1,2,3,4,6,5,za,zb)
      La125643=wz_a6loopa(1,2,5,6,4,3,za,zb)

c---case u-dbar
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

      Lb213456_z=Vpole12*Fb213456_z
      Lb213456_g=Vpole12*Fb213456_g
      La213456=wz_a6loopa(2,1,3,4,5,6,za,zb)
      La216543=wz_a6loopa(2,1,6,5,4,3,za,zb)

      Lb213465_z=Vpole12*Fb213465_z
      Lb213465_g=Vpole12*Fb213465_g
      La213465=wz_a6loopa(2,1,3,4,6,5,za,zb)
      La215643=wz_a6loopa(2,1,5,6,4,3,za,zb)

      if (.not.dronly) then
c---for supplementary diagrams.
      Fa346512=wz_a6treea(3,4,6,5,1,2,za,zb)
      Fa342156=wz_a6treea(3,4,2,1,5,6,za,zb)
      Fa652143=wz_a6treea(6,5,2,1,4,3,za,zb)
      Fa345612=wz_a6treea(3,4,5,6,1,2,za,zb)
      Fa342165=wz_a6treea(3,4,2,1,6,5,za,zb)
c      Fa562143=wz_a6treea(5,6,2,1,4,3,za,zb)
      Fa346521=wz_a6treea(3,4,6,5,2,1,za,zb)
      Fa341256=wz_a6treea(3,4,1,2,5,6,za,zb)
      Fa651243=wz_a6treea(6,5,1,2,4,3,za,zb)
      Fa345621=wz_a6treea(3,4,5,6,2,1,za,zb)
      Fa341265=wz_a6treea(3,4,1,2,6,5,za,zb)
c      Fa561243=wz_a6treea(5,6,1,2,4,3,za,zb)
      Fa653412=wz_a6treea(6,5,3,4,1,2,za,zb)
      Fa653421=wz_a6treea(6,5,3,4,2,1,za,zb)
c---loop diagrams just tree*wz_Vpole since they're all triangle-type
      Vpole12=wz_Vpole(s(1,2))
      endif

c---set up left/right handed couplings for both Z and gamma*
c---note that L/R labels the LEPTON coupling v2, NOT the quarks (all L)
      do j=-nf,nf
        ZgL(j)=L(j)*v2(1)*prop56+Q(j)*q1           
        ZgR(j)=L(j)*v2(2)*prop56+Q(j)*q1           
      enddo
      
      do j=-nf,nf
      do k=-nf,nf
      if (Vsq(j,k) .ne. 0d0) then
        if     ((j .gt. 0) .and. (k .lt. 0)) then
            AWZM=(FAC*(ZgL(+j)*Fa213456+ZgL(-k)*Fa216543)
     .           +FACM*(v2(1)*cotw*prop56*Fb213456_z
     .                                +q1*Fb213456_g)*prop12)*prop34
            AWZP=(FAC*(ZgR(+j)*Fa213465+ZgR(-k)*Fa215643)
     .           +FACM*(v2(2)*cotw*prop56*Fb213465_z
     .                                +q1*Fb213465_g)*prop12)*prop34
            BWZM=(FAC*(ZgL(+j)*La213456+ZgL(-k)*La216543)
     .           +FACM*(v2(1)*cotw*prop56*Lb213456_z
     .                                +q1*Lb213456_g)*prop12)*prop34
            BWZP=(FAC*(ZgR(+j)*La213465+ZgR(-k)*La215643)
     .           +FACM*(v2(2)*cotw*prop56*Lb213465_z
     .                                +q1*Lb213465_g)*prop12)*prop34
        elseif ((j .lt. 0) .and. (k .gt. 0)) then
            AWZM=(FAC*(ZgL(+k)*Fa123456+ZgL(-j)*Fa126543)
     .           +FACM*(v2(1)*cotw*prop56*Fb123456_z
     .                                +q1*Fb123456_g)*prop12)*prop34
            AWZP=(FAC*(ZgR(+k)*Fa123465+ZgR(-j)*Fa125643)
     .           +FACM*(v2(2)*cotw*prop56*Fb123465_z
     .                                +q1*Fb123465_g)*prop12)*prop34
            BWZM=(FAC*(ZgL(+k)*La123456+ZgL(-j)*La126543)
     .           +FACM*(v2(1)*cotw*prop56*Lb123456_z
     .                                +q1*Lb123456_g)*prop12)*prop34
            BWZP=(FAC*(ZgR(+k)*La123465+ZgR(-j)*La125643)
     .           +FACM*(v2(2)*cotw*prop56*Lb123465_z
     .                                +q1*Lb123465_g)*prop12)*prop34
        endif
        if (.not.dronly) then
c---we need supplementary diagrams for gauge invariance.
c---now also assume that we have lepton decay products for W
c---so that v2(1)=le, v2(2)=re
c---1st term is diagram where Z couples to electron
c---2nd term is diagram where Z couples to neutrino
c---3rd term is diagram where gamma* couples to electron
c---4th term (l-h only) contains two W propagators
        if     ((j .gt. 0) .and. (k .lt. 0)) then
            suppl=FAC*prop12*(
     .          (en1*Fa346512+en2*Fa342156)*v2(1)*prop56
     .          +q1**2*(cl1*Fa346512+cl2*Fa342156)
     .          +0.5d0/xw*prop34*(cl1*Fa652143+cl2*Fa653412))
            AWZM=AWZM+suppl
            BWZM=BWZM+suppl*Vpole12
            suppl=FAC*prop12*(
     .          (en1*Fa345612+en2*Fa342165)*v2(2)*prop56
     .          +q1**2*(cl1*Fa345612+cl2*Fa342165))
            AWZP=AWZP+suppl
            BWZP=BWZP+suppl*Vpole12
        elseif ((j .lt. 0) .and. (k .gt. 0)) then
            suppl=FAC*prop12*(
     .          (en1*Fa346521+en2*Fa341256)*v2(1)*prop56
     .          +q1**2*(cl1*Fa346521+cl2*Fa341256)
     .          +0.5d0/xw*prop34*(cl1*Fa651243+cl2*Fa653421))
            AWZM=AWZM+suppl
            BWZM=BWZM+suppl*Vpole12
            suppl=FAC*prop12*(
     .          (en1*Fa345621+en2*Fa341265)*v2(2)*prop56
     .          +q1**2*(cl1*Fa345621+cl2*Fa341265))
            AWZP=AWZP+suppl
            BWZP=BWZP+suppl*Vpole12
        endif
        endif
      
      msqv(j,k)=msqv(j,k) + FACNLO*Vsq(j,k)*ave*2d0*
     .  dble(dconjg(AWZM)*BWZM+dconjg(AWZP)*BWZP)

      if (iloop.eq.1) then
         if (interference) then
            AWZM_SAVE(j,k)=AWZM
            BWZM_SAVE(j,k)=BWZM
         endif
      elseif (iloop.eq.2) then
         msqv(j,k)=msqv(j,k)-FACNLO*Vsq(j,k)*ave*(
     .   dconjg(AWZM_SAVE(j,k))*BWZM +
     .   AWZM_SAVE(j,k)*dconjg(BWZM) + 
     .   dconjg(AWZM)*BWZM_SAVE(j,k) +
     .   AWZM*dconjg(BWZM_SAVE(j,k)))
      endif
            



      endif

      enddo
      enddo

      enddo !iloop
      
      msqv = msqv*vsymfact

        return
      end

