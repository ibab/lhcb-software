      subroutine wz_wzamps(j1,j2,j3,j4,j5,j6,j7,za,zb,f)
c- first label of fs,ft,fu, is gluon polarization, second is zdecay line

      implicit none
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/zprods_decl.h'
      include 'src/Lib/wz/anomcoup.h'
      include 'src/Lib/wz/zwcouple.h'
      include 'src/Lib/wz/vvsettings.h'
      integer j1,j2,j3,j4,j5,j6,j7,mplus,minus,jtype,j,k
      double complex wz_A7treea,wz_B7treea,wz_B7treeb,A7b_1,A7b_2,A7b_3,A7b_4
      double complex f(10,2,2)
      double precision s127
      common/wz_xanomcoup/xdelg1_z,xdelg1_g,xlambda_g,xlambda_z,
     . xdelk_g,xdelk_z
      data minus,mplus/1,2/

c----initialize to zero
c--- added amplitude 10 on 4/25/2002, to include anomalous couplings
      do jtype=4,9
      do j=1,2
      do k=1,2
      f(jtype,j,k)=dcmplx(0d0)
      enddo
      enddo
      enddo
      
      s127=dble(za(1,7)*zb(7,1)+za(2,7)*zb(7,2)+za(1,2)*zb(2,1))

c      f(1,mplus,minus)=+wz_a7treeb_anom_wz(j1,j2,j3,j4,j5,j6,j7,za,zb) !fs
      call wz_a7treeb_anom_wz(j1,j2,j3,j4,j5,j6,j7,za,zb,
     . A7b_1,A7b_2,A7b_3,A7b_4)                                     !fs
      f(1,mplus,minus)=
     .           A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A7b_3*2d0*(1d0+xdelg1_z) 
     .          +A7b_4*xlambda_z/wmass**2
      f(10,mplus,minus)=
     .           A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A7b_3*2d0*(1d0+xdelg1_g) 
     .          +A7b_4*xlambda_g/wmass**2
      f(2,mplus,minus)=+wz_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)         !ft
      f(3,mplus,minus)=+wz_A7treea(j1,j2,j6,j5,j4,j3,j7,za,zb)         !fu

c      f(1,mplus,mplus)=+wz_a7treeb_anom_wz(j1,j2,j3,j4,j6,j5,j7,za,zb)
      call wz_a7treeb_anom_wz(j1,j2,j3,j4,j6,j5,j7,za,zb,
     . A7b_1,A7b_2,A7b_3,A7b_4)
      f(1,mplus,mplus)=
     .           A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A7b_3*2d0*(1d0+xdelg1_z) 
     .          +A7b_4*xlambda_z/wmass**2
      f(10,mplus,mplus)=
     .           A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A7b_3*2d0*(1d0+xdelg1_g) 
     .          +A7b_4*xlambda_g/wmass**2
      f(2,mplus,mplus)=+wz_A7treea(j1,j2,j3,j4,j6,j5,j7,za,zb)
      f(3,mplus,mplus)=+wz_A7treea(j1,j2,j5,j6,j4,j3,j7,za,zb)

c-- old version
c      f(1,minus,minus)=-wz_a7treeb_anom_wz(j2,j1,j5,j6,j3,j4,j7,zb,za)
c-- new version
c      f(1,minus,minus)=+wz_a7treeb_anom_wz(j2,j1,j4,j3,j6,j5,j7,zb,za)
      call wz_a7treeb_anom_wz(j2,j1,j4,j3,j6,j5,j7,zb,za,
     . A7b_1,A7b_2,A7b_3,A7b_4)
      f(1,minus,minus)=
     .           A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A7b_3*2d0*(1d0+xdelg1_z) 
     .          +A7b_4*xlambda_z/wmass**2
      f(10,minus,minus)=
     .           A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A7b_3*2d0*(1d0+xdelg1_g) 
     .          +A7b_4*xlambda_g/wmass**2
      f(2,minus,minus)=-wz_A7treea(j2,j1,j5,j6,j3,j4,j7,zb,za)
      f(3,minus,minus)=-wz_A7treea(j2,j1,j4,j3,j6,j5,j7,zb,za)

c-- old version
c      f(1,minus,mplus)=-wz_a7treeb_anom_wz(j2,j1,j6,j5,j3,j4,j7,zb,za)
c-- new version
c      f(1,minus,mplus)=+wz_a7treeb_anom_wz(j2,j1,j4,j3,j5,j6,j7,zb,za)
      call wz_a7treeb_anom_wz(j2,j1,j4,j3,j5,j6,j7,zb,za,
     . A7b_1,A7b_2,A7b_3,A7b_4)
      f(1,minus,mplus)=
     .           A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .          +A7b_3*2d0*(1d0+xdelg1_z) 
     .          +A7b_4*xlambda_z/wmass**2
      f(10,minus,mplus)=
     .           A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g*s127/wmass**2)
     .          +A7b_2*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .          +A7b_3*2d0*(1d0+xdelg1_g) 
     .          +A7b_4*xlambda_g/wmass**2
      f(2,minus,mplus)=-wz_A7treea(j2,j1,j6,j5,j3,j4,j7,zb,za)
      f(3,minus,mplus)=-wz_A7treea(j2,j1,j4,j3,j5,j6,j7,zb,za)
            

      if (dronly) return
      
c+++ the extra amplitude 7 accounts for W+ production instead of W-
c+++ amplitudes 8 and 9 are needed when considering ub+d instead of
c+++ d+ub and are obtained from 6 and 7 by simply swapping 1 <+> 2

c+++ further review (12/05/01) shows that 8 and 9 are not needed,
c+++ hence they are now commented out

      f(4,mplus,minus)=+wz_B7treeb(j2,j1,j6,j5,j4,j3,j7,za,zb)    
      f(5,mplus,minus)=+wz_B7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)    
      f(6,mplus,minus)=+wz_B7treeb(j2,j1,j3,j4,j5,j6,j7,za,zb)    
      f(7,mplus,minus)=+wz_B7treea(j1,j2,j6,j5,j4,j3,j7,za,zb)    
c      f(8,mplus,minus)=+wz_B7treeb(j1,j2,j3,j4,j5,j6,j7,za,zb)    
c      f(9,mplus,minus)=+wz_B7treea(j2,j1,j6,j5,j4,j3,j7,za,zb)    

      f(4,mplus,mplus)=+wz_B7treeb(j2,j1,j5,j6,j4,j3,j7,za,zb)
      f(5,mplus,mplus)=+wz_B7treea(j1,j2,j3,j4,j6,j5,j7,za,zb)
         
      f(4,minus,minus)=-wz_B7treea(j2,j1,j4,j3,j6,j5,j7,zb,za)
      f(5,minus,minus)=-wz_B7treeb(j1,j2,j5,j6,j3,j4,j7,zb,za)
      f(6,minus,minus)=-wz_B7treea(j2,j1,j5,j6,j3,j4,j7,zb,za)
      f(7,minus,minus)=-wz_B7treeb(j1,j2,j4,j3,j6,j5,j7,zb,za)
c      f(8,minus,minus)=-wz_B7treea(j1,j2,j5,j6,j3,j4,j7,zb,za)
c      f(9,minus,minus)=-wz_B7treeb(j2,j1,j4,j3,j6,j5,j7,zb,za)

      f(4,minus,mplus)=-wz_B7treea(j2,j1,j3,j4,j6,j5,j7,zb,za)
      f(5,minus,mplus)=-wz_B7treeb(j1,j2,j5,j6,j4,j3,j7,zb,za)
      f(4,minus,mplus)=-wz_B7treea(j2,j1,j4,j3,j5,j6,j7,zb,za)
      f(5,minus,mplus)=-wz_B7treeb(j1,j2,j6,j5,j3,j4,j7,zb,za)
     
      return
      end
