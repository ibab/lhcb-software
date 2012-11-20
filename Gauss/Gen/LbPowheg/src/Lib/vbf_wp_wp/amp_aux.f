c
c---------- vbf_wp_wp_BCD_fill(pb1,pb2,pj1,pj2,pl1,pl2) ---------------------
c
      subroutine vbf_wp_wp_BCD_fill(pb1,pb2,pj1,pj2,pl1,pl2)
      implicit none
      double precision pb1(0:3),pb2(0:3),pj1(0:3),pj2(0:3),
     1                 pl1(0:3),pl2(0:3)
c
c vbf_wp_wp_BCD_fill determines the finite parts of all vbf_wp_wp_B0, C_ij, and D_ij 
c functions for the weak boson fusion process qQ --> qQV, V-->l1+l2 
c from the input momenta
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2002 October 14
c	Last modified:    2002 October 31

c  INPUT:  pb1,pb2           beam momenta
c          pj1,pj2           (anti)quark-jet momenta
c          pl1,pl2           decay momenta of the vector boson V
c
c     pb1  -------------------------- pj1
c                 $         ~ q2                    q2 = pl1+pl2 always
c                 $           ~------ pl1
c                 $ q1         \
c                 $             ----- pl2
c                 $
c     pb2  -------------------------- pj2
c
c The principal output are the D_ij(k2,q2,q1) functions for the various
c crossed processes and associated form factor functions Teps1(q1^2,t),
c Teps2(q2^2,t), Tborn(q1^2,q^2,t) as defined on p.4.8 and 4.9 of my 
c notes. The Tborn defined here combines several terms which appear 
c as coefficients of the vbf_wp_wp_born amplitude M_6:
c
c  Tborn(q1^2,q2^2,t) = T_b(q1^2,t)+T_b(q2^2,t)+vbf_wp_wp_B0t(t)-5+pi^2/3
c
c The D_ij etc. are calculated for ID=1,...,8 cases
c
c   ID = 1    q-line, t-channel, k1 = pb1, k2 = pj1, q1 = pj2-pb2
c                     t = t12 = (q2+pj1)^2
c      = 2    q-line, u-channel, k1 = pb1, k2 = pj1, q1 = pj2-pb2
c                     t = u12 = (q1+pj1)^2    i.e. q1 <--> q2
c      = 3    qbar-line, t-channel, k1 = -pj1, k2 = -pb1, q1 = pj2-pb2
c                     t = u12 = (q1+pj1)^2
c      = 4    qbar-line, u-channel, k1 = -pj1, k2 = -pb1, q1 = pj2-pb2
c                     t = u12 = (q2+pj1)^2    i.e. q1 <--> q2
c
c   ID = 5    Q-line, t-channel, k1 = pb2, k2 = pj2, q1 = pj1-pb1
c                     t = t12 = (q2+pj2)^2
c      = 6    Q-line, u-channel, k1 = pb2, k2 = pj2, q1 = pj1-pb1
c                     t = u12 = (q1+pj2)^2    i.e. q1 <--> q2
c      = 7    Qbar-line, t-channel, k1 = -pj2, k2 = -pb2, q1 = pj1-pb1
c                     t = u12 = (q1+pj2)^2
c      = 8    Qbar-line, u-channel, k1 = -pj2, k2 = -pb2, q1 = pj1-pb1
c                     t = u12 = (q2+pj2)^2    i.e. q1 <--> q2
c
c They are stored in D0v(ID) and Dijv(i,j,ID) etc. Similarly the 
c required Teps1 are stored in Teps1(ID) etc.
c
Common block for the output
      complex*16 D0v(8), Dijv(3,13,8), Teps1(8), Teps2(8), Tborn(8),
     1           Tg1(8), Tg2(8)
      double precision s(8), t(8), q1sq(8), q2sq(8)
      double precision q1(0:3,8), q2(0:3,8), k1(0:3,8), k2(0:3,8)
      common /vbf_wp_wp_bcd_qqv/ s,t,q1sq,q2sq, D0v,Dijv, Teps1,Teps2,Tborn,
     1                                          Tg1,Tg2
      common /vbf_wp_wp_bcd_qqv_aux/ q1,q2,k1,k2
c
c local variables, external functions etc.
      double precision p1p2, p1p3, p2p3
      complex*16 C0123(8),  C0124(8),  C0134(8),  C0234(8)
      complex*16 Cij123(2,4,8), Cij124(2,4,8), 
     1           Cij134(2,4,8), Cij234(2,4,8)
      complex*16 B0a(3), B0aq1(8), B0aq2(8), B0at(8)
      complex*16 vbf_wp_wp_B0t, vbf_wp_wp_C0t, vbf_wp_wp_D0t
      external vbf_wp_wp_B0t, vbf_wp_wp_C0t, vbf_wp_wp_D0t
      double precision s12, t12, u12, s34, t34, u34
      double precision x1,x2,x3,x4, tmq1i, tmq2i, q1v(0:3,2), q2v(0:3)
      double precision vbf_wp_wp_dotp, vbf_wp_wp_psumsq
      external vbf_wp_wp_dotp, vbf_wp_wp_psumsq
      double precision pi,pi2o3
      parameter (pi=3.14159 26535 89793,pi2o3=pi**2/3d0)
      integer id, mu, id0
      data id0/0/
      logical ldebug, ldb0
      parameter (ldebug=.false.)
      common /vbf_wp_wp_bdebug/ ldb0
common block for debugging via vbf_wp_wp_BCD_fill_c
      complex*16 D0c, Dijc(3,13), Teps1c, Teps2c, Tbornc,Tg1c,Tg2c
      double precision sc, tc, q1sqc, q2sqc
      common /vbf_wp_wp_bcd_qqv_c/ sc,tc,q1sqc,q2sqc, D0c,Dijc, 
     1                   Teps1c,Teps2c,Tbornc,Tg1c,Tg2c
c
c set the kinematic variables for the 8 cases
      do mu = 0,3
         q1v(mu,1) = pj2(mu)-pb2(mu)             ! the actual q1 and q2
         q1v(mu,2) = pj1(mu)-pb1(mu)
         q2v(mu) = pl1(mu)+pl2(mu)
c and the effective q1 and q2 for individual subsets of diagrams
         do id = 1,8
            id0 = 1 + id/5
            if (mod(id,2).eq.1) then             ! id=1,3,5,7
               q1(mu,id) = q1v(mu,id0)
               q2(mu,id) = q2v(mu)
            else                                 ! id=2,4,6,8
               q2(mu,id) = q1v(mu,id0)
               q1(mu,id) = q2v(mu)
            endif
         enddo
         do id = 1,2
            k1(mu,id) = pb1(mu)
            k2(mu,id) = pj1(mu)
            k1(mu,id+2) = -pj1(mu)
            k2(mu,id+2) = -pb1(mu)
            k1(mu,id+4) = pb2(mu)
            k2(mu,id+4) = pj2(mu)
            k1(mu,id+6) = -pj2(mu)
            k2(mu,id+6) = -pb2(mu)
         enddo
      enddo
      s12 = -2*vbf_wp_wp_dotp(pb1,pj1)
      t12 = vbf_wp_wp_psumsq(pj1,q2(0,1))
      u12 = vbf_wp_wp_psumsq(pj1,q1(0,1))
      s34 = -2*vbf_wp_wp_dotp(pb2,pj2)
      t34 = vbf_wp_wp_psumsq(pj2,q2(0,5))
      u34 = vbf_wp_wp_psumsq(pj2,q1(0,5))
      do id = 1,4
         s(id) = s12
         s(id+4) = s34
      enddo
      t(1) = t12
      t(2) = u12
      t(5) = t34
      t(6) = u34
      q1sq(1) = vbf_wp_wp_dotp(q1(0,1),q1(0,1))
      q2sq(1) = vbf_wp_wp_dotp(q2(0,1),q2(0,1))
      q1sq(2) = q2sq(1)
      q2sq(2) = q1sq(1)
      q1sq(5) = vbf_wp_wp_dotp(q1(0,5),q1(0,5))
      q2sq(5) = q2sq(1)
      q1sq(6) = q2sq(5)
      q2sq(6) = q1sq(5)
      do id = 3,4
         t(id) = t(5-id)
         q1sq(id) = q1sq(id-2)
         q2sq(id) = q2sq(id-2)
         t(id+4) = t(9-id)
         q1sq(id+4) = q1sq(id+2)
         q2sq(id+4) = q2sq(id+2)
      enddo
      if (ldebug) then
         print*,' table of s,t,q1sq,q2sq '
         print*,' s12, t12, u12 = ',s12,t12,u12
         print*,' s34, t34, u34 = ',s34,t34,u34
         do id = 1,8
            print('(i4,4f18.4)'),id,s(id),t(id),q1sq(id),q2sq(id)
         enddo
         print*,' q1sq,q2sq check '
         do id = 1,8
            print*,id,q1sq(id)/vbf_wp_wp_dotp(q1(0,id),q1(0,id)),
     1                q2sq(id)/vbf_wp_wp_dotp(q2(0,id),q2(0,id))
         enddo
      endif
c      if (ldebug .and. pb1(2).ne.0 .and. pb2(2).ne.0) then
      if (ldebug) then
         do id = 1,8
            ldb0 = id.eq.5
            call vbf_wp_wp_BCD_fill_c(k1(0,id),k2(0,id),q1(0,id),q2(0,id))
            print*,' D0: ',abs(d0v(id)/d0c-1)
            print('(a,3g10.2)'),' D1j ',abs(dijv(1,1,id)/dijc(1,1)-1),
     1                               abs(dijv(1,2,id)/dijc(1,2)-1),
     2                               abs(dijv(1,3,id)/dijc(1,3)-1)
            print('(a,7g10.2)'),' D2j ',abs(dijv(2,1,id)/dijc(2,1)-1),
     1                               abs(dijv(2,2,id)/dijc(2,2)-1),
     1                               abs(dijv(2,3,id)/dijc(2,3)-1),
     1                               abs(dijv(2,4,id)/dijc(2,4)-1),
     1                               abs(dijv(2,5,id)/dijc(2,5)-1),
     1                               abs(dijv(2,6,id)/dijc(2,6)-1),
     1                               abs(dijv(2,7,id)/dijc(2,7)-1)
            print('(a,7g10.2)'),' D3j ',abs(dijv(3,1,id)/dijc(3,1)-1),
     1                               abs(dijv(3,2,id)/dijc(3,2)-1),
     1                               abs(dijv(3,3,id)/dijc(3,3)-1),
     1                               abs(dijv(3,4,id)/dijc(3,4)-1),
     1                               abs(dijv(3,5,id)/dijc(3,5)-1),
     1                               abs(dijv(3,6,id)/dijc(3,6)-1),
     1                               abs(dijv(3,7,id)/dijc(3,7)-1)
            print('(a,7g10.2)'),'     ',abs(dijv(3,8,id)/dijc(3,8)-1),
     1                       abs(dijv(3,9,id)/dijc(3,9)-1),
     1                       abs(dijv(3,10,id)/dijc(3,10)-1),
     1                       abs(dijv(3,11,id)/dijc(3,11)-1),
     1                       abs(dijv(3,12,id)/dijc(3,12)-1),
     1                       abs(dijv(3,13,id)/dijc(3,13)-1)
         enddo
      endif
c
c now determine the required vbf_wp_wp_B0, vbf_wp_wp_C0 and D0 functions
c D0 = vbf_wp_wp_D0t(s,t,q1sq,q2sq) = vbf_wp_wp_D0t(s,t,q2sq,q1sq)
      do id = 1,7,2
         D0v(id) = vbf_wp_wp_D0t(s(id),t(id),q1sq(id),q2sq(id))
      enddo
      do id = 2,6,4
         D0v(id) = D0v(id+1)
         D0v(id+2) = D0v(id-1)
      enddo
c
c C0123 = vbf_wp_wp_C0(0,q2sq,t), C0124 = vbf_wp_wp_C0(0,s,0), C0134 = vbf_wp_wp_C0(t,q1sq,0),
c C0234 = vbf_wp_wp_C0(q2sq,q1sq,s)
      do id = 1,5,4
         C0124(id) = vbf_wp_wp_C0t(0d0,s(id),0d0,-s(id))
         C0234(id) = vbf_wp_wp_C0t(q2sq(id),q1sq(id),s(id),-s(id))
      enddo
      do id = 2,4
         C0124(id) = C0124(1)
         C0234(id) = C0234(1)
         C0124(id+4) = C0124(5)
         C0234(id+4) = C0234(5)
      enddo
      do id = 1,8,2
         C0123(id) = vbf_wp_wp_C0t(0d0,q2sq(id),t(id),-s(id))
         C0134(id) = vbf_wp_wp_C0t(t(id),q1sq(id),0d0,-s(id))
      enddo
      do id = 2,6,4
         C0123(id) = C0134(id+1)
         C0134(id) = C0123(id+1)
         C0123(id+2) = C0134(id-1)
         C0134(id+2) = C0123(id-1)
      enddo
c
      if (ldebug.and..false.) then
         print*,' check of vbf_wp_wp_C0 functions for ID = 1...8 '
         print*,' ID,   C0123,    C0124,    C0134,     C0234/correct-1'
         do id = 1,8
            x1 = abs(C0123(id)/vbf_wp_wp_C0t(q2sq(id),t(id),0d0,-s(id))-1)
            x2 = abs(C0124(id)/vbf_wp_wp_C0t(0d0,s(id),0d0,-s(id))-1)
            x3 = abs(C0134(id)/vbf_wp_wp_C0t(q1sq(id),0d0,t(id),-s(id))-1)
            x4 = abs(C0234(id)/vbf_wp_wp_C0t(q2sq(id),q1sq(id),s(id),-s(id))-1)
            print('(i4,4g12.4)'),id,x1,x2,x3,x4
         enddo
         do id = 1,8
            print*,' id = ',id
            print*,' s,t,u=',s(id),t(id),q1sq(id)+q2sq(id)-s(id)-t(id)
            print*,' q1sq,q2sq = ',q1sq(id),q2sq(id)
            print*,' 2k2.q1 = ',q2sq(id)-s(id)-t(id)
            print*,' 2k2.q2 = ',t(id)-q2sq(id)
         enddo
      endif
!      if (ldebug) pause
c
c and now the vbf_wp_wp_B0 functions: vbf_wp_wp_B0t(q1sq), vbf_wp_wp_B0t(q2sq), vbf_wp_wp_B0t(t)
c Note: vbf_wp_wp_B0t(s,musq=s) = 2
      do id = 1,5,4
         B0aq1(id) = vbf_wp_wp_B0t(q1sq(id),-s(id))
         B0aq2(id) = vbf_wp_wp_B0t(q2sq(id),-s(id))
      enddo
      do id = 2,4
         B0aq1(id) = B0aq2(id-1)
         B0aq2(id) = B0aq1(id-1)
      enddo
      do id = 6,8
         B0aq1(id) = B0aq2(id-1)
         B0aq2(id) = B0aq1(id-1)
      enddo
      do id = 1,2
         B0at(id) = vbf_wp_wp_B0t(t(id),-s(id))
         B0at(id+4) = vbf_wp_wp_B0t(t(id+4),-s(id+4))
      enddo
      do id = 3,7,4
         B0at(id) = B0at(id-1)
         B0at(id+1) = B0at(id-2)
      enddo
c
      if (ldebug) then
         print*,' check of vbf_wp_wp_B0 functions for ID = 1...8 '
         print*,' ID,   vbf_wp_wp_B0(q1sq),  vbf_wp_wp_B0(q2sq),  vbf_wp_wp_B0(t):|stored/correct-1|'
         do id = 1,8
            x1 = abs(B0aq1(id)/vbf_wp_wp_B0t(q1sq(id),-s(id))-1)
            x2 = abs(B0aq2(id)/vbf_wp_wp_B0t(q2sq(id),-s(id))-1)
            x3 = abs(B0at(id)/vbf_wp_wp_B0t(t(id),-s(id))-1)
            x4 = abs(2/vbf_wp_wp_B0t(s(id),-s(id))-1)
            print('(i4,4g12.4)'),id,x1,x2,x3,x4
         enddo
      endif
c
c Now everything is ready to calculate the C_ij and D_ij functions
      do id = 1,8
         B0a(1) = 0
         B0a(2) = B0aq2(id)
         B0a(3) = B0at(id)
         call vbf_wp_wp_tens3(0d0,0d0,q2sq(id),t(id),B0a,C0123(id),
     1              Cij123(1,1,id))
         B0a(1) = 0d0
         B0a(2) = 2
         B0a(3) = 0d0
         call vbf_wp_wp_tens3(0d0,0d0,s(id),0d0,B0a,C0124(id),
     1              Cij124(1,1,id))
         B0a(1) = B0at(id)
         B0a(2) = B0aq1(id)
         B0a(3) = 0d0
         call vbf_wp_wp_tens3(0d0,t(id),q1sq(id),0d0,B0a,C0134(id),
     1              Cij134(1,1,id))
         B0a(1) = B0aq2(id)
         B0a(2) = B0aq1(id)
         B0a(3) = 2
         call vbf_wp_wp_tens3(0d0,q2sq(id),q1sq(id),s(id),B0a,C0234(id),
     1              Cij234(1,1,id))
         p1p2 = (t(id)-q2sq(id))/2
         p1p3 = (q2sq(id)-s(id)-t(id))/2
         p2p3 = (s(id)-q1sq(id)-q2sq(id))/2
         ldb0 = ldebug .and. id.eq.5
         call vbf_wp_wp_tens4( 0d0, 0d0, q2sq(id), q1sq(id), p1p2, p1p3, p2p3, 
     &               C0123(id),  C0124(id),  C0134(id),  C0234(id),
     &               Cij123(1,1,id), Cij124(1,1,id), 
     &               Cij134(1,1,id), Cij234(1,1,id), 
     &               D0v(id), Dijv(1,1,id) )
!         if(ldebug) pause
c and the coefficients of the tensors in M_6:
         tmq1i = 1d0/(t(id)-q1sq(id))
         Teps1(id) = ( (B0at(id)-B0aq1(id))*(2*t(id)+3*q1sq(id))*tmq1i
     &                 +2*B0aq1(id)+1-2*q1sq(id)*C0134(id) ) * tmq1i
         Tg1(id) = (B0aq1(id)-B0at(id))*tmq1i
c
         tmq2i = 1d0/(t(id)-q2sq(id))
         Teps2(id) = ( (B0at(id)-B0aq2(id))*(2*t(id)+3*q2sq(id))*tmq2i
     &                 +2*B0aq2(id)+1-2*q2sq(id)*C0123(id) ) * tmq2i
         Tg2(id) = (B0aq2(id)-B0at(id))*tmq2i
         Tborn(id) = ( 2*q1sq(id)*(B0at(id)-B0aq1(id)) + t(id)*B0at(id)
     &               -q1sq(id)*B0aq1(id) )*tmq1i - 2*q1sq(id)*C0134(id)
     &             + ( 2*q2sq(id)*(B0at(id)-B0aq2(id)) + t(id)*B0at(id)
     &               -q2sq(id)*B0aq2(id) )*tmq2i - 2*q2sq(id)*C0123(id)
     &             + B0at(id) - 5d0 + pi2o3
C NOTE: the finite term (-5+pi^2/3) in Tborn is fixed by EW vbf_wp_wp_gauge invariance!
C Since I have factored out M_born_total*(-2/eps^2 + ... + pi^2/3 - 7),
C and since essentially unrelated couplings will multiply the two sets 
C of boxline graphs, each set individually must vanish when replacing
C eps_i ---> q_i in subroutine boxline
      enddo
      if (ldebug) then
         id0 = mod(id0,8)+1
         write(11,*) ' '
         write(11,*) ' id = ',id0
         write(11,'(a,4g22.14)') ' k1 = ',(k1(mu,id0),mu=0,3)
         write(11,'(a,4g22.14)') ' k2 = ',(k2(mu,id0),mu=0,3)
         write(11,'(a,4g22.14)') ' q1 = ',(q1(mu,id0),mu=0,3)
         write(11,'(a,4g22.14)') ' q2 = ',(q2(mu,id0),mu=0,3)
         write(11,*) ' s, t       = ',s(id0),t(id0)
         write(11,*) ' q1^2, q2^2 = ',q1sq(id0),q2sq(id0)
         write(11,*) ' D0(k2,q2,q1)  = ',d0v(id0)
         write(11,*) ' vbf_wp_wp_C0(k2,q2)     = ',C0123(id0)
         write(11,*) ' vbf_wp_wp_C0(k2,q1+q2)  = ',C0124(id0)
         write(11,*) ' vbf_wp_wp_C0(q2,q1)     = ',C0234(id0)
         write(11,*) ' vbf_wp_wp_C0(k2+q2,q1)  = ',C0134(id0)
         do id = 1,8
            call vbf_wp_wp_BCD_fill_c(k1(0,id),k2(0,id),q1(0,id),q2(0,id))
            print*,' D0: ',abs(d0v(id)/d0c-1)
            print('(a,3g10.2)'),' D1j ',abs(dijv(1,1,id)/dijc(1,1)-1),
     1                               abs(dijv(1,2,id)/dijc(1,2)-1),
     2                               abs(dijv(1,3,id)/dijc(1,3)-1)
            print('(a,7g10.2)'),' D2j ',abs(dijv(2,1,id)/dijc(2,1)-1),
     1                               abs(dijv(2,2,id)/dijc(2,2)-1),
     1                               abs(dijv(2,3,id)/dijc(2,3)-1),
     1                               abs(dijv(2,4,id)/dijc(2,4)-1),
     1                               abs(dijv(2,5,id)/dijc(2,5)-1),
     1                               abs(dijv(2,6,id)/dijc(2,6)-1),
     1                               abs(dijv(2,7,id)/dijc(2,7)-1)
            print('(a,7g10.2)'),' D3j ',abs(dijv(3,1,id)/dijc(3,1)-1),
     1                               abs(dijv(3,2,id)/dijc(3,2)-1),
     1                               abs(dijv(3,3,id)/dijc(3,3)-1),
     1                               abs(dijv(3,4,id)/dijc(3,4)-1),
     1                               abs(dijv(3,5,id)/dijc(3,5)-1),
     1                               abs(dijv(3,6,id)/dijc(3,6)-1),
     1                               abs(dijv(3,7,id)/dijc(3,7)-1)
            print('(a,7g10.2)'),'     ',abs(dijv(3,8,id)/dijc(3,8)-1),
     1                       abs(dijv(3,9,id)/dijc(3,9)-1),
     1                       abs(dijv(3,10,id)/dijc(3,10)-1),
     1                       abs(dijv(3,11,id)/dijc(3,11)-1),
     1                       abs(dijv(3,12,id)/dijc(3,12)-1),
     1                       abs(dijv(3,13,id)/dijc(3,13)-1)
         enddo
      endif
      return
      end

c
c---------------- vbf_wp_wp_BCD_fill_c(k1,k2,q1,q2) -------------------------------
c
      subroutine vbf_wp_wp_BCD_fill_c(k1,k2,q1,q2)
      implicit none
      double precision k1(0:3),k2(0:3),q1(0:3),q2(0:3)
c
c This subroutine does the same as vbf_wp_wp_BCD_fill for a single momentum
c assignment as in the diagram
c
c     k1  -->---------->------------- k2
c                 $         $
c                 $         $
c                 $ q1      $  q2
c                 $         $ 
c
c results are stored in Dijc(*,*) etc. i.e.
c
Common block for the output
      complex*16 D0c, Dijc(3,13), Teps1c, Teps2c, Tbornc,Tg1c,Tg2c
      double precision sc, tc, q1sqc, q2sqc
      common /vbf_wp_wp_bcd_qqv_c/ sc,tc,q1sqc,q2sqc, D0c,Dijc, 
     1                   Teps1c,Teps2c,Tbornc,Tg1c,Tg2c
c
c local variables, external functions etc.
      double precision p1p2, p1p3, p2p3
      complex*16 C0123,  C0124,  C0134,  C0234
      complex*16 Cij123(2,4), Cij124(2,4), 
     1           Cij134(2,4), Cij234(2,4)
      complex*16 B0a(3), B0aq1, B0aq2, B0at, B0as
      complex*16 vbf_wp_wp_B0t, vbf_wp_wp_C0t, vbf_wp_wp_D0t
      external vbf_wp_wp_B0t, vbf_wp_wp_C0t, vbf_wp_wp_D0t
      double precision tmq1i, tmq2i
      double precision vbf_wp_wp_dotp, vbf_wp_wp_psumsq
      external vbf_wp_wp_dotp, vbf_wp_wp_psumsq
      double precision pi,pi2o3
      parameter (pi=3.14159 26535 89793,pi2o3=pi**2/3d0)
c
      sc = -2*vbf_wp_wp_dotp(k1,k2)
      tc = vbf_wp_wp_psumsq(k2,q2)
      q1sqc = vbf_wp_wp_dotp(q1,q1)
      q2sqc = vbf_wp_wp_dotp(q2,q2)
c
c now determine the required vbf_wp_wp_B0, vbf_wp_wp_C0 and D0 functions
c D0 = vbf_wp_wp_D0t(s,t,q1sq,q2sq) = vbf_wp_wp_D0t(s,t,q2sq,q1sq)
      D0c = vbf_wp_wp_D0t(sc,tc,q2sqc,q1sqc)
c
c C0123 = vbf_wp_wp_C0(0,q2sq,t), C0124 = vbf_wp_wp_C0(0,s,0), C0134 = vbf_wp_wp_C0(t,q1sq,0),
c C0234 = vbf_wp_wp_C0(q2sq,q1sq,s)
      C0124 = vbf_wp_wp_C0t(0d0,sc,0d0,-sc)
      C0234 = vbf_wp_wp_C0t(q2sqc,q1sqc,sc,-sc)
      C0123 = vbf_wp_wp_C0t(0d0,q2sqc,tc,-sc)
      C0134 = vbf_wp_wp_C0t(tc,q1sqc,0d0,-sc)
c
c and now the vbf_wp_wp_B0 functions: vbf_wp_wp_B0t(q1sq), vbf_wp_wp_B0t(q2sq), vbf_wp_wp_B0t(t)
c Note: vbf_wp_wp_B0t(s,musq=s) = 2
      B0aq1 = vbf_wp_wp_B0t(q1sqc,-sc)
      B0aq2 = vbf_wp_wp_B0t(q2sqc,-sc)
      B0at  = vbf_wp_wp_B0t(tc,-sc)
      B0as  = vbf_wp_wp_B0t(sc,-sc)
c
c Now everything is ready to calculate the C_ij and D_ij functions
      B0a(1) = 0
      B0a(2) = B0aq2
      B0a(3) = B0at
      call vbf_wp_wp_tens3(0d0,0d0,q2sqc,tc,B0a,C0123,Cij123(1,1))
      B0a(1) = 0d0
      B0a(2) = B0as
      B0a(3) = 0d0
      call vbf_wp_wp_tens3(0d0,0d0,sc,0d0,B0a,C0124,Cij124(1,1))
      B0a(1) = B0at
      B0a(2) = B0aq1
      B0a(3) = 0d0
      call vbf_wp_wp_tens3(0d0,tc,q1sqc,0d0,B0a,C0134,Cij134(1,1))
      B0a(1) = b0aq2
      B0a(2) = B0aq1
      B0a(3) = B0as
      call vbf_wp_wp_tens3(0d0,q2sqc,q1sqc,sc,B0a,C0234,Cij234(1,1))
      p1p2 = vbf_wp_wp_dotp(k2,q2)
      p1p3 = vbf_wp_wp_dotp(k2,q1)
      p2p3 = vbf_wp_wp_dotp(q2,q1)
      call vbf_wp_wp_tens4( 0d0, 0d0, q2sqc, q1sqc, p1p2, p1p3, p2p3, 
     &            C0123,  C0124,  C0134,  C0234,
     &            Cij123(1,1), Cij124(1,1), 
     &            Cij134(1,1), Cij234(1,1), 
     &            D0c, Dijc(1,1) )
c and the coefficients of the tensors in M_6:
      tmq1i = 1d0/(tc-q1sqc)
      Teps1c = ( (B0at-B0aq1)*(2*tc+3*q1sqc)*tmq1i
     &     +2*B0aq1+1-2*q1sqc*C0134 ) * tmq1i
      Tg1c = (B0aq1-B0at)*tmq1i
c
      tmq2i = 1d0/(tc-q2sqc)
      Teps2c = ( (B0at-b0aq2)*(2*tc+3*q2sqc)*tmq2i
     &     +2*b0aq2+1-2*q2sqc*C0123 ) * tmq2i
      Tg2c = (B0aq2-B0at)*tmq2i
      Tbornc = ( 2*q1sqc*(B0at-B0aq1) + tc*B0at
     &     -q1sqc*B0aq1 )*tmq1i - 2*q1sqc*C0134
     &     + ( 2*q2sqc*(B0at-b0aq2) + tc*B0at
     &     -q2sqc*b0aq2 )*tmq2i - 2*q2sqc*C0123
     &     + B0at - 5d0 + pi2o3
      return
      end
c
c---------------  vbf_wp_wp_tens3 = 3point tensors ------------------------------
c
      subroutine vbf_wp_wp_tens3( mt, p1sq, p2sq, psq, vbf_wp_wp_B0, vbf_wp_wp_C0, 
     &                  Cij)
      implicit none
      double precision mt, p1sq, p2sq, psq
      complex*16 vbf_wp_wp_B0(3), vbf_wp_wp_C0, Cij(2,4)
c
c  determine the Passarino-Veltman tensor decomposition for the 
c  three-point tensor integrals
c
c                   1                 1;  k_mu;   k_mu k_nu
c vbf_wp_wp_C0;C_mu;C_munu =------Int d^4k --------------------------------------
c                 i pi^2        [-k^2+mt^2][-(k+p1)^2+mt^2][-(k+p1+p2)^2+mt^2] 
c
c with
c
c   C_mu = p1_mu C11  +  p_2_mu C12
c
c  C_munu = p1_mu p1_nu C21 + p2_mu p2_nu C22 + 
c           (p1_mu p2_nu + p1_nu p2_mu) C23  -  g_munu C24
c
c  for notation see Passarino&Veltman, NP B160 (1979) 151 and my notes
c  on gg-->Hgg, pages 4.1--4.8
c
C INPUT:  mt, p1sq, p2sq, psq=(p1+p2)^2     mass and 4-momenta**2
C         vbf_wp_wp_B0, vbf_wp_wp_C0                   4 scalar integrals; the 3 vbf_wp_wp_B0 are, 
c                                  in PV notation:
c         vbf_wp_wp_B0(1) = vbf_wp_wp_B0(1,2)          B_0 function with subtraction of 
c         vbf_wp_wp_B0(2) = vbf_wp_wp_B0(2,3)          divergent term
c         vbf_wp_wp_B0(3) = vbf_wp_wp_B0(1,3)
c
c OUTPUT: Cij(n,m) = C_nm          form factors in the tensor integrals
c         n=1,2; m=1,2,3,4         a la PV
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2000 November
c	Last modified:    

      real*8 f1, f2, Xi(2,2), p1p2, deti
      complex*16 b13, b12, b23, r1,r2,r3,r4,r5,r6
      complex*16 C11, C12, C21, C22, C23, C24, C23p
      integer i,j, iscount
      logical ldebug
      parameter (ldebug=.false.)
      data iscount /0/
      save iscount

      real*8 vbf_wp_wp_srt
      external vbf_wp_wp_srt

      p1p2 = (psq-p1sq-p2sq)/2
      f1 = p1sq
      f2 = psq - p1sq
      if (psq.eq.0d0) then
         deti = 0.25d0*(p1sq-p2sq)**2
      elseif (p1sq.eq.0d0) then
         deti = 0.25d0*(psq-p2sq)**2
      elseif (p2sq.eq.0d0) then
         deti = 0.25d0*(psq-p1sq)**2
      else
         deti = p1p2**2-p1sq*p2sq
         if (abs(deti).lt.1d-30) then
            do i=1,2
               do j = 1,4
                  Cij(i,j) = 0
               enddo
            enddo
            iscount = iscount+1
            if (iscount.le.1000) then
               print*,' Warning: singular point in vbf_wp_wp_tens3 '
               return
            else
               print*,' stop: too many singular points in vbf_wp_wp_tens3 '
               stop
            endif
         endif
         if (abs(deti).lt.1d-6*max(abs(p1p2**2),abs(p1sq*p2sq))) then
            print*,' singular Cij. deti suppressed by ',
     1           abs(deti)/max(abs(p1p2**2),abs(p1sq*p2sq))
            print*,' p1 =  ',vbf_wp_wp_srt(p1sq)
            print*,' p2 =  ',vbf_wp_wp_srt(p2sq)
            print*,' p12 = ',vbf_wp_wp_srt(psq)
         endif         
      endif
      deti = 1/deti
      Xi(1,1) = p2sq*deti
      Xi(1,2) = -p1p2*deti
      Xi(2,1) = -p1p2*deti
      Xi(2,2) = p1sq*deti
      b12 = vbf_wp_wp_B0(1)
      b23 = vbf_wp_wp_B0(2)
      b13 = vbf_wp_wp_B0(3)
c
      R1 = (f1*vbf_wp_wp_C0+b13-b23)/2
      R2 = (f2*vbf_wp_wp_C0+b12-b13)/2
      C11 = Xi(1,1)*R1 + Xi(1,2)*R2
      C12 = Xi(2,1)*R1 + Xi(2,2)*R2
      Cij(1,1) = C11
      Cij(1,2) = C12
      Cij(1,3) = 0
      Cij(1,4) = 0
c
      C24 = -0.5d0*mt**2*vbf_wp_wp_C0 + 0.25d0*(1+b23-f1*C11-f2*C12)
      R3 = -b13/4 + (b23+f1*C11)/2 - C24
      R4 = -(b13-b23)/4 + f1*C12/2
      R5 = -(b12-b13)/4 + f2*C11/2
      R6 = b13/4 + f2*C12/2 - C24

      Cij(2,1) = Xi(1,1)*R3 + Xi(1,2)*R5
      Cij(2,3) = Xi(2,1)*R3 + Xi(2,2)*R5
      Cij(2,2) = Xi(2,1)*R4 + Xi(2,2)*R6
      Cij(2,4) = C24

      if (ldebug) then
         c21 = cij(2,1)
         c22 = cij(2,2)
         c23 = cij(2,3)
         C23p= Xi(1,1)*R4 + Xi(1,2)*R6
         print*,' C23 comparison ',C23, C23p,C23/(C23p+1d-30)
         print*,'r1:',p1sq*C11+p1p2*c12+0.5d0*(p1sq*vbf_wp_wp_C0+b13-b23)
         print*,'r2:',p1p2*C11+p2sq*C12+0.5d0*((p2sq+2*p1p2)*vbf_wp_wp_C0+
     &                b12-b13)
c         print*,' c24 = ',c24
c         r1 = p1sq*C21+p2sq*C22+2*p1p2*C23-4*c24+b23-mt**2*vbf_wp_wp_C0
c         print*,r1,r1/(c24+1d-60)
         print*,'r4f',p1p2*c22+p1sq*c23+(b13-b23)/4+p1sq*c12/2
         print*,'r4:',p1p2*c22+p1sq*c23-(b13-b23)/4+p1sq*c12/2
         print*,'r5f',p1p2*c21+p2sq*c23+(b12-b13)/4+(2*p1p2+p2sq)*c11/2
         print*,'r5:',p1p2*c21+p2sq*c23-(b12-b13)/4+(2*p1p2+p2sq)*c11/2
      endif

      return
      end
c
c---------------  vbf_wp_wp_tens4 = 4point tensors -----------------------------
c
      subroutine vbf_wp_wp_tens4( mt, p1sq, p2sq, p3sq, p1p2, p1p3, p2p3, 
     &                  C0123,  C0124,  C0134,  C0234,
     &                  Cij123, Cij124, Cij134, Cij234, 
     &                  D0, Dij)
      implicit none
      double precision mt, p1sq, p2sq, p3sq, p1p2, p1p3, p2p3
      complex*16 C0123,  C0124,  C0134,  C0234
      complex*16 Cij123(2,4), Cij124(2,4), Cij134(2,4), Cij234(2,4)
      complex*16 D0, Dij(3,13)
c
c  determine the Passarino-Veltman tensor decomposition for the 
c  four-point tensor integrals
c
c                                       1
c   D0; D_mu; D_mu,nu; D_mu,nu,rho = ------- Int d^4k
c                                     i pi^2 
c
c           1;  k_mu;   k_mu k_nu; k_mu k_nu k_rho
c   -------------------------------------------------------------------
c   [-k^2+mt^2][-(k+p1)^2+mt^2][-(k+p1+p2)^2+mt^2][-(k+p1+p2+p3)^2+m^2]
c
c  with
c
c   D_mu = p1_mu D11  +  p2_mu D12  +  p3_mu D13
c
c   D_munu = p1_mu p1_nu D21 + p2_mu p2_nu D22 + ...
c
c  for notation see Passarino&Veltman, NP B160 (1979) 151 and my notes
c  on gg-->Hgg, pages 4.9--4.19. Note: since I use Bjorken Drell g_munu
c  one needs to replace pi.pj --> -pi.pj, delta_munu = -g_munu in PV
c  to get the results below
c
C INPUT:  mt, p1sq, p1p2,...                mass and 4-momenta**2
C         C0123 = vbf_wp_wp_C0(1,2,3) = vbf_wp_wp_C0(p1,p2)     scalar three point 
C         C0124 = vbf_wp_wp_C0(1,2,4) = vbf_wp_wp_C0(p1,p2+p3)  functions in PV notation
C         C0134 = vbf_wp_wp_C0(1,3,4) = vbf_wp_wp_C0(p1+p2,p3)
C         C0234 = vbf_wp_wp_C0(2,3,4) = vbf_wp_wp_C0(p2,p3)
C         Cij123(n,m) = C_nm(1,2,3) ....    higher C_nm form factors
C                                           as in vbf_wp_wp_tens3
c         D0 = D0(p1,p2,p3)                 scalar four point function
c
c OUTPUT: Dij(n,m) = D_nm                   form factors in the tensor 
c                                           integrals a la PV
c         nm = 11, 12, 13                   ff's for D_mu
c         nm = 21, 22, 23, 24, 25, 26, 27   ff's for D_munu
c         nm = 31, 32, 33, ..., 39, 310, 311, 312  ff's for D_mu,nu,rho
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2000 December 
c	Last modified:    2002 October 29

      real*8 f1, f2, f3, Xi(3,3), X(3,3), deti
      complex*16 r(20:58), Dijp(3,13), t(9), d310pp
      integer i,j, iscount
      logical ldebug, ldb0
      parameter (ldebug=.false.)
      common /vbf_wp_wp_bdebug/ ldb0
      real*8 vbf_wp_wp_srt !, dtexm
      external vbf_wp_wp_srt
      data iscount /0/
      save iscount

      f1 = p1sq
      f2 = p2sq+2*p1p2
      f3 = p3sq+2*(p1p3+p2p3)
      deti = -p1sq*p2sq*p3sq-2*p1p2*p2p3*p1p3
     &       +p1sq*p2p3**2+p2sq*p1p3**2+p3sq*p1p2**2
      do i=1,3
         do j = 1,13
            Dij(i,j) = 0
         enddo
      enddo
      if (abs(deti).lt.1d-30) then
         iscount = iscount+1
         if (iscount.le.1000) then
            print*,' Warning: singular point in vbf_wp_wp_tens4 '
            return
         else
            print*,' stop: too many singular points in vbf_wp_wp_tens4 '
            stop
         endif
      endif
      deti = 1/deti
c
      Xi(1,1) = (p2sq*p3sq-p2p3**2)*deti
      Xi(1,2) = (p1p3*p2p3-p3sq*p1p2)*deti
      Xi(1,3) = (p1p2*p2p3-p2sq*p1p3)*deti

      Xi(2,1) = Xi(1,2)
      Xi(2,2) = (p1sq*p3sq-p1p3**2)*deti
      Xi(2,3) = (p1p2*p1p3-p1sq*p2p3)*deti

      Xi(3,1) = Xi(1,3)
      Xi(3,2) = Xi(2,3)
      Xi(3,3) = (p1sq*p2sq-p1p2**2)*deti

      if (ldb0) then
         print('(g15.4)'),' det(-X) = ',1/deti
         print*,' 1,2,3 ',vbf_wp_wp_srt(p1sq),vbf_wp_wp_srt(p2sq),vbf_wp_wp_srt(p3sq)
c         if (p1sq.ne.0) then
c            dtexm = (p1p2+p2p3)**2*(p1p3+p2p3)**2/
c     1              (p1sq+p2sq+p3sq+2*(p1p2+p1p3+p2p3))
c         elseif (p2sq.ne.0) then
c            dtexm = (p1p2+p1p3)**2*(p1p3+p2p3)**2/
c     1              (p1sq+p2sq+p3sq+2*(p1p2+p1p3+p2p3))
c         else
c            dtexm = (p1p2+p1p3)**2*(p1p2+p2p3)**2/
c     1              (p1sq+p2sq+p3sq+2*(p1p2+p1p3+p2p3))
c         endif
c         print*,' detx ratio ',1/deti/dtexm
         print*,' 12 ',vbf_wp_wp_srt(p1sq+2*p1p2+p2sq),' 13 ',
     1          vbf_wp_wp_srt(p1sq+2*p1p3+p3sq),' 23 ',vbf_wp_wp_srt(p2sq+2*p2p3+p3sq)
         print*,' check inversion of X matrix for n = 3 '
         X(1,1) = -p1sq
         X(1,2) = -p1p2
         X(1,3) = -p1p3
         X(2,1) = -p1p2
         X(2,2) = -p2sq
         X(2,3) = -p2p3
         X(3,1) = -p1p3
         X(3,2) = -p2p3
         X(3,3) = -p3sq
         do i = 1,3
            do j = 1,3
               print*,'X*X^-1(',i,j,') = ', 
     &                X(i,1)*Xi(1,j)+X(i,2)*Xi(2,j)+X(i,3)*Xi(3,j)
            enddo
         enddo
         print*,' Xi = '
         do i = 1,3
            print 13,(Xi(i,j),j=1,3)
         enddo
         print*,' X = '
         do i = 1,3
            print 13,(X(i,j),j=1,3)
         enddo
 13      format (3g14.4)
      endif
c
      if (ldb0) then
         if (.false.) then
            C0123 = 0
            c0124 = 0
            c0234 = 0
            c0134 = 0
            do i=1,2
               do j = 1,4
                  cij123(i,j) = 0
                  cij124(i,j) = 0
                  cij234(i,j) = 0
                  cij134(i,j) = 0
               enddo
            enddo
         endif
         print*,' input for D1j '
         print*,' D0 = ',d0
         print*,' C0123 = ',C0123
         print*,' C0124 = ',C0124
         print*,' C0234 = ',C0234
         print*,' C0134 = ',C0134
      endif
      R(20) = (f1*D0+C0134-C0234)/2
      R(21) = (f2*D0+C0124-C0134)/2
      R(22) = (f3*D0+C0123-C0124)/2
      Dij(1,1) = Xi(1,1)*R(20) + Xi(1,2)*R(21) + Xi(1,3)*R(22)
      Dij(1,2) = Xi(2,1)*R(20) + Xi(2,2)*R(21) + Xi(2,3)*R(22)
      Dij(1,3) = Xi(3,1)*R(20) + Xi(3,2)*R(21) + Xi(3,3)*R(22)
c      do j = 4,13
c         Dij(1,j) = 0
c      enddo
ccccccc
      if (ldebug) then
         print*,' r20 = ',R(20)
         print*,' r21 = ',R(21)
         print*,' r22 = ',R(22)
         print*,'r20c ',
     &           -(p1sq*dij(1,1)+p1p2*dij(1,2)+p1p3*dij(1,3))/R(20)
         print*,'r21c ',
     &           -(p1p2*dij(1,1)+p2sq*dij(1,2)+p2p3*dij(1,3))/R(21)
         print*,'r22c ',
     &           -(p1p3*dij(1,1)+p2p3*dij(1,2)+p3sq*dij(1,3))/R(22)
      endif
ccccccc
      Dij(2,7) = -mt**2*D0 
     &           - (f1*dij(1,1)+f2*dij(1,2)+f3*dij(1,3)-C0234)/2
c
      R(30) = (f1*Dij(1,1)+Cij134(1,1)+C0234)/2 - Dij(2,7)
      R(33) = (f1*Dij(1,2)+Cij134(1,1)-Cij234(1,1))/2
      R(36) = (f1*Dij(1,3)+Cij134(1,2)-Cij234(1,2))/2

      R(31) = (f2*Dij(1,1)+Cij124(1,1)-Cij134(1,1))/2
      R(34) = (f2*Dij(1,2)+Cij124(1,2)-Cij134(1,1))/2 - Dij(2,7)
      R(37) = (f2*Dij(1,3)+Cij124(1,2)-Cij134(1,2))/2

      R(32) = (f3*Dij(1,1)+Cij123(1,1)-Cij124(1,1))/2
      R(35) = (f3*Dij(1,2)+Cij123(1,2)-Cij124(1,2))/2
      R(38) = (f3*Dij(1,3)-Cij124(1,2))/2 - Dij(2,7)

      Dij(2,1) = Xi(1,1)*R(30) + Xi(1,2)*R(31) + Xi(1,3)*R(32)
      Dij(2,4) = Xi(2,1)*R(30) + Xi(2,2)*R(31) + Xi(2,3)*R(32)
      Dij(2,5) = Xi(3,1)*R(30) + Xi(3,2)*R(31) + Xi(3,3)*R(32)

      Dij(2,2) = Xi(2,1)*R(33) + Xi(2,2)*R(34) + Xi(2,3)*R(35)
      Dij(2,6) = Xi(3,1)*R(33) + Xi(3,2)*R(34) + Xi(3,3)*R(35)

      Dij(2,3) = Xi(3,1)*R(36) + Xi(3,2)*R(37) + Xi(3,3)*R(38)
cccccccccc
      if (ldebug) then
         Dijp(2,4) =Xi(1,1)*R(33) + Xi(1,2)*R(34) + Xi(1,3)*R(35)
         Dijp(2,5) = Xi(1,1)*R(36) + Xi(1,2)*R(37) + Xi(1,3)*R(38)
         Dijp(2,6) = Xi(2,1)*R(36) + Xi(2,2)*R(37) + Xi(2,3)*R(38)
         print*,' ratios of 2 versions for D24, D25, D26 '
         print*,' D24 ',dij(2,4),dij(2,4)/dijp(2,4)
         print*,' D25 ',dij(2,5),dij(2,5)/dijp(2,5)
         print*,' D26 ',dij(2,6),dij(2,6)/dijp(2,6)
      endif
cccccccccc
c      do j = 8,13
c         Dij(2,j) = 0
c      enddo
c
      Dij(3,11) = -mt**2/2*dij(1,1) 
     &            - (f1*Dij(2,1)+f2*dij(2,4)+f3*dij(2,5)+C0234)/4
      Dij(3,12) = -mt**2/2*dij(1,2) 
     &            - (f1*Dij(2,4)+f2*dij(2,2)+f3*dij(2,6)-Cij234(1,1))/4
      Dij(3,13) = -mt**2/2*dij(1,3) 
     &            - (f1*Dij(2,5)+f2*dij(2,6)+f3*dij(2,3)-Cij234(1,2))/4
c
      R(41) = (f1*Dij(2,1)+Cij134(2,1)-C0234)/2 - 2*Dij(3,11)
      R(42) = (f2*Dij(2,1)+Cij124(2,1)-Cij134(2,1))/2
      R(43) = (f3*Dij(2,1)+Cij123(2,1)-Cij124(2,1))/2

      R(44) = (f1*Dij(2,4)+Cij134(2,1)+Cij234(1,1))/2 - Dij(3,12)
      R(50) = (f1*Dij(2,2)+Cij134(2,1)-Cij234(2,1))/2 
      R(56) = (f1*Dij(2,3)+Cij134(2,2)-Cij234(2,2))/2

      R(45) = (f2*Dij(2,4)+Cij124(2,3)-Cij134(2,1))/2 - Dij(3,11)
      R(51) = (f2*Dij(2,2)+Cij124(2,2)-Cij134(2,1))/2 - 2*Dij(3,12)
      R(57) = (f2*Dij(2,3)+Cij124(2,2)-Cij134(2,2))/2 

      R(46) = (f3*Dij(2,4)+Cij123(2,3)-Cij124(2,3))/2
      R(52) = (f3*Dij(2,2)+Cij123(2,2)-Cij124(2,2))/2
      R(58) = (f3*Dij(2,3)            -Cij124(2,2))/2 - 2*Dij(3,13) 

      Dij(3,1) = Xi(1,1)*R(41) + Xi(1,2)*R(42) + Xi(1,3)*R(43)
      Dij(3,4) = Xi(2,1)*R(41) + Xi(2,2)*R(42) + Xi(2,3)*R(43)
      Dij(3,5) = Xi(3,1)*R(41) + Xi(3,2)*R(42) + Xi(3,3)*R(43)

      Dij(3,6) = Xi(1,1)*R(50) + Xi(1,2)*R(51) + Xi(1,3)*R(52)
      Dij(3,2) = Xi(2,1)*R(50) + Xi(2,2)*R(51) + Xi(2,3)*R(52)
      Dij(3,8) = Xi(3,1)*R(50) + Xi(3,2)*R(51) + Xi(3,3)*R(52)

      Dij(3,7) = Xi(1,1)*R(56) + Xi(1,2)*R(57) + Xi(1,3)*R(58)
      Dij(3,9) = Xi(2,1)*R(56) + Xi(2,2)*R(57) + Xi(2,3)*R(58)
      Dij(3,3) = Xi(3,1)*R(56) + Xi(3,2)*R(57) + Xi(3,3)*R(58)

      Dij(3,10) = Xi(3,1)*R(44) + Xi(3,2)*R(45) + Xi(3,3)*R(46)
ccccccccccc
      if (ldb0) then
         print*,' R_20/R_22 = ',r(20)/r(22)
         print*,' R_30/R_32 = ',r(30)/r(32)
         print*,' R_33/R_35 = ',r(33)/r(35)
         print*,' R_36/R_38 = ',r(36)/r(38)

         print*,' R_41/R_43 = ',r(41)/r(43)
         print*,' R_50/R_52 = ',r(50)/r(52)
         print*,' R_56/R_58 = ',r(56)/r(58)
         print*,' R_44/R_46 = ',r(44)/r(46)
      endif
      if (ldebug) then
         Dijp(3,4) = Xi(1,1)*R(44) + Xi(1,2)*R(45) + Xi(1,3)*R(46)
         Dijp(3,6) = Xi(2,1)*R(44) + Xi(2,2)*R(45) + Xi(2,3)*R(46)
         print*,' ratios of 2 versions for D3j '
         print*,' D34 ',dij(3,4),dij(3,4)/dijp(3,4)
         print*,' D36 ',dij(3,6),dij(3,6)/dijp(3,6)
         t(1) = (cij134(2,3)+f1*Dij(2,5)+cij234(1,2))/2-dij(3,13)
         t(2) = (cij134(2,3)+f1*Dij(2,6)-cij234(2,3))/2
         t(3) = (cij134(2,4)+f1*Dij(2,7)-cij234(2,4))/2

         t(4) = (cij124(2,3)-cij134(2,3)+f2*Dij(2,5))/2
         t(5) = (cij124(2,2)-cij134(2,3)+f2*Dij(2,6))/2-dij(3,13)
         t(6) = (cij124(2,4)-cij134(2,4)+f2*Dij(2,7))/2

         t(7) = (           -cij124(2,3)+f3*Dij(2,5))/2-dij(3,11)
         t(8) = (           -cij124(2,2)+f3*Dij(2,6))/2-dij(3,12)
         t(9) = (cij123(2,4)-cij124(2,4)+f3*Dij(2,7))/2

         Dijp(3,5) = Xi(1,1)*t(1) + Xi(1,2)*t(4) + Xi(1,3)*t(7)
         Dijp(3,10)= Xi(2,1)*t(1) + Xi(2,2)*t(4) + Xi(2,3)*t(7)
         Dijp(3,7) = Xi(3,1)*t(1) + Xi(3,2)*t(4) + Xi(3,3)*t(7)

         D310pp    = Xi(1,1)*t(2) + Xi(1,2)*t(5) + Xi(1,3)*t(8)
         Dijp(3,8) = Xi(2,1)*t(2) + Xi(2,2)*t(5) + Xi(2,3)*t(8)
         Dijp(3,9) = Xi(3,1)*t(2) + Xi(3,2)*t(5) + Xi(3,3)*t(8)
         
         Dijp(3,11)= Xi(1,1)*t(3) + Xi(1,2)*t(6) + Xi(1,3)*t(9)
         Dijp(3,12)= Xi(2,1)*t(3) + Xi(2,2)*t(6) + Xi(2,3)*t(9)
         Dijp(3,13)= Xi(3,1)*t(3) + Xi(3,2)*t(6) + Xi(3,3)*t(9)

         print*,' D35 ',dij(3,5),dij(3,5)/dijp(3,5)
         print*,' D37 ',dij(3,7),dij(3,7)/dijp(3,7)
         print*,' D38 ',dij(3,8),dij(3,8)/dijp(3,8)
         print*,' D39 ',dij(3,9),dij(3,9)/dijp(3,9)
         print*,' D310 ',dij(3,10),dij(3,10)/dijp(3,10)
         print*,' D310pp ',d310pp,d310pp/dij(3,10)
         print*,' D311 ',dij(3,11),dij(3,11)/dijp(3,11)
         print*,' D312 ',dij(3,12),dij(3,12)/dijp(3,12)
         print*,' D313 ',dij(3,13),dij(3,13)/dijp(3,13)
      endif
cccccccccccc
      return
      end
c
c----  vbf_wp_wp_B0t(qsq): regularized 2-point function for massless loop  -----
c
      complex*16 function vbf_wp_wp_B0t(qsq,musq)
      implicit none
      double precision qsq, musq

c evaluate the finite piece of the massless vbf_wp_wp_B0 function
c  
c             d^d k 
c    vbf_wp_wp_B0 = Int------- [-k^2+idelta]^-1 [-(k-q)^2+idelta]^-1 
c             i pi^2
c
c       = pi^-eps Gamma(1+eps) musq^-eps [1/eps + vbf_wp_wp_B0t(qsq,musq)]
c
c i.e. vbf_wp_wp_B0t(qsq,musq) = 2 - log( (-qsq-idelta)/musq )
c
c subroutine requires positive musq
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2002 September 13
c	Last modified:    2002 September 13
c  
      complex*16 ipi
      parameter (ipi=(0d0,3.14159 26535 89793))
      logical ldebug
      parameter (ldebug=.true.)
c
      if (ldebug) then
         if (musq.le.0d0) then
            print*,' Unacceptable mu^2 in vbf_wp_wp_B0t(q^2,mu^2): mu^2 = ',
     1             musq,' ==> set vbf_wp_wp_B0t = 0 '
            vbf_wp_wp_B0t=0
            return
         endif
      endif
c
      if (qsq.lt.0d0) then
         vbf_wp_wp_B0t = 2 - log(-qsq/musq)
      else
         vbf_wp_wp_B0t = 2 - log(qsq/musq) + ipi
      endif
      end
c
c------------- vbf_wp_wp_C0(q1^2,q2^2,P^2)  massless 3-point fuction ------------
c
      complex*16 function vbf_wp_wp_C0t(q1sq,q2sq,Psq,musq)
      implicit none
      double precision q1sq, q2sq, psq, musq

c evaluate the finite part of the scalar 3-point function for zero mass
c propagators 
c  
c  vbf_wp_wp_C0 = 1/(i*pi^2) * Int d^dk [-k^2-i0]^-1 [-(k+q1)^2-i0]^-1 [-(k-q2)^2-i0]^-1
c
c       = pi^-eps Gamma(1+eps) musq^-eps [IR + vbf_wp_wp_C0t(q1sq,q2sq,psq)]
c
c where IR represents the potentially divergent terms when one or more
c of the arguments vanish. P = q1+q2. The divergent terms are
c 
c (1) IR = 0              all 3 of q1sq,q2sq,psq nonzero
c 
c (2) IR = 1/(t-qsq)*log(t/qsq)*1/eps    for 1 vanishing argument, the
c                                        other two being t and qsq
c
c (3) IR = -1/qsq*(1/eps**2 - 1/eps log(qsq/musq))  when exactly one 
c                                                   argument, qsq,
c                                                   is nonzero
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2002 September 13
c	Last modified:    2002 October 25
c  
      complex*16 ipi, z, res, vbf_wp_wp_li2
      parameter (ipi=(0d0,3.14159 26535 89793))
      external vbf_wp_wp_li2
      double precision pi,pi2o3,pi2o6,pi2o2
      parameter (pi=3.14159 26535 89793,pi2o3=pi**2/3d0)
      parameter (pi2o6=pi**2/6d0,pi2o2=pi**2/2d0)
      logical ldebug, ltaylor, first_call
      parameter (ldebug=.false.)
      double precision x, flambda, tsqlami, qsq(3), 
     1                 sum, rz, logz, ln1, ln2,
     2                 musq_t, asum, fac, resr, ser, fb(8), xn
      integer izero, inz, i, j
      data first_call /.true./
      save first_call, fb
c
      if (ldebug) then
         if (musq.le.0d0) then
            print*,' Unacceptable mu^2 in vbf_wp_wp_C0t(q1sq,q2sq,psq,mu^2): ',
     1         'mu^2 = ',musq,' ==> set vbf_wp_wp_C0t = 0 '
            vbf_wp_wp_C0t=0
            return
         endif
      endif
c
      inz = 0
      izero = 0
      do i=1,3
         qsq(i)=0
      enddo
      if (q1sq.eq.0d0) then
         izero = izero+1
      else
         inz = inz+1
         qsq(inz)=q1sq
      endif
      if (q2sq.eq.0d0) then
         izero = izero+1
      else
         inz = inz+1
         qsq(inz)=q2sq
      endif
      if (psq.eq.0d0) then
         izero = izero+1
      else
         inz = inz+1
         qsq(inz)=psq
      endif
      if (ldebug) then
         print('(a,3g12.4,a,g12.4)'), 'qsq_i = ',q1sq,q2sq,psq,
     1                                ' mu^2 = ',musq
         print*,' number of q_i^2 = 0  ',izero
         print*,' number of q_i^2 ne 0 ',inz,' Sum = ',inz+izero
         if ((inz+izero).ne.3) print*,' WARNING: inz+izero .ne. 3 '
      endif
c
      if (izero.eq.0) then   ! all qsq(i) nonzero, this is full vbf_wp_wp_C0
         flambda = (qsq(1)**2+qsq(2)**2+qsq(3)**2) - 
     1           2*(qsq(1)*qsq(2)+qsq(1)*qsq(3)+qsq(2)*qsq(3))
         sum = qsq(1)+qsq(2)+qsq(3)
         asum = abs(qsq(1))+abs(qsq(2))+abs(qsq(3))
         ltaylor = abs(abs(sum)/asum-1).lt.1d-13      ! same sign for all qsq
         asum=min(abs(sum-2*qsq(1)),abs(sum-2*qsq(2)),
     1            abs(sum-2*qsq(3)))
         ltaylor = ltaylor .and. abs(flambda).lt.(0.01*asum**2)
c
         res = (0d0,0d0)
         if (ldebug) then
            print*,' lambda = ',flambda/4,' x_taylor = ',
     1                          flambda/asum**2
            ltaylor=.false.
         endif
         if (flambda.gt.0d0 .and. .not.ltaylor) then
            tsqlami = 1/sqrt(flambda)
            do i=1,3
               x = (sum-2*qsq(i))*tsqlami
               if (x.gt.1d0) then
                  rz = (x-1)/(x+1)
                  z = rz
                  logz = log(rz)
                  res = res + 2*vbf_wp_wp_li2(z)+0.5d0*logz**2-pi2o3 
                  if (qsq(i).gt.0d0) then
                     res = res + ipi*logz
                  else
                     res = res - ipi*logz
                  endif
               elseif (x.gt.0d0) then
                  rz = (x-1)/(x+1)
                  z = rz
                  logz = log(-rz)
                  res = res + 2*vbf_wp_wp_li2(z)+0.5d0*logz**2+pi2o6
               elseif (x.gt.-1d0) then
                  rz = (x+1)/(x-1)
                  z = rz
                  logz = -log(-rz)
                  res = res - 2*vbf_wp_wp_li2(z)-0.5d0*logz**2-pi2o6
               else 
                  rz = (x+1)/(x-1)
                  z = rz
                  logz = -log(rz)
                  res = res - 2*vbf_wp_wp_li2(z)-0.5d0*logz**2+pi2o3 
                  if (qsq(i).gt.0d0) then
                     res = res + ipi*logz
                  else
                     res = res - ipi*logz
                  endif
               endif
               if (ldebug) print*,' i,x, z = ',i,x,rz
            enddo
            vbf_wp_wp_C0t = res*tsqlami
         elseif (flambda.lt.0d0 .and. .not.ltaylor) then  ! the lambda < 0 case
            tsqlami = 1/sqrt(-flambda)         ! + imag part of 2 sqrt(lambda)
            do i=1,3
               x = (sum-2*qsq(i))*tsqlami
               z = dcmplx(x,-1d0)/dcmplx(x,1d0)
               if (ldebug) then  
                  print*,' i,x, z = ',i,x,z
               endif
               res = res + dimag(vbf_wp_wp_li2(z))
            enddo
            vbf_wp_wp_C0t = res*2*tsqlami
         endif
         if (ldebug) then
            print*,' vbf_wp_wp_C0t via vbf_wp_wp_li2 calls: ',vbf_wp_wp_C0t
            ltaylor = abs(flambda).lt.(0.5*asum**2)
         endif
         if (ltaylor) then
            if (first_call) then
               fb(1) = 2d0/9d0
               fb(2) = 16d0/75d0
               fb(3) = 142d0/735d0
               fb(4) = 496d0/2835d0
               fb(5) = 6086d0/38115d0
               fb(6) = 86048d0/585585d0
               fb(7) = 92054d0/675675d0
               fb(8) = 1655008d0/13018005d0 
c               fb(9) = 32976682d0/276441165d0
               first_call = .false.
            endif
            musq_t = abs(sum/3)
            resr = 0
            do i = 1,3
               fac = 1/(sum-2*qsq(i))
               x = flambda*fac**2
c               if (ldebug) print*,' i = ',i,' x = ',x
               ln1 = log(abs(qsq(i))/musq_t)
               ser = ln1
               j = 1
               xn = x
c               if (ldebug) print*,' log = ',ln1
               do while (abs(xn).gt.1d-15 .and. j.lt.9)
                  ser = ser + xn*(ln1/(2*j+1)-fb(j))
c                  if (ldebug) then
c                     print*,' n = ',j,' a_n = ',ln1/(2*j+1)-fb(j)
c                  endif
                  xn = xn*x
                  j = j+1
               enddo
               if (ldebug) print*,' C_i = ',2*ser*fac
               resr = resr + 2*ser*fac
            enddo
            if (ldebug) then
               print*,' vbf_wp_wp_C0t via vbf_wp_wp_li2 calls: ',vbf_wp_wp_C0t
               print*,' vbf_wp_wp_C0t via Taylor exp.',dcmplx(resr,0d0)
            endif
            vbf_wp_wp_C0t = dcmplx(resr,0d0)
         endif
      elseif (izero.eq.1) then
         ln1 = log(abs(qsq(1))/musq)
         ln2 = log(abs(qsq(2))/musq)
         res = 0.5d0*(ln1**2-ln2**2)
         if (qsq(1).gt.0d0) then
            res = res - ipi*ln1 - pi2o2
         endif
         if (qsq(2).gt.0d0) then
            res = res + ipi*ln2 + pi2o2
         endif
         vbf_wp_wp_C0t = res/(qsq(2)-qsq(1))
      elseif (izero.eq.2) then
         ln1 = log(abs(qsq(1))/musq)
         res = 0.5d0*ln1**2-pi2o6
         if (qsq(1).gt.0d0) then
            res = res - ipi*ln1 - pi2o2
         endif
         vbf_wp_wp_C0t = -res/qsq(1)
      else
         print*,' WARNING: vbf_wp_wp_C0t called with 3 zero arguments '
         print*,' vbf_wp_wp_C0t reset to 0 '
         vbf_wp_wp_C0t = 0
      endif
      if (ldebug) then
         print*,' output: vbf_wp_wp_C0t = ',vbf_wp_wp_C0t
      endif
      return 
      end
c
c------------- D0(s,t,q1sq,q2sq)  massless 4-point function -----------
c
      complex*16 function vbf_wp_wp_D0t(s,t,q1sq,q2sq)
      implicit none
      double precision s, t, q1sq, q2sq
      double precision sl, tl, q1sql, q2sql
      double precision p1sq, p2sq, p3sq, p1p2, p1p3, p2p3

c evaluate the finite part of the scalar 4-point function for zero mass
c propagators 
c  
c  D0 = 1/(i*pi^2) * Int d^dk [-k^2-i0]^-1 [-(k+p1)^2-i0]^-1 
c                      [-(k+p1+p2)^2-i0]^-1 [-(k+p1+p2+p3)^2-i0]^-1
c
c       = pi^-eps Gamma(1+eps) (-s)^-eps [IR + vbf_wp_wp_D0t(s,t,q1sq,q2sq)]
c
c where p1^2 = 0 = (p1+p2+p3)^2 is assumed. The arguments of vbf_wp_wp_D0t are
c 
c   s = (p2+p3)^2, t = (p1+p2)^2, q1sq = p3^3 and q2sq = p2^2
c
c IR represents the divergent terms and is given by
c 
c  IR = 1/(s*t)*[1/eps**2 + 1/eps (log(q1sq/t)+log(q2sq/t)) ]
c
c Alternatively, arguments as in the call of vbf_wp_wp_tens4 can be used, i.e.
c
c  vbf_wp_wp_D0t(s,t,q1sq,q2sq) = vbf_wp_wp_D0t4(p1sq, p2sq, p3sq, p1p2, p1p3, p2p3) 
c
c This code agrees with the one written by Carlo. Checked 10/15/2002
c       Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2002 October 5
c	Last modified:    2002 October 14
c  
      complex*16 ipi, ieps, z1, z2, zlog, vbf_wp_wp_li2, vbf_wp_wp_D0t4
      parameter (ipi=(0d0,3.14159 26535 89793),ieps=(0d0,1d-16))
      external vbf_wp_wp_li2
      double precision pi,pi2o2,pi2o6
      parameter (pi=3.14159 26535 89793,pi2o2=pi**2/2d0)
      parameter (pi2o6=pi**2/6d0)
      logical ldebug
      parameter (ldebug=.false.)
      double precision rlog, rz1, rz2 
c
      sl = s
      tl = t
      q1sql = q1sq
      q2sql = q2sq
      goto 10
c
      entry vbf_wp_wp_D0t4(p1sq, p2sq, p3sq, p1p2, p1p3, p2p3)
      if (p1sq.ne.0d0 .or. 
     1    abs(2*(p1p2+p2p3+p1p3)/(p2sq+p3sq)+1).gt.1d-12) then
         print*,' Dont use vbf_wp_wp_D0t4 for for less than 2 massless momenta '
         print*,' (p1p2+p2p3+p1p3)/(p2sq+p3sq) = ',
     1           (p1p2+p2p3+p1p3)/(p2sq+p3sq)
         stop
      endif
      sl = p2sq+2*p2p3+p3sq
      tl = p2sq+2*p1p2
      q1sql = p3sq
      q2sql = p2sq
 10   continue
      if (q1sql.eq.0d0 .or. q2sql.eq.0d0) then
         print*,' WARNING: zero Q^2 in vbf_wp_wp_D0t: ',q1sql,q2sql
         print*,' Set vbf_wp_wp_D0t = 0 '
         vbf_wp_wp_D0t = 0
         return 
      endif
c
      rz1 = tl/q1sql
      rz2 = tl/q2sql
      rlog = -log( abs(rz1*rz2) )
      zlog = rlog
      if (rz1.gt.0) then
         z1 = 1-rz1
      elseif (tl.lt.0d0) then        !   t<0, q1sq>0
         z1 = 1-rz1-ieps
         zlog = zlog - ipi
      else                           !   t>0, q1sq<0
         z1 = 1-rz1+ieps
         zlog = zlog + ipi
      endif
      if (rz2.gt.0) then
         z2 = 1-rz2
      elseif (tl.lt.0d0) then        !   t<0, q2sq>0
         z2 = 1-rz2-ieps
         zlog = zlog - ipi
      else                           !   t>0, q2sq<0
         z2 = 1-rz2+ieps
         zlog = zlog + ipi
      endif
      vbf_wp_wp_D0t = (0.5d0*zlog**2 + 2*(vbf_wp_wp_li2(z1)+vbf_wp_wp_li2(z2)) - pi2o6)/(sl*tl)
c
      if (ldebug) then
         print('(a,2g14.4,a,g14.4)'), 
     1         ' t/qsq ratios: ',rz1, rz2,' t = ',tl
         print('(a,f8.4,a,f8.4)'),
     1         ' zlog = ',dreal(zlog),'+ i pi * ',dimag(zlog)/pi
         z1 = 0.5d0*zlog**2
         rz1 = (dreal(z1)-0.5d0*rlog**2)/pi**2
         if (rlog.ne.0d0) then
            rz2 = dimag(z1)/pi/rlog
         else
            rz2 = 0d0
         endif
         print('(2a,f8.4,a,f8.4,a)'),' extra terms from log: ',
     1   ' pi**2*(',rz1,')+ i pi rlog *(',rz2,')' 
         print*,' vbf_wp_wp_D0t = ',vbf_wp_wp_D0t
      endif
      return 
      end
c
c-------------  spence function  vbf_wp_wp_li2(z) ------------------------
c
      complex*16 function vbf_wp_wp_li2(zin)
      implicit none
      complex*16 zin, z, u, u2, unpo, ans, zext
      double precision r, r2, r2n, fac
c
c determine the value of the dilogarithm 
c
c    vbf_wp_wp_li2(z) = - int_0^1  log(1-zt)/t dt  with cut along the positive 
c                                        real axis, z>1
c
c	Dieter Zeppenfeld, <dieter@pheno.physics.wisc.edu>
c	Initial version:  2000 November 6
c	Last modified:    2000 November 12
c  
      integer i
      double precision vbf_wp_wp_C0,c1,c2,c4,c6,c8,c10,c12,c14,c16,c18,c20,c22
      double precision vbf_wp_wp_B0,b1,b2,b4,b6,b8,b10,b12,b14,b16,b18,b20,b22
      double precision d0,d1,d2,d4,d6,d8,d10,d12,d14,d16,d18,d20,d22
      parameter (vbf_wp_wp_B0=1d0,            d0 =1d0,      vbf_wp_wp_C0= vbf_wp_wp_B0/d0)
      parameter (b1=-1d0/2d0,       d1 =d0*2d0,   c1= b1/d1)
      parameter (b2= 1d0/6d0,       d2 =d1*3d0,   c2= b2/d2)
      parameter (b4=-1d0/30d0,      d4 =d2*20d0,  c4= b4/d4)
      parameter (b6=1d0/42d0,       d6 =d4*42d0,  c6= b6/d6)
      parameter (b8=-1d0/30d0,      d8 =d6*72d0,  c8= b8/d8)
      parameter (b10=5d0/66d0,      d10=d8*110d0, c10=b10/d10)
      parameter (b12=-691d0/2730d0, d12=d10*156d0,c12=b12/d12)
      parameter (b14=7d0/6d0,       d14=d12*210d0,c14=b14/d14)
      parameter (b16=-3617d0/510d0, d16=d14*272d0,c16=b16/d16)
      parameter (b18=43867d0/798d0, d18=d16*342d0,c18=b18/d18)
      parameter (b20=-174611d0/330d0,d20=d18*420d0,c20=b20/d20)
      parameter (b22=854513d0/138d0,d22=d20*506d0,c22=b22/d22)
      double precision eps, epst, pi, pi2o6
      parameter (eps=1d-16, epst=1d-3)
      parameter (pi=3.14159 26535 89793, pi2o6=pi**2/6)
c
c debug information
      logical ldebug
      parameter (ldebug=.false.)

      z = zin
c      print*,' vbf_wp_wp_li2 call with z = ',z
      u = z**2
      r2 = dreal(z)**2+dimag(z)**2 
      if (r2.lt.eps) then
         vbf_wp_wp_li2 = z + u/4d0
         return
      elseif (r2.lt.epst) then
         ans = z + u/4
         do i = 3,11
            u = u*z
            ans = ans + u/i**2
         enddo
         vbf_wp_wp_li2 = ans
         return
      endif
      if (dreal(z).ge.1d0 .and. dimag(z).eq.0 ) then
         z = z + (0d0,1d0)*eps
      endif
c
c use z-->1/z and z--> 1-z mappings of the spence function to restrict 
c agument to unit circle in the complex plane with Re(z) <= 0.5
c
      zext = (0d0,0d0)
      fac = 1
      if (r2.gt.1d0) then     ! map z ---> 1/z
         fac = -fac
         zext = -pi2o6 - 0.5d0*(log(-z))**2
         z = 1/z
      endif
      if (dreal(z).gt.0.5d0) then     ! map new z ---> 1-z
         zext = zext + fac*(pi2o6-log(z)*log(1-z))
         fac = -fac
         z = 1-z
      endif
c
c now use t = 1 - exp(-u) mapping to write Li(z) in terms of Bernoulli 
c numbers
c
      u = - log(1-z)
      r2 = abs(u)**2
      u2 = u*u
      ans = u*(vbf_wp_wp_C0 + u*(c1+c2*u))
      r2n = r2*r2       !r^4

      unpo = u2*u2*u
      ans = ans + c4*unpo

      unpo = unpo*u2
      ans = ans + c6*unpo

      r = r2n*r2n       !r^8
      unpo = unpo*u2
      ans = ans + c8*unpo

      r2n = r*r2        !r^10 
      if ((r2n*c10).gt.eps) then
         unpo = unpo*u2
         ans = ans + c10*unpo
      else
         vbf_wp_wp_li2 = fac * ans + zext
         if (ldebug) print*,' exit li2s at n=8 '
         return
      endif

      unpo = unpo*u2
      ans = ans + c12*unpo

      unpo = unpo*u2
      ans = ans + c14*unpo

      unpo = unpo*u2
      ans = ans + c16*unpo

      r2n = r2n*r
      if ((r2n*c18).gt.eps) then
         unpo = unpo*u2
         ans = ans + c18*unpo
      else
         vbf_wp_wp_li2 = fac * ans + zext
         if (ldebug) print*,' exit li2s at n=16 '
         return
      endif

      unpo = unpo*u2
      ans = ans + c20*unpo

      unpo = unpo*u2
      ans = ans + c22*unpo

      vbf_wp_wp_li2 = fac * ans + zext
      end
c
c----- vbf_wp_wp_dotp(p1,p2) = p1.p2 ----------------------------
c
      function vbf_wp_wp_dot0p(p1,p2)
      implicit none
      double precision vbf_wp_wp_dot0p,p1(0:3),p2(0:3), pt(3),fac
      vbf_wp_wp_dot0p = p1(0)*p2(0) - p1(1)*p2(1) - p1(2)*p2(2) - p1(3)*p2(3)
      if (abs(vbf_wp_wp_dot0p).lt.0.01*abs(p1(0)*p2(0))) then  !section for small p1.p2
         print*,' old vbf_wp_wp_dotp = ',vbf_wp_wp_dot0p,' ratio = ',vbf_wp_wp_dot0p/p1(0)/p2(0)
         fac = p2(0)/p1(0)
         pt(1)=p2(1)-fac*p1(1)
         pt(2)=p2(2)-fac*p1(2)
         pt(3)=p2(3)-fac*p1(3)
         vbf_wp_wp_dot0p = (pt(1)**2+pt(2)**2+pt(3)**2)/(2*fac)
         print*,' new vbf_wp_wp_dotp = ',vbf_wp_wp_dot0p
      endif
      end
c
c----- vbf_wp_wp_dotp(p1,p2) = p1.p2 ----------------------------
c
c      function vbf_wp_wp_dotp(p1,p2)
c      implicit none
c      double precision vbf_wp_wp_dotp,p1(0:3),p2(0:3)
c      vbf_wp_wp_dotp = p1(0)*p2(0) - p1(1)*p2(1) - p1(2)*p2(2) - p1(3)*p2(3)
c      end
c
c----- vbf_wp_wp_psumsq(p1,p2) = (p1+p2).(p1+p2) -----------------
c
      function vbf_wp_wp_psumsq(p1,p2)
      implicit none
      double precision vbf_wp_wp_psumsq,p1(0:3),p2(0:3)
      vbf_wp_wp_psumsq = (p1(0)+p2(0))**2 - (p1(1)+p2(1))**2 -
     1         (p1(2)+p2(2))**2 - (p1(3)+p2(3))**2
      end

      real*8 function vbf_wp_wp_srt(x2)
      real*8 x2
      if (x2.gt.0d0) then
         vbf_wp_wp_srt = sqrt(x2)
      elseif (x2.lt.0d0) then
         vbf_wp_wp_srt = -sqrt(-x2)
      else
         vbf_wp_wp_srt = 0d0
      endif
      end

