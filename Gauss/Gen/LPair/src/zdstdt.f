*CMZ :  3.00/00 18/05/93  15.01.43  by  ZEUS Offline Group
*-- Author :
        Block Data ZDSTDT
* =======================
*
*.. Load GCUNIT with apropriate o/p unit:
*
*KEEP,GCUNIT.
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
*KEEP,DISEED.
*
*..  Common to hold random number seeds:
*
        Integer    NDISSeed
        Parameter (NDISSeed=2)
*
        Integer         DISSeed1, DISSeed2 ,LUNRAN ,IRND_CHAIN
*
        Common /DISEED/ DISSeed1, DISSeed2 ,LUNRAN ,IRND_CHAIN
*
*KEND.
*
        Data LOUT/6/
        DATA LUNRAN /10/
*
        End
