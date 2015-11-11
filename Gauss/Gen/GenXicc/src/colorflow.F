
c**********************************************************
c***** setting the final state particles information
c***** mainly setting the color flow of the process
c***** As for color-flow, the intermediate two diquark
c***** states (not color-singlet) shall be transformed
c***** to \xi_{cc} immediately with 100\% efficiency.
c***** this is good approximation, since fraction of 
c***** diquark-->\xi_{cc}^+ is shapely peaked around z=1.
c*****--------------------------------
c***** only two color-flows for g+g; and one for g+c, c+c .
c**********************************************************

	subroutine xicolorflow
	implicit double precision(a-h, o-z)
	implicit integer(i-n)
				
c...    pythia common block.
#include "genxicc_constants.h"
	common/pypars/mstp(200),parp(200),msti(200),pari(200)
	common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,
	1    idup(maxnup),istup(maxnup),mothup(2,maxnup),
	1    icolup(2,maxnup),pup(5,maxnup),
	2    vtimup(maxnup),spinup(maxnup)
	save /hepeup/
	
	common/counter/ixiccstate,nev,xmaxwgt
	common/mixevnt/imix,imixtype
	common/mtypeofxi/mgenxi
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum
	common/wbstate/ratiou,ratiod,ratios,nbound
	
	logical ggsub,gcsub,ccsub
	
c------------------
	
	ggsub=.false.
	gcsub=.false.
	ccsub=.false.
	
c...    relative ratio for grabing u,d,s quarks to form steady bound state:
c...    1:1:0.3   (adjustable in parameter.F)
c...    since the ratio for heavy quarks is too small, so we shall only take u,d,s into consideration.
	
c...    for the baryon production:
c...    first step: to form (cc), (bc), (bb) diquarks
c...    second step: with relative probability, diquark grabs a certain quarks (u,d,s,) to form
c...    correspond baryons.
c...    (cc) -> (ccu) Xi_{cc}^{++} [4422]; (ccd) Xi_{cc}^{+} [4412]; (ccs) \Omega_{cc}^{+} [4432]
c...    (bc) -> (bcu) Xi_{bc}^{+}  [5242]; (bcd) Xi_{bc}^{0} [5142]; (bcs) \Omega_{bc}^{0} [5342]
c...    (bb) -> (bbu) Xi_{bb}^{0}  [5522]; (bbd) Xi_{bb}^{-} [5512]; (bbs) \Omega_{bb}^{-} [5532]
	
c************************
c***    as for (diquark)-production (nbound=4), we simply take its color flow to
c***    be one of nbound=1,2,3.
	ntmb=nbound
	if(nbound.eq.4) then
	   tm=pyr(0)*(ratiou+ratiod+ratios)
	   if(tm .le. ratiou) nbound=1      
	   if(tm .gt. ratiou .and. tm .le. (ratiou+ratiod)) nbound=2
	   if(tm .gt. (ratiou+ratiod)) nbound=3
	end if     
	
c------------------
c...    choose which subprocess is used to generate events.
	if(imix.eq.0) then
	   if(mgenxi.eq.2 .or. mgenxi.eq.3) then
	      ggsub=.true.
	   end if
	   if(mgenxi.eq.1.and.(ixiccstate.eq.1.or.ixiccstate.eq.2))then
	      ggsub=.true.
	   end if
	   if(mgenxi.eq.1.and.(ixiccstate.eq.3.or.ixiccstate.eq.4))then
	      gcsub=.true.
	   end if
	   if(mgenxi.eq.1.and.(ixiccstate.eq.5.or.ixiccstate.eq.6))then
	      ccsub=.true.
	   end if
	end if
	
	if(imix.eq.1) then
	   if(imixtype.eq.1)  ggsub=.true.
	   if(imixtype.eq.2) then
	      if(mgenxi.eq.1 .or. mgenxi.eq.3) gcsub=.true.
	      if(mgenxi.eq.2) ggsub=.true.
	   end if
	   if(imixtype.eq.3)  ccsub=.true.
	end if
	
	if(isubonly.eq.1) then
	   ggsub=.true.
	   gcsub=.false.
	   ccsub=.false.
	end if
	

	
c       $-----$-----$-----$-----$-----$-----$-----$-----$-----$-----$
c       $-----$-----$-----$-----$-----$-----$-----$-----$-----$-----$
c       $     Add the anti-particle by F. ZHANG
c       $     and a light baryon in the final state to conserve baryon number, to please PYTHIA
c       $     19-Oct-2011 
c       $-----$-----$-----$-----$-----$-----$-----$-----$-----$-----$
c       $-----$-----$-----$-----$-----$-----$-----$-----$-----$-----$     
	
	
	
c       G Graziani 16-Feb-2012: compactify and include all processes


c       G Graziani 5-Nov-2015 switch on/off creation of light baryon according to option iconsbarnum

	iCC = 1
	xrandom = pyr(1)
	if(xrandom.le.0.5d0) then
	   iCC = -1		! charge coniugation on
        end if

c----------------------
c...    xicc, xibc, xibb are all programmed for ggsub
c...    while only xicc sub is programmed for gcsub and ccsub
	if(ggsub) then
c...    define number of partons
	   nup=6
	   if (iconsbarnum.eq.1) nup=8
	   
				!write(*,*)pup(3,7)
c...    set up flavour and history of g + g -> xicc^+ +cbar +cbar (+dbar)
	   idup(1)= 21		! gluon 1
	   idup(2)= 21		! gluon 2
	   
	   if(mgenxi.eq.1) then
	      if(nbound.eq.1) then
		 idup(3)= 4422 * iCC ! xi_{cc}^++
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212 * iCC ! u-antiquark = antiproton + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC
                 else
		    idup(6)=-2 * iCC	! u-antiquark
		 end if
	      end if
	      if(nbound.eq.2) then
		 idup(3)= 4412	* iCC ! xi_{cc}^+
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212	* iCC ! d-antiquark = antiproton + u + u
		    idup(7)=2 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-1 * iCC	! d-antiquark
		 end if
	      end if
	      if(nbound.eq.3) then
		 idup(3)= 4432 * iCC ! omega_{cc}^+
		 if (iconsbarnum.eq.1) then
		    idup(6)=-3122 * iCC ! s-antiquark = antilambda + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-3 * iCC	! s-antiquark
		 end if
	      end if
	      idup(4)=-4	* iCC ! c-antiquark
	      idup(5)=-4	* iCC ! c-antiquark
	      
	      
	   end if
	   
	   if(mgenxi.eq.2) then
	      if(nbound.eq.1) then
		 idup(3)= 5242 * iCC ! xi_{bc}^+
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212 * iCC ! u-antiquark = antiproton + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-2 * iCC ! u-antiquark
		 end if
	      end if
	      if(nbound.eq.2) then
		 idup(3)= 5142 * iCC ! xi_{bc}^0
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212 * iCC ! d-antiquark = antiproton + u + u
		    idup(7)=2 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-1 * iCC ! d-antiquark
		 end if
	      end if
	      if(nbound.eq.3) then
		 idup(3)= 5342 * iCC ! omega_{bc}^0
		 if (iconsbarnum.eq.1) then
		    idup(6)=-3122 * iCC ! s-antiquark = antilambda + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC 
		 else
		    idup(6)=-3 * iCC	! s-antiquark
		 end if
	      end if
	      idup(4)=-5 * iCC	! b-antiquark
	      idup(5)=-4 * iCC	! c-antiquark
	   end if
	   if(mgenxi.eq.3) then
	      if(nbound.eq.1) then
		 idup(3)= 5522 * iCC ! xi_{bb}^0
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212 * iCC ! u-antiquark = antiproton + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-2 * iCC ! u-antiquark
		 end if
	      end if
	      if(nbound.eq.2) then
		 idup(3)= 5512 * iCC ! xi_{bb}^-
		 if (iconsbarnum.eq.1) then
		    idup(6)=-2212 * iCC ! d-antiquark = antiproton + u + u
		    idup(7)=2 * iCC
		    idup(8)=2 * iCC
		 else
		    idup(6)=-1 * iCC ! d-antiquark
		 end if
	      end if
	      if(nbound.eq.3) then
		 idup(3)= 5532 * iCC ! omega_{bb}^-
		 if (iconsbarnum.eq.1) then
		    idup(6)=-3122 * iCC ! s-antiquark = antilambda + u + d
		    idup(7)=1 * iCC
		    idup(8)=2 * iCC 
		 else
		    idup(6)=-3 * iCC ! s-antiquark
		 end if
	      end if
	      idup(4)=-5 * iCC	! b-antiquark
	      idup(5)=-5 * iCC	! b-antiquark
	   end if
	   
c...    status codes.
	   istup(1)=-1
	   istup(2)=-1
	   istup(3)= 1
	   istup(4)= 1
	   istup(5)= 1
	   istup(6)= 1
	   if (iconsbarnum.eq.1) then
	      istup(7)= 1
	      istup(8)= 1
	   end if
	   
c...    mother codes.
	   mothup(1,1)=0	! gluon 1
	   mothup(2,1)=0      
	   mothup(1,2)=0	! gluon 2
	   mothup(2,2)=0      
	   mothup(1,3)=1	! xicc+
	   mothup(2,3)=2      
	   mothup(1,4)=1	! cbar
	   mothup(2,4)=2      
	   mothup(1,5)=1	! cbar
	   mothup(2,5)=2
	   if (iconsbarnum.eq.1) then
	      mothup(1,6)=1	! anti-proton  
	      mothup(2,6)=2
	      mothup(1,7)=1	! ~d, or ~u
	      mothup(2,7)=2
	      mothup(1,8)=1	! ~u
	      mothup(2,8)=2
	   else
	      mothup(1,6)=1	! ~d, or ~u, or ~s
	      mothup(2,6)=2
	   end if
	end if
	
c------------------------------------------------
c...    for gc and cc, only xicc is programmed.
c------------------------------------------------
	if(gcsub) then
c...    define number of partons
	   nup=5
	   if (iconsbarnum.eq.1) nup=7
c...    set up flavour and history of g + c -> xicc +cbar
	   if(nbound.eq.1) then
	      idup(3)= 4422 * iCC ! xi_{cc}^++
	      if (iconsbarnum.eq.1) then
		 idup(5)=-2212 * iCC ! u-antiquark = antiproton + u + d
		 idup(6)=1 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-2 * iCC   ! u-antiquark
	      end if
	   end if
	   if(nbound.eq.2) then
	      idup(3)= 4412 * iCC ! xi_{cc}^+
	      if (iconsbarnum.eq.1) then
		 idup(5)=-2212 * iCC ! d-antiquark = antiproton + u + u	      
		 idup(6)=2 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-1 * iCC ! d-antiquark
	      end if
	   end if
	   if(nbound.eq.3) then
	      idup(3)= 4432 * iCC ! omega_{cc}^+
	      if (iconsbarnum.eq.1) then
		 idup(5)=-3122 * iCC ! s-antiquark = antilambda + u + d
		 idup(6)=1 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-3 * iCC ! s-antiquark
	      end if
	   end if
	   idup(1)= 4	 * iCC	! c-quark
	   idup(2)= 21 * iCC	! gluon 1
	   idup(4)=-4	 * iCC	! c-antiquark
	   
c...    status codes.
	   istup(1)=-1
	   istup(2)=-1
	   istup(3)= 1
	   istup(4)= 1
	   istup(5)= 1	 
	   if (iconsbarnum.eq.1) then
	      istup(6)= 1	 
	      istup(7)= 1	 
	   end if
	   
c...    mother codes.
	   mothup(1,1)=0	! c-quark
	   mothup(2,1)=0      
	   mothup(1,2)=0	! gluon
	   mothup(2,2)=0      
	   mothup(1,3)=1	! xicc+
	   mothup(2,3)=2      
	   mothup(1,4)=1	! cbar
	   mothup(2,4)=2      
	   if (iconsbarnum.eq.1) then
	      mothup(1,5)=1	! antybaryon
	      mothup(2,5)=2      
	      mothup(1,6)=1	! light quark
	      mothup(2,6)=2      
	      mothup(1,7)=1	! light quark
	      mothup(2,7)=2      
	   else
	      mothup(1,5)=1	! anti-light quark
	      mothup(2,5)=2      
	   end if
	end if
	
	if(ccsub) then
c...    define number of partons
	   nup=5
	   if (iconsbarnum.eq.1) nup=7
	   
c...    set up flavour and history of c + c -> xicc +g
	   if(nbound.eq.1) then
	      idup(3)= 4422 * iCC ! xi_{cc}^++
	      if (iconsbarnum.eq.1) then
		 idup(5)=-2212 * iCC ! u-antiquark = antiproton + u + d
		 idup(6)=1 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-2 * iCC ! u-antiquark
	      end if
	   end if
	   if(nbound.eq.2) then
	      idup(3)= 4412 * iCC ! xi_{cc}^+
	      if (iconsbarnum.eq.1) then
		 idup(5)=-2212 * iCC ! d-antiquark = antiproton + u + u
		 idup(6)=2 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-1 * iCC ! d-antiquark
	      end if
	   end if
	   if(nbound.eq.3) then
	      idup(3)= 4432 * iCC ! omega_{cc}^+
	      if (iconsbarnum.eq.1) then
		 idup(5)=-3122 * iCC ! s-antiquark = antilambda + u + d
		 idup(6)=1 * iCC
		 idup(7)=2 * iCC
	      else
		 idup(5)=-3 * iCC ! s-antiquark
	      end if
	   end if
	   
	   idup(1)= 4 * iCC	! c-quark 1
	   idup(2)= 4 * iCC	! c-quark 2
	   idup(4)= 21 * iCC	! gluon
	   
c...    status codes.
	   istup(1)=-1
	   istup(2)=-1
	   istup(3)= 1
	   istup(4)= 1
	   istup(5)= 1	 
	   if (iconsbarnum.eq.1) then
	      istup(6)= 1	 
	      istup(7)= 1	 
	   end if

c...    mother codes.
	   mothup(1,1)=0	! c-quark
	   mothup(2,1)=0      
	   mothup(1,2)=0	! c-quark
	   mothup(2,2)=0      
	   mothup(1,3)=1	! xicc+
	   mothup(2,3)=2      
	   mothup(1,4)=1	! gluon
	   mothup(2,4)=2
	   if (iconsbarnum.eq.1) then
	      mothup(1,5)=1	! antybaryon
	      mothup(2,5)=2      
	      mothup(1,6)=1	! light quark
	      mothup(2,6)=2      
	      mothup(1,7)=1	! light quark
	      mothup(2,7)=2      
	   else
	      mothup(1,5)=1	! anti-light quark
	      mothup(2,5)=2      
	   end if
	   
	end if
	
c--------------------------------------
c       color flows 
	iQu = (-iCC+3)/2	! GG 16/2/2012:: swap (1,2) to (2,1) for charge coniugation
	iQd = ( iCC+3)/2
c...    g+g  ---checked----
c...    there is only two independent color-flow.
	if(ggsub) then
	   if(pyr(0).ge.0.5d0) then
	      icolup(iQu,1)=502
	      icolup(iQd,1)=503
	      icolup(iQu,2)=501
	      icolup(iQd,2)=502
	   else
	      icolup(iQu,1)=501
	      icolup(iQd,1)=502
	      icolup(iQu,2)=502
	      icolup(iQd,2)=503
	   end if
	   
	   icolup(1,3)=0
	   icolup(2,3)=0
	   
	   icolup(iQu,4)=0
	   icolup(iQd,4)=504
	   icolup(iQu,5)=0
	   icolup(iQd,5)=503
	   
	   if (iconsbarnum.eq.1) then
	      icolup(1,6)=0
	      icolup(2,6)=0
	      
	      icolup(iQu,7)=501
	      icolup(iQd,7)=0
	      icolup(iQu,8)=504
	      icolup(iQd,8)=0
	   else 
	      icolup(iQu,6)=0
	      icolup(iQd,6)=505
	   end if
	end if
	
c....   g+c ----checked------ one independent color-flow.
	if(gcsub) then
	   icolup(iQu,1)=502
	   icolup(iQd,1)=0
	   icolup(iQu,2)=501
	   icolup(iQd,2)=502
	   
	   icolup(1,3)=0
	   icolup(2,3)=0
	   
	   icolup(iQu,4)=0
	   icolup(iQd,4)=504

	   if (iconsbarnum.eq.1) then
	      icolup(1,5)=0
	      icolup(2,5)=0
	      
	      icolup(iQu,6)=504
	      icolup(iQd,6)=0
	      
	      icolup(iQu,7)=501
	      icolup(iQd,7)=0
	   else
	      icolup(iQu,5)=0
	      icolup(iQd,5)=505
	   end if
	end if
	
c....   c+c --- the following agree with pythia rules, but
c....   it defines three color-flow lines that can not be dealt
c....   with by pytia itself. so this mechanism can not be run
c....   smoothly within pythia. So it should not be considered at the present.
c....   however, this mechanism is small in comparison to g+g and g+c
c....   mechanism, so it will not take much uncertainty for data analysis.
c       
c       
	if(ccsub) then
	   icolup(iQu,1)=502
	   icolup(iQd,1)=0
	   icolup(iQu,2)=501
	   icolup(iQd,2)=0
	   
	   icolup(1,3)=0
	   icolup(2,3)=0
	   
	   icolup(iQu,4)=502
	   icolup(iQd,4)=504
	   
	   if (iconsbarnum.eq.1) then
	      icolup(1,5)=0
	      icolup(2,5)=0
	      
	      icolup(iQu,6)=504
	      icolup(iQd,6)=0
	      
	      icolup(iQu,7)=501
	      icolup(iQd,7)=0
	   else
	      icolup(1,5)=0
	      icolup(2,5)=504
	   end if
	end if
	
	if(ntmb.eq.4) nbound=4
	
	return
	end
	
	
