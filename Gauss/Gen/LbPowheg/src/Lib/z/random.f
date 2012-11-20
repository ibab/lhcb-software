
      function z_random()
      real * 8 z_random
      real * 8 saverandom
      logical fixed
      COMMON/z_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         z_random=saverandom
         return
      endif
      call z_RM48(z_random,1)
      saverandom=z_random
      end


      subroutine z_resetrandom
      call z_RM48IN(54217137,0,0)
      end

      subroutine z_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to z_randomsave'
         stop
      endif
      call z_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry z_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to z_randomrestore'
         stop
      endif
      call z_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine z_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call z_RM48IN(I1,N1,N2)
         else
c     just change the z_random z_SEED
            call z_RM48IN(I1,0,0)
         endif
      else
         call z_resetrandom
      endif
      end


      subroutine z_savecurrentrandom
      implicit none
      integer ipar(3)
      common/z_crandom/ipar
      call z_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine z_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/z_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine z_printcurrentrandom
      implicit none
      integer ipar(3)
      common/z_crandom/ipar
      write(*,*) 'z_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
