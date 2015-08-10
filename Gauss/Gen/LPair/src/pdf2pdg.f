*CMZ :          03/05/96  11.33.34  by  Unknown
*-- Author :
***********************************************************************
      SUBROUTINE PDF2PDG(X,QSCALE,DENS)
*  CONVERT STRUCTF OUTPUT TO AN ARREAY USING PDG CODE AS INDEX
***********************************************************************
      REAL*8 DENS(-6:2)
      REAL*8 X,QSCALE
      CALL STRUCTF(X,QSCALE,DENS(2),DENS(1),DENS(-1),DENS(-3),
     &             DENS(-4),DENS(-5),DENS(-6),DENS(0))
      DENS(-2)=DENS(-1)
      END
