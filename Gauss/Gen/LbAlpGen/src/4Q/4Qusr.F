c-------------------------------------------------------------------
      subroutine alshis
c-------------------------------------------------------------------
      include 'alplib/alpgen.inc'
      include '4Qlib/4Q.inc'
      ptbin=4e0
      ptmax=400e0
      xmbin=4e0
      xmmax=400e0
      call mbook(1,'pt_1',ptbin,0e0,ptmax)
      call mbook(2,'pt_2',ptbin,0e0,ptmax)
      if(ihvy.eq.5.and.ihvy2.eq.5) then 
         call mbook(8,'pt_b_min',2.*ptbin,0e0,2.*ptmax)
         call mbook(9,'dR_min(b-bbar)',0.2,0e0,6e0)
         call mbook(10,'mbb_min ',xmbin,0e0,xmmax)
      endif
      if(min(ihvy,ihvy2).eq.5.and.max(ihvy,ihvy2).eq.6) then 
         call mbook(8,'pt_b_min',2.*ptbin,0e0,2.*ptmax)
         call mbook(9,'dR(b-bbar)',0.2,0e0,6e0)
         call mbook(10,'mbb ',xmbin,0e0,xmmax)
      endif
      call mbook(13,'ptjet(incl)',ptbin,0e0,ptmax)
      end

      subroutine usrcut(lnot,weight)
c     pt,p,eta,dr,njets,weight
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                                c
c     Applies kinematical cuts to the final state during the phase
c     -space generation, in addition to the default ones
c                                                                c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      implicit none
      include 'alplib/alpgen.inc'
      include '4Qlib/4Q.inc'

c new options settable in python and communicated to fortran:
      real letamin, betamin, letamin1, betamin1, betamin2
      common /alpgenetacut/ letamin, betamin, 
     + letamin1, betamin1, betamin2
      real lptmin1, bptmin1, bptmin2
      common /alpgenptcut/ lptmin1, bptmin1, bptmin2

      integer lnot, i, countbeta1, countbeta2, countbpt1, countbpt2
      double precision weight

      weight= 1.d0
      lnot= 0 
      
c      write (6,*) 'you are in 4Qusr.F'
c      write (6,*) 'etab1 = ', etab(1)
c      write (6,*) 'etab2 = ', etab(2)
c      write (6,*) 'etab3 = ', etab(3)
c      write (6,*) 'etabb1 = ', etabb(1)
c      write (6,*) 'etabb1 = ', etabb(2)
c      write (6,*) 'etabb1 = ', etabb(3)
      
      countbeta1=0
      countbeta2=0
      countbpt1=0
      countbpt2=0
     
      do 30 i = 3, 6
c        write (6,*) 'eta', i, ' = ', eta(i)
c       write (6,*) 'pT(', i , ') = ', pt(i)
       if(eta(i).LT.betamin) goto 10

       if(eta(i).GE.betamin1) countbeta1=countbeta1+1
       if(eta(i).GE.betamin2) countbeta2=countbeta2+1
       if(pt(i).GE.bptmin1) countbpt1=countbpt1+1
       if(pt(i).GE.bptmin2) countbpt2=countbpt2+1
c      if(cuts not passed) goto 10
 30   continue

      if (countbeta1.lt.1) goto 10
      if (countbeta2.lt.2) goto 10
      if (countbpt1.lt.1) goto 10
      if (countbpt2.lt.2) goto 10


     

 20   return
 10   lnot= 1
      return
      end

c-------------------------------------------------------------------
      subroutine alfhis
c-------------------------------------------------------------------
      implicit none
      include 'alplib/alpgen.inc'
      include '4Qlib/4Q.inc'
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
      do i=1,200
         call mopera(i,'F',i,i,xnorm,1.)
         call mfinal(i)
      enddo 
      do i=1,njets
         call mtop(i,99,'pt'//jet(i),' ','LOG')
      enddo
      if(ihvy.eq.5.and.ihvy2.eq.5) then
         call mtop(8,99,'pt_b_min',' ','LOG')
         call mtop(9,99,'dR_min(b-bbar)',' ','LIN')
         call mtop(10,99,'mbb_min',' ','LOG')
      endif
      if(min(ihvy,ihvy2).eq.5.and.max(ihvy,ihvy2).eq.6) then 
         call mtop(8,99,'pt_b_min',' ','LOG')
         call mtop(9,99,'dR_(b-bbar)',' ','LIN')
         call mtop(10,99,'mbb',' ','LOG')
      endif
      call mtop(13,99,'pt(jet)',' ','LOG')
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
      include '4Qlib/4Q.inc'
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
      include '4Qlib/4Q.inc'
      real*8 wgt,minptb,minptbb,mbb,mQQ(6)
      real rwgt
      integer i,j,jproc,ord(10)
c
      return
      rwgt=real(wgt)
      if(rwgt.lt.0e0) then
         write(*,*) 'negative wgt=',wgt
         return
      elseif (rwgt.eq.0e0) then
         return
      endif
c
c     light jets
      if(njets.eq.1) call mfill(1,real(ptj(1)),rwgt)
      if(njets.eq.2) then 
         call mfill(1,real(max(ptj(1),ptj(2))),rwgt)
         call mfill(2,real(min(ptj(1),ptj(2))),rwgt)
      endif
c
      if(ihvy.eq.5.and.ihvy2.eq.5) then
c        pt_t
         minptb= min(ptb(1),ptb(2))
         minptbb= min(ptbb(1),ptbb(2))
         call mfill(8,real(min(minptb,minptbb)),rwgt)
c        dR_bb
         call alusor(drbb,6,ord,2)
         call mfill(9,real(drbb(ord(1))),rwgt)
c        m_bb
         do i=1,2
            mQQ(i)=(pbott(4,i)+pbbar(4,i))**2
            do j=1,3
               mQQ(i)=mQQ(i)-(pbott(j,i)+pbbar(j,i))**2
            enddo
            if(mQQ(i).le.0) then
               write(*,*) 'mQQ^2=',mQQ,' <0'
               stop
            endif
            mQQ(i)=sqrt(mQQ(i))
         enddo
         mQQ(3)=(pbott(4,1)+pbott(4,2))**2
         do j=1,3
            mQQ(3)=mQQ(3)-(pbott(j,1)+pbott(j,2))**2
         enddo
         if(mQQ(3).le.0) then
            write(*,*) 'mQQ^2=',mQQ,' <0'
            stop
         endif
         mQQ(3)=sqrt(mQQ(3))
         mQQ(4)=(pbbar(4,1)+pbbar(4,2))**2
         do j=1,3
            mQQ(4)=mQQ(4)-(pbbar(j,1)+pbbar(j,2))**2
         enddo
         if(mQQ(4).le.0) then
            write(*,*) 'mQQ^2=',mQQ,' <0'
            stop
         endif
         mQQ(4)=sqrt(mQQ(4))
         mQQ(5)=(pbott(4,1)+pbbar(4,2))**2
         do j=1,3
            mQQ(5)=mQQ(5)-(pbott(j,1)+pbbar(j,2))**2
         enddo
         if(mQQ(5).le.0) then
            write(*,*) 'mQQ^2=',mQQ,' <0'
            stop
         endif
         mQQ(5)=sqrt(mQQ(5))
         mQQ(6)=(pbott(4,2)+pbbar(4,1))**2
         do j=1,3
            mQQ(6)=mQQ(6)-(pbott(j,2)+pbbar(j,1))**2
         enddo
         if(mQQ(6).le.0) then
            write(*,*) 'mQQ^2=',mQQ,' <0'
            stop
         endif
         mQQ(6)=sqrt(mQQ(6))
*
         call alusor(mQQ,6,ord,2)
         call mfill(10,real(mQQ(ord(1))),rwgt)
      endif
*
      if(min(ihvy,ihvy2).eq.5.and.max(ihvy,ihvy2).eq.6) then
c        pt_b
         call mfill(8,real(min(ptb(1),ptbb(1))),rwgt)
c        dR_bb
         call mfill(9,real(drbb(1)),rwgt)
c        m_bb
         mbb=(pbott(4,1)+pbbar(4,1))**2
         do j=1,3
            mbb=mbb-(pbott(j,1)+pbbar(j,1))**2
         enddo
         if(mbb.le.0)then
            write(*,*) 'mbb^2=',mbb,' <0'
            stop
         endif
         mbb=sqrt(mbb)
         call mfill(10,real(mbb),rwgt)
      endif

      end

