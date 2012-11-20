      subroutine vbf_wp_wp_born_phsp(xborn)
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/PhysPars.h'
      real * 8 xborn(ndiminteg-3)
      real * 8 xjac,tau,y,beta,vec(3)

C     -- WpWp2j stuff 
      double precision p1(4),p2(4),p5(4),p6(4),p3(4),p4(4),p7(4),p8(4)
      double precision p12(4),p78(4),p34(4),p56(4),p3456(4),p(4,8) 
      double precision wt,wt0,wt12,wt34,wt78,wt3456,wt56
      double precision mass2,width2,mass3,width3
      double precision xmin, rtsmin,taumin,sqrts,tmp
      double precision xx(2)
      parameter(wt0=1d0/2d0**4/pi**4)
      integer mu,k,i,n2,n3
      logical ini, debug 
      data ini/.true./
      data debug/.false./
      save ini

      if (debug)  write(*,*) 'Entering vbf_wp_wp_born_phsp: ndiminteg', ndiminteg
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

C     require minimum energy to gerate 2 onshell Ws 
      rtsmin = ph_wmass*2d0  
      taumin = (rtsmin/sqrts)**2
      tau=dexp(dlog(taumin)*xborn(9))
      kn_sborn = kn_sbeams*tau 

      y=0.5d0*dlog(tau)*(1d0-2d0*xborn(10))
      xjac=dlog(taumin)*tau*dlog(tau)

      xx(1)=dsqrt(tau)*dexp(+y)
      xx(2)=dsqrt(tau)*dexp(-y)


c---if x's out of normal range alternative return
      if   ((xx(1) .gt. 1d0)
     & .or. (xx(2) .gt. 1d0)
     & .or. (xx(1) .lt. xmin)
     & .or. (xx(2) .lt. xmin)) write(*,*) 'x1,x2', xx(1),xx(2) 

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

C     mass2, width2 not used for n2 = 0 
C     mass3, width3 not used for n3 = 0 
      n2=0;  mass2=0d0;  width2=0d0 
      n3=0;  mass3=0d0;  width3=0d0
      tmp=xborn(3)**2*(3-2*xborn(3))
      xjac=xjac*6*xborn(3)*(1-xborn(3))
      call vbf_wp_wp_phi1_2(xborn(1),xborn(2),tmp,xborn(4),p12,p3456,p78,
     .     n2,n3,mass2,mass3,width2,width3,wt12)

      n2=1;  mass2=ph_wmass;  width2=ph_wwidth
      n3=1;  mass3=ph_wmass;  width3=ph_wwidth
      
      tmp=xborn(7)**2*(3-2*xborn(7))
      xjac=xjac*6*xborn(7)*(1-xborn(7))
      call vbf_wp_wp_phi1_2(xborn(5),xborn(6),tmp,xborn(8),p3456,p56,p34,
     .     n2,n3,mass2,mass3,width2,width3,wt3456)

      call vbf_wp_wp_phi3m0(xborn(13),xborn(14),p34,p3,p4,wt34)
      call vbf_wp_wp_phi3m0(xborn(11),xborn(12),p56,p5,p6,wt56)
      call vbf_wp_wp_phi3m0(xborn(15),xborn(16),p78,p7,p8,wt78)

      wt=wt0*wt12*wt3456*wt34*wt56*wt78
      xjac = wt*xjac 

      p(:,1) = p1 
      p(:,2) = p2 
      p(:,3) = p3 
      p(:,4) = p4 
      p(:,5) = p5 
      p(:,6) = p6 
      p(:,7) = p7 
      p(:,8) = p8 

      kn_jacborn = xjac 
      do i=1,8 
      kn_pborn(0,i) = p(4,i)
      kn_pborn(1,i) = p(1,i)
      kn_pborn(2,i) = p(2,i)
      kn_pborn(3,i) = p(3,i)
      enddo 

c     now vbf_wp_wp_boost everything BACK along z-axis 
      kn_xb1 = xx(1)
      kn_xb2 = xx(2)
      beta=(kn_xb1-kn_xb2)/(kn_xb1+kn_xb2)
      vec(1)=0
      vec(2)=0
      vec(3)=-1
      call vbf_wp_wp_mboost(nlegborn-2,vec,beta,kn_pborn(:,3:),kn_cmpborn(:,3:))
      do mu=0,3
         kn_cmpborn(mu,1)=sqrt(kn_xb1*kn_xb2)*kn_beams(mu,1)
         kn_cmpborn(mu,2)=sqrt(kn_xb1*kn_xb2)*kn_beams(mu,2)
      enddo

c     minimal final state mass 
      kn_minmass=2d0*sqrt(ph_Wmass2low)

C     print out for checks 
      if (debug) then 
c     -- checks invariants, mom. conservation etc in Lab frame  
      write(*,*) '----> Lab FRAME' 
      do i=1,8 
         write(*,*) 'pborn', i, kn_pborn(:,i)
      enddo
      write(*,*) 'psum', sum(kn_pborn(:,1:2),dim=2) 
     .     -sum(kn_pborn(:,3:8),dim=2) 
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
      do i=1,8 
         write(*,*) 'CM pborn', i, kn_cmpborn(:,i)
      enddo
      write(*,*) 'psum', sum(kn_cmpborn(:,1:2),dim=2) 
     .     -sum(kn_cmpborn(:,3:8),dim=2) 

      p34(1:3) = kn_cmpborn(1:3,3)+kn_cmpborn(1:3,4)
      p56(1:3) = kn_cmpborn(1:3,5)+kn_cmpborn(1:3,6)
      p34(4)   = kn_cmpborn(0,3)+kn_cmpborn(0,4)
      p56(4)   = kn_cmpborn(0,5)+kn_cmpborn(0,6)
      write(*,*) 'm2(34)',i,p34(4)*p34(4)-
     .        p34(1)*p34(1)-p34(2)*p34(2)-p34(3)*p34(3)
      write(*,*) 'm2(56)',i,p56(4)*p56(4)-
     .        p56(1)*p56(1)-p56(2)*p56(2)-p56(3)*p56(3)

      do i=1,8 
         write(*,*) 'm2',i,kn_cmpborn(0,i)*kn_cmpborn(0,i)-
     .        kn_cmpborn(1,i)*kn_cmpborn(1,i)-
     .        kn_cmpborn(2,i)*kn_cmpborn(2,i)-
     .        kn_cmpborn(3,i)*kn_cmpborn(3,i)
      enddo
      endif

      end

      subroutine vbf_wp_wp_born_suppression(fact)
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      logical ini
      data ini/.true./
      real * 8 fact,pt2supp,vbf_wp_wp_powheginput,pt
      save ini,pt2supp,pt     
      if (ini) then
         pt = vbf_wp_wp_powheginput("#ptsupp")         
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


      subroutine vbf_wp_wp_set_fac_ren_scales(muf,mur)
      implicit none
      include 'src/Lib/vbf_wp_wp/PhysPars.h'
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      real * 8 muf,mur
      logical ini
      data ini/.true./
      real *8 muref
      real *8 vbf_wp_wp_dotp,vbf_wp_wp_powheginput
      external vbf_wp_wp_dotp,vbf_wp_wp_powheginput
      real * 8 fixedscale
      real * 8 ptj1,ptj2,etw1,etw2

      ptj1 = sqrt(kn_cmpborn(1,7)**2+kn_cmpborn(2,7)**2)                  ! pt_j1
      ptj2 = sqrt(kn_cmpborn(1,8)**2+kn_cmpborn(2,8)**2)                  ! pt_j2
      etw1 = sqrt(ph_wmass**2+(kn_cmpborn(1,3)+kn_cmpborn(1,4))**2        ! ET_W1
     .     +                  (kn_cmpborn(2,3)+kn_cmpborn(2,4))**2)
      etw2 = sqrt(ph_wmass**2+(kn_cmpborn(1,5)+kn_cmpborn(1,6))**2        ! ET_W2
     .     +                  (kn_cmpborn(2,5)+kn_cmpborn(2,6))**2)

      muref = (ptj1+ptj2+etw1+etw2)/2d0

c uncomment the following, if you want to use a fixed scale set in "powheg.input":
c      if (ini) then
c         fixedscale=vbf_wp_wp_powheginput("#fixedscale")
c      endif
c
c      if(fixedscale.lt.0) then
c         muref=ph_Wmass
c      else
c         muref=fixedscale
c      endif   

      muf=muref
      mur=muref

      if (ini) then
         write(*,*) '*************************************'
         write(*,*) '    Factorization and renormalization '
         write(*,*) '    scales (mur, muf) set to '
         write(*,*) '    sum_{i=1,2}(ptj_i+sqrt(MW^2+ptw_i^2))/2'
c         write(*,*) '    scales set to the mass of the W  '
c         write(*,*) '    scales set to  ',mur,muf
         write(*,*) '*************************************'
         ini=.false.
      endif

      end
