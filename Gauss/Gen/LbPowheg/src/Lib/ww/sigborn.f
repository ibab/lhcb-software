      subroutine ww_btildeborn(res)
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      include 'src/Lib/ww/pwhg_br.h'
      include 'src/Lib/ww/pwhg_flg.h'
      real * 8 pdf1(-6:6),pdf2(-6:6)
      real * 8 res(flst_nborn),tot
      integer j
      call ww_pdfcall(1,kn_xb1,pdf1)
      call ww_pdfcall(2,kn_xb2,pdf2)
      tot=0
      do j=1,flst_nborn
         res(j)=br_born(j) *
     #  pdf1(flst_born(1,j))*pdf2(flst_born(2,j))*kn_jacborn
         tot=tot+res(j)
c     if one wants to do only the integration over the whole phase space, then
c     uncomment the following (with or without the flux factor 1/(2*kn_sborn)
c        
c         res(j)=kn_jacborn/(2*kn_sborn)/flst_nborn
      enddo
      end

      subroutine ww_sigborn_rad(born)
      implicit none
      real * 8 born
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      include 'src/Lib/ww/pwhg_br.h'
      include 'src/Lib/ww/pwhg_rad.h'
      real * 8 pdf1(-6:6),pdf2(-6:6),bornjk(nlegborn,nlegborn),
     #         bmunu(0:3,0:3,nlegborn)
      call ww_pdfcall(1,kn_xb1,pdf1)
      call ww_pdfcall(2,kn_xb2,pdf2)
      call ww_setborn0(kn_cmpborn,flst_born(1,rad_ubornidx),born,
     #        bornjk,bmunu)
      born=born *
     #  pdf1(flst_born(1,rad_ubornidx))*pdf2(flst_born(2,rad_ubornidx))
      end

     

      subroutine ww_allborn
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      include 'src/Lib/ww/pwhg_br.h'
      include 'src/Lib/ww/pwhg_flg.h'
      integer equivto(maxprocborn)
      real * 8 equivcoef(maxprocborn)
      integer nmomset
      parameter (nmomset=10)
      real * 8 pborn(0:3,nlegborn,nmomset),cprop
      real * 8 born(nmomset,maxprocborn)
      real * 8 bornjk(nlegborn,nlegborn,nmomset,maxprocborn)
      real * 8 bmunu(0:3,0:3,nlegborn,nmomset,maxprocborn)
      integer iborn,ibornpr,mu,nu,k,j,iret
      logical ini
      data ini/.true./
      save ini,equivto,equivcoef
      if(ini) then
         do iborn=1,flst_nborn
            equivto(iborn)=-1
         enddo
         if(flg_smartsig) then
            call ww_randomsave
            call ww_fillmomenta(nlegborn,nmomset,kn_masses,pborn)
            do iborn=1,flst_nborn
               do j=1,nmomset
                  call ww_setborn0(pborn(0,1,j),flst_born(1,iborn),
     1                 born(j,iborn),bornjk(1,1,j,iborn),
     2                 bmunu(0,0,1,j,iborn))
               enddo
               call ww_compare_vecsb(nmomset,iborn,born,bornjk,bmunu,
     1              ibornpr,cprop,iret)
               if(iret.eq.0) then
                  equivto(iborn)=ibornpr
                  equivcoef(iborn)=1
               elseif(iret.eq.1) then
                  equivto(iborn)=ibornpr
                  equivcoef(iborn)=cprop
               endif
            enddo
            call ww_randomrestore
         endif
         ini=.false.
      endif
      do iborn=1,flst_nborn
         if(equivto(iborn).lt.0) then
            call ww_setborn0(kn_cmpborn,flst_born(1,iborn),br_born(iborn),
     #        br_bornjk(1,1,iborn),br_bmunu(0,0,1,iborn))
         else
            br_born(iborn)=br_born(equivto(iborn))*equivcoef(iborn)
            do j=1,nlegborn
               do k=1,nlegborn
                  br_bornjk(j,k,iborn)=br_bornjk(j,k,equivto(iborn))
     #*equivcoef(iborn)
               enddo
            enddo
            do mu=0,3
               do nu=0,3
                  do j=1,nlegborn
                     br_bmunu(mu,nu,j,iborn)=
     #                   br_bmunu(mu,nu,j,equivto(iborn))
     #                   *equivcoef(iborn)
                  enddo
               enddo
            enddo
         endif
      enddo
      end

      subroutine ww_compare_vecsb(nmomset,iborn,born,bornjk,bmunu,
     1     ibornpr,cprop,iret)
      implicit none
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      real * 8 ep
      parameter (ep=1d-12)
      integer nmomset,iborn,ibornpr,iret,jborn,k,jleg,kleg,mu,nu
      real * 8 born(nmomset,iborn),cprop,rat,resi,resj
      real * 8 bornjk(nlegborn,nlegborn,nmomset,maxprocborn)
      real * 8 bmunu(0:3,0:3,nlegborn,nmomset,maxprocborn)
      do jborn=1,iborn-1
         rat=born(1,iborn)/born(1,jborn)
         do k=1,nmomset
            resi=born(k,iborn)
            resj=born(k,jborn)
            if(abs(1-resi/resj/rat).gt.ep) goto 10
         enddo
c totals are identical; see if also colour correlated are
         do jleg=1,nlegborn
            do kleg=1,nlegborn
               do k=1,nmomset
                  resi=bornjk(jleg,kleg,k,iborn)
                  resj=bornjk(jleg,kleg,k,jborn)
c some of these are zero, be careful
                  if(resi.ne.resj*rat) then
                     if(abs(1-resi/resj/rat).gt.ep) goto 10
                  endif
               enddo
            enddo
         enddo
c totals are identical; see if also spin correlated are
         do jleg=1,nlegborn
            do mu=0,3
               do nu=0,3
                  do k=1,nmomset
                     resi=bmunu(mu,nu,jleg,k,iborn)
                     resj=bmunu(mu,nu,jleg,k,jborn)
c some of these are zero, be careful
                     if(resi.ne.resj*rat) then
                        if(abs(1-resi/resj/rat).gt.ep) goto 10
                     endif
                  enddo
               enddo
            enddo
         enddo
         if(abs(1-rat).lt.ep) then
            iret=0
            cprop=1
         else
            iret=1
            cprop=rat
         endif
         ibornpr=jborn
         return
 10      continue
      enddo
      iret=-1
      end

      subroutine ww_setborn0(p,bflav,born,bornjk,bmunu)
c provide the flux factor to the user Born routine
      implicit none
      include 'src/Lib/ww/pwhg_math.h'
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs)
      integer bflav(nlegs)
      real * 8 born,bornjk(nlegs,nlegs),bmunu(0:3,0:3,nlegs)
      integer j,k,mu,nu
      logical ww_pwhg_isfinite
      external ww_pwhg_isfinite
      call ww_setborn(p,bflav,born,bornjk,bmunu)
c     check if born, bornjk and bmunu are finite
      if (.not.ww_pwhg_isfinite(born)) born=0d0
      do j=1,nlegs
         do mu=0,3
            do nu=0,3
               if (.not.ww_pwhg_isfinite(bmunu(mu,nu,j))) 
     $               bmunu(mu,nu,j)=0d0
            enddo
         enddo
      enddo
      do j=1,nlegs
         do k=1,nlegs
            if (.not.ww_pwhg_isfinite(bornjk(j,k))) bornjk(j,k)=0d0
         enddo
      enddo
c
      born=born/(2*kn_sborn)
      do j=1,nlegs
         do mu=0,3
            do nu=0,3
               bmunu(mu,nu,j)=bmunu(mu,nu,j)/(2*kn_sborn)
            enddo
         enddo
      enddo
      do j=1,nlegs
         do k=1,nlegs
            bornjk(j,k)=bornjk(j,k)/(2*kn_sborn)
         enddo
      enddo
      end
