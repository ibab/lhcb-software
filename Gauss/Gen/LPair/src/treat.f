*CMZ :  1.00/00 16/01/92  17.24.04  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.26  by  O. Duenger
*CMZ :  0.00/00 18/12/91  16.56.39  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      DOUBLE PRECISION FUNCTION TREAT(F,X,NDIM)
C
C  AUTHOR      : J. VERMASEREN
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      EXTERNAL F
      DIMENSION X(10),Z(10)
      COMMON/VGB2/NDO,IT,SI,SI2,SWGT,SCHI,XI(50,10),SCALLS
     + ,D(50,10),DI(50,10)
      save /vgb2/
C
C
      DATA NCALL/0/
      save ncall,r
C
      IF(NCALL.EQ.0)THEN
         NCALL=1
         R=NDO
         R=R**NDIM
      ENDIF
      W=R
      DO 4 I=1,NDIM
         XX=X(I)*NDO
         J=XX
         JJ=J+1
         Y=XX-J
         IF(J.LE.0)THEN
            DD=XI(1,I)
         ELSE
            DD=XI(JJ,I)-XI(J,I)
         ENDIF
         Z(I)=XI(JJ,I)-DD*(1.-Y)
         W=W*DD
4     CONTINUE
      TREAT=W*F(Z)
C
      RETURN
      END
