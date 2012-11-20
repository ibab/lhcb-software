      double complex function zz_Vpole(sij)
      implicit none
c---  DKS Eq. 2.12
!      include 'src/Lib/zz/epinv.f'
!      include 'src/Lib/zz/epinv2.f'
!      include 'src/Lib/zz/scale.f'
      include 'src/Lib/zz/pwhg_st.h'
      !TM added these
      double precision musq,epinv,epinv2
      double precision sij
      double complex zz_Lnrat,xl12
        
      !TM set variables
      musq = st_muren2
      epinv = 0d0
      epinv2 = 0d0

      xl12=zz_Lnrat(-sij,musq)

      zz_Vpole=!-epinv*epinv2+epinv*(-1.5d0+xl12)
     .   -0.5d0*xl12**2+1.5d0*xl12-3.5d0

      return
      end
