      double complex function zz_a7treesra(j1,j2,j3,j4,j5,j6,j7,za,zb)
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/sprods_com.h'
      include 'src/Lib/zz/zprods_decl.h'
c     -- sing res real contributions      
      integer j1,j2,j3,j4,j5,j6,j7
      integer i1,i2,i3,i4
      integer k1,k2,k3,k4,k5, p,zz_L1,zz_L2,l3
      double complex z2, z3,iza
      double precision s17,s27,t456,t356,t127,s56,zab
      double complex gup56on4, gdo56on4,gup56on3,gdo56on3
      
      zab(zz_L1,zz_L2,l3) = za(zz_L1,zz_L2)*zb(zz_L2,l3)
      z2(i1,i2,i3,i4) = za(i1,i2)*zb(i2,i4)+za(i1,i3)*zb(i3,i4)
c    arbitrary momentum -- gauge choice
      p = j1

      s17 = s(j1,j7)
      s27 = s(j2,j7)
      s56 = s(j5,j6)
      t356 = s(j3,j5)+s(j3,j6)+s(j5,j6)
      t456 = s(j4,j5)+s(j4,j6)+s(j5,j6)
      t127 = s(j1,j2)+s(j1,j7)+s(j2,j7)

      gup56on4 = za(j1,j3)*zb(j7,j2)*zb(j4,j5)*
     .     (za(j6,j4)*z2(p,j2,j7,j4)
     .     +za(j6,j5)*z2(p,j2,j7,j5))/(s(j2,j7)*t456*za(p,j7))


      gdo56on4 = zb(j4,j5)*za(j1,p)*z2(j6,j4,j5,j2)*
     .     za(j3,j1)*zb(j1,j7)/(s(j1,j7)*t456)

      gup56on3 = -zb(j7,j2)*za(j6,j3)*z2(p,j2,j7,j4)*
     .     z2(j1,j3,j6,j5)/(t356*s(j2,j7)*za(p,j7))

      gdo56on3 = -zb(j2,j4)*za(j6,j3)*za(j1,p)*(zb(j5,j3)+zb(j5,j6))*
     .     (zb(j5,j3)*za(j3,j1)+zb(j5,j6)*za(j6,j1))*zb(j1,j7)
     .     /(s(j1,j7)*t356)


      zz_a7treesra = (gup56on4+gdo56on4+gup56on3+gdo56on3)

c --  overall propagator
      zz_a7treesra = zz_a7treesra/t127/s56
      
      return
      end
      


