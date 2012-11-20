      subroutine wz_checkmass(n,p)
      implicit none
      integer n
      real * 8 p(0:3,n)
      real * 8 s(0:3)
      integer mu,j
      do mu=0,3
         s(mu)=0
      enddo
      do j=1,2
         do mu=0,3
            s(mu)=s(mu)+p(mu,j)
         enddo
      enddo
      do j=3,n
         do mu=0,3
            s(mu)=s(mu)-p(mu,j)
         enddo
      enddo
      write(*,*) 'mass',sqrt(s(0)**2-s(1)**2-s(2)**2-s(3)**2)
      end

 
      subroutine wz_checkmomzero(n,p)
      implicit none
      integer n
      real * 8 p(0:3,n)
      real * 8 s(0:3),r(0:3)
      real * 8 ep
      parameter (ep=1d-10)
      integer mu,j
      do mu=0,3
         s(mu)=0
         r(mu)=0
      enddo
      do j=1,2
         do mu=0,3
            s(mu)=s(mu)+p(mu,j)
            r(mu)=r(mu)+abs(p(mu,j))
         enddo
      enddo
      do j=3,n
         do mu=0,3
            s(mu)=s(mu)-p(mu,j)
            r(mu)=r(mu)+abs(p(mu,j))
         enddo
      enddo
      if(s(0)**2+s(1)**2+s(2)**2+s(3)**2.ne.0d0
     # .and. (s(0)**2+s(1)**2+s(2)**2+s(3)**2)
     #  /(r(0)**2+r(1)**2+r(2)**2+r(3)**2).gt.ep) then
         write(*,*) ' momentum check not working',s
      endif
      end

        
