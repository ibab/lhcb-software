      subroutine dijet_gen_leshouches_reg
      implicit none
      include 'src/Lib/dijet/pwhg_math.h'
      include 'src/Lib/dijet/nlegborn.h'
      include 'src/Lib/dijet/pwhg_flst.h'
      include 'src/Lib/dijet/pwhg_kn.h'
      include 'src/Lib/dijet/pwhg_rad.h'
      include 'src/Lib/dijet/LesHouches.h'
      integer ireg
      nup=nlegreal
      scalup=sqrt(rad_pt2max)
      do ireg=1,nup
c Remember: gluons are marked 0 here!
         idup(ireg)=flst_regular(ireg,rad_realreg)
         if(ireg.le.2) then
            istup(ireg)=-1
            mothup(1,ireg)=0
            mothup(2,ireg)=0            
         else
            istup(ireg)=1
            mothup(1,ireg)=1
            mothup(2,ireg)=2            
         endif
         spinup(ireg)=9
         vtimup(ireg)=0
      enddo
c add resonances, perform decays, put particles on shell, etc.(or nothing!)
      call dijet_finalize_lh 
c no remnants for now!
c     Don't forget to set scale for scalup equal to the pt of the 
c     radiation (whatever it is now!)
c     set color connections for all particles
      write(*,*) 'dijet_gen_leshouches_reg: dummy interface to regular'//
     1           ' contributions'
      write(*,*) ' Replace with your own process-dependent one, to be'
      write(*,*) ' put in the process-specific directory (e.g. /W,'//'
     #     /Z, /VBF_H...)'
      write(*,*) ' The Makefile will automatically compile the version'
      write(*,*) ' in the process-specific directory'
      call exit(1)
      end
