C $Id: initpyblock.F,v 1.1.1.1 2005-06-21 12:40:51 robbep Exp $
      SUBROUTINE INITPYBLOCK(LULIS,FILLIS)
      IMPLICIT NONE
 
      INTEGER LULIS,IERR
      CHARACTER*(*) FILLIS

C
C          Open for write the pythia output file FILLIS
C          ====================================
C    
      IF(LULIS.GT.0)THEN
        OPEN ( UNIT=LULIS, FORM='FORMATTED',
     &    FILE=FILLIS,STATUS='NEW',IOSTAT=IERR)
        IF(IERR.NE.0)   THEN
          WRITE(6,*)
     &      'Error ', IERR,' failing to open PYTHIA Listing file' ,
     &      FILLIS(1:LEN(FILLIS))
          RETURN
        ELSE
          WRITE(6,*)
     &      'Open PYTHIA Listing  file ' , FILLIS(1:LEN(FILLIS))
        ENDIF
      ENDIF
      RETURN
      END
