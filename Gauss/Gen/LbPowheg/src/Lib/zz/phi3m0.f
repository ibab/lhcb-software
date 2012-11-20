      subroutine zz_phi3m0(xth,xphi,p0,p1,p2,wt)
c     massive particle p0 in rest frame 
c     decaying into p1 fixed mass 0 and p2 fixed mass 0.
c     vectors returned p1 and p2 are in the frame in which 
C     p0 is supplied
c result is 1/8/pi * 2|p|/sqrts  * domega/(4*pi)
c     factor of (2*pi)^4 included in definition of phase space
c     Expression evaluated is 
c     d^4 p1 d^4 p2 (2 pi)^4 delta(p0-p1-p2)/(2 pi)^6
c     delta(p2^2) delta(p3^2)
      implicit none
      include 'src/Lib/zz/pwhg_math.h'
      double precision p0(4),p1(4),p2(4),p1cm(4)
      double precision xth,xphi,phi,s,roots,costh,sinth
      double precision wt0,wt
      integer j
      parameter(wt0=1d0/8d0/pi)
      wt=0d0

      s=p0(4)**2-p0(1)**2-p0(2)**2-p0(3)**2  
c      if (s .lt. 0d0) then
c       write(*,*)'zz_phi3m0: s < 0' ,s
C       wt=0d0
c       write(*,*) 'zz_phi3m0: s < 0'
c       call exit(-1)
c         s=0
c      endif

      roots=dsqrt(abs(s))
      costh=2d0*xth-1d0    
      sinth=dsqrt(1d0-costh**2)
      phi=2d0*pi*xphi

      wt=wt0

      p1cm(4)=roots/2d0
      p1cm(1)=roots/2d0*sinth*dsin(phi)
      p1cm(2)=roots/2d0*sinth*dcos(phi)
      p1cm(3)=roots/2d0*costh

      call zz_boost(roots,p0,p1cm,p1)
      do j=1,4
      p2(j)=p0(j)-p1(j)
      enddo

      if (  (p0(4) .lt. 0d0) 
     & .or. (p1(4) .lt. 0d0) 
     & .or. (p2(4) .lt. 0d0)) then  
      write(6,*) 'p0',p0(4),p0(4)**2-p0(1)**2-p0(2)**2-p0(3)**2,s
      write(6,*) 'p1',p1(4),p1(4)**2-p1(1)**2-p1(2)**2-p1(3)**2
      write(6,*) 'p2',p2(4),p2(4)**2-p2(1)**2-p2(2)**2-p2(3)**2
      write(6,*) 'in zz_phi3m0'
      endif

      end

