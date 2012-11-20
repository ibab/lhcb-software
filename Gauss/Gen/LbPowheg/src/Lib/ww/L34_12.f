      double complex function ww_L34_12(j1,j2,j3,j4,j5,j6,za,zb)
      implicit none
c -- DKS eq. (2.15)      
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      integer i1,i2,i3,i4
      double complex z2,bit1,bit2

      double precision ww_t,t134,t234,del3,del12,del56
  
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)

      t134=ww_t(j1,j3,j4)
      t234=ww_t(j2,j3,j4)
      del12=s(j1,j2)-s(j3,j4)-s(j5,j6)
      del56=s(j5,j6)-s(j1,j2)-s(j3,j4)
      Del3=s(j1,j2)**2+s(j3,j4)**2+s(j5,j6)**2
     . -2d0*(s(j1,j2)*s(j3,j4)+s(j1,j2)*s(j5,j6)+s(j3,j4)*s(j5,j6))

      bit1=
     . +zb(j4,j1)*za(j1,j3)*zb(j3,j5)+zb(j4,j2)*za(j2,j3)*zb(j3,j5)
     . +zb(j4,j1)*za(j1,j4)*zb(j4,j5)+zb(j4,j2)*za(j2,j4)*zb(j4,j5)
      bit2=
     . +zb(j4,j5)*za(j5,j1)*zb(j1,j5)+zb(j4,j6)*za(j6,j1)*zb(j1,j5)
     . +zb(j4,j5)*za(j5,j2)*zb(j2,j5)+zb(j4,j6)*za(j6,j2)*zb(j2,j5)

c-----first line
      ww_L34_12=1.5d0*del56*(t134-t234)*z2(j3,j1,j2,j4)*z2(j6,j1,j2,j5)
     . /(z2(j2,j5,j6,j1)*Del3**2)
      ww_L34_12=ww_L34_12+1.5d0*za(j3,j6)*bit1/(z2(j2,j5,j6,j1)*Del3)

c----second line
      ww_L34_12=ww_L34_12+0.5d0*za(j3,j4)*zb(j4,j5)*bit2
     . /(zb(j5,j6)*z2(j2,j5,j6,j1)*del3)
      ww_L34_12=ww_L34_12+zb(j1,j4)*za(j2,j6)*t134
     . *(za(j3,j6)*del12-2d0*za(j3,j4)*zb(j4,j5)*za(j5,j6))
     . /(za(j5,j6)*z2(j2,j5,j6,j1)**2*del3)

c-----third line
      ww_L34_12=ww_L34_12+0.5d0*t134/(z2(j2,j5,j6,j1)*del3)
     . *( +za(j3,j4)*zb(j4,j5)**2/zb(j5,j6)
     .    +zb(j3,j4)*za(j3,j6)**2/za(j5,j6)-2d0*za(j3,j6)*zb(j4,j5)) 

c--line before penultimate line
      ww_L34_12=ww_L34_12
     . +(+z2(j3,j1,j4,j5)/zb(j5,j6)
     .   -za(j3,j4)*zb(j1,j4)*za(j2,j6)/z2(j2,j5,j6,j1))
     .  *(zb(j4,j5)*del12
     .  -2d0*zb(j4,j3)*za(j3,j6)*zb(j6,j5))/(z2(j2,j5,j6,j1)*Del3)

c--penultimate line
      ww_L34_12=ww_L34_12+4d0
     . *(za(j3,j4)*zb(j4,j5)*z2(j6,j1,j3,j4)
     .  +za(j6,j3)*zb(j3,j4)*z2(j3,j2,j4,j5))/(z2(j2,j5,j6,j1)*Del3)

c--last line
      ww_L34_12=ww_L34_12+2d0*del12/(z2(j2,j5,j6,j1)*Del3)
     . *(+zb(j4,j5)*z2(j3,j2,j4,j5)/zb(j5,j6)
     .   -za(j3,j6)*z2(j6,j1,j3,j4)/za(j5,j6))      

      return
      end


