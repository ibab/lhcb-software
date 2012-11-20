      subroutine gg_h_gen_leshouches_reg
      implicit none
      include 'src/Lib/gg_h/pwhg_math.h'
      include 'src/Lib/gg_h/nlegborn.h'
      include 'src/Lib/gg_h/pwhg_flst.h'
      include 'src/Lib/gg_h/pwhg_kn.h'
      include 'src/Lib/gg_h/pwhg_rad.h'
      include 'src/Lib/gg_h/LesHouches.h'
      include 'src/Lib/gg_h/pwhg_st.h'
      include 'src/Lib/gg_h/PhysPars.h'
      integer ireg,fl
c id of the event
      idprup=lprup(1)
      xwgtup=+1
c     aqedup must can be set here
c     since the process-dependent PhysPars.h 
c     is well defined
      aqedup=ph_alphaem
cccccccccccccccccccccccccccccccccccccccccccccccccccccc
      aqcdup=st_alpha
      nup=nlegreal
      do ireg=1,nup
         fl=flst_regular(ireg,rad_realreg)
c gluons are numbered 21 in pdg
         if(fl.eq.0) fl=21
         idup(ireg)=fl
         if(ireg.gt.2) then
            istup(ireg)=1
            mothup(1,ireg)=1
            mothup(2,ireg)=2
         else
            istup(ireg)=-1
            mothup(1,ireg)=0
            mothup(2,ireg)=0
         endif
         spinup(ireg)=9
         vtimup(ireg)=0
      enddo
c it is an event with radiation
      call gg_h_momenta_lh(kn_preal,nup)
c     set color connections for all particles
c     neutral particles
c     Higgs Boson         
      icolup(1,3)=0
      icolup(2,3)=0
c     colored particles
      if((idup(1).gt.0).and.(idup(2).lt.0)) then
         icolup(1,1)=501
         icolup(2,1)=0
         icolup(1,2)=0
         icolup(2,2)=502
         icolup(1,4)=501
         icolup(2,4)=502      
      elseif((idup(1).lt.0).and.(idup(2).gt.0)) then
         icolup(1,1)=0
         icolup(2,1)=501
         icolup(1,2)=502
         icolup(2,2)=0
         icolup(1,4)=502
         icolup(2,4)=501
      else
         write(*,*) ' invalid flavour'
         stop
      endif         
c     add resonance 
      call gg_h_finalize_lh 
c     Don't forget to set scale for scalup equal to the pt of the 
c     radiation (whatever it is now!)
      scalup=sqrt(rad_pt2max)
      end
