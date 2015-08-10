*CMZ :          03/05/96  11.33.34  by  Unknown
*-- Author :
*****************************************************
      SUBROUTINE GMURANX(IFLAV,X,DX)
*  MAPPING OF X
*****************************************************
      REAL*4 H1RN
      REAL*8 X,DX,Y
      REAL*8          XMIN,XMAX,Q2MIN,Q2MAX
      COMMON /W50513/ XMIN,XMAX,Q2MIN,Q2MAX
        Y=XMAX/XMIN
        X=XMIN*Y**H1RN(DUMMY)
        DX=X*DLOG(Y)
      END
