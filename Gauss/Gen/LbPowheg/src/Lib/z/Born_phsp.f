      subroutine z_born_phsp(xborn)
      implicit none
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      include 'src/Lib/z/pwhg_kn.h'
      include 'src/Lib/z/pwhg_math.h'
      include 'src/Lib/z/PhysPars.h'
      real * 8 xborn(ndiminteg-3)
      real * 8 m2,xjac,tau,y,beta,vec(3),cth,cthdec,phidec,s,
     # z,zhigh,zlow
      integer mu,k
      logical ini
      data ini/.true./
      save ini
      if(ini) then
c     set initial- and final-state masses for Born and real
         do k=1,nlegborn
            kn_masses(k)=0
         enddo
         kn_masses(nlegreal)=0
         ini=.false.
      endif
c Phase space:
c 1 /(16 pi S) d m^2 d cth d y
      xjac=1d0/kn_sbeams/(16*pi)
      zlow=atan((ph_Zmass2low  - ph_Zmass2)/ph_ZmZw)
      zhigh=atan((ph_Zmass2high  - ph_Zmass2)/ph_ZmZw)
      z=zlow+(zhigh-zlow)*xborn(1)
      xjac=xjac*(zhigh-zlow)
      m2=ph_ZmZw*tan(z)+ph_Zmass2
c d m^2 jacobian
      xjac=xjac*ph_ZmZw/cos(z)**2
c d x1 d x2 = d tau d y;
      tau=m2/kn_sbeams
      s=kn_sbeams*tau
      kn_sborn=s
c ymax=|log(tau)|/2
      y=-(1-2*xborn(2))*log(tau)/2
      xjac=-xjac*log(tau)
      z=1-2*xborn(3)
      xjac=xjac*2
      cth=1.5d0*(z-z**3/3)
      xjac=xjac*1.5d0*(1-z**2)
      kn_born_pt2=0d0
      
c
      cthdec=cth
      phidec=0d0
      kn_cthdec=cthdec
      kn_jacborn=xjac
c Build kinematics
      kn_xb1=sqrt(tau)*exp(y)
      kn_xb2=tau/kn_xb1
c decay products in their rest frame
      kn_cmpborn(0,3)=sqrt(m2)/2
      kn_cmpborn(0,4)=kn_cmpborn(0,3)
      kn_cmpborn(3,3)=kn_cthdec*kn_cmpborn(0,3)
      kn_cmpborn(1,3)=sqrt(1-kn_cthdec**2)*cos(phidec)*kn_cmpborn(0,3)
      kn_cmpborn(2,3)=sqrt(1-kn_cthdec**2)*sin(phidec)*kn_cmpborn(0,3)
      kn_cmpborn(1,4)=-kn_cmpborn(1,3)
      kn_cmpborn(2,4)=-kn_cmpborn(2,3)
      kn_cmpborn(3,4)=-kn_cmpborn(3,3)
c initial state particles
      kn_cmpborn(0,1)=sqrt(s)/2
      kn_cmpborn(0,2)=kn_cmpborn(0,1)
      kn_cmpborn(3,1)=kn_cmpborn(0,1)
      kn_cmpborn(3,2)=-kn_cmpborn(0,2)
      kn_cmpborn(1,1)=0
      kn_cmpborn(1,2)=0
      kn_cmpborn(2,1)=0
      kn_cmpborn(2,2)=0      
c now boost everything along 3
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call z_mboost(nlegborn-2,vec,beta,kn_cmpborn(0,3),kn_pborn(0,3))
      do mu=0,3
         kn_pborn(mu,1)=kn_xb1*kn_beams(mu,1)
         kn_pborn(mu,2)=kn_xb2*kn_beams(mu,2)
      enddo
c      call z_checkmomzero(nlegborn,kn_pborn)
c      call z_checkmass(2,kn_pborn(0,3))

c minimal final state mass 
      kn_minmass=sqrt(ph_Zmass2low)

      end


      subroutine z_born_suppression(fact)
      implicit none
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      include 'src/Lib/z/pwhg_kn.h'
      logical ini
      data ini/.true./
      real * 8 fact,pt
      real * 8 z_powheginput
      external z_powheginput
      if (ini) then
         pt = z_powheginput("#ptsupp")         
         if(pt.gt.0) then
            write(*,*) ' ******** WARNING: ptsupp is deprecated'
            write(*,*) ' ******** Replace it with bornsuppfact'
         else
            pt = z_powheginput("#bornsuppfact")
         endif
         if(pt.ge.0) then
            write(*,*) '**************************'
            write(*,*) 'No Born suppression factor'
            write(*,*) '**************************'
         endif
         ini=.false.
      endif
      fact=1d0
      end


      subroutine z_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/z/PhysPars.h'
      include 'src/Lib/z/nlegborn.h'
      include 'src/Lib/z/pwhg_flst.h'
      include 'src/Lib/z/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      real *8 muref
      real *8 z_dotp
      external z_dotp
      logical runningscales
      save runningscales
      real * 8 pt2
      real * 8 z_powheginput
      external z_powheginput
      if(ini) then
         if(z_powheginput('#runningscale').ge.1) then
            runningscales=.true.
         else
            runningscales=.false.
         endif
      endif
      if (runningscales) then
         if (ini) then
            write(*,*) '*************************************'
            write(*,*) '    Factorization and renormalization '
            if (z_powheginput('#runningscale').eq.1) then
               write(*,*) '    scales set to the Z virtuality '            
            else 
               write(*,*) "runningscale value not allowed"
               call exit(1)
            endif
            write(*,*) '*************************************'
            ini=.false.
         endif
         muref=sqrt(2d0*z_dotp(kn_pborn(0,3),kn_pborn(0,4)))
      else
         if (ini) then
            write(*,*) '*************************************'
            write(*,*) '    Factorization and renormalization '
            write(*,*) '    scales set to the Z mass '
            write(*,*) '*************************************'
            ini=.false.
         endif
         muref=ph_Zmass
      endif
      muf=muref
      mur=muref
      end
