      subroutine ww_boost(mass,p1,p_in,p_out)
c     take momenta p_in in frame in which one particle is at rest with mass "mass" 
c     and convert to frame in which the one particle has fourvector p1
      implicit none
      double precision mass,p1(4),p_in(4),p_out(4)
      double precision gam,beta(1:3),bdotp,one
      parameter(one=1d0)
      integer j,k
      gam=p1(4)/mass
      bdotp=0d0
      do j=1,3
      beta(j)=-p1(j)/p1(4)
      bdotp=bdotp+p_in(j)*beta(j)
      enddo
      p_out(4)=gam*(p_in(4)-bdotp)
      do k=1,3
      p_out(k)=p_in(k)+gam*beta(k)*(gam/(gam+one)*bdotp-p_in(4))
      enddo
      return
      end      

