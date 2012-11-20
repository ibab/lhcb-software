c -*-Fortran-*-

      real * 8 kn_pborn(0:3,nlegborn),kn_preal(0:3,nlegreal),
     #   kn_cmpborn(0:3,nlegborn),kn_cmpreal(0:3,nlegreal),
     #   kn_sborn,kn_sreal,
     #   kn_beams(0:3,2),kn_sbeams,kn_y,kn_azi,kn_csitilde,
     #   kn_softvec(0:3),
     #   kn_csimax,kn_csimaxp,kn_csimaxm,
     #   kn_csi,kn_csip,kn_csim,
     #   kn_jacborn,kn_jacreal,kn_xb1,kn_xb2,
     #   kn_x1,kn_x2,kn_masses(nlegreal),
     #   kn_dijterm(0:nlegreal,nlegreal),
     #   kn_dijterm_soft(0:nlegreal-1),kn_ktmin,
     #   kn_csimax_arr(0:nlegborn),kn_minmass,kn_born_pt2,kn_cthdec


      integer kn_emitter

      common/st_sch_pwhg_kn/kn_pborn,kn_cmpborn,kn_preal,kn_cmpreal,kn_y,
     #   kn_sborn,kn_sreal,kn_azi,kn_csitilde,kn_softvec,kn_csimax,
     #   kn_csimaxp,kn_csimaxm,kn_csi,kn_csip,kn_csim,kn_jacborn,
     #   kn_jacreal,kn_beams,kn_sbeams,kn_xb1,kn_xb2,kn_x1,kn_x2,
     #   kn_masses,kn_dijterm,kn_dijterm_soft,kn_ktmin,kn_csimax_arr,
     #   kn_minmass,kn_born_pt2, kn_cthdec,
c integers
     #   kn_emitter

      save /st_sch_pwhg_kn/
