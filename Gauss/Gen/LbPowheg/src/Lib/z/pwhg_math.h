c -*- Fortran -*-

      real * 8 pi,pi2
      parameter (pi=3.141592653589793238462643383279502884197D0,
     1           pi2=pi**2)
      real * 8 CF,CA,TF
c number of colors	
      integer nc
      parameter (CF=4d0/3, CA=3d0, TF=1d0/2, nc=3)
      
      real * 8 hc2
c     GeV^-2 to pb cross section
      parameter(hc2=3.8937966d8)
