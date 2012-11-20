      subroutine gg_h_gen_born_phsp(xborn)
      implicit none
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      real * 8 xborn(ndiminteg-3)
      call gg_h_born_phsp(xborn)
      call gg_h_compute_csimax_fsr
      end


      subroutine gg_h_compute_csimax_fsr
      implicit none
c Compute csimax for all possible final state emitters;
c for initial state emitters it is not possible, since
c csimax depends upon y in this case.
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      integer j
      real * 8 q0,mrec2
      logical gg_h_valid_emitter
      external gg_h_valid_emitter
      do j=0,nlegborn
         if(gg_h_valid_emitter(j)) then
            if(j.gt.2) then
               q0=2*kn_cmpborn(0,1)
               mrec2=(q0-kn_cmpborn(0,j))**2
     #               -kn_cmpborn(1,j)**2-kn_cmpborn(2,j)**2
     #               -kn_cmpborn(3,j)**2
               kn_csimax_arr(j)=1-mrec2/q0**2
            endif
         else
            kn_csimax_arr(j)=-1
         endif
      enddo
      end

