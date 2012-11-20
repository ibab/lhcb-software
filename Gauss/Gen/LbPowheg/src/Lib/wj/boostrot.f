
      
      subroutine wj_mboost(m,vec,beta,vin,vout)
c     boosts the m vectors vin(0:3,m) into the vectors vout(0:3,m) (that can
c     be the same) in the direction of vec(3) (|vec|=1) with velocity
c     beta.  Lorents convention: (t,x,y,z).
      implicit none
      integer m
      real * 8 vec(3),beta,vin(0:3,m),vout(0:3,m)
      real * 8 betav,gamma
      real * 8 vdotb
      integer ipart,idim
      gamma=1/sqrt(1-beta**2)
      do ipart=1,m
         vdotb=vin(1,ipart)*vec(1)
     #         +vin(2,ipart)*vec(2)+vin(3,ipart)*vec(3)
         do idim=1,3
            vout(idim,ipart)=vin(idim,ipart)
     #           +vec(idim)*((gamma-1)*vdotb
     #           +gamma*beta*vin(0,ipart))
         enddo
         vout(0,ipart)=gamma*(vin(0,ipart)+vdotb*beta)
      enddo
      end


      subroutine wj_mrotate(dir,sinphi,cosphi,vec)
c Rotates vector vec counterclockwise around the direction
c dir (|dir|=1) with angle phi, given sin phi and cos phi.
      implicit none
      real * 8 sinphi,cosphi,dir(3),vec(3)
      real * 8 dircrossvec(3),dirdotvec
      integer i
      dircrossvec(1)=dir(2)*vec(3)-dir(3)*vec(2)
      dircrossvec(2)=dir(3)*vec(1)-dir(1)*vec(3)
      dircrossvec(3)=dir(1)*vec(2)-dir(2)*vec(1)
      dirdotvec=dir(1)*vec(1)+dir(2)*vec(2)+dir(3)*vec(3)
      do i=1,3
         vec(i)=vec(i)+sinphi*dircrossvec(i)
     #        -(1-cosphi)*(vec(i)-dir(i)*dirdotvec)
      enddo
      end
