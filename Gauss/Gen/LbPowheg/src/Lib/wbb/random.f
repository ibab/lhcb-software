
      function wbb_random()
      real * 8 wbb_random
      real * 8 saverandom
      logical fixed
      COMMON/wbb_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         wbb_random=saverandom
         return
      endif
      call wbb_RM48(wbb_random,1)
      saverandom=wbb_random
      end


      subroutine wbb_resetrandom
      call wbb_RM48IN(54217137,0,0)
      end

      subroutine wbb_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to wbb_randomsave'
         stop
      endif
      call wbb_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry wbb_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to wbb_randomrestore'
         stop
      endif
      call wbb_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine wbb_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call wbb_RM48IN(I1,N1,N2)
         else
c     just change the wbb_random wbb_SEED
            call wbb_RM48IN(I1,0,0)
         endif
      else
         call wbb_resetrandom
      endif
      end


      subroutine wbb_savecurrentrandom
      implicit none
      integer ipar(3)
      common/wbb_crandom/ipar
      call wbb_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine wbb_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/wbb_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine wbb_printcurrentrandom
      implicit none
      integer ipar(3)
      common/wbb_crandom/ipar
      write(*,*) 'wbb_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
