      subroutine zz_init_couplings
      implicit none
      include 'src/Lib/zz/PhysPars.h'
      include 'src/Lib/zz/zcouple.h'  !TM now set the z-coupling parameters here
      include 'src/Lib/zz/ewcharge.h'
      include 'src/Lib/zz/qcdcouple.h'
      include 'src/Lib/zz/pwhg_st.h'
      include 'src/Lib/zz/pwhg_math.h'
      include 'src/Lib/zz/pwhg_physpar.h'
      include 'src/Lib/zz/cvecbos.h'
      logical verbose
      parameter(verbose=.true.)
      physpar_ml(1)=0.511d-3
      physpar_ml(2)=0.1057d0
      physpar_ml(3)=1.777d0
      physpar_mq(1)=0.33d0     ! up
      physpar_mq(2)=0.33d0     ! down
      physpar_mq(3)=0.50d0     ! strange
      physpar_mq(4)=1.50d0     ! charm
      physpar_mq(5)=4.80d0     ! bottom

      call zz_smcouplings

c     number of light flavors
      st_nlight = 5

      !TM added QCD couplings

      write(*,*)'alpha',st_alpha
      gsq = st_alpha*4d0*pi
      as  = st_alpha
      ason2pi = st_alpha/2d0/pi
      ason4pi = st_alpha/4d0/pi


      
      !TM added z couplings
      Q(-5)=+0.333333333333333d0
      Q(-4)=-0.666666666666667d0
      Q(-3)=+0.333333333333333d0
      Q(-2)=-0.666666666666667d0
      Q(-1)=+0.333333333333333d0
      Q(0)=+0d0
      Q(+1)=-0.333333333333333d0
      Q(+2)=+0.666666666666667d0
      Q(+3)=-0.333333333333333d0
      Q(+4)=+0.666666666666667d0
      Q(+5)=-0.333333333333333d0
      tau=(/1d0,-1d0,1d0,-1d0,1d0,0d0,-1d0,1d0,-1d0,1d0,-1d0/)
      esq = ph_unit_e**2
      zmass = ph_Zmass
      zwidth = ph_Zwidth

      call zz_couplz(ph_sthw2)

      if ((vdecaymodeZ1.eq.11).or.(vdecaymodeZ1.eq.13)
     .    .or.(vdecaymodeZ1.eq.15)) then
      q1=-1d0
      zz_L1=le
      r1=re
      elseif ((vdecaymodeZ1.eq.12).or.(vdecaymodeZ1.eq.14)
     .    .or.(vdecaymodeZ1.eq.16)) then
      q1=0d0
      zz_L1=ln
      r1=rn
      endif
      if ((vdecaymodeZ2.eq.11).or.(vdecaymodeZ2.eq.13)
     .    .or.(vdecaymodeZ2.eq.15)) then
      q2=-1d0
      zz_L2=le
      r2=re
      elseif ((vdecaymodeZ2.eq.12).or.(vdecaymodeZ2.eq.14)
     .    .or.(vdecaymodeZ2.eq.16)) then
      q2=0d0
      zz_L2=ln
      r2=rn
      endif




      if(verbose) then
      write(*,*) '*************************************'
      write(*,*) 'Z mass = ',ph_Zmass
      write(*,*) 'Z width = ',ph_Zwidth
      write(*,*) 'W mass = ',ph_Wmass
      write(*,*) 'W width = ',ph_Wwidth
      write(*,*) '1/alphaem = ',1d0/ph_alphaem
      write(*,*) 'sthw2 = ',ph_sthw2
      write(*,*) 'e**2  = ',ph_unit_e**2
      write(*,*) '*************************************'
      endif

      end



