
      function wz_random()
      real * 8 wz_random
      real * 8 saverandom
      logical fixed
      COMMON/wz_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         wz_random=saverandom
         return
      endif
      call wz_RM48(wz_random,1)
      saverandom=wz_random
      end


      subroutine wz_resetrandom
      call wz_RM48IN(54217137,0,0)
      end

      subroutine wz_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to wz_randomsave'
         stop
      endif
      call wz_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry wz_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to wz_randomrestore'
         stop
      endif
      call wz_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine wz_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call wz_RM48IN(I1,N1,N2)
         else
c     just change the wz_random wz_SEED
            call wz_RM48IN(I1,0,0)
         endif
      else
         call wz_resetrandom
      endif
      end


      subroutine wz_savecurrentrandom
      implicit none
      integer ipar(3)
      common/wz_crandom/ipar
      call wz_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine wz_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/wz_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine wz_printcurrentrandom
      implicit none
      integer ipar(3)
      common/wz_crandom/ipar
      write(*,*) 'wz_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
