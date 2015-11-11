
c********************************************************
c********************************************************

      double precision function sepfun(ndim,zup,papawt)
	implicit double precision (a-h,o-z)
	implicit integer (i-n)
#include "genxicc_constants.h"
c------------------------
c...pythia common block.
      common/pydat1/mstu(200),paru(200),mstj(200),parj(200)
	common/pypars/mstp(200),parp(200),msti(200),pari(200)
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &vtimup(maxnup),spinup(maxnup)
      save /hepeup/

c------------------------
c...user transformation.
	double complex colmat,bundamp
	common/upcom/ecm,pmxicc,pmb,pmc,fxicc,pmomup(5,8),
     & 	colmat(6,64),bundamp(4),pmomzero(5,8)
	common/counter/ixiccstate,nev,xmaxwgt
	common/rconst/pi
	common/ptpass/ptmin,ptmax,crossmax,etamin,etamax,
     &	smin,smax,ymin,ymax,psetamin,psetamax
	common/confine/ptcut,etacut,psetacut,pscutmin,pscutmax
      common/funtrans/npdfu

c...generate---switch for full events.
c...alphas used only in the function frag()---phase_point.F
	logical generate
	common/genefull/generate
      common/extraz/zfactor,zmin,zmax
	common/outpdf/ioutpdf,ipdfnum
	common/newpara/ncombine
	common/upalphas/alps
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum
c...transfer the matrix elements information: h1=h3*cmfactor
	common/valmatrix/cmfactor

	common/wbstate/ratiou,ratiod,ratios,nbound

	common/extramass/bmxicc,bmxibc,bmxibb,slqmass
c... note: bmxicc=baryon xicc mass; pmxicc=xicc diquark mass
c... note: slqmass=small light quark mass that constitute baryon.

	logical ggsub,gcsub,ccsub

	external cut

      dimension zup(ndim)
	dimension xpp(-25:25),xppbar(-25:25),pboo(4),pc(4),pl(4)
      data conv/3.8938573d+8/ !pb

c--------------------
      sepfun =0.0d0
	nextdim=ndim-1

	ggsub=.false.
	gcsub=.false.
	ccsub=.false.

	if(mgenxi.eq.1 .and. (ixiccstate.eq.1 .or. ixiccstate.eq.2))then
	 ggsub=.true.
	end if

	if(mgenxi.eq.1 .and. (ixiccstate.eq.3 .or. ixiccstate.eq.4))then
	 gcsub=.true.
	end if

	if(mgenxi.eq.1 .and. (ixiccstate.eq.5 .or. ixiccstate.eq.6))then
	 ccsub=.true.
	end if

	if(mgenxi.eq.2 .or. mgenxi.eq.3) then
	 ggsub=.true.
	end if

	if(isubonly.eq.1) ggsub=.true.

c--------------------

c--------------------
c*** gg-mechanism.
      if(ggsub) then
	  if(isubonly.eq.0) then
          if(mgenxi.eq.1) taumin =((pmxicc+pmc+pmc)/ecm)**2
	    if(mgenxi.eq.2) taumin =((pmxicc+pmc+pmb)/ecm)**2
	    if(mgenxi.eq.3) taumin =((pmxicc+pmb+pmb)/ecm)**2
	    taumax =1.0d0
	    tau=(taumax-taumin)*zup(nextdim)+taumin
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

c... gluon 1, in lab
        pup(1,1)= 0.0d0
        pup(2,1)= 0.0d0
        pup(3,1)= ecm*x1/2.0d0
        pup(4,1)= ecm*x1/2.0d0
        pup(5,1)= 0.0d0

c... gluon 2, in lab
        pup(1,2)= 0.0d0
        pup(2,2)= 0.0d0
        pup(3,2)=-ecm*x2/2.0d0
        pup(4,2)= ecm*x2/2.0d0
        pup(5,2)= 0.0d0

	end if

c-------------------
c*** gc-mechanism.
      if(gcsub) then
c...chose which parton is from which hadron.
	 extradim=pyr(0)

c... initial c, in lab
       pup(1,1)= 0.0d0
       pup(2,1)= 0.0d0
       pup(3,1)= ecm*zup(nextdim)/2.0d0
       pup(5,1)= pmc
       pup(4,1)= dsqrt(pup(3,1)**2+pup(5,1)**2)

c... initial gluon, in lab
       pup(1,2)= 0.0d0
       pup(2,2)= 0.0d0
       pup(3,2)=-ecm*zup(ndim)/2.0d0
       pup(4,2)= ecm*zup(ndim)/2.0d0
       pup(5,2)= 0.0d0

	 if(extradim.ge.0.5d0) then
	  do i=1,2
	   pup(1,i)= -pup(1,i)
         pup(2,i)= -pup(2,i)
         pup(3,i)= -pup(3,i)
	  end do
	 end if
	end if
	   
c-------------------
c*** cc-mechanism.
      if(ccsub) then
c... initial c 1, in lab
       pup(1,1)= 0.0d0
       pup(2,1)= 0.0d0
       pup(3,1)= ecm*zup(nextdim)/2.0d0
       pup(5,1)= pmc
       pup(4,1)= dsqrt(pup(3,1)**2+pup(5,1)**2)

c... initial c 2, in lab
       pup(1,2)= 0.0d0
       pup(2,2)= 0.0d0
       pup(3,2)=-ecm*zup(ndim)/2.0d0
       pup(5,2)= pmc
       pup(4,2)= dsqrt(pup(3,2)**2+pup(5,2)**2)

	end if

        
c above for g1 and g1 and is useless
c ---------------------------------------------------











c------------------------------------------------------------------
c...change momtuma of the final particals into lab coordinate system.
c...the original ones getting from phase_gen is result for c.m. system.
c------------------------------------------------------------------

c...reference momentum
	do i=1,4
          pboo(i)=pup(i,1)+pup(i,2)
      end do

c----------------------------------
c*** gg mechanism
c*** there is slight different from the Bc case, since
c*** Xi_cc is throught (cc)-diquark pair; Bc is (c b-bar)
c----------------------------------
	if(ggsub) then
       do j=1,4
	      pc(j)=pmomup(j,3)
       end do
	 call lorentz(pboo,pc,pl)
	 do j=1,4
	      pmomup(j,3)=pl(j)
       end do

       do i=5,6
	  do j=1,4
	      pc(j)=pmomup(j,i)
        end do
	  call lorentz(pboo,pc,pl)
	  do j=1,4
	      pmomup(j,i)=pl(j)
        end do
	 end do
	 
c...for s-wave bound state (cc), taking non-relativistic approximation.
	 do i=1,5
	  pmomup(i,4)=pmb/pmxicc*pmomup(i,3)
	  pmomup(i,7)=pmc/pmxicc*pmomup(i,3)
       end do

c............................
c...final states momenta for PYTHIA in the lab frame

      if (nbound.eq.4) goto 111

c========================
c...construct diquark decay kinematics isotropically in angle and boost.
c...in the diquark rest frame
       costhe=2d0*pyr(0)-1d0
       phi=2d0*pi*pyr(0)
	 temp=(pmxicc**2-bmxicc**2-0.938**2)**2-
     &	4d0*bmxicc**2*0.938**2
	 if(temp.le.1.0d-16) temp=1.0d-16
       pabs=0.5d0*sqrt(temp)/pmxicc

       pup(1,3)=pabs*sqrt(1d0-costhe**2)*cos(phi)
       pup(2,3)=pabs*sqrt(1d0-costhe**2)*sin(phi)
       pup(3,3)=pabs*costhe
       pup(4,3)=0.5d0*(pmxicc**2+bmxicc**2-0.938**2)/pmxicc
       pup(5,3)=bmxicc

       pup(1,6)=-pup(1,3)
       pup(2,6)=-pup(2,3)
       pup(3,6)=-pup(3,3)
       pup(4,6)=0.5d0*(pmxicc**2+0.938**2-bmxicc**2)/pmxicc
       pup(5,6)=0.938
       

c...boost to the diquark moving frame
	 do i=1,4
          pboo(i)=pmomup(i,3)   ! diquark momentum as boost
       end do

       do j=1,4
	      pc(j)=pup(j,3)
       end do
	 call lorentz(pboo,pc,pl)
	 do j=1,4
	      pup(j,3)=pl(j)   ! baryon
       end do

       do j=1,4
	      pc(j)=pup(j,6)
       end do
	 call lorentz(pboo,pc,pl)
	 do j=1,4
	      pup(j,6)=pl(j)   ! light quark
       end do

c======================

111	 if (nbound.eq.4) then
        do j=1,5
	    pup(j,3)=pmomup(j,3)  !diquark
	  end do
        pup(1,6)=0.0d0
        pup(2,6)=0.0d0
        pup(3,6)=0.0d0
        pup(4,6)=0.0d0
        pup(5,6)=0.0d0
	 end if

	 do j=1,4
	    pup(j,4)=pmomup(j,6)*0.97  !cbar 1
            pup(j,7)=pmomup(j,6)*0.03
	 end do
	 do j=1,4
	    pup(j,5)=pmomup(j,5)*0.97  !cbar 2
            pup(j,8)=pmomup(j,5)*0.03
	 end do
            pup(5,4)=pmomup(5,6)
            pup(5,5)=pmomup(5,5)
            pup(5,7)=slqmass
            pup(5,8)=slqmass
           !write(*,*)pup(3,7) 
         
	end if

c----------------------------------
c*** gc-mechanism or cc-mechanism
c----------------------------------
	if(gcsub .or. ccsub) then
       do 101, i=nextdim,ndim
	    do j=1,4
	      pc(j)=pmomup(j,i)
          end do
		call lorentz(pboo,pc,pl)
	    do j=1,4
	      pmomup(j,i)=pl(j)
          end do
101    continue

c........................

       if(nbound.eq.4) goto 112

c========================
c...construct diquark decay kinematics isotropically in angle and boost.
c...in the diquark rest frame
       costhe=2d0*pyr(0)-1d0
       phi=2d0*pi*pyr(0)
	 temp=(pmxicc**2-bmxicc**2-slqmass**2)**2-
     &	4d0*bmxicc**2*slqmass**2
	 if(temp.le.1.0d-16) temp=1.0d-16
       pabs=0.5d0*sqrt(temp)/pmxicc

       pup(1,3)=pabs*sqrt(1d0-costhe**2)*cos(phi)
       pup(2,3)=pabs*sqrt(1d0-costhe**2)*sin(phi)
       pup(3,3)=pabs*costhe
       pup(4,3)=0.5d0*(pmxicc**2+bmxicc**2-slqmass**2)/pmxicc
       pup(5,3)=bmxicc

       pup(1,5)=-pup(1,3)
       pup(2,5)=-pup(2,3)
       pup(3,5)=-pup(3,3)
       pup(4,5)=0.5d0*(pmxicc**2+slqmass**2-bmxicc**2)/pmxicc
       pup(5,5)=slqmass

c...boost to the diquark moving frame
	 do i=1,4
          pboo(i)=pmomup(i,3)   ! diquark momentum as boost
       end do

       do j=1,4
	      pc(j)=pup(j,3)
       end do
	 call lorentz(pboo,pc,pl)
	 do j=1,4
	      pup(j,3)=pl(j)   ! baryon
       end do

       do j=1,4
	      pc(j)=pup(j,5)
       end do
	 call lorentz(pboo,pc,pl)
	 do j=1,4
	      pup(j,5)=pl(j)   ! light quark
       end do

c======================

112	 if(nbound.eq.4) then
        do j=1,5
	    pup(j,3)=pmomup(j,3)  !diquark
	  end do
        pup(1,5)=0.0d0
        pup(2,5)=0.0d0
        pup(3,5)=0.0d0
        pup(4,5)=0.0d0
        pup(5,5)=0.0d0
	 end if

	 do j=1,5
	    pup(j,4)=pmomup(j,4)  !cbar or g
	 end do
	end if

c...save incoming particle momenta to intermediate pmomup().
     	do i=1,2
	  do j=1,5
	    pmomup(j,i)=pup(j,i)
	  end do
	end do

c-------------------------------------------------------------
c...two extra constraints from pqcd applicability, especially
c...for SELEX. this is only necessary for c+c mechanism:
c-------------------------------------------------------------
      if(isubonly.eq.0 .and. npdfu.eq.3) then
	 if(ccsub) then
	   momq1s=dotup(2,2)+dotup(3,3)/4.0d0-dotup(2,3)/2.0d0
	   momq2s=dotup(1,1)+dotup(3,3)/4.0d0-dotup(1,3)/2.0d0
	   xx=pmxicc**2
	   if(abs(momq1s).lt.xx .or. abs(momq2s).lt.xx) return
	 end if
	end if

c...this part is from the inner part of pythia subroutine pyp().
      ptxicc=dsqrt(pup(1,3)**2+pup(2,3)**2)
      pr    =max(1.0d-16,pup(5,3)**2+pup(1,3)**2+pup(2,3)**2)
	prs   =max(1.0d-16,pup(1,3)**2+pup(2,3)**2)
      eta   =sign(dlog(min((dsqrt(pr+pup(3,3)**2)+dabs(pup(3,3)))
     &          /dsqrt(pr),1.0d+20)),pup(3,3))
	pseta =sign(dlog(min((dsqrt(prs+pup(3,3)**2)+dabs(pup(3,3)))
     &          /dsqrt(prs),1.0d+20)),pup(3,3))

c...other confinement can also be added here.
	if(ptxicc.lt.ptcut .or. abs(eta).gt.etacut .or.
     &	abs(pseta).gt.psetacut) return

      if(pseta.lt.pscutmin .or. pseta.gt.pscutmax) return ! GG 10/2/2012 LHCb-like eta cut

c...energy scale---the transverse mass of xicc.
	q2 =ptxicc**2+pup(5,3)**2
	
c...get the value of alphas. all are in leading order.
	if(mstu(111).eq.0 .or. isubonly.eq.1) then
	 alps=paru(111)
	else
	 if(ioutpdf.eq.1) then
	   if(ipdfnum.eq.100) alps =alpgrv(q2,1)*4*pi
	   if(ipdfnum.eq.200) alps =alpmsrt(dsqrt(q2),0.220d0,0)
	   if(ipdfnum.eq.300) alps =alpcteq(q2,1)*4*pi
	 else
	   alps =pyalps(q2)
       end if
	end if


c...store scale choice and alphas.
      scalup=dsqrt(q2)
	qq    =dsqrt(q2)
      aqcdup=alps

c... determine which channel to used.
      if(ggsub) goto 1000
      if(gcsub) goto 2000
	if(ccsub) goto 3000

c-------------------------------------------------
c----  gg mechanism differential cross-ection  ---
c-------------------------------------------------

1000  continue

	if(isubonly.eq.0) then
	 if(ioutpdf.eq.0) then
c...tevatron or selex
c...evaluate parton distribution for (g1<--p,g2<---p~)
	   if(npdfu.eq.1 .or. npdfu.eq.3) then
	     call pypdfu( 2212,x1,q2,xpp)
           call pypdfu(-2212,x2,q2,xppbar)
	   end if
c...lhc
c...evaluate parton distribution for (g1<--p,g2<---p)
 	   if(npdfu.eq.2) then
	     call pypdfu( 2212,x1,q2,xpp)
           call pypdfu( 2212,x2,q2,xppbar)
	   end if
	 else
	   if(ipdfnum.eq.100) then
	     call grv98pa(1, x1, q2, uv, dv, us, ds, ss, gl1)
           call grv98pa(1, x2, q2, uv, dv, us, ds, ss, gl2)
		 xpp(21)   =gl1
		 xppbar(21)=gl2
	   end if
	   if(ipdfnum.eq.200) then
		 call mrstlo(x1,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu1)
	     call mrstlo(x2,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu2)
		 xpp(21)   =glu1
		 xppbar(21)=glu2
	   end if
	   if(ipdfnum.eq.300) then
	     xpp(21)   =ctq6pdf(0,x1,qq)*x1
		 xppbar(21)=ctq6pdf(0,x2,qq)*x2
	   end if
	 end if
	end if

c...this ensure the rightness of the extrapolation of the pdf.
c...(by using the pdfs, sometimes it will get negative value)
	if(isubonly.eq.0) then
	 if(xpp(21).lt.1.0d-16) xpp(21)=0.0d0
	 if(xppbar(21).lt.1.0d-16) xppbar(21)=0.0d0
	end if

c...common factor.
	phase =papawt*alps**4.0d0/(2.0d0**11*pi*dotup(1,2))

	call xsection(sepcross)

      conv    =3.8938573d+8                !pb
	if(isubonly.eq.0) then
	 sepfun  =conv*phase*sepcross*xpp(21)*xppbar(21)/x1/x2
	else
	 sepfun  =conv*phase*sepcross
	end if

c...getting an extra distribution about z=(2(k1+k2).p_bc)/shat.
      zfactor=2.0d0*(dotup(1,3)+dotup(2,3))/(x1*x2*ecm**2.0d0)

c...note the factor cmfactor has been included into function xsection().
c...the following should be commentted out.
cc	if(ixiccstate.eq.2) sepfun=cmfactor*sepfun

	if(sepfun.lt.1.0d-16) sepfun=1.0d-16
	return

c-------------------------------------------------
c----  gc mechanism differential cross-ection  ---
c-------------------------------------------------

2000  continue

	xx1=dsqrt(zup(nextdim)**2+4.0d0*pmc**2/ecm**2)
	xx2=zup(ndim)
	if(xx1.gt.1.0d0.or.xx2.gt.1.0d0) return

c----------------------------

	if(ncombine.eq.1) then
		call qtrap(qq,xx1,cut,xx1,1.0d0,sigma)
	    sigma=xx1*sigma
	else
	    sigma=0.0d0
	end if

c----------------------------

	if(ioutpdf.eq.0) then
c...tevatron or selex
	   if(npdfu.eq.1 .or. npdfu.eq.3) then
	     if(extradim.lt.0.5d0) then
	      call pypdfu( 2212,xx1,q2,xpp)
            call pypdfu(-2212,xx2,q2,xppbar)
	     else
	      call pypdfu(-2212,xx1,q2,xpp)
            call pypdfu( 2212,xx2,q2,xppbar)
	     end if
	   end if
c...lhc, evaluate parton distribution for (c<--p, c<---p)
 	   if(npdfu.eq.2) then
	     call pypdfu( 2212,xx1,q2,xpp)
           call pypdfu( 2212,xx2,q2,xppbar)
	   end if	   
	   xpdf1=xpp(4)-sigma
	   xpdf2=xppbar(21)
	else
c------------------
	   if(ipdfnum.eq.100) then
		 call grv98f2(1,xx1, q2, f2l, f2c, f2b, f2)
	     call grv98pa(1,xx2, q2, uv, dv, us, ds, ss, gl)
	     xpdf1=f2c-sigma
	     xpdf2=gl
	   end if
c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   if(ipdfnum.eq.200) then
		 call mrstlo(xx1,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu1)
	     call mrstlo(xx2,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu2)
		 xpdf1 =chm-sigma
		 xpdf2 =glu2
	   end if
c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   if(ipdfnum.eq.300) then
	     if(extradim.lt.0.5d0) then
	       xpdf1=ctq6pdf(-4,xx1,qq)*xx1-sigma
	       xpdf2=ctq6pdf( 0,xx2,qq)*xx2
           else
	       xpdf1=ctq6pdf( 0,xx2,qq)*xx2
		   xpdf2=ctq6pdf(-4,xx1,qq)*xx1-sigma
	       if(npdfu.eq.1 .or. npdfu.eq.3) then
	         xpdf2=ctq6pdf( 4,xx1,qq)*xx1-sigma
	       end if
	     end if
	   end if
	end if

	cfact=0.041666667d0       !1/24

c...the extra factor 2 comes from the combination of 
c...(g+c) or (c+g)--from the hadrons.
	if(ixiccstate.eq.3) smatval=2.0d0*alps**3*gcxicc3()
	if(ixiccstate.eq.4) smatval=2.0d0*alps**3*gcxicc6()

	phase =papawt*cfact/(2.0d0**2*4.0d0*dotup(1,2))

	sepfun=conv*phase*smatval*xpdf1*xpdf2/((2.0d0*pi)**2*xx1*xx2)

 	if(ixiccstate.eq.4) sepfun=cmfactor*sepfun

	if(sepfun.lt.1.0d-16) sepfun=1.0d-16

	return

c-------------------------------------------------
c----  cc mechanism differential cross-ection  ---
c-------------------------------------------------

3000  continue

	xx1=dsqrt(zup(nextdim)**2+4.0d0*pmc**2/ecm**2)
	xx2=dsqrt(zup(ndim)**2+4.0d0*pmc**2/ecm**2)
	if(xx1.gt.1.0d0 .or. xx2.gt.1.0d0) return

	if(ioutpdf.eq.0) then
c...tevatron or selex
	   if(npdfu.eq.1 .or. npdfu.eq.3) then
	     call pypdfu( 2212,xx1,q2,xpp)
           call pypdfu(-2212,xx2,q2,xppbar)
	     xpdf1=xpp(4)
	     xpdf2=xppbar(4)
	   end if
c...lhc
c...evaluate parton distribution for (c<--p, g<---p)
 	   if(npdfu.eq.2) then
	     call pypdfu(2212,xx1,q2,xpp)
           call pypdfu(2212,xx2,q2,xppbar)
	     xpdf1=xpp(4)
	     xpdf2=xppbar(4)
	   end if	   
	else
c------------------
	   if(ipdfnum.eq.100) then
	     call grv98f2(1,xx1, q2, f2l, f2c, f2b, f2)
		 xpdf1 =f2c
		 call grv98f2(1,xx2, q2, f2l, f2c, f2b, f2)
		 xpdf2 =f2c
	   end if
c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   if(ipdfnum.eq.200) then
		 call mrstlo(xx1,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu1)
		 xpdf1 =chm
	     call mrstlo(xx2,qq,1,upv,dnv,usea,dsea,str,chm,bot,glu2)
		 xpdf2 =chm
	   end if
c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   if(ipdfnum.eq.300) then
	    if(npdfu.eq.1 .or. npdfu.eq.3) then
	      xpdf1=ctq6pdf( 4,xx1,qq)*xx1
	      xpdf2=ctq6pdf(-4,xx2,qq)*xx2
          end if
	    if(npdfu.eq.2) then
	      xpdf1=ctq6pdf( 4,xx1,qq)*xx1
	      xpdf2=ctq6pdf( 4,xx2,qq)*xx2
          end if
	   end if
	end if

	if(ixiccstate.eq.5) smatval=alps**3*ccxicc3()
	if(ixiccstate.eq.6) smatval=alps**3*ccxicc6()

	phase =papawt/(2.0d0**2*3.0d0**2*4.0d0*
     &	 dsqrt(dotup(1,2)**2-pmb**2*pmc**2))

	sepfun=conv*phase*smatval*xpdf1*xpdf2/((2.0d0*pi)**2*xx1*xx2)

	if(ixiccstate.eq.6) sepfun=cmfactor*sepfun
	
	if(sepfun.lt.1.0d-16) sepfun=1.0d-16

	return
	end
