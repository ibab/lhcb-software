*CMZ :          06/01/08            by  A.Shamov: UCOPYi
*CMZ :  3.00/01 18/05/93  15.10.50  by  ZEUS Offline Group
*-- Author :
      SUBROUTINE ZLFM73(JERR)
* =======================
*
*    Translate the contents of the JETSET 7.3 and PYTHIA 5.5 common
*                           into FMCKin and FMCVtx.
*
*KEEP,DISSTE.
*
*..  Common to hold steering parameter(s) for the DIS generator interface,
*        DISGenerator ... Name of generator user wants to run (No default).
*
        Character*8   DISGenerator, Institute
        Integer  NDISList ,IFList
        Parameter (NDISList=1)
*
      COMMON /DISSTE/ DISGenerator, Institute, IFList(NDISList)
*
*KEEP,PYTHIDIS.
*      control cards for interface to LEPTO 5.2 MC Generator
*  LEPTO cards:
*        Lepin ........ Lund63 code of incoming lepton (Default=7 e-)
*Electron_Mmtm ........ Electron beam momentum (Default = -30 GeV/c)
*  Proton_Mmtm ........ Proton beam momentum   )Default = 820 GeV/c)
*        Inter ........ Lepto interaction type (Default = NC = 4)
*
        Real Electron_Mmtm, Proton_Mmtm
        Integer Lepin, Inter
*
        Common/LEPDIS/ Lepin, Electron_Mmtm, Proton_Mmtm, Inter
*
* -------- FOR LUND MC --------
*
      INTEGER SCAT
      COMMON /PYTFLA/ SCAT
*
*KEEP,FMCEVT.
      INTEGER FMCEvt,FMCEvt_9999
      INTEGER FMCEvt_ID,FMCEvt_EvtNum,FMCEvt_MozRNDM,FMCEvt_Generator_
      CHARACTER*16 FMCEvt_Generator
      REAL FMCEvt_Weight

      COMMON/FMCEvt/FMCEvt,FMCEvt_ID,FMCEvt_EvtNum,FMCEvt_MozRNDM(2),
     +   FMCEvt_Weight,FMCEvt_Generator,FMCEvt_Generator_,FMCEvt_9999

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
*KEEP,FMCPCD.
      INTEGER FMCPCd,FMCPCd_9999
      INTEGER FMCPCd_ID,FMCPCd_StandCod,FMCPCd_HerCod,FMCPCd_IsaCod,
     +   FMCPCd_EuroCod,FMCPCd_LundCod,FMCPCd_GeantCod

      COMMON/FMCPCd/FMCPCd,FMCPCd_ID,FMCPCd_StandCod,FMCPCd_HerCod,
     +   FMCPCd_IsaCod,FMCPCd_EuroCod,FMCPCd_LundCod,FMCPCd_GeantCod,
     +   FMCPCd_9999

*KEEP,FMCPRT.
      INTEGER FMCPrt,FMCPrt_9999
      INTEGER FMCPrt_ID
      CHARACTER*16 FMCPrt_Name
      REAL FMCPrt_Mass,FMCPrt_Charge,FMCPrt_LifeTime

      COMMON/FMCPrt/FMCPrt,FMCPrt_ID,FMCPrt_Name,FMCPrt_Mass,
     +   FMCPrt_Charge,FMCPrt_LifeTime,FMCPrt_9999

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
*KEEP,GCUNIT.
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
*KEEP,ZDLUJETS.
      Integer N, K
      Real    P, V
      COMMON/LUJETS/N,K(4000,5),P(4000,5),V(4000,5)
      SAVE /LUJETS/
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
*KEEP,FMCRUN.
      INTEGER FMCRun,FMCRun_9999
      INTEGER FMCRun_ID,FMCRun_Date,FMCRun_Generator_
      CHARACTER*16 FMCRun_GenerName,FMCRun_Institute,FMCRun_Generator

      COMMON/FMCRun/FMCRun,FMCRun_ID,FMCRun_GenerName,FMCRun_Institute,
     +   FMCRun_Date(2),FMCRun_Generator,FMCRun_Generator_,FMCRun_9999

*KEEP,ZDPYPARS.
      INTEGER MSTP, MSTI
      REAL    PARP, PARI
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      SAVE /PYPARS/
*KEEP,ZDPYSUBS.
      INTEGER MSEL, MSUB, KFIN
      REAL    CKIN
      COMMON/PYSUBS/MSEL,MSUB(200),KFIN(2,-40:40),CKIN(200)
      SAVE /PYSUBS/
*
* =============  for  HERACLES ===================
*
*KEEP,USGEVT.
      INTEGER USGEvt,USGEvt_9999
      INTEGER USGEvt_ID,USGEvt_IPA
      REAL USGEvt_RPA

      COMMON/USGEvt/USGEvt,USGEvt_ID,USGEvt_IPA(10),USGEvt_RPA(10),
     +   USGEvt_9999

*KEEP,ZDSEVT.
*
*..  Event parameters for the DIS generator interface
*
      INTEGER NEVT,RUNNO,GRNDMQSEED
C
      COMMON /ZDSEVT/ NEVT,RUNNO,GRNDMQSEED(2)
*
*KEND.
*
      INTEGER     NKF3
      PARAMETER ( NKF3 = 25 )
      INTEGER     KF3(NKF3)
*
      Real lutoze, zetolu
      Parameter(lutoze=0.1,zetolu=10.)
*
      Integer indv
      Character*16 colnam(3)
      Character*4 colord(3)
      SAVE INDV
*
        Integer I, IC1, IC2, Indx, KS, KH ,ABSKI2 ,Lepton_Beam
      INTEGER JERR
*
        Logical Init , Found_Scattered_Lepton
        Save    Init, Indx
*
      DATA KF3   / 1, 2, 3, 4, 5, 6, 7, 8, 21,
     &             81, 82, 83, 90, 91, 92, 93,
     &             2101, 3101, 3201, 1103, 2103,
     &             2203, 3103, 3203, 3303
     &           /
*
        Data    Init/.False./
*
*------on first event, set up an index on particle code table FMCPCd
*
        If (.Not.Init) Then
            Init = .TRUE.
            Indx = GETIND(FMCPCd, 'StandCod')
*
*------on first event, set up an index on vertex table FMCVtx
*
            colnam(1) = 'R(1)'
            colnam(2) = 'R(2)'
            colnam(3) = 'R(3)'
*
            colord(1) = 'ASC'
            colord(2) = 'ASC'
            colord(3) = 'ASC'
*
            call CREIND(FMCVtx,indv,'VPos',
     +                  3,colnam(1),colord(1))
        EndIf
*
*--------  Fill FMCEvt and LPTEvt
*
        JERR=0
C
C --- SIMULATION OF EVENT FAILED IN GENERATOR => DO NOT WRITE OUT
C
C
        CALL NULWIN(USGEVT)
        USGEVT_ID = NEXT
*
        CALL UCOPYi( MSTI(1), USGEVT_IPA(1), 10)
        CALL UCOPY( PARI(1), USGEVT_RPA(1), 10)
        CALL InsTab(USGEvt)
*
        FMCEvt_Id         = Next
C --- event counter updated after safe return to main program
      FMCEVT_EVTNUM = NEVT + 1
        FMCEvt_Generator  = 'USGEvt'
        FMCEvt_Generator_ = USGEvt_Id
        CALL InsTab(FMCEvt)
*
* --- FUTHER ROWS ...
*
       DO 10 IC1=11,60,10
        CALL NULWIN(USGEVT)
        USGEVT_ID = NEXT
        CALL UCOPYi( MSTI(IC1), USGEVT_IPA(1), 10)
        CALL UCOPY( PARI(IC1), USGEVT_RPA(1), 10)
        CALL INSTAB(USGEVT)
   10 CONTINUE
*
*
*************************************************************
*               LOOP TO FILL FMCKin / FMCVtx                *
*************************************************************
*
*
*
        Found_Scattered_Lepton = .False.
*
        Do 600 I=1, N
*
*-----fill in the kinematic variables
*
            If (I.Eq.1) Then
                CALL NULWIN(FMCKin)
                Lepton_Beam = 0
            EndIf
*
            FMCKIN_ID         = I
            FMCKin_PX         = P(I,1)
            FMCKin_PY         = P(I,2)
            FMCKin_PZ         = P(I,3)
            FMCKin_Energy     = P(I,4)
            FMCKin_Mass       = P(I,5)
            FMCKin_Decay      = .TRUE.
            KS                = K(I,1)
            KF                = K(I,2)
            KH                = K(I,3)
            IF( KS.EQ.21 ) THEN
                IF( I.LE.2 ) THEN
                  FMCKin_ISTHEP = 3
                ELSE
                  FMCKin_ISTHEP = 11
                ENDIF
            ELSEIF( KS.EQ.12 .OR. KS.EQ.13 ) THEN
              FMCKin_ISTHEP   = 11
            ELSEIF( KS.EQ.11 ) THEN
              IF( IUCOMP( ABS( KF ), KF3, NKF3 ).NE.0 ) THEN
                FMCKin_ISTHEP = 11
              ELSE
                FMCKin_ISTHEP = 2
              ENDIF
            ELSE
              FMCKin_ISTHEP   = 1
              FMCKin_decay    = .FALSE.
            ENDIF
*
*------insert the parent particle of the particle in question
*
            FMCKin_DaughterOf = KH
            If (KH.Eq.0) FMCKin_DaughterOf = INull
*
*..  Store the location of the lepton beam:
            IF( KS.EQ.21 .AND. KF.EQ.LEPIN .AND
     &         . Lepton_Beam.EQ.0 )                    Then
                Lepton_Beam = I
            EndIf
*
*..  Rescue the scattered lepton:
            If ( (.Not.Found_Scattered_Lepton)    .And.
     +                     KS.NE.21               .And.
     +               ABS( KF ).Eq.11           )      Then
*
*..  Got the scattered lepton:
                Found_Scattered_Lepton = .True.
                FMCKin_DaughterOf = Lepton_Beam
*
            EndIF
*
*-------insert the production vertex of the particle in question
*
*
            If (I.eq.1) Then
                Call NULWIN(FMCVtx)
                FMCVtx_ID         = Next
                FMCVtx_X          = VERTEX(1)
                FMCVtx_Y          = VERTEX(2)
                FMCVtx_Z          = VERTEX(3)
                FMCVtx_Type       = DISGenerator
                FMCVtx_PRoducedBy = FMCKin_Id
                Call INSTAB(FMCVtx)
            EndIf
*
*
*-------Insert the vertex where the particle was produced. No CALL to
*       FETTAB is needed because the FMCKin/FMCVtx WINs are in the corre
*       place
*
            FMCKin_PRoducedAt = FMCVtx_ID
*
*-------The ID of the particle in FMCPrt must be found
*       The index on FMCPCd is set up before first event
*       If its cursors are ok, insert all the attributes/relationships
*       so far in FMCKin
*
            FMCPCd_StandCod= KF
            CALL SELTAB(FMCPCd,Indx,IC1,IC2)
            If (IC1.eq.IC2) then
                FMCKin_FMCPrt = FMCPCd_Id
            Else
                Write (CHMAIL, 9000)
                CALL GMail(0, 0)
            EndIf
*
            CALL INSTAB(FMCKin)
*
  600   CONTINUE
*
9000    Format('ZLFM73. WARNING - FMCPCd cursors not equal')
*
        end
