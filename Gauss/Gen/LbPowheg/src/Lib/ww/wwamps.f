      subroutine ww_wwamps(j1,j2,j3,j4,j5,j6,j7,za,zb,f)
c  -first label of fs,ft is gluon polarization, second is qqb line
      implicit none
      include 'src/Lib/ww/constants.h' 
      include 'src/Lib/ww/sprods_com.h'
      include 'src/Lib/ww/zprods_decl.h'
      include 'src/Lib/ww/zcouple.h'
      include 'src/Lib/ww/wcouple.h'
      include 'src/Lib/ww/anomcoup.h'
      include 'src/Lib/ww/vvsettings.h'
      integer j,k,jtype,j1,j2,j3,j4,j5,j6,j7,mplus,minus
      double complex ww_A7treea,ww_B7treea,ww_B7treeb
      double complex f(5,2,2),A7b_1,A7b_2,A7b_3
      double complex prop34,prop56,propboth
      common/ww_xanomcoup/xdelg1_z,xdelg1_g,xlambda_g,xlambda_z,
     & xdelk_g,xdelk_z
      data minus,mplus/1,2/
      double complex ww_cpropfac
      external ww_cpropfac
      
c----initialize to zero
      do jtype=3,4
      do j=1,2
      do k=1,2
      f(jtype,j,k)=czip
      enddo
      enddo
      enddo


c      prop34=dcmplx(s(3,4))/dcmplx(s(3,4)-wmass**2,wmass*wwidth)
c      prop56=dcmplx(s(5,6))/dcmplx(s(5,6)-wmass**2,wmass*wwidth)
      prop34=ww_cpropfac(s(3,4),wmass,wwidth)
      prop56=ww_cpropfac(s(5,6),wmass,wwidth)

      propboth=prop34*prop56
      
      f(1,mplus,mplus)= czip
c     f(2,mplus,mplus)=-A7treeb(j2,j1,j3,j4,j5,j6,j7,za,zb)*propboth
      call ww_a7treeb_anom(j2,j1,j3,j4,j5,j6,j7,za,zb,A7b_1,A7b_2,A7b_3)
      f(2,mplus,mplus)=-(A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .                 +A7b_2*(2d0*(1d0+xdelg1_z))
     .                 +A7b_3*(xlambda_z/wmass**2))*propboth
      f(5,mplus,mplus)=-(A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .                 +A7b_2*(2d0*(1d0+xdelg1_g))
     .                 +A7b_3*(xlambda_g/wmass**2))*propboth

      f(1,mplus,minus)=+ww_A7treea(j1,j2,j3,j4,j5,j6,j7,za,zb)*propboth      
c      f(2,mplus,minus)=+A7treeb(j1,j2,j3,j4,j5,j6,j7,za,zb)*propboth
      call ww_a7treeb_anom(j1,j2,j3,j4,j5,j6,j7,za,zb,A7b_1,A7b_2,A7b_3)
      f(2,mplus,minus)=(A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .                 +A7b_2*(2d0*(1d0+xdelg1_z))
     .                 +A7b_3*(xlambda_z/wmass**2))*propboth
      f(5,mplus,minus)=(A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .                 +A7b_2*(2d0*(1d0+xdelg1_g))
     .                 +A7b_3*(xlambda_g/wmass**2))*propboth

      f(1,minus,mplus)= czip
c      f(2,minus,mplus)=+A7treeb(j1,j2,j5,j6,j3,j4,j7,zb,za)*propboth
      call ww_a7treeb_anom(j1,j2,j5,j6,j3,j4,j7,zb,za,A7b_1,A7b_2,A7b_3)
      f(2,minus,mplus)=(A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .                 +A7b_2*(2d0*(1d0+xdelg1_z))
     .                 +A7b_3*(xlambda_z/wmass**2))*propboth
      f(5,minus,mplus)=(A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .                 +A7b_2*(2d0*(1d0+xdelg1_g))
     .                 +A7b_3*(xlambda_g/wmass**2))*propboth

      f(1,minus,minus)=-ww_A7treea(j2,j1,j5,j6,j3,j4,j7,zb,za)*propboth
c      f(2,minus,minus)=-A7treeb(j2,j1,j5,j6,j3,j4,j7,zb,za)*propboth
      call ww_a7treeb_anom(j2,j1,j5,j6,j3,j4,j7,zb,za,A7b_1,A7b_2,A7b_3)
      f(2,minus,minus)=-(A7b_1*(2d0+xdelg1_z+xdelk_z+xlambda_z)
     .                 +A7b_2*(2d0*(1d0+xdelg1_z))
     .                 +A7b_3*(xlambda_z/wmass**2))*propboth
      f(5,minus,minus)=-(A7b_1*(2d0+xdelg1_g+xdelk_g+xlambda_g)
     .                 +A7b_2*(2d0*(1d0+xdelg1_g))
     .                 +A7b_3*(xlambda_g/wmass**2))*propboth

      if (dronly .eqv. .true.) return  
c---  Done all non-singly resonant amplitudes
      
c---  also return here for WW+jet process since no singly-resonant
c---  diagrams are included in the real contribution
c---  WW+jet not in POWHEG
c     if (case .eq. 'WW_jet') return    
      
      f(3,mplus,mplus)=-ww_B7treeb(j1,j2,3,4,5,6,j7,za,zb)*prop34
     .     -ww_B7treea(j2,j1,3,4,5,6,j7,za,zb)*prop56
      f(4,mplus,mplus)=-ww_B7treea(j2,j1,6,5,4,3,j7,za,zb)*prop34
     .     -ww_B7treeb(j1,j2,6,5,4,3,j7,za,zb)*prop56
      
      f(3,mplus,minus)=+ww_B7treeb(j2,j1,3,4,5,6,j7,za,zb)*prop34
     .     +ww_B7treea(j1,j2,3,4,5,6,j7,za,zb)*prop56
      f(4,mplus,minus)=+ww_B7treea(j1,j2,6,5,4,3,j7,za,zb)*prop34
     .     +ww_B7treeb(j2,j1,6,5,4,3,j7,za,zb)*prop56
      
      f(3,minus,mplus)=+ww_B7treea(j1,j2,5,6,3,4,j7,zb,za)*prop34
     .     +ww_B7treeb(j2,j1,5,6,3,4,j7,zb,za)*prop56
      f(4,minus,mplus)=+ww_B7treeb(j2,j1,4,3,6,5,j7,zb,za)*prop34
     .     +ww_B7treea(j1,j2,4,3,6,5,j7,zb,za)*prop56
      
      f(3,minus,minus)=-ww_B7treea(j2,j1,5,6,3,4,j7,zb,za)*prop34
     .     -ww_B7treeb(j1,j2,5,6,3,4,j7,zb,za)*prop56
      f(4,minus,minus)=-ww_B7treeb(j1,j2,4,3,6,5,j7,zb,za)*prop34
     .     -ww_B7treea(j2,j1,4,3,6,5,j7,zb,za)*prop56
      
      return
      end

