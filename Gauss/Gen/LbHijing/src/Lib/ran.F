C $Id: ran.F,v 1.1.1.1 2006-07-03 17:23:35 gcorti Exp $
c use Gaudi RndmNumberSvc as random generator
      
      REAL FUNCTION RAN(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      RAN=CPYR(IDUMMY)
      RETURN
      END
      
