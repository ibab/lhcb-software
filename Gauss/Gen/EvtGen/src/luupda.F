C********************************************************************* 
 
      SUBROUTINE LUUPDA(MUPDA,LFN) 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      INTEGER PYK,PYCHGE,PYCOMP
C...Commonblocks.
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON/PYDAT4/CHAF(500,2)
      CHARACTER CHAF*16
      COMMON/PYINT4/MWID(500),WIDS(500,5)
      SAVE /PYDAT1/,/PYDAT2/,/PYDAT3/,/PYDAT4/,/PYINT4/
 
C...Purpose: to facilitate the updating of particle and decay data. 
      CHARACTER CHINL*80,CHKC*4,CHVAR(19)*9,CHLIN*72, 
     &CHBLK(20)*72,CHOLD*12,CHTMP*12,CHNEW*12,CHCOM*12 
      DATA CHVAR/ 'KCHG(I,1)','KCHG(I,2)','KCHG(I,3)','PMAS(I,1)', 
     &'PMAS(I,2)','PMAS(I,3)','PMAS(I,4)','MDCY(I,1)','MDCY(I,2)', 
     &'MDCY(I,3)','MDME(I,1)','MDME(I,2)','BRAT(I)  ','KFDP(I,1)', 
     &'KFDP(I,2)','KFDP(I,3)','KFDP(I,4)','KFDP(I,5)','CHAF(I)  '/ 
 
        DO 130 I=1,MSTU(7) 
        MDME(I,1)=1 
        MDME(I,2)=0 
        BRAT(I)=0. 
        DO 120 J=1,5 
        KFDP(I,J)=0 
  120   CONTINUE 
  130   CONTINUE 
        KC=0 
        IDC=0 
        NDC=0 
  140   READ(LFN,5200,END=150) CHINL 
        IF(CHINL(2:5).NE.'    ') THEN 
          CHKC=CHINL(2:5) 
          IF(KC.NE.0) THEN 
            MDCY(KC,2)=0 
            IF(NDC.NE.0) MDCY(KC,2)=IDC+1-NDC 
            MDCY(KC,3)=NDC 
          ENDIF 
          READ(CHKC,5300) KC 
          IF(KC.LE.0.OR.KC.GT.MSTU(6)) CALL PYERRM(27, 
     &    '(PYUPDA:) Read KC code illegal, KC ='//CHKC) 
          READ(CHINL,5000) KCR,CHAF(KC,1),(KCHG(KC,J1),J1=1,3), 
     &    (PMAS(KC,J2),J2=1,4),MDCY(KC,1) 
          NDC=0 
        ELSE 
          IDC=IDC+1 
          NDC=NDC+1 
          IF(IDC.GE.MSTU(7)) CALL PYERRM(27, 
     &    '(PYUPDA:) Decay data arrays full by KC ='//CHKC) 
          READ(CHINL,5100) MDME(IDC,1),MDME(IDC,2),BRAT(IDC), 
     &    (KFDP(IDC,J),J=1,5) 
        ENDIF 
        GOTO 140 
  150   MDCY(KC,2)=0 
        IF(NDC.NE.0) MDCY(KC,2)=IDC+1-NDC 
        MDCY(KC,3)=NDC 
 
C...Perform possible tests that new information is consistent.
        DO 220 KC=1,MSTU(6)
          KF=KCHG(KC,4)
          IF(KF.EQ.0) GOTO 220
          WRITE(CHKC,5300) KF
          IF(MIN(PMAS(KC,1),PMAS(KC,2),PMAS(KC,3),PMAS(KC,1)-PMAS(KC,3),
     &    PMAS(KC,4)).LT.0D0.OR.MDCY(KC,3).LT.0) CALL PYERRM(17,
     &    '(PYUPDA:) Mass/width/life/(# channels) wrong for KF ='//CHKC)
          BRSUM=0D0
          DO 210 IDC=MDCY(KC,2),MDCY(KC,2)+MDCY(KC,3)-1
            IF(MDME(IDC,2).GT.80) GOTO 210
            KQ=KCHG(KC,1)
            PMS=PMAS(KC,1)-PMAS(KC,3)-PARJ(64)
            MERR=0
            DO 200 J=1,5
              KP=KFDP(IDC,J)
              IF(KP.EQ.0.OR.KP.EQ.81.OR.IABS(KP).EQ.82) THEN
                IF(KP.EQ.81) KQ=0
              ELSEIF(PYCOMP(KP).EQ.0) THEN
                MERR=3
              ELSE
                KQ=KQ-PYCHGE(KP)
                KPC=PYCOMP(KP)
                PMS=PMS-PMAS(KPC,1)
                IF(MSTJ(24).GT.0) PMS=PMS+0.5D0*MIN(PMAS(KPC,2),
     &          PMAS(KPC,3))
              ENDIF
  200       CONTINUE
            IF(KQ.NE.0) MERR=MAX(2,MERR)
            IF(MWID(KC).EQ.0.AND.KF.NE.311.AND.PMS.LT.0D0)
     &      MERR=MAX(1,MERR)
            IF(MERR.EQ.3) then 
              CALL PYERRM(17,
     &      '(PYUPDA:) Unknown particle code in decay of KF ='//CHKC)
             Write(6,*) ' KP: ',kp
            endif
            IF(MERR.EQ.2) CALL PYERRM(17,
     &      '(PYUPDA:) Charge not conserved in decay of KF ='//CHKC)
            IF(MERR.EQ.1) CALL PYERRM(7,
     &      '(PYUPDA:) Kinematically unallowed decay of KF ='//CHKC)
            BRSUM=BRSUM+BRAT(IDC)
  210     CONTINUE
          WRITE(CHTMP,5500) BRSUM
          IF(ABS(BRSUM).GT.0.0005D0.AND.ABS(BRSUM-1D0).GT.0.0005D0)
     &    CALL PYERRM(7,'(PYUPDA:) Sum of branching ratios is '//
     &    CHTMP(5:12)//' for KF ='//CHKC)
  220   CONTINUE
 
C...Formats for reading and writing particle data. 
 5000 FORMAT(1X,I4,2X,A8,3I3,3F12.5,2X,F12.5,I3) 
 5100 FORMAT(5X,2I5,F12.5,5I8) 
 5200 FORMAT(A80) 
 5300 FORMAT(I4) 
 5400 FORMAT(I12) 
 5500 FORMAT(F12.5) 
 5600 FORMAT(A72) 
 
      RETURN 
      END 
 
