*CMZ :  1.00/00 16/01/92  17.23.59  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.20  by  O. Duenger
*CMZ :  0.00/00 20/12/91  09.27.19  by  O. Duenger
*-- Author :    O. Duenger   19/12/91
*
      SUBROUTINE LUPSET(LINE,PX,PY,PZ,E,M)
*=================================================================
      IMPLICIT NONE
      INTEGER LINE
      REAL PX,PY,PZ,E,M,ULMASS
*
C---JETSET and GENOUT common
      REAL*4        P(4000,5),V(4000,5)
      INTEGER       N,K(4000,5)
      COMMON/LUJETS/N,K,P,V
*
      IF (LINE .GT. N) THEN
       WRITE(6,*) ' LUPSET : TOO BIG LINE NUMBER, LINE =',LINE,', N =',N
       RETURN
      ENDIF
*
      P(LINE,1)=PX
      P(LINE,2)=PY
      P(LINE,3)=PZ
      P(LINE,4)=E
      IF (M .GE. -9998.0) THEN
       P(LINE,5)=M
      ELSE
       P(LINE,5)=ULMASS(K(LINE,2))
      ENDIF
      END
