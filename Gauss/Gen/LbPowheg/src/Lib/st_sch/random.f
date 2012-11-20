
      function st_sch_random()
      real * 8 st_sch_random
      real * 8 saverandom
      logical fixed
      COMMON/st_sch_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         st_sch_random=saverandom
         return
      endif
      call st_sch_RM48(st_sch_random,1)
      saverandom=st_sch_random
      end


      subroutine st_sch_resetrandom
      call st_sch_RM48IN(54217137,0,0)
      end

      subroutine st_sch_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to st_sch_randomsave'
         stop
      endif
      call st_sch_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry st_sch_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to st_sch_randomrestore'
         stop
      endif
      call st_sch_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine st_sch_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call st_sch_RM48IN(I1,N1,N2)
         else
c     just change the st_sch_random st_sch_SEED
            call st_sch_RM48IN(I1,0,0)
         endif
      else
         call st_sch_resetrandom
      endif
      end


      subroutine st_sch_savecurrentrandom
      implicit none
      integer ipar(3)
      common/st_sch_crandom/ipar
      call st_sch_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine st_sch_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/st_sch_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine st_sch_printcurrentrandom
      implicit none
      integer ipar(3)
      common/st_sch_crandom/ipar
      write(*,*) 'st_sch_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
