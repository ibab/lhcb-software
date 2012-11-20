      subroutine zj_sigvirtual(virt_arr)
      implicit none
      include 'src/Lib/zj/nlegborn.h'
      include 'src/Lib/zj/pwhg_flst.h'
      include 'src/Lib/zj/pwhg_kn.h'
      include 'src/Lib/zj/pwhg_br.h'
      include 'src/Lib/zj/pwhg_flg.h'
      real * 8 virt_arr(maxprocborn)
      integer equivto(maxprocborn)
      real * 8 equivcoef(maxprocborn)
      integer nmomset
      parameter (nmomset=10)
      real * 8 pborn(0:3,nlegborn,nmomset),cprop
      real * 8 virtual(nmomset,maxprocborn)
      integer iborn,ibornpr,mu,nu,k,j,iret
      logical ini
      data ini/.true./
      save ini,equivto,equivcoef
      logical zj_pwhg_isfinite
      external zj_pwhg_isfinite
      if(ini) then
         do iborn=1,flst_nborn
            equivto(iborn)=-1
         enddo
         if(flg_smartsig) then
            call zj_randomsave
            call zj_fillmomenta(nlegborn,nmomset,kn_masses,pborn)
            do iborn=1,flst_nborn
               do j=1,nmomset
                  call zj_setvirtual(pborn(0,1,j),flst_born(1,iborn),
     #                 virtual(j,iborn))
c     check if virtual(j,iborn) is finite
                  if (.not.zj_pwhg_isfinite(virtual(j,iborn))) 
     #                 virtual(j,iborn)=0d0
               enddo
               call zj_compare_vecsv(nmomset,iborn,virtual,ibornpr,
     #              cprop,iret)
               if(iret.eq.0) then
                  equivto(iborn)=ibornpr
                  equivcoef(iborn)=1
               elseif(iret.eq.1) then
                  equivto(iborn)=ibornpr
                  equivcoef(iborn)=cprop
               endif
            enddo
            call zj_randomrestore
         endif
         ini=.false.
      endif
      do iborn=1,flst_nborn
         if(equivto(iborn).lt.0) then
            call zj_setvirtual(kn_cmpborn,flst_born(1,iborn),
     #           virt_arr(iborn))
c     check if virt_arr(iborn) is finite
                  if (.not.zj_pwhg_isfinite(virt_arr(iborn))) 
     #                 virt_arr(iborn)=0d0
            virt_arr(iborn)=virt_arr(iborn)/(2*kn_sborn)
         else
            virt_arr(iborn)=virt_arr(equivto(iborn))*equivcoef(iborn)
         endif
      enddo
      end

      subroutine zj_compare_vecsv(nmomset,iborn,res,ibornpr,cprop,iret)
      implicit none
      real * 8 ep
      parameter (ep=1d-12)
      integer nmomset,iborn,ibornpr,iret,j,k
      real * 8 res(nmomset,iborn),cprop,rat
      do j=1,iborn-1
         rat=res(1,iborn)/res(1,j)
         do k=1,nmomset
            if(abs(1-res(k,iborn)/res(k,j)/rat).gt.ep) goto 10
         enddo
         if(abs(1-rat).lt.ep) then
            iret=0
            cprop=1
         else
            iret=1
            cprop=rat
         endif
         ibornpr=j
         return
 10      continue
      enddo
      iret=-1
      end


