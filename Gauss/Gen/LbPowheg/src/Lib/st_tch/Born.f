cccccccccccccccccccccccccccccccccccccc
c     !: This version of the decay generation
c     does not include any particular treatment
c     for POWHEG-generated partonic kinematics from
c     subprocesses where the FKS parton is the 4th and
c     not the 5th one (this can happen for real processes
c     of type gd->** or dg->**, since both these have
c     2 FKS ISR regions, associated to the 4th or the
c     5th parton being collinear).
c     All (I hope) critical lines marked by !:!.
c     In particular, the correct handling should be already
c     possible in the subroutine st_tch_put_on_mass_shell
c     (see the !:!:! marks).
cccccccccccccccccccccccccccccccccccccc

      subroutine st_tch_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/PhysPars.h'
ccccccccccccccccc
      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
ccccccccccccccccc 
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),bbmunu(0:3,0:3),born,colcf
      real *8 borntmp
      integer j,k,mu,nu

      integer bflav_loc(nlegs)


ccccccccccccccccccccccccccccccccccccccc
c     charge conjugation
c     If ttype=-1, then bflav has been filled with tbar-production flavours.
c     Subroutines here work for t-production flavour assignment.
c     Therefore, invert the sign of local flavours.
      do j=1,nlegs
         bflav_loc(j)= ttype *bflav(j)
      enddo
ccccccccccccccccccccccccccccccccccccccc


      call st_tch_compborn(p,bflav_loc,borntmp,bbmunu)
      born=borntmp
      do j=1,nlegborn
         if(abs(bflav_loc(j)).le.6) then
            do mu=0,3
               do nu=0,3
                  bmunu(mu,nu,j)=bbmunu(mu,nu)
               enddo
            enddo
            if(bflav_loc(j).eq.0) then
               write(*,*) 'born with a gluon leg'
               write(*,*) 'Error: not present for this process'
               call exit(1)
            endif
c Colour factors for colour-correlated Born amplitudes;
c Rule from 2.98 in FNO2007, leads to B_ij=Cj * B,
c where i#j
            do k=j+1,nlegborn
               colcf=0d0
               if(abs(bflav_loc(k)).le.6) then
c     ddx
                  if ((three_ch(bflav_loc(1)).eq.-1).and.
     #(three_ch(bflav_loc(2)).eq.1)) then
c     13 24 = CF
                     if(j.eq.1.and.k.eq.3) colcf=cf
                     if(j.eq.2.and.k.eq.4) colcf=cf
                     
                     
c     dxd
                  elseif ((three_ch(bflav_loc(1)).eq.1).and.
     #(three_ch(bflav_loc(2)).eq.-1)) then
c     14 23 = CF
                     if(j.eq.1.and.k.eq.4) colcf=cf
                     if(j.eq.2.and.k.eq.3) colcf=cf
                     
                     
                     
c     ud
                  elseif ((three_ch(bflav_loc(1)).eq.2).and.
     #(three_ch(bflav_loc(2)).eq.-1)) then
c     14 23 = CF
                     if(j.eq.1.and.k.eq.4) colcf=cf
                     if(j.eq.2.and.k.eq.3) colcf=cf
                     
                     
c     du
                  elseif ((three_ch(bflav_loc(1)).eq.-1).and.
     #(three_ch(bflav_loc(2)).eq.2)) then
c     13 24 = CF
                     if(j.eq.1.and.k.eq.3) colcf=cf
                     if(j.eq.2.and.k.eq.4) colcf=cf
                     
                     
                  endif
                  bornjk(j,k)=born*colcf
                  bornjk(k,j)=bornjk(j,k)
               endif
            enddo
         endif
      enddo
      end


      subroutine st_tch_compborn(p,bflav,born,bmunu)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/pwhg_flst.h'
      include 'src/Lib/st_tch/PhysPars.h'
      integer nleg
      parameter (nleg=nlegborn)
      real * 8 p(0:3,nleg)
      integer bflav(nleg)
      real * 8 amp2,born,bmunu(0:3,0:3)
      integer i,j

cccccccccccccccccc
      integer mu,ileg
      real *8 kblab(0:3,nleg)
      real *8 ewcoupl,s,t,u
      real *8 amp2_ddx,amp2_dxd,amp2_ud,amp2_du
      real *8 st_tch_dotp
      external st_tch_dotp

      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/

      real *8 ckm_b_t
cccccccccccccccc

c     check
      if (bflav(3).ne.6) then
         write(*,*) 'born_ampsq: ERROR in flavor assignement'
         write(*,*) 'to be called only for top, not tbar'
         call exit(1)
      endif

c     local copy of variables
      do ileg=1,nleg
         do mu=0,3
            kblab(mu,ileg)=p(mu,ileg)
         enddo
      enddo

c     ew coupling
      ewcoupl=4d0*pi*alphaem_pow/sthw2_pow

c     Mandelstam variables
      s=2d0*st_tch_dotp(kblab(0,1),kblab(0,2))
      t=st_tch_dotp(kblab(0,3),kblab(0,3)) - 2d0*st_tch_dotp(kblab(0,1),kblab(0,3)) 
      u=st_tch_dotp(kblab(0,3),kblab(0,3))-s-t
      
ccccccccccccccccccccccccccccccccccccccccccc
c     >>> T CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc
      amp2_ddx=u*(u-topmass_pow**2)/(t-wmass_pow**2)**2/4
      amp2_dxd=t*(t-topmass_pow**2)/(u-wmass_pow**2)**2/4
      amp2_ud= s*(s-topmass_pow**2)/(u-wmass_pow**2)**2/4
      amp2_du= s*(s-topmass_pow**2)/(t-wmass_pow**2)**2/4


c     Fill born amp2
c     ddx
      if ((three_ch(bflav(1)).eq.-1).and.
     #(three_ch(bflav(2)).eq.1)) then
         amp2=amp2_ddx
         ckm_b_t=
     #CKM(abs(bflav(1)),abs(bflav(3)))**2 
     #* CKM(abs(bflav(2)),abs(bflav(4)))**2 

c     dxd
      elseif ((three_ch(bflav(1)).eq.1).and.
     #(three_ch(bflav(2)).eq.-1)) then
         amp2=amp2_dxd
         ckm_b_t=
     #CKM(abs(bflav(1)),abs(bflav(4)))**2 
     #* CKM(abs(bflav(2)),abs(bflav(3)))**2

c     ud
      elseif ((three_ch(bflav(1)).eq.2).and.
     #(three_ch(bflav(2)).eq.-1)) then
         amp2=amp2_ud
         ckm_b_t=
     #CKM(abs(bflav(1)),abs(bflav(4)))**2 
     #* CKM(abs(bflav(2)),abs(bflav(3)))**2

c     du
      elseif ((three_ch(bflav(1)).eq.-1).and.
     #(three_ch(bflav(2)).eq.2)) then
         amp2=amp2_du
         ckm_b_t=
     #CKM(abs(bflav(1)),abs(bflav(3)))**2 
     #* CKM(abs(bflav(2)),abs(bflav(4)))**2

      else
         write(*,*) 'Error in fill_born, (t)'
         call exit(1)
      endif


ccccccccccccccccccccccc
c     assign output
      born=amp2*ewcoupl**2 *ckm_b_t
      do i=0,3
         do j=0,3
            bmunu(i,j)=0d0
         enddo
      enddo
cccccccccccccccccccccc
      end


      subroutine st_tch_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      implicit none
      include 'src/Lib/st_tch/LesHouches.h'
      include 'src/Lib/st_tch/PhysPars.h'
      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/
      integer tbcol,udcol
      data tbcol/501/
      data udcol/502/
      integer ileg,tmp

c     There are 4 possible (equivalent) colour structures
c     for single-top t-channel Born processes.
c     ttype is needed to handle the charge-conjugated process

c     ddx
      if ((three_ch(ttype*idup(1)).eq.-1).and.
     #(three_ch(ttype*idup(2)).eq.1)) then
c     b  -->--W==>==  t
c             W
c             W
c             W
c     dx --<--W--<--  ux
         icolup(1,1)=tbcol
         icolup(2,1)=0
         icolup(1,2)=0
         icolup(2,2)=udcol
         icolup(1,3)=tbcol
         icolup(2,3)=0
         icolup(1,4)=0
         icolup(2,4)=udcol

c     dxd
      elseif ((three_ch(ttype*idup(1)).eq.1).and.
     #(three_ch(ttype*idup(2)).eq.-1)) then
c     dx --<--W\   ==  t
c             W \ /
c             W  /
c             W / \
c     b  -->--W/   --  ux
         icolup(1,1)=0
         icolup(2,1)=udcol
         icolup(1,2)=tbcol
         icolup(2,2)=0
         icolup(1,3)=tbcol
         icolup(2,3)=0
         icolup(1,4)=0
         icolup(2,4)=udcol

c     ud
      elseif ((three_ch(ttype*idup(1)).eq.2).and.
     #(three_ch(ttype*idup(2)).eq.-1)) then
c     u  -->--W\   ==  t
c             W \ /
c             W  /
c             W / \
c     b  -->--W/   --  d
         icolup(1,1)=udcol
         icolup(2,1)=0
         icolup(1,2)=tbcol
         icolup(2,2)=0
         icolup(1,3)=tbcol
         icolup(2,3)=0
         icolup(1,4)=udcol
         icolup(2,4)=0

c     du
      elseif ((three_ch(ttype*idup(1)).eq.-1).and.
     #(three_ch(ttype*idup(2)).eq.2)) then
c     b  -->--W==>==  t
c             W
c             W
c             W
c     u  -->--W-->--  d
         icolup(1,1)=tbcol
         icolup(2,1)=0
         icolup(1,2)=udcol
         icolup(2,2)=0
         icolup(1,3)=tbcol
         icolup(2,3)=0
         icolup(1,4)=udcol
         icolup(2,4)=0

      else
         write(*,*) 'Invalid flavour in st_tch_borncolour_lh'
         call exit(1)
      endif

c     if charge-conjugated process, exchange color
c     with anticolors
      if(ttype.lt.0) then
         do ileg=1,4
            tmp=icolup(1,ileg)
            icolup(1,ileg)=icolup(2,ileg)
            icolup(2,ileg)=tmp
         enddo
      endif

      end


      subroutine st_tch_finalize_lh
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
c     Before that, call the routine that generates the decay.
c     Notice that the current subroutine is called at the end
c     of st_tch_gen_leshouches (or st_tch_gen_leshouches_reg). This means
c     that the overall azimuthal rotation has been already
c     performed (st_tch_add_azimuth called in st_tch_pwhgevent). 
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/LesHouches.h'
      include 'src/Lib/st_tch/PhysPars.h'
      integer tdecaymode
      integer mu,ileg
      real *8 klab_dec(0:3,nlegreal+3),klab_os(0:3,nlegreal+3)
      real *8 st_tch_powheginput,st_tch_dotp
      external st_tch_powheginput,st_tch_dotp
      logical check_momcons
      parameter (check_momcons=.true.)
      real *8 toten,totpx,totpy,totpz
      logical tdecayflag
      real *8 kt(0:3)
      integer chflag
      common/st_tch_cchflag/chflag
      integer MC_mass
      real *8 mcmass(0:6)
      common/st_tch_cmcmass/mcmass
      integer ini
      data ini/0/
      save ini
c     decay products
      integer decids(1:3)
      common/st_tch_cdecids/decids
      integer nup_undec,iw,ie,ive
c     killev variables
      logical killev
      integer nkillev
      common/st_tch_ckillev/nkillev,killev
      
      tdecaymode=st_tch_powheginput('#topdecaymode')
      if(tdecaymode.lt.0) then
         write(*,*) 'Invalid value for tdecaymode'
         call exit(1)
      endif
         
c-------------------------------
c     >>> Add decay products <<<
c-------------------------------
c     st_tch_tdecay_offshell knows if the current event is a born or a real by means of nup
c     value in the LesHouches common block. To perform the decay, only massless
c     momenta must be used (no reshuffling) because there will be an evaluation
c     of Madgraph decayed matrix element that has to be done with the same
c     momenta used to calculate POWHEG matrix elements, where all particles
c     (apart the top) are massless. 

c     This is different wrt the POWHEG-hvq code, where a nonvanishing
c     bmass value was used.  Here I prefer to avoid this, because bmass
c     enters also in the 'undecayed part' of the Madgraph full matrix
c     element.
      call st_tch_tdecay_offshell(klab_dec)
c     In klab_dec there are the MASSLESS momenta of the decayed event,
c     with the following convention:
c     (1,...,nlegreal)   -> undecayed legs (3 is the top quark), 5 null if born event
c     nlegreal+1        -> d-type W-decay product (e,mu,tau,d,s)
c     nlegreal+2        -> u-type W-decay product (ve,vmu,vtau,u,c)
c     nlegreal+3        -> d-type t-decay product (d,s,b)

ccccccccccccccccccccccccccccccccccccccccccccc
c     Uncomment to check klab_dec
c$$$      toten=0
c$$$      totpx=0
c$$$      totpy=0
c$$$      totpz=0
c$$$      if(nup.eq.4) then
c$$$         write(*,*) 'Born-like'
c$$$         write(*,*) '----undec'
c$$$         do ileg=1,4
c$$$            write(*,*) pup(4,ileg), (pup(mu,ileg), mu=1,3)
c$$$         enddo
c$$$         write(*,*) '----dec'
c$$$         do ileg=1,2
c$$$            write(*,*) (klab_dec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(klab_dec(0,ileg),klab_dec(0,ileg))))
c$$$            toten =toten +klab_dec(0,ileg)
c$$$            totpx=totpx+klab_dec(1,ileg)
c$$$            totpy=totpy+klab_dec(2,ileg)
c$$$            totpz=totpz+klab_dec(3,ileg)
c$$$         enddo
c$$$         do ileg=4,8
c$$$            write(*,*) (klab_dec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(klab_dec(0,ileg),klab_dec(0,ileg))))
c$$$            toten =toten -klab_dec(0,ileg)
c$$$            totpx=totpx-klab_dec(1,ileg)
c$$$            totpy=totpy-klab_dec(2,ileg)
c$$$            totpz=totpz-klab_dec(3,ileg)
c$$$         enddo
c$$$         write(*,*) 'MUST BE ZERO',toten,totpx,totpy,totpz
c$$$      elseif(nup.eq.5) then
c$$$         write(*,*) 'real-like'
c$$$         write(*,*) '----undec'
c$$$         do ileg=1,5
c$$$            write(*,*) pup(4,ileg), (pup(mu,ileg), mu=1,3)
c$$$         enddo
c$$$         write(*,*) '----dec'
c$$$         do ileg=1,2
c$$$            write(*,*) (klab_dec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(klab_dec(0,ileg),klab_dec(0,ileg))))
c$$$            toten =toten +klab_dec(0,ileg)
c$$$            totpx=totpx+klab_dec(1,ileg)
c$$$            totpy=totpy+klab_dec(2,ileg)
c$$$            totpz=totpz+klab_dec(3,ileg)
c$$$         enddo
c$$$         do ileg=4,8
c$$$            write(*,*) (klab_dec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(klab_dec(0,ileg),klab_dec(0,ileg))))
c$$$            toten =toten -klab_dec(0,ileg)
c$$$            totpx=totpx-klab_dec(1,ileg)
c$$$            totpy=totpy-klab_dec(2,ileg)
c$$$            totpz=totpz-klab_dec(3,ileg)
c$$$         enddo
c$$$         write(*,*) 'MUST BE ZERO',toten,totpx,totpy,totpz
c$$$      endif
ccccccccccccccccccccccccccccccccccccccc

cccccccccccccccccccccccccccccccccccccccccccccccc
c     For the exact meaning of tdecayflag and MC_mass, see below
c     !: WARNING: tdecayflag=false HAS NOT BEEN 
c     tested as well as the others running modes.
      tdecayflag=tdecaymode.ne.0
      MC_mass=1
cccccccccccccccccccccccccccccccccccccccccccccccc

      chflag=2                  ! t-channel
      if(tdecayflag) then
         do mu=0,3
            kt(mu)=klab_dec(mu,6)+klab_dec(mu,7)+klab_dec(mu,8)
         enddo
      else
         do mu=0,3
            kt(mu)=klab_dec(mu,3)
         enddo
      endif
      mcmass(6) = sqrt(st_tch_dotp(kt,kt))
      mcmass(5) = st_tch_powheginput('#bottomthr')
      if (mcmass(5).lt.0d0) mcmass(5)=5.
      mcmass(4) = st_tch_powheginput('#charmthr')
      if (mcmass(4).lt.0d0) mcmass(4)=1.5
      mcmass(3) = 0d0
      mcmass(2) = 0d0
      mcmass(1) = 0d0
      mcmass(0) = 0d0
      call st_tch_put_on_mass_shell(tdecayflag,MC_mass,klab_dec,klab_os)

c     If tdecayflag=true, in klab_os there are the momenta of the decayed event.
c       If MC_mass is 1 (2), then the momenta have been put on the MC mass shell,
c       i.e. outgoing (all) particles will have the mass equal to the value assumed by the
c       MC that will shower the event. Otherwise they have been kept fixed.
c       The ordering convention is the same of klab_dec:
c       (1,...,nparton_r)  -> undecayed legs (3 is the top quark), 5 null if born event
c       nparton_r+1        -> d-type W-decay product (e,mu,tau,d,s)
c       nparton_r+2        -> u-type W-decay product (ve,vmu,vtau,u,c)
c       nparton_r+3        -> d-type t-decay product (d,s,b)

c     If tdecayflag=false, in klab_os there are the momenta of the undecayed event.
c       If MC_mass is 1 (2), then the momenta have been put on the MC mass shell,
c       i.e. outgoing (all) particles will have the mass equal to the value assumed by the
c       MC that will shower the event. Otherwise they have been kept fixed.
c       The convention is the same of krlab:
c       (1,...,nparton_r)  -> undecayed legs (3 is the top quark), 5 null if born event
c       >nparton_r         -> meaningless
cccccccccccccccccccccccccccccccccccccccccccc

cccccccccccccccccccccccccccccccccccccccccccccccccccc
c     !: WARNING: tdecayflag=false HAS NOT BEEN 
c     tested as well as the others running modes.
cccccccccccccccccccccccccccccccccccccccccccccccccccc


c     If klab_os(0,1)<0 then the massless kinematics was such that it was not
c     possible to perform the momenta reshuffling. In this case, throw away the
c     event.
      killev=.false.
      if(klab_os(0,1).lt.0d0) then
         write(*,*) 'KILLED AN EVENT'
         call st_tch_increasecnt('events killed: critical massless kinematic')
         killev=.true.
         nkillev=nkillev+1
         return
      endif

c     Now nup is still 4 or 5, depending on the original undecayed POWHEG event
c     From here on, only klab_os has to be used:
c     if tdecayflag=true, we have
c        KLAB_OS(1...nup) = undecayed momenta, top included, ON SHELL
c        KLAB_OS(nparton_r+1) = d-type W-decay product, ON SHELL
c        KLAB_OS(nparton_r+2) = u-type W-decay product, ON SHELL
c        KLAB_OS(nparton_r+3) = d-type t-decay product, ON SHELL
c     if tdecayflag=false
c        KLAB_OS(1...nup) = undecayed momenta, top included, ON SHELL

c     fill pup corresponding to the undecayed final state
      do ileg=1,nup
         pup(4,ileg)=klab_os(0,ileg)
         do mu=1,3
            pup(mu,ileg)=klab_os(mu,ileg)
         enddo
         pup(5,ileg)=sqrt(dabs(pup(4,ileg)**2-
     #pup(1,ileg)**2-pup(2,ileg)**2-pup(3,ileg)**2))
      enddo

      do ileg=1,nup
         spinup(ileg)=9
         vtimup(ileg)=0
      enddo

cccccccccccccccccccccccccccccccccccccccccccccccc
      if(check_momcons) then
c     momentum conservation check
         toten=0d0
         totpx=0d0
         totpy=0d0
         totpz=0d0
         do ileg=1,2
            toten=toten+pup(4,ileg)
            totpx=totpx+pup(1,ileg)
            totpy=totpy+pup(2,ileg)
            totpz=totpz+pup(3,ileg)
         enddo
         do ileg=3,nup
            toten=toten-pup(4,ileg)
            totpx=totpx-pup(1,ileg)
            totpy=totpy-pup(2,ileg)
            totpz=totpz-pup(3,ileg)
         enddo
      
         if((dabs(toten).gt.1d-5).or.(dabs(totpx).gt.1d-5).or.
     $        (dabs(totpy).gt.1d-5).or.(dabs(totpz).gt.1d-5)) then
            print*, '-------\n ERROR in PUP'
            if(nup.eq.4) then
               print*, 'BORNLIKE'
            else
               print*, 'REALLIKE'
            endif
            print*, '>>> ',toten,totpx,totpy,totpz
            do ileg=1,2
               print*, idup(ileg),pup(4,ileg),pup(1,ileg),pup(2,ileg),
     $              pup(3,ileg),
     $              '  mass',pup(5,ileg)
            enddo
            do ileg=3,nup
               print*, idup(ileg),pup(4,ileg),pup(1,ileg),pup(2,ileg),
     $              pup(3,ileg),
     $              '  mass',pup(5,ileg)
            enddo
         endif
      endif
cccccccccccccccccccccccccccccccccccccccccccccccc

c     save original nup before adding the decay
      nup_undec=nup

      if(tdecayflag) then
c     change top istup
         istup(3)=2
cccccccccccccccccccccccccccccccccccccccccccccccc
c     Add decay products on LesHouches interface
cccccccccccccccccccccccccccccccccccccccccccccccc
c     Decay pdg ids have been saved in decids(1:3) with convention (e,ve,b)

c     b
         nup=nup+1
         idup(nup)=decids(3) *ttype
         istup(nup)=1
c     color assignment safe also for tbar production, since
c     icolup(*,3) are already the correct ones
         icolup(1,nup)=icolup(1,3)
         icolup(2,nup)=icolup(2,3)
         mothup(1,nup)=3
         mothup(2,nup)=3
         pup(4,nup)=klab_os(0,nlegreal+3)
         do mu=1,3
            pup(mu,nup)=klab_os(mu,nlegreal+3)
         enddo
         pup(5,nup)=sqrt(dabs(pup(4,nup)**2-
     #pup(1,nup)**2-pup(2,nup)**2-pup(3,nup)**2))
         spinup(nup)=9
         vtimup(nup)=0

c     w
         nup=nup+1
         iw=nup
         idup(nup)=24 *ttype
         istup(nup)=2
         icolup(1,nup)=0
         icolup(2,nup)=0
         mothup(1,nup)=3
         mothup(2,nup)=3
         pup(4,nup)=klab_os(0,nlegreal+1)+klab_os(0,nlegreal+2)
         do mu=1,3
            pup(mu,nup)=klab_os(mu,nlegreal+1)+klab_os(mu,nlegreal+2)
         enddo
         pup(5,nup)=sqrt(dabs(pup(4,nup)**2-
     #pup(1,nup)**2-pup(2,nup)**2-pup(3,nup)**2))     
         spinup(nup)=9
         vtimup(nup)=0    

c     e
         nup=nup+1
         ie=nup
         idup(nup)=decids(1) *ttype
         istup(nup)=1
         icolup(1,nup)=0
         icolup(2,nup)=0
         mothup(1,nup)=iw
         mothup(2,nup)=iw
         pup(4,nup)=klab_os(0,nlegreal+1)
         do mu=1,3
            pup(mu,nup)=klab_os(mu,nlegreal+1)
         enddo
         pup(5,nup)=sqrt(dabs(pup(4,nup)**2-
     #pup(1,nup)**2-pup(2,nup)**2-pup(3,nup)**2))
         spinup(nup)=9
         vtimup(nup)=0

c     ve
         nup=nup+1
         ive=nup
         idup(nup)=decids(2) *ttype
         istup(nup)=1
         icolup(1,nup)=0
         icolup(2,nup)=0
         mothup(1,nup)=iw
         mothup(2,nup)=iw
         pup(4,nup)=klab_os(0,nlegreal+2)
         do mu=1,3
            pup(mu,nup)=klab_os(mu,nlegreal+2)
         enddo
         pup(5,nup)=sqrt(dabs(pup(4,nup)**2-
     #pup(1,nup)**2-pup(2,nup)**2-pup(3,nup)**2))
         if((abs(idup(nup)).eq.12).or.(abs(idup(nup)).eq.14).or.
     #(abs(idup(nup)).eq.16)) then
            pup(5,nup)=0d0
         endif
         spinup(nup)=9
         vtimup(nup)=0

c     color connenction in case of a w hadronic decay
         if(abs(idup(ie)).lt.4) then
            if(idup(ie).gt.0) then
               icolup(1,ie)=510
               icolup(2,ie)=0
               icolup(1,ive)=0
               icolup(2,ive)=510
            else
               icolup(1,ie)=0
               icolup(2,ie)=510
               icolup(1,ive)=510
               icolup(2,ive)=0
            endif
         endif
      else
         if(ini.eq.0) then
            ini=1
c     !: WARNING: tdecayflag=false HAS NOT BEEN 
c     tested as well as the others running modes.
            write(*,*) 'will not generate top-decay products'
         endif
      endif   

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     Here I don't use the subroutines 
c     st_tch_add_resonance or momenta_reshuffle.
c     Everything already performed by st_tch_tdecay_offshell
c     and st_tch_put_on_mass_shell
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     !: beginning of subroutines needed for the decay
cccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine st_tch_tdecay_offshell(klab_dec)
c     From a : parton parton -> t j
c          or: parton parton -> t j j
c     MASSLESS event in pup, add the t decay products, filling
c     klab_dec. Decay products are MASSLESS.
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/LesHouches.h'
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/pwhg_kn.h'
      include 'src/Lib/st_tch/pwhg_st.h'
      include 'src/Lib/st_tch/PhysPars.h'
      include 'src/Lib/st_tch/coupl.inc'
      real *8 klab_dec(0:3,nlegreal+3)

cccccccccccccccccccccccccccccccc    
c     common bl. originally present in lh_readin, needed
c     by st_tch_my_setpara
c
c     Common to lh_readin and st_tch_printout
c
      double precision  alpha, gfermi, alfas
      double precision  mtMS,mbMS,mcMS,mtaMS!MSbar masses
      double precision  Vud             !CKM matrix elements
      common/st_tch_values/    alpha,gfermi,alfas,   
     &                  mtMS,mbMS,mcMS,mtaMS,
     &                  Vud
ccccccccccccccccccccccccccccccccc

c     data
      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/

      real *8 klab_undec(0:3,nlegreal)
      real *8 kcm_undec(0:3,nlegreal)
      real *8 krcm_mad(0:3,nlegreal+2)
      real *8 kbcm_mad(0:3,nlegborn+2)
      real *8 kblab(0:3,nlegborn)

      integer igluon
      data igluon/21/
      integer mu,ileg
      real *8 beta_lab_to_cm(3),beta_cm_to_lab(3)
      real *8 amp2pow,amp2mad,rat
      real *8 m678_2,m67_2,m68_2,m68min_2,m68max_2,m6,m7,m8
      real *8 k3cm_dec(0:3,6:8)
      real *8 virt2,gam,mass,r,tvirt2,wvirt2
      real *8 E2s,E3s,m23min_2,m23max_2,m12,mt,m1,m2,m3,decmom
      real *8 xboundb

      real *8 k3reccm_undec(0:3,3:5),k3reccm_dec(0:3,3:5),  !:!
     $kcm_dec(0:3,nlegreal+3),kcm_undec_off(0:3,nlegreal)
      real *8 Etopdec_3reccm,Erecdec_3reccm,E3rec_cm,E3rec_3reccm,
     $k5vec2_cm,m3rec_dec,m3rec_undec
      real *8 diff,cfac,sqrttau_dec,sqrttau_undec
      real *8 beta_3reccm_to_cm(3),beta_cm_to_3reccm(3)
      real *8 xpdf(2),xpdf_dec(2),pdf1_dec(-6:6),pdf2_dec(-6:6),
     $     pdf1_und(-6:6),pdf2_und(-6:6)
      integer flav(2)

      integer idup_loc(nlegreal)
      real *8 ptemp(0:3)
      logical reorder

      logical check_bME
      parameter (check_bME=.false.)
      real *8 s,t,u,ewcoupl

      logical verbose
      parameter (verbose=.false.)

      real *8 tiny
      parameter (tiny=1.d-6)

      integer tdecaymode

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     variables needed to generate the decay with the veto method:
c     phase space & luminosity upper bound
      real *8 cfacbound
      data cfacbound/2.0d0/
      save cfacbound
c     decay upper bound normalization factor
      real *8 boundnorm
      data boundnorm/0d0/
      save boundnorm
c     to find the decay upper boun 
      integer iii,ini,nubound
      data ini/0/
      save ini
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      real *8 topdecamp,wdecamp
      real *8 st_tch_dotp,st_tch_random,st_tch_powheginput
      external st_tch_dotp,st_tch_random,st_tch_powheginput


      real *8 tmp
ccccccccccccccccccccccccccccccccccc
c     local functions:
c     spin summmed and averaged amplitude squared for top decay
c     (removed a sin2w factor in the denominator !!!!!)
      topdecamp(tvirt2,wvirt2)=-(2*wvirt2**2-tvirt2*wvirt2
     # -bmass**2*wvirt2-tvirt2**2+2*bmass**2*tvirt2-bmass**4)/wvirt2
     #    *4*pi*alpha/(2.)
     #    /2.
c     spin summmed and averaged amplitude squared for W decay
c     (removed a sin2w factor in the denominator !!!!!)
      wdecamp(wvirt2)=(2*wvirt2) / 3 * 4*pi*alpha/(2.)
c     other local functions
      virt2(mass,gam,r)=gam*mass*tan(pi/2*(1-2*r))+mass**2
      E2s(m12,m1,m2)=(m12**2-m1**2+m2**2)/(2*m12)
      E3s(mt,m12,m3)=(mt**2-m12**2-m3**2)/(2*m12)
      m23min_2(mt,m12,m1,m2,m3)=(E2s(m12,m1,m2)+E3s(mt,m12,m3))**2 -
     #(sqrt(E2s(m12,m1,m2)**2-m2**2)+sqrt(E3s(mt,m12,m3)**2-m3**2))**2
      m23max_2(mt,m12,m1,m2,m3)=(E2s(m12,m1,m2)+E3s(mt,m12,m3))**2 -
     #(sqrt(E2s(m12,m1,m2)**2-m2**2)-sqrt(E3s(mt,m12,m3)**2-m3**2))**2
c     modulus of spatial momenta for the decay m12 -> (m1,m2),
c     in m12 rest frame (PDG eq. 38.16)
      decmom(m12,m1,m2)=
     $sqrt((m12**2-(m1+m2)**2)*(m12**2-(m1-m2)**2))/2./m12
cccccccccccccccccccccccccccccccccc


c     to keep track of relabelling.
c     If at the end reorder=true, exchange momenta
c     and idup's of particles 4 and 5.
      reorder=.false.

ccccccccccccccccccccccccccccccccccccccc
c     charge conjugation
c     if ttype=-1, then idup has been filled with tbar
c     production flavours. Subroutines here work for t flavour.
c     Therefore, invert the sign of local flavours. To avoid confusion,
c     here I use idup_loc.
c     Moreover, define idup_loc using only fundamental flavours
      do ileg=1,nlegreal
         idup_loc(ileg)= ttype *idup(ileg)
         if(idup(ileg).eq.igluon) idup_loc(ileg)=0
         if(ileg.ne.3) then
            if(abs(idup_loc(ileg)).eq.3) then
               idup_loc(ileg)=sign(1,idup_loc(ileg))
            elseif(abs(idup_loc(ileg)).eq.4) then
               idup_loc(ileg)=sign(1,idup_loc(ileg)) *2
            elseif(abs(idup_loc(ileg)).eq.5) then
               idup_loc(ileg)=sign(1,idup_loc(ileg))
            endif
         endif
      enddo
ccccccccccccccccccccccccccccccccccccccc


ccccccccccccccccccccccccccccccccccccccccccc
c     >>> T CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc
c     Calculate undecayed matrix element.
c     No need of using realistic or current strong coupling for the
c     hit-and-miss procedure. Therefore, for simplicity here set alfas=1.
      alfas=1.
      mtMS=topmass_pow
      tmass=mtMS
c     undecayed matrix elements are computed assuming widths=0
      twidth=0d0
      wwidth=0d0
      call st_tch_my_setpara

c     -->> Born-like event
      if(nup.eq.nlegborn) then
c     fill klab_undec properly
         do ileg=1,nlegborn
            klab_undec(0,ileg)=pup(4,ileg)
            do mu=1,3
               klab_undec(mu,ileg)=pup(mu,ileg)
            enddo
         enddo
c     this is needed since for the decay generation
c     it is assumed that, for Born-like events, the 5th undecayed
c     momenta is null
         do mu=0,3
            klab_undec(mu,5)=0d0
         enddo
c     to avoid bugs in HELAS, restore exact masslessness of incoming partons 
         klab_undec(0,1)=dabs(klab_undec(3,1))
         klab_undec(0,2)=dabs(klab_undec(3,2))
c     from klab_undec to kblab
         call st_tch_preal_to_pborn(klab_undec,kblab)
         if(check_bME) then
            s=2d0*st_tch_dotp(kblab(0,1),kblab(0,2))
         t=st_tch_dotp(kblab(0,3),kblab(0,3)) - 2d0*st_tch_dotp(kblab(0,1),kblab(0,3)) 
            u=st_tch_dotp(kblab(0,3),kblab(0,3))-s-t
            ewcoupl=4d0*pi*alphaem_pow/sthw2_pow
         endif
c     now reevaluate Born squared amplitude, using HELAS.
c     ddx
         if ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.1)) then
            call st_tch_Sbdx_tux(kblab,amp2mad)
            if(check_bME) then
            write(*,*) 'bdx ',u*(u-topmass_pow**2)/(t-wmass_pow**2)**2/4
     $*ewcoupl**2/amp2mad
            endif
c     dxd
         elseif ((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
            call st_tch_Sdxb_tux(kblab,amp2mad)
            if(check_bME) then
            write(*,*) 'dxb ',t*(t-topmass_pow**2)/(u-wmass_pow**2)**2/4
     $*ewcoupl**2/amp2mad
            endif
c     ud
         elseif ((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
            call st_tch_Sub_td(kblab,amp2mad)
            if(check_bME) then
            write(*,*) 'ub ',s*(s-topmass_pow**2)/(u-wmass_pow**2)**2/4
     $*ewcoupl**2/amp2mad
            endif
c     du
         elseif ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.2)) then
            call st_tch_Sbu_td(kblab,amp2mad)
            if(check_bME) then
            write(*,*) 'bu ',s*(s-topmass_pow**2)/(t-wmass_pow**2)**2/4
     $*ewcoupl**2/amp2mad
            endif
         else
            write(*,*) 'Error in tdecay (born flavour)'
            call exit(1)
         endif

c     -->> real event
      elseif(nup.eq.nlegreal) then
c     fill klab_undec properly
         do ileg=1,nlegreal
            klab_undec(0,ileg)=pup(4,ileg)
            do mu=1,3
               klab_undec(mu,ileg)=pup(mu,ileg)
            enddo
         enddo
c     to avoid bugs in HELAS, restore exact masslessness of incoming partons 
         klab_undec(0,1)=dabs(klab_undec(3,1))
         klab_undec(0,2)=dabs(klab_undec(3,2))

c     reorder momenta, if needed. And keep track of this !!!!
c     for Born-like + gluon NOT needed
c     NEEDED for other processes, when f4 > f5
c     Extra check for gu/ug, because in this case
c     we want to have the sequences gu->tddx and ug->tddx.
         if(three_ch(idup_loc(5)).eq.0) then
            continue
         else
            if((idup_loc(4)).gt.(idup_loc(5))) then
c     revert, but keep as it is if gu/ug
               if(
     $ (three_ch(idup_loc(1)).eq.0.and.three_ch(idup_loc(2)).eq.2).or.
     $ (three_ch(idup_loc(2)).eq.0.and.three_ch(idup_loc(1)).eq.2)) then
                  if((idup_loc(4).ne.1).or.(idup_loc(5).ne.-1)) then
                     write(*,*) 'Error in tdecay, real fl. order',
     $                    idup_loc(1),idup(2),idup_loc(4),idup_loc(5)
                     call exit(1)
                  endif
               else
c     reorder momenta
                  do mu=0,3
                     ptemp(mu)=klab_undec(mu,4)
                     klab_undec(mu,4)=klab_undec(mu,5)
                     klab_undec(mu,5)=ptemp(mu)
                  enddo
c     reorder idup
                  ileg=idup_loc(4)
                  idup_loc(4)=idup_loc(5)
                  idup_loc(5)=ileg
                  reorder=.true.
               endif
            else
c     keep as it is, but revert if gu/ug
               if(
     $ (three_ch(idup_loc(1)).eq.0.and.three_ch(idup_loc(2)).eq.2).or.
     $ (three_ch(idup_loc(2)).eq.0.and.three_ch(idup_loc(1)).eq.2)) then
c     reorder momenta
                  do mu=0,3
                     ptemp(mu)=klab_undec(mu,4)
                     klab_undec(mu,4)=klab_undec(mu,5)
                     klab_undec(mu,5)=ptemp(mu)
                  enddo
c     reorder idup
                  ileg=idup_loc(4)
                  idup_loc(4)=idup_loc(5)
                  idup_loc(5)=ileg
                  reorder=.true.
               else
                  continue
               endif
            endif
         endif

         if(idup(1).eq.igluon) then
c     incoming gluon (leg 1)
c     gd -> tuxd
            if((three_ch(idup_loc(2)).eq.-1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sgb_tuxd_T(klab_undec,amp2mad)
c     gdx -> tuxdx
            elseif((three_ch(idup_loc(2)).eq.1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sgdx_tuxbx_T(klab_undec,amp2mad)
c     gu -> tddx
            elseif((three_ch(idup_loc(2)).eq.2).and.
     #(three_ch(idup_loc(4)).eq.-1)) then
               call st_tch_Sgu_tdbx_T(klab_undec,amp2mad)
            else
               write(*,*) 'Error in tdecay (real flavour 1)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         elseif(idup(2).eq.igluon) then
c     incoming gluon (leg 2)
c     dg -> tuxd
            if((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sbg_tuxd_T(klab_undec,amp2mad)
c     dxg -> tuxdx
            elseif((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sdxg_tuxbx_T(klab_undec,amp2mad)
c     ug -> tddx
            elseif((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(4)).eq.-1)) then
               call st_tch_Sug_tdbx_T(klab_undec,amp2mad)
            else
               write(*,*) 'Error in tdecay (real flavour 2)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         elseif((idup(1).ne.igluon).and.(idup(2).ne.igluon)) then
c     born-like + outgoing gluon (i.e. as born)
c     ddx
            if ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.1)) then
               call st_tch_Sbdx_tuxg(klab_undec,amp2mad)
c     dxd
            elseif ((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
               call st_tch_Sdxb_tuxg(klab_undec,amp2mad)
c     ud
            elseif ((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
               call st_tch_Sub_tdg(klab_undec,amp2mad)
c     du
            elseif ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.2)) then
               call st_tch_Sbu_tdg(klab_undec,amp2mad)
            else
               write(*,*) 'Error in tdecay (real flavour 3)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         else
            write(*,*) 'Error in tdecay (real flavour 4)'
            call exit(1)
         endif
      else
         write(*,*) 'Invalid nup in tdecay, 1'
         call exit(1)
      endif
      
c     restore the original ordering, and reset reorder at false
      if(reorder) then
c     reorder momenta
         do mu=0,3
            ptemp(mu)=klab_undec(mu,4)
            klab_undec(mu,4)=klab_undec(mu,5)
            klab_undec(mu,5)=ptemp(mu)
         enddo
c     reorder idup
         ileg=idup_loc(4)
         idup_loc(4)=idup_loc(5)
         idup_loc(5)=ileg
         reorder=.false.         
      endif

      amp2pow=amp2mad
c     now in amp2pow there is the full powheg squared amplitude
c     for the last MASSLESS generated event (no flux, no pdf, no ckm,
c     no phase space, alfas=1)



c     fill st_tch_boost velocities
      do mu=1,3
         beta_lab_to_cm(mu)=-(klab_undec(mu,1)+klab_undec(mu,2))/
     #(klab_undec(0,1)+klab_undec(0,2))
         beta_cm_to_lab(mu)=-beta_lab_to_cm(mu)
      enddo

c$$$      write(*,*) 'Undecayed momenta in lab frame'
c$$$      do ileg=1,nlegreal
c$$$         write(*,*) (klab_undec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(klab_undec(0,ileg),klab_undec(0,ileg))))
c$$$      enddo

c     st_tch_boost momenta in partonic cm frame
      do ileg=1,nlegreal
         call st_tch_boost(beta_lab_to_cm,klab_undec(0,ileg),kcm_undec(0,ileg))
      enddo

c$$$      write(*,*) 'Undecayed momenta in CM frame'
c$$$      do ileg=1,nlegreal
c$$$         write(*,*) (kcm_undec(mu,ileg), mu=0,3),sqrt(dabs(st_tch_dotp(kcm_undec(0,ileg),kcm_undec(0,ileg))))
c$$$      enddo

c     st_tch_boost 3rd and 'non-FKS-parton' momentum in their CM frame
c     Denote this system as 3rec
      do mu=1,3
         beta_cm_to_3reccm(mu)=-(kcm_undec(mu,3)+kcm_undec(mu,4))/
     $(kcm_undec(0,3)+kcm_undec(0,4))  !:!
      enddo
      do ileg=3,4 !:!
         call st_tch_boost(beta_cm_to_3reccm,kcm_undec(0,ileg),
     $        k3reccm_undec(0,ileg))
      enddo

c     decay products have to be massless
c     6-> e, 7->ve, 8->b
      m6=0d0
      m7=0d0
      m8=0d0

      iii=0
      rat=0d0

c      write(*,*) 'NEW EVENT'


c     Setting of Madgraph parameters for the full, decayed, matrix element.
c     wwidth and twidth are fixed st_tch_values (they don't change on an
c     event by event basis).  Neverthless they are reassigned here,
c     since for the previous call they were set to zero.
      alfas=1.
c     We want decayed matrix elements to be computed assuming nonvanishing
c     widths
      twidth=topwidth_pow
      wwidth=wwidth_pow
c     Top mass is assigned here, but it is also reassigned
c     inside the decay hit-and-miss loop, because we want to
c     evaluate the full, decayed Madgraph amplitude using
c     the top offshellness as value for the top mass.
c     Also, in graphs where a top propagator is present
c     in the production process, twidth has been set to ZERO
c     by hand (these are non-resonant propagators)
c     A backup copy of the 'original' madgraph sources has
c     been kept and named ME_decayed_bkp.
      mtMS=topmass_pow
      tmass=mtMS
      call st_tch_my_setpara
c     At this point all madgraph quark masses are set to zero (also
c     bmass).


c     Main hit-and-miss return point
c     Notice that kcm_undec and k3reccm_undec
c     MUST BE KEPT UNCHANGED during the hit-and-miss procedure.
 1    continue

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     Hit and miss procedure to take into account 
c     changes in luminosity and phase space factors

c     'phase-space & luminosity' hit-and-miss return point
 2    continue
c     generate top virtuality (offshellness)
      m678_2=virt2(topmass_pow,topwidth_pow,st_tch_random())
c     Uncomment the following to test strict t pole approximation
c$$$      m678_2=topmass_pow**2
      if(m678_2.lt.wmass_pow**2) then
c         write(*,*) 'PS&LUM HIT&MISS, m678'
         goto 2
      endif
c     restrict allowed range of m678 within 30 widths wrt peak value
c     (to avoid problems with veto upper bound)
      if(dabs(sqrt(m678_2)-topmass_pow)/topwidth_pow.gt.20) then
c         write(*,*) 'm678 too far wrt peak value: ',sqrt(m678_2),' regenerate'
         goto 2
      endif
c     generation of st_tch_random decay phase space.
c     Decay phase space is flat in M67_2 and M68_2.
c     Neverthless m67_2 is generated according to a BW peaked on mw and
c     with width wwidth, in the correct kinematical range.
c     Strictly speaking this is not necessary because the W propagator
c     in Madgraph ME will produce this effect. Having already generated m67_2
c     along its BW should improve the efficency.
c     m68_2 is generated flat between the correct boundaries that depends
c     upon m67.
      m67_2=virt2(wmass_pow,wwidth_pow,st_tch_random())
c     Uncomment the following to test strict w pole approximation
c$$$      m67_2=wmass_pow**2
      if((m67_2.lt.(m6+m7)**2).or.(m67_2.gt.m678_2)) then
c         write(*,*) 'PS&LUM HIT&MISS, m67'
         goto 2
      endif
c     extreme (large) st_tch_values of m67 seem to cause problems to herwig; avoid them!
      if((m67_2.lt.(wmass_pow-40*wwidth_pow)**2).or.
     #(m67_2.gt.(wmass_pow+40*wwidth_pow)**2)) then
c         write(*,*) 'extreme value for m67: ',sqrt(m67_2),' regenerate'
         goto 2
      endif
      m68min_2=m23min_2(sqrt(m678_2),sqrt(m67_2),m7,m6,m8)
      m68max_2=m23max_2(sqrt(m678_2),sqrt(m67_2),m7,m6,m8)
      m68_2=m68min_2 +st_tch_random()*(m68max_2-m68min_2)

c     !:! here is where I should check whether FKS parton
c     is the 4th and not the 5th, since in this case I'd like
c     to preserve the 4th (and not the 5th) momentum.

c     Now, from the generated value of m678, calculate the new value
c     of the top-quark energy (and of the recoiling particle)
c     in the 3rec rest frame. Then assign their momenta.
c     Here I use k4 (!:!) energy since in the 34 frame,
c     k4 energy is equal to the top spatial momentum 
      Etopdec_3reccm=sqrt(k3reccm_undec(0,4)**2+m678_2)  !:!
c     Following line because we want to keep k4 (!:!) massless
      Erecdec_3reccm=k3reccm_undec(0,4) !:!
      k3reccm_dec(0,3)=Etopdec_3reccm
      k3reccm_dec(0,4)=Erecdec_3reccm
      do ileg=3,4 !:!
         do mu=1,3
            k3reccm_dec(mu,ileg)=k3reccm_undec(mu,ileg)
         enddo
      enddo

c     calculate the new value of the energy of the 3rec system in the
c     3rec rest frame and in the CM frame, after the spreading of top virtuality
      E3rec_3reccm=Etopdec_3reccm+Erecdec_3reccm
      k5vec2_cm=kcm_undec(1,5)**2+kcm_undec(2,5)**2+kcm_undec(3,5)**2
      E3rec_cm=sqrt(E3rec_3reccm**2 + k5vec2_cm)

c     st_tch_boost the new 3rd and recth momenta back in the CM frame,
c     in a way that preserve the original 3-momentum of the
c     3rec system (i.e. the 3 momentum of the radiated parton)
      do mu=1,3
         beta_3reccm_to_cm(mu)=(kcm_undec(mu,3)+kcm_undec(mu,4))
     $        /E3rec_cm  !:!
      enddo
      do ileg=3,4 !:!
         call st_tch_boost(beta_3reccm_to_cm,k3reccm_dec(0,ileg),
     $        kcm_dec(0,ileg))
      enddo

c     check that k3recvec_dec and k3recvec_undec are the same
      do mu=1,3
         diff= (kcm_undec(mu,3)+kcm_undec(mu,4))-
     $(kcm_dec(mu,3)+kcm_dec(mu,4))   !:!
         if(dabs(diff).gt.tiny) then
            write(*,*) 'Error 1 in generating top virtuality'
            write(*,*) 'mu,diff ',mu,diff
            call exit(1)
         endif
      enddo

c     phase-space factor for hit-and-miss
c     6-> e, 7->ve, 8->b
c     1/(Etop*Erec) in 3rec frame (here Erec stays constant)
      cfac=      k3reccm_undec(0,3)/k3reccm_dec(0,3)
      cfac=cfac *k3reccm_undec(0,4)/k3reccm_dec(0,4) !:!
c     t->Wb phase space:  bvec/topmass (b assumed massless)
      cfac=cfac *(decmom(sqrt(m678_2),sqrt(m67_2),m8)/sqrt(m678_2))/
     $(decmom(topmass_pow,wmass_pow,m8)/topmass_pow)
c     W->lv phase space: lvec/wmass (l and v assumed massless)
      cfac=cfac *(decmom(sqrt(m67_2),m6,m7)/sqrt(m67_2))/
     $(decmom(wmass_pow,m6,m7)/wmass_pow)
c     Jacobian: sqrt(tau) * (m3rec/E3rec)  in CM frame
      m3rec_dec=E3rec_3reccm
      m3rec_undec=sqrt(topmass_pow**2 +
     $2d0*st_tch_dotp(k3reccm_undec(0,3),k3reccm_undec(0,4))) !:!
      sqrttau_dec  = E3rec_cm+kcm_undec(0,5)
      sqrttau_undec= kcm_undec(0,3)+kcm_undec(0,4)+
     $kcm_undec(0,5)
      cfac=cfac *(sqrttau_dec*m3rec_dec/E3rec_cm)/
     $(sqrttau_undec *m3rec_undec/(kcm_undec(0,3)+kcm_undec(0,4))) !:!
c     Luminosity change: f(x1)*f(x2)
c     The choice for the factorization scale here is not
c     unambiguos. I use topmass_pow (BW peak), a safe value.
c     It can be argued that other choices are more appropriate.
      st_mufact2=topmass_pow**2
      do ileg=1,2
         xpdf(ileg)=pup(4,ileg)/kn_beams(0,ileg)
         xpdf_dec(ileg)=xpdf(ileg)*sqrttau_dec/sqrttau_undec
         if(xpdf_dec(ileg).gt.1d0) then
         write(*,*)'PS&LUM HIT&MISS, x1_dec,x2_dec ',ileg,xpdf_dec(ileg)
            goto 2
         endif
         flav(ileg)=idup(ileg)
         if(flav(ileg).eq.igluon) flav(ileg)=0
         if(ileg.eq.1) then
            call st_tch_pdfcall(1,xpdf(1),pdf1_und)
            call st_tch_pdfcall(1,xpdf_dec(1),pdf1_dec)
            if(pdf1_und(flav(1)).ne.0) then
               cfac=cfac 
     $              *pdf1_dec(flav(1))/pdf1_und(flav(1))
            endif
         elseif(ileg.eq.2) then
            call st_tch_pdfcall(2,xpdf(2),pdf2_und)
            call st_tch_pdfcall(2,xpdf_dec(2),pdf2_dec)
            if(pdf2_und(flav(2)).ne.0) then
               cfac=cfac 
     $              *pdf2_dec(flav(2))/pdf2_und(flav(2))
            endif
         else
            write(*,*) 'Error 2 in generating top virtuality'
            call exit(1)
         endif
      enddo
c     hit-and-miss rule, as in POWHEG-hvq

      if(cfac.gt.cfacbound) then
         if(verbose) then
            write(*,*) 'PS&LUM upper bound violation: cfac/cfacbound= ',
     $           cfac/cfacbound,' new PS&LUM upper bound is = ',cfac
         endif
         cfacbound=cfac
      endif

      if(cfacbound*st_tch_random().gt.cfac) then
c         write(*,*) 'PS&LUM HIT&MISS, real goto, cfac',cfac
         goto 2
      endif
ccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     Now, we want to 'redefine kcm_undec', using the momenta
c     obtained with the procedure above (top offshellness).
c     Because we are in the hit-and miss, define kcm_undec to
c     be used in the following with a different notation
      do ileg=3,4 !:!
         do mu=0,3
            kcm_undec_off(mu,ileg)=kcm_dec(mu,ileg)
         enddo
      enddo
      do ileg=1,2
         do mu=0,3
            kcm_undec_off(mu,ileg)=kcm_undec(mu,ileg)
     $           *sqrttau_dec/sqrttau_undec
         enddo
      enddo
      if(nup.eq.4) then
         do mu=0,3
            if(dabs(kcm_undec(mu,5)).gt.tiny) then
               write(*,*) 'Error 3 in generating top virtuality'
               write(*,*)'kcm_undec(mu,5) ',(kcm_undec(ileg,5),ileg=0,3)
               call exit(1)
            endif
         enddo
      else
         do mu=0,3
            kcm_undec_off(mu,5)=kcm_undec(mu,5) !:!
         enddo 
      endif

      do mu=0,3
         diff=0d0
         do ileg=1,2
            diff=diff+kcm_undec_off(mu,ileg)
         enddo
         do ileg=3,nup
            diff=diff-kcm_undec_off(mu,ileg)
         enddo
         if(dabs(diff).gt.tiny) then
            write(*,*) 'Error in kcm_undec_off ',mu,diff,nup
            call exit(1)
         endif
      enddo

c     if we don't want the decay products, don't
c     waste time to compute their kinematics.
c     Fill the array klab_dec, and exit from this routine
      tdecaymode=st_tch_powheginput('#topdecaymode')
      if(tdecaymode.eq.0) then
         do ileg=1,nlegreal
            call st_tch_boost(beta_cm_to_lab,kcm_undec_off(0,ileg),klab_dec(0,ileg))
         enddo
         do ileg=nlegreal+1,nlegreal+3
            do mu=0,3
               klab_dec(mu,ileg)=0d0
            enddo
         enddo
         return
      endif



c     given top momentum in partonic cm frame, m67_2 and m68_2,
c     build top-decay momenta in partonic cm frame
c     6-> e, 7->ve, 8->b
      call st_tch_build_decay_mom(kcm_undec_off(0,3),m67_2,m68_2,m6,m7,m8,
     $     k3cm_dec)


c$$$      write(*,*) 't-dec mom conservation check'
c$$$      write(*,*) k3cm_dec(0,6)+k3cm_dec(0,7)+k3cm_dec(0,8)-kcm_undec_off(0,3)
c$$$      write(*,*) k3cm_dec(1,6)+k3cm_dec(1,7)+k3cm_dec(1,8)-kcm_undec_off(1,3)
c$$$      write(*,*) k3cm_dec(2,6)+k3cm_dec(2,7)+k3cm_dec(2,8)-kcm_undec_off(2,3)
c$$$      write(*,*) k3cm_dec(3,6)+k3cm_dec(3,7)+k3cm_dec(3,8)-kcm_undec_off(3,3)


c     fills madgraph momenta
c     Notice that madgraph 'decayed' subroutines expect momenta
c     in the order
c     f1,f2,ep,ve,b,f4(,f5)
c     where the undecayed process was f1 + f2 -> t + f4 (+ f5)
      if(nup.eq.nlegborn) then
         do mu=0,3
            do ileg=1,2
               kbcm_mad(mu,ileg)=kcm_undec_off(mu,ileg)
            enddo
            kbcm_mad(mu,6)=kcm_undec_off(mu,4)
            do ileg=3,5
               kbcm_mad(mu,ileg)=k3cm_dec(mu,ileg+3) !t decay products
            enddo
         enddo
c     to avoid bugs in HELAS, restore exact masslessness of incoming partons 
         kbcm_mad(0,1)=dabs(kbcm_mad(3,1))
         kbcm_mad(0,2)=dabs(kbcm_mad(3,2))
         
      elseif(nup.eq.nlegreal) then
         do mu=0,3
            do ileg=1,2
               krcm_mad(mu,ileg)=kcm_undec_off(mu,ileg)
            enddo
            krcm_mad(mu,6)=kcm_undec_off(mu,4)
            krcm_mad(mu,7)=kcm_undec_off(mu,5) !extra parton in the final state
            do ileg=3,5
               krcm_mad(mu,ileg)=k3cm_dec(mu,ileg+3) !t decay products
            enddo
         enddo
c     to avoid bugs in HELAS, restore exact masslessness of incoming partons 
         krcm_mad(0,1)=dabs(krcm_mad(3,1))
         krcm_mad(0,2)=dabs(krcm_mad(3,2))

c     reorder momenta, if needed. And keep track of this !!!!
c     for Born-like + gluon NOT needed
c     NEEDED for other processes, when f4 > f5
c     Extra check for gu/ug, because in this case
c     we want to have the sequences gu->tddx and ug->tddx.
         if(three_ch(idup_loc(5)).eq.0) then
            continue
         else
            if((idup_loc(4)).gt.(idup_loc(5))) then
c     revert, but keep as it is if gu/ug
               if(
     $ (three_ch(idup_loc(1)).eq.0.and.three_ch(idup_loc(2)).eq.2).or.
     $ (three_ch(idup_loc(2)).eq.0.and.three_ch(idup_loc(1)).eq.2)) then
                  if((idup_loc(4).ne.1).or.(idup_loc(5).ne.-1)) then
                     write(*,*) 'Error in tdecay, real fl. order',
     $                    idup_loc(1),idup(2),idup_loc(4),idup_loc(5)
                     call exit(1)
                  endif
               else
c     reorder momenta
                  do mu=0,3
                     ptemp(mu)=krcm_mad(mu,6)
                     krcm_mad(mu,6)=krcm_mad(mu,7)
                     krcm_mad(mu,7)=ptemp(mu)
                  enddo
c     reorder idup
                  ileg=idup_loc(4)
                  idup_loc(4)=idup_loc(5)
                  idup_loc(5)=ileg
                  reorder=.true.
               endif
            else
c     keep as it is, but revert if gu/ug
               if(
     $ (three_ch(idup_loc(1)).eq.0.and.three_ch(idup_loc(2)).eq.2).or.
     $ (three_ch(idup_loc(2)).eq.0.and.three_ch(idup_loc(1)).eq.2)) then
c     reorder momenta
                  do mu=0,3
                     ptemp(mu)=krcm_mad(mu,6)
                     krcm_mad(mu,6)=krcm_mad(mu,7)
                     krcm_mad(mu,7)=ptemp(mu)
                  enddo
c     reorder idup
                  ileg=idup_loc(4)
                  idup_loc(4)=idup_loc(5)
                  idup_loc(5)=ileg
                  reorder=.true.
               else
                  continue
               endif
            endif
         endif
      endif

c     check mom.cons. and invariant masses
      if(nup.eq.nlegborn) then
c$$$  call st_tch_check_kinematics(kbcm_mad,nlegborn+2)
         do mu=0,3
            diff=0d0
            do ileg=1,2
               diff=diff+kbcm_mad(mu,ileg)
            enddo
            do ileg=3,6
               diff=diff-kbcm_mad(mu,ileg)
            enddo
            if(dabs(diff).gt.tiny) then
               write(*,*) 'Error in kbcm_mad ',mu,diff,nup
               call exit(1)
            endif
         enddo
      
      elseif(nup.eq.nlegreal) then
c$$$  call st_tch_check_kinematics(krcm_mad,nlegreal+2)
         do mu=0,3
            diff=0d0
            do ileg=1,2
               diff=diff+krcm_mad(mu,ileg)
            enddo
            do ileg=3,7
               diff=diff-krcm_mad(mu,ileg)
            enddo
            if(dabs(diff).gt.tiny) then
               write(*,*) 'Error in krcm_mad ',mu,diff,nup
               call exit(1)
            endif
         enddo

      endif



ccccccccccccccccccccccccccccccccccccccccccc
c     >>> T CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc
c     Calculate decayed matrix element.
      amp2mad=0d0

cccccccccccccccccccccccccccccc
c     This assignment is needed to avoid problems with
c     decay upper bounds when a top offshellness is
c     generated. All other Madgraph parameters have
c     been already set by the call to st_tch_my_setpara before the
c     hit-and-miss return point.
c     In this way, madgraph top mass is NOT the central
c     point of the BW distribution
      mtMS=sqrt(m678_2)
      tmass=mtMS      
ccccccccccccccccccccccccccccc

c     -->> Born-like event
      if(nup.eq.nlegborn) then
c     ddx
         if ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.1)) then
            call st_tch_Sbdx_epvebux(kbcm_mad,amp2mad)
c     dxd
         elseif ((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
            call st_tch_Sdxb_epvebux(kbcm_mad,amp2mad)
c     ud
         elseif ((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
            call st_tch_Sub_epvebd(kbcm_mad,amp2mad)
c     du
         elseif ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.2)) then
            call st_tch_Sbu_epvebd(kbcm_mad,amp2mad)
         else
            write(*,*) 'Error in tdecay (born dec flavour)'
            call exit(1)
         endif

c     -->> real event
      elseif(nup.eq.nlegreal) then
         if(idup(1).eq.igluon) then
c     incoming gluon (leg 1)
c     gd -> tuxd
            if((three_ch(idup_loc(2)).eq.-1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sgb_epvebuxd_T(krcm_mad,amp2mad)
c     gdx -> tuxdx
            elseif((three_ch(idup_loc(2)).eq.1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sgdx_epvebuxbx_T(krcm_mad,amp2mad)
c     gu -> tddx
            elseif((three_ch(idup_loc(2)).eq.2).and.
     #(three_ch(idup_loc(4)).eq.-1)) then
               call st_tch_Sgu_epvebdbx_T(krcm_mad,amp2mad)
            else
               write(*,*) 'Error in tdecay (real dec flavour 1)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         elseif(idup(2).eq.igluon) then
c     incoming gluon (leg 2)
c     dg -> tuxd
            if((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sbg_epvebuxd_T(krcm_mad,amp2mad)
c     dxg -> tuxdx
            elseif((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(4)).eq.-2)) then
               call st_tch_Sdxg_epvebuxbx_T(krcm_mad,amp2mad)
c     ug -> tddx
            elseif((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(4)).eq.-1)) then
               call st_tch_Sug_epvebdbx_T(krcm_mad,amp2mad)
            else
               write(*,*) 'Error in tdecay (real dec flavour 2)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         elseif((idup(1).ne.igluon).and.(idup(2).ne.igluon)) then
c     born-like + outgoing gluon (i.e. as born)
c     ddx
            if ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.1)) then
               call st_tch_Sbdx_epvebuxg(krcm_mad,amp2mad)
c     dxd
            elseif ((three_ch(idup_loc(1)).eq.1).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
               call st_tch_Sdxb_epvebuxg(krcm_mad,amp2mad)
c     ud
            elseif ((three_ch(idup_loc(1)).eq.2).and.
     #(three_ch(idup_loc(2)).eq.-1)) then
               call st_tch_Sub_epvebdg(krcm_mad,amp2mad)
c     du
            elseif ((three_ch(idup_loc(1)).eq.-1).and.
     #(three_ch(idup_loc(2)).eq.2)) then
               call st_tch_Sbu_epvebdg(krcm_mad,amp2mad)
            else
               write(*,*) 'Error in tdecay (real dec flavour 3)',
     $              idup_loc(1),idup_loc(2),idup_loc(4),idup_loc(5)
               call exit(1)
            endif
         else
            write(*,*) 'Error in tdecay (real dec flavour 4)'
            call exit(1)
         endif

c     restore the original idup_loc ordering, and reset reorder at false
         if(reorder) then
c     reorder idup
            ileg=idup_loc(4)
            idup_loc(4)=idup_loc(5)
            idup_loc(5)=ileg
            reorder=.false.         
         endif
      else
         write(*,*) 'Invalid nup in tdecay, 2'
         call exit(1)
      endif


c     All subsequent factors are included in MadEvent, but not
c     in powheg squared matrix element
      xboundb=1d0
      xboundb=xboundb
     #/((m678_2-tmass**2)**2+tmass**2*twidth**2)
     #/((m67_2-wmass**2)**2+wmass**2*wwidth**2)
cccccccccccccccccccccccccccccccccccccccccc
c     Notice that (m678_2-tmass**2) is always zero,
c     because of the assignment above.
c     Notice that this term exactly cancels the corresponding
c     term present in the full madgraph amplitude, when the
c     hit-and-miss ratio is calculated.
c     Therefore, an arbitrary large value for twidth is no
c     longer needed
cccccccccccccccccccccccccccccccccccccccccc

c     Amplitude squared for (top decay * w decay)
      xboundb=xboundb*(topdecamp(m678_2,m67_2)/sthw2_pow)*
     #(wdecamp(m67_2)/sthw2_pow)

      rat=amp2mad/(xboundb*amp2pow)

      if(ini.eq.0) then
         nubound=st_tch_powheginput('nubound')
c     use as number of trial points nubound, or 3000 is nubound
c     is larger (3000 is an empyrical value)
         nubound=min(nubound,3000)
         iii=iii+1
         boundnorm=max(boundnorm,rat)
         if(iii.eq.nubound) goto 3
         goto 1
 3       continue
         write(*,*) 'top decay upper bound= ',boundnorm
         ini=1
      endif

      if(rat.gt.boundnorm) then
         call  st_tch_increasecnt('topdecay upper bound violations')
         if(verbose) then
            write(*,*) 'topdecay upper bound violation: rat/bound= ',
     $           rat/boundnorm
            write(*,*) 'new decay upper bound is = ',rat
         endif
         boundnorm=rat
         if(verbose) then
            write(*,*) 'current value of m67 and m678:',
     $           sqrt(m67_2),sqrt(m678_2)
            write(*,*) 'current value of xi and y:',
     $           kn_csitilde,kn_y
         endif
      endif

      if(boundnorm*st_tch_random().gt.rat) then
         call st_tch_increasecnt('topdecay vetoed configurations')
         goto 1
      endif
c     End Hit and Miss loop


c     If we are here, kinematics has been accepted: we can save the momenta
      do ileg=1,nlegreal+3
         do mu=0,3
            klab_dec(mu,ileg)=0d0
         enddo
      enddo
      do ileg=1,nlegreal
       call st_tch_boost(beta_cm_to_lab,kcm_undec_off(0,ileg),klab_dec(0,ileg))
      enddo
      do ileg=nlegreal+1,nlegreal+3
         call st_tch_boost(beta_cm_to_lab,k3cm_dec(0,ileg),klab_dec(0,ileg))
      enddo

c$$$      call st_tch_check_kinematics(klab_dec,nlegreal+3)
      end


      subroutine st_tch_pickwdecay(iw1,mdecw1,iw2,mdecw2,totbr)
c     !: originally taken from POWHEG-hvq
c     Finds which decays to choose with correct probability, according
c     to topdecaymode. It returns always particle ids of W+ decay.
c     iw1, iw2 refer to the pdg ids of W+ decay products 1 and 2;
c     by convention 1 is down type (e,mu,tau,d,s) and 2 is up type.
c     topdecaymode has to be an integer with 5 digits that are either 0 or 1
c     and they represent respectively the maximum number of the following particles
c     (antiparticles) in the top decay final state:
c     e  mu tau up charm
c     Relevant examples:
c     11111    All decays
c     leptonic:
c     10000    t->(b e ve) (with the appropriate signs)
c     11000    t->(b e ve) or t->(b mu vmu) (with the appropriate signs)
c     11100    fully leptonic
c     hadronic:
c     00010    t->(b u d) (with the appropriate signs)
c     00001    t->(b c s) (with the appropriate signs)
c     00011    fully hadronic
      implicit none
      include 'src/Lib/st_tch/PhysPars.h'
      integer iw1,iw2
      real * 8 mdecw1,mdecw2,totbr
c     local
      integer iwa(2)
      real * 8 prbs(1:5),totprbs(0:5),mass(16),sin2cabibbo,ebr,hbr,r
      integer ini,ii(5),j,k,imode,iwp(5,2)
      data ini/0/
c     pdg id's of the W+ decay products for e,mu,tau,up and charm decays (ignoring CKM)
      data ((iwp(j,k),k=1,2),j=1,5)/-11,12, -13,14, -15,16, -1,2, -3,4/
c     external
      real * 8 st_tch_random,st_tch_powheginput
      external st_tch_random,st_tch_powheginput
c     save
      save ini,totprbs,iwp,mass,sin2cabibbo
      if(ini.eq.2) return
      if(ini.eq.0.or.iw1.eq.-1000) then
         ini=1
c     on first run look for decay mode in st_tch_powheginput
         imode=st_tch_powheginput('topdecaymode')
         if(imode.le.0) then
            write(*,*) 'Invalid value for tdecaymode, in st_tch_pickwdecay'
            call exit(1)
         endif
c$$$         if(imode.eq.0) then
c$$$            ini=2
c$$$            return
c$$$         endif
         ii(1)=imode/10000
         imode=imode-ii(1)*10000
         ii(2)=imode/1000
         imode=imode-ii(2)*1000
         ii(3)=imode/100
         imode=imode-ii(3)*100
         ii(4)=imode/10
         imode=imode-ii(4)*10
         ii(5)=imode
c     load from input card the branching t->(b l vl) (only one lepton flavour)
         ebr=st_tch_powheginput('tdec/elbranching')
c     from ebr calculates the hadronic branching t->(b u d)
         hbr=(1-3*ebr)/2
         do j=1,5
            if(ii(j).eq.0) then
               prbs(j)=0
            else
               if(j.le.3) then
                  prbs(j)=ebr
               else
                  prbs(j)=hbr
               endif
            endif
         enddo
c     now in prbs(j) there is the branching ratio assumed by the program for the
c     j-type decay. If prbs(j)=0, the corresponding decay channel will be closed.
         totprbs(0)=0d0
         do j=1,5
            totprbs(j)=prbs(j)+totprbs(j-1)
         enddo

         totbr=totprbs(5)

c     mass of decay products. For internal consistency, here one should use
c     the masses assumed by the shower. Leptonic W decay products masses have to be
c     assigned here. The 3 light quarks are assumed massless.
         mass(11)=st_tch_powheginput('#tdec/emass')
         if(mass(11).lt.0) mass(11)=0.000511
         mass(13)=st_tch_powheginput('#tdec/mumass')
         if(mass(13).lt.0) mass(13)=0.1056
         mass(15)=st_tch_powheginput('#tdec/taumass')
         if(mass(15).lt.0) mass(15)=1.777 
         mass(12)=0
         mass(14)=0
         mass(16)=0
         mass(1)=0
         mass(2)=0
         mass(3)=0
c         mass(5)=st_tch_powheginput('tdec/bmass')
         mass(5)=st_tch_powheginput('#bottomthr')
         if (mass(5).lt.0d0) mass(5)=5.
c         mass(4)=st_tch_powheginput('tdec/cmass')
         mass(4)=st_tch_powheginput('#charmthr')
         if (mass(4).lt.0d0) mass(4)=1.5
         sin2cabibbo=(dCKM_pow(1,2))**2
         if(iw1.eq.-1000) return
      endif
c     end initialization

      r=st_tch_random()*totprbs(5)
      do j=1,5
         if(r.lt.totprbs(j)) goto 1
      enddo
 1    continue
c     now we have j decay mode
      if(j.gt.5) then
         write(*,*) 'Error in st_tch_pickwdecay, j',r,totprbs
         call exit(1)
      endif

c     W decay products
      iwa(1)=iwp(j,1)
      iwa(2)=iwp(j,2)
c     if any W decay product is down (or strange), it may turn to
c     strange (or down) with a probability sin^2 theta.
c     !: If it is down, there is also the chance it becomes a bottom.
c     !: This has not been implemented yet
      do j=1,2
         if(abs(iwa(j)).eq.1) then
            if(st_tch_random().lt.sin2cabibbo) then
               iwa(j)=sign(3,iwa(j))
            endif
         elseif(abs(iwa(j)).eq.3) then
            if(st_tch_random().lt.sin2cabibbo) then
               iwa(j)=sign(1,iwa(j))
            endif
         endif
      enddo
      iw1=iwa(1)
      iw2=iwa(2)
      mdecw1=mass(abs(iw1))
      mdecw2=mass(abs(iw2))
      end


      subroutine st_tch_build_decay_mom(xktop,m2eve,m2eb,me,mve,mb,xk3_dec)
c     !: originally taken from POWHEG-hvq (decaytop subroutine)
c     Put momenta of top decay in xk3_dec, distributed according phase space,
c     for given xktop,m2eve,m2eb,me,mve,mb. Phase space is flat in m2eve, m2eb
c     Input:
c      xktop: top 4 momentum in a given frame
c      m2eve, m2eb: squared invariant mass
c      me,mve,mb: particle masses
c     Output:
c      xk3_dec(0:3,1): e  (w decay)
c      xk3_dec(0:3,2): ve (w decay) 
c      xk3_dec(0:3,3): b  (t->wb decay)
c      xk3_dec are in the same frame of xktop
      implicit none
      real *8 xktop(0:3),xk3_dec(0:3,3),m2eve,m2eb,me,mve,mb
c     check parameters
      logical check
      real *8 tiny
      parameter (check=.true.,tiny=1.d-5)
c     local
      real *8 ktop(0:3),ktop_trest(0:3),k3_trest(0:3,3),
     #k2_wrest(0:3,2)
      real *8 beta_to_trest(3),beta_to_trest_inv(3),beta_wr_to_tr(3)
      real *8 m2t
      real *8 ee_trest,eve_trest,eb_trest,ew_trest,pb_trest
      real *8 ee_wrest,pe_wrest
      integer mu,ileg
c     external
      real *8 st_tch_dotp
      external st_tch_dotp

c     local copy of variables
      do mu=0,3
         ktop(mu)=xktop(mu)
      enddo
      
      do mu=0,3
         ktop_trest(mu)=0d0
         do ileg=1,2
            k3_trest(mu,ileg)=0d0
            k2_wrest(mu,ileg)=0d0
         enddo
         k3_trest(mu,3)=0d0
      enddo

c     fill st_tch_boost velocities
      do mu=1,3
         beta_to_trest(mu)=-ktop(mu)/ktop(0)
         beta_to_trest_inv(mu)=-beta_to_trest(mu)
      enddo

c     st_tch_boost top in its rest frame
      call st_tch_boost(beta_to_trest,ktop,ktop_trest)
      m2t=ktop_trest(0)**2

c     check
      if(check) then
         if(dabs(st_tch_dotp(ktop,ktop)-m2t).gt.tiny) then
            write(*,*) 'Error in st_tch_build_decay_mom'
            stop
         endif
      endif

      eve_trest=(m2t+mve**2-m2eb)/2d0/sqrt(m2t)
      eb_trest =(m2t+mb**2-m2eve)/2d0/sqrt(m2t)
      ee_trest =sqrt(m2t)-(eve_trest+eb_trest)

c     modulus of b momentum in t rest frame
      pb_trest=sqrt(dabs(eb_trest**2-mb**2))
c     generate st_tch_random directed b momentum in t rest frame
      call st_tch_rn3vec(k3_trest(1,3),pb_trest)
      k3_trest(0,3)=eb_trest

c     st_tch_boost from w rest frame to t rest frame
      ew_trest=ee_trest+eve_trest
      do mu=1,3
         beta_wr_to_tr(mu)=-k3_trest(mu,3)/ew_trest
      enddo


c     e: 1st W decay product in W rest frame
      ee_wrest=(m2eve+me**2-mve**2)/2d0/sqrt(m2eve)
      pe_wrest=sqrt(dabs(ee_wrest**2-me**2))
cccccccccccccccccccccccccccc
c     old version: it does not preserve e and ve energies separately, only the sum.
c     Also m2eb is not preserved.
c$$$      call st_tch_rn3vec(k2_wrest(1,1),pe_wrest)
c$$$      k2_wrest(0,1)=ee_wrest
ccccccccccccccccccccccccccc
ccccccccccccccccccccccccccccccccccccccccc
c     new version: relative orientations of 3 momenta are completely fixed by
c     chosen energies in t rest frame (apart from a st_tch_random azimuth in w rest frame)
      call st_tch_theta4vec(k2_wrest(0,1),ee_wrest,pe_wrest,ee_trest,
     #beta_wr_to_tr)
cccccccccccccccccccccccccccccccccccccccccc

c     ve: 2nd W decay product in W rest frame
      do mu=1,3
         k2_wrest(mu,2)=-k2_wrest(mu,1)
      enddo
      k2_wrest(0,2)=sqrt(m2eve)-k2_wrest(0,1)

c     st_tch_boost W decay products with W velocity in t rest frame
c     (its momentum is opposite to the b momentum)
      do ileg=1,2
         call st_tch_boost(beta_wr_to_tr,k2_wrest(0,ileg),
     #k3_trest(0,ileg))
      enddo

      if(check) then
         if(dabs(ee_trest+eve_trest-(k3_trest(0,1)+k3_trest(0,2)))
     #.gt.tiny) then
            write(*,*) 'error in build_dec_mom: sum'
         endif
         if((dabs(k3_trest(0,1)-ee_trest).gt.tiny).or.
     #(dabs(k3_trest(0,2)-eve_trest).gt.tiny)) then
            write(*,*) 'error in build_dec_mom ',k3_trest(0,1),ee_trest
            stop
         endif
         if(dabs(m2eve-2d0*st_tch_dotp(k3_trest(0,1),k3_trest(0,2))).gt.tiny)
     #then
            write(*,*) 'error in build_dec_mom: m2eve'
            stop
         endif
         if(dabs(m2eb-2d0*st_tch_dotp(k3_trest(0,1),k3_trest(0,3))).gt.tiny)
     #then
            write(*,*) 'error in build_dec_mom: m2eb'
            stop
         endif
      endif

c     now in k3_trest there are the t-decay products momenta.
c     st_tch_boost them back in original frame (all decay products along top velocity)
      do ileg=1,3
         call st_tch_boost(beta_to_trest_inv,k3_trest(0,ileg),
     #xk3_dec(0,ileg))
      enddo

      end


      subroutine st_tch_put_on_mass_shell(tdecayflag,MC_mass,xklab,xklab_os)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/LesHouches.h'
      include 'src/Lib/st_tch/PhysPars.h'
c     masses
      real *8 mcmass(0:6)
      common/st_tch_cmcmass/mcmass
c     channel flag
      integer chflag
      common/st_tch_cchflag/chflag
c     decay products
      integer decids(1:3)
      common/st_tch_cdecids/decids
c     check parameters
      logical check
      parameter (check=.true.)
      real *8 tiny
      parameter (tiny=1d-3)
      logical verbose
      parameter (verbose=.false.)
c     local
      real *8 toten,totpx,totpy,totpz
      integer MC_mass
      logical tdecayflag
      real *8 xklab(0:3,nlegreal+3),xklab_os(0:3,nlegreal+3)
      real *8 klab(0:3,nlegreal+3)
      integer ileg_dir
      real *8 betacm(3),betacm_inv(3),beta_cm_to_34(3),beta_34_to_cm(3),
     #dir(3)
      real *8 kcm(0:3,nlegreal+3),m(nlegreal+3),m_s(nlegreal+3),
     #ktmp(0:3),k34cm(0:3),kleg_34(0:3,3:4)
      real *8 ycminv,shat,kprime,xif,sol,Eprime,delta,gamma,kprime2,
     #m34
      real *8 k3tr(0:3,6:8),k2wr(0:3,6:7)
      real *8 beta_cm_to_tr_old(3),beta_tr_to_cm(3),beta_tr_to_wr(3),
     #beta_wr_to_tr(3)
      real *8 mt,m67,k67_mod,tmp
      integer iw1,iw2
      real *8 mdecw1,mdecw2,dummy
      logical iflag
      integer idone
      real *8 r,v2td,v2tdts
      integer mu,ileg
c     external
      real *8 st_tch_dotp,st_tch_norm,st_tch_random
      external st_tch_dotp,st_tch_norm,st_tch_random
ccccccccccccccccccccccccccccccccccccccccccccccccc
c     parameter to reshuffle momenta
      real *8 fact,delta_thr
      parameter (fact=0.98d0,delta_thr=1d-3)
ccccccccccccccccccccccccccccccccccccccccccccccccc

c     event left massless
      if(MC_mass.eq.0) then
         do ileg=1,nlegreal+3
            do mu=0,3
               xklab_os(mu,ileg)=xklab(mu,ileg)
            enddo
         enddo
         if(tdecayflag) then
            call st_tch_pickwdecay(iw1,mdecw1,iw2,mdecw2,dummy)
c     e
            decids(1)=iw1
c     ve
            decids(2)=iw2
c     b
            v2td=dCKM_pow(3,1)**2
            v2tdts=dCKM_pow(3,1)**2+dCKM_pow(3,2)**2
            r=st_tch_random()
            if(r.le.v2td) then
               decids(3)=1
            elseif((r.gt.v2td).and.(r.le.v2tdts)) then
               decids(3)=3
            else
               decids(3)=5
            endif
         endif
         return
      endif


      do ileg=1,nlegborn
         do mu=0,3
            klab(mu,ileg)=xklab(mu,ileg)
         enddo
         if(idup(ileg).ne.21) then
            m(ileg)=mcmass(abs(idup(ileg)))
            m_s(ileg)=mcmass(abs(idup(ileg)))
         else
            m(ileg)=mcmass(0)
            m_s(ileg)=mcmass(0)
         endif
      enddo

      if(nup.eq.nlegreal) then
         do mu=0,3
            klab(mu,nlegreal)=xklab(mu,nlegreal)
         enddo
         if(idup(nlegreal).ne.21) then
            m(nlegreal)=mcmass(abs(idup(nlegreal)))
            m_s(nlegreal)=mcmass(abs(idup(nlegreal)))
         else
            m(ileg)=mcmass(0)
            m_s(ileg)=mcmass(0)
         endif
      elseif(nup.eq.nlegborn) then
         do mu=0,3
            klab(mu,nlegreal)=0d0
         enddo
         m(nlegreal)=0d0
         m_s(nlegreal)=0d0
      else
         write(*,*) 'NUP ERROR in st_tch_put_on_mass_shell '
      endif



      if(tdecayflag) then
         call st_tch_pickwdecay(iw1,mdecw1,iw2,mdecw2,dummy)
c     e
         decids(1)=iw1
         m(nlegreal+1)=mdecw1
         m_s(nlegreal+1)=m(nlegreal+1)
c     ve
         decids(2)=iw2
         m(nlegreal+2)=mdecw2
         m_s(nlegreal+2)=m(nlegreal+2)
c     b
         v2td=dCKM_pow(3,1)**2
         v2tdts=dCKM_pow(3,1)**2+dCKM_pow(3,2)**2
         r=st_tch_random()
         if(r.le.v2td) then
            decids(3)=1
            m(nlegreal+3)=mcmass(1)
         elseif((r.gt.v2td).and.(r.le.v2tdts)) then
            decids(3)=3
            m(nlegreal+3)=mcmass(3)
         else
            decids(3)=5
            m(nlegreal+3)=mcmass(5)
         endif
         m_s(nlegreal+3)=m(nlegreal+3)

         do ileg=nlegreal+1,nlegreal+3
            do mu=0,3
               klab(mu,ileg)=xklab(mu,ileg)
            enddo
         enddo
      endif


c     klab are the input momenta and, at the beginning, they are all massless,
c     apart from the 3rd one, that is always the top quark.
c     If the event is Born-like (nup=4), klab(*,5) is null.
c     m(i) is the mass that the leg i has to acquire.
c     m_s(i) is a copy of this value that never changes in this subroutine.


ccccccccccccccccccccccccccccccccccc
c     st_tch_boost lab momenta in cm frame
ccccccccccccccccccccccccccccccccccc
      betacm(1)=0d0
      betacm(2)=0d0
      betacm(3)=(klab(3,1)+klab(3,2))/(klab(0,1)+klab(0,2))
      do mu=1,3
         betacm_inv(mu)=-betacm(mu)
      enddo
      do ileg=1,nlegreal
         call st_tch_boost(betacm_inv,klab(0,ileg),kcm(0,ileg))
      enddo

c     rapidity of lab frame wrt partonic cm frame (needed in the following)
      ycminv=-0.5d0*log(
     #((klab(0,1)+klab(0,2))+(klab(3,1)+klab(3,2)))/
     #((klab(0,1)+klab(0,2))-(klab(3,1)+klab(3,2))))
c--   
c     check that ycminv is ok
      if(check) then
         if(dabs(ycminv-
     #0.5d0*log((1d0+betacm_inv(3))/(1d0-betacm_inv(3)))).gt.tiny) then
            write(*,*) 'Error in put_on_mshell: check on ycminv #1'
            stop
         endif
      endif
c--   
      shat=(kcm(0,1)+kcm(0,2))**2


      if(MC_mass.eq.2) then
cccccccccccccccccccccccccccccccccccccccccc
c     reshuffling of initial state momenta
cccccccccccccccccccccccccccccccccccccccccc
c     try to change energies and longitudinal momenta of incoming partons
c     in their original cm frame. Then st_tch_boost back to the lab frame.
c     sqrt(s) and ycm are kept fixed
         write(*,*) '>>>>> WARNING <<<<<<'
         write(*,*) 
     $        'MC_mass=2 HAS NOT BEEN FULLY TESTED IN THE BOX'
         write(*,*) '>>>>>>>>>> PROGRAM STOPS <<<<<<<<<<'
         write(*,*) '>>>>>>>>>><<<<<<<<<<'
         call exit(1)
c$$$         idone=0
c$$$ 100     continue
c$$$c     spatial longitudinal momentum in cm frame
c$$$         kprime=0.5d0*sqrt(
c$$$     #        shat-2d0*(m(1)**2+m(2)**2)+(m(1)**2-m(2)**2)**2/shat)
c$$$         kcm(1,1)=0d0
c$$$         kcm(2,1)=0d0
c$$$         kcm(3,1)=kprime
c$$$         kcm(0,1)=0.5d0*sqrt(shat)*(1d0-(m(2)**2-m(1)**2)/shat) 
c$$$         do mu=1,3
c$$$            kcm(mu,2)=-kcm(mu,1)
c$$$         enddo
c$$$         kcm(0,2)=0.5d0*sqrt(shat)*(1d0-(m(1)**2-m(2)**2)/shat)
c$$$c     st_tch_boost back in the lab frame
c$$$         do ileg=1,2
c$$$            call st_tch_boost(betacm,kcm(0,ileg),klab(0,ileg))
c$$$         enddo
c$$$c--   
c$$$c     check that partons are on shell and that ycm and shat are
c$$$c     unchanged
c$$$         if(check) then
c$$$            if(dabs(st_tch_dotp(klab(0,1),klab(0,1))-m(1)**2).gt.tiny) then
c$$$               write(*,*) 'Error in put_on_mshell: check on m(1)'
c$$$               stop
c$$$            endif
c$$$            if(dabs(st_tch_dotp(klab(0,2),klab(0,2))-m(2)**2).gt.tiny) then
c$$$               write(*,*) 'Error in put_on_mshell: check on m(2)'
c$$$               stop
c$$$            endif
c$$$            if(dabs(ycminv+0.5d0*log(
c$$$     #              ((klab(0,1)+klab(0,2))+(klab(3,1)+klab(3,2)))/
c$$$     #     ((klab(0,1)+klab(0,2))-(klab(3,1)+klab(3,2))))).gt.tiny) then
c$$$               write(*,*) 'Error in put_on_mshell: check on ycminv #2'
c$$$               stop
c$$$            endif
c$$$            if(dabs((klab(0,1)+klab(0,2))**2-(klab(3,1)+klab(3,2))**2
c$$$     #              -shat).gt.tiny) then
c$$$               write(*,*) 'Error in put_on_mshell: check on shat'
c$$$               stop
c$$$            endif
c$$$         endif
c$$$c--   
c$$$c     check that new initial state 3-momenta are opposite.
c$$$c     If not, iflag remains false.
c$$$         iflag=.false.
c$$$         if((klab(3,1).ge.0d0).and.(klab(3,2).le.0d0)) then
c$$$            iflag=.true. 
c$$$         elseif((klab(3,1).lt.0d0).and.(klab(3,2).le.0d0)) then
c$$$c     pz of 1st leg is wrong
c$$$            if(betacm(3).gt.0d0)then
c$$$               write(*,*) 'Error in put_on_mshell: #1'
c$$$               stop
c$$$            endif
c$$$            ileg=1
c$$$            xif=m(2)**2/shat
c$$$         elseif((klab(3,1).ge.0d0).and.(klab(3,2).gt.0d0)) then
c$$$c     pz of 2nd leg is wrong
c$$$            if(betacm(3).lt.0d0)then
c$$$               write(*,*)'Error in put_on_mshell: #2'
c$$$               stop
c$$$            endif
c$$$            ileg=2
c$$$            xif=m(1)**2/shat
c$$$         else
c$$$            write(*,*)'Error in put_on_mshell: #3'
c$$$            stop
c$$$         endif
c$$$c     If iflag=false, something went wrong with the above reshuffling.
c$$$c     More precisely, the original kinematic was such that, after the reshuffling,
c$$$c     the 2 incoming partons, boosted back in the lab frame, travel in the same
c$$$c     direction.
c$$$c     If this happened, it tries in another way, changing slightly the
c$$$c     mass of the parton travelling in the wrong direction (using ileg and xif).
c$$$c     This part is exactly as in MC@NLO.
c$$$         if(.not.iflag) then
c$$$!:            print*, 'changed the masses of initial state partons'
c$$$            sol=xif+cosh(2*ycminv)-
c$$$     #           sqrt(2.d0)*cosh(ycminv)*sqrt(cosh(2*ycminv)-1+2*xif)
c$$$            if(sol.le.0.d0.or.idone.eq.1)then
c$$$c     The procedure failed; pass the massless event to Herwig (or Pythia), 
c$$$c     and let Herwig (or Pythia) deal with it.
c$$$               m(1)=0d0
c$$$               m(2)=0d0
c$$$               kcm(3,1)=sqrt(shat)/2d0
c$$$               kcm(3,2)=-sqrt(shat)/2d0
c$$$!:               print*, 'left the event massless'
c$$$               return
c$$$            endif
c$$$            if(ileg.eq.1)then
c$$$               m(1)=fact*sqrt(sol*shat)
c$$$               if(m(1).gt.m_s(1))then
c$$$                  write(*,*)'Error in put_on_mshell: Mass # 1 too large'
c$$$                  stop
c$$$               endif
c$$$            elseif(ileg.eq.2)then
c$$$               m(2)=fact*sqrt(sol*shat)
c$$$               if(m(2).gt.m_s(2))then
c$$$                  write(*,*)'Error in put_on_mshell: Mass # 2 too large'
c$$$                  stop
c$$$               endif
c$$$            else
c$$$               write(*,*)'Error in put_on_mshell: #4'
c$$$               stop
c$$$            endif
c$$$            idone=1
c$$$            goto 100            !go back at the beginning and reshuffle the momenta with the new masses
c$$$         endif
      endif


cccccccccccccccccccccccccccccccccccccccc
c     reshuffling of final state momenta
cccccccccccccccccccccccccccccccccccccccc
c     in case of real event, we need to choose the leg whose direction will
c     be kept fixed by the reshuffling algortihm.
      if(nup.eq.5) then

c     the following choice corresponds to keep fixed the fksparton direction  (!:!)
         if(chflag.eq.1) then
            ileg_dir=5    !:!:! it was fksreg_of_ar_s(2,jflreal)
         elseif(chflag.eq.2) then
            ileg_dir=5    !:!:! it was fksreg_of_ar_t(2,jflreal)
         else
            write(*,*) 'Error in put_on_mshell: wrong chflag'
            stop
         endif
         
c     now exchange momenta so that during the whole reshuffling the leg with
c     fixed direction is always the 5th leg. (!:!:!)
         if(ileg_dir.eq.5) then
            continue
         elseif(ileg_dir.eq.4) then
            do mu=0,3
               ktmp(mu)=kcm(mu,ileg_dir)
               kcm(mu,4)=kcm(mu,5)
               kcm(mu,5)=ktmp(mu)
            enddo
            ktmp(0)=m(5)
            m(5)=m(4)
            m(4)=ktmp(0)
            m_s(4)=m(4)
            m_s(5)=m(5)
         else
            write(*,*) 'Error in put_on_mshell: wrong ileg_dir'
            stop 
         endif
      endif

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     (!:!:!)
c     NB: from here on, kcm(*,5) is the parton whose direction
c     will be preserved during reshuffling. Strictly speaking it is
c     not needed this to be the fks-parton, also if this is a
c     reasonable choice. To implement a different choice, just change
c     the momenta exchange above.
c     ileg_dir is the integer used to save the original leg number.
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c     check wheter the massless kinematics will permit to reshuffle
c     momenta and put them on mass shell.

      if(nup.eq.4) then
         if(st_tch_norm(kcm(0,4)).lt.(m(4)*(m(4)/2+m(3))/(m(3)+m(4)))) then
            xklab_os(0,1)=-100d0
            return
         endif
      elseif(nup.eq.5) then
         do mu=0,3
            k34cm(mu)=kcm(mu,3)+kcm(mu,4)
         enddo
         m34=sqrt(st_tch_dotp(k34cm,k34cm))
         if(st_tch_norm(kcm(0,5)).lt.((2d0*m34*m(5)+m(5)**2)/2d0/(m34+m(5))))
     #then
            xklab_os(0,1)=-100d0
            return
         endif
      endif

c     In case of a real-like event, reshuffle the 5th momentum,
c     then the other ones.

      if(tdecayflag) then
c     needed only if decay products will be reshuffled
         do mu=1,3
            beta_cm_to_tr_old(mu)=-kcm(mu,3)/kcm(0,3)
         enddo
      endif


c     Try to keep the 5th 3-momentum unchanged, and change its energy
c     accordingly.
      kcm(0,5)=sqrt(kcm(1,5)**2+kcm(2,5)**2+kcm(3,5)**2+m(5)**2)
c     calculate the recoiling energy and the recoiling 3-mometum in the
c     cm frame. This is common to Born-like and real-like events.
      Eprime=kcm(0,1)+kcm(0,2)-kcm(0,5)
      kprime=sqrt(kcm(1,5)**2+kcm(2,5)**2+kcm(3,5)**2)
      
      delta=sqrt(dabs(Eprime**2-kprime**2)) -m(3)-m(4)
c     Possible problems can arise if delta is too small
c     (delta_thr is the small parameter used here).
c     Parton 3-momenta cannot be kept fixed: the total available energy is not sufficient.
c     In this case a reshuffle of initial state momenta is again needed, but shat will
c     be changed.
      if(delta.lt.delta_thr) then
         if(verbose) print*, 'event where delta < delta_thr. nup= ',nup
         call st_tch_increasecnt('events where delta < delta_thr ')
         gamma=sqrt((m(3)+m(4)+delta_thr)**2+kprime**2) + kcm(0,5)
         if(gamma.lt.(m(1)+m(2))) then
            write(*,*)'Error in put_on_mshell: gamma #1'
            stop
         endif
c     new 3-momentum of incoming partons in cm frame
         kprime2=0.5d0*sqrt(
     #gamma**2-2d0*(m(1)**2+m(2)**2)+(m(1)**2-m(2)**2)**2/gamma**2)
         kcm(1,1)=0d0
         kcm(2,1)=0d0
         kcm(3,1)=kprime2
         do mu=1,3
            kcm(mu,2)=-kcm(mu,1)
         enddo
c     new energies of incoming partons in cm frame
         kcm(0,1)=0.5d0*gamma*(1d0-(m(2)**2-m(1)**2)/gamma**2) 
         kcm(0,2)=0.5d0*gamma*(1d0-(m(1)**2-m(2)**2)/gamma**2)
c     evaluate again Eprime, needed in the following
         Eprime=kcm(0,1)+kcm(0,2)-kcm(0,5)
c     new shat
         shat=(kcm(0,1)+kcm(0,2))**2
         if(dabs(shat-gamma**2).gt.1d-3) then
            write(*,*) 'Error in put_on_mshell: gamma #2'
            stop
         endif
c     st_tch_boost back in the lab frame (with the old st_tch_boost vector)
         do ileg=1,2
            call st_tch_boost(betacm,kcm(0,ileg),klab(0,ileg))
            if(verbose) write(*,*)'\t',klab(0,ileg),klab(1,ileg),
     $           klab(2,ileg),klab(3,ileg)
         enddo

      endif

c     reshuffling of partons 3 and 4:
c     This is common to Born-like and real-like events.
c     k34cm corresponds to k3+k4 in the cm frame
      k34cm(0)=Eprime
      do mu=1,3
         k34cm(mu)=-kcm(mu,5)
      enddo
c     invariant mass of the system k3+k4
      m34=sqrt(dabs(Eprime**2-kprime**2))
c     3-momentum of k3 and k4 in their cm frame, with masses included
      if((m34**2-2d0*(m(3)**2+m(4)**2)+
     #(m(3)**2-m(4)**2)**2/m34**2).lt.0d0) then
         print*, 'radice negativa'
         print*, m34,m(3),m(4)
         stop
      endif
      kprime2=0.5d0*sqrt(
     #m34**2-2d0*(m(3)**2+m(4)**2)+(m(3)**2-m(4)**2)**2/m34**2)
      
      do mu=1,3
         dir(mu)=0d0
      enddo
c     st_tch_boost from cm frame to k3+k4 rest frame (and inverse) 
      do mu=1,3
         beta_cm_to_34(mu)=-k34cm(mu)/k34cm(0)
         beta_34_to_cm(mu)=-beta_cm_to_34(mu)
      enddo
c     st_tch_boost k3, k4 and k34 from cm to rest frame of (k3+k4)
      do ileg=3,4
         call st_tch_boost(beta_cm_to_34,kcm(0,ileg),kleg_34(0,ileg))
      enddo
      call st_tch_boost(beta_cm_to_34,k34cm(0),ktmp(0))
      if(check) then
         if((dabs(ktmp(1)).gt.tiny).or.
     #(dabs(ktmp(2)).gt.tiny).or.(dabs(ktmp(3)).gt.tiny)) then
            write(*,*) 'Error in put_on_mshell: ktmp=',ktmp
            stop
         endif
         if(dabs(st_tch_dotp(k34cm,k34cm)-ktmp(0)**2).gt.tiny) then
            write(*,*) 'Error in put_on_mshell: ktmp(0)=',ktmp(0)
            stop
         endif
      endif
      
      do mu=1,3
         dir(mu)=0.5d0* (kleg_34(mu,3)/st_tch_norm(kleg_34(0,3)) - 
     #kleg_34(mu,4)/st_tch_norm(kleg_34(0,4)))
      enddo

      tmp=sqrt(dir(1)**2+dir(2)**2+dir(3)**2)
      do mu=1,3
         dir(mu)=dir(mu)/tmp
      enddo
      
      do mu=1,3
         kleg_34(mu,3)=kprime2*dir(mu)
         kleg_34(mu,4)=-kprime2*dir(mu)
      enddo
      kleg_34(0,3)=0.5d0*m34*(1d0-(m(4)**2-m(3)**2)/m34**2)
      kleg_34(0,4)=0.5d0*m34*(1d0-(m(3)**2-m(4)**2)/m34**2)
      do ileg=3,4
         call st_tch_boost(beta_34_to_cm,kleg_34(0,ileg),kcm(0,ileg))
         if(dabs(st_tch_dotp(kcm(0,ileg),kcm(0,ileg))-m(ileg)**2).gt.tiny) then
            write(*,*) 'Error in put_on_mshell: #5', ileg
            write(*,*) m(ileg)**2,st_tch_dotp(kcm(0,ileg),kcm(0,ileg)),
     #st_tch_dotp(kleg_34(0,ileg),kleg_34(0,ileg))
            stop
         endif
      enddo

      if(nup.eq.5) then
c     Invert again leg 4 and 5 if the 5th parton at the beginning was the 4th
         if(ileg_dir.eq.4) then
            do mu=0,3
               ktmp(mu)=kcm(mu,ileg_dir)
               kcm(mu,4)=kcm(mu,5)
               kcm(mu,5)=ktmp(mu)
            enddo
         endif
      endif

      do ileg=3,5
         call st_tch_boost(betacm,kcm(0,ileg),klab(0,ileg))
      enddo
      do ileg=1,nlegreal
         do mu=0,3
            xklab_os(mu,ileg)=klab(mu,ileg)
         enddo
      enddo


cccccccccccccccccccccccccccccccccccccccccccccccc
c     leave to check momentum conservation
      toten=0d0
      totpx=0d0
      totpy=0d0
      totpz=0d0
      do ileg=1,2
         toten=toten+xklab_os(0,ileg)
         totpx=totpx+xklab_os(1,ileg)
         totpy=totpy+xklab_os(2,ileg)
         totpz=totpz+xklab_os(3,ileg)
      enddo
      do ileg=3,nup
         toten=toten-xklab_os(0,ileg)
         totpx=totpx-xklab_os(1,ileg)
         totpy=totpy-xklab_os(2,ileg)
         totpz=totpz-xklab_os(3,ileg)
      enddo

!      if(nup.eq.4) then
         if((toten.gt.1d-5).or.(totpx.gt.1d-5).or.
     #(totpy.gt.1d-5).or.(totpz.gt.1d-5)) then
            print*, '-------\n ERROR in xklab_os INSIDE putonmshell'
            if(nup.eq.4) then
               print*, 'BORNLIKE'
            else
               print*, 'REALLIKE'
            endif
            print*, '>>>',toten,totpx,totpy,totpz
            print*, 'kcm(*,5)', kcm(0,5),kcm(1,5),kcm(2,5),kcm(3,5)
            print*, 's partonic',shat,m34**2,2d0*st_tch_dotp(xklab_os(0,1),
     $       xklab_os(0,2)),175**2+2d0*st_tch_dotp(xklab_os(0,3),xklab_os(0,4))
            do ileg=1,2
               print*, xklab_os(0,ileg),xklab_os(1,ileg),
     $              xklab_os(2,ileg),xklab_os(3,ileg),
     #' mass ',dsqrt(xklab_os(0,ileg)**2-xklab_os(1,ileg)**2-
     $              xklab_os(2,ileg)**2-xklab_os(3,ileg)**2)
            enddo
            do ileg=3,nup
               print*, xklab_os(0,ileg),xklab_os(1,ileg),
     $              xklab_os(2,ileg),xklab_os(3,ileg),
     #' mass ',dsqrt(xklab_os(0,ileg)**2-xklab_os(1,ileg)**2-
     $              xklab_os(2,ileg)**2-xklab_os(3,ileg)**2)
            enddo
!            stop
         endif
!      endif
cccccccccccccccccccccccccccccccccccccccccccccccc



      if(tdecayflag) then

         do ileg=nlegreal+1,nlegreal+3
            call st_tch_boost(betacm_inv,klab(0,ileg),kcm(0,ileg))
         enddo

c     st_tch_boost decay products in the old top rest frame, i.e. in the t rest frame where
c     decay products are massless.
         do ileg=nlegreal+1,nlegreal+3
            call st_tch_boost(beta_cm_to_tr_old,kcm(0,ileg),k3tr(0,ileg))
         enddo
c     now k3tr(mu,6)+k3tr(mu,7)+k3tr(mu,8)=(mt,0,0,0)

         do mu=1,3
            beta_tr_to_cm(mu)=kcm(mu,3)/kcm(0,3)
         enddo

         mt=sqrt(st_tch_dotp(kcm(0,3),kcm(0,3)))
         m67=sqrt(2d0*st_tch_dotp(kcm(0,6),kcm(0,7)))
         Eprime=(mt**2+m(8)**2-m67**2)/2d0/mt
         kprime=sqrt(dabs(Eprime**2-m(8)**2))
         k67_mod=st_tch_norm(k3tr(0,8))
         do mu=1,3
            k3tr(mu,8)=kprime*k3tr(mu,8)/k67_mod
         enddo
         k3tr(0,8)=Eprime
         
         do mu=1,3
            beta_tr_to_wr(mu)=k3tr(mu,8)/(mt-Eprime)
            beta_wr_to_tr(mu)=-beta_tr_to_wr(mu)
         enddo
         do ileg=nlegreal+1,nlegreal+2
            call st_tch_boost(beta_tr_to_wr,k3tr(0,ileg),k2wr(0,ileg))
         enddo
         do mu=1,3
            dir(mu)=0.5d0* (k2wr(mu,6)/st_tch_norm(k2wr(0,6)) - 
     #k2wr(mu,7)/st_tch_norm(k2wr(0,7)))
         enddo
         tmp=sqrt(dir(1)**2+dir(2)**2+dir(3)**2)
         do mu=1,3
            dir(mu)=dir(mu)/tmp
         enddo
         kprime=0.5d0*sqrt(
     #m67**2-2d0*(m(6)**2+m(7)**2)+(m(6)**2-m(7)**2)**2/m67**2)
         do mu=1,3
            k2wr(mu,6)=kprime*dir(mu)
            k2wr(mu,7)=-kprime*dir(mu)
         enddo
         k2wr(0,6)=sqrt(kprime**2+m(6)**2)
         k2wr(0,7)=sqrt(kprime**2+m(7)**2)

         do ileg=nlegreal+1,nlegreal+2
            call st_tch_boost(beta_wr_to_tr,k2wr(0,ileg),k3tr(0,ileg))
         enddo

         do ileg=nlegreal+1,nlegreal+3
            call st_tch_boost(beta_tr_to_cm,k3tr(0,ileg),kcm(0,ileg))
         enddo

         do ileg=nlegreal+1,nlegreal+3
            call st_tch_boost(betacm,kcm(0,ileg),klab(0,ileg))
         enddo
         do ileg=nlegreal+1,nlegreal+3
            do mu=0,3
               xklab_os(mu,ileg)=klab(mu,ileg)
            enddo
         enddo
         
      endif

      end


      subroutine st_tch_rn3vec(vec,r)
c     !: originally taken from POWHEG-hvq      
c Generates a 3d vector in unit sphere
      implicit none
      real * 8 vec(3),r,r02,st_tch_norm
      integer j
c generate in unit cube -1<x,y,z<1
 1    call st_tch_RM48(vec,3)
      do j=1,3
         vec(j)=1-2*vec(j)
      enddo
c generate in unit sphere by hit and miss
      r02=vec(1)**2+vec(2)**2+vec(3)**2
      if(r02.gt.1) goto 1
c normalize
      st_tch_norm=r/sqrt(r02)
      do j=1,3
         vec(j)=vec(j)*st_tch_norm
      enddo
      end


      subroutine st_tch_theta4vec(vec,en,st_tch_norm,en_prime,beta)
c     given the st_tch_boost velocity beta, energy and spatial st_tch_norm in
c     the boosted frame (en,st_tch_norm) and energy in the initial frame (en_prime),
c     it returns in vec the full 4 momentum in the boosted frame
c     (i.e. the momentum that has en_prime when boosted back in the original frame).
c     vec azimuth wrt beta direction is cast randomly.
      implicit none
      include 'src/Lib/st_tch/pwhg_math.h'
      real *8 vec(0:3),en,st_tch_norm,en_prime,beta(3)
c     local
      real *8 phi,beta_mod,gamma_b,ctheta,vec_tmp(0:3)
      real *8 thn,phin
      integer mu
c     external
      real *8 st_tch_random
      external st_tch_random

      beta_mod=sqrt(beta(1)**2+beta(2)**2+beta(3)**2)
      gamma_b=1d0/sqrt(1d0-beta_mod**2)

      ctheta=(en_prime/gamma_b-en)/beta_mod/st_tch_norm

      phi=st_tch_random()*2d0*pi
      vec(3)=st_tch_norm*ctheta
      vec(1)=st_tch_norm*sqrt(1d0-ctheta**2)*dcos(phi)
      vec(2)=st_tch_norm*sqrt(1d0-ctheta**2)*dsin(phi)
      vec(0)=en

      thn=dacos(beta(3)/beta_mod)
      phin=datan2(beta(2)/beta_mod,beta(1)/beta_mod)

      call st_tch_rotate(vec,thn,phin,vec_tmp)
      do mu=0,3
         vec(mu)=vec_tmp(mu)
      enddo    

      end

      subroutine st_tch_preal_to_pborn(p,pprime)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      real *8 p(0:3,nlegreal)
      real *8 pprime(0:3,nlegborn)
c     local
      integer ileg,mu
      do ileg=1,nlegborn
         do mu=0,3
            pprime(mu,ileg)=p(mu,ileg)
         enddo
      enddo
      end  

      subroutine st_tch_rotate(p,th,phi,newp)
c     this subroutine is the product matrix of the two rotation matrixes:
c     R = R_z(phi)*R_y(th)
c     It brings a vector parallel to the z axis to form an angle th with the z axis
c     and an angle phi with the x axis
      implicit none
      real * 8 p(0:3),newp(0:3),th,phi
      real * 8 cphi,sphi,cth,sth
      cphi = dcos(phi)
      sphi = dsin(phi)
      cth = dcos(th)
      sth = dsin(th)
      newp(0) = p(0)
      newp(1) = cphi*cth*p(1)-sphi*p(2)+cphi*sth*p(3)
      newp(2) = sphi*cth*p(1)+cphi*p(2)+sphi*sth*p(3)
      newp(3) = -sth*p(1)+cth*p(3)
      end      


      subroutine st_tch_check_kinematics(p,nleg)
c     To check momentum conservation.
      implicit none
      integer nleg
      real *8 p(0:3,nleg)
c     local
      real *8 etotin,pxin,pyin,pzin,etotout,pxout,pyout,pzout
      integer j,mu
c     external
      real *8 st_tch_dotp
      external st_tch_dotp
      
      write(*,*) '\t----- KIN. TEST -----'
      write(*,*) '\tMOMENTA:'
      do j=1,nleg
         write(*,*) '\tpart ',j,': ',(p(mu,j), mu=0,3)
      enddo

      write(*,*) '\tINV MASS SQUARED:'
      do j=1,nleg
         write(*,*) '\tpart ',j,': ',st_tch_dotp(p(0,j),p(0,j))
      enddo

      etotout=0d0
      pxout=0d0
      pyout=0d0
      pzout=0d0
      do j=3,nleg
         etotout=etotout+p(0,j)
         pxout=pxout+p(1,j)
         pyout=pyout+p(2,j)
         pzout=pzout+p(3,j)
      enddo
      etotin=0d0
      pxin=0d0
      pyin=0d0
      pzin=0d0
      do j=1,2
         etotin=etotin+p(0,j)
         pxin=pxin+p(1,j)
         pyin=pyin+p(2,j)
         pzin=pzin+p(3,j)
      enddo
      write(*,*) '\tMUST BE ALL 0 ->',etotin-etotout,pxin-pxout,
     #     pyin-pyout,pzin-pzout 

      write(*,*) '\t---------'

      end

      function st_tch_norm(vec)
c     returns spatial st_tch_norm of a 4 vector
      implicit none
      real *8 st_tch_norm
      real *8 vec(0:3)
      st_tch_norm=sqrt(vec(1)**2+vec(2)**2+vec(3)**2)
      end


c     !: unused
c$$$      subroutine momenta_reshuffle(ires,i1,i2,decmass)
c$$$      implicit none
c$$$      end

