      subroutine wz_gen_uborn_idx
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
      include 'src/Lib/wz/pwhg_kn.h'
      include 'src/Lib/wz/pwhg_rad.h'
      integer j,alr,em
      do j=1,rad_nkinreg
         rad_kinreg_on(j)=.false.
      enddo
      call wz_pick_random(flst_nborn,rad_btilde_arr,rad_ubornidx)
      rad_alr_nlist=flst_born2alr(0,rad_ubornidx)
      do j=1,rad_alr_nlist
         alr=flst_born2alr(j,rad_ubornidx)
         em=flst_emitter(alr)
         rad_alr_list(j)=alr
         if(em.le.2) then
            rad_kinreg_on(1)=.true.
         else
            rad_kinreg_on(em-flst_lightpart+2)=.true.
         endif
      enddo
      end

      subroutine wz_gen_real_idx
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
      include 'src/Lib/wz/pwhg_kn.h'
      include 'src/Lib/wz/pwhg_rad.h'
      call wz_pick_random(rad_alr_nlist,rad_real_arr,rad_realidx)
      rad_realalr=rad_alr_list(rad_realidx)
      end

      subroutine wz_pick_random(n,r,jret)
      implicit none
      integer n,jret
      real * 8 r(n)
      real * 8 tot(0:n)
      integer j
      real * 8 wz_ran,wz_random
      tot(0)=0
      do j=1,n
         tot(j)=tot(j-1)+r(j)
      enddo
      wz_ran=tot(n)*wz_random()
      do j=1,n
         if(tot(j).gt.wz_ran) then
            jret=j
            return
         endif
      enddo
      end
      

      subroutine wz_gen_remnant(iret)
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
      include 'src/Lib/wz/pwhg_kn.h'
      include 'src/Lib/wz/pwhg_rad.h'
      integer iret
      real * 8 dum1,dum2,dum3,dum4
      real * 8 wz_random
      external wz_random
c Remnant or regular?
      if(wz_random().gt.rad_reg_tot/(rad_reg_tot+rad_damp_rem_tot)) then
c remnant
         iret=1
         call wz_pick_random(flst_nalr,rad_damp_rem_arr,rad_realalr)
         rad_ubornidx=flst_alr2born(rad_realalr)
         kn_emitter=flst_emitter(rad_realalr)
         if(kn_emitter.le.2) then
            call wz_gen_real_phsp_isr(rad_xradremn,dum1,dum2,dum3,dum4)
         else
            call wz_gen_real_phsp_fsr(rad_xradremn,dum1,dum2,dum3)
         endif
      else
c regular
         iret=2
         call wz_pick_random(flst_nregular,rad_reg_arr,rad_realreg)
         call wz_gen_real_phsp_isr(rad_xradremn,dum1,dum2,dum3,dum4)
      endif
      end
