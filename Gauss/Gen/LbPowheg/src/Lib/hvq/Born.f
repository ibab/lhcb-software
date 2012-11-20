      subroutine hvq_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_math.h'
      include 'src/Lib/hvq/pwhg_st.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      integer mu,mup
      real *8 v,n,b,bb
      parameter(v=nc**2-1,n=nc)
      parameter(b=(n**2-4)/(4*n))
C     Abbreviation of (4.*pi*st_alpha)**2
      real*8 gs4,xxx,yyy,xm2,p12,p13,p23,s,t1,t2,ro
      real * 8 gtens(0:3,0:3)
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      real * 8 hvq_dotp
      external hvq_dotp
      save gtens
      p12=hvq_dotp(p(0,1),p(0,2))
      p13=hvq_dotp(p(0,1),p(0,3))
      p23=hvq_dotp(p(0,2),p(0,3))
      s=p12*2
      t1=p13/p12
      t2=1-t1
      xm2=kn_masses(3)**2
      ro=4*xm2/s

      gs4=(4*pi*st_alpha)**2

      if(bflav(1).eq.0.and.bflav(2).eq.0) then
c gluon fusion
c the following expression equals the amplitude squared, summed
c and averaged over spins and colours. It is equal to h^(0)_gg
c in NPB327(1988)49 (NDE2), eq.(2.1)
         born = 1/(2*v*n)*(v/(p13*p23)-2*n**2/p12**2)*
     1        (p13**2+p23**2+2*xm2*p12-(xm2*p12)**2/(p13*p23))
         xxx  = n**2/(4*v)*( (p12+2*xm2)/p23-(p12+2*xm2)/p13
     1        +xm2**2/p13**2-xm2**2/p23**2+2*(p23-p13)/p12 )
         yyy  = 1/(4*v*n**2)*
     1        (P13**2+P23**2+2*XM2*P12-(XM2*P12)**2/(P13*P23))
     2        *(1/(P13*P23)+2*N**2/P12**2)
         born = born*gs4
         xxx = xxx*gs4
         yyy = yyy*gs4
         bornjk(1,2) = (2*yyy+born*(2*n-2*cf))/2.0E+0
         bornjk(2,1) = bornjk(1,2)
         bornjk(1,3) = (-yyy-xxx+born*cf)/2.0E+0
         bornjk(3,1) = bornjk(1,3)
         bornjk(1,4) = (-yyy+xxx+born*cf)/2.0E+0
         bornjk(4,1) = bornjk(1,4)
         bornjk(2,3) = (-yyy+xxx+born*cf)/2.0E+0
         bornjk(3,2) = bornjk(2,3)
         bornjk(2,4) = (-yyy-xxx+born*cf)/2.0E+0
         bornjk(4,2) = bornjk(2,4)
         bornjk(3,4) = yyy
         bornjk(4,3) = bornjk(3,4)
         do mu=0,3
            do mup=0,3
      bmunu(mu,mup,1) = (n**2*(t2**2+t1**2)-1)*((p(mu,1)*p(mup,2)+p(mu,2
     1   )*p(mup,1))*(t2**2+t1**2+ro)-gtens(mu,mup)*s*(t2**2+t1**2)/2.0D
     2   +0+p(mu,1)*p(mup,1)*ro*t2/t1+p(mu,2)*p(mup,2)*ro*t1/t2+p(mu,3)*
     3   p(mup,3)*ro/(t1*t2)+(-p(mu,2)*p(mup,3)-p(mu,3)*p(mup,2))*ro/t2+
     4   (-p(mu,1)*p(mup,3)-p(mu,3)*p(mup,1))*ro/t1)/(n*s*t1*t2*v)/2.0D+
     5   0
c The two spin correlated amplitudes are equal!
      bmunu(mu,mup,1) =  bmunu(mu,mup,1) * gs4
      bmunu(mu,mup,2) = bmunu(mu,mup,1)
            enddo
         enddo
      else
         if(bflav(1).gt.0) then
            bb=b
         else
            bb=-b
         endif
c     q qbar annihilation
         born=cf**2/v*(2*(t1**2+t2**2)+ro) * gs4
         bornjk(1,2) = born*(cf-ca/2)
         bornjk(2,1) = bornjk(1,2)
         bornjk(1,3) = born*(ca/4+bb)
         bornjk(3,1) = bornjk(1,3)
         bornjk(1,4) = born*(ca/4-bb)
         bornjk(4,1) = bornjk(1,4)
         bornjk(2,3) = born*(ca/4-bb)
         bornjk(3,2) = bornjk(2,3)
         bornjk(2,4) = born*(ca/4+bb)
         bornjk(4,2) = bornjk(2,4)
         bornjk(3,4) = born*(cf-ca/2)
         bornjk(4,3) = bornjk(3,4)
      endif
      end


      subroutine hvq_born_suppression(fact)
      implicit none
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      logical ini
      data ini/.true./
      real * 8 fact,pt2,pt2supp,hvq_powheginput,pt
      save ini,pt2supp,pt
      if (ini) then
         pt = hvq_powheginput("#bornsuppfact")         
         ini = .false.
         pt2supp = pt**2
      endif
      if (pt.lt.0) then
         fact=1d0
      else         
         pt2=kn_pborn(1,3)**2+kn_pborn(2,3)**2+kn_masses(3)**2
         fact=(pt2/(pt2+pt2supp))**3
      endif
      end



      subroutine hvq_finalize_lh
      implicit none
c This should not be here, or perhaps hvq_finalize_lh should be
c called tail_lh.
c In case of a real event, replace
c all colours with those computed using the planar approximation
c to the real cross section
      call hvq_realcolour_lh
      call hvq_ttdecay
      END


      subroutine hvq_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      implicit none
      include 'src/Lib/hvq/LesHouches.h'
      include 'src/Lib/hvq/nlegborn.h'
      include 'src/Lib/hvq/pwhg_flst.h'
      include 'src/Lib/hvq/pwhg_kn.h'
      integer iclabel
      common/hvq_ciclabel/iclabel
      real * 8 hvq_random
      external hvq_random
      iclabel=500
      if(idup(1).eq.21) then
c gg
         if(hvq_random().gt.0.5d0) then
            call hvq_clinkqgga
     1           (icolup(1,3),icolup(1,1),icolup(1,2),icolup(1,4))
         else
            call hvq_clinkqgga
     1           (icolup(1,3),icolup(1,2),icolup(1,1),icolup(1,4))
         endif
      elseif(idup(1).gt.0) then
         call hvq_clinkqa(icolup(1,3),icolup(1,1))
         call hvq_clinkqa(icolup(1,2),icolup(1,4))
      else
         call hvq_clinkqa(icolup(1,3),icolup(1,2))
         call hvq_clinkqa(icolup(1,1),icolup(1,4))
      endif
c     1 and 2 are incoming! conjugate color
      call hvq_conjcolor(icolup(1,1))
      call hvq_conjcolor(icolup(1,2))
      end


      subroutine hvq_conjcolor(cl)
      integer cl(2),i
      i=cl(1)
      cl(1)=cl(2)
      cl(2)=i
      end


      subroutine hvq_realcolour_lh
      implicit none
      include 'src/Lib/hvq/LesHouches.h'
      character * 2 genprc
      integer jflreal
      integer igluon
      parameter (igluon=21)
      real * 8 xm2,t512,t152,t125,t521,t251,t215,t1r,t2r,t5r
      integer iclabel
      common/hvq_ciclabel/iclabel
      integer hvq_ipick,ichoice
      external hvq_ipick
      iclabel=500
      if(nup.eq.4) return
      if(idup(1).eq.igluon.and.idup(2).eq.igluon) then
         genprc='gg'
      elseif(idup(1).eq.igluon) then
         genprc='gq'
         jflreal=-idup(2)
      elseif(idup(2).eq.igluon) then
         genprc='qg'
         jflreal=idup(1)
      else
         jflreal=idup(1)
         genprc='qq'
      endif
      xm2=pup(5,3)**2
      if(genprc.eq.'gg') then
         call hvq_ggplanar(pup(1,1),1,pup(1,2),1,pup(1,5),1,
     1        pup(1,3),pup(1,4),xm2,
     2        t512,t152,t125,t521,t251,t215)
         ichoice=hvq_ipick(t512,t152,t125,t521,t251,t215)
         if(ichoice.eq.1) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,5),
     1           icolup(1,1),icolup(1,2),icolup(1,4))
         elseif(ichoice.eq.2) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,1),
     1           icolup(1,5),icolup(1,2),icolup(1,4))
         elseif(ichoice.eq.3) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,1),
     1           icolup(1,2),icolup(1,5),icolup(1,4))
         elseif(ichoice.eq.4) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,5),
     1           icolup(1,2),icolup(1,1),icolup(1,4))
         elseif(ichoice.eq.5) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,2),
     1           icolup(1,5),icolup(1,1),icolup(1,4))
         elseif(ichoice.eq.6) then
            call hvq_clinkqggga(icolup(1,3),icolup(1,2),
     1           icolup(1,1),icolup(1,5),icolup(1,4))
         endif
c     1 and 2 are incoming
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'qq'.and.jflreal.gt.0) then
c     quark antiquark incoming: antiquark-quark outgoing
         call hvq_qqbplanar(pup(1,1),1,pup(1,2),1,pup(1,5),1,
     1        pup(1,3),pup(1,4),xm2,t1r,t2r)
         ichoice=hvq_ipick(t1r,t2r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,5),icolup(1,1))
            call hvq_clinkqa(icolup(1,2),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,1))
            call hvq_clinkqga(icolup(1,2),icolup(1,5),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'qq'.and.jflreal.lt.0) then
c     antiquark-quark
         call hvq_qqbplanar(pup(1,2),1,pup(1,1),1,pup(1,5),1,
     1        pup(1,3),pup(1,4),xm2,t2r,t1r)
         ichoice=hvq_ipick(t2r,t1r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,5),icolup(1,2))
            call hvq_clinkqa(icolup(1,1),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,2))
            call hvq_clinkqga(icolup(1,1),icolup(1,5),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'qg'.and.jflreal.gt.0) then
c     quark-gluone
         call hvq_qqbplanar(pup(1,1),1,pup(1,5),-1,pup(1,2),-1,
     1        pup(1,3),pup(1,4),xm2,t1r,t5r)
         ichoice=hvq_ipick(t1r,t5r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,2),icolup(1,1))
            call hvq_clinkqa(icolup(1,5),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,1))
            call hvq_clinkqga(icolup(1,5),icolup(1,2),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'qg'.and.jflreal.lt.0) then
c     antiquark-gluon
         call hvq_qqbplanar(pup(1,5),-1,pup(1,1),1,pup(1,2),-1,
     1        pup(1,3),pup(1,4),xm2,t5r,t1r)
         ichoice=hvq_ipick(t5r,t1r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,2),icolup(1,5))
            call hvq_clinkqa(icolup(1,1),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,5))
            call hvq_clinkqga(icolup(1,1),icolup(1,2),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'gq'.and.jflreal.lt.0) then
c     gluon-quark
         call hvq_qqbplanar(pup(1,2),1,pup(1,5),-1,pup(1,1),-1,
     1        pup(1,3),pup(1,4),xm2,t2r,t5r)
         ichoice=hvq_ipick(t2r,t5r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,1),icolup(1,2))
            call hvq_clinkqa(icolup(1,5),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,2))
            call hvq_clinkqga(icolup(1,5),icolup(1,1),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      elseif(genprc.eq.'gq'.and.jflreal.gt.0) then
c     gluon-antiquark
         call hvq_qqbplanar(pup(1,5),-1, pup(1,2),1,pup(1,1),-1,
     1        pup(1,3),pup(1,4),xm2,t5r,t2r)
         ichoice=hvq_ipick(t5r,t2r,0d0,0d0,0d0,0d0)
         if(ichoice.eq.1) then
            call hvq_clinkqga(icolup(1,3),icolup(1,1),icolup(1,5))
            call hvq_clinkqa(icolup(1,2),icolup(1,4))
         else
            call hvq_clinkqa(icolup(1,3),icolup(1,5))
            call hvq_clinkqga(icolup(1,2),icolup(1,1),icolup(1,4))
         endif
         call hvq_conjcolor(icolup(1,1))
         call hvq_conjcolor(icolup(1,2))
      endif
      end


c Subroutine to link colours for
c quark - gluon -gluon  -gluon - aquark
c in planar order
      subroutine hvq_clinkqggga(ic1,ic2,ic3,ic4,ic5)
      integer iclabel
      common/hvq_ciclabel/iclabel
      integer ic1(2),ic2(2),ic3(2),ic4(2),ic5(2)
c ic1 is a quark: has colour, zero anticolor
      ic1(1)=iclabel+2
      ic1(2)=0
c ic2 is a gluon: link to quark
      ic2(1)=iclabel+3
      ic2(2)=iclabel+2
c ic3 is a gluon
      ic3(1)=iclabel+4
      ic3(2)=iclabel+3
c ic4 is an gluon
      ic4(1)=iclabel+5
      ic4(2)=iclabel+4
c ic5 is an antiquark
      ic5(1)=0
      ic5(2)=iclabel+5
      iclabel=iclabel+10
      end

c Subroutine to link colours for
c quark - gluon -gluon - aquark
c in planar order
      subroutine hvq_clinkqgga(ic1,ic2,ic3,ic4)
      integer iclabel
      common/hvq_ciclabel/iclabel
      integer ic1(2),ic2(2),ic3(2),ic4(2)
c ic1 is a quark: has colour, zero anticolor
      ic1(1)=iclabel+2
      ic1(2)=0
c ic2 is a gluon: link to quark
      ic2(1)=iclabel+3
      ic2(2)=iclabel+2
c ic3 is a gluon
      ic3(1)=iclabel+4
      ic3(2)=iclabel+3
c ic4 is an anti-quark
      ic4(1)=0
      ic4(2)=iclabel+4
      iclabel=iclabel+10
      end

c Subroutine to link colours for
c quark - gluon - aquark
c in planar order
      subroutine hvq_clinkqga(ic1,ic2,ic3)
      integer iclabel
      common/hvq_ciclabel/iclabel
      integer ic1(2),ic2(2),ic3(2)
c ic1 is a quark: has colour, zero anticolor
      ic1(1)=iclabel+2
      ic1(2)=0
c ic2 is a gluon: link to quark
      ic2(1)=iclabel+3
      ic2(2)=iclabel+2
c ic3 is an antiquark
      ic3(1)=0
      ic3(2)=iclabel+3
      iclabel=iclabel+10
      end

c Subroutine to link colours for
c quark - aquark
c in planar order
      subroutine hvq_clinkqa(ic1,ic2)
      integer iclabel
      common/hvq_ciclabel/iclabel
      integer ic1(2),ic2(2)
c ic1 is a quark: has colour, zero anticolor
      ic1(1)=iclabel+2
      ic1(2)=0
c ic2 is an antiquark
      ic2(1)=0
      ic2(2)=iclabel+2
      iclabel=iclabel+10
      end

      subroutine hvq_ggplanar(p1,ip1,p2,ip2,k,ik,k1,k2,m2,
     # tr12,t1r2,t12r,tr21,t2r1,t21r)
      implicit none
      real * 8 p1(4),p2(4),k(4),k1(4),k2(4)
      REAL * 8 s,tk,uk,q1,q2,q1c,q2c,w1,w2,m,s2,
     # m2,tr12,t1r2,t12r,tr21,t2r1,t21r
      integer ip1,ip2,ik
      real * 8 hvq_dotprod
      external hvq_dotprod
      m=sqrt(m2)
      s=2*hvq_dotprod(p1,p2)*ip1*ip2
      tk=-2*hvq_dotprod(p1,k)*ip1*ik
      uk=-2*hvq_dotprod(p2,k)*ip2*ik
      q1=-2*hvq_dotprod(p1,k1)*ip1
      q2=-2*hvq_dotprod(p2,k2)*ip2
c Planar invariant squared amplitudes, summed over spins and color   
c for the planar g g -> q qbar g hvq_process; misses a factor Ncolor^4   
c k1: quark momentum
c k2: anti-quark momentum   
c p1: incoming gluon momentum   
c p2: incoming gluon momentum   
c k: radiated gluon momentum   
c   
c Planar configurations are obtained with the gluon all on the same
c side of the fermion line; a given colour flow is specified by
c the ordering of the attachment of the gluons on the fermion
c line. For example: k1,k,p1,p2,k2 is the color structure
c                             
c    k1 -<----||--<--  ---<---  ---<--- k2
c             ||     ||       ||       
c             ^V     ^V       ^V       
c             ||     ||       ||       
c             ||     ||       ||       
c             k      p1       p2
c
c
c return values:   
c tr12: planar configuration k1,k,p1,p2,k2   
c t1r2:                      k1,p1,k,p2,k2   
c t12r:                      k1,p1,p2,k,k2   
c tr21:                      k1,k,p2,p1,k2   
c t2r1:                      k1,p2,k,p1,k2   
c t21r:                      k1,p2,p1,k,k2   
c txyz: xyz stand for incoming gluon 1, 2, and radiated gluon (r)   
c   
c    d(p1,p1)=0   
c    d(p2,p2)=0   
c    d(k,k)=0   
c    d(k1,k1)=m2   
c    d(k2,k2)=m2   
c    d(p1,p2)=s/2   
c    d(p1,k)=-tk/2   
c    d(p2,k)=-uk/2   
c    d(p1,k1)=-q1/2   
c    d(p2,k2)=-q2/2   
c    d(k1,k2)=(s2-2*m2)/2   
c    d(p1,k2)=-q1c/2   
c    d(p2,k1)=-q2c/2   
c    d(k,k1)=w1/2   
c    d(k,k2)=w2/2   
c   
c  Relations among invariants   
c   
      s2=s+tk+uk   
      q1c=-s-tk-q1   
      q2c=-s-uk-q2   
      w1=-q1+q2-tk   
      w2=q1-q2-uk   
      TR12 = -(8*Q2*S*S2*TK*W1**5+(8*Q2*S*S2*TK**2+(8*Q2*S*S2**2+(-16*Q2
     1   *S**2-16*Q2**2*S)*S2)*TK-8*M**2*S**2*S2**2)*W1**4+(6*Q2*S*S2*TK
     2   **3+((8*M**2*Q2-24*Q2**2)*S-12*Q2*S**2)*S2*TK**2+(6*Q2*S*S2**3+
     3   ((-12*Q2-16*M**2)*S**2+8*M**2*Q2*S)*S2**2+(12*Q2*S**3+(24*Q2**2
     4   -16*M**2*Q2)*S**2+24*Q2**3*S)*S2)*TK+16*M**2*Q2*S**2*S2**2-16*M
     5   **2*Q2*S**3*S2)*W1**3+((2*Q2*S*S2-8*M**2*Q2**2)*TK**4+((-6*Q2*S
     6   **2+(8*M**2*Q2-12*Q2**2)*S+16*M**2*Q2**2)*S2-16*M**2*Q2**2*S)*T
     7   K**3+((-12*M**2*S**2-8*M**2*Q2**2)*S2**2+(6*Q2*S**3+(24*Q2**2-1
     8   6*M**2*Q2)*S**2+(24*Q2**3-8*M**2*Q2**2)*S)*S2-24*M**2*Q2**2*S**
     9   2)*TK**2+(2*Q2*S*S2**4+(8*M**2*Q2*S-6*Q2*S**2)*S2**3+(6*Q2*S**3
     :   +(-8*M**2*Q2-16*M**4)*S**2)*S2**2+(-4*Q2*S**4+(-12*Q2**2-16*M**
     ;   2*Q2)*S**3+(-24*Q2**3-8*M**2*Q2**2)*S**2-16*Q2**4*S)*S2-16*M**2
     <   *Q2**2*S**3)*TK-8*M**2*Q2**2*S**2*S2**2+16*M**2*Q2**2*S**3*S2-8
     =   *M**2*Q2**2*S**4)*W1**2+((-2*Q2*S**2-4*Q2**2*S)*S2*TK**4+((6*Q2
     >   **2*S-4*M**2*S**2)*S2**2+((6*Q2**2-4*M**2*Q2)*S**2+(12*Q2**3-16
     ?   *M**2*Q2**2+16*M**4*Q2)*S-16*M**2*Q2**3)*S2)*TK**3+(-6*Q2**2*S*
     @   S2**3+((-12*M**2*Q2-16*M**4)*S**2+(-12*Q2**3-8*M**2*Q2**2-16*M*
     1   *4*Q2)*S+16*M**2*Q2**3)*S2**2+(-2*Q2*S**4+(-6*Q2**2-4*M**2*Q2)*
     2   S**3+(-12*Q2**3-16*M**2*Q2**2+16*M**4*Q2)*S**2+(-16*Q2**4-16*M*
     3   *2*Q2**3)*S)*S2)*TK**2+(2*Q2**2*S*S2**4+(6*Q2**3+8*M**2*Q2**2+1
     4   6*M**4*Q2)*S*S2**3+((8*Q2**4+8*M**2*Q2**3)*S-16*M**4*Q2*S**2)*S
     5   2**2+(2*Q2**2*S**4+(6*Q2**3+8*M**2*Q2**2+16*M**4*Q2)*S**3+(8*Q2
     6   **4+8*M**2*Q2**3)*S**2+8*Q2**5*S)*S2)*TK)*W1+(-4*M**2*Q2*S**3+(
     7   -12*M**2*Q2**2-16*M**4*Q2-16*M**6)*S**2+(-16*M**2*Q2**3-16*M**4
     8   *Q2**2)*S-8*M**2*Q2**4)*S2**2*TK**2)/(Q2**2*S**2*S2**2*TK**2*W1
     9   **2)
      T1R2 = ((2*Q1*Q2*S2*TK**2+(4*Q1**2*Q2-2*Q1*Q2**2)*S2*TK+8*M**2*Q1*
     1   *2*Q2**2)*UK**4+((4*M**2*Q2*S2**2+(6*Q1*Q2**2+(4*M**2*Q1-6*Q1**
     2   2)*Q2)*S2)*TK**2+(-6*Q1**2*Q2*S2**2+(-6*Q1*Q2**3+(12*Q1**2-8*M*
     3   *2*Q1)*Q2**2+(-12*Q1**3+16*M**2*Q1**2-16*M**4*Q1)*Q2)*S2+16*M**
     4   2*Q1**2*Q2**2)*TK+(16*M**2*Q1**3*Q2-16*M**2*Q1**2*Q2**2)*S2)*UK
     5   **3+(2*Q1*Q2*S2*TK**4+(4*M**2*Q1*S2**2+((6*Q1**2+4*M**2*Q1)*Q2-
     6   6*Q1*Q2**2)*S2)*TK**3+((12*M**2*Q2**2+(12*M**2*Q1+16*M**4)*Q2+1
     7   2*M**2*Q1**2+16*M**4*Q1+16*M**6)*S2**2+(12*Q1*Q2**3+(16*M**2*Q1
     8   -24*Q1**2)*Q2**2+(12*Q1**3+16*M**2*Q1**2-16*M**4*Q1)*Q2)*S2+24*
     9   M**2*Q1**2*Q2**2)*TK**2+(6*Q1**2*Q2*S2**3+((12*Q1**3+8*M**2*Q1*
     :   *2+16*M**4*Q1)*Q2+16*M**2*Q1**3+16*M**4*Q1**2)*S2**2+(-8*Q1*Q2*
     ;   *4+(24*Q1**2-8*M**2*Q1)*Q2**3+(8*M**2*Q1**2-24*Q1**3)*Q2**2+(16
     <   *Q1**4+16*M**2*Q1**3)*Q2)*S2)*TK+(8*M**2*Q1**2*Q2**2-16*M**2*Q1
     =   **3*Q2+8*M**2*Q1**4)*S2**2)*UK**2+((4*Q1*Q2**2-2*Q1**2*Q2)*S2*T
     >   K**4+(-6*Q1*Q2**2*S2**2+(-12*Q1*Q2**3+(12*Q1**2+16*M**2*Q1)*Q2*
     ?   *2+(-6*Q1**3-8*M**2*Q1**2-16*M**4*Q1)*Q2)*S2+16*M**2*Q1**2*Q2**
     @   2)*TK**3+(6*Q1*Q2**2*S2**3+((12*Q1+16*M**2)*Q2**3+(8*M**2*Q1+16
     1   *M**4)*Q2**2+16*M**4*Q1*Q2)*S2**2+(16*Q1*Q2**4+(16*M**2*Q1-24*Q
     2   1**2)*Q2**3+(24*Q1**3+8*M**2*Q1**2)*Q2**2+(-8*Q1**4-8*M**2*Q1**
     3   3)*Q2)*S2)*TK**2+((-2*Q1*Q2**2-2*Q1**2*Q2)*S2**4+(-6*Q1*Q2**3-8
     4   *M**2*Q1*Q2**2+(-6*Q1**3-8*M**2*Q1**2-16*M**4*Q1)*Q2)*S2**3+(-8
     5   *Q1*Q2**4-8*M**2*Q1*Q2**3+(-8*Q1**4-8*M**2*Q1**3)*Q2)*S2**2+(-8
     6   *Q1*Q2**5+16*Q1**2*Q2**4-24*Q1**3*Q2**3+16*Q1**4*Q2**2-8*Q1**5*
     7   Q2)*S2)*TK)*UK+8*M**2*Q1**2*Q2**2*TK**4+(16*M**2*Q1*Q2**3-16*M*
     8   *2*Q1**2*Q2**2)*S2*TK**3+(8*M**2*Q2**4-16*M**2*Q1*Q2**3+8*M**2*
     9   Q1**2*Q2**2)*S2**2*TK**2)/(Q1**2*Q2**2*S2**2*TK**2*UK**2)
      T12R = -(8*Q1*S*S2*UK*W2**5+(8*Q1*S*S2*UK**2+(8*Q1*S*S2**2+(-16*Q1
     1   *S**2-16*Q1**2*S)*S2)*UK-8*M**2*S**2*S2**2)*W2**4+(6*Q1*S*S2*UK
     2   **3+((8*M**2*Q1-24*Q1**2)*S-12*Q1*S**2)*S2*UK**2+(6*Q1*S*S2**3+
     3   ((-12*Q1-16*M**2)*S**2+8*M**2*Q1*S)*S2**2+(12*Q1*S**3+(24*Q1**2
     4   -16*M**2*Q1)*S**2+24*Q1**3*S)*S2)*UK+16*M**2*Q1*S**2*S2**2-16*M
     5   **2*Q1*S**3*S2)*W2**3+((2*Q1*S*S2-8*M**2*Q1**2)*UK**4+((-6*Q1*S
     6   **2+(8*M**2*Q1-12*Q1**2)*S+16*M**2*Q1**2)*S2-16*M**2*Q1**2*S)*U
     7   K**3+((-12*M**2*S**2-8*M**2*Q1**2)*S2**2+(6*Q1*S**3+(24*Q1**2-1
     8   6*M**2*Q1)*S**2+(24*Q1**3-8*M**2*Q1**2)*S)*S2-24*M**2*Q1**2*S**
     9   2)*UK**2+(2*Q1*S*S2**4+(8*M**2*Q1*S-6*Q1*S**2)*S2**3+(6*Q1*S**3
     :   +(-8*M**2*Q1-16*M**4)*S**2)*S2**2+(-4*Q1*S**4+(-12*Q1**2-16*M**
     ;   2*Q1)*S**3+(-24*Q1**3-8*M**2*Q1**2)*S**2-16*Q1**4*S)*S2-16*M**2
     <   *Q1**2*S**3)*UK-8*M**2*Q1**2*S**2*S2**2+16*M**2*Q1**2*S**3*S2-8
     =   *M**2*Q1**2*S**4)*W2**2+((-2*Q1*S**2-4*Q1**2*S)*S2*UK**4+((6*Q1
     >   **2*S-4*M**2*S**2)*S2**2+((6*Q1**2-4*M**2*Q1)*S**2+(12*Q1**3-16
     ?   *M**2*Q1**2+16*M**4*Q1)*S-16*M**2*Q1**3)*S2)*UK**3+(-6*Q1**2*S*
     @   S2**3+((-12*M**2*Q1-16*M**4)*S**2+(-12*Q1**3-8*M**2*Q1**2-16*M*
     1   *4*Q1)*S+16*M**2*Q1**3)*S2**2+(-2*Q1*S**4+(-6*Q1**2-4*M**2*Q1)*
     2   S**3+(-12*Q1**3-16*M**2*Q1**2+16*M**4*Q1)*S**2+(-16*Q1**4-16*M*
     3   *2*Q1**3)*S)*S2)*UK**2+(2*Q1**2*S*S2**4+(6*Q1**3+8*M**2*Q1**2+1
     4   6*M**4*Q1)*S*S2**3+((8*Q1**4+8*M**2*Q1**3)*S-16*M**4*Q1*S**2)*S
     5   2**2+(2*Q1**2*S**4+(6*Q1**3+8*M**2*Q1**2+16*M**4*Q1)*S**3+(8*Q1
     6   **4+8*M**2*Q1**3)*S**2+8*Q1**5*S)*S2)*UK)*W2+(-4*M**2*Q1*S**3+(
     7   -12*M**2*Q1**2-16*M**4*Q1-16*M**6)*S**2+(-16*M**2*Q1**3-16*M**4
     8   *Q1**2)*S-8*M**2*Q1**4)*S2**2*UK**2)/(Q1**2*S**2*S2**2*UK**2*W2
     9   **2)
      TR21 = -(8*Q1C*S*S2*UK*W1**5+(8*Q1C*S*S2*UK**2+(8*Q1C*S*S2**2+(-16
     1   *Q1C*S**2-16*Q1C**2*S)*S2)*UK-8*M**2*S**2*S2**2)*W1**4+(6*Q1C*S
     2   *S2*UK**3+((8*M**2*Q1C-24*Q1C**2)*S-12*Q1C*S**2)*S2*UK**2+(6*Q1
     3   C*S*S2**3+((-12*Q1C-16*M**2)*S**2+8*M**2*Q1C*S)*S2**2+(12*Q1C*S
     4   **3+(24*Q1C**2-16*M**2*Q1C)*S**2+24*Q1C**3*S)*S2)*UK+16*M**2*Q1
     5   C*S**2*S2**2-16*M**2*Q1C*S**3*S2)*W1**3+((2*Q1C*S*S2-8*M**2*Q1C
     6   **2)*UK**4+((-6*Q1C*S**2+(8*M**2*Q1C-12*Q1C**2)*S+16*M**2*Q1C**
     7   2)*S2-16*M**2*Q1C**2*S)*UK**3+((-12*M**2*S**2-8*M**2*Q1C**2)*S2
     8   **2+(6*Q1C*S**3+(24*Q1C**2-16*M**2*Q1C)*S**2+(24*Q1C**3-8*M**2*
     9   Q1C**2)*S)*S2-24*M**2*Q1C**2*S**2)*UK**2+(2*Q1C*S*S2**4+(8*M**2
     :   *Q1C*S-6*Q1C*S**2)*S2**3+(6*Q1C*S**3+(-8*M**2*Q1C-16*M**4)*S**2
     ;   )*S2**2+(-4*Q1C*S**4+(-12*Q1C**2-16*M**2*Q1C)*S**3+(-24*Q1C**3-
     <   8*M**2*Q1C**2)*S**2-16*Q1C**4*S)*S2-16*M**2*Q1C**2*S**3)*UK-8*M
     =   **2*Q1C**2*S**2*S2**2+16*M**2*Q1C**2*S**3*S2-8*M**2*Q1C**2*S**4
     >   )*W1**2+((-2*Q1C*S**2-4*Q1C**2*S)*S2*UK**4+((6*Q1C**2*S-4*M**2*
     ?   S**2)*S2**2+((6*Q1C**2-4*M**2*Q1C)*S**2+(12*Q1C**3-16*M**2*Q1C*
     @   *2+16*M**4*Q1C)*S-16*M**2*Q1C**3)*S2)*UK**3+(-6*Q1C**2*S*S2**3+
     1   ((-12*M**2*Q1C-16*M**4)*S**2+(-12*Q1C**3-8*M**2*Q1C**2-16*M**4*
     2   Q1C)*S+16*M**2*Q1C**3)*S2**2+(-2*Q1C*S**4+(-6*Q1C**2-4*M**2*Q1C
     3   )*S**3+(-12*Q1C**3-16*M**2*Q1C**2+16*M**4*Q1C)*S**2+(-16*Q1C**4
     4   -16*M**2*Q1C**3)*S)*S2)*UK**2+(2*Q1C**2*S*S2**4+(6*Q1C**3+8*M**
     5   2*Q1C**2+16*M**4*Q1C)*S*S2**3+((8*Q1C**4+8*M**2*Q1C**3)*S-16*M*
     6   *4*Q1C*S**2)*S2**2+(2*Q1C**2*S**4+(6*Q1C**3+8*M**2*Q1C**2+16*M*
     7   *4*Q1C)*S**3+(8*Q1C**4+8*M**2*Q1C**3)*S**2+8*Q1C**5*S)*S2)*UK)*
     8   W1+(-4*M**2*Q1C*S**3+(-12*M**2*Q1C**2-16*M**4*Q1C-16*M**6)*S**2
     9   +(-16*M**2*Q1C**3-16*M**4*Q1C**2)*S-8*M**2*Q1C**4)*S2**2*UK**2)
     :   /(Q1C**2*S**2*S2**2*UK**2*W1**2)
      T2R1 = ((2*Q1C*Q2C*S2*TK**2+(4*Q1C**2*Q2C-2*Q1C*Q2C**2)*S2*TK+8*M*
     1   *2*Q1C**2*Q2C**2)*UK**4+((4*M**2*Q2C*S2**2+(6*Q1C*Q2C**2+(4*M**
     2   2*Q1C-6*Q1C**2)*Q2C)*S2)*TK**2+(-6*Q1C**2*Q2C*S2**2+(-6*Q1C*Q2C
     3   **3+(12*Q1C**2-8*M**2*Q1C)*Q2C**2+(-12*Q1C**3+16*M**2*Q1C**2-16
     4   *M**4*Q1C)*Q2C)*S2+16*M**2*Q1C**2*Q2C**2)*TK+(16*M**2*Q1C**3*Q2
     5   C-16*M**2*Q1C**2*Q2C**2)*S2)*UK**3+(2*Q1C*Q2C*S2*TK**4+(4*M**2*
     6   Q1C*S2**2+((6*Q1C**2+4*M**2*Q1C)*Q2C-6*Q1C*Q2C**2)*S2)*TK**3+((
     7   12*M**2*Q2C**2+(12*M**2*Q1C+16*M**4)*Q2C+12*M**2*Q1C**2+16*M**4
     8   *Q1C+16*M**6)*S2**2+(12*Q1C*Q2C**3+(16*M**2*Q1C-24*Q1C**2)*Q2C*
     9   *2+(12*Q1C**3+16*M**2*Q1C**2-16*M**4*Q1C)*Q2C)*S2+24*M**2*Q1C**
     :   2*Q2C**2)*TK**2+(6*Q1C**2*Q2C*S2**3+((12*Q1C**3+8*M**2*Q1C**2+1
     ;   6*M**4*Q1C)*Q2C+16*M**2*Q1C**3+16*M**4*Q1C**2)*S2**2+(-8*Q1C*Q2
     <   C**4+(24*Q1C**2-8*M**2*Q1C)*Q2C**3+(8*M**2*Q1C**2-24*Q1C**3)*Q2
     =   C**2+(16*Q1C**4+16*M**2*Q1C**3)*Q2C)*S2)*TK+(8*M**2*Q1C**2*Q2C*
     >   *2-16*M**2*Q1C**3*Q2C+8*M**2*Q1C**4)*S2**2)*UK**2+((4*Q1C*Q2C**
     ?   2-2*Q1C**2*Q2C)*S2*TK**4+(-6*Q1C*Q2C**2*S2**2+(-12*Q1C*Q2C**3+(
     @   12*Q1C**2+16*M**2*Q1C)*Q2C**2+(-6*Q1C**3-8*M**2*Q1C**2-16*M**4*
     1   Q1C)*Q2C)*S2+16*M**2*Q1C**2*Q2C**2)*TK**3+(6*Q1C*Q2C**2*S2**3+(
     2   (12*Q1C+16*M**2)*Q2C**3+(8*M**2*Q1C+16*M**4)*Q2C**2+16*M**4*Q1C
     3   *Q2C)*S2**2+(16*Q1C*Q2C**4+(16*M**2*Q1C-24*Q1C**2)*Q2C**3+(24*Q
     4   1C**3+8*M**2*Q1C**2)*Q2C**2+(-8*Q1C**4-8*M**2*Q1C**3)*Q2C)*S2)*
     5   TK**2+((-2*Q1C*Q2C**2-2*Q1C**2*Q2C)*S2**4+(-6*Q1C*Q2C**3-8*M**2
     6   *Q1C*Q2C**2+(-6*Q1C**3-8*M**2*Q1C**2-16*M**4*Q1C)*Q2C)*S2**3+(-
     7   8*Q1C*Q2C**4-8*M**2*Q1C*Q2C**3+(-8*Q1C**4-8*M**2*Q1C**3)*Q2C)*S
     8   2**2+(-8*Q1C*Q2C**5+16*Q1C**2*Q2C**4-24*Q1C**3*Q2C**3+16*Q1C**4
     9   *Q2C**2-8*Q1C**5*Q2C)*S2)*TK)*UK+8*M**2*Q1C**2*Q2C**2*TK**4+(16
     :   *M**2*Q1C*Q2C**3-16*M**2*Q1C**2*Q2C**2)*S2*TK**3+(8*M**2*Q2C**4
     ;   -16*M**2*Q1C*Q2C**3+8*M**2*Q1C**2*Q2C**2)*S2**2*TK**2)/(Q1C**2*
     <   Q2C**2*S2**2*TK**2*UK**2)
      T21R = -(8*Q2C*S*S2*TK*W2**5+(8*Q2C*S*S2*TK**2+(8*Q2C*S*S2**2+(-16
     1   *Q2C*S**2-16*Q2C**2*S)*S2)*TK-8*M**2*S**2*S2**2)*W2**4+(6*Q2C*S
     2   *S2*TK**3+((8*M**2*Q2C-24*Q2C**2)*S-12*Q2C*S**2)*S2*TK**2+(6*Q2
     3   C*S*S2**3+((-12*Q2C-16*M**2)*S**2+8*M**2*Q2C*S)*S2**2+(12*Q2C*S
     4   **3+(24*Q2C**2-16*M**2*Q2C)*S**2+24*Q2C**3*S)*S2)*TK+16*M**2*Q2
     5   C*S**2*S2**2-16*M**2*Q2C*S**3*S2)*W2**3+((2*Q2C*S*S2-8*M**2*Q2C
     6   **2)*TK**4+((-6*Q2C*S**2+(8*M**2*Q2C-12*Q2C**2)*S+16*M**2*Q2C**
     7   2)*S2-16*M**2*Q2C**2*S)*TK**3+((-12*M**2*S**2-8*M**2*Q2C**2)*S2
     8   **2+(6*Q2C*S**3+(24*Q2C**2-16*M**2*Q2C)*S**2+(24*Q2C**3-8*M**2*
     9   Q2C**2)*S)*S2-24*M**2*Q2C**2*S**2)*TK**2+(2*Q2C*S*S2**4+(8*M**2
     :   *Q2C*S-6*Q2C*S**2)*S2**3+(6*Q2C*S**3+(-8*M**2*Q2C-16*M**4)*S**2
     ;   )*S2**2+(-4*Q2C*S**4+(-12*Q2C**2-16*M**2*Q2C)*S**3+(-24*Q2C**3-
     <   8*M**2*Q2C**2)*S**2-16*Q2C**4*S)*S2-16*M**2*Q2C**2*S**3)*TK-8*M
     =   **2*Q2C**2*S**2*S2**2+16*M**2*Q2C**2*S**3*S2-8*M**2*Q2C**2*S**4
     >   )*W2**2+((-2*Q2C*S**2-4*Q2C**2*S)*S2*TK**4+((6*Q2C**2*S-4*M**2*
     ?   S**2)*S2**2+((6*Q2C**2-4*M**2*Q2C)*S**2+(12*Q2C**3-16*M**2*Q2C*
     @   *2+16*M**4*Q2C)*S-16*M**2*Q2C**3)*S2)*TK**3+(-6*Q2C**2*S*S2**3+
     1   ((-12*M**2*Q2C-16*M**4)*S**2+(-12*Q2C**3-8*M**2*Q2C**2-16*M**4*
     2   Q2C)*S+16*M**2*Q2C**3)*S2**2+(-2*Q2C*S**4+(-6*Q2C**2-4*M**2*Q2C
     3   )*S**3+(-12*Q2C**3-16*M**2*Q2C**2+16*M**4*Q2C)*S**2+(-16*Q2C**4
     4   -16*M**2*Q2C**3)*S)*S2)*TK**2+(2*Q2C**2*S*S2**4+(6*Q2C**3+8*M**
     5   2*Q2C**2+16*M**4*Q2C)*S*S2**3+((8*Q2C**4+8*M**2*Q2C**3)*S-16*M*
     6   *4*Q2C*S**2)*S2**2+(2*Q2C**2*S**4+(6*Q2C**3+8*M**2*Q2C**2+16*M*
     7   *4*Q2C)*S**3+(8*Q2C**4+8*M**2*Q2C**3)*S**2+8*Q2C**5*S)*S2)*TK)*
     8   W2+(-4*M**2*Q2C*S**3+(-12*M**2*Q2C**2-16*M**4*Q2C-16*M**6)*S**2
     9   +(-16*M**2*Q2C**3-16*M**4*Q2C**2)*S-8*M**2*Q2C**4)*S2**2*TK**2)
     :   /(Q2C**2*S**2*S2**2*TK**2*W2**2)
      END


      subroutine hvq_qqbplanar(p1,ip1,p2,ip2,k,ik,k1,k2,m2,
     # t1r,t2r)
      implicit none
      real * 8 p1(4),p2(4),k(4),k1(4),k2(4)
      REAL * 8 s,tk,uk,q1,q2,q1c,q2c,w1,w2,m,s2,
     # m2,t1r,t2r
      real * 8 hvq_dotprod
      external hvq_dotprod
      integer ip1,ip2,ik
c Planar invariant squared amplitudes, summed over spins and color 
c for the planar lq lqb -> q qbar g hvq_process; misses a factor Ncolor^4 
c k1: quark momentum 
c k2: anti-quark momentum 
c p1: incoming light quark momentum 
c p2: incoming light antiquark momentum 
c k: radiated gluon momentum 
c 
c return values: 
c t1r: planar configuration p1,k,k1 
c t2r: planar configuration p2,k,k2 
c 
c    d(p1,p1)=0 
c    d(p2,p2)=0 
c    d(k,k)=0 
c    d(k1,k1)=m2 
c    d(k2,k2)=m2 
c    d(p1,p2)=s/2 
c    d(p1,k)=-tk/2 
c    d(p2,k)=-uk/2 
c    d(p1,k1)=-q1/2 
c    d(p2,k2)=-q2/2 
c    d(k1,k2)=(s2-2*m2)/2 
c    d(p1,k2)=-q1c/2 
c    d(p2,k1)=-q2c/2 
c    d(k,k1)=w1/2 
c    d(k,k2)=w2/2 
c 
c  Relations among invariants 
c 
      m=sqrt(m2)
      s=2*hvq_dotprod(p1,p2)*ip1*ip2
      tk=-2*hvq_dotprod(p1,k)*ip1*ik
      uk=-2*hvq_dotprod(p2,k)*ip2*ik
      q1=-2*hvq_dotprod(p1,k1)*ip1
      q2=-2*hvq_dotprod(p2,k2)*ip2
      s2=s+tk+uk 
      q1c=-s-tk-q1 
      q2c=-s-uk-q2 
      w1=-q1+q2-tk 
      w2=q1-q2-uk 
      T1R = -(4*S*S2*W1**4+(8*S*S2*TK+(-4*S**2-12*Q2*S)*S2)*W1**3+(8*M**
     1   2*TK**3+((8*S-16*M**2)*S2+16*M**2*S)*TK**2+((8*M**2-4*S)*S2**2+
     2   ((-20*Q2-12*M**2)*S-4*S**2)*S2+12*M**2*S**2)*TK+2*S*S2**3+(4*Q2
     3   +4*M**2)*S*S2**2+(2*S**3+(8*Q2+4*M**2)*S**2+16*Q2**2*S)*S2+4*M*
     4   *2*S**3)*W1**2+(4*S*S2*TK**3+(((8*M**2-12*Q2)*S+16*M**2*Q2)*S2-
     5   4*S*S2**2)*TK**2+(2*S*S2**3+(8*Q2*S-16*M**2*Q2)*S2**2+(2*S**3+(
     6   4*Q2+12*M**2)*S**2+(16*Q2**2+16*M**2*Q2)*S)*S2)*TK-2*Q2*S*S2**3
     7   +(-4*Q2**2-4*M**2*Q2)*S*S2**2+(-2*Q2*S**3+(-4*Q2**2-4*M**2*Q2)*
     8   S**2-8*Q2**3*S)*S2)*W1+(4*M**2*S**2+(8*M**2*Q2+8*M**4)*S+8*M**2
     9   *Q2**2)*S2**2*TK)/(S**2*S2**2*TK*W1**2)
      T2R = -(4*S*S2*W2**4+(8*S*S2*UK+(-4*S**2-12*Q1*S)*S2)*W2**3+(8*M**
     1   2*UK**3+((8*S-16*M**2)*S2+16*M**2*S)*UK**2+((8*M**2-4*S)*S2**2+
     2   ((-20*Q1-12*M**2)*S-4*S**2)*S2+12*M**2*S**2)*UK+2*S*S2**3+(4*Q1
     3   +4*M**2)*S*S2**2+(2*S**3+(8*Q1+4*M**2)*S**2+16*Q1**2*S)*S2+4*M*
     4   *2*S**3)*W2**2+(4*S*S2*UK**3+(((8*M**2-12*Q1)*S+16*M**2*Q1)*S2-
     5   4*S*S2**2)*UK**2+(2*S*S2**3+(8*Q1*S-16*M**2*Q1)*S2**2+(2*S**3+(
     6   4*Q1+12*M**2)*S**2+(16*Q1**2+16*M**2*Q1)*S)*S2)*UK-2*Q1*S*S2**3
     7   +(-4*Q1**2-4*M**2*Q1)*S*S2**2+(-2*Q1*S**3+(-4*Q1**2-4*M**2*Q1)*
     8   S**2-8*Q1**3*S)*S2)*W2+(4*M**2*S**2+(8*M**2*Q1+8*M**4)*S+8*M**2
     9   *Q1**2)*S2**2*UK)/(S**2*S2**2*UK*W2**2)
      END

      function hvq_ipick(y1,y2,y3,y4,y5,y6)
      implicit none
      integer hvq_ipick
      real * 8 y1,y2,y3,y4,y5,y6
      real * 8 t1,t2,t3,t4,t5,t6,r
      real * 8 hvq_random
      external hvq_random
      t1=y1
      t2=t1+y2
      t3=t2+y3
      t4=t3+y4
      t5=t4+y5
      t6=t5+y6
      r=t6*hvq_random()
      if(r.lt.t1) then
         hvq_ipick=1
      elseif(r.lt.t2) then
         hvq_ipick=2
      elseif(r.lt.t3) then
         hvq_ipick=3
      elseif(r.lt.t4) then
         hvq_ipick=4
      elseif(r.lt.t5) then
         hvq_ipick=5
      else
         hvq_ipick=6
      endif
      end
