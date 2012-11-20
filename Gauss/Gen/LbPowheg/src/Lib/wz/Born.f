      subroutine wz_setborn(p,bflav,born,bornjk,bmunu)
      implicit none
      include 'src/Lib/wz/pwhg_math.h'
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 p(0:3,nlegs),bornjk(nlegs,nlegs)
      integer bflav(nlegs)
      real * 8 bmunu(0:3,0:3,nlegs),born,colcf
      integer j,k,mu,nu




      call wz_compborn(p,bflav,born,bmunu,bornjk)

      end


      subroutine wz_compborn(pin,bflav,born,bmunu,bornjk)
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
      include 'src/Lib/wz/pwhg_math.h'
      integer nlegs
      parameter (nlegs=nlegborn)
      real * 8 pin(0:3,nlegborn)
      integer bflav(nlegborn)
      real * 8 born,bmunu(0:3,0:3,nlegs),bornjk(nlegs,nlegs)
      real *8 amp2,colcf
      integer ferm_type(nlegborn)
      integer i,j,k,mu,nu
      real * 8 ferm_charge(nlegborn)
      real * 8 p(12,1:4)  ! 12 = mxpart in MCFM
      double precision msq(-5:5,-5:5)
      real * 8 wz_suppfact2e
c     vector boson id and decay
      integer idvecbos,vdecaymode
      common/wz_cvecbos/idvecbos,vdecaymode

      do i=1,nlegborn
         p(i,4)   = pin(0,i) 
         p(i,1:3) = pin(1:3,i) 
      enddo
      p(1,:) = -p(1,:) 
      p(2,:) = -p(2,:) 

      call wz_qqb_wz(p,msq)

      born = msq(bflav(1),bflav(2))
      born = born * wz_suppfact2e(pin,bflav)


C     -- no gluons, so no spin correlated Born  
      do i=0,3
         do j=0,3
            bmunu(i,j,:)=0d0
         enddo
      enddo
      
      bornjk=0d0
C     -- here the bornjk is very simple B_i j = B C_i
      do j=1,nlegs
         if(abs(bflav(j)).le.6) then
            do k=j+1,nlegs
               if(abs(bflav(k)).le.6) then
                  colcf=cf
                  bornjk(j,k)=born*colcf
                  bornjk(k,j)=bornjk(j,k)
               endif
            enddo
         endif
      enddo
      


      end


      subroutine wz_borncolour_lh
c Sets up the colour for the given flavour configuration
c already filled in the Les Houches interface.
c In case there are several colour structure, one
c should pick one with a probability proportional to
c the value of the corresponding cross section, for the
c kinematics defined in the Les Houches interface
      include 'src/Lib/wz/LesHouches.h'
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_flst.h'
c     neutral particles
      icolup(1,3)=0
      icolup(2,3)=0
      icolup(1,4)=0
      icolup(2,4)=0
      icolup(1,5)=0
      icolup(2,5)=0
      icolup(1,6)=0
      icolup(2,6)=0
c     colored particles
      if((idup(1).gt.0).and.(idup(2).lt.0)) then
         icolup(1,1)=501
         icolup(2,1)=0
         icolup(1,2)=0
         icolup(2,2)=501
      elseif((idup(1).lt.0).and.(idup(2).gt.0)) then
         icolup(1,1)=0
         icolup(2,1)=501
         icolup(1,2)=501
         icolup(2,2)=0
      else
         write(*,*) ' invalid flavour'
         stop
      endif
      end

      subroutine wz_finalize_lh
      implicit none
      include 'src/Lib/wz/LesHouches.h'
      integer i1,i2
      real * 8 v(5),v1
      integer itmp2(2)
      real * 8 wz_random
      external wz_random
c     Set up the resonances whose mass must be preserved
c     on the Les Houches interface.
c e- is 11, anti-nue is -12; so
c idup(3)+idup(4) is 0 for Z, 1 for W+, -1 for W-
      if(idup(3)+idup(4).eq.0) then
         call wz_add_resonance(23,3,4)
      else
         call wz_add_resonance(24*(idup(3)+idup(4)),3,4)
      endif
c Careful now! 5 6 have been bumped to 6 7 by the
c resonance inserted above!
      if(idup(6)+idup(7).eq.0) then
         call wz_add_resonance(23,6,7)
      else
         call wz_add_resonance(24*(idup(6)+idup(7)),6,7)
      endif
c in case of equal fermions, randomly exchange 3 and 5 to restore
c symmetry; remember that now 34->45 and 56->78;
c So we should now exchange 5 and 8
      if(idup(4).eq.idup(7)) then
         i1=4
         i2=7
      elseif(idup(4).eq.idup(8)) then
         i1=4
         i2=8
      elseif(idup(5).eq.idup(7)) then
         i1=5
         i2=7
      elseif(idup(5).eq.idup(8)) then
         i1=5
         i2=8
      else
         i1=0
         i2=0
      endif
      if(i1.ne.0.and.wz_random().gt.0.5d0) then
         
         v=pup(:,i1)
         pup(:,i1)=pup(:,i2)
         pup(:,i2)=v

         itmp2=mothup(:,i1)
         mothup(:,i1)=mothup(:,i2)
         mothup(:,i2)=itmp2

         itmp2=icolup(:,i1)
         icolup(:,i1)=icolup(:,i2)
         icolup(:,i2)=itmp2

         v1=vtimup(i1)
         vtimup(i1)=vtimup(i2)
         vtimup(i2)=v1

         v1=spinup(i1)
         spinup(i1)=spinup(i2)
         spinup(i2)=v1

      endif
c     give masses to final-state light particles
      call wz_lhefinitemasses
      end
