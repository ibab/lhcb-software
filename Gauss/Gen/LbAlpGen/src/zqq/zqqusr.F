c-----------------------------------------------------------------
      subroutine usrcut(lnot,wusr)
c-----------------------------------------------------------------
c PRIMARY CUTS ALREADY APPLIED TO PHASE-SPACE GENERATION:
c     ptjmin < pt(jet) < ptjmax for all light jets
c     -etajmax < eta(jet) < etajmax for all light jets
c     delta R(jj) > drjmin for all (light jet, light jet) pairs
c     delta R(bj) > drjmin for all (heavy quark, light jet) pairs
c     ptbmin < pt(b) < ptbmax for all heavy quarks
c     -etabmax < eta(b) < etabmax for all heavy quarks 
c     delta R(bb) > drjmin for all (heavy quark, heavy quark) pairs
c     mllmin < m(l+l-) < mllmax for charged leptons
c     etmiss > etmiss_min for neutrinos
c
c USE THIS ROUTINE TO ENFORCE OTHER CUTS    
c
      implicit none
      include 'alplib/alpgen.inc'
      include 'zqqlib/zqq.inc'
      double precision wusr
      integer lnot,i
c bb-jet declarations
      integer ib,ibb
c initialize output parameters
      lnot=0
      wusr=1d0
c
      return
c     the following cuts allow to select events where just 1 b-jet is
c     reconstructed. To activate remove previous "return" statement
c
c     check whether both b and bbar reconstruct individual jets
      if(drbb.gt.drjmin.and.min(ptb,ptbb).gt.ptjmin.and.max(abs(etab)
     $     ,abs(etabb)).lt.etajmax) goto 5
c
c     either one or both b's don't reconstruct a jet.
c     Study then the b-bbar system, using partons inside acceptance
      ib=0d0
      ibb=0d0
      if(abs(etab).lt.etajmax) ib=1d0
      if(abs(etabb).lt.etajmax) ibb=1d0
      do i=1,3
         pbjet(i)=ib*pbott(i)+ibb*pbbar(i)
      enddo
c     if parton outside acceptance, set pt to 0
      ptb=ib*ptb
      ptbb=ibb*ptbb
      ptbjet=sqrt(pbjet(1)**2+pbjet(2)**2)
      if(ptbjet.gt.0) then
         etabjet=-log(tan(0.5d0*atan2(ptbjet,pbjet(3))))
      else 
         etabjet=1d4
      endif
c     if both b and bbar below the jet threshold:
      if(max(ptb,ptbb).lt.ptjmin) then
c     1. they don't lie within a potential jet cone, then event has only
c     njet-2 jets => reject
         if(drbb.gt.drjmin) goto 10
c     2. they lie within a potential jet cone, check if reconstruct a
c     jet
         if(ptbjet.lt.ptjmin.or.abs(etabjet).gt.etajmax) goto 10
c     either b or  bbar below the jet threshold:
      elseif(min(ptb,ptbb).lt.ptjmin) then
         if(drbb.lt.drjmin) then
c     if within jet cone, check that jet passes cuts:
            if(ptbjet.lt.ptjmin.or.abs(etabjet).gt.etajmax) goto 10
         else
c     if outside jet cone, only stiff one can define the jet:
            if(ptb.gt.ptjmin) then
               ptbjet=ptb
               etabjet=etab
            elseif(ptbb.gt.ptjmin) then
               ptbjet=ptbb
               etabjet=etabb
            endif
            if(ptbjet.lt.ptjmin.or.abs(etabjet).gt.etajmax) goto 10
         endif
      endif
c     
 5    return
 10   lnot= 1
      return
      end



c-------------------------------------------------------------------
      subroutine alshis
c-------------------------------------------------------------------
      implicit none
      include 'alplib/alpgen.inc'
      include 'zqqlib/zqq.inc'
      real ptbin,ptmax,xmbin,xmmax
      ptbin=2e0
      ptmax=200e0
      xmbin=4e0
      xmmax=400e0
c
      call mbook(1,'pt_1',2.*ptbin,0e0,2.*ptmax)
      call mbook(2,'pt_2',ptbin,0e0,ptmax)
      call mbook(3,'pt_3',ptbin,0e0,ptmax)
      call mbook(4,'pt_4',ptbin,0e0,ptmax)
      call mbook(5,'pt_5',ptbin,0e0,ptmax)
      call mbook(6,'pt_6',ptbin,0e0,ptmax)
      if(ilep.eq.0) call mbook(7,'pt_min(lept)',ptbin,0e0,ptmax)
      if(ilep.eq.1) call mbook(7,'etmiss',ptbin,0e0,ptmax)
      call mbook(8,'pt_b',ptbin,0e0,ptmax)
      call mbook(9,'dR(b-bbar)',0.2,0e0,6e0)
      call mbook(10,'mbb ',xmbin,0e0,xmmax)
      call mbook(11,'dR(b-jetmax)',0.2,0e0,6e0)
      call mbook(12,'dR(b-jetmin)',0.2,0e0,6e0)
      call mbook(13,'ptjet(incl)',ptbin,0e0,ptmax)
c
      if(ilep.eq.0) call mbook(15,'m(ll)',2e0,0e0,200e0)
      end

c-------------------------------------------------------------------
      subroutine alfhis
c-------------------------------------------------------------------
      implicit none
      include 'alplib/alpgen.inc'
      include 'zqqlib/zqq.inc'
      integer i
      real  xnorm
      character *1 jet(9)
      data jet/'1','2','3','4','5','6','7','8','9'/
c
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
         if(i.ne.61) call mopera(i,'F',i,i,xnorm,1.)
         call mfinal(i)
      enddo 
c
      do i=1,njets
         call mtop(i,99,'pt'//jet(i),' ','LOG')
      enddo
      if(ilep.eq.0) call mtop(7,99,'pt_min(lept)',' ','LOG')
      if(ilep.eq.1) call mtop(7,99,'etmiss',' ','LOG')
      call mtop(8,99,'pt(b,bbar)',' ','LOG')
      call mtop(9,99,'dR(b-bbar)',' ','LIN')
      call mtop(10,99,'m(b-bbar)',' ','LOG')
      call mtop(11,99,'dR(b-jetmax)',' ','LIN')
      call mtop(12,99,'dR(b-jetmin)',' ','LIN')
      call mtop(13,99,'pt(jet)',' ','LOG')
      if(ilep.eq.0) call mtop(15,99,'m(ll)',' ','LOG')
c
 100  close(99)
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
      include 'zqqlib/zqq.inc'
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
      include 'zqqlib/zqq.inc'
      real*8 mbb,mll,wgt,tmp,ptlep,etmiss
      real rwgt
      integer i,j,jproc,ord(10)
c
      rwgt=real(wgt)
      if(rwgt.lt.0e0) then
         write(*,*) 'negative wgt=',wgt
         return
      elseif (rwgt.eq.0e0) then
         return
      endif 
c
c     reordering according to pt
      if(njets.ge.2) then
        call alusor(ptj,njets,ord,2)              
        do i=1,njets
          call mfill(i,real(ptj(ord(njets+1-i))),rwgt)
          call mfill(13,real(ptj(ord(njets+1-i))),rwgt)
        enddo
        if(ilep.eq.0) then
          ptlep=min(ptlm,ptlp)
          call mfill(7,real(ptlep),rwgt)
        elseif(ilep.eq.1) then
          etmiss=sqrt((pnu(1)+pnub(1))**2+(pnu(2)+pnub(2))**2)
          call mfill(7,real(etmiss),rwgt)
        endif
        call mfill(8,real(ptb),rwgt)
        call mfill(8,real(ptbb),rwgt)
        
        call mfill(9,real(drbb),rwgt)
c     dR(b-jetmax)
        tmp=drbj(ord(njets))
        if(tmp.eq.0) tmp=drbj(ord(njets-1))
        call mfill(11,real(tmp),rwgt)
c     dR(bbar-jetmax)
        tmp=drbbj(ord(njets))
        if(tmp.eq.0) tmp=drbbj(ord(njets-1))
        call mfill(11,real(tmp),rwgt)
c     dR(b-jetmin)
        tmp=drbj(ord(1))
        if(tmp.eq.0) tmp=drbj(ord(2))
        call mfill(12,real(tmp),rwgt)
c     dR(bbar-jetmin)
        tmp=drbbj(ord(1))
        if(tmp.eq.0) tmp=drbbj(ord(2))
        call mfill(12,real(tmp),rwgt)
c     b-bbar mass
        mbb=(pbott(4)+pbbar(4))**2
        do i=1,3
          mbb=mbb-(pbott(i)+pbbar(i))**2
        enddo
        mbb=sqrt(mbb)
        call mfill(10,real(mbb),rwgt)
c     dilepton mass
        if(ilep.eq.0) then
          mll=(plp(4)+plm(4))**2
          do i=1,3
            mll=mll-(plp(i)+plm(i))**2
          enddo
          mll=sqrt(mll)
          call mfill(15,real(mll),rwgt)
        endif
c     
      endif
      end

