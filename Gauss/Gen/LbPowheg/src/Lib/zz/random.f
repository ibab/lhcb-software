
      function zz_random()
      real * 8 zz_random
      real * 8 saverandom
      logical fixed
      COMMON/zz_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         zz_random=saverandom
         return
      endif
      call zz_RM48(zz_random,1)
      saverandom=zz_random
      end


      subroutine zz_resetrandom
      call zz_RM48IN(54217137,0,0)
      end

      subroutine zz_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to zz_randomsave'
         stop
      endif
      call zz_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry zz_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to zz_randomrestore'
         stop
      endif
      call zz_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine zz_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call zz_RM48IN(I1,N1,N2)
         else
c     just change the zz_random zz_SEED
            call zz_RM48IN(I1,0,0)
         endif
      else
         call zz_resetrandom
      endif
      end


      subroutine zz_savecurrentrandom
      implicit none
      integer ipar(3)
      common/zz_crandom/ipar
      call zz_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine zz_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/zz_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine zz_printcurrentrandom
      implicit none
      integer ipar(3)
      common/zz_crandom/ipar
      write(*,*) 'zz_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
