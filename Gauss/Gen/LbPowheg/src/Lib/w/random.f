
      function w_random()
      real * 8 w_random
      real * 8 saverandom
      logical fixed
      COMMON/w_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         w_random=saverandom
         return
      endif
      call w_RM48(w_random,1)
      saverandom=w_random
      end


      subroutine w_resetrandom
      call w_RM48IN(54217137,0,0)
      end

      subroutine w_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to w_randomsave'
         stop
      endif
      call w_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry w_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to w_randomrestore'
         stop
      endif
      call w_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine w_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call w_RM48IN(I1,N1,N2)
         else
c     just change the w_random w_SEED
            call w_RM48IN(I1,0,0)
         endif
      else
         call w_resetrandom
      endif
      end


      subroutine w_savecurrentrandom
      implicit none
      integer ipar(3)
      common/w_crandom/ipar
      call w_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine w_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/w_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine w_printcurrentrandom
      implicit none
      integer ipar(3)
      common/w_crandom/ipar
      write(*,*) 'w_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
