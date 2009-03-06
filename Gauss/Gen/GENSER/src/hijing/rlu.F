C $Id: rlu.F,v 1.1 2009-03-06 13:24:00 robbep Exp $
C Random number function for HIJING
C
      REAL FUNCTION RLU(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      RLU=CPYR(IDUMMY)
      RETURN
      END
      
