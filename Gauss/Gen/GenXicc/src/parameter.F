
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c...this subroutine is used to set the necessary parameters for    c
c...the initialization for hadronic production of xicc meson.      c
c...to use the program youd need to make a directory: (data) to    c
c...save all the obtained data-files.                              c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c   to have a better understanding of setting the parameters       c
c   you may see the README file to get more detailed information.  c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c copyright (c)            z.x zhang and x.g. wu                   c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


c GG 17/2/2012: separate the function that initializes parameters to default values (to be called before user settings in LbGenXicc)
c               from the function producing dependencies among parameters (to be called after user settings in LbGenXicc)

	subroutine setxiccdefaultparameters
c GG: here we just set the default parameter values
c...preamble: declarations.
      implicit double precision(a-h, o-z)
	implicit integer(i-n)

!CC#include "invegas.h"

c...user process event common block...pythia
      common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)

c...transform of running information.
	common/confine/ptcut,etacut,psetacut,pscutmin,pscutmax
	double complex colmat,bundamp
      common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
      common/funtrans/npdfu
	common/rconst/pi
	common/usertran/idpp
	common/vegcross/vegsec,vegerr,iveggrade
c...transform of pdf information
	common/outpdf/ioutpdf,ipdfnum
c...transform of the vegas information
      common/vegasinf/number,nitmx
	common/vegasbin/nvbin
c...transform of the events number and xicc state.
	common/counter/ixiccstate,nev,xmaxwgt
c...transform of some common variables
      common/loggrade/ievntdis,igenerate,ivegasopen,igrade,iusecurdir
	common/mixevnt/imix,imixtype

c...determing whether adding subtraction term in charm PDF, used for
c...g+c mechanism to avoid double counting of gc- and gg-mechanisms.
	common/newpara/ncombine

c...transfer the matrix elements information: h1=h3*cmfactor
	common/valmatrix/cmfactor
	common/mtypeofxi/mgenxi
      common/wbstate/ratiou,ratiod,ratios,nbound
	common/extramass/bmxicc,bmxibc,bmxibb,slqmass
c... note: bmxicc=baryon xicc mass; pmxicc=xicc diquark mass
c... note: slqmass=small light quark mass that constitute baryon.

c...whether to get the gg-subprocess information.
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

	logical wronginput

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc  Note: the UPPERCASE parameters are defined in run.F   cc
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c...starting.....

	pi = dacos(-1.0d0)

c*************************************************
c** commen parameters that are used for both
c** imix=0 or imix=1 are listed here for convenience.
c*************************************************

c********************************************
c...for the baryon production:
c...first step: to form (cc), (bc), (bb) diquarks
c...second step: with relative probability, diquark grabs a certain quarks (u,d,s,) to form
c...             correspond baryons.
c... (cc) -> (ccu) Xi_{cc}^{++} [4422]; (ccd) Xi_{cc}^{+} [4412]; (ccs) \Omega_{cc}^{+} [4432]
c... (bc) -> (bcu) Xi_{bc}^{+}  [5242]; (bcd) Xi_{bc}^{0} [5142]; (bcs) \Omega_{bc}^{0} [5342]
c... (bb) -> (bbu) Xi_{bb}^{0}  [5522]; (bbd) Xi_{bb}^{-} [5512]; (bbs) \Omega_{bb}^{-} [5532]

c* ------------------------------
c* define whether to generate ( xibc, xicc or xibb )
c* as for generation of xibc and xibb, only gluon-gluon
c* fusion mechanism are programmed, since the other two
c* mechanisms are small in comparison to gluon fusion
c* ------------------------------
c* mgenxi=1 ... for hadronic production of xicc
c* mgenxi=2 ... for hadronic production of xibc
c* mgenxi=3 ... for hadronic production of xibb
c* ------------------------------
      mgenxi=1

c... choose which bound state to be generated
c	nbound=1 ! Xi_{cc}^{++} or Xi_{bc}^{+} or Xi_{bb}^{0}
	nbound=2 ! Xi_{cc}^{+} or Xi_{bc}^{0} or Xi_{bb}^{-}
c	nbound=3 ! Omega_{cc}^{+} or Omega_{bc}^{0} or Omega_{bb}^{-}
c	nbound=4 ! diquark -- steady bound state -- old calculation

c...relative ratio for grabing u,d,s quarks to form steady bound state:
c...               1:1:0.3   (adjustable)
c...since the ratio for heavy quarks is too small, so we shall only take u,d,s into consideration.

	ratiou=1.0d0   !u-quark
	ratiod=1.0d0   !d-quark
	ratios=0.3d0   !s-quark
	ratioc=1.0d-11 !c-quark and hence b-quark, quite small, 
	               !so do not take them into consideration

c...set the constitute mass: mu=md=0.30GeV, ms=0.45GeV
c...set the current mass of the light quark u, d, s to be
c...center value of Particle Data Group

c...there we take light quark to be their current mass, since
c...diquark shall adopt light quark that comes from a soft gluon
      qmassu=2.80d-3
	qmassd=4.75d-3
	qmasss=104.0d-3

c...sub-process information....only for check of most complex
c...mechanism of gluon-gluon fusion.
	isubonly =0      !switch on/off subprocess information
      subenergy=60.0d0 !c.m. energy (gev) of the sub-process
c  iconsbarnum added by G.Graziani, Nov 5, 2015
      iconsbarnum =1 ! by default, impose explicit baryon number conservation in generated process (required by Pythia6)

c------------------------------------------
c...kinematical variables
c------------------------------------------
      ptcut =0.20d+0   !xicc p_t cut (gev)
      if(isubonly.eq.1) ptcut=0.0d+0

	etacut  =0.6d+10   !xicc rapidity (y) cut
	psetacut=1.0d+10   !xicc pseudo-rapidity cut
        pscutmin=-1.0d+10   ! GG 10/2/2012 allow for LHCb-like eta cut (default is no cut)
        pscutmax= 1.0d+10   !


c...h1=h3*cmfactor, Phys.Lett.B568,135(2003)
c...h1--->c6, h3--->cbar3
c...both are non-perturbative parameters
	cmfactor=1.0d0   !roughly same order of v^2

c---------------------------------------
c...for hadronic production
c...npdfu=1 ---> tevatron=1.96d+3GeV
c...npdfu=2 ---> lhc=1.40d+4GeV
c...npdfu=3 ---> selex=33.58GeV
c---------------------------------------
      npdfu =2      !=1, tevatron; =2, lhc; =3, selex
!     This part is useless now. The energy come in from outside
!     F. ZHANG 12-10-2012
!
!      if(npdfu.eq.1) ecm= ENERGYOFTEVA  !defined in run.F
!c      if(npdfu.eq.2) ecm=0.8d+4 ! F. Zhang 20-04-2011 ENERGYOFLHC   !defined in run.F
!	if(npdfu.eq.3) ecm=ENERGYOFSELEX !defined in run.F

      idpp     =3      !=idwtup: types of pythia generates events
      mstu(111)=1      !setting value of alphas.  pythia parameter

  	paru(111)=0.2d0  !constant value of alphas, used when mstu(111)=0.
      ievntdis =0      !switch on/off to get the event number
c                      !distributions for p_t, rap, shat, pseta, y, z.

c--------------------
c...event information
      igenerate =0  !whether generating full events used when idwtup=1

c...using pythia pdf or not.
      ioutpdf  =1

c...=100, grv98lo; =200, mrst2001l; =300, cteq6hq
c...Strictly, it must be cteq6hq, which is the only one suitable for
c...GM-VFN scheme to avoid double counting of gg- and gc- mechanisms.
c...Other choices can be used as a check with those in literature.
      ipdfnum  =300

c...used by VEGAS
	number=10000  ! NVEGCALL     !number of calling fxn in each iteration
	nitmx =2      ! NVEGITMX     !maximum iteration used
	nvbin =100    ! NVEGBIN      !bin number of (0,1) used in VEGAS

c...used for generation
      ! nev is useless now F. Zhang 28-04-2011
	nev   = 5! F. Zhang 20-04-2011 NUMOFEVENTS  !number of events to be generated

c*************************************************
c...four possible ways to get results are programmed:
c... 1) ivegasopen=1, no matter what value of igrade.
c       iveggrade=0, not improve the existed grade.
c... 2) ivegasopen=1, no matter what value of igrade.
c       iveggrade=1, to improve the existed grade.
c... 3) ivegasopen=0 and igrade=1.
c... 4) ivegasopen=0 and igrade=0.
c*************************************************
      methodevnt=3

      if(methodevnt.eq.1) then
	   ivegasopen=1
         igrade    =0   ! no use for the case
c***********************
c...improve vegas grade
         iveggrade=0    ! not improve the grade
      end if

      if(methodevnt.eq.2) then
	   ivegasopen=1
         igrade    =0   ! no use for the case
c***********************
c...improve vegas grade
         iveggrade=1    ! use existed grade for more precise
c                       ! grade, generated under same condition
      end if

      if(methodevnt.eq.3) then
!         ivegasopen=0
!         igrade    =1
         iveggrade =0   ! no use for the case
	end if

      if(methodevnt.eq.4) then
         ivegasopen=0
         igrade    =0
         iveggrade =0   ! no use for the case
      end if

c**************************************************
c**************************************************
c...whether to get the mixed events for xicc baryon. 
c...here, we only consider the mixed results for
c...the same mechanism, e.g. gg-, gc-, cc- mechanisms.
c...as for xibb and xicc, they involve only gg-mechnism.
c...mixing among different mechansims are not programed
c**************************************************
c...if(imix==1), then we can generate the mixed events 
c...for each mechanism, i.e. contributions from all the
c...intermediate diquark states are summed up.
c---------------------------------
c...if(imix==0), then one can generate every state of 
c...each mechanism separately.
c**************************************************
c**************************************************

				imix=0									! either to be 1 or 0 for xicc, xibb, xibc
				imixtype=1
				ixiccstate=1
c----------------------------------------------
c----------------------------------------------

c GG 16/02/2012  default value for max weight
				xmaxwgt=1.d+6

c...error message.
      wronginput=.false.
	call uperror(wronginput)

	if(wronginput) then
	 print *, '----- Input error! Resetting the parameters !!!'
	 print *, '-----     Stop the program !!!'
	end if
	
	return
	end


	subroutine setxiccconsistentparameters
c GG: here we set the parameters that depend on other parameters, that could have been changed by the LbGenXicc interface
c...preamble: declarations.
      implicit double precision(a-h, o-z)
	implicit integer(i-n)

!CC#include "invegas.h"

c...user process event common block...pythia
      common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)

c...transform of running information.
	common/confine/ptcut,etacut,psetacut,pscutmin,pscutmax
	double complex colmat,bundamp
      common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
      common/funtrans/npdfu
	common/rconst/pi
	common/usertran/idpp
	common/vegcross/vegsec,vegerr,iveggrade
c...transform of pdf information
	common/outpdf/ioutpdf,ipdfnum
c...transform of the vegas information
      common/vegasinf/number,nitmx
	common/vegasbin/nvbin
c...transform of the events number and xicc state.
	common/counter/ixiccstate,nev,xmaxwgt
c...transform of some common variables
      common/loggrade/ievntdis,igenerate,ivegasopen,igrade,iusecurdir
	common/mixevnt/imix,imixtype

c...determing whether adding subtraction term in charm PDF, used for
c...g+c mechanism to avoid double counting of gc- and gg-mechanisms.
	common/newpara/ncombine

c...transfer the matrix elements information: h1=h3*cmfactor
	common/valmatrix/cmfactor
	common/mtypeofxi/mgenxi
      common/wbstate/ratiou,ratiod,ratios,nbound
	common/extramass/bmxicc,bmxibc,bmxibb,slqmass
c... note: bmxicc=baryon xicc mass; pmxicc=xicc diquark mass
c... note: slqmass=small light quark mass that constitute baryon.

c...whether to get the gg-subprocess information.
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

	logical wronginput

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
cc  Note: the UPPERCASE parameters are defined in run.F   cc
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc


c...there we take light quark to be their current mass, since
c...diquark shall adopt light quark that comes from a soft gluon

	if(nbound.eq.1) slqmass=qmassu
	if(nbound.eq.2) slqmass=qmassd
	if(nbound.eq.3) slqmass=qmasss
	if(nbound.eq.4) slqmass=0.0d0


c------------------------------------------
c...set basic parameters. note pmxicc should be equal to
c...(pmb+pmc) to ensure gauge invariance of amplitude.
c...values adopted in Phys.Rev.D73,094022(2006).
c...
c... further more, to make the second step of diquark fragment 
c... into the baryon+light ~quark consistently, we make diquark 
c... slightly bigger than the final baryon mass; so the c and b 
c... quark masses are slightly bigger than that of Phys.Rev.D73,094022(2006).
c------------------------------------------
      if(mgenxi.eq.1) then  !xicc
          if(ichange.eq.0) then
             pmc0  = 1.75d0   ! c quark mass (gev)
             pmb0  = 1.75d0   ! c quark mass (gev)
             pmc   = pmc0 +slqmass/2.0d0   ! c quark mass (gev)
             pmb   = pmb0 +slqmass/2.0d0   ! c quark mass;

             bmxicc = pmb0+pmc0            ! xicc baryon mass (gev)
             pmxicc = pmb+pmc              ! diquark mass (gev)
         else if(ichange.eq.1) then
             bmxicc = pmb+pmc              ! xicc baryon mass (gev)
             pmxicc = pmb+pmc              ! diquark mass (gev)
         end if 
	  end if

      if(mgenxi.eq.2) then  !xibc
	      pmc0  =1.80d0    ! c quark mass (gev)
        pmb0  =5.10d0    ! b quark mass;
	      pmc  =pmc0 +slqmass/2.0d0    ! c quark mass (gev)
        pmb  =pmb0 +slqmass/2.0d0    ! b quark mass;
        bmxicc =pmb0+pmc0            ! baryon xibc mass (gev)
        pmxicc =pmb+pmc              ! diquark (bc) mass (gev)
	   end if

      if(mgenxi.eq.3) then  !xibb
	      pmc0  =5.10d0    ! b quark mass (gev)
        pmb0  =5.10d0    ! b quark mass;
				pmc  =pmc0 +slqmass/2.0d0	! b quark mass (gev)
        pmb  =pmb0 +slqmass/2.0d0    ! b quark mass;
        bmxicc =pmb0+pmc0            ! baryon xibb mass (gev)
        pmxicc =pmb+pmc              ! diquark (bb) mass (gev)
	    end if



      if(isubonly.eq.1) ptcut=0.0d+0


c------------------------------------------
c...all the state has the same value for wavefunction at zero:
c... r(0)=0.700063D0  <--->  \psi(0)=(0.197)   for xicc
c... r(0)=0.903773D0  <--->  \psi(0)=(0.255)   for xibc
c... r(0)=1.382095D0  <--->  \psi(0)=(0.390)   for xibb
c------------------------------------------
	if(mgenxi.eq.1) fxicc  =0.700063D0
	if(mgenxi.eq.2) fxicc  =0.903773D0
	if(mgenxi.eq.3) fxicc  =1.382095D0


c...for subprocess only
	if(isubonly.eq.1) subfactor=subenergy/ecm

      if(isubonly.eq.1) mstu(111)=0


      if(ioutpdf.eq.0) then
c	  mstp(51) =7   !used user-preferable one for pythia inner pdf
	  mstp(51) =10042   !used user-preferable one for pythia inner pdf
      end if

      if(ioutpdf.eq.1 .and. ipdfnum.eq.300) call setctq6(21)   !cteq6hq


	if(isubonly.eq.1) imix=0   ! must fix to 0 for gg-subprocess

c--------------------------------------------
c***** set the parameters for mixing results
c--------------------------------------------

	if (imix.eq.1) then
c*************************************************
c...the difference of xibc and xibc':
c...the light quark is symmetric for xibc
c...the light quark is anti-symmetric for xibc*
c*************************************************
c...imixtype: set how many states need to be mixed
c...for xicc, imixtype=1,2,3; 
c...for xibc and xibc', imixtype=1, 2;
c...for xibb, imixtype=1 only;

c...for xicc:
c...imixtype=1: mixed events for gg-mechanism.
c...imixtype=2: mixed events for gc-mechanism.
c...imixtype=3: mixed events for cc-mechanism.
c...for xibc and xibc* ( only a notation)
c...imixtype=1:  mixed events for gg-mechanism.---xibc
c...imixtype=2:  mixed events for gg-mechanism.---xibc*
c...for xibb:
c...imixtype=1: mixed events for gg-mechanism.

c************************************************

c****************************************
c...determing whether add subtraction term in charm PDF, used for
c...g+c mechanism. also defined in evntinit.F
	  ncombine=0
	  if(mgenxi.eq.1 .and. imixtype.eq.2)  ncombine=1 ! F. Zhang 20-04-2011 NSUBPDF
	end if

c--------------------------------------------------
c***** set the parameters for separate mechanism.
c***** The following is for generating single state
c***** of single mechanism: gg-, gc-, cc-
c--------------------------------------------------

c*****************************************
c...note: following initial values are only for checking whether
c...program is ok. to do numerical calculation/event generation, 
c...one needs to be careful about all these values, especailly
c...those parameters that can improve the precision.
c****************************************

	if(mgenxi.eq.1 .and. imix.eq.0) then
c****************************************
c...to choose what xicc/xibb state/channel we need to generate.
c...1--(3s1)_{cbar3} +gg; 2--(1s0)_{c6} +gg; 3--(3s1)_{cbar3} +gc; 
c...4--(1s0)_{c6} +gc;    5--(3s1)_{cbar3} +cc; 6--(1s0)_{c6} +cc;
c******************************************

c****************************************
c...determing whether add subtraction term in charm PDF, used for
c...g+c mechanism. also defined in evntinit.F
	  ncombine=0
	  if(isubonly.eq.0) then
	   if(ixiccstate.eq.3 .or. ixiccstate.eq.4) then
	    ncombine=1 ! F. Zhang 20-04-2011 NSUBPDF
	   end if
	  end if
      end if

	if(mgenxi.eq.2 .and. imix.eq.0) then
c****************************************
c...to choose what xibc state/channel we need to generate.
c...1--(3s1)_{cbar3} +gg; 2--(1s0)_{c6} +gg;
c...3--(3s1)_{c6} +gg; 4--(1s0)_{cbar3} +gg;
c******************************************
	  ncombine=0     ! no use for the case
      end if

	if(mgenxi.eq.3 .and. imix.eq.0) then
c****************************************
c...to choose what xibc state/channel we need to generate.
c...1--(3s1)_{cbar3} +gg; 2--(1s0)_{c6} +gg;
c******************************************
	  ncombine=0     ! no use for the case
      end if
	

c...error message.
      wronginput=.false.
	call uperror(wronginput)

	if(wronginput) then
	 print *, '----- Input error! Resetting the parameters !!!'
	 print *, '-----     Stop the program !!!'
	end if
	
	return
	end

