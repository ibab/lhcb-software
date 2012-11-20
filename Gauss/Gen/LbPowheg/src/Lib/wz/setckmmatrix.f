      subroutine wz_setckmmatrix
c --- reads in anomalous couplings from powheg.input
      implicit none
      include 'src/Lib/wz/constants.h'
      include 'src/Lib/wz/ckm.h'
      include 'src/Lib/wz/cabibbo.h'
      include 'src/Lib/wz/nwz.h'
      real * 8 wz_powheginput
      external wz_powheginput


      if (diagonal_CKM) then
         Vud=1d0
         Vus=0d0
         Vub=0d0
         Vcd=0d0
         Vcs=1d0
         Vcb=0d0
      else
         Vus=dsqrt(1d0-Vud**2) !0.222d0
         Vub=0d0
         Vcd=dsqrt(1d0-Vud**2) !0.222d0
         Vcs=Vud
         Vcb=0d0
      endif         
      Vsq=0d0
      if (wz_nwz.eq.1) then
         Vsq(2,-1)=Vud**2
         Vsq(2,-3)=Vus**2
         Vsq(2,-5)=Vub**2
         Vsq(4,-1)=Vcd**2
         Vsq(4,-3)=Vcs**2
         Vsq(4,-5)=Vcb**2
         Vsq(-1,2)=Vsq(2,-1)
         Vsq(-3,2)=Vsq(2,-3)
         Vsq(-5,2)=Vsq(2,-5)
         Vsq(-1,4)=Vsq(4,-1)
         Vsq(-3,4)=Vsq(4,-3)
         Vsq(-5,4)=Vsq(4,-5)
      else
         Vsq(1,-2)=Vud**2
         Vsq(3,-2)=Vus**2
         Vsq(5,-2)=Vub**2
         Vsq(1,-4)=Vcd**2
         Vsq(3,-4)=Vcs**2
         Vsq(5,-4)=Vcb**2
         Vsq(-2,1)=Vsq(1,-2)
         Vsq(-2,3)=Vsq(3,-2)
         Vsq(-2,5)=Vsq(5,-2)
         Vsq(-4,1)=Vsq(1,-4)
         Vsq(-4,3)=Vsq(3,-4)
         Vsq(-4,5)=Vsq(5,-4)
      endif

      return 
      end

