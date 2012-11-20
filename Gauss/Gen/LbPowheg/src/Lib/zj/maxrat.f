      subroutine zj_do_maxrat(mcalls,icalls)
      implicit none
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_rad.h'
      include 'src/Lib/zj/pwhg_rnd.h'
      integer mcalls,icalls
      character * 20 pwgprefix
      integer lprefix
      common/zj_cpwgprefix/pwgprefix,lprefix
      character * 4 chseed
      integer nubound,ios,iun,nynormsold,ncsinormsold,
     #  icsi,iy,j,k,jfile,nfiles
      real * 8 csiynorms(rad_ncsiynormsmx,
     1     rad_ncsiynormsmx,nlegborn-1,maxprocborn)
      logical lpresent,loaded,manyfiles
      real * 8 zj_powheginput,zj_random
      external zj_powheginput,zj_random      
      call zj_zerohistnorms
      loaded=.false.
      nubound=zj_powheginput('nubound')
      rad_nynorms=zj_powheginput('#iymax')
      if(rad_nynorms.lt.0) rad_nynorms=1
      rad_ncsinorms=zj_powheginput('#icsimax')
      if(rad_ncsinorms.lt.0) rad_ncsinorms=1
      if(rad_nynorms.gt.rad_ncsiynormsmx.or.
     #    rad_ncsinorms.gt.rad_ncsiynormsmx) then
         write(*,*)
     #  ' error in poweg.input: rad_nynorms or rad_ncsinorms too large'
         stop
      endif
      rad_normfact=zj_powheginput('#xupbound')
      if(rad_normfact.lt.0) rad_normfact=2
      if(nubound.eq.0) then
         write(*,*) ' ubound set =0, cannot proceed'
         call exit(1)
      endif
      call zj_newunit(iun)
      if(zj_powheginput('use-old-ubound').eq.1) then
         call zj_newunit(iun)
         open(unit=iun,file=pwgprefix(1:lprefix)//'ubound.dat',
     1        status='old',iostat=ios)
         if(ios.eq.0) then
            nfiles=1
         else
            if(rnd_cwhichseed.ne.'none') then
c Are we required to generate a grid file or to load a bunch of them?
c See if the grid file exists already
               inquire(file=pwgprefix(1:lprefix)//'ubound-'//
     1           rnd_cwhichseed//'.dat',exist=lpresent)
               if(.not.lpresent) then
c we must generate it: go to grid computation
                  goto 111
               endif
            endif
c try sequence of number files
            nfiles=9999
            manyfiles=.true.
         endif
      else
c new grid required: go to grid computation
         goto 111
      endif
      do jfile=1,nfiles
         if(nfiles.ne.1) then
c sequence of number files
            write(chseed,'(i4)') jfile
            do j=1,4
               if(chseed(j:j).eq.' ') chseed(j:j)='0'
            enddo
            inquire(file=pwgprefix(1:lprefix)//'ubound-'//
     1           chseed//'.dat',exist=lpresent)
c This one is not present: skip it and go to the next
            if(.not.lpresent) goto 110
            open(unit=iun,file=pwgprefix(1:lprefix)//'ubound-'//
     1           chseed//'.dat',  status='old',iostat=ios)
c error in loading; assume that grids have to be redone
            if(ios.ne.0) goto 111
            write(*,*) ' opened ',pwgprefix(1:lprefix)//'ubound-'//
     1           chseed//'.dat'
         endif
c rad_ncsinorms, rad_nynorms will be overridden by nynormsold,ncsinormsold
c if all goes well
         read(iun,fmt=*,iostat=ios,end=111) nynormsold,ncsinormsold
         do j=1,flst_nborn
            read(iun,fmt=*,iostat=ios,end=111)
            do iy=1,nynormsold
               read(iun,fmt=*,iostat=ios,end=111)
               do icsi=1,ncsinormsold
                  read(iun,fmt=*,iostat=ios,end=111)
                  read(iun,fmt=*,iostat=ios,end=111)
     1                 (csiynorms(icsi,iy,k,j),k=1,rad_nkinreg)
               enddo
            enddo
         enddo
         close(iun)
         do iy=1,nynormsold
            do icsi=1,ncsinormsold
               do j=1,flst_nborn
                  do k=1,rad_nkinreg
                     if(jfile.eq.1) then
                        rad_csiynorms(icsi,iy,k,j)=
     1                       csiynorms(icsi,iy,k,j)
                     else
                        rad_csiynorms(icsi,iy,k,j)=max(
     1                       rad_csiynorms(icsi,iy,k,j),
     2                       csiynorms(icsi,iy,k,j))
                     endif
                  enddo
               enddo
            enddo
         enddo
c all went well: override rad_ncsinorms, rad_nynorms
         rad_ncsinorms=ncsinormsold
         rad_nynorms=nynormsold
         loaded=.true.
 110     continue
      enddo
      if(loaded) then
         write(*,*)
     1 ' normalization of upper bounding function for radiation',
     2 ' successfully loaded'
         goto 998      
      endif
c Compute and store normalization grid
 111  continue
      write(*,*)
      write(*,*)
     # ' POWHEG: computing normalization of upper'//
     # ' bounding function for radiation'
      do j=1,flst_nborn
         do k=1,rad_nkinreg
            do iy=1,rad_nynorms
               do icsi=1,rad_ncsinorms
                  rad_csiynorms(icsi,iy,k,j)=0
               enddo
            enddo
            rad_norms(k,j)=0
         enddo
      enddo
      do j=1,nubound
         call zj_gen_born(mcalls,icalls)
         call zj_gen_uborn_idx
         kn_csitilde=zj_random()
         kn_y=2*zj_random()-1
         kn_azi=2*pi*zj_random()
         do rad_kinreg=1,rad_nkinreg
            if(rad_kinreg.eq.1.and.rad_kinreg_on(1)) then
c     initial state radiation
               call zj_gen_real_phsp_isr_rad0
               call zj_inc_norms
            elseif(rad_kinreg_on(rad_kinreg)) then
c     final state radiation
               call zj_gen_real_phsp_fsr_rad0
               call zj_inc_norms
            endif
         enddo    
      enddo
      call zj_newunit(iun)
      if(rnd_cwhichseed.eq.'none') then
         open(unit=iun,file=pwgprefix(1:lprefix)//'ubound.dat',
     1        status='unknown')
      else
         open(unit=iun,file=pwgprefix(1:lprefix)//'ubound-'
     1        //rnd_cwhichseed//'.dat',status='unknown')
      endif
      write(iun,*) rad_nynorms,rad_ncsinorms, '    n-y and n-csi grid'
      do j=1,flst_nborn
         write(iun,fmt=*,iostat=ios) j,'   Born index'
         do iy=1,rad_nynorms
            write(iun,fmt=*,iostat=ios) iy, '    i-y'
            do icsi=1,rad_ncsinorms
               write(iun,fmt=*,iostat=ios) icsi, '    i-csi'
               write(iun,fmt=*,iostat=ios)
     #(rad_csiynorms(icsi,iy,k,j),k=1,rad_nkinreg)
            enddo
         enddo
      enddo
      close(iun)
      write(*,*) ' Normalization of upper bounding function'//
     1 ' for radiation computed and stored'
      
      call zj_printhistnorms
 998  do j=1,flst_nborn
         do k=1,rad_nkinreg
            do iy=1,rad_nynorms
               do icsi=1,rad_ncsinorms
                  rad_csiynorms(icsi,iy,k,j)=
     #            rad_csiynorms(icsi,iy,k,j)*rad_normfact
                  rad_norms(k,j)=max(rad_norms(k,j),
     #                           rad_csiynorms(icsi,iy,k,j))
               enddo
            enddo
         enddo
      enddo
c this subroutine identifies the upper bounds for identical
c contributions
c      call maxratident
      end


      subroutine zj_inc_norms
      implicit none
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_rad.h'
      real * 8 ptsq,born,sig,xnorm
      real * 8 histnorms(nlegborn-1,maxprocborn,100)
      real * 8 efficiencies(nlegborn-1,maxprocborn)
      common/zj_chistnorms/histnorms,efficiencies
      integer icsi,iy,ind,k
      logical zj_refuse_pdf
      real * 8 zj_pwhg_pt2,zj_pwhg_upperb_rad
      external zj_pwhg_pt2,zj_pwhg_upperb_rad
      logical debug
      parameter (debug=.false.)
ccccccccccccccccccccccccccccccccc
      logical verbose
      parameter (verbose=.false.)
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_pdf.h'
      include 'src/Lib/zj/pwhg_br.h'
      real * 8 pdf1(-6:6),pdf2(-6:6)
cccccccccccccccccccccccccccccccc
      ptsq=zj_pwhg_pt2()
      if(ptsq.gt.rad_ptsqmin) then
         call zj_set_rad_scales(ptsq)
         if(.not.zj_refuse_pdf()) then
            call zj_sigborn_rad(born)
            call zj_sigreal_rad(sig)
            sig=sig*kn_jacreal
            if(born.le.0) then
               if(verbose) then
               write(*,*) ' *****************************'
               write(*,*) ' zj_inc_norms: Warning, born zero'
               write(*,*) ' RAD REG: ', rad_kinreg
               write(*,*) ' zj_inc_norms: x1,x2', kn_xb1,kn_xb2
               write(*,*) ' muf= ',sqrt(st_mufact2)
               write(*,*) ' Flavour: ', (flst_born(ind,rad_ubornidx),ind
     $              =1,5) 
               
               write(*,*) ' Born w/o pdf: ',br_born(rad_ubornidx)
     $              ,'  jac: ',kn_jacborn 
               call zj_pdfcall(1,kn_xb1,pdf1)
               call zj_pdfcall(2,kn_xb2,pdf2)
               write(*,*) " PDF's ",pdf1(flst_born(1
     $              ,rad_ubornidx)),pdf2(flst_born(2,rad_ubornidx))

               if ((abs(flst_born(1
     $              ,rad_ubornidx)).ge.4).or.(abs(flst_born(2
     $              ,rad_ubornidx)).ge.4)) then
                  
                  write(*,*) ' zj_inc_norms: hvq in initial state!'
               else
                  write(*,*) ' zj_inc_norms: more serious problem!'
               endif
               write(*,*) ' *****************************'
               endif
               return
            endif
            xnorm=sig/born/zj_pwhg_upperb_rad()
            if (debug) then
               if(xnorm.gt.0.1) then
                  write(*,*) ' **********************'
                  write(*,*) 
     $                 ' event with abnormally high ratio to bound:'
     $                 ,xnorm
                  call zj_printevent
                  do k=1,10000
                     write(*,*) ' enter csitilde,y'
                     read(*,*) kn_csitilde, kn_y
                     call zj_gen_real_phsp_isr_rad0
                     ptsq=zj_pwhg_pt2()
                     if(ptsq.gt.rad_ptsqmin) then
                        call zj_set_rad_scales(ptsq)
                        call zj_sigreal_rad(sig)
                        sig=sig*kn_jacreal
                        xnorm=sig/born/zj_pwhg_upperb_rad()
                        write(*,*) 'xnorm:',
     $                       xnorm
                        call zj_printevent
                     endif
                  enddo
               endif
               if(xnorm.gt.3e-5.and.xnorm.lt.3.1e-5) then
                  write(*,*) ' **********************'
                  write(*,*) ' typical event:',
     $                 xnorm
                  call zj_printevent
               endif
            endif
         else
            return
         endif
      else
         return            
      endif
c     fill the norm array
      iy=abs(kn_y)*rad_nynorms+1
      icsi=log(1/(1-kn_csi))/log(kn_sbeams/kn_minmass**2)
     1    *rad_ncsinorms+1
c
      if(iy.lt.1) iy=1
      if(iy.gt.rad_nynorms) iy=rad_nynorms
      if(icsi.lt.1) icsi=1
      if(icsi.gt.rad_ncsinorms) icsi=rad_ncsinorms
c
      rad_csiynorms(icsi,iy,rad_kinreg,rad_ubornidx)=
     #  max(xnorm,rad_csiynorms(icsi,iy,rad_kinreg,rad_ubornidx))
      rad_norms(rad_kinreg,rad_ubornidx)=
     #  max(xnorm,rad_norms(rad_kinreg,rad_ubornidx))
      if(xnorm.gt.0) then
         ind=log(xnorm)+50
      else
         ind=-100
      endif
      ind=max(ind,1)
      ind=min(ind,100)
      efficiencies(rad_kinreg,rad_ubornidx)=
     #    efficiencies(rad_kinreg,rad_ubornidx)+xnorm
      histnorms(rad_kinreg,rad_ubornidx,ind)=
     #    histnorms(rad_kinreg,rad_ubornidx,ind)+1
      end


      subroutine zj_printevent
      implicit none
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_rad.h'
      real * 8 zj_pwhg_pt2
      external zj_pwhg_pt2
      integer j,k,alr,mu
      write(*,*) 'kinematic region (1=isr,>2=fsr)',rad_kinreg
      write(*,*) ' born flavour:'
      write(*,*)
     # (flst_born(j,rad_ubornidx),j=1,nlegborn)
      write(*,*) ' real flavour:'
      do k=1,flst_born2alr(0,rad_ubornidx)
         alr=flst_born2alr(k,rad_ubornidx)
         write(*,*) (flst_alr(j,alr),j=1,nlegreal)
      enddo
      write(*,*) ' pt', sqrt(zj_pwhg_pt2())
      write(*,*) ' x1,x2 = ', kn_x1, kn_x2
      write(*,*) ' underlying born x1,x2', kn_xb1, kn_xb2
      write(*,*) ' y, csi:', kn_y,kn_csi
c      write(*,*) ' born pt', sqrt(kn_born_pt2)
c      write(*,*) ' cosine decay angle', kn_cthdec
      write(*,*) ' born cm kinematics'
      do k=1,nlegborn
         write(*,100) (kn_cmpborn(mu,k),mu=0,3)
      enddo
 100  format('[',3(d14.8,','),d14.8,'],')
      end





      subroutine zj_printhistnorms
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_rad.h'
      real * 8 ptsq,born,sig,xnorm
      real * 8 histnorms(nlegborn-1,maxprocborn,100)
      real * 8 efficiencies(nlegborn-1,maxprocborn)
      common/zj_chistnorms/histnorms,efficiencies
      integer iun,ireg,iborn,j
      real * 8 x1,x2,xm,dx
      real * 8 zj_powheginput
      external zj_powheginput
      call zj_newunit(iun)
      open(iun,status='unknown',file='pwghistnorms.top')
      do ireg=1,rad_nkinreg
         do iborn=1,flst_nborn
            write(iun,*)'set zj_scale x log'
            write(iun,*)'set order x dx y'
            write(iun,*)'set limits x 1e-14 1e14'
            write(iun,*)'title top "region=',ireg,', born=',iborn,'"'
            do j=1,100
               x1=exp(j-50d0)
               x2=exp(j+1-50d0)
               xm=(x1+x2)/2
               dx=x2-xm
               write(iun,*) xm,dx,histnorms(ireg,iborn,j)
            enddo
            write(iun,*)' hist'
            if (rad_norms(ireg,iborn).ne.0d0) then
               write(iun,*) 'title bottom " efficiency',
     1              efficiencies(ireg,iborn)/zj_powheginput('nubound')/
     2              rad_norms(ireg,iborn),'"'
            endif
            write(iun,*)' newplot'
         enddo
      enddo
c      do ireg=1,rad_nkinreg
c         do iborn=1,flst_nborn
c            write(iun,*)'( efficiencies: region=',ireg,', born=',iborn 
c            write(iun,*) '(', efficiencies(ireg,iborn)/
c     1           zj_powheginput('nubound')/rad_norms(ireg,iborn)
c         enddo
c      enddo
      close(iun)
      end

      subroutine zj_zerohistnorms
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_rad.h'
      real * 8 ptsq,born,sig,xnorm
      real * 8 histnorms(nlegborn-1,maxprocborn,100)
      real * 8 efficiencies(nlegborn-1,maxprocborn)
      common/zj_chistnorms/histnorms,efficiencies
      integer ireg,iborn,j
      do ireg=1,rad_nkinreg
         do iborn=1,flst_nborn
            efficiencies(ireg,iborn)=0
            do j=1,100
               histnorms(ireg,iborn,j)=0
            enddo
         enddo
      enddo
      end

      function zj_refuse_pdf()
c Some pdf have (inconsistently) vanishing quark density at x values
c when the gluon density is not zero; cannot use these points to compute
c maxrat
      implicit none
      logical zj_refuse_pdf
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_rad.h'
      include 'src/Lib/zj/pwhg_st.h'
      include 'src/Lib/zj/pwhg_pdf.h'
      real * 8 pdf(-6:6),sn,suppfc,x
      integer k,j,fl
      suppfc(x)=(1-x)*x
c in FKS, for final state radiation, pdf always cancel in real/born
      if(rad_kinreg.ne.1) then
         zj_refuse_pdf=.false.
         return
      endif
      if(st_mufact2.lt.2.or.kn_x1.gt.0.9.or.kn_x2.gt.0.9) then
         zj_refuse_pdf=.true.
         return
      endif
      do k=1,2
         if(k.eq.1) then
            fl=flst_born(1,rad_ubornidx)
            x=kn_xb1
            call zj_pdfcall(1,x,pdf)
         else
            fl=flst_born(2,rad_ubornidx)
            x=kn_xb2
            call zj_pdfcall(2,x,pdf)
         endif
         sn=0
         do j=1,5
            sn=sn+pdf(j)+pdf(-j)
         enddo
         if(fl.eq.0) then
            if(sn*suppfc(x).gt.30*pdf(0)) then
               zj_refuse_pdf=.true.
               return
            endif
         else
            if(pdf(0)*suppfc(x).gt.30*pdf(fl)) then
               zj_refuse_pdf=.true.
               return
            endif
         endif
      enddo
      zj_refuse_pdf=.false.
      end



      subroutine zj_uboundfct(fct,csi,y)
      implicit none
      include 'src/Lib/zj/pwhg_math.h'
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_rad.h'
      real * 8 fct,csi,y
      real * 8 unorm
      integer iy,icsi
      iy=abs(y)*rad_nynorms+1
      icsi=log(1/(1-csi))/log(kn_sbeams/kn_minmass**2)*rad_ncsinorms+1
c
      if(iy.lt.1) iy=1
      if(iy.gt.rad_nynorms) iy=rad_nynorms
      if(icsi.lt.1) icsi=1
      if(icsi.gt.rad_ncsinorms) icsi=rad_ncsinorms
c
      unorm=rad_norms(rad_kinreg,rad_ubornidx)
      if(unorm.ne.0) then
         fct=rad_csiynorms(icsi,iy,rad_kinreg,rad_ubornidx)/unorm
      else
         fct=1
      endif
      end
      


      subroutine zj_gen_born(mcalls,icalls)
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_flg.h'
      integer mcalls,icalls
      integer ndimbornint
      parameter (ndimbornint=ndiminteg-3)
      real * 8 xgrid(0:50,ndimbornint),ymax(50,ndimbornint)
     1     ,xmmm(0:50,ndimbornint),xint,sigbtl,errbtl
      integer ifold(ndimbornint),ncall1,ncall2,
     1     itmx1,itmx2,imode,j,k,iun
      character * 20 pwgprefix
      integer lprefix
      common/zj_cpwgprefix/pwgprefix,lprefix
      logical savenlotest,ini
      data ini/.true./
      real * 8 xx(ndimbornint)      
      real * 8 zj_btilde,zj_powheginput
      external zj_btilde,zj_powheginput
      save xgrid,ymax,xmmm,ini
      if(flg_bornonly) then
         call zj_gen_btilde(mcalls,icalls)
         return
      endif
      flg_bornonly=.true.
c
      savenlotest=flg_nlotest
      flg_nlotest=.false.
      if(ini) then
         write(*,'(/,a,/,a)') 
     1       ' Initializing grids for the generation of the underlying',
     1       ' Born configurations according to the Born cross section'
c set up the grids
         do j=1,ndimbornint
            do k=0,50
               xgrid(k,j)=0
               xmmm(k,j)=0
            enddo
            do k=1,50
               ymax(k,j)=0
            enddo
            ifold(j)=1
         enddo
         call zj_newunit(iun)
         open(unit=iun,file=pwgprefix(1:lprefix)//'borngrid.top',
     1        status='unknown')
         imode=0
         ncall1=zj_powheginput('ncall1')
         itmx1=zj_powheginput('itmx1')
c This is not really needed; the accumulated totals
c will not be used;
         call zj_resettotals
         call zj_mint(zj_btilde,ndimbornint,ncall1,itmx1,ifold,imode,iun,
     1        xgrid,xint,ymax,sigbtl,errbtl)
         close(iun)
         imode=1
         ncall2=zj_powheginput('ncall2')
         itmx2=zj_powheginput('itmx2')
         call zj_mint(zj_btilde,ndimbornint,ncall2,itmx2,ifold,imode,iun,
     1        xgrid,xint,ymax,sigbtl,errbtl)
         call zj_gen(zj_btilde,ndimbornint,xgrid,ymax,xmmm,ifold,0,
     1        mcalls,icalls,xx)
         ini=.false.
      endif
C - N.B. if -fno-automatic isn't used the ifold array may need reset to 1 here.
      call zj_gen(zj_btilde,ndimbornint,xgrid,ymax,xmmm,ifold,1,
     #    mcalls,icalls,xx)
      flg_nlotest=savenlotest
      flg_bornonly=.false.
      end
