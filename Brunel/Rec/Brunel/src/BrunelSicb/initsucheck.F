*-- Author :    A.Jacholkowska   03/10/98
      SUBROUTINE INITSUCHECK
*****************************************************************
*   Subroutine INITSUCHECK                                      *
*                                                               *
*   Initialisations for SICB DST checking algororithm SUCHECK   *
*                                                               *
*   called by: BrunelSICBChck                                   *
*                                                               *
*   Copy of SICBDST/dst/SUINIT by A.Jacholkowska                *
*****************************************************************
      IMPLICIT NONE
#include "inc/cbunit.inc"
*----------------------------------------------------------------
      INTEGER I,NSEL,J,K,L
      real rh,selec
      character*2 chst
      character*80 name
C----------------------------------------------------------------
      CHARACTER*4 TAG1(21)
      DATA TAG1/'RUN ','EVT ',
     &          'ATMC','AVMC','AXTK','AXVX',
     &          'VSCP','VSCR','WDRW','RIRS',
     &          'RIRW','ECL0','HCL0','ECPC',
     &          'ECEL','HCEL',
     &          'MUID','TETC','THEL','TMUT','TVTS'/
C----------------------------------------------------------------
C
C++   Open output Ntuple file.
C
      CALL UTOPEN('CHCK',LUNTPL)
      IF(LUNTPL.EQ.0) THEN
        WRITE(LUPRNT,100)
100     FORMAT(1X,' !!!!! SUINIT: Error opening N-tuple file')
        GOTO 999
      ENDIF
C
C++   Book hists. Note that you should not work in the PAWC root directory.
C++   The subdirectory has the same name as the output stream.
C
      CALL HCDIR('//PAWC/CHCK',' ')
C
C
      CALL HBOOKN(2000,'CHECK  ',21,' ',10000,TAG1)
C
C
C++   Print general event parameters from PASS bank.
C
C
C++   Generator tracks: hists 101 - 199.
C
      call hbook1(101,'No of ATMC banks',100,0.,5000.,0.)
      call hbook1(102,'No of AVMC banks',100,0.,4000.,0.)
      call hbook1(111,'Primary zvtx (PASS bank)',100,-60.,60.,0.)
C
C++   Reconstructed tracks: hists 201 - 299.
C
      call hbook1(201,'No of AXTK banks',100,0.,300.,0.)
      call hbook1(202,'No of AXVX banks',10,0.,10.,0.)
C
C
C++   Si detectors: hists 1001 - 1999.
C
      call hbook1(1001,'No of VSCP banks',100,0.,2000.,0.)
      call hbook1(1002,'No of VSCR banks',100,0.,2000.,0.)
C
C++   Tracking stations: hists 2001 - 2999.
C
      call hbook1(2001,'No of WDRW banks',100,0.,5000.,0.)
C
C++   RICH detectors: hists 3001 - 3999.
C
      call hbook1(3001,'No of RIRS banks',100,0.,200.,0.)
      call hbook1(3002,'No of RIRW banks',100,0.,1000.,0.)
C
C++   PS detector: hists 4001 - 4999.
C
      call hbook1(4001,'No of ECPC banks',100,0.,1000.,0.)
C
C++   1st sensisitive plane (ECAL): hists 5001 - 5999.
C
C
C++   ECAL calorimeter: hists 6001 - 6999.
C
      call hbook1(6001,'No of ECL0 banks',100,0.,100.,0.)
      call hbook1(6002,'No of ECEL banks',100,0.,10000.,0.)
C
C++   2nd sensisitive plane (HCAL): hists 7001 - 7999.
C
C
C++   HCAL calorimeter: hists 8001 - 8999.
C
      call hbook1(8001,'No of HCL0 banks',100,0.,50.,0.)
      call hbook1(8002,'No of HCEL banks',100,0.,1000.,0.)
C
C++   Muon stations: hists 9001 - 9999.
C
      call hbook1(9001,'No of MUID banks',100,0.,100.,0.)
C
C++   Trigger reconstruction: hists 10001 - 10099.
C
      call hbook1(10001,'No of TETC0 banks',30,0.,30.,0.)
      call hbook1(10101,'No of THEL0 banks',30,0.,30.,0.)
      call hbook1(10201,'No of TMUT0 banks',20,0.,20.,0.)
      call hbook1(10301,'No of TVTS1 banks',20,0.,20.,0.)
C
C
  999 CONTINUE
C                                               !   end SUINIT
      END
