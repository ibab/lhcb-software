      subroutine st_tch_setreal(p,rflav,amp2)
      implicit none
      include 'src/Lib/st_tch/nlegborn.h'
      include 'src/Lib/st_tch/pwhg_st.h'
      include 'src/Lib/st_tch/pwhg_math.h'
      include 'src/Lib/st_tch/PhysPars.h'
c     to use madgraph
      include 'src/Lib/st_tch/coupl.inc'

      real * 8 p(0:3,nlegreal)
      integer rflav(nlegreal)
      real * 8 amp2,amp2_mad



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


cccccccccccccccccc
      integer nleg
      parameter (nleg=nlegreal)
      real *8 krlab_ME_int(0:3,nleg),krlab_mad(0:3,nleg),ptemp(0:3)
      integer rflav_ME_int(nleg)
      integer rflav_loc(nleg)
      real *8 ewcoupl
      integer mu,ileg

      integer three_ch(-6:6)
      data three_ch /-2,1,-2,1,-2,1,0,-1,2,-1,2,-1,2/

      real *8 ckm_b_t

      integer chflag
      common/st_tch_cchflag/chflag

      real *8 st_real_qq_reg,st_real_qg_reg
      external st_real_qq_reg,st_real_qg_reg

      real *8 st_tch_dotp
      external st_tch_dotp

      logical ini
      data ini/.true./
      save ini
      

      logical mad_ME
      parameter (mad_ME=.true.)

      
      integer rflav_t(nleg)


ccccccccccccccccccccccccccccccccccccccc
c     charge conjugation
c     if ttype=-1, then rflav has been filled with tbar
c     production flavours. Subroutines here work for t flavour.
c     Therefore, invert the sign of local flavours. To avoid confusion,
c     here I use rflav_t, since in this subroutine rflav_loc is used
c     for another issue.
      do ileg=1,nleg
         rflav_t(ileg)= ttype *rflav(ileg)
      enddo
ccccccccccccccccccccccccccccccccccccccc


c     local copy of input variables (p->krlab_ME_int, rflav->rflav_loc)
c     convert input flavour string into rflav_ME_int, needed by ME
c     rflav_ME_int has only flavours -2,-1,0,1,2
      do ileg=1,nleg
         rflav_loc(ileg)=rflav_t(ileg)
         rflav_ME_int(ileg)=rflav_t(ileg)
         if(ileg.ne.3) then
            if(abs(rflav_ME_int(ileg)).eq.3) then
               rflav_ME_int(ileg)=sign(1,rflav_ME_int(ileg))
            elseif(abs(rflav_ME_int(ileg)).eq.4) then
               rflav_ME_int(ileg)=sign(1,rflav_ME_int(ileg)) *2
            elseif(abs(rflav_ME_int(ileg)).eq.5) then
               rflav_ME_int(ileg)=sign(1,rflav_ME_int(ileg))
            endif
         endif
         do mu=0,3
            krlab_ME_int(mu,ileg)=p(mu,ileg)
         enddo
      enddo

c     check
      if (abs(rflav_loc(3)).ne.6) then
         write(*,*) 'st_tch_real_ampsq: ERROR in flavor assignement'
         call exit(1)
      endif

c     ew coupling
      ewcoupl=4d0*pi*alphaem_pow/sthw2_pow
      if(ini) then
         write(*,*) 'ew coupl= ',ewcoupl
         ini=.false.
      endif

ccccccccccccccccccccccccccccccccccccccccccc
c     >>> T CHANNEL <<<
ccccccccccccccccccccccccccccccccccccccccccc
      if(rflav_loc(nleg).eq.0) then
c     ddx
         if ((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(2)).eq.1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
            ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(3)))**2 
     #* CKM(abs(rflav_loc(2)),abs(rflav_loc(4)))**2 

c     dxd
         elseif ((three_ch(rflav_loc(1)).eq.1).and.
     #(three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
            ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(2)),abs(rflav_loc(3)))**2
            
c     ud
         elseif ((three_ch(rflav_loc(1)).eq.2).and.
     #(three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
            ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(2)),abs(rflav_loc(3)))**2
            
c     du
         elseif ((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(2)).eq.2).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
            ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(3)))**2 
     #* CKM(abs(rflav_loc(2)),abs(rflav_loc(4)))**2

         else
            write(*,*) 'Error in fill_real, (t,1)'
            call exit(1)
         endif

      elseif((rflav_loc(1).eq.0).or.(rflav_loc(2).eq.0)) then
c     If rflav_loc(5) is not a gluon, sometimes an exchange is needed,
c     since both madgraph and mcnlo subroutines assume an order of parton 4 and 5 id's.
c     In particular, they assume that the 4th and 5th flavours are sorted INCREASINGLY.
c     The only exception is for the process gu->tddx and ug->tddx.
c     Therefore, reorder the 'ME_int' variables, if needed.
c     !: WARNING
c     In the t-channel, the BOX can call this subroutine with the wrong
c     ordering, BUT NEVER for gu->tddx and ug->tddx.
c     An explicit check that ordering is always correct
c     for gu->tddx and ug->tddx is ALWAYS PERFORMED.
         if((rflav_ME_int(1)+rflav_ME_int(2)).eq.2) then
            if((rflav_ME_int(4).ne.1).or.(rflav_ME_int(5).ne.-1)) then
               write(*,*) 'Error in fill_real'
               write(*,*) 'rflav_loc ',rflav_loc
               write(*,*) 'rflav_ME_int ',rflav_ME_int
               call exit(1)
            else
               continue
            endif
         else
            if(rflav_ME_int(4).gt.rflav_ME_int(5)) then
c     reorder momenta
               do mu=0,3
                  ptemp(mu)=krlab_ME_int(mu,4)
                  krlab_ME_int(mu,4)=krlab_ME_int(mu,5)
                  krlab_ME_int(mu,5)=ptemp(mu)
               enddo
c     reorder the fundamental flavour assignment
               ileg=rflav_ME_int(4)
               rflav_ME_int(4)=rflav_ME_int(5)
               rflav_ME_int(5)=ileg
c     reorder also rflav_loc, since the ordering described above is assumed
c     also by the if-sequence that fills CKM weight
               ileg=rflav_loc(4)
               rflav_loc(4)=rflav_loc(5)
               rflav_loc(5)=ileg
            endif
         endif

         if(rflav_loc(1).eq.0) then
c     gdx -> tuxdx (t-channel)
            if ((three_ch(rflav_loc(1)).eq.0).and.
     #(three_ch(rflav_loc(2)).eq.1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(2)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(3)),abs(rflav_loc(5)))**2            
c     gu -> tddx (t-channel)
            elseif ((three_ch(rflav_loc(1)).eq.0).and.
     #(three_ch(rflav_loc(2)).eq.2).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(2)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(3)),abs(rflav_loc(5)))**2            
c     gd -> tuxd (t-channel only)
            elseif ((three_ch(rflav_loc(1)).eq.0).and.
     #(three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(2)),abs(rflav_loc(3)))**2 
     #* CKM(abs(rflav_loc(4)),abs(rflav_loc(5)))**2            
            else
               write(*,*) 'Error in fill_real, (t,2)',rflav
               call exit(1)
            endif
         elseif(rflav_loc(2).eq.0) then
c     dxg -> tuxdx (t-channel) 
            if ((three_ch(rflav_loc(1)).eq.1).and.
     #(three_ch(rflav_loc(2)).eq.0).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(3)),abs(rflav_loc(5)))**2            
c     ug -> tddx (t-channel)
            elseif ((three_ch(rflav_loc(1)).eq.2).and.
     #(three_ch(rflav_loc(2)).eq.0).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(4)))**2 
     #* CKM(abs(rflav_loc(3)),abs(rflav_loc(5)))**2            
c     dg -> tuxd (t-channel only)
            elseif ((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(2)).eq.0).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               ckm_b_t=
     #CKM(abs(rflav_loc(1)),abs(rflav_loc(3)))**2 
     #* CKM(abs(rflav_loc(4)),abs(rflav_loc(5)))**2
            else
               write(*,*) 'Error in fill_real, (t,3)'
               call exit(1)
            endif
         else
            write(*,*) 'Error in fill_real, g in initial state'
            call exit(1)
         endif                              
      else
         write(*,*) 'problem in st_tch_real_ampsq'
         call exit(1)
      endif


cccccccccccccccccccccccccccccccccccccccccccccc
      if(mad_ME) then
c     USING MADGRAPH SUBROUTINES
c     assign krlab_mad, with momenta ordered as in mcnlo
         do ileg=1,5
            do mu=0,3
               krlab_mad(mu,ileg)=krlab_ME_int(mu,ileg)
            enddo
         enddo
c     to avoid bugs in HELAS, restore exact masslessness of incoming partons 
         krlab_mad(0,1)=dabs(krlab_mad(3,1))
         krlab_mad(0,2)=dabs(krlab_mad(3,2))
c     needed to check undecayed matrix elements
         alfas=st_alpha
         mtMS=sqrt(st_tch_dotp(krlab_mad(0,3),krlab_mad(0,3)))
         tmass=mtMS
         twidth=0d0
         wwidth=0d0
         call st_tch_my_setpara

         if(three_ch(rflav_loc(1)).eq.0) then
c     incoming gluon (leg 1)
c     gd -> tuxd
            if((three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sgb_tuxd_T(krlab_mad,amp2_mad)
c     gdx -> tuxdx
            elseif((three_ch(rflav_loc(2)).eq.1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sgdx_tuxbx_T(krlab_mad,amp2_mad)
c     gu -> tddx
            elseif((three_ch(rflav_loc(2)).eq.2).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               call st_tch_Sgu_tdbx_T(krlab_mad,amp2_mad)
            else
               write(*,*) 'Wrong rflav in gq: ',rflav
               call exit(1)
            endif
         elseif(three_ch(rflav_loc(2)).eq.0) then
c     incoming gluon (leg 2)
c     dg -> tuxd
            if((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sbg_tuxd_T(krlab_mad,amp2_mad)
c     dxg -> tuxdx
            elseif((three_ch(rflav_loc(1)).eq.1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sdxg_tuxbx_T(krlab_mad,amp2_mad)
c     ug -> tddx
            elseif((three_ch(rflav_loc(1)).eq.2).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               call st_tch_Sug_tdbx_T(krlab_mad,amp2_mad)
            else
               write(*,*) 'Wrong rflav in qg: ',rflav_loc
               call exit(1)
            endif
         elseif((three_ch(rflav_loc(1)).ne.0).and.
     #(three_ch(rflav_loc(2)).ne.0))then
c     born-like + outgoing gluon (i.e. as born)
c     ddx
            if ((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(2)).eq.1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sbdx_tuxg(krlab_mad,amp2_mad)
c     dxd
            elseif ((three_ch(rflav_loc(1)).eq.1).and.
     #(three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-2)) then
               call st_tch_Sdxb_tuxg(krlab_mad,amp2_mad)
c     ud
            elseif ((three_ch(rflav_loc(1)).eq.2).and.
     #(three_ch(rflav_loc(2)).eq.-1).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               call st_tch_Sub_tdg(krlab_mad,amp2_mad)
c     du
            elseif ((three_ch(rflav_loc(1)).eq.-1).and.
     #(three_ch(rflav_loc(2)).eq.2).and.
     #(three_ch(rflav_loc(4)).eq.-1)) then
               call st_tch_Sbu_tdg(krlab_mad,amp2_mad)
            else
               write(*,*) 'Wrong rflav in qq: ',rflav_loc
               call exit(1)
            endif
         else
            write(*,*) 'Error in tdecay (real, tch)'
            call exit(1)
         endif 
      endif
ccccccccccccccccccccccccccccccccccccc


cccccccccccccccccccccccccccc
c     assign output
      amp2_mad=amp2_mad               *ckm_b_t

      amp2=amp2_mad/(st_alpha/2./pi)
cccccccccccccccccccccc
      end
