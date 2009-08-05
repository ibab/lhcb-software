C $Id: setbeam.F,v 1.1.1.2 2009-08-05 09:43:37 ngueissa Exp $
      SUBROUTINE SETBEAM(P1X,P1Y,P1Z,P2X,P2Y,P2Z) 
      IMPLICIT NONE

      INTEGER N,NPAD,K
      DOUBLE PRECISION P,V,P1X,P1Y,P1Z,P2X,P2Y,P2Z
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      P(1,1)=P1X;
      P(1,2)=P1Y;
      P(1,3)=P1Z;

      P(2,1)=P2X;
      P(2,2)=P2Y;
      P(2,3)=P2Z;

      END
