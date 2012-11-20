      subroutine vbf_wp_wp_gen_born_phsp(xborn)
      implicit none
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      real * 8 xborn(ndiminteg-3)
      call vbf_wp_wp_born_phsp(xborn)
      call vbf_wp_wp_compute_csimax_fsr
      end


      subroutine vbf_wp_wp_compute_csimax_fsr
      implicit none
c Compute csimax for all possible final state emitters;
c for initial state emitters it is not possible, since
c csimax depends upon y in this case.
      include 'src/Lib/vbf_wp_wp/nlegborn.h'
      include 'src/Lib/vbf_wp_wp/pwhg_flst.h'
      include 'src/Lib/vbf_wp_wp/pwhg_kn.h'
      integer j
      real * 8 q0,mrec2
      logical vbf_wp_wp_valid_emitter
      external vbf_wp_wp_valid_emitter
      do j=0,nlegborn
         if(vbf_wp_wp_valid_emitter(j)) then
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

