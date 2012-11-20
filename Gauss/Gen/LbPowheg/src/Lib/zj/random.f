
      function zj_random()
      real * 8 zj_random
      real * 8 saverandom
      logical fixed
      COMMON/zj_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         zj_random=saverandom
         return
      endif
      call zj_RM48(zj_random,1)
      saverandom=zj_random
      end


      subroutine zj_resetrandom
      call zj_RM48IN(54217137,0,0)
      end

      subroutine zj_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to zj_randomsave'
         stop
      endif
      call zj_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry zj_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to zj_randomrestore'
         stop
      endif
      call zj_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine zj_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call zj_RM48IN(I1,N1,N2)
         else
c     just change the zj_random zj_SEED
            call zj_RM48IN(I1,0,0)
         endif
      else
         call zj_resetrandom
      endif
      end


      subroutine zj_savecurrentrandom
      implicit none
      integer ipar(3)
      common/zj_crandom/ipar
      call zj_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine zj_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/zj_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine zj_printcurrentrandom
      implicit none
      integer ipar(3)
      common/zj_crandom/ipar
      write(*,*) 'zj_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
