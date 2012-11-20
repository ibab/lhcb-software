c -*- Fortran -*-

c The user must set nlegborn to the appropriate value for his process.
      integer nlegborn,nlegreal
      
      parameter (nlegborn=3)
      parameter (nlegreal=nlegborn+1)

c     ndiminteg is the dimensionality of the full real integral
c     ndiminteg=(nlegreal-2)*3-4+2-1
c     if there are undecayed resonances, we need extra variables to pilot
c     the resonance's masses

      integer ndiminteg
      parameter (ndiminteg=(nlegreal-2)*3-4+2
C     $ -1      !    in 2->1 processes the phi 
C                    integration in the Born phase
C                    space is the same that in real one
     $   +1  )  ! Higgs gg_h_mass along a BW

 

      integer maxprocborn,maxprocreal
      parameter (maxprocborn=200,maxprocreal=500)
