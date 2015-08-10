*CMZ            17/02/08            by  A. Shamov
*  1) double precision args
*  2) /HEPEVT/ filling
*CMZ :  1.00/00 16/01/92  17.23.59  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.20  by  O. Duenger
*CMZ :  0.00/00 20/12/91  09.27.19  by  O. Duenger
*-- Author :    O. Duenger   19/12/91
*
      SUBROUTINE LUPSET_HEP(LINE,PX,PY,PZ,E,M)
*=================================================================
      IMPLICIT NONE
      INTEGER LINE
      real *8 PX,PY,PZ,E,M
      REAL ULMASS
*
C---JETSET and GENOUT common
      REAL*4        P(4000,5),V(4000,5)
      INTEGER       N,K(4000,5)
      COMMON/LUJETS/N,K,P,V
*
      include 'lpair_hepevt.inc'
*
      integer i
*
      IF (LINE .GT. N) THEN
       WRITE(6,*) ' LUPSET : TOO BIG LINE NUMBER, LINE =',LINE,', N =',N
       RETURN
      ENDIF
*
      PHEP(1,LINE)=PX
      PHEP(2,LINE)=PY
      PHEP(3,LINE)=PZ
      PHEP(4,LINE)=E
      IF (M .GE. -9998.0) THEN
         PHEP(5,LINE)=M
      ELSE
         PHEP(5,LINE)=ULMASS(K(LINE,2))
      ENDIF
      do i=1,5
         P(LINE,i)=PHEP(i,LINE)
      enddo
      END
