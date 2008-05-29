C $Id: phoran.F,v 1.2 2008-05-29 13:51:26 robbep Exp $
c overwrites the original PHOTOS random function
c puts Gaudi RndmNumberSvc in its place

      FUNCTION PHORAN(IDUMMY)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PHORAN=cpyr(IDUMMY)
      RETURN
      END
