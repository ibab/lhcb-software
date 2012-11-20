c -*- fortran -*-
      
      integer rnd_numseeds,rnd_initialseed,rnd_iwhichseed,rnd_i1,rnd_i2
      character * 4 rnd_cwhichseed
      common/wj_pwhg_rnd/rnd_numseeds,rnd_initialseed,rnd_iwhichseed,
     1 rnd_cwhichseed,rnd_i1,rnd_i2
      save /wj_pwhg_rnd/
