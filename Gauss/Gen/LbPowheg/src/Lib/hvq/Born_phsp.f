      subroutine hvq_born_phsp(xborn)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      include 'src/Lib/hvq/pwhg_math.h'
      real * 8 xborn(ndiminteg-3),vec(3),beta
      integer mu
      kn_jacborn=1
      call hvq_x1x2phspace(kn_sbeams,kn_minmass,xborn(1),kn_xb1,kn_xb2,
     1     kn_sborn,kn_jacborn)
      kn_cmpborn(0,1)=sqrt(kn_sborn)/2
      kn_cmpborn(3,1)=kn_cmpborn(0,1)
      kn_cmpborn(0,2)=kn_cmpborn(0,1)
      kn_cmpborn(3,2)=-kn_cmpborn(0,1)
      call hvq_twobody0(xborn(3),kn_sborn,kn_masses(3),kn_masses(4),
     1     kn_cmpborn(0,3),kn_cmpborn(0,4),kn_jacborn)
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=1
      call hvq_mboost(nlegborn-2,vec,beta,kn_cmpborn(0,3),kn_pborn(0,3))
      do mu=0,3
         kn_pborn(mu,1)=kn_xb1*kn_beams(mu,1)
         kn_pborn(mu,2)=kn_xb2*kn_beams(mu,2)
      enddo
      end


      subroutine hvq_x1x2phspace(sbeams,minmass,xx,x1,x2,s,jac)
      implicit none
      real * 8 sbeams,minmass,xx(2),x1,x2,s,jac
      real * 8 taumin,tau,y
      taumin=minmass**2/sbeams
      tau=exp(log(taumin)*(1-xx(1)**2))
      jac=jac*tau*abs(log(taumin))*2*xx(1)
      s=sbeams*tau
      y=-(1-2*xx(2))*log(tau)/2
      jac=-jac*log(tau)
      x1=sqrt(tau)*exp(y)
      x2=tau/x1
      end

      subroutine hvq_twobody0(xx,s,m1,m2,p1,p2,jac)
      implicit none
      include "pwhg_math.h"
      real * 8 xx(1),s,m1,m2,p1(0:3),p2(0:3),jac
      real * 8 k,m,ctheta,stheta
      m=sqrt(s)
      if(m.le.m1+m2) then
         jac=0
         return
      endif
      k=sqrt((m**2-(m1+m2)**2)*(m**2-(m1-m2)**2))/(2*m)
      ctheta=1-2*xx(1)
      stheta=sqrt(abs(1-ctheta**2))
      p1(3)=ctheta*k
      p1(1)=stheta*k
      p1(2)=0
      p2(1)=-p1(1)
      p2(2)=-p1(2)
      p2(3)=-p1(3)
      p1(0)=sqrt(m1**2+k**2)
      p2(0)=sqrt(m2**2+k**2)
      jac=jac/(8*pi)*k/p1(0)
      end

      subroutine hvq_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      logical runningscales
      save runningscales
      real * 8 pt2
      real * 8 hvq_powheginput
      external hvq_powheginput
      if(ini) then
         if(hvq_powheginput('#fixedscale').eq.1) then
            runningscales=.false.
         else
            runningscales=.true.
         endif
      endif
      if (runningscales) then
         if (ini) then
            write(*,*) '****************************************'
            write(*,*) '****************************************'
            write(*,*) '**   mur=sqrt(pt^2+m^2)               **'
            write(*,*) '**   muf=sqrt(pt^2+m^2)  in bbar func.**'
            write(*,*) '****************************************'
            write(*,*) '****************************************'
            ini=.false.            
         endif
         pt2=kn_pborn(1,3)**2+kn_pborn(2,3)**2+kn_masses(3)**2
         mur=sqrt(pt2)
         muf=mur
      else
         if (ini) then
            write(*,*) '***************************'
            write(*,*) '***************************'
            write(*,*) '**   mur=m               **'
            write(*,*) '**   muf=m  in bbar func.**'
            write(*,*) '***************************'
            write(*,*) '***************************'
            ini=.false.            
         endif
         muf=kn_masses(3)
         mur=muf
      endif
      end


