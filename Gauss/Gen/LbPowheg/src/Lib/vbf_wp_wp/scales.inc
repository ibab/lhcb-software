c
c alfas, scales etc. Notation for the arrays is
c
c  variable(i,L):   i=1 upper line,  i=2 lower line
c                   L=1 for use with main amplitude^2
c                   L=2,3 for use with upper/lower line finite
c                         vbf_wp_wp_born^2 terms in 3 parton run 
c
      real*8 mufsq(2,3), als(2,3), xir, xif, qmin(3)
      integer id_mur, id_muf
      common /vbf_wp_wp_cscales/ mufsq, als, xir, xif, qmin, id_mur, id_muf

      real*8 mursq(2,3)
      common /vbf_wp_wp_rscales/ mursq
