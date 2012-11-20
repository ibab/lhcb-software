      


      subroutine vbf_wp_wp_from_p6_to_p(p1,p2,p3,p4,p5,p6,p)
      implicit none
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3)
      real * 8 p(0:3,6)
      integer mu
      do mu=0,3
         p(mu,1) = p1(mu)
         p(mu,2) = p2(mu)
         p(mu,3) = p3(mu)
         p(mu,4) = p4(mu)
         p(mu,5) = p5(mu)
         p(mu,6) = p6(mu)
      enddo
      end

      subroutine vbf_wp_wp_from_p7_to_p(p1,p2,p3,p4,p5,p6,p7,p)
      implicit none
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3),p7(0:3)
      real * 8 p(0:3,7)
      integer mu
      do mu=0,3
         p(mu,1) = p1(mu)
         p(mu,2) = p2(mu)
         p(mu,3) = p3(mu)
         p(mu,4) = p4(mu)
         p(mu,5) = p5(mu)
         p(mu,6) = p6(mu)
         p(mu,7) = p7(mu)
      enddo
      end

      subroutine vbf_wp_wp_from_p_to_p6(p,p1,p2,p3,p4,p5,p6)
      implicit none
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3)
      real * 8 p(0:3,6)
      integer mu
      do mu=0,3
         p1(mu) = p(mu,1)
         p2(mu) = p(mu,2)
         p3(mu) = p(mu,3)
         p4(mu) = p(mu,4)
         p5(mu) = p(mu,5)
         p6(mu) = p(mu,6)
      enddo
      end

      subroutine vbf_wp_wp_from_p_to_p7(p,p1,p2,p3,p4,p5,p6,p7)
      implicit none
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3),p7(0:3)
      real * 8 p(0:3,7)
      integer mu
      do mu=0,3
         p1(mu) = p(mu,1)
         p2(mu) = p(mu,2)
         p3(mu) = p(mu,3)
         p4(mu) = p(mu,4)
         p5(mu) = p(mu,5)
         p6(mu) = p(mu,6)
         p7(mu) = p(mu,7)
      enddo
      end


      


      subroutine vbf_wp_wp_write_p(p,num_part)
      implicit none
      real * 8 p(0:3,7)
      integer num_part
      integer i,mu
      do i=1,num_part
         write(*,*) 'mom ',i,(p(mu,i),mu=0,3)
      enddo
      end

      subroutine vbf_wp_wp_write_plong(plong,num_part)
      implicit none
      real * 8 plong(0:7,7)
      integer num_part
      integer i,mu
      do i=1,num_part
c         write(*,'(a4,1x,i1,1x,7(g22.14))') 'mom ',i,
c     #        (plong(mu,i),mu=0,7)
         write(*,'(a3,1x,i1,1x,7(f15.9,1x))') 'mom',i,
     #        (plong(mu,i),mu=0,3)
         write(*,'(6x,7(f15.9,1x))') 
     #        (plong(mu,i),mu=4,7)
      enddo
      end

      

c Program to read numbers from strings
      subroutine vbf_wp_wp_reads(string,nstr,rarr,narr,karr)
      implicit real * 8 (a-h,o-z)
      dimension rarr(narr),isign(2)
      real * 8 num(2)
      character * (*) string
      character * 1 ch
      karr=0
c get token
      istr=1
 1    continue
c skip blanks
      if(istr.le.nstr.and.string(istr:istr).eq.' ') then
         istr=istr+1
         goto 1
      endif
      if(istr.gt.nstr) goto 999
      istart=istr
c find next blank
 2    if(istr.le.nstr.and.string(istr:istr).ne.' ') then
         istr=istr+1
         goto 2
      endif
      iend=istr-1 
      iperiod=0
c value
      num(1)=0
c exponent
      num(2)=0
      k=1
      js=istart
 10   if(string(js:js).eq.'-') then
         isign(k)=-1
         js=js+1
      elseif(string(js:js).eq.'+') then
         isign(k)=1
         js=js+1
      else
         isign(k)=1
      endif
      do j=js,iend
         ch=string(j:j)
         if(ch.le.'9'.and.ch.ge.'0') then
            num(k)=num(k)*10+ichar(ch)-ichar('0')
         elseif(ch.eq.'.') then
            if(iperiod.ne.0.or.k.eq.2)goto 998
            iperiod=j-iend
         elseif(ch.eq.'e'.or.ch.eq.'E'.or.ch.eq.'d'.or.ch.eq.'D')then
            if(j.eq.1.or.k.eq.2) goto 998
            if(iperiod.ne.0) iperiod=iperiod+iend-j+1
            k=2
            js=j+1
            goto 10
         else
            goto 999
         endif
       enddo
       karr=karr+1
       rarr(karr)=isign(1)*num(1)*(10.d0)**(isign(2)*num(2)+iperiod)
       if(karr.eq.narr) goto 999
       if(iend.lt.nstr) goto 1
       goto 999
 998   continue
       stop
 999   end

c Program to read integers from strings
      subroutine vbf_wp_wp_ireads(string,nstr,iarr,narr,karr)
      implicit real * 8 (a-h,o-z)
      dimension iarr(narr)
      character * (*) string
      character * 1 ch
      karr=0
c get token
      istr=1
 1    continue
c skip blanks
      if(string(istr:istr).eq.' '.and.istr.le.nstr) then
         istr=istr+1
         goto 1
      endif
      if(istr.gt.nstr) goto 999
      istart=istr
c find next blank
 2    if(string(istr:istr).ne.' '.and.istr.le.nstr) then
         istr=istr+1
         goto 2
      endif
      iend=istr-1 
c value
      num=0
      js=istart
 10   if(string(js:js).eq.'-') then
         isign=-1
         js=js+1
      elseif(string(js:js).eq.'+') then
         isign=1
         js=js+1
      else
         isign=1
      endif
      do j=js,iend
         ch=string(j:j)
         if(ch.le.'9'.and.ch.ge.'0') then
            num=num*10+ichar(ch)-ichar('0')
         else
            goto 999
         endif
       enddo
       karr=karr+1
       iarr(karr)=isign*num
       if(karr.eq.narr) goto 999
       if(iend.lt.nstr) goto 1
       goto 999
 998   continue
       stop
 999   end


c program to read numbers from input line
       subroutine vbf_wp_wp_iread(iun,iarr,nel,nread)
       dimension iarr(40)
       character * 80 str
       read(iun,'(a)') str
       call vbf_wp_wp_ireads(str,80,iarr,nel,nread)
       end




      subroutine vbf_wp_wp_read_all_results()
      implicit none
      integer event1,event2
      integer k
      real * 8 r1,r2
      character *3 string

      open(unit=10,file='all_results.txt',status='old')
      do k=1,1000000
c         read(10,'(i4,g22.14,9x,i4,g22.14)') ! ,err=888
c     #        event1,r1, event2,r2
c         write(*,*) event1,r1,r2
         read(10,*,err=888) 
     #        event1,r1, event2,r2
c         write(*,*) event1,r1,r2
         if (((r1.eq.0d0).and.(r2.ne.0d0)).or.
     #        ((r1.ne.0d0).and.(r2.eq.0d0))) then
            write(*,'(a17,i4,2g22.14)') 'ONLY ONE is ZERO ',event1,
     #           r1,r2
         endif
         if (r1*r2.lt.0d0) then
            write(*,'(a17,i4,2g22.14)') 'WRONG SIGN ',event1,
     #           r1,r2
         endif         
      enddo
 888  continue
      close(10)
      end


      


c if quark_type=1 it computes the counterterm with incoming quark in the vbf_wp_wp_born
c if quark_type=2 it computes the counterterm with incoming antiquark 
c     in the vbf_wp_wp_born
c p1 = in gluon
c p2 = in c 
c p3 = out u
c p4 = out s
c p5 = out e+
c p6 = out ve
c p7 = out ubar
      subroutine vbf_wp_wp_rescaled_momenta_g_old(p,quark_type,rescaledp,Q2,x,z1)
      implicit none
      real * 8 p(0:3,7)
      integer quark_type
      real * 8 amp2    
      real * 8 rescaledp(0:3,6)
      real * 8 p1(0:3),p2(0:3),p3(0:3),p4(0:3),p5(0:3),p6(0:3),p7(0:3)
      real * 8 q(0:3)
      real * 8 Q2,x,z1
      real * 8 vbf_wp_wp_dotp
      integer mu,i

      call vbf_wp_wp_from_p_to_p7(p,p1,p2,p3,p4,p5,p6,p7)
      do i=1,6
         do mu=0,3
            rescaledp(mu,i) = p(mu,i)
         enddo
      enddo
         

      if (quark_type.eq.1) then
c     quark interacting with the rest of the diagram
         do mu=0,3
            q(mu) = p2(mu)-p4(mu)-p5(mu)-p6(mu)
         enddo
         
         Q2 = - vbf_wp_wp_dotp(q,q)
         x = Q2/(2*vbf_wp_wp_dotp(p1,q))
         z1 = vbf_wp_wp_dotp(p3,p1)/vbf_wp_wp_dotp(p1,q)
         
c         write(*,*) 'uu line: x, z1 ', x, z1
         
         do mu=0,3
c     rescale incoming momentum
            rescaledp(mu,1) = x*p(mu,1)
c     rescale outgoing momentum
            rescaledp(mu,3) = q(mu) + x*p(mu,1)
         enddo     
         
      elseif (quark_type.eq.2) then
         
c     antiquark intereacting with the rest of the diagram

         do mu=0,3
            q(mu) = p2(mu)-p4(mu)-p5(mu)-p6(mu)
         enddo
         
         Q2 = - vbf_wp_wp_dotp(q,q)
         x = Q2/(2*vbf_wp_wp_dotp(p1,q))
         z1 = vbf_wp_wp_dotp(p7,p1)/vbf_wp_wp_dotp(p1,q)
         
         do mu=0,3
c     rescale incoming momentum
            rescaledp(mu,1) = x*p(mu,1)
c     rescale outgoing momentum
            rescaledp(mu,3) = q(mu) + x*p(mu,1)
         enddo     
         
      else
         write(*,*) 'ONLY two quark lines in u_and_ubarc_usepve_count'
         stop
      endif                     
      end
      

c return a vbf_wp_wp_random number between 0 and 1
      
c return a complex vbf_wp_wp_random number with real and imaginary part between 0 and 1
      function vbf_wp_wp_crand_num()
      implicit none
      complex * 16 vbf_wp_wp_crand_num
      real * 8 vbf_wp_wp_rand_num, re, im
      re = vbf_wp_wp_rand_num()
      im = vbf_wp_wp_rand_num()
c      vbf_wp_wp_crand_num = complex(re, im)
      vbf_wp_wp_crand_num = CMPLX(re, im)
      end
      
