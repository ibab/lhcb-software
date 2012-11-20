      subroutine ww_a6treeb_anom(j1,j2,j3,j4,j5,j6,za,zb,a6b_1,a6b_2,a6b_3)
      implicit none
c---  tree-level amplitude for WW
c---  hep-ph/9907305 Eq. 10 (multiplied by a factor of (-i))
c---  Note that this reduces to the SM result (ww_a6treeb)
c---  when delg1=0, delk=0, lambda=0
      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      integer i1,i2,i3,i4
      double complex z2,a6b_1,a6b_2,a6b_3

c--- statement function
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
 
c--- a6b_1 should be dressed with coupling 
c---  (2d0+delg1_v+delk_v+lambda_v)
      a6b_1=(za(j1,j3)*zb(j2,j4)*z2(j6,j1,j2,j5)
     .      +za(j1,j6)*zb(j2,j5)*z2(j3,j5,j6,j4))
     .      /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
    
c--- a6b_2 should be dressed with coupling 
c---  2d0*(1d0+delg1_v)
      a6b_2=z2(j1,j3,j4,j2)*za(j3,j6)*zb(j4,j5)
     .      /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
      
c--- a6b_3 should be dressed with coupling 
c---  lambda_v/wmass**2
      a6b_3=z2(j1,j3,j4,j2)*z2(j3,j1,j2,j5)*z2(j6,j1,j2,j4)
     .      /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
  
      return
      end


      subroutine ww_a7treeb_anom(j1,j2,j3,j4,j5,j6,j7,za,zb,
     . a7b_1,a7b_2,a7b_3)
c---  real amplitude for WW
c---  hep-ph/9907305 Eq. 12 (multiplied by a factor of (-i))
c---  Note that this reduces to the SM result (a7treeb)
c---  when delg1=0, delk=0, lambda=0
      implicit none

      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2,a7b_1,a7b_2,a7b_3
      double precision t127

c--- statement function
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)

      t127=s(j1,j2)+s(j1,j7)+s(j2,j7)

c--- a7b_1 should be dressed with coupling 
c---  (2d0+delg1_v+delk_v+lambda_v)
      a7b_1=(za(j1,j3)*z2(j1,j2,j7,j4)*z2(j6,j3,j4,j5)
     .      -za(j1,j6)*z2(j1,j2,j7,j5)*z2(j3,j5,j6,j4))
     .      /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
    
c--- a7b_2 should be dressed with coupling 
c---  2d0*(1d0+delg1_v)
      a7b_2=(z2(j1,j3,j4,j2)*za(j2,j1)+z2(j1,j3,j4,j7)*za(j7,j1))
     .      *za(j3,j6)*zb(j4,j5)
     .      /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
      
c--- a7b_3 should be dressed with coupling 
c---  lambda_v/wmass**2
      a7b_3=(z2(j1,j3,j4,j2)*za(j2,j1)+z2(j1,j3,j4,j7)*za(j7,j1))
     .      *z2(j3,j4,j6,j5)*z2(j6,j3,j5,j4)
     .      /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
  
      return
      end


      subroutine ww_a6treeb_anom_wz(j1,j2,j3,j4,j5,j6,za,zb,
     . a6b_1,a6b_2,a6b_3,a6b_4)
      implicit none
c---  tree-level amplitude for WZ
c---  hep-ph/9907305 Eq. 14 (multiplied by a factor of (-i))
c---  Note that this reduces to the SM result (ww_a6treeb)
c---  when delg1=0, delk=0, lambda=0
      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      integer i1,i2,i3,i4
      double complex z2,a6b_1,a6b_2,a6b_3,a6b_4

c--- statement function
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
 
c--- a6b_1 should be dressed with coupling 
c---  (2d0+xdelg1_v+xdelk_v+xlambda_v*s(j1,j2)/wmass**2)
      a6b_1=-za(j3,j6)*zb(j4,j5)*z2(j1,j5,j6,j2)
     .       /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
    
c--- a6b_2 should be dressed with coupling 
c---  (2d0+xdelg1_v+xdelk_v+xlambda_v)
      a6b_2=-za(j1,j6)*zb(j2,j5)*z2(j3,j1,j2,j4)
     .       /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
      
c--- a6b_3 should be dressed with coupling 
c---  2d0*(1d0+xdelg1_v) 
      a6b_3=-z2(j6,j3,j4,j5)*za(j1,j3)*zb(j2,j4)
     .       /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
  
c--- a6b_4 should be dressed with coupling 
c---  xlambda_v/wmass**2
      a6b_4=-z2(j6,j3,j4,j5)*z2(j3,j5,j6,j2)*z2(j1,j5,j6,j4)
     .       /(2d0*s(j1,j2)*s(j3,j4)*s(j5,j6))
  
      return
      end


      subroutine ww_a7treeb_anom_wz(j1,j2,j3,j4,j5,j6,j7,
     . za,zb,a7b_1,a7b_2,a7b_3,a7b_4)
      implicit none
c---  real amplitude for WZ
c---  hep-ph/9907305 Eq. 15 (multiplied by a factor of (-i))
c---  Note that this reduces to the SM result (a7treeb)
c---  when delg1=0, delk=0, lambda=0
      include 'src/Lib/ww/constants.h'
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      double complex z2,a7b_1,a7b_2,a7b_3,a7b_4
      double precision t127

c--- statement function
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
 
      t127=s(j1,j2)+s(j1,j7)+s(j2,j7)

c--- a7b_1 should be dressed with coupling 
c---  (2d0+xdelg1_v+xdelk_v+xlambda_v*s(j1,j2)/wmass**2)
      a7b_1=-za(j3,j6)*zb(j4,j5)*(z2(j1,j5,j6,j2)*za(j2,j1)
     .                       +z2(j1,j5,j6,j7)*za(j7,j1))
     .       /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
    
c--- a7b_2 should be dressed with coupling 
c---  (2d0+xdelg1_v+xdelk_v+xlambda_v)
      a7b_2=-za(j1,j6)*z2(j1,j2,j7,j5)*z2(j3,j5,j6,j4)
     .       /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
      
c--- a7b_3 should be dressed with coupling 
c---  2d0*(1d0+xdelg1_v) 
      a7b_3=z2(j6,j3,j4,j5)*za(j1,j3)*z2(j1,j2,j7,j4)
     .       /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
  
c--- a7b_4 should be dressed with coupling 
c---  xlambda_v/wmass**2
      a7b_4=-z2(j6,j3,j4,j5)*(z2(j3,j5,j6,j2)*za(j2,j1)
     .                   +z2(j3,j5,j6,j7)*za(j7,j1))*z2(j1,j5,j6,j4)
     .       /(2d0*za(j1,j7)*za(j7,j2)*s(j3,j4)*s(j5,j6)*t127)
  
      return
      end




