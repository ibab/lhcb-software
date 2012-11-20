      subroutine zz_pwhgevent
      implicit none
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_flg.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      include 'src/Lib/zz/LesHouches.h'
      integer iret,iun
      real * 8 suppfact
      real * 8 zz_random,zz_powheginput
      external zz_random,zz_powheginput
      integer mcalls,icalls
      real * 8 zz_pwhg_pt2,zz_pt2max_regular
      external zz_pwhg_pt2,zz_pt2max_regular
      integer i
      if(idwtup.eq.3) then
         xwgtup=1
      elseif(idwtup.eq.-4) then
         xwgtup=rad_totgen * rad_branching
      else
         write(*,*) ' only 3 and -4 are allowed for idwtup'
         call exit(-1)
      endif
c     store current zz_random seeds. To be used to restart at problematic events
      call zz_savecurrentrandom
      if(zz_random().gt.rad_totrm/rad_totgen) then
c     generate underlying Born kinematics
         call zz_gen_btilde(mcalls,icalls)
c     generate underlying Born flavour
         call zz_gen_uborn_idx
c
         if(zz_powheginput('#zz_testsuda').eq.1) then
            call zz_testsuda
         endif
c generate radiation
         call zz_gen_radiation
c add a zz_random azimuthal rotation around beam axis
         call zz_add_azimuth
c --- set up les houches interface
         rad_pt2max=zz_pwhg_pt2()
         call zz_gen_leshouches
c if negative weight, flip the sign of xwgtup
         if(rad_btilde_sign(rad_ubornidx).eq.-1) then
            xwgtup=-xwgtup
         endif
c rad_type=1 for zz_btilde events (used only for debugging purposes)
         rad_type=1
         if(flg_weightedev) then
            call zz_born_suppression(suppfact)
            if(suppfact.eq.0) then
               write(*,*) ' 0 suppression factor in event generation'
               write(*,*) ' aborting'
               call exit(-1)
            endif
            xwgtup=xwgtup/suppfact
         endif
      else
c generate remnant n+1 body cross section
         call zz_gen_sigremnant
c pick a configuration according to its cross section
c iret=1: rem contribution (leftover from damping factor on R)
c iret=2: reg contribution (real graphs without singular regions)
         call zz_gen_remnant(iret)
c         if (zz_pwhg_pt2().lt.rad_ptsqmin) then
c            write(*,*) '****************************************'
c            write(*,*) 'WARNING in zz_gen_remnant'
c            write(*,*) 'zz_pwhg_pt2 < rad_ptsqmin ',
c     #           zz_pwhg_pt2(),' < ',rad_ptsqmin
c            write(*,*) (flst_alr(i,rad_realalr),i=1,nlegreal)
c            write(*,*) 'To generate this event, use the following seeds'
c            call zz_printcurrentrandom
c            write(*,*) '****************************************'
c         endif
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC
         call zz_add_azimuth
         if(iret.eq.1) then
c     set st_muren2 equal to pt2 for scalup value
            rad_pt2max=max(rad_ptsqmin,zz_pwhg_pt2())
            call zz_set_rad_scales(rad_pt2max)
            call zz_gen_leshouches
            call zz_born_suppression(suppfact)
            if(suppfact.eq.0) then
               write(*,*) ' 0 suppression factor in event generation'
               write(*,*) ' aborting'
               call exit(-1)
            endif
            xwgtup=xwgtup/suppfact
c     rad_type=2 for remnants
            rad_type=2
         else
c     set st_muren2 for scalup value for regular contributions
            rad_pt2max=max(rad_ptsqmin,zz_pt2max_regular())
            call zz_set_rad_scales(rad_pt2max)
            call zz_gen_leshouches_reg
c rad_type=3 for regular contributions
            rad_type=3
         endif         
      endif
      end


      subroutine zz_gen_radiation
      implicit none
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      real * 8 zz_t,csi,y,azi,sig
      real * 8 tmax
      common/zz_ctmax/tmax
      integer kinreg,firstreg,lastreg
      logical ini
      data ini/.true./
      real * 8 zz_pwhg_pt2,zz_powheginput
      external zz_pwhg_pt2,zz_powheginput
      save ini,firstreg,lastreg
      if(ini) then
         firstreg=zz_powheginput("#radregion")
         if(firstreg.le.0) then
            firstreg=1
            lastreg=rad_nkinreg
         else
            lastreg=firstreg
         endif
         ini=.false.
      endif
c Use highest bid procedure (see appendix B of FNO2006)
      tmax=0
      kinreg=0
      do rad_kinreg=firstreg,lastreg
         if(rad_kinreg_on(rad_kinreg)) then
            if(rad_kinreg.eq.1) then
c     initial state radiation
               call zz_gen_rad_isr(zz_t)
            else
c     final state radiation
               call zz_gen_rad_fsr(zz_t)
            endif
            if(zz_t.gt.tmax) then
               tmax=zz_t
               kinreg=rad_kinreg
               csi=kn_csi
               y=kn_y
               azi=kn_azi
            endif
         endif
      enddo
c Set up radiation kinematics
      if(tmax.eq.0) then
c Generate a Born like event
         kn_csi=0
         rad_kinreg=0
         return
      else         
         rad_kinreg=kinreg
         kn_csi=csi
         kn_y=y
         kn_azi=azi
         zz_t=tmax
         if(rad_kinreg.eq.1) then
            call zz_gen_real_phsp_isr_rad
         else
            call zz_gen_real_phsp_fsr_rad
         endif
         call zz_set_rad_scales(zz_t)
         call zz_sigreal_rad(sig)
         call zz_gen_real_idx
      endif
      end



      function zz_pwhg_pt2()
      implicit none
      real * 8 zz_pwhg_pt2
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      if(rad_kinreg.eq.1) then
         zz_pwhg_pt2=(kn_sreal/4)*(1-kn_y**2)*kn_csi**2
      else
         zz_pwhg_pt2=(kn_sreal/2)*(1-kn_y)*kn_csi**2
      endif
      end



      function zz_pwhg_upperb_rad()
      implicit none
      real * 8 zz_pwhg_upperb_rad
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      include 'src/Lib/zz/pwhg_st.h'
      real * 8 x,y,csi
      csi=kn_csi
      x=1-csi
      y=kn_y
      if(rad_kinreg.eq.1) then
         if(rad_iupperisr.eq.1) then
            zz_pwhg_upperb_rad = st_alpha/((1-x)*(1-y**2))
c Possible alternatives:
c rad_iupper=2   zz_pwhg_upperb_rad = st_alpha/(x*(1-x)*(1-y**2))
c 
c rad_iupper=3:  zz_pwhg_upperb_rad = st_alpha/(x**2*(1-x)*(1-y**2))
         else
            write(*,*) ' rad_iupper=',rad_iupperisr,
     1        'alternative not implemented'
            call exit(1)
         endif
      else
c for now use the same
         if(rad_iupperfsr.eq.1) then
            zz_pwhg_upperb_rad = st_alpha/(csi*(1-y))
         elseif(rad_iupperfsr.eq.2) then
            zz_pwhg_upperb_rad = st_alpha/(csi**2*(1-y)*(1-csi/2*(1-y))**2)
     2          *csi
         elseif(rad_iupperfsr.eq.3) then
            zz_pwhg_upperb_rad = st_alpha/(csi*(1-y)*
     2         (1-csi/2*(1-y)))
         else
            write(*,*) ' rad_iupper=',rad_iupperfsr,
     1            'alternative not implemented'
            call exit(1)
         endif
      endif
      end



      function zz_pt2solve(pt2,i)
c Returns  xlr - log(Delta^{(tilde{V})}) , see eq. D14, D15 in ZZ paper
c We use it to find its zero in pt2.
      implicit none
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      include 'src/Lib/zz/pwhg_math.h'
      real * 8 zz_pt2solve,pt2
c i set by zz_DZERO: 1 for first call, 2 for subsequent calls, 3 for last call
c before a normal exit; not used here
      integer i
      real * 8 xlr,q2,xlam2c,kt2max,cunorm
      integer nlc
      common/zz_cpt2solve/xlr,q2,kt2max,xlam2c,cunorm,nlc
      real * 8 b0,sborn,xm,p
      sborn=kn_sborn
      b0=(11*CA-4*TF*nlc)/(12*pi)
      if(rad_kinreg.eq.1) then
         if(rad_iupperisr.eq.1) then
c see Notes/upperbounding-isr.pdf
            if(pt2.lt.sborn) then
               if(sborn.lt.kt2max) then
                  zz_pt2solve=cunorm*pi/b0*(
     #      (log(2*sborn/xlam2c)*log(log(sborn/xlam2c)/log(pt2/xlam2c))
     #        - log(sborn/pt2)) +
     #           log(2d0)*log(log(kt2max/xlam2c)/log(sborn/xlam2c)))
     #           + xlr
               else
                  zz_pt2solve=cunorm*pi/b0*(
     #     (log(2*sborn/xlam2c)*log(log(kt2max/xlam2c)/log(pt2/xlam2c))
     #        - log(kt2max/pt2)) )
     #           + xlr
               endif
            else
               zz_pt2solve=cunorm*pi/b0*(log(2d0)
     #           *log(log(kt2max/xlam2c)/log(pt2/xlam2c)))
     #           + xlr
            endif
         else
            write(*,*) ' rad_iupper=',rad_iupperisr,' not implemented'
            call exit(1)
c Alternatives: rad_iupper=2
c         zz_pt2solve=cunorm*pi/b0/2
c     #        *(log(q2/xlam2c)*log(log(kt2max/xlam2c)/log(pt2/xlam2c))
c     #        - log(kt2max/pt2)) + xlr
         endif
      else
         if(rad_iupperfsr.eq.1) then
c final state radiation
         zz_pt2solve=cunorm*pi/b0*(
     #     (log(kt2max/xlam2c)*log(log(kt2max/xlam2c)/log(pt2/xlam2c))
     #        - log(kt2max/pt2)) )
     #           + xlr
         elseif(rad_iupperfsr.eq.2) then
            xm=kn_csimax
            p=sqrt(pt2/sborn)
            zz_pt2solve=cunorm*2*pi*2*(
     3   (log(xm-xm**2)+(2*xm-2)*log(xm)-2*log(1-xm)*xm-2)/xm/2.d+0
     1   -(p*log(xm-p**2)+(2*p*log(p)-2*log(1-p)*p-2)*xm-2*p*log(p))
     2   /(p*xm)/2.d+0) + xlr
         elseif(rad_iupperfsr.eq.3) then
            xm=kn_csimax
            p=sqrt(pt2/sborn)
            zz_pt2solve=cunorm*2*pi*2*(
     3   (log(xm-xm**2)+(2*xm-2)*log(xm)-2*log(1-xm)*xm-2)/xm/2.d+0
     1   -(p*log(xm-p**2)+(2*p*log(p)-2*log(1-p)*p-2)*xm-2*p*log(p))
     2   /(p*xm)/2.d+0) + xlr
         else
            write(*,*) ' rad_iupper=',rad_iupperfsr,' not implemented'
            call exit(1)
         endif            
      endif
      end

      subroutine zz_gen_rad_isr(zz_t)
c Generates hard radiation kinematics according to
c appendix D in ZZ paper.
c
c  common/zz_cptmin/ptminsq: minimum pt^2 accepted
c 
      implicit none
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      include 'src/Lib/zz/pwhg_st.h'
      real * 8 zz_t
      real * 8 x,y,x1b,x2b
      real * 8 xlr,q2,xlam2c,kt2max,unorm
      integer nlc
      common/zz_cpt2solve/xlr,q2,kt2max,xlam2c,unorm,nlc
      real * 8 xmin,rv,xp,xm,chi,tk,uk,ubound,ufct,
     #   sborn,value,err,tmp1,tmp2,tmp,rvalue,born,sig
      common/zz_cdfxmin/xmin
      real * 8 tmax
      common/zz_ctmax/tmax
      real * 8 zz_random,zz_pt2solve,dfxmin,zz_pwhg_alphas0,zz_pwhg_upperb_rad
      external zz_random,zz_pt2solve,dfxmin,zz_pwhg_alphas0,zz_pwhg_upperb_rad
      unorm=rad_norms(rad_kinreg,rad_ubornidx)
      sborn=kn_sborn
      x1b=kn_xb1
      x2b=kn_xb2
c See Notes/kt2max.pdf
      kt2max = sborn*(1-x2b**2)*(1-x1b**2)/(x1b+x2b)**2
      if(kt2max.lt.rad_ptsqmin.or.kt2max.lt.tmax) then
         zz_t=-1
         goto 3
      endif
c upper bound is log(q2/zz_t)
      if(rad_iupperisr.eq.1) then
         q2=2*sborn
      elseif(rad_iupperisr.eq.2) then
         write(*,*) ' rad_iupper=',rad_iupperisr,' not implemented'
         call exit(1)
c Alternative rad_iupper=2
c         q2=4*sborn/min(x1b,x2b)**2
      endif
c see section 4 in ZZ paper, last paragraph
      xlam2c=rad_lamll**2
      nlc=5
      xlr=0
 1    continue
      xlr=xlr+log(zz_random())
c CERNLIB voodoo:
      call zz_KERSET('C205. ',0,0,101)
c solve for zero of zz_pt2solve
c zz_DZERO(xmin,xmax,x,err,eps,maxcalls,function)
c err: on exit if no error occours: |y-y0|<err 
c      error C205.1 function(xmin)*function(xmax)>0,
c                   x=0 and r=-2(ymax-ymin)
c      error C205.2 Number of calls to F exceeds maxcalls,
c                   x=0 and r=-(xmax-xmin)/2
c eps: required accuracy
      call zz_DZERO(rad_ptsqmin,kt2max,zz_t,err,1d-8,1000000,zz_pt2solve)
c error conditions
      if(zz_t.eq.0.and.err.lt.0d0
     # .and.err.gt.rad_ptsqmin-kt2max) then
         write(*,*) 'zz_DZERO fails'
         write(*,*) ' number of calls exceeded'
         call exit(1)
      endif
 3    if(zz_t.lt.rad_ptsqmin.or.zz_t.lt.tmax) then
c below cut (either below absolute minimum, or below previously generated
c radiation in highest bid loop): generate a born event
         zz_t=-1
         kn_csi=0
         return
      endif
c vetoes:
      rv=zz_random()
      xp=(sqrt(1+zz_t/sborn)+sqrt(zz_t/sborn))**2
      xm=(sqrt(1+zz_t/sborn)-sqrt(zz_t/sborn))**2
c tmp1: V(zz_t)/tilde{V}(zz_t) in appendix D of ZZ paper;
c (typo: in D.13, log log -> log
      xmin=min(x1b,x2b)/(2*sqrt(1+zz_t/sborn))
      if(rad_iupperisr.eq.1) then
         tmp1=log((sqrt(xp-xmin)+sqrt(xm-xmin))
     #       /(sqrt(xp-xmin)-sqrt(xm-xmin)))
         if(zz_t.lt.sborn) then
            tmp1=tmp1/(log(2*sborn/zz_t)/2)
         else
            tmp1=tmp1/(log(2d0)/2)
         endif
      elseif(rad_iupperisr.eq.2) then
         tmp1=log(2/xmin*(sqrt((xp-xmin)*(xm-xmin))
     # +1-xmin/2*(xp+xm))/(xp-xm)) /(log(q2/zz_t)/2)
      endif
c compare with D.11-D.12
c to set xmuren2:
      call zz_set_rad_scales(zz_t)
      tmp2=st_alpha / zz_pwhg_alphas0(zz_t,rad_lamll,nlc)
      tmp=tmp1*tmp2
      if(tmp.gt.1) then
         write(*,*) ' Error: upper bound lower than actual value',
     #        tmp,tmp1,tmp2,zz_t
         call exit(1)
      endif
      if(rv.gt.tmp) then
         goto 1
      endif
c At this stage: pt generated according to D.2
c generate x proportional to 1/(x sqrt((xp-x)*(xm-x)))
c in the range xmin < x < xm (cf. D.5)
c Generate in d sqrt(xm-x) /sqrt(xp-x)  (rad_iupper=1) or d sqrt(xm-x) /(x sqrt(xp-x)) (rad_iupper=2)
c using       d sqrt(xm-x) /sqrt(xp-xm) (rad_iupper=1) or d sqrt(xm-x) /(xmin sqrt(xp-xm)) (rad_iupper=2) as upper bound using hit and miss
 2    chi=sqrt(xm-xmin)*zz_random()
      x=xm-chi**2
      if(rad_iupperisr.eq.1) then
         if(zz_random().gt.sqrt(xp-xm)/sqrt(xp-x)) goto 2
      elseif(rad_iupperisr.eq.2) then
         if(zz_random().gt.(xmin*sqrt(xp-xm))/(x*sqrt(xp-x))) goto 2
      endif
c get y (abs to avoid tiny negative values)
      y=sqrt(abs(1-4*x/(1-x)**2*zz_t/sborn))
      if(zz_random().gt.0.5d0) y=-y
c At this point an x-y pair is generated according to the
c distribution upper().
c
c Veto if out of range (x1>1 or x2>1)
      tk=-1d0/2*(1-x)*(1-y)
      uk=-1d0/2*(1-x)*(1+y)
      if(   x1b*sqrt((1+tk)/(1+uk)/x) .gt. 1
     # .or. x2b*sqrt((1+uk)/(1+tk)/x) .gt. 1) then
         goto 1
      endif
c extra suppression factor of upper bounding function (may depend upon radiation variables)
      call zz_uboundfct(ufct,1-x,y)
      if(zz_random().gt.ufct) goto 1
c Veto from upper bound to real value. Count how many vetoes,
c since these may be expensive.
      call zz_sigborn_rad(born)
      if(born.lt.0) then
         born=0
      endif
      if(born.eq.0) then
c bizarre situation that may arise when the scale gets so low
c that some pdf vanish (typically heavy flavour pdf's)
         zz_t=-1
         goto 3
      endif
      kn_y=y
      kn_csi=1-x
      kn_azi=2*pi*zz_random()
      ubound=born*zz_pwhg_upperb_rad()*unorm*ufct
      call zz_gen_real_phsp_isr_rad
      call zz_sigreal_rad(sig)
      value=sig*kn_jacreal
      if(value.gt.ubound) then
         call zz_increasecnt(
     # 'upper bound failures in generation of radiation')
      endif
      rvalue=zz_random()*ubound
      if(rvalue.gt.value) then
         call zz_increasecnt('vetoed radiation')
         goto 1
      endif
      end


      subroutine zz_gen_rad_fsr(zz_t)
c Generates final state hard radiation kinematics according to
c Notes/upperbounding-fsr.pdf
c 
      implicit none
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_kn.h'
      include 'src/Lib/zz/pwhg_rad.h'
      include 'src/Lib/zz/pwhg_st.h'
      real * 8 zz_t
      real * 8 csi,y
      real * 8 xlr,q2,xlam2c,kt2max,unorm
      integer nlc
      common/zz_cpt2solve/xlr,q2,kt2max,xlam2c,unorm,nlc
      real * 8 xmin,rv,ubound,ufct,
     #   s,value,err,tmp,rvalue,born,sig
      common/zz_cdfxmin/xmin
      real * 8 tmax
      common/zz_ctmax/tmax
      real * 8 zz_random,zz_pt2solve,zz_pwhg_alphas0,zz_pwhg_upperb_rad
      external zz_random,zz_pt2solve,zz_pwhg_alphas0,zz_pwhg_upperb_rad
      unorm=rad_norms(rad_kinreg,rad_ubornidx)
c kn_sborn=kn_sreal:
      s=kn_sborn
      kn_emitter=flst_lightpart+rad_kinreg-2
      kn_csimax=kn_csimax_arr(kn_emitter)
c See Notes/kt2max.pdf
      kt2max = kn_csimax**2*s
      if(kt2max.lt.rad_ptsqmin.or.kt2max.lt.tmax) then
         zz_t=-1
         goto 3
      endif
c see section 4 in ZZ paper, last paragraph
      xlam2c=rad_lamll**2
      nlc=5
      xlr=0
 1    continue
      xlr=xlr+log(zz_random())
c CERNLIB voodoo:
      call zz_KERSET('C205. ',0,0,101)
c solve for zero of zz_pt2solve
c zz_DZERO(xmin,xmax,x,err,eps,maxcalls,function)
c err: on exit if no error occours: |y-y0|<err 
c      error C205.1 function(xmin)*function(xmax)>0,
c                   x=0 and r=-2(ymax-ymin)
c      error C205.2 Number of calls to F exceeds maxcalls,
c                   x=0 and r=-(xmax-xmin)/2
c eps: required accuracy
      call zz_DZERO(rad_ptsqmin,kt2max,zz_t,err,1d-8,1000000,zz_pt2solve)
c error conditions
      if(zz_t.eq.0.and.err.lt.0d0
     # .and.err.gt.rad_ptsqmin-kt2max) then
         write(*,*) 'zz_DZERO fails'
         write(*,*) ' number of calls exceeded'
         call exit(1)
      endif
 3    if(zz_t.lt.rad_ptsqmin.or.zz_t.lt.tmax) then
c below cut (either below absolute minimum, or below previously generated
c radiation in highest bid loop): generate a born event
         zz_t=-1
         kn_csi=0
         return
      endif
c vetoes:
      rv=zz_random()
      call zz_set_rad_scales(zz_t)
      if(rad_iupperfsr.eq.1) then
         tmp=st_alpha / zz_pwhg_alphas0(zz_t,rad_lamll,nlc)
      elseif(rad_iupperfsr.eq.2) then
         tmp=st_alpha
      elseif(rad_iupperfsr.eq.3) then
         tmp=st_alpha
      endif
      if(tmp.gt.1) then
         write(*,*) ' Error: upper bound lower than actual value',
     #        tmp,zz_t
         call exit(1)
      endif
      if(rv.gt.tmp) then
         goto 1
      endif
      if(rad_iupperfsr.eq.1) then         
c At this stage: pt generated according to (1) of upperbounding-fsr.pdf;
c generate csi uniformly in 1/csi
c in the range zz_t/s < csi^2 < csimax^2
         rv=zz_random()
         csi=exp(rv*log(zz_t/s)/2+(1-rv)*log(kn_csimax))
c get y
         y=1-2*zz_t/(s*csi**2)
c At this point a csi-y pair is generated according to the
c distribution upper(). It is automatically within range.
      elseif(rad_iupperfsr.eq.2) then
c     csi distributed uniformly in 1/(csi-zz_t/s)
         rv=zz_random()
         csi=1/(rv/(sqrt(zz_t/s)-zz_t/s)+(1-rv)/(kn_csimax-zz_t/s))+zz_t/s
c extra csi dependent factor
         if(zz_random().gt.csi) goto 1
c get y
         y=1-2*zz_t/(s*csi**2)
c At this point a csi-y pair is generated according to the
c distribution upper(). It is automatically within range.
      elseif(rad_iupperfsr.eq.3) then
c     csi distributed uniformly in 1/(csi-zz_t/s)
         rv=zz_random()
         csi=1/(rv/(sqrt(zz_t/s)-zz_t/s)+(1-rv)/(kn_csimax-zz_t/s))+zz_t/s
c get y
         y=1-2*zz_t/(s*csi**2)
         if(zz_random().gt.(csi-zz_t/s)) goto 1
      else
         write(*,*) ' zz_gen_rad_fsr:  rad_iupper=',rad_iupperfsr,
     1        ' invalid'
      endif
c
c extra suppression factor of upper bounding function (may depend upon radiation variables)
      call zz_uboundfct(ufct,csi,y)
      if(zz_random().gt.ufct) goto 1
c Veto from upper bound to real value. Count how many vetoes,
c since these may be expensive.
c      write(*,*) ' genrad_fsr: y and csi ',y,csi
      call zz_sigborn_rad(born)
      if(born.lt.0) then
         born=0
      endif
      if(born.eq.0) then
c bizarre situation that may arise when the scale gets so low
c that some pdf vanish (typically heavy flavour pdf's)
         zz_t=-1
         goto 3
      endif
      kn_y=y
      kn_csi=csi
      kn_azi=2*pi*zz_random()
      ubound=born*zz_pwhg_upperb_rad()*unorm*ufct
      call zz_gen_real_phsp_fsr_rad
      call zz_sigreal_rad(sig)
      value=sig*kn_jacreal
      if(value.gt.ubound) then
         call zz_increasecnt(
     # 'upper bound failures in generation of radiation')
      endif
      rvalue=zz_random()*ubound
      if(rvalue.gt.value) then
         call zz_increasecnt('vetoed radiation')
         goto 1
      endif
      end


      subroutine zz_add_azimuth
      implicit none
      include 'src/Lib/zz/nlegborn.h'
      include 'src/Lib/zz/pwhg_flst.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/pwhg_kn.h'
      integer ileg
      real * 8 azi,sazi,cazi
      real * 8 dir(3)
      data dir/0d0,0d0,1d0/
      real * 8 zz_random
      external zz_random
      azi=2d0*pi*zz_random()
      sazi = sin(azi)
      cazi = cos(azi)
      if (kn_csi.eq.0d0) then
         do ileg=1, nlegborn
            call zz_mrotate(dir,sazi,cazi,kn_pborn(1,ileg))
         enddo
      else
         do ileg=1, nlegreal
            call zz_mrotate(dir,sazi,cazi,kn_preal(1,ileg))
         enddo
      endif
      end
      
      
