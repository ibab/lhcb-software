      double complex function wz_B7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)
C---This function in the notation of DKS
c---Multiplied by a factor of (-i)
C---in the natural way this function refers to the process
c---u(2)+dbar(1)-->nu(3)+e^+(3)+b(5)+bbar(6)+g(7)
      implicit none
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2
      double precision s127,s356
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
      s127=s(j1,j2)+s(j1,j7)+s(j2,j7)
      s356=s(j3,j5)+s(j3,j6)+s(j5,j6)
      wz_B7treea=z2(j1,j2,j7,j4)*z2(j1,j3,j6,j5)*za(j3,j6)
     . /(za(j1,j7)*za(j2,j7)*dcmplx(s(j5,j6)*s127*s356))
      return 
      end


      double complex function wz_B7treeb(j1,j2,j3,j4,j5,j6,j7,za,zb)
C---This function in the notation of DKS
c---Multiplied by a factor of (-i)
C---in the natural way this function refers to the process
c---u(2)+dbar(1)-->nu(3)+e^+(3)+b(5)+bbar(6)+g(7)
      implicit none
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2
      double precision s127,s345
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
      s127=s(j1,j2)+s(j1,j7)+s(j2,j7)
      s345=s(j3,j4)+s(j4,j5)+s(j3,j5)

      wz_B7treeb=-za(j2,j6)*zb(j4,j5)
     . *(za(j1,j2)*z2(j3,j4,j5,j1)-za(j2,j7)*z2(j3,j4,j5,j7))
     . /(za(j1,j7)*za(j2,j7)*dcmplx(s(j3,j4)*s127*s345))
      return 
      end
