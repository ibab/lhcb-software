      double precision function wz_t(j1,j2,j3)  
      implicit none
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/sprods_com.h'
      integer j1,j2,j3
      wz_t=s(j1,j2)+s(j2,j3)+s(j1,j3)
      end
