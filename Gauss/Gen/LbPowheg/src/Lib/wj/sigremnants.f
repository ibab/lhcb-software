c Function to be integrated with wj_mint, used to generate
c non-singular contributions using standard wj_mint-wj_gen method
c These contributions arise from real graphs without singular regions,
c or, when damping of R/B value is adopted, from the remnant of the
c damping
      function wj_sigremnant(xx,ww,ifirst)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/pwhg_flg.h'
      include 'src/Lib/wj/pwhg_math.h'
      real * 8 wj_sigremnant,xx(ndiminteg),ww
      integer ifirst
      real * 8 xrad(3)
      real * 8 xborn(ndiminteg-3)
      integer j,alr
      real * 8 ttt
      real * 8 jac_over_csi,jac_over_csi_p,jac_over_csi_m,
     #         jac_over_csi_s,jac_over_csi_coll
      real * 8 xjac,suppfact
      logical wj_valid_emitter
      external wj_valid_emitter
      if(ifirst.eq.2) then
         wj_sigremnant=rad_reg_tot+rad_damp_rem_tot
         if(flg_nlotest) call wj_pwhgaccumup
         return
      endif
      do j=1,ndiminteg-3
         xborn(j)=xx(j)
      enddo
      do j=1,3
         xrad(j)=xx(ndiminteg-3 + j)
         rad_xradremn(j)=xrad(j)
      enddo
c regular contributions; any phase space parametrization should be OK
      kn_emitter=0
      call wj_gen_born_phsp(xborn)
c set scales
      call wj_setscalesbtilde
c the following is needed to compute wj_soft and collinear limits
      call wj_allborn
      call wj_born_suppression(suppfact)
      if(flg_withreg.or.(flg_withdamp.and.(
     # wj_valid_emitter(0).or.wj_valid_emitter(1).or.wj_valid_emitter(2)))) then
         call wj_gen_real_phsp_isr(xrad,
     #    jac_over_csi,jac_over_csi_p,jac_over_csi_m,jac_over_csi_s)
         xjac=jac_over_csi*kn_csi*kn_csimax*kn_jacborn*suppfact*ww*hc2
      endif
      if(flg_withreg) then
         call wj_sigreal_reg(xjac,rad_reg_tot,rad_reg_arr)
         if(flg_nlotest) then
            call wj_analysis_driver(rad_reg_tot/suppfact,1)
         endif
      else
         rad_reg_tot=0
      endif
      if(flg_withdamp) then
         rad_damp_rem_tot=0
         do alr=1,flst_nalr
            rad_damp_rem_arr(alr)=0
         enddo
         do kn_emitter=0,nlegborn
            if(wj_valid_emitter(kn_emitter)) then
               if(kn_emitter.le.2) then
c     No need to generate phase space; it is already available
                  call wj_sigreal_damp_rem(xjac,ttt,rad_damp_rem_arr)
                  if(flg_nlotest) then
                     call wj_analysis_driver(ttt/suppfact,1)
                  endif
                  rad_damp_rem_tot=rad_damp_rem_tot+ttt
               else
                  call wj_gen_real_phsp_fsr(xrad,
     #jac_over_csi,jac_over_csi_coll,jac_over_csi_s)
                  xjac=jac_over_csi*kn_csi*kn_csimax
     #                *kn_jacborn*suppfact*ww*hc2
                  call wj_sigreal_damp_rem(xjac,ttt,rad_damp_rem_arr)
                  if(flg_nlotest) then
                     call wj_analysis_driver(ttt/suppfact,1)
                  endif
                  rad_damp_rem_tot=rad_damp_rem_tot+ttt
               endif
            endif
         enddo
      else
         rad_damp_rem_tot=0
      endif
      wj_sigremnant=rad_reg_tot+rad_damp_rem_tot
      end

      subroutine wj_sigreal_reg(xjac,sig,r0)
c contributions from real graphs that do not have a singular region
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/pwhg_flg.h'
      real * 8 xjac,sig,r0(maxprocreal)
      integer lreg,lregpr,iret
      integer nmomset
      parameter (nmomset=10)
      real * 8 res(nmomset,maxprocreal),preal(0:3,nlegreal,nmomset),
     #   cprop
      integer equivto(maxprocreal)
      real * 8 equivcoef(maxprocreal)
      integer j
      real * 8 pdf1(-6:6),pdf2(-6:6)
      logical ini
      data ini/.true./
      save ini,equivto,equivcoef
      if(ini) then
         do lreg=1,flst_nregular
            equivto(lreg)=-1
         enddo
         if(flg_smartsig) then
            call wj_randomsave
c     generate "nmomset" wj_random real-phase space configurations
            call wj_fillmomenta(nlegreal,nmomset,kn_masses,preal)
            do lreg=1,flst_nregular
               do j=1,nmomset
                  call wj_realgr(
     1                 flst_regular(1,lreg),preal(0,1,j),res(j,lreg))
               enddo
               call wj_compare_vecs_reg(nmomset,lreg,res,lregpr,cprop,iret)
               if(iret.eq.0) then
c     they are equal
                  equivto(lreg)=lregpr
                  equivcoef(lreg)=1
               elseif(iret.eq.1) then
c     they are proportional
                  equivto(lreg)=lregpr
                  equivcoef(lreg)=cprop
               endif
            enddo
            call wj_randomrestore
         endif
         ini=.false.
      endif
c End initialization phase; compute graphs
      call wj_pdfcall(1,kn_x1,pdf1)
      call wj_pdfcall(2,kn_x2,pdf2)
      do lreg=1,flst_nregular
c ----------------
         if(equivto(lreg).lt.0) then
            call wj_realgr(flst_regular(1,lreg),kn_cmpreal,r0(lreg))
         else
            r0(lreg)=r0(equivto(lreg))*equivcoef(lreg)
         endif
      enddo
      sig=0
      do lreg=1,flst_nregular
         r0(lreg)=xjac*r0(lreg)*
     #      pdf1(flst_regular(1,lreg))*pdf2(flst_regular(2,lreg))
         sig=sig+r0(lreg)
      enddo
      end

      subroutine wj_compare_vecs_reg(nmomset,lreg,res,lregpr,cprop,iret)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      real * 8 ep
      parameter (ep=1d-12)
      integer nmomset,lreg,lregpr,iret,j,k
      real * 8 res(nmomset,*),cprop,rat
      do j=1,lreg-1
         rat=res(1,lreg)/res(1,j)
         do k=1,nmomset
            if(abs(1-res(k,lreg)/res(k,j)/rat).gt.ep) goto 10
         enddo
         if(abs(1-rat).lt.ep) then
            iret=0
            cprop=1
         else
            iret=1
            cprop=rat
         endif
         lregpr=j
         return
 10      continue
      enddo
      iret=-1
      end

      subroutine wj_sigreal_damp_rem(xjac,sig,r1)
      implicit none
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      real * 8 xjac,sig
      real * 8 r0(maxalr),r1(maxalr)
      integer alr
      sig=0
      call wj_sigreal_btl0(r0,1)
      do alr=1,flst_nalr
         if(kn_emitter.eq.flst_emitter(alr)) then
            if(kn_emitter.le.2) then
               r0(alr)=r0(alr)/((1-kn_y**2)*kn_csi**2)
            else
               r0(alr)=r0(alr)/((1-kn_y)*kn_csi**2)
            endif
            r0(alr)=r0(alr)*xjac
            r1(alr)=r1(alr)+r0(alr)
            sig=sig+r0(alr)
         endif
      enddo
      end

      
