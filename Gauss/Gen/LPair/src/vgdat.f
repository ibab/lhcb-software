*CMZ :  1.00/00 16/01/92  17.24.06  by  O. Duenger
*CMZ :  0.01/00 14/01/92  13.12.30  by  O. Duenger
*CMZ :  0.00/00 18/12/91  16.56.41  by  O. Duenger
*-- Author :    O. Duenger   17/12/91
      SUBROUTINE VGDAT
C
C  INITIAL SETTING OF THE IO CHANNELS
C  ALL INPUT CHANNELS ARE SET TO 5 ( STANDARD INPUT ) FOR VEGAS AND INPL
C  ALL OUTPUT CHANNELS ARE SET TO 6 ( STANDARD OUTPUT ) FOR VEGAS AND IN
C  INPUT AND OUTPUT CHANNELS FOR SAVE AND RESTR ARE SET TO 7
C  INPUT AND OUTPUT CHANNELS FOR SAVE2 AND RESTR2 ARE SET TO 9
C  INPUT AND OUTPUT CHANNELS FOR VGSAVE AND VGRSTR ARE SET TO 8
C  INPUT AND OUTPUT CHANNELS FOR THE RANDOM NUMBER GENERATOR STATE IS SE
C  AN ADDITIONAL IO CHANNEL IS SET TO 18 ( SO THIS CHANNEL IS RESERVED F
C                                         ALSO )
C
C  AUTHOR : S. DE JONG
C
      COMMON/VGASIO/NINP,NOUTP
      COMMON/VGPLIO/NIN,NOUT
      COMMON/VGSAV/LUN1,LUN2,LUN3,LUN4,LUN5
C
      LOGICAL FIRST
C
      DATA FIRST /.TRUE./
C
      IF(FIRST)THEN
         FIRST=.FALSE.
         NINP =5
         NIN  =5
         NOUTP=6
         NOUT =6
         LUN1 =21
         LUN2 =22
         LUN3 =8
         LUN4 =17
         LUN5 =18
      ENDIF
C
      RETURN
C
      END
