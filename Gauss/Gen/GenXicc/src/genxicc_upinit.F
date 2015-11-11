
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c upinit----initial parameters for pythia6.409                       c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c copyright (c)        z.x zhang and x.g. wu                         c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c      subroutine genxicc_upinit
      subroutine genxicc_upinit(IISET) ! F. Zhang 25-04-2011
 
c...double precision and integer declarations.
      implicit double precision(a-h, o-z)
      implicit integer(i-n)
#include "genxicc_constants.h"
      integer pdfgup,pdfsup,lprup
      common/heprup/idbmup(2),ebmup(2),pdfgup(2),pdfsup(2),
     &idwtup,nprup,xsecup(maxpup),xerrup(maxpup),xmaxup(maxpup),
     &lprup(maxpup)
      save /heprup/
 
c...user process event common block.
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &vtimup(maxnup),spinup(maxnup)
      save /hepeup/

c...user process event common block. 
      common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)

c-----------------------------

c...extra commonblock to transfer run info.
	common/ptpass/ptmin,ptmax,crossmax,etamin,etamax,
     &	smin,smax,ymin,ymax,psetamin,psetamax
	common/histcol/inx

c...the user own transfer of information.
      double complex colmat,bundamp
      common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/counter/ixiccstate,nev,xmaxwgt
	logical generate
	common/genefull/generate
	common/vegcross/vegsec,vegerr,iveggrade

c...parameters transformtion.
      common/funtrans/npdfu
	common/usertran/idpp
c...transform some variables
      common/loggrade/ievntdis,igenerate,ivegasopen,igrade,iusecurdir

c...set up incoming beams. tevotran or selex
      IF(IISET.EQ.1) THEN ! F. ZHANG 25-04-2011
      if(npdfu.eq.1 .or. npdfu.eq.3) then
	   idbmup(1) = 2212
         idbmup(2) = -2212
	end if

c...set up incoming beams. lhc
	if(npdfu.eq.2) then
	   idbmup(1) = 2212
	   idbmup(2) = 2212
	end if

      ebmup(1)  = 0.5d0*ecm
      ebmup(2)  = 0.5d0*ecm
      GOTO 999 ! F. ZHANG 25-04-2011
      ENDIF

c...switch off aspects: initial and final state
c...showers, multiple interactions, hadronization.

c...the following can be set either to 0 or 1 for
c...any mechanism of xicc production.   
	mstp(71) =1  ! no final state shower
      mstp(81) =1  ! no multiple interactions

c...the following should be fixed to 0 for gc or cc.
c...the following can be either 0 or 1 for gg.
	mstp(61) =1  ! initial state shower

c...the following should be fixed to 0 for any
c...any mechanism of xicc production.   
      mstp(111)=1  ! no hadronization

c...set up the external process.
      idwtup   = idpp
      nprup    = 1
c...more processes to be defined here.
c...if one wants to generate mixing events for gg-,
c...gc-, cc-mechanism, one need to define these three
c...mechansms with different process label here.
      lprup(1) = 1001
	idprup   = lprup(1)

c...set up the maximum cross section in pb. 
c...using the default xmaxup(1)=0 to make pyevnt accept almost 
c...all the upevnt events. crossmax is the maximum differential
c...cross-section.       
	if(idwtup.eq.1) then
	  if(generate) then
         if(ivegasopen.eq.1) then
	    xmaxup(1)=crossmax
	   else
c	    xmaxup(1)=0.462229E+10
		write(*,'(a)') 
     &	 'warning: here should input a maximum differential cross-sec'
		write(*,'(a)')
     &	 '!stop here! input a proper value in (subroutine upinit)!!!!'
	    stop 'or running vegas to get the correct value!program stop!'
c...      xmaxup(1)=100! the value added here! find value in old runs.
c...note: this value should be the one obtained under same condition.
	   end if
	  else
	     xmaxup(1)=0.0d0
	  end if
	end if

c----------------------------------
c...the value of xsecup(1) can be given arbitrarily. all of them are
c...only used for pystat(1) to produce a sensible cross-section table.
c...the actural total cross-sec of xsecup is (appcross).
	if (idwtup.eq.3) then
	  xsecup(1)=vegsec*1.0d+3  ! vegas value (nb) for initialization.
	  xmaxup(1)=crossmax       ! maximum differential cross-section
	end if

999   return
      end

