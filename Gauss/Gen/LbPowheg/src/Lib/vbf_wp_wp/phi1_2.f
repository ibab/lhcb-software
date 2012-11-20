      subroutine vbf_wp_wp_phi1_2(x1,x2,x3,x4,p1,p2,p3,n2,n3,
     .     mass2,mass3,width2,width3,wt)
c     massive particle p1 decaying into p2 mass m2 and p3 mass m3.
c     with invariant mass 
c     of particle two s2 and particle three s3 integrated over.
c     vectors returned p2 and p3 are in the same frame as p1 is supplied
c     Expression evaluate is 
c     ds2 ds3 d^4 p2 d^4 p3 (2 pi)^4 delta(p1-p2-p3)/(2 pi)^6 
c     delta(p2^2-s2) delta(p3^2-s3)
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_math.h'
      include 'src/Lib/vbf_wp_wp/PhysPars.h'
      include 'src/Lib/vbf_wp_wp/ww_widths.h'
      double precision p1(4),p2(4),p3(4),p3cm(4)
      double precision x1,x2,x3,x4,costh,sinth,phi,cphi,sphi
      double precision wt,wt0,w2,w3
      double precision s2max,s2min,s3max,s3min,one,two
      double precision m1,m2,s1,s2,s3,lambda,mass2,width2,mass3,width3
      integer j,n2,n3
      parameter(wt0=1d0/8d0/pi)
      parameter(one=1d0)
      parameter(two=2d0)

      wt=0d0
      s1=p1(4)**2-p1(1)**2-p1(2)**2-p1(3)**2  
      if (s1 .lt. 0d0) stop 'vbf_wp_wp_phi1_2: s1 < 0 ' 
      m1=dsqrt(s1)

c--- if both particles are produced on-shell, reject if m1 too small 
c     (should never happen) 
      if (zerowidth.and.(m1 .lt. mass2*dfloat(n2)+mass3*dfloat(n3))) then 
         write(*,*) 'p1', p1 
         write(*,*) 'm1,m2,m3', m1, mass2, mass3, n2, n3 
         stop 'vbf_wp_wp_phi1_2: m1 < m2+m3 ?'  
      endif

      s2min=1d-15
      s2max=s1
      if (s2min .gt. s2max) stop 'vbf_wp_wp_phi1_2: s2min < s2max' 

      if (n2 .eq. 0) then
C     when generating p3456 make sure invariant mass is enough to decay to WW, WpWp specific  
         if (zerowidth) then
            s2min = (2d0*ph_wmass)**2
         else
            s2min = 1d-15
         endif
         w2=s2max-s2min
         s2=s2max*x1+s2min*(1d0-x1)
      elseif (n2 .eq. 1) then
         call vbf_wp_wp_breitw(x1,s2min,s2max,mass2,width2,s2,w2)       
      endif
      
      m2=dsqrt(s2)
      s3min=1d-15
      s3max=(m2-m1)**2
      if (n3 .eq. 0) then
         w3=s3max-s3min
         s3=s3max*x2+s3min*(1d0-x2)
      elseif (n3 .eq. 1) then
         call vbf_wp_wp_breitw(x2,s3min,s3max,mass3,width3,s3,w3)       
      endif

      costh=two*x3-one      
      phi=two*pi*x4
      sinth=dsqrt(one-costh**2)
      cphi=dcos(phi)
      sphi=dsin(phi)
      lambda=((s1-s2-s3)**2-4d0*s2*s3)
      if (lambda .lt. 0d0) stop 'vbf_wp_wp_phi1_2: lambda < 0 ?' 

      lambda=dsqrt(lambda)
      wt=wt0*w2*w3*lambda/s1

      p3cm(4)=m1/two*(s1+s3-s2)/s1
      p3cm(1)=m1/two*lambda/s1*sinth*sphi
      p3cm(2)=m1/two*lambda/s1*sinth*cphi
      p3cm(3)=m1/two*lambda/s1*costh
      call vbf_wp_wp_boost(m1,p1,p3cm,p3)
      do j=1,4
      p2(j)=p1(j)-p3(j)
      enddo
      if (  (p1(4) .lt. 0d0) 
     & .or. (p2(4) .lt. 0d0) 
     & .or. (p3(4) .lt. 0d0)) then 
         stop 'vbf_wp_wp_phi1_2: one of E1,E2,E3 < 0' 
      endif

      end



