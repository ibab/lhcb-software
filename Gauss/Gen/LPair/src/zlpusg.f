*CMZ :  1.00/01 15/12/94  09.37.15  by  ZEUS Offline Group
*CMZ :  1.00/00 15/12/94  08.49.43  by  ZEUS Offline Group
*-- Author :    Unknown   15/12/94
        Subroutine ZLPUSG
*       =================
*
*----------------------------------------------------------------------------
*
*    ZLPUSG
*    ======
*
*   Subroutine stores the generation parameters in the USGRUN table.
*
*   USGRUN_... definitions are given below:
*
*   USGRUN_Comments(1:2)  -> Brief description of Generator.
*   USGRUN_Comments(  4)  -> Version number.
*   USGRUN_Comments(  5)  -> Release date.
*
* ROW 1:
*   USGRUN_IPA( 1) - IBEG   ->
*   USGRUN_IPA( 2) - IEND   ->
*   USGRUN_IPA( 3) - NTreat ->
*   USGRUN_IPA( 4) - NPrin  ->
*   USGRUN_IPA( 5) - NCVG   ->
*   USGRUN_IPA( 6) - ITMX   ->
*   USGRUN_IPA( 7) - NPoin  ->
*   USGRUN_IPA( 8) - PMod   ->
*   USGRUN_IPA( 9) - GPDF*1000000+SPDF   ->
*   USGRUN_IPA(10) - EMOD   ->
*
*   USGRUN_RPA( 1) - INPP   ->
*   USGRUN_RPA( 2) - INPE   ->
*   USGRUN_RPA( 3) - THMax  ->
*   USGRUN_RPA( 4) - THMin  ->
*   USGRUN_RPA( 5) - ECUT   ->
*   USGRUN_RPA( 6) - PTCUT  ->
*   USGRUN_RPA( 7) - Q2MN   ->
*   USGRUN_RPA( 8) - Q2MX   ->
*   USGRUN_RPA( 9) - MXMN  ->
*   USGRUN_RPA(10) - MXMX   ->
*
* ROW 2:
*   USGRUN_IPA( 1) - IPair  -> Undefined.
*   USGRUN_IPA( 2) - NQuark -> Undefined.
*   USGRUN_IPA( 3) - ModCut -> Undefined.
*   USGRUN_IPA( 4) - ----   -> Undefined.
*   USGRUN_IPA( 5) - ----   -> Undefined.
*   USGRUN_IPA( 6) - ----   -> Undefined.
*   USGRUN_IPA( 7) - ----   -> Undefined.
*   USGRUN_IPA( 8) - ----   -> Undefined.
*   USGRUN_IPA( 9) - ----   -> Undefined.
*   USGRUN_IPA(10) - ----   -> Undefined.
*
*   USGRUN_RPA( 1) - ----   -> Undefined.
*   USGRUN_RPA( 2) - ----   -> Undefined.
*   USGRUN_RPA( 3) - ----   -> Undefined.
*   USGRUN_RPA( 4) - ----   -> Undefined.
*   USGRUN_RPA( 5) - ----   -> Undefined.
*   USGRUN_RPA( 6) - ----   -> Undefined.
*   USGRUN_RPA( 7) - ----   -> Undefined.
*   USGRUN_RPA( 8) - ----   -> Undefined.
*   USGRUN_RPA( 9) - ----   -> Undefined.
*   USGRUN_RPA(10) - ----   -> Undefined.
*
*---------------------------------------------------------------------------
*
        Implicit NONE
*
*KEEP,PARTAP.
C
C        system-defined parameters for the Table Package Programmer
C
      INTEGER MINC,MAXC,NEXT,INULL,IANY,INS,REP,ORD,UNO,AND,OR,DIF,HOR,
     +        VER,ALL,ID,ALLCOL,
     +        COUTAB,COUSEL,GETIND,GETSEL,GETPRO,GETDFL,GETTDF,SPATAB,
     +        CHKTAP,MAKTAB

      REAL    RNULL,RANY

      CHARACTER*4 CNULL,CANY

      LOGICAL BELSEL,BELTAB,CHKREL,CHKTAB,CHKWIN
C
C       MINC and MAXC for cursors operation
C
      PARAMETER (MINC=1, MAXC=2147483647)
C
C       null values
C
      PARAMETER (INULL=2147483647, RNULL= 699050*16.0**26, CNULL='====')
C
C       ANY values
C
      PARAMETER (IANY=-INULL, RANY=-RNULL, CANY='!@)(')
C
C       NEXT for insertion
C
      PARAMETER (NEXT=INULL)
C
C       Modes
C
      PARAMETER (INS = 1, REP = 2,
     +           ORD = 1, UNO = 2,
     +           AND = 1, OR  = 2, DIF=3,
     +           HOR = 1, VER = 2)
C
C       Indices
C
      PARAMETER (ALL = 1-INULL , ID = INULL-1, ALLCOL = 1-INULL)
*KEEP,BEAM.
      INTEGER          INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,IPAIR,NQUARK
      REAL*8           INPE,INPP
      COMMON /BEAM/    INPE,INPP,INTGE,INTGP,GPDF,SPDF,PMOD,EMOD,
     &                 IPAIR,NQUARK

*KEEP,CUTS.
      INTEGER      MODCUT
      REAL*4       THMAX,THMIN,MXMN,MXMX,Q2MN,Q2MX
      REAL*8       COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,QP2MIN,QP2MAX
      COMMON /CUTS/COTTH1,COTTH2,ECUT,PTCUT,MXMIN2,MXMAX2,
     &             THMAX,THMIN,QP2MIN,QP2MAX,MODCUT,MXMN,MXMX,Q2MN,Q2MX

*KEEP,VEGPAR.
      INTEGER          NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND
      COMMON /VEGPAR/  NDIM,NCVG,ITMX,NPRN,IGRAPH,
     &                 NPOIN,NPRIN,NTREAT,IBEG,IEND

*KEEP,USGRUN.
      INTEGER USGRun,USGRun_9999
      INTEGER USGRun_ID,USGRun_IPA
      CHARACTER*16 USGRun_Comment
      REAL USGRun_RPA

      COMMON/USGRun/USGRun,USGRun_ID,USGRun_Comment(5),USGRun_IPA(10),
     +   USGRun_RPA(10),USGRun_9999

*KEND.
*
C*  End of common
*
        Character*32 Version
*
*--------  Initialise:
*
*..  Clear USGRUN:
        Call NulWin(USGRUN)
*
*..  Load version number string:
*       Version = '12345678901234567890123456789012'
        Version = '                                '
        Write (Version, 9000)
9000    Format(
*KEEP,QFTITLE,N=31.
     + 31HZLPAIR   1.00/03  02/02/95  19.
*KEND.
     +         )
*
*--------  Fill USGRUN:
*
*..  Text:
*       USGRUN_Comment(*) = '1234567890123456'
        USGRUN_Comment(1) = 'ZLPAIR. DiLepton'
        USGRUN_Comment(2) = 's via 2-photons.'
        USGRUN_Comment(3) = '                '
        USGRUN_Comment(4) = Version( 1:16)
        USGRUN_Comment(5) = Version(17:32)
*
*..  Run parameters:
*
* ROW 1:
        USGRUN_IPA( 1) = IBEG
        USGRUN_IPA( 2) = IEND
        USGRUN_IPA( 3) = NTreat
        USGRUN_IPA( 4) = NPrin
        USGRUN_IPA( 5) = NCVG
        USGRUN_IPA( 6) = ITMX
        USGRUN_IPA( 7) = NPoin
        USGRUN_IPA( 8) = PMOD
        USGRUN_IPA( 9) = GPDF*100000+SPDF
        USGRUN_IPA(10) = EMOD
*
        USGRUN_RPA( 1) = INPP
        USGRUN_RPA( 2) = INPE
        USGRUN_RPA( 3) = THMax
        USGRUN_RPA( 4) = THMin
        USGRUN_RPA( 5) = ECUT
        USGRUN_RPA( 6) = PTCUT
        USGRUN_RPA( 7) = Q2MN
        USGRUN_RPA( 8) = Q2MX
        USGRUN_RPA( 9) = MXMN
        USGRUN_RPA(10) = MXMX
*
        USGRUN_Id      = Next
        Call InsTab(USGRUN)
*
* ROW 2:
        USGRUN_IPA( 1) = IPAIR
        USGRUN_IPA( 2) = NQUARK
        USGRUN_IPA( 3) = MODCUT
        USGRUN_IPA( 4) = INull
        USGRUN_IPA( 5) = INull
        USGRUN_IPA( 6) = INull
        USGRUN_IPA( 7) = INull
        USGRUN_IPA( 8) = INull
        USGRUN_IPA( 9) = INull
        USGRUN_IPA(10) = INull
*
        USGRUN_RPA( 1) = RNull
        USGRUN_RPA( 2) = RNull
        USGRUN_RPA( 3) = RNull
        USGRUN_RPA( 4) = RNull
        USGRUN_RPA( 5) = RNull
        USGRUN_RPA( 6) = RNull
        USGRUN_RPA( 7) = RNull
        USGRUN_RPA( 8) = RNull
        USGRUN_RPA( 9) = RNull
        USGRUN_RPA(10) = RNull
*
        USGRUN_Id = Next
        Call InsTab(USGRUN)
*
        Return
        End
