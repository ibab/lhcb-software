
      function vbf_wp_wp_random()
      real * 8 vbf_wp_wp_random
      real * 8 saverandom
      logical fixed
      COMMON/vbf_wp_wp_tmpfixed/fixed
      data fixed/.false./
      save saverandom
      if(fixed) then
         vbf_wp_wp_random=saverandom
         return
      endif
      call vbf_wp_wp_RM48(vbf_wp_wp_random,1)
      saverandom=vbf_wp_wp_random
      end


      subroutine vbf_wp_wp_resetrandom
      call vbf_wp_wp_RM48IN(54217137,0,0)
      end

      subroutine vbf_wp_wp_randomsave
      implicit none
      integer j,ipar(3,10)
      data j/0/
      save j,ipar
      j=j+1
      if(j.gt.10) then
         write(*,*) ' Too many recursive calls to vbf_wp_wp_randomsave'
         stop
      endif
      call vbf_wp_wp_RM48UT(ipar(1,j),ipar(2,j),ipar(3,j))
      return
      entry vbf_wp_wp_randomrestore
      if(j.le.0) then
         write(*,*) ' Too many calls to vbf_wp_wp_randomrestore'
         stop
      endif
      call vbf_wp_wp_RM48IN(ipar(1,j),ipar(2,j),ipar(3,j))
      j=j-1
      return
      end


      subroutine vbf_wp_wp_setrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      
      if (I1.gt.0) then
         if (((N1.gt.0).and.(N2.ge.0)).or.(N1.ge.0).and.(N2.gt.0)) then
c     restart a previous run or start a new run with this initialization
            call vbf_wp_wp_RM48IN(I1,N1,N2)
         else
c     just change the vbf_wp_wp_random vbf_wp_wp_SEED
            call vbf_wp_wp_RM48IN(I1,0,0)
         endif
      else
         call vbf_wp_wp_resetrandom
      endif
      end


      subroutine vbf_wp_wp_savecurrentrandom
      implicit none
      integer ipar(3)
      common/vbf_wp_wp_crandom/ipar
      call vbf_wp_wp_RM48UT(ipar(1),ipar(2),ipar(3))
      end


      subroutine vbf_wp_wp_getcurrentrandom(i1,n1,n2)
      implicit none
      integer i1,n1,n2
      integer ipar(3)
      common/vbf_wp_wp_crandom/ipar
      i1 = ipar(1)
      n1 = ipar(2)
      n2 = ipar(3)
      end

      subroutine vbf_wp_wp_printcurrentrandom
      implicit none
      integer ipar(3)
      common/vbf_wp_wp_crandom/ipar
      write(*,*) 'vbf_wp_wp_random number seeds: ',ipar(1),ipar(2), ipar(3)
      end
