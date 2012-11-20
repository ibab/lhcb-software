
      function hvq_random()
      real * 8 hvq_random
      real * 8 saverandom
      logical fixed
      COMMON/hvq_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         hvq_random=saverandom
         return
      endif
      call hvq_RM48(hvq_random,1)
      saverandom=hvq_random
      end


      subroutine hvq_resetrandom
      call hvq_RM48IN(54217137,0,0)
      end

      subroutine hvq_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to hvq_randomsave'
         stop
      endif
      call hvq_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry hvq_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to hvq_randomrestore'
         stop
      endif
      call hvq_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine hvq_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call hvq_RM48IN(I1,N1,N2)
         else
c     just change the hvq_random hvq_SEED
            call hvq_RM48IN(I1,0,0)
         endif
      else
         call hvq_resetrandom
      endif
      end


      subroutine hvq_savecurrentrandom
      implicit none
      integer ipar(3)
      common/hvq_crandom/ipar
      call hvq_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine hvq_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/hvq_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine hvq_printcurrentrandom
      implicit none
      integer ipar(3)
      common/hvq_crandom/ipar
      write(*,*) 'hvq_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
