C $Id: HidValley.inc,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
C =====================================================================
C  @file 
C  Onia pairs production 
C
C  @author:  Alexey Novoselov Alexey.Novoselov@cern.ch
C  @date: 2011-05-30
C
C  $Revision$
C  Last Modification $Date$
C  by $Author$
C ===================================================================== 
      SUBROUTINE ONIAPAIRS_UPINIT ( IISET )

C...No more this implicit trash
      IMPLICIT NONE

C...Include Files
      include "pythia.inc"
      include "oniapairs.inc"

C...Local arrays and parameters.
      LOGICAL FIRST
      DATA FIRST /.TRUE./
      SAVE FIRST 
      INTEGER iproc,ii,IDC

C-----------------------------------------------------------------

      IF ( FIRST.AND.IISET.EQ.1 ) THEN 
        WRITE(*,*)'LbOniaPairsProduction: ONIAPAIRS_UPINIT: First call'
C        CALL PYLIST(12)
      ENDIF         

C-----------------------------------------------------------------

C...Set up incoming beams.
      IDBMUP(1)= 2212
      IDBMUP(2)= 2212
      EBMUP(1) = 0.5D0*ECM 
      EBMUP(2) = 0.5D0*ECM

      upsel = 1 + 2

      IF (IISET.EQ.1) THEN
C...Number of users processes. 
        NPRUP = 0
C...Set up the external processes.
        do ii=1,4
          if (mod(upsel,2).eq.1) then
            NPRUP = NPRUP + 1
            if (ii.eq.1) then
              LPRUP(NPRUP) = 1001
            elseif (ii.eq.2) then
              LPRUP(NPRUP) = 1002
            elseif (ii.eq.3) then
              LPRUP(NPRUP) = 1003
            elseif (ii.eq.4) then
              LPRUP(NPRUP) = 1004
            endif          
          endif
          upsel=upsel/2;
        enddo
      ENDIF

C...Decide on weighting strategy: unweighted on input.
      IDWTUP   = 1

C...Find some reasonable maximum for external process.
C...(This is done by a cheating call to UPEVNT with special switch.)
      IF ( FIRST ) THEN 
        INIT=.TRUE.
        DO iproc=1,NPRUP
          IDPRUP = LPRUP(iproc)
          CALL UPEVNT
          XMAXUP(iproc) = XWGTUP
          IF (IISET.EQ.2) THEN
            Wtmax(iproc) = XWGTUP
            WRITE (*,*)
     &           'LbOniaPairsProduction: ONIAPAIRS_UPINIT: XMAXUP(',
     &            IDPRUP, ') = ', XWGTUP
          ENDIF
        ENDDO 
        INIT=.FALSE.
        IF (IISET.EQ.2) FIRST = .FALSE.
      ELSE
        DO iproc=1,NPRUP
          XMAXUP(iproc) = Wtmax(iproc)
        ENDDO 
      ENDIF

      RETURN
      END
