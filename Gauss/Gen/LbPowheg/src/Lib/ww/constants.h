      double precision one,two,half
      parameter(one=1d0,two=2d0,half=1d0/2d0)
      double precision xn,xnsq,cf
      integer nf,mxpart
      parameter(xn=3d0,cf=4d0/3d0,nf=4,mxpart=12,xnsq=9d0)
      double precision v
      parameter (v=8d0)
      double precision spinave,aveqq,aveqg,avegg
      parameter(spinave=0.25d0)
      parameter(aveqq=0.25d0/xnsq,aveqg=0.25d0/xn/v,avegg=0.25d0/v**2)
      double complex czip,cone,im,impi
      parameter (czip=(0d0,0d0),cone=(1d0,0d0),im=(0d0,1d0))
      double precision pi,twopi,fourpi,pisq,pisqo6
      parameter(pi=3.14159265358979d0,pisq=pi*pi)
      parameter(twopi=2d0*pi,fourpi=4d0*pi)
      parameter(pisqo6=pisq/6d0)
      parameter (impi=(0d0,pi))

