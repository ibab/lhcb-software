
* \mu_p excluded
      real function GmPhysRevC76_2007_035205(q2)
      real a(3,4),b(5,4)
      data a/-1.465,1.260,0.262,  ! original
     &       -1.472,1.210,0.334,  ! my fit of quoted Ge+dGe values
     &       -1.374,1.080,0.124,  ! my fit of quted Ge-dGe values
     &       -1.436,1.196,0.210   ! my fit of quted Ge values
     &     /
      data b/9.627,0.,0.,11.179,13.245,
     &       9.486,0.,0., 9.440,15.416,
     &      10.003,0.,0., 7.680, 9.009,
     &       9.721,0.,0., 9.623,11.817
     &     /
      save a,b
      real Mp
      parameter (Mp=0.93827231)
      integer mFF
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
      double precision rNum,rDen,tau
c
      n=mFF/10-3
      if(n.lt.1.or.n.gt.3) n=1
      tau=q2/4./Mp**2
      rNum=1
      rDen=1
      do i=1,3
         rNum=rNum+a(i,n)*tau**i
      enddo
      do i=1,5
         rDen=rDen+b(i,n)*tau**i
      enddo
      GmPhysRevC76_2007_035205=rNum/rDen
      if(First.ne.0) then
         print *,'Gm according to Phys Rev C76(2007)035205, n',n
         print *,'  a',(a(i,n),i=1,3)
         print *,'  b',(b(i,n),i=1,5)
         First=0
      endif
      end

