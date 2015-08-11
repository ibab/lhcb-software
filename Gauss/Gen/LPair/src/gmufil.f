*cmz :          17/01/08            by a. shamov
* 1) add photos call for dimuon
*cmz :          30/12/07            by a. shamov
* 1) use real *8 internally
* 2) fill /hepevt/ common
*cmz :          04/08/95  20.04.35  by  zeus offline group
*cmz :  1.00/00 18/08/94  22.10.14  by  zeus offline group
*-- author :    zeus offline group   18/08/94
      subroutine gmufil
c
c   this subroutine should fill the gtr-bank
c
      implicit none
c
      real*8  pi2
      parameter (pi2=2*3.1415926535898)
*keep,inpu.
      real*8       me,mu,mp,mx,s,sq,pe,pp,ee,ep,const,pi, my
      common /inpu/me,mu,mp,mx,s,sq,pe,pp,ee,ep,const,pi, my
      save /inpu/

*kend.
      real*8         e,e1,e2,e3,e4,e5,p1,
     &                p3,p4,p5,ct3,st3,ct4,st4,ct5,
     &                st5,cp3,sp3,cp5,sp5
      common /variab/e,e1,e2,e3,e4,e5,p1,
     &                p3,p4,p5,ct3,st3,ct4,st4,ct5,
     &                st5,cp3,sp3,cp5,sp5
      real*8          e6,e7,p6,p7,ct6,st6,ct7,st7,cp6,sp6,cp7,sp7,w
      common /variad/ e6,e7,p6,p7,ct6,st6,ct7,st7,cp6,sp6,cp7,sp7,w
      real*8          s1,s2,t1,t2
      common /extra/  s1,s2,t1,t2
*keep,ltcom.
      real*8          gamma,betgam
      common /ltcom/  gamma,betgam
      save /variab/,/variad/,/extra/

*keep,xqcom.
      integer iusedf
      double precision xq,eq,pq,mq,
     &                 qscale,xdens(-6:2),
     &                 psea,pvald
      common /xqcom/   xq,eq,pq,mq,
     &                 qscale,xdens,
     &                 psea,pvald,iusedf

*keep,beam.
      integer          intge,intgp,gpdf,spdf,pmod,emod,ipair,nquark
      real*8           inpe,inpp
      common /beam/    inpe,inpp,intge,intgp,gpdf,spdf,pmod,emod,
     &                 ipair,nquark
      save /beam/

*keep,kinvar.
      real*8           gmux,gmuy,gmuw,gmunu
      common /kinvar/  gmux,gmuy,gmuw,gmunu
      save /kinvar/
*kend.
      integer nlimax
      parameter (nlimax=13)
      real*8 pl(4,nlimax),pmxda(4)
c
c   lund common <===================================================
      real*4        p(4000,5),v(4000,5)
      integer       n,k(4000,5)
      common/lujets/n,k,p,v
      save /lujets/
c
      include 'lpair_hepevt.inc'
c
      integer nlines
      real*8 plab(4,9)
      real*4 h1rn
      real*8 sinphi,cosphi,rany,ranudq,ranphi
c  information for jetset package <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      integer njoin
      parameter(njoin=2)
      integer jlpsf(njoin),jlval(njoin),jlsea1(njoin),jlsea2(njoin)
      data    jlpsf/10,11/
      data    jlval/2,7/
      data    jlsea1/4,9/
      data    jlsea2/2,3/
      logical lsea,lval
      save jlpsf,jlval,jlsea1,jlsea2
*keep,partic.
      integer i2stat(13),i2part(13),
     &        i2mo1(13),i2da1(13),i2da2(13),i2line
      common /partic/  i2line,i2stat,i2part,i2mo1,i2da1,i2da2
*kend.
c
      real*8  i2mass(nlimax)
      data    i2mass/nlimax*-9999.9/
c
      integer ncall,nfrac3,nterm3
      data ncall/0/
      data nfrac3/0/
      data nterm3/0/

      integer nextw
      common /gmufilcb/ nextw
      save /gmufilcb/

      save ncall,nfrac3,nterm3
      data nextw/1/
c
      real ulmass,gmuselx
c
      integer photoson
      real photomegamin
      common /photosonoff/ photoson,photomegamin
      data photoson/0/,photomegamin/2.e-3/
c
      real*8 phalpha,phxphcut
      common/phocop/phalpha,phxphcut
c
      integer kphot,imu1,imu2
      save kphot,imu1,imu2
      integer nreal(2,2)
      double precision preal(4,2,2)
      double precision pvirt(4,2),vq2(2),vq2old(2)
      double precision cosa,cosamx
      real demin,de
      integer mog,ipho,if,ihep,jhep,icmf,nhepold,lprint
c
      integer npout,ipdq,i,j,ninit,nfinal,nfracs,ipq
      real pmxp,ranmxp,ranmxt,dummy
      double precision ulmdq,ulmq
c     
      nfracs=0
c
      nlines=9
      nfinal=0
      ninit =0

      if(nextw.le.1) print *,'entering gmufil'
c
c lorenz tranformation and computing of intrmed particles <======
c particle 1 = "proton"    <==================
      plab(1,1)=0.0
      plab(2,1)=0.0
      plab(3,1)=gamma*p1 + betgam*e1
      plab(4,1)=gamma*e1 + betgam*p1
c particle 2 = elektron in <================
      plab(1,2)=0.0
      plab(2,2)=0.0
      plab(3,2)= -gamma*p1 + betgam*e2
      plab(4,2) =  gamma*e2 - betgam*p1
c particle 9 = elektron out <===============
      plab(1,9)=p5*st5*cp5
      plab(2,9)=p5*st5*sp5
      plab(3,9)=gamma*ct5*p5 + betgam  *  e5
      plab(4,9) =gamma  *  e5 + betgam*ct5*p5
c particle 4 = gamma_e   <==================
      plab(1,4)=-plab(1,9)
      plab(2,4)=-plab(2,9)
      plab(3,4)=plab(3,2)-plab(3,9)
      plab(4,4) =plab(4,2)-plab(4,9)
c particle 5 = quark out <==================
      plab(1,5)=p3*st3*cp3
      plab(2,5)=p3*st3*sp3
      plab(3,5)=gamma*ct3*p3 + betgam  *  e3
      plab(4,5) =gamma  *  e3 + betgam*ct3*p3
c particle 3 = gamma_p   <==================
      plab(1,3)=-plab(1,5)
      plab(2,3)=-plab(2,5)
      plab(3,3)=plab(3,1)-plab(3,5)
      plab(4,3) =plab(4,1)-plab(4,5)
c particle 6 = muon1    <==================
      plab(1,6)=p6*st6*cp6
      plab(2,6)=p6*st6*sp6
      plab(3,6)=gamma*ct6*p6 + betgam  *  e6
      plab(4,6) =gamma  *  e6 + betgam*ct6*p6
c particle 7 = muon1->2 <==================
      plab(1,7)=plab(1,3)-plab(1,6)
      plab(2,7)=plab(2,3)-plab(2,6)
      plab(3,7)=plab(3,3)-plab(3,6)
      plab(4,7) =plab(4,3)-plab(4,6)
c particle 8 = muon2     <==================
      plab(1,8)=p7*st7*cp7
      plab(2,8)=p7*st7*sp7
      plab(3,8)=gamma*ct7*p7 + betgam  *  e7
      plab(4,8) =gamma  *  e7 + betgam*ct7*p7
c====> set kinematic variables for gki   <============
       gmux= -t2 /(ep*plab(4,4)-pp*plab(3,4))/2.0d0
       gmuy= (ep*plab(4,4)-pp*plab(3,4))/
     &       (ee*plab(4,4)+pe*plab(3,4))
!       gmuw = (ep+plab(4,4))**2 - (pp+plab(3,4))**2
       gmuw =((ep-pp)+(plab(4,4)-plab(3,4)))*
     &                  ((ep+pp)+(plab(4,4)+plab(3,4)))
       if (gmuw .ge. 0) then
        gmuw=sqrt(gmuw)
       else
        write(6,*) ' gmufil : negativ w**2 computed : genw**2 = ', gmuw,
     &             ' genw is set to 0 '
        print *,'plab(1)',(plab(j,1),j=1,4)
        print *,'plab(9)',(plab(j,9),j=1,4)
        print *,'gamma',gamma,betgam
        print *,'p,e,s,c',p5,e5,st5,cp5
        gmuw=0.0
       endif
       gmunu= gmuy*2.0*ulmass(2212)/ep/ee
c===> random reflection at xz-plain <==================
      if (h1rn(dummy)  .ge. 0.5) then
       rany=-1.0
      else
       rany=1.0
      endif
c===> random rotation at z-axis <=====================
      ranphi=pi2*h1rn(dummy)
      sinphi=sin(ranphi)
      cosphi=cos(ranphi)
c====> rotate, refelect and transform to real*4 values <=============
      do 100 i=1,9
        pl(1,i) =  plab(1,i)*cosphi + rany*plab(2,i)*sinphi
        pl(2,i) = -plab(1,i)*sinphi + rany*plab(2,i)*cosphi
        pl(3,i) = sngl(plab(3,i))
        pl(4,i) = sngl(plab(4,i))
 100  continue
c===> random distribution of lepton+ and lepton- <===========
      if (h1rn(0.0) .lt. 0.5) then
       i2part(6) = ipair
       i2part(7) =-ipair
       i2part(8) =-ipair
      else
       i2part(6) =-ipair
       i2part(7) = ipair
       i2part(8) = ipair
      endif
c===> selection of hadron mode in parton model <================
      lval=.false.
      lsea=.false.
      if (pmod .eq. 101) then
         lval=.true.
      elseif (pmod .eq. 102) then
         lsea=.true.
      elseif (pmod .eq. 103) then
         if (h1rn(dummy) .gt. psea) then
            lval=.true.
         else
            lsea=.true.
         endif
      endif
c===> add. particles for val.quark scatering in parton model <=====
      if (lval) then
       ninit =2
c===> add initial proton <====
       i2stat(10)=21
       i2part(10)=2212
       i2mo1(10)=0
       i2da1(10)=2
       i2da2(10)=3
c====> correct part 1 <===
       i2mo1(1)=10
c====> add diquark  <====
       i2stat(11)=1
       i2mo1(11)=1
       i2da1(11)=0
       i2da2(11)=0
c===> random selection of u and d quarks <==
       ranudq=h1rn(dummy)
       if (ranudq .lt. pvald) then
        i2part(1)=1
        i2part(11)=2203
        i2part(5)=1
       elseif (ranudq .lt. 0.5+0.5*pvald) then
        i2part(1)=2
        i2part(11)=2101
        i2part(5)=2
       else
        i2part(1)=2
        i2part(11)=2103
        i2part(5)=2
       endif
       iusedf=i2part(1)
c==> set masses <=============
       i2mass(1)=mq
       i2mass(11)=ulmass(i2part(11))
       i2mass(10)=ulmass(2212)
c==> set momenta <============
       pl(1,10)=0.0
       pl(2,10)=0.0
       pl(3,10)=sngl(pp)
       pl(4,10)=sngl(ep)
       pl(1,11)=0.0
       pl(2,11)=0.0
       pl(3,11)=sngl(pp-pq)
       pl(4,11)=pl(4,10)-pl(4,1)
c???> computed energy may be in conflict to mass !!!!  ????????
c
c===> add. particles for sea quark scatering in parton model <=====
      elseif (lsea) then
       ninit =4
c===> add initial proton <====
       i2stat(10)=21
       i2part(10)=2212
       i2mo1(10)=0
       i2da1(10)=2
       i2da2(10)=5
c====> add part code for add particles <======
       do 234 i=11,13
          i2da1(i)=0
          i2da2(i)=0
          i2mo1(i)=1
          i2stat(i)=1
 234   continue
c====> correct part 1<===
       i2mo1(1)=1
c===> set scattered quark and his anti quark <==
       if (nquark .ne. 12) then
          i2part(1)=nquark
       else
          if (h1rn(dummy) .lt. 0.2) then
             i2part(1)=1
          else
             i2part(1)=2
          endif
       endif
       iusedf=i2part(1)
       if (h1rn(dummy) .le. 0.5) then
          i2part(12)=-i2part(1)
       else
          i2part(12)=i2part(1)
          i2part(1)=-i2part(1)
       endif
       i2part(5)=i2part(1)
c===> add quark and diquark from p <==
       if (i2part(1) .lt. 0) then
         ipq=13
         ipdq=11
       else
         ipq=11
         ipdq=13
       endif
       ranudq=h1rn(dummy)
       if (ranudq .lt. 1.0/3.0) then
        i2part(ipq)=1
        i2part(ipdq)=2203
       elseif (ranudq .lt. 2.0/3.0) then
        i2part(ipq)=2
        i2part(ipdq)=2101
       else
        i2part(ipq)=2
        i2part(ipdq)=2103
       endif
c==> set masses <=============
       i2mass(1)=mq
       i2mass(5)=mq
       i2mass(12)=mq
       i2mass(13)=ulmass(i2part(13))
       i2mass(11)=ulmass(i2part(11))
       i2mass(10)=ulmass(2212)
c==> set momenta <=============
       pl(1,10)=0.0
       pl(2,10)=0.0
       pl(3,10)=sngl(pp)
       pl(4,10)=sngl(ep)
       pl(1,12)=0.0
       pl(2,12)=0.0
       pl(3,12)=gmuselx(-iabs(iusedf),qscale)*pp
       pl(4,12)=sqrt(pl(3,12)**2+i2mass(12)**2)
       pl(1,ipq)=0.0
       pl(2,ipq)=0.0
       pl(3,ipq)=gmuselx(i2part(ipq),qscale)*pp
       pl(4,ipq)=sqrt(pl(3,ipq)**2+i2mass(ipq)**2)
       pl(1,ipdq)=0.0
       pl(2,ipdq)=0.0
       pl(3,ipdq)=pl(3,10)-pl(3,1)-pl(3,ipq)-pl(3,12)
       pl(4,ipdq)=pl(4,10)-pl(4,1)-pl(4,ipq)-pl(4,12)
      endif
c
c    for inelastic mode with structure functions build <========
c    hadronic system using lund shower mc.
c
      if (emod.ge.10 .and. emod.le.99) then
        i2mass(9)=my
      endif
c
      if (pmod.ge.10 .and. pmod.le.99) then
       nfinal=2
c====> insert the mass of the hadronic system <==================
       i2mass(5)=mx
c===> random selection of u , d and di quarks <===========
       ranudq=h1rn(dummy)
       if (ranudq .lt. 1.0/9.0) then
        i2part(10)=1
        i2part(11)=2203
       elseif (ranudq .lt. 5.0/9.0) then
        i2part(10)=2
        i2part(11)=2101
       else
        i2part(10)=2
        i2part(11)=2103
       endif
       ulmdq=ulmass(i2part(11))
       ulmq =ulmass(i2part(10))
c====> set of lund codes <====================================
       i2mo1(10)=5
       i2da1(10)=0
       i2da2(10)=0
       i2stat(10)=1
       i2mo1(11)=5
       i2da1(11)=0
       i2da2(11)=0
       i2stat(11)=1
c====> choose random direction in mx frame <===================
       ranmxp=pi2*h1rn(dummy)
       ranmxt=acos(2.0*h1rn(dummy)-1.0)
c====> compute momentum of decay particle from mx <=============
       pmxp=dsqrt((mx**2-ulmdq**2+ulmq**2)**2/4.0/mx/mx - ulmq**2 )
c=====> build 4-vectors and boost decay particles <===============
       pmxda(1)=sin(ranmxt)*cos(ranmxp)*pmxp
       pmxda(2)=sin(ranmxt)*sin(ranmxp)*pmxp
       pmxda(3)=cos(ranmxt)*pmxp
       pmxda(4)=sqrt(pmxp**2+ulmdq**2)
       call lorenb8(i2mass(5),pl(1,5),pmxda(1),pl(1,11))
       pmxda(1)=-pmxda(1)
       pmxda(2)=-pmxda(2)
       pmxda(3)=-pmxda(3)
       pmxda(4)=sqrt(pmxp**2+ulmq**2)
*       write(6,*) ' gmufil : e of quark and diquark befor lb:'
*     &   ,pmxda(4)+edqpr
       call lorenb8(i2mass(5),pl(1,5),pmxda(1),pl(1,10))
      endif
c====> prepare the lunt common <================================
 10   continue
      call lunset_hep(nlines+ninit+nfinal)
c ====> filling the lund common <================================
      do 200 i=1+nlines,ninit+nlines
c set mother/daugter values, marking particles as decayed <=======
        call lukset_hep(i-nlines,i2stat(i),i2part(i),
     &       i2mo1(i),i2da1(i),i2da2(i),0)
c set puls, energy and mass off the particles <==================
        call lupset_hep(i-nlines,pl(1,i),pl(2,i),pl(3,i),
     &       pl(4,i),i2mass(i))
 200  continue
      do 201 i=1,nlines+nfinal
c set mother/daugter values, marking particles as decayed <=======
        call lukset_hep(i+ninit,i2stat(i),i2part(i),
     &       i2mo1(i),i2da1(i),i2da2(i),ninit)
c set puls, energy and mass off the particles <==================
        call lupset_hep(i+ninit,pl(1,i),pl(2,i),pl(3,i),
     &       pl(4,i),i2mass(i))
 201  continue
c putting quark and diquark to a color singlet <======================
      if (lval) call lujoin(njoin,jlval)
      if (lsea) then
         call lujoin(njoin,jlsea1)
         call lujoin(njoin,jlsea2)
      endif
c
c using pythia's model or lpair's own model
      if (pmod.ge.10 .and. pmod.le.19 .or.
     &          emod.ge.10 .and. emod.le.19) then
         n=9
         if(pmod.ge.10.and.pmod.le.19) then
            call zmdiff(5)
         endif
         if(emod.ge.10.and.emod.le.19) then
            call zmdiff(9)
         endif
      else
         if(pmod.gt.2.or.emod.gt.2) then
            call lujoin(njoin,jlpsf)
         endif
      endif
c
c execute lund fragmentation program  <==============================
      if(nextw.le.1) print *,'calling luexec'
      call luexec
c check wether the hadronic system is inelastic  <===================
      if (pmod.ge.10 .and. pmod.le.99) then
        npout=0
        do 300 i=1,n
          if (k(i,1) .eq. 1) npout=npout+1
 300    continue
        nfrac3=nfrac3+1
        nfracs=nfracs+1
        if (npout .eq. 4 .and. nfracs .le. 1000) goto 10
        if (nfracs .gt. 1000) nterm3=nterm3+1
      endif
      ncall=ncall+1
      if(photoson.lt.-10) then
         lprint=1
      else
         lprint=0
      endif
      if (ncall .ge. nextw) then
       lprint=1
       if (pmod.ge.10 .and. pmod.le.99) then
        write(6,*) ' gmufil : number of calls is ',ncall
     &  ,'  pmod 10-99:  # frac try :',nfrac3,'  # frac term :',nterm3
       else
        write(6,*) ' gmufil : number of calls is ',ncall
       endif
       nextw=nextw*2
      endif
c
c   call photos to simulate radiation off muons
      if(photoson.ne.0) then
         if(photoson.gt.0) then
            print *,'------------------------- '
            print *,'photos initialization'
            call phoini
            print *,'don''t bother about xphcut'
            print *,'--------------------------'
            kphot=mod(photoson,10)
            imu1=6
            imu2=8
            photoson=-photoson
         endif
         nhepold=nhep
c     photos for muons
         if(photoson.lt.-10.and.lprint.ne.0) then
            print *
            print *,'event',ncall,'-----------------------'
         endif
         if(photoson.lt.-10.and.lprint.ne.0) then
            print *,'mu-in ',imu1,': p',(phep(j,imu1),j=1,5)
            print *,'mu-in ',imu2,': p',(phep(j,imu2),j=1,5)
         endif
         if(kphot.ne.2) then
            nhep=nhep+1         ! photon cms
            icmf=nhep
            do i=1,4
               phep(i,icmf)=phep(i,imu1)+phep(i,imu2)
               vhep(i,icmf)=0
            enddo
            phep(5,icmf)=sqrt((phep(4,icmf)**2-
     &           phep(3,icmf)**2)-(phep(1,icmf)**2+
     &           phep(2,icmf)**2))
            jmohep(1,icmf)=6
            jmohep(2,icmf)=8
            idhep(icmf)=36
            isthep(icmf)=21
            jdahep(1,icmf)=icmf+1
            jdahep(2,icmf)=icmf+2
            if(photoson.lt.-10.and.lprint.ne.0) then
               print *,'cmf',icmf,' mu',imu1,imu2
               print *,'cmf ',icmf,': p',(phep(j,icmf),j=1,5)
            endif
            do ihep=imu1,imu2,imu2-imu1 ! muons
               k(ihep,1)=21
               isthep(ihep)=21
               nhep=nhep+1
               do i=1,4
                  phep(i,nhep)=phep(i,ihep)
                  vhep(i,nhep)=0
               enddo
               phep(5,nhep)=phep(5,ihep)
               idhep(nhep)=idhep(ihep)
               isthep(nhep)=1
               jmohep(1,nhep)=icmf
               jmohep(2,nhep)=0
               jdahep(1,nhep)=0
               jdahep(2,nhep)=0
            enddo
            phxphcut=photomegamin/max(phep(4,imu1),phep(4,imu2))
            call photos(-icmf)
            if(photoson.lt.-10.and.lprint.ne.0) then
               do ihep=icmf+1,icmf+2
                  print *,'mu-out',ihep,': p',(phep(j,ihep),j=1,5)
               enddo
               do ihep=icmf+3,nhep
                  print *,'new',ihep,' kf',idhep(ihep),
     &                 ': p',(phep(j,ihep),j=1,5)
               enddo
            endif
         endif
c      photos for protons (just check)
         if(kphot.ge.2) then
            do 9090 i=1,2       ! protons
               do if=1,2        ! clear real photon data
                  nreal(if,i)=0
                  do j=1,4
                     preal(j,if,i)=0
                  enddo
               enddo
               icmf=nhep+1
               do j=1,5         ! initial proton -> antiproton
                  phep(j,icmf+1)=phep(j,i)
               enddo
               phep(4,icmf+1)=sqrt((phep(1,icmf+1)**2+
     &              phep(2,icmf+1)**2+phep(5,icmf+1)**2)+
     &              phep(3,icmf+1)**2)
               jmohep(1,icmf+1)=icmf
               jmohep(2,icmf+1)=0
               jdahep(1,icmf+1)=0
               jdahep(2,icmf+1)=0
               idhep(icmf+1)=-idhep(i)
               isthep(icmf+1)=1
               if(photoson.lt.-10.and.lprint.ne.0) then
                  print *,'p-in ',icmf+1,': p',
     &                 (phep(j,icmf+1),j=1,5),
     &                 ' d',phep(4,icmf+1)-abs(phep(3,icmf+1))
               endif
c
               do j=1,5         ! final proton
                  phep(j,icmf+2)=phep(j,jdahep(2,i))
               enddo
               phep(4,icmf+2)=sqrt((phep(1,icmf+2)**2+
     &              phep(2,icmf+2)**2+phep(5,icmf+2)**2)+
     &              phep(3,icmf+2)**2)
               jmohep(1,icmf+2)=icmf
               jmohep(2,icmf+2)=0
               jdahep(1,icmf+2)=0
               jdahep(2,icmf+2)=0
               idhep(icmf+2)=idhep(jdahep(2,i))
               isthep(icmf+2)=1
               if(photoson.lt.-10.and.lprint.ne.0) then
                  print *,'p-in ',icmf+2,': p',
     &                 (phep(j,icmf+2),j=1,5),
     &                 ' d',phep(4,icmf+2)-abs(phep(3,icmf+2))
               endif
c
               do j=1,4         ! cmf
                  phep(j,icmf)=phep(j,icmf+1)+
     &                 phep(j,icmf+2)
               enddo
               phep(5,icmf)=sqrt((phep(4,icmf)-phep(3,icmf))*
     *              (phep(4,icmf)+phep(3,icmf))-
     &              (phep(1,icmf)**2+
     &              phep(2,icmf)**2))
               jmohep(1,icmf)=0
               jmohep(2,icmf)=0
               jdahep(1,icmf)=icmf+1
               jdahep(2,icmf)=icmf+2
               idhep(icmf)=36
               isthep(icmf)=21
               if(photoson.lt.-10.and.lprint.ne.0) then
                  print *,'cmf  ',icmf,': p',(phep(j,icmf),j=1,5)
               endif
               nhep=icmf+2
c
               phxphcut=photomegamin/max(ee,ep)
               call photos(-icmf)
               if(nhep.lt.icmf+3) then ! no photons
                  if(photoson.lt.-10.and.lprint.ne.0) then
                     print *,'photos, nhep',nhep,' icmf',icmf
                  endif
                  nhep=icmf-1
                  goto 9090
               endif
               do ihep=icmf+1,nhep ! check final particles
                  if(phep(1,ihep).lt.0.d0) then
                  elseif(phep(1,ihep).ge.0.d0) then
                  else          ! nan
                     print *,'photos error: ihep',ihep,' id',
     &                    idhep(ihep),' p',(phep(j,ihep),j=1,4)
                     goto 9090
                  endif
               enddo
               if(photoson.lt.-10.and.lprint.ne.0) then
                  print *,'photos, nhep',nhep
                  do ihep=icmf+1,icmf+2
                     print *,'p-out',ihep,': p',(phep(j,ihep),j=1,5)
                  enddo
                  do ihep=icmf+3,nhep
                     print *,'new',ihep,' kf',idhep(ihep),
     &                    ': p',(phep(j,ihep),j=1,5)
                  enddo
               endif
               do jhep=icmf+3,nhep ! check photons
                  demin=1.e10
                  cosamx=-2.
                  do ihep=icmf+1,icmf+2
                     isthep(ihep)=21
                     cosa=(phep(1,jhep)*phep(1,ihep)+
     &                    phep(2,jhep)*phep(2,ihep)+
     &                    phep(3,jhep)*phep(3,ihep))/
     &                    phep(4,jhep)/sqrt(phep(1,ihep)**2+
     &                    phep(2,ihep)**2+phep(3,ihep)**2)
                     de=phep(4,jhep)+phep(4,ihep)-phep(4,i)
                     if(photoson.lt.-10.and.lprint.ne.0) then
                        print *,'de',de,' cosa',cosa,' ij',ihep,jhep
                     endif
                     if(cosamx.lt.cosa) then
                        mog=ihep
                        cosamx=cosa
                        demin=abs(de)
                     endif
                  enddo         ! i/f photons
                  if=mog-icmf   ! initial/final
                  if(photoson.lt.-10.and.lprint.ne.0) then
                     print *,'emitter',mog,' cosamx',cosamx,
     &                    ' i/f',if
                  endif
                  nreal(if,i)=nreal(if,i)+1
                  do j=1,4
                     preal(j,if,i)=preal(j,if,i)+phep(j,jhep)
                  enddo
               enddo            ! real photons
               if(nreal(1,i).ne.0) then ! initial state radiation
                  ihep=jdahep(2,i) ! modify final proton momentum
                  do j=1,4
                     phep(j,ihep)=phep(j,ihep)-preal(j,1,i)
                  enddo
                  do ihep=imu1,imu2,imu2-imu1 ! modify muon momenta
                     do j=1,4
                        phep(j,ihep)=phep(j,ihep)-preal(j,1,i)
                     enddo
                     if(photoson.lt.-10.and.lprint.ne.0) then
                        print *,'mu-out',ihep,': p',
     &                       (phep(j,ihep),j=1,5)
                     endif
                  enddo
               endif
               if(nreal(2,i).ne.0) then ! final state radiation
                  ihep=jdahep(2,i) ! modify final proton momentum
                  do j=1,4
                     phep(j,ihep)=phep(j,ihep)-preal(j,if,i)
                  enddo
               endif
               do j=1,4
                  pvirt(j,i)=(phep(j,i)-preal(j,1,i))-
     &                 (phep(j,jdahep(2,i))+preal(j,2,i))
               enddo
               ipho=2+i
               vq2(i)=-((pvirt(4,i)-pvirt(3,i))*
     &              (pvirt(4,i)+pvirt(3,i))-
     &              (pvirt(1,i)**2+pvirt(2,i)**2))
               vq2old(i)=-((phep(4,ipho)-phep(3,ipho))*
     &              (phep(4,ipho)+phep(3,ipho))-
     &              (phep(1,ipho)**2+phep(2,ipho)**2))
               if(photoson.lt.-10.and.lprint.ne.0) then
                  print *,'pvirt',(pvirt(j,i),j=1,4),
     &                 ' d',pvirt(4,i)-abs(pvirt(3,i))
                  print *,'pvold',(phep(j,ipho),j=1,4),
     &                 ' d',phep(4,ipho)-abs(phep(3,ipho))
                  print *,'q2',vq2(i),' old',vq2old(i),
     &                 ' ipho',ipho
               endif
 9090       enddo               ! i, proton
         endif
c      update /lujets/
         do i=nhepold+1,nhep
            k(i,1)=isthep(i)
            k(i,2)=idhep(i)
            k(i,3)=jmohep(1,i)
            k(i,4)=jdahep(1,i)
            k(i,5)=jdahep(2,i)
            do j=1,5
               v(i,j)=0
               p(i,j)=phep(j,i)
            enddo
            n=nhep
         enddo
      endif
c
c
      end
