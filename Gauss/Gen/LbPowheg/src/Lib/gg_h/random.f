
      function gg_h_random()
      real * 8 gg_h_random
      real * 8 saverandom
      logical fixed
      COMMON/gg_h_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         gg_h_random=saverandom
         return
      endif
      call gg_h_RM48(gg_h_random,1)
      saverandom=gg_h_random
      end


      subroutine gg_h_resetrandom
      call gg_h_RM48IN(54217137,0,0)
      end

      subroutine gg_h_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to gg_h_randomsave'
         stop
      endif
      call gg_h_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry gg_h_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to gg_h_randomrestore'
         stop
      endif
      call gg_h_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine gg_h_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call gg_h_RM48IN(I1,N1,N2)
         else
c     just change the gg_h_random gg_h_SEED
            call gg_h_RM48IN(I1,0,0)
         endif
      else
         call gg_h_resetrandom
      endif
      end


      subroutine gg_h_savecurrentrandom
      implicit none
      integer ipar(3)
      common/gg_h_crandom/ipar
      call gg_h_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine gg_h_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/gg_h_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine gg_h_printcurrentrandom
      implicit none
      integer ipar(3)
      common/gg_h_crandom/ipar
      write(*,*) 'gg_h_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
