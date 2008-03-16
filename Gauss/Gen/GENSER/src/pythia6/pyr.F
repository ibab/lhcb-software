C  $Id: pyr.F,v 1.1 2008-03-16 20:20:15 robbep Exp $
c overwrites the original Pythia function
c puts Gaudi RndmNumberSvc in its place

      FUNCTION PYR(IDUMMY)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PYR=cpyr(IDUMMY)
      RETURN
      END
