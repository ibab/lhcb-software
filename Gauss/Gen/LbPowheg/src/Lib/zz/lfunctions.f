************************************************************************
*     Author: R.K. Ellis                                               *
*     July, 1998/July 2005/June 2007                                   *
************************************************************************

      double complex function zz_L0(x,y)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double complex zz_Lnrat
      double precision x,y,denom
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      zz_L0=-cone-dcmplx(denom*(half+denom/3d0))
      else
      zz_L0=zz_Lnrat(x,y)/dcmplx(denom)
      endif
      return
      end

      double complex function zz_L1(x,y)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision x,y,denom
      double complex zz_L0
      denom=one-x/y
      if (abs(denom) .lt. 1d-7) then
      zz_L1=-half*cone-dcmplx(denom/3d0*(one+0.75d0*denom))
      else
      zz_L1=(zz_L0(x,y)+cone)/dcmplx(denom)
      endif
      return
      end

      double complex function zz_L2(x,y)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double complex zz_Lnrat
      double precision x,y,r,denom
      r=x/y
      denom=one-r
      if (abs(denom) .lt. 1d-7) then
      zz_L2=(dcmplx(10d0)+denom*(dcmplx(15d0)+dcmplx(18d0)*denom))
     . /dcmplx(60d0)
      else
      zz_L2=(zz_Lnrat(x,y)-dcmplx(0.5d0*(r-1d0/r)))/dcmplx(denom)**3
      endif  
      return
      end

      double complex function zz_L0old(x,y)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double complex zz_Lnrat
      double precision x,y,denom
      denom=one-x/y
      zz_L0old=zz_Lnrat(x,y)/dcmplx(denom)
      return
      end


      double complex function zz_L1old(x,y)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision x,y,denom
      double complex zz_L0old
      denom=one-x/y
      zz_L1old=(zz_L0old(x,y)+cone)/dcmplx(denom)
      return
      end

      double complex function zz_Ls0(x1,y1,x2,y2)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision x1,x2,y1,y2,r1,r2
      double complex zz_Lsm1
      r1=x1/y1
      r2=x2/y2
      zz_Ls0=zz_Lsm1(x1,y1,x2,y2)/dcmplx(one-r1-r2)
      return
      end

      double complex function zz_Ls1(x1,y1,x2,y2)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision x1,x2,y1,y2,r1,r2
      double complex zz_Ls0,zz_L0
      r1=x1/y1
      r2=x2/y2
      zz_Ls1=(zz_Ls0(x1,y1,x2,y2)+zz_L0(x1,y1)+zz_L0(x2,y2))/dcmplx(one-r1-r2)
      return
      end



      double complex function zz_Lsm1(x1,y1,x2,y2)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision x1,x2,y1,y2,r1,r2,omr1,omr2,zz_DDILOG
      double complex dilog1,dilog2,zz_Lnrat
      r1=x1/y1
      r2=x2/y2
      omr1=one-r1
      omr2=one-r2
      if (omr1 .gt. one) then 
         dilog1=dcmplx(pisqo6-zz_DDILOG(r1))-zz_Lnrat(x1,y1)*dcmplx(log(omr1))
      else
         dilog1=dcmplx(zz_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
         dilog2=dcmplx(pisqo6-zz_DDILOG(r2))-zz_Lnrat(x2,y2)*dcmplx(log(omr2))
      else
          dilog2=dcmplx(zz_DDILOG(omr2))
      endif
      zz_Lsm1=dilog1+dilog2+zz_Lnrat(x1,y1)*zz_Lnrat(x2,y2)-dcmplx(pisqo6)
      return
      end

      double complex function zz_Lsm1_2mh(s,zz_t,m1sq,m2sq)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision s,zz_t,m1sq,m2sq
      double complex zz_Lsm1_2mht,zz_I3m
      zz_Lsm1_2mh=zz_Lsm1_2mht(s,zz_t,m1sq,m2sq)
     & +(half*(s-m1sq-m2sq)+m1sq*m2sq/zz_t)*zz_I3m(s,m1sq,m2sq)
      return
      end

      double complex function zz_Lsm1_2mht(s,zz_t,m1sq,m2sq)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      double precision s,zz_t,m1sq,m2sq,zz_DDILOG,r1,r2,omr1,omr2
      double complex zz_Lnrat,dilog1,dilog2
      r1=m1sq/zz_t
      r2=m2sq/zz_t
      omr1=one-r1
      omr2=one-r2

      if (omr1 .gt. one) then 
      dilog1=dcmplx(pisqo6-zz_DDILOG(r1))-zz_Lnrat(-m1sq,-zz_t)*dcmplx(log(omr1))
      else
      dilog1=dcmplx(zz_DDILOG(omr1))
      endif
      if (omr2 .gt. one) then 
      dilog2=dcmplx(pisqo6-zz_DDILOG(r2))-zz_Lnrat(-m2sq,-zz_t)*dcmplx(log(omr2))
      else
      dilog2=dcmplx(zz_DDILOG(omr2))
      endif
      zz_Lsm1_2mht=-dilog1-dilog2
     & +half*(zz_Lnrat(-s,-m1sq)*zz_Lnrat(-s,-m2sq)-zz_Lnrat(-s,-zz_t)**2)
      return
      end


      double complex function zz_Lsm1_2me(s,zz_t,m1sq,m3sq)
      implicit none
c---- formula taken from G.~Duplancic and B~Nizic [arXiv:hep-ph/0006249 v2]
c---- Eq 71
c---- zz_Lsm1_2me notation follows from 
c----  Z.~Bern, L.~J.~Dixon and D.~A.~Kosower,
c----  %``Dimensionally regulated pentagon integrals,''
c----  Nucl.\ Phys.\ B {\bf 412}, 751 (1994)
c----  [arXiv:hep-ph/9306240].
c----  %%CITATION = HEP-PH 9306240;%%
c----  Eqs. (I.13)
C---- analytic continuation has been checked by calculating numerically.
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/pwhg_math.h'
      double precision one,half,pisqo6
      double complex cone,impi
      parameter (one=1d0,half=1d0/2d0,pisqo6=pi**2/6d0,cone=(1d0,0d0),
     .           impi=(0d0,pi))
      integer j
      double precision s,zz_t,m1sq,m3sq,zz_DDILOG,arg(4),omarg(4),f2me,htheta
      double complex Li2(4),wlog(4)
C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
      htheta(s)=half+half*sign(one,s)

      f2me=(s+zz_t-m1sq-m3sq)/(s*zz_t-m1sq*m3sq)
     
      arg(1)=f2me*s
      arg(2)=f2me*zz_t
      arg(3)=f2me*m1sq
      arg(4)=f2me*m3sq

      do j=1,4
         omarg(j)=one-arg(j)
         wlog(j)=log(abs(arg(j)))    
     .     +impi*dcmplx(htheta(-arg(j))*sign(one,f2me))
         if (omarg(j) .gt. one) then 
             Li2(j)=dcmplx(pisqo6-zz_DDILOG(arg(j)))
     .       -wlog(j)*dcmplx(log(omarg(j)))
          else
             Li2(j)=dcmplx(zz_DDILOG(omarg(j)))
         endif
      enddo
      zz_Lsm1_2me=Li2(1)+Li2(2)-Li2(3)-Li2(4)

      return
      end

c      double complex function zz_Lsm1_2me(s,zz_t,m1sq,m3sq)
c      implicit none
c      include 'src/Lib/zz/constants.f'
c      integer j
c      double precision s,zz_t,m1sq,m3sq,zz_DDILOG,arg(5),omarg(5)
c      double complex zz_Lnrat,Li2(5),wlog(5)
c      arg(1)=m1sq/s
c      wlog(1)=zz_Lnrat(-m1sq,-s)

c      arg(2)=m1sq/zz_t
c      wlog(2)=zz_Lnrat(-m1sq,-zz_t)

c      arg(3)=m3sq/s
c      wlog(3)=zz_Lnrat(-m3sq,-s)

c      arg(4)=m3sq/zz_t
c      wlog(4)=zz_Lnrat(-m3sq,-zz_t)

c      arg(5)=arg(1)*arg(4)
c      wlog(5)=zz_Lnrat(-m1sq,-s)+zz_Lnrat(-m3sq,-zz_t)

c      do j=1,5
c         omarg(j)=one-arg(j)
c         if (omarg(j) .gt. one) then 
c             Li2(j)=dcmplx(pisqo6-zz_DDILOG(arg(j)))-wlog(j)*log(omarg(j))
c          else
c             Li2(j)=dcmplx(zz_DDILOG(omarg(j)))
c         endif
c      enddo
c      zz_Lsm1_2me=Li2(5)-Li2(1)-Li2(2)-Li2(3)-Li2(4)-half*zz_Lnrat(-s,-zz_t)**2
c      return
c      end

