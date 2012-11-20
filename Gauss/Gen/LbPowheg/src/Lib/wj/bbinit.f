      subroutine wj_bbinit
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_flg.h'
      include 'src/Lib/wj/pwhg_rnd.h'
      include 'src/Lib/wj/pwhg_rad.h'
      integer iret1,iret2,iun
      real * 8 sigbtl,errbtl,sigrm,errrm,
     #         xint,xintrm
      real * 8 wj_btilde,wj_sigremnant
      integer ncall1,ncall2,itmx1,itmx2
      real * 8 xx(ndiminteg),xgrid(0:50,ndiminteg),ymax(50,ndiminteg)
     #                  ,xgridrm(0:50,ndiminteg),ymaxrm(50,ndiminteg)
     #                  ,xmmm(0:50,ndiminteg),xmmmrm(0:50,ndiminteg)
      integer ifold(ndiminteg),ifoldrm(ndiminteg)
      common/wj_cgengrids/xgrid,ymax,xmmm,xgridrm,ymaxrm,xmmmrm,
     #                ifold,ifoldrm
      character * 20 pwgprefix
      integer lprefix
      common/wj_cpwgprefix/pwgprefix,lprefix
      integer j,k,mcalls,icalls,imode,iunstat
      real * 8 wj_powheginput
      external wj_btilde,wj_sigremnant,wj_powheginput
      do j=1,ndiminteg
         do k=0,50
            xgrid(k,j)=0
            xgridrm(k,j)=0
            xmmm(k,j)=0
            xmmmrm(k,j)=0
         enddo
         do k=1,50
            ymax(k,j)=0
            ymaxrm(k,j)=0
         enddo
         ifold(j)=1
         ifoldrm(j)=1
      enddo
      call wj_newunit(iunstat)
      if(rnd_cwhichseed.eq.'none') then
         open(unit=iunstat,file=pwgprefix(1:lprefix)//'stat.dat',
     1        status='unknown')
      else
         open(unit=iunstat,file=pwgprefix(1:lprefix)//'stat-'//
     1        rnd_cwhichseed//'.dat',status='unknown')
      endif
      ncall1=wj_powheginput('ncall1')
      ncall2=wj_powheginput('ncall2')
      itmx1=wj_powheginput('itmx1')
      itmx2=wj_powheginput('itmx2')
      iret1=0
      iret2=0
      call wj_loadgrids(iret1,xgrid,ymax,xgridrm,ymaxrm,
     2     ifold,ifoldrm)
      if(iret1.ne.0) call wj_loadxgrid(iret2,xgrid,xint,xgridrm,xintrm)
      if(iret2.ne.0) then
         write(*,*)
         write(*,*)' POWHEG: initialization'
         write(*,*)' Computing the integral of the absolute value'
         if(flg_weightedev) then
         write(*,*)' of the wj_cross section times the suppression' 
         write(*,*)' factor to set up the adaptive grid'
         else
         write(*,*)' of the wj_cross section to set up the adaptive grid'
         endif
         call wj_newunit(iun)
         open(unit=iun,file=pwgprefix(1:lprefix)//'btlgrid.top',
     #        status='unknown')
         write(*,*)' result +- errtot (picobarn) for each iteration'
         flg_nlotest=.false.
         imode=0
         call wj_mint(wj_btilde,ndiminteg,ncall1,itmx1,ifold,imode,iun,
     #        xgrid,xint,ymax,sigbtl,errbtl)
         close(iun)
         if((flg_withreg.or.flg_withdamp).and..not.flg_bornonly) then
            write(*,*) ' Computing the integral of the'//
     #           ' remnant wj_cross section'
            if(flg_weightedev) then
               write(*,*) 'times the suppression factor'
            endif
            write(*,*) ' to set up the adaptive grid'
            flg_nlotest=.false.
            call wj_newunit(iun)
            open(unit=iun,file=pwgprefix(1:lprefix)//'rmngrid.top',
     #        status='unknown')
            imode=0
            call wj_mint(wj_sigremnant,ndiminteg,ncall1,itmx1,ifoldrm,imode,
     #         iun,xgridrm,xintrm,ymaxrm,sigrm,errrm)
            close(iun)
         endif
         call wj_storexgrid(xgrid,xint,xgridrm,xintrm)
         write(*,*)' Importance sampling x grids generated and stored'
      endif
      if(ncall2.eq.0) then
         write(*,*) ' ncall2 set to 0; nothing else to do'
         call exit(0)
      endif
c importance sampling grids have been initialized with default wj_SEED;
c they must all be the same. Now set current wj_SEED, if required
      if(rnd_cwhichseed.ne.'none')
     1     call wj_setrandom(rnd_initialseed,rnd_i1,rnd_i2)
      if(iret1.ne.0) then
         if (wj_powheginput('#testplots').eq.1d0) call wj_init_hist
c set  up the folding here, if required
         ifold(ndiminteg-2) = wj_powheginput("foldcsi")
         ifold(ndiminteg-1) = wj_powheginput("foldy")
         ifold(ndiminteg)   = wj_powheginput("foldphi")
         if(flg_withnegweights) then
            write(*,*)' POWHEG: Computing pos.+|neg.| '
     1           //' weight contribution to inclusive wj_cross section' 
         else
            write(*,*)' POWHEG: Computing positive weight'
     1           //' contribution to inclusive wj_cross section' 
         endif
         flg_nlotest=.true.
         imode=1
c Totals will also be made available in the rad_tot???btl variables.
c The output in sigbtl is: positive weight only (flg_withnegweights=.false.)
c                          pos-|neg|            (flg_withnegweights=.true.)
c Results in rad_tot???btl do not depend upon flg_withnegweights
         call wj_resettotals
         call wj_mint(wj_btilde,ndiminteg,ncall2,itmx2,ifold,imode,iun,
     #        xgrid,xint,ymax,sigbtl,errbtl)
         call wj_finaltotals
c finalize wj_btilde output in histograms
         call wj_pwhgaddout
         flg_nlotest=.false.
         write(*,*) 'wj_btilde pos.   weights:', rad_totposbtl,' +-',
     1        rad_etotposbtl
         write(*,*) 'wj_btilde |neg.| weights:', rad_totnegbtl,' +-',
     1        rad_etotnegbtl
         write(*,*) 'wj_btilde total (pos.-|neg.|):', rad_totbtl,' +-',
     1        rad_etotbtl
         write(iunstat,*) 'wj_btilde pos.   weights:', rad_totposbtl,' +-',
     1        rad_etotposbtl
         write(iunstat,*) 'wj_btilde |neg.| weights:', rad_totnegbtl,' +-',
     1        rad_etotnegbtl
         write(iunstat,*) 'wj_btilde Total (pos.-|neg.|):', rad_totbtl,
     1        ' +-',rad_etotbtl
c Now compute the remnant contributions
         if((flg_withreg.or.flg_withdamp).and..not.flg_bornonly) then
            write(*,*)' POWHEG: Computing remnant'//
     #                ' and/or regular remnants'
            flg_nlotest=.true.
            imode=1
            call wj_mint(wj_sigremnant,ndiminteg,ncall2,itmx2,ifoldrm,imode,
     #         iun,xgridrm,xintrm,ymaxrm,sigrm,errrm)
c add finalized remnant contributions in histograms
            call wj_pwhgaddout
            flg_nlotest=.false.
         else
            sigrm=0
            errrm=0
         endif
         rad_totrm=sigrm
         rad_etotrm=errrm
c rad_totgen is used for the generation of the events.
c wj_btilde and remnant event are chosen in proportion to
c rad_totbtlgen and rad_totrm.
         if(flg_withnegweights) then
            rad_totbtlgen=rad_totabsbtl
            rad_etotbtlgen=rad_etotabsbtl
         else
c notice: this is correct only if the negative fraction is
c negligible
            rad_totbtlgen=rad_totbtl
            rad_etotbtlgen=rad_etotbtl
         endif
         rad_totgen=rad_totrm+rad_totbtlgen
         rad_etotgen=sqrt(rad_etotbtlgen**2+rad_etotrm**2)
         rad_tot=rad_totrm+rad_totbtl
         rad_etot=sqrt(rad_etotbtl**2+rad_etotrm**2)
         
c        
         call wj_storegrids(xgrid,ymax,xgridrm,ymaxrm,ifold,ifoldrm,
     1        ncall2,itmx2)
c Output NLO histograms
         if (wj_powheginput('#testplots').eq.1d0) then
            if(rnd_cwhichseed.eq.'none') then
               open(unit=99,file=pwgprefix(1:lprefix)//'NLO.top')
            else
               open(unit=99,file=pwgprefix(1:lprefix)//'NLO-'
     1              //rnd_cwhichseed//'.top')
            endif
            call wj_pwhgtopout
            close(99)
         endif
         
         if(flg_withreg.or.flg_withdamp) then
            write(iunstat,*) ' Remnant wj_cross section in pb',
     1           rad_totrm,'+-',rad_etotrm
         endif
         
         if (flg_weightedev) then
            write(iunstat,*) 
     1 ' total (wj_btilde+remnants) wj_cross section times '
            write(iunstat,*) ' suppression factor in pb',
     1        rad_tot,'+-',rad_etot         
         else
            write(iunstat,*) 
     1 ' total (wj_btilde+remnants) wj_cross section in pb',
     2     rad_tot,'+-',rad_etot
         endif
         write(iunstat,*) ' negative weight fraction:',
     1        rad_totnegbtl/(2*rad_totnegbtl+rad_tot)
      else
         write(*,*)
     1     ' stored grids successfully loaded'
         write(*,*) 'wj_btilde pos.   weights:', rad_totposbtl,' +-',
     1        rad_etotposbtl
         write(*,*) 'wj_btilde |neg.| weights:', rad_totnegbtl,' +-',
     1        rad_etotnegbtl
         write(*,*) 'wj_btilde total (pos.-|neg.|):', rad_totbtl,' +-',
     1        rad_etotbtl
      endif

      if(flg_withreg.or.flg_withdamp) then
         write(*,*) ' Remnant wj_cross section in pb',
     1        rad_totrm,'+-',rad_etotrm
      endif

      if (flg_weightedev) then
         write(*,*) ' total (wj_btilde+remnants) wj_cross section times '
         write(*,*) ' suppression factor in pb',
     1        rad_tot,'+-',rad_etot         
      else
         write(*,*) ' total (wj_btilde+remnants) wj_cross section in pb',
     1     rad_tot,'+-',rad_etot
      endif
      write(*,*) ' negative weight fraction:',
     1     rad_totnegbtl/(2*rad_totnegbtl+rad_tot)
      close(iunstat)
      call flush
c initialize wj_gen; the array xmmm is set up at this stage.
      call wj_gen(wj_btilde,ndiminteg,xgrid,ymax,xmmm,ifold,0,
     #    mcalls,icalls,xx)
c compute normalization of upper bounding function for radiation
      call wj_do_maxrat(mcalls,icalls)
c print statistics
      call wj_gen(wj_btilde,ndiminteg,xgrid,ymax,xmmm,ifold,3,
     #    mcalls,icalls,xx)
      if(xx(1).gt.0) write(*,*) 'POWHEG: efficiency in the generation'
     # //' of the Born variables =',xx(1)
      if((flg_withreg.or.flg_withdamp).and..not.flg_bornonly) then
c initialize wj_gen for remnants
         call wj_gen(wj_sigremnant,ndiminteg,xgridrm,ymaxrm,xmmmrm,ifoldrm,
     #            0,mcalls,icalls,xx)
c     save wj_random number seeds
         call wj_randomsave
c generate few events from remnants, just to determine the generation efficiency
         do j=1,int(min(wj_powheginput('nubound'),10d0))
            call wj_gen(wj_sigremnant,ndiminteg,xgridrm,ymaxrm,xmmmrm,ifoldrm,
     #            1,mcalls,icalls,xx)
         enddo
c     restore  wj_random number seeds
         call wj_randomrestore
c     print statistics
         call wj_gen(wj_sigremnant,ndiminteg,xgridrm,ymaxrm,xmmmrm,ifoldrm,
     #            3,mcalls,icalls,xx)
         if(xx(1).gt.0) then
            write(*,*) 'POWHEG: efficiency in the generation'
     #                   //' of the remnant variables =',xx(1)
         endif
      endif
      end


      subroutine wj_gen_btilde(mcalls,icalls)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      integer mcalls,icalls
      real * 8 xgrid(0:50,ndiminteg),ymax(50,ndiminteg)
     #        ,xgridrm(0:50,ndiminteg),ymaxrm(50,ndiminteg)
     #        ,xmmm(0:50,ndiminteg),xmmmrm(0:50,ndiminteg)
      integer ifold(ndiminteg),ifoldrm(ndiminteg)
      common/wj_cgengrids/xgrid,ymax,xmmm,xgridrm,ymaxrm,xmmmrm,
     #                ifold,ifoldrm
      real * 8 xx(ndiminteg)      
      real * 8 wj_btilde
      external wj_btilde
      call wj_gen(wj_btilde,ndiminteg,xgrid,ymax,xmmm,ifold,1,
     #    mcalls,icalls,xx)
      end

      subroutine wj_gen_sigremnant
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      real * 8 xgrid(0:50,ndiminteg),ymax(50,ndiminteg)
     #        ,xgridrm(0:50,ndiminteg),ymaxrm(50,ndiminteg)
     #        ,xmmm(0:50,ndiminteg),xmmmrm(0:50,ndiminteg)
      integer ifold(ndiminteg),ifoldrm(ndiminteg)
      common/wj_cgengrids/xgrid,ymax,xmmm,xgridrm,ymaxrm,xmmmrm,
     #                ifold,ifoldrm
      real * 8 xx(ndiminteg)
      integer mcalls,icalls
      real * 8 wj_sigremnant
      external wj_sigremnant
      call wj_gen(wj_sigremnant,ndiminteg,xgridrm,ymaxrm,xmmmrm,ifoldrm,1,
     #    mcalls,icalls,xx)
      end


      subroutine wj_storegrids(xgrid,ymax,xgridrm,ymaxrm,
     #                ifold,ifoldrm,ncall2,itmx2)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_pdf.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/pwhg_rnd.h'
      real * 8 xgrid(0:50,ndiminteg),ymax(50,ndiminteg)
     #        ,xgridrm(0:50,ndiminteg),ymaxrm(50,ndiminteg)
      integer nbins
      parameter (nbins=50)
      integer ifold(ndiminteg),ifoldrm(ndiminteg),ncall2,itmx2
      character * 20 pwgprefix
      integer lprefix
      common/wj_cpwgprefix/pwgprefix,lprefix
      integer j,k,iun
      call wj_newunit(iun)
      if(rnd_cwhichseed.eq.'none') then
         open(unit=iun,file=pwgprefix(1:lprefix)//'grid.dat',
     #     form='unformatted',status='unknown')
      else
         open(unit=iun,
     1        file=pwgprefix(1:lprefix)//'grid-'//rnd_cwhichseed//
     2        '.dat',form='unformatted',status='unknown')
      endif
      write(iun) ((xgrid(j,k),k=1,ndiminteg),j=0,nbins)
      write(iun) ((ymax(j,k),k=1,ndiminteg),j=1,nbins)
      write(iun) ((xgridrm(j,k),k=1,ndiminteg),j=0,nbins)
      write(iun) ((ymaxrm(j,k),k=1,ndiminteg),j=1,nbins)
      write(iun) (ifold(k),k=1,ndiminteg)
      write(iun) (ifoldrm(k),k=1,ndiminteg)
      write(iun) ncall2*itmx2
      write(iun) kn_sbeams, pdf_ih1, pdf_ih2, pdf_ndns1, pdf_ndns2
      write(iun)
     1     rad_totbtl,rad_etotbtl,
     2     rad_totabsbtl,rad_etotabsbtl,
     3     rad_totposbtl,rad_etotposbtl,
     4     rad_totnegbtl,rad_etotnegbtl,
     5     rad_totrm,rad_etotrm,
     6     rad_totbtlgen,rad_etotbtlgen,
     7     rad_totgen,rad_etotgen,
     8     rad_tot,rad_etot
      close(iun)
      end

      subroutine wj_loadgrids(iret,xgrid,ymax,xgridrm,ymaxrm,
     #           ifold,ifoldrm)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_pdf.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/pwhg_rnd.h'
      real * 8 xgrid(0:50,ndiminteg),ymax(50,ndiminteg)
     #        ,xgridrm(0:50,ndiminteg),ymaxrm(50,ndiminteg)
      real * 8 xxgrid(0:50,ndiminteg),xymax(50,ndiminteg)
     #        ,xxgridrm(0:50,ndiminteg),xymaxrm(50,ndiminteg)
      real * 8 tot(2,8),rtot(2,8)
      integer ifold(ndiminteg),ifoldrm(ndiminteg)
      integer iifold(ndiminteg),iifoldrm(ndiminteg)
      integer iret,jfound
c
      integer ios
      integer nbins
      parameter (nbins=50)
      character * 20 pwgprefix
      integer lprefix
      common/wj_cpwgprefix/pwgprefix,lprefix
      character * 4 chseed
      real * 8 shx
      integer ih1x, ih2x, ndns1x, ndns2x
      integer j,k,iun,jfile,nfiles,ncall2,itmx2
      logical lpresent,manyfiles,filefound
      real * 8 wj_powheginput
      external wj_powheginput
      if(wj_powheginput('use-old-grid').eq.0) then
         iret=1
         return
      endif
      iret=0
      call wj_newunit(iun)
      open(unit=iun,file=pwgprefix(1:lprefix)//'grid.dat',
     #     form='unformatted',status='old',iostat=ios)
      if(ios.eq.0) then
         nfiles=1
      else
         if(rnd_cwhichseed.ne.'none') then
c Are we required to generate a grid file or to load a bunch of them?
c See if the grid file exists already
            inquire(file=pwgprefix(1:lprefix)//'grid-'//
     1           rnd_cwhichseed//'.dat',exist=lpresent)
            if(.not.lpresent) then
               iret=-1
               return
            endif
         endif
         nfiles=9999
         manyfiles=.true.
      endif
c Try to open and merge a set of grid files, generated with different
c wj_random seeds
      filefound=.false.
      jfound=0
      do jfile=1,nfiles
         if(manyfiles) then
            write(chseed,'(i4)') jfile
            do k=1,4
               if(chseed(k:k).eq.' ') chseed(k:k)='0'
            enddo
            inquire(file=pwgprefix(1:lprefix)//'grid-'//
     1           chseed//'.dat',exist=lpresent)
            if(.not.lpresent) goto 111
            open(unit=iun,file=pwgprefix(1:lprefix)//'grid-'//
     1           chseed//'.dat',
     2           form='unformatted',status='old',iostat=ios)
            if(ios.ne.0) then
               iret=-1
               return
            else
               write(*,*) ' Opened ',pwgprefix(1:lprefix)//'grid-'//
     1              chseed//'.dat'
            endif
         endif
         filefound=.true.
         read(iun,iostat=ios) ((xxgrid(j,k),k=1,ndiminteg),j=0,nbins)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) ((xymax(j,k),k=1,ndiminteg),j=1,nbins)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) ((xxgridrm(j,k),k=1,ndiminteg),j=0,nbins)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) ((xymaxrm(j,k),k=1,ndiminteg),j=1,nbins)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) (iifold(k),k=1,ndiminteg)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) (iifoldrm(k),k=1,ndiminteg)
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) ncall2
         if(wj_powheginput("#ncallfrominput").eq.1) then
            ncall2=wj_powheginput("ncall2")
            itmx2=wj_powheginput("itmx2")
            ncall2=ncall2*itmx2
         endif
         if(ios.ne.0) goto 998
         read(iun,iostat=ios) shx, ih1x, ih2x, ndns1x, ndns2x
         if(ios.ne.0) goto 998
         if(shx.ne.kn_sbeams.or.ih1x.ne.pdf_ih1.or.ih2x.ne.pdf_ih2
     1      .or.ndns1x.ne.pdf_ndns1.or.ndns2x.ne.pdf_ndns2.or.ios.ne.0)
     2        goto 998
         read(iun,iostat=ios) ((tot(k,j),k=1,2),j=1,8)
         if(ios.ne.0) goto 998
         jfound=jfound+1
         if(jfile.lt.2) then
            do k=1,ndiminteg
               do j=0,nbins
                  xgrid(j,k)=xxgrid(j,k)
                  xgridrm(j,k)=xxgridrm(j,k)
               enddo
               ifold(k)=iifold(k)
               ifoldrm(k)=iifoldrm(k)
            enddo
            do k=1,ndiminteg
               do j=1,nbins
                  ymax(j,k)=xymax(j,k)
                  ymaxrm(j,k)=xymaxrm(j,k)
               enddo
            enddo
            do k=1,2
               do j=1,8
                  rtot(k,j)=tot(k,j)
               enddo
            enddo
         else
            do k=1,ndiminteg
               do j=0,nbins
                  if(xgrid(j,k).ne.xxgrid(j,k).or.
     1                 xgridrm(j,k).ne.xxgridrm(j,k)) then
                     write(*,*) ' error loading grids: '
                     write(*,*)  pwgprefix(1:lprefix)//'grid-'//
     1          rnd_cwhichseed//'.dat does not have the same importance'
                    write(*,*) 'sampling grid as',pwgprefix(1:lprefix)//
     1                    'grid.dat'
                     call exit(-1)
                  endif
               enddo
               if(ifold(k).ne.iifold(k)
     1              .or.ifoldrm(k).ne.ifoldrm(k)) then
                  write(*,*) ' error loading grids: '
                  write(*,*)  pwgprefix(1:lprefix)//'grid-'//
     1                 rnd_cwhichseed//
     2                 '.dat does not have the same folding as'
                  write(*,*) pwgprefix(1:lprefix)//'grid.dat'
                  call exit(-1)
               endif
            enddo
            do k=1,ndiminteg
               do j=1,nbins
                  ymax(j,k)=max(ymax(j,k),xymax(j,k))
                  ymaxrm(j,k)=max(ymaxrm(j,k),xymaxrm(j,k))
               enddo
            enddo
            do j=1,8
               rtot(2,j)=sqrt((rtot(2,j)**2*(jfound-1)**2+tot(2,j)**2)
     1              /jfound**2+(jfound-1)*(rtot(1,j)-tot(1,j))**2/
     2              (jfound**3*ncall2))
               rtot(1,j)=(rtot(1,j)*(jfound-1)+tot(1,j))/jfound
            enddo
         endif
         rad_totbtl     =rtot(1,1)
         rad_etotbtl    =rtot(2,1)
         rad_totabsbtl  =rtot(1,2)
         rad_etotabsbtl =rtot(2,2)
         rad_totposbtl  =rtot(1,3)
         rad_etotposbtl =rtot(2,3)
         rad_totnegbtl  =rtot(1,4)
         rad_etotnegbtl =rtot(2,4)
         rad_totrm      =rtot(1,5)
         rad_etotrm     =rtot(2,5)
         rad_totbtlgen  =rtot(1,6)
         rad_etotbtlgen =rtot(2,6)
         rad_totgen     =rtot(1,7)
         rad_etotgen    =rtot(2,7)
         rad_tot        =rtot(1,8)
         rad_etot       =rtot(2,8)
         close(iun)
 111     continue
      enddo
      if(filefound) return
 998  continue
      iret=-1
      end

      subroutine wj_storexgrid(xgrid,xint,xgridrm,xintrm)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_pdf.h'
      include 'src/Lib/wj/pwhg_rad.h'
      real * 8 xgrid(0:50,ndiminteg),xgridrm(0:50,ndiminteg),
     1     xint,xintrm
      integer nbins
      parameter (nbins=50)
      character * 20 pwgprefix
      integer lprefix
      common/wj_cpwgprefix/pwgprefix,lprefix
      integer j,k,iun
      call wj_newunit(iun)
      open(unit=iun,file=pwgprefix(1:lprefix)//'xgrid.dat',
     #     form='unformatted',status='unknown')
      write(iun) ((xgrid(j,k),k=1,ndiminteg),j=0,nbins),xint
      write(iun) ((xgridrm(j,k),k=1,ndiminteg),j=0,nbins),xintrm
      write(iun) kn_sbeams, pdf_ih1, pdf_ih2, pdf_ndns1, pdf_ndns2
      close(iun)
      end

      subroutine wj_loadxgrid(iret,xgrid,xint,xgridrm,xintrm)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_pdf.h'
      include 'src/Lib/wj/pwhg_rad.h'
      real * 8 xgrid(0:50,ndiminteg),xgridrm(0:50,ndiminteg),
     1     xint,xintrm
      integer iret
c
      integer ios
      integer nbins
      parameter (nbins=50)
      character * 20 pwgprefix
      integer lprefix
      common/wj_cpwgprefix/pwgprefix,lprefix
      real * 8 shx
      integer ih1x, ih2x, ndns1x, ndns2x
      integer j,k,iun
      real * 8 wj_powheginput
      external wj_powheginput
      if(wj_powheginput('use-old-grid').eq.0) then
         iret=1
         return
      endif
      call wj_newunit(iun)
      open(unit=iun,file=pwgprefix(1:lprefix)//'xgrid.dat',
     #     form='unformatted',status='old',iostat=ios)
      if(ios.ne.0) then
         iret=-1
         return
      endif
      read(iun,iostat=ios) ((xgrid(j,k),k=1,ndiminteg),j=0,nbins),xint
      read(iun,iostat=ios) ((xgridrm(j,k),k=1,ndiminteg),j=0,nbins),
     1     xintrm
      read(iun,iostat=ios) shx, ih1x, ih2x, ndns1x, ndns2x
      if(shx.ne.kn_sbeams.or.ih1x.ne.pdf_ih1.or.ih2x.ne.pdf_ih2
     #  .or.ndns1x.ne.pdf_ndns1.or.ndns2x.ne.pdf_ndns2.or.ios.ne.0) then
         iret=-1
         close(iun)
         return
      endif
      close(iun)
      iret=0
      end
