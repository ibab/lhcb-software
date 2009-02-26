C $Id: ran.F,v 1.3 2009-02-26 18:15:43 robbep Exp $
c use Gaudi RndmNumberSvc as random generator
      
      REAL FUNCTION RAN(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      RAN=CPYR(IDUMMY)
      RETURN
      END
      
