      SUBROUTINE DROPBANKS
*****************************************************************
*                                                               *
*   Subroutine DROPBANKS                                        *
*        Drop and compress banks for DST output                 *
*                                                               *
*   Author: M.Cattaneo, based on SICBDST/dst/suanal.F           *
*****************************************************************
      IMPLICIT NONE
*------------------------------------------------------------------
C      call ubdrop('E1RW')
      call ubdrop('E2RW')
      call ubdrop('E3RW')
      call ubdrop('E4RW')
      call ubdrop('H1RW')
      call ubdrop('RIDT')
      call ubpress('ECMT','R')
      call ubpress('HCMT','R')
*      
      END
