      double precision one,two
      parameter(one=1d0,two=2d0)
      double precision xn,xnsq
      integer nf,mxpart
      parameter(xn=3d0,nf=5,mxpart=12,xnsq=9d0)
      double precision v
      parameter (v=8d0)
      double precision spinave,aveqq,aveqg,avegg
      parameter(spinave=0.25d0)
      parameter(aveqq=0.25d0/xnsq,aveqg=0.25d0/xn/v,avegg=0.25d0/v**2)

