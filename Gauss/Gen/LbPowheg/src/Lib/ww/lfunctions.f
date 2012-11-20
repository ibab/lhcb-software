************************************************************************
*     Author: R.K. Ellis                                               *
*     July, 1998/July 2005/June 2007                                   *
************************************************************************

      double complex function ww_L0(x,y)
      implicit none
      include 'src/Lib/ww/constants.h' 
      double complex ww_Lnrat
      double precision x,y,denom
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      ww_L0=-cone-dcmplx(denom*(half+denom/3d0))
      else
      ww_L0=ww_Lnrat(x,y)/dcmplx(denom)
      endif
      return
      end

      double complex function ww_L1(x,y)
      implicit none
      include 'src/Lib/ww/constants.h' 
      double precision x,y,denom
      double complex ww_L0
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      ww_L1=-half*cone-dcmplx(denom/3d0*(one+0.75d0*denom))
      else
      ww_L1=(ww_L0(x,y)+cone)/dcmplx(denom)
      endif
      return
      end

      double complex function ww_L2(x,y)
      implicit none
      include 'src/Lib/ww/constants.h' 
      double complex ww_Lnrat
      double precision x,y,r,denom
      r=x/y
      denom=one-r
      if (abs(denom) .lt. 1d-7) then
      ww_L2=(dcmplx(10d0)+denom*(dcmplx(15d0)+dcmplx(18d0)*denom))
     . /dcmplx(60d0)
      else
      ww_L2=(ww_Lnrat(x,y)-dcmplx(0.5d0*(r-1d0/r)))/dcmplx(denom)**3
      endif  
      return
      end

      double complex function ww_L0old(x,y)
      implicit none
      include 'src/Lib/ww/constants.h' 
      double complex ww_Lnrat
      double precision x,y,denom
      denom=one-x/y
      ww_L0old=ww_Lnrat(x,y)/dcmplx(denom)
      return
      end


      double complex function ww_L1old(x,y)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision x,y,denom
      double complex ww_L0old
      denom=one-x/y
      ww_L1old=(ww_L0old(x,y)+cone)/dcmplx(denom)
      return
      end

      double complex function ww_Ls0(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision x1,x2,y1,y2,r1,r2
      double complex ww_Lsm1
      r1=x1/y1
      r2=x2/y2
      ww_Ls0=ww_Lsm1(x1,y1,x2,y2)/dcmplx(one-r1-r2)
      return
      end

      double complex function ww_Ls1(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision x1,x2,y1,y2,r1,r2
      double complex ww_Ls0,ww_L0
      r1=x1/y1
      r2=x2/y2
      ww_Ls1=(ww_Ls0(x1,y1,x2,y2)+ww_L0(x1,y1)+ww_L0(x2,y2))/dcmplx(one-r1-r2)
      return
      end



      double complex function ww_Lsm1(x1,y1,x2,y2)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision x1,x2,y1,y2,r1,r2,omr1,omr2,ww_DDILOG
      double complex dilog1,dilog2,ww_Lnrat
      r1=x1/y1
      r2=x2/y2
      omr1=one-r1
      omr2=one-r2
      if (omr1 .gt. one) then 
         dilog1=dcmplx(pisqo6-ww_DDILOG(r1))-ww_Lnrat(x1,y1)*dcmplx(log(omr1))
      else
         dilog1=dcmplx(ww_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
         dilog2=dcmplx(pisqo6-ww_DDILOG(r2))-ww_Lnrat(x2,y2)*dcmplx(log(omr2))
      else
          dilog2=dcmplx(ww_DDILOG(omr2))
      endif
      ww_Lsm1=dilog1+dilog2+ww_Lnrat(x1,y1)*ww_Lnrat(x2,y2)-dcmplx(pisqo6)
      return
      end

      double complex function ww_Lsm1_2mh(s,ww_t,m1sq,m2sq)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision s,ww_t,m1sq,m2sq
      double complex ww_Lsm1_2mht,ww_I3m
      ww_Lsm1_2mh=ww_Lsm1_2mht(s,ww_t,m1sq,m2sq)
     & +(half*(s-m1sq-m2sq)+m1sq*m2sq/ww_t)*ww_I3m(s,m1sq,m2sq)
      return
      end

      double complex function ww_Lsm1_2mht(s,ww_t,m1sq,m2sq)
      implicit none
      include 'src/Lib/ww/constants.h'
      double precision s,ww_t,m1sq,m2sq,ww_DDILOG,r1,r2,omr1,omr2
      double complex ww_Lnrat,dilog1,dilog2
      r1=m1sq/ww_t
      r2=m2sq/ww_t
      omr1=one-r1
      omr2=one-r2

      if (omr1 .gt. one) then 
      dilog1=dcmplx(pisqo6-ww_DDILOG(r1))-ww_Lnrat(-m1sq,-ww_t)*dcmplx(log(omr1))
      else
      dilog1=dcmplx(ww_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
      dilog2=dcmplx(pisqo6-ww_DDILOG(r2))-ww_Lnrat(-m2sq,-ww_t)*dcmplx(log(omr2))
      else
      dilog2=dcmplx(ww_DDILOG(omr2))
      endif
      ww_Lsm1_2mht=-dilog1-dilog2
     & +half*(ww_Lnrat(-s,-m1sq)*ww_Lnrat(-s,-m2sq)-ww_Lnrat(-s,-ww_t)**2)
      return
      end


      double complex function ww_Lsm1_2me(s,ww_t,m1sq,m3sq)
      implicit none
c---- formula taken from G.~Duplancic and B~Nizic [arXiv:hep-ph/0006249 v2]
c---- Eq 71
c---- ww_Lsm1_2me notation follows from 
c----  Z.~Bern, L.~J.~Dixon and D.~A.~Kosower,
c----  %``Dimensionally regulated pentagon integrals,''
c----  Nucl.\ Phys.\ B {\bf 412}, 751 (1994)
c----  [arXiv:hep-ph/9306240].
c----  %%CITATION = HEP-PH 9306240;%%
c----  Eqs. (I.13)
C---- analytic continuation has been checked by calculating numerically.
      include 'src/Lib/ww/constants.h'
      integer j
      double precision s,ww_t,m1sq,m3sq,ww_DDILOG,arg(4),omarg(4),f2me,htheta
      double complex Li2(4),wlog(4)
C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
      htheta(s)=half+half*sign(one,s)

      f2me=(s+ww_t-m1sq-m3sq)/(s*ww_t-m1sq*m3sq)
     
      arg(1)=f2me*s
      arg(2)=f2me*ww_t
      arg(3)=f2me*m1sq
      arg(4)=f2me*m3sq

      do j=1,4
         omarg(j)=one-arg(j)
         wlog(j)=log(abs(arg(j)))    
     .     +impi*dcmplx(htheta(-arg(j))*sign(one,f2me))
         if (omarg(j) .gt. one) then 
             Li2(j)=dcmplx(pisqo6-ww_DDILOG(arg(j)))
     .       -wlog(j)*dcmplx(log(omarg(j)))
          else
             Li2(j)=dcmplx(ww_DDILOG(omarg(j)))
         endif
      enddo
      ww_Lsm1_2me=Li2(1)+Li2(2)-Li2(3)-Li2(4)

      return
      end

