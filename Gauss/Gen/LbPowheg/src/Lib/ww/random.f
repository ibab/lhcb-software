
      function ww_random()
      real * 8 ww_random
      real * 8 saverandom
      logical fixed
      COMMON/ww_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         ww_random=saverandom
         return
      endif
      call ww_RM48(ww_random,1)
      saverandom=ww_random
      end


      subroutine ww_resetrandom
      call ww_RM48IN(54217137,0,0)
      end

      subroutine ww_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to ww_randomsave'
         stop
      endif
      call ww_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry ww_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to ww_randomrestore'
         stop
      endif
      call ww_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine ww_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call ww_RM48IN(I1,N1,N2)
         else
c     just change the ww_random ww_SEED
            call ww_RM48IN(I1,0,0)
         endif
      else
         call ww_resetrandom
      endif
      end


      subroutine ww_savecurrentrandom
      implicit none
      integer ipar(3)
      common/ww_crandom/ipar
      call ww_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine ww_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/ww_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine ww_printcurrentrandom
      implicit none
      integer ipar(3)
      common/ww_crandom/ipar
      write(*,*) 'ww_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
