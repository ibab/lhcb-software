      subroutine wz_couplz(sin2xw)
      implicit none       
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/zwcouple.h'
      include 'src/Lib/wz/ewcharge.h'
c---calculate the couplings as given in Kunszt and Gunion
c---Modified to notation of DKS (ie divided by 2*sw*cw)
c---xw=sin^2 theta_w
      integer j
      double precision sin2xw

      sin2w=two*sqrt(sin2xw*(1d0-sin2xw))
      do j=1,5   !-- 5 is nf
      l(j)=(tau(j)-two*Q(j)*sin2xw)/sin2w
      r(j)=      (-two*Q(j)*sin2xw)/sin2w
      enddo

      le=(-1d0-two*(-1d0)*sin2xw)/sin2w
      re=(-two*(-1d0)*sin2xw)/sin2w

      ln=(+1d0-two*(+0d0)*sin2xw)/sin2w
      rn=0d0

      return
      end
