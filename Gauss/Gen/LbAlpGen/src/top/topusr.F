c-------------------------------------------------------------------
      subroutine alshis
c-------------------------------------------------------------------
      include 'alplib/alpgen.inc'
      include 'toplib/top.inc'
      end

      subroutine usrcut(lnot,weight)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                                c
c     Applies kinematical cuts to the final state during the phase
c     -space generation                                          c
c                                                                c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      implicit none
      include 'alplib/alpgen.inc'
      include 'toplib/top.inc'

      integer lnot
      double precision cutkin(10)
      common/loccut/cutkin
      double precision weight
      real*8 xptb,xptbb,xetb,xetbb,xetmu,xete,xptmu,xpte,xetab,xetabb,
     >       xetamu,xetae,xetmiss,xetsum
      real*8 xpb(4),xpnm(4),xpmu(4),xpbb(4),xpe(4),xpne(4)
      integer i
*
      weight= 1.d0
      lnot= 0
c
c     if(cut-not-passed) goto 10
c


 20   return
 10   lnot= 1
      return
      end

c-------------------------------------------------------------------
      subroutine alfhis
c-------------------------------------------------------------------
      implicit none
      include 'alplib/alpgen.inc'
      include 'toplib/top.inc'
c debug
      double precision rate(16)
      common/dbg/rate
c
      integer i
      real  xnorm
      character *1 jet(9)
      data jet/'1','2','3','4','5','6','7','8','9'/
      open(unit=99,file=topfile,err=101,status='unknown')
      if(imode.le.1) then
         xnorm=sngl(avgwgt/totwgt)
      elseif(imode.eq.2) then
         xnorm=1e0/real(unwev)
      else
         write(*,*) 'imode type not allowed, stop'
         stop
      endif
c
      do i=1,200
         call mopera(i,'F',i,i,xnorm,1.)
         call mfinal(i)
      enddo 
c
      close(99)
 101  return
      end

      subroutine monitor(n,mon_fname)
c     This routine is called by default every 100K events.
c     The user can use it to get regular updates on the run
c     while this is progressing. Textual output can be written to file
c     fname, where partial cross-sections and and generation
c     efficiencies have already been printed by default
      implicit none
      include 'alplib/alpgen.inc'
      include 'toplib/top.inc'
      integer n
      character *50 mon_fname
c
      if(evgen) then
         if(mod(n,100000).eq.0) then
c     save histograms' contents
            call msave
c     print out histograms
            call alfhis
c     restore original contents, to proceed with analysis
            call mrestore
         endif 
      endif 
      end

c-------------------------------------------------------------------
      subroutine aleana(jproc,wgt)
c     analyse event, fill histograms
c-------------------------------------------------------------------
      implicit none
      include 'alplib/alpgen.inc'
      include 'toplib/top.inc'
      real*8 wgt
      real rwgt
      integer i,jproc,ord(10)
c
*
      integer nmax        !maximum number of external particles, change here
      parameter (nmax=10)          

      real*8 mbb,mnn,mmt,gi(4),ptrb,ptrbbar,ptt,ptm,ptnt,ptnm
      real*8 ctnn,ctbb,ctmt,ctbts,ctnts,ctmts
      real*8 qboost(4),pbstar(4),qtop(4),pmustar(4),pnustar(4)
      data gi/1.d0,-1.d0,-1.d0,-1.d0/

*
      rwgt=real(wgt)

      if(rwgt.lt.0e0) then
         write(*,*) 'negative wgt=',wgt
         return
      elseif (rwgt.eq.0e0) then
         return
      endif
*
      end


