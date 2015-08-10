*CMZ            17/02/08            by  A. Shamov
*  2) /HEPEVT/ filling
*CMZ :  1.00/00 16/01/92  17.23.59  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.20  by  O. Duenger
*CMZ :  0.00/00 19/12/91  13.03.55  by  O. Duenger
*-- Author :    O. Duenger   19/12/91
*
      SUBROUTINE LUNSET_HEP(LINE)
*=================================================================
      IMPLICIT NONE
      INTEGER LINE,I,II
*
C---JETSET and GENOUT common
      REAL*4        P(4000,5),V(4000,5)
      INTEGER       N,K(4000,5)
      COMMON/LUJETS/N,K,P,V
*
      include 'lpair_hepevt.inc'
      data NEVHEP/0/
*
      IF ((LINE .LT. 1) .OR. (LINE .GT. 4000)) THEN
       WRITE(6,*) ' LUNSET : WRONG LINE, LINE =',LINE
       RETURN
      ENDIF
*
      N=LINE
      NHEP=LINE
      NEVHEP=NEVHEP+1
*
      DO 100 I=1,5
         DO 200 II=1,N
            V(II,I)=0.0
 200     CONTINUE
 100  CONTINUE
*
      do II=1,N
         do I=1,4
            VHEP(I,II)=0.0
         enddo
      enddo
*     
      END
