      subroutine wbb_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      real * 8 amp2
c     
      amp2 = 0d0
      call wbb_compborn(p,bflav,amp2,bornjk,bmunu)      
      if(amp2.eq.0d0) then
        write(*,*) 'WARNING wbb_setborn: returning 0 amplitude!'
        write(*,*) bflav(1),' ',bflav(2),'->',bflav(3),' ',bflav(4),
     -         ' ',bflav(5)
      endif
      born=amp2
      end
c
c
      subroutine wbb_compborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/PhysPars.h'  
      include 'src/Lib/wbb/pwhg_math.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/pwhg_st.h'
      include 'src/Lib/wbb/coupl.inc'       !  Madgraph common block
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      integer mu,nu,i,j,k
      real * 8 gs2,gweak
      double precision amp2_mad,amp2_mad1,amp2_mad2,amp2_mad3,amp2_mad4
      double precision amp2_dub_wmbbb_mad,amp2_ubd_wmbbb_mad,
     $     amp2_dbu_wpbbb_mad,amp2_udb_wpbbb_mad
      logical MG_check
      parameter (MG_check=.false.)
      logical ini
      save ini
      data ini/.true./

      call wbb_M2_Wbb(p,born)
c 
      born=born*ph_CKM_matrix(abs(bflav(1)),abs(bflav(2)))**2
c     couplings
      born=born*(ph_unit_e/ph_sthw)**2/8 * (4*pi*st_alpha)**2
c     color factor
      born=born*(nc**2-1d0)/4d0

c     check with Madgraph   
      if (MG_check) then
         if (ini) then
            call wbb_initialize
            ini = .false.
         endif
c     Set MADGRAPH Wmass to the W virtuality
         WMASS=kn_masses(3)
         call wbb_SDUB_WMBBB(p,amp2_dub_wmbbb_mad)
         call wbb_SUBD_WMBBB(p,amp2_ubd_wmbbb_mad)
         call wbb_SDBU_WPBBB(p,amp2_dbu_wpbbb_mad)
         call wbb_SUDB_WPBBB(p,amp2_udb_wpbbb_mad)
         amp2_mad = amp2_dub_wmbbb_mad
         amp2_mad1 = amp2_dub_wmbbb_mad
         amp2_mad2 = amp2_ubd_wmbbb_mad
         amp2_mad3 = amp2_dbu_wpbbb_mad
         amp2_mad4 = amp2_udb_wpbbb_mad
         gs2 = 4*pi*st_alpha
         gweak = ph_unit_e/ph_sthw     
c         write(*,*) 'must be -1 =============>',gweak/(gwf(1)*sqrt(2d0))   
         amp2_mad=amp2_mad*ph_CKM_matrix(abs(bflav(1)),abs(bflav(2)))**2
         amp2_mad=amp2_mad* (gweak/(gwf(1)*dsqrt(2d0)))**2 * gs2**2
         write(*,*) 'bflav ',bflav(1),bflav(2),
     $        ' MUST BE 1 ===> ',born / amp2_mad
         write(*,*)  'ratio dub/ubd',amp2_mad1 / amp2_mad2
         write(*,*)  'ratio dub/dbu',amp2_mad1 / amp2_mad3
         write(*,*)  'ratio dub/udb',amp2_mad1 / amp2_mad4
      endif
c
c     Born amplitude and the color- and spin-correlated ones, needed as
c     counterterms for real contributions
      do j=1,nlegs
         do mu=0,3
            do nu=0,3
               bmunu(mu,nu,j)=0d0
            enddo
         enddo
      enddo
      do j=1,nlegs
         do k=1,nlegs
            bornjk(j,k)=0
         enddo
      enddo
      bornjk(1,1)=cf
      bornjk(2,2)=cf
      bornjk(4,4)=cf
      bornjk(5,5)=cf
      if (bflav(1).lt.0) then
         bornjk(1,2)=-(cf-ca/2)
         bornjk(1,4)=(2*(cf-ca/2))
         bornjk(1,5)=-(2*cf-ca/2)
         bornjk(2,4)=bornjk(1,5)
         bornjk(2,5)=bornjk(1,4)
         bornjk(4,5)=bornjk(1,2)
      else
         bornjk(1,2)=-(cf-ca/2)
         bornjk(1,4)=-(2*cf-ca/2)
         bornjk(1,5)=(2*(cf-ca/2))
         bornjk(2,4)=bornjk(1,5)
         bornjk(2,5)=bornjk(1,4)
         bornjk(4,5)=bornjk(1,2)
      endif
      do j=1,nlegs
         do k=1,nlegs
            bornjk(j,k)=-born*bornjk(j,k)
         enddo
      enddo
      do j=1,nlegs
         do k=j+1,nlegs
            bornjk(k,j)=bornjk(j,k)
         enddo
      enddo
      end

c
c subroutine for Born
c
      subroutine wbb_M2_Wbb(pphy,amp2)
      implicit none
      external wbb_dot_4
      real*8 wbb_dot_4
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_math.h'
      include 'src/Lib/wbb/pwhg_st.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/PhysPars.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real *8 pphy(0:3,nlegs)
      real*8 q1(4),q2(4),pb(4),p1b(4),pw(4),mb,mw
      real*8 VC,AC,den0x1,den0x2,den0x3,q1Dq2,
     #     q1Dpb,q1Dp1b,q2Dpb,q2Dp1b,
     #     pbDp1b,s1,s2,s3,s4,s5,s6,t0
c change this 
      real* 8 amp2
c
      logical*4 ldebug
      parameter(ldebug=.false.)
c      parameter(ldebug=.true.)
      integer j
c
      VC=1d0
      AC=-1d0
      mb=ph_mb
c      mw=ph_Wmass
      mw=kn_masses(3)             ! NOT necessarily equal to the W mass

      do j=1,3
         q1(j)=pphy(j,1)
         q2(j)=pphy(j,2)
         pw(j)=pphy(j,3)
         pb(j)=pphy(j,4)
         p1b(j)=pphy(j,5)
      enddo
      q1(4)=pphy(0,1)
      q2(4)=pphy(0,2)
      pw(4)=pphy(0,3)
      pb(4)=pphy(0,4)
      p1b(4)=pphy(0,5)
c     
      q1Dq2=wbb_dot_4(q1,q2)
      q1Dpb=wbb_dot_4(q1,pb)
      q1Dp1b=wbb_dot_4(q1,p1b)
      q2Dpb=wbb_dot_4(q2,pb)
      q2Dp1b=wbb_dot_4(q2,p1b)
      pbDp1b=wbb_dot_4(pb,p1b)
      den0x1=-2d0*wbb_dot_4(q1,pw)+wbb_dot_4(pw,pw)
      den0x2=-2d0*wbb_dot_4(q2,pw)+wbb_dot_4(pw,pw)
      den0x3=wbb_dot_4(pb,pb)+2d0*wbb_dot_4(pb,p1b)+wbb_dot_4(p1b,p1b)
c
      s2 = VC**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb
     #*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp
     #1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b)
      s3 = VC**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b+384*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b+
     #384*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+384*q1Dq2*q2Dp1b*mb*
     #*2-256*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2-512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b-256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s1 = s2+s3
      s2 = s1+VC**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq
     #2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q
     #1Dq2*q1Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb
     #*q1Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q
     #1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q
     #1Dp1b*q2Dp1b*pbDp1b+128*q1Dp1b**2*q2Dpb)
      s3 = s2
      s5 = AC**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2+128*q1Dq2*pbDp1b*mb**2+128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b-128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb
     #*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b-128*q1Dp1b*q2Dpb*mb**2+128*q1Dp
     #1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b)
      s6 = AC**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b+384*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b+384*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b+
     #384*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b+384*q1Dq2*q2Dp1b*mb*
     #*2-256*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2-512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b-256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s4 = s5+s6
      t0 = s3+s4
      s2 = VC**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq2*q
     #2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq
     #2*q2Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q2
     #Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+128*q1Dpb*
     #q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1D
     #p1b*q2Dp1b*mb**2)
      s4 = VC**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-
     #128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb*
     #*2+768*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b+256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s5 = VC**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q
     #1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq
     #2*q1Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q1
     #Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dp
     #b*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*mb**2+128
     #*q1Dp1b**2*q2Dpb)
      s3 = s4+s5
      s1 = s2+s3
      s2 = s1+AC**2/den0x1**2/den0x3**2*(256*q1Dq2*q2Dpb*q2Dp1b-128*q1Dq
     #2*q2Dpb*pbDp1b-128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q
     #1Dq2*q2Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb
     #*q2Dpb*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dpb*q2Dpb*mb**2+128*q1D
     #pb*q2Dp1b*mb**2+128*q1Dpb*q2Dp1b**2-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1
     #Dp1b*q2Dpb*mb**2+128*q1Dp1b*q2Dpb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*
     #q1Dp1b*q2Dp1b*mb**2)
      s3 = s2
      s5 = AC**2/den0x1/den0x2/den0x3**2*(-256*q1Dq2*q1Dpb*q2Dp1b-128*q1
     #Dq2*q1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-256*q1Dq2*q1Dp1b*q2Dpb-128*
     #q1Dq2*q1Dp1b*pbDp1b-128*q1Dq2*q1Dp1b*mb**2-128*q1Dq2*q2Dpb*pbDp1b-
     #128*q1Dq2*q2Dpb*mb**2-128*q1Dq2*q2Dp1b*pbDp1b-128*q1Dq2*q2Dp1b*mb*
     #*2+768*q1Dq2*pbDp1b*mb**2+256*q1Dq2*pbDp1b**2+512*q1Dq2*mb**4+256*
     #q1Dq2**2*pbDp1b+256*q1Dq2**2*mb**2-128*q1Dpb*q1Dp1b*q2Dpb-128*q1Dp
     #b*q1Dp1b*q2Dp1b-128*q1Dpb*q2Dpb*q2Dp1b+128*q1Dpb*q2Dp1b**2+128*q1D
     #pb**2*q2Dp1b-128*q1Dp1b*q2Dpb*q2Dp1b+128*q1Dp1b*q2Dpb**2+128*q1Dp1
     #b**2*q2Dpb)
      s6 = AC**2/den0x2**2/den0x3**2*(256*q1Dq2*q1Dpb*q1Dp1b-128*q1Dq2*q
     #1Dpb*pbDp1b-128*q1Dq2*q1Dpb*mb**2-128*q1Dq2*q1Dp1b*pbDp1b-128*q1Dq
     #2*q1Dp1b*mb**2-128*q1Dq2*pbDp1b*mb**2-128*q1Dq2*mb**4-128*q1Dpb*q1
     #Dp1b*q2Dpb-128*q1Dpb*q1Dp1b*q2Dp1b+128*q1Dpb*q2Dpb*pbDp1b+256*q1Dp
     #b*q2Dpb*mb**2+128*q1Dpb*q2Dp1b*mb**2+128*q1Dpb**2*q2Dp1b+128*q1Dp1
     #b*q2Dpb*mb**2+128*q1Dp1b*q2Dp1b*pbDp1b+256*q1Dp1b*q2Dp1b*mb**2+128
     #*q1Dp1b**2*q2Dpb)
      s4 = s5+s6
      t0 = s3+s4
c
      amp2=t0/4d0/9d0
c
      if (ldebug) then
         print*,'ampl2_wbb_lo = ',amp2
      endif
c
      return
      end
c
      real*8 function wbb_dot_4(p1,p2)
      implicit none
      real*8 p1(4),p2(4)
      wbb_dot_4=-p1(1)*p2(1)-p1(2)*p2(2)-p1(3)*p2(3)+p1(4)*p2(4)
      return
      end
c
c


      subroutine wbb_finalize_lh
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
      
      call wbb_Vdecay
     
c     Resonance Z -> e-(3) e+(4)
c      call wbb_add_resonance(23,3,4)
      end


      subroutine wbb_borncolour_lh
      implicit none
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface

C We deal here with the CKM matrix elements for VBF Higgs boson production
      include 'src/Lib/wbb/LesHouches.h'
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/pwhg_flst.h'
      integer i

      if(idup(1).gt.0) then
         icolup(1,1)=501
         icolup(2,1)=0
      else
         icolup(1,1)=0
         icolup(2,1)=501
      endif
      
      if(idup(2).gt.0) then
         icolup(1,2)=502
         icolup(2,2)=0
      else
         icolup(1,2)=0
         icolup(2,2)=502
      endif

c     W Boson                
      icolup(1,3)=0
      icolup(2,3)=0

      if(idup(1).gt.0) then
         do i=1,2
            icolup(i,4)=icolup(i,1)
            icolup(i,5)=icolup(i,2)
         enddo
      else
         do i=1,2
            icolup(i,4)=icolup(i,2)
            icolup(i,5)=icolup(i,1)
         enddo
      endif
      end


      function wbb_signn(j)
      implicit none
      integer wbb_signn, j
      if (j.eq.0) then
         wbb_signn =0
      else
         wbb_signn =j/abs(j)
      endif
      end



      subroutine wbb_pwhg_boost(beta,pin,pout)
      implicit none
      real * 8 beta(1:3),pin(0:3),pout(0:3)
      real * 8 vec(1:3),b,bsq,gamma,bdv,pin0
      integer j
c
      vec(1) = pin(1)
      vec(2) = pin(2)
      vec(3) = pin(3)
c beta value
      bsq = beta(1)**2+beta(2)**2+beta(3)**2
      b = sqrt(bsq)
      if (b**2.ge.1d0) then
         write(*,*) 'ERROR: boost vector has norm bigger than 1'
         write(*,*) 'RETURN 0'
         pout(0) = 0d0
         pout(1) = 0d0
         pout(2) = 0d0
         pout(3) = 0d0
         RETURN
      elseif (b.eq.0d0) then
         pout(0) = pin(0)
         pout(1) = pin(1)
         pout(2) = pin(2)
         pout(3) = pin(3)
         RETURN
      endif
      
      gamma = 1d0/sqrt(1d0-bsq)
c beta . vec
      bdv = beta(1)*vec(1)+beta(2)*vec(2)+beta(3)*vec(3)
c boost
      do j=1,3
         vec(j) = vec(j)+gamma*beta(j)/b*(bdv/b*(1-1/gamma)+b*pin(0))
c         vec(j) = gamma*beta(j)*pin(0) + vec(j) + 
c     #        beta(j)*bdv*(gamma-1d0)/vsq
      enddo
      pin0 = gamma*(pin(0)+bdv)
c return values
      pout(0) = pin0
      pout(1) = vec(1)
      pout(2) = vec(2)
      pout(3) = vec(3)
      end


      subroutine wbb_LH_pwhg_mom(pin,pout)
      implicit none
      real * 8 pin(5),pout(0:3)
      pout(0)=pin(4)
      pout(1)=pin(1)
      pout(2)=pin(2)
      pout(3)=pin(3)
      end
 
      subroutine wbb_assign_mom(pin,pout)
      implicit none
      real * 8 pin(0:3),pout(0:3)
      integer mu
      do mu=0,3
         pout(mu)=pin(mu)
      enddo
      end
      
      subroutine wbb_LH_pwhg_flav(flav)
      implicit none      
      include 'src/Lib/wbb/LesHouches.h'
      integer flav(nup)
      integer i
      do i=1,nup
         flav(i)=idup(i)
         if (flav(i).eq.21) then
            flav(i)=0
         endif
      enddo
      end
      


      subroutine wbb_Vdecay
      implicit none
      include 'src/Lib/wbb/nlegborn.h'
      include 'src/Lib/wbb/LesHouches.h'
      include 'src/Lib/wbb/pwhg_math.h'
      include 'src/Lib/wbb/pwhg_kn.h'
      include 'src/Lib/wbb/PhysPars.h'
      real * 8 pw_rest(0:3),beta(3),beta_L_CM(3),Vdec_rest(0:3,2),
     $     Vdec(0:3,2), beta_CM_L(3),betamod_L_CM,vec(1:3)
      real * 8 sinth,costh,phi
      integer i,mu
      integer mup
      parameter (mup=6)         ! maximum number of particles
      integer flav(mup),flav_dec(mup+1)
      real * 8 p(0:3,mup),bornjk(nlegborn,nlegborn),p_dec(0:3,mup+1),
     $     p_decboost(0:3,mup+1),p_deccm(0:3,mup+1)
      real * 8 bmunu(0:3,0:3,nlegborn)
      real * 8 born_dec,born_pwhg,amp2_dec,amp2_pwhg,mW2,amp2_deccm
      real * 8 wbb_random
      external wbb_random
      logical ini
      data ini/.true./
      save ini 
      real * 8 ratio,pwvirt2,upperf
      real * 8 rat
      save rat
      data rat/0d0/
      integer count,nubound,countmax
      save count
      data count/1/
      real * 8 wbb_powheginput
      external wbb_powheginput
      integer iii      
      real *8 boundnorm
      data boundnorm/0d0/
      save boundnorm
      logical verbose
      parameter (verbose=.true.)
      integer idvecbos,vdecaymode,chargelept,neutralept,sign
      save idvecbos,vdecaymode,chargelept,neutralept
      real * 8 Edec,pdec
      logical newevent

      if (ini) then
         idvecbos=wbb_powheginput('idvecbos')
         vdecaymode=wbb_powheginput('vdecaymode')
         if (idvecbos.eq.24) then
c     W+ production
            sign=+1
         else
c     W- production            
            sign=-1
         endif
         if (vdecaymode.eq.0) then
c     decay into electron
            chargelept = -11*sign
            neutralept =  12*sign 
         elseif (vdecaymode.eq.1) then
c     decay into electron
            chargelept = -11*sign
            neutralept =  12*sign 
         elseif (vdecaymode.eq.2) then
c     decay into muon
            chargelept = -13*sign
            neutralept =  14*sign 
         elseif (vdecaymode.eq.3) then
c     decay into tau
            chargelept = -15*sign
            neutralept =  16*sign 
         endif
         ini=.false.
      endif
      newevent=.true.

c      write(*,*) 'begin V decay'      
      iii=0

      do i=1,nup
         call wbb_LH_pwhg_mom(pup(1,i),p(0,i))
      enddo

c     go to CM rest frame                  
      do i=1,3
         beta_L_CM(i) = -(p(i,1)+p(i,2))/(p(0,1)+p(0,2))
      enddo  
      betamod_L_CM=sqrt(beta_L_CM(1)**2+beta_L_CM(2)**2+beta_L_CM(3)**2)
      do i=1,3
         beta_L_CM(i) = beta_L_CM(i)/betamod_L_CM
      enddo      
      call wbb_mboost(nup,beta_L_CM,betamod_L_CM,p,p)

      call wbb_LH_pwhg_flav(flav)
c      write(*,*) flav
c      if (nup.eq.5)  then
c         write(*,*) flav
c         write(*,*) nup
c      endif

c     go to V rest frame: it is the 3rd momentum now                  
      do i=1,3
         beta(i) = -p(i,3)/p(0,3)
      enddo
      call wbb_pwhg_boost(beta,p(0,3),pw_rest)
c     
      pwvirt2 = pw_rest(0)**2-pw_rest(1)**2-pw_rest(2)**2-pw_rest(3)**2
c     set the W mass to its virtuality
c      mW2 = pwvirt2
      mW2 = ph_Wmass2
c     massless charged lepton 
c      upperf = pwvirt2/((pwvirt2-mW2)**2+ (ph_Wwidth)**2*mw2)
c     massive charged lepton 
c      upperf=(2*pwvirt2 + ph_lmass**2)*(pwvirt2 - ph_lmass**2)/pwvirt2/
c     $     ((pwvirt2-mW2)**2+ (ph_Wwidth)**2*mw2)
            
      upperf=(2*pwvirt2 + ph_lmass**2)/
     $     ((pwvirt2-mW2)**2+ (ph_Wwidth)**2*mw2)

c     assign momenta for decay process
      call wbb_assign_mom(p(0,1),   p_dec(0,1))
      call wbb_assign_mom(p(0,2),   p_dec(0,2))
      call wbb_assign_mom(p(0,4),   p_dec(0,5))
      call wbb_assign_mom(p(0,5),   p_dec(0,6))
c     assign flavors for decay process
      flav_dec(1)=flav(1)
      flav_dec(2)=flav(2)
      flav_dec(3)=chargelept
      flav_dec(4)=neutralept
      flav_dec(5)=flav(4)
      flav_dec(6)=flav(5)
            
c     boost back in the CM frame
      do i=1,3
         beta(i) =-beta(i) 
      enddo

      if (nup.eq.5) then 
         call wbb_setborn(p,flav,born_pwhg,bornjk,bmunu)
      elseif (nup.eq.6) then
c     the following will be used later by wbb_setreal_Wmdecay
         call wbb_assign_mom(p(0,6),p_dec(0,7))
         flav_dec(7)=flav(6)
         call wbb_setreal(p,flav,amp2_pwhg)
      else
         write(*,*) 'INVALID nup. Should be 5 or 6. nup = ',nup
         call exit(1)
      endif      

c     start generation of decay variables
 1    continue
c      generate flat cos(th) and phi
      costh=(1-2*wbb_random())
      phi=2*pi*wbb_random()
c
      sinth = sqrt(abs(1-costh**2))
      Edec = pw_rest(0)
      pdec = (Edec**2 - ph_lmass**2)/(2*Edec)      
c     charged lepton momentum
      vdec_rest(0,1)=sqrt(pdec**2 + ph_lmass**2)
      vdec_rest(1,1)=pdec*sinth*cos(phi)
      vdec_rest(2,1)=pdec*sinth*sin(phi)
      vdec_rest(3,1)=pdec*costh
c     neutral lepton momentum
      vdec_rest(0,2)=pdec
      do i=1,3
         vdec_rest(i,2)=-vdec_rest(i,1)      
      enddo
      call wbb_pwhg_boost(beta,vdec_rest(0,1),vdec(0,1))
      call wbb_pwhg_boost(beta,vdec_rest(0,2),vdec(0,2))

c     assign momenta for decay process
      call wbb_assign_mom(vdec(0,1),p_dec(0,3))
      call wbb_assign_mom(vdec(0,2),p_dec(0,4))

      if (nup.eq.5) then 
         if (idvecbos.eq.24) then
            call wbb_setborn_Wpdecay(p_dec,flav_dec,born_dec)
         else
            call wbb_setborn_Wmdecay(p_dec,flav_dec,born_dec)
         endif
         ratio = born_dec/born_pwhg
c     write(*,*) ratio
      elseif (nup.eq.6) then
         if (idvecbos.eq.24) then
            call wbb_setreal_Wpdecay(p_dec,flav_dec,amp2_dec)
         else
            call wbb_setreal_Wmdecay(p_dec,flav_dec,amp2_dec)
         endif
         ratio=amp2_dec/amp2_pwhg
c     write(*,*) ratio
      else
         write(*,*) 'INVALID nup. Should be 5 or 6. nup = ',nup
         call exit(1)
      endif      
      rat = ratio/upperf
c      write(*,*) rat

      countmax = 100
      if (count.le.countmax.and.newevent) then
         nubound=wbb_powheginput('nubound')
c     use as number of trial points the minimum between nubound and  3000
         nubound=min(nubound,3000)
         iii=iii+1
c         write(*,*) iii
         boundnorm=max(boundnorm,rat)
         if(iii.eq.nubound) goto 3
         goto 1
 3       continue
         if (count.eq.2) then
            write(*,*) 'LOOP over the first ',countmax,' events'
         endif
         write(*,*) count,'/',countmax,' W decay upper bound = ',
     $        boundnorm
         count=count+1
         newevent=.false.
c         ini=.false.
      endif
      
      if(rat.gt.boundnorm) then
         call  wbb_increasecnt('W decay upper bound violations')
         if (verbose) then
            write(*,*) 'W decay upper bound violation: rat/bound= ',
     $           rat/boundnorm, ' mw=',sqrt(mW2), 
     $           ' sqrt(p2)=',sqrt(pwvirt2)
            write(*,*) 'rad vars ',kn_csi, kn_y
            write(*,*) 'New decay upper bound is = ',rat
         endif         
         boundnorm=rat
      endif

      if(boundnorm*wbb_random().gt.rat) then
         call wbb_increasecnt('W decay vetoed configurations')
         goto 1
      endif
c     End Hit and Miss loop
c      write(*,*) 'have decay '

c     now we have the momenta of the decay products in the CM frame.
c     go back to the lab frame
      do i=1,3
         beta_CM_L(i) = -beta_L_CM(i)
      enddo  
      call wbb_mboost(2,beta_CM_L,betamod_L_CM,vdec(0,1),p_dec(0,3))      
c     SAVE info in the LH common block
c     change status of the vector boson
      istup(3)=+2
c     add vector-boson decay products
c     add charged lepton
      nup=nup+1
      idup(nup)=chargelept
      istup(nup)=+1
      mothup(1,nup)=3
      mothup(2,nup)=3
      icolup(1,nup)=0
      icolup(2,nup)=0
      spinup(nup)=9
      vtimup(nup)=0
      do mu=1,3
         pup(mu,nup)=p_dec(mu,3)
      enddo
      pup(4,nup)=p_dec(0,3)
      pup(5,nup)=ph_lmass       ! massive fermion

c     add neutral lepton
      nup=nup+1
      idup(nup)=neutralept
      istup(nup)=+1
      mothup(1,nup)=3
      mothup(2,nup)=3
      icolup(1,nup)=0
      icolup(2,nup)=0
      spinup(nup)=9
      vtimup(nup)=0
      do mu=1,3
         pup(mu,nup)=p_dec(mu,4)
      enddo
      pup(4,nup)=p_dec(0,4)
      pup(5,nup)=0d0            ! massless fermion

      end
c
      SUBROUTINE wbb_SUBD_WMBBB(P1,ANS)
C  
C Generated by MadGraph II                                              
C MadGraph StandAlone Version
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR PROCESS : u~ d -> w- b b~  
C  
C Crossing   1 is u~ d -> w- b b~  
      IMPLICIT NONE
C  
C wbb_constants
C  
      include "nexternal_2to3.inc"
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB=  48, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 wbb_UBD_WMBBB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      DATA NTRY/0/
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1, 5) /-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1, 5) /-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1, 5) /-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1, 5) /-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1, 5) /-1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1, 5) /-1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1, 5) /-1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1, 5) /-1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1, 5) /-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1, 5) /-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1, 5) /-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1, 5) /-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1, 5) /-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1, 5) /-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1, 5) /-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1, 5) /-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1, 5) /-1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1, 5) /-1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1, 5) /-1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1, 5) /-1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1, 5) /-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1, 5) /-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1, 5) /-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1, 5) /-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1, 5) / 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1, 5) / 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1, 5) / 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1, 5) / 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1, 5) / 1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1, 5) / 1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1, 5) / 1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1, 5) / 1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1, 5) / 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1, 5) / 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1, 5) / 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1, 5) / 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1, 5) / 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1, 5) / 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1, 5) / 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1, 5) / 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1, 5) / 1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1, 5) / 1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1, 5) / 1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1, 5) / 1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1, 5) / 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1, 5) / 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1, 5) / 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1, 5) / 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1, 5) / 1, 2, 3, 4, 5/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL wbb_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
      ANS(IPROC) = 0D0
          DO IHEL=1,NCOMB
             IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=wbb_UBD_WMBBB(P ,NHEL(1,IHEL),JC(1))            
               ANS(IPROC)=ANS(IPROC)+T
               IF (T .NE. 0D0 .AND. .NOT.    GOODHEL(IHEL,IPROC)) THEN
                   GOODHEL(IHEL,IPROC)=.TRUE.
               ENDIF
             ENDIF
          ENDDO
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
      REAL*8 FUNCTION wbb_UBD_WMBBB(P,NHEL,IC)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR PROCESS : u~ d -> w- b b~  
C  
      IMPLICIT NONE
C  
C wbb_constants
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   2,NEIGEN=  1) 
      include "nexternal_2to3.inc"
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=   9, NCOLOR=   1) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(18,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
      include "coupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            1/                                       
      DATA (CF(i,1  ),i=1  ,1  ) /     2/                                  
C               T[ 4, 2]T[ 1, 5]                                           
C ----------
C BEGIN CODE
C ----------
      CALL wbb_oxxxxx(P(0,1   ),ZERO ,NHEL(1   ),-1*IC(1   ),W(1,1   ))        
      CALL wbb_ixxxxx(P(0,2   ),ZERO ,NHEL(2   ),+1*IC(2   ),W(1,2   ))        
      CALL wbb_vxxxxx(P(0,3   ),WMASS ,NHEL(3   ),+1*IC(3   ),W(1,3   ))       
      CALL wbb_oxxxxx(P(0,4   ),BMASS ,NHEL(4   ),+1*IC(4   ),W(1,4   ))       
      CALL wbb_ixxxxx(P(0,5   ),BMASS ,NHEL(5   ),-1*IC(5   ),W(1,5   ))       
      CALL wbb_fvixxx(W(1,2   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,6   ))    
      CALL wbb_jioxxx(W(1,6   ),W(1,1   ),GG ,ZERO    ,ZERO    ,W(1,7   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,7   ),GG ,AMP(1   ))             
      CALL wbb_fvoxxx(W(1,1   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,8   ))    
      CALL wbb_jioxxx(W(1,2   ),W(1,8   ),GG ,ZERO    ,ZERO    ,W(1,9   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,9   ),GG ,AMP(2   ))             
      JAMP(   1) = +AMP(   1)+AMP(   2)
      wbb_UBD_WMBBB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          wbb_UBD_WMBBB =wbb_UBD_WMBBB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
      SUBROUTINE wbb_SDUB_WMBBB(P1,ANS)
C  
C Generated by MadGraph II                                              
C MadGraph StandAlone Version
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR PROCESS : d u~ -> w- b b~  
C  
C Crossing   1 is d u~ -> w- b b~  
      IMPLICIT NONE
C  
C wbb_constants
C  
      include "nexternal_2to3.inc"
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB=  48, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 wbb_DUB_WMBBB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      DATA NTRY/0/
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1, 5) /-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1, 5) /-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1, 5) /-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1, 5) /-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1, 5) /-1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1, 5) /-1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1, 5) /-1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1, 5) /-1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1, 5) /-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1, 5) /-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1, 5) /-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1, 5) /-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1, 5) /-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1, 5) /-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1, 5) /-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1, 5) /-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1, 5) /-1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1, 5) /-1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1, 5) /-1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1, 5) /-1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1, 5) /-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1, 5) /-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1, 5) /-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1, 5) /-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1, 5) / 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1, 5) / 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1, 5) / 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1, 5) / 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1, 5) / 1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1, 5) / 1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1, 5) / 1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1, 5) / 1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1, 5) / 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1, 5) / 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1, 5) / 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1, 5) / 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1, 5) / 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1, 5) / 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1, 5) / 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1, 5) / 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1, 5) / 1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1, 5) / 1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1, 5) / 1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1, 5) / 1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1, 5) / 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1, 5) / 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1, 5) / 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1, 5) / 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1, 5) / 1, 2, 3, 4, 5/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL wbb_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
      ANS(IPROC) = 0D0
          DO IHEL=1,NCOMB
             IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=wbb_DUB_WMBBB(P ,NHEL(1,IHEL),JC(1))            
               ANS(IPROC)=ANS(IPROC)+T
               IF (T .NE. 0D0 .AND. .NOT.    GOODHEL(IHEL,IPROC)) THEN
                   GOODHEL(IHEL,IPROC)=.TRUE.
               ENDIF
             ENDIF
          ENDDO
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
      REAL*8 FUNCTION wbb_DUB_WMBBB(P,NHEL,IC)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR PROCESS : d u~ -> w- b b~  
C  
      IMPLICIT NONE
C  
C wbb_constants
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   2,NEIGEN=  1) 
      include "nexternal_2to3.inc"
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=   9, NCOLOR=   1) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(18,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
      include "coupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            1/                                       
      DATA (CF(i,1  ),i=1  ,1  ) /     2/                                  
C               T[ 4, 1]T[ 2, 5]                                           
C ----------
C BEGIN CODE
C ----------
      CALL wbb_ixxxxx(P(0,1   ),ZERO ,NHEL(1   ),+1*IC(1   ),W(1,1   ))        
      CALL wbb_oxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL wbb_vxxxxx(P(0,3   ),WMASS ,NHEL(3   ),+1*IC(3   ),W(1,3   ))       
      CALL wbb_oxxxxx(P(0,4   ),BMASS ,NHEL(4   ),+1*IC(4   ),W(1,4   ))       
      CALL wbb_ixxxxx(P(0,5   ),BMASS ,NHEL(5   ),-1*IC(5   ),W(1,5   ))       
      CALL wbb_fvoxxx(W(1,2   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,6   ))    
      CALL wbb_jioxxx(W(1,1   ),W(1,6   ),GG ,ZERO    ,ZERO    ,W(1,7   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,7   ),GG ,AMP(1   ))             
      CALL wbb_fvixxx(W(1,1   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,8   ))    
      CALL wbb_jioxxx(W(1,8   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,9   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,9   ),GG ,AMP(2   ))             
      JAMP(   1) = -AMP(   1)-AMP(   2)
      wbb_DUB_WMBBB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          wbb_DUB_WMBBB =wbb_DUB_WMBBB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
      SUBROUTINE wbb_SUDB_WPBBB(P1,ANS)
C  
C Generated by MadGraph II                                              
C MadGraph StandAlone Version
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR PROCESS : u d~ -> w+ b b~  
C  
C Crossing   1 is u d~ -> w+ b b~  
      IMPLICIT NONE
C  
C wbb_constants
C  
      include "nexternal_2to3.inc"
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB=  48, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 wbb_UDB_WPBBB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      DATA NTRY/0/
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1, 5) /-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1, 5) /-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1, 5) /-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1, 5) /-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1, 5) /-1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1, 5) /-1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1, 5) /-1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1, 5) /-1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1, 5) /-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1, 5) /-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1, 5) /-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1, 5) /-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1, 5) /-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1, 5) /-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1, 5) /-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1, 5) /-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1, 5) /-1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1, 5) /-1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1, 5) /-1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1, 5) /-1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1, 5) /-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1, 5) /-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1, 5) /-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1, 5) /-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1, 5) / 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1, 5) / 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1, 5) / 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1, 5) / 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1, 5) / 1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1, 5) / 1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1, 5) / 1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1, 5) / 1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1, 5) / 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1, 5) / 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1, 5) / 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1, 5) / 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1, 5) / 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1, 5) / 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1, 5) / 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1, 5) / 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1, 5) / 1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1, 5) / 1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1, 5) / 1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1, 5) / 1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1, 5) / 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1, 5) / 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1, 5) / 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1, 5) / 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1, 5) / 1, 2, 3, 4, 5/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL wbb_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
      ANS(IPROC) = 0D0
          DO IHEL=1,NCOMB
             IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=wbb_UDB_WPBBB(P ,NHEL(1,IHEL),JC(1))            
               ANS(IPROC)=ANS(IPROC)+T
               IF (T .NE. 0D0 .AND. .NOT.    GOODHEL(IHEL,IPROC)) THEN
                   GOODHEL(IHEL,IPROC)=.TRUE.
               ENDIF
             ENDIF
          ENDDO
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
      REAL*8 FUNCTION wbb_UDB_WPBBB(P,NHEL,IC)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR PROCESS : u d~ -> w+ b b~  
C  
      IMPLICIT NONE
C  
C wbb_constants
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   2,NEIGEN=  1) 
      include "nexternal_2to3.inc"
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=   9, NCOLOR=   1) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(18,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
      include "coupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            1/                                       
      DATA (CF(i,1  ),i=1  ,1  ) /     2/                                  
C               T[ 4, 1]T[ 2, 5]                                           
C ----------
C BEGIN CODE
C ----------
      CALL wbb_ixxxxx(P(0,1   ),ZERO ,NHEL(1   ),+1*IC(1   ),W(1,1   ))        
      CALL wbb_oxxxxx(P(0,2   ),ZERO ,NHEL(2   ),-1*IC(2   ),W(1,2   ))        
      CALL wbb_vxxxxx(P(0,3   ),WMASS ,NHEL(3   ),+1*IC(3   ),W(1,3   ))       
      CALL wbb_oxxxxx(P(0,4   ),BMASS ,NHEL(4   ),+1*IC(4   ),W(1,4   ))       
      CALL wbb_ixxxxx(P(0,5   ),BMASS ,NHEL(5   ),-1*IC(5   ),W(1,5   ))       
      CALL wbb_fvoxxx(W(1,2   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,6   ))    
      CALL wbb_jioxxx(W(1,1   ),W(1,6   ),GG ,ZERO    ,ZERO    ,W(1,7   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,7   ),GG ,AMP(1   ))             
      CALL wbb_fvixxx(W(1,1   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,8   ))    
      CALL wbb_jioxxx(W(1,8   ),W(1,2   ),GG ,ZERO    ,ZERO    ,W(1,9   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,9   ),GG ,AMP(2   ))             
      JAMP(   1) = -AMP(   1)-AMP(   2)
      wbb_UDB_WPBBB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          wbb_UDB_WPBBB=wbb_UDB_WPBBB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
c
      SUBROUTINE wbb_SDBU_WPBBB(P1,ANS)
C  
C Generated by MadGraph II                                              
C MadGraph StandAlone Version
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C AND HELICITIES
C FOR THE POINT IN PHASE SPACE P(0:3,NEXTERNAL)
C  
C FOR PROCESS : d~ u -> w+ b b~  
C  
C Crossing   1 is d~ u -> w+ b b~  
      IMPLICIT NONE
C  
C wbb_constants
C  
      include "nexternal_2to3.inc"
      INTEGER                 NCOMB,     NCROSS         
      PARAMETER (             NCOMB=  48, NCROSS=  1)
      INTEGER    THEL
      PARAMETER (THEL=NCOMB*NCROSS)
C  
C ARGUMENTS 
C  
      REAL*8 P1(0:3,NEXTERNAL),ANS(NCROSS)
C  
C LOCAL VARIABLES 
C  
      INTEGER NHEL(NEXTERNAL,NCOMB),NTRY
      REAL*8 T, P(0:3,NEXTERNAL)
      REAL*8 wbb_DBU_WPBBB
      INTEGER IHEL,IDEN(NCROSS),IC(NEXTERNAL,NCROSS)
      INTEGER IPROC,JC(NEXTERNAL), I
      LOGICAL GOODHEL(NCOMB,NCROSS)
      DATA NTRY/0/
      DATA GOODHEL/THEL*.FALSE./
      DATA (NHEL(IHEL,   1),IHEL=1, 5) /-1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,   2),IHEL=1, 5) /-1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,   3),IHEL=1, 5) /-1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,   4),IHEL=1, 5) /-1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,   5),IHEL=1, 5) /-1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,   6),IHEL=1, 5) /-1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,   7),IHEL=1, 5) /-1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,   8),IHEL=1, 5) /-1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,   9),IHEL=1, 5) /-1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  10),IHEL=1, 5) /-1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  11),IHEL=1, 5) /-1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  12),IHEL=1, 5) /-1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  13),IHEL=1, 5) /-1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  14),IHEL=1, 5) /-1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  15),IHEL=1, 5) /-1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  16),IHEL=1, 5) /-1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  17),IHEL=1, 5) /-1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  18),IHEL=1, 5) /-1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  19),IHEL=1, 5) /-1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  20),IHEL=1, 5) /-1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  21),IHEL=1, 5) /-1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  22),IHEL=1, 5) /-1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  23),IHEL=1, 5) /-1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  24),IHEL=1, 5) /-1, 1, 1, 1, 1/
      DATA (NHEL(IHEL,  25),IHEL=1, 5) / 1,-1,-1,-1,-1/
      DATA (NHEL(IHEL,  26),IHEL=1, 5) / 1,-1,-1,-1, 1/
      DATA (NHEL(IHEL,  27),IHEL=1, 5) / 1,-1,-1, 1,-1/
      DATA (NHEL(IHEL,  28),IHEL=1, 5) / 1,-1,-1, 1, 1/
      DATA (NHEL(IHEL,  29),IHEL=1, 5) / 1,-1, 0,-1,-1/
      DATA (NHEL(IHEL,  30),IHEL=1, 5) / 1,-1, 0,-1, 1/
      DATA (NHEL(IHEL,  31),IHEL=1, 5) / 1,-1, 0, 1,-1/
      DATA (NHEL(IHEL,  32),IHEL=1, 5) / 1,-1, 0, 1, 1/
      DATA (NHEL(IHEL,  33),IHEL=1, 5) / 1,-1, 1,-1,-1/
      DATA (NHEL(IHEL,  34),IHEL=1, 5) / 1,-1, 1,-1, 1/
      DATA (NHEL(IHEL,  35),IHEL=1, 5) / 1,-1, 1, 1,-1/
      DATA (NHEL(IHEL,  36),IHEL=1, 5) / 1,-1, 1, 1, 1/
      DATA (NHEL(IHEL,  37),IHEL=1, 5) / 1, 1,-1,-1,-1/
      DATA (NHEL(IHEL,  38),IHEL=1, 5) / 1, 1,-1,-1, 1/
      DATA (NHEL(IHEL,  39),IHEL=1, 5) / 1, 1,-1, 1,-1/
      DATA (NHEL(IHEL,  40),IHEL=1, 5) / 1, 1,-1, 1, 1/
      DATA (NHEL(IHEL,  41),IHEL=1, 5) / 1, 1, 0,-1,-1/
      DATA (NHEL(IHEL,  42),IHEL=1, 5) / 1, 1, 0,-1, 1/
      DATA (NHEL(IHEL,  43),IHEL=1, 5) / 1, 1, 0, 1,-1/
      DATA (NHEL(IHEL,  44),IHEL=1, 5) / 1, 1, 0, 1, 1/
      DATA (NHEL(IHEL,  45),IHEL=1, 5) / 1, 1, 1,-1,-1/
      DATA (NHEL(IHEL,  46),IHEL=1, 5) / 1, 1, 1,-1, 1/
      DATA (NHEL(IHEL,  47),IHEL=1, 5) / 1, 1, 1, 1,-1/
      DATA (NHEL(IHEL,  48),IHEL=1, 5) / 1, 1, 1, 1, 1/
      DATA (  IC(IHEL,  1),IHEL=1, 5) / 1, 2, 3, 4, 5/
      DATA (IDEN(IHEL),IHEL=  1,  1) /  36/
C ----------
C BEGIN CODE
C ----------
      NTRY=NTRY+1
      DO IPROC=1,NCROSS
      CALL wbb_switchmom(P1,P,IC(1,IPROC),JC,NEXTERNAL)
      DO IHEL=1,NEXTERNAL
         JC(IHEL) = +1
      ENDDO
      ANS(IPROC) = 0D0
          DO IHEL=1,NCOMB
             IF (GOODHEL(IHEL,IPROC) .OR. NTRY .LT. 2) THEN
                 T=wbb_DBU_WPBBB(P ,NHEL(1,IHEL),JC(1))            
               ANS(IPROC)=ANS(IPROC)+T
               IF (T .NE. 0D0 .AND. .NOT.    GOODHEL(IHEL,IPROC)) THEN
                   GOODHEL(IHEL,IPROC)=.TRUE.
               ENDIF
             ENDIF
          ENDDO
      ANS(IPROC)=ANS(IPROC)/DBLE(IDEN(IPROC))
      ENDDO
      END
       
       
      REAL*8 FUNCTION wbb_DBU_WPBBB(P,NHEL,IC)
C  
C Generated by MadGraph II                                              
C RETURNS AMPLITUDE SQUARED SUMMED/AVG OVER COLORS
C FOR THE POINT WITH EXTERNAL LINES W(0:6,NEXTERNAL)
C  
C FOR PROCESS : d~ u -> w+ b b~  
C  
      IMPLICIT NONE
C  
C wbb_constants
C  
      INTEGER    NGRAPHS,    NEIGEN 
      PARAMETER (NGRAPHS=   2,NEIGEN=  1) 
      include "nexternal_2to3.inc"
      INTEGER    NWAVEFUNCS     , NCOLOR
      PARAMETER (NWAVEFUNCS=   9, NCOLOR=   1) 
      REAL*8     ZERO
      PARAMETER (ZERO=0D0)
C  
C ARGUMENTS 
C  
      REAL*8 P(0:3,NEXTERNAL)
      INTEGER NHEL(NEXTERNAL), IC(NEXTERNAL)
C  
C LOCAL VARIABLES 
C  
      INTEGER I,J
      COMPLEX*16 ZTEMP
      REAL*8 DENOM(NCOLOR), CF(NCOLOR,NCOLOR)
      COMPLEX*16 AMP(NGRAPHS), JAMP(NCOLOR)
      COMPLEX*16 W(18,NWAVEFUNCS)
C  
C GLOBAL VARIABLES
C  
      include "coupl.inc"
C  
C COLOR DATA
C  
      DATA Denom(1  )/            1/                                       
      DATA (CF(i,1  ),i=1  ,1  ) /     2/                                  
C               T[ 4, 2]T[ 1, 5]                                           
C ----------
C BEGIN CODE
C ----------
      CALL wbb_oxxxxx(P(0,1   ),ZERO ,NHEL(1   ),-1*IC(1   ),W(1,1   ))        
      CALL wbb_ixxxxx(P(0,2   ),ZERO ,NHEL(2   ),+1*IC(2   ),W(1,2   ))        
      CALL wbb_vxxxxx(P(0,3   ),WMASS ,NHEL(3   ),+1*IC(3   ),W(1,3   ))       
      CALL wbb_oxxxxx(P(0,4   ),BMASS ,NHEL(4   ),+1*IC(4   ),W(1,4   ))       
      CALL wbb_ixxxxx(P(0,5   ),BMASS ,NHEL(5   ),-1*IC(5   ),W(1,5   ))       
      CALL wbb_fvixxx(W(1,2   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,6   ))    
      CALL wbb_jioxxx(W(1,6   ),W(1,1   ),GG ,ZERO    ,ZERO    ,W(1,7   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,7   ),GG ,AMP(1   ))             
      CALL wbb_fvoxxx(W(1,1   ),W(1,3   ),GWF ,ZERO    ,ZERO    ,W(1,8   ))    
      CALL wbb_jioxxx(W(1,2   ),W(1,8   ),GG ,ZERO    ,ZERO    ,W(1,9   ))     
      CALL wbb_iovxxx(W(1,5   ),W(1,4   ),W(1,9   ),GG ,AMP(2   ))             
      JAMP(   1) = +AMP(   1)+AMP(   2)
      wbb_DBU_WPBBB = 0.D0 
      DO I = 1, NCOLOR
          ZTEMP = (0.D0,0.D0)
          DO J = 1, NCOLOR
              ZTEMP = ZTEMP + CF(J,I)*JAMP(J)
          ENDDO
          wbb_DBU_WPBBB =wbb_DBU_WPBBB+ZTEMP*DCONJG(JAMP(I))/DENOM(I)   
      ENDDO
C      CALL GAUGECHECK(JAMP,ZTEMP,EIGEN_VEC,EIGEN_VAL,NCOLOR,NEIGEN) 
      END
       



