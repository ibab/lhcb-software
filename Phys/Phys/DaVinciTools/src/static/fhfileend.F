*
      SUBROUTINE FHFILEEND(name)
*****************************************************************
*                                                               *
*   Subroutine SULAST                                           *
*                                                               *
*                                                               *
*   called by:  UGLAST                                          *
*                                                               *
*   modified by:                                                *
*****************************************************************
      IMPLICIT NONE
      CHARACTER*4 name
      INTEGER ICYCLE
*----------------------------------------------------------------
*
      CALL HCDIR('//PAWC',' ')
      CALL HCDIR('//'//name,' ')
      call hrout(0,icycle,'T')
      call hrend(name)
* the following is only for testing
*      call histdo
*
      END
