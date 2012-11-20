      subroutine zz_zzamps(j1,j2,j3,j4,j5,j6,j7,za,zb,f)
c---labels on array f are polarizations of partons:
c--- 1=quark, 2=lepton1, 3=lepton2, 4=gluon

      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7,mplus,minus
      double complex zz_A7trees
      double complex f(2,2,2,2)
      data minus,mplus/1,2/

      f(minus,minus,minus,mplus)=+zz_A7trees(j1,j2,j3,j4,j5,j6,j7,za,zb)  
      f(minus,minus,mplus,mplus)=+zz_A7trees(j1,j2,j4,j3,j5,j6,j7,za,zb)  
      f(minus,mplus,minus,mplus)=+zz_A7trees(j1,j2,j3,j4,j6,j5,j7,za,zb)  
      f(minus,mplus,mplus,mplus)=+zz_A7trees(j1,j2,j4,j3,j6,j5,j7,za,zb)  

      f(mplus,minus,minus,mplus)=+zz_A7trees(j1,j2,j4,j3,j6,j5,j7,zb,za)  
      f(mplus,minus,mplus,mplus)=+zz_A7trees(j1,j2,j3,j4,j6,j5,j7,zb,za)  
      f(mplus,mplus,minus,mplus)=+zz_A7trees(j1,j2,j4,j3,j5,j6,j7,zb,za)  
      f(mplus,mplus,mplus,mplus)=+zz_A7trees(j1,j2,j3,j4,j5,j6,j7,zb,za)  

      f(minus,minus,minus,minus)=-zz_A7trees(j2,j1,j5,j6,j3,j4,j7,zb,za)  
      f(minus,minus,mplus,minus)=-zz_A7trees(j2,j1,j5,j6,j4,j3,j7,zb,za)  
      f(minus,mplus,minus,minus)=-zz_A7trees(j2,j1,j6,j5,j3,j4,j7,zb,za)  
      f(minus,mplus,mplus,minus)=-zz_A7trees(j2,j1,j6,j5,j4,j3,j7,zb,za)  

      f(mplus,minus,minus,minus)=-zz_A7trees(j2,j1,j6,j5,j4,j3,j7,za,zb)  
      f(mplus,minus,mplus,minus)=-zz_A7trees(j2,j1,j6,j5,j3,j4,j7,za,zb)  
      f(mplus,mplus,minus,minus)=-zz_A7trees(j2,j1,j5,j6,j4,j3,j7,za,zb)  
      f(mplus,mplus,mplus,minus)=-zz_A7trees(j2,j1,j5,j6,j3,j4,j7,za,zb)  

      return
      end
