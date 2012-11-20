c   Bubble correction: finite part.           
c
c    musq = mu^2 e' una scala dimensionale esterna!!
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2 = 
c          N_ep * vbf_wp_wp_B0fin(p1sq,musq)
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)

      function vbf_wp_wp_B0fin(p1sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_B0fin
      real * 8 p1sq,musq
c     ris(1) = finite part, ris(2) = coeff of 1/ep, 
c     ris(3) = coeff of 1/ep^2      
      complex * 16 ris(3)
      complex * 16 l1
      complex * 16 I
      parameter (I=(0,1))
      double precision pi
      parameter (pi=3.141592653589793238462643383279502884197D0)
      complex * 16 ipi
      parameter (ipi=(0,3.141592653589793238462643383279502884197D0))
      integer j,offshellleg


      complex * 16 vbf_wp_wp_B0
      logical debugB0C0D0
      common/vbf_wp_wp_debug_B0C0D0/debugB0C0D0
      real * 8 tiny
      parameter (tiny=1d-10)
      
      if (debugB0C0D0) then
         vbf_wp_wp_B0fin = vbf_wp_wp_B0(p1sq,musq)
         return
      endif

      if (musq.lt.0) then
         write(*,*) 'ERROR in vbf_wp_wp_B0fin: mu^2 MUST be a positive number'
         stop
      endif

c      if (p1sq.eq.0) then
      if (abs(p1sq).lt.tiny) then
c BUBBLE WITH ZERO EXTERNAL INVARIANTS
c         write(*,*) 'Warning: vbf_wp_wp_B0fin called with external'//
c     #       ' invariant equal to zero'
         ris(3) = 0.d0
         ris(2) = 0.d0
         ris(1) = 0.d0
         vbf_wp_wp_B0fin = ris(1)
         return
      endif
         
      l1 =  log(abs(p1sq/musq))
      if (p1sq.gt.0) then
         l1 = l1 -ipi
      endif
      ris(3) = 0d0
      ris(2) = 1d0
      ris(1) = 2-l1
      vbf_wp_wp_B0fin = ris(1)
      end




c     vbf_wp_wp_C0fin(p1sq,p2sq,p3sq,musq)
c
c             
c              /|===== p3^2
c             / | 
c            /  |
c           /   |
c p1^2 =====    | 
c           \   |
c            \  |
c             \ |
c              \|===== p2^2
c             
c
c    musq = mu^2 e' una scala dimensionale esterna!!
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2 = 
c          N_ep * vbf_wp_wp_C0fin(p1sq,p2sq,p3sq,musq)
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)

      function vbf_wp_wp_C0fin(p1sq,p2sq,p3sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_C0fin
      real * 8 p1sq,p2sq,p3sq,musq
c     ris(1) = finite part, ris(2) = coeff of 1/ep, 
c     ris(3) = coeff of 1/ep^2      
      complex * 16 ris(3)
      real * 8 qsq(3),tmp(3)
      real * 8 vbf_wp_wp_DILOG
      complex * 16 lr,l1,l2,lr2,lr3
      complex * 16 I
      parameter (I=(0,1))
      double precision pi
      parameter (pi=3.141592653589793238462643383279502884197D0)
      complex * 16 ipi
      parameter (ipi=(0,3.141592653589793238462643383279502884197D0))
      integer j,offshellleg,imax,ii
      complex * 16 C03
      real * 8 r3,r2,detsq,det,x,y,max,lomx,lomy
c      complex*16 vbf_wp_wp_C0t

      real * 8 tiny
      parameter (tiny=1d-10)
      

      complex * 16 vbf_wp_wp_C0
      logical debugB0C0D0
      common/vbf_wp_wp_debug_B0C0D0/debugB0C0D0

      if (debugB0C0D0) then
         vbf_wp_wp_C0fin = vbf_wp_wp_C0(p1sq,p2sq,p3sq,musq)
         return
      endif


      if (musq.lt.0) then
         write(*,*) 'ERROR in vbf_wp_wp_C0fin: mu^2 MUST be a positive number'
         stop
      endif
      
c      vbf_wp_wp_C0fin = vbf_wp_wp_C0t(p1sq,p2sq,p3sq,musq)
c      return
      
c      write(*,*) 'vbf_wp_wp_C0 with args',p1sq,p2sq,p3sq,musq

      offshellleg = 0
      do j=1,3 
         qsq(j) = 0.d0
      enddo
c      if (p1sq.ne.0) then
      if (abs(p1sq).gt.tiny) then
         offshellleg = offshellleg +1
         qsq(offshellleg) = p1sq
      endif
c      if (p2sq.ne.0) then
      if (abs(p2sq).gt.tiny) then
         offshellleg = offshellleg +1
         qsq(offshellleg) = p2sq
      endif
c      if (p3sq.ne.0) then
      if (abs(p3sq).gt.tiny) then
         offshellleg = offshellleg +1
         qsq(offshellleg) = p3sq
      endif

      if (offshellleg.eq.1) then
c     TRIANGLE WITH ONLY ONE EXTERNAL INVARIANT      
         l1 =  log(abs(qsq(1)/musq))
         if (qsq(1).lt.0) then
c     do nothing
         else
            l1 = l1 -ipi
         endif
         ris(3) = 1.d0
         ris(2) = -l1
         ris(1) = 1.d0/2*l1**2 - pi**2/6         
         do j=1,3
            ris(j) = ris(j)/qsq(1)
         enddo

      elseif (offshellleg.eq.2) then
c TRIANGLE WITH TWO EXTERNAL INVARIANTS  
         lr = log(abs(qsq(2)/qsq(1)))
         l1 = log(abs(qsq(1)/musq))
         l2 = log(abs(qsq(2)/musq))
         if ((qsq(1).lt.0).and.(qsq(2).lt.0)) then
c     do nothing            
         elseif ((qsq(1).gt.0).and.(qsq(2).lt.0)) then
            lr = lr + ipi
            l1 = l1 - ipi
         elseif ((qsq(1).lt.0).and.(qsq(2).gt.0)) then
            lr = lr - ipi
            l2 = l2 - ipi
         elseif ((qsq(1).gt.0).and.(qsq(2).gt.0)) then
            l1 = l1 - ipi
            l2 = l2 - ipi
         endif
         ris(3) = 0.d0
         ris(2) = lr
         ris(1) = 1.d0/2*(l1**2 - l2**2)
         do j=1,3
            ris(j) = ris(j)/(qsq(1)-qsq(2))
         enddo
      elseif (offshellleg.eq.3) then         
c TRIANGLE WITH THREE EXTERNAL INVARIANTS           
         ris(3) = 0.d0
         ris(2) = 0.d0

c     ris(1) = C03(qsq(1),qsq(2),qsq(3),0d0,0d0,0d0)
         
         
c order the qsq(i) with absolute max value in the first position
         max = 0d0
         do ii=1,3
            if (abs(qsq(ii)).gt.max) then
               max = abs(qsq(ii))
               imax = ii
            endif
         enddo
         tmp(1) = qsq(imax)

         j = 2
         do ii=1,3
            if (ii.ne.imax) then               
               tmp(j) = qsq(ii)
               j = j+1
            endif
         enddo
         do ii=1,3
            qsq(ii) = tmp(ii)
         enddo

c         write(*,*) 'qsq(i)  ============> ', qsq

         r3 = qsq(3)/qsq(1)
         r2 = qsq(2)/qsq(1)

c         if (sqrt(r2)+sqrt(r3).gt.1d0) then 
c            write(*,*) 'NOT YET IMPLEMENTED'
c            stop
c         endif
         
         detsq = (1-r2-r3)**2 - 4*r2*r3
         if (detsq.lt.0d0) then
c            write(*,*) qsq,detsq
c            write(*,*) r2,r3
c            write(*,*) 'WARNING: this case has NOT yet been implemented',detsq
c            write(*,*) 'RETURN 0 from vbf_wp_wp_C0fin function'
            vbf_wp_wp_C0fin = 0d0
            detsq = 0
            RETURN
         endif
         
         det = sqrt(detsq)
         x = 1.d0/2/r2*(r3+r2-1+det)
         y = 1.d0/2/r3*(r3+r2-1+det)

c         write(*,*) 'x, y ==> ',x,y

         if ((x.gt.1d0).or.(y.gt.1d0)) then             
            write(*,*) 'ERROR in vbf_wp_wp_C0fin: x and/or y have values 
     #           bigger than one', x, y
            stop
         endif
        
         lomx = log(1-x)
         lomy = log(1-y)
         lr2 = log(abs(r2))
         lr3 = log(abs(r3))
         if (r2.lt.0d0) then
            if (qsq(1).lt.0) then
               lr2 = lr2 - ipi
            else
               lr2 = lr2 + ipi
            endif
         endif
         if (r3.lt.0d0) then
            if (qsq(1).lt.0) then
               lr3 = lr3 - ipi
            else
               lr3 = lr3 + ipi
            endif
         endif

         
         
         ris(1) = 1/qsq(1)*(1-x)*(1-y)/(1-x*y)*(2*vbf_wp_wp_DILOG(x)+2*vbf_wp_wp_DILOG(y)+
     #        (lomx+lomy)**2+2*lr2*lomy+2*lr3*lomx+lr3*lr2+Pi**2/3)
 

      else
c TRIANGLE WITH ZERO EXTERNAL INVARIANTS
         write(*,*) 'Warning: vbf_wp_wp_C0fin called with external '//
     #        'invariants equal to zero'
         write(*,*) 'dot prods',p1sq,p2sq,p3sq,musq
         ris(3) = 0.d0
         ris(2) = 0.d0
         ris(1) = 0.d0
      endif

      vbf_wp_wp_C0fin = ris(1)
      end



c     vbf_wp_wp_D0fin(s,t,qq1sq,qq2sq,musq)
c                     k
c     p1 ->-----------<-------------<-- q2
c             |                 |
c             |                 |
c             |                 |  
c     p2 ->-------------------------<-- q1
c
c
c    s=(p1+p2)^2
c    t=(p2+q1)^2
c    p1^2=0, p2^2=0, q1^2=qq1sq<>0, q2=qq2sq^2<>0
c    musq = mu^2 e' una scala dimensionale esterna!!
c 
c  int d^dk/(2 pi)^d 1/(k^2)/(k+p1)^2/(k+p1+p2)^2/(k+p1+p2+q1)^2= 
c          N_ep * vbf_wp_wp_D0fin(s,t,q1^2,q2^2)
c          N_ep = i/(4 pi)^2 (4 pi)^ep Gamma(1+ep) (musq)^(-ep)

      function vbf_wp_wp_D0fin(s,t,qq1sq,qq2sq,musq)
      implicit none
      complex * 16 vbf_wp_wp_D0fin
      real * 8 s,t,qq1sq,qq2sq,musq
c     ris(1) = finite part, ris(2) = coeff of 1/ep, 
c     ris(3) = coeff of 1/ep^2      
      complex * 16 ris(3)
      real * 8 r1,r2,l1,l2,l1mr1,l1mr2,lsomu2,tmp,q1sq,q2sq
      real * 8 vbf_wp_wp_DILOG
      complex * 16 I
      parameter (I=(0,1))
      double precision pi
      parameter (pi=3.141592653589793238462643383279502884197D0)
      integer j
c      complex*16  vbf_wp_wp_D0t

      if (musq.lt.0) then
         write(*,*) 'ERROR in vbf_wp_wp_D0fin: mu^2 MUST be a positive number'
         stop
      endif

c      write(*,*) 'D0 with args',s,t,qq1sq,qq2sq,musq

c      vbf_wp_wp_D0fin = vbf_wp_wp_D0t(s,t,qq1sq,qq2sq)
c      return

      q1sq = qq1sq
      q2sq = qq2sq


      if ((q1sq.gt.0d0).and.(q2sq.lt.0d0)) then
c exchange who is positive with the one that is negative         
         tmp = q1sq
         q1sq = q2sq
         q2sq = tmp
      endif
         
      if ((s.lt.0.d0).and.(t.lt.0.d0).and.
     #     (q1sq.lt.0).and.(q2sq.gt.0)) then
         r1 = t/q1sq
         r2 = t/q2sq
         l1 = log(r1)
         l2 = log(-r2)
         l1mr2 = log(1.d0-r2)
         lsomu2 = log(-s/musq)         
         if (r1.lt.1.d0) then
            ris(3) = 1.d0
            ris(2) = -(l1+l2) - I*pi
            ris(1) = -2*(
     #           vbf_wp_wp_DILOG(r1) + l1*log(1.d0-r1) + 
     #           vbf_wp_wp_DILOG(r2) + l2*l1mr2) + 1.d0/2*(l1**2+l2**2)+l1*l2
     #           + I*pi*(-2*l1mr2+l2+l1)
     #           + 1.d0/2*lsomu2**2 - lsomu2*ris(2)
            ris(2) = ris(2) - lsomu2

         elseif (r1.ge.1.d0) then
            ris(3) = 1.d0
            ris(2) = -(l1+l2) - I*pi
            ris(1) = -2*(
     #           -vbf_wp_wp_DILOG(q1sq/t)-1.d0/2*l1**2+pi**2/3+l1*log(r1-1.d0)
     #           +vbf_wp_wp_DILOG(r2) + l2*l1mr2) + 1.d0/2*(l1**2+l2**2)+l1*l2
     #           + I*pi*(-2*l1mr2+l2+l1)
     #           + 1.d0/2*lsomu2**2 - lsomu2*ris(2)
            ris(2) = ris(2) - lsomu2
         endif                    
      elseif ((s.lt.0.d0).and.(t.gt.0.d0).and.
     #        (q1sq.lt.0).and.(q2sq.gt.0)) then
         r1 = t/q1sq
         r2 = t/q2sq
         l1 = log(-r1)
         l2 = log(r2)
         l1mr1 = log(1.d0-r1)
         lsomu2 = log(-s/musq)               
         if (r2.lt.1.d0) then
            ris(3) = 1.d0
            ris(2) = -(l1+l2) + I*pi
            ris(1) = -2*(
     #           vbf_wp_wp_DILOG(r1) + l1*l1mr1 + 
     #           vbf_wp_wp_DILOG(r2) + l2*log(1-r2))+1.d0/2.d0*(l1**2+l2**2)+l1*l2
     #           + I*pi*( 2*l1mr1-l1-l2)
     #           + 1.d0/2*lsomu2**2 - lsomu2*ris(2)
            ris(2) = ris(2) - lsomu2

         elseif (r2.ge.1.d0) then
            ris(3) = 1.d0
            ris(2) = -(l1+l2) + I*pi
            ris(1) = -2*(
     #           vbf_wp_wp_DILOG(r1) + l1*l1mr1  
     #           -vbf_wp_wp_DILOG(q2sq/t)-1.d0/2*l2**2+pi**2/3+l2*log(r2-1.d0))
     #           +1.d0/2.d0*(l1**2+l2**2)+l1*l2
     #           + I*pi*( 2*l1mr1-l2-l1)
     #           + 1.d0/2*lsomu2**2 - lsomu2*ris(2)
            ris(2) = ris(2) - lsomu2
         endif                    
      else
         write(*,*) 'ERROR in the vbf_wp_wp_D0fin function. Cannot be called with
     #        these arguments'
         write(*,*) 's,t,q1^2,q2^2',s,t,q1sq,q2sq
         write(*,*) 'RETURN ZERO!!!'
         vbf_wp_wp_D0fin = 0d0
         return
c         stop
      endif

      do j=1,3
         ris(j) = ris(j) /(s*t)
      enddo
      
      vbf_wp_wp_D0fin = ris(1)
      end


