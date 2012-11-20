      
      subroutine hvq_ttdecay
c From a : parton parton -> t tbar
c      or: parton parton -> t tbar parton
c event on the Les Houches interface, add the t-tbar decay
c products on the Les Houches interface.
c The products of W decays are decided by the subroutine
c hvq_pickwdecays(iwp1,mdecwp1,iwp2,mdecwp2,iwm1,mdecwm1,iwm2,mdecwm2)
c that returns the pdg codes and masses of the decay products of the w+
c and w-.
      implicit none
      include 'src/Lib/hvq/LesHouches.h'
c Momenta in our (p1,p2,p3,E) and Madvent (E,p1,p2,p3) convention
      real * 8 pp(4,9),pps(4,9),ppss(4,9),pme(0:3,9)
c Madevent parameters
      include 'src/Lib/hvq/MEcoupl.inc'
      real * 8 beta(3),betacm(3)
      integer itop,itbr,ijet,j,k,iwp,iwp1,iwp2,iwm,iwm1,iwm2
      parameter (itop=3,itbr=4,ijet=5)
      real * 8 mdecwp1,mdecwp2,mdecwm1,mdecwm2
      real * 8 xm2,s,q1q,tk,uk,q2q,w1h,w2h,x,y
      real * 8 tpvirt2,tmvirt2,wpvirt2,ebp,wmvirt2,ebm,res,resmnr,rat,
     #         cth2,ttenmnr,cmenmnr,krtop2,kpair,tten,cmen
      real * 8 xboundb
      character * 2 proc
      real * 8 boundnorm,ubound
      integer ini
      data ini/0/
      data boundnorm,ubound/8d0,0d0/
      save ini,boundnorm,ubound
      real * 8 hvq_dotprod,hvq_random,hvq_FGG,hvq_FQQ,hvq_FQG,hvq_ggborn,hvq_qqborn,hvq_powheginput
      external hvq_dotprod,hvq_random,hvq_FGG,hvq_FQQ,hvq_FQG,hvq_ggborn,hvq_qqborn,hvq_powheginput
      real * 8 topdecamp,wdecamp,tvirt2,wvirt2
      real * 8 pi
      parameter (pi=3.141592653589793d0)
      real * 8 dummy
c spin summmed and averaged amplitude squared for top decay
      topdecamp(tvirt2,wvirt2)=-(2*wvirt2**2-tvirt2*wvirt2
     # -bmass**2*wvirt2-tvirt2**2+2*bmass**2*tvirt2-bmass**4)/wvirt2
     #    *4*pi*alpha/(2*sin2w)
     #    /2
c spin summmed and averaged amplitude squared for W decay
      wdecamp(wvirt2)=(2*wvirt2) / 3 * 4*pi*alpha/(2*sin2w)
      if(ini.eq.2) then
         return
      elseif(ini.eq.0) then
         if(hvq_powheginput('#topdecaymode').eq.-1d6) then
            ini=2
            return
         endif
         if(hvq_powheginput('topdecaymode').eq.0d0) then
            ini=2
            return
         endif
         ini=1
         call hvq_madeventinput
      endif
c Compute MNR cross section for normalization
c now compute the MNR invariants; we need to do this after permutations
      do j=1,4
         do k=1,nup
            pp(j,k)=pup(j,k)
c pps shall not change from now on, in case one need to recompute
c things because of hit and miss
            pps(j,k)=pup(j,k)
         enddo
      enddo
      call hvq_momcons(2,pp,nup-2,pp(1,3))
c exchange momenta in pp to put them in standard form
      call hvq_momstdform(idup,1,3,4,proc,pp)
c compute mnr invariants
      s=2*hvq_dotprod(pp(1,1),pp(1,2))
      q1q=-2*hvq_dotprod(pp(1,1),pp(1,3))
      xm2=tmass**2
      if(nup.eq.5) then
         tk=-2*hvq_dotprod(pp(1,1),pp(1,5))
         uk=-2*hvq_dotprod(pp(1,2),pp(1,5))
         q2q=-2*hvq_dotprod(pp(1,2),pp(1,4))
c w1=-q1+q2-tk, w2=q1-q2-uk, w1(w2) = w1h(w2h) *s*(1-x)/2
         w1h=2*(q2q-q1q-tk)/(-tk-uk)
         w2h=2*(q1q-q2q-uk)/(-tk-uk)
         x=1+(tk+uk)/s
         y=(tk-uk)/(s*(1-x))
c cth2 is not used by hvq_FGG,hvq_FQG,hvq_FQQ, unless we are very near the collinear limit
         cth2=0
         if(proc.eq.'gg')  then
            resmnr = hvq_FGG(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)/(4*tk*uk)
         elseif(proc.eq.'qg') then
            resmnr = hvq_FQG(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)/(4*tk*uk)
         elseif(proc.eq.'qa') then
            resmnr = hvq_FQQ(s,x,y,xm2,q1q,q2q,w1h,w2h,cth2)/(4*tk*uk)
         else
            write(*,*)' error: hvq_ttdecay, invalid hvq_process 1',proc
            stop
         endif
      elseif(nup.eq.4) then
         if(proc.eq.'gg') then
            resmnr=hvq_ggborn(s,q1q,xm2)
         elseif(proc.eq.'qa') then
            resmnr=hvq_qqborn(s,q1q,xm2)
         else
            write(*,*)' error: hvq_ttdecay, invalid hvq_process 2'
            stop
         endif
      endif
c End of MNR calculation of cross section
c
c find decay mode of W+ and W-; iwp1, iwp2 refer to the pdg id' of
c W decay products 1 and 2; by convention 1 is down type (e,mu,tau,d,s
c for W-, their antiparticles for W+) and 2 is up type.
c mdecw%% are the corresponding masses of the decay products.
      call hvq_pickwdecays(iwp1,mdecwp1,iwp2,mdecwp2,
     #                 iwm1,mdecwm1,iwm2,mdecwm2,dummy)
      rat=0
c Main hit-and-miss return point
 1    continue
c restore pp momenta
      do j=1,4
         do k=1,nup
            pp(j,k)=pps(j,k)
         enddo
      enddo      
c boost to partonic CM
      do j=1,3
         betacm(j)=-(pp(j,1)+pp(j,2))/(pp(4,1)+pp(4,2))
      enddo
      call hvq_boostm(betacm,nup,pp(1,1),pp(1,1))
      ttenmnr=pp(4,3)+pp(4,4)
      if(nup.eq.5) then
         kpair=pp(4,5)
      else
         kpair=0
      endif
      if(nup.eq.5) then
c     boost heavy quarks in their rest frame
         do j=1,3
            beta(j)=pp(j,5)/ttenmnr
         enddo
         call hvq_boostm(beta,2,pp(1,3),pp(1,3))
      endif
c krtop: relative top momentum (i.e. in the top-antitop CM)
      krtop2=pp(4,3)**2-tmass**2
      call hvq_twvirts(krtop2,kpair,tpvirt2,wpvirt2,tmvirt2,wmvirt2)
c Uncomment the following to test strict pole approximation
c      tpvirt2=tmass**2
c      tmvirt2=tmass**2
c      wpvirt2=wmass**2
c      wmvirt2=wmass**2
c
      pp(4,3)=sqrt(tpvirt2+krtop2)
      pp(4,4)=sqrt(tmvirt2+krtop2)
      if(nup.eq.5) then
c     boost heavy quarks back to partonic CM
         do j=1,3
            beta(j)=-pp(j,5)/sqrt(kpair**2+(pp(4,3)+pp(4,4))**2)
         enddo
         call hvq_boostm(beta,2,pp(1,3),pp(1,3))
      endif
c t tbar energy partonic CM
      tten=pp(4,3)+pp(4,4)
c total final state energy in partonic CM
      cmen=tten+kpair
c total final state energy in partonic CM in the zero top width case
      cmenmnr=ttenmnr+kpair
c rescale initial momenta to conserve energy
      do k=1,2
         do j=1,4
            pp(j,k)=pp(j,k)*cmen/cmenmnr
         enddo
      enddo
c boost to lab cm
      do j=1,3
         betacm(j)=-betacm(j)
      enddo
      call hvq_boostm(betacm,nup,pp(1,1),pp(1,1))
c to avoid bugs in HELAS, restore exact masslessness of incoming partons
      pp(4,1)=abs(pp(3,1))
      pp(4,2)=abs(pp(3,2))
c reset the values of the Les Houches common block, to account for off-shell tt.
c We need to do this now; pp is used later to store decay products momenta
      do k=1,nup
         do j=1,4
            pup(j,k)=pp(j,k)
         enddo
      enddo
      pup(5,3)=sqrt(tpvirt2)
      pup(5,4)=sqrt(tmvirt2)
      call hvq_momcons(2,pp,nup-2,pp(1,3))
c fill the first two entries of the madevent phase space array;
      do k=1,2
         do j=1,4
            ppss(j,k)=pp(j,k)
         enddo
      enddo
c if we have an extra parton in the final state
      if(nup.eq.5) then
         do j=1,4
            ppss(j,9)=pp(j,5)
         enddo
      endif
c t and tbar decay products
      ebm=(tpvirt2+bmass**2-wpvirt2)/(2*sqrt(tpvirt2))
      ebp=(tmvirt2+bmass**2-wmvirt2)/(2*sqrt(tmvirt2))
      call hvq_decaytop(pup(1,3),wpvirt2,ebm,ppss(1,3))
      call hvq_decaytop(pup(1,4),wmvirt2,ebp,ppss(1,6))
c check mom.cons.
      call hvq_momcons(1,pup(1,3),3,ppss(1,3))
      call hvq_momcons(1,pup(1,4),3,ppss(1,6))
      call hvq_momcons(2,ppss,nup-4+6,ppss(1,3))
c Store the ppss in pp for permutations (ppss are saved)
      do k=1,9
         do j=1,4
            pp(j,k)=ppss(j,k)
         enddo
      enddo
c exchange momenta to get standard form
      call hvq_momstdform(idup,3,3,6,proc,pp)
c check mom.cons.
      call hvq_momcons(2,pp,nup-4+6,pp(1,3))
c Check the invariant masses
c      call hvq_invmasschain(pp)
c      write(*,*) sqrt(wpvirt2),sqrt(wmvirt2),sqrt(tpvirt2),sqrt(tmvirt2)
c
c MadEvent uses the (E,p1,p2,p3)
c convention for momenta
      do j=1,9
         do k=1,3
            pme(k,j)=pp(k,j)
         enddo
         pme(0,j)=pp(4,j)
      enddo
c         
      if(nup.eq.4) then
         if(proc.eq.'qa') then
            call hvq_UUB_TTB(pme,res)
         elseif(proc.eq.'gg') then
            call hvq_GG_TTB(pme,res)
         endif
      else
         if(proc.eq.'qa') then
            call hvq_UUB_TTBG(pme,res)
         elseif(proc.eq.'gg') then
            call hvq_GG_TTBG(pme,res)
         elseif(proc.eq.'qg') then
            call hvq_UG_TTBU(pme,res)
         endif
      endif
c /(2*s)
      res=res/(4*hvq_dotprod(pp(1,1),pp(1,2)))
c All subsequent factors are included in MadEvent, but not
c in MNR
      xboundb=1
      xboundb=xboundb/(
     #       ((tpvirt2-tmass**2)**2+tmass**2*twidth**2)*
     #       ((wpvirt2-wmass**2)**2+wmass**2*wwidth**2)*
     #       ((tmvirt2-tmass**2)**2+tmass**2*twidth**2)*
     #       ((wmvirt2-wmass**2)**2+wmass**2*wwidth**2) )
c Amplitude squared for top decay
      xboundb=xboundb*topdecamp(tpvirt2,wpvirt2)*
     #                topdecamp(tmvirt2,wmvirt2)*
     #                wdecamp(wpvirt2)*
     #                wdecamp(wmvirt2)
c
      rat=res/(xboundb*resmnr)
      if(rat.gt.boundnorm)
     #  call  hvq_increasecnt('topdecay upper bound violations')
      ubound=max(rat,ubound)
      if(boundnorm*hvq_random().gt.rat) then
         call hvq_increasecnt('topdecay vetoed configurations')
         goto 1
      endif
c End Hit and Miss loop
c restore pp as before exchanges
      do k=1,9
         do j=1,4
            pp(j,k)=ppss(j,k)
         enddo
      enddo
c Put finite masses to W decay products
      call hvq_momcons(2,pp,nup-4+6,pp(1,3))
      call hvq_truemasses(pp(1,3),mdecwp1,mdecwp2)
      call hvq_truemasses(pp(1,6),mdecwm1,mdecwm2)
      call hvq_momcons(2,pp,nup-4+6,pp(1,3))
c Add the decay products on the Les Houches interface.
c Set istup=2 for t,tbar (intermediate resonance, preserve mass
c We also add the W momentum as istup=2
      istup(itop)=2
      istup(itbr)=2
c W+
      nup=nup+1
      iwp=nup
      idup(nup)=24
      do j=1,4
         pup(j,nup)=pp(j,3)+pp(j,4)
      enddo
      pup(5,nup)=sqrt(wpvirt2)
      istup(nup)=2
      mothup(1,nup)=itop
      mothup(2,nup)=itop
      icolup(1,nup)=0
      icolup(2,nup)=0
      spinup(nup)=9
c b
      nup=nup+1
      idup(nup)=5
      do j=1,4
         pup(j,nup)=pp(j,5)
      enddo
      pup(5,nup) = bmass
      istup(nup) = 1
      mothup(1,nup)=itop
      mothup(2,nup)=itop
      icolup(1,nup)=icolup(1,itop)
      icolup(2,nup)=icolup(2,itop)
      spinup(nup)=-1
c W-
      nup=nup+1      
      iwm=nup
      idup(nup)=-24
      do j=1,4
         pup(j,nup)=pp(j,6)+pp(j,7)
      enddo
      pup(5,nup)=sqrt(wmvirt2)
      istup(nup)=2
      mothup(1,nup)=itbr
      mothup(2,nup)=itbr
      icolup(1,nup)=0
      icolup(2,nup)=0
      spinup(nup)=9
c bbar
      nup=nup+1
      idup(nup)=-5
      do j=1,4
         pup(j,nup)=pp(j,8)
      enddo
      pup(5,nup) = bmass
      istup(nup) = 1
      mothup(1,nup)=itbr
      mothup(2,nup)=itbr
      icolup(1,nup)=icolup(1,itbr)
      icolup(2,nup)=icolup(2,itbr)
      spinup(nup)=1
c 1st W+ decay products
      nup=nup+1
      idup(nup)=iwp1
      do j=1,4
         pup(j,nup)=pp(j,3)
      enddo
      pup(5,nup) = mdecwp1
      istup(nup) = 1
      mothup(1,nup)=iwp
      mothup(2,nup)=iwp
      spinup(nup)=1
c setup colours
      if(abs(iwp1).lt.11) then
c find free colour tag
         k=1
         do j=1,nup
            k=max(icolup(1,j),icolup(2,j),k)
         enddo
         k=k+1
c tag found
         if(iwp1.gt.0) then
            icolup(1,nup)=k
            icolup(2,nup)=0
         else
            icolup(1,nup)=0
            icolup(2,nup)=k
         endif
      else
         icolup(1,nup)=0
         icolup(2,nup)=0
      endif
c second
      nup=nup+1
      idup(nup)=iwp2
      do j=1,4
         pup(j,nup)=pp(j,4)
      enddo
      pup(5,nup) = mdecwp2
      istup(nup) = 1
      mothup(1,nup)=iwp
      mothup(2,nup)=iwp
      icolup(1,nup)=icolup(2,nup-1)
      icolup(2,nup)=icolup(1,nup-1)
      spinup(nup)=-1
c 1st W- decay products
      nup=nup+1
      idup(nup)=iwm1
      do j=1,4
         pup(j,nup)=pp(j,6)
      enddo
      pup(5,nup) = mdecwm1
      istup(nup) = 1
      mothup(1,nup)=iwm
      mothup(2,nup)=iwm
      spinup(nup)=-1
c setup colours
      if(abs(iwm1).lt.11) then
c find free colour tag
         k=1
         do j=1,nup
            k=max(icolup(1,j),icolup(2,j),k)
         enddo
         k=k+1
c tag found
         if(iwm1.gt.0) then
            icolup(1,nup)=k
            icolup(2,nup)=0
         else
            icolup(1,nup)=0
            icolup(2,nup)=k
         endif
      else
         icolup(1,nup)=0
         icolup(2,nup)=0
      endif
c second
      nup=nup+1
      idup(nup)=iwm2
      do j=1,4
         pup(j,nup)=pp(j,7)
      enddo
      pup(5,nup) = mdecwp2
      istup(nup) = 1
      mothup(1,nup)=iwm
      mothup(2,nup)=iwm
      icolup(1,nup)=icolup(2,nup-1)
      icolup(2,nup)=icolup(1,nup-1)
      spinup(nup)=1
c
      do k=1,nup
         vtimup(k)=0
      enddo
      end


      subroutine hvq_truemasses(decprod,m1,m2)
      implicit none
      real * 8 decprod(4,2),m1,m2
      real * 8 beta(3),wv,mom
      integer j
      real * 8 hvq_decmom
      external hvq_decmom
      do j=1,3
         beta(j)= - (decprod(j,1)+decprod(j,2)) /
     #              (decprod(4,1)+decprod(4,2))
      enddo
      call hvq_boostm(beta,2,decprod,decprod)
      wv=decprod(4,1)+decprod(4,2)
      mom=hvq_decmom(wv,m1,m2)
      do j=1,3
         decprod(j,1)=decprod(j,1)*mom/decprod(4,1)
         decprod(j,2)=decprod(j,2)*mom/decprod(4,2)
      enddo
      decprod(4,1)=sqrt(mom**2+m1**2)
      decprod(4,2)=sqrt(mom**2+m2**2)
      do j=1,3
         beta(j)=-beta(j)
      enddo
      call hvq_boostm(beta,2,decprod,decprod)
      end

      subroutine hvq_invmasschain(pp)
c print the invarian masses of the W's and tops in the event
c (for edbugging)
      implicit none
      real * 8 pp(4,9),w(2),t(2),tt
      integer k,i,j
c w virtualities
      do k=1,2
         w(k)=0
c i=0,3
         i=3*(k-1)
         do j=1,3
            w(k)=w(k)-(pp(j,3+i)+pp(j,4+i))**2
         enddo
         w(k)=w(k)+(pp(4,3+i)+pp(4,4+i))**2
      enddo
c t virtualities
      do k=1,2
         t(k)=0
         i=3*(k-1)
         do j=1,3
            t(k)=t(k)-(pp(j,3+i)+pp(j,4+i)+pp(j,5+i))**2
         enddo
         t(k)=t(k)+(pp(4,3+i)+pp(4,4+i)+pp(4,5+i))**2
      enddo
      tt=0
      do j=1,3
         tt=tt-(pp(j,3)+pp(j,4)+pp(j,5)+pp(j,6)+pp(j,7)+pp(j,8))**2
      enddo
      tt=tt+(pp(4,3)+pp(4,4)+pp(4,5)+pp(4,6)+pp(4,7)+pp(4,8))**2
      write(*,*) sqrt(w(1)),sqrt(w(2)),sqrt(t(1)),sqrt(t(2)),sqrt(tt)
      end

      function hvq_pdfcorr(corfac)
c compute the correction factor to the luminosity,
c due to the rescaling of the initial momenta when
c the top and anti-top invariant masses are off the pole mass
      implicit none
      include 'src/Lib/hvq/LesHouches.h'
      include 'src/Lib/hvq/pwhg_pdf.h'
      real * 8 hvq_pdfcorr,corfac
      real * 8 mufc2,x(2),xc(2),r
      real * 8 fx(-6:6)
      real * 8 hvq_powheginput
      external hvq_powheginput
      integer ndns(2),ipart(2),ih(2),k
      ndns(1)=pdf_ndns1
      ndns(2)=pdf_ndns2
      ih(1)=pdf_ih1
      ih(2)=pdf_ih2
c Factorization scale (relay on 5 being the light parton)
      if(nup.eq.4) then
         mufc2=5d0
      else
         mufc2=max(5d0,pup(2,5)**2+pup(3,5)**2)
      endif
      do k=1,2
         x(k)=pup(4,k)/ebmup(k)
         xc(k)=x(k)*corfac
         if(xc(k).gt.1) then
            hvq_pdfcorr=0
            return
         endif
      enddo
      do k=1,2
         if(idbmup(k).eq.2212) then
            ih(k)=1
         elseif(idbmup(k).eq.-2212) then
            ih(k)=-1
         else
            write(*,*) ' hvq_pdfcorr: cannot handle incoming hadrons',
     #           idbmup(1),idbmup(2)
            stop
         endif
      enddo
c parton types, from pdg to generic pdf
      do k=1,2
         if(idup(k).eq.21) then
            ipart(k)=0
         else
            ipart(k)=idup(k)
         endif
      enddo
      r=1
      do k=1,2
         call hvq_genericpdf(ndns(k),ih(k),mufc2,x(k),fx)
         if(fx(ipart(k)).ne.0) then
            r=r/fx(ipart(k))
            call hvq_genericpdf(ndns(k),ih(k),mufc2,xc(k),fx)
            r=r*fx(ipart(k))
         endif
      enddo
      hvq_pdfcorr=r
      end

      function hvq_decmom(m,m1,m2)
c compute the momentum of the two body decay products, from mass m-> m1,m2
      implicit none
      real * 8 hvq_decmom,m,m1,m2
      if(m.lt.m1+m2) then
         hvq_decmom=0
      else
         hvq_decmom=sqrt((m-m1-m2)*(m-m1+m2)*(m+m1-m2)*(m+m1+m2))/(2*m)
      endif
      end


      subroutine hvq_twvirts(krtop2,kpair,
     #     tpvirt2,wpvirt2,tmvirt2,wmvirt2)
c Computes the hvq_random virtualities for t+, t-, W+ and W-, according to
c their breit-Wigner shape, to phase space, and to (small) luminosity changes
c due to t and tbar going off the pole mass. The hvq_scheme adopted here is that
c the relative 3-momentum of the top in the t-tbar rest system, and the
c 3 momentum of the radiated light parton (if any) in the partonic CM system
c are kept fixed.
c Input:
c krtop2: relative 3-momentum squared of t (i.e. t momentum in t+ t- rest frame
c kpair:  t+ t- total 3-momentum in partonic CM system
c tpvirt2,wpvirt2,tmvirt2,wmvirt2: t+,W+,t-,W- virtualities squared
c The values of the top and W mass and widths, and the value of the b mass,
c are taken from the MEcoupl.inc common blocks.
      implicit none
      include 'src/Lib/hvq/MEcoupl.inc'
      real * 8 krtop2,kpair,
     #     tpvirt2,wpvirt2,tmvirt2,wmvirt2,tpvirt,tmvirt,wpvirt,wmvirt
      real * 8 bmass2,tmass2,wmass2,eb0,ttmass0,tten0,cfac,ttmass,
     #         kb0,wpkl0,wmkl0,tten,cmen,cmen0,hvq_ran(5)
      real * 8  hvq_pdfcorr,hvq_decmom
      external hvq_pdfcorr,hvq_decmom
      integer j
      real * 8 pi
      parameter (pi=3.141592653589793d0)
      real * 8 virt2,mass,gam
c generate squared virtuality according to Breit-Wigner and hvq_random number hvq_ran(j)
      virt2(mass,gam,j)=gam*mass*tan(pi/2*(1-2*hvq_ran(j)))+mass**2
      bmass2=bmass**2
      tmass2=tmass**2
      wmass2=wmass**2
c b energy in t rest frame in zero width limit
      eb0=(tmass2+bmass2-wmass2)/(2*tmass)
c momentum of decay products in t rest frame
      kb0=hvq_decmom(tmass,wmass,bmass)
c momentum of decay products in W decays
      wpkl0=hvq_decmom(wmass,0d0,0d0)
      wmkl0=hvq_decmom(wmass,0d0,0d0)
c invariant mass of t+t- system (zero width)
      ttmass0=2*sqrt(tmass2+krtop2)
      tten0=sqrt(ttmass0**2+kpair**2)
      cmen0=tten0+kpair
c Begin loop for hit and miss
 1    continue
c get 5 hvq_random numbers
      call hvq_RM48(hvq_ran,5)
c First get virtualities according to Lorentian;
c avoid too extreme values
      tpvirt2=virt2(tmass,twidth,1)
      if(tpvirt2.lt.wmass2.or.tpvirt2.gt.2*tmass2) goto 1
      tpvirt=sqrt(tpvirt2)
      tmvirt2=virt2(tmass,twidth,2)
      if(tmvirt2.lt.wmass2.or.tmvirt2.gt.2*tmass2) goto 1
      tmvirt=sqrt(tmvirt2)
      wpvirt2=virt2(wmass,wwidth,3)
      if(wpvirt2.gt.tpvirt2.or.wpvirt2.lt.0) goto 1
      wpvirt=sqrt(wpvirt2)
      wmvirt2=virt2(wmass,wwidth,4)
      if(wmvirt2.gt.tmvirt2.or.wmvirt2.lt.0) goto 1
      wmvirt=sqrt(wmvirt2)
c estimate phase space factor and do hit and miss with it
c this is from missing 1/(k0(top) * k0(antitop)) in top-antitop rest frame
      cfac=(tmass2+krtop2)/sqrt((tpvirt2+krtop2)*(tmvirt2+krtop2))
c this is from phase space in t->W+b
      cfac=cfac*(hvq_decmom(tpvirt,wpvirt,bmass)/tpvirt)/(kb0/tmass)
c this is from phase space in tbar->W- + b
      cfac=cfac*(hvq_decmom(tmvirt,wmvirt,bmass)/tmvirt)/(kb0/tmass)
c this is from phase space in W+ decay
      cfac=cfac*(hvq_decmom(wpvirt,0d0,0d0)/wpvirt)/(wpkl0/wmass)
c this is from phase space in W- decay
      cfac=cfac*(hvq_decmom(wmvirt,0d0,0d0)/wmvirt)/(wmkl0/wmass)
c this is from ttmass/tten*sqrt(tau) factor
      ttmass=sqrt(tpvirt2+krtop2)+sqrt(tmvirt2+krtop2)
      tten=sqrt(ttmass**2+kpair**2)
      cmen=tten+kpair
      cfac=cfac * (cmen*ttmass/tten) / (cmen0*ttmass0/tten0)
      cfac=cfac*hvq_pdfcorr(cmen/cmen0)
c we optimize for cfac at most = 2 (no prize for larger values)
      if(2*hvq_ran(5).gt.cfac) goto 1
      end
      
      subroutine hvq_momstdform(idup,ndec,it,itbar,proc,pp)
c puts momenta in standard order, for evaluation by MNR and MadEvent
c MadEvent -> g(1)g(2) -> l+(3)nu(4)b(5) l-(6)nub(7)bb(8)
c MadEvent -> q(1)qb(2)-> l+(3)nu(4)b(5) l-(6)nub(7)bb(8)
c MadEvent -> g(1)g(2) -> l+(3)nu(4)b(5) l-(6)nub(7)bb(8) g(9)
c MadEvent -> q(1)qb(2)-> l+(3)nu(4)b(5) l-(6)nub(7)bb(8) g(9)
c MadEvent -> q(1)g(2) -> l+(3)nu(4)b(5) l-(6)nub(7)bb(8) q(9)
c or
c MNR -> g(1)g(2) -> t+(3) t-(4)
c MNR -> q(1)qb(2)-> t+(3) t-(4)
c MNR -> g(1)g(2) -> t+(3) t-(4) g(5)
c MNR -> q(1)qb(2)-> t+(3) t-(4) g(5)
c MNR -> q(1)g(2) -> t+(3) t-(4) q(5)
c
c by exchanging 1<->2, and by mimicking charge conjugation by 3<->4 (MNR)
c or (3,4,5)<->(6,7,8) (MadEvent) exchange
c
c Calling sequence:
c call hvq_momstdform(idup,1,3,4,proc,pp) (MNR)
c call hvq_momstdform(idup,3,3,6,proc,pp) (MadEvent)
c idup: idup array of Les Houches Common Block
c return values:
c proc: two characted string: 'gg', 'qa', 'qg'
c pp: momentum array in input, permuted array in output
      implicit none
      character * 2 proc
      integer idup(2)
      real * 8 pp(4,*)
      integer ndec,it,itbar
      integer k
c now label the hvq_process and do the exchanges
      if(idup(1).gt.0.and.idup(2).lt.0) then
c quark antiquark
         proc='qa'
      elseif(idup(1).eq.21.and.idup(2).eq.21) then
         proc='gg'
      elseif(idup(1).gt.0.and.idup(2).eq.21) then
         proc='qg'
      elseif(idup(1).lt.0.and.idup(2).gt.0) then
         call hvq_exchmom(pp(1,1),pp(1,2))
         proc='qa'
      elseif(idup(1).eq.21.and.idup(2).gt.0) then
         call hvq_exchmom(pp(1,1),pp(1,2))
         proc='qg'
      elseif(idup(1).lt.0.and.idup(2).eq.21) then
c Exchange t and t-tbar, to mimic charge conjugation.
         do k=0,ndec-1
            call hvq_exchmom(pp(1,it+k),pp(1,itbar+k))
         enddo
         proc='qg'
      elseif(idup(1).eq.21.and.idup(2).lt.0) then
c Exchange initial state momenta and t-tbar decay products
         call hvq_exchmom(pp(1,1),pp(1,2))
         do k=0,ndec-1
            call hvq_exchmom(pp(1,it+k),pp(1,itbar+k))
         enddo
         proc='qg'
      endif
      end

      subroutine hvq_decaytop(ptop,wvirt2,eb,decprod)
c Put momenta of top decay in decprod, distributed according phase space,
c for a given W virtuality
c Input (momenta as p(1:4)=(p1,p2,p3,E)
c ptop: top 4 momentum
c wvirt2:    W virtuality squared
c eb:   b energy in top rest frame
c m1:   mass of first W decay product
c m2:   mass of second W decay product
c output:
c decprod(1:4,1): W first decay product
c decprod(1:4,2): W second decay product
c decprod(1:4,3): bottom momentum
      implicit none
      include 'src/Lib/hvq/MEcoupl.inc'
      real * 8 ptop(4),wvirt2,eb,decprod(4,3)
      real * 8 wvirt,pb,ew,pl,beta(3)
      wvirt=sqrt(wvirt2)
c modulus of b momentum
      pb=sqrt(eb**2-bmass**2)
c W energy in top rest frame
      ew=sqrt(pb**2+wvirt2)
c generate hvq_random directed b momentum in t rest frame
      call hvq_rn3vec(decprod(1,3),pb)
      decprod(4,3)=eb
c 1st W decay product in W rest frame
      pl=wvirt/2
      call hvq_rn3vec(decprod(1,1),pl)
      decprod(4,1)=pl
c 2nd W decay product in W rest frame
      decprod(1,2)=-decprod(1,1)
      decprod(2,2)=-decprod(2,1)
      decprod(3,2)=-decprod(3,1)
      decprod(4,2)=pl
c boost W decay products with W velocity in t rest frame
c (its momentum is opposite to the b momentum)
      beta(1)=-decprod(1,3)/ew
      beta(2)=-decprod(2,3)/ew
      beta(3)=-decprod(3,3)/ew
      call hvq_boostm(beta,2,decprod(1,1),decprod(1,1))
c boost all decay products along top velocity
      beta(1)=ptop(1)/ptop(4)
      beta(2)=ptop(2)/ptop(4)
      beta(3)=ptop(3)/ptop(4)
      call hvq_boostm(beta,3,decprod(1,1),decprod(1,1))
      end

      subroutine hvq_momcons(n1,p1,n2,p2)
c check that sum of n1 momenta in p1
c equals the sum of n2 momenta in p2
      implicit none
      integer n1,n2
      real * 8 p1(4,*),p2(4,*)
      real * 8 res(4)
      integer j,k
      do j=1,4
         res(j)=0
         do k=1,n1
            res(j)=res(j)+p1(j,k)
         enddo
         do k=1,n2
            res(j)=res(j)-p2(j,k)
         enddo
      enddo
      do j=1,4
         if(abs(res(j)).gt.1d-7) then
            write(*,*) ' mom. check failed'
            write(*,*) (res(k),k=1,4)
            return
         endif
      enddo
      end




      subroutine hvq_exchmom(p1,p2)
      implicit none
      real * 8 p1(4),p2(4),tmp
      integer j
      do j=1,4
         tmp=p1(j)
         p1(j)=p2(j)
         p2(j)=tmp
      enddo
      end


      subroutine hvq_pickwdecays(iwp1,mdecwp1,iwp2,mdecwp2,
     #                 iwm1,mdecwm1,iwm2,mdecwm2,totbr)
c Finds which decays to choose with correct probability, according
c to an integer of 5 digits that are either 0, or 2, representing in
c the order the maximum number of the following particles(antiparticles)
c in the final state:
c          e  mu tau up charm
c For example
c 22222    All decays (up to 2 units of everything)
c 20000    both top go into b l nu (with the appropriate signs)
c 10011    one top goes into electron (or positron), the other into (any) hadrons
c 00022    Fully hadronic
c 00002    Fully hadronic with two charms
c 00011    Fully hadronic with a single charm
c 00012    Fully hadronic with at least one charm
      implicit none
      integer iwp1,iwp2, iwm1,iwm2, iwp(5,2),iwa(4)
      real * 8 mdecwp1,mdecwp2,mdecwm1,mdecwm2
      real * 8 probs(5,5),prbs(25),pr(5),mass(16),
     #     sin2cabibbo,ebr,hbr,r,totbr
      equivalence (probs(1,1),prbs(1))
      integer ini,ii(5),j,k,imode
      logical semileptonic
      real * 8 hvq_random,hvq_powheginput
      external hvq_random,hvq_powheginput
      data ini/0/
c pdg id's of 1st and 2nd W+ decay products for e,mu,tau,up and charm decays (ignoring CKM)
      data ((iwp(j,k),k=1,2),j=1,5)/-11,12, -13,14, -15,16, -1,2, -3,4/
      save ini,probs,iwp,mass,sin2cabibbo,semileptonic
      if(ini.eq.2) return
      if(ini.eq.0) then
         ini=1
c on first run look for decay mode in hvq_powheginput
         imode=hvq_powheginput('topdecaymode')
         semileptonic=hvq_powheginput('#semileptonic').eq.1
         if(imode.eq.0) then
            ini=2
            return
         endif
         ii(1)=imode/10000
         imode=imode-ii(1)*10000
         ii(2)=imode/1000
         imode=imode-ii(2)*1000
         ii(3)=imode/100
         imode=imode-ii(3)*100
         ii(4)=imode/10
         imode=imode-ii(4)*10
         ii(5)=imode
         ebr=hvq_powheginput('tdec/elbranching')
         hbr=(1-3*ebr)/2
         do j=1,5
            if(ii(j).eq.0) then
               pr(j)=0
            else
               if(j.le.3) then
                  pr(j)=ebr
               else
                  pr(j)=hbr
               endif
            endif
         enddo
         do j=1,5
            do k=1,5
               if(j.eq.k.and.ii(k).lt.2) then
                  probs(j,k)=0
               else
                  if(semileptonic.and.( (j.gt.3.and.k.gt.3)
     1                 .or.(j.le.3.and.k.le.3) ) ) then
                     probs(j,k)=0
                  else
                     probs(j,k)=pr(j)*pr(k)
                  endif
               endif
            enddo
         enddo
         do j=2,25
            prbs(j)=prbs(j)+prbs(j-1)
         enddo
         totbr=prbs(25)
         if(totbr.eq.0) then
            write(*,*) 'hvq_pickwdecays: the input parameters are such'
            write(*,*) 'that no decays are possible:'
            write(*,*) 'topdecaymode=',nint(hvq_powheginput('topdecaymode'))
            if(semileptonic) then
               write(*,*) 'semileptonic=1'
            endif
            write(*,*) ' Halting execution'
            stop
         endif
         mass(11)=hvq_powheginput('tdec/emass')
         mass(13)=hvq_powheginput('tdec/mumass')
         mass(15)=hvq_powheginput('tdec/taumass')
         mass(12)=0
         mass(14)=0
         mass(16)=0
         mass(1)=hvq_powheginput('tdec/dmass')
         mass(2)=hvq_powheginput('tdec/umass')
         mass(3)=hvq_powheginput('tdec/smass')
         mass(4)=hvq_powheginput('tdec/cmass')
         sin2cabibbo=hvq_powheginput('tdec/sin2cabibbo')
         return
      endif
c end initialization
      r=hvq_random()*probs(5,5)
      do k=1,5
         do j=1,5
            if(r.lt.probs(j,k)) goto 1
         enddo
      enddo
 1    continue
c now we have j,k decay mode
      iwa(1)=iwp(j,1)
      iwa(2)=iwp(j,2)
      iwa(3)=-iwp(k,1)
      iwa(4)=-iwp(k,2)
c if any is down or strange, it may turn to
c strange/down with a probability sin^2 theta
      do j=1,4,1
         if(abs(iwa(j)).eq.1) then
            if(hvq_random().lt.sin2cabibbo) then
               iwa(j)=sign(3,iwa(j))
            endif
         elseif(abs(iwa(j)).eq.3) then
            if(hvq_random().lt.sin2cabibbo) then
               iwa(j)=sign(1,iwa(j))
            endif
         endif
      enddo
      iwp1=iwa(1)
      iwp2=iwa(2)
      iwm1=iwa(3)
      iwm2=iwa(4)
      mdecwp1=mass(abs(iwp1))
      mdecwp2=mass(abs(iwp2))
      mdecwm1=mass(abs(iwm1))
      mdecwm2=mass(abs(iwm2))
      end




      subroutine hvq_madeventinput
      implicit none
c Fills HELAS common blocks for masses and couplings. The electron charge
c squared and the masses may eventually be passed through a common block
c on a event-by-event basis. This code is mainly taken from coupsm-ORIGINAL.F 
c of the HELAS package. Here, we limit ourselves to setting the following
c parameters:
c
c       real    gw                : weak coupling constant
c       real    gwwa              : dimensionless WWA  coupling
c       real    gwwz              : dimensionless WWZ  coupling
c       complex gal(2)            : coupling with A of charged leptons
c       complex gau(2)            : coupling with A of up-type quarks
c       complex gad(2)            : coupling with A of down-type quarks
c       complex gwf(2)            : coupling with W-,W+ of fermions
c       complex gzn(2)            : coupling with Z of neutrinos
c       complex gzl(2)            : coupling with Z of charged leptons
c       complex gzu(2)            : coupling with Z of up-type quarks
c       complex gzd(2)            : coupling with Z of down-type quarks
c       complex gg(2)             : QCD gqq coupling (L,R)
c
c through the following parameters, given by hvq_powheginput
c
c       real    zmass,wmass       : weak boson masses
c       real    zwidth,wwidth     : weak boson width
c       real    tmass,twidth      : top mass and width
c       real    bmass             : bottom mass
c       real    sin2w             : square of sine of the weak angle
c       real    ee2               : positron charge squared
c       real    g                 : QCD 3-,4-gluon coupling
c
      include 'src/Lib/hvq/MEcoupl.inc'
      real * 8 hvq_powheginput
      double precision zero,half,one,two,three,pi,ee2,sw,cw,ez,ey,sc2,v
      parameter (zero=0.d0)
      parameter (half=0.5d0)
      parameter (one=1.d0)
      parameter (two=2.d0)
      parameter (three=3.d0)
      parameter (pi=3.14159265358979312D0)
      integer ini
      data ini/0/
      save ini
      external hvq_powheginput
      if(ini.eq.1) return
      ini=1
      wmass=hvq_powheginput('tdec/wmass')
      wwidth=hvq_powheginput('tdec/wwidth')
      zmass=0
      zwidth=0
      tmass=hvq_powheginput('qmass')
      twidth=hvq_powheginput('tdec/twidth')
      bmass=hvq_powheginput('tdec/bmass')
      sin2w=0.23
      alpha=1
      ee2=4*pi*alpha
c
      g     = 1
c
      amass=0.d0
      awidth=1.d-99
c
      ee=sqrt(ee2)
c
      sw  = sqrt( sin2w )
      cw  = sqrt( One - sin2w )
      ez  = ee/(sw*cw)
      ey  = ee*(sw/cw)
      sc2 = sin2w*( One - sin2w )
      v   = Two*zmass*sqrt(sc2)/ee
c
c vector boson couplings
c
      gw   = ee/sw
      gwwa = ee
      gwwz = ee*cw/sw
c
c fermion-fermion-vector couplings
c
      gal(1) = dcmplx(  ee          , Zero )
      gal(2) = dcmplx(  ee          , Zero )
      gau(1) = dcmplx( -ee*Two/Three, Zero )
      gau(2) = dcmplx( -ee*Two/Three, Zero )
      gad(1) = dcmplx(  ee/Three    , Zero )
      gad(2) = dcmplx(  ee/Three    , Zero )
c
      gwf(1) = dcmplx( -ee/sqrt(Two*sin2w), Zero )
      gwf(2) = dcmplx(  Zero              , Zero )
c
      gzn(1) = dcmplx( -ez*Half                     , Zero )
      gzn(2) = dcmplx(  Zero                        , Zero )
      gzl(1) = dcmplx( -ez*(-Half + sin2w)          , Zero )
      gzl(2) = dcmplx( -ey                          , Zero )
      gzu(1) = dcmplx( -ez*( Half - sin2w*Two/Three), Zero )
      gzu(2) = dcmplx(  ey*Two/Three                , Zero )
      gzd(1) = dcmplx( -ez*(-Half + sin2w/Three)    , Zero )
      gzd(2) = dcmplx( -ey/Three                    , Zero )
c
c QCD coupling
c
      gg(1) = dcmplx( -g, Zero )
      gg(2) = gg(1)
c
      return
      end


      subroutine hvq_switchmom(p1,p,ic,jc,nexternal)
c**************************************************************************
c     Changes stuff for crossings
c**************************************************************************
      implicit none
      integer nexternal
      integer jc(nexternal),ic(nexternal)
      real*8 p1(0:3,nexternal),p(0:3,nexternal)
      integer i,j
c-----
c Begin Code
c-----
      do i=1,nexternal
         do j=0,3
            p(j,ic(i))=p1(j,i)
         enddo
      enddo
      do i=1,nexternal
         jc(i)=1
      enddo
      jc(ic(1))=-1
      jc(ic(2))=-1
      end


c
c
c Begin of qqbar --> ttbar
c
c
C SF: The following routine is SMATRIX generated by MadEvent, suitably modified
      SUBROUTINE hvq_UUB_TTB(P1,ANS)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR hvq_process : u u~ -> e+ ve b e- ve~ b~  
C  
C Crossing   1 is u u~ -> e+ ve b e- ve~ b~  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      Include "genps.inc"
      integer    nexternal
      parameter (nexternal=  8)
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 256, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 hvq_MATUUB_TTB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
      REAL*8 hwgt, xtry, xrej
      INTEGER idum, ngood, igood(ncomb), jhel, j
      LOGICAL warned
      REAL     xran1
      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2

      character*79         hel_buff
C SF: comment out all common blocks
c      common/hvq_to_helicity/  hel_buff

      integer          isum_hel
      logical                    multi_channel
C SF: comment out all common blocks
c      common/hvq_to_matrix/isum_hel, multi_channel
C SF: comment out all instances of mapconfig, used by multi_channel
c      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
c      common/hvq_to_mconfigs/mapconfig, iconfig
      DATA NTRY,IDUM /0,-1/
      DATA xtry, xrej, ngood /0,0,0/
      DATA warned, isum_hel/.false.,0/
      DATA multi_channel/.true./
      SAVE igood, IDUM, jhel
      DATA NGRAPHS /    1/          
C SF: comment out all instances of amp2, used by multi_channel
c      DATA jamp2(0) /   1/          
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,8) /-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,8) /-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,8) /-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,8) /-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,8) /-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,8) /-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,8) /-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,8) /-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,8) /-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,8) /-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,8) /-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,8) /-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,8) /-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,8) /-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,8) /-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,8) /-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,8) /-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,8) /-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,8) /-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,8) /-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,8) /-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,8) /-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,8) /-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,8) /-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,8) /-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,8) /-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,8) /-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,8) /-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,8) /-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,8) /-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,8) /-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,8) /-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,8) /-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,8) /-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,8) /-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,8) /-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,8) /-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,8) /-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,8) /-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,8) /-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,8) /-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,8) /-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,8) /-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,8) /-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,8) /-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,8) /-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,8) /-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,8) /-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,8) /-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,8) /-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,8) /-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,8) /-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,8) /-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,8) /-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,8) /-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,8) /-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,8) /-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,8) /-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,8) /-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,8) /-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,8) /-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,8) /-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,8) /-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,8) /-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,8) /-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,8) /-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,8) /-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,8) /-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,8) /-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,8) /-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,8) /-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,8) /-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,8) /-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,8) /-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,8) /-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,8) /-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,8) /-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,8) /-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,8) /-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,8) /-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,8) /-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,8) /-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,8) /-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,8) /-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,8) /-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,8) /-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,8) /-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,8) /-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,8) /-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,8) /-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,8) /-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,8) /-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,8) /-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,8) /-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,8) /-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,8) /-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,8) /-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,8) /-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,8) /-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,8) /-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,8) /-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,8) /-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,8) /-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,8) /-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,8) /-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,8) /-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,8) /-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,8) /-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,8) /-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,8) /-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,8) /-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,8) /-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,8) /-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,8) /-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,8) /-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,8) /-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,8) /-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,8) /-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,8) /-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,8) /-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,8) /-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,8) /-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,8) /-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,8) /-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,8) /-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,8) /-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,8) /-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,8) /-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 129),IHEL=1,8) / 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 130),IHEL=1,8) / 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 131),IHEL=1,8) / 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 132),IHEL=1,8) / 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,8) / 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,8) / 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 135),IHEL=1,8) / 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 136),IHEL=1,8) / 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 137),IHEL=1,8) / 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 138),IHEL=1,8) / 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,8) / 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,8) / 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 141),IHEL=1,8) / 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 142),IHEL=1,8) / 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 143),IHEL=1,8) / 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 144),IHEL=1,8) / 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,8) / 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,8) / 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 147),IHEL=1,8) / 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 148),IHEL=1,8) / 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 149),IHEL=1,8) / 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 150),IHEL=1,8) / 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,8) / 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,8) / 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 153),IHEL=1,8) / 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 154),IHEL=1,8) / 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 155),IHEL=1,8) / 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 156),IHEL=1,8) / 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,8) / 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,8) / 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 159),IHEL=1,8) / 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 160),IHEL=1,8) / 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 161),IHEL=1,8) / 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 162),IHEL=1,8) / 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,8) / 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,8) / 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 165),IHEL=1,8) / 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 166),IHEL=1,8) / 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 167),IHEL=1,8) / 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 168),IHEL=1,8) / 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,8) / 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,8) / 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 171),IHEL=1,8) / 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 172),IHEL=1,8) / 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 173),IHEL=1,8) / 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 174),IHEL=1,8) / 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,8) / 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,8) / 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 177),IHEL=1,8) / 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 178),IHEL=1,8) / 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 179),IHEL=1,8) / 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 180),IHEL=1,8) / 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,8) / 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,8) / 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 183),IHEL=1,8) / 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 184),IHEL=1,8) / 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 185),IHEL=1,8) / 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 186),IHEL=1,8) / 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,8) / 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,8) / 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 189),IHEL=1,8) / 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 190),IHEL=1,8) / 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 191),IHEL=1,8) / 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 192),IHEL=1,8) / 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 193),IHEL=1,8) / 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 194),IHEL=1,8) / 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 195),IHEL=1,8) / 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 196),IHEL=1,8) / 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 197),IHEL=1,8) / 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 198),IHEL=1,8) / 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 199),IHEL=1,8) / 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 200),IHEL=1,8) / 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 201),IHEL=1,8) / 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 202),IHEL=1,8) / 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 203),IHEL=1,8) / 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 204),IHEL=1,8) / 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 205),IHEL=1,8) / 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 206),IHEL=1,8) / 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 207),IHEL=1,8) / 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 208),IHEL=1,8) / 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 209),IHEL=1,8) / 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 210),IHEL=1,8) / 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 211),IHEL=1,8) / 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 212),IHEL=1,8) / 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 213),IHEL=1,8) / 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 214),IHEL=1,8) / 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 215),IHEL=1,8) / 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 216),IHEL=1,8) / 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 217),IHEL=1,8) / 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 218),IHEL=1,8) / 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 219),IHEL=1,8) / 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 220),IHEL=1,8) / 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 221),IHEL=1,8) / 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 222),IHEL=1,8) / 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 223),IHEL=1,8) / 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 224),IHEL=1,8) / 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 225),IHEL=1,8) / 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 226),IHEL=1,8) / 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 227),IHEL=1,8) / 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 228),IHEL=1,8) / 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 229),IHEL=1,8) / 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 230),IHEL=1,8) / 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 231),IHEL=1,8) / 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 232),IHEL=1,8) / 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 233),IHEL=1,8) / 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 234),IHEL=1,8) / 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 235),IHEL=1,8) / 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 236),IHEL=1,8) / 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 237),IHEL=1,8) / 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 238),IHEL=1,8) / 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 239),IHEL=1,8) / 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 240),IHEL=1,8) / 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 241),IHEL=1,8) / 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 242),IHEL=1,8) / 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 243),IHEL=1,8) / 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 244),IHEL=1,8) / 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 245),IHEL=1,8) / 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 246),IHEL=1,8) / 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 247),IHEL=1,8) / 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 248),IHEL=1,8) / 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 249),IHEL=1,8) / 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 250),IHEL=1,8) / 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 251),IHEL=1,8) / 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 252),IHEL=1,8) / 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 253),IHEL=1,8) / 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 254),IHEL=1,8) / 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 255),IHEL=1,8) / 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 256),IHEL=1,8) / 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,8) / 1, 2, 3, 4, 5, 6, 7, 8/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL hvq_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
       
C SF: comment out all instances of multi_channel
c      IF (multi_channel) THEN
c          DO IHEL=1,NGRAPHS
c              amp2(ihel)=0d0
c              jamp2(ihel)=0d0
c          ENDDO
c          DO IHEL=1,int(jamp2(0))
c              jamp2(ihel)=0d0
c          ENDDO
c      ENDIF
      ANS(IPROC) = 0D0
      write(hel_buff,'(16i5)') (0,i=1,nexternal)
      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
          DO IHEL=1,NCOMB
              IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=hvq_MATUUB_TTB(P ,NHEL(1,IHEL),JC(1))            
                 ANS(IPROC)=ANS(IPROC)+T
                  IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
                      GOODHEL(IHEL,IPROC)=.TRUE.
                      NGOOD = NGOOD +1
                      IGOOD(NGOOD) = IHEL
C                WRITE(*,*) ngood,IHEL,T
                  ENDIF
              ENDIF
          ENDDO
          JHEL = 1
          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
      ELSE              !hvq_random HELICITY
          DO J=1,ISUM_HEL
              JHEL=JHEL+1
              IF (JHEL .GT. NGOOD) JHEL=1
              HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
              IHEL = IGOOD(JHEL)
              T=hvq_MATUUB_TTB(P ,NHEL(1,IHEL),JC(1))            
           ANS(IPROC)=ANS(IPROC)+T*HWGT
          ENDDO
          IF (ISUM_HEL .EQ. 1) THEN
              WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
          ENDIF
      ENDIF
C SF: comment out all instances of multi_channel
c      IF (MULTI_CHANNEL) THEN
c          XTOT=0D0
c          DO IHEL=1,MAPCONFIG(0)
c              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
c          ENDDO
c          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
c      ENDIF
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
C SF: the original name MATRIX has been replaced by hvq_MATUUB_TTB
      REAL*8 FUNCTION hvq_MATUUB_TTB(P,NHEL,IC)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR hvq_process : u u~ -> e+ ve b e- ve~ b~  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   1,NEIGEN=  1) 
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      include "genps.inc"
      integer    nexternal
      parameter (nexternal=  8)
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  13, NCOLOR=   1) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(6,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2
C SF: The original coupl.inc has been renamed MEcoupl.inc
      include "MEcoupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            1/                                       
      DATA (CF(i,1  ),i=1  ,1  ) /     2/                                  
C               T[8,2]T[1,5]                                               
C ----------
C BEGIN CODE
C ----------
      CALL hvq_ixxxxx(P(0,1   ),ZERO ,NHEL(1   ),+1*IC(1   ),W(1,1   ))
      CALL hvq_oxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))
      CALL hvq_ixxxxx(P(0,3   ),ZERO ,NHEL(3   ),-1*IC(3   ),W(1,3   ))
      CALL hvq_oxxxxx(P(0,4   ),ZERO ,NHEL(4   ),+1*IC(4   ),W(1,4   ))
      CALL hvq_oxxxxx(P(0,5   ),BMASS ,NHEL(5   ),+1*IC(5   ),W(1,5   ))
      CALL hvq_oxxxxx(P(0,6   ),ZERO ,NHEL(6   ),+1*IC(6   ),W(1,6   ))
      CALL hvq_ixxxxx(P(0,7   ),ZERO ,NHEL(7   ),-1*IC(7   ),W(1,7   ))
      CALL hvq_ixxxxx(P(0,8   ),BMASS ,NHEL(8   ),-1*IC(8   ),W(1,8   ))
      CALL hvq_jioxxx(W(1,1   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,9   ))
      CALL hvq_jioxxx(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,10  ))
      CALL hvq_fvoxxx(W(1,5   ),W(1,10  ),GWF ,TMASS   ,TWIDTH  ,W(1,11  ))
      CALL hvq_fvoxxx(W(1,11  ),W(1,9   ),GG ,TMASS   ,TWIDTH  ,W(1,12  ))
      CALL hvq_jioxxx(W(1,7   ),W(1,6   ),GWF ,WMASS   ,WWIDTH  ,W(1,13  ))
      CALL hvq_iovxxx(W(1,8   ),W(1,12  ),W(1,13  ),GWF ,AMP(1   ))
      JAMP(   1) = +AMP(   1)
      hvq_MATUUB_TTB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          hvq_MATUUB_TTB =hvq_MATUUB_TTB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C SF: comment out all instances of amp2, used by multi_channel
c      Do I = 1, NGRAPHS
c          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
c      Enddo
c      Do I = 1, NCOLOR
c          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
c      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
c
c End of qqbar --> ttbar
c
c
c
c
c Begin of gg --> ttbar
c
c
C SF: The following routine is SMATRIX generated by MadEvent, suitably modified
      SUBROUTINE hvq_GG_TTB(P1,ANS)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR hvq_process : g g -> e+ ve b e- ve~ b~  
C  
C Crossing   1 is g g -> e+ ve b e- ve~ b~  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      Include "genps.inc"
      integer    nexternal
      parameter (nexternal=  8)
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 256, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 hvq_MATGG_TTB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
      REAL*8 hwgt, xtot, xtry, xrej, xr, yfrac(0:ncomb)
      INTEGER idum, ngood, igood(ncomb), jhel, j
      LOGICAL warned
      REAL     xran1
      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2

      character*79         hel_buff
C SF: comment out all common blocks
c      common/hvq_to_helicity/  hel_buff

      integer          isum_hel
      logical                    multi_channel
C SF: comment out all common blocks
c      common/hvq_to_matrix/isum_hel, multi_channel
C SF: comment out all instances of mapconfig, used by multi_channel
c      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
c      common/hvq_to_mconfigs/mapconfig, iconfig
      DATA NTRY,IDUM /0,-1/
      DATA xtry, xrej, ngood /0,0,0/
      DATA warned, isum_hel/.false.,0/
      DATA multi_channel/.true./
      SAVE yfrac, igood, IDUM, jhel
      DATA NGRAPHS /    3/          
C SF: comment out all instances of amp2, used by multi_channel
c      DATA jamp2(0) /   2/          
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,8) /-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,8) /-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,8) /-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,8) /-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,8) /-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,8) /-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,8) /-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,8) /-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,8) /-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,8) /-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,8) /-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,8) /-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,8) /-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,8) /-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,8) /-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,8) /-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,8) /-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,8) /-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,8) /-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,8) /-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,8) /-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,8) /-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,8) /-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,8) /-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,8) /-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,8) /-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,8) /-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,8) /-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,8) /-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,8) /-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,8) /-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,8) /-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,8) /-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,8) /-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,8) /-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,8) /-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,8) /-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,8) /-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,8) /-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,8) /-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,8) /-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,8) /-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,8) /-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,8) /-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,8) /-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,8) /-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,8) /-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,8) /-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,8) /-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,8) /-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,8) /-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,8) /-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,8) /-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,8) /-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,8) /-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,8) /-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,8) /-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,8) /-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,8) /-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,8) /-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,8) /-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,8) /-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,8) /-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,8) /-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,8) /-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,8) /-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,8) /-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,8) /-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,8) /-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,8) /-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,8) /-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,8) /-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,8) /-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,8) /-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,8) /-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,8) /-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,8) /-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,8) /-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,8) /-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,8) /-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,8) /-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,8) /-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,8) /-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,8) /-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,8) /-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,8) /-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,8) /-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,8) /-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,8) /-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,8) /-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,8) /-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,8) /-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,8) /-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,8) /-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,8) /-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,8) /-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,8) /-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,8) /-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,8) /-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,8) /-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,8) /-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,8) /-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,8) /-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,8) /-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,8) /-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,8) /-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,8) /-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,8) /-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,8) /-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,8) /-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,8) /-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,8) /-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,8) /-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,8) /-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,8) /-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,8) /-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,8) /-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,8) /-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,8) /-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,8) /-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,8) /-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,8) /-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,8) /-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,8) /-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,8) /-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,8) /-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,8) /-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,8) /-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 129),IHEL=1,8) / 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 130),IHEL=1,8) / 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 131),IHEL=1,8) / 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 132),IHEL=1,8) / 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,8) / 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,8) / 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 135),IHEL=1,8) / 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 136),IHEL=1,8) / 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 137),IHEL=1,8) / 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 138),IHEL=1,8) / 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,8) / 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,8) / 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 141),IHEL=1,8) / 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 142),IHEL=1,8) / 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 143),IHEL=1,8) / 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 144),IHEL=1,8) / 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,8) / 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,8) / 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 147),IHEL=1,8) / 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 148),IHEL=1,8) / 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 149),IHEL=1,8) / 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 150),IHEL=1,8) / 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,8) / 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,8) / 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 153),IHEL=1,8) / 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 154),IHEL=1,8) / 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 155),IHEL=1,8) / 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 156),IHEL=1,8) / 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,8) / 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,8) / 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 159),IHEL=1,8) / 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 160),IHEL=1,8) / 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 161),IHEL=1,8) / 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 162),IHEL=1,8) / 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,8) / 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,8) / 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 165),IHEL=1,8) / 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 166),IHEL=1,8) / 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 167),IHEL=1,8) / 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 168),IHEL=1,8) / 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,8) / 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,8) / 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 171),IHEL=1,8) / 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 172),IHEL=1,8) / 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 173),IHEL=1,8) / 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 174),IHEL=1,8) / 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,8) / 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,8) / 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 177),IHEL=1,8) / 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 178),IHEL=1,8) / 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 179),IHEL=1,8) / 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 180),IHEL=1,8) / 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,8) / 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,8) / 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 183),IHEL=1,8) / 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 184),IHEL=1,8) / 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 185),IHEL=1,8) / 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 186),IHEL=1,8) / 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,8) / 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,8) / 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 189),IHEL=1,8) / 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 190),IHEL=1,8) / 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 191),IHEL=1,8) / 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 192),IHEL=1,8) / 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 193),IHEL=1,8) / 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 194),IHEL=1,8) / 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 195),IHEL=1,8) / 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 196),IHEL=1,8) / 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 197),IHEL=1,8) / 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 198),IHEL=1,8) / 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 199),IHEL=1,8) / 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 200),IHEL=1,8) / 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 201),IHEL=1,8) / 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 202),IHEL=1,8) / 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 203),IHEL=1,8) / 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 204),IHEL=1,8) / 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 205),IHEL=1,8) / 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 206),IHEL=1,8) / 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 207),IHEL=1,8) / 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 208),IHEL=1,8) / 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 209),IHEL=1,8) / 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 210),IHEL=1,8) / 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 211),IHEL=1,8) / 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 212),IHEL=1,8) / 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 213),IHEL=1,8) / 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 214),IHEL=1,8) / 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 215),IHEL=1,8) / 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 216),IHEL=1,8) / 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 217),IHEL=1,8) / 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 218),IHEL=1,8) / 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 219),IHEL=1,8) / 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 220),IHEL=1,8) / 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 221),IHEL=1,8) / 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 222),IHEL=1,8) / 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 223),IHEL=1,8) / 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 224),IHEL=1,8) / 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 225),IHEL=1,8) / 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 226),IHEL=1,8) / 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 227),IHEL=1,8) / 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 228),IHEL=1,8) / 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 229),IHEL=1,8) / 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 230),IHEL=1,8) / 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 231),IHEL=1,8) / 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 232),IHEL=1,8) / 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 233),IHEL=1,8) / 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 234),IHEL=1,8) / 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 235),IHEL=1,8) / 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 236),IHEL=1,8) / 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 237),IHEL=1,8) / 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 238),IHEL=1,8) / 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 239),IHEL=1,8) / 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 240),IHEL=1,8) / 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 241),IHEL=1,8) / 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 242),IHEL=1,8) / 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 243),IHEL=1,8) / 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 244),IHEL=1,8) / 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 245),IHEL=1,8) / 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 246),IHEL=1,8) / 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 247),IHEL=1,8) / 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 248),IHEL=1,8) / 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 249),IHEL=1,8) / 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 250),IHEL=1,8) / 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 251),IHEL=1,8) / 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 252),IHEL=1,8) / 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 253),IHEL=1,8) / 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 254),IHEL=1,8) / 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 255),IHEL=1,8) / 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 256),IHEL=1,8) / 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,8) / 1, 2, 3, 4, 5, 6, 7, 8/
      DATA (IDEN(IHEL),IHEL=  1,  1) / 256/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL hvq_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
       
C SF: comment out all instances of multi_channel
c      IF (multi_channel) THEN
c          DO IHEL=1,NGRAPHS
c              amp2(ihel)=0d0
c              jamp2(ihel)=0d0
c          ENDDO
c          DO IHEL=1,int(jamp2(0))
c              jamp2(ihel)=0d0
c          ENDDO
c      ENDIF
      ANS(IPROC) = 0D0
      write(hel_buff,'(16i5)') (0,i=1,nexternal)
      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
          DO IHEL=1,NCOMB
              IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=hvq_MATGG_TTB(P ,NHEL(1,IHEL),JC(1))            
                 ANS(IPROC)=ANS(IPROC)+T
                  IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
                      GOODHEL(IHEL,IPROC)=.TRUE.
                      NGOOD = NGOOD +1
                      IGOOD(NGOOD) = IHEL
C                WRITE(*,*) ngood,IHEL,T
                  ENDIF
              ENDIF
          ENDDO
          JHEL = 1
          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
      ELSE              !hvq_random HELICITY
          DO J=1,ISUM_HEL
              JHEL=JHEL+1
              IF (JHEL .GT. NGOOD) JHEL=1
              HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
              IHEL = IGOOD(JHEL)
              T=hvq_MATGG_TTB(P ,NHEL(1,IHEL),JC(1))            
           ANS(IPROC)=ANS(IPROC)+T*HWGT
          ENDDO
          IF (ISUM_HEL .EQ. 1) THEN
              WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
          ENDIF
      ENDIF
C SF: comment out all instances of multi_channel
c      IF (MULTI_CHANNEL) THEN
c          XTOT=0D0
c          DO IHEL=1,MAPCONFIG(0)
c              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
c          ENDDO
c          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
c      ENDIF
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
C SF: the original name MATRIX has been replaced by hvq_MATGG_TTB
      REAL*8 FUNCTION hvq_MATGG_TTB(P,NHEL,IC)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR hvq_process : g g -> e+ ve b e- ve~ b~  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   3,NEIGEN=  2) 
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      include "genps.inc"
      integer    nexternal
      parameter (nexternal=  8)
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  16, NCOLOR=   2) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(6,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2
C SF: The original coupl.inc has been renamed MEcoupl.inc
      include "MEcoupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            3/                                       
      DATA (CF(i,1  ),i=1  ,2  ) /    16,   -2/                            
C               T[8,5,1,2]                                                 
      DATA Denom(2  )/            3/                                       
      DATA (CF(i,2  ),i=1  ,2  ) /    -2,   16/                            
C               T[8,5,2,1]                                                 
C ----------
C BEGIN CODE
C ----------
      CALL hvq_vxxxxx(P(0,1   ),ZERO ,NHEL(1   ),-1*IC(1   ),W(1,1   ))        
      CALL hvq_vxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL hvq_ixxxxx(P(0,3   ),ZERO ,NHEL(3   ),-1*IC(3   ),W(1,3   ))        
      CALL hvq_oxxxxx(P(0,4   ),ZERO ,NHEL(4   ),+1*IC(4   ),W(1,4   ))        
      CALL hvq_oxxxxx(P(0,5   ),BMASS ,NHEL(5   ),+1*IC(5   ),W(1,5   ))       
      CALL hvq_oxxxxx(P(0,6   ),ZERO ,NHEL(6   ),+1*IC(6   ),W(1,6   ))        
      CALL hvq_ixxxxx(P(0,7   ),ZERO ,NHEL(7   ),-1*IC(7   ),W(1,7   ))        
      CALL hvq_ixxxxx(P(0,8   ),BMASS ,NHEL(8   ),-1*IC(8   ),W(1,8   ))       
      CALL hvq_jioxxx(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,9   ))    
      CALL hvq_fvoxxx(W(1,5   ),W(1,9   ),GWF ,TMASS   ,TWIDTH  ,W(1,10  ))    
      CALL hvq_jioxxx(W(1,7   ),W(1,6   ),GWF ,WMASS   ,WWIDTH  ,W(1,11  ))    
      CALL hvq_fvixxx(W(1,8   ),W(1,11  ),GWF ,TMASS   ,TWIDTH  ,W(1,12  ))    
      CALL hvq_fvoxxx(W(1,10  ),W(1,2   ),GG ,TMASS   ,TWIDTH  ,W(1,13  ))     
      CALL hvq_iovxxx(W(1,12  ),W(1,13  ),W(1,1   ),GG ,AMP(1   ))             
      CALL hvq_fvoxxx(W(1,10  ),W(1,1   ),GG ,TMASS   ,TWIDTH  ,W(1,14  ))     
      CALL hvq_iovxxx(W(1,12  ),W(1,14  ),W(1,2   ),GG ,AMP(2   ))             
      CALL hvq_jvvxxx(W(1,1   ),W(1,2   ),G ,ZERO    ,ZERO    ,W(1,15  ))      
      CALL hvq_fvoxxx(W(1,10  ),W(1,15  ),GG ,TMASS   ,TWIDTH  ,W(1,16  ))     
      CALL hvq_iovxxx(W(1,8   ),W(1,16  ),W(1,11  ),GWF ,AMP(3   ))            
      JAMP(   1) = +AMP(   1)-AMP(   3)
      JAMP(   2) = +AMP(   2)+AMP(   3)
      hvq_MATGG_TTB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          hvq_MATGG_TTB =hvq_MATGG_TTB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C SF: comment out all instances of amp2, used by multi_channel
c      Do I = 1, NGRAPHS
c          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
c      Enddo
c      Do I = 1, NCOLOR
c          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
c      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
c
c End of gg --> ttbar
c
c
c
c
c Begin of qqbar --> ttbar g
c
c
C SF: The following routine is SMATRIX generated by MadEvent, suitably modified
      SUBROUTINE hvq_UUB_TTBG(P1,ANS)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR hvq_process : u u~ -> e+ ve b e- ve~ b~ g  
C  
C Crossing   1 is u u~ -> e+ ve b e- ve~ b~ g  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      Include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 512, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 hvq_MATUUB_TTBG
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
      REAL*8 hwgt, xtot, xtry, xrej, xr, yfrac(0:ncomb)
      INTEGER idum, ngood, igood(ncomb), jhel, j
      LOGICAL warned
      REAL     xran1
      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2

      character*79         hel_buff
C SF: comment out all common blocks
c      common/hvq_to_helicity/  hel_buff

      integer          isum_hel
      logical                    multi_channel
C SF: comment out all common blocks
c      common/hvq_to_matrix/isum_hel, multi_channel
C SF: comment out all instances of mapconfig, used by multi_channel
c      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
c      common/hvq_to_mconfigs/mapconfig, iconfig
      DATA NTRY,IDUM /0,-1/
      DATA xtry, xrej, ngood /0,0,0/
      DATA warned, isum_hel/.false.,0/
      DATA multi_channel/.true./
      SAVE yfrac, igood, IDUM, jhel
      DATA NGRAPHS /    5/          
C SF: comment out all instances of amp2, used by multi_channel
c      DATA jamp2(0) /   4/          
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 129),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 130),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 131),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 132),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 135),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 136),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 137),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 138),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 141),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 142),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 143),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 144),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 147),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 148),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 149),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 150),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 153),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 154),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 155),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 156),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 159),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 160),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 161),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 162),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 165),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 166),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 167),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 168),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 171),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 172),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 173),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 174),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 177),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 178),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 179),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 180),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 183),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 184),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 185),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 186),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 189),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 190),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 191),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 192),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 193),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 194),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 195),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 196),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 197),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 198),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 199),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 200),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 201),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 202),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 203),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 204),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 205),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 206),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 207),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 208),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 209),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 210),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 211),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 212),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 213),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 214),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 215),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 216),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 217),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 218),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 219),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 220),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 221),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 222),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 223),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 224),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 225),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 226),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 227),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 228),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 229),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 230),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 231),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 232),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 233),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 234),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 235),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 236),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 237),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 238),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 239),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 240),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 241),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 242),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 243),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 244),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 245),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 246),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 247),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 248),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 249),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 250),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 251),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 252),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 253),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 254),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 255),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 256),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 257),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 258),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 259),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 260),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 261),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 262),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 263),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 264),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 265),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 266),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 267),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 268),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 269),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 270),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 271),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 272),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 273),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 274),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 275),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 276),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 277),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 278),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 279),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 280),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 281),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 282),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 283),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 284),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 285),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 286),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 287),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 288),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 289),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 290),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 291),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 292),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 293),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 294),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 295),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 296),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 297),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 298),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 299),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 300),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 301),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 302),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 303),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 304),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 305),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 306),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 307),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 308),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 309),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 310),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 311),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 312),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 313),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 314),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 315),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 316),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 317),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 318),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 319),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 320),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 321),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 322),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 323),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 324),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 325),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 326),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 327),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 328),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 329),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 330),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 331),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 332),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 333),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 334),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 335),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 336),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 337),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 338),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 339),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 340),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 341),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 342),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 343),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 344),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 345),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 346),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 347),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 348),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 349),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 350),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 351),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 352),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 353),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 354),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 355),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 356),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 357),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 358),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 359),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 360),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 361),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 362),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 363),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 364),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 365),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 366),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 367),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 368),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 369),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 370),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 371),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 372),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 373),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 374),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 375),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 376),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 377),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 378),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 379),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 380),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 381),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 382),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 383),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 384),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 385),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 386),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 387),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 388),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 389),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 390),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 391),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 392),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 393),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 394),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 395),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 396),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 397),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 398),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 399),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 400),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 401),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 402),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 403),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 404),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 405),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 406),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 407),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 408),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 409),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 410),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 411),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 412),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 413),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 414),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 415),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 416),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 417),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 418),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 419),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 420),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 421),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 422),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 423),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 424),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 425),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 426),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 427),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 428),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 429),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 430),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 431),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 432),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 433),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 434),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 435),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 436),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 437),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 438),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 439),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 440),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 441),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 442),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 443),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 444),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 445),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 446),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 447),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 448),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 449),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 450),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 451),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 452),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 453),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 454),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 455),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 456),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 457),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 458),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 459),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 460),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 461),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 462),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 463),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 464),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 465),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 466),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 467),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 468),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 469),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 470),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 471),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 472),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 473),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 474),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 475),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 476),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 477),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 478),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 479),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 480),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 481),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 482),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 483),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 484),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 485),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 486),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 487),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 488),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 489),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 490),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 491),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 492),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 493),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 494),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 495),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 496),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 497),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 498),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 499),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 500),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 501),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 502),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 503),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 504),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 505),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 506),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 507),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 508),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 509),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 510),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 511),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 512),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,9) / 1, 2, 3, 4, 5, 6, 7, 8, 9/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL hvq_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
       
C SF: comment out all instances of multi_channel
c      IF (multi_channel) THEN
c          DO IHEL=1,NGRAPHS
c              amp2(ihel)=0d0
c              jamp2(ihel)=0d0
c          ENDDO
c          DO IHEL=1,int(jamp2(0))
c              jamp2(ihel)=0d0
c          ENDDO
c      ENDIF
      ANS(IPROC) = 0D0
      write(hel_buff,'(16i5)') (0,i=1,nexternal)
      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
          DO IHEL=1,NCOMB
              IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=hvq_MATUUB_TTBG(P ,NHEL(1,IHEL),JC(1))            
                 ANS(IPROC)=ANS(IPROC)+T
                  IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
                      GOODHEL(IHEL,IPROC)=.TRUE.
                      NGOOD = NGOOD +1
                      IGOOD(NGOOD) = IHEL
C                WRITE(*,*) ngood,IHEL,T
                  ENDIF
              ENDIF
          ENDDO
          JHEL = 1
          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
      ELSE              !hvq_random HELICITY
          DO J=1,ISUM_HEL
              JHEL=JHEL+1
              IF (JHEL .GT. NGOOD) JHEL=1
              HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
              IHEL = IGOOD(JHEL)
              T=hvq_MATUUB_TTBG(P ,NHEL(1,IHEL),JC(1))            
           ANS(IPROC)=ANS(IPROC)+T*HWGT
          ENDDO
          IF (ISUM_HEL .EQ. 1) THEN
              WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
          ENDIF
      ENDIF
C SF: comment out all instances of multi_channel
c      IF (MULTI_CHANNEL) THEN
c          XTOT=0D0
c          DO IHEL=1,MAPCONFIG(0)
c              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
c          ENDDO
c          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
c      ENDIF
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
C SF: the original name MATRIX has been replaced by hvq_MATUUB_TTBG
      REAL*8 FUNCTION hvq_MATUUB_TTBG(P,NHEL,IC)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR hvq_process : u u~ -> e+ ve b e- ve~ b~ g  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   5,NEIGEN=  4) 
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  21, NCOLOR=   4) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(6,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2
C SF: The original coupl.inc has been renamed MEcoupl.inc
      include "MEcoupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            3/                                       
      DATA (CF(i,1  ),i=1  ,4  ) /     8,   -1,    7,   -2/                
C               T[8,2]T[1,5,9]                                             
      DATA Denom(2  )/            3/                                       
      DATA (CF(i,2  ),i=1  ,4  ) /    -1,    8,   -2,    7/                
C               T[8,2,9]T[1,5]                                             
      DATA Denom(3  )/            3/                                       
      DATA (CF(i,3  ),i=1  ,4  ) /     7,   -2,    8,   -1/                
C               T[8,2]T[1,5,9]                                             
      DATA Denom(4  )/            3/                                       
      DATA (CF(i,4  ),i=1  ,4  ) /    -2,    7,   -1,    8/                
C               T[1,5]T[8,2,9]                                             
C ----------
C BEGIN CODE
C ----------
      CALL hvq_ixxxxx(P(0,1   ),ZERO ,NHEL(1   ),+1*IC(1   ),W(1,1   ))        
      CALL hvq_oxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL hvq_ixxxxx(P(0,3   ),ZERO ,NHEL(3   ),-1*IC(3   ),W(1,3   ))        
      CALL hvq_oxxxxx(P(0,4   ),ZERO ,NHEL(4   ),+1*IC(4   ),W(1,4   ))        
      CALL hvq_oxxxxx(P(0,5   ),BMASS ,NHEL(5   ),+1*IC(5   ),W(1,5   ))       
      CALL hvq_oxxxxx(P(0,6   ),ZERO ,NHEL(6   ),+1*IC(6   ),W(1,6   ))        
      CALL hvq_ixxxxx(P(0,7   ),ZERO ,NHEL(7   ),-1*IC(7   ),W(1,7   ))        
      CALL hvq_ixxxxx(P(0,8   ),BMASS ,NHEL(8   ),-1*IC(8   ),W(1,8   ))       
      CALL hvq_vxxxxx(P(0,9   ),ZERO ,NHEL(9   ),+1*IC(9   ),W(1,9   ))        
      CALL hvq_jioxxx(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,10  ))    
      CALL hvq_fvoxxx(W(1,5   ),W(1,10  ),GWF ,TMASS   ,TWIDTH  ,W(1,11  ))    
      CALL hvq_jioxxx(W(1,7   ),W(1,6   ),GWF ,WMASS   ,WWIDTH  ,W(1,12  ))    
      CALL hvq_fvixxx(W(1,8   ),W(1,12  ),GWF ,TMASS   ,TWIDTH  ,W(1,13  ))    
      CALL hvq_fvixxx(W(1,1   ),W(1,9   ),GG ,ZERO    ,ZERO    ,W(1,14  ))     
      CALL hvq_jioxxx(W(1,14  ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,15  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,15  ),GG ,AMP(1   ))             
      CALL hvq_fvoxxx(W(1,2   ),W(1,9   ),GG ,ZERO    ,ZERO    ,W(1,16  ))     
      CALL hvq_jioxxx(W(1,1   ),W(1,16  ),GG ,ZERO    ,ZERO    ,W(1,17  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,17  ),GG ,AMP(2   ))             
      CALL hvq_jioxxx(W(1,1   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,18  ))     
      CALL hvq_fvoxxx(W(1,11  ),W(1,9   ),GG ,TMASS   ,TWIDTH  ,W(1,19  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,19  ),W(1,18  ),GG ,AMP(3   ))             
      CALL hvq_jvvxxx(W(1,9   ),W(1,18  ),G ,ZERO    ,ZERO    ,W(1,20  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,20  ),GG ,AMP(4   ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,18  ),GG ,TMASS   ,TWIDTH  ,W(1,21  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,21  ),W(1,9   ),GG ,AMP(5   ))             
      JAMP(   1) = +AMP(   1)
      JAMP(   2) = +AMP(   2)
      JAMP(   3) = +AMP(   3)+AMP(   4)
      JAMP(   4) = -AMP(   4)+AMP(   5)
      hvq_MATUUB_TTBG = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          hvq_MATUUB_TTBG =hvq_MATUUB_TTBG+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C SF: comment out all instances of amp2, used by multi_channel
c      Do I = 1, NGRAPHS
c          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
c      Enddo
c      Do I = 1, NCOLOR
c          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
c      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
c
c End of qqbar --> ttbar g
c
c
c
c
c Begin of gg --> ttbar g
c
c
C SF: The following routine is SMATRIX generated by MadEvent, suitably modified
      SUBROUTINE hvq_GG_TTBG(P1,ANS)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR hvq_process : g g -> e+ ve b e- ve~ b~ g  
C  
C Crossing   1 is g g -> e+ ve b e- ve~ b~ g  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      Include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 512, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 hvq_MATGG_TTBG
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
      REAL*8 hwgt, xtot, xtry, xrej, xr, yfrac(0:ncomb)
      INTEGER idum, ngood, igood(ncomb), jhel, j
      LOGICAL warned
      REAL     xran1
      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2

      character*79         hel_buff
C SF: comment out all common blocks
c      common/hvq_to_helicity/  hel_buff

      integer          isum_hel
      logical                    multi_channel
C SF: comment out all common blocks
c      common/hvq_to_matrix/isum_hel, multi_channel
C SF: comment out all instances of mapconfig, used by multi_channel
c      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
c      common/hvq_to_mconfigs/mapconfig, iconfig
      DATA NTRY,IDUM /0,-1/
      DATA xtry, xrej, ngood /0,0,0/
      DATA warned, isum_hel/.false.,0/
      DATA multi_channel/.true./
      SAVE yfrac, igood, IDUM, jhel
      DATA NGRAPHS /   18/          
C SF: comment out all instances of amp2, used by multi_channel
c      DATA jamp2(0) /   6/          
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 129),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 130),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 131),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 132),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 135),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 136),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 137),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 138),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 141),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 142),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 143),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 144),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 147),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 148),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 149),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 150),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 153),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 154),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 155),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 156),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 159),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 160),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 161),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 162),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 165),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 166),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 167),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 168),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 171),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 172),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 173),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 174),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 177),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 178),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 179),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 180),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 183),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 184),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 185),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 186),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 189),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 190),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 191),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 192),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 193),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 194),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 195),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 196),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 197),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 198),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 199),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 200),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 201),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 202),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 203),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 204),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 205),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 206),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 207),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 208),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 209),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 210),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 211),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 212),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 213),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 214),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 215),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 216),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 217),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 218),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 219),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 220),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 221),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 222),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 223),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 224),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 225),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 226),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 227),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 228),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 229),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 230),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 231),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 232),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 233),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 234),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 235),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 236),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 237),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 238),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 239),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 240),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 241),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 242),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 243),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 244),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 245),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 246),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 247),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 248),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 249),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 250),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 251),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 252),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 253),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 254),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 255),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 256),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 257),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 258),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 259),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 260),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 261),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 262),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 263),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 264),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 265),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 266),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 267),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 268),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 269),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 270),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 271),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 272),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 273),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 274),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 275),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 276),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 277),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 278),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 279),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 280),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 281),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 282),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 283),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 284),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 285),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 286),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 287),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 288),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 289),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 290),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 291),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 292),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 293),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 294),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 295),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 296),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 297),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 298),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 299),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 300),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 301),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 302),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 303),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 304),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 305),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 306),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 307),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 308),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 309),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 310),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 311),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 312),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 313),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 314),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 315),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 316),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 317),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 318),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 319),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 320),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 321),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 322),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 323),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 324),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 325),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 326),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 327),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 328),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 329),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 330),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 331),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 332),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 333),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 334),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 335),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 336),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 337),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 338),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 339),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 340),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 341),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 342),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 343),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 344),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 345),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 346),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 347),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 348),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 349),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 350),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 351),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 352),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 353),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 354),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 355),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 356),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 357),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 358),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 359),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 360),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 361),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 362),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 363),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 364),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 365),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 366),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 367),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 368),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 369),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 370),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 371),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 372),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 373),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 374),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 375),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 376),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 377),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 378),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 379),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 380),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 381),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 382),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 383),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 384),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 385),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 386),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 387),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 388),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 389),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 390),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 391),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 392),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 393),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 394),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 395),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 396),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 397),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 398),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 399),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 400),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 401),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 402),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 403),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 404),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 405),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 406),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 407),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 408),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 409),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 410),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 411),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 412),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 413),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 414),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 415),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 416),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 417),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 418),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 419),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 420),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 421),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 422),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 423),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 424),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 425),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 426),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 427),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 428),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 429),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 430),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 431),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 432),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 433),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 434),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 435),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 436),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 437),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 438),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 439),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 440),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 441),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 442),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 443),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 444),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 445),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 446),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 447),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 448),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 449),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 450),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 451),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 452),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 453),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 454),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 455),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 456),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 457),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 458),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 459),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 460),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 461),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 462),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 463),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 464),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 465),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 466),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 467),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 468),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 469),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 470),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 471),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 472),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 473),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 474),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 475),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 476),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 477),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 478),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 479),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 480),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 481),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 482),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 483),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 484),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 485),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 486),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 487),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 488),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 489),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 490),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 491),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 492),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 493),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 494),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 495),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 496),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 497),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 498),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 499),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 500),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 501),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 502),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 503),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 504),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 505),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 506),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 507),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 508),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 509),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 510),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 511),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 512),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,9) / 1, 2, 3, 4, 5, 6, 7, 8, 9/
      DATA (IDEN(IHEL),IHEL=  1,  1) / 256/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL hvq_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
       
C SF: comment out all instances of multi_channel
c      IF (multi_channel) THEN
c          DO IHEL=1,NGRAPHS
c              amp2(ihel)=0d0
c              jamp2(ihel)=0d0
c          ENDDO
c          DO IHEL=1,int(jamp2(0))
c              jamp2(ihel)=0d0
c          ENDDO
c      ENDIF
      ANS(IPROC) = 0D0
      write(hel_buff,'(16i5)') (0,i=1,nexternal)
      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
          DO IHEL=1,NCOMB
              IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=hvq_MATGG_TTBG(P ,NHEL(1,IHEL),JC(1))            
                 ANS(IPROC)=ANS(IPROC)+T
                  IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
                      GOODHEL(IHEL,IPROC)=.TRUE.
                      NGOOD = NGOOD +1
                      IGOOD(NGOOD) = IHEL
C                WRITE(*,*) ngood,IHEL,T
                  ENDIF
              ENDIF
          ENDDO
          JHEL = 1
          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
      ELSE              !hvq_random HELICITY
          DO J=1,ISUM_HEL
              JHEL=JHEL+1
              IF (JHEL .GT. NGOOD) JHEL=1
              HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
              IHEL = IGOOD(JHEL)
              T=hvq_MATGG_TTBG(P ,NHEL(1,IHEL),JC(1))            
           ANS(IPROC)=ANS(IPROC)+T*HWGT
          ENDDO
          IF (ISUM_HEL .EQ. 1) THEN
              WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
          ENDIF
      ENDIF
C SF: comment out all instances of multi_channel
c      IF (MULTI_CHANNEL) THEN
c          XTOT=0D0
c          DO IHEL=1,MAPCONFIG(0)
c              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
c          ENDDO
c          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
c      ENDIF
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
C SF: the original name MATRIX has been replaced by hvq_MATGG_TTBG
      REAL*8 FUNCTION hvq_MATGG_TTBG(P,NHEL,IC)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR hvq_process : g g -> e+ ve b e- ve~ b~ g  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=  18,NEIGEN=  6) 
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  36, NCOLOR=   6) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(6,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2
C SF: The original coupl.inc has been renamed MEcoupl.inc
      include "MEcoupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            9/                                       
      DATA (CF(i,1  ),i=1  ,6  ) /    64,   -8,    1,    1,   10,   -8/    
C               T[8,5,9,1,2]                                               
      DATA Denom(2  )/            9/                                       
      DATA (CF(i,2  ),i=1  ,6  ) /    -8,   64,   -8,   10,    1,    1/    
C               T[8,5,1,9,2]                                               
      DATA Denom(3  )/            9/                                       
      DATA (CF(i,3  ),i=1  ,6  ) /     1,   -8,   64,    1,   -8,   10/    
C               T[8,5,1,2,9]                                               
      DATA Denom(4  )/            9/                                       
      DATA (CF(i,4  ),i=1  ,6  ) /     1,   10,    1,   64,   -8,   -8/    
C               T[8,5,2,9,1]                                               
      DATA Denom(5  )/            9/                                       
      DATA (CF(i,5  ),i=1  ,6  ) /    10,    1,   -8,   -8,   64,    1/    
C               T[8,5,2,1,9]                                               
      DATA Denom(6  )/            9/                                       
      DATA (CF(i,6  ),i=1  ,6  ) /    -8,    1,   10,   -8,    1,   64/    
C               T[8,5,9,2,1]                                               
C ----------
C BEGIN CODE
C ----------
      CALL hvq_vxxxxx(P(0,1   ),ZERO ,NHEL(1   ),-1*IC(1   ),W(1,1   ))        
      CALL hvq_vxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL hvq_ixxxxx(P(0,3   ),ZERO ,NHEL(3   ),-1*IC(3   ),W(1,3   ))        
      CALL hvq_oxxxxx(P(0,4   ),ZERO ,NHEL(4   ),+1*IC(4   ),W(1,4   ))        
      CALL hvq_oxxxxx(P(0,5   ),BMASS ,NHEL(5   ),+1*IC(5   ),W(1,5   ))       
      CALL hvq_oxxxxx(P(0,6   ),ZERO ,NHEL(6   ),+1*IC(6   ),W(1,6   ))        
      CALL hvq_ixxxxx(P(0,7   ),ZERO ,NHEL(7   ),-1*IC(7   ),W(1,7   ))        
      CALL hvq_ixxxxx(P(0,8   ),BMASS ,NHEL(8   ),-1*IC(8   ),W(1,8   ))       
      CALL hvq_vxxxxx(P(0,9   ),ZERO ,NHEL(9   ),+1*IC(9   ),W(1,9   ))        
      CALL hvq_jioxxx(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,10  ))    
      CALL hvq_fvoxxx(W(1,5   ),W(1,10  ),GWF ,TMASS   ,TWIDTH  ,W(1,11  ))    
      CALL hvq_jioxxx(W(1,7   ),W(1,6   ),GWF ,WMASS   ,WWIDTH  ,W(1,12  ))    
      CALL hvq_fvixxx(W(1,8   ),W(1,12  ),GWF ,TMASS   ,TWIDTH  ,W(1,13  ))    
      CALL hvq_jvvxxx(W(1,9   ),W(1,1   ),G ,ZERO    ,ZERO    ,W(1,14  ))      
      CALL hvq_fvoxxx(W(1,11  ),W(1,2   ),GG ,TMASS   ,TWIDTH  ,W(1,15  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,15  ),W(1,14  ),GG ,AMP(1   ))             
      CALL hvq_jvvxxx(W(1,9   ),W(1,2   ),G ,ZERO    ,ZERO    ,W(1,16  ))      
      CALL hvq_fvoxxx(W(1,11  ),W(1,16  ),GG ,TMASS   ,TWIDTH  ,W(1,17  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,17  ),W(1,1   ),GG ,AMP(2   ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,9   ),GG ,TMASS   ,TWIDTH  ,W(1,18  ))     
      CALL hvq_fvoxxx(W(1,18  ),W(1,2   ),GG ,TMASS   ,TWIDTH  ,W(1,19  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,19  ),W(1,1   ),GG ,AMP(3   ))             
      CALL hvq_fvixxx(W(1,13  ),W(1,1   ),GG ,TMASS   ,TWIDTH  ,W(1,20  ))     
      CALL hvq_iovxxx(W(1,20  ),W(1,15  ),W(1,9   ),GG ,AMP(4   ))             
      CALL hvq_fvixxx(W(1,13  ),W(1,9   ),GG ,TMASS   ,TWIDTH  ,W(1,21  ))     
      CALL hvq_iovxxx(W(1,21  ),W(1,15  ),W(1,1   ),GG ,AMP(5   ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,14  ),GG ,TMASS   ,TWIDTH  ,W(1,22  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,22  ),W(1,2   ),GG ,AMP(6   ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,1   ),GG ,TMASS   ,TWIDTH  ,W(1,23  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,23  ),W(1,16  ),GG ,AMP(7   ))             
      CALL hvq_fvoxxx(W(1,18  ),W(1,1   ),GG ,TMASS   ,TWIDTH  ,W(1,24  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,24  ),W(1,2   ),GG ,AMP(8   ))             
      CALL hvq_fvixxx(W(1,13  ),W(1,2   ),GG ,TMASS   ,TWIDTH  ,W(1,25  ))     
      CALL hvq_iovxxx(W(1,25  ),W(1,23  ),W(1,9   ),GG ,AMP(9   ))             
      CALL hvq_iovxxx(W(1,21  ),W(1,23  ),W(1,2   ),GG ,AMP(10  ))             
      CALL hvq_jvvxxx(W(1,14  ),W(1,2   ),G ,ZERO    ,ZERO    ,W(1,26  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,26  ),GG ,AMP(11  ))             
      CALL hvq_jvvxxx(W(1,1   ),W(1,16  ),G ,ZERO    ,ZERO    ,W(1,27  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,27  ),GG ,AMP(12  ))             
      CALL hvq_jvvxxx(W(1,1   ),W(1,2   ),G ,ZERO    ,ZERO    ,W(1,28  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,18  ),W(1,28  ),GG ,AMP(13  ))             
      CALL hvq_jvvxxx(W(1,9   ),W(1,28  ),G ,ZERO    ,ZERO    ,W(1,29  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,29  ),GG ,AMP(14  ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,28  ),GG ,TMASS   ,TWIDTH  ,W(1,30  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,30  ),W(1,9   ),GG ,AMP(15  ))             
      CALL hvq_jgggxx(W(1,1   ),W(1,2   ),W(1,9   ),G ,W(1,31  ))              
      CALL hvq_fvoxxx(W(1,11  ),W(1,31  ),GG ,TMASS   ,TWIDTH  ,W(1,32  ))     
      CALL hvq_iovxxx(W(1,8   ),W(1,32  ),W(1,12  ),GWF ,AMP(16  ))            
      CALL hvq_jgggxx(W(1,9   ),W(1,1   ),W(1,2   ),G ,W(1,33  ))              
      CALL hvq_fvoxxx(W(1,11  ),W(1,33  ),GG ,TMASS   ,TWIDTH  ,W(1,34  ))     
      CALL hvq_iovxxx(W(1,8   ),W(1,34  ),W(1,12  ),GWF ,AMP(17  ))            
      CALL hvq_jgggxx(W(1,2   ),W(1,9   ),W(1,1   ),G ,W(1,35  ))              
      CALL hvq_fvoxxx(W(1,11  ),W(1,35  ),GG ,TMASS   ,TWIDTH  ,W(1,36  ))     
      CALL hvq_iovxxx(W(1,8   ),W(1,36  ),W(1,12  ),GWF ,AMP(18  ))            
      JAMP(   1) = -AMP(   1)+AMP(   5)+AMP(  11)+AMP(  14)-AMP(  15)
     &             +AMP(  16)-AMP(  17)
      JAMP(   2) = +AMP(   1)-AMP(   2)+AMP(   4)-AMP(  11)+AMP(  12)
     &             +AMP(  17)-AMP(  18)
      JAMP(   3) = +AMP(   2)+AMP(   3)-AMP(  12)-AMP(  13)-AMP(  14)
     &             -AMP(  16)+AMP(  18)
      JAMP(   4) = -AMP(   6)+AMP(   7)+AMP(   9)-AMP(  11)+AMP(  12)
     &             +AMP(  17)-AMP(  18)
      JAMP(   5) = +AMP(   6)+AMP(   8)+AMP(  11)+AMP(  13)+AMP(  14)
     &             +AMP(  16)-AMP(  17)
      JAMP(   6) = -AMP(   7)+AMP(  10)-AMP(  12)-AMP(  14)+AMP(  15)
     &             -AMP(  16)+AMP(  18)
      hvq_MATGG_TTBG = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          hvq_MATGG_TTBG =hvq_MATGG_TTBG+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C SF: comment out all instances of amp2, used by multi_channel
c      Do I = 1, NGRAPHS
c          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
c      Enddo
c      Do I = 1, NCOLOR
c          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
c      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
c
c End of gg --> ttbar g
c
c
c
c
c Begin of qg --> ttbar q
c
c
C SF: The following routine is SMATRIX generated by MadEvent, suitably modified
      SUBROUTINE hvq_UG_TTBU(P1,ANS)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR hvq_process : u g -> e+ ve b e- ve~ b~ u  
C  
C Crossing   1 is u g -> e+ ve b e- ve~ b~ u  
      IMPLICIT NONE
C  
C CONSTANTS
C  
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      Include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB= 512, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 hvq_MATUG_TTBU
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      INTEGER NGRAPHS
      REAL*8 hwgt, xtot, xtry, xrej, xr, yfrac(0:ncomb)
      INTEGER idum, ngood, igood(ncomb), jhel, j
      LOGICAL warned
      REAL     xran1
      EXTERNAL xran1
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2

      character*79         hel_buff
C SF: comment out all common blocks
c      common/hvq_to_helicity/  hel_buff

      integer          isum_hel
      logical                    multi_channel
C SF: comment out all common blocks
c      common/hvq_to_matrix/isum_hel, multi_channel
C SF: comment out all instances of mapconfig, used by multi_channel
c      INTEGER MAPCONFIG(0:LMAXCONFIGS), ICONFIG
c      common/hvq_to_mconfigs/mapconfig, iconfig
      DATA NTRY,IDUM /0,-1/
      DATA xtry, xrej, ngood /0,0,0/
      DATA warned, isum_hel/.false.,0/
      DATA multi_channel/.true./
      SAVE yfrac, igood, IDUM, jhel
      DATA NGRAPHS /    5/          
C SF: comment out all instances of amp2, used by multi_channel
c      DATA jamp2(0) /   4/          
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1,9) /-1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1,9) /-1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1,9) /-1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1,9) /-1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1,9) /-1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1,9) /-1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1,9) /-1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1,9) /-1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1,9) /-1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1,9) /-1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1,9) /-1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1,9) /-1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  49),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  50),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  51),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  52),IHEL=1,9) /-1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  53),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  54),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  55),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  56),IHEL=1,9) /-1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  57),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  58),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  59),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  60),IHEL=1,9) /-1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  61),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  62),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  63),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  64),IHEL=1,9) /-1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  65),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  66),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  67),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  68),IHEL=1,9) /-1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  69),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  70),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  71),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  72),IHEL=1,9) /-1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  73),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  74),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  75),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  76),IHEL=1,9) /-1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  77),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  78),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  79),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  80),IHEL=1,9) /-1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  81),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  82),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  83),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  84),IHEL=1,9) /-1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  85),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  86),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  87),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  88),IHEL=1,9) /-1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  89),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  90),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  91),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  92),IHEL=1,9) /-1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  93),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  94),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  95),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  96),IHEL=1,9) /-1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  97),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  98),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  99),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 100),IHEL=1,9) /-1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 101),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 102),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 103),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 104),IHEL=1,9) /-1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 105),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 106),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 107),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 108),IHEL=1,9) /-1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 109),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 110),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 111),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 112),IHEL=1,9) /-1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 113),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 114),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 115),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 116),IHEL=1,9) /-1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 117),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 118),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 119),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 120),IHEL=1,9) /-1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 121),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 122),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 123),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 124),IHEL=1,9) /-1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 125),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 126),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 127),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 128),IHEL=1,9) /-1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 129),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 130),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 131),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 132),IHEL=1,9) /-1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 133),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 134),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 135),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 136),IHEL=1,9) /-1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 137),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 138),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 139),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 140),IHEL=1,9) /-1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 141),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 142),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 143),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 144),IHEL=1,9) /-1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 145),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 146),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 147),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 148),IHEL=1,9) /-1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 149),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 150),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 151),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 152),IHEL=1,9) /-1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 153),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 154),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 155),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 156),IHEL=1,9) /-1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 157),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 158),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 159),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 160),IHEL=1,9) /-1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 161),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 162),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 163),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 164),IHEL=1,9) /-1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 165),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 166),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 167),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 168),IHEL=1,9) /-1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 169),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 170),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 171),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 172),IHEL=1,9) /-1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 173),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 174),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 175),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 176),IHEL=1,9) /-1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 177),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 178),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 179),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 180),IHEL=1,9) /-1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 181),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 182),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 183),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 184),IHEL=1,9) /-1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 185),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 186),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 187),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 188),IHEL=1,9) /-1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 189),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 190),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 191),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 192),IHEL=1,9) /-1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 193),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 194),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 195),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 196),IHEL=1,9) /-1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 197),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 198),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 199),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 200),IHEL=1,9) /-1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 201),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 202),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 203),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 204),IHEL=1,9) /-1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 205),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 206),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 207),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 208),IHEL=1,9) /-1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 209),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 210),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 211),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 212),IHEL=1,9) /-1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 213),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 214),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 215),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 216),IHEL=1,9) /-1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 217),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 218),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 219),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 220),IHEL=1,9) /-1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 221),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 222),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 223),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 224),IHEL=1,9) /-1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 225),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 226),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 227),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 228),IHEL=1,9) /-1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 229),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 230),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 231),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 232),IHEL=1,9) /-1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 233),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 234),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 235),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 236),IHEL=1,9) /-1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 237),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 238),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 239),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 240),IHEL=1,9) /-1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 241),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 242),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 243),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 244),IHEL=1,9) /-1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 245),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 246),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 247),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 248),IHEL=1,9) /-1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 249),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 250),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 251),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 252),IHEL=1,9) /-1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 253),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 254),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 255),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 256),IHEL=1,9) /-1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 257),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 258),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 259),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 260),IHEL=1,9) / 1,-1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 261),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 262),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 263),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 264),IHEL=1,9) / 1,-1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 265),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 266),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 267),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 268),IHEL=1,9) / 1,-1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 269),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 270),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 271),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 272),IHEL=1,9) / 1,-1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 273),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 274),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 275),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 276),IHEL=1,9) / 1,-1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 277),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 278),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 279),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 280),IHEL=1,9) / 1,-1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 281),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 282),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 283),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 284),IHEL=1,9) / 1,-1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 285),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 286),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 287),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 288),IHEL=1,9) / 1,-1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 289),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 290),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 291),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 292),IHEL=1,9) / 1,-1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 293),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 294),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 295),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 296),IHEL=1,9) / 1,-1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 297),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 298),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 299),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 300),IHEL=1,9) / 1,-1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 301),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 302),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 303),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 304),IHEL=1,9) / 1,-1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 305),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 306),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 307),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 308),IHEL=1,9) / 1,-1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 309),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 310),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 311),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 312),IHEL=1,9) / 1,-1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 313),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 314),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 315),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 316),IHEL=1,9) / 1,-1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 317),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 318),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 319),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 320),IHEL=1,9) / 1,-1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 321),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 322),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 323),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 324),IHEL=1,9) / 1,-1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 325),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 326),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 327),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 328),IHEL=1,9) / 1,-1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 329),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 330),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 331),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 332),IHEL=1,9) / 1,-1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 333),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 334),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 335),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 336),IHEL=1,9) / 1,-1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 337),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 338),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 339),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 340),IHEL=1,9) / 1,-1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 341),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 342),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 343),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 344),IHEL=1,9) / 1,-1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 345),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 346),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 347),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 348),IHEL=1,9) / 1,-1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 349),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 350),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 351),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 352),IHEL=1,9) / 1,-1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 353),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 354),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 355),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 356),IHEL=1,9) / 1,-1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 357),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 358),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 359),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 360),IHEL=1,9) / 1,-1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 361),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 362),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 363),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 364),IHEL=1,9) / 1,-1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 365),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 366),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 367),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 368),IHEL=1,9) / 1,-1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 369),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 370),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 371),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 372),IHEL=1,9) / 1,-1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 373),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 374),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 375),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 376),IHEL=1,9) / 1,-1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 377),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 378),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 379),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 380),IHEL=1,9) / 1,-1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 381),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 382),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 383),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 384),IHEL=1,9) / 1,-1, 1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 385),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 386),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 387),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 388),IHEL=1,9) / 1, 1,-1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 389),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 390),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 391),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 392),IHEL=1,9) / 1, 1,-1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 393),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 394),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 395),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 396),IHEL=1,9) / 1, 1,-1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 397),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 398),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 399),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 400),IHEL=1,9) / 1, 1,-1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 401),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 402),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 403),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 404),IHEL=1,9) / 1, 1,-1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 405),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 406),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 407),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 408),IHEL=1,9) / 1, 1,-1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 409),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 410),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 411),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 412),IHEL=1,9) / 1, 1,-1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 413),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 414),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 415),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 416),IHEL=1,9) / 1, 1,-1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 417),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 418),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 419),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 420),IHEL=1,9) / 1, 1,-1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 421),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 422),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 423),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 424),IHEL=1,9) / 1, 1,-1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 425),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 426),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 427),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 428),IHEL=1,9) / 1, 1,-1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 429),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 430),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 431),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 432),IHEL=1,9) / 1, 1,-1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 433),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 434),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 435),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 436),IHEL=1,9) / 1, 1,-1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 437),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 438),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 439),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 440),IHEL=1,9) / 1, 1,-1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 441),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 442),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 443),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 444),IHEL=1,9) / 1, 1,-1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 445),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 446),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 447),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 448),IHEL=1,9) / 1, 1,-1, 1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 449),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 450),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 451),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 452),IHEL=1,9) / 1, 1, 1,-1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 453),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 454),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 455),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 456),IHEL=1,9) / 1, 1, 1,-1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 457),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 458),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 459),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 460),IHEL=1,9) / 1, 1, 1,-1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 461),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 462),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 463),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 464),IHEL=1,9) / 1, 1, 1,-1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 465),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 466),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 467),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 468),IHEL=1,9) / 1, 1, 1,-1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 469),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 470),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 471),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 472),IHEL=1,9) / 1, 1, 1,-1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 473),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 474),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 475),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 476),IHEL=1,9) / 1, 1, 1,-1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 477),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 478),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 479),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 480),IHEL=1,9) / 1, 1, 1,-1, 1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 481),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 482),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 483),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 484),IHEL=1,9) / 1, 1, 1, 1,-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 485),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 486),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 487),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 488),IHEL=1,9) / 1, 1, 1, 1,-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 489),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 490),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 491),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 492),IHEL=1,9) / 1, 1, 1, 1,-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 493),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 494),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 495),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 496),IHEL=1,9) / 1, 1, 1, 1,-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL, 497),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL, 498),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL, 499),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL, 500),IHEL=1,9) / 1, 1, 1, 1, 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL, 501),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL, 502),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL, 503),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL, 504),IHEL=1,9) / 1, 1, 1, 1, 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL, 505),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL, 506),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL, 507),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL, 508),IHEL=1,9) / 1, 1, 1, 1, 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL, 509),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL, 510),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL, 511),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL, 512),IHEL=1,9) / 1, 1, 1, 1, 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1,9) / 1, 2, 3, 4, 5, 6, 7, 8, 9/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  96/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL hvq_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
       
C SF: comment out all instances of multi_channel
c      IF (multi_channel) THEN
c          DO IHEL=1,NGRAPHS
c              amp2(ihel)=0d0
c              jamp2(ihel)=0d0
c          ENDDO
c          DO IHEL=1,int(jamp2(0))
c              jamp2(ihel)=0d0
c          ENDDO
c      ENDIF
      ANS(IPROC) = 0D0
      write(hel_buff,'(16i5)') (0,i=1,nexternal)
      IF (ISUM_HEL .EQ. 0 .OR. NTRY .LT. 10) THEN
          DO IHEL=1,NCOMB
              IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=hvq_MATUG_TTBU(P ,NHEL(1,IHEL),JC(1))            
                 ANS(IPROC)=ANS(IPROC)+T
                  IF (T .GT. 0D0 .AND. .NOT. GOODHEL(IHEL,IPROC)) THEN
                      GOODHEL(IHEL,IPROC)=.TRUE.
                      NGOOD = NGOOD +1
                      IGOOD(NGOOD) = IHEL
C                WRITE(*,*) ngood,IHEL,T
                  ENDIF
              ENDIF
          ENDDO
          JHEL = 1
          ISUM_HEL=MIN(ISUM_HEL,NGOOD)
      ELSE              !hvq_random HELICITY
          DO J=1,ISUM_HEL
              JHEL=JHEL+1
              IF (JHEL .GT. NGOOD) JHEL=1
              HWGT = REAL(NGOOD)/REAL(ISUM_HEL)
              IHEL = IGOOD(JHEL)
              T=hvq_MATUG_TTBU(P ,NHEL(1,IHEL),JC(1))            
           ANS(IPROC)=ANS(IPROC)+T*HWGT
          ENDDO
          IF (ISUM_HEL .EQ. 1) THEN
              WRITE(HEL_BUFF,'(16i5)')(NHEL(i,IHEL),i=1,nexternal)
          ENDIF
      ENDIF
C SF: comment out all instances of multi_channel
c      IF (MULTI_CHANNEL) THEN
c          XTOT=0D0
c          DO IHEL=1,MAPCONFIG(0)
c              XTOT=XTOT+AMP2(MAPCONFIG(IHEL))
c          ENDDO
c          ANS(IPROC)=ANS(IPROC)*AMP2(MAPCONFIG(ICONFIG))/XTOT
c      ENDIF
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
C SF: the original name MATRIX has been replaced by hvq_MATUG_TTBU
      REAL*8 FUNCTION hvq_MATUG_TTBU(P,NHEL,IC)
C  
C Generated by MadGraph II Version 3.90. Updated 08/26/05               
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR hvq_process : u g -> e+ ve b e- ve~ b~ u  
C  
      IMPLICIT NONE
C  
C CONSTANTS
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   5,NEIGEN=  4) 
C SF: replace the following include with the explicit inclusion of
c nexternal.inc, originally included by genps.inc
c      include "genps.inc"
      integer    nexternal
      parameter (nexternal=  9)
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=  21, NCOLOR=   4) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(6,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
C SF: comment out all instances of amp2, used by multi_channel
c      Double Precision amp2(maxamps), jamp2(0:maxamps)
c      common/hvq_to_amps/  amp2,       jamp2
C SF: The original coupl.inc has been renamed MEcoupl.inc
      include "MEcoupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            3/                                       
      DATA (CF(i,1  ),i=1  ,4  ) /     8,   -1,   -2,    7/                
C               T[8,9]T[1,5,2]                                             
      DATA Denom(2  )/            3/                                       
      DATA (CF(i,2  ),i=1  ,4  ) /    -1,    8,    7,   -2/                
C               T[1,5]T[8,9,2]                                             
      DATA Denom(3  )/            3/                                       
      DATA (CF(i,3  ),i=1  ,4  ) /    -2,    7,    8,   -1/                
C               T[8,9,2]T[1,5]                                             
      DATA Denom(4  )/            3/                                       
      DATA (CF(i,4  ),i=1  ,4  ) /     7,   -2,   -1,    8/                
C               T[8,9]T[1,5,2]                                             
C ----------
C BEGIN CODE
C ----------
      CALL hvq_ixxxxx(P(0,1   ),ZERO ,NHEL(1   ),+1*IC(1   ),W(1,1   ))        
      CALL hvq_vxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL hvq_ixxxxx(P(0,3   ),ZERO ,NHEL(3   ),-1*IC(3   ),W(1,3   ))        
      CALL hvq_oxxxxx(P(0,4   ),ZERO ,NHEL(4   ),+1*IC(4   ),W(1,4   ))        
      CALL hvq_oxxxxx(P(0,5   ),BMASS ,NHEL(5   ),+1*IC(5   ),W(1,5   ))       
      CALL hvq_oxxxxx(P(0,6   ),ZERO ,NHEL(6   ),+1*IC(6   ),W(1,6   ))        
      CALL hvq_ixxxxx(P(0,7   ),ZERO ,NHEL(7   ),-1*IC(7   ),W(1,7   ))        
      CALL hvq_ixxxxx(P(0,8   ),BMASS ,NHEL(8   ),-1*IC(8   ),W(1,8   ))       
      CALL hvq_oxxxxx(P(0,9   ),ZERO ,NHEL(9   ),+1*IC(9   ),W(1,9   ))        
      CALL hvq_jioxxx(W(1,3   ),W(1,4   ),GWF ,WMASS   ,WWIDTH  ,W(1,10  ))    
      CALL hvq_fvoxxx(W(1,5   ),W(1,10  ),GWF ,TMASS   ,TWIDTH  ,W(1,11  ))    
      CALL hvq_jioxxx(W(1,7   ),W(1,6   ),GWF ,WMASS   ,WWIDTH  ,W(1,12  ))    
      CALL hvq_fvixxx(W(1,8   ),W(1,12  ),GWF ,TMASS   ,TWIDTH  ,W(1,13  ))    
      CALL hvq_jioxxx(W(1,1   ),W(1,9   ),GG ,ZERO    ,ZERO    ,W(1,14  ))     
      CALL hvq_fvoxxx(W(1,11  ),W(1,2   ),GG ,TMASS   ,TWIDTH  ,W(1,15  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,15  ),W(1,14  ),GG ,AMP(1   ))             
      CALL hvq_fvoxxx(W(1,11  ),W(1,14  ),GG ,TMASS   ,TWIDTH  ,W(1,16  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,16  ),W(1,2   ),GG ,AMP(2   ))             
      CALL hvq_jvvxxx(W(1,14  ),W(1,2   ),G ,ZERO    ,ZERO    ,W(1,17  ))      
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,17  ),GG ,AMP(3   ))             
      CALL hvq_fvoxxx(W(1,9   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,18  ))     
      CALL hvq_jioxxx(W(1,1   ),W(1,18  ),GG ,ZERO    ,ZERO    ,W(1,19  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,19  ),GG ,AMP(4   ))             
      CALL hvq_fvixxx(W(1,1   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,20  ))     
      CALL hvq_jioxxx(W(1,20  ),W(1,9   ),GG ,ZERO    ,ZERO    ,W(1,21  ))     
      CALL hvq_iovxxx(W(1,13  ),W(1,11  ),W(1,21  ),GG ,AMP(5   ))             
      JAMP(   1) = +AMP(   1)-AMP(   3)
      JAMP(   2) = +AMP(   2)+AMP(   3)
      JAMP(   3) = +AMP(   4)
      JAMP(   4) = +AMP(   5)
      hvq_MATUG_TTBU = 0.D0   
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          hvq_MATUG_TTBU =hvq_MATUG_TTBU+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C SF: comment out all instances of amp2, used by multi_channel
c      Do I = 1, NGRAPHS
c          amp2(i)=amp2(i)+amp(i)*dconjg(amp(i))
c      Enddo
c      Do I = 1, NCOLOR
c          Jamp2(i)=Jamp2(i)+Jamp(i)*dconjg(Jamp(i))
c      Enddo
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
c
c End of qg --> ttbar q
c
c
c
c This file collects all the files of the HELAS package as obtained from
c MadEvent, except those listed in packHELAS.com.
c
c The lines relevant to preprocessor instructions, to be found between
c #ifdef and #endif in the original codes, have been eliminated by hand.
c They are affected by trivial programming bugs (as one can see by commenting
c out #ifdef and #endif), and are therefore apparently not used, since the 
c compilation usually runs smoothly, which is equivalent to setting
c HELAS_CHECK=0
c
      subroutine hvq_boostx(p,q , pboost)
c
c This subroutine performs the Lorentz boost of a four-momentum.  The
c momentum p is assumed to be given in the rest frame of q.  pboost is
c the momentum p boosted to the frame in which q is given.  q must be a
c timelike momentum.
c
c input:
c       real    p(0:3)         : four-momentum p in the q rest  frame
c       real    q(0:3)         : four-momentum q in the boosted frame
c
c output:
c       real    pboost(0:3)    : four-momentum p in the boosted frame
c
      implicit none
      double precision p(0:3),q(0:3),pboost(0:3),pq,qq,m,lf

      double precision rZero
      parameter( rZero = 0.0d0 )

c
      qq = q(1)**2+q(2)**2+q(3)**2

      if ( qq.ne.rZero ) then
         pq = p(1)*q(1)+p(2)*q(2)+p(3)*q(3)
         m = sqrt(q(0)**2-qq)
         lf = ((q(0)-m)*pq/qq+p(0))/m
         pboost(0) = (p(0)*q(0)+pq)/m
         pboost(1) =  p(1)+q(1)*lf
         pboost(2) =  p(2)+q(2)*lf
         pboost(3) =  p(3)+q(3)*lf
      else
         pboost(0) = p(0)
         pboost(1) = p(1)
         pboost(2) = p(2)
         pboost(3) = p(3)
      endif
c
      return
      end
      subroutine hvq_eaixxx(eb,ea,shlf,chlf,phi,nhe,nha , eai)
c
c This subroutine computes an off-shell electron wavefunction after
c emitting a photon from the electron beam, with a special care for the
c small angle region.  The momenta are measured in the laboratory frame,
c where the e- beam is along the positive z axis.
c
c input:
c       real    eb             : energy (GeV)    of beam  e-
c       real    ea             : energy (GeV)    of final photon
c       real    shlf           : sin(theta/2)    of final photon
c       real    chlf           : cos(theta/2)    of final photon
c       real    phi            : azimuthal angle of final photon
c       integer nhe  = -1 or 1 : helicity        of beam  e-
c       integer nha  = -1 or 1 : helicity        of final photon
c
c output:
c       complex eai(6)         : off-shell electron             |e',A,e>
c
      implicit none
      double complex eai(6),phs
      double precision eb,ea,shlf,chlf,phi,alpha,gal,rnhe,x,c,s,d
      double precision coeff,xnnp,xnnm,snp,csp
      integer nhe,nha,nn

      double precision rHalf, rOne, rTwo, rFour, rOte
      double precision rPi, rIalph
      parameter( rHalf = 0.5d0, rOne = 1.0d0, rTwo = 2.0d0 )
      parameter( rFour = 4.0d0, rOte = 128.9d0 )
      parameter( rPi = 3.14159265358979323846d0 )
      parameter( rIalph = 137.0359895d0 )

      double precision me
      parameter( me = 0.510998902d-3 )

c
      alpha = rOne/rOte
      gal = sqrt(alpha*rFour*rPi)

      nn = nha*nhe
      rnhe = nhe
      x = ea/eb
      c = (chlf+shlf)*(chlf-shlf)
      s = rTwo*chlf*shlf
      d = -rOne/(ea*eb*(rFour*shlf**2+(me/eb)**2*c))
      coeff = -nn*gal*sqrt(eb)*d
      xnnp = x*(1+nn)
      xnnm = x*(1-nn)
      snp = sin(phi)
      csp = cos(phi)
      phs = dcmplx( csp, rnhe*snp )

      eai((5-3*nhe)/2) = -rnhe*coeff*me*s*(rOne+xnnp*rHalf)
      eai((5-nhe)/2)   =  xnnp*coeff*me*chlf**2*phs
      eai((5+nhe)/2)   =  rnhe*coeff*eb*s*(-rTwo+xnnm)
      eai((5+3*nhe)/2) =  xnnm*coeff*eb*shlf**2*phs*rTwo

      eai(5) =  eb*dcmplx( rOne-x, rOne-x*c )
      eai(6) = -eb*x*s*dcmplx( csp, snp )
c
      return
      end
      subroutine hvq_eaoxxx(eb,ea,shlf,chlf,phi,nhe,nha , eao)
c
c This subroutine computes an off-shell positron wavefunction after
c emitting a photon from the positron beam, with a special care for the
c small angle region.  The momenta are measured in the laboratory frame,
c where the e+ beam is along the negative z axis.
c
c input:
c       real    eb             : energy (GeV)    of beam  e+
c       real    ea             : energy (GeV)    of final photon
c       real    shlf           : sin(theta/2)    of final photon
c       real    chlf           : cos(theta/2)    of final photon
c       real    phi            : azimuthal angle of final photon
c       integer nhe  = -1 or 1 : helicity        of beam  e+
c       integer nha  = -1 or 1 : helicity        of final photon
c
c output:
c       complex eao(6)         : off-shell positron             <e,A,e'|
c
      implicit none
      double complex eao(6),phs
      double precision eb,ea,shlf,chlf,phi,alpha,gal,rnhe,x,c,s,d
      double precision coeff,xnnp,xnnm,snp,csp
      integer nhe,nha,nn

      double precision rHalf, rOne, rTwo, rFour, rOte
      double precision rPi, rIalph
      parameter( rHalf = 0.5d0, rOne = 1.0d0, rTwo = 2.0d0 )
      parameter( rFour = 4.0d0, rOte = 128.9d0 )
      parameter( rPi = 3.14159265358979323846d0 )
      parameter( rIalph = 137.0359895d0 )

      double precision me
      parameter( me = 0.510998902d-3 )

c

      alpha = rOne/rOte
      gal = sqrt(alpha*rFour*rPi)

      nn = nha*nhe
      rnhe = nhe
      x = ea/eb
      c = (chlf+shlf)*(chlf-shlf)
      s = rTwo*chlf*shlf
      d = -rOne/(ea*eb*(rFour*chlf**2-(me/eb)**2*c))
      coeff = nn*gal*sqrt(eb)*d
      xnnp = x*(1+nn)
      xnnm = x*(1-nn)
      snp = sin(phi)
      csp = cos(phi)
      phs = dcmplx( csp, -rnhe*snp )

      eao((5-3*nhe)/2) =               coeff*me*s*(rOne+xnnp*rHalf)
      eao((5-nhe)/2)   = rnhe*xnnp    *coeff*me*shlf**2*phs
      eao((5+nhe)/2)   =               coeff*eb*s*(-rTwo+xnnm)
      eao((5+3*nhe)/2) = real(nha-nhe)*coeff*eb*x*chlf**2*phs*rTwo

      eao(5) = eb*dcmplx( x-rOne, x*c+rOne )
      eao(6) = eb*x*s*dcmplx( csp, snp )
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_ficxxx(fi , fic)
c
c this subroutine charge conjugates a flowing-in fermion wavefunction.  
c                                                                       
c input:                                                                
c       complex fi(6)          : flowing-in fermion                 |fi>
c                                                                       
c output:                                                               
c       complex fic(6)         : charge conjugated fermion         <fic|
c
      implicit none
      double complex fi(6), fic(6)
c
      fic(1) = -fi(2)
      fic(2) =  fi(1)
      fic(3) =  fi(4)
      fic(4) = -fi(3)
      fic(5) = -fi(5)
      fic(6) = -fi(6)
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_focxxx(fo , foc)
c
c this subroutine charge conjugates a flowing-out fermion wavefunction. 
c                                                                       
c input:                                                                
c       complex fo(6)          : flowing-out fermion                <fo|
c                                                                       
c output:                                                               
c       complex foc(6)         : charge conjugated fermion         |foc>
c
      implicit none
      double complex fo(6), foc(6)
c
      foc(1) =  fo(2)
      foc(2) = -fo(1)
      foc(3) = -fo(4)
      foc(4) =  fo(3)
      foc(5) = -fo(5)
      foc(6) = -fo(6)
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fsicxx(fic,sc,gc,fmass,fwidth , fsic)
c
c this subroutine computes an off-shell antifermion wavefunction from a 
c flowing-in external antifermion and a vector boson.                   
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion           |fic>
c       complex sc(3)          : input    scalar                   s 
c       complex gc(2)          : coupling constants              gchf
c       real    fmass          : mass  of output antifermion fc'     
c       real    fwidth         : width of output antifermion fc'     
c                                                                       
c output:                                                               
c       complex fsic(6)        : off-shell fermion        |fc',s,fic>
c
      implicit none
      double complex fic(6),sc(3),fsic(6),gc(2),sl1,sl2,sr1,sr2,ds
      double precision pf(0:3),fmass,fwidth,pf2,p0p3,p0m3
c
      fsic(5) = fic(5)-sc(2)
      fsic(6) = fic(6)-sc(3)

      pf(0) = dble( fsic(5))
      pf(1) = dble( fsic(6))
      pf(2) = dimag(fsic(6))
      pf(3) = dimag(fsic(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)

      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )
      p0p3 = pf(0)+pf(3)
      p0m3 = pf(0)-pf(3)
      sl1 = gc(1)*(p0p3*fic(1)+dconjg(fsic(6))*fic(2))
      sl2 = gc(1)*(p0m3*fic(2)       +fsic(6) *fic(1))
      sr1 = gc(2)*(p0m3*fic(3)-dconjg(fsic(6))*fic(4))
      sr2 = gc(2)*(p0p3*fic(4)       -fsic(6) *fic(3))

      fsic(1) = ( gc(1)*fmass*fic(1) + sr1 )*ds
      fsic(2) = ( gc(1)*fmass*fic(2) + sr2 )*ds
      fsic(3) = ( gc(2)*fmass*fic(3) + sl1 )*ds
      fsic(4) = ( gc(2)*fmass*fic(4) + sl2 )*ds
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fsigld(fi,sc,gc,fmass,fwidth,smass,mNLSP,idecay , fsi)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-in external fermion and a scalar boson, for the NLSP-boson-
c Goldstino vertex. The h.c. of the NLSP decay is handled via the
c input parameter idecay.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex sc(3)          : input    scalar                      s
c       complex gc(2)          : coupling constants                  gsf
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex fsi(6)         : off-shell fermion             |f',s,fi>
c
      implicit none
      double complex  fi(6), sc(3), gc(2), fsi(6), s1, s2, s3, s4, ds
      double complex  p14p, p14m, p23p, p23m
      double precision  pf(0:3), fmass, fwidth, mNLSP, smass, pf2
      integer idecay

      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fsi(5) = fi(5) - sc(2)
      fsi(6) = fi(6) - sc(3)

      pf(0) = dble( fsi(5))
      pf(1) = dble( fsi(6))
      pf(2) = dimag(fsi(6))
      pf(3) = dimag(fsi(5))
      pf2 = pf(0)**2 - pf(1)**2 - pf(2)**2 - pf(3)**2

      if ( idecay.ne.1 .or. idecay.ne.-1 ) then
         write(6,*) 'error in idecay of hvq_fsigld'
         stop
      end if

      p14p = dble(sc(2)) + dimag(sc(2))
      p14m = dble(sc(2)) - dimag(sc(2))
      p23p = dble(sc(3)) + dimag(sc(3))*ci
      p23m = dble(sc(3)) - dimag(sc(3))*ci

      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )

      s1 = -idecay*gc(1)*fi(1)*smass**2
      s2 = -idecay*gc(1)*fi(2)*smass**2
      s3 = gc(1)*mNLSP*( fi(1)*p14p + fi(2)*p23m ) 
      s4 = gc(1)*mNLSP*( fi(1)*p23p + fi(2)*p14m )

      if ( gc(2).ne.cZero ) then
         s1 = s1 + gc(2)*mNLSP*( fi(3)*p14m - fi(4)*p23m )
         s2 = s2 + gc(2)*mNLSP*(-fi(3)*p23p + fi(4)*p14p )
         s3 = s3 - gc(2)*idecay*fi(3)*smass**2
         s4 = s4 - gc(2)*idecay*fi(4)*smass**2
      end if

      fsi(1) = ( (pf(0)-pf(3))*s3 - dconjg(fsi(6))*s4 + fmass*s1 )*ds
      fsi(2) = (       -fsi(6)*s3 +  (pf(0)+pf(3))*s4 + fmass*s2 )*ds
      fsi(3) = ( (pf(0)+pf(3))*s1 + dconjg(fsi(6))*s2 + fmass*s3 )*ds
      fsi(4) = (        fsi(6)*s1 +  (pf(0)-pf(3))*s2 + fmass*s4 )*ds
c
      return          
      end
      subroutine hvq_fsixxx(fi,sc,gc,fmass,fwidth , fsi)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-IN external fermion and a vector boson.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex sc(3)          : input    scalar                      s
c       complex gc(2)          : coupling constants                 gchf
c       real    fmass          : mass  of OUTPUT fermion f'
c       real    fwidth         : width of OUTPUT fermion f'
c
c output:
c       complex fsi(6)         : off-shell fermion             |f',s,fi>
c
      implicit none
      double complex fi(6),sc(3),fsi(6),gc(2),sl1,sl2,sr1,sr2,ds
      double precision pf(0:3),fmass,fwidth,pf2,p0p3,p0m3

c

      fsi(5) = fi(5)-sc(2)
      fsi(6) = fi(6)-sc(3)

      pf(0) = dble( fsi(5))
      pf(1) = dble( fsi(6))
      pf(2) = dimag(fsi(6))
      pf(3) = dimag(fsi(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )
      p0p3 = pf(0)+pf(3)
      p0m3 = pf(0)-pf(3)
      sl1 = gc(1)*(p0p3*fi(1)+dconjg(fsi(6))*fi(2))
      sl2 = gc(1)*(p0m3*fi(2)       +fsi(6) *fi(1))
      sr1 = gc(2)*(p0m3*fi(3)-dconjg(fsi(6))*fi(4))
      sr2 = gc(2)*(p0p3*fi(4)       -fsi(6) *fi(3))

      fsi(1) = ( gc(1)*fmass*fi(1) + sr1 )*ds
      fsi(2) = ( gc(1)*fmass*fi(2) + sr2 )*ds
      fsi(3) = ( gc(2)*fmass*fi(3) + sl1 )*ds
      fsi(4) = ( gc(2)*fmass*fi(4) + sl2 )*ds
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fsocxx(foc,sc,gc,fmass,fwidth , fsoc)
c
c this subroutine computes an off-shell antifermion wavefunction from a 
c flowing-out external antifermion and a vector boson.                  
c                                                                       
c input:                                                                
c       complex foc(6)         : flow-out fermion               <foc|
c       complex sc(6)          : input    scalar                   s 
c       complex gc(2)          : coupling constants              gchf
c       real     fmass         : mass  of output antifermion fc'     
c       real     fwidth        : width of output antifermion fc'     
c                                                                       
c output:                                                               
c       complex fsoc(6)        : off-shell fermion         <fo,s,fc'|
c
      implicit none
      double complex foc(6),sc(6),fsoc(6),gc(2),sl1,sl2,sr1,sr2,ds
      double precision pf(0:3),fmass,fwidth,pf2,p0p3,p0m3
c
      fsoc(5) = foc(5)+sc(2)
      fsoc(6) = foc(6)+sc(3)

      pf(0) = dble( fsoc(5))
      pf(1) = dble( fsoc(6))
      pf(2) = dimag(fsoc(6))
      pf(3) = dimag(fsoc(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)

      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )
      p0p3 = pf(0)+pf(3)
      p0m3 = pf(0)-pf(3)
      sl1 = gc(2)*(p0p3*foc(3)       +fsoc(6) *foc(4))
      sl2 = gc(2)*(p0m3*foc(4)+dconjg(fsoc(6))*foc(3))
      sr1 = gc(1)*(p0m3*foc(1)       -fsoc(6) *foc(2))
      sr2 = gc(1)*(p0p3*foc(2)-dconjg(fsoc(6))*foc(1))

      fsoc(1) = ( gc(1)*fmass*foc(1) + sl1 )*ds
      fsoc(2) = ( gc(1)*fmass*foc(2) + sl2 )*ds
      fsoc(3) = ( gc(2)*fmass*foc(3) + sr1 )*ds
      fsoc(4) = ( gc(2)*fmass*foc(4) + sr2 )*ds
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fsogld(fo,sc,gc,fmass,fwidth,smass,mNLSP,idecay , fso)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-out external fermion and a scalar boson, for the NLSP-boson-
c Goldstino vertex. The h.c. of the NLSP decay is handled via the
c input parameter idecay.
c
c input:
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex sc(3)          : input    scalar                      s
c       complex gc(2)          : coupling constants                  gsf
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex fso(6)         : off-shell fermion             <fo,s,f'|
c
      implicit none
      double complex  fo(6), sc(3), gc(2), fso(6), s1, s2, s3, s4, ds
      double precision  pf(0:3), fmass, fwidth, mNLSP, smass, pf2
      double precision  p14p, p14m, p23p, p23m
      integer idecay

      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fso(5) = fo(5) + sc(2)
      fso(6) = fo(6) + sc(3)

      pf(0) = dble( fso(5))
      pf(1) = dble( fso(6))
      pf(2) = dimag(fso(6))
      pf(3) = dimag(fso(5))
      pf2 = pf(0)**2 - pf(1)**2 - pf(2)**2 - pf(3)**2

      if ( idecay.ne.1 .or. idecay.ne.-1 ) then
         write(6,*) 'error in idecay of hvq_fsogld'
         stop
      end if

      p14p = dble(sc(2)) + dimag(sc(2))
      p14m = dble(sc(2)) - dimag(sc(2))
      p23p = dble(sc(3)) + dimag(sc(3))*ci
      p23m = dble(sc(3)) - dimag(sc(3))*ci

      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )

      s1 = -idecay*gc(1)*fo(1)*smass**2
      s2 = -idecay*gc(1)*fo(2)*smass**2
      s3 = gc(1)*mNLSP*( fo(1)*p14m - fo(2)*p23p )
      s4 = gc(1)*mNLSP*(-fo(1)*p23m + fo(2)*p14p )

      if ( gc(2).ne.cZero ) then
         s1 = s1 + gc(2)*mNLSP*( fo(3)*p14p + fo(4)*p23p )
         s2 = s2 + gc(2)*mNLSP*( fo(3)*p23m + fo(4)*p14m )
         s3 = s3 - gc(2)*idecay*fo(3)*smass**2
         s4 = s4 - gc(2)*idecay*fo(4)*smass**2
      end if

      fso(1) = (  (pf(0)+pf(3))*s3 +         fso(6)*s4 + fmass*s1 )*ds
      fso(2) = ( dconjg(fso(6))*s3 +  (pf(0)-pf(3))*s4 + fmass*s2 )*ds
      fso(3) = (  (pf(0)-pf(3))*s1 -         fso(6)*s2 + fmass*s3 )*ds
      fso(4) = (-dconjg(fso(6))*s1 +  (pf(0)+pf(3))*s2 + fmass*s4 )*ds
c
      return          
      end
      subroutine hvq_fsoxxx(fo,sc,gc,fmass,fwidth , fso)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-OUT external fermion and a vector boson.
c
c input:
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex sc(6)          : input    scalar                      s
c       complex gc(2)          : coupling constants                 gchf
c       real    fmass          : mass  of OUTPUT fermion f'
c       real    fwidth         : width of OUTPUT fermion f'
c
c output:
c       complex fso(6)         : off-shell fermion             <fo,s,f'|
c
      implicit none
      double complex fo(6),sc(6),fso(6),gc(2),sl1,sl2,sr1,sr2,ds
      double precision pf(0:3),fmass,fwidth,pf2,p0p3,p0m3

c

      fso(5) = fo(5)+sc(2)
      fso(6) = fo(6)+sc(3)

      pf(0) = dble( fso(5))
      pf(1) = dble( fso(6))
      pf(2) = dimag(fso(6))
      pf(3) = dimag(fso(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      ds = -sc(1)/dcmplx( pf2-fmass**2, fmass*fwidth )
      p0p3 = pf(0)+pf(3)
      p0m3 = pf(0)-pf(3)
      sl1 = gc(2)*(p0p3*fo(3)       +fso(6) *fo(4))
      sl2 = gc(2)*(p0m3*fo(4)+dconjg(fso(6))*fo(3))
      sr1 = gc(1)*(p0m3*fo(1)       -fso(6) *fo(2))
      sr2 = gc(1)*(p0p3*fo(2)-dconjg(fso(6))*fo(1))

      fso(1) = ( gc(1)*fmass*fo(1) + sl1 )*ds
      fso(2) = ( gc(1)*fmass*fo(2) + sl2 )*ds
      fso(3) = ( gc(2)*fmass*fo(3) + sr1 )*ds
      fso(4) = ( gc(2)*fmass*fo(4) + sr2 )*ds
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fvicxx(fic,vc,gc,fmass,fwidth , fvic)
c
c this subroutine computes an off-shell antifermion wavefunction from a 
c flowing-in external antifermion and a vector boson.                   
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion              |fic>
c       complex vc(6)          : input    vector                      v 
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of output antifermion f'         
c       real    fwidth         : width of output antifermion f'         
c                                                                       
c output:                                                               
c       complex fvic(6)        : off-shell antifermion       |fc',v,fic>
c
      implicit none
      double complex fic(6),vc(6),gc(2),fvic(6),sl1,sl2,sr1,sr2,d
      double precision pf(0:3),fmass,fwidth,pf2

      double precision rOne
      parameter( rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fvic(5) = fic(5)-vc(5)
      fvic(6) = fic(6)-vc(6)

      pf(0) = dble( fvic(5))
      pf(1) = dble( fvic(6))
      pf(2) = dimag(fvic(6))
      pf(3) = dimag(fvic(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)

      d = rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      sl1 =   (vc(1)+      vc(4))*fic(1)
     &      + (vc(2)-cImag*vc(3))*fic(2)
      sl2 =   (vc(2)+cImag*vc(3))*fic(1)
     &      + (vc(1)-      vc(4))*fic(2)

      if ( gc(2).ne.cZero ) then
         sr1 =   (vc(1)-      vc(4))*fic(3)
     &         - (vc(2)-cImag*vc(3))*fic(4)
         sr2 = - (vc(2)+cImag*vc(3))*fic(3)
     &         + (vc(1)+      vc(4))*fic(4)

         fvic(1) = ( gc(2)*((pf(0)-pf(3))*sl1 - dconjg(fvic(6))*sl2)
     &              +gc(1)*fmass*sr1 )*d
         fvic(2) = ( gc(2)*(     -fvic(6)*sl1 +   (pf(0)+pf(3))*sl2)
     &              +gc(1)*fmass*sr2 )*d
         fvic(3) = ( gc(1)*((pf(0)+pf(3))*sr1 + dconjg(fvic(6))*sr2)
     &              +gc(2)*fmass*sl1 )*d
         fvic(4) = ( gc(1)*(      fvic(6)*sr1 +   (pf(0)-pf(3))*sr2)
     &              +gc(2)*fmass*sl2 )*d
      else
         d = d * gc(1)
         fvic(1) = fmass*sr1*d
         fvic(2) = fmass*sr2*d
         fvic(3) = ((pf(0)+pf(3))*sr1 + dconjg(fvic(6))*sr2)*d
         fvic(4) = (      fvic(6)*sr1 +   (pf(0)-pf(3))*sr2)*d
      end if
c
      return
      end
      subroutine hvq_fvidmx(fi,vc,gc,fmass,fwidth , fvi)
c
c This subroutine computes a dipole moment off-shell fermion
c wavefunction from a flowing-IN external fermion and a vector boson.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex vc(6)          : input    vector                      v
c       complex gc(2,2)        : coupling constants                  gvf
c                              : first index is L,R as normal
c                              : second index is EDM,-MDM
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c
c output:
c       complex fvi(6)         : off-shell fermion             |f',v,fi>
c
      implicit none
      double complex fi(6), vc(6), fvi(6), sl1, sl2, sr1, sr2, d
      double complex gc(2,2), gL, gR
      double precision pf(0:3), fmass, fwidth, pf2

      double complex kvc21, kvc31, kvc41, kvc32, kvc42, kvc43
      double precision k(1:4)
      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      gL = -gc(1,1) + cImag*gc(1,2)
      gR =  gc(2,1) + cImag*gc(2,2)

c k in vertex formula defined as (pi - po)
      k(1) = dble( vc(5))
      k(2) = dble( vc(6))
      k(3) = dimag(vc(6))
      k(4) = dimag(vc(5))

      kvc21 = (k(2)*vc(1) - k(1)*vc(2))*cImag
      kvc31 =  k(3)*vc(1) - k(1)*vc(3)
      kvc41 = (k(4)*vc(1) - k(1)*vc(4))*cImag
      kvc32 =  k(3)*vc(2) - k(2)*vc(3)
      kvc42 = (k(4)*vc(2) - k(2)*vc(4))*cImag
      kvc43 =  k(4)*vc(3) - k(3)*vc(4)

      fvi(5) = fi(5) - vc(5)
      fvi(6) = fi(6) - vc(6)

      pf(0) = dble( fvi(5))
      pf(1) = dble( fvi(6))
      pf(2) = dimag(fvi(6))
      pf(3) = dimag(fvi(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )

      sl1 = gL*(  fi(1)*(kvc41 + kvc32)
     &          + fi(2)*(kvc42 + kvc21 + kvc43 + kvc31) )
      sl2 = gL*(- fi(1)*(kvc42 - kvc21 - kvc43 + kvc31)
     &          - fi(2)*(kvc41 + kvc32)                 )

      if ( gc(2,1).ne.cZero .or.
     &     gc(2,2).ne.cZero      ) then
         sr1 = gR*(- fi(3)*(kvc41 - kvc32)
     &             + fi(4)*(kvc42 - kvc21 + kvc43 - kvc31) )
         sr2 = gR*(- fi(3)*(kvc42 + kvc21 - kvc43 - kvc31)
     &             + fi(4)*(kvc41 - kvc32)                 )

         fvi(1) = ( (pf(0)-pf(3))*sr1 - dconjg(fvi(6))*sr2
     &             + fmass*sl1                             )*d
         fvi(2) = (      - fvi(6)*sr1 +  (pf(0)+pf(3))*sr2
     &             + fmass*sl2                             )*d
         fvi(3) = ( (pf(0)+pf(3))*sl1 + dconjg(fvi(6))*sl2
     &             + fmass*sr1                             )*d
         fvi(4) = (        fvi(6)*sl1 +  (pf(0)-pf(3))*sl2
     &             + fmass*sr2                             )*d

      else
         fvi(1) = fmass*sl1*d
         fvi(2) = fmass*sl2*d
         fvi(3) = ( (pf(0)+pf(3))*sl1 + dconjg(fvi(6))*sl2 )*d
         fvi(4) = (        fvi(6)*sl1 +  (pf(0)-pf(3))*sl2 )*d
      end if
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fvigld(fi,vc,gc,fmass,fwidth,idecay , fvi)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-in external fermion and a vector boson, for the NLSP-boson-
c Goldstino vertex. The h.c. of the NLSP decay is handled via the
c input parameter idecay (picks out correct Goldstino momentum).
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex fvi(6)         : off-shell fermion             |f',v,fi>
c
      implicit none
      double complex  fi(6), vc(6), gc(2), fvi(6), sl1, sl2, sr1, sr2, d
      double complex  p14p, p14m, p23p, p23m, A14p, A14m, A23p, A23m
      double complex  AdotpG
      double precision  fmass, fwidth
      double precision  pf(0:3), pv(4), pf2, pdotpG
      integer idecay

      double precision rOne
      parameter( rOne = 1.0d0 )
      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fvi(5) = fi(5) - vc(5)
      fvi(6) = fi(6) - vc(6)

      pv(1) = dble( vc(5))
      pv(2) = dble( vc(6))
      pv(3) = dimag(vc(6))
      pv(4) = dimag(vc(5))

      pf(0) = dble( fvi(5))
      pf(1) = dble( fvi(6))
      pf(2) = dimag(fvi(6))
      pf(3) = dimag(fvi(5))
      pf2 = pf(0)**2 - pf(1)**2 - pf(2)**2 - pf(3)**2

      if ( idecay.eq.1 ) then
         pdotpG = pv(1)*pf(0) - pv(2)*pf(1) - pv(3)*pf(2) - pv(4)*pf(3)
         AdotpG = vc(1)*pf(0) - vc(2)*pf(1) - vc(3)*pf(2) - vc(4)*pf(3)
      else if ( idecay.eq.-1 ) then
         pdotpG =  pv(1)*dble( fi(5)) - pv(2)*dble( fi(6))
     &           - pv(3)*dimag(fi(6)) - pv(4)*dimag(fi(5))
         AdotpG =  vc(1)*dble( fi(5)) - vc(2)*dble( fi(6))
     &           - vc(3)*dimag(fi(6)) - vc(4)*dimag(fi(5))
      else
         write(6,*) 'error in idecay of hvq_fvigld'
         stop
      end if

      p14p = dble(vc(5)) + dimag(vc(5))
      p14m = dble(vc(5)) - dimag(vc(5))
      p23p = vc(6)
      p23m = dconjg(vc(6))
      A14p = vc(1) + vc(4)
      A14m = vc(1) - vc(4)
      A23p = vc(2) + vc(3)*ci
      A23m = vc(2) - vc(3)*ci

      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      d = d*idecay

      sl1 =  (p14p*AdotpG - A14p*pdotpG)*fi(1)
     &      +(p23m*AdotpG - A23m*pdotpG)*fi(2)
      sl2 =  (p23p*AdotpG - A23p*pdotpG)*fi(1)
     &      +(p14m*AdotpG - A14m*pdotpG)*fi(2)

      if ( gc(2).ne.cZero ) then
         sr1 =  (p14m*AdotpG - A14m*pdotpG)*fi(3)
     &         -(p23m*AdotpG - A23m*pdotpG)*fi(4)
         sr2 = -(p23p*AdotpG - A23p*pdotpG)*fi(3)
     &         +(p14p*AdotpG - A14p*pdotpG)*fi(4)

         fvi(1) = ( gc(1)*((pf(0)-pf(3))*sl1 - dconjg(fvi(6))*sl2 )
     &             +gc(2)*fmass*sr1 )*d
         fvi(2) = ( gc(1)*(      -fvi(6)*sl1 +  (pf(0)+pf(3))*sl2 )
     &             +gc(2)*fmass*sr2 )*d
         fvi(3) = ( gc(2)*((pf(0)+pf(3))*sr1 + dconjg(fvi(6))*sr2 )
     &             +gc(1)*fmass*sl1 )*d
         fvi(4) = ( gc(2)*(       fvi(6)*sr1 +  (pf(0)-pf(3))*sr2 )
     &             +gc(1)*fmass*sl2 )*d

      else
         d = d*gc(1)
         fvi(1) = d*((pf(0)-pf(3))*sl1 - dconjg(fvi(6))*sl2)
         fvi(2) = d*(      -fvi(6)*sl1 +  (pf(0)+pf(3))*sl2)
         fvi(3) = d*fmass*sl1
         fvi(4) = d*fmass*sl2
      end if
c
      return          
      end
      subroutine hvq_fvixxx(fi,vc,gc,fmass,fwidth , fvi)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-IN external fermion and a vector boson.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c
c output:
c       complex fvi(6)         : off-shell fermion             |f',v,fi>
c
      implicit none
      double complex fi(6),vc(6),gc(2),fvi(6),sl1,sl2,sr1,sr2,d
      double precision pf(0:3),fmass,fwidth,pf2
      
      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      fvi(5) = fi(5)-vc(5)
      fvi(6) = fi(6)-vc(6)

      pf(0) = dble( fvi(5))
      pf(1) = dble( fvi(6))
      pf(2) = dimag(fvi(6))
      pf(3) = dimag(fvi(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      sl1 =   (vc(1)+      vc(4))*fi(1)
     &      + (vc(2)-cImag*vc(3))*fi(2)
      sl2 =   (vc(2)+cImag*vc(3))*fi(1)
     &      + (vc(1)-      vc(4))*fi(2)

      if ( gc(2).ne.cZero ) then
         sr1 =   (vc(1)-      vc(4))*fi(3)
     &         - (vc(2)-cImag*vc(3))*fi(4)
         sr2 = - (vc(2)+cImag*vc(3))*fi(3)
     &         + (vc(1)+      vc(4))*fi(4)

         fvi(1) = ( gc(1)*((pf(0)-pf(3))*sl1 - dconjg(fvi(6))*sl2)
     &             +gc(2)*fmass*sr1 )*d
         fvi(2) = ( gc(1)*(      -fvi(6)*sl1 +  (pf(0)+pf(3))*sl2)
     &             +gc(2)*fmass*sr2 )*d
         fvi(3) = ( gc(2)*((pf(0)+pf(3))*sr1 + dconjg(fvi(6))*sr2)
     &             +gc(1)*fmass*sl1 )*d
         fvi(4) = ( gc(2)*(       fvi(6)*sr1 +  (pf(0)-pf(3))*sr2)
     &             +gc(1)*fmass*sl2 )*d

      else
         d = d * gc(1)
         fvi(1) = ((pf(0)-pf(3))*sl1 - dconjg(fvi(6))*sl2)*d
         fvi(2) = (      -fvi(6)*sl1 +  (pf(0)+pf(3))*sl2)*d
         fvi(3) = fmass*sl1*d
         fvi(4) = fmass*sl2*d
      end if
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fvocxx(foc,vc,gc,fmass,fwidth , fvoc)
c
c this subroutine computes an off-shell antifermion wavefunction from a 
c flowing-out external antifermion and a vector boson.                  
c                                                                       
c input:                                                                
c       complex foc(6)         : flow-out antifermion              <foc|
c       complex vc(6)          : input    vector                      v 
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of output antifermion f'         
c       real    fwidth         : width of output antifermion f'         
c                                                                       
c output:                                                               
c       complex fvoc(6)        : off-shell antifermion       <foc,v,fc'|
c
      implicit none
      double complex foc(6),vc(6),gc(2),fvoc(6),sl1,sl2,sr1,sr2,d
      double precision pf(0:3),fmass,fwidth,pf2

      double precision rOne
      parameter( rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fvoc(5) = foc(5)+vc(5)
      fvoc(6) = foc(6)+vc(6)

      pf(0) = dble( fvoc(5))
      pf(1) = dble( fvoc(6))
      pf(2) = dimag(fvoc(6))
      pf(3) = dimag(fvoc(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)

      d = rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      sl1 =   (vc(1)+      vc(4))*foc(3)
     &      + (vc(2)+cImag*vc(3))*foc(4)
      sl2 =   (vc(2)-cImag*vc(3))*foc(3)
     &      + (vc(1)-      vc(4))*foc(4)

      if ( gc(2).ne.cZero ) then
         sr1 =   (vc(1)-      vc(4))*foc(1)
     &         - (vc(2)+cImag*vc(3))*foc(2)
         sr2 = - (vc(2)-cImag*vc(3))*foc(1)
     &         + (vc(1)+      vc(4))*foc(2)

         fvoc(1) = ( gc(1)*( (pf(0)+pf(3))*sr1          +fvoc(6)*sr2)
     &              +gc(2)*fmass*sl1 )*d
         fvoc(2) = ( gc(1)*( dconjg(fvoc(6))*sr1 + (pf(0)-pf(3))*sr2)
     &              +gc(2)*fmass*sl2 )*d
         fvoc(3) = ( gc(2)*( (pf(0)-pf(3))*sl1          -fvoc(6)*sl2)
     &              +gc(1)*fmass*sr1 )*d
         fvoc(4) = ( gc(2)*(-dconjg(fvoc(6))*sl1 + (pf(0)+pf(3))*sl2)
     &              +gc(1)*fmass*sr2 )*d
      else
         d = d * gc(1)
         fvoc(1) = ((pf(0)+pf(3))*sr1          +fvoc(6)*sr2)*d
         fvoc(2) = (dconjg(fvoc(6))*sr1 + (pf(0)-pf(3))*sr2)*d
         fvoc(3) = fmass*sr1*d
         fvoc(4) = fmass*sr2*d
      end if
c
      return
      end
      subroutine hvq_fvodmx(fo,vc,gc,fmass,fwidth , fvo)
c
c This subroutine computes a dipole moment off-shell fermion
c wavefunction from a flowing-OUT external fermion and a vector boson.
c
c input:
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2,2)        : coupling constants                  gvf
c                              : first index is L,R as normal
c                              : second index is EDM,-MDM
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c
c output:
c       complex fvo(6)         : off-shell fermion             <fo,v,f'|
c
      implicit none
      double complex fo(6), vc(6), fvo(6), sl1, sl2, sr1, sr2, d
      double complex gc(2,2), gL, gR
      double precision  pf(0:3), fmass, fwidth, pf2

      double complex kvc21, kvc31, kvc41, kvc32, kvc42, kvc43
      double precision k(1:4)
      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      gL = -gc(1,1) + cImag*gc(1,2)
      gR =  gc(2,1) + cImag*gc(2,2)

c k in vertex formula defined as (pi - po)
      k(1) = dble( vc(5))
      k(2) = dble( vc(6))
      k(3) = dimag(vc(6))
      k(4) = dimag(vc(5))

      kvc21 = (k(2)*vc(1) - k(1)*vc(2))*cImag
      kvc31 =  k(3)*vc(1) - k(1)*vc(3)
      kvc41 = (k(4)*vc(1) - k(1)*vc(4))*cImag
      kvc32 =  k(3)*vc(2) - k(2)*vc(3)
      kvc42 = (k(4)*vc(2) - k(2)*vc(4))*cImag
      kvc43 =  k(4)*vc(3) - k(3)*vc(4)

      fvo(5) = fo(5) + vc(5)
      fvo(6) = fo(6) + vc(6)

      pf(0) = dble( fvo(5))
      pf(1) = dble( fvo(6))
      pf(2) = dimag(fvo(6))
      pf(3) = dimag(fvo(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )

      sl1 = gL*(  fo(1)*(kvc41 + kvc32)
     &          - fo(2)*(kvc42 - kvc21 - kvc43 + kvc31) )
      sl2 = gL*(  fo(1)*(kvc42 + kvc21 + kvc43 + kvc31)
     &          - fo(2)*(kvc41 + kvc32)                 )

      if ( gc(2,1).ne.cZero .or.
     &     gc(2,2).ne.cZero      ) then
         sr1 = gR*(- fo(3)*(kvc41 - kvc32)
     &             - fo(4)*(kvc42 + kvc21 - kvc43 - kvc31) )
         sr2 = gR*(  fo(3)*(kvc42 - kvc21 + kvc43 - kvc31)
     &             + fo(4)*(kvc41 - kvc32)                 )

         fvo(1) = (  (pf(0)+pf(3))*sr1       + fvo(6)*sr2
     &             + fmass*sl1                            )*d
         fvo(2) = ( dconjg(fvo(6))*sr1 +(pf(0)-pf(3))*sr2
     &             + fmass*sl2                            )*d
         fvo(3) = (  (pf(0)-pf(3))*sl1       - fvo(6)*sl2
     &             + fmass*sr1                            )*d
         fvo(4) = (-dconjg(fvo(6))*sl1 +(pf(0)+pf(3))*sl2
     &             + fmass*sr2                            )*d

      else
         fvo(1) = fmass*sl1*d
         fvo(2) = fmass*sl2*d
         fvo(3) = (  (pf(0)-pf(3))*sl1        - fvo(6)*sl2)*d
         fvo(4) = (-dconjg(fvo(6))*sl1 + (pf(0)+pf(3))*sl2)*d
      end if
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_fvogld(fo,vc,gc,fmass,fwidth,idecay , fvo)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-out external fermion and a vector boson, for the NLSP-boson-
c Goldstino vertex. The h.c. of the NLSP decay is handled via the
c input parameter idecay (picks out correct Goldstino momentum).
c
c input:
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of output fermion f'
c       real    fwidth         : width of output fermion f'
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex fvo(6)         : off-shell fermion             <fo,v,f'|
c
      implicit none
      double complex  fo(6), vc(6), gc(2), fvo(6), sl1, sl2, sr1, sr2, d
      double complex  p14p, p14m, p23p, p23m, A14p, A14m, A23p, A23m
      double complex  AdotpG
      double precision  fmass, fwidth
      double precision  pf(0:3), pv(4), pf2, pdotpG
      integer idecay

      double precision rOne
      parameter( rOne = 1.0d0 )
      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      fvo(5) = fo(5) + vc(5)
      fvo(6) = fo(6) + vc(6)

      pv(1) = dble( vc(5))
      pv(2) = dble( vc(6))
      pv(3) = dimag(vc(6))
      pv(4) = dimag(vc(5))

      pf(0) = dble( fvo(5))
      pf(1) = dble( fvo(6))
      pf(2) = dimag(fvo(6))
      pf(3) = dimag(fvo(5))
      pf2 = pf(0)**2 - pf(1)**2 - pf(2)**2 - pf(3)**2

      if ( idecay.eq.1 ) then
         pdotpG = pv(1)*pf(0) - pv(2)*pf(1) - pv(3)*pf(2) - pv(4)*pf(3)
         AdotpG = vc(1)*pf(0) - vc(2)*pf(1) - vc(3)*pf(2) - vc(4)*pf(3)
      else if ( idecay.eq.-1 ) then
         pdotpG =  pv(1)*dble( fo(5)) - pv(2)*dble( fo(6))
     &           - pv(3)*dimag(fo(6)) - pv(4)*dimag(fo(5))
         AdotpG =  vc(1)*dble( fo(5)) - vc(2)*dble( fo(6))
     &           - vc(3)*dimag(fo(6)) - vc(4)*dimag(fo(5))
      else
         write(6,*) 'error in idecay of hvq_fvogld'
         stop
      end if

      p14p = dble(vc(5)) + dimag(vc(5))
      p14m = dble(vc(5)) - dimag(vc(5))
      p23p = vc(6)
      p23m = dconjg(vc(6))
      A14p = vc(1) + vc(4)
      A14m = vc(1) - vc(4)
      A23p = vc(2) + vc(3)*ci
      A23m = vc(2) - vc(3)*ci

      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      d = d*idecay

      sl1 =  (p14p*AdotpG - A14p*pdotpG)*fo(3)
     &      +(p23p*AdotpG - A23p*pdotpG)*fo(4)
      sl2 =  (p23m*AdotpG - A23m*pdotpG)*fo(3)
     &      +(p14m*AdotpG - A14m*pdotpG)*fo(4)

      if ( gc(2).ne.cZero ) then
         sr1 =  (p14m*AdotpG - A14m*pdotpG)*fo(1)
     &         -(p23p*AdotpG - A23p*pdotpG)*fo(2)
         sr2 = -(p23m*AdotpG - A23m*pdotpG)*fo(1)
     &         +(p14p*AdotpG - A14p*pdotpG)*fo(2)

         fvo(1) = ( gc(2)*(  (pf(0)+pf(3))*sr1         +fvo(6)*sr2 )
     &             +gc(1)*fmass*sl1 )*d
         fvo(2) = ( gc(2)*( dconjg(fvo(6))*sr1 + (pf(0)-pf(3))*sr2 )
     &             +gc(1)*fmass*sl2 )*d
         fvo(3) = ( gc(1)*(  (pf(0)-pf(3))*sl1         -fvo(6)*sl2 )
     &             +gc(2)*fmass*sr1 )*d
         fvo(4) = ( gc(1)*(-dconjg(fvo(6))*sl1 + (pf(0)+pf(3))*sl2 )
     &             +gc(2)*fmass*sr2 )*d

      else
         d = d*gc(1)
         fvo(1) = d*fmass*sl1
         fvo(2) = d*fmass*sl2
         fvo(3) = d*(  (pf(0)-pf(3))*sl1         -fvo(6)*sl2)
         fvo(4) = d*(-dconjg(fvo(6))*sl1 + (pf(0)+pf(3))*sl2)
      end if
c
      return          
      end
      subroutine hvq_fvoxxx(fo,vc,gc,fmass,fwidth , fvo)
c
c This subroutine computes an off-shell fermion wavefunction from a
c flowing-OUT external fermion and a vector boson.
c
c
c input:
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c       real    fmass          : mass  of OUTPUT fermion f'
c       real    fwidth         : width of OUTPUT fermion f'
c
c output:
c       complex fvo(6)         : off-shell fermion             <fo,v,f'|
c
      implicit none
      double complex fo(6),vc(6),gc(2),fvo(6),sl1,sl2,sr1,sr2,d
      double precision pf(0:3),fmass,fwidth,pf2

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      fvo(5) = fo(5)+vc(5)
      fvo(6) = fo(6)+vc(6)

      pf(0) = dble( fvo(5))
      pf(1) = dble( fvo(6))
      pf(2) = dimag(fvo(6))
      pf(3) = dimag(fvo(5))
      pf2 = pf(0)**2-(pf(1)**2+pf(2)**2+pf(3)**2)


      d = -rOne/dcmplx( pf2-fmass**2, fmass*fwidth )
      sl1 =   (vc(1)+      vc(4))*fo(3)
     &      + (vc(2)+cImag*vc(3))*fo(4)
      sl2 =   (vc(2)-cImag*vc(3))*fo(3)
     &      + (vc(1)-      vc(4))*fo(4)

      if ( gc(2).ne.cZero ) then
         sr1 =   (vc(1)-      vc(4))*fo(1)
     &         - (vc(2)+cImag*vc(3))*fo(2)
         sr2 = - (vc(2)-cImag*vc(3))*fo(1)
     &         + (vc(1)+      vc(4))*fo(2)

         fvo(1) = ( gc(2)*( (pf(0)+pf(3))*sr1  +        fvo(6)*sr2)
     &             +gc(1)*fmass*sl1 )*d
         fvo(2) = ( gc(2)*( dconjg(fvo(6))*sr1 + (pf(0)-pf(3))*sr2)
     &             +gc(1)*fmass*sl2 )*d
         fvo(3) = ( gc(1)*( (pf(0)-pf(3))*sl1  -        fvo(6)*sl2)
     &             +gc(2)*fmass*sr1 )*d
         fvo(4) = ( gc(1)*(-dconjg(fvo(6))*sl1 + (pf(0)+pf(3))*sl2)
     &             +gc(2)*fmass*sr2 )*d

      else
         d = d * gc(1)
         fvo(1) = fmass*sl1*d
         fvo(2) = fmass*sl2*d
         fvo(3) = (  (pf(0)-pf(3))*sl1 -        fvo(6)*sl2)*d
         fvo(4) = (-dconjg(fvo(6))*sl1 + (pf(0)+pf(3))*sl2)*d
      end if
c
      return
      end
      subroutine hvq_ggggxx(ga,gb,gc,gd,g, vertex)
c
c This subroutine computes the portion of the amplitude of the four-point 
c coupling of 4 massless color octet gauge bosons (gluons) corresponding 
c to the color structure f^{a,b,e} f{c,d,e}. 
c To optain the complete amplitude, this coupling must be called three
c times (once for each color structure) with the following permutations:
c	call hvq_ggggxx(ga,gb,gc,gd,g,v1)
c       call hvq_ggggxx(ga,gc,gd,gb,g,v2)
c       call hvq_ggggxx(ga,gd,gb,gc,g,v3)
c
c	f^{a,b,e} f{c,d,e}*v1+
c	f^{a,c,e} f{d,b,e}*v2+
c	f^{a,d,e} f{b,c,e}*v3
c (See 2.9.1 of the manual for more information).
c                                                                       
c input:                                                                
c       complex ga(0:3)        : Boson with adjoint color index a 
c       complex gb(0:3)        : Boson with adjoint color index b
c       complex gc(0:3)        : Boson with adjoint color index c 
c       complex gd(0:3)        : Boson with adjoint color index d
c       real    g              : coupling of w31 with w-/w+             
c
      implicit none
      double complex ga(6),gb(6),gc(6),gd(6),vertex
      double complex dv1(0:3),dv2(0:3),dv3(0:3),dv4(0:3),
     &     dvertx,v12,v13,v14,v23,v24,v34
      double precision pga(0:3),pgb(0:3),pgc(0:3),pgd(0:3),g

      save dv1,dv2,dv3, dv4
c      save dv1,dv2,dv3,dv4,dvertx,v12,v13,v14,v23,v24,v34

c

      dv1(0) = dcmplx(ga(1))
      dv1(1) = dcmplx(ga(2))
      dv1(2) = dcmplx(ga(3))
      dv1(3) = dcmplx(ga(4))
      dv2(0) = dcmplx(gb(1))
      dv2(1) = dcmplx(gb(2))
      dv2(2) = dcmplx(gb(3))
      dv2(3) = dcmplx(gb(4))
      dv3(0) = dcmplx(gc(1))
      dv3(1) = dcmplx(gc(2))
      dv3(2) = dcmplx(gc(3))
      dv3(3) = dcmplx(gc(4))
      dv4(0) = dcmplx(gd(1))
      dv4(1) = dcmplx(gd(2))
      dv4(2) = dcmplx(gd(3))
      dv4(3) = dcmplx(gd(4))

      v12 = dv1(0)*dv2(0)-dv1(1)*dv2(1)-dv1(2)*dv2(2)-dv1(3)*dv2(3)
      v13 = dv1(0)*dv3(0)-dv1(1)*dv3(1)-dv1(2)*dv3(2)-dv1(3)*dv3(3)
      v14 = dv1(0)*dv4(0)-dv1(1)*dv4(1)-dv1(2)*dv4(2)-dv1(3)*dv4(3)
      v23 = dv2(0)*dv3(0)-dv2(1)*dv3(1)-dv2(2)*dv3(2)-dv2(3)*dv3(3)
      v24 = dv2(0)*dv4(0)-dv2(1)*dv4(1)-dv2(2)*dv4(2)-dv2(3)*dv4(3)
      v34 = dv3(0)*dv4(0)-dv3(1)*dv4(1)-dv3(2)*dv4(2)-dv3(3)*dv4(3)

      dvertx = v14*v23 -v13*v24

      vertex = dcmplx( dvertx ) * (g*g)

c      if (abs(dvertx) .gt. 1d40) then
c         write(*,*) 'Careful',abs(dvertx)
c         write(*,*) v12,v13,v14
c         write(*,*) v23,v24,v34
c      endif
c
      return
      end
      subroutine hvq_gggxxx(wm,wp,w3,g , vertex)
c
c This subroutine computes an amplitude of the three-point coupling of
c the gauge bosons.
c
c input:
c       complex wm(6)          : vector               flow-out W-
c       complex wp(6)          : vector               flow-out W+
c       complex w3(6)          : vector               j3 or A    or Z
c       real    g              : coupling constant    gw or gwwa or gwwz
c
c output:
c       complex vertex         : amplitude               gamma(wm,wp,w3)
c
      implicit none
      double complex wm(6),wp(6),w3(6),vertex
      double complex xv1,xv2,xv3,v12,v23,v31
      double complex p12,p13,p21,p23,p31,p32
      double precision pwm(0:3),pwp(0:3),pw3(0:3),g

      double precision rZero, rTenth
      parameter( rZero = 0.0d0, rTenth = 0.1d0 )

c
      pwm(0) = dble( wm(5))
      pwm(1) = dble( wm(6))
      pwm(2) = dimag(wm(6))
      pwm(3) = dimag(wm(5))
      pwp(0) = dble( wp(5))
      pwp(1) = dble( wp(6))
      pwp(2) = dimag(wp(6))
      pwp(3) = dimag(wp(5))
      pw3(0) = dble( w3(5))
      pw3(1) = dble( w3(6))
      pw3(2) = dimag(w3(6))
      pw3(3) = dimag(w3(5))


      v12 = wm(1)*wp(1)-wm(2)*wp(2)-wm(3)*wp(3)-wm(4)*wp(4)
      v23 = wp(1)*w3(1)-wp(2)*w3(2)-wp(3)*w3(3)-wp(4)*w3(4)
      v31 = w3(1)*wm(1)-w3(2)*wm(2)-w3(3)*wm(3)-w3(4)*wm(4)
      xv1 = rZero
      xv2 = rZero
      xv3 = rZero
      if ( abs(wm(1)).ne.rZero ) then
         if ( abs(wm(1)).ge.max(abs(wm(2)),abs(wm(3)),abs(wm(4)))
     &        *rTenth )
     &      xv1 = pwm(0)/wm(1)
      endif
      if ( abs(wp(1)).ne.rZero ) then
         if ( abs(wp(1)).ge.max(abs(wp(2)),abs(wp(3)),abs(wp(4)))
     &        *rTenth )
     &      xv2 = pwp(0)/wp(1)
      endif
      if ( abs(w3(1)).ne.rZero ) then
         if ( abs(w3(1)).ge.max(abs(w3(2)),abs(w3(3)),abs(w3(4)))
     &        *rTenth )
     &      xv3 = pw3(0)/w3(1)
      endif

      p12 = (pwm(0)-xv1*wm(1))*wp(1)-(pwm(1)-xv1*wm(2))*wp(2)
     &     -(pwm(2)-xv1*wm(3))*wp(3)-(pwm(3)-xv1*wm(4))*wp(4)
      p13 = (pwm(0)-xv1*wm(1))*w3(1)-(pwm(1)-xv1*wm(2))*w3(2)
     &     -(pwm(2)-xv1*wm(3))*w3(3)-(pwm(3)-xv1*wm(4))*w3(4)
      p21 = (pwp(0)-xv2*wp(1))*wm(1)-(pwp(1)-xv2*wp(2))*wm(2)
     &     -(pwp(2)-xv2*wp(3))*wm(3)-(pwp(3)-xv2*wp(4))*wm(4)
      p23 = (pwp(0)-xv2*wp(1))*w3(1)-(pwp(1)-xv2*wp(2))*w3(2)
     &     -(pwp(2)-xv2*wp(3))*w3(3)-(pwp(3)-xv2*wp(4))*w3(4)
      p31 = (pw3(0)-xv3*w3(1))*wm(1)-(pw3(1)-xv3*w3(2))*wm(2)
     &     -(pw3(2)-xv3*w3(3))*wm(3)-(pw3(3)-xv3*w3(4))*wm(4)
      p32 = (pw3(0)-xv3*w3(1))*wp(1)-(pw3(1)-xv3*w3(2))*wp(2)
     &     -(pw3(2)-xv3*w3(3))*wp(3)-(pw3(3)-xv3*w3(4))*wp(4)

      vertex = -(v12*(p13-p23)+v23*(p21-p31)+v31*(p32-p12))*g
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_hiocxx(fic,foc,gc,smass,swidth , hioc)                
c                                                                       
c this subroutine computes an off-shell scalar current from an external 
c antifermion pair.                                                     
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion              |fic>
c       complex foc(6)         : flow-out antifermion              <foc|
c       complex gc(2)          : coupling constants                 gchf
c       real    smass          : mass  of output scalar s               
c       real    swidth         : width of output scalar s               
c                                                                       
c output:                                                               
c       complex hioc(3)        : scalar current           j(<fic|s|foc>)
c                                                                       
      implicit none
      double complex fic(6),foc(6),hioc(3),gc(2),dn
      double precision q(0:3),smass,swidth,q2
c
      hioc(2) = foc(5)-fic(5)
      hioc(3) = foc(6)-fic(6)

      q(0) = dble( hioc(2))
      q(1) = dble( hioc(3))
      q(2) = dimag(hioc(3))
      q(3) = dimag(hioc(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)

      dn = -dcmplx( q2-smass**2, smass*swidth )

      hioc(1) = (  gc(1)*(foc(1)*fic(1)+foc(2)*fic(2))
     &           + gc(2)*(foc(3)*fic(3)+foc(4)*fic(4)) )/dn
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_hiogld(fi,fo,gc,smass,swidth,mNLSP,idecay , hio)
c
c This subroutine computes an off-shell scalar current for the NLSP-
c Goldstino vertex from the external fermion pair. The h.c. of the NLSP
c decay is handled via the input parameter idecay.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gc(2)          : coupling constants                  gsf
c       real    smass          : mass  of output scalar s
c       real    swidth         : width of output scalar s
c       real    mNLSP          : mass of NLSP
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex hio(3)         : scalar current          j^mu(<fo|s|fi>)
c
      implicit none
      double complex fi(6), fo(6), gc(2), hio(3)
      double complex dn, p14p, p14m, p23p, p23m
      double precision q(0:3), smass, swidth, mNLSP, q2
      double precision pG(1:4)
      integer idecay

      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      hio(2) = -fi(5) + fo(5)
      hio(3) = -fi(6) + fo(6)

      if ( idecay.ne.1 .or. idecay.ne.-1 ) then
         write(6,*) 'error in idecay of hvq_hiogld'
         stop
      end if

      q(0) = dble( hio(2))
      q(1) = dble( hio(3))
      q(2) = dimag(hio(3))
      q(3) = dimag(hio(2))
      q2 = q(0)**2 - q(1)**2 - q(2)**2 - q(3)**2

      p14p = q(0) + q(3)
      p14m = q(0) - q(3)
      p23p = q(1) + q(2)*ci
      p23m = q(1) - q(2)*ci

      dn = -dcmplx( q2-smass**2, smass*swidth )

      hio(1) = gc(1)*( ( ( fo(3)*p14p + fo(4)*p23p )*fi(1)
     &                  +( fo(3)*p23m + fo(4)*p14m )*fi(2) )*mNLSP
     &                -( fo(1)*fi(1) + fo(2)*fi(2) )*idecay*smass**2 )

      if ( gc(2).ne.cZero ) then
         hio(1) = hio(1) + gc(2) *
     &            ( ( ( fo(1)*p14m - fo(2)*p23p )*fi(3)
     &               -( fo(1)*p23m - fo(2)*p14p )*fi(4) )*mNLSP
     &             -( fo(3)*fi(3) + fo(4)*fi(4) )*idecay*smass**2 )
      end if

      hio(1) = hio(1)/dn
c
      return
      end
      subroutine hvq_hioxxx(fi,fo,gc,smass,swidth , hio)
c
c This subroutine computes an off-shell scalar current from an external
c fermion pair.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gc(2)          : coupling constants                 gchf
c       real    smass          : mass  of OUTPUT scalar s
c       real    swidth         : width of OUTPUT scalar s
c
c output:
c       complex hio(3)         : scalar current             j(<fi|s|fo>)
c
      implicit none
      double complex fi(6),fo(6),hio(3),gc(2),dn
      double precision q(0:3),smass,swidth,q2

c

      hio(2) = fo(5)-fi(5)
      hio(3) = fo(6)-fi(6)

      q(0) = dble( hio(2))
      q(1) = dble( hio(3))
      q(2) = dimag(hio(3))
      q(3) = dimag(hio(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)


      dn = -dcmplx( q2-smass**2, smass*swidth )

      hio(1) = ( gc(1)*(fo(1)*fi(1)+fo(2)*fi(2))
     &          +gc(2)*(fo(3)*fi(3)+fo(4)*fi(4)) )/dn
c
      return
      end
      subroutine hvq_hsssxx(s1,s2,s3,gc,smass,swidth , hsss)
c
c This subroutine computes an off-shell scalar current from the four-
c scalar coupling.
c
c input:
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex s3(3)          : third  scalar                        s3
c       complex gc             : coupling constant                 ghhhh
c       real    smass          : mass  of OUTPUT scalar s'
c       real    swidth         : width of OUTPUT scalar s'
c
c output:
c       complex hsss(3)        : scalar current           j(s':s1,s2,s3)
c     
      implicit none
      double complex s1(3),s2(3),s3(3),gc,hsss(3),dg
      double precision q(0:3),smass,swidth,q2

c

      hsss(2) = s1(2)+s2(2)+s3(2)
      hsss(3) = s1(3)+s2(3)+s3(3)

      q(0) = dble( hsss(2))
      q(1) = dble( hsss(3))
      q(2) = dimag(hsss(3))
      q(3) = dimag(hsss(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)


      dg = -gc/dcmplx( q2-smass**2, smass*swidth )

      hsss(1) = dg * s1(1)*s2(1)*s3(1)
c
      return
      end
      subroutine hvq_hssxxx(s1,s2,gc,smass,swidth , hss)
c
c This subroutine computes an off-shell scalar current from the three-
c scalar coupling.
c
c input:
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex gc             : coupling constant                  ghhh
c       real    smass          : mass  of OUTPUT scalar s'
c       real    swidth         : width of OUTPUT scalar s'
c
c output:
c       complex hss(3)         : scalar current              j(s':s1,s2)
c     
      implicit none
      double complex s1(3),s2(3),gc,hss(3),dg
      double precision q(0:3),smass,swidth,q2

c

      hss(2) = s1(2)+s2(2)
      hss(3) = s1(3)+s2(3)

      q(0) = dble( hss(2))
      q(1) = dble( hss(3))
      q(2) = dimag(hss(3))
      q(3) = dimag(hss(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)


      dg = -gc/dcmplx( q2-smass**2, smass*swidth )

      hss(1) = dg*s1(1)*s2(1)
c
      return
      end
      subroutine hvq_hvsxxx(vc,sc,gc,smass,swidth , hvs)
c
c This subroutine computes an off-shell scalar current from the vector-
c scalar-scalar coupling.  The coupling is absent in the minimal SM in
c unitary gauge.
c
c input:
c       complex vc(6)          : input vector                          v
c       complex sc(3)          : input scalar                          s
c       complex gc             : coupling constant (s charge)
c       real    smass          : mass  of OUTPUT scalar s'
c       real    swidth         : width of OUTPUT scalar s'
c
c examples of the coupling constant gc for susy particles are as follows:
c   -----------------------------------------------------------
c   |    s1    | (q,i3) of s1  ||   v=A   |   v=Z   |   v=W   |
c   -----------------------------------------------------------
c   | nu~_l    | (  0  , +1/2) ||   ---   |  gzn(1) |  gwf(1) |
c   | e~_l     | ( -1  , -1/2) ||  gal(1) |  gzl(1) |  gwf(1) |
c   | u~_l     | (+2/3 , +1/2) ||  gau(1) |  gzu(1) |  gwf(1) |
c   | d~_l     | (-1/3 , -1/2) ||  gad(1) |  gzd(1) |  gwf(1) |
c   -----------------------------------------------------------
c   | e~_r-bar | ( +1  ,  0  ) || -gal(2) | -gzl(2) | -gwf(2) |
c   | u~_r-bar | (-2/3 ,  0  ) || -gau(2) | -gzu(2) | -gwf(2) |
c   | d~_r-bar | (+1/3 ,  0  ) || -gad(2) | -gzd(2) | -gwf(2) |
c   -----------------------------------------------------------
c where the sc charge is defined by the flowing-OUT quantum number.
c
c output:
c       complex hvs(3)         : scalar current                j(s':v,s)
c     
      implicit none
      double complex vc(6),sc(3),hvs(3),dg,qvv,qpv,gc
      double precision qv(0:3),qp(0:3),qa(0:3),smass,swidth,q2

      double precision rTwo
      parameter( rTwo = 2.0d0 )

c

      hvs(2) = vc(5)+sc(2)
      hvs(3) = vc(6)+sc(3)

      qv(0) = dble(  vc(5))
      qv(1) = dble(  vc(6))
      qv(2) = dimag( vc(6))
      qv(3) = dimag( vc(5))
      qp(0) = dble(  sc(2))
      qp(1) = dble(  sc(3))
      qp(2) = dimag( sc(3))
      qp(3) = dimag( sc(2))
      qa(0) = dble( hvs(2))
      qa(1) = dble( hvs(3))
      qa(2) = dimag(hvs(3))
      qa(3) = dimag(hvs(2))
      q2 = qa(0)**2-(qa(1)**2+qa(2)**2+qa(3)**2)


      dg = -gc/dcmplx( q2-smass**2, smass*swidth )
      qvv = qv(0)*vc(1)-qv(1)*vc(2)-qv(2)*vc(3)-qv(3)*vc(4)
      qpv = qp(0)*vc(1)-qp(1)*vc(2)-qp(2)*vc(3)-qp(3)*vc(4)

      hvs(1) = dg*(rTwo*qpv+qvv)*sc(1)
c
      return
      end
      subroutine hvq_hvvsxx(v1,v2,sc,gc,smass,swidth , hvvs)
c
c This subroutine computes an off-shell scalar current of the vector-
c vector-scalar-scalar coupling.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       complex sc(3)          : input  scalar                        s
c       complex gc             : coupling constant                 gvvhh
c       real    smass          : mass  of OUTPUT scalar s'
c       real    swidth         : width of OUTPUT scalar s'
c
c output:
c       complex hvvs(3)        : scalar current            j(s':v1,v2,s)
c     
      implicit none
      double complex v1(6),v2(6),sc(3),gc,hvvs(3),dg
      double precision q(0:3),smass,swidth,q2

c

      hvvs(2) = v1(5)+v2(5)+sc(2)
      hvvs(3) = v1(6)+v2(6)+sc(3)

      q(0) = dble( hvvs(2))
      q(1) = dble( hvvs(3))
      q(2) = dimag(hvvs(3))
      q(3) = dimag(hvvs(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)


      dg = -gc/dcmplx( q2-smass**2, smass*swidth )

      hvvs(1) = dg*sc(1)
     &         *(v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4))
c
      return
      end
      subroutine hvq_hvvxxx(v1,v2,gc,smass,swidth , hvv)
c
c This subroutine computes an off-shell scalar current from the vector-
c vector-scalar coupling.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       complex gc             : coupling constant                  gvvh
c       real    smass          : mass  of OUTPUT scalar s
c       real    swidth         : width of OUTPUT scalar s
c
c output:
c       complex hvv(3)         : off-shell scalar current     j(s:v1,v2)
c     
      implicit none
      double complex v1(6),v2(6),gc,hvv(3),dg
      double precision q(0:3),smass,swidth,q2

      double precision rZero
      parameter( rZero = 0.0d0 )

c

      hvv(2) = v1(5)+v2(5)
      hvv(3) = v1(6)+v2(6)

      q(0) = dble( hvv(2))
      q(1) = dble( hvv(3))
      q(2) = dimag(hvv(3))
      q(3) = dimag(hvv(2))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)


      dg = -gc/dcmplx( q2-smass**2, smass*swidth )

      hvv(1) = dg*(v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4))
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_ioscxx(fic,foc,sc,gc , vertex)
c
c This subroutine computes an amplitude of the antifermion-antifermion- 
c scalar coupling.                                                      
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion              |fic>
c       complex foc(6)         : flow-out antifermion              <foc|
c       complex sc(3)          : input    scalar                      s 
c       complex gc(2)          : coupling constants                 gchf
c                                                                       
c output:                                                               
c       complex vertex         : amplitude                   <foc|s|fic>
c
      implicit none
      double complex fic(6),foc(6),sc(3),gc(2),vertex
c
      vertex = sc(1)*( gc(1)*(fic(1)*foc(1)+fic(2)*foc(2))
     &                +gc(2)*(fic(3)*foc(3)+fic(4)*foc(4)) )
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_iosgld(fi,fo,sc,gc,smass,mNLSP,idecay , vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-scalar
c SUSY Goldstino coupling. In this routine, the NLSP is decaying to a
c boson and a Goldstino. The h.c. of the NLSP decay is handled via the
c input parameter idecay.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex sc(3)          : input    scalar                      s
c       complex gc(2)          : coupling constants                  gsf
c       real    mNLSP          : mass of the NLSP
c       real    smass          : mass of the scalar boson
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex vertex         : amplitude                     <fo|s|fi>
c
      implicit none
      double complex  fi(6), fo(6), gc(2), sc(3), vertex
      double complex  p14p, p14m, p23p, p23m
      double precision  mNLSP, smass
      integer idecay

      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      if ( idecay.ne.1 .or. idecay.ne.-1 ) then
         write(6,*) 'error in idecay of hvq_iosgld'
         stop
      end if

      p14p = dble(sc(2)) + dimag(sc(2))
      p14m = dble(sc(2)) - dimag(sc(2))
      p23p = dble(sc(3)) + dimag(sc(3))*ci
      p23m = dble(sc(3)) - dimag(sc(3))*ci

      vertex = gc(1) *
     &         ( ( ( fo(3)*p14p + fo(4)*p23p )*fi(1)
     &            +( fo(3)*p23m + fo(4)*p14m )*fi(2) )*mNLSP
     &          -( fo(1)*fi(1) + fo(2)*fi(2) )*idecay*smass**2 )

      if ( gc(2).ne.cZero ) then
         vertex = vertex + gc(2) *
     &            ( ( ( fo(1)*p14m - fo(2)*p23p )*fi(3)
     &               -( fo(1)*p23m - fo(2)*p14p )*fi(4) )*mNLSP
     &             -( fo(3)*fi(3) + fo(4)*fi(4) )*idecay*smass**2 )
      end if

      vertex = vertex * sc(1)
c
      return
      end
      subroutine hvq_iosxxx(fi,fo,sc,gc , vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-scalar
c coupling.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex sc(3)          : input    scalar                      s
c       complex gc(2)          : coupling constants                 gchf
c
c output:
c       complex vertex         : amplitude                     <fo|s|fi>
c     
      implicit none
      double complex fi(6),fo(6),sc(3),gc(2),vertex

c

      vertex = sc(1)*( gc(1)*(fi(1)*fo(1)+fi(2)*fo(2))
     &                +gc(2)*(fi(3)*fo(3)+fi(4)*fo(4)) )
c
      return
      end
      subroutine hvq_iotxkk(fi,fo,tc,g,fmass , vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-vector
c coupling.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex tc(6,4)        : input    tensor                      t
c       real    g              : coupling constant                 -kappa/8
c       real    fmass          : fermion mass                        m_f
c
c output:
c       complex vertex         : amplitude                        <fo|t|fi>
c     
      implicit none
      double complex fi(6), fo(6), tc(6,4), vertex
      double precision g, fmass

      double complex k23, k23s, D1, D2, D3, D4, Tii
      double complex T11, T22, T33, T44, T12, T13, T14, T23, T24, T34
      double complex f13, f14, f23, f24, f31, f32, f41, f42
      double precision k(4), k14p, k14m, m2

      double precision rZero, rTwo
      parameter( rZero = 0.0d0, rTwo = 2.0d0 )
      double complex ci
      parameter( ci = ( 0.0d0, 1.0d0 ) )
c
      m2 = rTwo*fmass

      k(1) = dreal(fi(5)+fo(5))
      k(2) = dreal(fi(6)+fo(6))
      k(3) = dimag(fi(6)+fo(6))
      k(4) = dimag(fi(5)+fo(5))
      k23  = dcmplx( k(2),k(3) )
      k23s = dconjg( k23 )
      k14p = k(1) + k(4)
      k14m = k(1) - k(4)

      f13 = fo(1)*fi(3)
      f14 = fo(1)*fi(4)
      f23 = fo(2)*fi(3)
      f24 = fo(2)*fi(4)
      f31 = fo(3)*fi(1)
      f32 = fo(3)*fi(2)
      f41 = fo(4)*fi(1)
      f42 = fo(4)*fi(2)

      T11 = rTwo*tc(1,1)
      T22 = rTwo*tc(2,2)
      T33 = rTwo*tc(3,3)
      T44 = rTwo*tc(4,4)
      T12 = tc(1,2) + tc(2,1)
      T13 = tc(1,3) + tc(3,1)
      T14 = tc(1,4) + tc(4,1)
      T23 = tc(2,3) + tc(3,2)
      T24 = tc(2,4) + tc(4,2)
      T34 = tc(3,4) + tc(4,3)

      D1 =   k(1)*(T11-T14) - k(2)*(T12-T24)
     &     - k(3)*(T13-T34) - k(4)*(T14-T44)

      D2 = - k(1)*(T12-ci*T13) + k(2)*(T22-ci*T23)
     &     + k(3)*(T23-ci*T33) + k(4)*(T24-ci*T34)

      D3 = - k(1)*(T12+ci*T13) + k(2)*(T22+ci*T23)
     &     + k(3)*(T23+ci*T33) + k(4)*(T24+ci*T34)

      D4 =   k(1)*(T11+T14) - k(2)*(T12+T24)
     &     - k(3)*(T13+T34) - k(4)*(T14+T44)

      Tii = T11 - T22 - T33 - T44

      vertex = D1*(f13+f42) + D2*(f14-f32) + D3*(f23-f41) + D4*(f24+f31)

      vertex = vertex + Tii*( - k14p*(f24+f31) - k14m*(f13+f42)
     &                        +  k23*(f23-f41) + k23s*(f14-f32) )

      if ( fmass.ne.rZero ) then
         vertex = vertex + m2*Tii*(  fo(1)*fi(1) + fo(2)*fi(2)
     &                             + fo(3)*fi(3) + fo(4)*fi(4) )
      end if

      vertex = vertex * g
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_iovcxx(fic,foc,vc,gc , vertex)
c
c this subroutine computes an amplitude of the antifermion-antifermion- 
c vector coupling.                                                      
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion              |fic>
c       complex foc(6)         : flow-out antifermion              <foc|
c       complex vc(6)          : input    vector                      v 
c       complex gc(2)          : coupling constants                  gvf
c                                                                       
c output:                                                               
c       complex vertex         : amplitude                   <foc|v|fic>
c
      implicit none
      double complex fic(6),foc(6),vc(6),gc(2),vertex

      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      vertex = - gc(1)*( (foc(1)*fic(3)+foc(2)*fic(4))*vc(1)
     &                  -(foc(1)*fic(4)+foc(2)*fic(3))*vc(2)
     &                  +(foc(1)*fic(4)-foc(2)*fic(3))*vc(3)*cImag
     &                  -(foc(1)*fic(3)-foc(2)*fic(4))*vc(4)       )

      if ( gc(2).ne.cZero ) then
         vertex = vertex
     &            - gc(2)*( (foc(3)*fic(1)+foc(4)*fic(2))*vc(1)
     &                     +(foc(3)*fic(2)+foc(4)*fic(1))*vc(2)
     &                     -(foc(3)*fic(2)-foc(4)*fic(1))*vc(3)*cImag
     &                     +(foc(3)*fic(1)-foc(4)*fic(2))*vc(4)       )
      end if
c
      return
      end
      subroutine hvq_iovdmx(fi,fo,vc,gc, vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-vector
c dipole moment (non-renormalizable) coupling.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2,2)        : coupling constants                  gvf
c                              : first index is L,R as normal
c                              : second index is EDM,-MDM
c
c output:
c       complex vertex         : amplitude                     <fo|v|fi>
c
      implicit none
      double complex fi(6), fo(6), vc(6), vertex, gc(2,2)

      double complex q(5:6), dum1, dum2
      double complex f1122, f12, f21, f3344, f34, f43
      double complex f12p21, f12m21, f34p43, f34m43
      double complex kvc21, kvc31, kvc41, kvc32, kvc42, kvc43
      double precision  rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      q(5) = fi(5) - fo(5)
      q(6) = fi(6) - fo(6)

      f1122  = fo(1)*fi(1) - fo(2)*fi(2)
      f12    = fo(1)*fi(2)
      f21    = fo(2)*fi(1)
      f12p21 = f12 + f21
      f12m21 = f12 - f21

      kvc21 = ( dble(q(6))*vc(1) -  dble(q(5))*vc(2))*cImag
      kvc31 =  dimag(q(6))*vc(1) -  dble(q(5))*vc(3)
      kvc41 = (dimag(q(5))*vc(1) -  dble(q(5))*vc(4))*cImag
      kvc32 =  dimag(q(6))*vc(2) -  dble(q(6))*vc(3)
      kvc42 = (dimag(q(5))*vc(2) -  dble(q(6))*vc(4))*cImag
      kvc43 =  dimag(q(5))*vc(3) - dimag(q(6))*vc(4)

      dum1 =   ( kvc31 + kvc42 )*f12m21
     &       + ( kvc32 + kvc41 )*f1122
     &       + ( kvc43 + kvc21 )*f12p21

c     (-) from gamma^5 in EDM only
      vertex = ( -gc(1,1) + cImag*gc(1,2) )*dum1    

      if ( gc(2,1).ne.cZero .or.
     &     gc(2,2).ne.cZero      ) then
         f3344  = fo(3)*fi(3) - fo(4)*fi(4)
         f34    = fo(3)*fi(4)
         f43    = fo(4)*fi(3)
         f34p43 = f34 + f43
         f34m43 = f34 - f43
         dum2 =   (-kvc31 + kvc42 )*f34m43
     &          + ( kvc32 - kvc41 )*f3344
     &          + ( kvc43 - kvc21 )*f34p43
         vertex = vertex + ( gc(2,1) + cImag*gc(2,2) )*dum2
      end if
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_iovgld(fi,fo,vc,gc,idecay , vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-vector
c SUSY Goldstino coupling. In this routine, the NLSP is decaying to a
c boson and a Goldstino. The h.c. of the NLSP decay is handled via the
c input parameter idecay (picks out correct Goldstino momentum).
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex vertex         : amplitude                     <fo|v|fi>
c
      implicit none
      double complex  fi(6), fo(6), gc(2), vc(6), vertex
      double complex  AdotpG, A14p, A14m, A23p, A23m
      double complex  p14p, p14m, p23p, p23m
      double precision  pdotpG
      integer idecay

      double precision rOne
      parameter( rOne = 1.0d0 )
      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      if ( idecay.eq.1 ) then
         pdotpG =  dble( vc(5))*dble( fo(5))
     &           - dble( vc(6))*dble( fo(6))
     &           - dimag(vc(6))*dimag(fo(6))
     &           - dimag(vc(5))*dimag(fo(5))
         AdotpG =  vc(1)*dble( fo(5)) - vc(2)*dble( fo(6))
     &           - vc(3)*dimag(fo(6)) - vc(4)*dimag(fo(5))
      else if ( idecay.eq.-1 ) then
         pdotpG =  dble( vc(5))*dble( fi(5))
     &           - dble( vc(6))*dble( fi(6))
     &           - dimag(vc(6))*dimag(fi(6))
     &           - dimag(vc(5))*dimag(fi(5))
         AdotpG =  vc(1)*dble( fi(5)) - vc(2)*dble( fi(6))
     &           - vc(3)*dimag(fi(6)) - vc(4)*dimag(fi(5))
      else
         write(6,*) 'error in idecay of hvq_iovgld'
         stop
      end if

      p14p = dble(vc(5)) + dimag(vc(5))
      p14m = dble(vc(5)) - dimag(vc(5))
      p23p = vc(6)
      p23m = dconjg(vc(6))
      A14p = vc(1) + vc(4)
      A14m = vc(1) - vc(4)
      A23p = vc(2) + vc(3)*ci
      A23m = vc(2) - vc(3)*ci

      vertex = gc(1)*( ( ( fo(3)*p14p + fo(4)*p23p )*fi(1)
     &                  +( fo(3)*p23m + fo(4)*p14m )*fi(2) )*AdotpG
     &                -( ( fo(3)*A14p + fo(4)*A23p )*fi(1)
     &                  +( fo(3)*A23m + fo(4)*A14m )*fi(2) )*pdotpG )

      if ( gc(2).ne.cZero ) then
         vertex = vertex
     &          + gc(2)*( ( (fo(1)*p14m - fo(2)*p23p )*fi(3)
     &                     -(fo(1)*p23m - fo(2)*p14p )*fi(4) )*AdotpG
     &                   -( (fo(1)*A14m - fo(2)*A23p )*fi(3)
     &                     -(fo(1)*A23m - fo(2)*A14p )*fi(4) )*pdotpG )
      end if

      vertex = vertex * idecay
c
      return
      end
      subroutine hvq_iovtkk(fi,fo,vc,tc,g , vertex)
c
c This subroutine computes an amplitude of the three-point coupling of
c two gauge bosons and a Kaluza-Klein tensor boson.
c
c input:
c       complex fi(6)          : flow-in  fermion     SM |fi>
c       complex fo(6)          : flow-out fermion     SM <fo|
c       complex vc(6)          : vector               SM   v
c       complex tc(6,4)        : tensor               KK   t
c       real    g(2)           : coupling constant    -g(L,R)*kappa/4
c
c output:
c       complex vertex         : amplitude            gamma(fi,fo,vc,tc)
c     
      implicit none
      double complex fi(6), fo(6), vc(6), tc(6,4), vertex
      double precision g(2)

      double complex f13, f14, f23, f24, f31, f32, f41, f42
      double complex fs1L, fs1R, fs2L, fs2R, fs3L, fs3R, fs4L, fs4R
      double complex T12, T13, T14, T23, T24, T34

      double precision rZero, rTwo
      parameter( rZero = 0.0d0, rTwo = 2.0d0 )
      double complex ci
      parameter( ci = ( 0.0d0, 1.0d0 ) )
c
      f31 = fo(3)*fi(1)*g(1)
      f32 = fo(3)*fi(2)*g(1)
      f41 = fo(4)*fi(1)*g(1)
      f42 = fo(4)*fi(2)*g(1)

      fs1L =  f31 + f42
      fs2L = -f32 - f41
      fs3L = (f32 - f41)*ci
      fs4L = -f31 + f42

      if ( g(2).ne.rZero ) then
         f14 = fo(1)*fi(4)*g(2)
         f13 = fo(1)*fi(3)*g(2)
         f23 = fo(2)*fi(3)*g(2)
         f24 = fo(2)*fi(4)*g(2)
         fs1R =  f13 + f24
         fs2R =  f23 + f14
         fs3R = (f23 - f14)*ci
         fs4R =  f13 - f24
      end if

      T12 = tc(1,2) + tc(2,1)
      T13 = tc(1,3) + tc(3,1)
      T14 = tc(1,4) + tc(4,1)
      T23 = tc(2,3) + tc(3,2)
      T24 = tc(2,4) + tc(4,2)
      T34 = tc(3,4) + tc(4,3)

      if ( g(2).ne.rZero ) then

         vertex =  (fs1L + fs1R)*(  vc(1)*rTwo*tc(1,1)
     &                            - vc(2)*T12 - vc(3)*T13 - vc(4)*T14 )

     &           + (fs2L + fs2R)*(  vc(2)*rTwo*tc(2,2)
     &                            - vc(1)*T12 + vc(3)*T23 + vc(4)*T24 )

     &           + (fs3L + fs3R)*(  vc(3)*rTwo*tc(3,3)
     &                            - vc(1)*T13 + vc(2)*T23 + vc(4)*T34 )

     &           + (fs4L + fs4R)*(  vc(4)*rTwo*tc(4,4)
     &                            - vc(1)*T14 + vc(2)*T24 + vc(3)*T34 )

         vertex = vertex - rTwo*( tc(1,1)-tc(2,2)-tc(3,3)-tc(4,4) )
     &                         *(  (vc(1)+      vc(4))*(f31+f24)
     &                           + (vc(1)-      vc(4))*(f13+f42)
     &                           + (vc(2)+ci*vc(3))*(f41-f23)
     &                           + (vc(2)-ci*vc(3))*(f32-f14) )

      else

         vertex =  fs1L*(  vc(1)*rTwo*tc(1,1)
     &                   - vc(2)*T12 - vc(3)*T13 - vc(4)*T14 )

     &           + fs2L*(  vc(2)*rTwo*tc(2,2)
     &                   - vc(1)*T12 + vc(3)*T23 + vc(4)*T24 )

     &           + fs3L*(  vc(3)*rTwo*tc(3,3)
     &                   - vc(1)*T13 + vc(2)*T23 + vc(4)*T34 )

     &           + fs4L*(  vc(4)*rTwo*tc(4,4)
     &                   - vc(1)*T14 + vc(2)*T24 + vc(3)*T34 )

         vertex = vertex - rTwo*( tc(1,1)-tc(2,2)-tc(3,3)-tc(4,4) )
     &                         *(  (vc(1)+      vc(4))*f31
     &                           + (vc(1)-      vc(4))*f42
     &                           + (vc(2)+ci*vc(3))*f41
     &                           + (vc(2)-ci*vc(3))*f32 )

      end if
c
      return
      end
      subroutine hvq_iovxxx(fi,fo,vc,gc , vertex)
c
c This subroutine computes an amplitude of the fermion-fermion-vector
c coupling.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex vc(6)          : input    vector                      v
c       complex gc(2)          : coupling constants                  gvf
c
c output:
c       complex vertex         : amplitude                     <fo|v|fi>
c     
      implicit none
      double complex fi(6),fo(6),gc(2),vc(6),vertex

      double precision rZero, rOne
      parameter( rZero = 0.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      vertex =  gc(1)*( (fo(3)*fi(1)+fo(4)*fi(2))*vc(1)
     &                 +(fo(3)*fi(2)+fo(4)*fi(1))*vc(2)
     &                 -(fo(3)*fi(2)-fo(4)*fi(1))*vc(3)*cImag
     &                 +(fo(3)*fi(1)-fo(4)*fi(2))*vc(4)        )

      if ( gc(2).ne.cZero ) then
         vertex = vertex
     &          + gc(2)*( (fo(1)*fi(3)+fo(2)*fi(4))*vc(1)
     &                   -(fo(1)*fi(4)+fo(2)*fi(3))*vc(2)
     &                   +(fo(1)*fi(4)-fo(2)*fi(3))*vc(3)*cImag
     &                   -(fo(1)*fi(3)-fo(2)*fi(4))*vc(4)        )
      end if
c
      return
      end
      subroutine hvq_ixxxxx(p,fmass,nhel,nsf , fi)
c
c This subroutine computes a fermion wavefunction with the flowing-IN
c fermion number.
c
c input:
c       real    p(0:3)         : four-momentum of fermion
c       real    fmass          : mass          of fermion
c       integer nhel = -1 or 1 : helicity      of fermion
c       integer nsf  = -1 or 1 : +1 for particle, -1 for anti-particle
c
c output:
c       complex fi(6)          : fermion wavefunction               |fi>
c     
      implicit none
      double complex fi(6),chi(2)
      double precision p(0:3),sf(2),sfomeg(2),omega(2),fmass,
     &     pp,pp3,sqp0p3,sqm
      integer nhel,nsf,ip,im,nh

      double precision rZero, rHalf, rTwo
      parameter( rZero = 0.0d0, rHalf = 0.5d0, rTwo = 2.0d0 )
      
c

      fi(5) = dcmplx(p(0),p(3))*nsf
      fi(6) = dcmplx(p(1),p(2))*nsf

      nh = nhel*nsf

      if ( fmass.ne.rZero ) then

         pp = min(p(0),dsqrt(p(1)**2+p(2)**2+p(3)**2))
         
         if ( pp.eq.rZero ) then
            
            sqm = dsqrt(fmass)
            ip = (1+nh)/2
            im = (1-nh)/2
            
            fi(1) = ip     * sqm
            fi(2) = im*nsf * sqm
            fi(3) = ip*nsf * sqm
            fi(4) = im     * sqm            

         else

            sf(1) = dble(1+nsf+(1-nsf)*nh)*rHalf
            sf(2) = dble(1+nsf-(1-nsf)*nh)*rHalf
            omega(1) = dsqrt(p(0)+pp)
            omega(2) = fmass/omega(1)
            ip = (3+nh)/2
            im = (3-nh)/2
            sfomeg(1) = sf(1)*omega(ip)
            sfomeg(2) = sf(2)*omega(im)
            pp3 = max(pp+p(3),rZero)
            chi(1) = dcmplx( dsqrt(pp3*rHalf/pp) )
            if ( pp3.eq.rZero ) then
               chi(2) = dcmplx(-nh )
            else
               chi(2) = dcmplx( nh*p(1) , p(2) )/dsqrt(rTwo*pp*pp3)
            endif
            
            fi(1) = sfomeg(1)*chi(im)
            fi(2) = sfomeg(1)*chi(ip)
            fi(3) = sfomeg(2)*chi(im)
            fi(4) = sfomeg(2)*chi(ip)
            
         endif
         
      else
         
         sqp0p3 = dsqrt(max(p(0)+p(3),rZero))*nsf
         chi(1) = dcmplx( sqp0p3 )
         if ( sqp0p3.eq.rZero ) then
            chi(2) = dcmplx(-nhel )*dsqrt(rTwo*p(0))
         else
            chi(2) = dcmplx( nh*p(1), p(2) )/sqp0p3
         endif
         if ( nh.eq.1 ) then
            fi(1) = dcmplx( rZero )
            fi(2) = dcmplx( rZero )
            fi(3) = chi(1)
            fi(4) = chi(2)
         else
            fi(1) = chi(2)
            fi(2) = chi(1)
            fi(3) = dcmplx( rZero )
            fi(4) = dcmplx( rZero )
         endif
      endif
c
      return
      end
      subroutine hvq_j3xxxx(fi,fo,gaf,gzf,zmass,zwidth , j3)
c
c This subroutine computes the sum of photon and Z currents with the
c suitable weights ( j(W3) = cos(theta_W) j(Z) + sin(theta_W) j(A) ).
c The output j3 is useful as an input of hvq_vvvxxx, hvq_jvvxxx or w3w3xx.
c The photon propagator is given in Feynman gauge, and the Z propagator
c is given in unitary gauge.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gaf(2)         : fi couplings with A                 gaf
c       complex gzf(2)         : fi couplings with Z                 gzf
c       real    zmass          : mass  of Z
c       real    zwidth         : width of Z
c
c output:
c       complex j3(6)          : W3 current             j^mu(<fo|w3|fi>)
c     
      implicit none
      double complex fi(6),fo(6),j3(6),gaf(2),gzf(2)
      double complex c0l,c1l,c2l,c3l,csl,c0r,c1r,c2r,c3r,csr,dz,ddif
      double complex gn,gz3l,ga3l
      double complex cm2  ! mass**2- I Gamma mass (Fabio)
      double precision q(0:3),zmass,zwidth,zm2,zmw
      double precision q2,da,ww,cw,sw

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      j3(5) = fo(5)-fi(5)
      j3(6) = fo(6)-fi(6)

      q(0) = -dble( j3(5))
      q(1) = -dble( j3(6))
      q(2) = -dimag(j3(6))
      q(3) = -dimag(j3(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      zm2 = zmass**2
      zmw = zmass*zwidth


      da = rOne/q2
C      ww = max(dsign(zmw,q2), rZero)
      dz = rOne/dcmplx( q2-zm2, zmw )
      ddif = dcmplx( -zm2, zmw )*da*dz

c ddif is the difference : ddif=da-dz
c  For the running width, use below instead of the above ww,dz and ddif.
c      ww = max( zwidth*q2/zmass, rZero )
c      dz = rOne/dcmplx( q2-zm2, zmw )
c      ddif = dcmplx( -zm2, zmw )*da*dz



      cw = rOne/sqrt(rOne+(gzf(2)/gaf(2))**2)
      sw = sqrt((rOne-cw)*(rOne+cw))
      gn = gaf(2)*sw
      gz3l = gzf(1)*cw
      ga3l = gaf(1)*sw
      c0l =   fo(3)*fi(1)+fo(4)*fi(2)
      c0r =   fo(1)*fi(3)+fo(2)*fi(4)
      c1l = -(fo(3)*fi(2)+fo(4)*fi(1))
      c1r =   fo(1)*fi(4)+fo(2)*fi(3)
      c2l =  (fo(3)*fi(2)-fo(4)*fi(1))*cImag
      c2r = (-fo(1)*fi(4)+fo(2)*fi(3))*cImag
      c3l =  -fo(3)*fi(1)+fo(4)*fi(2)
      c3r =   fo(1)*fi(3)-fo(2)*fi(4)

c     Fabio's implementation of the fixed width
      cm2=dcmplx( zm2, -zmw )
c     csl = (q(0)*c0l-q(1)*c1l-q(2)*c2l-q(3)*c3l)/zm2
c     csr = (q(0)*c0r-q(1)*c1r-q(2)*c2r-q(3)*c3r)/zm2
      csl = (q(0)*c0l-q(1)*c1l-q(2)*c2l-q(3)*c3l)/cm2
      csr = (q(0)*c0r-q(1)*c1r-q(2)*c2r-q(3)*c3r)/cm2
      
      j3(1) =  gz3l*dz*(c0l-csl*q(0))+ga3l*c0l*da
     &       + gn*(c0r*ddif+csr*q(0)*dz)
      j3(2) =  gz3l*dz*(c1l-csl*q(1))+ga3l*c1l*da
     &       + gn*(c1r*ddif+csr*q(1)*dz)
      j3(3) =  gz3l*dz*(c2l-csl*q(2))+ga3l*c2l*da
     &       + gn*(c2r*ddif+csr*q(2)*dz)
      j3(4) =  gz3l*dz*(c3l-csl*q(3))+ga3l*c3l*da
     &       + gn*(c3r*ddif+csr*q(3)*dz)
c
      return
      end
      subroutine hvq_jeexxx(eb,ef,shlf,chlf,phi,nhb,nhf,nsf , jee)
c
c This subroutine computes an off-shell photon wavefunction emitted from
c the electron or positron beam, with a special care for the small angle
c region.  The momenta are measured in the laboratory frame, where the
c e- (e+) beam is along the positive (negative) z axis.
c
c input:
c       real    eb             : energy (gev)    of beam  e-/e+
c       real    ef             : energy (gev)    of final e-/e+
c       real    shlf           : sin(theta/2)    of final e-/e+
c       real    chlf           : cos(theta/2)    of final e-/e+
c       real    phi            : azimuthal angle of final e-/e+
c       integer nhb  = -1 or 1 : helicity        of beam  e-/e+
c       integer nhf  = -1 or 1 : helicity        of final e-/e+
c       integer nsf  = -1 or 1 : +1 for electron, -1 for positron
c
c output:
c       complex jee(6)         : off-shell photon          j^mu(<e|a|e>)
c     
      implicit none
      double complex jee(6),coeff
      double precision cs(2),eb,ef,shlf,chlf,phi,alpha,gal,hi,sf,sfh
      double precision x,me2,q2,rfp,rfm,snp,csp,rxc,c,s
      integer nhb,nhf,nsf

      double precision rZero, rHalf, rOne, rTwo, rFour, rOte
      double precision rPi, rIalph
      parameter( rZero = 0.0d0, rHalf = 0.5d0, rOne = 1.0d0 )
      parameter( rTwo = 2.0d0, rFour = 4.0d0, rOte = 128.9d0 )
      parameter( rPi = 3.14159265358979323846d0 )
      parameter( rIalph = 137.0359895d0 )

      double precision me
      parameter( me = 0.51099906d-3 )

c

      alpha = rOne/rOte
      gal = sqrt(alpha*rFour*rPi)

      hi = nhb
      sf = nsf
      sfh = nhb*nsf
      cs((3+nsf)/2) = shlf
      cs((3-nsf)/2) = chlf
c cs(1)=chlf and cs(2)=shlf for electron
c cs(1)=shlf and cs(2)=chlf for positron
      x = ef/eb
      me2 = me**2
      q2 = - rFour*cs(2)**2*(ef*eb-me2)
     &     + sf*(rOne-x)**2/x*(shlf+chlf)*(shlf-chlf)*me2
      rfp = (1+nsf)
      rfm = (1-nsf)
      snp = sin(phi)
      csp = cos(phi)

      if ( nhb.eq.nhf ) then
         rxc = rTwo*x/(rOne-x)*cs(1)**2
         coeff = gal*rTwo*eb*sqrt(x)*cs(2)/q2
     &          *(dcmplx( rfp )-rfm*dcmplx( csp, -snp*hi ))*rHalf
         jee(1) = dcmplx( rZero )
         jee(2) = coeff*dcmplx( (rOne+rxc)*csp, -sfh*snp )
         jee(3) = coeff*dcmplx( (rOne+rxc)*snp,  sfh*csp )
         jee(4) = coeff*(-sf*rxc/cs(1)*cs(2))
      else
         coeff = gal*me/q2/sqrt(x)
     &          *(dcmplx( rfp )+rfm*dcmplx( csp, snp*hi ))*rHalf*hi
         jee(1) = -coeff*(rOne+x)*cs(2)*dcmplx( csp , sfh*snp )
         jee(2) =  coeff*(rOne-x)*cs(1)
         jee(3) =  jee(2)*dcmplx( rZero, sfh )
         jee(4) =  jee(1)*sf*(rOne-x)/(rOne+x)
      endif

      c = (chlf+shlf)*(chlf-shlf)
      s = rTwo*chlf*shlf

      jee(5) = -eb*dcmplx( rOne-x, sf-x*c )
      jee(6) =  eb*x*s*dcmplx( csp, snp )
c
      return
      end
      subroutine hvq_jgggxx(w1,w2,w3,g, jw3w)
c
c This subroutine computes an off-shell W+, W-, W3, Z or photon current
c from the four-point gauge boson coupling, including the contributions
c of W exchange diagrams.  The vector propagator is given in Feynman
c gauge for a photon and in unitary gauge for W and Z bosons.  If one
c sets wmass=0.0, then the ggg-->g current is given (see sect 2.9.1 of
c the manual).
c
c input:
c       complex w1(6)          : first  vector                        w1
c       complex w2(6)          : second vector                        w2
c       complex w3(6)          : third  vector                        w3
c       real    g             : first  coupling constant
c                                                  (see the table below)
c
c output:
c       complex jw3w(6)        : W current             j^mu(w':w1,w2,w3)
c
      implicit none
      double complex w1(6),w2(6),w3(6),jw3w(6)
      double complex dw1(0:3),dw2(0:3),dw3(0:3)
      double complex jj(0:3),dv,w32,w13
      double precision p1(0:3),p2(0:3),p3(0:3),q(0:3),g,dg2,q2

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )

c

      jw3w(5) = w1(5)+w2(5)+w3(5)
      jw3w(6) = w1(6)+w2(6)+w3(6)


      dw1(0) = dcmplx(w1(1))
      dw1(1) = dcmplx(w1(2))
      dw1(2) = dcmplx(w1(3))
      dw1(3) = dcmplx(w1(4))
      dw2(0) = dcmplx(w2(1))
      dw2(1) = dcmplx(w2(2))
      dw2(2) = dcmplx(w2(3))
      dw2(3) = dcmplx(w2(4))
      dw3(0) = dcmplx(w3(1))
      dw3(1) = dcmplx(w3(2))
      dw3(2) = dcmplx(w3(3))
      dw3(3) = dcmplx(w3(4))
      p1(0) = dble(      w1(5))
      p1(1) = dble(      w1(6))
      p1(2) = dble(dimag(w1(6)))
      p1(3) = dble(dimag(w1(5)))
      p2(0) = dble(      w2(5))
      p2(1) = dble(      w2(6))
      p2(2) = dble(dimag(w2(6)))
      p2(3) = dble(dimag(w2(5)))
      p3(0) = dble(      w3(5))
      p3(1) = dble(      w3(6))
      p3(2) = dble(dimag(w3(6)))
      p3(3) = dble(dimag(w3(5)))
      q(0) = -(p1(0)+p2(0)+p3(0))
      q(1) = -(p1(1)+p2(1)+p3(1))
      q(2) = -(p1(2)+p2(2)+p3(2))
      q(3) = -(p1(3)+p2(3)+p3(3))

      q2 = q(0)**2 -(q(1)**2 +q(2)**2 +q(3)**2)

      dg2 = dble(g)*dble(g)

      dv = rOne/dcmplx( q2 )

      w32 = dw3(0)*dw2(0)-dw3(1)*dw2(1)-dw3(2)*dw2(2)-dw3(3)*dw2(3)

      w13 = dw1(0)*dw3(0)-dw1(1)*dw3(1)-dw1(2)*dw3(2)-dw1(3)*dw3(3)

      jj(0) = dg2*( dw1(0)*w32 - dw2(0)*w13 )
      jj(1) = dg2*( dw1(1)*w32 - dw2(1)*w13 )
      jj(2) = dg2*( dw1(2)*w32 - dw2(2)*w13 )
      jj(3) = dg2*( dw1(3)*w32 - dw2(3)*w13 )

      jw3w(1) = dcmplx( jj(0)*dv )
      jw3w(2) = dcmplx( jj(1)*dv )
      jw3w(3) = dcmplx( jj(2)*dv )
      jw3w(4) = dcmplx( jj(3)*dv )
c
      return
      end
      subroutine hvq_jggxxx(v1,v2,g, jvv)
c
c This subroutine computes an off-shell vector current from the three-
c point gauge boson coupling.  The vector propagator is given in Feynman
c gauge for a massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       real    g              : coupling constant (see the table below)
c
c output:
c       complex jvv(6)         : vector current            j^mu(v:v1,v2)
c
      implicit none
      double complex v1(6),v2(6),jvv(6),j12(0:3)
      double complex sv1,sv2,v12
      double precision p1(0:3),p2(0:3),q(0:3),g,gs,s

c

      jvv(5) = v1(5) + v2(5)
      jvv(6) = v1(6) + v2(6)

      p1(0) = dble( v1(5))
      p1(1) = dble( v1(6))
      p1(2) = dimag(v1(6))
      p1(3) = dimag(v1(5))
      p2(0) = dble( v2(5))
      p2(1) = dble( v2(6))
      p2(2) = dimag(v2(6))
      p2(3) = dimag(v2(5))
      q(0) = -dble( jvv(5))
      q(1) = -dble( jvv(6))
      q(2) = -dimag(jvv(6))
      q(3) = -dimag(jvv(5))
      s = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)

      v12 = v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4)
      sv1 =   (p2(0)-q(0))*v1(1) -(p2(1)-q(1))*v1(2)
     &      - (p2(2)-q(2))*v1(3) -(p2(3)-q(3))*v1(4)
      sv2 = - (p1(0)-q(0))*v2(1) +(p1(1)-q(1))*v2(2)
     &      + (p1(2)-q(2))*v2(3) +(p1(3)-q(3))*v2(4)
      j12(0) = (p1(0)-p2(0))*v12 +sv1*v2(1) +sv2*v1(1)
      j12(1) = (p1(1)-p2(1))*v12 +sv1*v2(2) +sv2*v1(2)
      j12(2) = (p1(2)-p2(2))*v12 +sv1*v2(3) +sv2*v1(3)
      j12(3) = (p1(3)-p2(3))*v12 +sv1*v2(4) +sv2*v1(4)

      gs = -g/s

      jvv(1) = gs*j12(0)
      jvv(2) = gs*j12(1)
      jvv(3) = gs*j12(2)
      jvv(4) = gs*j12(3)
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_jiocxx(fic,foc,gc,vmass,vwidth , jioc)
c
c This subroutine computes an off-shell vector current from an external 
c antifermion pair. The vector boson propagator is given in Feynman     
c gauge for a massless vector and in unitary gauge for a massive vector.
c                                                                       
c input:                                                                
c       complex fic(6)         : flow-in  antifermion              |fic>
c       complex foc(6)         : flow-out antifermion              <foc|
c       complex gc(2)          : coupling constants                  gvf
c       real    vmass          : mass  of output vector v               
c       real    vwidth         : width of output vector v               
c                                                                       
c output:                                                               
c       complex jioc(6)        : vector current        j^mu(<foc|v|fic>)
c
      implicit none
      double complex fic(6),foc(6),gc(2),jioc(6),c0,c1,c2,c3,cs,d
      double precision q(0:3),vmass,vwidth,q2,vm2,dd

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      jioc(5) = foc(5)-fic(5)
      jioc(6) = foc(6)-fic(6)

      q(0) = dble( jioc(5))
      q(1) = dble( jioc(6))
      q(2) = dimag(jioc(6))
      q(3) = dimag(jioc(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2

      if ( vmass.ne.rZero ) then

         d = -rOne/dcmplx( q2-vm2, vmass*vwidth )
c  for the running width, use below instead of the above d.
c         d = -rOne/dcmplx( q2-vm2, max(vwidth*q2/vmass,rZero) )

         if ( gc(2).ne.cZero ) then
            c0=  gc(2)*( foc(3)*fic(1)+foc(4)*fic(2))
     &          +gc(1)*( foc(1)*fic(3)+foc(2)*fic(4))
            c1= -gc(2)*( foc(3)*fic(2)+foc(4)*fic(1))
     &          +gc(1)*( foc(1)*fic(4)+foc(2)*fic(3))
            c2=( gc(2)*( foc(3)*fic(2)-foc(4)*fic(1)) 
     &          +gc(1)*(-foc(1)*fic(4)+foc(2)*fic(3)))*cImag
            c3=  gc(2)*(-foc(3)*fic(1)+foc(4)*fic(2))
     &          +gc(1)*( foc(1)*fic(3)-foc(2)*fic(4))
         else
            d = d*gc(1)
            c0 =  foc(1)*fic(3)+foc(2)*fic(4)
            c1 =  foc(1)*fic(4)+foc(2)*fic(3)
            c2 =(-foc(1)*fic(4)+foc(2)*fic(3))*cImag
            c3 =  foc(1)*fic(3)-foc(2)*fic(4)
         end if
         cs = (q(0)*c0-q(1)*c1-q(2)*c2-q(3)*c3)/vm2
         jioc(1) = (c0-cs*q(0))*d
         jioc(2) = (c1-cs*q(1))*d
         jioc(3) = (c2-cs*q(2))*d
         jioc(4) = (c3-cs*q(3))*d

      else

         d = dcmplx( -rOne/q2, rZero )
         if ( gc(2).ne.cZero ) then
            jioc(1) = ( gc(2)*( foc(3)*fic(1)+foc(4)*fic(2))
     &                 +gc(1)*( foc(1)*fic(3)+foc(2)*fic(4)) )*d
            jioc(2) = (-gc(2)*( foc(3)*fic(2)+foc(4)*fic(1))
     &                 +gc(1)*( foc(1)*fic(4)+foc(2)*fic(3)) )*d
            jioc(3) = ( gc(2)*( foc(3)*fic(2)-foc(4)*fic(1))
     &                 +gc(1)*(-foc(1)*fic(4)+foc(2)*fic(3)) )
     &                *d*cImag
            jioc(4) = ( gc(2)*(-foc(3)*fic(1)+foc(4)*fic(2))
     &                 +gc(1)*( foc(1)*fic(3)-foc(2)*fic(4)) )*d
         else
            d = d*gc(1)
            jioc(1) = ( foc(1)*fic(3)+foc(2)*fic(4))*d
            jioc(2) = ( foc(1)*fic(4)+foc(2)*fic(3))*d
            jioc(3) = (-foc(1)*fic(4)+foc(2)*fic(3))*d*cImag
            jioc(4) = ( foc(1)*fic(3)-foc(2)*fic(4))*d
         end if

      end if
c
      return
      end
      subroutine hvq_jiodmx(fi,fo,gc,vmass,vwidth , jio)
c
c This subroutine computes an off-shell vector dipole moment
c (non-renormalizable) current from an external
c fermion pair.  The vector boson propagator is given in Feynman gauge
c for a massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gc(2,2)        : coupling constants                 gvf
c                              : first index is L,R as normal
c                              : second index is EDM,-MDM
c       real    vmass          : mass  of output vector v
c       real    vwidth         : width of output vector v
c
c output:
c       complex jio(6)         : vector current          j^mu(<fo|v|fi>)
c
      implicit none
      double complex fi(6), fo(6), jio(6), c0, c1, c2, c3, d
      double complex gc(2,2), gL, gR
      double precision  q(0:3), vmass, vwidth, q2, vm2, dd

      double complex f1122, f12, f21, f12p21, f12m21
      double complex f3344, f34, f43, f34p43, f34m43
      double complex dumL1, dumL2, dumL3, dumL4
      double complex dumR1, dumR2, dumR3, dumR4
      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      gL = -gc(1,1) + cImag*gc(1,2)
      gR =  gc(2,1) + cImag*gc(2,2)

      jio(5) = fo(5) - fi(5)
      jio(6) = fo(6) - fi(6)

      q(0) = dble( jio(5))
      q(1) = dble( jio(6))
      q(2) = dimag(jio(6))
      q(3) = dimag(jio(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      f1122  = fo(1)*fi(1) - fo(2)*fi(2)
      f12    = fo(1)*fi(2)
      f21    = fo(2)*fi(1)
      f12p21 = f12 + f21
      f12m21 = f12 - f21
      if ( gc(2,1).ne.cZero .or. gc(2,2).ne.cZero ) then
         f3344  = fo(3)*fi(3) - fo(4)*fi(4)
         f34    = fo(3)*fi(4)
         f43    = fo(4)*fi(3)
         f34p43 = f34 + f43
         f34m43 = f34 - f43
      end if

c note overall (-), since k in vertex is -q above
      dumL1 = -q(2)*f12m21 - cImag*( q(1)*f12p21 + q(3)*f1122 )
      dumL2 =  q(2)*f1122  - cImag*( q(0)*f12p21 - q(3)*f12m21 )
      dumL3 = -q(0)*f12m21 - q(1)*f1122 - q(3)*f12p21
      dumL4 = -q(2)*f12p21 - cImag*( q(0)*f1122  + q(1)*f12m21 )
      if ( gc(2,1).ne.cZero .or. gc(2,2).ne.cZero ) then
         dumR1 =  q(2)*f34m43 + cImag*( q(1)*f34p43 + q(3)*f3344 )
         dumR2 =  q(2)*f3344  + cImag*( q(0)*f34p43 + q(3)*f34m43 )
         dumR3 =  q(0)*f34m43 - q(1)*f3344 - q(3)*f34p43
         dumR4 = -q(2)*f34p43 + cImag*( q(0)*f3344  - q(1)*f34m43 )
      end if

      if ( vmass.ne.rZero ) then

         d = rOne/dcmplx( q2-vm2, vmass*vwidth )

         c0 = gL*dumL1
         c1 = gL*dumL2
         c2 = gL*dumL3
         c3 = gL*dumL4

         if ( gc(2,1).ne.cZero .or.
     &        gc(2,2).ne.cZero      ) then
            c0 = c0 + gR*dumR1
            c1 = c1 + gR*dumR2
            c2 = c2 + gR*dumR3
            c3 = c3 + gR*dumR4
         end if

         jio(1) = c0*d
         jio(2) = c1*d
         jio(3) = c2*d
         jio(4) = c3*d

      else

         dd = rOne/q2

         jio(1) = gL*dumL1
         jio(2) = gL*dumL2
         jio(3) = gL*dumL3
         jio(4) = gL*dumL4

         if ( gc(2,1).ne.cZero .or.
     &        gc(2,2).ne.cZero      ) then
            jio(1) = jio(1) + gR*dumR1
            jio(2) = jio(2) + gR*dumR2
            jio(3) = jio(3) + gR*dumR3
            jio(4) = jio(4) + gR*dumR4
         end if

         jio(1) = jio(1)*dd
         jio(2) = jio(2)*dd
         jio(3) = jio(3)*dd
         jio(4) = jio(4)*dd

      end if
c
      return
      end
c
c ----------------------------------------------------------------------
c
      subroutine hvq_jiogld(fi,fo,gc,vmass,vwidth,idecay , jio)
c
c This subroutine computes an off-shell vector current for the NLSP-
c Goldstino vertex from an external fermion pair. The vector boson 
c propagator is given in feynman gauge for a massless vector and in 
c unitary gauge for a massive vector. The h.c. of the NLSP decay is
c handled via the input parameter idecay (picks out correct
c Goldstino momentum).
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gc(2)          : coupling constants                  gvf
c       real    vmass          : mass  of output vector v
c       real    vwidth         : width of output vector v
c       integer idecay         :  1 for NLSP decay to Goldstino
c                              : -1 for Goldstino to NLSP (h.c. of above)
c
c output:
c       complex jio(6)         : vector current          j^mu(<fo|v|fi>)
c
      implicit none
      double complex  fi(6), fo(6), gc(2), jio(6), c0, c1, c2, c3, cs
      double complex  d, dum, p14p, p14m, p23p, p23m
      double precision  q(0:3), vmass, vwidth, q2, vm2, dd
      double precision  pG(1:4), pdotpG
      integer idecay

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex ci, cZero
      parameter( ci = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )
c
      if ( idecay.eq.1 ) then
         pG(1) =  dble(fo(5))
         pG(2) =  dble(fo(6))
         pG(3) = dimag(fo(6))
         pG(4) = dimag(fo(5))
      else if ( idecay.eq.-1 ) then
         pG(1) =  dble(fi(5))
         pG(2) =  dble(fi(6))
         pG(3) = dimag(fi(6))
         pG(4) = dimag(fi(5))
      else
         write(6,*) 'error in idecay of hvq_jiogld'
         stop
      end if

      jio(5) = fo(5) - fi(5)
      jio(6) = fo(6) - fi(6)

      q(0) = dble( jio(5))
      q(1) = dble( jio(6))
      q(2) = dimag(jio(6))
      q(3) = dimag(jio(5))
      q2  = q(0)**2 - q(1)**2 - q(2)**2 - q(3)**2
      vm2 = vmass**2

      pdotpG = q(0)*pG(1) - q(1)*pG(2) - q(2)*pG(3) - q(3)*pG(4)

      p14p = q(0) + q(3)
      p14m = q(0) - q(3)
      p23p = jio(6)
      p23m = dconjg(jio(6))

      if ( vmass.ne.rZero ) then

         d = rOne/dcmplx( q2-vm2, vmass*vwidth )
         d = d*idecay
c  for the running width, use below instead of the above d.
c         d = rOne/dcmplx( q2-vm2, max(vwidth*q2/vmass,rZero) )

         if ( gc(2).ne.cZero ) then
            dum =  ( (fo(3)*p14p + fo(4)*p23p)*fi(1)
     &              +(fo(3)*p23m + fo(4)*p14m)*fi(2) )*gc(1)
     &           + ( (fo(1)*p14m - fo(2)*p23p)*fi(3)
     &              -(fo(1)*p23m - fo(2)*p14p)*fi(4) )*gc(2)

            c0 =  dum*pG(1)
     &           -pdotpG*( gc(1)*( fo(3)*fi(1) + fo(4)*fi(2) )
     &                    +gc(2)*( fo(1)*fi(3) + fo(2)*fi(4) ) )
            c1 =  dum*pG(2)
     &           -pdotpG*(-gc(1)*( fo(4)*fi(1) + fo(3)*fi(2) )
     &                    +gc(2)*( fo(2)*fi(3) + fo(1)*fi(4) ) )
            c2 =  dum*pG(3)
     &           -pdotpG*( gc(1)*(-fo(4)*fi(1) + fo(3)*fi(2) )
     &                    +gc(2)*( fo(2)*fi(3) - fo(1)*fi(4) ) )*ci
            c3 =  dum*pG(4)
     &           -pdotpG*( gc(1)*(-fo(3)*fi(1) + fo(4)*fi(2) )
     &                    +gc(2)*( fo(1)*fi(3) - fo(2)*fi(4) ) )
         else
            d = d*gc(1)
            dum =  (fo(3)*p14p + fo(4)*p23p)*fi(1)
     &            +(fo(3)*p23m + fo(4)*p14m)*fi(2)

            c0 = dum*pG(1) - ( fo(3)*fi(1) + fo(4)*fi(2) )*pdotpG
            c1 = dum*pG(2) + ( fo(4)*fi(1) + fo(3)*fi(2) )*pdotpG
            c2 = dum*pG(3) + ( fo(4)*fi(1) - fo(3)*fi(2) )*pdotpG*ci
            c3 = dum*pG(4) + ( fo(3)*fi(1) - fo(4)*fi(2) )*pdotpG
         end if

         cs = (q(0)*c0 - q(1)*c1 - q(2)*c2 - q(3)*c3) / vm2

         jio(1) = (c0-cs*q(0))*d
         jio(2) = (c1-cs*q(1))*d
         jio(3) = (c2-cs*q(2))*d
         jio(4) = (c3-cs*q(3))*d

      else
         dd = idecay*rOne/q2

         if ( gc(2).ne.cZero ) then
            dum =  ( (fo(3)*p14p + fo(4)*p23p)*fi(1)
     &              +(fo(3)*p23m + fo(4)*p14m)*fi(2) )*gc(1)
     &           + ( (fo(1)*p14m - fo(2)*p23p)*fi(3)
     &              -(fo(1)*p23m - fo(2)*p14p)*fi(4) )*gc(2)

            jio(1) = ( dum*pG(1) - pdotpG*(
     &                 gc(1)*( fo(3)*fi(1) + fo(4)*fi(2) )
     &                +gc(2)*( fo(1)*fi(3) + fo(2)*fi(4) ) ) )*dd
            jio(2) = ( dum*pG(2) - pdotpG*(
     &                -gc(1)*( fo(4)*fi(1) + fo(3)*fi(2) )
     &                +gc(2)*( fo(2)*fi(3) + fo(1)*fi(4) ) ) )*dd
            jio(3) = ( dum*pG(3) - pdotpG*ci*(
     &                 gc(1)*(-fo(4)*fi(1) + fo(3)*fi(2) )
     &                +gc(2)*( fo(2)*fi(3) - fo(1)*fi(4) ) ) )*dd
            jio(4) = ( dum*pG(4) - pdotpG*(
     &                 gc(1)*(-fo(3)*fi(1) + fo(4)*fi(2) )
     &                +gc(2)*( fo(1)*fi(3) - fo(2)*fi(4) ) ) )*dd

         else
            dd = dd*gc(1)
            dum =  (fo(3)*p14p + fo(4)*p23p)*fi(1)
     &            +(fo(3)*p23m + fo(4)*p14m)*fi(2)

            jio(1)=dd*(dum*pG(1) - pdotpG*(fo(3)*fi(1) + fo(4)*fi(2)))
            jio(2)=dd*(dum*pG(2) + pdotpG*(fo(4)*fi(1) + fo(3)*fi(2)))
            jio(3)=dd*(dum*pG(3) + ci*pdotpG*(fo(4)*fi(1)-fo(3)*fi(2)))
            jio(4)=dd*(dum*pG(4) + pdotpG*(fo(3)*fi(1) - fo(4)*fi(2)))
         end if
      end if
c
      return
      end
      subroutine hvq_jioxxx(fi,fo,gc,vmass,vwidth , jio)
c
c This subroutine computes an off-shell vector current from an external
c fermion pair.  The vector boson propagator is given in Feynman gauge
c for a massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex fi(6)          : flow-in  fermion                   |fi>
c       complex fo(6)          : flow-out fermion                   <fo|
c       complex gc(2)          : coupling constants                  gvf
c       real    vmass          : mass  of OUTPUT vector v
c       real    vwidth         : width of OUTPUT vector v
c
c output:
c       complex jio(6)         : vector current          j^mu(<fo|v|fi>)
c     
      implicit none
      double complex fi(6),fo(6),gc(2),jio(6),c0,c1,c2,c3,cs,d
      double precision q(0:3),vmass,vwidth,q2,vm2
      double complex cm2 ! mass**2- I Gamma mass (Fabio)


      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
      double complex cImag, cZero
      parameter( cImag = ( 0.0d0, 1.0d0 ), cZero = ( 0.0d0, 0.0d0 ) )

c

      jio(5) = fo(5)-fi(5)
      jio(6) = fo(6)-fi(6)

      q(0) = dble( jio(5))
      q(1) = dble( jio(6))
      q(2) = dimag(jio(6))
      q(3) = dimag(jio(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      if ( vmass.ne.rZero ) then

         d = rOne/dcmplx( q2-vm2, vmass*vwidth )
c     For the running width, use below instead of the above d.
c     d = rOne/dcmplx( q2-vm2, max(vwidth*q2/vmass,rZero) )


         if ( gc(2).ne.cZero ) then
            c0 =  gc(1)*( fo(3)*fi(1)+fo(4)*fi(2))
     &           +gc(2)*( fo(1)*fi(3)+fo(2)*fi(4))
            c1 = -gc(1)*( fo(3)*fi(2)+fo(4)*fi(1))
     &           +gc(2)*( fo(1)*fi(4)+fo(2)*fi(3))
            c2 =( gc(1)*( fo(3)*fi(2)-fo(4)*fi(1))
     &           +gc(2)*(-fo(1)*fi(4)+fo(2)*fi(3)))*cImag
            c3 =  gc(1)*(-fo(3)*fi(1)+fo(4)*fi(2))
     &           +gc(2)*( fo(1)*fi(3)-fo(2)*fi(4))
         else
            d = d*gc(1)
            c0 =   fo(3)*fi(1)+fo(4)*fi(2)
            c1 =  -fo(3)*fi(2)-fo(4)*fi(1)
            c2 = ( fo(3)*fi(2)-fo(4)*fi(1))*cImag
            c3 =  -fo(3)*fi(1)+fo(4)*fi(2)
         end if

c     Fabio's implementation of the fixed width
         cm2=dcmplx( vm2, -vmass*vwidth )
c     cs = (q(0)*c0-q(1)*c1-q(2)*c2-q(3)*c3)/vm2
         cs = (q(0)*c0-q(1)*c1-q(2)*c2-q(3)*c3)/cm2
         jio(1) = (c0-cs*q(0))*d
         jio(2) = (c1-cs*q(1))*d
         jio(3) = (c2-cs*q(2))*d
         jio(4) = (c3-cs*q(3))*d

      else

         d = dcmplx( rOne/q2, rZero )
         if ( gc(2).ne.cZero ) then
            jio(1) = ( gc(1)*( fo(3)*fi(1)+fo(4)*fi(2))
     &                +gc(2)*( fo(1)*fi(3)+fo(2)*fi(4)) )*d
            jio(2) = (-gc(1)*( fo(3)*fi(2)+fo(4)*fi(1))
     &                +gc(2)*( fo(1)*fi(4)+fo(2)*fi(3)) )*d
            jio(3) = ( gc(1)*( fo(3)*fi(2)-fo(4)*fi(1))
     &                +gc(2)*(-fo(1)*fi(4)+fo(2)*fi(3)))
     &               *d*cImag
            jio(4) = ( gc(1)*(-fo(3)*fi(1)+fo(4)*fi(2))
     &                +gc(2)*( fo(1)*fi(3)-fo(2)*fi(4)) )*d
         else
            d = d*gc(1)
            jio(1) =  ( fo(3)*fi(1)+fo(4)*fi(2))*d
            jio(2) = -( fo(3)*fi(2)+fo(4)*fi(1))*d
            jio(3) =  ( fo(3)*fi(2)-fo(4)*fi(1))*d*cImag
            jio(4) =  (-fo(3)*fi(1)+fo(4)*fi(2))*d
         end if

      end if
c
      return
      end
      subroutine hvq_jssxxx(s1,s2,gc,vmass,vwidth , jss)
c
c This subroutine computes an off-shell vector current from the vector-
c scalar-scalar coupling.  The coupling is absent in the minimal SM in
c unitary gauge.  The propagator is given in Feynman gauge for a
c massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex gc             : coupling constant (s1 charge)
c       real    vmass          : mass  of output vector v
c       real    vwidth         : width of output vector v
c
c examples of the coupling constant g for susy particles are as follows:
c   -----------------------------------------------------------
c   |    s1    | (q,i3) of s1  ||   v=A   |   v=Z   |   v=W   |
c   -----------------------------------------------------------
c   | nu~_l    | (  0  , +1/2) ||   ---   |  gzn(1) |  gwf(1) |
c   | e~_l     | ( -1  , -1/2) ||  gal(1) |  gzl(1) |  gwf(1) |
c   | u~_l     | (+2/3 , +1/2) ||  gau(1) |  gzu(1) |  gwf(1) |
c   | d~_l     | (-1/3 , -1/2) ||  gad(1) |  gzd(1) |  gwf(1) |
c   -----------------------------------------------------------
c   | e~_r-bar | ( +1  ,  0  ) || -gal(2) | -gzl(2) | -gwf(2) |
c   | u~_r-bar | (-2/3 ,  0  ) || -gau(2) | -gzu(2) | -gwf(2) |
c   | d~_r-bar | (+1/3 ,  0  ) || -gad(2) | -gzd(2) | -gwf(2) |
c   -----------------------------------------------------------
c where the s1 charge is defined by the flowing-OUT quantum number.
c
c output:
c       complex jss(6)         : vector current            j^mu(v:s1,s2)
c     
      implicit none
      double complex s1(3),s2(3),gc,jss(6),dg,adg
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision pp(0:3),pa(0:3),q(0:3),vmass,vwidth
      double precision q2,vm2,mp2,ma2,m2d

      double precision rZero
      parameter( rZero = 0.0d0 )

c

      jss(5) = s1(2)+s2(2)
      jss(6) = s1(3)+s2(3)

      q(0) = dble( jss(5))
      q(1) = dble( jss(6))
      q(2) = dimag(jss(6))
      q(3) = dimag(jss(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      if ( vmass.ne.rZero ) then

         dg = gc/dcmplx( q2-vm2, vmass*vwidth )
c  For the running width, use below instead of the above dg.
c         dg = g/dcmplx( q2-vm2, max(vwidth*q2/vmass,rZero) )


         adg = dg*s1(1)*s2(1)

         pp(0) = dble( s1(2))
         pp(1) = dble( s1(3))
         pp(2) = dimag(s1(3))
         pp(3) = dimag(s1(2))
         pa(0) = dble( s2(2))
         pa(1) = dble( s2(3))
         pa(2) = dimag(s2(3))
         pa(3) = dimag(s2(2))
         mp2 = pp(0)**2-(pp(1)**2+pp(2)**2+pp(3)**2)
         ma2 = pa(0)**2-(pa(1)**2+pa(2)**2+pa(3)**2)
         m2d = mp2-ma2

c     Fabio's implementation of the fixed width
         cm2=dcmplx( vm2, -vmass*vwidth )
c     jss(1) = adg*( (pp(0)-pa(0)) - q(0)*m2d/vm2)
c     jss(2) = adg*( (pp(1)-pa(1)) - q(1)*m2d/vm2)
c     jss(3) = adg*( (pp(2)-pa(2)) - q(2)*m2d/vm2)
c     jss(4) = adg*( (pp(3)-pa(3)) - q(3)*m2d/vm2)
         jss(1) = adg*( (pp(0)-pa(0)) - q(0)*m2d/cm2)
         jss(2) = adg*( (pp(1)-pa(1)) - q(1)*m2d/cm2)
         jss(3) = adg*( (pp(2)-pa(2)) - q(2)*m2d/cm2)
         jss(4) = adg*( (pp(3)-pa(3)) - q(3)*m2d/cm2)

      else

         adg = gc*s1(1)*s2(1)/q2

         jss(1) = adg*dble( s1(2)-s2(2))
         jss(2) = adg*dble( s1(3)-s2(3))
         jss(3) = adg*dimag(s1(3)-s2(3))
         jss(4) = adg*dimag(s1(2)-s2(2))

      endif
c
      return
      end
      subroutine hvq_jvssxx(vc,s1,s2,gc,vmass,vwidth , jvss)
c
c This subroutine computes an off-shell vector current from the vector-
c vector-scalar-scalar coupling.  The vector propagator is given in
c Feynman gauge for a massless vector and in unitary gauge for a massive
c vector.
c
c input:
c       complex vc(6)          : input  vector                        v
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex gc             : coupling constant                 gvvhh
c       real    vmass          : mass  of output vector v'
c       real    vwidth         : width of output vector v'
c
c output:
c       complex jvss(6)        : vector current         j^mu(v':v,s1,s2)
c     
      implicit none
      double complex vc(6),s1(3),s2(3),gc,jvss(6),dg
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision q(0:3),vmass,vwidth,q2,vk,vm2

      double precision rZero
      parameter( rZero = 0.0d0 )

c

      jvss(5) = vc(5)+s1(2)+s2(2)
      jvss(6) = vc(6)+s1(3)+s2(3)

      q(0) = dble( jvss(5))
      q(1) = dble( jvss(6))
      q(2) = dimag(jvss(6))
      q(3) = dimag(jvss(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      if ( vmass.ne.rZero ) then

         dg = gc*s1(1)*s2(1)/dcmplx( q2-vm2, vmass*vwidth )
c  For the running width, use below instead of the above dg.
c         dg = gc*s1(1)*s2(1)/cmplx( q2-vm2 , max( vwidth*q2/vmass ,rZero))

c     Fabio's implementation of the fixed width
         cm2=dcmplx( vm2, -vmass*vwidth )
c     vk = (q(0)*vc(1)-q(1)*vc(2)-q(2)*vc(3)-q(3)*vc(4))/vm2
         vk = (q(0)*vc(1)-q(1)*vc(2)-q(2)*vc(3)-q(3)*vc(4))/cm2

         jvss(1) = dg*(vc(1)-vk*q(0))
         jvss(2) = dg*(vc(2)-vk*q(1))
         jvss(3) = dg*(vc(3)-vk*q(2))
         jvss(4) = dg*(vc(4)-vk*q(3))

      else

         dg = gc*s1(1)*s2(1)/q2

         jvss(1) = dg*vc(1)
         jvss(2) = dg*vc(2)
         jvss(3) = dg*vc(3)
         jvss(4) = dg*vc(4)

      endif
c
      return
      end
      subroutine hvq_jvsxxx(vc,sc,gc,vmass,vwidth , jvs)
c
c This subroutine computes an off-shell vector current from the vector-
c vector-scalar coupling.  The vector propagator is given in Feynman
c gauge for a massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex vc(6)          : input vector                          v
c       complex sc(3)          : input scalar                          s
c       complex gc             : coupling constant                  gvvh
c       real    vmass          : mass  of output vector v'
c       real    vwidth         : width of output vector v'
c
c output:
c       complex jvs(6)         : vector current             j^mu(v':v,s)
c     
      implicit none
      double complex vc(6),sc(3),gc,jvs(6),dg,vk
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision q(0:3),vmass,vwidth,q2,vm2

      double precision rZero
      parameter( rZero = 0.0d0 )

c

      jvs(5) = vc(5)+sc(2)
      jvs(6) = vc(6)+sc(3)

      q(0) = dble( jvs(5))
      q(1) = dble( jvs(6))
      q(2) = dimag(jvs(6))
      q(3) = dimag(jvs(5))
      q2 = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      if ( vmass.ne.rZero ) then

         dg = gc*sc(1)/dcmplx( q2-vm2, vmass*vwidth )
c  For the running width, use below instead of the above dg.
c         dg = g*sc(1)/dcmplx( q2-vm2, max(vwidth*q2/vmass,rZero) )

c     Fabio's implementation of the fixed width
         cm2=dcmplx( vm2, -vmass*vwidth )
c     vk = (-q(0)*vc(1)+q(1)*vc(2)+q(2)*vc(3)+q(3)*vc(4))/vm2
         vk = (-q(0)*vc(1)+q(1)*vc(2)+q(2)*vc(3)+q(3)*vc(4))/cm2

         jvs(1) = dg*(q(0)*vk+vc(1))
         jvs(2) = dg*(q(1)*vk+vc(2))
         jvs(3) = dg*(q(2)*vk+vc(3))
         jvs(4) = dg*(q(3)*vk+vc(4))

      else

         dg=gc*sc(1)/q2

         jvs(1) = dg*vc(1)
         jvs(2) = dg*vc(2)
         jvs(3) = dg*vc(3)
         jvs(4) = dg*vc(4)

      endif
c
      return
      end
      subroutine hvq_jvvxxx(v1,v2,g,vmass,vwidth , jvv)
c
c This subroutine computes an off-shell vector current from the three-
c point gauge boson coupling.  The vector propagator is given in Feynman
c gauge for a massless vector and in unitary gauge for a massive vector.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       real    g              : coupling constant (see the table below)
c       real    vmass          : mass  of output vector v
c       real    vwidth         : width of output vector v
c
c the possible sets of the inputs are as follows:
c    ------------------------------------------------------------------
c    |   v1   |   v2   |  jvv   |      g       |   vmass  |  vwidth   |
c    ------------------------------------------------------------------
c    |   W-   |   W+   |  A/Z   |  gwwa/gwwz   | 0./zmass | 0./zwidth |
c    | W3/A/Z |   W-   |  W+    | gw/gwwa/gwwz |   wmass  |  wwidth   |
c    |   W+   | W3/A/Z |  W-    | gw/gwwa/gwwz |   wmass  |  wwidth   |
c    ------------------------------------------------------------------
c where all the bosons are defined by the flowing-OUT quantum number.
c
c output:
c       complex jvv(6)         : vector current            j^mu(v:v1,v2)
c     
      implicit none
      double complex v1(6),v2(6),jvv(6),j12(0:3),js,dg
      double complex sv1,sv2,s11,s12,s21,s22,v12
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision p1(0:3),p2(0:3),q(0:3),g,vmass,vwidth,gs,s
      double precision vm2,m1,m2

      double precision rZero
      parameter( rZero = 0.0d0 )
 
c

      jvv(5) = v1(5)+v2(5)
      jvv(6) = v1(6)+v2(6)

      p1(0) =  dble( v1(5))
      p1(1) =  dble( v1(6))
      p1(2) =  dimag(v1(6))
      p1(3) =  dimag(v1(5))
      p2(0) =  dble( v2(5))
      p2(1) =  dble( v2(6))
      p2(2) =  dimag(v2(6))
      p2(3) =  dimag(v2(5))
      q(0)  = -dble( jvv(5))
      q(1)  = -dble( jvv(6))
      q(2)  = -dimag(jvv(6))
      q(3)  = -dimag(jvv(5))
      s = q(0)**2-(q(1)**2+q(2)**2+q(3)**2)
      vm2 = vmass**2


      v12 = v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4)
      sv1 =   (p2(0)-q(0))*v1(1) -(p2(1)-q(1))*v1(2)
     &      - (p2(2)-q(2))*v1(3) -(p2(3)-q(3))*v1(4)
      sv2 = - (p1(0)-q(0))*v2(1) +(p1(1)-q(1))*v2(2)
     &      + (p1(2)-q(2))*v2(3) +(p1(3)-q(3))*v2(4)
      j12(0) = (p1(0)-p2(0))*v12 +sv1*v2(1) +sv2*v1(1)
      j12(1) = (p1(1)-p2(1))*v12 +sv1*v2(2) +sv2*v1(2)
      j12(2) = (p1(2)-p2(2))*v12 +sv1*v2(3) +sv2*v1(3)
      j12(3) = (p1(3)-p2(3))*v12 +sv1*v2(4) +sv2*v1(4)

      if ( vmass.ne.rZero ) then

         m1 = p1(0)**2-(p1(1)**2+p1(2)**2+p1(3)**2)
         m2 = p2(0)**2-(p2(1)**2+p2(2)**2+p2(3)**2)
         s11 = p1(0)*v1(1)-p1(1)*v1(2)-p1(2)*v1(3)-p1(3)*v1(4)
         s12 = p1(0)*v2(1)-p1(1)*v2(2)-p1(2)*v2(3)-p1(3)*v2(4)
         s21 = p2(0)*v1(1)-p2(1)*v1(2)-p2(2)*v1(3)-p2(3)*v1(4)
         s22 = p2(0)*v2(1)-p2(1)*v2(2)-p2(2)*v2(3)-p2(3)*v2(4)

c     Fabio's implementation of the fixed width
         cm2=dcmplx( vm2, -vmass*vwidth )
c     js = (v12*(-m1+m2) +s11*s12 -s21*s22)/vm2
         js = (v12*(-m1+m2) +s11*s12 -s21*s22)/cm2
        
         dg = -g/dcmplx( s-vm2, vmass*vwidth )

c  For the running width, use below instead of the above dg.
c         dg = -g/dcmplx( s-vm2, max(vwidth*s/vmass,rZero) )

         jvv(1) = dg*(j12(0)-q(0)*js)
         jvv(2) = dg*(j12(1)-q(1)*js)
         jvv(3) = dg*(j12(2)-q(2)*js)
         jvv(4) = dg*(j12(3)-q(3)*js)

      else

         gs = -g/s

         jvv(1) = gs*j12(0)
         jvv(2) = gs*j12(1)
         jvv(3) = gs*j12(2)
         jvv(4) = gs*j12(3)

      end if
c
      return
      end
      subroutine hvq_jw3wxx(w1,w2,w3,g1,g2,vmass,vwidth , jw3w)
c
c This subroutine computes an off-shell W+, W-, W3, Z or photon current
c from the four-point gauge boson coupling.  The vector propagator is
c given in Feynman gauge for a photon and in unitary gauge for W and
c Z bosons.  If one sets wmass=0.0, then the ggg-->g current is given
c (see sect 2.9.1 of the manual).
c
c input:
c       complex w1(6)          : first  vector                        w1
c       complex w2(6)          : second vector                        w2
c       complex w3(6)          : third  vector                        w3
c       real    g1             : first  coupling constant
c       real    g2             : second coupling constant
c                                                  (see the table below)
c       real    wmass          : mass  of internal W
c       real    wwidth         : width of internal W
c       real    vmass          : mass  of output W'
c       real    vwidth         : width of output W'
c
c the possible sets of the inputs are as follows:
c   -------------------------------------------------------------------
c   |  w1  |  w2  |  w3  | g1 | g2 |wmass|wwidth|vmass|vwidth || jw3w |
c   -------------------------------------------------------------------
c   |  W-  |  W3  |  W+  | gw |gwwz|wmass|wwidth|zmass|zwidth ||  Z   |
c   |  W-  |  W3  |  W+  | gw |gwwa|wmass|wwidth|  0. |  0.   ||  A   |
c   |  W-  |  Z   |  W+  |gwwz|gwwz|wmass|wwidth|zmass|zwidth ||  Z   |
c   |  W-  |  Z   |  W+  |gwwz|gwwa|wmass|wwidth|  0. |  0.   ||  A   |
c   |  W-  |  A   |  W+  |gwwa|gwwz|wmass|wwidth|zmass|zwidth ||  Z   |
c   |  W-  |  A   |  W+  |gwwa|gwwa|wmass|wwidth|  0. |  0.   ||  A   |
c   -------------------------------------------------------------------
c   |  W3  |  W-  |  W3  | gw | gw |wmass|wwidth|wmass|wwidth ||  W+  |
c   |  W3  |  W+  |  W3  | gw | gw |wmass|wwidth|wmass|wwidth ||  W-  |
c   |  W3  |  W-  |  Z   | gw |gwwz|wmass|wwidth|wmass|wwidth ||  W+  |
c   |  W3  |  W+  |  Z   | gw |gwwz|wmass|wwidth|wmass|wwidth ||  W-  |
c   |  W3  |  W-  |  A   | gw |gwwa|wmass|wwidth|wmass|wwidth ||  W+  |
c   |  W3  |  W+  |  A   | gw |gwwa|wmass|wwidth|wmass|wwidth ||  W-  |
c   |  Z   |  W-  |  Z   |gwwz|gwwz|wmass|wwidth|wmass|wwidth ||  W+  |
c   |  Z   |  W+  |  Z   |gwwz|gwwz|wmass|wwidth|wmass|wwidth ||  W-  |
c   |  Z   |  W-  |  A   |gwwz|gwwa|wmass|wwidth|wmass|wwidth ||  W+  |
c   |  Z   |  W+  |  A   |gwwz|gwwa|wmass|wwidth|wmass|wwidth ||  W-  |
c   |  A   |  W-  |  A   |gwwa|gwwa|wmass|wwidth|wmass|wwidth ||  W+  |
c   |  A   |  W+  |  A   |gwwa|gwwa|wmass|wwidth|wmass|wwidth ||  W-  |
c   -------------------------------------------------------------------
c where all the bosons are defined by the flowing-OUT quantum number.
c
c output:
c       complex jw3w(6)        : W current             j^mu(w':w1,w2,w3)
c     
      implicit none
      double complex w1(6),w2(6),w3(6),jw3w(6)
      double complex dw1(0:3),dw2(0:3),dw3(0:3)
      double complex jj(0:3),j4(0:3),dv,w12,w32,w13,jq
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision g1,g2,vmass,vwidth
      double precision p1(0:3),p2(0:3),p3(0:3),q(0:3)
      double precision dg2,dmv,dwv,mv2,q2

      double precision rZero, rOne, rTwo
      parameter( rZero = 0.0d0, rOne = 1.0d0, rTwo = 2.0d0 )

c

      jw3w(5) = w1(5)+w2(5)+w3(5)
      jw3w(6) = w1(6)+w2(6)+w3(6)

      dw1(0) = dcmplx(w1(1))
      dw1(1) = dcmplx(w1(2))
      dw1(2) = dcmplx(w1(3))
      dw1(3) = dcmplx(w1(4))
      dw2(0) = dcmplx(w2(1))
      dw2(1) = dcmplx(w2(2))
      dw2(2) = dcmplx(w2(3))
      dw2(3) = dcmplx(w2(4))
      dw3(0) = dcmplx(w3(1))
      dw3(1) = dcmplx(w3(2))
      dw3(2) = dcmplx(w3(3))
      dw3(3) = dcmplx(w3(4))
      p1(0) = dble(      w1(5))
      p1(1) = dble(      w1(6))
      p1(2) = dble(dimag(w1(6)))
      p1(3) = dble(dimag(w1(5)))
      p2(0) = dble(      w2(5))
      p2(1) = dble(      w2(6))
      p2(2) = dble(dimag(w2(6)))
      p2(3) = dble(dimag(w2(5)))
      p3(0) = dble(      w3(5))
      p3(1) = dble(      w3(6))
      p3(2) = dble(dimag(w3(6)))
      p3(3) = dble(dimag(w3(5)))
      q(0) = -(p1(0)+p2(0)+p3(0))
      q(1) = -(p1(1)+p2(1)+p3(1))
      q(2) = -(p1(2)+p2(2)+p3(2))
      q(3) = -(p1(3)+p2(3)+p3(3))

      q2 = q(0)**2 -(q(1)**2 +q(2)**2 +q(3)**2)
      dg2 = dble(g1)*dble(g2)
      dmv = dble(vmass)
      dwv = dble(vwidth)
      mv2 = dmv**2


      if ( vmass.eq.rZero ) then
         dv = rOne/dcmplx( q2 )
      else
         dv = rOne/dcmplx( q2-mv2, dmv*dwv )
      endif

c  For the running width, use below instead of the above dv.
c      dv = rOne/dcmplx( q2-mv2 , max(dwv*q2/dmv,rZero) )

      w12=dw1(0)*dw2(0)-dw1(1)*dw2(1)-dw1(2)*dw2(2)-dw1(3)*dw2(3)
      w32=dw3(0)*dw2(0)-dw3(1)*dw2(1)-dw3(2)*dw2(2)-dw3(3)*dw2(3)

      w13=dw1(0)*dw3(0)-dw1(1)*dw3(1)-dw1(2)*dw3(2)-dw1(3)*dw3(3)
      
      j4(0) = dg2*( dw1(0)*w32 + dw3(0)*w12 - rTwo*dw2(0)*w13 )
      j4(1) = dg2*( dw1(1)*w32 + dw3(1)*w12 - rTwo*dw2(1)*w13 )
      j4(2) = dg2*( dw1(2)*w32 + dw3(2)*w12 - rTwo*dw2(2)*w13 )
      j4(3) = dg2*( dw1(3)*w32 + dw3(3)*w12 - rTwo*dw2(3)*w13 )

      jj(0) = j4(0)
      jj(1) = j4(1)
      jj(2) = j4(2)
      jj(3) = j4(3)

      if ( vmass.ne.rZero ) then

c     Fabio's implementation of the fixed width
         cm2=dcmplx( mv2, -dmv*dwv)
c     jq = (jj(0)*q(0)-jj(1)*q(1)-jj(2)*q(2)-jj(3)*q(3))/mv2
         jq = (jj(0)*q(0)-jj(1)*q(1)-jj(2)*q(2)-jj(3)*q(3))/cm2
         
         jw3w(1) = dcmplx( (jj(0)-jq*q(0))*dv )
         jw3w(2) = dcmplx( (jj(1)-jq*q(1))*dv )
         jw3w(3) = dcmplx( (jj(2)-jq*q(2))*dv )
         jw3w(4) = dcmplx( (jj(3)-jq*q(3))*dv )

      else

         jw3w(1) = dcmplx( jj(0)*dv )
         jw3w(2) = dcmplx( jj(1)*dv )
         jw3w(3) = dcmplx( jj(2)*dv )
         jw3w(4) = dcmplx( jj(3)*dv )
      end if
c
      return
      end
      subroutine hvq_jwwwxx(w1,w2,w3,gwwa,gwwz,wmass,wwidth , jwww)
c
c This subroutine computes an off-shell W+/W- current from the four-
c point gauge boson coupling.  The vector propagators for the output
c W and the internal Z bosons are given in unitary gauge, and that of
c the internal photon is given in Feynman gauge.
c
c input:
c       complex w1(6)          : first  vector                        w1
c       complex w2(6)          : second vector                        w2
c       complex w3(6)          : third  vector                        w3
c       real    gwwa           : coupling constant of W and A       gwwa
c       real    gwwz           : coupling constant of W and Z       gwwz
c       real    zmass          : mass  of internal Z
c       real    zwidth         : width of internal Z
c       real    wmass          : mass  of output W
c       real    wwidth         : width of output W
c
c the possible sets of the inputs are as follows:
c   -------------------------------------------------------------------
c   |  w1  |  w2  |  w3  |gwwa|gwwz|zmass|zwidth|wmass|wwidth || jwww |
c   -------------------------------------------------------------------
c   |  W-  |  W+  |  W-  |gwwa|gwwz|zmass|zwidth|wmass|wwidth ||  W+  |
c   |  W+  |  W-  |  W+  |gwwa|gwwz|zmass|zwidth|wmass|wwidth ||  W-  |
c   -------------------------------------------------------------------
c where all the bosons are defined by the flowing-OUT quantum number.
c
c output:
c       complex jwww(6)        : W current             j^mu(w':w1,w2,w3)
c     
      implicit none
      double complex w1(6),w2(6),w3(6),jwww(6)
      double complex dw1(0:3),dw2(0:3),dw3(0:3),jj(0:3)
      double complex dw,w12,w32,w13,jq
      double complex cm2        ! mass**2- I Gamma mass (Fabio)
      double precision gwwa,gwwz,wmass,wwidth
      double precision p1(0:3),p2(0:3),p3(0:3),q(0:3)
      double precision dgwwa2,dgwwz2,dgw2,dmw,dww,mw2,q2

      double precision rZero, rOne, rTwo
      parameter( rZero = 0.0d0, rOne = 1.0d0, rTwo = 2.0d0 )

c

      jwww(5) = w1(5)+w2(5)+w3(5)
      jwww(6) = w1(6)+w2(6)+w3(6)

      dw1(0) = dcmplx(w1(1))
      dw1(1) = dcmplx(w1(2))
      dw1(2) = dcmplx(w1(3))
      dw1(3) = dcmplx(w1(4))
      dw2(0) = dcmplx(w2(1))
      dw2(1) = dcmplx(w2(2))
      dw2(2) = dcmplx(w2(3))
      dw2(3) = dcmplx(w2(4))
      dw3(0) = dcmplx(w3(1))
      dw3(1) = dcmplx(w3(2))
      dw3(2) = dcmplx(w3(3))
      dw3(3) = dcmplx(w3(4))
      p1(0) = dble(      w1(5))
      p1(1) = dble(      w1(6))
      p1(2) = dble(dimag(w1(6)))
      p1(3) = dble(dimag(w1(5)))
      p2(0) = dble(      w2(5))
      p2(1) = dble(      w2(6))
      p2(2) = dble(dimag(w2(6)))
      p2(3) = dble(dimag(w2(5)))
      p3(0) = dble(      w3(5))
      p3(1) = dble(      w3(6))
      p3(2) = dble(dimag(w3(6)))
      p3(3) = dble(dimag(w3(5)))
      q(0) = -(p1(0)+p2(0)+p3(0))
      q(1) = -(p1(1)+p2(1)+p3(1))
      q(2) = -(p1(2)+p2(2)+p3(2))
      q(3) = -(p1(3)+p2(3)+p3(3))
      q2 = q(0)**2 -(q(1)**2 +q(2)**2 +q(3)**2)
      dgwwa2 = dble(gwwa)**2
      dgwwz2 = dble(gwwz)**2
      dgw2 = dgwwa2+dgwwz2
      dmw = dble(wmass)
      dww = dble(wwidth)
      mw2 = dmw**2


      dw = -rOne/dcmplx( q2-mw2, dmw*dww )
c  For the running width, use below instead of the above dw.
c      dw = -rOne/dcmplx( q2-mw2 , max(dww*q2/dmw,rZero) )

      w12=dw1(0)*dw2(0)-dw1(1)*dw2(1)-dw1(2)*dw2(2)-dw1(3)*dw2(3)
      w32=dw3(0)*dw2(0)-dw3(1)*dw2(1)-dw3(2)*dw2(2)-dw3(3)*dw2(3)

      w13=dw1(0)*dw3(0)-dw1(1)*dw3(1)-dw1(2)*dw3(2)-dw1(3)*dw3(3)

      jj(0) = dgw2*( dw1(0)*w32 + dw3(0)*w12 - rTwo*dw2(0)*w13 )
      jj(1) = dgw2*( dw1(1)*w32 + dw3(1)*w12 - rTwo*dw2(1)*w13 )
      jj(2) = dgw2*( dw1(2)*w32 + dw3(2)*w12 - rTwo*dw2(2)*w13 )
      jj(3) = dgw2*( dw1(3)*w32 + dw3(3)*w12 - rTwo*dw2(3)*w13 )

c     Fabio's implementation of the fixed width
      cm2=dcmplx( mw2, -dmw*dww )
c     jq = (jj(0)*q(0)-jj(1)*q(1)-jj(2)*q(2)-jj(3)*q(3))/mw2
      jq = (jj(0)*q(0)-jj(1)*q(1)-jj(2)*q(2)-jj(3)*q(3))/cm2

      jwww(1) = dcmplx( (jj(0)-jq*q(0))*dw )
      jwww(2) = dcmplx( (jj(1)-jq*q(1))*dw )
      jwww(3) = dcmplx( (jj(2)-jq*q(2))*dw )
      jwww(4) = dcmplx( (jj(3)-jq*q(3))*dw )
c
      return
      end
      subroutine hvq_mom2cx(esum,mass1,mass2,costh1,phi1 , p1,p2)
c
c This subroutine sets up two four-momenta in the two particle rest
c frame.
c
c input:
c       real    esum           : energy sum of particle 1 and 2
c       real    mass1          : mass            of particle 1
c       real    mass2          : mass            of particle 2
c       real    costh1         : cos(theta)      of particle 1
c       real    phi1           : azimuthal angle of particle 1
c
c output:
c       real    p1(0:3)        : four-momentum of particle 1
c       real    p2(0:3)        : four-momentum of particle 2
c     
      implicit none
      double precision p1(0:3),p2(0:3),
     &     esum,mass1,mass2,costh1,phi1,md2,ed,pp,sinth1

      double precision rZero, rHalf, rOne, rTwo
      parameter( rZero = 0.0d0, rHalf = 0.5d0 )
      parameter( rOne = 1.0d0, rTwo = 2.0d0 )

c

      md2 = (mass1-mass2)*(mass1+mass2)
      ed = md2/esum
      if ( mass1*mass2.eq.rZero ) then
         pp = (esum-abs(ed))*rHalf
      else
         pp = sqrt((md2/esum)**2-rTwo*(mass1**2+mass2**2)+esum**2)*rHalf
      endif
      sinth1 = sqrt((rOne-costh1)*(rOne+costh1))

      p1(0) = max((esum+ed)*rHalf,rZero)
      p1(1) = pp*sinth1*cos(phi1)
      p1(2) = pp*sinth1*sin(phi1)
      p1(3) = pp*costh1

      p2(0) = max((esum-ed)*rHalf,rZero)
      p2(1) = -p1(1)
      p2(2) = -p1(2)
      p2(3) = -p1(3)
c
      return
      end
      subroutine hvq_momntx(energy,mass,costh,phi , p)
c
c This subroutine sets up a four-momentum from the four inputs.
c
c input:
c       real    energy         : energy
c       real    mass           : mass
c       real    costh          : cos(theta)
c       real    phi            : azimuthal angle
c
c output:
c       real    p(0:3)         : four-momentum
c     
      implicit none
      double precision p(0:3),energy,mass,costh,phi,pp,sinth

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )

c

      p(0) = energy

      if ( energy.eq.mass ) then

         p(1) = rZero
         p(2) = rZero
         p(3) = rZero

      else

         pp = sqrt((energy-mass)*(energy+mass))
         sinth = sqrt((rOne-costh)*(rOne+costh))
         p(3) = pp*costh
         if ( phi.eq.rZero ) then
            p(1) = pp*sinth
            p(2) = rZero
         else
            p(1) = pp*sinth*cos(phi)
            p(2) = pp*sinth*sin(phi)
         endif

      endif
c
      return
      end
      subroutine hvq_oxxxxx(p,fmass,nhel,nsf , fo)
c
c This subroutine computes a fermion wavefunction with the flowing-OUT
c fermion number.
c
c input:
c       real    p(0:3)         : four-momentum of fermion
c       real    fmass          : mass          of fermion
c       integer nhel = -1 or 1 : helicity      of fermion
c       integer nsf  = -1 or 1 : +1 for particle, -1 for anti-particle
c
c output:
c       complex fo(6)          : fermion wavefunction               <fo|
c     
      implicit none
      double complex fo(6),chi(2)
      double precision p(0:3),sf(2),sfomeg(2),omega(2),fmass,
     &     pp,pp3,sqp0p3,sqm
      integer nhel,nsf,nh,ip,im

      double precision rZero, rHalf, rTwo
      parameter( rZero = 0.0d0, rHalf = 0.5d0, rTwo = 2.0d0 )

c

      fo(5) = dcmplx(p(0),p(3))*nsf
      fo(6) = dcmplx(p(1),p(2))*nsf

      nh = nhel*nsf

      if ( fmass.ne.rZero ) then

         pp = min(p(0),dsqrt(p(1)**2+p(2)**2+p(3)**2))

         if ( pp.eq.rZero ) then
            
            sqm = dsqrt(fmass)
            ip = -((1+nh)/2)
            im =  (1-nh)/2
            
            fo(1) = im     * sqm
            fo(2) = ip*nsf * sqm
            fo(3) = im*nsf * sqm
            fo(4) = ip     * sqm
            
         else
            
            pp = min(p(0),dsqrt(p(1)**2+p(2)**2+p(3)**2))
            sf(1) = dble(1+nsf+(1-nsf)*nh)*rHalf
            sf(2) = dble(1+nsf-(1-nsf)*nh)*rHalf
            omega(1) = dsqrt(p(0)+pp)
            omega(2) = fmass/omega(1)
            ip = (3+nh)/2
            im = (3-nh)/2
            sfomeg(1) = sf(1)*omega(ip)
            sfomeg(2) = sf(2)*omega(im)
            pp3 = max(pp+p(3),rZero)
            chi(1) = dcmplx( dsqrt(pp3*rHalf/pp) )
            if ( pp3.eq.rZero ) then
               chi(2) = dcmplx(-nh )
            else
               chi(2) = dcmplx( nh*p(1) , -p(2) )/dsqrt(rTwo*pp*pp3)
            endif
            
            fo(1) = sfomeg(2)*chi(im)
            fo(2) = sfomeg(2)*chi(ip)
            fo(3) = sfomeg(1)*chi(im)
            fo(4) = sfomeg(1)*chi(ip)

         endif
         
      else
         
         sqp0p3 = dsqrt(max(p(0)+p(3),rZero))*nsf
         chi(1) = dcmplx( sqp0p3 )
         if ( sqp0p3.eq.rZero ) then
            chi(2) = dcmplx(-nhel )*dsqrt(rTwo*p(0))
         else
            chi(2) = dcmplx( nh*p(1), -p(2) )/sqp0p3
         endif
         if ( nh.eq.1 ) then
            fo(1) = chi(1)
            fo(2) = chi(2)
            fo(3) = dcmplx( rZero )
            fo(4) = dcmplx( rZero )
         else
            fo(1) = dcmplx( rZero )
            fo(2) = dcmplx( rZero )
            fo(3) = chi(2)
            fo(4) = chi(1)
         endif
         
      endif
c
      return
      end
      subroutine hvq_rotxxx(p,q , prot)
c
c This subroutine performs the spacial rotation of a four-momentum.
c the momentum p is assumed to be given in the frame where the spacial
c component of q points the positive z-axis.  prot is the momentum p
c rotated to the frame where q is given.
c
c input:
c       real    p(0:3)         : four-momentum p in q(1)=q(2)=0 frame
c       real    q(0:3)         : four-momentum q in the rotated frame
c
c output:
c       real    prot(0:3)      : four-momentum p in the rotated frame
c     
      implicit none
      double precision p(0:3),q(0:3),prot(0:3),qt2,qt,psgn,qq,p1

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )

c
      prot(0) = p(0)

      qt2 = q(1)**2 + q(2)**2


      if ( qt2.eq.rZero ) then
          if ( q(3).eq.rZero ) then
             prot(1) = p(1)
             prot(2) = p(2)
             prot(3) = p(3)
          else
             psgn = dsign(rOne,q(3))
             prot(1) = p(1)*psgn
             prot(2) = p(2)*psgn
             prot(3) = p(3)*psgn
          endif
      else
          qq = sqrt(qt2+q(3)**2)
          qt = sqrt(qt2)
          p1 = p(1)
          prot(1) = q(1)*q(3)/qq/qt*p1 -q(2)/qt*p(2) +q(1)/qq*p(3)
          prot(2) = q(2)*q(3)/qq/qt*p1 +q(1)/qt*p(2) +q(2)/qq*p(3)
          prot(3) =          -qt/qq*p1               +q(3)/qq*p(3)
      endif
c
      return
      end
      subroutine hvq_ssssxx(s1,s2,s3,s4,gc , vertex)
c
c This subroutine computes an amplitude of the four-scalar coupling.
c
c input:
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex s3(3)          : third  scalar                        s3
c       complex s4(3)          : fourth scalar                        s4
c       complex gc             : coupling constant                 ghhhh
c
c output:
c       complex vertex         : amplitude            gamma(s1,s2,s3,s4)
c     
      implicit none
      double complex s1(3),s2(3),s3(3),s4(3),gc,vertex

c

      vertex = gc*s1(1)*s2(1)*s3(1)*s4(1)
c
      return
      end
      subroutine hvq_sssxxx(s1,s2,s3,gc , vertex)
c
c This subroutine computes an amplitude of the three-scalar coupling.
c
c input:
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex s3(3)          : third  scalar                        s3
c       complex gc             : coupling constant                  ghhh
c
c output:
c       complex vertex         : amplitude               gamma(s1,s2,s3)
c     
      implicit none
      double complex s1(3),s2(3),s3(3),gc,vertex

c

      vertex = gc*s1(1)*s2(1)*s3(1)
c
      return
      end
      subroutine hvq_sxxxxx(p,nss , sc)
c
c This subroutine computes a complex SCALAR wavefunction.
c
c input:
c       real    p(0:3)         : four-momentum of scalar boson
c       integer nss  = -1 or 1 : +1 for final, -1 for initial
c
c output:
c       complex sc(3)          : scalar wavefunction                   s
c     
      implicit none
      double complex sc(3)
      double precision p(0:3)
      integer nss

      double precision rOne
      parameter( rOne = 1.0d0 )

c

      sc(1) = dcmplx( rOne )
      sc(2) = dcmplx(p(0),p(3))*nss
      sc(3) = dcmplx(p(1),p(2))*nss
c
      return
      end
      subroutine hvq_txxxx2(p,tmass,nhel,nst , tc)
c
c This subroutine computes k^mu*e^nu where e is delta(i,nhel).
c It is used to test for gauge invariance of the tensor routines.
c
c input:
c       real    p(0:3)             : four-momentum of tensor boson
c       real    tmass              : mass          of tensor boson
c       integer nhel = 1..4        : construction of e^nu
c       integer nst  = -1 or 1     : +1 for final, -1 for initial
c
c output:
c       complex tc(6,4)            : tensor wavefunction       epsilon^mu^nu(t)
c     
      implicit none
      double complex tc(6,4), ep(4), em(4)
      double precision p(0:3), tmass
      integer nhel, nst, i, j

      double precision rZero, rOne
      parameter( rZero = 0.0d0, rOne = 1.0d0 )
c
      tc(5,1) = dcmplx(p(0),p(3))*nst
      tc(6,1) = dcmplx(p(1),p(2))*nst

      ep(1) = dcmplx( p(0), rZero )
      ep(2) = dcmplx( p(1), rZero )
      ep(3) = dcmplx( p(2), rZero )
      ep(4) = dcmplx( p(3), rZero )

      if ( nhel.eq.1 ) then
         em(1) = dcmplx( rOne , rZero )
         em(2) = dcmplx( rZero, rZero )
         em(3) = dcmplx( rZero, rZero )
         em(4) = dcmplx( rZero, rZero )
      else if ( nhel.eq.2 ) then
         em(1) = dcmplx( rZero, rZero )
         em(2) = dcmplx( rOne , rZero )
         em(3) = dcmplx( rZero, rZero )
         em(4) = dcmplx( rZero, rZero )
      else if ( nhel.eq.3 ) then
         em(1) = dcmplx( rZero, rZero )
         em(2) = dcmplx( rZero, rZero )
         em(3) = dcmplx( rOne , rZero )
         em(4) = dcmplx( rZero, rZero )
      else if ( nhel.eq.4 ) then
         em(1) = dcmplx( rZero, rZero )
         em(2) = dcmplx( rZero, rZero )
         em(3) = dcmplx( rZero, rZero )
         em(4) = dcmplx( rOne , rZero )
      end if

      do j = 1,4
         do i = 1,4
            tc(i,j) = ep(i)*em(j)
         end do
      end do
c
      return
      end
      subroutine hvq_txxxxx(p,tmass,nhel,nst , tc)
c
c This subroutine computes a TENSOR wavefunction.
c
c input:
c       real    p(0:3)             : four-momentum of tensor boson
c       real    tmass              : mass          of tensor boson
c       integer nhel = -2,-1,0,1,2 : helicity      of tensor boson
c                                    (0 is forbidden if tmass = 0)
c       integer nst  = -1 or 1     : +1 for final, -1 for initial
c
c output:
c       complex tc(6,4)            : tensor wavefunction   epsilon^mu^nu(t)
c     
      implicit none
      double complex tc(6,4), ep(4), em(4), e0(4)
      double precision p(0:3), tmass, pt, pt2, pp, pzpt, emp, sqh, sqs
      integer nhel, nst, i, j

      double precision rZero, rHalf, rOne, rTwo
      parameter( rZero = 0.0d0, rHalf = 0.5d0 )
      parameter( rOne = 1.0d0, rTwo = 2.0d0 )
c
      sqh = sqrt(rHalf)
      sqs = sqrt(rHalf/3.d0)

      pt2 = p(1)**2 + p(2)**2
      pp = min(p(0),sqrt(pt2+p(3)**2))
      pt = min(pp,sqrt(pt2))

      tc(5,1) = dcmplx(p(0),p(3))*nst
      tc(6,1) = dcmplx(p(1),p(2))*nst

      if ( nhel.ge.0 ) then  !construct eps+
         if ( pp.eq.rZero ) then
            ep(1) = dcmplx( rZero )
            ep(2) = dcmplx( -sqh )
            ep(3) = dcmplx( rZero , nst*sqh )
            ep(4) = dcmplx( rZero )
         else
            ep(1) = dcmplx( rZero )
            ep(4) = dcmplx( pt/pp*sqh )
            if ( pt.ne.rZero ) then
               pzpt = p(3)/(pp*pt)*sqh
               ep(2) = dcmplx( -p(1)*pzpt , -nst*p(2)/pt*sqh )
               ep(3) = dcmplx( -p(2)*pzpt ,  nst*p(1)/pt*sqh )
            else
               ep(2) = dcmplx( -sqh )
               ep(3) = dcmplx( rZero , nst*sign(sqh,p(3)) )
            endif
         endif
      end if

      if ( nhel.le.0 ) then  !construct eps-
         if ( pp.eq.rZero ) then
            em(1) = dcmplx( rZero )
            em(2) = dcmplx( sqh )
            em(3) = dcmplx( rZero , nst*sqh )
            em(4) = dcmplx( rZero )
         else
            em(1) = dcmplx( rZero )
            em(4) = dcmplx( -pt/pp*sqh )
            if ( pt.ne.rZero ) then
               pzpt = -p(3)/(pp*pt)*sqh
               em(2) = dcmplx( -p(1)*pzpt , -nst*p(2)/pt*sqh )
               em(3) = dcmplx( -p(2)*pzpt ,  nst*p(1)/pt*sqh )
            else
               em(2) = dcmplx( sqh )
               em(3) = dcmplx( rZero , nst*sign(sqh,p(3)) )
            endif
         endif
      end if

      if ( abs(nhel).le.1 ) then  !construct eps0
         if ( pp.eq.rZero ) then
            e0(1) = dcmplx( rZero )
            e0(2) = dcmplx( rZero )
            e0(3) = dcmplx( rZero )
            e0(4) = dcmplx( rOne )
         else
            emp = p(0)/(tmass*pp)
            e0(1) = dcmplx( pp/tmass )
            e0(4) = dcmplx( p(3)*emp )
            if ( pt.ne.rZero ) then
               e0(2) = dcmplx( p(1)*emp )
               e0(3) = dcmplx( p(2)*emp )
            else
               e0(2) = dcmplx( rZero )
               e0(3) = dcmplx( rZero )
            endif
         end if
      end if

      if ( nhel.eq.2 ) then
         do j = 1,4
            do i = 1,4
               tc(i,j) = ep(i)*ep(j)
            end do
         end do
      else if ( nhel.eq.1 ) then
         do j = 1,4
            do i = 1,4
               tc(i,j) = sqh*( ep(i)*e0(j) + e0(i)*ep(j) )
            end do
         end do
      else if ( nhel.eq.0 ) then
         do j = 1,4
            do i = 1,4
               tc(i,j) = sqs*( ep(i)*em(j) + em(i)*ep(j)
     &                                + rTwo*e0(i)*e0(j) )
            end do
         end do
      else if ( nhel.eq.-1 ) then
         do j = 1,4
            do i = 1,4
               tc(i,j) = sqh*( em(i)*e0(j) + e0(i)*em(j) )
            end do
         end do
      else if ( nhel.eq.-2 ) then
         do j = 1,4
            do i = 1,4
               tc(i,j) = em(i)*em(j)
            end do
         end do
      else
         write(6,*) 'invalid helicity in hvq_txxxxx'
         stop
      end if
c
      return
      end
      subroutine hvq_vssxxx(vc,s1,s2,gc , vertex)
c
c This subroutine computes an amplitude from the vector-scalar-scalar
c coupling.  The coupling is absent in the minimal SM in unitary gauge.
c
c       complex vc(6)          : input  vector                        v
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex gc             : coupling constant (s1 charge)
c
c examples of the coupling constant gc for SUSY particles are as follows:
c   -----------------------------------------------------------
c   |    s1    | (q,i3) of s1  ||   v=a   |   v=z   |   v=w   |
c   -----------------------------------------------------------
c   | nu~_l    | (  0  , +1/2) ||   ---   |  gzn(1) |  gwf(1) |
c   | e~_l     | ( -1  , -1/2) ||  gal(1) |  gzl(1) |  gwf(1) |
c   | u~_l     | (+2/3 , +1/2) ||  gau(1) |  gzu(1) |  gwf(1) |
c   | d~_l     | (-1/3 , -1/2) ||  gad(1) |  gzd(1) |  gwf(1) |
c   -----------------------------------------------------------
c   | e~_r-bar | ( +1  ,  0  ) || -gal(2) | -gzl(2) | -gwf(2) |
c   | u~_r-bar | (-2/3 ,  0  ) || -gau(2) | -gzu(2) | -gwf(2) |
c   | d~_r-bar | (+1/3 ,  0  ) || -gad(2) | -gzd(2) | -gwf(2) |
c   -----------------------------------------------------------
c where the s1 charge is defined by the flowing-OUT quantum number.
c
c output:
c       complex vertex         : amplitude                gamma(v,s1,s2)
c     
      implicit none
      double complex vc(6),s1(3),s2(3),gc,vertex
      double precision p(0:3)

c

      p(0) = dble( s1(2)-s2(2))
      p(1) = dble( s1(3)-s2(3))
      p(2) = dimag(s1(3)-s2(3))
      p(3) = dimag(s1(2)-s2(2))

      vertex = gc*s1(1)*s2(1)
     &        *(vc(1)*p(0)-vc(2)*p(1)-vc(3)*p(2)-vc(4)*p(3))
c
      return
      end
      subroutine hvq_vvssxx(v1,v2,s1,s2,gc , vertex)
c
c This subroutine computes an amplitude of the vector-vector-scalar-
c scalar coupling.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       complex s1(3)          : first  scalar                        s1
c       complex s2(3)          : second scalar                        s2
c       complex gc             : coupling constant                 gvvhh
c
c output:
c       complex vertex         : amplitude            gamma(v1,v2,s1,s2)
c     
      implicit none
      double complex v1(6),v2(6),s1(3),s2(3),gc,vertex

c

      vertex = gc*s1(1)*s2(1)
     &        *(v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4))
c
      return
      end
      subroutine hvq_vvsxxx(v1,v2,sc,gc , vertex)
c
c This subroutine computes an amplitude of the vector-vector-scalar
c coupling.
c
c input:
c       complex v1(6)          : first  vector                        v1
c       complex v2(6)          : second vector                        v2
c       complex sc(3)          : input  scalar                        s
c       complex gc             : coupling constant                  gvvh
c
c output:
c       complex vertex         : amplitude                gamma(v1,v2,s)
c     
      implicit none
      double complex v1(6),v2(6),sc(3),gc,vertex

c

      vertex = gc*sc(1)
     &        *(v1(1)*v2(1)-v1(2)*v2(2)-v1(3)*v2(3)-v1(4)*v2(4))
c
      return
      end
      subroutine hvq_vvtxkk(wm,wp,tc,g,vmass , vertex)
c
c This subroutine computes an amplitude of the three-point coupling of
c two gauge bosons and a Kaluza-Klein tensor boson.
c
c input:
c       complex wm(6)          : vector               flow-in  V
c       complex wp(6)          : vector               flow-out V~
c       complex tc(6,4)        : tensor               KK mode T
c       real    g              : coupling constant    -kappa/2
c       real    vmass          : V boson mass          m_V
c
c output:
c       complex vertex         : amplitude            gamma(wm,wp,tc)
c     
      implicit none
      double complex wm(6), wp(6), tc(6,4), vertex
      double precision g, vmass

      double complex T12, T13, T14, T23, T24, T34
      double complex V1V2, k1V2, k2V1
      double complex Tkk, TVV, Tk1V2, Tk2V1, dum
      double precision pwm(4), pwp(4), F

      integer i, j

      double complex cZero
      double precision rZero, rTwo
      parameter( rZero = 0.0d0, rTwo = 2.0d0 )
      parameter( cZero = ( 0.0d0, 0.0d0 ) )
c
      pwm(1) = dreal(wm(5))
      pwm(2) = dreal(wm(6))
      pwm(3) = dimag(wm(6))
      pwm(4) = dimag(wm(5))
      pwp(1) = dreal(wp(5))
      pwp(2) = dreal(wp(6))
      pwp(3) = dimag(wp(6))
      pwp(4) = dimag(wp(5))

      T12 = tc(1,2) + tc(2,1)
      T13 = tc(1,3) + tc(3,1)
      T14 = tc(1,4) + tc(4,1)
      T23 = tc(2,3) + tc(3,2)
      T24 = tc(2,4) + tc(4,2)
      T34 = tc(3,4) + tc(4,3)

      V1V2 =  wm(1)*wp(1) -  wm(2)*wp(2) -  wm(3)*wp(3) -  wm(4)*wp(4)
      k1V2 = pwm(1)*wp(1) - pwm(2)*wp(2) - pwm(3)*wp(3) - pwm(4)*wp(4)
      k2V1 = pwp(1)*wm(1) - pwp(2)*wm(2) - pwp(3)*wm(3) - pwp(4)*wm(4)

      F = pwm(1)*pwp(1) - pwm(2)*pwp(2) - pwm(3)*pwp(3) - pwm(4)*pwp(4)
      if ( vmass.ne.rZero ) then
         F = F + vmass**2
      end if

      Tkk   = cZero
      TVV   = cZero
      Tk1V2 = cZero
      Tk2V1 = cZero

      do i = 1,4
         dum   = tc(i,i)*pwm(i)
         Tkk   = Tkk   + dum*pwp(i)
         Tk1V2 = Tk1V2 + dum*wp(i)
         dum   = tc(i,i)*wm(i)
         TVV   = TVV   + dum*wp(i)
         Tk2V1 = Tk2V1 + dum*pwp(i)
      end do

      Tkk   = rTwo*Tkk
      TVV   = rTwo*TVV
      Tk1V2 = rTwo*Tk1V2
      Tk2V1 = rTwo*Tk2V1

      Tkk = Tkk - T12*(pwm(1)*pwp(2) + pwm(2)*pwp(1))
     &          - T13*(pwm(1)*pwp(3) + pwm(3)*pwp(1))
     &          - T14*(pwm(1)*pwp(4) + pwm(4)*pwp(1))
     &          + T23*(pwm(2)*pwp(3) + pwm(3)*pwp(2))
     &          + T24*(pwm(2)*pwp(4) + pwm(4)*pwp(2))
     &          + T34*(pwm(3)*pwp(4) + pwm(4)*pwp(3))

      Tk1V2 = Tk1V2 - T12*(pwm(1)*wp(2) + pwm(2)*wp(1))
     &              - T13*(pwm(1)*wp(3) + pwm(3)*wp(1))
     &              - T14*(pwm(1)*wp(4) + pwm(4)*wp(1))
     &              + T23*(pwm(2)*wp(3) + pwm(3)*wp(2))
     &              + T24*(pwm(2)*wp(4) + pwm(4)*wp(2))
     &              + T34*(pwm(3)*wp(4) + pwm(4)*wp(3))

      TVV = TVV - T12*(wm(1)*wp(2) + wm(2)*wp(1))
     &          - T13*(wm(1)*wp(3) + wm(3)*wp(1))
     &          - T14*(wm(1)*wp(4) + wm(4)*wp(1))
     &          + T23*(wm(2)*wp(3) + wm(3)*wp(2))
     &          + T24*(wm(2)*wp(4) + wm(4)*wp(2))
     &          + T34*(wm(3)*wp(4) + wm(4)*wp(3))

      Tk2V1 = Tk2V1 - T12*(wm(1)*pwp(2) + wm(2)*pwp(1))
     &              - T13*(wm(1)*pwp(3) + wm(3)*pwp(1))
     &              - T14*(wm(1)*pwp(4) + wm(4)*pwp(1))
     &              + T23*(wm(2)*pwp(3) + wm(3)*pwp(2))
     &              + T24*(wm(2)*pwp(4) + wm(4)*pwp(2))
     &              + T34*(wm(3)*pwp(4) + wm(4)*pwp(3))

      vertex =  (tc(1,1)-tc(2,2)-tc(3,3)-tc(4,4))*( k1V2*k2V1 - V1V2*F )
     &        + F*TVV + V1V2*Tkk - k2V1*Tk1V2 - k1V2*Tk2V1

C      vertex = F*TVV + V1V2*Tkk - k2V1*Tk1V2 - k1V2*Tk2V1

      vertex = vertex * g
c
      return
      end
      subroutine hvq_vvvxxx(wm,wp,w3,g , vertex)
c
c This subroutine computes an amplitude of the three-point coupling of
c the gauge bosons.
c
c input:
c       complex wm(6)          : vector               flow-out W-
c       complex wp(6)          : vector               flow-out W+
c       complex w3(6)          : vector               j3 or A    or Z
c       real    g              : coupling constant    gw or gwwa or gwwz
c
c output:
c       complex vertex         : amplitude               gamma(wm,wp,w3)
c     
      implicit none
      double complex wm(6),wp(6),w3(6),vertex,
     &     xv1,xv2,xv3,v12,v23,v31,p12,p13,p21,p23,p31,p32
      double precision pwm(0:3),pwp(0:3),pw3(0:3),g

      double precision rZero, rTenth
      parameter( rZero = 0.0d0, rTenth = 0.1d0 )

c
      pwm(0) = dble( wm(5))
      pwm(1) = dble( wm(6))
      pwm(2) = dimag(wm(6))
      pwm(3) = dimag(wm(5))
      pwp(0) = dble( wp(5))
      pwp(1) = dble( wp(6))
      pwp(2) = dimag(wp(6))
      pwp(3) = dimag(wp(5))
      pw3(0) = dble( w3(5))
      pw3(1) = dble( w3(6))
      pw3(2) = dimag(w3(6))
      pw3(3) = dimag(w3(5))


      v12 = wm(1)*wp(1)-wm(2)*wp(2)-wm(3)*wp(3)-wm(4)*wp(4)
      v23 = wp(1)*w3(1)-wp(2)*w3(2)-wp(3)*w3(3)-wp(4)*w3(4)
      v31 = w3(1)*wm(1)-w3(2)*wm(2)-w3(3)*wm(3)-w3(4)*wm(4)
      xv1 = rZero
      xv2 = rZero
      xv3 = rZero

      if ( abs(wm(1)).ne.rZero ) then
         if ( abs(wm(1)).ge.max(abs(wm(2)),abs(wm(3)),abs(wm(4)))
     &        *rTenth )
     &      xv1 = pwm(0)/wm(1)
      endif
      if ( abs(wp(1)).ne.rZero) then
         if ( abs(wp(1)).ge.max(abs(wp(2)),abs(wp(3)),abs(wp(4)))
     &        *rTenth )
     &      xv2 = pwp(0)/wp(1)
      endif
      if ( abs(w3(1)).ne.rZero) then
         if ( abs(w3(1)).ge.max(abs(w3(2)),abs(w3(3)),abs(w3(4)))
     &        *rTenth )
     &      xv3 = pw3(0)/w3(1)
      endif

      p12 = (pwm(0)-xv1*wm(1))*wp(1)-(pwm(1)-xv1*wm(2))*wp(2)
     &     -(pwm(2)-xv1*wm(3))*wp(3)-(pwm(3)-xv1*wm(4))*wp(4)
      p13 = (pwm(0)-xv1*wm(1))*w3(1)-(pwm(1)-xv1*wm(2))*w3(2)
     &     -(pwm(2)-xv1*wm(3))*w3(3)-(pwm(3)-xv1*wm(4))*w3(4)
      p21 = (pwp(0)-xv2*wp(1))*wm(1)-(pwp(1)-xv2*wp(2))*wm(2)
     &     -(pwp(2)-xv2*wp(3))*wm(3)-(pwp(3)-xv2*wp(4))*wm(4)
      p23 = (pwp(0)-xv2*wp(1))*w3(1)-(pwp(1)-xv2*wp(2))*w3(2)
     &     -(pwp(2)-xv2*wp(3))*w3(3)-(pwp(3)-xv2*wp(4))*w3(4)
      p31 = (pw3(0)-xv3*w3(1))*wm(1)-(pw3(1)-xv3*w3(2))*wm(2)
     &     -(pw3(2)-xv3*w3(3))*wm(3)-(pw3(3)-xv3*w3(4))*wm(4)
      p32 = (pw3(0)-xv3*w3(1))*wp(1)-(pw3(1)-xv3*w3(2))*wp(2)
     &     -(pw3(2)-xv3*w3(3))*wp(3)-(pw3(3)-xv3*w3(4))*wp(4)

      vertex = -(v12*(p13-p23)+v23*(p21-p31)+v31*(p32-p12))*g
c
      return
      end
      subroutine hvq_vxxxxx(p,vmass,nhel,nsv , vc)
c
c This subroutine computes a VECTOR wavefunction.
c
c input:
c       real    p(0:3)         : four-momentum of vector boson
c       real    vmass          : mass          of vector boson
c       integer nhel = -1, 0, 1: helicity      of vector boson
c                                (0 is forbidden if vmass=0.0)
c       integer nsv  = -1 or 1 : +1 for final, -1 for initial
c
c output:
c       complex vc(6)          : vector wavefunction       epsilon^mu(v)
c     
      implicit none
      double complex vc(6)
      double precision p(0:3),vmass,hel,hel0,pt,pt2,pp,pzpt,emp,sqh
      integer nhel,nsv,nsvahl

      double precision rZero, rHalf, rOne, rTwo
      parameter( rZero = 0.0d0, rHalf = 0.5d0 )
      parameter( rOne = 1.0d0, rTwo = 2.0d0 )
      
c

      sqh = dsqrt(rHalf)
      hel = dble(nhel)
      nsvahl = nsv*dabs(hel)
      pt2 = p(1)**2+p(2)**2
      pp = min(p(0),dsqrt(pt2+p(3)**2))
      pt = min(pp,dsqrt(pt2))

      vc(5) = dcmplx(p(0),p(3))*nsv
      vc(6) = dcmplx(p(1),p(2))*nsv


      if ( vmass.ne.rZero ) then

         hel0 = rOne-dabs(hel)

         if ( pp.eq.rZero ) then

            vc(1) = dcmplx( rZero )
            vc(2) = dcmplx(-hel*sqh )
            vc(3) = dcmplx( rZero , nsvahl*sqh )
            vc(4) = dcmplx( hel0 )

         else

            emp = p(0)/(vmass*pp)
            vc(1) = dcmplx( hel0*pp/vmass )
            vc(4) = dcmplx( hel0*p(3)*emp+hel*pt/pp*sqh )
            if ( pt.ne.rZero ) then
               pzpt = p(3)/(pp*pt)*sqh*hel
               vc(2) = dcmplx( hel0*p(1)*emp-p(1)*pzpt , 
     &                         -nsvahl*p(2)/pt*sqh       )
               vc(3) = dcmplx( hel0*p(2)*emp-p(2)*pzpt ,  
     &                          nsvahl*p(1)/pt*sqh       )
            else
               vc(2) = dcmplx( -hel*sqh )
               vc(3) = dcmplx( rZero , nsvahl*sign(sqh,p(3)) )
            endif

         endif

      else

         pp = p(0)
         pt = sqrt(p(1)**2+p(2)**2)
         vc(1) = dcmplx( rZero )
         vc(4) = dcmplx( hel*pt/pp*sqh )
         if ( pt.ne.rZero ) then
            pzpt = p(3)/(pp*pt)*sqh*hel
            vc(2) = dcmplx( -p(1)*pzpt , -nsv*p(2)/pt*sqh )
            vc(3) = dcmplx( -p(2)*pzpt ,  nsv*p(1)/pt*sqh )
         else
            vc(2) = dcmplx( -hel*sqh )
            vc(3) = dcmplx( rZero , nsv*sign(sqh,p(3)) )
         endif

      endif
c
      return
      end
      subroutine hvq_w3w3xx(wm,w31,wp,w32,g31,g32, vertex)
c
c This subroutine computes an amplitude of the four-point coupling of
c the W-, W+ and two W3/Z/A.
c If one sets wmass=0.0, then the gggg vertex is given
c (see sect 2.9.1 of the manual).
c
c input:
c       complex wm(0:3)        : flow-out W-                         wm
c       complex w31(0:3)       : first    W3/Z/A                     w31
c       complex wp(0:3)        : flow-out W+                         wp
c       complex w32(0:3)       : second   W3/Z/A                     w32
c       real    g31            : coupling of w31 with W-/W+
c       real    g32            : coupling of w32 with W-/W+
c                                                  (see the table below)
c       real    wmass          : mass  of W
c       real    wwidth         : width of W
c
c the possible sets of the inputs are as follows:
c   -------------------------------------------
c   |  wm  |  w31 |  wp  |  w32 |  g31 |  g32 |
c   -------------------------------------------
c   |  W-  |  W3  |  W+  |  W3  |  gw  |  gw  |
c   |  W-  |  W3  |  W+  |  Z   |  gw  | gwwz |
c   |  W-  |  W3  |  W+  |  A   |  gw  | gwwa |
c   |  W-  |  Z   |  W+  |  Z   | gwwz | gwwz |
c   |  W-  |  Z   |  W+  |  A   | gwwz | gwwa |
c   |  W-  |  A   |  W+  |  A   | gwwa | gwwa |
c   -------------------------------------------
c where all the bosons are defined by the flowing-OUT quantum number.
c
c output:
c       complex vertex         : amplitude          gamma(wm,w31,wp,w32)
c     
      implicit none
      double complex wm(6),w31(6),wp(6),w32(6),vertex
      double complex dv1(0:3),dv2(0:3),dv3(0:3),dv4(0:3),dvertx
      double complex v12,v13,v14,v23,v24,v34
      double precision pwm(0:3),pw31(0:3),pwp(0:3),pw32(0:3)
      double precision g31,g32

      double precision rZero, rOne, rTwo
      parameter( rZero = 0.0d0, rOne = 1.0d0, rTwo = 2.0d0 )


      pwm(0) = dble( wm(5))
      pwm(1) = dble( wm(6))
      pwm(2) = dimag(wm(6))
      pwm(3) = dimag(wm(5))
      pwp(0) = dble( wp(5))
      pwp(1) = dble( wp(6))
      pwp(2) = dimag(wp(6))
      pwp(3) = dimag(wp(5))
      pw31(0) = dble( w31(5))
      pw31(1) = dble( w31(6))
      pw31(2) = dimag(w31(6))
      pw31(3) = dimag(w31(5))
      pw32(0) = dble( w32(5))
      pw32(1) = dble( w32(6))
      pw32(2) = dimag(w32(6))
      pw32(3) = dimag(w32(5))


      dv1(0) = dcmplx(wm(1))
      dv1(1) = dcmplx(wm(2))
      dv1(2) = dcmplx(wm(3))
      dv1(3) = dcmplx(wm(4))
      dv2(0) = dcmplx(w31(1))
      dv2(1) = dcmplx(w31(2))
      dv2(2) = dcmplx(w31(3))
      dv2(3) = dcmplx(w31(4))
      dv3(0) = dcmplx(wp(1))
      dv3(1) = dcmplx(wp(2))
      dv3(2) = dcmplx(wp(3))
      dv3(3) = dcmplx(wp(4))
      dv4(0) = dcmplx(w32(1))
      dv4(1) = dcmplx(w32(2))
      dv4(2) = dcmplx(w32(3))
      dv4(3) = dcmplx(w32(4))

      v12 = dv1(0)*dv2(0)-dv1(1)*dv2(1)-dv1(2)*dv2(2)-dv1(3)*dv2(3)
      v13 = dv1(0)*dv3(0)-dv1(1)*dv3(1)-dv1(2)*dv3(2)-dv1(3)*dv3(3)
      v14 = dv1(0)*dv4(0)-dv1(1)*dv4(1)-dv1(2)*dv4(2)-dv1(3)*dv4(3)
      v23 = dv2(0)*dv3(0)-dv2(1)*dv3(1)-dv2(2)*dv3(2)-dv2(3)*dv3(3)
      v24 = dv2(0)*dv4(0)-dv2(1)*dv4(1)-dv2(2)*dv4(2)-dv2(3)*dv4(3)
      v34 = dv3(0)*dv4(0)-dv3(1)*dv4(1)-dv3(2)*dv4(2)-dv3(3)*dv4(3)

      dvertx = v12*v34 + v14*v23 - rTwo*v13*v24
      
      vertex = dcmplx( dvertx ) * (g31*g32)
c
      return
      end
      subroutine hvq_wwwwxx(wm1,wp1,wm2,wp2,gwwa,gwwz , vertex)
c
c This subroutine computes an amplitude of the four-point W-/W+ coupling.
c
c input:
c       complex wm1(0:3)       : first  flow-out W-                  wm1
c       complex wp1(0:3)       : first  flow-out W+                  wp1
c       complex wm2(0:3)       : second flow-out W-                  wm2
c       complex wp2(0:3)       : second flow-out W+                  wp2
c       real    gwwa           : coupling constant of W and A       gwwa
c       real    gwwz           : coupling constant of W and Z       gwwz
c       real    zmass          : mass  of Z
c       real    zwidth         : width of Z
c
c output:
c       complex vertex         : amplitude        gamma(wm1,wp1,wm2,wp2)
c     
      implicit none
      double complex wm1(6),wp1(6),wm2(6),wp2(6),vertex
      double complex dv1(0:3),dv2(0:3),dv3(0:3),dv4(0:3),dvertx
      double complex v12,v13,v14,v23,v24,v34
      double precision pwm1(0:3),pwp1(0:3),pwm2(0:3),pwp2(0:3)
      double precision gwwa,gwwz

      double precision rZero, rOne, rTwo
      parameter( rZero = 0.0d0, rOne = 1.0d0, rTwo = 2.0d0 )

c
      pwm1(0) = dble( wm1(5))
      pwm1(1) = dble( wm1(6))
      pwm1(2) = dimag(wm1(6))
      pwm1(3) = dimag(wm1(5))
      pwp1(0) = dble( wp1(5))
      pwp1(1) = dble( wp1(6))
      pwp1(2) = dimag(wp1(6))
      pwp1(3) = dimag(wp1(5))
      pwm2(0) = dble( wm2(5))
      pwm2(1) = dble( wm2(6))
      pwm2(2) = dimag(wm2(6))
      pwm2(3) = dimag(wm2(5))
      pwp2(0) = dble( wp2(5))
      pwp2(1) = dble( wp2(6))
      pwp2(2) = dimag(wp2(6))
      pwp2(3) = dimag(wp2(5))


      dv1(0) = dcmplx(wm1(1))
      dv1(1) = dcmplx(wm1(2))
      dv1(2) = dcmplx(wm1(3))
      dv1(3) = dcmplx(wm1(4))
      dv2(0) = dcmplx(wp1(1))
      dv2(1) = dcmplx(wp1(2))
      dv2(2) = dcmplx(wp1(3))
      dv2(3) = dcmplx(wp1(4))
      dv3(0) = dcmplx(wm2(1))
      dv3(1) = dcmplx(wm2(2))
      dv3(2) = dcmplx(wm2(3))
      dv3(3) = dcmplx(wm2(4))
      dv4(0) = dcmplx(wp2(1))
      dv4(1) = dcmplx(wp2(2))
      dv4(2) = dcmplx(wp2(3))
      dv4(3) = dcmplx(wp2(4))

      v12 = dv1(0)*dv2(0)-dv1(1)*dv2(1)-dv1(2)*dv2(2)-dv1(3)*dv2(3)
      v13 = dv1(0)*dv3(0)-dv1(1)*dv3(1)-dv1(2)*dv3(2)-dv1(3)*dv3(3)
      v14 = dv1(0)*dv4(0)-dv1(1)*dv4(1)-dv1(2)*dv4(2)-dv1(3)*dv4(3)
      v23 = dv2(0)*dv3(0)-dv2(1)*dv3(1)-dv2(2)*dv3(2)-dv2(3)*dv3(3)
      v24 = dv2(0)*dv4(0)-dv2(1)*dv4(1)-dv2(2)*dv4(2)-dv2(3)*dv4(3)
      v34 = dv3(0)*dv4(0)-dv3(1)*dv4(1)-dv3(2)*dv4(2)-dv3(3)*dv4(3)

      dvertx = (v12*v34 + v14*v23 - rTwo*v13*v24)*(gwwa**2+gwwz**2)

      vertex = -dcmplx( dvertx )
c
      return
      end
      function hvq_dotprod(p,q)
      implicit none
      real * 8 hvq_dotprod,p(4),q(4)
      hvq_dotprod=p(4)*q(4)-p(1)*q(1)-p(2)*q(2)-p(3)*q(3)
      end

      subroutine hvq_rn3vec(vec,r)
c Generates a 3d vector in unit sphere
      implicit none
      real * 8 vec(3),r,r02,norm
      integer j
c generate in unit cube -1<x,y,z<1
 1    call hvq_RM48(vec,3)
      do j=1,3
         vec(j)=1-2*vec(j)
      enddo
c generate in unit sphere by hit and miss
      r02=vec(1)**2+vec(2)**2+vec(3)**2
      if(r02.gt.1) goto 1
c normalize
      norm=r/sqrt(r02)
      do j=1,3
         vec(j)=vec(j)*norm
      enddo
      end


c NR06:  Normalization:
c NR06:   d sigma = hvq_ggborn
c NR06:          * f(x1) * f(x2) * gs^4/(16 pi)   beta dcosth1 dx1 dx2
       function hvq_ggborn(s,t,m2)
c
c      d sigma (born_gg) = g^4 hvq_ggborn d phi2
c
       implicit real * 8 (a-z)
       v = 8
       n = 3
       p12 = s/2
       p13 = -t/2
       p23 = p12 - p13
          hvq_ggborn = 1/(2*v*n)*(v/(p13*p23)-2*n**2/p12**2)*
     #           (p13**2+p23**2+2*m2*p12-(m2*p12)**2/(p13*p23))
       hvq_ggborn = hvq_ggborn/(2*s)
       return
       end

       function hvq_qqborn(s,t,m2)
c
c      d sigma (born_qq) = g^4 hvq_qqborn d phi2
c
       implicit real * 8 (a-z)
       v = 8
       n2 = 3**2
       p12 = s/2
       p13 = -t/2
       p23 = p12 - p13
       hvq_qqborn = v/(2*n2)*( (p13**2+p23**2)/p12**2+m2/p12)
       hvq_qqborn = hvq_qqborn/(2*s)
       return
       end


c boosts the m 4 vectors vin, output in vout, boost velocity beta
c vin and vout may be the same
      subroutine hvq_boostm(beta,m,vin,vout)
      implicit none
      integer m
      real * 8 beta(3),vin(4,m),vout(4,m)
      real * 8 betav,gamma
      real * 8 vdotb
      integer ipart,idim
      betav=sqrt(beta(1)**2 + beta(2)**2 + beta(3)**2)
      gamma=1/sqrt(1-betav**2)
      do ipart=1,m
         vdotb=vin(1,ipart)*beta(1)
     #         +vin(2,ipart)*beta(2)+vin(3,ipart)*beta(3)
         do idim=1,3
            vout(idim,ipart)=vin(idim,ipart)
     # +beta(idim)/betav*((gamma-1)*vdotb/betav
     #     +gamma*betav*vin(4,ipart))
         enddo
         vout(4,ipart)=gamma*(vin(4,ipart)+vdotb)
      enddo
      end

