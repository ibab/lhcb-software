      function GePhysRevC76_2007_035205(q2)
      real a(3,4),b(5,4)
      data a/3.439,-1.602, 0.068,  ! original fit
     &       4.309,-1.108,-0.324,  ! my fit of quoted Ge+dGe values
     &       4.286,-1.281,-0.486,  ! my fit of quted Ge-dGe values
     &       4.109,-1.052,-0.375   ! my fit of quoted Ge values
     &     /
      data b/15.055,48.061,99.304,0.012,8.650,
     &       15.340,58.321,124.11,3.927,0.589,
     &       16.308,54.535,138.03,7.005,0.014,
     &       15.602,55.519,123.96,11.403,1.931
     &     /
      save a,b
      real Mp
      parameter (Mp=0.93827231)
      common /mGmGe/ mFF
      integer First
      data First/1/
      save First
      double precision rNum,rDen,tau
c
      n=mFF/10
      if(n.gt.3) n=n-3
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
      GePhysRevC76_2007_035205=rNum/rDen
      if(First.ne.0) then
         print *,'Ge according to Phys Rev C76(2007)035205, n',n
         print *,'  a',(a(i,n),i=1,3)
         print *,'  b',(b(i,n),i=1,5)
         First=0
      endif
      end

