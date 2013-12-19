C $Id: $
C ======================================================================
      SUBROUTINE ONIAPAIRSPRINTX 
C ======================================================================
      IMPLICIT NONE
C ======================================================================
C- Include Files
      include "pythia.inc"
      include "oniapairs.inc"
C ======================================================================
C- Local Variables
C ======================================================================
      INTEGER II
C ======================================================================

      Write(*,'(A)') 'LbOniaPairsProduction: Cross-section summary:' 

      DO II=1,20
         IF (SwOn(ii)) then
            if (CSsum(ii)/Nexec(ii)*1d-3.ge.1d-1)
     &           Write(*,'(A,A,I2,A,F8.3,A)') 
     &           '                       ',
     &           '*----------- Process #', II, 
     &           ' --  Cross-section  = ',
     &           CSsum(ii)/Nexec(ii)*1d-3, ' nb'
            if (CSsum(ii)/Nexec(ii)*1d-3.lt.1d-1) 
     &           Write(*,'(A,A,I2,A,F8.3,A)') 
     &           '                       ',
     &           'Process #', II, 
     &           ' --  Cross-section               = ',
     &           CSsum(ii)/Nexec(ii), ' pb'
            Write(*,'(A,A,F8.0)') 
     &           '                       ',
     &           '| Corresponding number of unweighted events = ',
     &           CSsum(ii)/WtMax(ii)
            Write(*,'(A,A,F8.5)') 
     &           '                       ',
     &           '| Generator level efficiency                = ',
     &           CSsum(ii)/(WtMax(ii)*Nexec(ii))            
            Write(*,'(A,A,F8.5)') 
     &           '                       ',
     &           '| Fraction of events with excessive weight  = ', 
     &           1D0*Nexcs(ii)/Nexec(ii)
         ENDIF
      ENDDO

      Write(*,'(A,A)') 
     &     '                       ',
     &     'Please note, these cross-sections do not account any cuts.'

      Write(*,'(A,A,A)') 
     &     '                       ',
     &     'No reasons to worry if fraction of events ',
     &     'with excessive weight it is not far above 0.01.'

      Write(*,'(A)') 'LbOniaPairsProduction: Goodbye from OniaPairs!'

  999 RETURN
      END
C ======================================================================
C The END 
C ======================================================================
