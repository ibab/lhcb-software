*-- Author :    A.Jacholkowska   03/10/98
      SUBROUTINE SUCHECK
*****************************************************************
*                                                               *
*   Subroutine   CHECK                                          *
*                                                               *
*   User entry for the event analysis                           *
*                                                               *
*   called by:  GUOUT                                           *
*                                                               *
*   Author: A.Jacholkowska, 03/10/98 20.42.02                   *
*   modified by:                                                *
*****************************************************************
      IMPLICIT NONE
#include "inc/cbunit.inc"
#include "inc/cbbank.inc"
#include "inc/wdrw_bank.inc"
#include "inc/vscp_bank.inc"
#include "inc/vscr_bank.inc"
#include "inc/ecl0_bank.inc"
#include "inc/ecel_bank.inc"
#include "inc/ecpc_bank.inc"
#include "inc/hcl0_bank.inc"
#include "inc/hcel_bank.inc"
#include "inc/muid_bank.inc"
#include "inc/rirs_bank.inc"
#include "inc/rirw_bank.inc"
#include "inc/tetc0_bank.inc"
#include "inc/thel0_bank.inc"
#include "inc/tmut0_bank.inc"
#include "inc/tvts1_bank.inc"
#include "inc/cbpass.inc"
#include "inc/axtk_bank.inc"
#include "inc/avmc_bank.inc"
#include "inc/atmc_bank.inc"
#include "inc/axvx_bank.inc"
#include "inc/pass_bank.inc"
#include "inc/ubmacro_para.inc"
C------------------------------------------------------------------
      INTEGER NOBJ1,NOBJ2,I,J,K,ICODE
     &       ,NOBJ10,NOBJ20,NOBJ30,NOBJ40,NOBJ50,NOBJ60
     &       ,NOBJ70,NOBJ80,NOBJ90,NOBJ100,NOBJ101,NOBJ102,NOBJ103
     &       ,IUVERS,IU,IPASS,PVZ
      PARAMETER (ICODE=73)
      REAL RUN,EVT
     &    ,IATMC,IAVMC,IAXTK,IAXVX
     &    ,IVSCP,IVSCR,IWDRW,IRIRS
     &    ,IRIRW,IECL0,IHCL0,IECPC
     &    ,IECEL,IHCEL
     &    ,IMUID,ITETC,ITHEL,ITMUT,ITVTS
     &    ,ZVERT
      LOGICAL FIRST / .TRUE. /
C----------------------------------------------------------------
#include "inc/ubmacro_func.inc"
#include "inc/vscp_func.inc"
#include "inc/vscr_func.inc"
#include "inc/wdrw_func.inc"
#include "inc/ecl0_func.inc"
#include "inc/ecel_func.inc"
#include "inc/ecpc_func.inc"
#include "inc/hcl0_func.inc"
#include "inc/hcel_func.inc"
#include "inc/muid_func.inc"
#include "inc/tetc0_func.inc"
#include "inc/thel0_func.inc"
#include "inc/tmut0_func.inc"
#include "inc/tvts1_func.inc"
#include "inc/rirs_func.inc"
#include "inc/rirw_func.inc"
#include "inc/axtk_func.inc"
#include "inc/avmc_func.inc"
#include "inc/atmc_func.inc"
#include "inc/axvx_func.inc"
#include "inc/pass_func.inc"
C------------------------------------------------------------------
 
C
      IF( FIRST ) THEN
        CALL INITSUCHECK()
        FIRST = .FALSE.
      ENDIF
 
      CALL HCDIR('//PAWC/CHCK',' ')
C
C
C++   Generator tracks: hists 101 - 199.
C
      NOBJ1 = N_ATMC()
      IATMC = FLOAT(NOBJ1)
      CALL HFILL(101,IATMC,0.,1.)
      NOBJ1 = N_AVMC()
      IAVMC = FLOAT(NOBJ1)
      CALL HFILL(102,IAVMC,0.,1.)
      IPASS = N_PASS()
      IF(IPASS.EQ.1) THEN
        ZVERT = PASS$PVZ(1)/10000.
        CALL HFILL(111,ZVERT,0.,1.)
      ENDIF
C
C++   Reconstructed tracks: hists 201 - 299.
C
      NOBJ2 = N_AXTK()
      IAXTK = FLOAT(NOBJ2)
      CALL HFILL(201,IAXTK,0.,1.)
      NOBJ2 = N_AXVX()
      IAXVX = FLOAT(NOBJ2)
      CALL HFILL(202,IAXVX,0.,1.)
      CALL caxtkg
      CALL naxtkg
C
C++   Si detectors: hists 1001 - 1999.
C
      NOBJ10 = N_VSCP()
      IVSCP = FLOAT(NOBJ10)
      CALL HFILL(1001,IVSCP,0.,1.)
      NOBJ10 = N_VSCR()
      IVSCR = FLOAT(NOBJ10)
      CALL HFILL(1002,IVSCR,0.,1.)
c--   iuvers VSIL
      iu = IUVERS('HIST','VSIL')
c     CALL simap
c     print *,'simap1',iu
      IF(IUVERS('HIST','VSIL').GE.1) THEN
      ENDIF
C
C++   Tracking stations: hists 2001 - 2999.
C
      NOBJ20 = N_WDRW()
      IWDRW = FLOAT(NOBJ20)
      CALL HFILL(2001,IWDRW,0.,1.)
c--   iuvers WDRF
      IF(IUVERS('HIST','WDRF').GE.1) THEN
c       CALL trmonitor
      ENDIF
C
C++   RICH detectors: hists 3001 - 3999.
C
      NOBJ30 = N_RIRS()
      IRIRS = FLOAT(NOBJ30)
      CALL HFILL(3001,IRIRS,0.,1.)
      NOBJ30 = N_RIRW()
      IRIRW = FLOAT(NOBJ30)
      CALL HFILL(3002,IRIRW,0.,1.)
c--   iuvers RICH
      IF(IUVERS('HIST','RICH').GE.1) THEN
      ENDIF
C
C++   PS detector: hists 4001 - 4999.
C
      NOBJ40 = N_ECPC()
      IECPC = FLOAT(NOBJ40)
      CALL HFILL(4001,IECPC,0.,1.)
      CALL TEST_PRES
c--   iuvers ECAL
      IF(IUVERS('HIST','ECAL').GE.1) THEN
      ENDIF
C
C++   1st sensisitive plane (ECAL): hists 5001 - 5999.
C
c--   iuvers ECAL
      IF(IUVERS('HIST','ECAL').GE.1) THEN
      ENDIF
C
C++   ECAL calorimeter: hists 6001 - 6999.
C
      NOBJ60 = N_ECL0()
      IECL0 = FLOAT(NOBJ60)
      CALL HFILL(6001,IECL0,0.,1.)
      NOBJ60 = N_ECEL()
      IECEL = FLOAT(NOBJ60)
      CALL HFILL(6002,IECEL,0.,1.)
      CALL TEST_ECAL
c--   iuvers ECAL
      IF(IUVERS('HIST','ECAL').GE.1) THEN
      ENDIF
C
C++   2nd sensisitive plane (HCAL): hists 7001 - 7999.
C
c--   iuvers HCAL
      IF(IUVERS('HIST','HCAL').GE.1) THEN
      ENDIF
C
C++   HCAL calorimeter: hists 8001 - 8999.
C
      NOBJ80 = N_HCL0()
      IHCL0 = FLOAT(NOBJ80)
      CALL HFILL(8001,IHCL0,0.,1.)
      NOBJ80 = N_HCEL()
      IHCEL = FLOAT(NOBJ80)
      CALL HFILL(8002,IHCEL,0.,1.)
      CALL TEST_HCAL
c--   iuvers HCAL
      IF(IUVERS('HIST','HCAL').GE.1) THEN
      ENDIF
C
C++   Muon stations: hists 9001 - 9999.
C
      NOBJ90 = N_MUID()
      IMUID = FLOAT(NOBJ90)
      CALL HFILL(9001,IMUID,0.,1.)
c--   iuvers MUCH
      IF(IUVERS('HIST','MUCH').GE.1) THEN
      ENDIF
C
C++   Trigger reconstruction: hists 10001 - 10099.
C
      NOBJ100 = N_TETC0()
      ITETC = FLOAT(NOBJ100)
      CALL HFILL(10001,ITETC,0.,1.)
      NOBJ101 = N_THEL0()
      ITHEL = FLOAT(NOBJ101)
      CALL HFILL(10101,ITHEL,0.,1.)
      NOBJ102 = N_TMUT0()
      ITMUT = FLOAT(NOBJ102)
      CALL HFILL(10201,ITMUT,0.,1.)
c--   iuvers TRIG
c     IF(IUVERS('HIST','TRIG').GE.1) THEN
c       NOBJ103 = N_TVTS1()
c       ITVTS = FLOAT(NOBJ103)
c       CALL HFILL(10301,ITVTS,0.,1.)
c       print *,'TVTS1 ',ITVTS
c       if(ITVTS.eq.1) print *,
c    &     TVTS1$FLAG(1),TVTS1$PILU(1),TVTS1$NTR(1),TVTS1$NSEC(1),
c    &     TVTS1$PROB(1)
c      call tvtsch
c     ENDIF
C
C++   Fill ntuple
C     if you uncomment the following lines, please uncomment
C     the line *#include "geant321/gcflags.inc" as well
C
C      RUN = IDRUN
C      EVT = IDEVT
c     CALL MHFN(2000,RUN,EVT
c    &    ,IATMC,IAVMC,IAXTK,IAXVX
c    &    ,IVSCP,IVSCR,IWDRW,IRIRS
c    &    ,IRIRW,IECL0,IHCL0,IECPC
c    &    ,IECEL,IHCEL
c    &    ,IMUID,ITETC,ITHEL,ITMUT,ITVTS)
 
C
 1001 format(1x,'------ Event Print ------')
 1002 format(1x,i5,i6,3f10.3)
 1011 format(1x,'------ B     Print ------',2i6)
 1012 format(1x,i5,i6,3f10.3)
 1013 format(1x,6f10.4)
C
 999  continue
C                                               !   end SUCHECK
      END
