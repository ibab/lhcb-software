      double complex function ww_A7trees(j1,j2,j3,j4,j5,j6,j7,za,zb)
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/zprods_decl.h'
c---  DKS Eq. 3.15
      integer j1,j2,j3,j4,j5,j6,j7
      double complex ww_A7treea
     
      ww_A7trees=ww_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)
     .       +ww_A7treea(j1,j2,j6,j5,j4,j3,j7,za,zb)
        
      return
      end
        
      double complex function ww_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)
C---This function taken from Eq. 2.22 of DKS
c---Multiplied by a factor of (-i)
C---in the natural way this function refers to the process
c---u(2)+dbar(1)-->nu(3)+e^+(3)+b(5)+bbar(6)+g(7)
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2
      double precision t134,t256
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
      t134=s(j1,j3)+s(j1,j4)+s(j3,j4)
      t256=s(j2,j5)+s(j2,j6)+s(j5,j6)
      ww_A7treea=za(j1,j3)
     . *(za(j1,j3)*zb(j3,j4)*zb(j2,j5)*z2(j6,j2,j5,j7)/t256
     . +z2(j6,j1,j3,j4)*z2(j1,j2,j7,j5)/za(j7,j2))
     . /(za(j1,j7)*s(j3,j4)*s(j5,j6)*t134)
      return 
      end
