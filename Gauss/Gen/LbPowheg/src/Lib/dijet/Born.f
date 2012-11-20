      subroutine dijet_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_kn.h'

      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      real *8 borntmp
      integer mu,nu,j,k

      integer ileg,ioleg
C     define a real *8 value for nc in order
C     to avoid integer by integer division
      real *8 ncol
      parameter (ncol=nc)
      real *8 VC
      parameter(VC=ncol**2-1)

C     Abbreviation of (4.*pi*st_alpha)**2
      real*8 gs4

      real * 8 gtens(0:3,0:3),ap
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      save gtens

      real *8  HAt  ,HAu  ,HB  ,HCs  ,HCt  ,HCu
      real *8  dijet_HAtFn,dijet_HAuFn,dijet_HBFn,dijet_HCsFn,dijet_HCtFn,dijet_HCuFn,dijet_HDsFn,dijet_HDtFn,dijet_HDuFn,dijet_dotp
      external dijet_HAtFn,dijet_HAuFn,dijet_HBFn,dijet_HCsFn,dijet_HCtFn,dijet_HCuFn,dijet_HDsFn,dijet_HDtFn,dijet_HDuFn,dijet_dotp
      real * 8 symfac

c     The process class according to Kunszt Soper
      character*1  ks_label
c     The map from POWHEG-BOX to Kunszt-Soper particles and momenta.
      integer ksmap(4)
c     Kunszt-Soper momenta analogous to POWHEG-BOX.
      real*8  k1(0:3),k2(0:3),k3(0:3),k4(0:3)
c     Variables to hold spin / colour average factor (& overall +/- sign)
      real*8 spin_col_avg

C     Setting the QCD coupling squared.
      gs4 = (4.*pi*st_alpha)**2

c     Get the Kunszt Soper and Madgraph labels as well as the map
c     to Kunszt Soper conventions:
      call dijet_KS_2TO2_MAP(bflav,nlegborn,ks_label,ksmap)  
c     Assign Kunszt Soper momenta
      do mu=0,3
        p(mu,3)=-p(mu,3)      ! temprarily flip the outgoing momenta
        p(mu,4)=-p(mu,4)      !
        k1(mu)=p(mu,ksmap(1)) ! setting the Kunszt Soper momenta using
        k2(mu)=p(mu,ksmap(2)) ! the Kunszt Soper map.
        k3(mu)=p(mu,ksmap(3)) ! 
        k4(mu)=p(mu,ksmap(4)) !
        p(mu,3)=-p(mu,3)      ! restore the powheg outgoing momenta
        p(mu,4)=-p(mu,4)      !
      enddo      

c Colour factors for colour-correlated Born amplitudes;
c Rule from Kunszt-Soper.
c First, identify the flavour structure

C --------------------------------------------------------------------
C     A-type: q + Q -> q + Q plus charge conjugations and crossings
C --------------------------------------------------------------------
      if(ks_label.eq.'A') then
         spin_col_avg = 4.*ncol*ncol

         HAt = dijet_HAtFn(k1,k2,k3,k4)

         bornjk(ksmap(1),ksmap(2)) = 2*VC/nc*HAt
         bornjk(ksmap(1),ksmap(3)) =  -VC/nc*HAt
         bornjk(ksmap(1),ksmap(4)) =   VC/nc*(nc**2-2)*HAt

         bornjk(ksmap(3),ksmap(4)) = bornjk(ksmap(1),ksmap(2))
         bornjk(ksmap(2),ksmap(4)) = bornjk(ksmap(1),ksmap(3))
         bornjk(ksmap(2),ksmap(3)) = bornjk(ksmap(1),ksmap(4))

C --------------------------------------------------------------------
C     B-type: q + q -> q + q plus charge conjugations
C --------------------------------------------------------------------
      elseif(ks_label.eq.'B') then
         spin_col_avg = 4.*ncol*ncol

         HB  = dijet_HBFn(k1,k2,k3,k4)
         HAt = dijet_HAtFn(k1,k2,k3,k4)
         HAu = dijet_HAuFn(k1,k2,k3,k4)

         bornjk(ksmap(1),ksmap(2)) = 2*VC/nc/nc *
     $                               (-HB+nc*(HAt+HAu)-nc**2*HB)
         bornjk(ksmap(1),ksmap(3)) = 2*VC/nc/nc *
     $                               (HB-nc*(HAu+0.5*HAt)+0.5*nc**3*HAu)
         bornjk(ksmap(1),ksmap(4)) = 2*VC/nc/nc *
     $                               (HB-nc*(HAt+0.5*HAu)+0.5*nc**3*HAt)

         bornjk(ksmap(3),ksmap(4)) = bornjk(ksmap(1),ksmap(2))
         bornjk(ksmap(2),ksmap(4)) = bornjk(ksmap(1),ksmap(3))
         bornjk(ksmap(2),ksmap(3)) = bornjk(ksmap(1),ksmap(4))

C --------------------------------------------------------------------
C     C-type: q + qb -> g + g plus charge conjugations & crossings
C --------------------------------------------------------------------
      elseif(ks_label.eq.'C') then

         spin_col_avg=1
         do k=1,2
            if(bflav(k).ne.0) then
c - sign for crossing fermion line
               spin_col_avg=-spin_col_avg*(2*ncol)
            else
               spin_col_avg=spin_col_avg*(2*Vc)
            endif
         enddo

         HCs = dijet_HCsFn(k1,k2,k3,k4)
         HCt = dijet_HCtFn(k1,k2,k3,k4)
         HCu = dijet_HCuFn(k1,k2,k3,k4)

         bornjk(ksmap(1),ksmap(2)) = VC*(-(HCt+HCu-HCs) + 1./nc/nc*HCs )
         bornjk(ksmap(1),ksmap(3)) = VC*(  nc**2*HCu-HCs )
         bornjk(ksmap(1),ksmap(4)) = VC*(  nc**2*HCt-HCs )

         bornjk(ksmap(2),ksmap(4)) = bornjk(ksmap(1),ksmap(3))
         bornjk(ksmap(2),ksmap(3)) = bornjk(ksmap(1),ksmap(4))
         bornjk(ksmap(3),ksmap(4)) = VC*nc**2 * (HCt+HCu)

C --------------------------------------------------------------------
C     D-type: g + g -> g + g
C --------------------------------------------------------------------
      elseif(ks_label.eq.'D') then
         spin_col_avg =  4.*VC*VC

         bornjk(ksmap(1),ksmap(2)) = 2*VC*nc**3*dijet_HDsFn(k1,k2,k3,k4)
         bornjk(ksmap(1),ksmap(3)) = 2*VC*nc**3*dijet_HDtFn(k1,k2,k3,k4)
         bornjk(ksmap(1),ksmap(4)) = 2*VC*nc**3*dijet_HDuFn(k1,k2,k3,k4)

         bornjk(ksmap(2),ksmap(4)) = bornjk(ksmap(1),ksmap(3))
         bornjk(ksmap(2),ksmap(3)) = bornjk(ksmap(1),ksmap(4))
         bornjk(ksmap(3),ksmap(4)) = bornjk(ksmap(1),ksmap(2))

      else
         write(*,*) 'dijet_setborn: could not identify flavour list!'
         call exit(1)
      endif



C --------------------------------------------------------------------
C     Symmetrize and normalise bornjk matrix
C --------------------------------------------------------------------
      do j=1,nlegborn
C - bornjk(j,j) is not used in dijet_soft
         bornjk(j,j)=0d0
         do k=j+1,nlegborn
            bornjk(ksmap(k),ksmap(j))=bornjk(ksmap(j),ksmap(k))
         enddo
      enddo

c Normalize: Kunszt and Soper have an extra 2, see eq A8 and A11 in
c PRD46-192
      if(bflav(3).eq.bflav(4)) then
         symfac=0.5d0
      else
         symfac=1
      endif
      do j=1,nlegborn
         do k=1,nlegborn
            bornjk(j,k)=bornjk(j,k)/2*symfac*gs4/spin_col_avg
         enddo
      enddo

      
      born=0
      DO J=2,nlegborn
         born=born+bornjk(1,j)
      enddo
      if(bflav(1).eq.0) then
         born=born/ca
      else
         born=born/cf
      endif
c     spin-projected here are trivial:
      do ileg=1,nlegborn
         if(bflav(ileg).eq.0) then
c find opposite leg
            if(ileg.eq.1) then
               ioleg=2
            elseif(ileg.eq.2) then
               ioleg=1
            elseif(ileg.eq.3) then
               ioleg=4
            elseif(ileg.eq.4) then
               ioleg=3
            endif
            do mu=0,3
               do nu=0,3
                  bmunu(mu,nu,ileg)=(-gtens(mu,nu)+
     1           (kn_cmpborn(mu,ileg)*kn_cmpborn(nu,ioleg)
     2           +kn_cmpborn(nu,ileg)*kn_cmpborn(mu,ioleg))/
     2            dijet_dotp(kn_cmpborn(0,ileg),kn_cmpborn(0,ioleg)))*born/2
               enddo
            enddo
         endif
      enddo
      end


C - The following functions are taken from Kunszt & Soper Phys.Rev.D46,1 192 

      function dijet_HAtFn(p1,p2,p3,p4)
      real *8 dijet_HAtFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
      dijet_HAtFn=2.*(s**2+u**2)/t**2
      return
      end

      function dijet_HAuFn(p1,p2,p3,p4)
      real *8 dijet_HAuFn,dijet_HAtFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      dijet_HAuFn=dijet_HAtFn(p1,p2,p4,p3)
      return
      end


      function dijet_HBFn(p1,p2,p3,p4)
      real *8 dijet_HBFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
      dijet_HBFn=2.*s**2/t/u
      return
      end


      function dijet_HCtFn(p1,p2,p3,p4)
      real *8 dijet_HCtFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
      dijet_HCtFn=(2.*(t**2+u**2)/s**2 )*t/u
      return
      end

      function dijet_HCuFn(p1,p2,p3,p4)
      real *8 dijet_HCuFn,dijet_HCtFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      dijet_HCuFn=dijet_HCtFn(p1,p2,p4,p3)
      return
      end

      function dijet_HCsFn(p1,p2,p3,p4)
      real *8 dijet_HCsFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
      dijet_HCsFn=2.*(t**2+u**2)/t/u
      return
      end


      function dijet_HDsFn(p1,p2,p3,p4)
      real *8 dijet_HDsFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      real *8 s,t,u
      real *8 dijet_dotp
      external dijet_dotp
      s=2.*dijet_dotp(p1,p2)
      t=2.*dijet_dotp(p1,p3)
      u=2.*dijet_dotp(p1,p4)
      dijet_HDsFn=2.*(t**2+u**2) *(s**4+t**4+u**4)/(s*t*u)**2
      return
      end

      function dijet_HDtFn(p1,p2,p3,p4)
      real *8 dijet_HDtFn,dijet_HDsFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      dijet_HDtFn=dijet_HDsFn(p1,p3,p2,p4)
      return
      end

      function dijet_HDuFn(p1,p2,p3,p4)
      real *8 dijet_HDuFn,dijet_HDsFn
      real *8 p1(0:3),p2(0:3),p3(0:3),p4(0:3)
      dijet_HDuFn=dijet_HDsFn(p1,p4,p2,p3)
      return
      end


      subroutine dijet_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface.
c Here we assume all particles to be outgoing, and
c assign colour according to the corresponding colour amplitudes.
c At the end, the colour of incoming partons are conjugated.
      implicit none
      include 'src/Lib/dijet/LesHouches.h'
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      integer iq,ia,iq1,iq2,ia1,ia2,ig1,ig2,j,itmp
      real * 8 s,t,u
      real * 8 dijet_dotp
c g g g g
      if(idup(1).eq.21.and.idup(2).eq.21
     1       .and.idup(3).eq.21.and.idup(4).eq.21) then
         s=2*dijet_dotp(kn_cmpborn(0,1),kn_cmpborn(0,2))
         t=-2*dijet_dotp(kn_cmpborn(0,2),kn_cmpborn(0,3))
         u=-s-t
         call dijet_borncolour4g(icolup(1,1),icolup(1,2),
     1                     icolup(1,3),icolup(1,4),s,t,u)
c q qb g g or permutations-crossing
      elseif(idup(1).eq.21.or.idup(2).eq.21
     1       .or.idup(3).eq.21.or.idup(4).eq.21) then
c find the quarks and gluons
         ig1=-1
         do j=1,4
            if(idup(j).eq.21) then
               if(ig1.lt.0) then
                  ig1=j
               else
                  ig2=j
               endif
            elseif(idup(j)*istup(j).gt.0) then
               iq=j
            elseif(idup(j)*istup(j).lt.0) then
               ia=j
            else
               write(*,*) 'dijet_borncolour_lh: should not be here!'
               call exit(1)
            endif
         enddo
c using istup we reverse the sign of incoming particle momenta,
c so that the choice of colour can be made independently of which
c particle is incoming.
         s=istup(iq)*istup(ia)*2*dijet_dotp(kn_cmpborn(0,iq),kn_cmpborn(0,ia))
         t=istup(ia)*istup(ig1)*2*
     1          dijet_dotp(kn_cmpborn(0,ia),kn_cmpborn(0,ig1))
         u=-s-t
         call dijet_borncolour2g(icolup(1,iq),icolup(1,ia),
     1        icolup(1,ig1),icolup(1,ig2),s,t,u)
c q q qb qb, or q Q qb Qb, plus permutations-crossing
      else
         iq1=-1
         iq2=-1
         ia1=-1
         ia2=-1
         do j=1,4
            if(idup(j)*istup(j).gt.0) then
               if(iq1.lt.0) then
                  iq1=j
               else
                  iq2=j
               endif
            else
               if(ia1.lt.0) then
                  ia1=j
               else
                  ia2=j
               endif
            endif
         enddo
         if(idup(iq1)*istup(iq1).eq.idup(iq2)*istup(iq2)) then
c     q q qb qb
            s=istup(iq1)*istup(iq2)*2*
     1          dijet_dotp(kn_cmpborn(0,iq1),kn_cmpborn(0,iq2))
            t=istup(iq2)*istup(ia1)*2*
     1          dijet_dotp(kn_cmpborn(0,iq2),kn_cmpborn(0,ia1))
            u=-s-t
            call dijet_borncolour4q(icolup(1,iq1),icolup(1,iq2),
     1           icolup(1,ia1),icolup(1,ia2),s,t,u)
         else
c     q Q qb Qb
            if(istup(iq1)*idup(iq1).ne.-istup(ia1)*idup(ia1)) then
               itmp=ia1
               ia1=ia2
               ia2=itmp
            endif
            call dijet_colourjoin4q(icolup(1,iq1),icolup(1,iq2),
     1           icolup(1,ia1),icolup(1,ia2))
         endif
      endif
c Conjugate incoming colours
      call dijet_colour_conj(icolup(1,1))
      call dijet_colour_conj(icolup(1,2))   
      end

      subroutine dijet_borncolour4g(icol1,icol2,icol3,icol4,s,t,u)
c g g g g
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      real * 8 s,t,u
      real * 8 rst,rtu,rsu,r
      real * 8 dijet_random
c planar results for st channel
      rst=(t/s+s/t+1)**2
c su channel
      rsu=(u/s+s/u+1)**2
c tu channel
      rtu=(t/u+u/t+1)**2
c Obtained by maxima; check:
c rst+rsu+rtu=2(3-us/t^2-ut/s^2-st/u^2)
      r=dijet_random()*(rst+rsu+rtu)
      if(r.lt.rst) then
         call dijet_colourjoin4g(icol1,icol2,icol3,icol4)
      elseif(r.lt.rst+rsu) then
         call dijet_colourjoin4g(icol1,icol2,icol4,icol3)
      else
         call dijet_colourjoin4g(icol1,icol3,icol2,icol4)
      endif
      end

      subroutine dijet_colourjoin4g(icol1,icol2,icol3,icol4)
c perform a planar colour connection on the planar sequence
c of gluons
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      integer newcolor
      call dijet_getnewcolor(newcolor)
      icol1(2)=newcolor
      icol2(1)=newcolor
      call dijet_getnewcolor(newcolor)
      icol2(2)=newcolor
      icol3(1)=newcolor
      call dijet_getnewcolor(newcolor)
      icol3(2)=newcolor
      icol4(1)=newcolor
      call dijet_getnewcolor(newcolor)
      icol4(2)=newcolor
      icol1(1)=newcolor
      end

      subroutine dijet_borncolour2g(icol1,icol2,icol3,icol4,s,t,u)
c                             q     qbar  g     g
c q qb g g
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      real * 8 s,t,u
      real * 8 rt,ru,r
      real * 8 dijet_random
c      rt=u/t*(u**2+t**2)/s**2
c      ru=t/u*(u**2+t**2)/s**2
c obtained by maxima; check: rt+ru=(1/(tu)-2/s^2)*(u^2+t^2)
c watch out! crossin a fermion line to get q g->q g needs an
c extra - sign;
      rt=abs(u/t)
      ru=abs(t/u)
      r=dijet_random()*(rt+ru)
      if(r.lt.rt) then
         call dijet_colourjoin2g(icol1,icol2,icol3,icol4)
      else
         call dijet_colourjoin2g(icol1,icol2,icol4,icol3)
      endif
      end

      subroutine dijet_colourjoin2g(icol1,icol2,icol3,icol4)
c                             q     qbar  g     g
c perform a planar colour connection on the planar sequence
c q qbar g g
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      integer newcolor
      icol1(2)=0
      icol2(1)=0
      call dijet_getnewcolor(newcolor)
      icol1(1)=newcolor
      icol4(2)=newcolor
      call dijet_getnewcolor(newcolor)
      icol4(1)=newcolor
      icol3(2)=newcolor
      call dijet_getnewcolor(newcolor)
      icol3(1)=newcolor
      icol2(2)=newcolor
      end

      
      subroutine dijet_borncolour4q(icol1,icol2,icol3,icol4,s,t,u)
c                             q     q     qbar  qbar
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      real * 8 s,t,u
      real * 8 rt,ru,r
      real * 8 dijet_random
c the following is from q Q -> Q q
      rt=(s**2+u**2)/t**2
c q Q->q Q
      ru=(s**2+t**2)/u**2
      r=dijet_random()*(rt+ru)
      if(r.lt.rt) then
c t channel gluon (23 channel; thus colour is exchanged
c 2->4 and 1->3
         call dijet_colourjoin4q(icol1,icol2,icol4,icol3)
      else
         call dijet_colourjoin4q(icol1,icol2,icol3,icol4)
      endif
      end

      subroutine dijet_colourjoin4q(icol1,icol2,icol3,icol4)
c                             q     q     qbar  qbar
c perform a planar colour connection on the planar sequence
c q qbar g g
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      integer newcolor
      icol1(2)=0
      icol2(2)=0
      icol3(1)=0
      icol4(1)=0
      call dijet_getnewcolor(newcolor)
      icol1(1)=newcolor
      icol4(2)=newcolor
      call dijet_getnewcolor(newcolor)
      icol2(1)=newcolor
      icol3(2)=newcolor
      end


      subroutine dijet_finalize_lh
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
      implicit none

c     The general reshuffling procedure.
      call dijet_lhefinitemasses

      end
