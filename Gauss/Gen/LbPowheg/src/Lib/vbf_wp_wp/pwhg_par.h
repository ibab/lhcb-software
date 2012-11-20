c          -*- fortran -*-
      real * 8 par_csicut
      parameter (par_csicut=1)
      
      real * 8 par_diexp,par_dijexp,par_2gsupp,
     1         par_fsrtinycsi,par_fsrtinyy,
     2         par_isrtinycsi,par_isrtinyy
      common/vbf_wp_wp_pwhg_par/par_diexp,par_dijexp,par_2gsupp,
     1         par_fsrtinycsi,par_fsrtinyy,
     2         par_isrtinycsi,par_isrtinyy
      save /vbf_wp_wp_pwhg_par/
