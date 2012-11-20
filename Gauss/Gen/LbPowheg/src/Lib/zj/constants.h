      double precision pi,pisq,pisqo6
      parameter(pi=3.14159265358979d0,pisq=pi*pi,pisqo6=pisq/6d0)
      double precision twopi,fourpi,pion4,pion10,pisqm8,rt2onpi
      parameter(twopi=2d0*pi)
      parameter(fourpi=4d0*pi)
      parameter(pion4=7.853981633974483d-1)  
      parameter(pion10=pi/10d0)      
      parameter(pisqm8=pisq-8d0)
c sqrt(2d0/pi)
      parameter(rt2onpi=0.797884560802865d0)
c-----------------------------------------------------
      double precision cf,ca,xn,xnsq,xn4,v,tr,qu,qd,qe,aem,Von4,ninth
      parameter(cf=4d0/3d0,ca=3d0,xn=3d0,xnsq=9d0,v=8d0,tr=0.5d0)
      parameter(Von4=2d0,ninth=1d0/9d0,xn4=xnsq-4d0)
      parameter(qu=2d0/3d0,qd=-1d0/3d0,qe=-1d0)
      parameter(aem=1d0/137.035989d0)
      double precision spinave,aveqq,aveqg,avegg
      parameter(spinave=0.25d0)
      parameter(aveqq=0.25d0/xnsq,aveqg=0.25d0/xn/v,avegg=0.25d0/v**2)
c-----------------------------------------------------
      double precision zip,half,one,two,three,four,eight
      parameter(zip=0d0,half=0.5d0,one=1d0,two=2d0)
      parameter(three=3d0,four=4d0,eight=8d0)
      double precision rt2,twort2,fourrt2
      parameter(rt2=1.4142135624d0,twort2=two*rt2,fourrt2=four*rt2)
      double precision dfbGeV2,fbGeV2,pbGeV2,nbGeV2,overa
      parameter(nbGeV2=0.389379d6)
      parameter(pbGeV2=0.389379d9)
      parameter(fbGeV2=0.389379d12)
c----decifemtobarns
      parameter(dfbGeV2=0.389379d13)
      parameter(overa=pbGeV2/xn/256d0/pi)
c-----------------------------------------------------
      double complex im,impi,czip,cone
      parameter(im=(0d0,1d0),impi=(0d0,3.14159265358979d0),
     . czip=(0d0,0d0),cone=(1d0,0d0))
c-----------------------------------------------------
      integer nloop,nf,fn,mxpart
      parameter(nf=5,fn=-5,nloop=2,mxpart=12)

