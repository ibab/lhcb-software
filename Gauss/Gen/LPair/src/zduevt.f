*CMZ :  1.00/00 29/08/94  18.35.58  by  ZEUS Offline Group
*CMZ :  3.00/01 18/05/93  15.10.49  by  ZEUS Offline Group
*-- Author :
      SUBROUTINE ZDUEVT(IWANT)
*     ========================
*
*------------------------------------------------------------------------
*
*  ZDUEVT: Adminstrate event generation by LPair.
*  =======
*
*------------------------------------------------------------------------
*
        Implicit NONE
*
*KEEP,EXTRA.
      REAL*8          S1,S2,T1,T2
      COMMON /EXTRA/  S1,S2,T1,T2
*KEEP,FMCKIN.
      INTEGER FMCKin,FMCKin_9999
      INTEGER FMCKin_ID,FMCKin_ISTHEP,FMCKin_DaughterOf,FMCKin_FMCPrt,
     +   FMCKin_PRoducedAt
      REAL FMCKin_P
      LOGICAL FMCKin_Decay

      COMMON/FMCKin/FMCKin,FMCKin_ID,FMCKin_P(5),FMCKin_Decay,
     +   FMCKin_ISTHEP,FMCKin_DaughterOf,FMCKin_FMCPrt,
     +   FMCKin_PRoducedAt,FMCKin_9999

      REAL                      FMCKin_PX
      EQUIVALENCE              (FMCKin_PX,
     +                          FMCKin_P(1))
      REAL                      FMCKin_PY
      EQUIVALENCE              (FMCKin_PY,
     +                          FMCKin_P(2))
      REAL                      FMCKin_PZ
      EQUIVALENCE              (FMCKin_PZ,
     +                          FMCKin_P(3))
      REAL                      FMCKin_Energy
      EQUIVALENCE              (FMCKin_Energy,
     +                          FMCKin_P(4))
      REAL                      FMCKin_Mass
      EQUIVALENCE              (FMCKin_Mass,
     +                          FMCKin_P(5))
*KEEP,FMCVTX.
      INTEGER FMCVtx,FMCVtx_9999
      INTEGER FMCVtx_ID,FMCVtx_PRoducedBy
      CHARACTER*8 FMCVtx_Type
      REAL FMCVtx_R,FMCVtx_ProdTime

      COMMON/FMCVtx/FMCVtx,FMCVtx_ID,FMCVtx_R(3),FMCVtx_ProdTime,
     +   FMCVtx_Type,FMCVtx_PRoducedBy,FMCVtx_9999

      REAL                      FMCVtx_X
      EQUIVALENCE              (FMCVtx_X,
     +                          FMCVtx_R(1))
      REAL                      FMCVtx_Y
      EQUIVALENCE              (FMCVtx_Y,
     +                          FMCVtx_R(2))
      REAL                      FMCVtx_Z
      EQUIVALENCE              (FMCVtx_Z,
     +                          FMCVtx_R(3))
*KEEP,FMCEVT.
      INTEGER FMCEvt,FMCEvt_9999
      INTEGER FMCEvt_ID,FMCEvt_EvtNum,FMCEvt_MozRNDM,FMCEvt_Generator_
      CHARACTER*16 FMCEvt_Generator
      REAL FMCEvt_Weight

      COMMON/FMCEvt/FMCEvt,FMCEvt_ID,FMCEvt_EvtNum,FMCEvt_MozRNDM(2),
     +   FMCEvt_Weight,FMCEvt_Generator,FMCEvt_Generator_,FMCEvt_9999

*KEEP,USGEVT.
      INTEGER USGEvt,USGEvt_9999
      INTEGER USGEvt_ID,USGEvt_IPA
      REAL USGEvt_RPA

      COMMON/USGEvt/USGEvt,USGEvt_ID,USGEvt_IPA(10),USGEvt_RPA(10),
     +   USGEvt_9999

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
*KEND.
*
C*  End of common
*
      integer nextw_gmufil
      common /gmufilcb/ nextw_gmufil
      save /gmufilcb/

        Integer IWant, JErr, NPri
*
        Data NPri/0/
*
*--------  Initialise
*
*..  Want the event is the default:
        IWant = 1
*
*--------  Generate the event:
*
*..  Generate the event with LPair:

        if(nextw_gmufil.le.1) print *,'calling GMUGNA'
        Call GMUGNA
*
*..  Fill /LUJETS/
        Call GMUFIL
*
*..  Build FMCKin/FMCEVT:
!-        Call ZLFM73(JErr)
*
*..  Fill the USGEVT table:
!-        Call CleTab(USGEvt)
!-        Call NulWin(USGEVT)
!-        USGEVT_Id     = 1
!-        USGEVT_RPA(1) = T1
!-        USGEVT_RPA(2) = T2
!-        Call InsTab(USGEVT)
*
*..  Print the event:
!-        If (NPri.LT.10) Then
!-            NPri = NPri + 1
!-            Call PriTab(FMCEVT, Id, MinC, MaxC, AllCOl)
!-            Call PriTab(FMCVTX, Id, MinC, MaxC, AllCOl)
!-            Call PriTab(FMCKIN, Id, MinC, MaxC, AllCOl)
!-            Call PriTab(USGEVT, Id, MinC, MaxC, AllCol)
!-        EndIf
*
        End
