c overwrites the original RANF function used in B->PiPiPi model
c puts Gaudi RndmNumberSvc in its place

      REAL FUNCTION RANF( )
      IMPLICIT NONE
      DOUBLE PRECISION cranf
      EXTERNAL         cranf
      RANF=cranf( )
      RETURN
      END
