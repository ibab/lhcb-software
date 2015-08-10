*CMZ :  1.00/00 02/04/92  19.31.33  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.29  by  O. Duenger
*CMZ :  0.00/00 18/12/91  16.56.40  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      SUBROUTINE MAPW2(W2,X,W2MIN,W2MAX,DW)

      IMPLICIT DOUBLE PRECISION (A-Z)

      Y    = W2MAX/W2MIN
      W2   = W2MIN*Y**X
      DW   = W2*DLOG(Y)

      RETURN
      END


      SUBROUTINE MAPXQ(W2,X,W2MIN,W2MAX,DW)

      IMPLICIT DOUBLE PRECISION (A-Z)

      Y    = W2MAX/W2MIN
      W2   = W2MIN*Y**X
      DW   = W2*DLOG(Y)

      RETURN
      END


      SUBROUTINE MAPWX(W2,X,W2MIN,W2MAX,DW)

      IMPLICIT DOUBLE PRECISION (A-Z)

      Y    = W2MAX/W2MIN
      W2   = W2MIN*Y**X
      DW   = W2*DLOG(Y)

      RETURN
      END


      SUBROUTINE MAPT1(T,X,TMIN,TMAX,DT)

      IMPLICIT DOUBLE PRECISION (A-Z)

      Y  = TMAX/TMIN
      T  = TMIN*Y**X
      DT =-T*DLOG(Y)

C     WRITE(6,*)  '  MAPT1 :',T,X,TMIN,TMAX,DT,'  MAPT1 END'

      RETURN
      END


      SUBROUTINE MAPT2(T,X,TMIN,TMAX,DT)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      Y  = TMAX/TMIN
      T  = TMIN*Y**X
      DT =-T*DLOG(Y)

      RETURN
      END


      SUBROUTINE MAPS2(S2,X,SMIN,SMAX,DS)

      IMPLICIT DOUBLE PRECISION (A-Z)

      Y=SMAX/SMIN
      S2=SMIN*Y**X
      DS=S2*DLOG(Y)

      RETURN
      END


      SUBROUTINE MAPLA(X,Y,Z,U,XM,XP,D)

      IMPLICIT DOUBLE PRECISION (A-Z)

      XMB=XM-Y-Z
      XPB=XP-Y-Z
      C=-4.*Y*Z
      ALP=DSQRT(XPB*XPB+C)
      ALM=DSQRT(XMB*XMB+C)
      AM=XMB+ALM
      AP=XPB+ALP
      YY=AP/AM
      ZZ=YY**U
      X=Y+Z+(AM*ZZ-C/(AM*ZZ))*0.5
      AX=DSQRT((X-Y-Z)**2+C)
      D=AX*DLOG(YY)

      RETURN
      END
