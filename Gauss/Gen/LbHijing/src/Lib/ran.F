C $Id: ran.F,v 1.2 2009-01-19 16:48:22 jonrob Exp $
c use Gaudi RndmNumberSvc as random generator
      
      REAL FUNCTION HIJING_RAN(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      HIJING_RAN=CPYR(IDUMMY)
      RETURN
      END
      
