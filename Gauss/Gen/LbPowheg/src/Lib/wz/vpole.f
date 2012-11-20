      double complex function wz_Vpole(sij)
      implicit none
c---  DKS Eq. 2.12
!      include 'src/Lib/wz/epinv.f'
!      include 'src/Lib/wz/epinv2.f'
!      include 'src/Lib/wz/scale.f'
      include 'src/Lib/wz/pwhg_st.h'
      !TM added these
      double precision musq,epinv,epinv2
      double precision sij
      double complex wz_Lnrat,xl12
        
      !TM set variables
      musq = st_muren2
      epinv = 0d0
      epinv2 = 0d0

      xl12=wz_Lnrat(-sij,musq)

      wz_Vpole=!-epinv*epinv2+epinv*(-1.5d0+xl12)
     .   -0.5d0*xl12**2+1.5d0*xl12-3.5d0

      return
      end
