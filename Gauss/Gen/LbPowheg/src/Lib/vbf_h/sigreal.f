      subroutine vbf_h_btildereal(xrad,resreal,www)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_flg.h'
      include 'src/Lib/vbf_h/pwhg_par.h'
      real * 8 xrad(3),resreal(maxprocborn),www
      real * 8 r0(maxalr),rc(maxalr),rp(maxalr),rm(maxalr),
     # r0s(maxalr),rcs(maxalr),rps(maxalr),rms(maxalr),xl,xlp,xlm,
     # jac_over_csi,jac_over_csi_coll,jac_over_csi_soft,
     # jac_over_csi_p,jac_over_csi_m,rrr0,rrrc,rrr0s,rrrcs,
     # rrrp,rrrps,rrrm,rrrms,remnant,out0,out1
      integer j,iuborn
      logical vbf_h_valid_emitter
      external vbf_h_valid_emitter
      do j=1,flst_nborn
         resreal(j)=0
      enddo
      do kn_emitter=0,nlegborn
c output values for vbf_h_analysis_driver
         out0=0
         out1=0
c check that emitter is valid
         if(vbf_h_valid_emitter(kn_emitter)) then
            if(kn_emitter.gt.2) then
c     final state radiation
               call vbf_h_gen_real_phsp_fsr(xrad,jac_over_csi,
     #              jac_over_csi_coll,jac_over_csi_soft)
c sigreal fills the array r0 with the value of the R_alpha contribution
c that have emitter equal to kn_emitter. All other contributions are set
c to zero. 
               call vbf_h_sigreal_btl(r0)
               if(flg_withsubtr) then
                  call vbf_h_collfsr(rc)
c     vbf_h_soft subtraction
                  call vbf_h_soft(r0s)
                  call vbf_h_softcollfsr(rcs)
c     in final state radiation csimax is independent of y
                  xl=log(kn_csimax/par_csicut)
               endif
               do j=1,flst_nalr
                  iuborn=flst_alr2born(j)
                  rrr0=r0(j)*kn_jacborn
     #                 *jac_over_csi/(1-kn_y)/kn_csitilde
                  if(flg_withsubtr) then
                     rrrc=rc(j)*kn_jacborn
     #                 *jac_over_csi_coll/(1-kn_y)/kn_csitilde
                     rrr0s=r0s(j)*kn_jacborn
     #                 *jac_over_csi_soft/(1-kn_y)/kn_csitilde
                     rrrcs=rcs(j)*kn_jacborn
     #                 *jac_over_csi_soft/(1-kn_y)/kn_csitilde
                     remnant=(rrr0s-rrrcs)*xl*kn_csitilde
                  endif
                  if(flg_withsubtr) then
                     resreal(iuborn)= resreal(iuborn)+rrr0-rrrc
     #-rrr0s+rrrcs+remnant
                  else
c     provide a damping factor for the singular region,
c     to avoid divergent integral (25 is an ad hoc value
                     resreal(iuborn)= resreal(iuborn)
     #               +rrr0*(1-kn_y**2)*kn_csi/
     #                  (25/kn_sbeams+(1-kn_y**2)*kn_csi)
                  endif
                  if(flg_nlotest) then
                     out1=out1+rrr0
                     if(flg_withsubtr) then
                        out0=out0-rrrc-rrr0s+rrrcs+remnant
                     endif
                  endif
               enddo
            else
c     initial state singularities.
c     Regions that have only + (-) collinear singularity should return
c     zero rm (rp).
               call vbf_h_gen_real_phsp_isr
     #(xrad,jac_over_csi,jac_over_csi_p,jac_over_csi_m,
     #jac_over_csi_soft)
               call vbf_h_sigreal_btl(r0)
               if(flg_withsubtr) then
                  call vbf_h_soft(r0s)
                  if(kn_emitter.ne.2) then
                     call vbf_h_collisrp(rp)
                     call vbf_h_softcollisrp(rps)
                  endif
                  if(kn_emitter.ne.1) then
                     call vbf_h_collisrm(rm)
                     call vbf_h_softcollisrm(rms)
                  endif
c     remnants (see xscaled.pdf in docs directory)
                  xl =log(kn_csimax/par_csicut)
                  xlp=log(kn_csimaxp/par_csicut)
                  xlm=log(kn_csimaxm/par_csicut)
               endif
               do j=1,flst_nalr
                  rrr0=r0(j)*kn_jacborn
     #                 *jac_over_csi/(1-kn_y**2)/kn_csitilde
                  if(flg_withsubtr) then
                     rrr0s=r0s(j)*kn_jacborn
     #                 *jac_over_csi_soft/(1-kn_y**2)/kn_csitilde
                     remnant=rrr0s*xl*kn_csitilde
                     if(kn_emitter.ne.2) then
                        rrrp=rp(j)*kn_jacborn
     #                 *jac_over_csi_p/(1-kn_y)/kn_csitilde/2
                        rrrps=rps(j)*kn_jacborn
     #                 *jac_over_csi_soft/(1-kn_y)/kn_csitilde/2
                        remnant=remnant-rrrps*xlp*kn_csitilde
                     else
                        rrrp=0
                        rrrps=0
                     endif
                     if(kn_emitter.ne.1) then
                        rrrm=rm(j)*kn_jacborn
     #                 *jac_over_csi_m/(1+kn_y)/kn_csitilde/2
                        rrrms=rms(j)*kn_jacborn
     #                 *jac_over_csi_soft/(1+kn_y)/kn_csitilde/2
                        remnant=remnant-rrrms*xlm*kn_csitilde
                     else
                        rrrm=0
                        rrrms=0
                     endif
                  endif
                  iuborn=flst_alr2born(j)
                  if(flg_withsubtr) then
                     resreal(iuborn)= resreal(iuborn)+rrr0
     #              -rrr0s-rrrp-rrrm+rrrps+rrrms+remnant
                  else
c     provide a damping factor for the singular region,
c     to avoid divergent integral (25 is an ad hoc value)
                     resreal(iuborn)= resreal(iuborn)
     #               +rrr0*(1-kn_y**2)*kn_csi/
     #                  (25/kn_sbeams+(1-kn_y**2)*kn_csi)
                  endif
                  if(flg_nlotest) then
                     out1=out1+rrr0
                     if(flg_withsubtr) then
                        out0=out0-rrr0s-rrrp-rrrm+rrrps+rrrms+remnant
                     endif
                  endif
               enddo
            endif
         endif
         if(flg_nlotest) then
            out0=out0*www
            out1=out1*www
            if(out0.ne.0d0) call vbf_h_analysis_driver(out0,0)
            if(out1.ne.0d0) call vbf_h_analysis_driver(out1,1)
         endif
      enddo
      end

      subroutine vbf_h_checklims(iun)
      implicit none
      integer iun
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_dbg.h'
      external vbf_h_sigreal_btl,vbf_h_soft,vbf_h_collfsr,vbf_h_softcollfsr, vbf_h_collisrp,
     $     vbf_h_softcollisrp,vbf_h_collisrm,vbf_h_softcollisrm
      call vbf_h_randomsave
      if(dbg_softtest) then
         write(iun,*) '******************************************'   
         write(iun,*) '           CHECK  vbf_h_soft LIMITS             '     
         write(iun,*)
         do kn_emitter=0,nlegborn
            call vbf_h_checksoft(vbf_h_sigreal_btl,vbf_h_soft,' vbf_h_soft',iun)
         enddo
         write(iun,*) '******************************************'
         write(iun,*)
      endif
      
      if(dbg_colltest) then
         write(iun,*) '******************************************'   
         write(iun,*) '      CHECK  COLL. LIMITS FOR FSR       '         
         write(iun,*)
         do kn_emitter=3,nlegborn
            call vbf_h_checkcoll(vbf_h_sigreal_btl,vbf_h_collfsr,1,' coll',iun)
         enddo
         write(iun,*) '******************************************'   
         write(iun,*)
         write(iun,*) '******************************************'   
         write(iun,*) '      CHECK  COLL. LIMITS FOR ISR       '         
         write(iun,*)
         do kn_emitter=0,2
c            call vbf_h_randomsave
            if(kn_emitter.ne.2) call vbf_h_checkcoll(vbf_h_sigreal_btl,vbf_h_collisrp,1
     $           ,' coll-plus',iun)
c            call vbf_h_randomrestore
            if(kn_emitter.ne.1) call vbf_h_checkcoll(vbf_h_sigreal_btl,vbf_h_collisrm,-1
     $           ,' coll-minus',iun)
         enddo
         write(iun,*) '******************************************'   
         write(iun,*)
      endif
      
      if(dbg_softtest.and.dbg_colltest) then  
         write(iun,*) '******************************************'   
         write(iun,*) '   CHECK  vbf_h_soft-COLL. LIMITS FOR FSR     '         
         write(iun,*)
         do kn_emitter=3,nlegborn
            call vbf_h_checksoft(vbf_h_collfsr,vbf_h_softcollfsr,' vbf_h_soft-coll',iun)
         enddo
         write(iun,*) '******************************************'
         write(iun,*)
         write(iun,*) '******************************************'   
         write(iun,*) '   CHECK  vbf_h_soft-COLL. LIMITS FOR ISR +   '         
         write(iun,*)
         do kn_emitter=0,2
            if(kn_emitter.ne.2)call vbf_h_checksoft(vbf_h_collisrp,vbf_h_softcollisrp,
     $           ' vbf_h_soft-coll-plus',iun)
         enddo
         write(iun,*) '******************************************'
         write(iun,*)
         write(iun,*) '******************************************'   
         write(iun,*) '   CHECK  vbf_h_soft-COLL. LIMITS FOR ISR -   '         
         write(iun,*)
         do kn_emitter=0,2
            if(kn_emitter.ne.1)call vbf_h_checksoft(vbf_h_collisrm,vbf_h_softcollisrm,
     $           ' vbf_h_soft-coll-minus',iun)
         enddo
         write(iun,*) '******************************************'
         write(iun,*)

         
         write(iun,*) '******************************************'   
         write(iun,*) '   CHECK  COLL.-vbf_h_soft LIMITS FOR FSR     '         
         write(iun,*)
         do kn_emitter=3,nlegborn
            call vbf_h_checkcoll(vbf_h_soft,vbf_h_softcollfsr,1,' coll-vbf_h_soft',iun)
         enddo
         write(iun,*) '******************************************'
         write(iun,*)
         write(iun,*) '******************************************'   
         write(iun,*) '   CHECK  COLL.-vbf_h_soft LIMITS FOR ISR     '         
         write(iun,*)
         do kn_emitter=0,2
            if(kn_emitter.ne.2) call vbf_h_checkcoll(vbf_h_soft,vbf_h_softcollisrp,1
     $           ,' coll-plus-vbf_h_soft',iun)
            if(kn_emitter.ne.1) call vbf_h_checkcoll(vbf_h_soft,vbf_h_softcollisrm,-1
     $           ,' coll-minus-vbf_h_soft',iun)
         enddo
      endif
      call vbf_h_randomrestore
      end

      subroutine vbf_h_checkborn(iun)
c Check if born, colour correlated born and spin correlated born
c are consistent with total Born
      implicit none
      integer iun
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_br.h'
      integer  iborn,j,k,mu
      real * 8 tot
      real * 8 gtens(0:3,0:3),ap
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      save gtens
      do iborn=1,flst_nborn
         do j=1,nlegborn
            if(abs(flst_born(j,iborn)).le.6) then
               tot=0
               do k=1,nlegborn
                  if(abs(flst_born(k,iborn)).le.6) then
                     if(k.ne.j) then
                        tot=tot+br_bornjk(j,k,iborn)
                     endif
                  endif
               enddo
               if(flst_born(j,iborn).eq.0) then
                  tot=tot/(ca*br_born(iborn))
               else
                  tot=tot/(cf*br_born(iborn))
               endif
               if(abs((tot-1)/tot).gt.1d-8) then
                  write(iun,'(f6.3,a,20(i2,1x))') tot,
     1   ' colour check fails for flav. struct:',
     2     (flst_born(k,iborn),k=1,nlegborn)
               endif
            endif
         enddo
      enddo
      do iborn=1,flst_nborn
         do j=1,nlegborn
            if(flst_born(j,iborn).eq.0) then
               tot=0
               do mu=0,3
                  tot=tot-gtens(mu,mu)*br_bmunu(mu,mu,j,iborn)
               enddo
               tot=tot/br_born(iborn)
               if(abs((tot-1)/tot).gt.1d-8) then
                  write(iun,'(f6.3,a,i2,a,20(i2,1x))')
     1    tot, ' spin correlated amplitude'//
     2 ' wrong for leg', j, ' flavour struct:',
     3                 (flst_born(k,iborn),k=1,nlegborn)
               endif
            endif
         enddo
      enddo
      end



      subroutine vbf_h_checksoft(sig,sigs,label,iun)
      implicit none
      include 'src/Lib/vbf_h/pwhg_dbg.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      character *(*) label
      integer iun
      real * 8 xborn(ndiminteg-3),xrad(3)
      integer nexp
      parameter (nexp=5)
      real * 8 jac_over_csi,
     #jac_over_csi_coll,jac_over_csi_soft,r0(maxalr,nexp),
     #r0s(maxalr,nexp),jac_over_csi_p,jac_over_csi_m
      integer j,jexp,alr,alrp
      character * 11 flag
      logical ident(maxalr)
      real * 8 vbf_h_random,vbf_h_dotp
      external vbf_h_random,vbf_h_dotp
      logical vbf_h_valid_emitter,iszero,isnonzero,isequal
      external vbf_h_valid_emitter
      do j=1,ndiminteg-3
         xborn(j)=vbf_h_random()
      enddo
      call vbf_h_gen_born_phsp(xborn)
      call vbf_h_setscalesbtilde
      call vbf_h_allborn
      call vbf_h_checkborn(iun)
c      write(iun,*)' vbf_h_mass',sqrt(2*vbf_h_dotp(kn_pborn(0,3),kn_pborn(0,4)))
      do j=1,3
         xrad(j)=vbf_h_random()
      enddo
c Check vbf_h_soft limits
      if(vbf_h_valid_emitter(kn_emitter)) then
         write(iun,*) ' vbf_h_random Born variables ====> ',xborn
          write(iun,*) ' vbf_h_random radiation variables ====> ',xrad
          do jexp=1,nexp
            xrad(1)=10d0**(-jexp)
            if(kn_emitter.gt.2) then
               call vbf_h_gen_real_phsp_fsr(xrad,jac_over_csi,
     $              jac_over_csi_coll,jac_over_csi_soft)
            else
               call vbf_h_gen_real_phsp_isr (xrad,jac_over_csi,jac_over_csi_p,
     $              jac_over_csi_m,jac_over_csi_soft)
            endif
            write(iun,*) '### Check vbf_h_soft',xrad(1)
            call sig(r0(1,jexp))
            call sigs(r0s(1,jexp))
         enddo
         do alr=1,flst_nalr
            ident(alr)=.false.
         enddo
         do alr=1,flst_nalr
c     if one r0 is zero, all others must be zero
            iszero=.false.
            isnonzero=.false.
            do jexp=1,nexp
c               if(r0s(alr,jexp).ne.0) isnonzero=.true.
c               if(r0s(alr,jexp).eq.0) iszero=.true.
               if(r0(alr,jexp).ne.0) isnonzero=.true.
               if(r0(alr,jexp).eq.0) iszero=.true.
            enddo
            if(iszero.and.isnonzero) then
               write(iun,*) ' some vanish and some do not'
            endif
            if(isnonzero.and..not.iszero) then
               write(iun,*) ' emitter ',kn_emitter, ', process ',
     $              (flst_alr(j,alr),j=1,nlegreal),', ',label,':'
               do alrp=alr+1,flst_nalr
                  isequal=.true.
                  do jexp=1,nexp
                     if(r0(alr,jexp).ne.r0(alrp,jexp).or. r0s(alr,jexp)
     $                    .ne.r0s(alrp,jexp)) isequal=.false.
                  enddo
                  if(isequal) then
                     write(iun,*) ' emitter ',kn_emitter, ', process ',
     $                    (flst_alr(j,alrp),j=1,nlegreal),', ',label,':'
                     ident(alrp)=.true.
                  endif
               enddo
               do jexp=2,nexp
                  flag=' '
                  if(abs(r0s(alr,jexp)/r0(alr,jexp)-1).gt.0.01) then
                     if(r0s(alr,jexp).ne.0) then
                        if(jexp.eq.2) then
                           flag='*-WARN-*'
                        elseif(jexp.eq.3) then
                           flag='*-WWARN-*'
                        elseif(jexp.ge.4) then
                           flag='*-WWWARN-*'
                        endif
                     endif
                  endif
                  write(iun,*) (r0(alr,jexp)-r0s(alr,jexp))/ 
     $            (r0(alr,jexp-1)-r0s(alr,jexp-1)),
     $             r0s(alr,jexp)/r0(alr,jexp),flag
               enddo
            endif
         enddo
      endif
      end


      subroutine vbf_h_checkcoll(sig,sigc,idir,label,iun)
      implicit none
      integer iun
      include 'src/Lib/vbf_h/pwhg_dbg.h'
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      character *(*) label
      integer idir
      real * 8 xborn(ndiminteg-3),xrad(3)
      integer nexp
      parameter (nexp=8)
      real * 8 jac_over_csi,
     #jac_over_csi_coll,jac_over_csi_soft,r0(maxalr,nexp),
     #r0c(maxalr,nexp),jac_over_csi_p,jac_over_csi_m
      integer j,jexp,jexpfirst,alr,alrp
      real * 8 vbf_h_random
      external vbf_h_random
      logical ident(maxalr)
      character * 11 flag
      logical vbf_h_valid_emitter,iszero,isnonzero,isequal
      external vbf_h_valid_emitter
      do j=1,ndiminteg-3
         xborn(j)=vbf_h_random()
      enddo
      call vbf_h_gen_born_phsp(xborn)
      call vbf_h_setscalesbtilde
      call vbf_h_allborn
      do j=1,3
         xrad(j)=vbf_h_random()
      enddo
      if(vbf_h_valid_emitter(kn_emitter)) then
         write(iun,*) ' vbf_h_random Born variables ====> ',xborn
         write(iun,*) ' vbf_h_random radiation variables ====> ',xrad
         do jexp=1,nexp
            if(idir.ne.-1) then
               xrad(2)=10d0**(-jexp)
            else
               xrad(2)=1-10d0**(-jexp)
            endif
            if(kn_emitter.gt.2) then
               call vbf_h_gen_real_phsp_fsr(xrad,jac_over_csi,
     #jac_over_csi_coll,jac_over_csi_soft)
            else
               call vbf_h_gen_real_phsp_isr
     #(xrad,jac_over_csi,jac_over_csi_p,jac_over_csi_m,
     #jac_over_csi_soft)
            endif
            write(iun,*) '######### Check coll',xrad(2)
            call sig(r0(1,jexp))
            call sigc(r0c(1,jexp))
         enddo
         do alr=1,flst_nalr
            ident(alr)=.false.
         enddo
         do alr=1,flst_nalr
            do jexp=1,nexp
               if(r0c(alr,jexp).ne.r0c(alr,1)) then
                  write(iun,*)
     #' vbf_h_checklims error : coll lim depends upon coll variable'
               endif
            enddo
         enddo
         do alr=1,flst_nalr
c     if one r0 is zero, all others must be zero
            iszero=.false.
            isnonzero=.false.
            jexpfirst=2
            do jexp=nexp,1,-1
c               if(r0c(alr,jexp).ne.0) isnonzero=.true.
c               if(r0c(alr,jexp).eq.0) iszero=.true.
c               if(r0(alr,jexp).ne.0) isnonzero=.true.
               if(r0(alr,jexp).eq.0) then
                  if(r0c(alr,jexp).ne.0) then
                     write(iun,*) ' some vanish and some do not'
                  endif
                  jexpfirst=jexp+1
                  goto 111
               endif
            enddo
 111        continue
            if(jexpfirst.le.nexp) then
               write(iun,*) ' emitter ',kn_emitter,
     #', process ',(flst_alr(j,alr),j=1,nlegreal),', ',label,':'
               do alrp=alr+1,flst_nalr
                  isequal=.true.
                  do jexp=1,nexp
                     if(r0(alr,jexp).ne.r0(alrp,jexp).or.
     #r0c(alr,jexp).ne.r0c(alrp,jexp)) isequal=.false.
                  enddo
                  if(isequal) then
                     write(iun,*) ' emitter ',kn_emitter,
     #', process ',(flst_alr(j,alrp),j=1,nlegreal),', ',label,':'
                     ident(alrp)=.true.
                  endif
               enddo
               do jexp=jexpfirst,nexp
                  flag=' '
                  if(abs(r0c(alr,jexp)/r0(alr,jexp)-1).gt.0.01) then
                     if(jexp.eq.jexpfirst) then
                        flag='*-WARN-*'
                     elseif(jexp.eq.3) then
                        flag='*-WWARN-*'
                     elseif(jexp.ge.4) then
                        flag='*-WWWARN-*'
                     endif
                   endif
c     Added this 'if' to be sure that no division by zero occurs
                  if((r0(alr,jexp-1)-r0c(alr,jexp-1)).ne.0d0) then
                  write(iun,*) (r0(alr,jexp)-r0c(alr,jexp))/
     #(r0(alr,jexp-1)-r0c(alr,jexp-1)),r0c(alr,jexp)/r0(alr,jexp),flag
                  endif
               enddo
            endif
         enddo
      endif
      end


      subroutine vbf_h_sigreal_rad(sig)
      implicit none
      real * 8 sig
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_rad.h'
      include 'src/Lib/vbf_h/pwhg_flg.h'
      include 'src/Lib/vbf_h/pwhg_par.h'
      real * 8 r0(maxalr),rc(maxalr),rs(maxalr)
      integer alr,alrpr,iret,em
      integer nmomset,emitter
      parameter (nmomset=10)
      real * 8 res(nmomset,maxalr),preal(0:3,nlegreal,nmomset),cprop
      integer equivto(maxalr)
      real * 8 equivcoef(maxalr)
      integer j,k
      real * 8 sumdijinv,dampfac,r
      real * 8 pdf1(-6:6),pdf2(-6:6)
      real * 8 ptsq,vbf_h_pwhg_pt2
      logical computed(maxalr)
      logical condition
      logical ini
      data ini/.true./
      save ini,equivto,equivcoef
      external vbf_h_pwhg_pt2
      if(ini) then
         do alr=1,flst_nalr
            equivto(alr)=-1
         enddo
         if(flg_smartsig) then
            call vbf_h_randomsave
c     generate "nmomset" vbf_h_random real-phase space configurations
            call vbf_h_fillmomenta(nlegreal,nmomset,kn_masses,preal)
            do alr=1,flst_nalr
               do j=1,nmomset
                  call vbf_h_realgr(
     1                 flst_alr(1,alr),preal(0,1,j),res(j,alr))
               enddo
               call vbf_h_compare_vecs(nmomset,alr,res,1,alrpr,cprop,iret)
               if(iret.eq.0) then
c     they are equal
                  equivto(alr)=alrpr
                  equivcoef(alr)=1
               elseif(iret.eq.1) then
c     they are proportional
                  equivto(alr)=alrpr
                  equivcoef(alr)=cprop
               else
c     < 0 for unequal:
                  equivto(alr)=-1
               endif
            enddo
            call vbf_h_randomrestore
         endif
         ini=.false.
      endif
c End initialization phase; compute graphs
      do alr=1,flst_nalr
         r0(alr)=0
      enddo
      call vbf_h_pdfcall(1,kn_x1,pdf1)
      call vbf_h_pdfcall(2,kn_x2,pdf2)
      if(flg_withdamp) then
         call vbf_h_collrad(rc)
         call vbf_h_softrad(rs)
      endif
      do alr=1,flst_nalr
         computed(alr)=.false.
      enddo
      do j=1,rad_alr_nlist
         alr=rad_alr_list(j)
         em=flst_emitter(alr)
c check if emitter corresponds to current radiation region (i.e. rad_kinreg):
         if((rad_kinreg.eq.1.and.em.le.2).or.(em.gt.2.and.
     #       flst_lightpart+rad_kinreg-2.eq.em))then
c check if we have a g -> Q Qbar splitting below threshold:
            if(em.gt.0) then
               if(flst_alr(em,alr)+flst_alr(nlegreal,alr).eq.0.and.
     #abs(flst_alr(em,alr)).ge.4) then
                  ptsq=vbf_h_pwhg_pt2()
                  if(abs(flst_alr(em,alr)).eq.4
     #  .and.ptsq.lt.rad_charmthr2.or.
     # abs(flst_alr(em,alr)).eq.5.and.ptsq.lt.rad_bottomthr2) then
                     r0(alr)=0
                     goto 995
                  endif
               endif
            endif
c ----------------
c Gimnastic to avoid problem with non-lazy evaluation of logical
c expressions in gfortran; replaces the line
c            if(equivto(alr).lt.0.or..not.computed(equivto(alr))) then
            if(equivto(alr).lt.0) then
               condition=.true.
            elseif(.not.computed(equivto(alr))) then
               condition=.true.
            else
               condition=.false.
            endif
            if(condition) then
               call vbf_h_realgr(flst_alr(1,alr),kn_cmpreal,r0(alr))
               sumdijinv=0
               do k=1,flst_allreg(1,0,alr)
                  sumdijinv=sumdijinv
     #+1/kn_dijterm(flst_allreg(1,k,alr),flst_allreg(2,k,alr))
               enddo
               r0(alr)=r0(alr)/kn_dijterm(em,nlegreal)/sumdijinv
c If the emitter is in the final state, and if the emitted and emitter
c are both gluons, supply a factor E_em/(E_em+E_rad) * 2
               if(em.gt.2) then
                  if(flst_alr(em,alr).eq.0.and.
     1              flst_alr(nlegreal,alr).eq.0) then
                     r0(alr)=r0(alr)*2
     1                    *kn_cmpreal(0,em)**par_2gsupp/
     2                    (kn_cmpreal(0,em)**par_2gsupp
     3                    +kn_cmpreal(0,nlegreal)**par_2gsupp)
                  endif
               endif
               r0(alr)=r0(alr)*flst_mult(alr)
c supply Born zero damping factor, if required
               if(flg_withdamp) then
                  r=r0(alr)
                  call vbf_h_bornzerodamp(alr,r,rc(alr),rs(alr),dampfac)
                  r0(alr)=r0(alr) * dampfac
               endif
               computed(alr)=.true.
               if(equivto(alr).gt.0) then
                  r0(equivto(alr))=r0(alr)/equivcoef(alr)
                  computed(equivto(alr))=.true.
               endif
            else
               r0(alr)=r0(equivto(alr))*equivcoef(alr)
            endif
         else
            r0(alr)=0
         endif
 995     continue
      enddo
      sig=0
      do j=1,rad_alr_nlist
         alr=rad_alr_list(j)
         if(r0(alr).ne.0) then
            r0(alr)=r0(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
            sig=sig+r0(alr)
            rad_real_arr(j)=r0(alr)
         else
            rad_real_arr(j)=0
         endif
      enddo
      end


      subroutine vbf_h_sigreal_btl(r0)
      implicit none
      real * 8 r0(*)
      call vbf_h_sigreal_btl0(r0,0)
      end

c Real cross section, required by vbf_h_btilde;
c fills the array r0(alr) with the invariant cross section, multiplied
c by csi^2 (1-y^2) for ISR regions
c    csi^2 (1-y)   for FSR regions
      subroutine vbf_h_sigreal_btl0(r0,imode)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_kn.h'
      include 'src/Lib/vbf_h/pwhg_flg.h'
      include 'src/Lib/vbf_h/pwhg_par.h'
      integer imode
      real * 8 r0(maxalr)
      real * 8 rc(maxalr),rs(maxalr),r
      integer alr,alrpr,iret
      integer nmomset
      parameter (nmomset=10)
      real * 8 res(nmomset,maxalr),preal(0:3,nlegreal,nmomset),cprop
      integer equivto(maxalr),markused(maxalr)
      real * 8 equivcoef(maxalr)
      integer j,k
      real * 8 sumdijinv,dampfac
      real * 8 pdf1(-6:6),pdf2(-6:6)
      logical ini
      data ini/.true./
      save ini,equivto,equivcoef
      if(ini) then
         do alr=1,flst_nalr
            equivto(alr)=-1
         enddo
         if(flg_smartsig) then
            call vbf_h_randomsave
c     generate "nmomset" vbf_h_random real-phase space configurations
            call vbf_h_fillmomenta(nlegreal,nmomset,kn_masses,preal)
            do alr=1,flst_nalr
               do j=1,nmomset
                  call vbf_h_realgr(
     1                 flst_alr(1,alr),preal(0,1,j),res(j,alr))
               enddo
               call vbf_h_compare_vecs(nmomset,alr,res,0,alrpr,cprop,iret)
               if(iret.eq.0) then
c     they are equal:
                  equivto(alr)=alrpr
                  equivcoef(alr)=1
               elseif(iret.eq.1) then
c     they are proportional:
                  equivto(alr)=alrpr
                  equivcoef(alr)=cprop
               else
c     < 0 for unequal:
                  equivto(alr)=-1
               endif
            enddo
            call vbf_h_randomrestore
         endif
         ini=.false.
      endif
c End initialization phase; compute graphs
      do alr=1,flst_nalr
         r0(alr)=0
         markused(alr)=0
      enddo
      if(flg_withdamp) then
         call vbf_h_collbtl(rc)
         call vbf_h_softbtl(rs)
      endif
      do alr=1,flst_nalr
c Only R_alpha (namely alr) with the current emitter: 
         if(flst_emitter(alr).eq.kn_emitter) then
            if(equivto(alr).lt.0) then
c Not equal to any previous one, compute explicitly.
c First mark as being computed
               markused(alr)=1
               call vbf_h_realgr(flst_alr(1,alr),kn_preal,r0(alr))
c Supply FKS factor to separate singular region:
               sumdijinv=0
c Loop over all singular regions of the given contribution
               do k=1,flst_allreg(1,0,alr)
c flst_allreg({1,2},...) are the two legs that identify the k'th region
                  sumdijinv=sumdijinv
     #+1/kn_dijterm(flst_allreg(1,k,alr),flst_allreg(2,k,alr))
               enddo
               r0(alr)=r0(alr)/kn_dijterm(kn_emitter,nlegreal)/sumdijinv
c If the emitter is in the final state, and if the emitted and emitter
c are both gluons, supply a factor E_em/(E_em+E_rad) * 2
               if(kn_emitter.gt.2) then
                  if(flst_alr(kn_emitter,alr).eq.0.and.
     1                 flst_alr(nlegreal,alr).eq.0) then
                     r0(alr)=r0(alr)*2
     1                    *kn_cmpreal(0,kn_emitter)**par_2gsupp/
     2                    (kn_cmpreal(0,kn_emitter)**par_2gsupp
     3                    +kn_cmpreal(0,nlegreal)**par_2gsupp)
                  endif
               endif
c supply Born zero damping factor, if required
               if(flg_withdamp) then
                  if(kn_emitter.gt.2) then
                     r=r0(alr)*(1-kn_y)*kn_csi**2
                  else
                     r=r0(alr)*(1-kn_y**2)*kn_csi**2
                  endif
                  r=r*flst_mult(alr)
                  call vbf_h_bornzerodamp(alr,r,rc(alr),rs(alr),dampfac)
                  if(imode.eq.0) then
                     r0(alr) =r0(alr) * dampfac
                  elseif(imode.eq.1) then
                     r0(alr) =r0(alr) * (1-dampfac)
                  else
                     write(*,*) ' vbf_h_sigreal_btl0: improper call'
                  endif
               endif
            else
               if(markused(equivto(alr)).ne.1) then
                  write(*,*) ' error: vbf_h_sigreal_btl flg_smartsig bug'
                  call exit(1)
               endif
               r0(alr)=r0(equivto(alr))*equivcoef(alr)
            endif
         endif
      enddo
      call vbf_h_pdfcall(1,kn_x1,pdf1)
      call vbf_h_pdfcall(2,kn_x2,pdf2)
      do alr=1,flst_nalr
         r0(alr)=r0(alr)*flst_mult(alr)
         if(kn_emitter.gt.2) then
            r0(alr)=r0(alr)*(1-kn_y)*kn_csi**2
         else
            r0(alr)=r0(alr)*(1-kn_y**2)*kn_csi**2
         endif
c include pdf's
         r0(alr)=r0(alr)*pdf1(flst_alr(1,alr))*pdf2(flst_alr(2,alr))
      enddo
      end


      subroutine vbf_h_fillmomenta(nlegreal,nmomset,kn_masses,preal)
      implicit none
      integer nlegreal,nmomset
      real * 8 preal(0:3,nlegreal,nmomset),kn_masses(nlegreal)
      integer mu,j,k
      real * 8 pl,en
      real * 8 vbf_h_random
      external vbf_h_random
      do j=1,nmomset
         do k=3,nlegreal-1
            do mu=1,3
               preal(mu,k,j)=vbf_h_random()
            enddo
         enddo
         preal(3,nlegreal,j)=vbf_h_random()
         preal(1,nlegreal,j)=0
         preal(2,nlegreal,j)=0
         do k=3,nlegreal-1
            preal(1,nlegreal,j)=preal(1,nlegreal,j)-preal(1,k,j)
            preal(2,nlegreal,j)=preal(2,nlegreal,j)-preal(2,k,j)
         enddo
         pl=0
         en=0
         do k=3,nlegreal
c We should have the masses as a function of flavour id, somewhere!
            preal(0,k,j)=sqrt(kn_masses(k)**2+
     #         preal(1,k,j)**2+preal(2,k,j)**2+preal(3,k,j)**2)
            pl=pl+preal(3,k,j)
            en=en+preal(0,k,j)
         enddo
         preal(0,1,j)=(en+pl)/2
         preal(0,2,j)=(en-pl)/2
         preal(3,1,j)=preal(0,1,j)
         preal(3,2,j)=-preal(0,2,j)
         preal(1,1,j)=0
         preal(1,2,j)=0
         preal(2,1,j)=0
         preal(2,2,j)=0
      enddo
      end
         

      subroutine vbf_h_compare_vecs(nmomset,alr,res,imode,alrpr,cprop,iret)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      real * 8 ep
      parameter (ep=1d-12)
      integer nmomset,alr,alrpr,imode,iret,j,k
      real * 8 res(nmomset,*),cprop,rat
c imode=0 when called from vbf_h_btilde,
c imode=1 when called for radiation. In the latter
c case, graphs that do not have the same underlying Born
c are not considered.
      do j=1,alr-1
         if(flst_emitter(j).ne.flst_emitter(alr)) goto 10
         if(imode.eq.1.and.flst_alr2born(j).ne.flst_alr2born(alr))
     1        goto 10
         rat=res(1,alr)/res(1,j)
         do k=1,nmomset
            if(abs(1-res(k,alr)/res(k,j)/rat).gt.ep) goto 10
         enddo
         if(abs(1-rat).lt.ep) then
            iret=0
            cprop=1
         else
            iret=1
            cprop=rat
         endif
         alrpr=j
         return
 10      continue
      enddo
      iret=-1
      end

      subroutine vbf_h_realgr(rflav,p,res)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      integer rflav(nlegreal)
      real * 8 p(0:3,nlegreal),res
      call vbf_h_real_ampsq(p,rflav,res)
c flux factor
      res=res/(8*p(0,1)*p(0,2))
      if(res.eq.0) then
c         write(*,*) 'vbf_h_realgr:', rflav
         continue
      endif
      end



      subroutine vbf_h_real_ampsq(p,rflav,amp2)
      implicit none
      include 'src/Lib/vbf_h/nlegborn.h'
      include 'src/Lib/vbf_h/pwhg_flst.h'
      include 'src/Lib/vbf_h/pwhg_math.h'
      include 'src/Lib/vbf_h/pwhg_st.h'
      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2 
      logical vbf_h_pwhg_isfinite
      external vbf_h_pwhg_isfinite
      call vbf_h_setreal(p,rflav,amp2)
c     check if amp2 is finite
      if (.not.vbf_h_pwhg_isfinite(amp2)) amp2=0d0
      amp2 = amp2*st_alpha/(2*pi)
      end
