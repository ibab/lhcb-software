C $Id: HidValley.inc,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C =====================================================================
C  @file 
C  Purpose : local common block for "Onia Pairs" production, 
C
C  @author:  Alexey Novoselov Alexey.Novoselov@cern.ch
C  @date: 2011-05-30
C
C  $Revision$
C  Last Modification $Date$
C  by $Author$
C =====================================================================
C...The user's own transfer of information.
      DOUBLE PRECISION ECM, Wtmax, csSumm
      INTEGER upsel, Nruns
      LOGICAL INIT
      DOUBLE PRECISION PSI01S/0.2115/
      DOUBLE PRECISION PSI02S/0.166/
      DOUBLE PRECISION UPS01S/0.635/
      COMMON /DIONIA/ECM,INIT,upsel,Wtmax(4),Nruns(4),csSumm(4)
      SAVE   /DIONIA/
C =====================================================================
C 
C =====================================================================
