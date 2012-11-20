      subroutine wz_newunit(iun)
      implicit none
      integer iun
      logical ok
      integer j
      do j=10,99
         inquire(unit=j,opened=ok)
         if(.not.ok) then
            iun=j
            return
         endif
      enddo
      end
