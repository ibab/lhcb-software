      double complex function zz_Lnrat(x,y)
************************************************************************
*     Author: R.K. Ellis                                               *
*     August, 1998.                                                    *
c     zz_Lnrat(x,y)=log(x-i*ep)-log(y-i*ep)                               *
c     this function is hard-wired for sign of epsilon we must adjust   *
c     sign of x and y to get the right sign for epsilon                *
************************************************************************
      implicit none
!      include 'src/Lib/zz/constants.f'
      !TM added
      include 'src/Lib/zz/pwhg_math.h'
      double precision half,one
      double complex impi
      parameter (half=1d0/2d0,one=1d0,impi=(0d0,pi))
      double precision x,y,htheta
C--- define Heaviside theta function (=1 for x>0) and (0 for x < 0)
      htheta(x)=half+half*sign(one,x)
      zz_Lnrat=dcmplx(dlog(abs(x/y)))-impi*dcmplx((htheta(-x)-htheta(-y)))
      return
      end

