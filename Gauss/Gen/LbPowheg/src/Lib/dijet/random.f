
      function dijet_random()
      real * 8 dijet_random
      real * 8 saverandom
      logical fixed
      COMMON/dijet_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         dijet_random=saverandom
         return
      endif
      call dijet_RM48(dijet_random,1)
      saverandom=dijet_random
      end


      subroutine dijet_resetrandom
      call dijet_RM48IN(54217137,0,0)
      end

      subroutine dijet_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to dijet_randomsave'
         stop
      endif
      call dijet_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry dijet_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to dijet_randomrestore'
         stop
      endif
      call dijet_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine dijet_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call dijet_RM48IN(I1,N1,N2)
         else
c     just change the dijet_random dijet_SEED
            call dijet_RM48IN(I1,0,0)
         endif
      else
         call dijet_resetrandom
      endif
      end


      subroutine dijet_savecurrentrandom
      implicit none
      integer ipar(3)
      common/dijet_crandom/ipar
      call dijet_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine dijet_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/dijet_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine dijet_printcurrentrandom
      implicit none
      integer ipar(3)
      common/dijet_crandom/ipar
      write(*,*) 'dijet_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
