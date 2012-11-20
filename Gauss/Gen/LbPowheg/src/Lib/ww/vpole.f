      double complex function ww_Vpole(sij)
      implicit none
c---  DKS Eq. 2.12

      include 'src/Lib/ww/pwhg_st.h'
      double precision musq,epinv,epinv2
      double precision sij
      double complex ww_Lnrat,xl12
        

      musq = st_muren2
      epinv=0d0
      epinv2=0d0
      
      epinv=1d0
      epinv2=1d0
      xl12=ww_Lnrat(-sij,musq)

c      ww_Vpole=-epinv*epinv2+epinv*(-1.5d0+xl12)
c     .   -0.5d0*xl12**2+1.5d0*xl12-3.5d0
      
      ww_Vpole = -0.5d0*xl12**2+1.5d0*xl12-3.5d0


      return
      end
