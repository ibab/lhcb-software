      double complex function wz_A7trees(j1,j2,j3,j4,j5,j6,j7,za,zb)
      implicit none
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/zprods_decl.h'
c---  DKS Eq. 3.15
      integer j1,j2,j3,j4,j5,j6,j7
      double complex wz_A7treea
     
      wz_A7trees=wz_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)
     .       +wz_A7treea(j1,j2,j6,j5,j4,j3,j7,za,zb)
        
      return
      end
        
      double complex function wz_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)
C---This function taken from Eq. 2.22 of DKS
c---Multiplied by a factor of (-i)
C---in the natural way this function refers to the process
c---u(2)+dbar(1)-->nu(3)+e^+(3)+b(5)+bbar(6)+g(7)
      implicit none
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2
      double precision t134,t256
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
      t134=s(j1,j3)+s(j1,j4)+s(j3,j4)
      t256=s(j2,j5)+s(j2,j6)+s(j5,j6)
      wz_A7treea=za(j1,j3)
     . *(za(j1,j3)*zb(j3,j4)*zb(j2,j5)*z2(j6,j2,j5,j7)/t256
     . +z2(j6,j1,j3,j4)*z2(j1,j2,j7,j5)/za(j7,j2))
     . /(za(j1,j7)*s(j3,j4)*s(j5,j6)*t134)
      return 
      end


c      double complex function A7treeb(j1,j2,j3,j4,j5,j6,j7,za,zb)
C---This function taken from Eq. 2.23 of DKS
c---Multiplied by a factor of (-i)
C---in the natural way this function refers to the process
c---u(2)+dbar(1)-->nu(3)+e^+(3)+b(5)+bbar(6)+g(7)
c      implicit none
c      include 'src/Lib/wz/constants.h'
c      include 'src/Lib/wz/sprods_com.h'
c      include 'src/Lib/wz/zprods_decl.h'
c      integer j1,j2,j3,j4,j5,j6,j7
c      integer i1,i2,i3,i4
c      double complex z2
c      double precision t127
c      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
c      t127=s(j1,j2)+s(j1,j7)+s(j2,j7)
c      A7treeb=(-za(j3,j6)*zb(j4,j5)
c     . *(z2(j1,j5,j6,j2)*za(j2,j1)+z2(j1,j5,j6,j7)*za(j7,j1))
c     . +za(j1,j3)*z2(j1,j2,j7,j4)*z2(j6,j3,j4,j5)
c     . -za(j1,j6)*z2(j1,j2,j7,j5)*z2(j3,j5,j6,j4))
c     . /(za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
c      return 
c      end
