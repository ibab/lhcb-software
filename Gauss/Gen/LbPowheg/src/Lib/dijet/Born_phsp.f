      subroutine dijet_born_phsp(xborn)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_math.h'

      real * 8 xborn(ndiminteg-3)
      real * 8 vec(3),phidec
      integer mu,k
      logical ini
      data ini/.true./
      save ini

      real*8 xx(ndiminteg-3)
      real*8 tau,tau_min,tau_max
      real*8 ycm,ycm_min,ycm_max
      real*8 cth1,cth1_min,cth1_max,lcthl,lcthh,lcth,tcth
      real*8 dijet_powheginput,beta,tmp
      integer ixx_tau,ixx_ycm,ixx_cth1
      parameter(
     #ixx_tau  =1,
     #ixx_ycm  =2,
     #ixx_cth1 =3)

C -   N.B: To check the phase space volume is correct,
C -   replace res(j) in sigborn.f by res(j)=kn_jacborn
C -   then check that the cross section is equal to,
C -   hc^2/(8*pi) = 1.54929*10^7.

C -   Parameter to select phase space importance sampling:
C -   psgen=0:     flat in 1/tau, flat in 1/(1-cth1)
C -   psgen=1:     flat in tau, flat in cth1
c -   psgen=2:     flat in log tau, flat in log((1+cth1)/(1-cth1))
      integer psgen
      parameter (psgen=3)

C -   Phase space: 1 /(16 pi) d tau d y d cth 
C -   4*kn_ktmin**2 / S < tau < 1
C -   |y| < ln(tau)/2
C -   |cth1| < sqrt(1-4*kn_ktmin**2 / S /tau)

      if (ini) then
         write(*,*) '************************************'
         write(*,*) '  Minimum kT of jets in underlying  '
         write(*,*) '  Born is limited to',kn_ktmin,' GeV'
         write(*,*) '************************************'
      endif

C -   Set minimum sqrt(\hat{s}) value
      kn_minmass = 2.*kn_ktmin

C -   Set initial- and final-state masses for Born and real
      if(ini) then
         do k=1,nlegborn
            kn_masses(k)=0
         enddo
         kn_masses(nlegreal)=0
         ini=.false.
      endif

C -   Making a local copy of the unit dijet_random variables (?)
      do k=1,ndiminteg-3
         xx(k)=xborn(k)
      enddo

C -   Initial Jacobian prefactor
      kn_jacborn = 1.0d0/16.0d0/pi
      
C -   Set minimum and maximum tau values then get tau:
      tau_min = 4.*kn_ktmin**2 / kn_sbeams
      tau_max = 1.0d0
      if(psgen.eq.0)then
C -      Sampling flat in 1/tau
         tmp  = 1.0d0/ tau_max + xx(ixx_tau) * (1d0/tau_min-1d0/tau_max)
         tau  = 1.0d0/ tmp
         kn_jacborn = kn_jacborn * tau**2 * (1d0/tau_min-1d0/tau_max)
      elseif(psgen.eq.1) then
C -      Sampling flat in tau
         tau  = tau_min + xx(ixx_tau) * (tau_max-tau_min)
         kn_jacborn = kn_jacborn * (tau_max-tau_min)
      elseif(psgen.eq.3) then
C -      Flat in log(tau)
         tau = tau_min*exp(xx(ixx_tau) * log(tau_max/tau_min))
         kn_jacborn = kn_jacborn * tau * log(tau_max/tau_min)
      else
         write(*,*) 'Wrong psgen in gen_born_vars'
         call exit(1)
      endif

C -   Now we can work out \hat{s}
      kn_sborn = tau * kn_sbeams

C -   Set minimum and maximum y values 
      ycm_min =  log(tau)/2.
      ycm_max = -log(tau)/2.
C -   Now get a y uniformly in the allowed range
      ycm  = ycm_min + xx(ixx_ycm) * (ycm_max-ycm_min)
      kn_jacborn = kn_jacborn * (ycm_max-ycm_min)

C -   Feynman x's can be computed and saved now:
      kn_xb1 = sqrt(tau) * exp( ycm)
      kn_xb2 = sqrt(tau) * exp(-ycm)

C -   Set minimum and maximum cos(dijet_THETA) values (dijet_THETA is the 
C -   angle of 1st outgoing particle wrt +z axis).
      tmp = sqrt(kn_sborn)
      cth1_max =  sqrt(1.-2.d0*kn_ktmin/tmp)*sqrt(1.+2.d0*kn_ktmin/tmp)
      cth1_min = -cth1_max
      if(psgen.eq.0) then
C -      Sample cos(dijet_THETA) as 1/(1-cos(dijet_THETA)) ~ 1/sin^4(dijet_THETA/2)
         tmp  = 2.0D0*cth1_max/(1.0D0-cth1_max)/(1.0D0+cth1_max) 
         cth1 = 1.0D0-1.0D0/(1.0D0/(1.0D0-cth1_min) + xx(ixx_cth1)*tmp)
         kn_jacborn = kn_jacborn * (1D0-cth1)**2 * tmp
      elseif(psgen.eq.1) then
C -      Sample cos(dijet_THETA) uniformly
         cth1 = cth1_min + xx(ixx_cth1) * (cth1_max-cth1_min)
         kn_jacborn = kn_jacborn * (cth1_max-cth1_min)
      elseif(psgen.eq.3) then
c -      Sample cos(dijet_THETA) uniformly in log((1-cos)/(1+cos))
         lcthl=log((1-cth1_max)/(1+cth1_max))
         lcthh=-lcthl
         lcth=xx(ixx_cth1)*(lcthh-lcthl)+lcthl
         kn_jacborn = kn_jacborn * (lcthh-lcthl)
         tcth=exp(lcth)
         kn_jacborn = kn_jacborn * tcth
         cth1=(tcth-1)/(tcth+1)
         kn_jacborn = kn_jacborn * 2/(tcth+1)**2
      else
         write(*,*) 'Wrong psgen in gen_born_vars'
         call exit(1)
      endif
 
      if(kn_jacborn.lt.0.0) then
         write(6,*) 'dijet_born_phsp: Error kn_jacborn < 0. !'
         write(*,*) 'kn_jacborn        = ', kn_jacborn
         write(*,*) 'sqrt(kn_sborn)    = ', sqrt(kn_sborn)
         write(*,*) 'cth1_max-cth1_min = ', (cth1_max-cth1_min)
      endif

C -   Assign born jacobian and default kinematics variables
      kn_born_pt2 = (1d0-cth1**2)*kn_sborn/4.0d0
      kn_cthdec   = cth1
      phidec      = 0d0

C -   With this choice px_3 is always positive, but at the
C -   end the whole event will be randomly rotated around z-axis

C -   Initial state momenta:
      kn_cmpborn(0,1) =  sqrt(kn_sborn)/2        ! E
      kn_cmpborn(0,2) =  kn_cmpborn(0,1)         !
      kn_cmpborn(3,1) =  kn_cmpborn(0,1)         ! Pz
      kn_cmpborn(3,2) = -kn_cmpborn(0,2)         !
      kn_cmpborn(1,1) = 0                        ! Px
      kn_cmpborn(1,2) = 0                        !
      kn_cmpborn(2,1) = 0                        ! Py
      kn_cmpborn(2,2) = 0                        !

C -   Final state momenta:
      kn_cmpborn(0,3) = sqrt(kn_sborn)/2            ! E
      kn_cmpborn(0,4) =  kn_cmpborn(0,3)            !
      kn_cmpborn(3,3) =  kn_cthdec*kn_cmpborn(0,3)  ! Pz
      kn_cmpborn(3,4) = -kn_cmpborn(3,3)            !
      kn_cmpborn(1,3) = sqrt(1-kn_cthdec**2)*sin(phidec)*kn_cmpborn(0,3)  ! Px
      kn_cmpborn(1,4) = -kn_cmpborn(1,3)                                  !
      kn_cmpborn(2,3) = sqrt(1-kn_cthdec**2)*cos(phidec)*kn_cmpborn(0,3)  ! Py
      kn_cmpborn(2,4) = -kn_cmpborn(2,3)                                  !

C -   Boost to the LAB frame:
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call dijet_mboost(nlegborn-2,vec,beta,kn_cmpborn(0,3),kn_pborn(0,3))
      do mu=0,3
         kn_pborn(mu,1)=kn_xb1*kn_beams(mu,1)
         kn_pborn(mu,2)=kn_xb2*kn_beams(mu,2)
      enddo
      call dijet_checkmomzero(nlegborn,kn_pborn)
C -      call dijet_checkmass(2,kn_pborn(0,3))

      end



      subroutine dijet_born_suppression(fact)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      logical ini
      data ini/.true./
      real*8 fact,pt2,pt2supp,dijet_powheginput,pt
      save ini,pt2supp,pt     
      if (ini) then
         pt = dijet_powheginput("#ptsupp")         
         if(pt.gt.0) then
            write(*,*) ' ******** WARNING: ptsupp is deprecated'
            write(*,*) ' ******** Replace it with bornsuppfact'
         else
            pt = dijet_powheginput("#bornsuppfact")
         endif
         ini = .false.
         pt2supp = pt**2
      endif
      if (pt.le.0) then
         fact=1d0
      else         
         pt2=kn_cmpborn(1,3)**2+kn_cmpborn(2,3)**2
         fact=(pt2/(pt2+pt2supp))**3
      endif
      end


      subroutine dijet_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      logical ini,fixedscale
      data ini/.true./
      real * 8 muf,mur
      real *8 muref
      save ini,fixedscale,muref
      real * 8 dijet_powheginput
      external dijet_powheginput
      if(ini) then
         muref=dijet_powheginput("#fixedscale")
         if(muref.lt.0) then
            fixedscale=.false.
         else
            fixedscale=.true.
         endif
         ini=.false.
      endif
      if(.not.fixedscale) then
         muref=sqrt(kn_cmpborn(1,3)**2+kn_cmpborn(2,3)**2)
      endif
      muf=muref
      mur=muref
      end

