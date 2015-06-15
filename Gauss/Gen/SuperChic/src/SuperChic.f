c Generated automatically with Gen/SuperChic/python/SuperChic.py.
c Do not manually modify. Based on superchicv1.47.f.
      subroutine superchic(entryp, exitp)

      implicit double precision(a-y)  
      implicit complex*16(z)
      double precision pt1(2),pt2(2),ptdif(2),ptx(2),q(4,20),g(4,4)
      double precision pboo(4),pcm(4),plb(4),grid(5,5,50,50,50,50)
      double precision  n1(4),n2(4),n1rot(4),n2rot(4),e0(4),e0rot(4)
     &,n3(4)
      double precision gamqq(10),gamqg(10),gamgq(10),gamgg(10),
     &gamp(10),gamm(10),rhop(10),rhom(10)
      double precision q5(4),q6(4),q7(4),q8(4),q9(4),q6c(4),q7c(4),
     &q14(4),q15(4),q14c(4),q15c(4),rec(20,20),y(3),x(3)
      double precision tvec(4),uvec(4),pgtot(4)
      complex*16 zq6(4),zq5(4),zq7(4),zq8(4),zq14(4),zq15(4)
      complex*16 echiplus(4),echiminus(4),echi0(4),echi1(4),cechi1(4)
      complex*16 epsi0(4),epsiplus(4),epsiminus(4),epsi1(4),cepsi1(4)
     &,epsi1c(4),cepsi1c(4),echi1p(4),cechi1p(4),epsi1p(4),cepsi1p(4)
      complex*16 echi2p2(4,4),echi2p1(4,4),echi20(4,4),echi2m1(4,4)
     &,echi2m2(4,4),echi2(4,4),cechi2(4,4),echi2p(4,4),cechi2p(4,4)
      complex*16 rho1psi(3,3),rho1chi(3,3),rho2chi(5,5)
      integer d,h,i,j,k,l,m,n,o,p,r,j1,j2,j3,j4,nt,ii,naf
      integer nhist,ntotal,eflag,chiflag,icut,cross,ncut
     &,id(20),id0,id1,id2,id0m,pdf,decay,num,af,ap,eflagp
     &,nev,ll,ntotal1,mm,iset,iord,idh1,idh2,e,chiv,idh3,idh4
      character xflag*10,gen*10,surv*10,corr*10,photo*10,prefix*50
     &,pflag*10,phdecay*10
      common/mom/q
      common/mompt/pt1,pt2,ptx
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx
      common/flags/pdf,eflag
      common/it1/af
      common/init/rg,incp,nt,ii,ll
      common/dat/grid
ccccc hepevt output
      integer nmxhep,kk
      parameter (nmxhep=4000)
      integer nevhep,nhep,isthep,idhep,jmohep,jdahep
      double precision phep,vhep
      common /hepevt/ nevhep,nhep,isthep(nmxhep),idhep(nmxhep),
     &jmohep(2,nmxhep),jdahep(2,nmxhep),phep(5,nmxhep),vhep(4,nmxhep)
ccccc rambo variables
      integer npart,nrun
      double precision pin(4),am(100),pout(4,100),wt,ein
      integer entryp,exitp,itr1,itr2
      logical outevt,outhst
      common/output/outevt,outhst
      common/sccom/corr,gen,pflag,phdecay,photo,prefix,surv,xflag,ap,
     &chiflag,chiv,cross,d,decay,e,eflagp,h,i,icut,id,id0,id0m,id1,id2,
     &idh1,idh2,idh3,idh4,iord,iset,j,j1,j2,j3,j4,k,kk,l,m,mm,n,naf,
     &ncut,nev,nhist,npart,nrun,ntotal,ntotal1,num,o,p,r,a21,a21n,a28,
     &a28n,a2g,a2gn,aa1,aa2,alfas,alphap,alphapb,alphas0,alphasg,
     &alphasg0,alphasp,am,amp,amp1,amp2,amp3,asmur,b,b0,b1,bare,bb,
     &beff,beff0,beff0m,beff1,beff2,beta,beta0,bm2,bp,bp2,bpsi,bpsi0,
     &brchi0had,brchi1had,brchi2had,brchihad,brchipsi,brchipsi0,
     &brchipsi1,brchipsi2,bretahad,brpsimu,cc1,cc2,ccut,cf,clpm,clpp,
     &conv,cost,cost6,cost8,costg,costpi,cphi6,cphi8,cphig,cpi,d1,d2,
     &delta,deltad,e0,e0rot,ebeam,ein,epm,epp,eppg,errbrchi,errbrchi0,
     &errbrchi1,errbrchi2,errbrpsi,fe,fm,fn,fnorm,fnt,fnx,fny,fpi,fpi1,
     &fpi8,fpip,fr2,g,gamgg,gamgq,gamm,gamp,gamqg,gamqq,ge,glu,gm,
     &lambdacap,mb,mc,mchi0,mchi1,mchi2,mepm,mepp,meppg,meta,mgam,mh,
     &mh1,mh2,mjpsi,mmax,mmes,mmes1,mmes2,mmin,mmu,mp,mpsi,mt,mu,mum,
     &mur,mxx,my,n1,n1rot,n2,n2rot,n3,nc,neff,neff0,neff1,neff2,nf,ng,
     &norm,norm0,norm0m,norm1,norm2,normpsip,num1,p1m,p1p,p2m,p2p,pboo,
     &pcm,pcmod,pepm,pepp,peppg,pgmod,pgtot,phi,phi1,phi2,phi6,phi8,
     &phig,pi,picostsq,pin,plb,pmod,pnorm,pout,ppi,pt1sq,pt1x,pt1y,
     &pt2sq,pt2x,pt2y,ptdif,ptmax,ptmin,ptw,ptxsq,ptxx,q14,q14c,q14q15,
     &q15,q15c,q5,q6,q6c,q6q7,q7,q7c,q8,q8q9,q9,qf,qs,qsq,qsqmin,ran0,
     &ran1,ran10,ran11,ran12,ran2,ran3,ran4,ran5,ran6,ran7,ran8,ran9,
     &rang1,rang2,ranhist,ranpsi,ranyp,rec,rhom,rhop,rm,rmx,root1sq,
     &root2sq,rpsip,rt2,rtso,senh,senh0,senh1,senh2,sh,sign,sint6,
     &sint8,sintg,sphi6,sphi8,sphig,sum,sum1,sum2,sum3,sum4,sumr,sumr1,
     &sumt,sv,sv0,sv0m,sv1,sv2,svj0,svj2,t,t1,t2,theta1,thetap,thetap1,
     &thetap8,tvec,u,uvec,v,var,vmp,vpm,w0,w0b,w0psip,w2,weight,
     &weight1,weight2,weightgam,weightm,weightnf,weightnfpi,weightp,
     &weighty,width,wl,wpsi,wt,wthist,ww1,x,x1,x11,x2,x22,xcap2,xgam,
     &xglu,xgmin,xnorm,xo,xoo,y,ycap2,ycut,ym,ymax,ymin,ynorm,yp,ypmax,
     &ypmin,cechi1,cechi1p,cechi2,cechi2p,cepsi1,cepsi1c,cepsi1p,echi0,
     &echi1,echi1p,echi2,echi20,echi2m1,echi2m2,echi2p,echi2p1,echi2p2,
     &echiminus,echiplus,epsi0,epsi1,epsi1c,epsi1p,epsiminus,epsiplus,
     &rho1chi,rho1psi,rho2chi,zammpp,zamp,zamp1,zamp2,zamppp,zapmmp,
     &zapppp,zcechi1pcepsi1,zcechi1pechi1,zechi1cechi1p,zechi1epsi1p,
     &zepsi1cepsi1p,zepsi1pcepsi1,zi,zq14,zq14cechi1p,zq14echi1,zq15,
     &zq15cechi1p,zq15echi1,zq5,zq5epsi1p,zq6,zq6cechi1p,zq6cepsi1,
     &zq6echi1,zq6epsi1p,zq7,zq7epsi1p,zq8,zq8cepsi1p,zq8epsi1,
     &zweight1
      if (entryp.eq.26) then
      goto 2026
      endif
      if (entryp.eq.24) then
      goto 2024
      endif
      if (entryp.eq.22) then
      goto 2022
      endif
      if (entryp.eq.20) then
      goto 2020
      endif
      if (entryp.eq.18) then
      goto 2018
      endif
      if (entryp.eq.16) then
      goto 2016
      endif
      if (entryp.eq.14) then
      goto 2014
      endif
      if (entryp.eq.12) then
      goto 2012
      endif
      if (entryp.eq.10) then
      goto 2010
      endif
      if (entryp.eq.8) then
      goto 2008
      endif
      if (entryp.eq.6) then
      goto 2006
      endif
      if (entryp.eq.4) then
      goto 2004
      endif
      if (entryp.eq.2) then
      goto 2002
      endif
      if (entryp.eq.1) then
      goto 2001
      endif
      if (entryp.eq.0) then
      goto 2000
      endif
      if (entryp.eq.3) then
      goto 2003
      endif
      if (entryp.eq.5) then
      goto 2005
      endif
      if (entryp.eq.7) then
      goto 2007
      endif
      if (entryp.eq.9) then
      goto 2009
      endif
      if (entryp.eq.11) then
      goto 2011
      endif
      if (entryp.eq.13) then
      goto 2013
      endif
      if (entryp.eq.17) then
      goto 2017
      endif
      if (entryp.eq.19) then
      goto 2019
      endif
      if (entryp.eq.21) then
      goto 2021
      endif
      if (entryp.eq.23) then
      goto 2023
      endif
      if (entryp.eq.25) then
      goto 2025
      endif
      if (entryp.eq.27) then
      goto 2027
      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c 
c     superchic monte carlo generator for central         c 
c     exclusive  production:                              c
c                                                         c                                                         
c     p(1) p(2)  --> p(3) +  x(5) + p(4)                  c
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c
c     xflag= 'chic' : chi_c production                    c
c                                                         c
c     xflag= 'chib' : chi_b production                    c
c                                                         c
c     xflag= 'etac' : eta_c production                    c
c                                                         c
c     xflag= 'etab' : eta_b production                    c
c                                                         c
c     chiflag=0   chi(0++)                                c
c                                                         c
c     chiflag=1   chi(1++)                                c 
c                                                         c
c     chiflag=2   chi(2++)                                c
c                                                         c
c     chi_b/chi_c decays:                                 c
c                                                         c
c     x(5) --> gamma(6) psi/ups(7)--> gamma(6)mu(8)mu(9)  c
c                                                         c
c     (decay=1)                                           c
c                                                         c
c     or                                                  c 
c                                                         c
c     x(5) --> h(14)h(15)  with                           c
c                                                         c
c     h(14),h(15) = fermions      (decay=2)               c
c                                                         c
c     or                                                  c 
c                                                         c 
c     h(14),h(15) = scalars       (decay=3)               c
c                                                         c
c     decay = 4 :                                         c
c                                                         c
c     chi_c --> h(14) + h(15) + h(16) + h(17)             c
c                                                         c
c     decayed according to phase space                    c
c                                                         c 
c     for decay = 2-4, decay product masses/pdg numbers   c
c     are to be assigned by the user on lines 233-244     c 
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c 
c     xflag= 'gam' : digamma cep :-                       c
c                                                         c
c     x(5) = gamma(12)gamma(13)                           c
c                                                         c
c     pdf=1  :    mrst99 partons                          c
c     pdf=2  :    mstw08lo partons                        c
c                                                         c 
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c 
c     xflag='pi' : pi0pi0 cep :  -                        c
c                                                         c
c     x(5) = pi(12)pi(13)                                 c
c                                                         c
c     pi(12) --> gamma(14)gamma(15)                       c
c     pi(13) --> gamma(16)gamma(17)                       c
c                                                         c
c     xflag='eta' : etaeta cep, via same decay as default c
c     (br(eta-->gammagamma) not included in output        c
c     cross section)                                      c 
c                                                         c
c     pdf=1  :    mrst99 partons                          c
c     pdf=2  :    mstw08lo partons                        c
c                                                         c
c     also generates other mesons:                        c
c                                                         c
c     pi+(12)pi-(13)    xflag='pipm'                      c
c     rho0(12)rho0(13)  xflag='rho'                       c
c     eta'(12)eta'(13)  xflag='etap'                      c
c     eta(12)eta'(13)   xflag='etaetap'                   c 
c     k+(12)k-(13)      xflag='kpkm'                      c
c     k0(12)k0(13)      xflag='ks'                        c
c                                                         c
c     for eta(')eta(') : parameter a2g (=a_2^g, defined   c
c     in arxiv:1302.2004) can be set for gg contribution. c
c     default value a2g=0                                 c
c                                                         c 
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c 
c     xflag= 'jpsi','ups','psip'  : exclusive x = j/psi,  c
c     upsilon, psi(2s) production                         c
c                                                         c
c     decays:                                             c
c                                                         c
c     phdecay='mu'  :                                     c
c                                                         c
c     x(5) --> mu^+(8) mu^-(9)                            c 
c                                                         c
c     note  : use with corr='on'                          c
c                                                         c
c     'psipi'  :                                          c
c                                                         c
c     psi[2s](5) --> j/psi(8) pi^+(9) pi^-(10)            c
c                                                         c
c                --> mu^+(11) mu^-(12) pi^+(9) pi^-(10)   c
c                                                         c
c     fit to survival factor : s^2 = 0.85 - |y_x|/0.3     c
c     taken from arxiv:1301.7084                          c 
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c
c     momenta for each event in array q(i,j), where j is  c
c     the particle label and i is the 4-momentum          c
c     component, with:                                    c
c                                                         c
c     1,2 = transverse components                         c
c     3   = beam component                                c 
c     4   = energy                                        c
c                                                         c
c     calculates total cross section integrated over a    c
c     given chic rapidity interval (cross = 1) with       c
c     a phenomenological fit for the rapidity dependence, c
c     or dsigma/dy(chic) at y=0 (cross = 2).              c
c                                                         c
c     pdg number for ith particle in arrary id(i)         c
c     event record info. stored in rec(20,20)             c
c                                                         c
c     cms energies:                                       c
c                                                         c
c     eflag=500        rts= 0.5 tev                       c
c     eflag=2          rts= 1.96 tev                      c   
c     eflag=7          rts= 7 tev                         c  
c     eflag=8          rts= 8 tev                         c   
c     eflag=10         rts= 10 tev                        c   
c     eflag=14         rts= 14 tev                        c   
c                                                         c
c     note: -changing rts from these values manually will c
c     result in incorrect normalisation due to survival   c
c     factor energy dependence                            c
c     -diphoton & eta_b cep not generated at rts=500 gev  c
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c
c     gen='exact':  generates exact outgoing proton       c
c     kinematics, with survival effects included for      c
c     chi_c/eta_c cep at rts=500 gev                      c
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                         c
c     this is version 1.47           : 6 march 2014       c 
c                                                         c
c     comments etc to: lucian.harland-lang@durham.ac.uk   c
c                                                         c 
c     for more details see, e.g.:                         c
c                                                         c
c     l.a. harland-lang, v.a. khoze, m.g. ryskin          c
c     and w.j. stirling arxiv:0909.4748  (chi_c/b)        c  
c                                                         c 
c     l.a. harland-lang, v.a. khoze, m.g. ryskin          c
c     and w.j. stirling arxiv:1005.1695  (gamgam)         c
c                                                         c
c     l.a. harland-lang, v.a. khoze, m.g. ryskin          c
c     and w.j. stirling arxiv:1011.0680                   c 
c                                                         c
c     l.a. harland-lang, v.a. khoze, m.g. ryskin          c
c     and w.j. stirling arxiv:1105.1626  (meson pair)     c 
c                                                         c
c     l.a. harland-lang, v.a. khoze, m.g. ryskin          c
c     and w.j. stirling arxiv:1302.2004 (meson pair: gg)  c
c                                                         c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

 2000 continue
      if (exitp.eq.0) then
      return
      endif
      if (outevt) then
      open(30,file='exerec.dat')! event record
      endif
      weightm=0d0

c     set flags

 2002 continue
      if (exitp.eq.2) then
      return
      endif
      xflag='jpsi'              ! 'etac','chic','etab','chib','gam','jpsi','ups','psip'
                                ! 'pi','pipm,'rho','eta','etap','etaetap','ks','kpkm'
      chiflag=0                 ! chi_(0,1,2)
      eflag=7                   ! 0.5,1.96,7,10,14 tev = 500,2,7,8,10,14 
c                               (only currently generates chi_c events for 500 gev)
c                               for meson pair cep, 0.3,0.9 tev = 300, 900 

ccc   chi_c/b, eta_c/b

c     2-body decays

      decay=2                 ! set decay for chi/eta states
      if(xflag.eq.'etac'.or.xflag.eq.'etab')then
         decay=2
      endif
      mh=0d0                  ! set two-body decay product mass
      idh1=90                 ! set pdg no for two-body decay products
      idh2=90

c     4-body decay (chi_c0 only) -- decay = 4

      mh1=0.13957018d0         ! masses of particles 1 & 2 in decay
      idh1=211                 ! pdg no's
      idh2=-211
      mh2=0.13957018d0         ! masses of particles 3 & 4 in decay
      idh3=211                 ! pdg no's
      idh4=-211
 2003 continue
      if (exitp.eq.3) then
      return
      endif

 2004 continue
      if (exitp.eq.4) then
      return
      endif
      if(decay.eq.4)then

      npart=4
      
      if(chiflag.eq.0)then
         ein=3.41475d0          ! chi_c mass
      elseif(chiflag.eq.1)then
         ein=3.51066d0
      else
         ein=3.56620d0
      endif

      do j=1,2
         am(j)=mh1
         am(j+2)=mh2
      enddo

      nrun=1000000             ! no. of runs to find total integrated 4-body p.s.
      sumr=0d0
      sumr1=0d0

      do i=1,nrun              ! integrated 4-body p.s weight (sets normalisation)
      call rambo(npart,ein,am,pout,wt)
      sumr=sumr+wt
      sumr1=sumr1+wt**2
      enddo

      sumr=sumr/dfloat(nrun)
      sumr1=sumr1/dfloat(nrun)
      var=dsqrt((sumr1-sumr**2)/dfloat(nrun))
c      print*,sumr,var

      endif
 2005 continue
      if (exitp.eq.5) then
      return
      endif

ccc

 2006 continue
      if (exitp.eq.6) then
      return
      endif
      gen='fit'                 ! 'exact'= exact kinematics (in particular, outgoing protons)
                                ! 'fit'= fit (approx. kinematics, but faster run-- as in v1.2)
                                ! grids only provided for rts= 500 gev, but others available
                                ! upon request
 
      surv='on'                 ! survival factors 'on' or 'off' for generating proton
                                ! distributions with gen='exact'

      corr='on'                 ! 'on'=generate decays with spin correlations on
                                ! 'off'=generate decays uniformly

      chiv=2                    ! additional fit to gg --> chi_({c,b}2) vertex
                                ! 1 = previous fit, gives best description of proton p_t distributions
                                ! 2 = new fit, gives best description of chi_c p_t distribution

cccccccccccc   j/psi (upsilon)   --- use with corr='on'

      photo='fit'               ! j/psi photoproduction: 'fit'= fit to hera data
                                ! 'theory'= lo theory expression (pdf dependent)
                                ! default sets: 
      pflag='mstw'              ! 'mstw'= mstw08 set, 'cteq'=cteq6 set
                                ! note: to use this feature, first uncomment 
                                ! lines 618-619, 635, 1490-1507 & 5814-5816
                                ! and add relevant pdf .f files to directory

      phdecay='mu'              ! particle decays-- 'mu' : x --> mu^+ mu^-
 2007 continue
      if (exitp.eq.7) then
      return
      endif
                                !                'psipi' : psi(2s) --> (j/psi) pi^+ pi^-
                                !                                  --> mu^+mu^- pi^+ pi^-  

 2008 continue
      if (exitp.eq.8) then
      return
      endif
      if(phdecay.eq.'psipi')then

         if(xflag.eq.'jpsi'.or.xflag.eq.'ups')then
         print*,"warning:"
         print*,"phdecay=psipi only possible for psi(2s) (pflag=psip)"
         stop
         endif

         am(1)=3.096916d0       ! j/psi mass
         am(2)=0.13957018d0     ! pion masses
         am(3)=0.13957018d0
         idh1=443               ! pdg numbers
         idh2=211
         idh3=-211

         npart=3
         ein=3.686108d0         ! psi(2s) mass
         
         nrun=1000000           ! no. of runs to find total integrated 4-body p.s.
         sumr=0d0
         sumr1=0d0
         
         do i=1,nrun            ! integrated 4-body p.s weight (sets normalisation)
            call rambo(npart,ein,am,pout,wt)
            sumr=sumr+wt
            sumr1=sumr1+wt**2
         enddo
         
         sumr=sumr/dfloat(nrun)
         sumr1=sumr1/dfloat(nrun)
         var=dsqrt((sumr1-sumr**2)/dfloat(nrun))
         
      endif
 2009 continue
      if (exitp.eq.9) then
      return
      endif
      
cccccccccccccccccccccccccccccccc

ccc   diphoton/dimeson
   
 2010 continue
      if (exitp.eq.10) then
      return
      endif
      pdf=2                     ! pdf set choice for diphoton/dimeson production
                 
c     total or differential cross section (see preamble)

      cross=1

c     eta(')eta('): size of gluonic component a_g^2(mu_0^2)- defined in arxiv:1302.2004

      a2g=0d0

ccc   sample cuts- to apply to a given final state, un-comment relevant
ccc   region in subroutine 'icut'
      
c     chi cuts: pt(muons) and y(chi), implemented in subroutine icut

      ptcut=0d0                 ! ptmin of final state muons 
      ycut=10d0                 ! |rapidity| max of chic
      etacut=0.6d6              ! |pseudorapidity| max of final state muons

c     diphoton/dimeson cuts:
     
      ecut=2.5d0                  ! min photon/meson e_perp      
 2011 continue
      if (exitp.eq.11) then
      return
      endif
      mmin=2d0*ecut
c      mmin=5d0
      rcut=2.5d1                ! |pseudorapidity| max of photons/mesons

c     beam energy etc

 2012 continue
      if (exitp.eq.12) then
      return
      endif
      if(eflag.eq.500)then
         rts=0.5d3              ! gev
      elseif(eflag.eq.2)then         
         rts=1.96d3             
      elseif(eflag.eq.7)then
         rts=7d3
      elseif(eflag.eq.8)then
         rts=8d3
         eflagp=8
         eflag=7
      elseif(eflag.eq.10)then
         rts=10d3
      elseif(eflag.eq.14)then
         rts=14d3
      elseif(eflag.eq.300)then
         rts=300d0
      elseif(eflag.eq.900)then
         rts=900d0
      endif
      
      ebeam=rts/2d0
      s=rts*rts
      zi=(0d0,1d0)
      rt2=dsqrt(2d0)
      pi=dacos(-1d0)
 2013 continue
      if (exitp.eq.13) then
      return
      endif

c     initialise counters etc

 2014 continue
      if (exitp.eq.14) then
      return
      endif
      num=0
      nev=1000                  ! no. of unweighted events generated to event record
 2016 continue
      if (exitp.eq.16) then
      return
      endif

      do itr1=1,2

      if (ll.ge.0) then
      ll = itr1
      if(ll.eq.1)then
 2018 continue
      if (exitp.eq.18) then
      return
      endif
         ntotal=1000000          ! no. of runs to find max weight
         ntotal1=ntotal
      else
c         ntotal=nev*10
         ntotal=1000000000
      endif
      else
      ll = -ll
      endif

      ii=ntotal+1
      nhist=1
      sum=0d0
      sum1=0d0
      sum2=0d0
      sum3=0d0
      sum4=0d0
      ncut=0

c     proton form factor (as in ~ exp(bt)), in gev^-2    

      b0=2d0                    ! elastic slope
      b=2d0*b0
      bp=rts/dsqrt(2.0d0)
      alphap=0.1d0              ! pomeron slope
      delta=0.2d0               ! pomeron intercept

c     initialise histograms

      do i=1,nhist
         call histo3(i)
      enddo
      
c     meson evolution

      cf=4d0/3d0
      nf=3d0
      nc=3d0
      beta0=11d0-2d0*nf/3d0

      do i=2,4,2

         ii=dble(i)

         num1=0d0
         do j=1,i+1
            num1=num1+1d0/dble(j)
         enddo

         gamqq(i)=cf*(3d0+2d0/(ii+1d0)/(ii+2d0)-4d0*num1)
         gamgg(i)=beta0+nc*(8d0/(ii+1d0)/(ii+2d0)-4d0*num1)
         gamgq(i)=nf*12d0/(ii+1d0)/(ii+2d0)
         gamqg(i)=cf*ii*(ii+3d0)/3d0/(ii+1d0)/(ii+2d0)

         gamp(i)=(gamqq(i)+gamgg(i)+dsqrt((gamqq(i)-gamgg(i))**2+
     &        4d0*gamqg(i)*gamgq(i)))/2d0
         gamm(i)=(gamqq(i)+gamgg(i)-dsqrt((gamqq(i)-gamgg(i))**2+
     &        4d0*gamqg(i)*gamgq(i)))/2d0

         rhop(i)=6d0*gamgq(i)/(gamp(i)-gamgg(i))
         rhom(i)=gamqg(i)/(gamm(i)-gamqq(i))/6d0

      enddo

c     set masses and other parameters

      if(xflag.eq.'pi')then
         mmes=0.1349766d0        ! pi0 mass, pdg 2011 value
         fpi=0.133d0             ! pion form factor, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         id(12)=111
         id(13)=111
         a28=2d0/3d0
      elseif(xflag.eq.'ks')then
         mmes=0.497614d0         ! k_0 mass, pdg 2011 value
         fpi=0.133d0             ! effective kaon form factor, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         id(12)=310
         id(13)=310
         a28=2d0/3d0
      elseif(xflag.eq.'pipm')then
         mmes=0.13957018d0       ! pi+/- mass, pdg 2011 value
         fpi=0.133d0             ! pion form factor, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         id(12)=211
         id(13)=-211
         a28=2d0/3d0
      elseif(xflag.eq.'kpkm')then
         mmes=0.493677d0         ! k+/- mass, pdg 2011 value
         fpi=0.133d0             ! effective kaon form factor, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         id(12)=321
         id(13)=-321
         a28=2d0/3d0
      elseif(xflag.eq.'rho')then
         mmes=0.77549d0          ! rho mass, pdg 2011 value
         fpi=0.2d0               ! rho form factor, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         id(12)=113
         id(13)=113
         a28=2d0/3d0
      elseif(xflag.eq.'eta')then
         mmes=0.547853d0         ! eta mass, pdg 2011 value
         fpi=0.133d0             ! eta form factor= pi f.f, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         thetap=-15d0*pi/180d0   ! eta-eta' mixing angle
         thetap1=-9.2d0*pi/180d0
         thetap8=-21.2d0*pi/180d0
         fpi8=1.26d0*fpi
         fpi1=1.17d0*fpi
         a21=2d0/3d0
         a28=2d0/3d0
         id(12)=221
         id(13)=221
      elseif(xflag.eq.'etap')then
         mmes=0.95778d0          ! eta' mass, pdg 2011 value
         fpi=0.133d0             ! eta' form factor= pi f.f, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         thetap=-15d0*pi/180d0   ! eta-eta' mixing angle
         thetap1=-9.2d0*pi/180d0
         thetap8=-21.2d0*pi/180d0
         fpi8=1.26d0*fpi
         fpi1=1.17d0*fpi
         a21=2d0/3d0
         a28=2d0/3d0
         id(12)=331
         id(13)=331
      elseif(xflag.eq.'etaetap')then
         mmes1=0.547853d0        ! eta mass, pdg 2011 value
         mmes2=0.95778d0         ! eta' mass, pdg 2011 value
         fpi=0.133d0             ! eta form factor= pi f.f, default value
         fpip=fpi/(2d0*dsqrt(6d0))
         thetap=-15d0*pi/180d0   ! eta-eta' mixing angle
         thetap1=-9.2d0*pi/180d0
         thetap8=-21.2d0*pi/180d0
         fpi8=1.26d0*fpi
         fpi1=1.17d0*fpi
         a21=2d0/3d0
         a28=2d0/3d0
         id(12)=221
         id(13)=331         
      endif

      if(xflag.eq.'jpsi'.or.xflag.eq.'ups'.or.xflag.eq.'psip')then

      if(xflag.eq.'jpsi')then
         
      id(5)=443
      mx=3.096916d0
      brpsimu=5.93d-2           ! br(j/psi --> mu+mu-)
      width=5.52d-6
      bpsi=4.5d0
      norm=3d0
      conv=389d3
      nf=4d0
      w0=1d0
      delta=0.72d0

      w0b=90d0
      alphapb=0.16d0
      bpsi0=4.6d0

      elseif(xflag.eq.'psip')then

      id(5)=100443
      mx=3.686108d0
      mjpsi=3.096916d0
      brpsimu=7.7d-3          ! br(psi(2s) --> mu+mu-)
      width=304d-6
      norm=3d0
      conv=389d3
      nf=4d0
      w0=1d0
      delta=0.72d0

      w0b=90d0
      alphapb=0.16d0
      bpsi0=4.6d0

c      deltad=0.24d0
      deltad=0d0

c      rpsip=0.15d0
      rpsip=0.166d0
      w0psip=90d0
      normpsip=rpsip*(w0psip/53.2d0)**deltad

      elseif(xflag.eq.'ups')then

      id(5)=553
      mx=9.4603d0
      brpsimu=2.48d-2           ! br(ups --> mu+mu-)
      width=1.34d-6
      bpsi=4.1d0
      norm=0.12d0
      conv=389379d3
      nf=5d0
      w0=1d0
      delta=1.63d0

      w0b=90d0          ! set to j/psi value by default
      alphapb=0.16d0
      bpsi0=4.6d0

      endif

      mp=0.9383d0
      mmu=0.1057d0

      if(photo.eq.'theory')then

cccc     pdfs

      if(pflag.eq.'cteq')then
         
c$$$         iset=3
c$$$         call setctq6(iset)  
    
      elseif(pflag.eq.'mstw')then
   
         iset=0 
         prefix = "mstw2008lo"
         iord=0
         fr2=1d0          
         mur=1d0               
         asmur=0.5d0          
         mc=1.4d0              
         mb=4.75d0              
         mt=1d10   

c     initialise alphas

c$$$      call initalphas(iord,fr2,mur,asmur,mc,mb,mt)

      endif

c     chi_c values

      endif

      elseif(xflag.eq.'chic'.or.xflag.eq.'etac')then

      rtso=60d0

c     j/psi

      id(7)=443                 ! pdg number
      mpsi=3.096916d0
      brpsimu=5.93d-2           ! br(j/psi --> mu+mu-)
      errbrpsi=0.06d-2

ccc     chi_c(0++)

      id0=10441
      mchi0=3.41475d0     
      brchipsi0=1.14d-2         ! br(chi_c --> j/psi)  
      errbrchi0=0.08d-2
      brchi0had=1d0             ! chi_c 2-body decay branching ratio

c     'bare' forward chi_c0 central sigma at 60 gev in nb

      bare=2.1d3

ccc     chi_c(1++)
      
      id1=20443
      mchi1=3.51066d0                    
      brchipsi1=34.1d-2
      errbrchi1=1.5d-2
      brchi1had=1d0 

ccc     chi_c(2++)

      id2=445
      mchi2=3.55620d0
      brchipsi2=19.4d-2
      errbrchi2=0.8d-2
      brchi2had=1d0 

ccc     eta_c(0-+)

      id0m=441
      meta=2.9837d0
      bretahad=1d0

c     rapidity dependence (~same for 3 chi/eta states)

      ng=30d0+8.1d-3*(rts-1.96d3)

         if(eflag.eq.2)then
            sv0=0.058d0        !<eikonal survival factor^2>, chi_c0
            sv1=0.15d0
            sv2=0.11d0
            sv0m=0.18d0
            senh0=0.49d0       !<enhanced survival factor^2>, chi_c0
            senh1=0.49d0
            senh2=0.49d0  
            beff0=6.2d0        ! effective slope*sqrt{r_scr}
            beff1=6.2d0
            beff2=7.6d0
            beff0m=7.3d0
            norm0=1.41d0         
            norm1=3.41d-2
            norm2=9.93d-2
            norm0m=391d0
         elseif(eflag.eq.7)then
            sv0=0.037d0
            sv1=0.11d0
            sv2=0.084d0
            sv0m=0.13d0
            senh0=0.32d0
            senh1=0.32d0
            senh2=0.32d0
            senh2=0.32d0  
            beff0=6.2d0        
            beff1=6.3d0
            beff2=7.6d0
            beff0m=7.3d0
            norm0=1.46d0         
            norm1=3.64d-2
            norm2=8.94d-2
            norm0m=298d0
         elseif(eflag.eq.10)then
            sv0=0.033d0
            sv1=0.10d0
            sv2=0.078d0
            sv0m=0.11d0
            senh0=0.28d0
            senh1=0.28d0
            senh2=0.28d0
            beff0=6.2d0        
            beff1=6.4d0
            beff2=7.6d0
            beff0m=7.3d0
            norm0=1.44d0         
            norm1=3.69d-2
            norm2=8.47d-2
            norm0m=287d0
         elseif(eflag.eq.14)then
            sv0=0.029d0
            sv1=0.091d0
            sv2=0.072d0
            sv0m=0.10d0
            senh0=0.25d0
            senh1=0.25d0
            senh2=0.25d0
            beff0=6.2d0        
            beff1=6.4d0
            beff2=7.6d0
            beff0m=7.3d0
            norm0=1.47d0         
            norm1=3.75d-2
            norm2=8.23d-2
            norm0m=270d0
         elseif(eflag.eq.500)then
            sv0=0.092d0
            sv1=0.23d0
            sv2=0.15d0
            sv0m=0.26d0
            senh0=0.71d0
            senh1=0.71d0
            senh2=0.71d0
            beff0=6.3d0        
            beff1=6.1d0
            beff2=7.6d0
            beff0m=7.3d0
            norm0=1.47d0         
            norm1=3.32d-2
            norm2=1.13d-1
            norm0m=682d0
         endif

c     chi_b values

      elseif(xflag.eq.'chib'.or.xflag.eq.'etab')then

      rtso=173d0  

c     upsilon
     
      id(7)=553
      mpsi=9.46d0
      brpsimu=2.48d-2
      errbrpsi=0.05d-2

ccc     chi_b(0++)

      id0=10551
      mchi0=9.85944d0                  
      brchipsi0=3d-2     !!! note branching ratio experimentally < 6% only 
      errbrchi0=1d-2 
      brchi0had=1d0       

c     'bare' central sigma at 173 gev in pb

      bare=2.16d3

ccc     chi_b(1++)
     
      id1=20553
      mchi1=9.89278d0                      
      brchipsi1=35d-2
      errbrchi1=8d-2
      brchi1had=1d0

ccc     chi_b(2++)

      id2=555
      mchi2=9.91221d0
      brchipsi2=22d-2
      errbrchi2=4d-2
      brchi2had=1d0

ccc     eta_b(0-+)

      id0m=551
      meta=9.398d0   
      bretahad=1d0

cccccccccccc

      ng=14d0+3.5d-3*(rts-1.96d3)

         if(eflag.eq.2)then
            sv0=0.057d0         
            sv1=0.15d0
            sv2=0.11d0
            sv0m=0.17d0
            senh0=0.65d0
            senh1=0.65d0
            senh2=0.65d0
            beff0=6.0d0        
            beff1=6.0d0
            beff2=7.5d0
            beff0m=7.0d0
            norm0=1.11d0         
            norm1=3.16d-3
            norm2=6.58d-2
            norm0m=0.44d0
         elseif(eflag.eq.7)then
            sv0=0.036d0
            sv1=0.11d0
            sv2=0.088d0
            sv0m=0.12d0
            senh0=0.43d0
            senh1=0.43d0
            senh2=0.43d0
            beff0=6.0d0        
            beff1=6.0d0
            beff2=7.5d0
            beff0m=7.0d0
            norm0=1.18d0         
            norm1=3.18d-3
            norm2=5.77d-2
            norm0m=0.32d0
         elseif(eflag.eq.10)then
            sv0=0.032d0
            sv1=0.094d0
            sv2=0.082d0
            sv0m=0.11d0
            senh0=0.37d0
            senh1=0.37d0
            senh2=0.37d0
            beff0=6.0d0        
            beff1=6.0d0
            beff2=7.5d0
            beff0m=6.9d0
            norm0=1.21d0         
            norm1=3.39d-3
            norm2=5.65d-2
            norm0m=0.28d0
         elseif(eflag.eq.14)then
            sv0=0.028d0
            sv1=0.086d0
            sv2=0.077d0
            sv0m=0.095d0
            senh0=0.32d0
            senh1=0.32d0
            senh2=0.32d0
            beff0=6.0d0        
            beff1=6.1d0
            beff2=7.5d0
            beff0m=6.9d0
            norm0=1.22d0         
            norm1=3.65d-3
            norm2=5.43d-2
            norm0m=0.25d0  
         elseif(eflag.eq.500)then
            print*,'chib/etab not generated at rts=500 gev'
            stop
         endif  

      endif

      if(ll.eq.1)then
         
         if(xflag.eq.'chic'.or.xflag.eq.'etac')then

            if(gen.eq.'exact')then ! read in hard amplitude from grid
               
               if(xflag.eq.'etac')then
                  open(20,file='grids/etac.dat')
                  naf=1
               elseif(xflag.eq.'chic')then
                  if(chiflag.eq.0)then
                     open(20,file='grids/chic0.dat')
                     naf=1
                  elseif(chiflag.eq.1)then
                     open(20,file='grids/chic1.dat')
                     naf=2
                  elseif(chiflag.eq.2)then
                     open(20,file='grids/chic2.dat')
                     naf=3
                  endif
               endif
               
               read(20,*)nt,rg,incp
               
               do j1=1,nt
                  do j2=1,nt
                     do j3=1,nt
                        do j4=1,nt
                           do k=1,naf
                     grid(k,1,j1,j2,j3,j4)=-rg/2d0+(dble(j1)-0.5d0)*incp      
                     grid(k,2,j1,j2,j3,j4)=-rg/2d0+(dble(j2)-0.5d0)*incp      
                     grid(k,3,j1,j2,j3,j4)=-rg/2d0+(dble(j3)-0.5d0)*incp     
                     grid(k,4,j1,j2,j3,j4)=-rg/2d0+(dble(j4)-0.5d0)*incp   
                           enddo          
                           
                           if(naf.eq.1)then
                              read(20,*)grid(1,5,j1,j2,j3,j4)
                           elseif(naf.eq.2)then
                  read(20,*)grid(1,5,j1,j2,j3,j4),grid(2,5,j1,j2,j3,j4)
                           elseif(naf.eq.3)then
                  read(20,*)grid(1,5,j1,j2,j3,j4),grid(2,5,j1,j2,j3,j4)
     &                             ,grid(3,5,j1,j2,j3,j4)
                           endif
                           
                        enddo
                     enddo
                  enddo
               enddo
               
               close(20)
               
            endif         
         endif         
      endif

        if(gen.eq.'exact')then
            if(xflag.eq.'etac')then
               norm0m=23d0
               beff0m=b
            elseif(xflag.eq.'chic')then
               norm0=1.45d-2
               norm1=1.98d-4
               norm2=3.42d-3
               beff0=b       
               beff1=b
               beff2=b
               beff0m=b
            endif
            
         endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                       c
c     start of event loop                               c
c                                                       c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
 2020 continue
      if (exitp.eq.20) then
      return
      endif

      do itr2=1,ntotal
      if (i.ge.0) then
      i = itr2
      else
      i = -i
      endif
 2022 continue
      if (exitp.eq.22) then
      return
      endif
            weight=0d0

      call r2455(ran0)
      call r2455(ran1)
      call r2455(ran2)
      call r2455(ran3)

      if(ran2.eq.0d0)then
         call r2455(ran2)
      endif

      if(ran3.eq.0d0)then
         call r2455(ran3)
      endif

      call r2455(ran4)
      call r2455(ran5)
      call r2455(ran6)

      if(xflag.eq.'chic'.or.xflag.eq.'chib')then   ! chi_c/b

         if(chiflag.eq.0)then
         
            id(5)=id0
            mchi=mchi0
            sv=sv0                     
            beff=beff0
            neff=neff0
            brchipsi=brchipsi0
            errbrchi=errbrchi0
            norm=norm0
            brchihad=brchi0had

         elseif(chiflag.eq.1)then
            
            id(5)=id1
            mchi=mchi1
            sv=sv1                 
            beff=beff1
            neff=neff1
            brchipsi=brchipsi1
            errbrchi=errbrchi1
            norm=norm1
            brchihad=brchi1had

         elseif(chiflag.eq.2)then

            id(5)=id2
            mchi=mchi2
            sv=sv2
            beff=beff2
            neff=neff2
            brchipsi=brchipsi2
            errbrchi=errbrchi2
            norm=norm2
            brchihad=brchi2had

         endif

         mx=mchi
         mmu=0.1057d0 
         bb=beff

      elseif(xflag.eq.'etac'.or.xflag.eq.'etab')then

         id(5)=id0m
         mchi=meta
         sv=sv0m
         beff=beff0m
         neff=1d0
         norm=norm0m
         mx=mchi
         bb=beff

      elseif(xflag.eq.'gam'.or.xflag.eq.'pi'.or.xflag.eq.'rho'.or.
     &xflag.eq.'eta'.or.xflag.eq.'etap'.or.xflag.eq.'etaetap'.or
     &.xflag.eq.'pipm'.or.xflag.eq.'ks'.or.xflag.eq.'kpkm')then 

c     integrate over digamma system invariant mass

         call r2455(rm)

c     mgam integration range

         mmax=70d0         ! note high mmax values strongly suppressed

         id(5)=93

         my=(1d0/(mmin)**3-1d0/(mmax)**3)/3d0*rm-1d0/(3d0*mmin**3)
         mgam=1d0/(-my*3d0)**(1d0/3d0)
         weightgam=mgam**4*(1d0/(mmin)**3-1d0/(mmax)**3)/3d0
         mx=mgam
         bare=1d0

c     fit to rapidity dep.

         if(pdf.eq.1)then
            if(mgam.lt.30d0)then
               ng=100d0*mgam**(-1.6d0)+14.1d0+6d-3*(rts-1.96d3)
            else
               ng=-0.0422d0*(mgam-30d0)+14.53d0+6d-3*(rts-1.96d3)
            endif
         elseif(pdf.eq.2)then
            ng=63.4d0*mgam**(-0.541d0)+6d-3*(rts-1.96d3)
         endif

c    non-zero p_t effects

         if(pdf.eq.1)then
            if(mgam.lt.10d0)then
            beff=5.57d0*mgam**(-0.042d0)
            else
            beff=5.06d0
            endif
         elseif(pdf.eq.2)then
            if(mgam.lt.10d0)then
            beff=5.46d0*mgam**(-0.039d0)
            else
            beff=4.99d0
            endif
         endif

         bb=beff

      if(xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag.eq.'ks'.or.xflag.eq
     &.'pipm'.or.xflag.eq.'kpkm')then
            if(eflag.eq.2)then
               sv=0.11d0*(bb/5d0)**2
            elseif(eflag.eq.7)then
               sv=0.084d0*(bb/5d0)**2
            elseif(eflag.eq.10)then
               sv=0.078d0*(bb/5d0)**2
            elseif(eflag.eq.14)then
               sv=0.072d0*(bb/5d0)**2
            elseif(eflag.eq.500)then
               sv=0.15d0*(bb/5d0)**2
            elseif(eflag.eq.300)then
               sv=0.17d0*(bb/5d0)**2
            elseif(eflag.eq.900)then
               sv=0.13d0*(bb/5d0)**2
            endif
            elseif(xflag.eq.'etap'.or.xflag.eq.'eta'.or.xflag.eq
     &.'etaetap')then
            if(eflag.eq.2)then
               svj0=0.058d0*(bb/5d0)**2
               svj2=0.11d0*(bb/5d0)**2
            elseif(eflag.eq.7)then
               svj0=0.037d0*(bb/5d0)**2
               svj2=0.084d0*(bb/5d0)**2
            elseif(eflag.eq.10)then
               svj0=0.033d0*(bb/5d0)**2
               svj2=0.078d0*(bb/5d0)**2
            elseif(eflag.eq.14)then
               svj0=0.03d0*(bb/5d0)**2
               svj2=0.072d0*(bb/5d0)**2
            elseif(eflag.eq.500)then
               svj0=0.092d0*(bb/5d0)**2
               svj2=0.15d0*(bb/5d0)**2
            elseif(eflag.eq.300)then
               svj0=0.11d0*(bb/5d0)**2
               svj2=0.17d0*(bb/5d0)**2
            elseif(eflag.eq.900)then
               svj0=0.076d0*(bb/5d0)**2
               svj2=0.13d0*(bb/5d0)**2
            endif    
         else
            if(eflag.eq.2)then
               sv=0.058d0*(bb/5d0)**2
            elseif(eflag.eq.7)then
               sv=0.037d0*(bb/5d0)**2
            elseif(eflag.eq.10)then
               sv=0.033d0*(bb/5d0)**2
            elseif(eflag.eq.14)then
               sv=0.03d0*(bb/5d0)**2
           elseif(eflag.eq.500)then
               sv=0.092d0*(bb/5d0)**2
            elseif(eflag.eq.300)then
               sv=0.11d0*(bb/5d0)**2
            elseif(eflag.eq.900)then
               sv=0.076d0*(bb/5d0)**2
            endif    
      endif


         beff=beff*1.2d0       ! r_{scr}
         bb=beff

      weightnf=0.55d0*(bb/b)**2

      weightnfpi=(bb/b)**2
      endif
                       
      xoo=mx/rtso
      xo=mx/rts

ccccccccccccccccccccccccccccccccccccccccccccccccccc

      if(xflag.eq.'jpsi'.or.xflag.eq.'ups'.or.xflag.eq.'psip')then

      beta=dsqrt(1d0-4d0*mp**2/s)

c     incoming protons

      id(1)=2212
      q(1,1)=0d0
      q(2,1)=0d0
      q(3,1)=ebeam*beta
      q(4,1)=ebeam

      rec(1,1)=1.
      rec(1,2)=dfloat(id(1))
      rec(1,3)=-1.
      rec(1,4)=0.
      rec(1,5)=0.
      rec(1,6)=0.
      rec(1,7)=0.      
      rec(1,8)=q(1,1)
      rec(1,9)=q(2,1)
      rec(1,10)=q(3,1)
      rec(1,11)=q(4,1)
      rec(1,12)=dsqrt(q(4,1)**2-q(3,1)**2-q(2,1)**2-q(1,1)**2)
      rec(1,13)=0d0
      rec(1,14)=0d0
      rec(1,15)=0d0
      rec(1,16)=0d0

      if(eflag.eq.2)then

      id(2)=-2212
      
      else

      id(2)=2212

      endif

      q(1,2)=0d0
      q(2,2)=0d0
      q(3,2)=-ebeam*beta
      q(4,2)=ebeam

      rec(2,1)=2.
      rec(2,2)=dfloat(id(2))
      rec(2,3)=-1.
      rec(2,4)=0.
      rec(2,5)=0.
      rec(2,6)=0.
      rec(2,7)=0.  
      rec(2,8)=q(1,2)
      rec(2,9)=q(2,2)
      rec(2,10)=q(3,2)
      rec(2,11)=q(4,2)
      rec(2,12)=dsqrt(q(4,2)**2-q(3,2)**2-q(2,2)**2-q(1,2)**2)
      rec(2,13)=0d0
      rec(2,14)=0d0
      rec(2,15)=0d0
      rec(2,16)=0d0      

      ptmax=dsqrt(5d0)              ! max pt
      ptmin=0d0

      pt2sq=(ptmax-ptmin)*ran3+ptmin
      pt2sq=pt2sq**2   

      phi1=2d0*pi*ran0
      phi2=2d0*pi*ran1

      rmx=dsqrt(pt2sq+mx**2)

      weight=2d0*dsqrt(pt2sq)*(ptmax-ptmin)

      call r2455(ranyp)

      xgmin=(mx/rts)**2
      ypmax=dlog(xgmin**2*mp**2+ptmax**2)
      ypmin=dlog(xgmin**2*mp**2)
      yp=(ypmax-ypmin)*ranyp+ypmin
      weight=weight*(ypmax-ypmin)

      call r2455(ranpsi)   

      if(ranpsi.gt.0.5d0)then  
         pt1sq=dexp(yp)-xgmin**2*mp**2
         weight=weight*(xgmin**2*mp**2+pt1sq)
         pt2x=dsqrt(pt2sq)*dcos(phi2)
         pt2y=dsqrt(pt2sq)*dsin(phi2)
         pt1x=dsqrt(pt1sq)*dcos(phi1)
         pt1y=dsqrt(pt1sq)*dsin(phi1)
      else                    
         pt1sq=pt2sq         
         pt2sq=dexp(yp)-xgmin**2*mp**2       
         weight=weight*(xgmin**2*mp**2+pt2sq)
         pt1x=dsqrt(pt2sq)*dcos(phi2)
         pt1y=dsqrt(pt2sq)*dsin(phi2)
         pt2x=dsqrt(pt1sq)*dcos(phi1)
         pt2y=dsqrt(pt1sq)*dsin(phi1)
      endif

      ptxx=(pt1x+pt2x)**2+(pt1y+pt2y)**2
      rmx=dsqrt(ptxx+mx**2)

      if(cross.eq.1)then

c     total cross section calculation

         ym=dlog(rts/rmx)

         if(ycut.gt.ym)then
         ymax=ym             ! kinematic limit on psi rapidity
         else
         ymax=ycut
         endif

         ymin=-ymax
         yx=(ymax-ymin)*ran4+ymin
         weight=weight*(ymax-ymin)

      elseif(cross.eq.2)then

c     dsigma/dy|y=yx

      yx=0d0

      endif

         if(ranpsi.gt.0.5d0)then    ! photon emitted from q(1,k) 
            xgam=rmx*dexp(yx)/rts   ! photon mom. fraction 
            wpsi=dsqrt(xgam*s)      ! proton-photon cms energy
            xglu=(rmx/wpsi)**2      ! gluon mom. fraction
            x1=xgam
            x2=xglu
            qsq=(xgam**2*mp**2+pt1sq)/(1d0-xgam)
         else                       ! photon emitted from q(1,k)
            xgam=rmx*dexp(-yx)/rts
            wpsi=dsqrt(xgam*s)  
            xglu=(rmx/wpsi)**2
            x2=xgam
            x1=xglu
            qsq=(xgam**2*mp**2+pt2sq)/(1d0-xgam)
         endif

       weight=weight*(0.85d0-0.1d0*dabs(yx)/3d0)    ! survival factor

ccccccccccccccccccccccccccccccccccccccccccccccccccccc

      aa1=bp*(1d0-x1)
      aa2=bp*(1d0-x2)
      cc1=0.5d0*(pt2sq+mp**2)
      cc2=0.5d0*(pt1sq+mp**2)

c     impose massive on-shell condition by solving
c                   p1+ + cc1/p2- = aa1
c                   p2- + cc2/p1+ = aa2 

      root1sq=(cc1-cc2-aa1*aa2)**2-4d0*cc2*aa1*aa2
      root2sq=(cc2-cc1-aa1*aa2)**2-4d0*cc1*aa1*aa2
      if(root1sq.le.0d0.or.root2sq.le.0d0)then
         weight=0d0
         goto 808
      endif
      p1p=(cc2-cc1+aa1*aa2+dsqrt(root1sq))/(2d0*aa2)
      p2m=(cc1-cc2+aa1*aa2+dsqrt(root2sq))/(2d0*aa1)
      p1m=(pt1sq+mp**2)/(2d0*p1p)
      p2p=(pt2sq+mp**2)/(2d0*p2m)

      if(ranpsi.gt.0.5d0)then

      q(1,3)=-pt1x
      q(2,3)=-pt1y
      q(3,3)=(p1p-p1m)/rt2
      q(4,3)=(p1p+p1m)/rt2

      q(1,4)=-dsqrt(pt2sq)*dcos(phi2)
      q(2,4)=-dsqrt(pt2sq)*dsin(phi2)
      q(3,4)=(p2p-p2m)/rt2
      q(4,4)=(p2p+p2m)/rt2

      do j=1,4
         q(j,15)=q(j,1)-q(j,3)
      enddo

      else

      q(1,3)=-dsqrt(pt1sq)*dcos(phi1)
      q(2,3)=-dsqrt(pt1sq)*dsin(phi1)
      q(3,3)=(p1p-p1m)/rt2
      q(4,3)=(p1p+p1m)/rt2

      q(1,4)=-pt1x
      q(2,4)=-pt1y
      q(3,4)=(p2p-p2m)/rt2
      q(4,4)=(p2p+p2m)/rt2

      do j=1,4
         q(j,15)=q(j,2)-q(j,4)
      enddo

      endif

c     outgoing protons

      id(3)=id(1)
      rec(3,1)=3.
      rec(3,2)=dfloat(id(3))
      rec(3,3)=1.
      rec(3,4)=1.
      rec(3,5)=1.
      rec(3,6)=0.
      rec(3,7)=0.  
      rec(3,8)=q(1,3)
      rec(3,9)=q(2,3)
      rec(3,10)=q(3,3)
      rec(3,11)=q(4,3)
      rec(3,12)=dsqrt(q(4,3)**2-q(3,3)**2-q(2,3)**2-q(1,3)**2)
      rec(3,13)=0d0
      rec(3,14)=0d0
      rec(3,15)=0d0
      rec(3,16)=0d0

      id(4)=id(2)

      rec(4,1)=4.
      rec(4,2)=dfloat(id(4))
      rec(4,3)=1.
      rec(4,4)=2.
      rec(4,5)=2.
      rec(4,6)=0.
      rec(4,7)=0.  
      rec(4,8)=q(1,4)
      rec(4,9)=q(2,4)
      rec(4,10)=q(3,4)
      rec(4,11)=q(4,4)
      rec(4,12)=dsqrt(q(4,4)**2-q(3,4)**2-q(2,4)**2-q(1,4)**2)
      rec(4,13)=0d0
      rec(4,14)=0d0
      rec(4,15)=0d0
      rec(4,16)=0d0

c     j/psi (upsilon) 4-momentum

      do j=1,4
      q(j,5)=q(j,1)+q(j,2)-q(j,3)-q(j,4)
      enddo

      rec(5,1)=5.
      rec(5,2)=dfloat(id(5))  
      rec(5,3)=2.
      rec(5,4)=1.
      rec(5,5)=2.
      rec(5,6)=6.           
      if(phdecay.eq.'mu')then
      rec(5,7)=7.      
      elseif(phdecay.eq.'psipi')then
      rec(5,7)=8.
      endif 
      rec(5,8)=q(1,5)
      rec(5,9)=q(2,5)
      rec(5,10)=q(3,5)
      rec(5,11)=q(4,5)
      rec(5,12)=dsqrt(q(4,5)**2-q(3,5)**2-q(2,5)**2-q(1,5)**2)
      rec(5,13)=0d0
      rec(5,14)=0d0
      rec(5,15)=0d0
      rec(5,16)=0d0

cccccccccccccccccccccccccccccccccccccccccccccccc

c     weights 

cccccccccccccccccccccccccccccccccccccccccccccccc

cccccc proton slope - fit to hera data

      bpsi=bpsi0+4d0*alphapb*dlog(wpsi/w0b)

ccccccc

      if(ranpsi.gt.0.5d0)then
      weightp=dexp(-bpsi*pt2sq)      !  pt dependence
      else
      weightp=dexp(-bpsi*pt1sq) 
      endif

      qsqmin=mp**2*xgam**2/(1d0-xgam)
      ge=1d0/(1d0+qsq/0.71d0)**4
      mum=7.78d0
      gm=ge*mum
      fe=(4d0*mp**2*ge+qsq*gm)/(4d0*mp**2+qsq)
      fm=gm
      ww1=((1d0-xgam)*(1d0-qsqmin/qsq)*fe+xgam**2/2d0*fm)/qsq
      ww1=ww1/pi/(1d0-xgam)/137d0*2d0

      weight=weight*ww1
      weight=weight*weightp

      if(photo.eq.'fit')then

         if(xflag.eq.'psip')then
            
            weight=weight*(wpsi/w0)**delta*norm*bpsi
            weight=weight*normpsip*(wpsi/w0psip)**deltad

            else

               weight=weight*(wpsi/w0)**delta*norm*bpsi
               
            endif

        elseif(photo.eq.'theory')then

         mu=mx/2d0

cccc     mstwlo  pdfs as default

c$$$         if(pflag.eq.'mstw')then
c$$$
c$$$         glu= getonepdf(prefix,iset,xglu,mu,0)
c$$$         alfas=alphas(mu)
c$$$
c$$$         elseif(pflag.eq.'cteq')then
c$$$
c$$$         glu=ctq6pdf(0,xglu,mu)
c$$$         glu=glu*xglu
c$$$
c$$$         lambdacap=0.226d0
c$$$
c$$$         b00=(33d0-2d0*nf)/(12d0*pi)
c$$$         b11=(153d0-19d0*nf)/(2d0*pi*(33d0-2d0*nf))
c$$$         alfas=1d0/(b00*dlog(mu**2/lambdacap**2))
c$$$         alfas=alfas*(1d0-alfas*b11*dlog(dlog(mu**2/lambdacap**2)))
c$$$
c$$$         endif

         weight=weight*16d0*width*pi**3*alfas**2*glu**2*137d0/
     &        (3d0*mx**5)*conv

      endif

      else

      id(1)=2212
      q(1,1)=0d0
      q(2,1)=0d0
      q(3,1)=ebeam
      q(4,1)=ebeam

      rec(1,1)=1.
      rec(1,2)=dfloat(id(1))
      rec(1,3)=-1.
      rec(1,4)=0.
      rec(1,5)=0.
      rec(1,6)=0.
      rec(1,7)=0.      
      rec(1,8)=q(1,1)
      rec(1,9)=q(2,1)
      rec(1,10)=q(3,1)
      rec(1,11)=q(4,1)
      rec(1,12)=0d0
      rec(1,13)=0d0
      rec(1,14)=0d0
      rec(1,15)=0d0
      rec(1,16)=0d0

      if(eflag.eq.2)then

      id(2)=-2212
      
      else

      id(2)=2212

      endif

      q(1,2)=0d0
      q(2,2)=0d0
      q(3,2)=-ebeam
      q(4,2)=ebeam

      rec(2,1)=2.
      rec(2,2)=dfloat(id(2))
      rec(2,3)=-1.
      rec(2,4)=0.
      rec(2,5)=0.
      rec(2,6)=0.
      rec(2,7)=0.  
      rec(2,8)=q(1,2)
      rec(2,9)=q(2,2)
      rec(2,10)=q(3,2)
      rec(2,11)=q(4,2)
      rec(2,12)=0d0
      rec(2,13)=0d0
      rec(2,14)=0d0
      rec(2,15)=0d0
      rec(2,16)=0d0      

c     transverse momenta
c     note: minor change so proton 1 does not always have p_x=0

      phi1=2d0*pi*ran0
      phi2=2d0*pi*ran1

      if(xflag.eq.'chic'.or.xflag.eq.'etac')then

      if(gen.eq.'exact')then

      pt1sq=dsqrt(2d0)*ran2
      pt2sq=dsqrt(2d0)*ran3
      pt1sq=pt1sq**2
      pt2sq=pt2sq**2

      weight=4d0*dsqrt(pt1sq*pt2sq)*2d0

      elseif(gen.eq.'fit')then

      pt1sq=-dlog(ran2)/(bb)
      pt2sq=-dlog(ran3)/(bb)

      weight=(dexp(bb*pt1sq)*dexp(bb*pt2sq))/bb**2      

      endif
      
      else

      pt1sq=-dlog(ran2)/(bb)
      pt2sq=-dlog(ran3)/(bb)

      weight=(dexp(bb*pt1sq)*dexp(bb*pt2sq))/bb**2 

      endif

      pt1(1)=dsqrt(pt1sq)*dsin(phi1)
      pt1(2)=dsqrt(pt1sq)*dcos(phi1)
      pt2(1)=dsqrt(pt2sq)*dsin(phi2)
      pt2(2)=dsqrt(pt2sq)*dcos(phi2)
      ptx(1)=-pt1(1)-pt2(1) 
      ptx(2)=-pt1(2)-pt2(2)
      ptxsq=ptx(1)**2+ptx(2)**2
      ptdif(1)=pt1(1)-pt2(1)
      ptdif(2)=pt1(2)-pt2(2)
      ptw=(ptdif(1))**2+(ptdif(2))**2

      pt1x=pt1(1)
      pt1y=pt1(2)
      pt2x=pt2(1)
      pt2y=pt2(2)

cccccccccccccccccccccccccccccccccccccccccccccccccccc

c     x rapidity

c     transverse mass

      rmx=dsqrt(mx**2+ptxsq)   

      if(cross.eq.1)then

c     total cross section calculation

         ym=dlog(rts/rmx)

         if(ycut.gt.ym)then
         ymax=ym       ! kinematic limit on chi rapidity
         else
         ymax=ycut
         endif

      ymin=-ymax
      yx=ymax*ran4*ran5**(1d0/3d0)

           if(ran6.gt.0.5d0) yx=-yx
      weighty=ymax**3*4d0/3d0/(ymax**2-yx**2)
      weight=weight*weighty

      elseif(cross.eq.2)then

c     dsigma/dy|y=yx

      yx=0d0

      endif

c     longitudinal momenta fractions
 
      x1=dexp(yx)*rmx/rts
      x2=dexp(-yx)*rmx/rts

ccccccccccccccccccccccccccccccccccccccccccccccccccccc

      aa1=bp*(1d0-x1)
      aa2=bp*(1d0-x2)
      cc1=0.5d0*pt2sq
      cc2=0.5d0*pt1sq

c     impose massless on-shell condition by solving
c                   p1+ + cc1/p2- = aa1
c                   p2- + cc2/p1+ = aa2 

      root1sq=(cc1-cc2-aa1*aa2)**2-4d0*cc2*aa1*aa2
      root2sq=(cc2-cc1-aa1*aa2)**2-4d0*cc1*aa1*aa2
      if(root1sq.le.0d0.or.root2sq.le.0d0)then
         weight=0d0
         goto 808
      endif
      p1p=(cc2-cc1+aa1*aa2+dsqrt(root1sq))/(2d0*aa2)
      p2m=(cc1-cc2+aa1*aa2+dsqrt(root2sq))/(2d0*aa1)
      p1m=pt1sq/(2d0*p1p)
      p2p=pt2sq/(2d0*p2m)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     outgoing protons

      id(3)=id(1)
      q(1,3)=pt1(1)
      q(2,3)=pt1(2)
      q(3,3)=(p1p-p1m)/rt2
      q(4,3)=(p1p+p1m)/rt2

      rec(3,1)=3.
      rec(3,2)=dfloat(id(3))
      rec(3,3)=1.
      rec(3,4)=1.
      rec(3,5)=1.
      rec(3,6)=0.
      rec(3,7)=0.  
      rec(3,8)=q(1,3)
      rec(3,9)=q(2,3)
      rec(3,10)=q(3,3)
      rec(3,11)=q(4,3)
      rec(3,12)=0d0
      rec(3,13)=0d0
      rec(3,14)=0d0
      rec(3,15)=0d0
      rec(3,16)=0d0

      id(4)=id(2)
      q(1,4)=pt2(1)
      q(2,4)=pt2(2)
      q(3,4)=(p2p-p2m)/rt2
      q(4,4)=(p2p+p2m)/rt2

      rec(4,1)=4.
      rec(4,2)=dfloat(id(4))
      rec(4,3)=1.
      rec(4,4)=2.
      rec(4,5)=2.
      rec(4,6)=0.
      rec(4,7)=0.  
      rec(4,8)=q(1,4)
      rec(4,9)=q(2,4)
      rec(4,10)=q(3,4)
      rec(4,11)=q(4,4)
      rec(4,12)=0d0
      rec(4,13)=0d0
      rec(4,14)=0d0
      rec(4,15)=0d0
      rec(4,16)=0d0

c     chi momentum

      do j=1,4
      q(j,5)=q(j,1)+q(j,2)-q(j,3)-q(j,4)
      enddo

      rec(5,1)=5.
      rec(5,2)=dfloat(id(5))  
      rec(5,3)=2.
      rec(5,4)=1.
      rec(5,5)=2.
      rec(5,6)=6.      
      rec(5,7)=7.      
      rec(5,8)=q(1,5)
      rec(5,9)=q(2,5)
      rec(5,10)=q(3,5)
      rec(5,11)=q(4,5)
      rec(5,12)=dsqrt(q(4,5)**2-q(3,5)**2-q(2,5)**2-q(1,5)**2)
      rec(5,13)=0d0
      rec(5,14)=0d0
      rec(5,15)=0d0
      rec(5,16)=0d0

      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     final state particle 4-vector generation

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     digamma cep

      if(xflag.eq.'gam')then

      x11=x1
      x22=x2
      d1=-x1*rts*(1d0-dsqrt(1d0+4d0*pt1sq/(x1*rts)**2))
     &/2d0
      d2=-x2*rts*(1d0-dsqrt(1d0+4d0*pt2sq/(x2*rts)**2))
     &/2d0

c     assign incoming gluon momenta...

      do l=1,4
      pboo(l)=q(l,5)
      q(l,10)=x1*q(l,1)
      q(l,11)=x2*q(l,2)
      enddo

      q(1,10)=-pt1(1)
      q(2,10)=-pt1(2)
      q(1,11)=-pt2(1)
      q(2,11)=-pt2(2)
      q(4,10)=q(4,10)+d1
      q(4,11)=q(4,11)+d2

c     ...and make on-shell approximation

      pboo(4)=pboo(4)+d1+d2
      sh=(pboo(4)**2-pboo(3)**2-pboo(2)**2-pboo(1)**2)
      mxx=dsqrt(sh)

c     2-body phase space for weight generation

      call r2455(rang1)
      call r2455(rang2)

      costg=2d0*rang1-1d0
      phig=2d0*pi*rang2
      sintg=dsqrt(1d0-costg**2)
      cphig=dcos(phig)
      sphig=dsin(phig)

      pgmod=mxx/2d0
      pcm(4)=pgmod
      pcm(1)=pgmod*sintg*sphig
      pcm(2)=pgmod*sintg*cphig
      pcm(3)=pgmod*costg

      call boost(mxx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      tvec(k)=-q(k,13)+x22*q(k,2)
      uvec(k)=-q(k,13)+x11*q(k,1)
      enddo

      tvec(1)=tvec(1)-pt2(1)
      tvec(2)=tvec(2)-pt2(2)
      tvec(4)=tvec(4)+d2
      uvec(1)=uvec(1)-pt1(1)
      uvec(2)=uvec(2)-pt1(2)
      uvec(4)=uvec(4)+d1
      t=rdot(tvec,tvec)
      u=rdot(uvec,uvec)

c     generate photon 4-vectors (exact kinematics)

      do l=1,4
      pboo(l)=q(l,5)
      enddo

      costg=2d0*rang1-1d0
      phig=2d0*pi*rang2
      sintg=dsqrt(1d0-costg**2)
      cphig=dcos(phig)
      sphig=dsin(phig)

      pgmod=mgam/2d0
      pcm(4)=pgmod
      pcm(1)=pgmod*sintg*sphig
      pcm(2)=pgmod*sintg*cphig
      pcm(3)=pgmod*costg

      call boost(mx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      pgtot(k)=q(k,12)+q(k,13)
      enddo

      q(4,10)=q(4,10)-d1
      q(4,11)=q(4,11)-d2

      call boost(mx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      pgtot(k)=q(k,12)+q(k,13)
      enddo

      id(12)=22
      id(13)=22

      rec(12,1)=6
      rec(12,2)=dfloat(id(12))
      rec(12,3)=1
      rec(12,4)=5
      rec(12,5)=5
      rec(12,6)=0
      rec(12,7)=0 
      rec(12,8)=q(1,12)
      rec(12,9)=q(2,12)
      rec(12,10)=q(3,12)
      rec(12,11)=q(4,12)
      rec(12,12)=0d0
      rec(12,13)=0d0
      rec(12,14)=0d0
      rec(12,15)=0d0
      rec(12,16)=0d0

      rec(13,1)=7
      rec(13,2)=dfloat(id(13))
      rec(13,3)=1
      rec(13,4)=5
      rec(13,5)=5
      rec(13,6)=0
      rec(13,7)=0
      rec(13,8)=q(1,13)
      rec(13,9)=q(2,13)
      rec(13,10)=q(3,13)
      rec(13,11)=q(4,13)
      rec(13,12)=0d0
      rec(13,13)=0d0
      rec(13,14)=0d0
      rec(13,15)=0d0
      rec(13,16)=0d0

      elseif(xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag
     &.eq.'eta'.or.xflag.eq.'etap'.or.xflag.eq.'etaetap'
     &.or.xflag.eq.'pipm'.or.xflag.eq.'ks'.or.xflag.eq.'kpkm')then

      x11=x1
      x22=x2
      d1=-x1*rts*(1d0-dsqrt(1d0+4d0*pt1sq/(x1*rts)**2))
     &/2d0
      d2=-x2*rts*(1d0-dsqrt(1d0+4d0*pt2sq/(x2*rts)**2))
     &/2d0

c     assign incoming gluon momenta...

      do l=1,4
      pboo(l)=q(l,5)
      q(l,10)=x1*q(l,1)
      q(l,11)=x2*q(l,2)
      enddo

      q(1,10)=-pt1(1)
      q(2,10)=-pt1(2)
      q(1,11)=-pt2(1)
      q(2,11)=-pt2(2)
      q(4,10)=q(4,10)+d1
      q(4,11)=q(4,11)+d2

c     ...and make on-shell approximation

      pboo(4)=pboo(4)+d1+d2
      sh=(pboo(4)**2-pboo(3)**2-pboo(2)**2-pboo(1)**2)
      mxx=dsqrt(sh)

c     2-body phase space for weight generation

      call r2455(rang1)
      call r2455(rang2)

      costg=2d0*rang1-1d0
      phig=2d0*pi*rang2
      sintg=dsqrt(1d0-costg**2)
      cphig=dcos(phig)
      sphig=dsin(phig)

      pgmod=mxx/2d0
      pcm(4)=pgmod
      pgmod=dsqrt(pgmod**2-mmes**2)
      
      if(xflag.eq.'etaetap')then
         pgmod=(mxx**2+(mmes1**2-mmes2**2))/(2d0*mxx)
         pcm(4)=pgmod
         pgmod=dsqrt(pgmod**2-mmes1**2)
      endif

      pcm(1)=pgmod*sintg*sphig
      pcm(2)=pgmod*sintg*cphig
      pcm(3)=pgmod*costg


      call boost(mxx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      tvec(k)=-q(k,13)+x22*q(k,2)
      uvec(k)=-q(k,13)+x11*q(k,1)
      enddo

      tvec(1)=tvec(1)-pt2(1)
      tvec(2)=tvec(2)-pt2(2)
      tvec(4)=tvec(4)+d2
      uvec(1)=uvec(1)-pt1(1)
      uvec(2)=uvec(2)-pt1(2)
      uvec(4)=uvec(4)+d1
      t=rdot(tvec,tvec)
      u=rdot(uvec,uvec)

      ppi=dsqrt(sh/4d0-mmes**2)
      
      if(xflag.eq.'etaetap')then
         ppi=dsqrt(sh/4d0-mmes2**2)
      endif

      costpi=-(1d0+2d0*t/sh)*dsqrt(sh)/(2d0*ppi)
      picostsq=costpi**2
      sign=costpi/dabs(costpi)

c     generate meson 4-vectors (exact kinematics)

      do l=1,4
      pboo(l)=q(l,5)
      enddo

      costg=2d0*rang1-1d0
      phig=2d0*pi*rang2
      sintg=dsqrt(1d0-costg**2)
      cphig=dcos(phig)
      sphig=dsin(phig)

      pgmod=mgam/2d0
      pcm(4)=pgmod
      pgmod=dsqrt(pgmod**2-mmes**2)

      if(xflag.eq.'etaetap')then
         pgmod=(mx**2+(mmes1**2-mmes2**2))/(2d0*mx)
         pcm(4)=pgmod
         pgmod=dsqrt(pgmod**2-mmes1**2)
      endif

      pcm(1)=pgmod*sintg*sphig
      pcm(2)=pgmod*sintg*cphig
      pcm(3)=pgmod*costg

      call boost(mx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      pgtot(k)=q(k,12)+q(k,13)
      enddo

      q(4,10)=q(4,10)-d1
      q(4,11)=q(4,11)-d2

      call boost(mx,pboo,pcm,plb)

      do k=1,4
      q(k,12)=plb(k)
      q(k,13)=pboo(k)-q(k,12)
      pgtot(k)=q(k,12)+q(k,13)
      enddo

      rec(12,1)=6
      rec(12,2)=dfloat(id(12))
      rec(12,3)=2
      rec(12,4)=5
      rec(12,5)=5
      rec(12,6)=8
      rec(12,7)=9 
      rec(12,8)=q(1,12)
      rec(12,9)=q(2,12)
      rec(12,10)=q(3,12)
      rec(12,11)=q(4,12)
      rec(12,12)=dsqrt(q(4,12)**2-q(3,12)**2-q(2,12)**2-q(1,12)**2)
      rec(12,13)=0d0
      rec(12,14)=0d0
      rec(12,15)=0d0
      rec(12,16)=0d0

      rec(13,1)=7
      rec(13,2)=dfloat(id(13))
      rec(13,3)=2
      rec(13,4)=5
      rec(13,5)=5
      rec(13,6)=10
      rec(13,7)=11
      rec(13,8)=q(1,13)
      rec(13,9)=q(2,13)
      rec(13,10)=q(3,13)
      rec(13,11)=q(4,13)
      rec(13,12)=dsqrt(q(4,13)**2-q(3,13)**2-q(2,13)**2-q(1,13)**2)
      rec(13,13)=0d0
      rec(13,14)=0d0
      rec(13,15)=0d0
      rec(13,16)=0d0

ccccccccccccccccccccccccccccccccccccccccccccccccc

c     pi/eta(12)-->gamma(14)+gamma(15)

cccccccccccccccccccccccccccccccccccccccccccccccccc

      if(xflag.eq.'pi'.or.xflag.eq.'eta')then

      call r2455(ran9)
      call r2455(ran10)

      cost8=2d0*ran9-1d0
      phi8=2d0*pi*ran10
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mmes/2d0
      pcmod=pcm(4)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,12)
      enddo
      call boost(mmes,pboo,pcm,plb)
      do k=1,4
      q(k,14)=plb(k)
      q(k,15)=q(k,12)-q(k,14)
      enddo

      id(14)=22
      id(15)=22

      rec(14,1)=8
      rec(14,2)=dfloat(id(14))
      rec(14,3)=1
      rec(14,4)=6
      rec(14,5)=6
      rec(14,6)=0
      rec(14,7)=0 
      rec(14,8)=q(1,14)
      rec(14,9)=q(2,14)
      rec(14,10)=q(3,14)
      rec(14,11)=q(4,14)
      rec(14,12)=0d0
      rec(14,13)=0d0
      rec(14,14)=0d0
      rec(14,15)=0d0
      rec(14,16)=0d0

      rec(15,1)=9
      rec(15,2)=dfloat(id(15))
      rec(15,3)=1
      rec(15,4)=6
      rec(15,5)=6
      rec(15,6)=0
      rec(15,7)=0
      rec(15,8)=q(1,15)
      rec(15,9)=q(2,15)
      rec(15,10)=q(3,15)
      rec(15,11)=q(4,15)
      rec(15,12)=0d0
      rec(15,13)=0d0
      rec(15,14)=0d0
      rec(15,15)=0d0
      rec(15,16)=0d0

ccccccccccccccccccccccccccccccccccccccccccccccccc

c     pi/eta(13)-->gamma(16)+gamma(17)

cccccccccccccccccccccccccccccccccccccccccccccccccc

      call r2455(ran9)
      call r2455(ran10)

      cost8=2d0*ran9-1d0
      phi8=2d0*pi*ran10
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mmes/2d0
      pcmod=pcm(4)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,13)
      enddo
      call boost(mmes,pboo,pcm,plb)
      do k=1,4
      q(k,16)=plb(k)
      q(k,17)=q(k,13)-q(k,16)
      enddo

      id(16)=22
      id(17)=22

      rec(16,1)=10
      rec(16,2)=dfloat(id(16))
      rec(16,3)=1
      rec(16,4)=7
      rec(16,5)=7
      rec(16,6)=0
      rec(16,7)=0 
      rec(16,8)=q(1,16)
      rec(16,9)=q(2,16)
      rec(16,10)=q(3,16)
      rec(16,11)=q(4,16)
      rec(16,12)=0d0
      rec(16,13)=0d0
      rec(16,14)=0d0
      rec(16,15)=0d0
      rec(16,16)=0d0

      rec(17,1)=11
      rec(17,2)=dfloat(id(17))
      rec(17,3)=1
      rec(17,4)=7
      rec(17,5)=7
      rec(17,6)=0
      rec(17,7)=0
      rec(17,8)=q(1,17)
      rec(17,9)=q(2,17)
      rec(17,10)=q(3,17)
      rec(17,11)=q(4,17)
      rec(17,12)=0d0
      rec(17,13)=0d0
      rec(17,14)=0d0
      rec(17,15)=0d0
      rec(17,16)=0d0

      endif

      elseif(xflag.eq.'chic'.or.xflag.eq.'chib')then

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     chi_(c/b) polarization + decay products

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
         
c     generate chi polarization vectors in pt(chi)=0 frame...

c     longitudinal vector

      e0(1)=0d0
      e0(2)=0d0
      e0(3)=q(4,5)/mchi
      e0(4)=dsqrt(q(4,5)**2-mchi**2)/mchi

c     real transverse basis vectors

      n1(1)=1d0
      n1(2)=0d0
      n1(3)=0d0
      n1(4)=0d0

      n2(1)=0d0
      n2(2)=1d0
      n2(3)=0d0
      n2(4)=0d0

c     ...and rotate to lab basis

      do k=1,4
      pboo(k)=q(k,5)
      enddo

      call rotate(e0,5,e0rot)

c     generate polarization vectors

      n1rot(1)=-(q(2,1)*q(3,5)-q(3,1)*q(2,5))
      n1rot(2)=-(q(3,1)*q(1,5)-q(1,1)*q(3,5))
      n1rot(3)=-(q(1,1)*q(2,5)-q(2,1)*q(1,5))
      
      fnorm=dsqrt(n1rot(1)**2+n1rot(2)**2+n1rot(3)**2)

      do j=1,3
         n1rot(j)=n1rot(j)/fnorm
      enddo  
      n1rot(4)=0d0

      n2rot(1)=-(n1rot(2)*q(3,5)-n1rot(3)*q(2,5))
      n2rot(2)=-(n1rot(3)*q(1,5)-n1rot(1)*q(3,5))
      n2rot(3)=-(n1rot(1)*q(2,5)-n1rot(2)*q(1,5))
      n2rot(4)=0d0

      fnorm=dsqrt(n2rot(1)**2+n2rot(2)**2+n2rot(3)**2)

      do j=1,4
         n2rot(j)=n2rot(j)/fnorm
      enddo

      do j=1,4
      echiplus(j)=-(n1rot(j)+zi*n2rot(j))/rt2
      echiminus(j)=(n1rot(j)-zi*n2rot(j))/rt2
      echi0(j)=e0rot(j)
      enddo

c     various checks
      
c      qmod=dsqrt(q(1,5)**2+q(2,5)**2+q(3,5)**2)
c      emod=sqrt(abs(echi0(1))**2+abs(echi0(2))**2+abs(echi0(3))**2)

c      print*,q(1,5)/qmod,q(2,5)/qmod,q(3,5)/qmod
c      print*,echi0(1)/emod,echi0(2)/emod,echi0(3)/emod

c      print*,abs(echi0(4))**2-abs(echi0(3))**2-abs(echi0
c     &(2))**2-abs(echi0(1))**2
      
c      print*,cdabs(echiminus(4)*q(4,5)-echiminus(3)*q(3,5)-echiminus(2)*
c     &q(2,5)-echiminus(1)*q(1,5))

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c -- decay 5(mchi) --> 6(0) + 7(mpsi)

      call r2455(ran7)
      call r2455(ran8)

      phi6=2d0*pi*ran7
      cost6=2d0*ran8-1d0
      sint6=dsqrt(1d0-cost6**2)
      cphi6=dcos(phi6)
      sphi6=dsin(phi6)
      pmod=(mchi**2-mpsi**2)/(2d0*mchi)
      pcm(4)=pmod
      pcm(1)=pmod*sint6*sphi6
      pcm(2)=pmod*sint6*cphi6
      pcm(3)=pmod*cost6
      do k=1,4
      pboo(k)=q(k,5)
      enddo

      call boost(mchi,pboo,pcm,plb)
      do k=1,4
      q(k,6)=plb(k)
      q(k,7)=q(k,5)-q(k,6)
      enddo

      id(6)=22

      rec(6,1)=6
      rec(6,2)=dfloat(id(6))
      rec(6,3)=1
      rec(6,4)=5
      rec(6,5)=5
      rec(6,6)=0
      rec(6,7)=0 
      rec(6,8)=q(1,6)
      rec(6,9)=q(2,6)
      rec(6,10)=q(3,6)
      rec(6,11)=q(4,6)
      rec(6,12)=0d0
      rec(6,13)=0d0
      rec(6,14)=0d0
      rec(6,15)=0d0
      rec(6,16)=0d0

      rec(7,1)=7
      rec(7,2)=dfloat(id(7))
      rec(7,3)=2
      rec(7,4)=5
      rec(7,5)=5
      rec(7,6)=8
      rec(7,7)=9 
      rec(7,8)=q(1,7)
      rec(7,9)=q(2,7)
      rec(7,10)=q(3,7)
      rec(7,11)=q(4,7)
      rec(7,12)=dsqrt(q(4,7)**2-q(3,7)**2-q(2,7)**2-q(1,7)**2)
      rec(7,13)=0d0
      rec(7,14)=0d0
      rec(7,15)=0d0
      rec(7,16)=0d0

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c -- decay 7(mpsi) --> 8+(mmu) + 9-(mmu)

      call r2455(ran9)
      call r2455(ran10)

      cost8=2d0*ran9-1d0
      phi8=2d0*pi*ran10
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mpsi/2d0
      pcmod=dsqrt(pcm(4)**2-mmu**2)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,7)
      enddo
      call boost(mpsi,pboo,pcm,plb)
      do k=1,4
      q(k,8)=plb(k)
      q(k,9)=q(k,7)-q(k,8)
      enddo

      id(8)=-13
      id(9)=13
   
      rec(8,1)=8
      rec(8,2)=dfloat(id(8))
      rec(8,3)=1
      rec(8,4)=7
      rec(8,5)=7
      rec(8,6)=0
      rec(8,7)=0 
      rec(8,8)=q(1,8)
      rec(8,9)=q(2,8)
      rec(8,10)=q(3,8)
      rec(8,11)=q(4,8)
      rec(8,12)=dsqrt(q(4,8)**2-q(3,8)**2-q(2,8)**2-q(1,8)**2)
      rec(8,13)=0d0
      rec(8,14)=0d0
      rec(8,15)=0d0
      rec(8,16)=0d0

      rec(9,1)=9
      rec(9,2)=dfloat(id(9))
      rec(9,3)=1
      rec(9,4)=7
      rec(9,5)=7
      rec(9,6)=0
      rec(9,7)=0 
      rec(9,8)=q(1,9)
      rec(9,9)=q(2,9)
      rec(9,10)=q(3,9)
      rec(9,11)=q(4,9)
      rec(9,12)=dsqrt(q(4,9)**2-q(3,9)**2-q(2,9)**2-q(1,9)**2)
      rec(9,13)=0d0
      rec(9,14)=0d0
      rec(9,15)=0d0
      rec(9,16)=0d0

      elseif(xflag.eq.'jpsi'.or.xflag.eq.'ups'.or.xflag.eq.'psip')then

      if(phdecay.eq.'mu')then

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c -- decay 5(mpsi) --> 8+(mmu) + 9-(mmu)

      call r2455(ran9)
      call r2455(ran10)

      cost8=2d0*ran9-1d0
      phi8=2d0*pi*ran10
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mx/2d0
      pcmod=dsqrt(pcm(4)**2-mmu**2)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,5)
      enddo
      call boost(mx,pboo,pcm,plb)
      do k=1,4
      q(k,8)=plb(k)
      q(k,9)=q(k,5)-q(k,8)
      enddo

      id(8)=-13
      id(9)=13
   
      rec(8,1)=6
      rec(8,2)=dfloat(id(8))
      rec(8,3)=1
      rec(8,4)=5
      rec(8,5)=5
      rec(8,6)=0
      rec(8,7)=0 
      rec(8,8)=q(1,8)
      rec(8,9)=q(2,8)
      rec(8,10)=q(3,8)
      rec(8,11)=q(4,8)
      rec(8,12)=dsqrt(q(4,8)**2-q(3,8)**2-q(2,8)**2-q(1,8)**2)
      rec(8,13)=0d0
      rec(8,14)=0d0
      rec(8,15)=0d0
      rec(8,16)=0d0

      rec(9,1)=7
      rec(9,2)=dfloat(id(9))
      rec(9,3)=1
      rec(9,4)=5
      rec(9,5)=5
      rec(9,6)=0
      rec(9,7)=0 
      rec(9,8)=q(1,9)
      rec(9,9)=q(2,9)
      rec(9,10)=q(3,9)
      rec(9,11)=q(4,9)
      rec(9,12)=dsqrt(q(4,9)**2-q(3,9)**2-q(2,9)**2-q(1,9)**2)
      rec(9,13)=0d0
      rec(9,14)=0d0
      rec(9,15)=0d0
      rec(9,16)=0d0

      elseif(phdecay.eq.'psipi')then

      call rambo(npart,ein,am,pout,wt) ! call rambo 

      do j=1,4
         pboo(j)=q(j,5)
      enddo

c      print*,dsqrt(q(4,5)**2-q(3,5)**2-q(2,5)**2-q(1,5)**2)

      do k=1,3
         do j=1,4
            pcm(j)=pout(j,k)
         enddo
         call boost(mx,pboo,pcm,plb)
         kk=7+k
         do j=1,4
            q(j,kk)=plb(j)
         enddo
      enddo
      
      weight=weight*wt/sumr

      call r2455(ran9)
      call r2455(ran10)

      cost8=2d0*ran9-1d0
      phi8=2d0*pi*ran10
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mjpsi/2d0
      pcmod=dsqrt(pcm(4)**2-mmu**2)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,8)
      enddo
      call boost(mjpsi,pboo,pcm,plb)
      do k=1,4
      q(k,11)=plb(k)
      q(k,12)=q(k,8)-q(k,11)
      enddo

c      print*,dsqrt(q(4,11)**2-q(3,11)**2-q(2,11)**2-q(1,11)**2)
c      print*,dsqrt(q(4,12)**2-q(3,12)**2-q(2,12)**2-q(1,12)**2)

      rec(8,1)=6
      rec(8,2)=dfloat(idh1)
      rec(8,3)=2
      rec(8,4)=5
      rec(8,5)=5
      rec(8,6)=9
      rec(8,7)=10 
      rec(8,8)=q(1,8)
      rec(8,9)=q(2,8)
      rec(8,10)=q(3,8)
      rec(8,11)=q(4,8)
      rec(8,12)=dsqrt(q(4,8)**2-q(3,8)**2-q(2,8)**2-q(1,8)**2)
      rec(8,13)=0d0
      rec(8,14)=0d0
      rec(8,15)=0d0
      rec(8,16)=0d0

      rec(9,1)=7
      rec(9,2)=dfloat(idh2)
      rec(9,3)=1
      rec(9,4)=5
      rec(9,5)=5
      rec(9,6)=0
      rec(9,7)=0 
      rec(9,8)=q(1,9)
      rec(9,9)=q(2,9)
      rec(9,10)=q(3,9)
      rec(9,11)=q(4,9)
      rec(9,12)=dsqrt(q(4,9)**2-q(3,9)**2-q(2,9)**2-q(1,9)**2)
      rec(9,13)=0d0
      rec(9,14)=0d0
      rec(9,15)=0d0
      rec(9,16)=0d0

      rec(10,1)=8
      rec(10,2)=dfloat(idh3)
      rec(10,3)=1
      rec(10,4)=5
      rec(10,5)=5
      rec(10,6)=0
      rec(10,7)=0 
      rec(10,8)=q(1,10)
      rec(10,9)=q(2,10)
      rec(10,10)=q(3,10)
      rec(10,11)=q(4,10)
      rec(10,12)=dsqrt(q(4,10)**2-q(3,10)**2-q(2,10)**2-q(1,10)**2)
      rec(10,13)=0d0
      rec(10,14)=0d0
      rec(10,15)=0d0
      rec(10,16)=0d0

      id(11)=-13
      id(12)=13

      rec(11,1)=9
      rec(11,2)=dfloat(id(11))
      rec(11,3)=1
      rec(11,4)=6
      rec(11,5)=6
      rec(11,6)=0
      rec(11,7)=0 
      rec(11,8)=q(1,11)
      rec(11,9)=q(2,11)
      rec(11,10)=q(3,11)
      rec(11,11)=q(4,11)
      rec(11,12)=dsqrt(q(4,11)**2-q(3,11)**2-q(2,11)**2-q(1,11)**2)
      rec(11,13)=0d0
      rec(11,14)=0d0
      rec(11,15)=0d0
      rec(11,16)=0d0

      rec(12,1)=10
      rec(12,2)=dfloat(id(12))
      rec(12,3)=1
      rec(12,4)=6
      rec(12,5)=6
      rec(12,6)=0
      rec(12,7)=0 
      rec(12,8)=q(1,12)
      rec(12,9)=q(2,12)
      rec(12,10)=q(3,12)
      rec(12,11)=q(4,12)
      rec(12,12)=dsqrt(q(4,12)**2-q(3,12)**2-q(2,12)**2-q(1,12)**2)
      rec(12,13)=0d0
      rec(12,14)=0d0
      rec(12,15)=0d0
      rec(12,16)=0d0

c      print*,dsqrt(q(4,8)**2-q(3,8)**2-q(2,8)**2-q(1,8)**2)
c      print*,dsqrt(q(4,9)**2-q(3,9)**2-q(2,9)**2-q(1,9)**2)
c      print*,dsqrt(q(4,10)**2-q(3,10)**2-q(2,10)**2-q(1,10)**2)

      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      endif

      if(xflag.eq.'chic'.or.xflag.eq.'etac'.or.
     &     xflag.eq.'chib'.or.xflag.eq.'etab')then

ccc   chi_c0 4-body decay

      if(decay.eq.4)then

         npart=4
         ein=mx
         do j=1,2
            am(j)=mh1
            am(j+2)=mh2
         enddo

         call rambo(npart,ein,am,pout,wt)   ! call rambo

         do j=1,4
            pboo(j)=q(j,5)
         enddo

         do k=1,4
            do j=1,4
               pcm(j)=pout(j,k)
            enddo
            call boost(mx,pboo,pcm,plb)
            kk=13+k
            do j=1,4
               q(j,kk)=plb(j)
            enddo
         enddo

         weight=weight*wt/sumr

      id(14)=idh1
      id(15)=idh2

      rec(14,1)=6
      rec(14,2)=dfloat(id(14))
      rec(14,3)=1
      rec(14,4)=5
      rec(14,5)=5
      rec(14,6)=0
      rec(14,7)=0 
      rec(14,8)=q(1,14)
      rec(14,9)=q(2,14)
      rec(14,10)=q(3,14)
      rec(14,11)=q(4,14)
      rec(14,12)=mh1
      rec(14,13)=0d0
      rec(14,14)=0d0
      rec(14,15)=0d0
      rec(14,16)=0d0

      rec(15,1)=7
      rec(15,2)=dfloat(id(15))
      rec(15,3)=1
      rec(15,4)=5
      rec(15,5)=5
      rec(15,6)=0
      rec(15,7)=0 
      rec(15,8)=q(1,15)
      rec(15,9)=q(2,15)
      rec(15,10)=q(3,15)
      rec(15,11)=q(4,15)
      rec(15,12)=mh1
      rec(15,13)=0d0
      rec(15,14)=0d0
      rec(15,15)=0d0
      rec(15,16)=0d0

      id(16)=idh3
      id(17)=idh4

      rec(16,1)=8
      rec(16,2)=dfloat(id(16))
      rec(16,3)=1
      rec(16,4)=5
      rec(16,5)=5
      rec(16,6)=0
      rec(16,7)=0 
      rec(16,8)=q(1,16)
      rec(16,9)=q(2,16)
      rec(16,10)=q(3,16)
      rec(16,11)=q(4,16)
      rec(16,12)=mh2
      rec(16,13)=0d0
      rec(16,14)=0d0
      rec(16,15)=0d0
      rec(16,16)=0d0

      rec(17,1)=9
      rec(17,2)=dfloat(id(17))
      rec(17,3)=1
      rec(17,4)=5
      rec(17,5)=5
      rec(17,6)=0
      rec(17,7)=0 
      rec(17,8)=q(1,17)
      rec(17,9)=q(2,17)
      rec(17,10)=q(3,17)
      rec(17,11)=q(4,17)
      rec(17,12)=mh2
      rec(17,13)=0d0
      rec(17,14)=0d0
      rec(17,15)=0d0
      rec(17,16)=0d0

      else

c -- decay 5(mx) --> 14(mh) + 15(mh)

      call r2455(ran11)
      call r2455(ran12)

      cost8=2d0*ran11-1d0
      phi8=2d0*pi*ran12
      sint8=dsqrt(1d0-cost8**2)
      cphi8=dcos(phi8)
      sphi8=dsin(phi8)
      pcm(4)=mx/2d0
      pcmod=dsqrt(pcm(4)**2-mh**2)
      pcm(1)=pcmod*sint8*sphi8
      pcm(2)=pcmod*sint8*cphi8
      pcm(3)=pcmod*cost8   

      do k=1,4
      pboo(k)=q(k,5)
      enddo
      call boost(mx,pboo,pcm,plb)
      do k=1,4
      q(k,14)=plb(k)
      q(k,15)=q(k,5)-q(k,14)
      enddo

      id(14)=idh1
      id(15)=idh2

      rec(14,1)=6
      rec(14,2)=dfloat(id(14))
      rec(14,3)=1
      rec(14,4)=5
      rec(14,5)=5
      rec(14,6)=0
      rec(14,7)=0 
      rec(14,8)=q(1,14)
      rec(14,9)=q(2,14)
      rec(14,10)=q(3,14)
      rec(14,11)=q(4,14)
      rec(14,12)=mh
      rec(14,13)=0d0
      rec(14,14)=0d0
      rec(14,15)=0d0
      rec(14,16)=0d0

      rec(15,1)=7
      rec(15,2)=dfloat(id(15))
      rec(15,3)=1
      rec(15,4)=5
      rec(15,5)=5
      rec(15,6)=0
      rec(15,7)=0 
      rec(15,8)=q(1,15)
      rec(15,9)=q(2,15)
      rec(15,10)=q(3,15)
      rec(15,11)=q(4,15)
      rec(15,12)=mh
      rec(15,13)=0d0
      rec(15,14)=0d0
      rec(15,15)=0d0
      rec(15,16)=0d0

      endif

      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c      place cuts

      call cut(icut)

      if(icut.eq.0)then
 
         weight=0d0
         if(ll.eq.1)then
         ncut=ncut+1
         endif
         goto 700 

       endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     weight evaluation

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      if(xflag.eq.'gam')then

      if(mgam.lt.2d0)then
         print*,'warning, m_x region too low (< 2 gev) for pqcd calculat
     &ion to be reliable'
         goto 889
      endif

c     (g(10)g(11) --> gam(12)gam(13)) subprocess (j_z=0) amplitude:

         zamppp=1d0
         zammpp=1d0
         zapmmp=-0.5d0*((sh**2+t**2)/u**2)*(dlog(-t/sh))**2
     &   -((t-sh)/u)*dlog(-t/sh)-1d0-zi*pi*(((t**2+sh**2)/u**2)
     &   *dlog(-t/sh)+(t-sh)/u)
         zapppp=-0.5d0*((t**2+u**2)/sh**2)*(dlog(t/u)**2+pi**2)
     &   -((t-u)/sh)*dlog(t/u)-1d0

c     average over gluon helicities at amplitude level:

         zamp1=0.5d0*(zapppp+zammpp)
         zamp2=0.5d0*(zamppp+zamppp)

         mu=0.5d0*mgam           ! hard scale mu = mgam/2

         if(mu.gt.4.75d0)then
            nf=5d0
            qf=11d0/9d0
            if(pdf.eq.1)then
               lambdacap=0.22d0
            elseif(pdf.eq.2)then
               lambdacap=0.25d0
            endif
         else
            nf=4d0
            qf=10d0/9d0
            if(pdf.eq.1)then
               lambdacap=0.3d0
            elseif(pdf.eq.2)then
               lambdacap=0.33d0
            endif
         endif

         b0=(33d0-2d0*nf)/(12d0*pi)
         b1=(153d0-19d0*nf)/(2d0*pi*(33d0-2d0*nf))
         alfas=1d0/(b0*dlog(mu**2/lambdacap**2))
         alfas=alfas*(1d0-alfas*b1*dlog(dlog(mu**2/lambdacap**2)))

         weight=weight*2d0*(cdabs(zamp1)**2+cdabs(zamp2)**2)
         weight=weight*(8d0*0.5d0*(qf)*(1d0/137d0)*alfas)**2
         weight=weight/(2d0*16d0*pi*mgam**2)

         call lumw(mgam,wl,i)      !'effective luminosity'

         weight=weight*wl/mgam*2d0*b**2

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      elseif(xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag.eq.'eta'
     &.or.xflag.eq.'etap'.or.xflag.eq.'etaetap'.or.xflag.eq.'pipm'.or.
     &xflag.eq.'ks'.or.xflag.eq.'kpkm')then

      ccut=0.8d0   ! avoid singular high cos(theta) region

      if(picostsq.gt.ccut)then   ! gg --> mmbar sub cross section
      call piint1(dsqrt(ccut)*sign,cpi,clpp,clpm,vpm,vmp)
      else
      call piint1(costpi,cpi,clpp,clpm,vpm,vmp)
      endif

      qs=(rmx/2d0)

         if(qs.gt.4.75d0)then
            nf=5d0
            if(pdf.eq.1)then
               lambdacap=0.22d0
            elseif(pdf.eq.2)then
               lambdacap=0.25d0
            endif
         else
            nf=4d0
            if(pdf.eq.1)then
               lambdacap=0.3d0
            elseif(pdf.eq.2)then
               lambdacap=0.33d0
            endif
         endif

         b0=(33d0-2d0*nf)/(12d0*pi)
         b1=(153d0-19d0*nf)/(2d0*pi*(33d0-2d0*nf))
         
         qs=qs**2
         alphasp=1d0/(b0*dlog(qs/lambdacap**2))  
         alphas0=1d0/(b0*dlog(1d0/lambdacap**2))  
         alphasg=1d0/(b0*dlog(qs/lambdacap**2*2d0))  
         alphasg0=1d0/(b0*dlog(1d0/lambdacap**2)) 
     
         a28n=a28*(alphasg0/alphasg)**(gamqq(2)/b0/4d0/pi)  ! evolution of meson wfs
         xcap2=(alphasg0/alphasg)**(gamp(2)/b0/4d0/pi)
         ycap2=(alphasg0/alphasg)**(gamm(2)/b0/4d0/pi)
      
         bm2=(a21-a2g/rhop(2))/(rhom(2)-1d0/rhop(2))
         bp2=(a21/rhom(2)-a2g)/(1d0/rhom(2)-rhop(2))
      
         a21n=bp2*xcap2+rhom(2)*bm2*ycap2
         a2gn=rhop(2)*bp2*xcap2+bm2*ycap2   

      if(mx.lt.2d0)then
         print*,'warning, m_x region too low (< 2 gev) for pqcd calculat
     &ion to be reliable'
         goto 889
      endif

      call lumpijz0(mx,w0)    ! j_z=0 luminosity
      call lumpijz2(mx,w2)    ! |j_z|=2 luminosity

      if(xflag.eq.'pi'.or.xflag.eq.'ks')then
      cpi=cpi*(1d0+a28n)**2/(1d0+a28)**2
      weight=weight*(cpi)**2*w2/4d0
      elseif(xflag.eq.'pipm'.or.xflag.eq.'kpkm')then
      cpi=cpi*(1d0+a28n)**2/(1d0+a28)**2
      weight=weight*(cpi)**2*w2/4d0
      weight=weight*2d0   ! particles not identical in final state
      elseif(xflag.eq.'rho')then
      cpi=cpi*(1d0+a28n)**2/(1d0+a28)**2
      vpm=vpm*(1d0+a28n)**2/(1d0+a28)**2
      vmp=vmp*(1d0+a28n)**2/(1d0+a28)**2
      weight=weight*((vpm**2+vmp**2+cpi**2)*w2)/4d0
      elseif(xflag.eq.'eta')then
      cpi=cpi*(1d0+a28n)**2/(1d0+a28)**2
      clpp=clpp*(1d0+a21n)**2/(1d0+a21)**2
      clpm=clpm*(1d0+a21n)**2/(1d0+a21)**2
      eppg=dsin(thetap1)**2*(1d0+1d0*a2gn*5d0/9d0/(1d0+a21n) ! cz
     &+25d0*a2gn**2/108d0/(1d0+a21n)**2/3d0)*fpi1**2*clpp
      epp=eppg
      epm=(fpi8**2*dcos(thetap8)**2+fpi1**2*dsin(thetap1)**2)*cpi
     &+dsin(thetap1)**2*clpm*fpi1**2
      weight=weight*((2d0*epp)**2*w0+epm**2*w2)/4d0
      sv=((2d0*epp)**2*w0*svj0+epm**2*w2*svj2)/
     &((2d0*epp)**2*w0+epm**2*w2)
      weight=weight/fpi**4
      elseif(xflag.eq.'etap')then
      cpi=cpi*(1d0+a28n)**2/(1d0+a28)**2
      clpp=clpp*(1d0+a21n)**2/(1d0+a21)**2
      clpm=clpm*(1d0+a21n)**2/(1d0+a21)**2
      peppg=dcos(thetap1)**2*(1d0+1d0*a2gn*5d0/9d0/(1d0+a21n)  ! cz 
     &+25d0*a2gn**2/108d0/(1d0+a21n)**2/3d0)*fpi1**2*clpp
      pepp=peppg
      pepm=(fpi8**2*dcos(thetap8)**2+fpi1**2*dsin(theta1)**2)*cpi
     &+dcos(thetap1)**2*fpi1**2*clpm
      weight=weight*((2d0*pepp)**2*w0+pepm**2*w2)/4d0
      sv=((2d0*pepp)**2*w0*svj0+pepm**2*w2*svj2)/
     &((2d0*pepp)**2*w0+pepm**2*w2)
      weight=weight/fpi**4
      elseif(xflag.eq.'etaetap')then
      clpp=clpp*(1d0+a21n)**2/(1d0+a21)**2
      clpm=clpm*(1d0+a21n)**2/(1d0+a21)**2
      meppg=-dcos(thetap1)*dsin(thetap1)*(1d0+1d0*a2gn*5d0/9d0/   ! cz
     &(1d0+a21n)+25d0*a2gn**2/108d0/(1d0+a21n)**2/3d0)*fpi1**2*clpp
      mepp=meppg
      mepm=-dcos(thetap1)*dsin(thetap1)*clpm*fpi1**2
      weight=weight*((2d0*mepp)**2*w0+mepm**2*w2)/4d0
      sv=((2d0*mepp)**2*w0*svj0+mepm**2*w2*svj2)/
     &((2d0*mepp)**2*w0+mepm**2*w2)
      weight=weight*2d0   ! particles not identical in final state
      weight=weight/fpi**4
      endif     

      weight=weight/(0.36d0/alphasp)**4  ! include running alphas

      weight=weight/mx*2d0*b**2
      weight=weight*fpip**4
      weight=weight/(16d0*pi)
      weight=weight/mx**6
      weight=weight/2d0   ! symmetry factor

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


      elseif(xflag.eq.'chic'.or.xflag.eq.'chib')then

         if(chiflag.eq.0)then

c     gg --> chi(0++) vertex
            
            if(ii.gt.i)then
               ii=1
            else
               ii=0
            endif

            if(gen.eq.'exact')then
               
               if(surv.eq.'on')then
                  
               call schimc(chiflag,echi1,echi2,pt1x,pt1y,pt2x,pt2y,zamp) ! survival effects
                  
                  weight=weight*cdabs(zamp)**2

               else
                  
                  af=1               
                  call interp(grid,pt1x,pt1y,pt2x,pt2y,amp)
                  weight=weight*amp**2
                  
               endif
               
            endif
            
c     gg --> chi(1++) vertex
            
         elseif(chiflag.eq.1)then
            
               do k=1,3
                  do j=1,3

                     do h=1,4

                     if(k.eq.1)then
                        cechi1(h)=echi0(h)   
                        echi1(h)=echi0(h)              
                     elseif(k.eq.2)then  
                        cechi1(h)=conjg(echiplus(h))
                        echi1(h)=echiplus(h)
                     else                          
                        cechi1(h)=conjg(echiminus(h))
                        echi1(h)=echiminus(h)
                     endif

                     if(j.eq.1)then
                        echi1p(h)=echi0(h)               
                     elseif(j.eq.2)then   
                        echi1p(h)=echiplus(h)
                     else                          
                        echi1p(h)=echiminus(h)
                     endif
                   
                     enddo
                     
                     if(gen.eq.'fit')then

c     chi density matrix (unnormalised)

                     rho1chi(k,j)=(cechi1(1)*ptdif(2)-cechi1(2)*ptdif(1)
     &                       )*(echi1p(1)*ptdif(2)-echi1p(2)*ptdif(1))
                        
                     elseif(gen.eq.'exact')then
                       
                        if(surv.eq.'on')then

                        if(ii.gt.i)then
                           ii=1
                        else
                           ii=0
                        endif
 
                           if(k.eq.j)then
                       
             call schimc(chiflag,cechi1,echi2,pt1x,pt1y,pt2x,pt2y,zamp)
             call schimc(chiflag,echi1p,echi2,pt1x,pt1y,pt2x,pt2y,zamp1)

                           rho1chi(k,j)=zamp*zamp1
              
                           else

                              rho1chi(k,j)=0d0    ! omits azimuthal correlations for quicker runtime

c             call schimc(chiflag,cechi1,echi2,pt1x,pt1y,pt2x,pt2y,zamp)
c             call schimc(chiflag,echi1p,echi2,pt1x,pt1y,pt2x,pt2y,zamp1)

c                           rho1chi(k,j)=zamp*zamp1

                           endif

                           else
             
                           do af=1,2
                              
               call interp(grid,pt1x,pt1y,pt2x,pt2y,amp)
                              
                              if(af.eq.1)then
                                 amp1=amp
                              elseif(af.eq.2)then
                                 amp2=amp
                              elseif(af.eq.3)then
                                 amp3=amp
                              endif
                              
                        enddo
       
                       rho1chi(k,j)=((cechi1(1))*amp1-(cechi1(2))*amp2)*
     &                       ((echi1p(1))*amp1-(echi1p(2))*amp2)
                        
                     endif
                  
               endif
               
            enddo
         enddo

         if(corr.eq.'off')then
         weight=weight*(rho1chi(1,1)+rho1chi(2,2)+rho1chi(3,3))
         endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     gg --> chi(2++) vertex

            elseif(chiflag.eq.2)then
               
               do l=1,4
                  do k=1,4
         echi2p2(l,k)=echiplus(l)*echiplus(k)
         echi2p1(l,k)=(echiplus(l)*echi0(k)+echi0(l)*echiplus(k))/rt2
         echi20(l,k)=(echiplus(l)*echiminus(k)+2d0*echi0(l)*echi0(k)
     &   +echiminus(l)*echiplus(k))/dsqrt(6d0)
         echi2m1(l,k)=(echiminus(l)*echi0(k)+echi0(l)*echiminus(k))/rt2
         echi2m2(l,k)=echiminus(l)*echiminus(k)
                  enddo
               enddo
               
               do k=1,5
                  do j=1,5
                     do h=1,4
                        do l=1,4

                           if(k.eq.1)then
                              cechi2(h,l)=conjg(echi2m2(h,l))                 
                           elseif(k.eq.2)then  
                              cechi2(h,l)=conjg(echi2m1(h,l))  
                           elseif(k.eq.3)then  
                              cechi2(h,l)=conjg(echi20(h,l))
                           elseif(k.eq.4)then  
                              cechi2(h,l)=conjg(echi2p1(h,l))  
                           else  
                              cechi2(h,l)=conjg(echi2p2(h,l))    
                           endif

                           if(j.eq.1)then
                              echi2p(h,l)=echi2m2(h,l)                
                           elseif(j.eq.2)then  
                              echi2p(h,l)=echi2m1(h,l)
                           elseif(j.eq.3)then  
                              echi2p(h,l)=echi20(h,l)
                           elseif(j.eq.4)then  
                              echi2p(h,l)=echi2p1(h,l)
                           else  
                              echi2p(h,l)=echi2p2(h,l)   
                           endif

                        enddo
                     enddo

                     if(gen.eq.'fit')then

cccc  form 1   

           if(chiv.eq.1)then

        rho2chi(k,j)=2d0*((s*(cechi2(1,1)*ptx(1)*ptx(1)
     &  +cechi2(1,2)*ptx(1)*ptx(2)+cechi2(2,1)*ptx(2)*ptx(1)
     &  +cechi2(2,2)*ptx(2)*ptx(2))-2d0*(ptx(1)*ptx(1)+ptx(2)*ptx(2))
     &  *(cechi2(3,3)*q(3,1)*q(3,2)-cechi2(3,4)*q(3,1)*q(4,2)
     &  -cechi2(4,3)*q(4,1)*q(3,2)+cechi2(4,4)*q(4,1)*q(4,2))))/s**2
                     
        rho2chi(k,j)=rho2chi(k,j)*((s*(echi2p(1,1)*ptx(1)*ptx(1)
     &  +echi2p(1,2)*ptx(1)*ptx(2)+echi2p(2,1)*ptx(2)*ptx(1)
     &  +echi2p(2,2)*ptx(2)*ptx(2))-2d0*(ptx(1)*ptx(1)+ptx(2)*ptx(2))
     &  *(echi2p(3,3)*q(3,1)*q(3,2)-echi2p(3,4)*q(3,1)*q(4,2)
     &  -echi2p(4,3)*q(4,1)*q(3,2)+echi2p(4,4)*q(4,1)*q(4,2))))

ccc   replacement- form 2
            
           elseif(chiv.eq.2)then

        rho2chi(k,j)=2d0*((s*(cechi2(1,1)*pt1(1)*pt2(1)
     &  +cechi2(1,2)*pt1(1)*pt2(2)+cechi2(2,1)*pt1(2)*pt2(1)
     &  +cechi2(2,2)*pt1(2)*pt2(2))-2d0*(pt1(1)*pt2(1)+pt1(2)*pt2(2))
     &  *(cechi2(3,3)*q(3,1)*q(3,2)-cechi2(3,4)*q(3,1)*q(4,2)
     &  -cechi2(4,3)*q(4,1)*q(3,2)+cechi2(4,4)*q(4,1)*q(4,2))))/s**2
                     
        rho2chi(k,j)=rho2chi(k,j)*((s*(echi2p(1,1)*pt1(1)*pt2(1)
     &  +echi2p(1,2)*pt1(1)*pt2(2)+echi2p(2,1)*pt1(2)*pt2(1)
     &  +echi2p(2,2)*pt1(2)*pt2(2))-2d0*(pt1(1)*pt2(1)+pt1(2)*pt2(2))
     &  *(echi2p(3,3)*q(3,1)*q(3,2)-echi2p(3,4)*q(3,1)*q(4,2)
     &  -echi2p(4,3)*q(4,1)*q(3,2)+echi2p(4,4)*q(4,1)*q(4,2))))

            endif

      elseif(gen.eq.'exact')then
         
         if(surv.eq.'on')then
            
            if(ii.gt.i)then
               ii=1
            else
               ii=0
            endif
            
            if(k.eq.j)then
               
             call schimc(chiflag,echi1,cechi2,pt1x,pt1y,pt2x,pt2y,zamp)
             call schimc(chiflag,echi1,echi2p,pt1x,pt1y,pt2x,pt2y,zamp1)
               
               rho2chi(k,j)=zamp*zamp1
               
            else
               
               rho2chi(k,j)=0d0
               
c     call schimc(chiflag,echi1,cechi2,pt1x,pt1y,pt2x,pt2y,zamp)
c     call schimc(chiflag,echi1,echi2p,pt1x,pt1y,pt2x,pt2y,zamp1)
               
c     rho2chi(k,j)=zamp*zamp1
               
            endif
            
            else

         do af=1,3
            
               call interp(grid,pt1x,pt1y,pt2x,pt2y,amp)

            if(af.eq.1)then
               amp1=amp
            elseif(af.eq.2)then
               amp2=amp
            elseif(af.eq.3)then
               amp3=amp
            endif
            
         enddo
         
         rho2chi(k,j)=(dsqrt(2d0)*(cechi2(1,1)*amp2+(cechi2(2,1))*amp1
     &        +cechi2(2,2)*amp3-
     &        2d0*(amp2+amp3)*(q(3,1)*q(3,2)*cechi2(3,3)
     &        +q(4,1)*q(4,2)*cechi2(4,4)-q(4,1)*q(3,2)*cechi2(4,3)
     &        -q(3,1)*q(4,2)*cechi2(3,4))/s))
         
         rho2chi(k,j)=rho2chi(k,j)*(dsqrt(2d0)*(echi2p(1,1)*amp2+
     &        (echi2p(2,1))*amp1+echi2p(2,2)*amp3-
     &        2d0*(amp2+amp3)*(q(3,1)*q(3,2)*echi2p(3,3)
     &        +q(4,1)*q(4,2)*echi2p(4,4)-q(4,1)*q(3,2)*echi2p(4,3)
     &        -q(3,1)*q(4,2)*echi2p(3,4))/s))
         
      endif 
      endif
      
      enddo
      enddo

      if(corr.eq.'off')then
      weight=weight*(rho2chi(1,1)+rho2chi(2,2)+rho2chi(3,3)
     &     +rho2chi(4,4)+rho2chi(5,5))
      endif
      
            if(chiv.eq.2)then
               weight=weight*0.0439d0/0.00542d0
            endif

            endif
      
      elseif(xflag.eq.'etac'.or.xflag.eq.'etab')then

            if(gen.eq.'exact')then

               if(surv.eq.'on')then

                  if(ii.gt.i)then
                     ii=1
                  else
                     ii=0
                  endif

                  chiflag=0
                 
               call schimc(chiflag,echi1,echi2,pt1x,pt1y,pt2x,pt2y,zamp) ! survival effects
                
                  weight=weight*cdabs(zamp)**2

               else
                  
                  af=1
                  
                  call interp(grid,pt1x,pt1y,pt2x,pt2y,amp)
                 
                  weight=weight*amp**2

               endif

               elseif(gen.eq.'fit')then

         weight=weight*(pt1(1)*pt2(2)-pt1(2)*pt2(1))**2

                  endif

               endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      t1=-rts*p1m*rt2
      t2=-rts*p2p*rt2

ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     vertex factor

      fnt=dexp((bb*t1+bb*t2))

c     energy dependence for chi_c/b
      
      fnx=(x1*x2/xoo**2)**(-2d0*delta)

c     matrix element rapidity dependence (phenomenological fit)
c     note: fny=1 by definition for y(chic)=0

      fny=((1d0-x1)*(1d0-x2)/(1d0-xo)**2)**ng


ccccccccccc

c     more precise rapidity dependence - currently only implemented for 7,8,14 tev

      if(xflag.eq.'jpsi'.or.xflag.eq.'ups'.or.xflag.eq.'psip')then
      else
         if(eflagp.eq.8)then
            call yint8(fn)
            fny=fn
         elseif(eflag.eq.7)then
            call yint7(fn)
            fny=fn
         elseif(eflag.eq.14)then
            call yint14(fn)
            fny=fn
         endif
      endif

      if(eflagp.eq.8.and.cross.eq.1)then
         fny=fn*(8d0/7d0)**(-4d0*delta)
      endif

c     enhanced rescattering effects, s_{enh}^2. nb: value subject to change

      if(xflag.eq.'chic'.or.xflag.eq.'etac'.or.
     &     xflag.eq.'chib'.or.xflag.eq.'etab')then

         if(chiflag.eq.0)then
            senh=senh0
         elseif(chiflag.eq.1)then
            senh=senh1
         elseif(chiflag.eq.2)then
            senh=senh2
         endif

      elseif(xflag.eq.'gam'.or.xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag
     &.eq.'eta'.or.xflag.eq.'etap'.or.xflag.eq.'etaetap'.or
     &.xflag.eq.'pipm'.or.xflag.eq.'kpkm'.or.xflag.eq.'ks')then

         if(eflag.eq.2)then
            if(mgam.lt.10d0)then
               senh=0.328d0*mgam**0.303d0        ! 1.96 tev
            else
               senh=0.285d0*mgam**0.365d0
            endif
         elseif(eflag.eq.7)then
            if(mgam.lt.10d0)then
               senh=0.2d0*mgam**(0.321d0)        ! 7  tev
            else
               senh=0.171d0*mgam**(0.389d0)              
            endif
         elseif(eflag.eq.10)then
            if(mgam.lt.10d0)then
              senh=0.176d0*mgam**(0.31d0)        ! 10 tev
            else
               senh=0.148d0*mgam**(0.385d0)
            endif
         elseif(eflag.eq.14)then
            if(mgam.lt.10d0)then
               senh=0.161d0*mgam**(0.284d0)      ! 14 tev
            else
               senh=0.122d0*mgam**(0.403d0)            
            endif
         elseif(eflag.eq.500)then
            if(mx.lt.10d0)then
               senh=0.475d0*mx**0.303d0          ! 0.5 tev
            else
               senh=0.413d0*mx**0.365d0
            endif  
         elseif(eflag.eq.300)then
            if(mx.lt.10d0)then
               senh=0.545d0*mx**0.303d0          ! 0.3 tev
            else
               senh=0.474d0*mx**0.365d0
            endif  
         elseif(eflag.eq.900)then
            if(mx.lt.10d0)then
               senh=0.406d0*mx**0.303d0          ! 0.9 tev
            else
               senh=0.353d0*mx**0.365d0
            endif  
          endif
       endif

      if(senh.gt.1d0)then   ! fit only good for lower masses
         senh=1d0
      endif

      if(gen.eq.'exact'.and.surv.eq.'on')then
      fnt=1d0
      endif
      
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      if(xflag.eq.'chic'.or.xflag.eq.'chib')then

         if(decay.eq.1)then
         weight=weight*fnt*bare*norm*fnx*fny*sv
     &        *senh*brchipsi*brpsimu
         else
         weight=weight*fnt*bare*norm*fnx*fny*sv*senh*brchihad
         endif

      elseif(xflag.eq.'etac'.or.xflag.eq.'etab')then
         weight=weight*fnt*bare*norm*fnx*fny*neff*sv
     &        *senh*bretahad       
      elseif(xflag.eq.'gam')then
         weight=weight*fnt*weightgam*fny*senh*sv*389379d6*weightnf
      elseif(xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag.eq.'eta'
     &.or.xflag.eq.'etap'.or.xflag.eq.'etaetap'.or.xflag.eq.'pipm'.or.
     &xflag.eq.'ks'.or.xflag.eq.'kpkm')then
         weight=weight*fnt*weightgam*fny*senh*sv*389379d3*weightnfpi
      endif
      
      if(corr.eq.'off')goto 808

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     j/psi (upsilon) decay --> l^+ l^-
c     default assumes schc, but can psi(ups) density matrix
c     'rho1psi(k,j)' can be edited for general case

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      if(xflag.eq.'jpsi'.or.xflag.eq.'ups'.or.xflag.eq.'psip')then

         if(phdecay.eq.'mu')then

            if(ranpsi.gt.0.5d0)then
               do j=1,3
                  pboo(j)=-q(j,15)-q(j,2)
               enddo
               pboo(4)=q(4,15)+q(4,2)
            else
               do j=1,3
                  pboo(j)=-q(j,15)-q(j,1)
               enddo
               pboo(4)=q(4,15)+q(4,1)              
            endif

            sh=dsqrt(pboo(4)**2-pboo(3)**2-pboo(2)**2-pboo(1)**2)

            do j=1,4
               pcm(j)=q(j,5)
            enddo      
            call boost(sh,pboo,pcm,plb)  
            do j=1,4
               q(j,16)=plb(j)
            enddo

            do j=1,4
               pcm(j)=q(j,8)
            enddo           
            call boost(sh,pboo,pcm,plb)  
            do j=1,4
               q(j,18)=plb(j)
            enddo

c     real transverse basis vectors
            
            n1(1)=1d0
            n1(2)=0d0
            n1(3)=0d0
            n1(4)=0d0
            
            n2(1)=0d0
            n2(2)=1d0
            n2(3)=0d0
            n2(4)=0d0

            n3(1)=0d0
            n3(2)=0d0
            n3(3)=1d0
            n3(4)=0d0
            
c     ...and rotate to lab basis
           
            call rotate(n3,16,e0rot)

      x(1)=-(q(2,1)*q(3,16)-q(3,1)*q(2,16))
      x(2)=-(q(3,1)*q(1,16)-q(1,1)*q(3,16))
      x(3)=-(q(1,1)*q(2,16)-q(2,1)*q(1,16))
      
      xnorm=dsqrt(x(1)**2+x(2)**2+x(3)**2)

      do j=1,3
         n1rot(j)=x(j)/xnorm
      enddo
      n1rot(4)=0d0

      n2rot(1)=-(n1rot(2)*q(3,16)-n1rot(3)*q(2,16))
      n2rot(2)=-(n1rot(3)*q(1,16)-n1rot(1)*q(3,16))
      n2rot(3)=-(n1rot(1)*q(2,16)-n1rot(2)*q(1,16))
      n2rot(4)=0d0

      fnorm=dsqrt(n2rot(1)**2+n2rot(2)**2+n2rot(3)**2)
      do j=1,4
         n2rot(j)=n2rot(j)/fnorm
      enddo

c     generate polarization vectors
            
      do j=1,4
         epsiplus(j)=-(n1rot(j)+zi*n2rot(j))/rt2
         epsiminus(j)=(n1rot(j)-zi*n2rot(j))/rt2
         epsi0(j)=e0rot(j)
      enddo
        
c     decay 7(mpsi) --> 8(mmu) + 9(mmu)

      do j=1,3
         pboo(j)=-q(j,5)
      enddo
      pboo(4)=q(4,5)

      do j=1,4
         pcm(j)=q(j,8)
      enddo

      call boost(mx,pboo,pcm,plb)  

      do k=1,4
         q8(k)=plb(k)
         zq8(k)=plb(k)
      enddo

c     calculate (polarization dependent) weights for decay

      q8q9=(mx**2-2d0*mmu**2)/2d0
      weight1=0d0

cccc  psi density matrix:  1=long.
cccc                       2,3=transv.
cccc  default given by schc

      do k=1,3
         do j=1,3
            rho1psi(k,j)=0d0
         enddo
      enddo

      rho1psi(1,1)=0d0
      rho1psi(2,2)=(1d0-rho1psi(1,1))/2d0
      rho1psi(3,3)=rho1psi(2,2)
c      rho1psi(2,3)=0d0
c      rho1psi(3,2)=0d0

cccc

      do j=1,3
         do k=1,3
            do h=1,4
               
               if(k.eq.1)then
                  epsi1(h)=epsi0(h)              
               elseif(k.eq.2)then  
                  epsi1(h)=epsiplus(h)
               else                          
                  epsi1(h)=epsiminus(h)
               endif
               if(j.eq.1)then
                  cepsi1p(h)=epsi0(h)                 
               elseif(j.eq.2)then   
                  cepsi1p(h)=conjg(epsiplus(h))
               else                          
                  cepsi1p(h)=conjg(epsiminus(h))
               endif

               enddo

               call cdot(epsi1,zq8,zq8epsi1)
               call cdot(cepsi1p,zq8,zq8cepsi1p)
               call cdot(epsi1,cepsi1p,zepsi1cepsi1p)

            weight1=weight1+(-(q8q9+mmu**2)*zepsi1cepsi1p-2d0*zq8epsi1*
     &zq8cepsi1p)*rho1psi(k,j)/(2d0*(q8q9+2d0*mmu**2))*3d0

         enddo
      enddo

      weight=weight*weight1

      do j=1,3
         pboo(j)=-q(j,5)
      enddo
      pboo(4)=q(4,5)

      do j=1,4
         pcm(j)=q(j,8)
      enddo

      call boost(mx,pboo,pcm,plb)  

      cost=(plb(1)*q(1,16)+plb(2)*q(2,16)+plb(3)*q(3,16))/
     &dsqrt((plb(1)**2+plb(2)**2+plb(3)**2)*
     &(q(1,16)**2+q(2,16)**2+q(3,16)**2))

      do j=1,4
         pcm(j)=q(j,9)
      enddo

      call boost(mx,pboo,pcm,plb)  

      x(1)=-(q(2,1)*q(3,16)-q(3,1)*q(2,16))
      x(2)=-(q(3,1)*q(1,16)-q(1,1)*q(3,16))
      x(3)=-(q(1,1)*q(2,16)-q(2,1)*q(1,16))
      
      xnorm=dsqrt(x(1)**2+x(2)**2+x(3)**2)
      
      y(1)=-(q(2,18)*q(3,16)-q(3,18)*q(2,16))
      y(2)=-(q(3,18)*q(1,16)-q(1,18)*q(3,16))
      y(3)=-(q(1,18)*q(2,16)-q(2,18)*q(1,16))          
      
      ynorm=dsqrt(y(1)**2+y(2)**2+y(3)**2)
      
      phi=dacos((x(1)*y(1)+x(2)*y(2)+x(3)*y(3))/(xnorm*ynorm))

      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     chi_(c/b) decays-
c     calculates (chi and psi polarization dependent) weight for decay   
c     (assuming minimal lorentz invariant amplitudes, and e1 dominant
c     radiative transitions)

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      elseif(xflag.eq.'chic'.or.xflag.eq.'chib')then

         if(decay.eq.1)then

cccccccccccccccccccccccccccccccccccccccccccccccc

c     decay 5(mchi) --> 6(0) + 7(mpsi)

cccccccccccccccccccccccccccccccccccccccccccccccccc

c     assign j/psi polarization vectors...

      e0(1)=0d0
      e0(2)=0d0
      e0(3)=q(4,7)/mpsi
      e0(4)=dsqrt(q(4,7)**2-mpsi**2)/mpsi

      call rotate(e0,7,e0rot)

      n1rot(1)=-(q(2,1)*q(3,7)-q(3,1)*q(2,7))
      n1rot(2)=-(q(3,1)*q(1,7)-q(1,1)*q(3,7))
      n1rot(3)=-(q(1,1)*q(2,7)-q(2,1)*q(1,7))
      
      fnorm=dsqrt(n1rot(1)**2+n1rot(2)**2+n1rot(3)**2)

      do j=1,3
         n1rot(j)=n1rot(j)/fnorm
      enddo  
      n1rot(4)=0d0

      n2rot(1)=-(n1rot(2)*q(3,7)-n1rot(3)*q(2,7))
      n2rot(2)=-(n1rot(3)*q(1,7)-n1rot(1)*q(3,7))
      n2rot(3)=-(n1rot(1)*q(2,7)-n1rot(2)*q(1,7))
      n2rot(4)=0d0

      fnorm=dsqrt(n2rot(1)**2+n2rot(2)**2+n2rot(3)**2)

      do j=1,4
         n2rot(j)=n2rot(j)/fnorm
      enddo

      do j=1,4
         epsiplus(j)=-(n1rot(j)+zi*n2rot(j))/rt2
         epsiminus(j)=(n1rot(j)-zi*n2rot(j))/rt2
         epsi0(j)=e0rot(j)
      enddo

c     various vectors...
     
      do j=1,4
         q5(j)=q(j,5)
         q6(j)=q(j,6)
         q7(j)=q(j,7)
         zq5(j)=q(j,5)
         zq6(j)=q(j,6)
         zq7(j)=q(j,7)
      enddo

ccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     chi0

cccccccccccccccccccccccccccccccccccccccccccccccccccccc

      if(chiflag.eq.0)then 

         do k=1,3
            do j=1,3
               
               do h=1,4
                  
                  if(k.eq.1)then
                     cepsi1(h)=epsi0(h)                 
                  elseif(k.eq.2)then  
                     cepsi1(h)=conjg(epsiplus(h))
                  else                          
                     cepsi1(h)=conjg(epsiminus(h))
                  endif
                  
                  if(j.eq.1)then
                     epsi1p(h)=epsi0(h)             
                  elseif(j.eq.2)then   
                     epsi1p(h)=epsiplus(h)
                  else                          
                     epsi1p(h)=epsiminus(h)
                  endif
                  
               enddo

               call cdot(zq6,epsi1p,zq6epsi1p)              
               call cdot(zq7,epsi1p,zq7epsi1p)
               call cdot(zq6,cepsi1,zq6cepsi1)
               call cdot(epsi1p,cepsi1,zepsi1pcepsi1)
               call dot(q6,q7,q6q7)


c     j/psi density matrix (unnormalised)
               
               rho1psi(k,j)=(-zepsi1pcepsi1*q6q7**2-mpsi**2
     &              *zq6epsi1p*zq6cepsi1)/(2d0*q6q7**2)
               
            enddo
         enddo

ccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     chi1

cccccccccccccccccccccccccccccccccccccccccccccccccccc
      
      elseif(chiflag.eq.1)then

         call dot(q6,q7,q6q7)

         pnorm=2d0*q6q7**2*(mchi**2+mpsi**2)/(mchi**2*mpsi**2)/3d0
         weight1=0d0
         zweight1=(0d0,0d0)
         
         do k=1,3
            do j=1,3
               
               do h=1,4
                  
                  if(k.eq.1)then
                     cepsi1(h)=epsi0(h)                 
                  elseif(k.eq.2)then  
                     cepsi1(h)=conjg(epsiplus(h))
                  else                          
                     cepsi1(h)=conjg(epsiminus(h))
                  endif
                  
                  if(j.eq.1)then
                     epsi1p(h)=epsi0(h)             
                  elseif(j.eq.2)then   
                     epsi1p(h)=epsiplus(h)
                  else                          
                     epsi1p(h)=epsiminus(h)
                  endif
                  
               enddo
               
               call cdot(zq6,epsi1p,zq6epsi1p)
               call cdot(zq6,cepsi1,zq6cepsi1)
               call cdot(zq5,epsi1p,zq5epsi1p)
               
               rho1psi(k,j)=0d0
               
               do l=1,3
                  do h=1,3
                     
                     do m=1,4
                        
                        if(l.eq.1)then
                           echi1(m)=echi0(m)                 
                        elseif(l.eq.2)then  
                           echi1(m)=echiplus(m)
                        else                          
                           echi1(m)=echiminus(m)
                        endif
                        
                        if(h.eq.1)then
                           cechi1p(m)=echi0(m)             
                        elseif(h.eq.2)then   
                           cechi1p(m)=conjg(echiplus(m))
                        else                          
                           cechi1p(m)=conjg(echiminus(m))
                        endif
              
                     enddo
                     
                     call cdot(echi1,epsi1p,zechi1epsi1p)
                     call cdot(cechi1p,cepsi1,zcechi1pcepsi1)
                     call cdot(epsi1p,cepsi1,zepsi1pcepsi1)
                     call cdot(cechi1p,echi1,zcechi1pechi1)
                     call cdot(zq6,cechi1p,zq6cechi1p)
                     call cdot(zq6,echi1,zq6echi1)

       rho1psi(k,j)=rho1psi(k,j)+(-zq6echi1*zq6cechi1p*zepsi1pcepsi1-
     &   zcechi1pechi1*zq6cepsi1*zq6epsi1p+zq6cechi1p*zechi1epsi1p
     &   *zq6cepsi1+zq6echi1*zcechi1pcepsi1*zq6epsi1p)*rho1chi(l,h)
                     
                  enddo
               enddo
               
            enddo
         enddo

      weight=weight/pnorm

cccccccccccccccccccccccccccccccccccccccccccccccccccc

c     chi2

cccccccccccccccccccccccccccccccccccccccccccccccccccc

      elseif(chiflag.eq.2)then

c     initialise values

       weight2=0d0
       pnorm=0d0

c     metric tensor
        
       do j=1,4
         do k=1,4
            g(j,k)=0d0
         enddo
       enddo

      g(1,1)=-1d0
      g(2,2)=-1d0
      g(3,3)=-1d0
      g(4,4)=1d0

c     generate contravariant 4-vectors

       q6c(4)=q(4,6)
       q7c(4)=q(4,7)
       do j=1,3
       q6c(j)=-q(j,6)
       q7c(j)=-q(j,7)
       enddo
       call dot(q6,q7,q6q7)

c     normalisation

      do k=1,3
         do j=1,3
            do h=1,4

               if(k.eq.1)then
                  cepsi1(h)=epsi0(h)                 
               elseif(k.eq.2)then  
                  cepsi1(h)=conjg(epsiplus(h))
               else                          
                  cepsi1(h)=conjg(epsiminus(h))
               endif
               
               if(j.eq.1)then
                  epsi1p(h)=epsi0(h)             
               elseif(j.eq.2)then   
                  epsi1p(h)=epsiplus(h)
               else                          
                  epsi1p(h)=epsiminus(h)
               endif
               
            enddo
            
            epsi1c(4)=epsi1p(4)
            cepsi1c(4)=cepsi1(4)
            
            do m=1,3
               epsi1c(m)=-epsi1p(m)
               cepsi1c(m)=-cepsi1(m)
            enddo
            
            call cdot(zq6,cepsi1,zq6cepsi1)
            call cdot(zq6,epsi1p,zq6epsi1p)
            
            rho1psi(k,j)=0d0
            
            do m=1,5
               do n=1,5
                  do h=1,4
                     do l=1,4
                        
                        if(m.eq.1)then
                           echi2(h,l)=echi2m2(h,l)                
                        elseif(m.eq.2)then  
                           echi2(h,l)=echi2m1(h,l)
                        elseif(m.eq.3)then  
                           echi2(h,l)=echi20(h,l)
                        elseif(m.eq.4)then  
                           echi2(h,l)=echi2p1(h,l)
                        else  
                           echi2(h,l)=echi2p2(h,l) 
                        endif
                        
                        if(n.eq.1)then
                           cechi2p(h,l)=conjg(echi2m2(h,l))                 
                        elseif(n.eq.2)then  
                           cechi2p(h,l)=conjg(echi2m1(h,l))  
                        elseif(n.eq.3)then  
                           cechi2p(h,l)=conjg(echi20(h,l))
                        elseif(n.eq.4)then  
                           cechi2p(h,l)=conjg(echi2p1(h,l))  
                        else  
                           cechi2p(h,l)=conjg(echi2p2(h,l))    
                        endif
                        
                     enddo
                  enddo
                  
                  do o=1,4
                     do p=1,4
                        do d=1,4
                           do r=1,4
                              
          rho1psi(k,j)=rho1psi(k,j)+(echi2(r,d)*
     &    cechi2p(p,o)*q6c(r)*(q6q7*epsi1c(o)-zq6epsi1p*q7c(o)
     &    )*(cepsi1c(d)*q7c(p)-cepsi1c(p)*q7c(d))+
     &    cechi2p(r,d)*echi2(p,o)*q6c(r)*(q6q7*cepsi1c(o)-zq6cepsi1
     &    *q7c(o))*(epsi1c(d)*q7c(p)-epsi1c(p)*q7c(d))
     &    -echi2(r,d)*cechi2p(p,o)*(q6c(p)*q6c(r)*(mpsi**2*epsi1c(o)
     &    *cepsi1c(d)-q7c(d)*q7c(o))+g(r,p)*(q6q7*epsi1c(o)-
     &    zq6epsi1p*q7c(o))*(q6q7*cepsi1c(d)
     &    -zq6cepsi1*q7c(d))))*rho2chi(m,n)
                              
                           enddo
                        enddo
                     enddo
                  enddo
      
               enddo
            enddo
            
         enddo
      enddo
         
         pnorm=10d0*(1d0-q6q7/mchi**2)*q6q7**2/15d0
         weight=weight/pnorm
         
      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc     

c     decay 7(mpsi) --> 8(mmu) + 9(mmu)

      do k=1,4
      q8(k)=q(k,8)
      zq8(k)=q(k,8)
      q9(k)=q(k,9)
      enddo

c     calculate (polarization dependent) weights for decay

      call dot(q8,q9,q8q9)

      weight1=0d0
      zweight1=(0d0,0d0)
      
      do j=1,3
         do k=1,3
            do h=1,4
               
               if(k.eq.1)then
                  epsi1(h)=epsi0(h)              
               elseif(k.eq.2)then  
                  epsi1(h)=epsiplus(h)
               else                          
                  epsi1(h)=epsiminus(h)
               endif

               if(j.eq.1)then
                  cepsi1p(h)=epsi0(h)                 
               elseif(j.eq.2)then   
                  cepsi1p(h)=conjg(epsiplus(h))
               else                          
                  cepsi1p(h)=conjg(epsiminus(h))
               endif

               enddo

               call cdot(epsi1,zq8,zq8epsi1)
               call cdot(cepsi1p,zq8,zq8cepsi1p)
               call cdot(epsi1,cepsi1p,zepsi1cepsi1p)

            weight1=weight1+(-(q8q9+mmu**2)*zepsi1cepsi1p-2d0*zq8epsi1*
     &zq8cepsi1p)*rho1psi(k,j)/(2d0*(q8q9+2d0*mmu**2))*3d0

         enddo
      enddo

      weight=weight*weight1

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      elseif(decay.eq.2)then

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c -- decay 5(mx) --> 14(mh) + 15(mh)   (fermions)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

         if(chiflag.eq.1)then

            weight1=0d0

            do k=1,4
               q14(k)=q(k,14)
               zq14(k)=q(k,14)
               q15(k)=q(k,15)
               zq15(k)=q(k,15)
            enddo

            do l=1,3
               do h=1,3
                  do m=1,4

                     if(l.eq.1)then
                        echi1(m)=echi0(m)                 
                     elseif(l.eq.2)then  
                        echi1(m)=echiplus(m)
                     else                          
                        echi1(m)=echiminus(m)
                     endif
              
                     if(h.eq.1)then
                        cechi1p(m)=echi0(m)             
                     elseif(h.eq.2)then   
                        cechi1p(m)=conjg(echiplus(m))
                     else                          
                        cechi1p(m)=conjg(echiminus(m))
                     endif
                     
                  enddo
                  
                  call cdot(echi1,zq14,zq14echi1)
                  call cdot(cechi1p,zq14,zq14cechi1p)
                  call cdot(echi1,cechi1p,zechi1cechi1p)
                  call dot(q14,q15,q14q15)
                  
         weight1=weight1+(-(q14q15+mh**2)*zechi1cechi1p-2d0*zq14echi1*
     &      zq14cechi1p)*rho1chi(l,h)/(2d0*(q14q15+2d0*mh**2))*3d0
                  
               enddo
            enddo
            
            weight=weight*weight1
            
         elseif(chiflag.eq.2)then

            weight2=0d0
            
            do k=1,4
               q14(k)=q(k,14)
               q15(k)=q(k,15)
            enddo

            call dot(q14,q15,q14q15)

            q14c(4)=q(4,14)
            q15c(4)=q(4,15)

            do j=1,3
               q14c(j)=-q(j,14)
               q15c(j)=-q(j,15)
            enddo

            do j=1,4
               do k=1,4
                  g(j,k)=0d0
               enddo
            enddo

            g(1,1)=-1d0
            g(2,2)=-1d0
            g(3,3)=-1d0
            g(4,4)=1d0

               do m=1,5
                  do n=1,5
                     do h=1,4
                        do l=1,4

                     if(m.eq.1)then
                        cechi2(h,l)=conjg(echi2m2(h,l))                 
                     elseif(m.eq.2)then  
                        cechi2(h,l)=conjg(echi2m1(h,l))  
                     elseif(m.eq.3)then  
                        cechi2(h,l)=conjg(echi20(h,l))
                     elseif(m.eq.4)then  
                        cechi2(h,l)=conjg(echi2p1(h,l))  
                     else  
                        cechi2(h,l)=conjg(echi2p2(h,l))    
                     endif

                     if(n.eq.1)then
                        echi2p(h,l)=echi2m2(h,l)                
                     elseif(n.eq.2)then  
                        echi2p(h,l)=echi2m1(h,l) 
                     elseif(n.eq.3)then  
                        echi2p(h,l)=echi20(h,l)
                     elseif(n.eq.4)then  
                        echi2p(h,l)=echi2p1(h,l)
                     else  
                        echi2p(h,l)=echi2p2(h,l) 
                     endif
                   
                     enddo
                     enddo

            do j=1,4
               do o=1,4
                  do k=1,4
                     do p=1,4
                        weight2=weight2+4d0*echi2p(p,k)*cechi2(o,j)*
     &                (q14c(p)*q15c(o)+q14c(o)*q15c(p)-mx**2*g(p,o)/2d0)
     &                 *(q14c(k)-q15c(k))*(q14c(j)-q15c(j))*rho2chi(m,n)
                     enddo
                  enddo
               enddo
            enddo

         enddo
      enddo

         pnorm=16d0*(mx**2/4d0-mh**2)*(mx**2/2d0+4d0*mh**2/3d0)/5d0
         weight=weight*weight2/pnorm

      endif

      elseif(decay.eq.3)then

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c -- decay 5(mx) --> 14(mh) + 15(mh)   (scalars)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

         do k=1,4
            q14(k)=q(k,14)
            zq14(k)=q(k,14)
            q15(k)=q(k,15)
            zq15(k)=q(k,15)
         enddo

         if(chiflag.eq.1)then

            weight1=0d0

        do l=1,3
           do h=1,3
              do m=1,4

                 if(l.eq.1)then
                    echi1(m)=echi0(m)                 
                 elseif(l.eq.2)then  
                    echi1(m)=echiplus(m)
                 else                          
                    echi1(m)=echiminus(m)
                 endif
                 
                 if(h.eq.1)then
                    cechi1p(m)=echi0(m)             
                 elseif(h.eq.2)then   
                    cechi1p(m)=conjg(echiplus(m))
                 else                          
                    cechi1p(m)=conjg(echiminus(m))
                 endif

              enddo
              
              call cdot(echi1,zq14,zq14echi1)
              call cdot(echi1,zq15,zq15echi1)
              call cdot(cechi1p,zq14,zq14cechi1p)
              call cdot(cechi1p,zq15,zq15cechi1p)
              call dot(q14,q15,q14q15)
              
              weight1=weight1+(zq14echi1-zq15echi1)
     &*(zq14cechi1p-zq15cechi1p)/(mchi**2-4d0*mh**2)*3d0*rho1chi(l,h)
              
           enddo
        enddo

              weight=weight*weight1

           elseif(chiflag.eq.2)then

              q14c(4)=q(4,14)
              q15c(4)=q(4,15)

              do j=1,3
                 q14c(j)=-q(j,14)
                 q15c(j)=-q(j,15)
              enddo

              weight2=0d0
              
              do m=1,5
                 do n=1,5
                    do h=1,4
                       do l=1,4
                          
                          if(m.eq.1)then
                             echi2(h,l)=echi2m2(h,l)              
                          elseif(m.eq.2)then  
                             echi2(h,l)=echi2m1(h,l)
                          elseif(m.eq.3)then  
                             echi2(h,l)=echi20(h,l)
                          elseif(m.eq.4)then  
                             echi2(h,l)=echi2p1(h,l)
                          else  
                             echi2(h,l)=echi2p2(h,l)   
                          endif
                          
                          if(n.eq.1)then
                             cechi2p(h,l)=conjg(echi2m2(h,l))                 
                          elseif(n.eq.2)then  
                             cechi2p(h,l)=conjg(echi2m1(h,l))  
                          elseif(n.eq.3)then  
                             cechi2p(h,l)=conjg(echi20(h,l))
                          elseif(n.eq.4)then 
                             cechi2p(h,l)=conjg(echi2p1(h,l))  
                          else  
                             cechi2p(h,l)=conjg(echi2p2(h,l))    
                          endif
                          
                       enddo
                    enddo
                    
                     do j=1,4
                        do k=1,4
                           do p=1,4
                              do o=1,4
            weight2=weight2+echi2(j,k)*cechi2p(p,o)*(q14c(j)-q15c(j))
     &          *(q14c(k)-q15c(k))*(q14c(p)-q15c(p))*(q14c(o)-q15c(o))
     &                                *rho2chi(m,n)
                              enddo
                           enddo
                        enddo
                     enddo
                  
                  enddo
               enddo

               weight=weight*weight2
               weight=weight/(mchi**2-4d0*mh**2)**2*15d0/2d0
               
            endif
            
         endif
         
      endif
      

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      
c     event record:
c
c     event number, number of lines
c
c     each line includes the following columns (in this order):
c
c     ->line number
c     -> particle identity pdg
c     -> status code isthep
c     -> pointer to the position where the mother is stored (0 for initial entries).
c     -> pointer to position of second mother.
c     -> pointer to the position of the first daughter.
c     -> pointer to the position of the last daughter.
c     -> momentum in the x direction
c     -> momentum in the y direction
c     -> momentum in the z direction
c     -> energy
c     -> mass
c     -> production vertex x position
c     -> production vertex y position
c     -> production vertex z position
c     -> production time

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

 808  weight=weight

      if(weightm.lt.weight)then

         weightm=weight

      endif

      if(ll.eq.1)then    ! weighted

      wthist=weight/dfloat(ntotal)
      
      call binit(wthist)
     
      elseif(ll.eq.2)then ! hit and miss

         call r2455(ranhist)

         if(ranhist.lt.weight/weightm)then

            num=num+1

         call binit(sumt/nev)

         if(xflag.eq.'gam')then
      if (outevt) then
            write(30,302)num,7
      endif
            nevhep=num 
            nhep=7     
            elseif(xflag.eq.'pi'.or.xflag.eq.'eta'.or.xflag.eq.'ks')then
      if (outevt) then
            write(30,302)num,11
      endif
            nevhep=num 
            nhep=11        
            elseif(xflag.eq.'rho'.or.xflag.eq.'etap'.or.xflag.
     &           eq.'etaetap'.or.xflag.eq.'pipm'.or.xflag.eq.'kpkm')then
      if (outevt) then
            write(30,302)num,7
      endif
            nevhep=num 
            nhep=7     
        elseif(xflag.eq.'ups'.or.xflag.eq.'jpsi'.or.xflag.eq.'psip')then    
            if(phdecay.eq.'mu')then
      if (outevt) then
               write(30,302)num,7
      endif
               nevhep=num 
               nhep=7     
            elseif(phdecay.eq.'psipi')then
      if (outevt) then
               write(30,302)num,10
      endif
               nevhep=num 
               nhep=10 
            endif
         elseif(decay.eq.1)then
      if (outevt) then
            write(30,302)num,9
      endif
            nevhep=num 
            nhep=9     
         elseif(decay.eq.2.or.decay.eq.3)then
      if (outevt) then
            write(30,302)num,7
      endif
            nevhep=num 
            nhep=7     
         elseif(decay.eq.4)then
      if (outevt) then
            write(30,302)num,9
      endif
            nevhep=num 
            nhep=9     
        endif

      do k=1,5
      if (outevt) then
         write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &        nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &        nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &       ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

         idhep(k)=nint(rec(k,2)) 
         isthep(k)=nint(rec(k,3))
         jmohep(1,k)=nint(rec(k,4))
         jmohep(2,k)=nint(rec(k,5))
         jdahep(1,k)=nint(rec(k,6))
         jdahep(2,k)=nint(rec(k,7))
         phep(1,k)=(rec(k,8))
         phep(2,k)=(rec(k,9))
         phep(3,k)=(rec(k,10))
         phep(4,k)=(rec(k,11))
         phep(5,k)=(rec(k,12))
         vhep(1,k)=(rec(k,13))
         vhep(2,k)=(rec(k,14))
         vhep(3,k)=(rec(k,15))
         vhep(4,k)=(rec(k,16))

      enddo

      if(xflag.eq.'gam')then
     
         do k=12,13         
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-6
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

      elseif(xflag.eq.'pi'.or.xflag.eq.'eta'.or.xflag.eq.'ks')then

           do k=12,17        
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-6
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo       

         elseif(xflag.eq.'rho'.or.xflag.eq.'etap'.or.xflag.
     &        eq.'etaetap'.or.xflag.eq.'pipm'.or.xflag.eq.'kpkm')then

            
         do k=12,13         
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-6
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

      elseif(xflag.eq.'ups'.or.xflag.eq.'jpsi'.or.xflag.eq.'psip')then
            
         if(phdecay.eq.'mu')then

         do k=8,9     
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-2
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

         elseif(phdecay.eq.'psipi')then

         do k=8,12    
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-2
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

         endif


      elseif(decay.eq.1)then

         do k=6,9 
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            idhep(k)=nint(rec(k,2)) 
            isthep(k)=nint(rec(k,3))
            jmohep(1,k)=nint(rec(k,4))
            jmohep(2,k)=nint(rec(k,5))
            jdahep(1,k)=nint(rec(k,6))
            jdahep(2,k)=nint(rec(k,7))
            phep(1,k)=(rec(k,8))
            phep(2,k)=(rec(k,9))
            phep(3,k)=(rec(k,10))
            phep(4,k)=(rec(k,11))
            phep(5,k)=(rec(k,12))
            vhep(1,k)=(rec(k,13))
            vhep(2,k)=(rec(k,14))
            vhep(3,k)=(rec(k,15))
            vhep(4,k)=(rec(k,16))

         enddo

      elseif(decay.eq.2.or.decay.eq.3)then

         do k=14,15  
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-8
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

         elseif(decay.eq.4)then

         do k=14,17 
      if (outevt) then
            write(30,300)nint(rec(k,1)),nint(rec(k,2)),nint(rec(k,3)),
     &           nint(rec(k,4)),nint(rec(k,5)),nint(rec(k,6)),
     &           nint(rec(k,7)),rec(k,8),rec(k,9),rec(k,10),rec(k,11)
     &           ,rec(k,12),rec(k,13),rec(k,14),rec(k,15),rec(k,16)
      endif

            kk=k-8
            idhep(kk)=nint(rec(k,2)) 
            isthep(kk)=nint(rec(k,3))
            jmohep(1,kk)=nint(rec(k,4))
            jmohep(2,kk)=nint(rec(k,5))
            jdahep(1,kk)=nint(rec(k,6))
            jdahep(2,kk)=nint(rec(k,7))
            phep(1,kk)=(rec(k,8))
            phep(2,kk)=(rec(k,9))
            phep(3,kk)=(rec(k,10))
            phep(4,kk)=(rec(k,11))
            phep(5,kk)=(rec(k,12))
            vhep(1,kk)=(rec(k,13))
            vhep(2,kk)=(rec(k,14))
            vhep(3,kk)=(rec(k,15))
            vhep(4,kk)=(rec(k,16))

         enddo

      endif
      endif
      endif
     
 700  sum=sum+weight
      sum1=sum1+weight**2
      sum2=sum2+weight*sv
      sum3=sum3+weight*senh
      sum4=sum4+weight*beff

 2024 continue
      if (exitp.eq.24) then
      return
      endif
      if(ll.eq.2)then

      if(num.gt.nev-1)then  ! exit loop once required no. of unweighted events generated
 2026 continue
      if (exitp.eq.26) then
      return
      endif

         ntotal=i
         goto 888

 2027 continue
      if (exitp.eq.27) then
      return
      endif
      endif
      endif
 2025 continue
      if (exitp.eq.25) then
      return
      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                                    c                                                                      
c     end of event loop                                              c
c                                                                    c
      if (itr2.ge.ntotal) then
      exit
      endif

 2023 continue
      if (exitp.eq.23) then
      return
      endif
      enddo
 2021 continue
      if (exitp.eq.21) then
      return
      endif

 300  format(i4,1x,i8,1x,i4,1x,i4,1x,i4,1x,i4,1x,i4,1x,e13.6,1x,e13.6
     &,1x,e13.6,1x,e13.6,1x,e13.6,1x,e13.6,1x,e13.6,1x,e13.6,1x
     &,e13.6)
 302  format(1x,i8,1x,i5,1x,e13.6)

 888  sum=sum/dfloat(ntotal)
      sum1=sum1/dfloat(ntotal)
      var=dsqrt((sum1-sum**2)/dfloat(ntotal))
      sum2=sum2/dfloat(ntotal)
      sum3=sum3/dfloat(ntotal)
      sum4=sum4/dfloat(ntotal)

c     create histograms

      if(ll.eq.1)then
         print*,'weighted'
         write(6,*)
         sumt=sum
      else
         print*,'unweighted'
         write(6,*)
      endif

      do i=1,nhist
           call histo2(i,0)
      enddo

ccccccc     print information to screen

      if(ll.eq.1)then

         if(xflag.eq.'jpsi')then

            write(6,*)
            write(6,*)'  j/psi photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,240)rts,mx,bpsi,ntotal,ntotal-ncut,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,250)rts,mx,bpsi,ntotal,ntotal-ncut,yx,sum,var

            endif

         elseif(xflag.eq.'ups')then

            write(6,*)
            write(6,*)'  upsilon photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,241)rts,mx,bpsi,ntotal,ntotal-ncut,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,251)rts,mx,bpsi,ntotal,ntotal-ncut,yx,sum,var

            endif

         elseif(xflag.eq.'psip')then

            write(6,*)
            write(6,*)'  psi(2s) photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,242)rts,mx,ntotal,ntotal-ncut,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,252)rts,mx,ntotal,ntotal-ncut,yx,sum,var

            endif

         elseif(xflag.eq.'chic')then

            if(chiflag.eq.0)then
          
               write(6,*)
               write(6,*)'  chi_c(0++) production'

            elseif(chiflag.eq.1)then

               write(6,*)
               write(6,*)'  chi_c(1++) production'

            elseif(chiflag.eq.2)then
            
               write(6,*)
               write(6,*)'  chi_c(2++) production'

            endif

            if(decay.eq.1)then

            if(cross.eq.1)then

           write(6,640)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,650)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,yx,sum,var

           endif

           else

            if(cross.eq.1)then

              write(6,643)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,653)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,yx,sum,var

           endif

        endif

         elseif(xflag.eq.'chib')then

            if(chiflag.eq.0)then
         
               write(6,*)
               write(6,*)'  chi_b(0++) production'

            elseif(chiflag.eq.1)then

               write(6,*)
               write(6,*)'  chi_b(1++) production'

            elseif(chiflag.eq.2)then
            
               write(6,*)
               write(6,*)'  chi_b(2++) production'

            endif

            if(decay.eq.1)then

         if(cross.eq.1)then

           write(6,641)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,2d0*ymax,sum,var

         elseif(cross.eq.2)then

           write(6,651)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,yx,sum,var

         endif

           else

            if(cross.eq.1)then

              write(6,642)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,652)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,ntotal,ntotal-ncut,yx,sum,var

           endif

           endif

      endif
      
      if(xflag.eq.'etac'.or.xflag.eq.'etab')then
         
         if(xflag.eq.'etac')then

            write(6,*)
            write(6,*)'  eta_c(0-+) production'
              
            elseif(xflag.eq.'etab')then

            write(6,*)
            write(6,*)'  eta_b(0-+) production'

         endif

         if(cross.eq.1)then

            write(6,647)rts,mchi,beff,bretahad,delta,
     .           sum2/sum,senh,ntotal,ntotal-ncut,2d0*ymax,sum,var

         elseif(cross.eq.2)then
           
            write(6,657)rts,mchi,beff,bretahad,delta,
     .           sum2/sum,senh,ntotal,ntotal-ncut,yx,sum,var

         endif
               
      endif

      elseif(ll.eq.2)then

         if(xflag.eq.'jpsi')then

            write(6,*)
            write(6,*)'  j/psi photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,220)rts,mx,bpsi,num,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,230)rts,mx,bpsi,num,yx,sum,var

            endif

         elseif(xflag.eq.'ups')then

            write(6,*)
            write(6,*)'  upsilon photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,221)rts,mx,bpsi,num,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,231)rts,mx,bpsi,num,yx,sum,var

            endif

        elseif(xflag.eq.'psip')then

            write(6,*)
            write(6,*)'  psi(2s) photoproduction'          
     
            if(cross.eq.1)then
               
               write(6,222)rts,mx,num,2d0*ymax
     &,sum,var
               
            elseif(cross.eq.2)then

               write(6,232)rts,mx,num,yx,sum,var

            endif

         elseif(xflag.eq.'chic')then

            if(chiflag.eq.0)then
         
               write(6,*)
               write(6,*)'  chi_c(0++) production'

            elseif(chiflag.eq.1)then

               write(6,*)
               write(6,*)'  chi_c(1++) production'

            elseif(chiflag.eq.2)then
            
               write(6,*)
               write(6,*)'  chi_c(2++) production'

            endif

            if(decay.eq.1)then

            if(cross.eq.1)then

           write(6,620)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,num,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,630)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,num,yx,sum,var

           endif

           else

            if(cross.eq.1)then

              write(6,623)rts,mchi,brchihad,beff,delta,
     .            sum2/sum,senh,num,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,633)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,num,yx,sum,var

           endif

        endif

         elseif(xflag.eq.'chib')then

            if(chiflag.eq.0)then
         
               write(6,*)
               write(6,*)'  chi_b(0++) production'

            elseif(chiflag.eq.1)then

               write(6,*)
               write(6,*)'  chi_b(1++) production'

            elseif(chiflag.eq.2)then
            
               write(6,*)
               write(6,*)'  chi_b(2++) production'

            endif

            if(decay.eq.1)then

         if(cross.eq.1)then

           write(6,621)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,num,2d0*ymax,sum,var

         elseif(cross.eq.2)then

           write(6,631)rts,mchi,mpsi,brchipsi,errbrchi,brpsimu,errbrpsi,
     .            beff,delta,
     .            sum2/sum,senh,num,yx,sum,var

         endif

           else

            if(cross.eq.1)then

              write(6,622)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,num,2d0*ymax,sum,var

            elseif(cross.eq.2)then

           write(6,632)rts,mchi,beff,brchihad,delta,
     .            sum2/sum,senh,num,yx,sum,var

           endif

           endif

      endif
      
      if(xflag.eq.'etac'.or.xflag.eq.'etab')then
         
         if(xflag.eq.'etac')then

            write(6,*)
            write(6,*)'  eta_c(0-+) production'
              
            elseif(xflag.eq.'etab')then

            write(6,*)
            write(6,*)'  eta_b(0-+) production'

         endif

         if(cross.eq.1)then

            write(6,627)rts,mchi,beff,bretahad,delta,
     .           sum2/sum,senh,num,2d0*ymax,sum,var

         elseif(cross.eq.2)then
           
            write(6,637)rts,mchi,beff,bretahad,delta,
     .           sum2/sum,senh,num,yx,sum,var

         endif
               
      endif
      endif

      if(xflag.eq.'gam')then

               write(6,*)
               write(6,*)'  digamma production'

               if(ll.eq.1)then

         if(cross.eq.1)then

           write(6,540)rts,sum4/sum,sum2/sum,sum3/sum,ntotal,ntotal-ncut
     .      ,2d0*ymax,sum,var

         elseif(cross.eq.2)then

           write(6,550)rts,sum4/sum,sum2/sum,sum3/sum,ntotal,ntotal-ncut
     .      ,yx,sum,var

          endif

          elseif(ll.eq.2)then

              if(cross.eq.1)then

           write(6,541)rts,sum4/sum,sum2/sum,sum3/sum,num
     .      ,2d0*ymax,sum,var

         elseif(cross.eq.2)then

           write(6,551)rts,sum4/sum,sum2/sum,sum3/sum,num
     .      ,yx,sum,var

          endif
          endif

      endif

      if(xflag.eq.'pi'.or.xflag.eq.'rho'.or.xflag.eq.'pipm'.or.xflag.eq
     &.'kpkm'.or.xflag.eq.'ks')then
         
         write(6,*)
         if(xflag.eq.'pi')then
         write(6,*)'  pi0 pi0 production'
         elseif(xflag.eq.'pipm')then
         write(6,*)'  pi+ pi- production'    
         elseif(xflag.eq.'rho')then
         write(6,*)'  rho0 rho0 production'
         elseif(xflag.eq.'kpkm')then
         write(6,*)'  k+ k- production'   
         elseif(xflag.eq.'ks')then
         write(6,*)'  k0 k0 production'   
         endif

         if(ll.eq.1)then
            
            if(cross.eq.1)then
               
          write(6,560)rts,mmes,fpi,sum4/sum,sum2/sum,sum3/sum,ntotal
     .              ,ntotal-ncut,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
           write(6,570)rts,mmes,fpi,sum4/sum,sum2/sum,sum3/sum,ntotal
     .              ,ntotal-ncut,yx,sum,var
               
            endif
            
         elseif(ll.eq.2)then

            if(cross.eq.1)then
               
               write(6,561)rts,mmes,fpi,sum4/sum,sum2/sum,sum3/sum,num
     .              ,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
               write(6,571)rts,mmes,fpi,sum4/sum,sum2/sum,sum3/sum,num
     .              ,yx,sum,var
               
            endif
         endif
         
      endif

      if(xflag.eq.'eta'.or.xflag.eq.'etap')then
         
         write(6,*)
         if(xflag.eq.'eta')then
         write(6,*)'  eta eta production'
         elseif(xflag.eq.'etap')then
         write(6,*)'  eta(prime) eta(prime) production'
         endif

         if(ll.eq.1)then
            
            if(cross.eq.1)then
               
          write(6,562)rts,mmes,fpi,thetap*180d0/pi,sum4/sum,sum2/sum
     .              ,sum3/sum,ntotal,ntotal-ncut,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
           write(6,572)rts,mmes,fpi,thetap*180d0/pi,sum4/sum,sum2/sum
     .              ,sum3/sum,ntotal,ntotal-ncut,yx,sum,var
               
            endif
            
         elseif(ll.eq.2)then

            if(cross.eq.1)then
               
               write(6,563)rts,mmes,fpi,thetap*180d0/pi,sum4/sum,
     .              sum2/sum,sum3/sum,num,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
               write(6,573)rts,mmes,fpi,thetap*180d0/pi,sum4/sum
     .              ,sum2/sum,sum3/sum,num,yx,sum,var
               
            endif
         endif
         
      endif

      if(xflag.eq.'etaetap')then
         
         write(6,*)
         write(6,*)'  eta eta(prime) production'

         if(ll.eq.1)then
            
            if(cross.eq.1)then
               
          write(6,564)rts,mmes1,mmes2,fpi,thetap*180d0/pi,sum4/sum
     .              ,sum2/sum,sum3/sum,ntotal,ntotal-ncut,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
           write(6,574)rts,mmes1,mmes2,fpi,thetap*180d0/pi,sum4/sum
     .              ,sum2/sum,sum3/sum,ntotal,ntotal-ncut,yx,sum,var
               
            endif
            
         elseif(ll.eq.2)then

            if(cross.eq.1)then
               
               write(6,565)rts,mmes1,mmes2,fpi,thetap*180d0/pi,sum4/sum,
     .              sum2/sum,sum3/sum,num,2d0*ymax,sum,var
               
            elseif(cross.eq.2)then
               
               write(6,575)rts,mmes1,mmes2,fpi,thetap*180d0/pi,sum4/sum
     .              ,sum2/sum,sum3/sum,num,yx,sum,var
               
            endif
         endif
         
      endif

      if (ll.ge.2) then
      exit
      endif

 2019 continue
      if (exitp.eq.19) then
      return
      endif
      enddo
 2017 continue
      if (exitp.eq.17) then
      return
      endif

      close(30)

cccccccccccccccccccccccccccccccccccccccccccccccccccccccc

240   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

250   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

241   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

251   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

242   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'psi(2s) mass (gev)        : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

252   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'psi(2s) mass (gev)        : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)


220   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

230   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

221   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

231   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'t slope b_el (gev^-2)     : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

222   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'psi(2s) mass (gev)        : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

232   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'psi(2s) mass (gev)        : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)


540   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (fb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

550   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (fb)      : ',f15.5,'  +- ',f15.5,
     .   /)

560   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

570   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

562   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

572   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

564   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'eta(prime) mass (gev)     : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

574   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'eta(prime) mass (gev)     : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

541   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (fb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

551   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (fb)      : ',f15.5,'  +- ',f15.5,
     .   /)

561   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

571   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

563   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

573   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'meson mass (gev)          : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

565   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'eta(prime) mass (gev)     : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

575   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'eta(prime) mass (gev)     : ',f10.5,/,
     .       3x,'meson form factor (gev)   : ',f10.5,/,
     .       3x,'mixing angle (deg.)       : ',f10.5,/,
     .       3x,'<t slope b_exp> (gev^-2)  : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'<s^2_enh>                 : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

640   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'br(chic --> psi gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(psi --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)



650   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'br(chic --> psi gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(psi --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

620   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'br(chic --> psi gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(psi --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)



630   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'j/psi mass (gev)          : ',f10.5,/,
     .       3x,'br(chic --> psi gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(psi --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

641   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'br(chib --> ups gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(eps --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 642  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chib mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chib --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

621   format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'br(chib --> ups gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(eps --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 622  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chib mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chib --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 647  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(eta --> h1h2)          : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 627  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(eta --> h1h2)          : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (pb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 651  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'br(chib --> ups gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(ups --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 652  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chib mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chib --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 631  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chi mass (gev)            : ',f10.5,/,
     .       3x,'upsilon mass (gev)        : ',f10.5,/,
     .       3x,'br(chib --> ups gamma)    : ',f10.5,'  +- ',f10.5,/
     .       3x,'br(ups --> mu+mu-)        : ',f10.5,'  +- ',f10.5,/
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 632  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chib mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chib --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 657  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(eta --> h1h2)          : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 637  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'eta mass (gev)            : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(eta --> h1h2)          : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (pb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 643  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chic mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chic --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 653  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chic mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chic --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'events passing cuts       : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 623  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chic mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chic --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,
     .       3x,'delta y                   : ',f10.5,/,/,
     .       3x,'sigma (nb)                : ',f15.5,'  +-  ',f15.5,
     .   /)

 633  format(/,
     .       3x,'collider energy  (gev)    : ',f10.3,/,
     .       3x,'chic mass (gev)           : ',f10.5,/,
     .       3x,'t slope b_exp (gev^-2)    : ',f10.5,/,
     .       3x,'br(chic --> h1h2)         : ',f10.5,/,
     .       3x,'delta=alpha_p(0)-1        : ',f10.5,/,
     .       3x,'<s^2_eik>                 : ',f10.5,/,
     .       3x,'s^2_enh                   : ',f10.5,/,
     .       3x,'number of events          : ',i9,/,/,
     .       3x,'dsigma/dy|y=',f3.1,' (nb)      : ',f15.5,'  +- ',f15.5,
     .   /)

 889  return
 2001 continue
      if (exitp.eq.1) then
      return
      endif
      end

c     please include relevant .f files in same folder to run

c$$$      include 'mstwpdf.f'
c$$$      include 'alphas.f'
c$$$      include 'cteq6pdf-2010.f'

ccccccccccccccccccccccccccccccccccccccccccccc

c     binning subroutine

      subroutine binit(wt)
      implicit double precision(a-y)
      double precision q(4,20),pt1(2),pt2(2),ptx(2)
      common/mom/q
      common/mompt/pt1,pt2,ptx     
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx

       pt1sqr=((q(1,3)**2+q(2,3)**2))
       pt2sqr=(q(1,4)**2+q(2,4)**2)

       ptpsi=dsqrt(q(1,7)**2+q(2,7)**2)
       ptchi=dsqrt(q(1,5)**2+q(2,5)**2)
       ptmu1=dsqrt(q(1,8)**2+q(2,8)**2)
       ptmu2=dsqrt(q(1,9)**2+q(2,9)**2)
       ptgam=dsqrt(q(1,6)**2+q(2,6)**2)
 
       ptpi1=dsqrt(q(1,14)**2+q(2,14)**2)
       ptpi2=dsqrt(q(1,15)**2+q(2,15)**2)
 
       call histo1(1,60,-7.5d0,7.5d0,yx,wt)
c       call histo1(1,30,0d0,2d0,ptchi,wt)

      return
      end

      subroutine cut(icut)
      implicit double precision(a-y)
      double precision q(4,20)
      integer icut
      common/mom/q
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx
      common/az/phi1,phi2

      icut=0

c -- insert  cuts here if desired...
c     for example, a minimum energy cut on the final state leptons (chi_c cep)
c      if(q(4,8).lt.1d0) return
c      if(q(4,9).lt.1d0) return
c
c     etc....

ccc

c     sample cuts below- uncomment relevant section to apply

ccc

ccc     diphoton cuts

c$$$        pmod1=dsqrt(q(1,12)**2+q(2,12)**2+q(3,12)**2)
c$$$        pmod2=dsqrt(q(1,13)**2+q(2,13)**2+q(3,13)**2)
c$$$        eta1=dabs(0.5d0*dlog((pmod1+q(3,12))/(pmod1-q(3,12))))
c$$$        eta2=dabs(0.5d0*dlog((pmod2+q(3,13))/(pmod2-q(3,13))))
c$$$        et1=dsqrt(q(4,12)**2-q(3,12)**2)
c$$$        et2=dsqrt(q(4,13)**2-q(3,13)**2)
c$$$
c$$$      if(eta1.gt.rcut)return
c$$$      if(eta2.gt.rcut)return
c$$$      if(et1.lt.ecut)return
c$$$      if(et2.lt.ecut)return      

ccc      meson pair cuts

      pmod1=dsqrt(q(1,12)**2+q(2,12)**2+q(3,12)**2)
      pmod2=dsqrt(q(1,13)**2+q(2,13)**2+q(3,13)**2)
      
      eta1=dabs(0.5d0*dlog((pmod1+q(3,12))/(pmod1-q(3,12))))
      eta2=dabs(0.5d0*dlog((pmod2+q(3,13))/(pmod2-q(3,13))))
      et1=dsqrt(q(4,12)**2-q(3,12)**2)
      et2=dsqrt(q(4,13)**2-q(3,13)**2)

c      et1=dsqrt(q(1,12)**2+q(2,12)**2)
c      et2=dsqrt(q(1,13)**2+q(2,13)**2)


c$$$      if(eta1.gt.rcut)return
c$$$      if(eta2.gt.rcut)return
c$$$      if(et1.lt.ecut)return
c$$$      if(et2.lt.ecut)return    
      
ccc    chi_c sample cuts (on muons)

c$$$       ptmu1=dsqrt(q(1,8)**2+q(2,8)**2)
c$$$       ptmu2=dsqrt(q(1,9)**2+q(2,9)**2)
c$$$       pmod1=dsqrt(q(1,8)**2+q(2,8)**2+q(3,8)**2)
c$$$       pmod2=dsqrt(q(1,9)**2+q(2,9)**2+q(3,9)**2)
c$$$
c$$$       eta1=dabs(0.5d0*dlog((pmod1+q(3,8))/(pmod1-q(3,8))))
c$$$       eta2=dabs(0.5d0*dlog((pmod2+q(3,9))/(pmod2-q(3,9))))
c$$$       etag=dabs(0.5d0*dlog((pmod1+q(3,6))/(pmod1-q(3,6))))
c$$$       etg=dsqrt(q(4,6)**2-q(3,6)**2)
c$$$
c$$$       if(ptmu1.lt.ptcut)return
c$$$       if(ptmu2.lt.ptcut)return
c$$$       if(eta1.gt.etacut)return
c$$$       if(eta2.gt.etacut)return

ccc   chi_c0 4-body decay cuts

       pmod1=dsqrt(q(1,14)**2+q(2,14)**2+q(3,14)**2)
       pmod2=dsqrt(q(1,15)**2+q(2,15)**2+q(3,15)**2)
       pmod3=dsqrt(q(1,16)**2+q(2,16)**2+q(3,16)**2)
       pmod4=dsqrt(q(1,17)**2+q(2,17)**2+q(3,17)**2)

       eta1=dabs(0.5d0*dlog((pmod1+q(3,14))/(pmod1-q(3,14))))
       eta2=dabs(0.5d0*dlog((pmod2+q(3,15))/(pmod2-q(3,15))))
       eta3=dabs(0.5d0*dlog((pmod3+q(3,16))/(pmod3-q(3,16))))
       eta4=dabs(0.5d0*dlog((pmod4+q(3,17))/(pmod4-q(3,17))))
      
c     pseudorapidity cuts

c$$$       etacut4=1d0      ! |eta|_max
c$$$
c$$$       if(eta1.gt.etacut4)return
c$$$       if(eta2.gt.etacut4)return
c$$$       if(eta3.gt.etacut4)return
c$$$       if(eta4.gt.etacut4)return

       pt1=dsqrt(q(1,14)**2+q(2,14)**2)
       pt2=dsqrt(q(1,15)**2+q(2,15)**2)
       pt3=dsqrt(q(1,16)**2+q(2,16)**2)
       pt4=dsqrt(q(1,17)**2+q(2,17)**2)

c     pt cuts

c$$$       ptcut4=0.2d0     ! pt_min
c$$$
c$$$       if(pt1.lt.ptcut4)return
c$$$       if(pt2.lt.ptcut4)return
c$$$       if(pt3.lt.ptcut4)return
c$$$       if(pt4.lt.ptcut4)return

cccccccccccccccccccccccccc

        icut=1

      return
      end
 
c     prints histograms

      subroutine histo1(ih,ib,x0,x1,x,w)
      implicit real*8(a-h,o-z)
      character*1 regel(30),blank,star
      dimension h(20,100),hx(20),io(20),iu(20),ii(20)
      dimension y0(20),y1(20),ic(20)
      data regel / 30*' ' /,blank /' ' /,star /'*'/
      save
      logical outevt,outhst
      common/output/outevt,outhst
      if (outhst) then
      open(10,file="output.dat")
      endif
c      print*,hx(ih)
      y0(ih)=x0
      y1(ih)=x1
      ic(ih)=ib
      if(x.lt.x0) goto 11
      if(x.gt.x1) goto 12
      ix=idint((x-x0)/(x1-x0)*dble(ib))+1
      h(ih,ix)=h(ih,ix)+w
      if(h(ih,ix).gt.hx(ih)) hx(ih)=h(ih,ix)
      ii(ih)=ii(ih)+1
      return
   11 iu(ih)=iu(ih)+1
      return
   12 io(ih)=io(ih)+1
      return
      entry histo2(ih,il)
      ib1=ic(ih)
      x01=y0(ih)
      x11=y1(ih)
      bsize=(x11-x01)/dble(ib1)
      hx(ih)=hx(ih)*(1.d0+1.d-06)
      if(il.eq.0) write(6,21) ih,ii(ih),iu(ih),io(ih)
      if(il.eq.1) write(6,22) ih,ii(ih),iu(ih),io(ih)
   21 format(' no.',i3,' lin : inside,under,over ',3i6)
   22 format(' no.',i3,' log : inside,under,over ',3i6)
      if(ii(ih).eq.0) goto 28
      write(6,23)
   23 format(35(1h ),3(10h----+----i))
      do 27 iv=1,ib1
      z=(dble(iv)-0.5d0)/dble(ib1)*(x11-x01)+x01
      if(il.eq.1) goto 24
      iz=idint(h(ih,iv)/hx(ih)*30.)+1
      goto 25
   24 iz=-1
      if(h(ih,iv).gt.0.d0)
     .iz=idint(dlog(h(ih,iv))/dlog(hx(ih))*30.)+1
   25 if(iz.gt.0.and.iz.le.30) regel(iz)=star
      write(6,26) z,h(ih,iv)/bsize,(regel(i),i=1,30)
      if(ih.eq.1)then
      if (outhst) then
      write(10,*)z,h(ih,iv)/bsize        ! print histogram to file
      endif
      elseif(ih.eq.2)then
      write(20,*)z,h(ih,iv)/bsize
      endif
   26 format(1h ,2g15.6,4h   i,30a1,1hi)
   36 format(1h ,2g15.6)
      if(iz.gt.0.and.iz.le.30) regel(iz)=blank
   27 continue
      write(6,23)
      return
   28 write(6,29)
   29 format('  no entries inside histogram')
      return
      entry histo3(ih)
      do 31 i=1,100
   31 h(ih,i)=0.
      hx(ih)=0.
      io(ih)=0
      iu(ih)=0
      ii(ih)=0
      close(10)
      close(20)
      return 
      end

c     interpolates gg --> mmbar subamplitudes

      subroutine piint1(cost,out,out1,out2,out3,out4)
      implicit double precision (a-y)
      double precision dpi(39),dlpp(39),dlpm(39),vpm(39),vmp(39)
      data dpi/37735.8852,20055.3697,14893.9645,11594.2857,9051.88403  
     &,7105.60133,5504.60239,4160.81487,2962.91244,1946.37167,1100.36035  
     &,369.37271,-250.740006,-796.117001,-1229.43425,-1581.31089
     &,-1854.41934,-2044.21432,-2156.62229,-2193.37809,-2157.25669 
     &,-2045.23602,-1851.86616,-1587.07445,-1234.25965,-793.57717 
     &,-267.233729,360.204652,1113.84025,1949.57858,2971.95248 
     &,4139.07056,5476.72328,7129.11127,9095.01997,11556.3616 
     &,14820.68,20145.4536,37975.3654/
      data dlpp/163750140.,1244285.56,340991.151,157043.058,90233.9988  
     &,58845.4912,41626.1966,31203.9026,24407.865,19801.6924,16541.7545  
     &,14161.5873,12401.9307,11066.3497,10084.1272,9345.97718,8813.90295  
     &,8460.39601,8249.38857,8181.85028,8253.01158,8463.8526,8816.17957  
     &,9351.4626,10081.223,11078.5665,12387.909,14147.8609,16537.6903  
     &,19795.7355,24426.4572,31197.8779,41623.1058,58814.0166,90283.1695  
     &,156986.332,340903.524,1244432.55,163747150./
      data dlpm/163339741.,1207572.22,321261.832,143298.535,79549.4399
     &,49995.2568,33995.887,24436.0701,18285.3268,14162.5992,11277.5426
     &,9195.26593,7670.11396,6527.56213,5689.08382,5065.8446,4619.25666
     &,4321.99986,4147.25596,4090.9251,4149.07737,4323.76566,4620.44981
     &,5068.81789,5687.44574,6534.76826,7661.44212,9186.35322,11274.7718
     &,14158.3387,18299.2553,24431.3521,33993.3628,49968.516,79592.7883
     &,143246.773,321179.276,1207714.87,163336758./
      data vpm/291.731868,1644.546,2464.04083,3060.22272,3480.52959 
     &,3803.83209,4029.65331,4172.56016,4197.95202,4154.03875 
     &,4066.6799,3903.84933,3680.78936,3338.20161,2976.05074,2525.5204 
     &,1993.05188,1407.16244,742.386445,0.,-823.1656387,-1731.9835 
     &,-2756.21982,-3836.2926,-5056.10686,-6417.28236,-7864.34198 
     &,-9492.3603,-11351.705,-13297.5227,-15641.4482,-18228.8082 
     &,-21149.3413,-24759.1765,-29085.4957,-34609.3726,-42279.33   
     &,-55876.7288,-116968.015/
      data vmp/-116401.015,-55688.25032,-42444.13732,-34695.77195
     &,-28975.94269,-24706.152 ,-21217.9684,-18285.97891,-15606.1668
     &,-13288.33392,-11317.475,-9533.313213,-7941.146316,-6397.152273
     &,-5087.402252,-3863.94532,-2736.012846,-1736.410326,-824.4385986
     &,0.,741.240176,1403.575,2007.77168,2507.44625,2957.74343
     &,3348.70601,3645.1899,3887.07929,4078.97967,4156.91125,4207.44249 
     &,4159.51473,4016.61985,3811.9959,3493.68888,3052.60217,2454.47314 
     &,1650.11201,293.152919/ 
      integer n,ii

      n=39

      inc=2d0*0.995d0/38d0
      ii=(cost+0.995d0)/inc+1

      m=(dpi(ii+1)-dpi(ii))/inc
      out=m*(cost-dble(ii-1)*inc+0.995d0)+dpi(ii)

      m1=(dlpp(ii+1)-dlpp(ii))/inc
      out1=m1*(cost-dble(ii-1)*inc+0.995d0)+dlpp(ii)

      m2=(dlpm(ii+1)-dlpm(ii))/inc
      out2=m2*(cost-dble(ii-1)*inc+0.995d0)+dlpm(ii)

      m3=(vpm(ii+1)-vpm(ii))/inc
      out3=m3*(cost-dble(ii-1)*inc+0.995d0)+vpm(ii)

      m4=(vmp(ii+1)-vmp(ii))/inc
      out4=m4*(cost-dble(ii-1)*inc+0.995d0)+vmp(ii)

      return
      end

      subroutine lumpijz0(mgam,wl)

      double precision wl,mgam,m,v1
      double precision warr(55),rm(55),w082tev(55),w087tev(55)
     &,w08rtev(55),w0814tev(55),w992tev(55),w997tev(55),w99rtev(55)
     &,w9914tev(55),w0810tev(55),w9910tev(55),w0803tev(55),w0809tev(55)
     &,w9903tev(55),w9909tev(55)
      integer v,i,k,pdf,eflag
      common/flags/pdf,eflag
      save
      data rm/2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,
     &  16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.,27.,28.,29.,
     &  30.,31.,32.,33.,34.,35.,36.,37.,38.,39.,40.,41.,42.,43.,
     &  44.,45.,46.,47.,48.,49.,50.,60.,70.,80.,90.,100.,110/
      data w0809tev/1.51089689,1.3632804,0.945055223,0.648079363
     &,0.456537838,0.332123892,0.247792895,0.189142959
     &,0.147157573,0.116408991,0.0933498237,0.0758009013
     &,0.0622224989,0.0515796442,0.0431521209,0.0363741641
     &,0.030851378,0.0263562963,0.022638207,0.0195594707
     &,0.0169790048,0.0148212088,0.0129749316,0.0114108038
     &,0.0100674391,0.0089222309,0.00792149909,0.00706062479
     &,0.00631738947,0.00565971451,0.00508444217,0.00457950097
     &,0.00413400366,0.00374271772,0.00339455163,0.0030835974
     &,0.00280607533,0.00255897907,0.00233766921,0.00213755302
     &,0.00196116462,0.00180029095,0.00165235229,0.00152224627
     &,0.0014022604,0.00129462021,0.0011966656,0.0011077706
     &,0.00102635897,0.000504175176,0.000269625964,0.000153156119
     &,9.14786936e-05,5.67881563e-05,3.64554861e-05/
      data w0803tev/0.314909648,0.258620083,0.166544062,0.648079363
     &,0.0740714549,0.0523528414,0.0380890316,0.189142959
     &,0.0216511262,0.0167908986,0.0132172872,0.0758009013
     &,0.0085025762,0.00693326929,0.00570739553,0.0363741641
     &,0.00395696917,0.00333074364,0.00281921858,0.0195594707
     &,0.00205552951,0.0017713208,0.00152975275,0.0114108038
     &,0.00115569915,0.00101105224,0.000886643621,0.00706062479
     &,0.000689429187,0.00061006108,0.000541413253,0.00457950097
     &,0.000429749747,0.000384257583,0.000344342298,0.0030835974
     &,0.000277662161,0.000250090741,0.000225811476,0.00213755302
     &,0.000184738405,0.000167544298,0.000151755311,0.00152224627
     &,0.000125556578,0.000114321737,0.000104319098,0.0011077706
     &,8.71470893e-05,3.69806746e-05,1.66683942e-05,0.000153156119
     &,3.62688224e-06,1.69794946e-06,7.87759837e-07/
      data w9903tev/0.157511651,0.162629629,0.124453918,0.290676874
     &,0.0717984026,0.0561812834,0.0445851596,0.117090219
     &,0.0291130249,0.023865711,0.0197285596,0.0587282625
     &,0.0135662114,0.0113677858,0.00958189268,0.0334200864
     &,0.00690208396,0.00584791248,0.00497112511,0.0205563696
     &,0.00363746943,0.00313426102,0.0027012442,0.0133661413
     &,0.00200216724,0.00173222637,0.00150113296,0.00902860121
     &,0.00113715242,0.000992338183,0.000866594609,0.00628489954
     &,0.000663292666,0.000580981322,0.000509445506,0.00448219105
     &,0.000392761202,0.000346607738,0.000306429634,0.00323626
     &,0.000239451413,0.000211989383,0.000187257184,0.00238383183
     &,0.00014779612,0.000131581818,0.00011733367,0.00178434453
     &,9.34427964e-05,3.13492212e-05,1.14128768e-05,0.000249366374
     &,1.67913802e-06,6.5163894e-07,2.49690186e-07/
      data w9909tev/0.436601,0.474794052,0.37861779,0.290676874
     &,0.225436531,0.178189145,0.143385857,0.117090219
     &,0.0970965611,0.0813718447,0.0688025093,0.0587282625
     &,0.0505365505,0.0437667116,0.0381480078,0.0334200864
     &,0.0293980061,0.0259954988,0.0230671969,0.0205563696
     &,0.018369799,0.0164914645,0.0148223345,0.0133661413
     &,0.0120803537,0.0109553518,0.00993112632,0.00902860121
     &,0.00823165086,0.00750867707,0.00686390028,0.00628489954
     &,0.00576594302,0.00530025826,0.00487865668,0.00448219105
     &,0.00412288917,0.0037990359,0.00350595785,0.00323626
     &,0.00299598376,0.00277450326,0.0025670527,0.00238383183
     &,0.00221301686,0.00205732144,0.00191490608,0.00178434453
     &,0.00166378809,0.000843030205,0.00045133535,0.000249366374
     &,0.000143099279,8.45455239e-05,5.07170801e-05/
      data w082tev/4.50808001,4.24885868,3.08241188,2.17943586
     &,1.57311921,1.16661615,0.884242413,0.684341404
     &,0.53956918,0.431990569,0.350160865,0.28735006
     &,0.238283777,0.199386981,0.168330933,0.143183897
     &,0.122486497,0.105537809,0.0913851921,0.0795724338
     &,0.0696181381,0.0612032262,0.0539694934,0.0478227103
     &,0.042495508,0.037923619,0.0338846304,0.0304042531
     &,0.0273869787,0.024693241,0.0223146557,0.0202331989
     &,0.0183701945,0.0167341797,0.0152689623,0.0139524798
     &,0.01277172,0.011712132,0.0107536446,0.00989163849
     &,0.00912361626,0.00841391411,0.007775168,0.00719377243
     &,0.00666441312,0.00618314917,0.00574621583,0.00534415515
     &,0.00498002065,0.00256684728,0.00143445606,0.00085012108
     &,0.00052926149,0.00034151958,0.000228038282/
      data w992tev/0.93854443,1.04321939,0.859004564,0.672162707
     &,0.527078604,0.419583046,0.339189794,0.278180394
     &,0.231237543,0.194274302,0.164750824,0.141139753
     &,0.121816494,0.105821119,0.0926021541,0.0815388204
     &,0.0720695308,0.0640431097,0.0571448706,0.051235008
     &,0.0460821643,0.0416162558,0.0376435049,0.034185737
     &,0.0311075786,0.0284041954,0.0259539114,0.0238033233
     &,0.0218978026,0.020147945,0.0185724886,0.017164088
     &,0.0158751353,0.0147237724,0.0136721495,0.0127073927
     &,0.0118255184,0.0110211664,0.0102802762,0.00960051964
     &,0.00898706207,0.00840924139,0.00787911153,0.00739043908
     &,0.00693878604,0.00652184796,0.00613872405,0.00578035389
     &,0.00545249173,0.00312857904,0.00189699162,0.00119995262
     &,0.000780374589,0.000521773087,0.000358584238/
      data w087tev/26.2229848,25.6820181,19.8223018,14.6792279
     &,10.9597585,8.34728216,6.47644025,5.11633807
     &,4.10876394,3.34432605,2.75146255,2.28875877
     &,1.92188273,1.62706481,1.39031215,1.19654128
     &,1.03491986,0.901671962,0.789194861,0.694519727
     &,0.613990774,0.544881864,0.484983177,0.433952554
     &,0.389222647,0.350340683,0.315452319,0.28548022
     &,0.259354259,0.235725696,0.214573041,0.196222427
     &,0.179453639,0.164674158,0.151375244,0.139343546
     &,0.128495487,0.118660676,0.109706533,0.101648345
     &,0.0943658239,0.0875968928,0.0814802165,0.0758453788
     &,0.0707202175,0.0660622511,0.0617765552,0.0578017635
     &,0.0541739758,0.0295872255,0.0174152416,0.0108418319
     &,0.0070674115,0.00475900584,0.00330989969/
      data w997tev/3.34322208,3.81300035,3.29362864,2.67120239
     &,2.14668506,1.73684005,1.42125932,1.1767706
     &,0.986959927,0.835753557,0.713521743,0.614726373
     &,0.533703502,0.465671323,0.409443304,0.361999065
     &,0.321199576,0.286636834,0.256776187,0.231082138
     &,0.208676151,0.189125315,0.171699606,0.156681308
     &,0.143235722,0.131320898,0.120360467,0.110868038
     &,0.102436001,0.0946311292,0.0875305325,0.0812985324
     &,0.0754554391,0.070259424,0.0655225883,0.0611578114
     &,0.0571673431,0.0534988331,0.050117436,0.0470392434
     &,0.0442204833,0.0415707568,0.039135991,0.0368657097
     &,0.0347815683,0.0328756617,0.0310908665,0.0294140862
     &,0.0278724385,0.0168315505,0.010807411,0.00727037114
     &,0.00507732128,0.00363932147,0.00267802307/
      data w0814tev/68.665682,67.3278534,53.1323246,40.1691014
     &,30.4349412,23.4784377,18.4182465,14.6782059
     &,11.8817386,9.7396937,8.06382385,6.75025679
     &,5.70451537,4.85808855,4.17545707,3.61266819
     &,3.14008776,2.74898287,2.4167472,2.1363919
     &,1.8968434,1.68982985,1.50977697,1.35633075
     &,1.22128211,1.10307124,0.996282502,0.90493462
     &,0.825067984,0.752393159,0.68693462,0.630419449
     &,0.578275745,0.532152596,0.490691706,0.453085298
     &,0.419107971,0.388168165,0.359941797,0.334630512
     &,0.311544774,0.290084105,0.270616402,0.25254155
     &,0.236195488,0.221308,0.207512914,0.194654072
     &,0.182863731,0.102218208,0.0614619464,0.0390455127
     &,0.0259049339,0.0177460302,0.0125430191/
      data w9914tev/6.73299769,7.70815271,6.78047886,5.58881194
     &,4.55199175,3.72525078,3.08033676,2.57162098
     &,2.17160756,1.84967132,1.58406557,1.36798433
     &,1.19059555,1.04159131,0.918517402,0.814217598
     &,0.724255506,0.647984696,0.581901776,0.525022001
     &,0.475344066,0.431788197,0.392864136,0.359447922
     &,0.329452669,0.30267763,0.277900604,0.256472539
     &,0.237373467,0.219650243,0.203445452,0.189326877
     &,0.175942949,0.164020519,0.153208402,0.14321315
     &,0.134075014,0.125646142,0.117867221,0.110836632
     &,0.104326103,0.0982149848,0.0925858159,0.087297982
     &,0.0825021461,0.078122463,0.0739861895,0.0700870292
     &,0.0664907481,0.0406797702,0.0264113967,0.0179650277
     &,0.0126812116,0.00918910028,0.00683667762/
      data w08rtev/0.655513647,0.565184359,0.377147933,0.252097653
     &,0.174383367,0.124954371,0.0920025168,0.0694325802
     &,0.053428767,0.0418272952,0.0332306697,0.0267474527
     &,0.0217659995,0.017891072,0.0148440035,0.0124124428
     &,0.0104527164,0.00886519022,0.0075587313,0.00648457015
     &,0.00558846359,0.00484690827,0.004214955,0.003681901
     &,0.00322805687,0.00284311892,0.00250950102,0.00222335429
     &,0.00197703993,0.00176070175,0.0015728036,0.00140809885
     &,0.00126454787,0.00113841639,0.00102700341,0.000927964764
     &,0.000839872735,0.000761848517,0.000692254412,0.000629774846
     &,0.000574686477,0.000524685194,0.00047949004,0.000439126272
     &,0.000402786459,0.000369664334,0.000340141273,0.000313161653
     &,0.000288858142,0.00013495891,6.85739574e-05,3.6841706e-05
     &,2.07134518e-05,1.20010031e-05,7.13709967e-06/
      data w99rtev/0.24936938,0.264853216,0.206520207,0.156934756
     &,0.121245405,0.0954302803,0.076577,0.0623279081
     &,0.0513295499,0.0427696576,0.0359480088,0.0304546633
     &,0.0259722917,0.0222914771,0.01921119,0.0166352986
     &,0.0144704817,0.0126448516,0.0110840735,0.0097133745
     &,0.00853374798,0.00753841977,0.00666416483,0.00590982286
     &,0.00525475322,0.00468763425,0.00418692029,0.00375005105
     &,0.00336725115,0.00301042129,0.00269730742,0.00242046523
     &,0.00217833037,0.00196309976,0.00177216285,0.00160120097
     &,0.00144833212,0.00131247958,0.00119137607,0.00107794808
     &,0.000978040997,0.000887839534,0.000805908267,0.000733253694
     &,0.000668017152,0.000608575917,0.000555883425,0.000507872781
     &,0.000464665762,0.000195234183,8.72225097e-05,4.07993017e-05
     &,1.99478384e-05,1.00828669e-05,5.28233279e-06/
      data w0810tev/42.9462064,42.2009153,32.975754,24.6789659
     &,18.5862481,14.2532661,11.1240395,8.82872663
     &,7.11716339,5.81247536,4.79871915,4.00679052
     &,3.37855502,2.87150033,2.46224528,2.12520879
     &,1.84249686,1.60851553,1.41006499,1.24286876
     &,1.10064888,0.978415937,0.872478356,0.782361743
     &,0.703319499,0.634464259,0.572527873,0.519426758
     &,0.473006175,0.430828361,0.39290118,0.360048897
     &,0.329843892,0.303153295,0.279122665,0.257336346
     &,0.237658805,0.219766953,0.203520261,0.188683845
     &,0.17562978,0.163268745,0.151933916,0.141812666
     &,0.132247427,0.123895246,0.115849383,0.108652679
     &,0.101816637,0.0564003616,0.0335526515,0.0211546856
     &,0.0138740213,0.00945115006,0.0066352406/
      data w9910tev/4.78949492,5.47221629,4.77776287,3.90683404
     &,3.16532604,2.57897551,2.12481227,1.76454956
     &,1.48378144,1.25804622,1.07512234,0.927084928
     &,0.80567552,0.704426821,0.620714176,0.549879713
     &,0.488826665,0.43705595,0.392232844,0.353293539
     &,0.319327057,0.289630878,0.263139865,0.240363259
     &,0.219950993,0.201804618,0.185069917,0.170663572
     &,0.157845635,0.145962517,0.135122365,0.125645771
     &,0.116707712,0.108753921,0.101519522,0.0948421832
     &,0.0887369862,0.0831135798,0.077949983,0.0731645986
     &,0.0689317376,0.0648078562,0.0610073536,0.0575874934
     &,0.0542971373,0.051423689,0.048586444,0.0460500076
     &,0.0435875253,0.0265251668,0.0171263501,0.0116131827
     &,0.00813046214,0.00587764345,0.004358583/



         if(eflag.eq.2)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w992tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w082tev(k)
               enddo 
            endif

         elseif(eflag.eq.7)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w997tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w087tev(k)
               enddo 
            endif

         elseif(eflag.eq.10)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9910tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0810tev(k)
               enddo 
            endif

         elseif(eflag.eq.14)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9914tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0814tev(k)
               enddo 
            endif

         elseif(eflag.eq.500)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w99rtev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w08rtev(k)
               enddo 
            endif

         elseif(eflag.eq.300)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9903tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0803tev(k)
               enddo 
            endif

         elseif(eflag.eq.900)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9909tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0809tev(k)
               enddo 
            endif

         endif

      if(mgam.lt.50d0)then

      v=nint((mgam-2d0)/1d0)+1
      v1=((mgam-2d0)/1d0)+1d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      else

      v=nint((mgam-50d0)/10d0)+49
      v1=((mgam-50d0)/10d0)+49d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      endif

         m=(dlog(warr(v+1))-dlog(warr(v)))/
     &        (rm(v+1)-rm(v))
         wl=m*(mgam-rm(v))+dlog(warr(v))

      wl=dexp(wl)
    
      return
      end

      subroutine lumpijz2(mgam,wl)

      double precision wl,mgam,m,v1
      double precision warr(55),rm(55),w082tev(55),w087tev(55)
     &,w08rtev(55),w0814tev(55),w992tev(55),w997tev(55),w99rtev(55)
     &,w9914tev(55),w0810tev(55),w9910tev(55),w0803tev(55),w0809tev(55)
     &,w9903tev(55),w9909tev(55)
      integer v,i,k,pdf,eflag
      common/flags/pdf,eflag
      save
      data rm/2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,
     &  16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.,27.,28.,29.,
     &  30.,31.,32.,33.,34.,35.,36.,37.,38.,39.,40.,41.,42.,43.,
     &  44.,45.,46.,47.,48.,49.,50.,60.,70.,80.,90.,100.,110/
      data w9903tev/0.00588045959,0.00594558394,0.00451027744,0.0061842
     &0772,0.00279970919,0.00225146707,0.00185783765,0.00271105272
     &,0.00132994028,0.00108725166,0.00095539563,0.00142051972
     &,0.000690769828,0.000587268192,0.000521169782,0.000841673343
     &,0.000385217622,0.0003339439,0.000288916581,0.000565044277
     &,0.000219777417,0.000190582665,0.000167296007,0.000381183919
     &,0.000125921455,0.000107981695,9.57079596e-05,0.000269902558
     &,7.24608014e-05,6.39652704e-05,5.56918609e-05,0.000189937719
     &,4.3178118e-05,3.76627502e-05,3.31066e-05,0.000146191091
     &,2.60141989e-05,2.29100703e-05,2.01445459e-05,0.00010856997
     &,1.59590873e-05,1.38580351e-05,1.21963483e-05,8.17885882e-05
     &,9.69420343e-06,8.61302613e-06,7.72609899e-06,6.4146913e-05
     &,6.11614998e-06,2.03583734e-06,7.29979752e-07,1.00296312e-05
     &,1.05996382e-07,4.04407797e-08,1.52269763e-08/
      data w9909tev/0.0112648762,0.0122645424,0.00934673049,0.0061842077
     &,0.005261992,0.00396988513,0.0032230939,0.00271105272
     &,0.00230508415,0.00183910877,0.00165704129,0.00142051972
     &,0.00121153786,0.0010564061,0.000978074559,0.000841673343
     &,0.000775244768,0.000694971037,0.00062597727,0.000565044277
     &,0.000523345021,0.00047061439,0.000442859803,0.000381183919
     &,0.000360055485,0.000316232054,0.000297064383,0.000269902558
     &,0.000250760039,0.000239591729,0.000215500913,0.000189937719
     &,0.000186465396,0.000167069535,0.000159842548,0.000146191091
     &,0.000137897416,0.000126909591,0.000117913322,0.00010856997
     &,0.00010311896,9.29003349e-05,8.66154415e-05,8.17885882e-05
     &,7.74630738e-05,7.20402539e-05,6.8582287e-05,6.4146913e-05
     &,5.971171e-05,3.20888673e-05,1.8000709e-05,1.00296312e-05
     &,6.03868196e-06,3.54137469e-06,2.14306724e-06/
      data w0803tev/0.0119636919,0.00947689144,0.0058848163,0.01461226
     &,0.00266486372,0.00187481379,0.00137744583,0.00451130588
     &,0.00082585123,0.000627552306,0.000518252389,0.00179236631
     &,0.000343095286,0.000280828962,0.000243793991,0.000864795435
     &,0.000171641493,0.000146638459,0.000126284312,0.000490935132
     &,9.62177291e-05,8.35248244e-05,7.37755742e-05,0.000288285371
     &,5.65634429e-05,4.88730662e-05,4.41279364e-05,0.000181434628
     &,3.4624308e-05,3.13386075e-05,2.78735492e-05,0.000115407909
     &,2.26729538e-05,2.01701755e-05,1.83197594e-05,8.22725924e-05
     &,1.51641317e-05,1.37731798e-05,1.24496036e-05,5.8228789e-05
     &,1.04706108e-05,9.3485678e-06,8.49774094e-06,4.18363771e-05
     &,7.19040031e-06,6.57867699e-06,6.09520743e-06,3.17284867e-05
     &,5.12541971e-06,2.30558208e-06,1.08418049e-06,4.82888638e-06
     &,2.55003102e-07,1.22073814e-07,5.78739241e-08/
      data w0809tev/0.0416380002,0.0374264433,0.0244362722,0.01461226
     &,0.0112068343,0.00776684043,0.00571578682,0.00451130588
     &,0.00352047834,0.00264447281,0.00221778411,0.00179236631
     &,0.00145536001,0.00119407814,0.00106242014,0.000864795435
     &,0.000766619483,0.000648626834,0.000564836361,0.000490935132
     &,0.000437652261,0.00038215331,0.000348035191,0.000288285371
     &,0.000263268561,0.000222430086,0.000205267596,0.000181434628
     &,0.000165135843,0.00015528454,0.000136292472,0.000115407909
     &,0.000112597189,9.75517961e-05,9.31322993e-05,8.22725924e-05
     &,7.76816148e-05,6.99362626e-05,6.48934113e-05,5.8228789e-05
     &,5.47333185e-05,4.8562505e-05,4.47755574e-05,4.18363771e-05
     &,3.96634897e-05,3.62876687e-05,3.44405138e-05,3.17284867e-05
     &,2.93171984e-05,1.50792727e-05,8.46170629e-06,4.82888638e-06
     &,3.09147072e-06,1.94704124e-06,1.28410201e-06/
      data w082tev/0.103069016,0.099581197,0.0676373613,0.0392377423
     &,0.0316803451,0.0217248584,0.0161276137,0.0127924023
     &,0.0102571735,0.00764030158,0.00647794722,0.00522200826
     &,0.00415026427,0.00344518921,0.00309391061,0.0025782975
     &,0.00226865643,0.00191900673,0.00171263368,0.00147128846
     &,0.00130861361,0.00114350916,0.00106404536,0.000865637125
     &,0.000788627063,0.000661710931,0.000617587765,0.000558417586
     &,0.000508991555,0.000498151041,0.000432325027,0.000348874178
     &,0.000350933956,0.000293728785,0.000296196218,0.000245172288
     &,0.000249284023,0.000213993646,0.000212706466,0.000199051731
     &,0.000160524738,0.000169372415,0.00014651007,0.000135561719
     &,0.000127638199,0.000123342714,0.000107130736,0.000103939343
     &,9.61294647e-05,4.78257025e-05,2.70320532e-05,1.70909867e-05
     &,1.06582858e-05,7.06595825e-06,4.84275315e-06/
      data w992tev/0.0199269999,0.0228925963,0.0179348011,0.0112226836
     &,0.009935106,0.00718564456,0.00583791747,0.00480729404
     &,0.0041750555,0.00323038085,0.00292063512,0.00245951775
     &,0.00199136575,0.00174299594,0.00159737513,0.0014166133
     &,0.00128071188,0.001143145,0.00105232437,0.000923513199
     &,0.00085198284,0.000755333951,0.000737943483,0.00061368678
     &,0.000581049132,0.000505553973,0.000476711336,0.000445585147
     &,0.000413920922,0.000417605109,0.000367352023,0.000308587104
     &,0.000314161878,0.000269496578,0.000275795259,0.000235722734
     &,0.000242827506,0.000213650873,0.00021318067,0.000203993725
     &,0.000170866209,0.000180454573,0.000159054949,0.000151168296
     &,0.000143598463,0.000141253162,0.000124689013,0.000123399485
     &,0.000116098546,6.6441803e-05,4.17924109e-05,2.85418356e-05
     &,1.88207735e-05,1.31261968e-05,9.41800331e-06/
      data w087tev/0.468421516,0.489979928,0.353932147,0.196407533
     &,0.173809649,0.116634245,0.0891651299,0.0692431506
     &,0.0605578864,0.0450797139,0.0386547352,0.0313243051
     &,0.0227602028,0.0197429257,0.017579818,0.0159744573
     &,0.0131088342,0.0114799784,0.0110407469,0.00896685949
     &,0.00786619904,0.0068147725,0.00645446448,0.00527379787
     &,0.00456143064,0.0040385168,0.00372646773,0.00368400447
     &,0.00329142959,0.00351050248,0.00310112958,0.002206416
     &,0.00225539453,0.00172231861,0.00201458989,0.00144100974
     &,0.00175133979,0.00130253079,0.00149657984,0.00144046692
     &,0.000999721436,0.00120406141,0.000979519006,0.00093529018
     &,0.000827180105,0.000911695376,0.000791962659,0.000766250474
     &,0.000706921855,0.00031102297,0.000191426307,0.000119537798
     &,8.85527749e-05,5.26465733e-05,3.90924876e-05/
      data w997tev/0.0548303078,0.0683355745,0.0565289166,0.0329278875
     &,0.0320723821,0.0218813142,0.01870815,0.0143885315
     &,0.0139315447,0.0107299849,0.00973002363,0.00817313142
     &,0.00573847099,0.00517357656,0.00457415965,0.00452197024
     &,0.00366814578,0.00347303052,0.00347092516,0.00275072799
     &,0.0024642144,0.00208689583,0.00214428805,0.00175076467
     &,0.00155046004,0.00148246643,0.00131868887,0.0013809348
     &,0.00121864869,0.00138836818,0.0012326742,0.000893051079
     &,0.000897357014,0.000668153897,0.000829819704,0.000575976367
     &,0.000758962667,0.000552826311,0.000649839308,0.000643614046
     &,0.000439194139,0.000546778311,0.00043231487,0.000441864781
     &,0.000376584576,0.000441739997,0.000388867905,0.000385374361
     &,0.000359728259,0.000168861917,0.000113937557,7.6192457e-05
     &,6.26908371e-05,3.96023615e-05,3.25024006e-05/
      data w0814tev/1.11460637,1.17636553,0.869755002,0.472381866
     &,0.438323859,0.29004741,0.22661431,0.17082894
     &,0.159463649,0.119873768,0.102831258,0.0839456096
     &,0.0564901454,0.0508175647,0.0443121559,0.0433091112
     &,0.0332293173,0.0302774931,0.0307202211,0.0237309199
     &,0.0206154229,0.0176578664,0.0166258682,0.013908035
     &,0.0113239776,0.0108311383,0.00971711946,0.0103859836
     &,0.00901242951,0.0101662944,0.00928292781,0.00604015342
     &,0.00610050642,0.00429241519,0.0057034659,0.00371775963
     &,0.00503940101,0.00338362387,0.00429139596,0.00421492661
     &,0.0026972415,0.00350034927,0.00274899584,0.0026942443
     &,0.00220970906,0.0027275504,0.0024223359,0.00231253367
     &,0.00214155349,0.000838320695,0.000552181751,0.000328184517
     &,0.000283858024,0.000148798733,0.000120239435/
      data w9914tev/0.0969680238,0.126231343,0.107108189,0.0604310303
     &,0.0622560173,0.0412340358,0.0367700811,0.0267839498
     &,0.0282496278,0.0221619337,0.020038769,0.0169607064
     &,0.0107841943,0.00987987362,0.00848434117,0.0090984566
     &,0.00676044698,0.00684990345,0.00724359461,0.00534400541
     &,0.00471109919,0.00386988834,0.0040387415,0.00334986537
     &,0.00274989783,0.00299083189,0.00250501238,0.00288618704
     &,0.00241344557,0.00297529714,0.00275357422,0.00181043889
     &,0.00175417641,0.00116384272,0.00170278101,0.00103635038
     &,0.00158321614,0.00102930857,0.00134031219,0.00134829975
     &,0.000835026315,0.00113008745,0.00084183135,0.000914685142
     &,0.000689906091,0.000941756679,0.000858009715,0.00083604073
     &,0.000784263879,0.000311449649,0.000217437491,0.000133456095
     &,0.000131857438,6.96691043e-05,6.46310971e-05/
      data w08rtev/0.0212360988,0.0178923935,0.0113551447,0.0069467955
     &,0.00515825857,0.00360378174,0.00264608525,0.00206938547
     &,0.0016023654,0.00121172013,0.00100780065,0.000817263025
     &,0.00066676548,0.000545752453,0.000479789881,0.000388647017
     &,0.000341726226,0.000290861213,0.000251252749,0.000218414502
     &,0.000193842671,0.000168857921,0.000151184463,0.000127530069
     &,0.000115597811,9.8976347e-05,9.02986529e-05,7.92350207e-05
     &,7.17441126e-05,6.5944977e-05,5.8467322e-05,5.06963057e-05
     &,4.81216818e-05,4.24715956e-05,3.93723613e-05,3.58316369e-05
     &,3.27990498e-05,2.99097633e-05,2.75594887e-05,2.51991928e-05
     &,2.25916252e-05,2.15604505e-05,1.94326579e-05,1.78474711e-05
     &,1.66553419e-05,1.54686474e-05,1.37555885e-05,1.30923578e-05
     &,1.21657292e-05,5.95980256e-06,3.13671222e-06,1.80651918e-06
     &,1.01973028e-06,6.17025018e-07,3.80281037e-07/
      data w99rtev/0.00767745773,0.00802659534,0.00606379447,0.004269156
     &,0.00358830276,0.00280386987,0.0023057589,0.00196051408
     &,0.00166294496,0.00135873376,0.0012186039,0.00105690887
     &,0.000912608468,0.00079270403,0.00072575939,0.00061959475
     &,0.000563065818,0.000503770847,0.000449351122,0.000401771748
     &,0.000364319015,0.000324342894,0.000295206388,0.000257015045
     &,0.00023676917,0.000208319075,0.000191857278,0.000171067049
     &,0.000156239307,0.000143361355,0.000128164266,0.000113124349
     &,0.000106184509,9.4974553e-05,8.69867474e-05,8.00175817e-05
     &,7.27981222e-05,6.63555876e-05,6.06943567e-05,5.49534962e-05
     &,4.96282354e-05,4.63400108e-05,4.16911415e-05,3.80787673e-05
     &,3.50920292e-05,3.2357832e-05,2.86369021e-05,2.68797809e-05
     &,2.47392167e-05,1.05223453e-05,4.67998076e-06,2.24136561e-06
     &,1.05978736e-06,5.36905803e-07,2.81136399e-07/
      data w0810tev/0.726164046,0.767878361,0.561782358,0.308079036
     &,0.279748082,0.186438896,0.14407952,0.110358538
     &,0.0996400841,0.0744553658,0.0638849046,0.0519794954
     &,0.0364074194,0.0321691011,0.0283906845,0.0267329061
     &,0.0212322816,0.0189383899,0.0186932356,0.0148083469
     &,0.0129215962,0.0111325857,0.010528732,0.00869008982
     &,0.00731058927,0.00670314268,0.00611110715,0.00627648528
     &,0.00553624547,0.00607485849,0.00544987873,0.00370800563
     &,0.00377409667,0.00277302017,0.0034465396,0.00235215822
     &,0.00302279031,0.00213593294,0.00240960409,0.00198195694
     &,0.00195235166,0.00179484378,0.00115025561,0.00158888667
     &,0.00146508651,0.00118077766,0.00141782821,0.00107085382
     &,0.00102250402,0.000583065967,0.000351686659,0.000178940144
     &,0.000153863934,8.20851255e-05,8.04095215e-05/
      data w9910tev/0.0734244448,0.093557251,0.0784527783,0.044948154
     &,0.0450826398,0.0303117704,0.0264747444,0.0198013661
     &,0.0200001866,0.0155140457,0.0140506182,0.0118454338
     &,0.00792764211,0.00720202863,0.00627651872,0.00645844142
     &,0.00501352687,0.00490307757,0.00504836924,0.00385613352
     &,0.00342741723,0.00286039093,0.00296886899,0.00244022676
     &,0.00208907228,0.00211695551,0.00183241271,0.00200731218
     &,0.0017276479,0.00204558719,0.00185040034,0.00127651543
     &,0.00126108743,0.000888354231,0.00119291828,0.00077422548
     &,0.00110146408,0.000755498619,0.000860264932,0.000706151747
     &,0.000740371292,0.000684233189,0.000379774338,0.000615061672
     &,0.000561928005,0.000454777349,0.000590119393,0.000430866101
     &,0.000406065124,0.00027270092,0.000173200323,9.02929357e-05
     &,9.11027612e-05,4.79733969e-05,5.08224578e-05/
      
         if(eflag.eq.2)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w992tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w082tev(k)
               enddo 
            endif

         elseif(eflag.eq.7)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w997tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w087tev(k)
               enddo 
            endif

         elseif(eflag.eq.10)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9910tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0810tev(k)
               enddo 
            endif

         elseif(eflag.eq.14)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9914tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0814tev(k)
               enddo 
            endif

         elseif(eflag.eq.500)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w99rtev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w08rtev(k)
               enddo 
            endif

         elseif(eflag.eq.300)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9903tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0803tev(k)
               enddo 
            endif

         elseif(eflag.eq.900)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9909tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0809tev(k)
               enddo 
            endif

         endif

      if(mgam.lt.50d0)then

      v=nint((mgam-2d0)/1d0)+1
      v1=((mgam-2d0)/1d0)+1d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      else

      v=nint((mgam-50d0)/10d0)+49
      v1=((mgam-50d0)/10d0)+49d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      endif

         m=(dlog(warr(v+1))-dlog(warr(v)))/
     &        (rm(v+1)-rm(v))
         wl=m*(mgam-rm(v))+dlog(warr(v))

      wl=dexp(wl)
    
      return
      end


c     calculates lorentzian dot product for real 4-vectors

      double precision function rdot(a,b)
      double precision a(4),b(4)

      rdot=a(4)*b(4)-a(3)*b(3)-a(2)*b(2)-a(1)*b(1)

      return
      end

c     calculates lorentzian dot product for real 4-vectors

      subroutine dot(v1,v2,dt)
      double precision v1(4),v2(4),dt

      dt=-(v1(1)*v2(1)+v1(2)*v2(2)+v1(3)*v2(3)
     &-v1(4)*v2(4))

      return
      end

c     calculates lorentzian dot product for complex 4-vectors

      subroutine cdot(v1,v2,dt)
      complex*16 v1(4),v2(4)
      complex*16 dt

       dt=-(v1(1)*v2(1)+v1(2)*v2(2)+v1(3)*v2(3)
     &-v1(4)*v2(4))

      return
      end


c     interpolates dsig/dy for rts=14 tev


      subroutine yint14(out)
      implicit double precision (a-y)
      double precision yarrc(34),yarrb(34),yarr15(34),yarr20(34),
     &yarr40(34)
      integer yi,r,j
      data yarrc/0.1000000e+01,0.9965862e+00,0.9871170e+00
     &,0.9749448e+00,0.9533667e+00,0.9289191e+00,0.8993025e+00
     &,0.8638354e+00,0.8283355e+00,0.7864793e+00,0.7395991e+00
     &,0.6948727e+00,0.6433354e+00,0.5935635e+00,0.5446861e+00
     &,0.4937801e+00,0.4427451e+00,0.3940202e+00,0.3475776e+00
     &,0.3023724e+00,0.2603749e+00,0.2204498e+00,0.1838319e+00
     &,0.1514104e+00,0.1216417e+00,0.9534277e-01,0.7198532e-01
     &,0.5212556e-01,0.3540999e-01,0.2212058e-01,0.1224154e-01
     &,0.5713542e-02,0.2084297e-02,0.3578895e-03/
      data yarrb/0.1000000e+01,0.9962374e+00,0.9848566e+00
     &,0.9690764e+00,0.9433815e+00,0.9133903e+00,0.8770868e+00
     &,0.8349043e+00,0.7917451e+00,0.7417213e+00,0.6872421e+00
     &,0.6347271e+00,0.5760491e+00,0.5200355e+00,0.4654468e+00
     &,0.4100556e+00,0.3561599e+00,0.3057645e+00,0.2585964e+00
     &,0.2143878e+00,0.1742866e+00,0.1373668e+00,0.1046924e+00
     &,0.7661551e-01,0.5244752e-01,0.3298869e-01,0.1830539e-01
     &,0.8580266e-02,0.3130408e-02,0.1205917e-04,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr15/0.1000000e+01,0.9953004e+00,0.9826911e+00
     &,0.9656834e+00,0.9373712e+00,0.9049406e+00,0.8662532e+00
     &,0.8199840e+00,0.7731779e+00,0.7200664e+00,0.6620413e+00
     &,0.6062655e+00,0.5450849e+00,0.4863567e+00,0.4295863e+00
     &,0.3730496e+00,0.3183425e+00,0.2678748e+00,0.2211517e+00
     &,0.1776556e+00,0.1385921e+00,0.1035087e+00,0.7324384e-01
     &,0.4827382e-01,0.2850271e-01,0.1453294e-01,0.5982276e-02
     &,0.1809851e-02,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr20/0.1000000e+01,0.9957086e+00,0.9826150e+00
     &,0.9638819e+00,0.9343731e+00,0.8997033e+00,0.8577505e+00
     &,0.8095536e+00,0.7598938e+00,0.7036612e+00,0.6430619e+00
     &,0.5845366e+00,0.5210583e+00,0.4608469e+00,0.4029251e+00
     &,0.3452514e+00,0.2903060e+00,0.2396753e+00,0.1931375e+00
     &,0.1503803e+00,0.1124960e+00,0.7915890e-01,0.5150437e-01
     &,0.3016664e-01,0.1511378e-01,0.6103985e-02,0.7383781e-03
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr40/0.1000000e+01,0.9945684e+00,0.9793573e+00
     &,0.9566426e+00,0.9217920e+00,0.8809438e+00,0.8323813e+00
     &,0.7764767e+00,0.7188586e+00,0.6551256e+00,0.5875767e+00
     &,0.5228144e+00,0.4543425e+00,0.3898204e+00,0.3285534e+00
     &,0.2691222e+00,0.2134694e+00,0.1633992e+00,0.1186797e+00
     &,0.8004250e-01,0.4895504e-01,0.2593047e-01,0.1130814e-01
     &,0.3758990e-02,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx

      ys=0.25d0
      yabs=dabs(yx)

      yi=yabs/0.25d0+1

      if(mx.lt.9.859d0)then
         ypp=yarrc(yi+1)
         yp=yarrc(yi)
         ypp1=yarrb(yi+1)
         yp1=yarrb(yi)
         y0=yarrc(33)
         y0p=yarrc(34)
         y01=yarrb(33)
         y0p1=yarrb(34)
         m1=3.415d0
         m2=9.859d0
      elseif(mx.lt.15d0)then
         ypp=yarrb(yi+1)
         yp=yarrb(yi)
         ypp1=yarr15(yi+1)
         yp1=yarr15(yi)
         y0=yarrb(33)
         y0p=yarrb(34)
         y01=yarr15(33)
         y0p1=yarr15(34)
         m1=9.859d0
         m2=15d0
      elseif(mx.lt.20d0)then
         ypp=yarr15(yi+1)
         yp=yarr15(yi)
         ypp1=yarr20(yi+1)
         yp1=yarr20(yi)
         y0=yarr15(33)
         y0p=yarr15(34)
         y01=yarr20(33)
         y0p1=yarr20(34)
         m1=15d0
         m2=20d0
      else
         ypp=yarr20(yi+1)
         yp=yarr20(yi)
         ypp1=yarr40(yi+1)
         yp1=yarr40(yi)
         y0=yarr20(33)
         y0p=yarr20(34)
         y01=yarr40(33)
         y0p1=yarr40(34)
         m1=20d0
         m2=40d0
      endif


      if(yabs.lt.8.25d0)then
         my=(ypp-yp)/ys
         out1=my*(yabs-(yi-1)*ys)+yp
      else
         my=(y0p-y0)/ys
         out1=my*(yabs-(30-1)*ys)+y0
         if(out1.lt.0d0)then
            out1=0d0
         endif
      endif

      if(yabs.lt.8.25d0)then
         my=(ypp1-yp1)/ys
         out2=my*(yabs-(yi-1)*ys)+yp1
      else
         my=(y0p1-y01)/ys
         out2=my*(yabs-(30-1)*ys)+y01
         if(out2.lt.0d0)then
            out2=0d0
         endif
      endif
     
      out=out1+(out2-out1)/(m2-m1)*(mx-m1)

      return
      end

c     interpolates dsig/dy for rts=8 tev

      subroutine yint8(out)
      implicit double precision (a-y)
      double precision yarrc(32),yarrb(32),yarr15(32),yarr20(32),
     &yarr40(32)
      integer yi,r,j
      data yarrc/0.1000000e+01,0.9959896e+00,0.9853430e+00
     &,0.9717281e+00,0.9483332e+00,0.9215462e+00,0.8892281e+00
     &,0.8504163e+00,0.8114401e+00,0.7661928e+00,0.7159354e+00
     &,0.6676252e+00,0.6127411e+00,0.5598776e+00,0.5080650e+00
     &,0.4549454e+00,0.4023107e+00,0.3525340e+00,0.3056343e+00
     &,0.2606083e+00,0.2191253e+00,0.1804777e+00,0.1456206e+00
     &,0.1149392e+00,0.8722356e-01,0.6331880e-01,0.4297696e-01
     &,0.2677286e-01,0.1467726e-01,0.6754428e-02,0.2398923e-02
     &,0.1118604e-04/
      data yarrb/0.1000000e+01,0.9953466e+00,0.9824738e+00
     &,0.9647637e+00,0.9362888e+00,0.9030448e+00,0.8629577e+00
     &,0.8159331e+00,0.7680778e+00,0.7136283e+00,0.6546755e+00
     &,0.5979616e+00,0.5357049e+00,0.4764014e+00,0.4192747e+00
     &,0.3624247e+00,0.3076585e+00,0.2571411e+00,0.2104112e+00
     &,0.1670451e+00,0.1282885e+00,0.9360334e-01,0.6404008e-01
     &,0.4022531e-01,0.2212327e-01,0.1019205e-01,0.3614673e-02
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00/
      data yarr15/0.1000000e+01,0.9948797e+00,0.9807730e+00
     &,0.9613100e+00,0.9301001e+00,0.8934525e+00,0.8499548e+00
     &,0.7989617e+00,0.7467768e+00,0.6882799e+00,0.6255284e+00
     &,0.5651177e+00,0.5002099e+00,0.4385517e+00,0.3793054e+00
     &,0.3214543e+00,0.2663359e+00,0.2156476e+00,0.1692870e+00
     &,0.1270818e+00,0.9022226e-01,0.5894432e-01,0.3447811e-01
     &,0.1735554e-01,0.6966936e-02,0.3429731e-03,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00/
      data yarr20/0.1000000e+01,0.9944915e+00,0.9794310e+00
     &,0.9582203e+00,0.9246061e+00,0.8854741e+00,0.8389283e+00
     &,0.7848932e+00,0.7294847e+00,0.6679496e+00,0.6023524e+00
     &,0.5394747e+00,0.4726127e+00,0.4089641e+00,0.3486012e+00
     &,0.2898323e+00,0.2343049e+00,0.1836445e+00,0.1379064e+00
     &,0.9725969e-01,0.6326228e-01,0.3649246e-01,0.1792767e-01
     &,0.7015277e-02,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00/
      data yarr40/0.1000000e+01,0.9930846e+00,0.9751367e+00
     &,0.9492335e+00,0.9093584e+00,0.8625950e+00,0.8077605e+00
     &,0.7448546e+00,0.6804195e+00,0.6107213e+00,0.5373818e+00
     &,0.4673684e+00,0.3950701e+00,0.3268732e+00,0.2626503e+00
     &,0.2019300e+00,0.1465340e+00,0.9876804e-01,0.5995651e-01
     &,0.3134570e-01,0.1334127e-01,0.1724133e-02,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00/
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx

      ys=0.25d0
      yabs=dabs(yx)

      yi=yabs/0.25d0+1

      if(mx.lt.9.859d0)then
         ypp=yarrc(yi+1)
         yp=yarrc(yi)
         ypp1=yarrb(yi+1)
         yp1=yarrb(yi)
         y0=yarrc(31)
         y0p=yarrc(32)
         y01=yarrb(31)
         y0p1=yarrb(32)
         m1=3.415d0
         m2=9.859d0
      elseif(mx.lt.15d0)then
         ypp=yarrb(yi+1)
         yp=yarrb(yi)
         ypp1=yarr15(yi+1)
         yp1=yarr15(yi)
         y0=yarrb(31)
         y0p=yarrb(32)
         y01=yarr15(31)
         y0p1=yarr15(32)
         m1=9.859d0
         m2=15d0
      elseif(mx.lt.20d0)then
         ypp=yarr15(yi+1)
         yp=yarr15(yi)
         ypp1=yarr20(yi+1)
         yp1=yarr20(yi)
         y0=yarr15(31)
         y0p=yarr15(32)
         y01=yarr20(31)
         y0p1=yarr20(32)
         m1=15d0
         m2=20d0
      else
         ypp=yarr20(yi+1)
         yp=yarr20(yi)
         ypp1=yarr40(yi+1)
         yp1=yarr40(yi)
         y0=yarr20(31)
         y0p=yarr20(32)
         y01=yarr40(31)
         y0p1=yarr40(32)
         m1=20d0
         m2=40d0
      endif


      if(yabs.lt.7.75d0)then
         my=(ypp-yp)/ys
         out1=my*(yabs-(yi-1)*ys)+yp
      else
         my=(y0p-y0)/ys
         out1=my*(yabs-(30-1)*ys)+y0
         if(out1.lt.0d0)then
            out1=0d0
         endif
      endif

      if(yabs.lt.7.75d0)then
         my=(ypp1-yp1)/ys
         out2=my*(yabs-(yi-1)*ys)+yp1
      else
         my=(y0p1-y01)/ys
         out2=my*(yabs-(30-1)*ys)+y01
         if(out2.lt.0d0)then
            out2=0d0
         endif
      endif
     
      out=out1+(out2-out1)/(m2-m1)*(mx-m1)

      return
      end

c     interpolates dsig/dy for rts=7 tev

      subroutine yint7(out)
      implicit double precision (a-y)
      double precision yarrc(31),yarrb(31),yarr15(31),yarr20(31),
     &yarr40(31)
      integer yi,r,j
      data yarrc/0.1000000e+01,0.9957492e+00,0.9849075e+00
     &,0.9710111e+00,0.9471068e+00,0.9196249e+00,0.8864688e+00
     &,0.8467178e+00,0.8069319e+00,0.7607784e+00,0.7094412e+00
     &,0.6603871e+00,0.6047615e+00,0.5511748e+00,0.4987120e+00
     &,0.4451705e+00,0.3922184e+00,0.3422689e+00,0.2952671e+00
     &,0.2502251e+00,0.2090242e+00,0.1706929e+00,0.1362079e+00
     &,0.1059739e+00,0.7891199e-01,0.5574772e-01,0.3640038e-01
     &,0.2145549e-01,0.1086938e-01,0.4461758e-02,0.1339749e-02/
      data yarrb/0.1000000e+01,0.9953765e+00,0.9820576e+00
     &,0.9635778e+00,0.9345158e+00,0.9003370e+00,0.8587669e+00
     &,0.8108082e+00,0.7617975e+00,0.7060472e+00,0.6461191e+00
     &,0.5884110e+00,0.5252661e+00,0.4653056e+00,0.4076716e+00
     &,0.3502781e+00,0.2955337e+00,0.2449979e+00,0.1983079e+00
     &,0.1551946e+00,0.1168551e+00,0.8287485e-01,0.5445483e-01
     &,0.3227655e-01,0.1636318e-01,0.6703386e-02,0.1401943e-02
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr15/0.1000000e+01,0.9946336e+00,0.9800901e+00
     &,0.9598986e+00,0.9273125e+00,0.8895715e+00,0.8450872e+00
     &,0.7927925e+00,0.7392674e+00,0.6797333e+00,0.6158585e+00
     &,0.5543705e+00,0.4886942e+00,0.4261671e+00,0.3664505e+00
     &,0.3082638e+00,0.2529020e+00,0.2021358e+00,0.1560074e+00
     &,0.1142714e+00,0.7832720e-01,0.4869762e-01,0.2653406e-01
     &,0.1206526e-01,0.4177343e-02,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr20/0.1000000e+01,0.9946387e+00,0.9792261e+00
     &,0.9569081e+00,0.9227133e+00,0.8824711e+00,0.8343918e+00
     &,0.7790738e+00,0.7221909e+00,0.6590160e+00,0.5922283e+00
     &,0.5281151e+00,0.4599517e+00,0.3958153e+00,0.3349237e+00
     &,0.2757330e+00,0.2200506e+00,0.1694889e+00,0.1240782e+00
     &,0.8439911e-01,0.5213658e-01,0.2794279e-01,0.1235301e-01
     &,0.4145441e-02,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      data yarr40/0.1000000e+01,0.9195388e+00,0.9174350e+00
     &,0.8577421e+00,0.8225827e+00,0.8426238e+00,0.7316099e+00
     &,0.6694060e+00,0.6149684e+00,0.5468037e+00,0.4926762e+00
     &,0.4392537e+00,0.3669131e+00,0.2929776e+00,0.2291817e+00
     &,0.1823953e+00,0.1224679e+00,0.7467013e-01,0.4276049e-01
     &,0.2068876e-01,0.7399211e-02,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00
     &,0.0000000e+00,0.0000000e+00,0.0000000e+00,0.0000000e+00/
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx

      ys=0.25d0
      yabs=dabs(yx)

      yi=yabs/0.25d0+1

      if(mx.lt.9.859d0)then
         ypp=yarrc(yi+1)
         yp=yarrc(yi)
         ypp1=yarrb(yi+1)
         yp1=yarrb(yi)
         y0=yarrc(30)
         y0p=yarrc(31)
         y01=yarrb(30)
         y0p1=yarrb(31)
         m1=3.415d0
         m2=9.859d0
      elseif(mx.lt.15d0)then
         ypp=yarrb(yi+1)
         yp=yarrb(yi)
         ypp1=yarr15(yi+1)
         yp1=yarr15(yi)
         y0=yarrb(30)
         y0p=yarrb(31)
         y01=yarr15(30)
         y0p1=yarr15(31)
         m1=9.859d0
         m2=15d0
      elseif(mx.lt.20d0)then
         ypp=yarr15(yi+1)
         yp=yarr15(yi)
         ypp1=yarr20(yi+1)
         yp1=yarr20(yi)
         y0=yarr15(30)
         y0p=yarr15(31)
         y01=yarr20(30)
         y0p1=yarr20(31)
         m1=15d0
         m2=20d0
      else
         ypp=yarr20(yi+1)
         yp=yarr20(yi)
         ypp1=yarr40(yi+1)
         yp1=yarr40(yi)
         y0=yarr20(30)
         y0p=yarr20(31)
         y01=yarr40(30)
         y0p1=yarr40(31)
         m1=20d0
         m2=40d0
      endif


      if(yabs.lt.7.5d0)then
         my=(ypp-yp)/ys
         out1=my*(yabs-(yi-1)*ys)+yp
      else
         my=(y0p-y0)/ys
         out1=my*(yabs-(30-1)*ys)+y0
         if(out1.lt.0d0)then
            out1=0d0
         endif
      endif

      if(yabs.lt.7.5d0)then
         my=(ypp1-yp1)/ys
         out2=my*(yabs-(yi-1)*ys)+yp1
      else
         my=(y0p1-y01)/ys
         out2=my*(yabs-(30-1)*ys)+y01
         if(out2.lt.0d0)then
            out2=0d0
         endif
      endif
      
      out=out1+(out2-out1)/(m2-m1)*(mx-m1)

      return
      end

c     reads in 'effective luminosity' for digamma cep

      subroutine lumw(mgam,wl,i)

      double precision wl,mgam,m,v1
      double precision warr(55),rm(55),w082tev(55),w087tev(55)
     &,w0810tev(55),w0814tev(55),w992tev(55),w997tev(55),w9910tev(55)
     &,w9914tev(55)
      integer v,i,k,pdf,eflag
      common/flags/pdf,eflag
      save
      data rm/2.,3.,4.,5.,6.,7.,8.,9.,10.,11.,12.,13.,14.,15.,
     &  16.,17.,18.,19.,20.,21.,22.,23.,24.,25.,26.,27.,28.,29.,
     &  30.,31.,32.,33.,34.,35.,36.,37.,38.,39.,40.,41.,42.,43.,
     &  44.,45.,46.,47.,48.,49.,50.,60.,70.,80.,90.,100.,110/
      data w082tev/
     &    11.512743200,10.086120100,7.086280030,4.903311720,3.481008810,
     &     2.573293700, 1.941231390,1.500678930,1.178456110,0.941021720,
     &     0.763998063, 0.624963018,0.516094797,0.432307513,0.366726158,
     &     0.310423059, 0.265467985,0.229289257,0.198466675,0.173215561,
     &     0.150682702, 0.132421247,0.117464274,0.103890441,0.092311848,
     &     0.082685159, 0.074245335,0.066498622,0.059719810,0.053724145,
     &     0.048845206, 0.044115735,0.040025072,0.036365736,0.033164958,
     &     0.030452901, 0.027927449,0.025762527,0.023544041,0.021657256,
     &     0.019939234, 0.018499899,0.017078153,0.015732299,0.014669711,
     &     0.013569658, 0.012584128,0.011769516,0.010955387,0.005651772,
     &     0.003168302, 0.001895919,0.001181362,0.000764406,0.000511856/
      data w087tev/
     &    74.24410820,65.56878120,46.54664230,33.43397960,24.35794700,
     &    18.38780230,13.95496240,11.00813870, 8.76869030, 7.09853720,
     &     5.83640971, 4.84554665, 4.05480557, 3.42007603, 2.93528382,
     &     2.50636580, 2.15651665, 1.88485114, 1.64307443, 1.45082936,
     &     1.27193315, 1.12696086, 1.01297048, 0.90310996, 0.80974793,
     &     0.73306149, 0.66646608, 0.59940074, 0.54226977, 0.49026242,
     &     0.45003243, 0.40847192, 0.37250580, 0.33972083, 0.31177965,
     &     0.28870697, 0.26735438, 0.24967466, 0.22854734, 0.21278380,
     &     0.19627939, 0.18445197, 0.17151346, 0.15799670, 0.14951398,
     &     0.13820916, 0.12906460, 0.12174358, 0.11387835, 0.06189081,
     &     0.03676559, 0.02307014, 0.01519420, 0.01032504, 0.00720241/
      data w0810tev/
     &    134.85086700,110.11836300,80.31782720,56.95170400,41.88485600,
     &     31.45930850, 24.10758410,19.08467180,15.29361780,12.36459150,
     &     10.14631100,  8.40972626, 7.02902031, 5.96874701, 5.15926839,
     &      4.43238966,  3.83062161, 3.36835786, 2.94762337, 2.60204213,
     &      2.27840914,  2.01724006, 1.81412938, 1.61665031, 1.44944352,
     &      1.31255853,  1.19461309, 1.07666029, 0.97728628, 0.88565852,
     &      0.81556335,  0.74182741, 0.67776963, 0.61905939, 0.56929331,
     &      0.52849072,  0.49075823, 0.45994871, 0.42101632, 0.39254945,
     &      0.36132592,  0.34051585, 0.31715846, 0.29198165, 0.27739197,
     &      0.25610587,  0.23963817, 0.22646250, 0.21197058, 0.11684093,
     &      0.07009796,  0.04453728, 0.02962131, 0.02036145, 0.01432898/
      data w0814tev/
     &   222.67253600,189.91562800,130.81313500,95.00013810,69.81585820,
     &    52.32681460, 40.15282180, 31.94448500,25.50971290,20.78741060,
     &    17.09716270, 14.21058060, 11.92922380,10.12113370, 8.74510357,
     &     7.51029717,  6.48071576,  5.70205690, 4.98634251, 4.42421086,
     &     3.88961705,  3.45814221,  3.12504486, 2.79517610, 2.51538110,
     &     2.28593952,  2.08874537,  1.88001930, 1.70656368, 1.54548381,
     &     1.42331605,  1.29404963,  1.18152255, 1.07840518, 0.99157305,
     &     0.92095238,  0.85589445,  0.80357526, 0.73452488, 0.68756121,
     &     0.63300775,  0.59873093,  0.55912263, 0.51486598, 0.49136558,
     &     0.45337074,  0.42533253,  0.40284910, 0.37741812, 0.20960618,
     &     0.12703515,  0.08144812,  0.05474725, 0.03794800, 0.02694325/
      data w992tev/
     &     1.604498960, 1.743490970,1.317296910,1.052541630,0.831680280,
     &     0.672860093, 0.512874901,0.428948910,0.360817053,0.306535913,
     &     0.250957300, 0.217331264,0.189094541,0.166010138,0.143108392,
     &     0.127006977, 0.113246593,0.102041979,0.089114975,0.081006570,
     &     0.072993665, 0.066553779,0.061159135,0.055927845,0.051256528,
     &     0.047429530, 0.042752804,0.039451006,0.036531268,0.033838203,
     &     0.031615550, 0.029341669,0.027311027,0.025462603,0.023781951,
     &     0.022356246, 0.021000940,0.019833687,0.018404319,0.017307026,
     &     0.016269027, 0.015412606,0.014537724,0.013677947,0.013008201,
     &     0.012270048, 0.011604283,0.011051162,0.010472796,0.006471534,
     &     0.004141631, 0.002891019,0.001953655,0.001358847,0.000965633/
      data w997tev/
     &     6.114567100, 6.633434970,5.589494310,4.510316240,3.567212370,
     &     2.883373570, 2.328934240,1.954989130,1.644358800,1.399375990,
     &     1.199823920, 1.037721660,0.879634446,0.752734458,0.671866626,
     &     0.596015772, 0.528376168,0.477447538,0.428079232,0.389149720,
     &     0.348369153, 0.317241281,0.292355415,0.266469917,0.243689123,
     &     0.225980504, 0.204760222,0.183802472,0.170548023,0.157060513,
     &     0.147222316, 0.136261846,0.126573833,0.117351272,0.109560852,
     &     0.103165973, 0.097234795,0.092570231,0.085990570,0.081285950,
     &     0.074733165, 0.070196863,0.066339844,0.062017996,0.059775240,
     &     0.055878422, 0.053018045,0.050691985,0.048046781,0.028960281,
     &     0.019044720, 0.013102687,0.009430430,0.006846802,0.005152188/
      data w9910tev/
     &     8.760634090, 9.587747690,8.099977190,6.615270510,5.301238310,
     &     4.338112970, 3.539961650,2.965930280,2.474568440,2.089659720,
     &     1.791459090, 1.550159000,1.345396680,1.179184770,1.054050470,
     &     0.936732854, 0.830342718,0.751811336,0.660604668,0.589112904,
     &     0.526814023, 0.479964346,0.442968457,0.403697348,0.369183385,
     &     0.342708767, 0.317991429,0.291507151,0.270810027,0.249149436,
     &     0.233724750, 0.216307340,0.200886797,0.186060019,0.173744760,
     &     0.163706471, 0.154416891,0.147347272,0.133921677,0.124403087,
     &     0.116012040, 0.110905449,0.104892162,0.097872892,0.094657879,
     &     0.088208906, 0.083789780,0.080170182,0.075947678,0.046086803,
     &     0.030287800, 0.020839409,0.014911999,0.010971142,0.008201274/
      data w9914tev/
     &   12.319707700,13.440783000,11.491441300,9.493690890,7.610021460,
     &    6.278766650, 5.154265340, 4.327352030,3.653921370,3.125950860,
     &    2.665356260, 2.308524980, 1.988748820,1.729643370,1.547585640,
     &    1.377269000, 1.219451070, 1.105967670,0.991774818,0.903471636,
     &    0.807941125, 0.736941113, 0.681172363,0.621204828,0.568467062,
     &    0.528338830, 0.482187709, 0.433846588,0.403705551,0.370930420,
     &    0.348276132, 0.322297565, 0.299275207,0.276830296,0.258599932,
     &    0.243848052, 0.230204260, 0.220250338,0.203901225,0.193513814,
     &    0.180179115, 0.172463441, 0.163263170,0.152128530,0.147567921,
     &    0.137172301, 0.130458353, 0.124921481,0.118294700,0.070369137,
     &    0.047054268, 0.032719233, 0.023100996,0.017232414,0.013007874/           

         if(eflag.eq.2)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w992tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w082tev(k)
               enddo 
            endif

         elseif(eflag.eq.7)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w997tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w087tev(k)
               enddo 
            endif

         elseif(eflag.eq.10)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9910tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0810tev(k)
               enddo 
            endif

         elseif(eflag.eq.14)then
            
            if(pdf.eq.1)then
               do k=1,55
                  warr(k)=w9914tev(k)
               enddo             
            elseif(pdf.eq.2)then
               do k=1,55
                  warr(k)=w0814tev(k)
               enddo 
            endif

         endif

      if(mgam.lt.50d0)then

      v=nint((mgam-2d0)/1d0)+1
      v1=((mgam-2d0)/1d0)+1d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      else

      v=nint((mgam-50d0)/10d0)+49
      v1=((mgam-50d0)/10d0)+49d0

      if((v1-dble(v)).lt.0d0)then

         v=v-1
         
      endif

      endif

         m=(dlog(warr(v+1))-dlog(warr(v)))/
     &        (rm(v+1)-rm(v))
         wl=m*(mgam-rm(v))+dlog(warr(v))

      wl=dexp(wl)
    
      return
      end

c     rotates from pt(q(l))=0 frame to general lab frame

      subroutine rotate(vin,l,vout)
      double precision  vin(4),vout(4)
      double precision rmatrix(4,4),q(4,20)
      double precision sy,cy,sx,cx
      common/mom/q
      integer i,j,k,l

      do k=1,4
         vout(k)=0d0
      enddo

      sx=q(1,l)/dsqrt(q(1,l)**2+q(2,l)**2+q(3,l)**2)
      cx=dsqrt(1d0-sx**2)
      sy=q(2,l)/dsqrt(q(2,l)**2+q(3,l)**2)
      cy=q(3,l)/dsqrt(q(2,l)**2+q(3,l)**2)
     
      rmatrix(1,1)=cx
      rmatrix(1,2)=0d0
      rmatrix(1,3)=sx
      rmatrix(1,4)=0d0
      rmatrix(2,1)=-sx*sy
      rmatrix(2,2)=cy
      rmatrix(2,3)=sy*cx
      rmatrix(2,4)=0d0
      rmatrix(3,1)=-sx*cy
      rmatrix(3,2)=-sy
      rmatrix(3,3)=cy*cx
      rmatrix(3,4)=0d0
      rmatrix(4,1)=0d0
      rmatrix(4,2)=0d0
      rmatrix(4,3)=0d0
      rmatrix(4,4)=1d0

      do i=1,4
         do j=1,4
            vout(i)=vout(i)+rmatrix(i,j)*vin(j)
         enddo
      enddo

      return
      end

c     boosting subroutine

      subroutine boost(p,pboo,pcm,plb)
      real*8  pboo(4),pcm(4),plb(4),p,fact
         plb(4)=(pboo(4)*pcm(4)+pboo(3)*pcm(3)
     &             +pboo(2)*pcm(2)+pboo(1)*pcm(1))/p
         fact=(plb(4)+pcm(4))/(p+pboo(4))
         do 10 j=1,3
  10     plb(j)=pcm(j)+fact*pboo(j)
      return
      end

c     calculates soft survival effects according to two-channel eikonal model
c     see arxiv:0807.4115 [hep-ph] and references therein

      subroutine schimc(c,echi1,echi2,p1,p2,p3,p4,zeps)
      implicit double precision (a-y)
      double precision dfr(1000),fk(80000)
      complex*16 x0,sm,zeps
      complex*16 echi1(4),echi2(4,4)
      integer eflag,pdf,iphi,i1,i2,n1,ef,n,ii,c,ll,nx,nt,nphi,nr1,nk
     &,nnkt,kr,kt,jx,jy,jp
      double precision grid(5,5,50,50,50,50)
      common/dlse/nt,nphi,ht,hphi,bh
      common/it/iphi,i1,i2,ef,n
      common/init/rg,incp,n1,ii,ll
      common/dat/grid
      common/flags/pdf,eflag
      save

      bh=4d0	
      bb=16.8d0
      sigt=1.2d0*73d0/0.39d0
      pi=dacos(-1d0)
      
      if(ii.eq.1.and.ll.eq.1)then

            if(eflag.eq.500)then
               open(77,status='unknown',file='grids/dfr500.dat') ! proton opacity
            else
               print*,'exact kinematics only generated for rts = 500'
                stop
            endif
            
            read(77,*) dfr
         
      nx=40
      hx=2d0/dble(nx)
      
      nr1=1000
      nk=4000
      htt=2d0/dble(nk)
      h0=20d0/dble(nr1)

      nnkt=1
      nnamp=1
        

	do 32 kt=1,nk
 32        fk(kt)=0d0
   
           if(nnamp.eq.1) then
              do 30 kr=1,nr1
                 r1s=dble(kr)*h0
                 
                 fd=dfr(kr)
                 f1=0.25d0*(1.96d0*dexp(1.96d0*fd)+0.36d0*dexp(0.36d0*fd
     &                )+2d0*(1d0-0.16d0)*dexp(1.4d0*0.6d0*fd))-1d0
                 
                 do 31 kt=1,nk
                    tkb=r1s*(dble(kt)*htt)
 31        fk(kt)=fk(kt)+r1s*h0*besj0(tkb)*f1
 30     continue
        
      else

         do 27 kt=1,nk
            tkk=(dble(kt)*htt)**2
 27         fk(kt)=-sigt*dexp(-tkk*bb/2d0)/(4d0*pi)
         endif
         do 33 kt=1,nk
            if(fk(kt).gt.0d0) fk(kt)=-0.00001d0
 33         fk(kt)=dlog(-fk(kt))
            
            close(77)
            
         endif
         
         ps=0d0
         ps0=0d0
         p1x=p1
         p1y=p2
         p2x=p3
         p2y=p4
         
         sm=(0d0,0d0)
         
         do 21 jx=-nx,nx
            do 21 jy=-nx,nx
               tpx=dble(jx)*hx
               tpy=dble(jy)*hx

               tp2=tpx**2+tpy**2
               wp=hx*hx/2d0/pi
               t1x=tpx-p1x
               t1y=tpy-p1y
               t12=t1x**2+t1y**2
               t2x=tpx+p2x
               t2y=tpy+p2y
               t22=t2x**2+t2y**2

               tp=dsqrt(tp2)
               jp=tp/htt
               
               ddp=tp/htt-dble(jp)
               if(jp.lt.1) then
                  jp=1
                  ddp=0.
               endif
               if(jp.gt.nk-1) then
                  jp=nk-1
                  ddp=1.
               endif

	fp=fk(jp)*(1d0-ddp)+fk(jp+1)*ddp
	fp=dexp(fp)
        
	call alu(c,echi1,echi2,t1x,t1y,t2x,t2y,x0)
	sm=sm+wp*fp*x0
        
 21   continue

      call alu(c,echi1,echi2,-p1x,-p1y,p2x,p2y,x0)

      as=cdabs(x0)**2
      as=x0
         
      eps0=as
      zeps=(x0-sm)

ccc     uncomment to omit survival effects

c      zeps=x0

      return
      end

c     reads in 'bare' amplitude p_t dep. from grid in '/grids'

      subroutine alu(c,echi1,echi2,t1x,t1y,t2x,t2y,x0)
      implicit double precision (a-y)
      double precision amp,amp1,amp2
      double precision grid(5,5,50,50,50,50),q(4,20)
      complex*16 echi1(4),echi2(4,4),x0
      integer nt,nphi,af,c,naf
      common/it1/af	
      common/dlse/nt,nphi,ht,hphi,bh
      common/dat/grid
      common/vars/s,rts,mchi,ptcut,etacut,ecut,rcut,yx,mx
      common/mom/q

         tt12=t1x**2+t1y**2
         tt22=t2x**2+t2y**2
        
         x0=dexp(-bh*(tt12+tt22)/2d0)

         if(c.eq.0)then
            naf=1
         elseif(c.eq.1)then
            naf=2
         elseif(c.eq.2)then
            naf=3
         endif

         do af=1,naf
            
            call interp(grid,-t1x,-t1y,t2x,t2y,amp)

            if(af.eq.1)then
               amp1=amp
            elseif(af.eq.2)then
               amp2=amp
            elseif(af.eq.3)then
               amp3=amp
            endif
            
         enddo
         
         if(c.eq.0)then
            
         x0=x0*amp1

         elseif(c.eq.1)then

         x0=x0*((echi1(1))*amp1-(echi1(2))*amp2)

         elseif(c.eq.2)then

      x0=x0*(dsqrt(2d0)*(echi2(1,1)*amp2+(echi2(2,1))*amp1
     &        +echi2(2,2)*amp3-
     &        2d0*(amp2+amp3)*(q(3,1)*q(3,2)*echi2(3,3)
     &        +q(4,1)*q(4,2)*echi2(4,4)-q(4,1)*q(3,2)*echi2(4,3)
     &        -q(3,1)*q(4,2)*echi2(3,4))/s))
         
         endif

      return
      end
      
c     4-d linear interpolation subroutine

      subroutine interp(grid,pt1x,pt1y,pt2x,pt2y,amp)
      implicit double precision (a-y)
      integer p1,p2,p3,p4,nt,af,ii,ll
      double precision grid(5,5,50,50,50,50)
      common/it1/af	
      common/init/rg,incp,nt,ii,ll     

      p1=(pt1x+rg/2d0)/incp+1 
      p2=(pt1y+rg/2d0)/incp+1
      p3=(pt2x+rg/2d0)/incp+1
      p4=(pt2y+rg/2d0)/incp+1

      if(p1.eq.nt)then
         p1=p1-1
      endif
      if(p2.eq.nt)then
         p2=p2-1
      endif
      if(p3.eq.nt)then
         p3=p3-1
      endif
      if(p4.eq.nt)then
         p4=p4-1
      endif

      if(pt1x.gt.rg/2d0)then
         p1=nt-1
      endif
      if(pt1y.gt.rg/2d0)then
         p2=nt-1
      endif
      if(pt2x.gt.rg/2d0)then
         p3=nt-1
      endif
      if(pt2y.gt.rg/2d0)then
         p4=nt-1
      endif

      if(pt1x.lt.-rg/2d0)then
         p1=1
      endif
      if(pt1y.lt.-rg/2d0)then
         p2=1
      endif
      if(pt2x.lt.-rg/2d0)then
         p3=1
      endif
      if(pt2y.lt.-rg/2d0)then
         p4=1
      endif

      if(p1.gt.nt)then
         p1=nt-1
      endif
      if(p2.gt.nt)then
         p2=nt-1
      endif
      if(p3.gt.nt)then
         p3=nt-1
      endif
      if(p4.gt.nt)then
         p4=nt-1
      endif

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      int1a=value(pt1x,grid(af,1,p1,p2,p3,p4),grid(af,5,p1,p2,p3,p4)
     &,grid(af,1,p1+1,p2,p3,p4),grid(af,5,p1+1,p2,p3,p4))

      int1b=value(pt1x,grid(af,1,p1,p2+1,p3,p4),grid(af,5,p1,p2+1,p3,p4)
     &,grid(af,1,p1+1,p2+1,p3,p4),grid(af,5,p1+1,p2+1,p3,p4))

      int1=value(pt1y,grid(af,2,p1,p2,p3,p4),int1a,
     &grid(af,2,p1,p2+1,p3,p4),int1b)

      int2a=value(pt1x,grid(af,1,p1,p2,p3+1,p4),grid(af,5,p1,p2,p3+1,p4)
     &,grid(af,1,p1+1,p2,p3+1,p4),grid(af,5,p1+1,p2,p3+1,p4))

      int2b=value(pt1x,grid(af,1,p1,p2+1,p3+1,p4),grid(af,5,p1,p2+1,p3+1
     &,p4),grid(af,1,p1+1,p2+1,p3+1,p4),grid(af,5,p1+1,p2+1,p3+1,p4))

      int2=value(pt1y,grid(af,2,p1,p2,p3+1,p4),int2a,
     &grid(af,2,p1,p2+1,p3+1,p4),int2b)

cccccccccccccccccccccccccccc

      inta=value(pt2x,grid(af,3,p1,p2,p3,p4),int1,
     &grid(af,3,p1,p2,p3+1,p4),int2)

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      int1a=value(pt1x,grid(af,1,p1,p2,p3,p4+1),grid(af,5,p1,p2,p3,p4+1)
     &,grid(af,1,p1+1,p2,p3,p4+1),grid(af,5,p1+1,p2,p3,p4+1))

      int1b=value(pt1x,grid(af,1,p1,p2+1,p3,p4+1),grid(af,5,p1,p2+1,p3
     &,p4+1),grid(af,1,p1+1,p2+1,p3,p4+1),grid(af,5,p1+1,p2+1,p3,p4+1))

      int1=value(pt1y,grid(af,2,p1,p2,p3,p4+1),int1a,
     &grid(af,2,p1,p2+1,p3,p4+1),int1b)

      int2a=value(pt1x,grid(af,1,p1,p2,p3+1,p4+1),grid(af,5,p1,p2,p3+1
     &,p4+1),grid(af,1,p1+1,p2,p3+1,p4+1),grid(af,5,p1+1,p2,p3+1,p4+1))

      int2b=value(pt1x,grid(af,1,p1,p2+1,p3+1,p4+1),grid(af,5,p1,p2+1,
     &p3+1,p4+1),grid(af,1,p1+1,p2+1,p3+1,p4+1)
     &,grid(af,5,p1+1,p2+1,p3+1,p4+1))

      int2=value(pt1y,grid(af,2,p1,p2,p3+1,p4+1),int2a,
     &grid(af,2,p1,p2+1,p3+1,p4+1),int2b)

cccccccccccccccccccccccccccc

      intb=value(pt2x,grid(af,3,p1,p2,p3,p4+1),int1,
     &grid(af,3,p1,p2,p3+1,p4+1),int2)

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      amp=value(pt2y,grid(af,4,p1,p2,p3,p4),inta,
     &grid(af,4,p1,p2,p3,p4+1),intb)
   

      return
      end

      function value(in,x1,y1,x2,y2)
      implicit double precision(a-y)

      m=(y2-y1)/(x2-x1)
      value=m*(in-x1)+y1

      return
      end

*
* subtractive mitchell-moore generator
* ronald kleiss - october 2, 1987
*
* the algorithm is n(i)=[ n(i-24) - n(i-55) ]mod m,
* implemented in a cirucular array with identifcation
* of nr(i+55) and nr(i), such that effectively:
*        n(1)   <---   n(32) - n(1)
*        n(2)   <---   n(33) - n(2)  ....
*   .... n(24)  <---   n(55) - n(24)
*        n(25)  <---   n(1)  - n(25) ....
*   .... n(54)  <---   n(30) - n(54)
*        n(55)  <---   n(31) - n(55)
*
* in this version  m =2**30  and  rm=1/m=2.d0**(-30.d0)
*
* the array nr has been initialized by putting nr(i)=i
* and subsequently running the algorithm 100,000 times.
*

      subroutine r2455(ran)
      real*8 ran,cpyr
      integer idummy
      ran = cpyr(idummy)
      end

      subroutine rambo(n,et,xm,p,wt)
c------------------------------------------------------
c
c                       rambo
c
c    ra(ndom)  m(omenta)  b(eautifully)  o(rganized)
c
c    a democratic multi-particle phase space generator
c    authors:  s.d. ellis,  r. kleiss,  w.j. stirling
c    this is version 1.0 -  written by r. kleiss
c
c    n  = number of particles (>1, in this version <101)
c    et = total centre-of-mass energy
c    xm = particle masses ( dim=100 )
c    p  = particle momenta ( dim=(4,100) )
c    wt = weight of the event
c
c------------------------------------------------------
      implicit none
!     boost arguments
      integer n
      double precision et,xm(100),p(4,100),wt
!     function
      double precision rn
!     internal variables
      double precision q(4,100),z(100),r(4),
     &     b(3),p2(100),xm2(100),e(100),v(100)
      integer iwarn(5)
      double precision acc
      integer itmax,ibegin
      data acc/1.0d-14/,itmax/6/,ibegin/0/,iwarn/5*0/

      integer i,k,iter
      double precision twopi,po2log,xmt,c,s,f,rmas,g,a,x,bq,
     &     accu,f0,g0,wt2,wt3,wtm,x2,xmax
      integer nm
      save


c
c initialization step: factorials for the phase space weight
      if(ibegin.ne.0) goto 103
      ibegin=1
      twopi=8.*datan(1.d0)
      po2log=dlog(twopi/4.)
      z(2)=po2log
      do 101 k=3,100
  101 z(k)=z(k-1)+po2log-2.*dlog(dfloat(k-2))
      do 102 k=3,100
  102 z(k)=(z(k)-dlog(dfloat(k-1)))
c
c check on the number of particles
  103 if(n.gt.1.and.n.lt.101) goto 104
      print 1001,n
      stop
c
c check whether total energy is sufficient; count nonzero masses
  104 xmt=0.
      nm=0
      do 105 i=1,n
      if(xm(i).ne.0.d0) nm=nm+1
  105 xmt=xmt+dabs(xm(i))
      if(xmt.le.et) goto 201
      print 1002,xmt,et
      stop
c
c the parameter values are now accepted
c
c generate n massless momenta in infinite phase space
  201 do 202 i=1,n
      c=2.*rn(1)-1.
      s=dsqrt(1.-c*c)
      f=twopi*rn(2)
      q(4,i)=-dlog(rn(3)*rn(4))
      q(3,i)=q(4,i)*c
      q(2,i)=q(4,i)*s*dcos(f)
 202  q(1,i)=q(4,i)*s*dsin(f)
 
c
c calculate the parameters of the conformal transformation
      do 203 i=1,4
  203 r(i)=0.
      do 204 i=1,n
      do 204 k=1,4
  204 r(k)=r(k)+q(k,i)
      rmas=dsqrt(r(4)**2-r(3)**2-r(2)**2-r(1)**2)
      do 205 k=1,3
  205 b(k)=-r(k)/rmas
      g=r(4)/rmas
      a=1./(1.+g)
      x=et/rmas
c
c transform the q's conformally into the p's
      do 207 i=1,n
      bq=b(1)*q(1,i)+b(2)*q(2,i)+b(3)*q(3,i)
      do 206 k=1,3
  206 p(k,i)=x*(q(k,i)+b(k)*(q(4,i)+a*bq))
  207 p(4,i)=x*(g*q(4,i)+bq)
c
c calculate weight and possible warnings
      wt=po2log
      if(n.ne.2) wt=(2.*n-4.)*dlog(et)+z(n)
      if(wt.ge.-180.d0) goto 208
      if(iwarn(1).le.5) print 1004,wt
      iwarn(1)=iwarn(1)+1
  208 if(wt.le. 174.d0) goto 209
      if(iwarn(2).le.5) print 1005,wt
      iwarn(2)=iwarn(2)+1
c
c return for weighted massless momenta
  209 if(nm.ne.0) goto 210
      wt=dexp(wt)
      return
c
c massive particles: rescale the momenta by a factor x
  210 xmax=dsqrt(1.-(xmt/et)**2)
      do 301 i=1,n
      xm2(i)=xm(i)**2
  301 p2(i)=p(4,i)**2
      iter=0
      x=xmax
      accu=et*acc
  302 f0=-et
      g0=0.
      x2=x*x
      do 303 i=1,n
      e(i)=dsqrt(xm2(i)+x2*p2(i))
      f0=f0+e(i)
  303 g0=g0+p2(i)/e(i)
      if(dabs(f0).le.accu) goto 305
      iter=iter+1
      if(iter.le.itmax) goto 304
c      print 1006,itmax,accu,dabs(f0)
      write(99,1006)itmax,accu,dabs(f0)
      if (dabs(f0).gt.1.0e-6) then
         write(*,1007)dabs(f0)
      endif
      goto 305
  304 x=x-f0/(x*g0)
      goto 302
  305 do 307 i=1,n
      v(i)=x*p(4,i)
      do 306 k=1,3
  306 p(k,i)=x*p(k,i)
  307 p(4,i)=e(i)
c
c calculate the mass-effect weight factor
      wt2=1.
      wt3=0.
      do 308 i=1,n
      wt2=wt2*v(i)/e(i)
  308 wt3=wt3+v(i)**2/e(i)
      wtm=(2.*n-3.)*dlog(x)+dlog(wt2/wt3*et)
c
c return for  weighted massive momenta
      wt=wt+wtm
      if(wt.ge.-180.d0) goto 309
      if(iwarn(3).le.5) print 1004,wt
      iwarn(3)=iwarn(3)+1
  309 if(wt.le. 174.d0) goto 310
      if(iwarn(4).le.5) print 1005,wt
      iwarn(4)=iwarn(4)+1
  310 wt=dexp(wt)
      return
c
 1001 format(' rambo fails: # of particles =',i5,' is not allowed')
 1002 format(' rambo fails: total mass =',d15.6,' is not',
     . ' smaller than total energy =',d15.6)
 1004 format(' rambo warns: weight = exp(',f20.9,') may underflow')
 1005 format(' rambo warns: weight = exp(',f20.9,') may  overflow')
 1006 format(' rambo warns:',i3,' iterations did not give the',
     . ' desired accuracy =',d10.3,' (stops at',d10.3,')')
 1007 format(' rambo warns: end of iteration accuracy too low =',d10.3)
      end
c

      function rn(idum)
*
* subtractive mitchell-moore generator
* ronald kleiss - october 2, 1987
*
* the algorithm is n(i)=[ n(i-24) - n(i-55) ]mod m,
* implemented in a cirucular array with identifcation
* of nr(i+55) and nr(i), such that effectively:
*        n(1)   <---   n(32) - n(1)
*        n(2)   <---   n(33) - n(2)  ....
*   .... n(24)  <---   n(55) - n(24)
*        n(25)  <---   n(1)  - n(25) ....
*   .... n(54)  <---   n(30) - n(54)
*        n(55)  <---   n(31) - n(55)
*
* in this version  m =2**30  and  rm=1/m=2.d0**(-30d0)
*
* the array nr has been initialized by putting nr(i)=i
* and subsequently running the algorithm 100,000 times.
*

      implicit none
      double precision rn
      integer idum
      integer n(55)
      data n/
     . 980629335, 889272121, 422278310,1042669295, 531256381,
     . 335028099,  47160432, 788808135, 660624592, 793263632,
     . 998900570, 470796980, 327436767, 287473989, 119515078,
     . 575143087, 922274831,  21914605, 923291707, 753782759,
     . 254480986, 816423843, 931542684, 993691006, 343157264,
     . 272972469, 733687879, 468941742, 444207473, 896089285,
     . 629371118, 892845902, 163581912, 861580190,  85601059,
     . 899226806, 438711780, 921057966, 794646776, 417139730,
     . 343610085, 737162282,1024718389,  65196680, 954338580,
     . 642649958, 240238978, 722544540, 281483031,1024570269,
     . 602730138, 915220349, 651571385, 405259519, 145115737/
      double precision eps
      double precision rm
      integer j,k,l,m

      data eps/1d-9/
      data m/1073741824/
      data rm/0.9313225746154785d-09/
      data k/55/,l/31/
   

  1   continue
      if(k.eq.55) then
         k=1
      else
         k=k+1
      endif
      if(l.eq.55) then
         l=1
      else
         l=l+1
      endif
      j=n(l)-n(k)
      if(j.lt.0) j=j+m
      n(k)=j
      rn=j*rm
      if(rn.lt.eps) goto 1
      if(rn.gt.1d0-eps) goto 1
      return
      end
      

c
 
