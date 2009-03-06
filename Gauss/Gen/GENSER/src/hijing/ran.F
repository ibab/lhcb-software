C $Id: ran.F,v 1.1 2009-03-06 13:24:00 robbep Exp $
c use Gaudi RndmNumberSvc as random generator
      
      REAL FUNCTION RAN(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      RAN=CPYR(IDUMMY)
      RETURN
      END
      
