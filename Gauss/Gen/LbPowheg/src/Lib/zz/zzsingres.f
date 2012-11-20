      subroutine zz_zzsingres(j1,j2,j3,j4,j5,j6,j7,za,zb,s)
c --- full contrjbution of real single resonant processes, all helicities incl
c --- helicities:1=quark, 2=non-resonant lepton, 3=resonant lepton, 4=gluon
      implicit none
!      include 'src/Lib/zz/constants.f'
      include 'src/Lib/zz/zprods_decl.h'
c      include 'src/Lib/zz/a7treesingres.f'
      integer j1,j2,j3,j4,j5,j6,j7,mplus,minus,gplus,gminus
      double complex zz_a7treesra
      double complex s(2,2,2,2)
      data minus,mplus/1,2/
      data gminus,gplus/2,1/
      
c -- quark LH :

c ------ gluon +ve 

c (3,4) LH, (5,6) LH      
      s(minus,minus,minus,mplus) = zz_a7treesra(j1,j2,j3,j4,j5,j6,j7,za,zb)       
c (3,4) LH, (5,6) RH      
      s(minus,mplus,minus,mplus) = zz_a7treesra(j1,j2,j3,j4,j6,j5,j7,za,zb)
c (3,4) RH, (5,6) LH      
      s(minus,minus,mplus,mplus) =-zz_a7treesra(j1,j2,j4,j3,j5,j6,j7,za,zb)
c (3,4) RH, (5,6) RH      
      s(minus,mplus,mplus,mplus) =-zz_a7treesra(j1,j2,j4,j3,j6,j5,j7,za,zb)

c  ------- gluon -ve : swap za <--> zb, 1 <--> 2, 3 <--> 4, 6 <--> 5

c (3,4) LH, (5,6) LH      
      s(minus,minus,minus,minus)=-zz_a7treesra(j2,j1,j4,j3,j6,j5,j7,zb,za)       
c (3,4) LH, (5,6) RH     
      s(minus,mplus,minus,minus)=-zz_a7treesra(j2,j1,j4,j3,j5,j6,j7,zb,za)
c (3,4) RH, (5,6) LH      
      s(minus,minus,mplus,minus)= zz_a7treesra(j2,j1,j3,j4,j6,j5,j7,zb,za)
c (3,4) RH, (5,6) RH      
      s(minus,mplus,mplus,minus)= zz_a7treesra(j2,j1,j3,j4,j5,j6,j7,zb,za)




c -- quark RH

c --- swap 1 <--> 2
c --- NOTE: this is a different swap to that used in zz_zzamps: za<->zb, 3<->4, 5<->6

c ------ gluon +

      
      
c (3,4) LH, (5,6) LH      
      s(mplus,minus,minus,gplus) = zz_a7treesra(j2,j1,j3,j4,j5,j6,j7,za,zb)       
c (3,4) LH, (5,6) RH      
      s(mplus,mplus,minus,gplus) = zz_a7treesra(j2,j1,j3,j4,j6,j5,j7,za,zb)
c (3,4) RH, (5,6) LH      
      s(mplus,minus,mplus,gplus) =-zz_a7treesra(j2,j1,j4,j3,j5,j6,j7,za,zb)
c (3,4) RH, (5,6) RH      
      s(mplus,mplus,mplus,gplus) =-zz_a7treesra(j2,j1,j4,j3,j6,j5,j7,za,zb)

c  ------- gluon -

c (3,4) LH, (5,6) LH      
      s(mplus,minus,minus,gminus)=-zz_a7treesra(j1,j2,j4,j3,j6,j5,j7,zb,za)       
c (3,4) LH, (5,6) RH     
      s(mplus,mplus,minus,gminus)=-zz_a7treesra(j1,j2,j4,j3,j5,j6,j7,zb,za)
c (3,4) RH, (5,6) LH      
      s(mplus,minus,mplus,gminus)= zz_a7treesra(j1,j2,j3,j4,j6,j5,j7,zb,za)
c (3,4) RH, (5,6) RH      
      s(mplus,mplus,mplus,gminus)= zz_a7treesra(j1,j2,j3,j4,j5,j6,j7,zb,za)





      return
      end
      

      
