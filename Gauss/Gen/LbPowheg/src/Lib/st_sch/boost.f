      subroutine st_sch_boost(beta,pin,pout)
      implicit none
      real * 8 beta(1:3),pin(0:3),pout(0:3)
      real * 8 vec(1:3),b,bsq,gamma,bdv,pin0
      integer j
c
      vec(1) = pin(1)
      vec(2) = pin(2)
      vec(3) = pin(3)
c beta value
      bsq = beta(1)**2+beta(2)**2+beta(3)**2
      b = sqrt(bsq)
      if (b**2.ge.1d0) then
         write(*,*) 'ERROR: st_sch_boost vector has st_sch_norm bigger than 1'
         write(*,*) 'RETURN 0'
         pout(0) = 0d0
         pout(1) = 0d0
         pout(2) = 0d0
         pout(3) = 0d0
         RETURN
      elseif (b.eq.0d0) then
         pout(0) = pin(0)
         pout(1) = pin(1)
         pout(2) = pin(2)
         pout(3) = pin(3)
         RETURN
      endif
      
      gamma = 1d0/sqrt(1d0-bsq)
c beta . vec
      bdv = beta(1)*vec(1)+beta(2)*vec(2)+beta(3)*vec(3)
c st_sch_boost
      do j=1,3
         vec(j) = vec(j)+gamma*beta(j)/b*(bdv/b*(1-1/gamma)+b*pin(0))
c         vec(j) = gamma*beta(j)*pin(0) + vec(j) + 
c     #        beta(j)*bdv*(gamma-1d0)/vsq
      enddo
      pin0 = gamma*(pin(0)+bdv)
c return st_sch_values
      pout(0) = pin0
      pout(1) = vec(1)
      pout(2) = vec(2)
      pout(3) = vec(3)
      end


c     To avoid problems with MC@NLO when using the same st_sch_analysis subroutine
      subroutine st_sch_boost_pow(beta,pin,pout)
      implicit none
      real * 8 beta(1:3),pin(0:3),pout(0:3)

      call st_sch_boost(beta,pin,pout)
      end
