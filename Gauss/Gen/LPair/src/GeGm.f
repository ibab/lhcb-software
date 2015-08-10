*-- Author :    A. Shamov 06/01/08
* Calculate proton form factors Ge,Gm
* Factor mu_p is excluded from Gm!
      subroutine GeGm(T,Ge,Gm)
      double precision T

      real GmPhysRevC76_2007_035205,GePhysRevC76_2007_035205
      real GmPhysRevC65_2002_051001,GePhysRevC65_2002_051001

      integer mFF
      common /mGmGe/ mFF

      integer First
      data First/1/
      save First
      real Gm2,Ge2,Gm3,Ge3
      data Gm2/0./,Gm3/0./
      data Ge2/0./,Ge3/0./
      save Gm2,Ge2,Gm3,Ge3
      real q2,q2_0

      q2=-T
      if(mod(mFF,10).eq.1) then
         Gm=GmPhysRevC76_2007_035205(q2)
         Ge=GePhysRevC76_2007_035205(q2)
      elseif(mFF.eq.2) then
         if(Gm2.eq.0) then
            Gm2=GmPhysRevC65_2002_051001(0.)
            Ge2=GePhysRevC65_2002_051001(0.)
         endif
         Gm=GmPhysRevC65_2002_051001(q2)/Gm2
         Ge=GePhysRevC65_2002_051001(q2)/Ge2
      elseif(mFF.eq.3) then
         if(Gm3.eq.0) then
            Gm3=GmNuclPhysA596_1996_367(0.)
            Ge3=GeNuclPhysA596_1996_367(0.)
         endif
         Gm=GmNuclPhysA596_1996_367(q2)/Gm3
         Ge=GeNuclPhysA596_1996_367(q2)/Ge3
      elseif(mod(mFF,10).eq.4) then
         Ge=GeArringtonLQ(q2)
         Gm=GmArringtonLQ(q2)
      elseif(mFF.eq.-98) then
         if(First.ne.0) then
            print *,'Ge,Gm',1.,0.
            First=0
         endif
         Ge=1.
         Gm=0.
      elseif(mFF.eq.-97) then
         if(First.ne.0) then
            print *,'Ge,Gm',0.,1.
            First=0
         endif
         Ge=0.
         Gm=1.
      elseif(mFF.eq.-96) then
         if(First.ne.0) then
            print *,'Ge,Gm',1.,1.
            First=0
         endif
         Ge=1.
         Gm=1.
      elseif(mFF.gt.60.and.mFF.lt.80) then
         q2_0=mFF/100.
         if(First.ne.0) then
            print *,'Dipole Ge,Gm, q0^2',q2_0
            First=0
         endif
         Ge=1./(1.+q2/q2_0)**2
         Gm=Ge
      else
         if(First.ne.0) then
            print *,'Dipole Ge,Gm, q0^2',0.71
            First=0
         endif
         Ge=1./(1.+q2/0.71)**2
         Gm=Ge
      endif

      end
