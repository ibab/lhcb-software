*-- Author :    Peter Schleper   27/03/97
      SUBROUTINE TRIGGER
*****************************************************************
*   Brunel PRIVATE copy of triskel/trigger.F
*****************************************************************
*
*   Subroutine TRIGGER
*   DD : TRIGGER: main trigger routine
*   in : digi step output
*   out: trigger banks
*
*   called by: RECOGO
*
*   Author: Peter Schleper,   27/03/97   15.07.44
*   modified by: Ian Tomalin  12/1/98    Call L2 routines
*   modified by: M.Cattaneo   2001-05-08 Remove call to T1SEED, T1TRACK
*****************************************************************
      IMPLICIT NONE
#include "inc/cbunit.inc"
*23456789!-------------------------------------------------------
         INTEGER IRECO,IDEBG,IPRNT,IHIST
         SAVE    IRECO,IDEBG,IPRNT,IHIST
*
         INTEGER    IUVERS,LUN
*
         INTEGER ISTART
         DATA ISTART /1/
         SAVE ISTART
*23456789!-------------------------------------------------------
 
         CALL PTIME('TRIGGER ',1,1)
 
*** init
         IF(ISTART.eq.1) THEN
            ISTART = 0
 
*** print summary of trigger steering
            CALL TiniCHECK
*** check steering
            IRECO = IUVERS('RECO','TRIG')
            IDEBG = IUVERS('DEBG','TRIG')
            IPRNT = IUVERS('PRNT','TRIG')
            IHIST = IUVERS('HIST','TRIG')

*** set vdet geometry
            call vrpigeom
 
*** HBOOK output stream for trigger
            IF(IRECO.gt.0 .and. IHIST.gt.0)  THEN
               CALL UTOPEN('TRIG',LUN)
               IF(LUN.LE.0) THEN
                  WRITE(*,*) 'TRIGGER: error opening HBOOK stream TRIG'
               END IF
            END IF
             
         END IF
*** for each event *****************************************
 
         IF(IRECO.le.0) GOTO 99
 
 
*** HBOOK directory
         IF(IHIST.gt.0)  THEN
           CALL HCDIR('//PAWC/TRIG',' ')
           CALL HCDIR('//TRIG',' ')
         ENDIF
 
*** LEVEL 0 ************************************************
 
*** Muon
         CALL PTIME('T0MUON  ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T0MUON'
         CALL T0MUON
         CALL PTIME('T0MUON  ',2,1)
*** Ecal
         CALL PTIME('T0ECAL  ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T0ECAL'
         CALL T0ECAL
         CALL PTIME('T0ECAL  ',2,1)
*** Hcal
         CALL PTIME('T0HCAL  ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T0HCAL'
         CALL T0HCAL
         CALL PTIME('T0HCAL  ',2,1)
*** Pileup (the silicon one)
         CALL PTIME('T0VERTEX',1,1)
         IF(IPRNT.GE.3) WRITE(*,1000) 'T0VERTEX'
         CALL T0VERTEX
         CALL PTIME('T0VERTEX',2,1)
*** Pileup (calorimeter)
         CALL PTIME('T0PILEUP',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T0PILEUP'
         CALL T0PILEUP
         CALL PTIME('T0PILEUP',2,1)
*** 2x2 Calorimeters
         CALL PTIME('T2X2    ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T2X2'
         CALL L0_2x2
         CALL PTIME('T2X2    ',2,1)
 
*** LEVEL 1 ************************************************
 
*** Tracking
         CALL PTIME('T1TRACK ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T1SEED'
         CALL T1SEED
C         IF(IPRNT.ge.3) WRITE(*,1000) 'T1TRACK'
C         CALL T1TRACK
C         CALL PTIME('T1TRACK ',2,1)
 
*** Vertex
         CALL PTIME('T1VERTEX',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T1VERTEX'
         CALL T1VERTEX
         CALL PTIME('T1VERTEX',2,1)
 
*** LEVEL 2 ************************************************
 
*** Tracking
C         CALL PTIME('T2TRACK ',1,1)
C         IF(IPRNT.ge.3) WRITE(*,1000) 'T2SEED'
C         CALL DSSEED
C         IF(IPRNT.ge.3) WRITE(*,1000) 'AXTDRIVE'
C         CALL AXTDRIVE
C         CALL PTIME('T2TRACK ',2,1)
 
*** Vertex
         CALL PTIME('T2VERTEX',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'T2VERTEX'
         CALL T2VERTEX
         CALL PTIME('T2VERTEX',2,1)
 
*** LEVEL 3 ************************************************
 
*** print event summary
         CALL PTIME('TevSUM  ',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'TevSUM'
         CALL TevSUM
         CALL PTIME('TevSUM  ',2,1)
 
*** histograms and n-tuples
         CALL PTIME('TRIGhist',1,1)
         IF(IPRNT.ge.3) WRITE(*,1000) 'TRIGhist'
         CALL TRIGhist
         CALL PTIME('TRIGhist',2,1)
 
*** update trigger statistics at end of run ****************
*** print trigger statistics
*        this is done by a call to  TRlast  at the end of the job
 
         CALL PTIME('TRIGGER ',2,1)
         RETURN
1000     FORMAT('TRIGGER: call module ',A10,' ***********************')
 
99       CONTINUE
         END
