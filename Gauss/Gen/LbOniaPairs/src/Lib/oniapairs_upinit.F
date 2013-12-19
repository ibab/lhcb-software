      
      SUBROUTINE ONIAPAIRS_UPINIT ( IISET )
      
C...  No more this implicit trash
      IMPLICIT NONE
      
C...  Include Files
      include "pythia.inc"
      include "oniapairs.inc"
      
C...  Local arrays and parameters.
      LOGICAL FIRST
      DATA FIRST /.TRUE./
      SAVE FIRST 
      INTEGER ii
      
C-----------------------------------------------------------------
      
      IF ( FIRST.AND.IISET.EQ.1 ) THEN 
         WRITE(*,'(A)') 'LbOniaPairsProduction: Hello from OniaPairs!'
         WRITE(*,'(A,A)') '                       ',
     &        'ONIAPAIRS_UPINIT: First call'
         WRITE(*,'(A,A,F6.0,A)') '                       ',
     &        'Initialization for ', ECM, 'GeV pp-collisions.'
      ENDIF         
      
C-----------------------------------------------------------------
      
C...  Set up incoming beams.
      IDBMUP(1)= 2212
      IDBMUP(2)= 2212
      EBMUP(1) = 0.5D0*ECM 
      EBMUP(2) = 0.5D0*ECM
      
C...  Number of user processes. 
      NPRUP = 0

C...  Set up the external processes.
      do ii=1,20
         if (SwOn(ii).and.Kfct(ii).gt.0) then
            NPRUP = NPRUP + 1
            LPRUP(NPRUP) = ii
         endif
      enddo
      
C...  Decide on weighting strategy: unweighted on input.
      IDWTUP = 1

C...  Find some reasonable maximum for external process.
C...  (This is done by a cheating call to UPEVNT with special switch.)
      IF ( FIRST ) THEN 
         INIT=.TRUE.
         DO ii=1,NPRUP
            IDPRUP = LPRUP(ii)
            CALL UPEVNT
            XMAXUP(ii) = XWGTUP
            IF ( IISET.EQ.2 ) THEN
               Wtmax(IDPRUP) = XWGTUP*MaxWghtMult
               IF (ii.eq.1) WRITE(*,'(A,A)') 
     &              'LbOniaPairsProduction: ONIAPAIRS_UPINIT: ',
     &              'Determining maximum weights for subprocesses:'
               WRITE (*,'(A,A,I3,A,I3,A,F12.5)') 
     &              '                       ', 'XMAXUP[', ii,
     &              '(', IDPRUP, ')] = ', XWGTUP
            ENDIF
         ENDDO 
         INIT=.FALSE.
      ELSE
         DO ii=1,NPRUP
            XMAXUP(ii) = Wtmax(LPRUP(ii))
         ENDDO 
      ENDIF
      
C-----------------------------------------------------------------
      
      IF ( FIRST.AND.IISET.EQ.2 ) THEN 
         WRITE(*,'(A,A)') 'LbOniaPairsProduction: ONIAPAIRS_UPINIT: ', 
     &        'Initialization done.'
         FIRST = .FALSE.
      ENDIF         
      
C-----------------------------------------------------------------
      
      RETURN
      END
      
