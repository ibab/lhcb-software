      subroutine vbf_h_born_phsp(xborn)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/PhysPars.h'
      real * 8 xborn(ndiminteg-3)
      real * 8 m2,xjac,taumin,tau,y,beta,betaCM,vec(3),cth,s,
     #     z,zhigh,zlow
      integer mu,k,j
      logical ini
      data ini/.true./
      save ini
      real * 8 Vmass2,Vmass2low,Vmass2high,VmVw  
      real * 8 m2jj,pV(0:3),pVmod,pVmod2,pJ(0:3,2),cthj,phij,pcmjj(0:3),
     #     pcmjjmod,ptmp(0:3,2)
      real * 8 vbf_h_mass      
      external vbf_h_mass      
      logical check
      parameter(check=.false.)
      logical BW
      parameter (BW=.true.)
      real * 8 epsilon
      parameter (epsilon=1d-10)
      real * 8 pt1cut,pt2cut,pt1,pt2,m2jjmin
      real * 8 BW_fixed,BW_running
      logical higgsfixedwidth
      save higgsfixedwidth
      real * 8 vbf_h_powheginput
      external vbf_h_powheginput
      if(ini) then
c     set initial- and final-state masses for Born and real
         do k=1,nlegborn
            kn_masses(k)=0
         enddo
         kn_masses(nlegreal)=0
         higgsfixedwidth=vbf_h_powheginput("#higgsfixedwidth").gt.0
         ini=.false.
         pt1cut = 0d0
         pt2cut = 0d0
c         m2jjmin = 0.1d0**2
         m2jjmin = 0d0
         if ((pt1cut.ne.0d0).or.(pt2cut.ne.0d0).or.(m2jjmin.ne.0)) then
            write(*,*) '*************************************'
            write(*,*) '****       CUTS IN PLACE!!!      ****' 
            write(*,*) '*************************************'
         endif
      endif

      Vmass2 = ph_Hmass2
      Vmass2low = ph_Hmass2low
      Vmass2high = ph_Hmass2high
      VmVw = ph_HmHw

      if (BW) then
         zlow=atan((Vmass2low  - Vmass2)/VmVw)
         zhigh=atan((min(Vmass2high,kn_sbeams)  - Vmass2)/VmVw)
         z=zlow+(zhigh-zlow)*xborn(1)
         xjac=zhigh-zlow
         m2=VmVw*tan(z)+Vmass2
c     The BW integrates to Pi ==> divide by Pi
         xjac=xjac/pi

         if(.not.higgsfixedwidth) then
c     running width
            BW_fixed=ph_HmHw/((m2-ph_Hmass2)**2 + ph_HmHw**2)
            BW_running= (m2*ph_Hwidth/ph_Hmass) /
     $           ((m2-ph_Hmass2)**2+(m2*ph_Hwidth/ph_Hmass)**2)
            xjac = xjac * BW_running/BW_fixed
         endif
      else
         xjac = 1
         m2 = Vmass2
      endif

      kn_masses(3)=sqrt(m2)

c     d x1 d x2 = d tau d y;
      taumin=m2/kn_sbeams
      tau=exp(log(taumin)*(1-xborn(2)**2))
      xjac=xjac*tau*abs(log(taumin))*2*xborn(2)
      s=kn_sbeams*tau
      kn_sborn=s
c     ymax=|log(tau)|/2
      y=-(1-2*xborn(3))*log(tau)/2
      xjac=-xjac*log(tau)

c     generate dijet squared vbf_h_mass m2jj
      m2jj = xborn(4)*(sqrt(s)-sqrt(m2))**2
      xjac = xjac * (sqrt(s)-sqrt(m2))**2
      
      pV(0)=(s+m2-m2jj)/(2*sqrt(s))
      pVmod2 = pV(0)**2-m2
      
      if (pVmod2.lt.epsilon) then
         pVmod = 0d0
c         write(*,*) '============================>',pVmod2
      else
         pVmod = sqrt(pVmod2)
      endif
      
      z=1-2*xborn(5)
      xjac=xjac*2
      cth=1.5d0*(z-z**3/3)
      xjac=xjac*1.5d0*(1-z**2)
      
      pV(1) = pVmod*sqrt(1-cth**2)
      pV(2) = 0d0
      pV(3) = pVmod*cth

c     supply 2 pi for azimuthal integration (not performed)
      xjac=xjac*2*pi
c     supply the other factors to the jacobian
c     factor for the two-body jet phase space
      xjac=xjac/(8*(2*pi)**2)
c     factor for V production
      xjac=xjac/(4*(2*pi)**3)*pVmod/sqrt(s)

c     Build kinematics
      kn_xb1=sqrt(tau)*exp(y)
      kn_xb2=tau/kn_xb1
c     boost back in the lab frame
c     now boost everything along 3rd axis
      betaCM=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call vbf_h_mboost(1,vec,betaCM,pV,kn_pborn(0,3))      

c     build jet momenta in the jet CM frame
      pJ(0,1) = sqrt(m2jj)/2
      pJ(0,2) = pJ(0,1)
c     azimuth and polar angle of a jet
      z=1-2*xborn(6)**2
      xjac=xjac*4*xborn(6)
      cthj=1.5d0*(z-z**3/3)
      xjac=xjac*1.5d0*(1-z**2)

      phij = 2*pi*xborn(7)
      xjac=xjac*2*pi

      kn_jacborn = xjac
      
      pJ(1,1) = pJ(0,1)*sqrt(1-cthj**2)*sin(phij)
      pJ(2,1) = pJ(0,1)*sqrt(1-cthj**2)*cos(phij)
      pJ(3,1) = pJ(0,1)*cthj
      do mu=1,3
         pJ(mu,2)=-pJ(mu,1)
      enddo

      do mu=0,3
         kn_pborn(mu,1)=kn_xb1*kn_beams(mu,1)
         kn_pborn(mu,2)=kn_xb2*kn_beams(mu,2)
      enddo

      if (check) then
         call vbf_h_mboost(2,vec,-betaCM,kn_pborn(0,1),ptmp(0,1))      
         write(*,*) 'CM vec1',(ptmp(mu,1),mu=0,3)
         write(*,*) 'CM vec2',(ptmp(mu,2),mu=0,3)
      endif

c     boost in the lab frame
c     compute first p_plus+p_minus-pV
      do mu=0,3
         pcmjj(mu)= kn_pborn(mu,1) + kn_pborn(mu,2) - kn_pborn(mu,3)
      enddo
      pcmjjmod = sqrt(pcmjj(1)**2+pcmjj(2)**2+pcmjj(3)**2)
c     recompute pcmjj(0) from m2jj, otherwise there are points where 
c     beta > 1 or beta < 0
      pcmjj(0) = sqrt(m2jj+pcmjjmod**2)

c      write(*,*) '1 ===========> ',(pcmjj(0)**2-pcmjjmod**2)/m2jj

      beta=pcmjjmod/pcmjj(0)

      do mu=1,3
         vec(mu)=pcmjj(mu)/pcmjjmod
      enddo

      call vbf_h_mboost(2,vec,beta,pJ(0,1),kn_pborn(0,4))

c     CUTS!!!!!!!!
      pt1 = sqrt(kn_pborn(1,4)**2+kn_pborn(2,4)**2)
      pt2 = sqrt(kn_pborn(1,5)**2+kn_pborn(2,5)**2)           
      if ((pt1.lt.pt1cut).or.(pt2.lt.pt2cut).or.m2jj.lt.m2jjmin) then
         kn_jacborn=0d0
      endif


      if (check) then
         call vbf_h_mboost(1,vec,-beta,pcmjj(0),ptmp(0,1))
         write(*,*) 'only time component ==> ',(ptmp(mu,1),mu=0,3)
         write(*,*) ''
         write(*,*) 'new set'
         do j=1,nlegborn
            write(*,*) 'mom ',j,(kn_pborn(mu,j),mu=0,3)
            write(*,*) 'vbf_h_mass ',j,vbf_h_mass(kn_pborn(0,j))
         enddo
         call vbf_h_checkmomzero(nlegborn,kn_pborn)
      endif
      
c     boost in the CM frame
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call vbf_h_mboost(nlegborn,vec,-betaCM,kn_pborn(0,1),kn_cmpborn(0,1))      
      if (check) then
         write(*,*) ''
         write(*,*) 'new set'     
         do j=1,nlegborn
            write(*,*) 'mom ',j,(kn_cmpborn(mu,j),mu=0,3)
         enddo
      endif

      kn_minmass=sqrt(Vmass2low)
      
      end


      function vbf_h_mass(p)
      implicit none
      real * 8 p(0:3),vbf_h_mass
      vbf_h_mass = sqrt(abs(p(0)**2-p(1)**2-p(2)**2-p(3)**2))
      end



      subroutine vbf_h_born_suppression(fact)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      logical ini
      data ini/.true./
      real * 8 fact,pt2,pt2supp,vbf_h_powheginput,pt
      save ini,pt2supp,pt     
c CAVEAT!!!  process dependent subroutine
      if (ini) then
         pt = vbf_h_powheginput("#ptsupp")         
         ini = .false.
         pt2supp = pt**2
      endif
      if (pt.lt.0) then
         fact=1d0
      else         
         pt2=kn_pborn(1,5)**2+kn_pborn(2,5)**2
         fact=pt2/(pt2+pt2supp)         
c      if (pt2.gt.10) then
c         fact = 1d0
c      else
c         fact = 0d0
c      endif
      endif
      end


      subroutine vbf_h_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/vbf_h/PhysPars.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      save ini
      muf=ph_Hmass
      mur=ph_Hmass
      if (ini) then
         write(*,*) '**********************************'
         write(*,*) 'RENORMALIZATION SCALE = ',mur
         write(*,*) 'FACTORIZATION   SCALE = ',muf
         write(*,*) '**********************************'
         ini=.false.
      endif

      end
