*
      SUBROUTINE SUANAL
*****************************************************************
*                                                               *
*   Subroutine SUANAL                                           *
*                                                               *
*   User entry for the event analysis, sample of how to         *
*   retrieve info from the AXSEL package when running           *
*   AxSelectAlgortihm as toplevel algorithm                     *
*                                                               *
*   called by:  DaVinciFinalisation                             *              *
*                                                               *
*****************************************************************
*
      IMPLICIT NONE
*
#include "inc/cbbank.inc"
#include "inc/cbunit.inc"
#include "inc/ubmacro_para.inc"
#include "inc/pass_bank.inc"
#include "inc/atmc_bank.inc"
*------------------------------------------------------------------
*
* Local variables
      INTEGER npasso
      LOGICAL MCFlag,SELFlag,TRKFlag,PQUALFlag,AGRFlag,TAGFlag
      INTEGER I, ILUND, natmc
      LOGICAL IPRINT
      DATA IPRINT/.FALSE./
*
*------------------------------------------------------------------
#include "inc/ubmacro_func.inc"
#include "inc/pass_func.inc"
#include "inc/atmc_func.inc"
*------------------------------------------------------------------
*
      IF( IPRINT ) THEN
        WRITE(luprnt,*) '**********************************************'
        WRITE(LUPRNT,*) ' Testing Accessing AXSEL results from fortran '
        npasso = N_PASS()
        IF (npasso.ge.1) THEN
          WRITE(luprnt,*) '  EVTYPE   = ', PASS$EVTYPE(1)
        ENDIF
      ENDIF

      CALL AXSELRESULTS(MCFlag,TRKFlag,PQUALFlag,
     &                  SELFlag,AGRFlag,TAGFlag)
C
      IF( IPRINT ) THEN
        WRITE(LUPRNT,*) '  MCFlag =', MCFlag
        WRITE(LUPRNT,*) '  TRKFlag = ', TRKFlag 
        WRITE(LUPRNT,*) '  PQUALFlag = ', PQUALFlag
        WRITE(LUPRNT,*) '  SELFlag = ', SElFlag
        WRITE(LUPRNT,*) '  AGRFlag = ', AGRFlag
        WRITE(LUPRNT,*) '  TAGFlag = ', TAGFlag
        WRITE(luprnt,*) '**********************************************'
C
C      NATMC = N_ATMC()
C      DO I = 1,NATMC
C        ILUND = ATMC$IPRT(I)
C        IF( (ILUND.GE.71) .and. (ilund.le.78) ) THEN
C          CALL PRINT_ATMC_TREE(LUPRNT,ID_ATMC(I),'H')
C        ENDIF
C      ENDDO
      ENDIF
C                                               !   end SUANAL
      RETURN
      END
