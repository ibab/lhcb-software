      double complex function ww_fa(j1,j2,j3,j4,j5,j6,za,zb)
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      integer i1,i2,i3,i4
      double complex ww_Lsm1_2mht,z2,ww_BigT,ww_I3m,ww_flipbit

      double precision t134,t234,del3,del12
  
C---statement function  
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)

c     Taken from Eq.(2.14)
      t134=s(j1,j3)+s(j1,j4)+s(j3,j4)
      t234=s(j2,j3)+s(j2,j4)+s(j3,j4)
      del12=s(j1,j2)-s(j3,j4)-s(j5,j6)
      del3=s(j1,j2)**2+s(j3,j4)**2+s(j5,j6)**2
     . -2d0*(s(j1,j2)*s(j3,j4)+s(j1,j2)*s(j5,j6)+s(j3,j4)*s(j5,j6))

      ww_fa=(za(j1,j3)**2*zb(j2,j5)**2
     . /(za(j3,j4)*zb(j5,j6)*t134*(z2(j1,j5,j6,j2)))
     . -(z2(j2,j5,j6,j4)*z2(j6,j2,j5,j1))**2
     . /(zb(j3,j4)*za(j5,j6)*t134*(z2(j2,j5,j6,j1)**3)))
     . *ww_Lsm1_2mht(s(j1,j2),t134,s(j3,j4),s(j5,j6))

      ww_fa=ww_fa+ww_flipbit(j1,j2,j3,j4,j5,j6,za,zb)
     .     -ww_flipbit(j2,j1,j5,j6,j3,j4,zb,za)

      ww_fa=ww_fa
     . +ww_BigT(j1,j2,j3,j4,j5,j6,za,zb)*ww_I3m(s(j1,j2),s(j3,j4),s(j5,j6))
     
      ww_fa=ww_fa+0.5d0*(t234*del12+2d0*s(j3,j4)*s(j5,j6))
     . /(z2(j2,j5,j6,j1)*del3)
     . *(zb(j4,j5)**2/(zb(j3,j4)*zb(j5,j6))
     .  +za(j3,j6)**2/(za(j3,j4)*za(j5,j6)))
      ww_fa=ww_fa+za(j3,j6)*zb(j4,j5)*(t134-t234)/(z2(j2,j5,j6,j1)*del3)
     . -0.5d0*z2(j6,j2,j5,j4)**2
     . /(zb(j3,j4)*za(j5,j6)*t134*z2(j2,j5,j6,j1))
      return
      end



      double complex function ww_flipbit(j1,j2,j3,j4,j5,j6,za,zb)
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      integer i1,i2,i3,i4
      double complex ww_L0,ww_L1,ww_Lnrat,z2,ww_L34_12,fb

      double precision t134
  
      z2(i1,i2,i3,i4)=za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
      t134=s(j1,j3)+s(j1,j4)+s(j3,j4)

      fb=0.5d0*(za(j6,j1)*zb(j1,j4))**2*t134
     ./(zb(j3,j4)*za(j5,j6)*z2(j2,j5,j6,j1)*t134**2)*ww_L1(-s(j3,j4),-t134)

      fb=fb+2d0*za(j6,j1)*zb(j1,j4)*z2(j6,j2,j5,j4)*ww_L0(-t134,-s(j3,j4))
     ./(zb(j3,j4)*za(j5,j6)*z2(j2,j5,j6,j1)*s(j3,j4))

      fb=fb-za(j1,j6)*za(j2,j6)*zb(j1,j4)**2*t134*ww_L0(-t134,-s(j3,j4))
     ./(zb(j3,j4)*za(j5,j6)*z2(j2,j5,j6,j1)**2*s(j3,j4))

      fb=fb-0.5d0*za(j2,j6)*zb(j1,j4)*z2(j6,j2,j5,j4)
     .*(ww_Lnrat(-t134,-s(j3,j4))+ww_Lnrat(-s(j1,j2),-s(j3,j4)))
     ./(zb(j3,j4)*za(j5,j6)*z2(j2,j5,j6,j1)**2)
      
      fb=fb-0.75d0*z2(j6,j2,j5,j4)**2
     ./(zb(j3,j4)*za(j5,j6)*t134*z2(j2,j5,j6,j1))
     .*(ww_Lnrat(-t134,-s(j3,j4))+ww_Lnrat(-s(j1,j2),-s(j3,j4)))

      fb=fb+ww_L34_12(j1,j2,j3,j4,j5,j6,za,zb)*ww_Lnrat(-s(j3,j4),-s(j1,j2))
      ww_flipbit=fb
      return
      end

