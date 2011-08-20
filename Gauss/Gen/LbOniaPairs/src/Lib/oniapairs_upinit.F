
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
      INTEGER ii

C-----------------------------------------------------------------

      IF ( FIRST.AND.IISET.EQ.1 ) THEN 
        WRITE(*,*)'LbOniaPairsProduction: ONIAPAIRS_UPINIT: First call'
      ENDIF         

C-----------------------------------------------------------------

C...Set up incoming beams.
      IDBMUP(1)= 2212
      IDBMUP(2)= 2212
      EBMUP(1) = 0.5D0*ECM 
      EBMUP(2) = 0.5D0*ECM

C...Number of users processes. 
      NPRUP = 0
C...Set up the external processes.
      do ii=1,20
        if (SwOn(ii)) then
          NPRUP = NPRUP + 1
          LPRUP(NPRUP) = ii
        endif
      enddo

C...Decide on weighting strategy: unweighted on input.
      IDWTUP   = 1

C...Find some reasonable maximum for external process.
C...(This is done by a cheating call to UPEVNT with special switch.)
      IF ( FIRST ) THEN 
        INIT=.TRUE.
        DO ii=1,NPRUP
          IDPRUP = LPRUP(ii)
          CALL UPEVNT
          XMAXUP(ii) = XWGTUP
          IF (IISET.EQ.2) THEN
            Wtmax(ii) = XWGTUP
            WRITE (*,*)
     &           'LbOniaPairsProduction: ONIAPAIRS_UPINIT: XMAXUP(',
     &            IDPRUP, ') = ', XWGTUP
          ENDIF
        ENDDO 
        INIT=.FALSE.
      ELSE
        DO ii=1,NPRUP
          XMAXUP(ii) = Wtmax(ii)
        ENDDO 
      ENDIF

C-----------------------------------------------------------------

      IF ( FIRST.AND.IISET.EQ.2 ) THEN 
        FIRST = .FALSE.
      ENDIF         

C-----------------------------------------------------------------

      RETURN
      END
