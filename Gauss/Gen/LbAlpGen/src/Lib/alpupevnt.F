C $Id: alpupevnt.F,v 1.1.1.1 2007-07-02 09:25:04 ibelyaev Exp $
C----------------------------------------------------------------------
      SUBROUTINE ALPGEN_UPEVNT
C----------------------------------------------------------------------
c     Puts Alpgen event into GUPI common block HEPEU
c----------------------------------------------------------------------
      implicit none
      INCLUDE 'alpsho.inc'
C--   GUP Event common block
      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,
     &     IDUP(MAXNUP),ISTUP(MAXNUP),MOTHUP(2,MAXNUP),
     &     ICOLUP(2,MAXNUP),PUP(5,MAXNUP),VTIMUP(MAXNUP),
     &     SPINUP(MAXNUP)
C--   GUP Run common block
      INTEGER MAXPUP
      PARAMETER(MAXPUP=100)
      INTEGER IDBMUP,PDFGUP,PDFSUP,IDWTUP,NPRUP,LPRUP
      DOUBLE PRECISION EBMUP,XSECUP,XERRUP,XMAXUP
      COMMON /HEPRUP/ IDBMUP(2),EBMUP(2),PDFGUP(2),PDFSUP(2),
     &     IDWTUP,NPRUP,XSECUP(MAXPUP),XERRUP(MAXPUP),
     &     XMAXUP(MAXPUP),LPRUP(MAXPUP)
c     
c     local variables
      INTEGER INIT
      DATA INIT/0/
      CHARACTER *3 CSHO
      INTEGER MAXPAR
      PARAMETER (MAXPAR=100)
      INTEGER NEV,IPROC,IFL(MAXPAR)
      REAL SQ,SP(3,MAXPAR),SM(MAXPAR),SWGTRES
      INTEGER I,IUP,IWCH,IST
      REAL *8 TMP,WGTRES
C     LOCAL VARIABLES FOR TOP DECAYS
      INTEGER IT,ITB,IW,IWDEC,IBUP,IWUP
C     LOCAL VARIABLES TOP HIGGS DECAYS
      INTEGER IH
C     LOCAL VARIABLES FOR GAUGE BOSON  DECAYS
      INTEGER IVSTART,IVEND,NVB
C     
C* =====================================================================
C* I.B. 2007-06-28
C* =====================================================================
C* Pythia:
C* =====================================================================
C...Parameters. 
      INTEGER MSTP,MSTI
      DOUBLE PRECISION PARP,PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
C* =====================================================================
C* Local:
C* =====================================================================
      LOGICAL YES 
C* =====================================================================

C* =====================================================================
C* I.B. 2007-06-28
C* =====================================================================
      IF ( 0 .GE. MSTP(162) ) then      
        WRITE(*,*) 'ALPGEN_UPEVNT: invalid MSPT(162)=',
     *    MSTP(162),' unit'
        STOP       'ALPGEN_UPEVNT: invalid MSPT(162) unit'
      ELSE
        YES=.FALSE.
        INQUIRE(UNIT=MSTP(162),OPENED=YES)
        IF ( .NOT. YES ) THEN
          WRITE(*,*) 'ALPGEN_UPEVNT: MSPT(162)=',
     *      MSTP(162),' unit not opened'
          STOP       'ALPGEN_UPEVNT: MSPT(162) unit not opened'
        ENDIF
      ENDIF
      NUNIT=MSTP(162)           ! define unit 
      MSTI(51)=0
C* =====================================================================
C* =====================================================================
      
      
C UPDATE MAXIMUM NMBER OF ALLOWED ERRORS
      CALL ALSHER(I)
C
      IST=0
C     INPUT EVENT NUMBER, PROCESS TYPE, N PARTONS, SAMPLE'S AVERAGE
C     WEIGHT AND QSCALE
      READ(NUNIT,2,END=500,ERR=501) NEV,IPROC,NPART,SWGTRES,SQ
 2    FORMAT(I8,1X,I4,1X,I2,2(1X,E12.6))
C     FLAVOUR, COLOUR AND Z-MOMENTUM OF INCOMING PARTONS
      READ(NUNIT,8) IFL(1),ICOLUP(1,1),ICOLUP(2,1),SP(3,1)
      READ(NUNIT,8) IFL(2),ICOLUP(1,2),ICOLUP(2,2),SP(3,2)
C     FLAVOUR, COLOUR, 3-MOMENTUM AND MASS OF OUTGOING PARTONS
      DO I=3,NPART
        READ(NUNIT,9) IFL(I),ICOLUP(1,I),ICOLUP(2,I),SP(1,I),SP(2,I)
     $       ,SP(3,I),SM(I)
      ENDDO
 8    FORMAT(I8,1X,2(I4,1X),F10.3)
 9    FORMAT(I8,1X,2(I4,1X),4(1X,F10.3))
C     
C     START PROCESSING INPUT DATA
C     
C     SCALES AND WEIGHTS
      SCALUP=DBLE(SQ)
      IF(IDWTUP.EQ.3) THEN
	XWGTUP=DBLE(SWGTRES) !AVGWGT
      ELSE
        WRITE(*,*) 'ONLY UNWEIGHTED EVENTS ACCEPTED AS INPUT, STOP'
        STOP
      ENDIF
c     
c---  incoming lines
      do 100 i=1,2
        iup=i
        idup(iup)=ifl(i)
        istup(iup)=-1
        mothup(1,iup)=0
        mothup(2,iup)=0
        pup(1,iup)=0.
        pup(2,iup)=0.
        pup(3,iup)=dble(Sp(3,iup))
        pup(4,iup)=abs(pup(3,iup))
        pup(5,iup)=0d0
 100  continue
c---  outgoing lines
      do 110 i=3,npart
        iup=i
        idup(iup)=ifl(i)
        istup(iup)=1
        mothup(1,iup)=1
        mothup(2,iup)=2
        pup(1,iup)=dble(Sp(1,i))
        pup(2,iup)=dble(Sp(2,i))
        pup(3,iup)=dble(Sp(3,i))
        pup(5,iup)=dble(Sm(i))
        tmp=(pup(5,iup)**2+pup(1,iup)**2+pup(2,iup)**2+pup(3,iup)**2)
        pup(4,iup)=sqrt(tmp)
 110  continue
c     
      nup=npart
c---  set up colour structure labels
      Do iup=1,nup
        if(icolup(1,iup).ne.0) icolup(1,iup)=icolup(1,iup)+500
        if(icolup(2,iup).ne.0) icolup(2,iup)=icolup(2,iup)+500
      Enddo
c     
c     
c     and now consider assignements specific to individual hard
C     processes
c     
c---  W/Z/gamma b bbar + jets, or W/Z + jets
      if (ihrd.le.4.or.ihrd.eq.10.OR.IHRD.EQ.14.OR.IHRD.EQ.15) then
        iwch=0
        do iup=nup-1,nup
          mothup(1,iup)=nup+1
          mothup(2,iup)=0
          if(ihrd.ne.2) iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) => -(1)+0 = -1  => W-
c     positron+nu    -> -11+ 12    => -(-1)+0 = -1 => W+
c     u dbar -> 2 -1  => 0 -(-1) = 1 => W+
c     c dbar -> 4 -1  => W+
c etc.
        enddo
        iup=nup+1
        If (iwch.gt.0) then
          idup(iup)=24
        Elseif (iwch.lt.0) then
          idup(iup)=-24
        Else
          idup(iup)=23
        Endif
        istup(iup)=2
        mothup(1,iup)=1
        mothup(2,iup)=2
        tmp=pup(4,iup-2)+pup(4,iup-1)
        pup(4,iup)=tmp
        tmp=tmp**2
        do i=1,3
          pup(i,iup)=pup(i,iup-2)+pup(i,iup-1)
          tmp=tmp-pup(i,iup)**2
        enddo
        pup(5,iup)=sqrt(tmp)
        nup=nup+1
        icolup(1,nup)=0
        icolup(2,nup)=0
c---  nW + mZ + kH + jets
      elseif (ihrd.eq.5) then
c     find first gauge bosons
        ivstart=0
        ivend=0
        do i=1,npart
          if(abs(idup(i)).eq.24.or.idup(i).eq.23) then
            istup(i)=2
            if(ivstart.eq.0) ivstart=i
            ivend=i+1
          endif
        enddo
        nvb=ivend-ivstart
c     decay products pointers, starting from the end
        do i=1,nvb
          mothup(1,npart-2*i+2)=ivend-i
          mothup(1,npart-2*i+1)=ivend-i
          mothup(2,npart-2*i+2)=0
          mothup(2,npart-2*i+1)=0
        enddo
c---  t tbar + jets
c     t tb jets f fbar f fbar W+ b W- bbar
      elseif (ihrd.eq.6.and.abs(ifl(3)).eq.6) then
c     reset top status codes
        istup(3)=2
        istup(4)=2
        if(ifl(3).eq.6) then
          it=3
          itb=4
        else
          it=4
          itb=3
        endif
c     reconstruct W's from decay products
        do iw=1,2
          iwdec=nup-5+2*iw
          iwup=nup+iw
          ibup=iwup+2
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          If (iwch.gt.0) then
            idup(iwup)=24
            idup(ibup)=5
            mothup(1,iwup)=it
            mothup(2,iwup)=0
            mothup(1,ibup)=it
            mothup(2,ibup)=0
          Elseif (iwch.lt.0) then
            idup(iwup)=-24
            idup(ibup)=-5
            mothup(1,iwup)=itb
            mothup(2,iwup)=0
            mothup(1,ibup)=itb
            mothup(2,ibup)=0
          Endif
          istup(iwup)=2
          istup(ibup)=1
c     reconstruct W momentum
          tmp=pup(4,iwdec)+pup(4,iwdec+1)
          pup(4,iwup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
            tmp=tmp-pup(i,iwup)**2
          enddo
          pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
          tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
          pup(4,ibup)=tmp 
          tmp=tmp**2
          do i=1,3
            pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
            tmp=tmp-pup(i,ibup)**2
          enddo
          pup(5,ibup)=sqrt(tmp)
          icolup(1,iwup)=0
          icolup(2,iwup)=0
          icolup(1,ibup)=icolup(1,mothup(1,iwup))
          icolup(2,ibup)=icolup(2,mothup(1,iwup))
        enddo
c     stop
        nup=nup+4
c---  H t tbar + jets
c     H t tb jets f fbar f fbar W+ b W- bbar
      elseif (ihrd.eq.8.and.abs(ifl(4)).eq.6) then
c     reset top status codes
        istup(4)=2
        istup(5)=2
        if(ifl(4).eq.6) then
          it=4
          itb=5
        else
          it=5
          itb=4
        endif
c     reconstruct W's from decay products
        do iw=1,2
          iwdec=nup-5+2*iw
          iwup=nup+iw
          ibup=iwup+2
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          If (iwch.gt.0) then
            idup(iwup)=24
            idup(ibup)=5
            mothup(1,iwup)=it
            mothup(2,iwup)=0
            mothup(1,ibup)=it
            mothup(2,ibup)=0
          elseif (iwch.lt.0) then
            idup(iwup)=-24
            idup(ibup)=-5
            mothup(1,iwup)=itb
            mothup(2,iwup)=0
            mothup(1,ibup)=itb
            mothup(2,ibup)=0
          endif
          istup(iwup)=2
          istup(ibup)=1
c     reconstruct W momentum
          tmp=pup(4,iwdec)+pup(4,iwdec+1)
          pup(4,iwup)=tmp
          tmp=tmp**2
          do i=1,3
            pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
            tmp=tmp-pup(i,iwup)**2
          enddo
          pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
          tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
          pup(4,ibup)=tmp 
          tmp=tmp**2
          do i=1,3
            pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
            tmp=tmp-pup(i,ibup)**2
          enddo
          pup(5,ibup)=sqrt(tmp)
          icolup(1,iwup)=0
          icolup(2,iwup)=0
          icolup(1,ibup)=icolup(1,mothup(1,iwup))
          icolup(2,ibup)=icolup(2,mothup(1,iwup))
        enddo
c     stop
        nup=nup+4
c---  SINGLE TOP
c     Input: T  
c     output: jets t b w f fbar t b w f fbar 
      elseif (ihrd.eq.13) then
        nw=1
        if(itopprc.ge.3) nw=2
c     assign mass to the incoming bottom quark, if required
        DO I=1,2
          IF(ABS(IFL(I)).EQ.5) THEN
            IUP=I
            PUP(5,IUP)=mb
            PUP(4,IUP)=SQRT(PUP(3,IUP)**2+PUP(5,IUP)**2)
          ENDIF
        ENDDO
        istup(3)=2
        it=0
        itb=0
        if(ifl(3).eq.6) then
          it=3
        elseif(ifl(3).eq.-6) then
          itb=3
        else
          write(*,*) 'wrong assumption about top position, stop'
          stop
        endif
c
c     TOP DECAY
c     reconstruct W's from decay products
c
c iwdec: 1st W decay product.
        if(nw.eq.1) then
          iwdec=nup-1
        elseif(nw.eq.2) then
          iwdec=nup-3
        endif
c put W and b  at the end
        iwup=nup+1
        ibup=iwup+1
c
        iwch=0
        do iup=iwdec,iwdec+1
          mothup(1,iup)=iwup
          mothup(2,iup)=0
          iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
        enddo
        If (iwch.gt.0) then
          idup(iwup)=24
          idup(ibup)=5
          mothup(1,iwup)=it
          mothup(2,iwup)=0
          mothup(1,ibup)=it
          mothup(2,ibup)=0
        Elseif (iwch.lt.0) then
          idup(iwup)=-24
          idup(ibup)=-5
          mothup(1,iwup)=itb
          mothup(2,iwup)=0
          mothup(1,ibup)=itb
          mothup(2,ibup)=0
        Endif
        istup(iwup)=2
        istup(ibup)=1
c     reconstruct W momentum
        tmp=pup(4,iwdec)+pup(4,iwdec+1)
        pup(4,iwup)=tmp
        tmp=tmp**2
        do i=1,3
          pup(i,iwup)=pup(i,iwdec)+pup(i,iwdec+1)
          tmp=tmp-pup(i,iwup)**2
        enddo
        pup(5,iwup)=sqrt(tmp)
c     reconstruct b momentum
        tmp=pup(4,mothup(1,iwup))-pup(4,iwup)
        pup(4,ibup)=tmp 
        tmp=tmp**2
        do i=1,3
          pup(i,ibup)=pup(i,mothup(1,iwup))-pup(i,iwup)
          tmp=tmp-pup(i,ibup)**2
        enddo
c        write(*,*) (pup(i,ibup),i=1,4),sqrt((tmp))
        pup(5,ibup)=sqrt(tmp)
        icolup(1,iwup)=0
        icolup(2,iwup)=0
        icolup(1,ibup)=icolup(1,mothup(1,iwup))
        icolup(2,ibup)=icolup(2,mothup(1,iwup))
c     
        nup=nup+2
        if(nw.eq.2) then
c
c     W DECAY
c
c iwdec: 1st W decay product. 
          iwdec=nup-3
c iwup: location of the W in the event record
          iwup=nup-6
          iwch=0
          do iup=iwdec,iwdec+1
            mothup(1,iup)=iwup
            mothup(2,iup)=0
            iwch=iwch-mod(idup(iup),2)
c     electron+nubar -> 11 + (-12) = -1 => W-
c     d + ubar -> 1 + (-2) = -1 => W-
c     positron+nu    -> -11+ 12    =  1 => W+
c     u + dbar -> 2 + (-1) = 1 => W+
          enddo
          istup(iwup)=2
          icolup(1,iwup)=0
          icolup(2,iwup)=0
        endif
      endif
c     herwig debugging:
c      call HWUPUP
      return
c     
c     
c     end of file
 500  ist=1
c     error reading file
 501  if(ist.eq.0) ist=2
C     RESET CROSS-SECTION INFORMATION FOR END OF RUN AND FINALIZE
      IF(IST.GT.0) THEN
        MSTI(51)=1              !  I.B. 2007-06-29
        CALL ALSFIN
      ENDIF
C* ===================================================================
C* I.B. 2007-06-28
C* ===================================================================
C$$      close(Nunit)
C$$      close(NunitOut)
C* ===================================================================
      END

