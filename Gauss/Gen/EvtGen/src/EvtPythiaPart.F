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
C Module: EvtJetSetInit.F
C
C Description:
C
C Modification history:
C
C    DJL/RYD     August 11, 1998         Module created
C
C------------------------------------------------------------------------
      subroutine evtpythiapart(kc,amass,width,widthmax,ctau)

      implicit double precision(A-H,O-Z)
      implicit integer(I-N)
      
CCC      external pydata

      Common /pydat2/ kchg(500,4),pmas(500,4),parf(2000),vckm(4,4)

      if(kc.gt.500) Then
       Write(6,*) 'EVTPYTHIAPART: Lund KC code > 500 !!!'
       Return
      endif
      pmas(kc,1) = amass
      pmas(kc,2) = width
      pmas(kc,3) = widthmax
      pmas(kc,4) = ctau

      end






