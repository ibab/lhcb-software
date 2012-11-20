      subroutine ww_born_phsp(xborn)
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      include 'src/Lib/ww/pwhg_math.h'
      include 'src/Lib/ww/PhysPars.h'
      include 'src/Lib/ww/vvsettings.h'
      real * 8 xborn(ndiminteg-3)
      real * 8 xjac,tau,beta,vec(3)

      integer mu
      double precision p1(4),p2(4),p3(4),p4(4),p5(4),p6(4)
      double precision p(4,6)
      double precision xx(2)
      double precision p12(4),p34(4),p56(4)
      double precision wt
      double precision m34,m56,smin,smax,s,z
      double precision xmin,taumin,sqrts
      integer i,k
      real * 8 ww_powheginput
      external ww_powheginput
      logical debug,ini
      data ini/.true./
      data debug/.false./
      save ini

      double precision lntaum,ymax,ycm

      if (debug)  write(*,*) 'Entering ww_born_phsp: ndiminteg', ndiminteg
      if(ini) then
         do k=1,nlegborn
            kn_masses(k)=0
         enddo
         kn_masses(nlegreal)=0
         ini=.false.
      endif
C     
      xmin  = 1d-8 
      sqrts = sqrt(kn_sbeams)
      
c First determine virtualities of lepton pairs
      smin=0d0
      smax= sqrts**2
      z=xborn(1)
      xjac=1d0
c ww_breitw, if zerowidth is true, does the right thing
      call ww_breitw(z,smin,smax,ph_wmass,ph_wwidth,s,wt)
c wt is the Jacobian from z to s; we do ds/(2 pi), so provide
c 2 pi
      xjac=xjac*wt/(2*pi)
      m34=sqrt(s)
      smax=(sqrts-m34)**2
      z=xborn(2)
      
      call ww_breitw(z,smin,smax,ph_wmass,ph_wwidth,s,wt)
      xjac=xjac*wt/(2*pi)
      m56=sqrt(s)

      taumin = ((m34+m56)/sqrts)**2
      lntaum = dlog(taumin)
      tau = dexp(lntaum*(1d0-xborn(9)))
      xjac = xjac*(-lntaum*tau)

      kn_sborn = kn_sbeams*tau

      ymax=-0.5d0*log(tau)
      ycm=xborn(10)*2*ymax-ymax
      xjac = xjac*2*ymax
      xx(1)=sqrt(tau)*exp(ycm)
      xx(2)=tau/xx(1)

      if (xborn(10) .eq. 0d0) then 
         write(*,*) 'xborn(10).eq 0: xborn',xborn
      endif

c---if x's out of normal range abort
c      if   ((xx(1) .gt. 1d0)
c     & .or. (xx(2) .gt. 1d0)
c     & .or. (xx(1) .lt. xmin)
c     & .or. (xx(2) .lt. xmin)) then
c         write(*,*) 'ww_born_phsp: warning x1,x2 our of range:',xx(1),xx(2)
c         call exit(-1)
c      endif

C     NB positive energy even if incoming, i.e. p1+p2 = \sum_3^8 p_i   
c     pos rapidity
      p1(4)=xx(1)*sqrts*0.5d0
      p1(1)=0d0
      p1(2)=0d0
      p1(3)=xx(1)*sqrts*0.5d0

c     neg rapidity 
      p2(4)=xx(2)*sqrts*0.5d0
      p2(1)=0d0
      p2(2)=0d0
      p2(3)=-xx(2)*sqrts*0.5d0

C     total incoming momentum 
      p12 = p1+p2 

      call ww_phi1_2(xborn(3),xborn(4),p12,p34,p56,m34,m56,wt)
      xjac=xjac*wt
      call ww_phi3m0(xborn(7),xborn(8),p34,p3,p4,wt)
      xjac=xjac*wt
      call ww_phi3m0(xborn(5),xborn(6),p56,p5,p6,wt)
      xjac=xjac*wt

      p(:,1) = p1 
      p(:,2) = p2 
      p(:,3) = p3 
      p(:,4) = p4 
      p(:,5) = p5 
      p(:,6) = p6 

      kn_jacborn = xjac 

      do i=1,6
      kn_pborn(0,i) = p(4,i)
      kn_pborn(1,i) = p(1,i)
      kn_pborn(2,i) = p(2,i)
      kn_pborn(3,i) = p(3,i)
      enddo 

c     now ww_boost everything BACK along z-axis 
      kn_xb1 = xx(1)
      kn_xb2 = xx(2)
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=-1
      call ww_mboost(nlegborn-2,vec,beta,kn_pborn(:,3:),kn_cmpborn(:,3:))
      do mu=0,3
         kn_cmpborn(mu,1)=sqrt(kn_xb1*kn_xb2)*kn_beams(mu,1)
         kn_cmpborn(mu,2)=sqrt(kn_xb1*kn_xb2)*kn_beams(mu,2)
      enddo

c     minimal final state mass 
      if(zerowidth) then
         kn_minmass= 2 * ph_wmass
      else
         kn_minmass=0d0
      endif

c     print out for checks 
      if (debug) then 
c     -- checks invariants, mom. conservation etc in Lab frame  
      write(*,*) '----> Lab FRAME' 
      do i=1,6
         write(*,*) 'pborn', i, kn_pborn(:,i)
      enddo
      write(*,*) 'psum', sum(kn_pborn(:,1:2),dim=2) 
     .     -sum(kn_pborn(:,3:6),dim=2) 
      p34(1:3) = kn_pborn(1:3,3)+kn_pborn(1:3,4)
      p56(1:3) = kn_pborn(1:3,5)+kn_pborn(1:3,6)
      p34(4)   = kn_pborn(0,3)+kn_pborn(0,4)
      p56(4)   = kn_pborn(0,5)+kn_pborn(0,6)
      write(*,*) 'm2(34)',i,p34(4)*p34(4)-
     .        p34(1)*p34(1)-p34(2)*p34(2)-p34(3)*p34(3)
      write(*,*) 'm2(56)',i,p56(4)*p56(4)-
     .        p56(1)*p56(1)-p56(2)*p56(2)-p56(3)*p56(3)

      do i=1,8 
         write(*,*) 'm2',i,kn_pborn(0,i)*kn_pborn(0,i)-
     .        kn_pborn(1,i)*kn_pborn(1,i)-
     .        kn_pborn(2,i)*kn_pborn(2,i)-
     .        kn_pborn(3,i)*kn_pborn(3,i)
      enddo

c     -- checks invariants, mom. conservation etc in CM frame  
      write(*,*) '----> CM FRAME' 
      do i=1,6 
         write(*,*) 'CM pborn', i, kn_cmpborn(:,i)
      enddo
      write(*,*) 'psum', sum(kn_cmpborn(:,1:2),dim=2) 
     .     -sum(kn_cmpborn(:,3:6),dim=2) 

      p34(1:3) = kn_cmpborn(1:3,3)+kn_cmpborn(1:3,4)
      p56(1:3) = kn_cmpborn(1:3,5)+kn_cmpborn(1:3,6)
      p34(4)   = kn_cmpborn(0,3)+kn_cmpborn(0,4)
      p56(4)   = kn_cmpborn(0,5)+kn_cmpborn(0,6)
      write(*,*) 'm2(34)',i,p34(4)*p34(4)-
     .        p34(1)*p34(1)-p34(2)*p34(2)-p34(3)*p34(3)
      write(*,*) 'm2(56)',i,p56(4)*p56(4)-
     .        p56(1)*p56(1)-p56(2)*p56(2)-p56(3)*p56(3)

      do i=1,6 
         write(*,*) 'm2',i,kn_cmpborn(0,i)*kn_cmpborn(0,i)-
     .        kn_cmpborn(1,i)*kn_cmpborn(1,i)-
     .        kn_cmpborn(2,i)*kn_cmpborn(2,i)-
     .        kn_cmpborn(3,i)*kn_cmpborn(3,i)
      enddo

      endif

      end






      subroutine ww_born_suppression(fact)
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      logical ini
      data ini/.true./
      real * 8 fact,pt2supp,ww_powheginput,pt
      save ini,pt2supp,pt     
      if (ini) then
         pt = ww_powheginput("#ptsupp")         
         ini = .false.
         pt2supp = pt**2
      endif
      if (pt.lt.0) then
         fact=1d0
      else         
         fact=1d0
c CAVEAT!!!   No suppression
c         pt2=kn_pborn(1,5)**2+kn_pborn(2,5)**2
c         fact=pt2/(pt2+pt2supp)         
c      if (pt2.gt.10) then
c         fact = 1d0
c      else
c         fact = 0d0
c      endif
      endif
      end


      subroutine ww_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/ww/PhysPars.h'
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      real *8 ww_dotp,ww_powheginput
      external ww_dotp,ww_powheginput
      real * 8 pww(0:3)
      logical fixedscale
      save fixedscale,ini
      if (ini) then

         if (ww_powheginput("#fixedscale").lt.0) then

             write(*,*) '*************************************'
            write(*,*) '    Factorization and renormalization'
            write(*,*) '    Scales mur=muf=M(WW)               '
            write(*,*) '*************************************'
            fixedscale=.false.
         else
            write(*,*) '*************************************'
            write(*,*) '    Factorization and renormalization'
            write(*,*) '    Scales mur=muf=MW               '
            write(*,*) '*************************************'
            fixedscale=.true.
         endif
         ini=.false.
      endif

      if (fixedscale) then
         muf = ph_Wmass
         mur = ph_Wmass
c -- this scale is unobservable, but use it still
      else
         pww = kn_cmpborn(:,3)+kn_cmpborn(:,4) +
     .        kn_cmpborn(:,5)+kn_cmpborn(:,6)
         muf = sqrt(pww(0)**2-pww(1)**2-pww(2)**2-pww(3)**2)
         mur=muf
      endif
      end

