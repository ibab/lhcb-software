      double precision function ww_t(j1,j2,j3)  
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      integer j1,j2,j3
      ww_t=s(j1,j2)+s(j2,j3)+s(j1,j3)
      end
