C--------------------------------------------------------------------------
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See EvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: uclacont.F
C
C Description:
C
C Modification history:
C
C    SBC     Nov. 24, 1998         Module created
C
C------------------------------------------------------------------------
      subroutine uclacont(energy,flavor,ndaug,kf,px,py,pz,e)

      implicit none

      common/lujets/n,k(4000,5),p(4000,5),v(4000,5)

      integer n,k
      real p,v

      real*8 energy

      real senergy

      integer gbrcomp
      external gbrcomp

      integer flavor,kf(100),i,ndaug,istart
      real*8  px(100),py(100),pz(100),e(100)

      senergy=energy

C      if(flavor.ne.0) kf_user=flavor
C      flavor = kf_user

 4    call gbreevt(flavor,senergy) ! e+e- --> qq shower parton model

      ndaug=0

C--- Save the primary quarks

      do 1,i=1,n
        if(abs(k(i,2)).gt.6) goto 1
        if(k(i,2).eq.0)      goto 1
        if(k(i,3).ne.0)      goto 1
        ndaug=ndaug+1
        kf(ndaug)=k(i,2)
        px(ndaug)=p(i,1)
        py(ndaug)=p(i,2)
        pz(ndaug)=p(i,3)
        e(ndaug) =p(i,4)
1     continue


C---  Find where the String ended
      istart=0
      do 2, i=1,n
        if(k(i,2).eq.92) istart=i
2     continue

      if(istart.eq.0) then
        call gbrlist(1)
        print *, '  UCLAII model was not run! '
        goto 4
c        stop
      endif


C     Copy particle.  Secondary decays were stopped in Jetset, so only 
C     the particles from fragmentation are copied.

      do 3,i=istart+1,n
        if(k(i,3).ne.istart) goto 3
        ndaug = ndaug + 1
        kf(ndaug)=k(i,2)
        px(ndaug)=p(i,1)
        py(ndaug)=p(i,2)
        pz(ndaug)=p(i,3)
        e(ndaug) =p(i,4)
 3    continue


      return
      end






