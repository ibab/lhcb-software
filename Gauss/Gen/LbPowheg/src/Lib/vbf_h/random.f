
      function vbf_h_random()
      real * 8 vbf_h_random
      real * 8 saverandom
      logical fixed
      COMMON/vbf_h_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         vbf_h_random=saverandom
         return
      endif
      call vbf_h_RM48(vbf_h_random,1)
      saverandom=vbf_h_random
      end


      subroutine vbf_h_resetrandom
      call vbf_h_RM48IN(54217137,0,0)
      end

      subroutine vbf_h_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to vbf_h_randomsave'
         stop
      endif
      call vbf_h_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry vbf_h_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to vbf_h_randomrestore'
         stop
      endif
      call vbf_h_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine vbf_h_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call vbf_h_RM48IN(I1,N1,N2)
         else
c     just change the vbf_h_random vbf_h_SEED
            call vbf_h_RM48IN(I1,0,0)
         endif
      else
         call vbf_h_resetrandom
      endif
      end


      subroutine vbf_h_savecurrentrandom
      implicit none
      integer ipar(3)
      common/vbf_h_crandom/ipar
      call vbf_h_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine vbf_h_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/vbf_h_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine vbf_h_printcurrentrandom
      implicit none
      integer ipar(3)
      common/vbf_h_crandom/ipar
      write(*,*) 'vbf_h_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
