      double complex function wz_a6loops(j1,j2,j3,j4,j5,j6,za,zb)
      implicit none
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/zprods_decl.h'
c---  DKS Eq. 3.15
      integer j1,j2,j3,j4,j5,j6
      double complex wz_a6loopa
      wz_a6loops=wz_a6loopa(j1,j2,j3,j4,j5,j6,za,zb)
     .       +wz_a6loopa(j1,j2,j6,j5,j4,j3,za,zb)
      return
      end
        
      double complex function wz_a6loopa(j1,j2,j3,j4,j5,j6,za,zb)
      implicit none
c---  DKS Eq. 2.10 for alpha = a
c---  note that (-i) included in A(alpha),A(tree,alpha)
c---  so no factor of (+i) in front of F(alpha)
!      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/sprods_com.h'
      include 'src/Lib/wz/zprods_decl.h'
      integer j1,j2,j3,j4,j5,j6
      double complex tree,wz_Vpole,wz_a6treea,wz_fa
      tree=wz_a6treea(j1,j2,j3,j4,j5,j6,za,zb)
      wz_a6loopa=tree*wz_Vpole(s(1,2))+wz_fa(j1,j2,j3,j4,j5,j6,za,zb)
      return 
      end

c      double complex function a6loopb(j1,j2,j3,j4,j5,j6,za,zb)
c      implicit none
c---  DKS Eq. 2.10 for alpha = b
c---  note that (-i) included in A(alpha),A(tree,alpha)
c---  so no factor of (+i) in front of F(alpha)
c      include 'src/Lib/wz/constants.h'
c      include 'src/Lib/wz/sprods_com.h'
c      include 'src/Lib/wz/zprods_decl.h'
c      integer j1,j2,j3,j4,j5,j6
c      double complex tree,wz_Vpole,wz_a6treeb
        
c      tree=wz_a6treeb(j1,j2,j3,j4,j5,j6,za,zb)    
c      a6loopb=tree*wz_Vpole(s(j1,j2))
c
c      return 
c      end

              

        
