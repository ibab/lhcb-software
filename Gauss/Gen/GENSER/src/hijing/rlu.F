C $Id: rlu.F,v 1.2 2010-09-17 16:25:00 gcorti Exp $
C Random number function for HIJING
C
      REAL FUNCTION RLU(DUMMY)
      IMPLICIT NONE
      DOUBLE PRECISION DUMMY, CPYR
      INTEGER IDUMMY
      RLU=CPYR(IDUMMY)
      RETURN
      END
      
