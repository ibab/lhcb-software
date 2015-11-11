
c******************************************************
c... generate phase_space and right kinematical ranges
c...      (to ensure subprocess has enough energy)
c******************************************************

      subroutine phpoint(ndim,nextdim,zup,wt)
c...preamble: declarations.
      implicit double precision (a-h, o-z)
	implicit integer(i-n)
#include "genxicc_constants.h"
c...pythia common block.
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &vtimup(maxnup),spinup(maxnup)
      save /hepeup/

	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/counter/ixiccstate,nev,xmaxwgt
	common/mixevnt/imix,imixtype
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

      dimension zup(ndim),zzup(nextdim)

	logical ggsub,gcsub,ccsub

c--------------------
	ggsub=.false.
	gcsub=.false.
	ccsub=.false.

	if(imix.eq.0) then
	 if(mgenxi.eq.1 .and.(ixiccstate.eq.1 .or. ixiccstate.eq.2))then
	  ggsub=.true.
	 end if
	 if(mgenxi.eq.1 .and.(ixiccstate.eq.3 .or. ixiccstate.eq.4))then
	  gcsub=.true.
	 end if
	 if(mgenxi.eq.1 .and.(ixiccstate.eq.5 .or. ixiccstate.eq.6))then
	  ccsub=.true.
	 end if
	 if(mgenxi.eq.2 .or. mgenxi.eq.3) ggsub=.true.
	end if

	if(imix.eq.1 .and. imixtype.eq.1) ggsub=.true.
	if(imix.eq.1 .and. imixtype.eq.2) then 
           if(mgenxi.eq.1 .or. mgenxi.eq.3) gcsub=.true.
           if(mgenxi.eq.2) ggsub=.true.
        end if
	if(imix.eq.1 .and. imixtype.eq.3) ccsub=.true.

	if(isubonly.eq.1) then
	  ggsub=.true.
	  gcsub=.false.
	  ccsub=.false.
	end if

c-------------------
c...initial value.
c-------------------
      wt=0.0d0

      do i=1,nextdim
	   zzup(i)=0.0d0
	end do

c--------------------------------------------
c**** constraints from the kinematical region.
c--------------------------------------------

c*********************
c...for g+g mechanism.
c*********************
      if(ggsub) then
       if(mgenxi.eq.1) taumin =((pmxicc+pmc+pmc)/ecm)**2
	 if(mgenxi.eq.2) taumin =((pmxicc+pmc+pmb)/ecm)**2
	 if(mgenxi.eq.3) taumin =((pmxicc+pmb+pmb)/ecm)**2
	 taumax =1.0d0

	 if(isubonly.eq.1 .and. (subfactor**2).lt.taumin) then
	  write(*,'(a)')
     &	  'energy not high enough for generating bound state.'
c	  write(3,'(a)')
c     &	  'energy not high enough for generating bound state.'
	  stop
	 end if

	 if(isubonly.eq.0) then
	  tau=(taumax-taumin)*zup(ndim-1)+taumin
	  yymin= dlog(dsqrt(tau))
	  yymax=-dlog(dsqrt(tau))
	  yy   =(yymax-yymin)*zup(ndim)+yymin
	  x1 =dsqrt(tau)*exp(yy)
	  x2 =dsqrt(tau)*exp(-yy)     
	 end if

	 if(isubonly.eq.1) then
	  x1=subfactor
	  x2=subfactor
	 end if

	 et   =dsqrt(x1*x2)*ecm
	end if

c*********************
c...for g+c mechanism.
c*********************
      if(gcsub) then
	 e0   =ecm/2.0d0
	 dot12=e0**2*zup(ndim-1)*zup(ndim)+
     &	 dsqrt(pmc**2+e0**2*zup(ndim-1)**2)*zup(ndim)*e0
	
	 et =dot12/dsqrt(pmc**2+2.0d0*dot12)+
     &	dsqrt((dot12/dsqrt(pmc**2+2.0d0*dot12))**2+pmc**2)
	 if(et.lt.(pmxicc+pmc)) return
	end if

c*********************
c...for c+c mechanism.
c*********************
      if(ccsub) then
       e0   =ecm/2.0d0
	 dot12=e0**2*zup(ndim-1)*zup(ndim)+dsqrt(pmc**2+
     &    e0**2*zup(ndim-1)**2)*dsqrt(pmc**2+e0**2*zup(ndim)**2)

	 if((dot12-pmc**2).lt.0.0d0) return

       et =2.0d0*dsqrt((dot12+pmc**2)/2.0d0)
	 if(et.lt.pmxicc) return
	end if

c****************************
c****************************
c****************************

      do i=1,nextdim
	  zzup(i)=zup(i)
	end do

c-----------------------------------
c...increase phase-space efficiency
      izero=0
321   call phase_gen(nextdim,zzup,et,wt)
	if((wt .lt. 1.0d-16).and.(izero.lt.100000)) then
	  izero=izero+1
	  goto 321
	end if

c-------------------------------------
c...an extra jacobin for gg-mechanism
	if(ggsub .and. isubonly.eq.0) then
	  wt=(taumax-taumin)*(yymax-yymin)*wt
	end if

	return
	end

