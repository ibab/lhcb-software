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
*
* routine to fill the stdhep common blocks from
* evtgen (C++). This routine allows the C++ program not to
* have to mess with common blocks.
*
* Anders Ryd,  Dec 96   Created.
*
* 
      implicit none
C      INTEGER NMXHEP
C      PARAMETER (NMXHEP=4000) !!! KRAL - NEW SIZE
C      INTEGER IDHEP,ISTHEP,JDAHEP,JMOHEP,NEVHEP,NHEP
C      real PHEP,VHEP !!! Kral - double precision for STDHEP v3.0
C      COMMON/XHEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
C     &JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
C      SAVE /XHEPEVT/

#include "EvtGenModels/stdhep.inc"

        integer entry,eventnum,numparticle,istat,partnum
        integer mother,daugfirst,dauglast

        integer stdhepnum

        double precision px,py,pz,e,m,x,y,z,t

        stdhepnum=partnum


        nevhep=eventnum
        nhep=numparticle
        isthep(entry)=istat
        idhep(entry)=stdhepnum
        jmohep(1,entry)=mother
        jmohep(2,entry)=0
        jdahep(1,entry)=daugfirst
        jdahep(2,entry)=dauglast
        phep(1,entry)=px
        phep(2,entry)=py
        phep(3,entry)=pz
        phep(4,entry)=e
        phep(5,entry)=m
        vhep(1,entry)=x
        vhep(2,entry)=y
        vhep(3,entry)=z
        vhep(4,entry)=t

 	 return

      end


