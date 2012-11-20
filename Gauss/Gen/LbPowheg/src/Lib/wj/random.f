
      function wj_random()
      real * 8 wj_random
      real * 8 saverandom
      logical fixed
      COMMON/wj_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         wj_random=saverandom
         return
      endif
      call wj_RM48(wj_random,1)
      saverandom=wj_random
      end


      subroutine wj_resetrandom
      call wj_RM48IN(54217137,0,0)
      end

      subroutine wj_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to wj_randomsave'
         stop
      endif
      call wj_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry wj_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to wj_randomrestore'
         stop
      endif
      call wj_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine wj_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call wj_RM48IN(I1,N1,N2)
         else
c     just change the wj_random wj_SEED
            call wj_RM48IN(I1,0,0)
         endif
      else
         call wj_resetrandom
      endif
      end


      subroutine wj_savecurrentrandom
      implicit none
      integer ipar(3)
      common/wj_crandom/ipar
      call wj_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine wj_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/wj_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine wj_printcurrentrandom
      implicit none
      integer ipar(3)
      common/wj_crandom/ipar
      write(*,*) 'wj_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
