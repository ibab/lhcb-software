C--------------------------------------------------------------------------
C $Id: begevtgengetx.F,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
C
C Environment:
C      This software is part of the EvtGen package developed jointly
C      for the BaBar and CLEO collaborations.  If you use all or part
C      of it, please give an appropriate acknowledgement.
C
C Copyright Information: See EvtGen/COPYRIGHT
C      Copyright (C) 1998      Caltech, UCSB
C
C Module: begevtgengetx.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C
C------------------------------------------------------------------------
      subroutine begevtgengetx(entry,eventnum,numparticle,istat,
     +                          partnum,mother,daugfirst,dauglast,
     +                          px,py,pz,e,m,x,y,z,t)
*
* routine to read from the stdhep common block from
* evtgen (C++). This routine allows the C++ program not to
* have to mess with common blocks.
*
* Anders Ryd,  Oct 1   Created.
*
* 
      implicit none

#include "EvtGenModels/common_hepevt.inc"

        integer entry,eventnum,numparticle,istat,partnum
        integer mother,daugfirst,dauglast

        integer stdhepnum

        double precision px,py,pz,e,m,x,y,z,t

        stdhepnum=partnum



        eventnum=d_h_nevhep
        numparticle=d_h_nhep
        istat=d_h_isthep(entry)
        stdhepnum=d_h_idhep(entry)
        mother=d_h_jmohep(1,entry)
        daugfirst=d_h_jdahep(1,entry)
        dauglast=d_h_jdahep(2,entry)
        px=d_h_phep(1,entry)
        py=d_h_phep(2,entry)
        pz=d_h_phep(3,entry)
        e=d_h_phep(4,entry)
        m=d_h_phep(5,entry)
        x=d_h_vhep(1,entry)
        y=d_h_vhep(2,entry)
        z=d_h_vhep(3,entry)
        t=d_h_vhep(4,entry)

 	 return

      end


