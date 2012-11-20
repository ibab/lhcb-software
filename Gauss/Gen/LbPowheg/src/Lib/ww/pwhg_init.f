      subroutine ww_pwhginit
      implicit none
      include 'src/Lib/ww/LesHouches.h'
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      include 'src/Lib/ww/pwhg_kn.h'
      include 'src/Lib/ww/pwhg_pdf.h'
      include 'src/Lib/ww/pwhg_rad.h'
      include 'src/Lib/ww/pwhg_flg.h'
      real * 8 ww_powheginput
      external ww_powheginput
      integer i1,n1,n2
      call ww_init_flsttag
      flg_debug=.false.
      if(ww_powheginput("#flg_debug").eq.1) flg_debug=.true.
c whether to output negative weights or not
      flg_withnegweights=.false.
      if(ww_powheginput("#withnegweights").eq.1)flg_withnegweights=.true.
c See if we have weighted events
      flg_weightedev=.false.
      if(ww_powheginput("#bornsuppfact").gt.0) flg_weightedev=.true.
      if(ww_powheginput("#ptsupp").gt.0) then
         write(*,*) ' ******** WARNING: ptsupp is deprecated'
         write(*,*) ' ******** Replace it with bornsuppfact'
         call flush
         call exit(-1)
      endif
c     Set to true to remember and use identical values of the computed 
c     amplitudes, for Born, real and virtual contributions
      flg_smartsig=.true.
      if(ww_powheginput("#smartsig").eq.0) flg_smartsig=.false.
c     if true also counterterm are output in NLO tests (default)
      flg_withsubtr=.true.
      if(ww_powheginput("#withsubtr").eq.0) flg_withsubtr=.false.
c     if true use Born zero damping factor and include remnants
c     set flg_withdamp to true also if the damping of real radiation
c     is required
      flg_withdamp=.false.
      if((ww_powheginput("#withdamp").eq.1).or.
     $  (ww_powheginput("#hfact").gt.0d0))   flg_withdamp=.true.
c     If set do only the Born term
      flg_bornonly=.false.
      if (ww_powheginput("#bornonly").eq.1) flg_bornonly=.true.
c     initialize ww_random number sequence
      i1=ww_powheginput('#iseed')
      if (i1.lt.0) i1=0
      n1=ww_powheginput('#rand1')
      if (n1.lt.0) n1=0
      n2=ww_powheginput('#rand2')
      if (n2.lt.0) n2=0
c select which upper bounding function form
      rad_iupperisr=ww_powheginput("#iupperisr")
      if(rad_iupperisr.lt.0) rad_iupperisr=1
      rad_iupperfsr=ww_powheginput("#iupperfsr")
      if(rad_iupperfsr.lt.0) rad_iupperfsr=2
c info on pdf for each event
      flg_pdfreweight=.false.
      if(ww_powheginput("#pdfreweight").eq.1)flg_pdfreweight=.true.
c
      call ww_setrandom(i1,n1,n2)
c     assign a default id for the process at hand
c     if the user want to assign different id's
c     to each subprocess, he/she can reassign lprup(1)
c     inside the user-defined subroutine ww_init_processes
      lprup(1)=10001
c     initialize physical parameters
      call ww_init_phys
c ID of beam particles 1 and 2 (pdg convention)
c proton:
      ebmup(1)=kn_beams(0,1)
      ebmup(2)=kn_beams(0,2)
      idbmup(1) = 2212*pdf_ih1
      idbmup(2) = 2212*pdf_ih2
c pdf group; negative to use internal herwig pdf's for showering
      pdfgup(1)=-1
      pdfgup(2)=-1
c pdf set
      pdfsup(1)=-1
      pdfsup(2)=-1
c If either weighted events or event with negative weights are
c required, use idwtup=-4. Thuse, in both these cases, the average of the
c event weight is the total cross section.
c Otherwise the weight is set to 1 for each event.
c User processes may override these choices. In particular, using -4 in
c all cases is recommended for new processes. Using 3 is left here for
c compatibility with older implementations.
      if(flg_withnegweights.or.flg_weightedev) then
         idwtup = -4
      else
         idwtup = 3
      endif
c number of user subprocesses
c Irrelevant if idwtup=+-3,+-4
      nprup = 1
      call ww_bbinit
c now the cross section is available
      if(flg_weightedev) then
         xsecup(1)=-1
         xerrup(1)=-1
      else
         xsecup(1)=rad_tot  *rad_branching
         xerrup(1)=rad_etot *rad_branching
      endif
      xmaxup(1)=1
      end

      subroutine ww_init_flsttag
      include 'src/Lib/ww/nlegborn.h'
      include 'src/Lib/ww/pwhg_flst.h'
      integer j,l
      do j=1,maxprocreal
         do l=1,nlegreal
            flst_realtags(l,j)=0
         enddo
      enddo
      do j=1,maxprocborn
         do l=1,nlegborn
            flst_borntags(l,j)=0
         enddo
      enddo
      end
