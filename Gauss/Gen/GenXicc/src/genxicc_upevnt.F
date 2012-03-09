
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     upevnt----call event process and possible error messages.     c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     copyright (c)        z.x zhang and x.g. wu                    c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

c      subroutine upevnt
      subroutine genxicc_upevnt 
      
c...  all real arithmetic in double precision.
      implicit double precision(a-h, o-z)
      implicit integer(i-n)

cc!#include "invegas.h"

c...  user process event common block.
      parameter (maxnup=500)
      common/hepeup/nup,idprup,xwgtup,scalup,aqedup,aqcdup,idup(maxnup),
     &istup(maxnup),mothup(2,maxnup),icolup(2,maxnup),pup(5,maxnup),
     &     vtimup(maxnup),spinup(maxnup)
      save /hepeup/

      parameter (maxpup=100)
      integer pdfgup,pdfsup,lprup
      common/heprup/idbmup(2),ebmup(2),pdfgup(2),pdfsup(2),
     &     idwtup,nprup,xsecup(maxpup),xerrup(maxpup),xmaxup(maxpup),
     &     lprup(maxpup)
      save /heprup/

c-----------------------------------
      common/vegasbin/nvbin
      common/ptpass/ptmin,ptmax,crossmax,etamin,etamax,
     &     smin,smax,ymin,ymax,psetamin,psetamax
      common/counter/ixiccstate,nev
      common/mixevnt/imix,imixtype
      logical generate,ggsub
      common/genefull/generate
      common/totcross/appcross
      common/mtypeofxi/mgenxi
      common/subopen/subfactor,subenergy,isubonly

      dimension x(10),ia(10)

c------------------
      ggsub=.false.

      if(mgenxi.eq.2 .or. mgenxi.eq.3) then
	 ggsub=.true.
      end if

      if(mgenxi.eq.1 .and. (ixiccstate.eq.1 .or. ixiccstate.eq.2))then
	 ggsub=.true.
      end if
      if(mgenxi.eq.1 .and. imixtype.eq.1) ggsub=.true.

      if(isubonly.eq.1) ggsub=.true.

c---------------------------
c...  call respective routine to generate event.
      if(idprup.eq.1001) then
         xnd  =nvbin*1.0d0
         if(ggsub) then
            ndim =7             ! gg-mechanism
            if(isubonly.eq.1) ndim=5
         else  
            ndim =4             ! gc-, cc-mechanisms
         end if
c---------------------------
c...  using generated grade to generate events.
c...  added by yangzw, 2011.05.18
 800  continue
         call generand(ndim,xnd,x,ia,wgt)

         if(isubonly.eq.0) nextdim=ndim-2
         if(isubonly.eq.1) nextdim=ndim

         call phpoint(ndim,nextdim,x,wt)

         if(wt.lt.1.0d-16) then
            xwgtup=0.0d0
         else
            xwgtup=totfun(ndim,x,wt)*wgt
         end if

         if(idwtup.eq.1 .and. generate) then
*     a dirty trick: limiting diff xsection to current xwgtup
*     the current xwgtup is high enough, so the total xsection 
*     does not deviate much. this trick is from yu gouzs program
            if(xwgtup.gt.xmaxup(1)) then
               xwgtup = xmaxup(1)*0.9999999d0
            end if
         else
c...  record maximum differential cross-section.
	    if(xwgtup.gt.crossmax) then
               crossmax=xwgtup
	    end if
         end if
c...  added by yangzw, 2011.05.18
c******the ratio of xwgtup and the xwgtup_max setted at the beginning******
c      xwt_max = xmaxup(1)+1.
      xwt_max = 1.0d6     
      xwt_r=xwgtup/xwt_max

      x_r = rand(0)

      if (x_r.le.xwt_r.and.xwt_r.le.1) then
c...  this event is accepted. 
         goto 900 
      else
         goto 800
      end if
      if(xwt_r.gt.1) then
         goto 900
      end if

c      if(xwt_r.gt.1) then
c         do ic=1,int(xwt_r)
cc write(*,*) "fill times",ic
c            call bcvegpy_write_lhe
c         end do
c         sxwt=xwt_r-int(xwt_r)
c         x_r = rand(0)
c         if (x_r.le.sxwt) then
c            call bcvegpy_write_lhe
c         end if
c       end if
 900  continue


c...  calculate approximate crossection.
         appcross=appcross+xwgtup/nev*1.0d-3 !nb

c...  for color-flow.
         call xicolorflow
c         write(*,*)'///////Begin/////'
c         call pylist(7)
c         write(*,*)'/////// End /////'
      else
         write(*,*) 'fatal error! unknown process',idprup
         stop
      end if

      return
      end
