C--------------------------------------------------------------------------
C $Id: begevtgenstorex.F,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See EvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: begevtgenstorex.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C
C------------------------------------------------------------------------
      subroutine begevtgenstorex(entry,eventnum,numparticle,istat,
     +                          partnum,mother,daugfirst,dauglast,
     +                          px,py,pz,e,m,x,y,z,t)
      implicit none
*
* routine to fill the stdhep common blocks from
* evtgen (C++). This routine allows the C++ program not to
* have to mess with common blocks.
*
* Anders Ryd,  Dec 96   Created.
*
* 

#include "EvtGenModels/common_hepevt.inc"
      logical qedrad
      integer ph_nmxhep ! this is parameter nmxhep in photos/photos_make
*                     ! Renamed here to avoid name conflict in stdhep.inc
      parameter (ph_nmxhep=10000)
      common / phoqed / qedrad(ph_nmxhep)
      integer entry,eventnum,numparticle,istat,partnum
      integer mother,daugfirst,dauglast

      integer stdhepnum,i
      
      double precision px,py,pz,e,m,x,y,z,t
      
      stdhepnum=partnum
      
      
      d_h_nevhep=eventnum
      d_h_nhep=numparticle
      d_h_isthep(entry)=istat
      d_h_idhep(entry)=stdhepnum
      d_h_jmohep(1,entry)=mother
      d_h_jmohep(2,entry)=0
      d_h_jdahep(1,entry)=daugfirst
      d_h_jdahep(2,entry)=dauglast
      d_h_phep(1,entry)=px
      d_h_phep(2,entry)=py
      d_h_phep(3,entry)=pz
      d_h_phep(4,entry)=e
      d_h_phep(5,entry)=m
      d_h_vhep(1,entry)=x
      d_h_vhep(2,entry)=y
      d_h_vhep(3,entry)=z
      d_h_vhep(4,entry)=t
      
      qedrad(entry)=.true.
      if (daugfirst.gt.0 .and. dauglast.gt.0) THEN
        do i=daugfirst, dauglast
          qedrad(i) = .true.
        end do
      end if
      
      return
      
      end
      

