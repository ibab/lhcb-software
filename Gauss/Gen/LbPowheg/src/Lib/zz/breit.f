      subroutine zz_breitw(x1,mminsq,mmaxsq,rmass,rwidth,msq,wt)       
      implicit none
c---- Given a number 0<x<1 generate a mass-squared msq and a weight wt 
c---- such that mminsq<msq<mmaxsq
c---- points are generated around resonance position rmass, but 
c---- breit-wigner should still be included in the matrix element
c     wt is the jacobian between integration in msq and integration in x1
      double precision x1,mminsq,mmaxsq,rmass,rwidth,msq,wt
      double precision almin,almax,al,tanal
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/vvsettings.h' 

c--- in case the maximum msq is very small, just generate linearly for safety
      if (.not.zerowidth.and.mmaxsq .lt. rmass*1d-3) then
        msq=mminsq+x1*(mmaxsq-mminsq)
        wt=mmaxsq-mminsq
        return
      endif

      if (zerowidth) then
          tanal=0d0
          almax=+pi/2d0 
          almin=-pi/2d0 
      else
          almin=datan((mminsq-rmass**2)/rmass/rwidth)
          almax=datan((mmaxsq-rmass**2)/rmass/rwidth)
          al=(almax-almin)*x1+almin
          tanal=dtan(al)
      endif

      msq=rmass**2+rmass*rwidth*tanal
c---- bw=(1d0+tanal**2)*rmass**2*rwidth**2
      wt=(almax-almin)*rmass*rwidth*(1d0+tanal**2)
      return
      end

c This does importance sampling according to a Lorentian.
c We use the integration variable
c           msq - M2                 1               1  
c al = atan -------- ; d al = -------------------- ------ d msq
c            M Gam            1+(msq-M2)^2/(M Gam)  M Gam
c
c so
c     d msq = M Gam (1+((msq-M2)^2/(M Gam))^2) = M Gam (1+tan(al)^2)
c and wt is the required Jacobian between x and msq.
c
c Notice that (1+((msq-M2)^2/(M Gam))^2) = ((msq-M2)^2 + (M Gam)^2)/(M Gam)^2
c so that the Jacobian cancels the square of the complex propagator in the
c amplitude.
c
c The special
c case zerowidth works by magic; in the zerowidth limit, we have
c              1                            pi  
c --------------------------------- ==> ------------ delta(msq-rmass^2)
c (msq-rmass^2)^2+(rmass*rwidth)**2     rmass*rwidth
c
c Thus we should fix msq to rmass^2, and replace the squared complex
c propagator with pi/(rmass*rwidth). But when msr=rmass^2, the squared
c complex propagator yields 1/(rmass*rwidth)**2. So we need to supply
c the factor pi*(rmass*rwidth) to reach the desired pi/(rmass*rwidth)
c factor; and in fact wt=pi*rmass*rwidth in the zerowidth limit.
