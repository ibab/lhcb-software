C $Id: phoran.F,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $
c overwrites the original PHOTOS random function
c puts Gaudi RndmNumberSvc in its place

      FUNCTION PHORAN(IDUMMY)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PHORAN=cpyr(IDUMMY)
      RETURN
      END
