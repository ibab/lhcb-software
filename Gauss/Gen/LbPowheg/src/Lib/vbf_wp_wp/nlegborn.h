c -*- Fortran -*-

c The user must set nlegborn to the appropriate value for his process.
      integer nlegborn,nlegreal
      
      parameter (nlegborn=8)
      parameter (nlegreal=nlegborn+1)

c     ndiminteg is the dimensionality of the full real integral
c     ndiminteg=(nlegreal-2)*3-4+2-1
c     if there are undecayed resonances, we need extra variables to pilot
c     the resonance's masses

      integer ndiminteg
c adapt according to GZ:
C     -4: (p,E) conservation 
C     +2: x1 x2 
C     -2: both Ws onshell      
C      parameter (ndiminteg=(nlegreal-2)*3-4+2-2) ! GZ Ok ? 
      parameter (ndiminteg=(nlegreal-2)*3-4+2) ! GZ Ok ? 

      integer maxprocborn,maxprocreal
      parameter (maxprocborn=200,maxprocreal=500)
