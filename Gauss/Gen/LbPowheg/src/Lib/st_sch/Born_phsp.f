      subroutine st_sch_born_phsp(xborn)
      implicit none
      include 'src/Lib/st_sch/nlegborn.h'
      include 'src/Lib/st_sch/pwhg_flst.h'
      include 'src/Lib/st_sch/pwhg_kn.h'
      include 'src/Lib/st_sch/pwhg_math.h'
      include 'src/Lib/st_sch/PhysPars.h'

      real * 8 xborn(ndiminteg-3)
      real * 8 vec(3),cth,cthdec,phidec,s,
     #     z,zhigh,zlow
      integer mu,k,j
      logical ini
      data ini/.true./
      save ini

cccccccccccccccccccc
c     from POWHEG-st_dec code
      real *8 xx(ndiminteg-3),m2top,xplus,xminus,cth1
      real *8 tau,tau_min,tau_max,tmp,ycm,ycm_min,ycm_max
      real *8 jacb,shat,s_had,ro,beta

      real *8 tmin,tmax,random_min,random_max,random_t

      integer ixx_mtop,ixx_tau,ixx_ycm,ixx_cth1
      parameter(
     #ixx_tau  =1,
     #ixx_ycm  =2,
     #ixx_cth1 =3)
      real *8 Eloc,m2topbar
ccccccccccccccccccccc

      if(ini) then
c     Parameter to generate phase space
c     psgen=0:     flat in 1/tau, flat in cth1bar
c     psgen=1:     flat in tau, flat in cth1bar
         psgen=0   
c     numgamma(1): <=0: top mass fixed; 
c     otherwise top mass distributed according to its Breit-Wigner
         numgamma(1)=-1d0
c     numgamma(2): not used
         numgamma(2)=-1d0
         do k=1,nlegborn
            kn_masses(k)=0
         enddo
         kn_masses(nlegreal)=0
         ini=.false.
      endif

c     top is massive
      kn_masses(3)=topmass_pow


c     local copy of variables
c     xx(1) -> tau
c     xx(2) -> ycm
c     xx(3) -> cth1
      do k=1,ndiminteg-3
         xx(k)=xborn(k)
      enddo

      s_had=kn_sbeams
      jacb=1d0
      
c     born phase space: variables
      if(numgamma(1).gt.0d0) then
         print*, 'Error in st_sch_born_phsp: top Breit-Wigner not implemented'
         call exit(1)
      else
         m2top=topmass_pow**2
      endif

c$$$c     sets mtop used in mcnlo subroutines (passed with stpcblks.h)
c$$$c     Needed here because in case of BW distribution, this mass
c$$$c     has to change event by event
c$$$      xm1=sqrt(m2top)
c$$$      xm12=m2top

      tau_min=(sqrt(m2top)+0d0)**2/s_had
      tau_max=1d0
      if(psgen.eq.0)then
c     imp. sampling (flat in 1/tau )
         tmp=1d0/tau_max + (1d0/tau_min-1d0/tau_max)*xx(ixx_tau)
         tau=1d0/tmp
         jacb=jacb * tau**2 * (1d0/tau_min-1d0/tau_max)
      elseif(psgen.eq.1) then
c     uniform generation
         tau=tau_min + (tau_max-tau_min)*xx(ixx_tau)
         jacb=jacb * (tau_max-tau_min)
      else
         write(*,*) 'Wrong psgen in gen_born_vars'
         call exit(1)
      endif
      ycm_min=  log(tau)/2
      ycm_max= -log(tau)/2
      ycm = ycm_min + xx(ixx_ycm)*(ycm_max-ycm_min)
      jacb=jacb * (ycm_max-ycm_min)

      shat=tau*s_had
      ro = 2d0/shat*(m2top + 0d0) - 
     #     (m2top - 0d0)**2/shat**2
      beta = sqrt(1-ro)

c     th1 is the angle of 1st outgoing particle wrt +z axis
      if(psgen.eq.0)then
c     uniform generation
c$$$         call zzchvar(xx(ixx_cth1),cth1,jacb,ro) !: use this to do exactly the same as mc@nlo
         cth1=-1d0+xx(ixx_cth1)*2d0 
         jacb=jacb * 2d0 
      elseif(psgen.eq.1) then
c     uniform generation
         cth1=-1d0+xx(ixx_cth1)*2d0 
         jacb=jacb * 2d0 
      else
         write(*,*) 'Wrong psgen in gen_born_vars'
         call exit(1)
      endif
 
c     born phase space: physical phase space
      jacb=jacb * beta/16d0/pi   ! *( d_phi/(2d0*pi) )

c     Feynman x's and Mandelstam invariants
      xplus=sqrt(tau) * exp(ycm)
      xminus=sqrt(tau) * exp(-ycm)

cccccccccccccccccccccccccccc
c     assign born jacobian and default kinematics variables
      kn_jacborn=jacb
      kn_born_pt2=0d0
      kn_cthdec=cth1
      phidec=0d0
c     With this choice px_top is always positive, but at the end the whole
c     event will be randomly rotated around z-axis
      kn_sborn=shat
cccccccccccccccccccccccccccc

cccccccccccccccccccccccccccccc
c     compute momenta
cccccccccccccccccccccccccccccc

c Build kinematics
      kn_xb1=xplus
      kn_xb2=xminus

c initial state particles
      kn_cmpborn(0,1)=sqrt(shat)/2
      kn_cmpborn(0,2)=kn_cmpborn(0,1)
      kn_cmpborn(3,1)=kn_cmpborn(0,1)
      kn_cmpborn(3,2)=-kn_cmpborn(0,2)
      kn_cmpborn(1,1)=0
      kn_cmpborn(1,2)=0
      kn_cmpborn(2,1)=0
      kn_cmpborn(2,2)=0  

c final state particles    
      m2topbar=m2top
      Eloc=(shat-m2topbar)/2d0/sqrt(shat)

      kn_cmpborn(0,3)=sqrt(Eloc**2 + m2topbar)
      kn_cmpborn(0,4)=Eloc

      kn_cmpborn(3,3)=kn_cthdec                       *Eloc
      kn_cmpborn(1,3)=sqrt(1-kn_cthdec**2)*cos(phidec)*Eloc
      kn_cmpborn(2,3)=sqrt(1-kn_cthdec**2)*sin(phidec)*Eloc

      kn_cmpborn(1,4)=-kn_cmpborn(1,3)
      kn_cmpborn(2,4)=-kn_cmpborn(2,3)
      kn_cmpborn(3,4)=-kn_cmpborn(3,3)

c now st_sch_boost everything along 3
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call st_sch_mboost(nlegborn-2,vec,beta,kn_cmpborn(0,3),kn_pborn(0,3))
      do mu=0,3
         kn_pborn(mu,1)=kn_xb1*kn_beams(mu,1)
         kn_pborn(mu,2)=kn_xb2*kn_beams(mu,2)
      enddo
      call st_sch_checkmomzero(nlegborn,kn_pborn)
c      call st_sch_checkmass(2,kn_pborn(0,3))

      kn_minmass=topmass_pow

      end

      subroutine st_sch_born_suppression(fact)
      implicit none
      logical ini
      data ini/.true./
      real * 8 fact
      if (ini) then
         write(*,*) '**************************'
         write(*,*) 'No Born suppression factor'
         write(*,*) '**************************'
         ini=.false.
      endif
      fact=1d0
      end


      subroutine st_sch_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/st_sch/PhysPars.h'
      include 'src/Lib/st_sch/nlegborn.h'
      include 'src/Lib/st_sch/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      real *8 muref
      real *8 st_sch_dotp
      external st_sch_dotp
      if (ini) then
         write(*,*) '*************************************'
         write(*,*) '    Factorization and renormalization '
         write(*,*) '    scales set to top mass         '
         write(*,*) '*************************************'
         ini=.false.
      endif
      muf=topmass_pow
      mur=topmass_pow

      end

