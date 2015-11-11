
c*******************************************************************
c*******************************************************************
c*** vegas program: subroutines
c*** vegas()---- integrate over [0,1] integratal variables
c*** generand--- generate random number according to grade function
c*** randa------ generate a group of random number with pyr(0)
c*******************************************************************
c*******************************************************************

c*******************************************************************
c...changes are made which make program only to get importance
c...function. 
      subroutine vegas(fxn,ndim,ncall,itmx,nprn)
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c    arguments:                                                    c
c       fxn    = function to be integrated/mapped                  c
c       ndim   = # dimensions                                      c
c       ncall  = maximum total # of calls to the function          c
c       itmx   = maximum # of iterations allowed                   c
c       nprn   = printout level:                                   c
c              >=2  additionally inf. about accumulated values     c
c                     per iteration.                               c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      implicit double precision(a-h,o-z)
	implicit integer (i-n)
#include "genxicc_constants.h"
c...pythia common block.
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &vtimup(maxnup),spinup(maxnup)
      save /hepeup/

!CC#include "invegas.h"

c...user process event common block.
      common/vegasbin/nvbin  ! F. Zhang 20-04-2011 IMPORTANT MODIFICATION !!!!
                             ! ":1,$s/NVEGBIN/nvbin/g" 
	!common/grade/xi(nvbin,10)
	common/grade/xi(MAXNVBIN,10)

	common/ptpass/ptmin,ptmax,crossmax,etamin,etamax,
     &	smin,smax,ymin,ymax,psetamin,psetamax
	common/vegcross/vegsec,vegerr,iveggrade
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

	character*8 begin_time,end_time

      dimension x(ndim),xin(nvbin),r(nvbin),ia(ndim),d(nvbin,ndim)
c      data alph/1.5d0/

      alph=1.50d0
c...number of bins.
      nd=nvbin
c===============================================================
c a))   initializing some variables
c===============================================================
      si    =0.0d0
      si2   =0.0d0
      swgt  =0.0d0
      schi  =0.0d0
      scalls=0.0d0
	calls =ncall
      xnd   =nd
      ndm   =nd-1
c...............................................................
c  defining the initial intervals distribution
c...............................................................
      if(iveggrade.eq.0) then
	  rc=1.0d0/xnd
        do 7 j=1,ndim
          xi(nd,j)=1.0d0
          dr=0.0d0
        do 7 i=1,ndm
          dr=dr+rc
          xi(i,j)=dr
7       continue
      end if

c===============================================================
c b))  iterating loop
c===============================================================
      do 1000 it=1,itmx
c        if(it.eq.1) then
c	    call time(begin_time)
c	    write(*,'(a)') begin_time
c	    write(3,'(a)') begin_time
c	  end if
c...............................................................
c  initializing iteration variables
c...............................................................
        ti   =0.0d0
        sfun2=0.0d0
        do 10 j=1,ndim
        do 10 i=1,nd
         d(i,j)=0.0d0
10      continue
	  do 11 jj=1,ncall
          call generand(ndim,xnd,x,ia,wgt)

		if(isubonly.eq.0) nextdim=ndim-2
		if(isubonly.eq.1) nextdim=ndim
		
		call phpoint(ndim,nextdim,x,wt)

		if(wt.lt.1.0d-15) then
	      tempfxn=0.0d0
		else
		  tempfxn=fxn(ndim,x,wt)*wgt
	    end if
c...record the maximum differential cross-section.
		if(tempfxn.gt.crossmax) then
	       crossmax=tempfxn
		end if
          fun=tempfxn/calls
          fun2=fun*fun
          weight=wgt/calls
          ti=ti+fun
          sfun2=sfun2+fun2
          do j=1,ndim
            iaj=ia(j)
            d(iaj,j)=d(iaj,j)+fun2
          end do
11      continue
c...............................................................
c  computing the integral and error values
c...............................................................
        ti2=ti*ti
        tsi=dsqrt((sfun2*calls-ti2)/(calls-1.0d0))
        wgta=ti2/tsi**2
        si=si+ti*wgta
        si2=si2+ti2
        swgt=swgt+wgta
        schi=schi+ti2*wgta
        scalls=scalls+calls
        avgi=si/swgt
        sd=swgt*it/si2
        chi2a=0.0d0
        if(it.gt.1)chi2a=sd*(schi/swgt-avgi*avgi)/(it-1)
        sd=1.0d0/dsqrt(sd)
        err=sd*100.0d0/avgi

c...record the cross-section and the corresponding err obtained.
c...may be used for pythia running(initialization).
	  vegsec=avgi  !pb
	  vegerr=err
c...............................................................
c  printing
c...............................................................        
c...cross unit pb.
	  if(nprn.ge.2) then
	    print 201,it,avgi,sd,err
c		write(3,201) it,avgi,sd,err
c	    call time(end_time)
c		write(*,'(a)') end_time
c		write(3,'(a)') end_time
	  end if

778     format('integral value =',g10.4,'+/-',
     &   g10.4,3x,' chi**2=',g10.4, /' ')
201     format('iter.no',i3,' acc.result(pb)==>',g14.5,'+/-',g10.4,
     .   '... % err=',g10.2)
c===============================================================
c  c))   redefining the grid
c===============================================================
c...............................................................
c  smoothing the f**2 valued stored for each interval
c...............................................................
        do 23 j=1,ndim
          xo=d(1,j)
          xn=d(2,j)
          d(1,j)=(xo+xn)/2.0d0
          x(j)=d(1,j)
          do 22 i=2,ndm
            d(i,j)=xo+xn 
            xo=xn
            xn=d(i+1,j)
            d(i,j)=(d(i,j)+xn)/3.0d0
            x(j)=x(j)+d(i,j)
22        continue
            d(nd,j)=(xn+xo)/2.0d0
            x(j)=x(j)+d(nd,j)
23      continue
c...............................................................
c   computing the 'importance function' of each interval
c...............................................................
        do 28 j=1,ndim
         rc=0.0d0
         do i=1,nd
          r(i)=0.0d0
          if(d(i,j).le.0.) go to 224
          xo=x(j)/d(i,j)
          r(i)=((xo-1.0d0)/xo/dlog(xo))**alph
224       rc=rc+r(i)
         end do
c...............................................................
c  redefining the size of each interval
c...............................................................
        rc=rc/xnd
        kk=0
        xn=0.0d0
        dr=0.0d0
        i=0
25      kk=kk+1
        dr=dr+r(kk)
        xo=xn
        xn=xi(kk,j)
26      if(rc.gt.dr) go to 25
        i=i+1
        dr=dr-rc
        xin(i)=xn-(xn-xo)*dr/r(kk)
        if(i.lt.ndm) go to 26
        do i=1,ndm
          xi(i,j)=xin(i)
        end do
        xi(nd,j)=1.0d0
28      continue
1000  continue
	
	return
      end

c**************************************************************
      subroutine generand(ndim,xnd,x,ia,weight)
	implicit double precision(a-h,o-z)
      implicit integer (i-n)
#include "genxicc_constants.h"
!CC#include "invegas.h"
      
      common/vegasbin/nvbin  ! F. Zhang 20-04-2011 IMPORTANT MODIFICATION !!!!
                             ! ":1,$s/NVEGBIN/nvbin/g" 
	!common/grade/xi(nvbin,10)
	common/grade/xi(MAXNVBIN,10)
	dimension ia(ndim),x(ndim),randx(ndim)

      weight=1.0d0

      call randa(ndim,randx)
c...............................................................
c  computing the point position
c...............................................................
	do 15 j=1,ndim
         xn=randx(j)*xnd+1.0d0
         ia(j)=xn
         xim1=0.0d0
         if(ia(j).gt.1) xim1=xi(ia(j)-1,j)
         xo=xi(ia(j),j)-xim1
         x(j)=xim1+(xn-ia(j))*xo
         weight=weight*xo*xnd
15    continue   

      return
	end

c*****************************************************************

      subroutine randa(n,randx)
      implicit double precision(a-h,o-z)
	implicit integer (i-n)
      dimension randx(n)

      do i=1,n
         randx(i)=pyr(0)
      end do

      return
      end

