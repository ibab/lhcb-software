      subroutine gg_h_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      real * 8 amp2

      amp2 = 0d0
      call gg_h_compborn(p,bflav,amp2,bornjk,bmunu)      
      if(amp2.eq.0d0) then
        write(*,*) 'WARNING gg_h_setborn: returning 0 amplitude!'
        write(*,*) bflav(1),' ',bflav(2),'->',bflav(3)
      endif
      born=amp2
      end

      subroutine gg_h_setbornmassdep
      implicit none
      logical large_mtlim,bloop
      common/gg_h_clarge_mtlim/large_mtlim,bloop
      logical ini
      data ini/.true./ 
      save ini
      integer mtdep
      common/gg_h_cmtdep/mtdep
      real *8 gg_h_powheginput
      external gg_h_powheginput
      logical initialized
      common/gg_h_cinitialized/initialized
      if(ini) then
         mtdep=int(gg_h_powheginput("#largemtlim"))
         if (mtdep.lt.0) then
            mtdep=0
         endif
         if(mtdep.eq.0) then
            write(*,*)
            write(*,*) ' POWHEG: top gg_h_mass -> inf for both Born'
            write(*,*) '         and NLO contributions.'
            write(*,*) '         Results rescaled by a finite top-gg_h_mass'
            write(*,*) '         correction factor given by the ratio'
            write(*,*) '         Born(mt=topmass) / Born(mt=inf).' 
            write(*,*)
            bloop=gg_h_powheginput("#includebloop").eq.1
            if(bloop) then
               write(*,*) '        b-quark loop contributions included'
               write(*,*) '        in rescaling factor.             ' 
               write(*,*)
            endif
         elseif(mtdep.eq.1) then
            write(*,*)
            write(*,*) ' POWHEG: top gg_h_mass -> inf for both Born'
            write(*,*) '         and NLO contributions.'
            write(*,*)
         elseif(mtdep.eq.2) then
            write(*,*)
            write(*,*) ' POWHEG: Exact top gg_h_mass dependence in Born.'
            write(*,*) '         top gg_h_mass -> inf for NLO contributions.'
            write(*,*)
         else   
            write(*,*) ' Error with mtdep flag'
            call exit(1)
         endif   
         ini=.false.
         initialized=.true.
      endif
      if(mtdep.ne.2) then   
         large_mtlim=.true.
      else
         large_mtlim=.false.
      endif
      end
      
      subroutine gg_h_setbornmass2inf
      implicit none
      logical large_mtlim,bloop
      common/gg_h_clarge_mtlim/large_mtlim,bloop
      integer mtdep
      common/gg_h_cmtdep/mtdep
      large_mtlim=.true.
      if (mtdep.eq.2) then
c     Only re-evaluate the Born in the large mt-limit
c     if they were not already evaluated previously
         call gg_h_allborn
      endif
      end
      
      subroutine gg_h_compborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born
      integer mu,nu,i,j,k
      real* 8 amp2_inf,colcf
      real * 8 gtens(0:3,0:3)
      data gtens/1d0, 0d0, 0d0, 0d0,
     #           0d0,-1d0, 0d0, 0d0,
     #           0d0, 0d0,-1d0, 0d0,
     #           0d0, 0d0, 0d0,-1d0/
      save gtens
      logical large_mtlim,bloop
      common/gg_h_clarge_mtlim/large_mtlim,bloop
      data large_mtlim/.true./
C     large_mtlim has to be initialized to .true.
C     in order to correctly perform the check
C     of gg_h_soft/collinear limits
      if(.not.large_mtlim) then
C    Born Cross section with finite top gg_h_mass 
         call gg_h_M2_gg_h(p,1,born)
      else
C     Born Cross section in the large top gg_h_mass limit
         call gg_h_M2_gg_h(p,2,amp2_inf)
c     Born amplitude and the color- and spin-correlated ones, needed as
c     counterterms for real contributions, must be evaluated only in
c     the M_top --> inf limit (amp2_inf)
         born=amp2_inf
         do j=1,nlegs
            do mu=0,3
               do nu=0,3
                  bmunu(mu,nu,j)=0d0
               enddo
            enddo
            do k=j+1,nlegs
               bornjk(j,k)=0d0
               bornjk(k,j)=bornjk(j,k)
            enddo
            if(abs(bflav(j)).le.6) then
C     Spin-correlated Born amplitudes
               if(bflav(j).eq.0) then
                  if(j.eq.1) then 
                     i=2
                  elseif(j.eq.2) then
                     i=1
                  else
                     write(*,*) 'Error in bmunu'
                     call exit(1)
                  endif
                  do mu=0,3
                     do nu=0,3
                        bmunu(mu,nu,j)=amp2_inf* ((p(mu,j)*p(nu,i)+p(nu
     $                       ,j)*p(mu,i))/kn_sborn-gtens(mu,nu)/2d0) 
                     enddo
                  enddo
               endif
c     Colour factors for colour-correlated Born amplitudes; Rule from
c     2.98 in FNO2007, leads to B_ij=Cj * B, where i#j
               do k=j+1,nlegs
                  if(bflav(k).eq.0) then
                     colcf=ca
                     bornjk(j,k)=amp2_inf*colcf
                     bornjk(k,j)=bornjk(j,k)
                  endif
               enddo
            endif
         enddo
      endif
      end
      
      subroutine gg_h_M2_gg_h(pphy,iborn,amp2)
c     Born matrix element times normalizations and averages.  IMPORTANT:
c     the flux factor 1/2s is intentionally missing The results are
c     given exact in M_top (iborn=1) or in the M_top --> inf limit
c     (iborn=2). We assume that only the top quark is flowing in the
c     loop
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/PhysPars.h'
      real * 8 s,v2,tiny,tmp,xnorm,tauq,etapl,etamn
      complex * 16 zic,tmpc
      parameter (v2=0.70710678118654757d0)
      parameter (tiny=1.d-8)
      parameter (zic=(0.d0,1.d0))
      real* 8 amp2
      integer nlegs,iborn
      parameter (nlegs=nlegborn)
      real *8 pphy(0:3,nlegs)
      s=(pphy(0,1)+pphy(0,2))**2-(pphy(1,1)+pphy(1,2))**2
     $     -(pphy(2,1)+pphy(2,2))**2 -(pphy(3,1)+pphy(3,2))**2
      xnorm=1/(2.d0*pi)*1/(pi)*s/(256*v2)
c     1/(2pi) comes from the 2*pi*delta(s-m^2) of phase space
      if(iborn.eq.1)then
c     From eq.(2.2) of NPB359(91)283
         tauq=4*ph_topmass**2/s
         if(tauq.gt.1.d0)then
            tmp=tauq*(1+(1-tauq)*(asin(1/sqrt(tauq)))**2)
            tmp=tmp**2
         else
            etapl=1+sqrt(1-tauq)
            etamn=1-sqrt(1-tauq)
            tmpc=tauq*(1-(1-tauq)*(log(etapl/etamn)-zic*pi)**2/4.d0)
            tmp=tmpc*dconjg(tmpc)
         endif
      elseif(iborn.eq.2)then
         tmp=4.d0/9.d0
      else
         write(*,*)'Unknown option in f1born',iborn
         call exit(1)
      endif
      
      amp2=xnorm*tmp*st_alpha*st_alpha*ph_GF*2d0*s
c     the multiplication for 2s is needed to remove the flux factor
       end


      function gg_h_afunc(mq,mh)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/PhysPars.h'
      real*8 tauq,mq,mh,tmp,etapl,etamn
      complex * 16 gg_h_afunc,tmpc,zic
      parameter (zic=(0.d0,1.d0))
c     From eq.(2.2) of NPB359(91)283
      tauq=4*(mq/mh)**2
      tmpc=(0d0,0d0)
      if(tauq.gt.1.d0)then
         tmp=tauq*(1+(1-tauq)*(asin(1/sqrt(tauq)))**2)
         tmpc=tmp
      else
         etapl=1+sqrt(1-tauq)
         etamn=1-sqrt(1-tauq)
         tmpc=tauq*(1-(1-tauq)*(log(etapl/etamn)-zic*pi)**2/4.d0)
      endif
      gg_h_afunc=tmpc
      end

      function gg_h_finitemtcorr()
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/PhysPars.h'
      integer mtdep
      common/gg_h_cmtdep/mtdep
      real * 8 gg_h_finitemtcorr
      complex * 16 zic,tmpc,gg_h_afunc
      external gg_h_afunc
      parameter (zic=(0.d0,1.d0))
      real * 8 tmp,tauq,etapl,etamn
      logical large_mtlim,bloop
      common/gg_h_clarge_mtlim/large_mtlim,bloop
      logical initialized,ini
      common/gg_h_cinitialized/initialized
      save ini
      data ini/.true./
      if ((mtdep.ne.0).or.(.not.initialized)) then
         gg_h_finitemtcorr=1d0
         return
      else
         tmpc=gg_h_afunc(ph_topmass,ph_hmass)
         if(bloop) tmpc=tmpc+gg_h_afunc(ph_bmass,ph_hmass)
         tmp=tmpc*dconjg(tmpc)
      endif
c     divide by the result in large top gg_h_mass limit
      gg_h_finitemtcorr=tmp*9d0/4d0
      if (ini) then
         write(*,*)
         write(*,*) " POWHEG: finite top-gg_h_mass correction "
         write(*,*) " factor is :", gg_h_finitemtcorr
         write(*,*)
         ini=.false.
      endif
      end

      subroutine gg_h_borncolour_lh
c     Sets up the colour for the given flavour configuration already
c     filled in the Les Houches interface.  In case there are several
c     colour structure, one should pick one with a probability
c     proportional to the value of the corresponding cross section, for
c     the kinematics defined in the Les Houches interface
      include 'src/Lib/gg_h/LesHouches.h'
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
c colours of incoming gluons
      integer icolgi(2)
      data icolgi/ 502, 501 /
      save icolgi

      icolup(1,1)=icolgi(1)
      icolup(2,1)=icolgi(2)
      icolup(1,2)=icolgi(2)
      icolup(2,2)=icolgi(1)
c neutral particles
      icolup(1,3)=0
      icolup(2,3)=0

      end

      subroutine gg_h_finalize_lh
c     Set up the resonances whose gg_h_mass must be preserved
c     on the Les Houches interface.
c     
c     Since the Higgs decay is always handled by the SMC
c     no resonance is present here. Higgs boson is a final state
c     particle. 
c      call gg_h_add_resonance(25,3,4)
      end


