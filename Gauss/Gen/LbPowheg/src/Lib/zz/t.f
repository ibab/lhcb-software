      double precision function zz_t(j1,j2,j3)  
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/sprods_com.h'
      integer j1,j2,j3
      zz_t=s(j1,j2)+s(j2,j3)+s(j1,j3)
      end
