      subroutine wj_gen_leshouches_reg
      implicit none
      include 'src/Lib/wj/pwhg_st.h'
      include 'src/Lib/wj/nlegborn.h'
      include 'src/Lib/wj/PhysPars.h'
      include 'src/Lib/wj/pwhg_flst.h'
      include 'src/Lib/wj/pwhg_kn.h'
      include 'src/Lib/wj/pwhg_rad.h'
      include 'src/Lib/wj/LesHouches.h'
      integer ireg,fl
      nup=nlegreal
      do ireg=1,nup
c Remember: gluons are marked 0 here!
         fl=flst_regular(ireg,rad_realreg)
c gluons are numbered 21 in pdg
         if(fl.eq.0) fl=21
         idup(ireg)=fl
         icolup(1,ireg)=0
         icolup(2,ireg)=0
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
c it is an event with radiation
      call wj_momenta_lh(kn_preal,nup)

c     Don't forget to set scale.hor scalup equal to the pt of th c     radiation (whatever it is now!)
      scalup=sqrt(rad_pt2max)
c     set color connections for all particles
      call wj_regularcolor_lh
c id of the event
      idprup=lprup(1)
      xwgtup=+1
c     aqedup must can be set here
c     since the process-dependent PhysPars.h 
c     is well defined
      aqedup=ph_alphaem
      aqcdup=st_alpha
c     add resonance 
      call wj_finalize_lh 
      end


      subroutine wj_regularcolor_lh
c Sets up the colour for the given (real) flavour configuration already
c filled in the Les Houches interface.  For W+2jets the non singular
c contributions (regular) are of the form q qb -> W+/- qp qbpp only.
c Two diagrams contribute to each partonic channel, one when W is
c emitted from qp and one from qbpp.  However, this does not affect the
c color connections. We also assume that processes are symmetrized with
c respect to final state particles.  Thus, there is only one possible
c color configuration, irrespectively of which final state leg actually
c emits the W. Diagrammatically,
c            
c          q(1) \           / qp(5) 
c                \         / 
c                 \_______/       e(3)  
c                  _______   W  /  
c                 /       \====< 
c                /         \    \   
c               /           \     ve(4)
c          qb(2)            qbpp(6)
c
      implicit none
      include 'src/Lib/wj/LesHouches.h'
      integer iq,iqb,iqp,iqbpp
      if(idup(1).gt.0) then
         iq=1
         iqb=2
      else
         iq=2
         iqb=1
      endif
      if (idup(5).gt.0) then
         iqp=5
         iqbpp=6
      else
         iqp=6
         iqbpp=5
      endif
      call wj_colourjoin(icolup(1,iq),icolup(1,iqb),
     1           icolup(1,iqp),icolup(1,iqbpp))
      end


      subroutine wj_colourjoin(icol1,icol2,icol3,icol4)
c     join the color of entry 1-3 and 2-4
c     1-3 are assumed to be quarks
c     2-4 are assomed to be anti-quarks
      implicit none
      integer icol1(2),icol2(2),icol3(2),icol4(2)
      integer newcolor
      icol1(2)=0
      icol3(2)=0
      icol2(1)=0
      icol4(1)=0
      call wj_getnewcolor(newcolor)
      icol1(1)=newcolor
      icol3(1)=newcolor
      call wj_getnewcolor(newcolor)
      icol2(2)=newcolor
      icol4(2)=newcolor
      end
