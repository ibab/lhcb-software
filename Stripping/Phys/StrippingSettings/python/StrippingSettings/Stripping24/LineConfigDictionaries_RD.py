################################################################################
##                          S T R I P P I N G  2 4                            ##
##                                                                            ##
##  Configuration for RD WG                                                   ##
##  Contact person: Pablo Ruiz Valls pruizval@cern.ch                         ##
################################################################################

B23Mu = {
    "BUILDERTYPE": "B23MuLinesConf", 
    "CONFIG": {
        "B23MuPrescale": 1, 
        "B23PiPrescale": 1, 
        "B2MueePrescale": 1, 
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTau": 0.0, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2": 225, 
        "Postscale": 1, 
        "mDiffBuLoose": 500, 
        "mDiffBuTight": 100
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B24p = {
    "BUILDERTYPE": "B24pLinesConf", 
    "CONFIG": {
        "B24pPrescale": 1, 
        "B2JpsiKpiPrescale": 1, 
        "B2PhiKhPrescale": 1, 
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTau": 1.0, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2": 225, 
        "Postscale": 1, 
        "mDiffb": 500, 
        "mJpsiMax": 3200, 
        "mJpsiMin": 2990, 
        "mKstMax": 1200
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

B2KstTauTau = {
    "BUILDERTYPE": "B2KstTauXConf", 
    "CONFIG": {
        "AMAXDOCA_D": "0.2", 
        "APT_D": "800", 
        "B2DDSL_LinePostscale": 1, 
        "B2DDSL_LinePrescale": 1, 
        "B2KstMuMu_LinePostscale": 1, 
        "B2KstMuMu_LinePrescale": 1, 
        "B2KstMuMu_SameSign_LinePostscale": 1, 
        "B2KstMuMu_SameSign_LinePrescale": 1, 
        "B2KstTauMu_LinePostscale": 1, 
        "B2KstTauMu_LinePrescale": 1, 
        "B2KstTauMu_SameSign_LinePostscale": 1, 
        "B2KstTauMu_SameSign_LinePrescale": 0.5, 
        "B2KstTauTau_LinePostscale": 1, 
        "B2KstTauTau_LinePrescale": 1, 
        "B2KstTauTau_SameSign_LinePostscale": 1, 
        "B2KstTauTau_SameSign_LinePrescale": 1, 
        "B_COSDIRA_KMM": "0.999", 
        "DIRA_D": "0.99", 
        "DOCAMAX_KST_KMM": "0.15", 
        "FDCHI2OWNPV_KST_KMM": "120", 
        "FDCHI2_B": "80", 
        "FDCHI2_D": "16", 
        "FD_B_Max": "70", 
        "FD_B_Max_KTT": "40", 
        "FD_B_Mu": "3", 
        "FD_B_Mu_KMM": "3", 
        "FD_Kst_Mu_KMM": "3", 
        "IPCHI2_HAD_ALL_FINAL_STATE": "16", 
        "MASS_HIGH_B": "10000", 
        "MASS_HIGH_B_KMM": "10000", 
        "MASS_HIGH_D": "1900", 
        "MASS_HIGH_Ds": "1998", 
        "MASS_HIGH_Kst": "1100", 
        "MASS_LOW_B": "2000", 
        "MASS_LOW_B_KMM": "1500", 
        "MASS_LOW_D": "1840", 
        "MASS_LOW_Ds": "1938", 
        "MASS_LOW_Kst": "700", 
        "MINIPCHI2_KST_KMM": "3", 
        "MINIPCHI2_K_KMM": "4", 
        "MINIPCHI2_MU_KMM": "4", 
        "MINIPCHI2_PI_KMM": "4", 
        "MaxPT_D": "800", 
        "PT_B_KMM": "2000", 
        "PT_B_KTM": "3000", 
        "PT_B_KTT": "3000", 
        "PT_D": "1000", 
        "PT_HAD_ALL_FINAL_STATE": "250", 
        "PT_Kst": "1000", 
        "PT_MU": "1000", 
        "PT_MU_KMM": "800", 
        "P_HAD_ALL_FINAL_STATE": "2000", 
        "RelatedInfoTools": [
            {
                "Location": "B2KstTauTau_MuonIsolationBDT", 
                "Type": "RelInfoBKsttautauMuonIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUMUONISOBDTFIRSTVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTSECONDVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTTHIRDVALUETAUP", 
                    "BKSTTAUTAUMUONISOBDTFIRSTVALUETAUM", 
                    "BKSTTAUTAUMUONISOBDTSECONDVALUETAUM", 
                    "BKSTTAUTAUMUONISOBDTTHIRDVALUETAUM"
                ]
            }, 
            {
                "Location": "B2KstTauTau_TauIsolationBDT", 
                "Type": "RelInfoBKsttautauTauIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUETAUP", 
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUETAUM", 
                    "BKSTTAUTAUTAUISOBDTFIRSTVALUEKST", 
                    "BKSTTAUTAUTAUISOBDTSECONDVALUEKST", 
                    "BKSTTAUTAUTAUISOBDTTHIRDVALUEKST"
                ]
            }, 
            {
                "Location": "B2KstTauTau_TrackIsolationBDT", 
                "Type": "RelInfoBKsttautauTrackIsolationBDT", 
                "Variables": [
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIM", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP1", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUPPIP2", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIP", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM1", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUETAUMPIM2", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTK", 
                    "BKSTTAUTAUTRKISOBDTFIRSTVALUEKSTPI", 
                    "BKSTTAUTAUTRKISOBDTSECONDVALUEKSTPI", 
                    "BKSTTAUTAUTRKISOBDTTHIRDVALUEKSTPI"
                ]
            }, 
            {
                "Location": "B2KstTauTau_CDFIso", 
                "Type": "RelInfoBstautauCDFIso"
            }
        ], 
        "SpdMult": "600", 
        "TRACKCHI2_HAD_ALL_FINAL_STATE": "4", 
        "TRACKCHI2_MU": "4", 
        "TRGHOPROB_HAD_ALL_FINAL_STATE": "0.4", 
        "VCHI2_B": "100", 
        "VCHI2_B_Mu": "150", 
        "VCHI2_B_Mu_KMM": "100", 
        "VCHI2_D": "16", 
        "VCHI2_Kst": "15", 
        "VDRHOmax_D": "7.0", 
        "VDRHOmin_D": "0.1", 
        "VDZ_D": "5.0"
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

B2Lambda0Mu = {
    "BUILDERTYPE": "B2Lambda0MuLines", 
    "CONFIG": {
        "BDIRA": 0.99, 
        "BVCHI2DOF": 4.0, 
        "GEC_nLongTrk": 300.0, 
        "Lambda0DaugMIPChi2": 10.0, 
        "Lambda0DaugP": 2000.0, 
        "Lambda0DaugPT": 250.0, 
        "Lambda0DaugTrackChi2": 4.0, 
        "Lambda0PT": 700.0, 
        "Lambda0VertexChi2": 10.0, 
        "LambdaMuMassLowTight": 1500.0, 
        "LambdaZ": 0.0, 
        "MajoranaCutFDChi2": 100.0, 
        "MajoranaCutM": 1500.0, 
        "MuonGHOSTPROB": 0.5, 
        "MuonMINIPCHI2": 12, 
        "MuonP": 3000.0, 
        "MuonPIDK": 0.0, 
        "MuonPIDmu": 0.0, 
        "MuonPIDp": 0.0, 
        "MuonPT": 250.0, 
        "MuonTRCHI2": 4.0, 
        "XMuMassUpperHigh": 6500.0
    }, 
    "STREAMS": [ "Dimuon" ], 
    "WGs": [ "RD" ]
}

B2XLL = {
    "BUILDERTYPE": "B2XLLConf", 
    "CONFIG": {
        "BDIRA": 0.9995, 
        "BFlightCHI2": 100, 
        "BIPCHI2": 25, 
        "BMassWindow": 1500, 
        "BVertexCHI2": 9, 
        "DiHadronMass": 2600, 
        "DiLeptonFDCHI2": 16, 
        "DiLeptonIPCHI2": 0, 
        "DiLeptonPT": 0, 
        "KaonIPCHI2": 9, 
        "KaonPT": 400, 
        "LeptonIPCHI2": 9, 
        "LeptonPT": 300, 
        "PIDe": 0, 
        "UpperMass": 5500, 
        "eeXLinePrescale": 1, 
        "eeXSSLinePrescale": 1, 
        "meXLinePrescale": 1, 
        "meXSSLinePrescale": 1, 
        "mmXLinePrescale": 1, 
        "mmXSSLinePrescale": 1
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B2XMuMu = {
    "BUILDERTYPE": "B2XMuMuConf", 
    "CONFIG": {
        "A1_Comb_MassHigh": 5550.0, 
        "A1_Comb_MassLow": 0.0, 
        "A1_Dau_MaxIPCHI2": 9.0, 
        "A1_FlightChi2": 25.0, 
        "A1_MassHigh": 5500.0, 
        "A1_MassLow": 0.0, 
        "A1_MinIPCHI2": 4.0, 
        "A1_VtxChi2": 10.0, 
        "B_Comb_MassHigh": 7100.0, 
        "B_Comb_MassLow": 4800.0, 
        "B_DIRA": 0.9999, 
        "B_Dau_MaxIPCHI2": 9.0, 
        "B_FlightCHI2": 121.0, 
        "B_IPCHI2": 16.0, 
        "B_MassHigh": 7000.0, 
        "B_MassLow": 4900.0, 
        "B_VertexCHI2": 8.0, 
        "DECAYS": [
            "B0 -> J/psi(1S) phi(1020)", 
            "[B0 -> J/psi(1S) K*(892)0]cc", 
            "B0 -> J/psi(1S) rho(770)0", 
            "[B+ -> J/psi(1S) rho(770)+]cc", 
            "B0 -> J/psi(1S) f_2(1950)", 
            "B0 -> J/psi(1S) KS0", 
            "[B0 -> J/psi(1S) D~0]cc", 
            "[B+ -> J/psi(1S) K+]cc", 
            "[B+ -> J/psi(1S) pi+]cc", 
            "[B+ -> J/psi(1S) K*(892)+]cc", 
            "[B+ -> J/psi(1S) D+]cc", 
            "[B+ -> J/psi(1S) D*(2010)+]cc", 
            "[Lambda_b0 -> J/psi(1S) Lambda0]cc", 
            "[Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc", 
            "B0 -> J/psi(1S) pi0", 
            "[B+ -> J/psi(1S) a_1(1260)+]cc", 
            "[B+ -> J/psi(1S) K_1(1270)+]cc", 
            "[B+ -> J/psi(1S) K_2(1770)+]cc", 
            "B0 -> J/psi(1S) K_1(1270)0", 
            "[B+ -> J/psi(1S) K_1(1400)+]cc", 
            "B0 -> J/psi(1S) K_1(1400)0"
        ], 
        "Dau_DIRA": -0.9, 
        "Dau_VertexCHI2": 12.0, 
        "Dimu_Dau_MaxIPCHI2": 9.0, 
        "Dimu_FlightChi2": 9.0, 
        "DimuonUPPERMASS": 7100.0, 
        "DimuonWS": True, 
        "DplusLOWERMASS": 1600.0, 
        "DplusUPPERMASS": 2300.0, 
        "HLT1_FILTER": None, 
        "HLT2_FILTER": None, 
        "HadronWS": True, 
        "Hadron_MinIPCHI2": 6.0, 
        "K12OmegaK_CombMassHigh": 2000, 
        "K12OmegaK_CombMassLow": 400, 
        "K12OmegaK_MassHigh": 2100, 
        "K12OmegaK_MassLow": 300, 
        "K12OmegaK_VtxChi2": 10, 
        "KpiVXCHI2NDOF": 9.0, 
        "KsWINDOW": 30.0, 
        "Kstar_Comb_MassHigh": 6200.0, 
        "Kstar_Comb_MassLow": 0.0, 
        "Kstar_Dau_MaxIPCHI2": 9.0, 
        "Kstar_FlightChi2": 9.0, 
        "Kstar_MassHigh": 6200.0, 
        "Kstar_MassLow": 0.0, 
        "Kstar_MinIPCHI2": 0.0, 
        "KstarplusWINDOW": 300.0, 
        "L0DU_FILTER": None, 
        "LambdaWINDOW": 30.0, 
        "LongLivedPT": 0.0, 
        "LongLivedTau": 2, 
        "MuonNoPIDs_PIDmu": 0.0, 
        "MuonPID": -3.0, 
        "Muon_IsMuon": True, 
        "Muon_MinIPCHI2": 9.0, 
        "OmegaChi2Prob": 1e-05, 
        "Omega_CombMassWin": 200, 
        "Omega_MassWin": 100, 
        "Pi0ForOmegaMINPT": 500.0, 
        "Pi0MINPT": 800.0, 
        "RelatedInfoTools": [
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPTASYM", 
                    "CONEPT", 
                    "CONEP", 
                    "CONEPASYM", 
                    "CONEDELTAETA", 
                    "CONEDELTAPHI"
                ]
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation", 
                "Variables": [
                    "VTXISONUMVTX", 
                    "VTXISODCHI2ONETRACK", 
                    "VTXISODCHI2MASSONETRACK", 
                    "VTXISODCHI2TWOTRACK", 
                    "VTXISODCHI2MASSTWOTRACK"
                ]
            }, 
            {
                "Location": "VtxIsoBDTInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }
        ], 
        "SpdMult": 600, 
        "Track_GhostProb": 0.5, 
        "UseNoPIDsHadrons": True
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

B2XTau = {
    "BUILDERTYPE": "B2XTauConf", 
    "CONFIG": {
        "AMAXDOCA_D": "0.2", 
        "APT_D": "800", 
        "B2DD_LinePostscale": 1, 
        "B2DD_LinePrescale": 1, 
        "B2DD_SameSign_LinePostscale": 1, 
        "B2DD_SameSign_LinePrescale": 0.5, 
        "B2DPi_LinePostscale": 1, 
        "B2DPi_LinePrescale": 1, 
        "B2DPi_SameSign_LinePostscale": 1, 
        "B2DPi_SameSign_LinePrescale": 0.5, 
        "B2TauMu_SameSign_TOSLinePostscale": 1, 
        "B2TauMu_SameSign_TOSLinePrescale": 0.5, 
        "B2TauMu_TOSLinePostscale": 1, 
        "B2TauMu_TOSLinePrescale": 1, 
        "B2TauTau_SameSign_TOSLinePostscale": 1, 
        "B2TauTau_SameSign_TOSLinePrescale": 1, 
        "B2TauTau_TOSLinePostscale": 1, 
        "B2TauTau_TOSLinePrescale": 1, 
        "B_TAUPI_2NDMINIPS": "20", 
        "DIRA_B": "0.99", 
        "DIRA_D": "0.99", 
        "FDCHI2_B": "225", 
        "FDCHI2_D": "16", 
        "FDCHI2_TAU": "4000", 
        "FD_B": "90", 
        "FD_B_MU": "35", 
        "HLT_DECISIONS_HAD": {
            "Hlt2Topo(2|3|4)Body.*Decision%TOS": 0
        }, 
        "HLT_DECISIONS_MUON": {
            "Hlt2(TopoMu|SingleMuon).*Decision%TOS": 0
        }, 
        "IPCHI2_B_CHILD_BEST": "16", 
        "IPCHI2_B_MU": "200", 
        "IPCHI2_B_TAU_CHILD_BEST": "150", 
        "IPCHI2_B_TAU_CHILD_WORSE": "16", 
        "IPCHI2_B_TAU_MU": "50", 
        "IPCHI2_HAD_ALL_FINAL_STATE": "16", 
        "IPCHI2_MU": "16", 
        "MASS_HIGH_B": "7000", 
        "MASS_HIGH_D": "2080", 
        "MASS_HIGH_Ds": "1998", 
        "MASS_HIGH_TAU": "2100", 
        "MASS_LOW_B": "2000", 
        "MASS_LOW_D": "1750", 
        "MASS_LOW_Ds": "1938", 
        "MASS_LOW_TAU": "400", 
        "MCOR_HIGH_B": "10000", 
        "MCOR_LOW_B": "0", 
        "MaxPT_D": "800", 
        "PT_B_CHILD_BEST": "2000", 
        "PT_B_MU_PIONS_TOTAL": "2500", 
        "PT_B_PIONS_TOTAL": "7000", 
        "PT_B_TAU_CHILD_BEST": "4000", 
        "PT_B_TM": "1900", 
        "PT_B_TM_HIGH": "5000", 
        "PT_B_TT": "1900", 
        "PT_B_TT_HIGH": "2000", 
        "PT_D": "1000", 
        "PT_HAD_ALL_FINAL_STATE": "250", 
        "PT_MU": "1000", 
        "P_HAD_ALL_FINAL_STATE": "2000", 
        "P_MU": "6000", 
        "RelatedInfoTools": [
            {
                "Location": "MuonIsolationBDT", 
                "Type": "RelInfoBstautauMuonIsolationBDT", 
                "Variables": [
                    "BSTAUTAUMUONISOBDTFIRSTVALUE", 
                    "BSTAUTAUMUONISOBDTSECONDVALUE", 
                    "BSTAUTAUMUONISOBDTTHIRDVALUE"
                ]
            }, 
            {
                "Location": "MuonIsolation", 
                "Type": "RelInfoBstautauMuonIsolation", 
                "Variables": [
                    "BSTAUTAUMUONISOFIRSTVALUE", 
                    "BSTAUTAUMUONISOSECONDVALUE"
                ]
            }, 
            {
                "Location": "TauIsolationBDT", 
                "Type": "RelInfoBstautauTauIsolationBDT", 
                "Variables": [
                    "BSTAUTAUTAUISOBDTFIRSTVALUETAUP", 
                    "BSTAUTAUTAUISOBDTSECONDVALUETAUP", 
                    "BSTAUTAUTAUISOBDTTHIRDVALUETAUP", 
                    "BSTAUTAUTAUISOBDTFIRSTVALUETAUM", 
                    "BSTAUTAUTAUISOBDTSECONDVALUETAUM", 
                    "BSTAUTAUTAUISOBDTTHIRDVALUETAUM"
                ]
            }, 
            {
                "Location": "TauIsolation", 
                "Type": "RelInfoBstautauTauIsolation", 
                "Variables": [
                    "BSTAUTAUTAUISOFIRSTVALUETAUP", 
                    "BSTAUTAUTAUISOSECONDVALUETAUP", 
                    "BSTAUTAUTAUISOFIRSTVALUETAUM", 
                    "BSTAUTAUTAUISOSECONDVALUETAUM"
                ]
            }, 
            {
                "Location": "TrackIsolationBDT", 
                "Type": "RelInfoBstautauTrackIsolationBDT", 
                "Variables": [
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM2", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM2", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM2"
                ]
            }, 
            {
                "Location": "TrackIsolation", 
                "Type": "RelInfoBstautauTrackIsolation", 
                "Variables": [
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIM2"
                ]
            }, 
            {
                "Location": "CDFIso", 
                "Type": "RelInfoBstautauCDFIso"
            }, 
            {
                "Location": "ZVisoBDT", 
                "Type": "RelInfoBstautauZVisoBDT", 
                "Variables": [
                    "ZVISOTAUP", 
                    "ZVISOTAUM"
                ]
            }
        ], 
        "TRACKCHI2_HAD_ALL_FINAL_STATE": "4", 
        "TRACKCHI2_MU": "4", 
        "TRGHOPROB_HAD_ALL_FINAL_STATE": "0.4", 
        "TRGHOPROB_MU": "0.4", 
        "VCHI2_B": "90", 
        "VCHI2_B_TAU_MU": "12", 
        "VCHI2_D": "16", 
        "VDRHOmax_D": "7.0", 
        "VDRHOmin_D": "0.1", 
        "VDZ_D": "5.0"
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

BLV = {
    "BUILDERTYPE": "BLVLinesConf", 
    "CONFIG": {
        "B2DpiPrescale": 0.1, 
        "B2KhhPrescale": 1, 
        "B2LcmuPrescale": 1, 
        "B2LcpPrescale": 0.2, 
        "Bs2DspiPrescale": 0.1, 
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "La2KmuPrescale": 1, 
        "Lb2DmuPrescale": 1, 
        "Lb2DsmuPrescale": 1, 
        "Lb2KmuPrescale": 1, 
        "Lb2LcpiPrescale": 0.1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTauLambda": 10.0, 
        "MinTauLoose": 0.0, 
        "MinTauTight": 1.0, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2Loose": 100, 
        "MinVDChi2Tight": 225, 
        "Postscale": 1, 
        "Xib2KhmuPrescale": 1, 
        "mDiffD": 100.0, 
        "mDiffDs": 100.0, 
        "mDiffLa": 400.0, 
        "mDiffbLoose": 500.0, 
        "mDiffbStand": 400.0, 
        "mDiffbTight": 300.0, 
        "mKst": 1200.0
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

Bd2eeKstarBDT = {
    "BUILDERTYPE": "Bd2eeKstarBDTConf", 
    "CONFIG": {
        "BComMassW": 1200.0, 
        "BDIRA": 0.999, 
        "BDTCutValue": -0.95, 
        "BDTWeightsFile": "$TMVAWEIGHTSROOT/data/Bd2eeKstar_BDTG_v1r0.xml", 
        "BMassW": 1000.0, 
        "BVertexCHI2": 16.0, 
        "ElectronGhostProb": 0.5, 
        "ElectronIPCHI2": 1.0, 
        "ElectronPIDepi": -2.0, 
        "ElectronPT": 200.0, 
        "ElectronTrackCHI2pNDOF": 5.0, 
        "KaonGhostProb": 0.35, 
        "KaonIPCHI2": 4.0, 
        "KaonP": 3000.0, 
        "KaonPIDKpi": -5.0, 
        "KaonPT": 400.0, 
        "KaonTrackCHI2pNDOF": 5.0, 
        "KstarMassW": 150.0, 
        "KstarVertexCHI2": 16.0, 
        "LinePostscale": 1.0, 
        "LinePrescale": 1.0, 
        "PionGhostProb": 0.35, 
        "PionIPCHI2": 4.0, 
        "PionP": 2000.0, 
        "PionPIDpiK": 10.0, 
        "PionPT": 250.0, 
        "PionTrackCHI2pNDOF": 5.0, 
        "RelatedInfoTools": [
            {
                "DaughterLocations": {
                    "[B0 -> ( K*(892)0 -> K+ pi-) ( J/psi(1S) ->  e+^e-) ]CC": "Electron2ISO", 
                    "[B0 -> ( K*(892)0 -> K+ pi-) ( J/psi(1S) -> ^e+ e-) ]CC": "Electron1ISO", 
                    "[B0 -> ( K*(892)0 -> K+^pi-) ( J/psi(1S) ->  e+ e-) ]CC": "PionISO", 
                    "[B0 -> ( K*(892)0 ->^K+ pi-) ( J/psi(1S) ->  e+ e-) ]CC": "KaonISO"
                }, 
                "IsoTwoBody": False, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }
        ], 
        "RelatedInfoTools2": [
            {
                "DaughterLocations": {
                    "[B0 -> ( K*(892)0 -> K+ pi-) ( J/psi(1S) ->  e+^e-) ]CC": "Electron2ISO", 
                    "[B0 -> ( K*(892)0 -> K+ pi-) ( J/psi(1S) -> ^e+ e-) ]CC": "Electron1ISO", 
                    "[B0 -> ( K*(892)0 -> K+^pi-) ( J/psi(1S) ->  e+ e-) ]CC": "PionISO", 
                    "[B0 -> ( K*(892)0 ->^K+ pi-) ( J/psi(1S) ->  e+ e-) ]CC": "KaonISO"
                }, 
                "IsoTwoBody": False, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }
        ], 
        "eeCuts": "(HASVERTEX) & (VFASPF(VCHI2)<16) & (((MM<1.5*GeV)) | ((MM>2.2*GeV) & (MM<4.2*GeV)))"
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

Beauty2XGamma = {
    "BUILDERTYPE": "Beauty2XGammaConf", 
    "CONFIG": {
        "B2PhiOmega2pipipi0MPrescale": 1.0, 
        "B2PhiOmega2pipipi0RPrescale": 1.0, 
        "B2XG2pi2KsPrescale": 1.0, 
        "B2XG2piCNVDDPrescale": 1.0, 
        "B2XG2piCNVLLPrescale": 1.0, 
        "B2XG2piKsPrescale": 1.0, 
        "B2XG2piPrescale": 1.0, 
        "B2XG2pipi0MPrescale": 1.0, 
        "B2XG2pipi0RPrescale": 1.0, 
        "B2XG3piCNVDDPrescale": 1.0, 
        "B2XG3piCNVLLPrescale": 1.0, 
        "B2XG3piKsPrescale": 1.0, 
        "B2XG3piPrescale": 1.0, 
        "B2XG3pipi0MPrescale": 1.0, 
        "B2XG3pipi0RPrescale": 1.0, 
        "B2XG4piPrescale": 1.0, 
        "B2XGBMaxCorrM": 73000.0, 
        "B2XGBMaxM": 9000.0, 
        "B2XGBMinBPVDIRA": 0.0, 
        "B2XGBMinM2pi": 3280.0, 
        "B2XGBMinM3pi": 2900.0, 
        "B2XGBMinM4pi": 2560.0, 
        "B2XGBMinMLambda": 2560.0, 
        "B2XGBMinPT": 200.0, 
        "B2XGBSumPtMin": 5000, 
        "B2XGBVtxChi2DOF": 9.0, 
        "B2XGBVtxMaxIPChi2": 9.0, 
        "B2XGGammaCL": 0.0, 
        "B2XGGammaCNVPTMin": 1000.0, 
        "B2XGGammaPTMin": 2000.0, 
        "B2XGLambda2piPrescale": 1.0, 
        "B2XGLambda3piPrescale": 1.0, 
        "B2XGLambdapiPrescale": 1.0, 
        "B2XGLbLambdaPrescale": 1.0, 
        "B2XGPhiOmegaMaxMass": 1300.0, 
        "B2XGPhiOmegaMinMass": 700.0, 
        "B2XGResBPVVDCHI2Min": 0.0, 
        "B2XGResDocaMax": 100.5, 
        "B2XGResIPCHI2Min": 0.0, 
        "B2XGResMaxMass": 7900.0, 
        "B2XGResMinMass": 0.0, 
        "B2XGResMinPT": 150.0, 
        "B2XGResSumPtMin": 1500.0, 
        "B2XGResVtxChi2DOF": 10.0, 
        "B2XGTrkChi2DOF": 3.0, 
        "B2XGTrkMinIPChi2": 16.0, 
        "B2XGTrkMinP": 1000, 
        "B2XGTrkMinPT": 300.0, 
        "B2XGpiKsPrescale": 1.0, 
        "Pi0MPMin": 4000.0, 
        "Pi0MPTMin": 700.0, 
        "Pi0MPTReCut": 1200.0, 
        "Pi0RPMin": 4000.0, 
        "Pi0RPTMin": 700.0, 
        "Pi0RPTReCut": 1700.0, 
        "TISTOSLinesDict": {
            "Hlt2IncPhi.*Decision%TIS": 0, 
            "Hlt2IncPhi.*Decision%TOS": 0, 
            "Hlt2Radiative.*Decision%TIS": 0, 
            "Hlt2Radiative.*Decision%TOS": 0, 
            "Hlt2Topo(2|3|4)Body.*Decision%TIS": 0, 
            "Hlt2Topo(2|3|4)Body.*Decision%TOS": 0
        }, 
        "TrackGhostProb": 0.4
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Beauty2XGammaExclusive = {
    "BUILDERTYPE": "Beauty2XGammaExclusiveConf", 
    "CONFIG": {
        "B0DirAngle": 0.02, 
        "B0MassWin": 1000.0, 
        "B0PVIPchi2": 15.0, 
        "B_PT": 3000.0, 
        "Bd2KstGammaPostScale": 1.0, 
        "Bd2KstGammaPreScale": 1.0, 
        "Bs2PhiGammaPostScale": 1.0, 
        "Bs2PhiGammaPreScale": 1.0, 
        "BsDirAngle": 0.02, 
        "BsMassWin": 1000.0, 
        "BsPVIPchi2": 15.0, 
        "KstMassWin": 100.0, 
        "KstVCHI2": 9.0, 
        "PhiMassWin": 15.0, 
        "PhiVCHI2": 9.0, 
        "TrChi2": 5.0, 
        "TrIPchi2": 25.0, 
        "photonPT": 2600.0
    }, 
    "STREAMS": {
        "Calibration": [ "StrippingBeauty2XGammaExclusiveBd2KstGammaLine" ], 
        "Radiative": [ "StrippingBeauty2XGammaExclusiveBs2PhiGammaLine" ]
    }, 
    "WGs": [ "RD" ]
}

Beauty2XGammaNoBias = {
    "BUILDERTYPE": "Beauty2XGammaNoBiasConf", 
    "CONFIG": {
        "B0MassWin": 3000.0, 
        "Bd2KstGammaPostScale": 1.0, 
        "Bd2KstGammaPreScale": 1.0, 
        "Bs2PhiGammaPostScale": 1.0, 
        "Bs2PhiGammaPreScale": 1.0, 
        "BsMassWin": 3000.0, 
        "KstMassWin": 3000.0, 
        "PhiMassWin": 400.0, 
        "TISTOSLinesDict": {
            "Hlt2RadiativeBd2KstGammaULUnbiased.*Decision%TOS": 0, 
            "Hlt2RadiativeBs2PhiGammaUnbiased.*Decision%TOS": 0
        }, 
        "TrChi2": 100.0, 
        "photonPT": 2600.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Bs2GammaGamma = {
    "BUILDERTYPE": "StrippingBs2gammagammaConf", 
    "CONFIG": {
        "BsHighMass": 6500, 
        "BsHighMassDD": 6700, 
        "BsHighMassDouble": 7000, 
        "BsHighMassNone": 6300, 
        "BsLowMass": 4300, 
        "BsLowMassDD": 4400, 
        "BsLowMassDouble": 4000, 
        "BsLowMassNone": 4800, 
        "BsNonePT": 2000, 
        "BsPT": 2000, 
        "BsVertexCHI2pDOF": 20, 
        "ConvGhostDD": 0.3, 
        "ConvGhostLL": 0.3, 
        "DDProbNNe": 0.3, 
        "HLT2DD": "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaDDDecision')", 
        "HLT2Double": "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaDoubleDecision')", 
        "HLT2LL": "HLT_PASS_RE('Hlt2RadiativeB2GammaGammaLLDecision')", 
        "HLTNone": "HLT_PASS_RE('.*GammaGammaDecision')", 
        "LLProbNNe": 0.5, 
        "NoConvHCAL2ECAL": 0.1, 
        "gammaCL": 0.3, 
        "gammaConvIPCHI": 0, 
        "gammaConvMDD": 60, 
        "gammaConvPT": 2000, 
        "gammaNoneCL": 0.3, 
        "gammaNoneP": 6000, 
        "gammaNonePT": 1100, 
        "gammaP": 6000, 
        "gammaPT": 1000
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Bs2MuMuLines = {
    "BUILDERTYPE": "Bs2MuMuLinesConf", 
    "CONFIG": {
        "BFDChi2_loose": 100, 
        "BIPChi2_loose": 64, 
        "BPVVDChi2": 121, 
        "B_BPVIPChi2": 25, 
        "B_BPVIPChi2_LTUB": 25, 
        "B_Pt": 350, 
        "B_Pt_LTUB": 500, 
        "B_minimum_decaytime_LTUB": 0.6, 
        "BdPrescale": 1, 
        "Bs2KKLTUBLinePrescale": 1, 
        "Bs2mmLTUBLinePrescale": 1, 
        "Bs2mmWideLinePrescale": 1, 
        "BsPrescale": 1, 
        "BuPrescale": 1, 
        "DOCA": 0.3, 
        "DOCA_LTUB": 0.3, 
        "DOCA_loose": 0.5, 
        "DefaultLinePrescale": 1, 
        "DefaultPostscale": 1, 
        "JPsiLinePrescale": 1, 
        "JPsiLooseLinePrescale": 0.1, 
        "JPsiPromptLinePrescale": 0.005, 
        "LooseLinePrescale": 0.0, 
        "MuIPChi2_loose": 9, 
        "MuTrChi2_loose": 10, 
        "SSPrescale": 1, 
        "TrackGhostProb": 0.4, 
        "VCHI2_VDOF": 9, 
        "VCHI2_VDOF_LTUB": 9, 
        "VCHI2_VDOF_loose": 25, 
        "daughter_IPChi2": 9, 
        "daughter_TrChi2": 4, 
        "daughter_TrChi2_LTUB": 4, 
        "muon_PT_LTUB": 40
    }, 
    "STREAMS": {
        "Dimuon": [
            "StrippingBs2MuMuLinesNoMuIDLine", 
            "StrippingBs2MuMuLinesWideMassLine", 
            "StrippingBs2MuMuLinesBu2JPsiKLine"
        ], 
        "Leptonic": [
            "StrippingBs2MuMuLinesWideMassLine", 
            "StrippingBs2MuMuLinesBs2JPsiPhiLine", 
            "StrippingBs2MuMuLinesBs2KKLTUBLine", 
            "StrippingBs2MuMuLinesNoMuIDLine", 
            "StrippingBs2MuMuLinesSSLine", 
            "StrippingBs2MuMuLinesBd2JPsiKstLine", 
            "StrippingBs2MuMuLinesLTUBLine", 
            "StrippingBs2MuMuLinesBu2JPsiKLine"
        ]
    }, 
    "WGs": [ "RD" ]
}

Bs2st2KKMuX = {
    "BUILDERTYPE": "Bs2st2KKMuXConf", 
    "CONFIG": {
        "Bs2PT": 50.0, 
        "Bs2st2KKJpsiPrescale": 1.0, 
        "Bs2st2KKJpsiWSPrescale": 1.0, 
        "Bs2st2KKMuPrescale": 1.0, 
        "Bs2st2KKMuWSPrescale": 1.0, 
        "DMKKJpsi": 1093.677, 
        "DMKKMu": 713.677, 
        "DZBPV": 1.0, 
        "GEC_nLongTrk": 1000.0, 
        "HLT_FILTER": "", 
        "JpsiMassWindow": 80.0, 
        "K1MinIPChi2": 9.0, 
        "K1PIDK": 16.0, 
        "K1PT": 500.0, 
        "K1PTLoose": 250.0, 
        "K2MinIPChi2": 9.0, 
        "K2P": 3000.0, 
        "K2PIDK": 0.0, 
        "K2PT": 1000.0, 
        "KJpsiFdChi2": 25.0, 
        "KJpsiMassMax": 5500.0, 
        "KJpsiMassMin": 5050.0, 
        "KJpsiVChi2Dof": 4.0, 
        "KMuFdChi2": 100.0, 
        "KMuMassMax": 5500.0, 
        "KMuMassMin": 1000.0, 
        "KMuVChi2Dof": 4.0, 
        "MuMinIPChi2": 9.0, 
        "MuP": 3000.0, 
        "MuPIDmu": 0.0, 
        "MuPT": 1000.0, 
        "RelatedInfoTools": [
            {
                "DaughterLocations": {
                    "[B*_s20 -> (B+ -> K+ [mu-]cc) ^[K-]cc]CC": "K1ISO", 
                    "[B*_s20 -> (B+ -> K+ ^[mu-]cc) [K-]cc]CC": "MuISO", 
                    "[B*_s20 -> (B+ -> ^K+ [mu-]cc) [K-]cc]CC": "K2ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS", 
                    "BSMUMUTRACKID"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }
        ]
    }, 
    "STREAMS": [ "Semileptonic" ], 
    "WGs": [ "RD" ]
}

Bu2LLK = {
    "BUILDERTYPE": "Bu2LLKConf", 
    "CONFIG": {
        "BDIRA": 0.9995, 
        "BFlightCHI2": 100, 
        "BIPCHI2": 25, 
        "BMassWindow": 1500, 
        "BVertexCHI2": 9, 
        "Bu2eeKLinePrescale": 1, 
        "Bu2meKLinePrescale": 1, 
        "Bu2meKSSLinePrescale": 1, 
        "Bu2mmKLinePrescale": 1, 
        "DiHadronMass": 2600, 
        "DiLeptonFDCHI2": 16, 
        "DiLeptonIPCHI2": 0, 
        "DiLeptonPT": 0, 
        "KaonIPCHI2": 9, 
        "KaonPT": 400, 
        "LeptonIPCHI2": 9, 
        "LeptonPT": 300, 
        "PIDe": 0, 
        "UpperMass": 5500
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Bu2MuNu = {
    "BUILDERTYPE": "Bu2MuNuConf", 
    "CONFIG": {
        "IPchi2Mu": 400.0, 
        "IPchi2MuControl": 100.0, 
        "PostscaleControl": 1.0, 
        "PostscaleSignal": 1.0, 
        "PrescaleControl": 0.03, 
        "PrescaleSignal": 1.0, 
        "TrChi2Mu": 3, 
        "maxNTracks": 150, 
        "pTmaxMu": 40.0, 
        "pTminMu": 5.0, 
        "pTminMuControl": 2.5, 
        "pmaxMu": 500.0, 
        "useNN": True
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

D23Mu = {
    "BUILDERTYPE": "D23MuLinesConf", 
    "CONFIG": {
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "D23MuPrescale": 1, 
        "D23PiPrescale": 0.01, 
        "D2MueePrescale": 1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 25, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2Dof": 9, 
        "MinDira": 0.0, 
        "MinTau": 0.1, 
        "MinTrIPChi2": 25.0, 
        "MinVDChi2": 225, 
        "Postscale": 1, 
        "mDiffDLoose": 150, 
        "mDiffDTight": 150
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

DarkBoson = {
    "BUILDERTYPE": "DarkBosonConf", 
    "CONFIG": {
        "B2HHX": {
            "AM_MAX": "5800*MeV", 
            "AM_MIN": "4800*MeV", 
            "BPVIPCHI2_MAX": 10, 
            "BPVLTIME_MIN": "0.2*ps", 
            "HAD_MINIPCHI2_MIN": 9, 
            "PT_MIN": "1000*MeV", 
            "SUMPT_MIN": "0*MeV", 
            "VCHI2DOF_MAX": 15
        }, 
        "B2KX": {
            "AM_MAX": "5800*MeV", 
            "AM_MIN": "4800*MeV", 
            "BPVIPCHI2_MAX": 10, 
            "BPVLTIME_MIN": "0.2*ps", 
            "HAD_MINIPCHI2_MIN": 25, 
            "PT_MIN": "1000*MeV", 
            "SUMPT_MIN": "0*MeV", 
            "VCHI2DOF_MAX": 15
        }, 
        "E": {
            "MIPCHI2DV_MIN": 9, 
            "PIDe_MIN": 0, 
            "PT_MIN": "100*MeV", 
            "TRCHI2DOF_MAX": 5, 
            "TRGHP_MAX": 0.4
        }, 
        "GECNTrkMax": 250, 
        "J": {
            "ADAMASS_MAX": "100*MeV", 
            "VCHI2DOF_MAX": 12
        }, 
        "KB": {
            "MIPCHI2DV_MIN": 9, 
            "PROBNNK_MIN": 0.1, 
            "PT_MIN": "250*MeV", 
            "P_MIN": "2000*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "KBhard": {
            "MIPCHI2DV_MIN": 36, 
            "PROBNNK_MIN": 0.2, 
            "PT_MIN": "250*MeV", 
            "P_MIN": "3000*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "KDX": {
            "MIPCHI2DV_MIN": 25, 
            "PROBNNK_MIN": 0.1, 
            "PT_MIN": "125*MeV", 
            "P_MIN": "0*MeV", 
            "TRCHI2DOF_MAX": 4, 
            "TRGHP_MAX": 0.3
        }, 
        "KX": {
            "MIPCHI2DV_MIN": 25, 
            "PROBNNK_MIN": 0.1, 
            "PT_MIN": "250*MeV", 
            "P_MIN": "3000*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "MuJ": {
            "MIPCHI2DV_MIN": 25, 
            "PIDmu_MIN": -4, 
            "PT_MIN": "125*MeV", 
            "P_MIN": "0*MeV", 
            "TRCHI2DOF_MAX": 4, 
            "TRGHP_MAX": 0.3
        }, 
        "MuX": {
            "MIPCHI2DV_MIN": 9, 
            "PIDmu_MIN": -5, 
            "PT_MIN": "100*MeV", 
            "P_MIN": "0*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "PiB": {
            "MIPCHI2DV_MIN": 9, 
            "PROBNNpi_MIN": 0.2, 
            "PT_MIN": "250*MeV", 
            "P_MIN": "2000*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "PiDX": {
            "MIPCHI2DV_MIN": 25, 
            "PROBNNpi_MIN": 0.1, 
            "PT_MIN": "125*MeV", 
            "P_MIN": "0*MeV", 
            "TRCHI2DOF_MAX": 4, 
            "TRGHP_MAX": 0.3
        }, 
        "PiX": {
            "MIPCHI2DV_MIN": 36, 
            "PROBNNpi_MIN": 0.6, 
            "PT_MIN": "250*MeV", 
            "P_MIN": "3000*MeV", 
            "TRCHI2DOF_MAX": 3, 
            "TRGHP_MAX": 0.3
        }, 
        "Prescales": {
            "DD": 0.5, 
            "KK": 0.25, 
            "SS": 0.1
        }, 
        "XDD": {
            "BPVVDCHI2_MIN": 25, 
            "PT_MIN": "0*MeV", 
            "VCHI2DOF_MAX": 15
        }, 
        "XLL": {
            "BPVVDCHI2_MIN": 25, 
            "PT_MIN": "250*MeV", 
            "VCHI2DOF_MAX": 10
        }, 
        "XLLhard": {
            "BPVVDCHI2_MIN": 25, 
            "PT_MIN": "250*MeV", 
            "VCHI2DOF_MAX": 5
        }
    }, 
    "STREAMS": {
        "Dimuon": [
            "StrippingB2KpiX2MuMuDDDarkBosonLine", 
            "StrippingB2KpiX2MuMuDDSSDarkBosonLine", 
            "StrippingB2KKX2MuMuDDDarkBosonLine", 
            "StrippingB2KKX2MuMuDDSSDarkBosonLine", 
            "StrippingB2KX2MuMuDDDarkBosonLine", 
            "StrippingB2KX2MuMuDDSSDarkBosonLine", 
            "StrippingB2KpiX2PiPiDDDarkBosonLine", 
            "StrippingB2KKX2PiPiDDDarkBosonLine", 
            "StrippingB2KX2PiPiDDDarkBosonLine", 
            "StrippingB2KX2PiPiDDSSDarkBosonLine", 
            "StrippingB2RhoX2MuMuDDDarkBosonLine", 
            "StrippingB2KpiX2KKDDDarkBosonLine", 
            "StrippingB2KKX2KKDDDarkBosonLine", 
            "StrippingB2KX2KKDDDarkBosonLine", 
            "StrippingB2KX2KKDDSSDarkBosonLine"
        ], 
        "Leptonic": [
            "StrippingB2KpiX2MuMuDarkBosonLine", 
            "StrippingB2KpiX2MuMuSSDarkBosonLine", 
            "StrippingB2KKX2MuMuDarkBosonLine", 
            "StrippingB2KKX2MuMuSSDarkBosonLine", 
            "StrippingB2KX2MuMuDarkBosonLine", 
            "StrippingB2KX2MuMuSSDarkBosonLine", 
            "StrippingB2KpiX2PiPiDarkBosonLine", 
            "StrippingB2KKX2PiPiDarkBosonLine", 
            "StrippingB2KX2PiPiDarkBosonLine", 
            "StrippingB2KX2PiPiSSDarkBosonLine", 
            "StrippingB2RhoX2MuMuDarkBosonLine", 
            "StrippingB2KpiX2KKDarkBosonLine", 
            "StrippingB2KKX2KKDarkBosonLine", 
            "StrippingB2KX2KKDarkBosonLine", 
            "StrippingB2KX2KKSSDarkBosonLine", 
            "StrippingB2KpiX2EEDarkBosonLine", 
            "StrippingB2KpiX2EESSDarkBosonLine", 
            "StrippingB2KKX2EEDarkBosonLine", 
            "StrippingB2KKX2EESSDarkBosonLine", 
            "StrippingB2KX2EEDarkBosonLine", 
            "StrippingB2KX2EESSDarkBosonLine", 
            "StrippingB2JKDarkBosonLine", 
            "StrippingB2JKstDarkBosonLine"
        ]
    }, 
    "WGs": [ "RD" ]
}

Kshort2MuMuMuMu = {
    "BUILDERTYPE": "Kshort2MuMuMuMuConf", 
    "CONFIG": {
        "DDIRA": 0.9999, 
        "DFDCHI2_mumumumu": 9, 
        "DIPCHI2_mumumumu": 20, 
        "DMAXDOCA_mumumumu": 0.2, 
        "DPT_mumumumu": 2500.0, 
        "DVCHI2DOF_mumumumu": 8, 
        "KsDauMAXIPCHI2_mumumumu": 15, 
        "KsMotherMassCut": 540.0, 
        "Kshort2MuMuMuMuLinePostscale": 1, 
        "Kshort2MuMuMuMuLinePrescale": 1, 
        "MINIPCHI2_mumumumu": 4.0, 
        "MaxDimuonMass": 260.0, 
        "MaxKsMass": 550.0, 
        "MuonMINIPCHI2": 2, 
        "MuonP": 3000.0, 
        "MuonPIDmu_mumumumu": -1, 
        "MuonPT": 500.0, 
        "MuonTRCHI2": 5, 
        "PT_mumumumu": 300
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Kshort2PiPiMuMu = {
    "BUILDERTYPE": "Kshort2PiPiMuMuConf", 
    "CONFIG": {
        "DDIRA": 0.9999, 
        "DFDCHI2_hhmumu": 9, 
        "DIPCHI2_hhmumu": 20, 
        "DMAXDOCA_hhmumu": 0.2, 
        "DPT_hhmumu": 300.0, 
        "DVCHI2DOF_hhmumu": 8, 
        "KsDauMAXIPCHI2_hhmumu": 15, 
        "KsMotherMassCut": 540.0, 
        "Kshort2PiPiMuMuLinePostscale": 1, 
        "Kshort2PiPiMuMuLinePrescale": 1, 
        "MINIPCHI2_hhmumu": 4.0, 
        "MaxDimuonMass": 260.0, 
        "MaxKsMass": 550.0, 
        "MuonMINIPCHI2": 2, 
        "MuonP": 3000.0, 
        "MuonPIDmu_hhmumu": -1, 
        "MuonPT": 500.0, 
        "MuonTRCHI2": 5, 
        "PT_hhmumu": 300, 
        "PionMINIPCHI2": 2, 
        "PionP": 2000.0, 
        "PionPT": 300.0, 
        "PionTRCHI2": 5
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Kshort2eePiPi = {
    "BUILDERTYPE": "Kshort2eePiPiConf", 
    "CONFIG": {
        "KsIP": 1, 
        "KsLifetime": 0.8953, 
        "KsMAXDOCA": 1.0, 
        "KsVtxChi2": 50, 
        "Kshort2eePiPiLinePostscale": 1, 
        "Kshort2eePiPiLinePrescale": 1, 
        "MaxKsMass": 800.0, 
        "PionGhostProb": 0.5, 
        "PionMINIPCHI2": 16, 
        "PionPIDK": 5, 
        "PionPT": 100, 
        "eGhostProb": 0.5, 
        "eMINIPCHI2": 16, 
        "ePIDe": -4, 
        "ePT": 100.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

LFV = {
    "BUILDERTYPE": "LFVLinesConf", 
    "CONFIG": {
        "B2TauMuPrescale": 1, 
        "B2eMuPrescale": 1, 
        "B2eePrescale": 1, 
        "B2hTauMuPrescale": 1, 
        "B2heMuPrescale": 1, 
        "B2pMuPrescale": 1, 
        "Bu2KJPsieePrescale": 1, 
        "Postscale": 1, 
        "RelatedInfoTools_B2eMu": [
            {
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B_s0 -> ^e+ [mu-]cc]CC": "Electron_ISO", 
                    "[B_s0 -> e+ ^[mu-]cc]CC": "Muon_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_B2ee": [
            {
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B_s0 -> ^e+ [e-]cc]CC": "Electron1_ISO", 
                    "[B_s0 -> e+ ^[e-]cc]CC": "Electron2_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_Bu2KJPsiee": [
            {
                "DaughterLocations": {
                    "[B+ -> ^(J/psi(1S) -> e+ e-) K+]CC": "Jpsi_ISO"
                }, 
                "Location": "BSMUMUVARIABLES", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [
                    "BSMUMUCDFISO", 
                    "BSMUMUOTHERBMAG", 
                    "BSMUMUOTHERBANGLE", 
                    "BSMUMUOTHERBBOOSTMAG", 
                    "BSMUMUOTHERBBOOSTANGLE", 
                    "BSMUMUOTHERBTRACKS"
                ], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }, 
            {
                "DaughterLocations": {
                    "[B+ -> (J/psi(1S) -> ^e+ e-) K+]CC": "Electron1_ISO", 
                    "[B+ -> (J/psi(1S) -> e+ ^e-) K+]CC": "Electron2_ISO", 
                    "[B+ -> (J/psi(1S) -> e+ e-) ^K+]CC": "Kplus_ISO"
                }, 
                "IsoTwoBody": True, 
                "Type": "RelInfoBs2MuMuTrackIsolations", 
                "Variables": [
                    "BSMUMUTRACKPLUSISO", 
                    "BSMUMUTRACKPLUSISOTWO", 
                    "ISOTWOBODYQPLUS", 
                    "ISOTWOBODYMASSISOPLUS", 
                    "ISOTWOBODYCHI2ISOPLUS", 
                    "ISOTWOBODYISO5PLUS"
                ], 
                "angle": 0.27, 
                "doca_iso": 0.13, 
                "fc": 0.6, 
                "ips": 3.0, 
                "makeTrackCuts": False, 
                "pvdis": 0.5, 
                "pvdis_h": 40.0, 
                "svdis": -0.15, 
                "svdis_h": 30.0, 
                "tracktype": 3
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoConeVariables"
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "ConeIsoInfo", 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "RelatedInfoTools_Tau2PhiMu": [
            {
                "ConeAngle": 0.5, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu05", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus05", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus05", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi05"
                }, 
                "Location": "coneInfoTau05", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 0.8, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu08", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus08", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus08", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi08"
                }, 
                "Location": "coneInfoTau08", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 1.0, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu10", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus10", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus10", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi10"
                }, 
                "Location": "coneInfoTau10", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "ConeAngle": 1.2, 
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "coneInfoMu12", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "coneInfoKminus12", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "coneInfoKplus12", 
                    "[tau+ -> ^(phi(1020)->K+ K-) mu+]CC": "coneInfoPhi12"
                }, 
                "Location": "coneInfoTau12", 
                "Type": "RelInfoConeVariables", 
                "Variables": [
                    "CONEANGLE", 
                    "CONEMULT", 
                    "CONEPT", 
                    "CONEPTASYM"
                ]
            }, 
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation", 
                "Variables": [
                    "VTXISONUMVTX", 
                    "VTXISODCHI2ONETRACK", 
                    "VTXISODCHI2MASSONETRACK", 
                    "VTXISODCHI2TWOTRACK", 
                    "VTXISODCHI2MASSTWOTRACK"
                ]
            }, 
            {
                "DaughterLocations": {
                    "[tau+ -> (phi(1020)->K+ K-) ^mu+]CC": "MuonTrackIsoBDTInfo", 
                    "[tau+ -> (phi(1020)->K+ ^K-) mu+]CC": "KminusTrackIsoBDTInfo", 
                    "[tau+ -> (phi(1020)->^K+ K-) mu+]CC": "KplusTrackIsoBDTInfo"
                }, 
                "Type": "RelInfoTrackIsolationBDT"
            }
        ], 
        "Tau2MuMuePrescale": 1, 
        "TauPrescale": 1, 
        "config_B2eMu": {
            "max_ADAMASS": 1200.0, 
            "max_AMAXDOCA": 0.3, 
            "max_BPVIPCHI2": 25, 
            "max_TRCHI2DV": 3.0, 
            "max_TRGHOSTPROB": 0.3, 
            "min_BPVDIRA": 0, 
            "min_BPVVDCHI2": 225, 
            "min_MIPCHI2DV": 25.0
        }
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Lb2L0Gamma = {
    "BUILDERTYPE": "StrippingLb2L0GammaConf", 
    "CONFIG": {
        "HLT1": [], 
        "HLT2": [], 
        "L0": [
            "Photon", 
            "Electron", 
            "Hadron"
        ], 
        "Lambda0DD_MassWindow": 30.0, 
        "Lambda0LL_IP_Min": 0.05, 
        "Lambda0LL_MassWindow": 20.0, 
        "Lambda0_Pt_Min": 1000.0, 
        "Lambda0_VtxChi2_Max": 9.0, 
        "Lambdab_IPChi2_Max": 9.0, 
        "Lambdab_MTDOCAChi2_Max": 7.0, 
        "Lambdab_MassWindow": 1100.0, 
        "Lambdab_Pt_Min": 1000.0, 
        "Lambdab_SumPt_Min": 5000.0, 
        "Lambdab_VtxChi2_Max": 9.0, 
        "Lb2L0GammaConvertedPrescale": 1.0, 
        "Lb2L0GammaPrescale": 1.0, 
        "PhotonCnv_MM_Max": 100.0, 
        "PhotonCnv_PT_Min": 1000.0, 
        "PhotonCnv_VtxChi2_Max": 9.0, 
        "Photon_CL_Min": 0.2, 
        "Photon_PT_Min": 2500.0, 
        "Pion_P_Min": 2000.0, 
        "Pion_Pt_Min": 300.0, 
        "Proton_P_Min": 7000.0, 
        "Proton_Pt_Min": 800.0, 
        "TrackLL_IPChi2_Min": 16.0, 
        "Track_Chi2ndf_Max": 3.0, 
        "Track_GhostProb_Max": 0.4, 
        "Track_MinChi2ndf_Max": 2.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

Lc23Mu = {
    "BUILDERTYPE": "Lc23MuLinesConf", 
    "CONFIG": {
        "CommonRelInfoTools": [
            {
                "Location": "VtxIsoInfo", 
                "Type": "RelInfoVertexIsolation"
            }, 
            {
                "Location": "VtxIsoInfoBDT", 
                "Type": "RelInfoVertexIsolationBDT"
            }, 
            {
                "Location": "BsMuMuBIsolation", 
                "Type": "RelInfoBs2MuMuBIsolations", 
                "Variables": [], 
                "makeTrackCuts": False, 
                "tracktype": 3
            }
        ], 
        "Lc23muPrescale": 1, 
        "Lc2mueePrescale": 1, 
        "Lc2pKpiPrescale": 0.01, 
        "Lc2peePrescale": 1, 
        "Lc2pmumuPrescale": 1, 
        "MaxDoca": 0.3, 
        "MaxIPChi2": 100, 
        "MaxTrChi2Dof": 4.0, 
        "MaxTrGhp": 0.4, 
        "MaxVtxChi2": 15, 
        "MinTrIPChi2": 9, 
        "MinTrPT": 300, 
        "MinVD": 70, 
        "Postscale": 1, 
        "mDiffLcLoose": 200, 
        "mDiffLcTight": 150
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

PhiToKSKS = {
    "BUILDERTYPE": "PhiToKSKSAllLinesConf", 
    "CONFIG": {
        "D0_DOCACHI2_MAX": 20, 
        "D0_IPCHI2_MAX": 9, 
        "D0_MASS_WIN": 40, 
        "D0_PT_MIN": 800, 
        "D0_VCHI2NDOF_MAX": 6, 
        "DoDTF": True, 
        "GEC_nLongTrk": 250, 
        "GHOSTPROB_MAX": 0.35, 
        "JPsi_DOCACHI2_MAX": 20, 
        "JPsi_IPCHI2_MAX": 9, 
        "JPsi_MASS_WIN": 120, 
        "JPsi_PT_MIN": 500, 
        "JPsi_VCHI2NDOF_MAX": 6, 
        "KS_DD_DIRA_MIN": 0.999, 
        "KS_DD_FDCHI2_MIN": 100, 
        "KS_DD_FD_MIN": 10.0, 
        "KS_DD_MASS_WINDOW": 20, 
        "KS_DD_PTMIN": 400, 
        "KS_DD_VCHI2NDOF_MAX": 4, 
        "KS_LL_DIRA_MIN": 0.9999, 
        "KS_LL_FDCHI2_MIN": 100, 
        "KS_LL_FD_MIN": 10.0, 
        "KS_LL_MASS_WINDOW": 20, 
        "KS_LL_PTMIN": 400, 
        "KS_LL_VCHI2NDOF_MAX": 4, 
        "K_IPCHI2_MAX": 9, 
        "K_PIDK_MIN": 7, 
        "K_PTMIN": 200, 
        "Lambda_DIRA_MIN": 0.999, 
        "Lambda_FDCHI2_MIN": 100, 
        "Lambda_FD_MIN": 10.0, 
        "Lambda_MASS_WINDOW": 50, 
        "Lambda_PTMIN": 400, 
        "Lambda_VCHI2NDOF_MAX": 4, 
        "Mu_IPCHI2_MAX": 9, 
        "Mu_PIDmu_MIN": 0, 
        "Mu_PTMIN": 200, 
        "Phi_DOCACHI2_MAX": 20, 
        "Phi_IPCHI2_MAX": 9, 
        "Phi_MASS_MAX": 1100, 
        "Phi_PT_MIN": 800, 
        "Phi_VCHI2NDOF_MAX": 6, 
        "etaC_DOCACHI2_MAX": 20, 
        "etaC_IPCHI2_MAX": 9, 
        "etaC_MASS_WIN": 200, 
        "etaC_PT_MIN": 400, 
        "etaC_VCHI2NDOF_MAX": 6, 
        "prescale_D0ToKsKs": 0.25, 
        "prescale_EtaCToLL": 1.0, 
        "prescale_JPsiToKK": 0.002, 
        "prescale_JPsiToKsKs": 1.0, 
        "prescale_PhiToKK": 0.001, 
        "prescale_PhiToKsKs": 1.0, 
        "prescale_PhiToMuMu": 0.01
    }, 
    "STREAMS": [ "Charm" ], 
    "WGs": [ "RD" ]
}

RareStrange = {
    "BUILDERTYPE": "RareStrangeLinesConf", 
    "CONFIG": {
        "KDauMinIpChi2": 9.0, 
        "KDauMinIpChi2Down": 5.0, 
        "KDauMinIpChi2MassMeas": 8.0, 
        "KDauMinIpChi2MassMeasDown": 4.0, 
        "KDauTrChi2": 3.0, 
        "KMassWin": 100.0, 
        "KMassWinDown": 100.0, 
        "KMassWinMassMeas": 50.0, 
        "KMassWinMassMeasDown": 100.0, 
        "KMaxDOCA": 3.0, 
        "KMaxDOCADown": 10.0, 
        "KMaxDOCAMassMeas": 2.0, 
        "KMaxDOCAMassMeasDown": 2.0, 
        "KMaxIpChi2": 25.0, 
        "KMaxIpChi2MassMeas": 25.0, 
        "KMinDIRA": 0.98, 
        "KMinDIRADown": 0.98, 
        "KMinDIRAMassMeas": 0.9998, 
        "KMinDIRAMassMeasDown": 0.999, 
        "KMinPT": 100.0, 
        "KMinPTDown": 0.0, 
        "KMinPTMassMeas": 300.0, 
        "KMinPTMassMeasDown": 250.0, 
        "KMinVDChi2": 36.0, 
        "KMinVDChi2Down": 49.0, 
        "KMinVDChi2MassMeas": 100.0, 
        "KMinVDChi2MassMeasDown": 64.0, 
        "KPiMuMuDownPrescale": 1, 
        "KPiMuMuLFVDownPrescale": 1, 
        "KPiMuMuLFVPrescale": 1, 
        "KPiMuMuPrescale": 1, 
        "KPiPiPiDownPrescale": 0.1, 
        "KPiPiPiMassMeasDownPrescale": 1, 
        "KPiPiPiMassMeasPrescale": 1, 
        "KPiPiPiPrescale": 0.01, 
        "KVDPVMaxDown": 2500.0, 
        "KVDPVMaxMassMeasDown": 2200.0, 
        "KVDPVMinDown": 500.0, 
        "KVDPVMinMassMeasDown": 900.0, 
        "KVtxChi2": 25.0, 
        "KVtxChi2Down": 25.0, 
        "KVtxChi2MassMeas": 10.0, 
        "KVtxChi2MassMeasDown": 20.0, 
        "Postscale": 1, 
        "Sigma3DauTrChi2Down": 9.0, 
        "Sigma3MassWin": 500.0, 
        "Sigma3MassWinDown": 500.0, 
        "Sigma3MaxDOCA": 25.0, 
        "Sigma3MaxDOCADown": 50.0, 
        "Sigma3MaxIpChi2": 50.0, 
        "Sigma3MaxIpChi2Down": 500.0, 
        "Sigma3MinDIRA": 0.9, 
        "Sigma3MinDIRADown": 0.1, 
        "Sigma3MinPt": 0.0, 
        "Sigma3MinPtDown": 0.0, 
        "Sigma3MinTauPs": 0.1, 
        "Sigma3MinTauPsDown": 0.1, 
        "Sigma3VtxChi2": 50.0, 
        "Sigma3VtxChi2Down": 500.0, 
        "SigmaDauTrChi2Down": 9.0, 
        "SigmaMassWin": 500.0, 
        "SigmaMassWinDown": 500.0, 
        "SigmaMaxDOCA": 2.0, 
        "SigmaMaxDOCADown": 10.0, 
        "SigmaMaxIpChi2": 36.0, 
        "SigmaMaxIpChi2Down": 25.0, 
        "SigmaMinDIRA": 0.9, 
        "SigmaMinDIRADown": 0.9, 
        "SigmaMinPt": 500.0, 
        "SigmaMinPtDown": 0.0, 
        "SigmaMinTauPs": 6.0, 
        "SigmaMinTauPsDown": 7.0, 
        "SigmaMuMuMuDownPrescale": 1, 
        "SigmaMuMuMuPrescale": 1, 
        "SigmaPEEDownPrescale": 0.1, 
        "SigmaPEEMassWinDown": 100.0, 
        "SigmaPEEPrescale": 1, 
        "SigmaPMuMuDownPrescale": 1, 
        "SigmaPMuMuLFVDownPrescale": 0.1, 
        "SigmaPMuMuLFVPrescale": 0.1, 
        "SigmaPMuMuPrescale": 1, 
        "SigmaPPi0CalPrescale": 0.25, 
        "SigmaPPi0MassWin": 150.0, 
        "SigmaPPi0Prescale": 1, 
        "SigmaVtxChi2": 36.0, 
        "SigmaVtxChi2Down": 25.0, 
        "electronMinIpChi2": 9.0, 
        "electronMinIpChi2Down": 4.0, 
        "electronPIDe": 2.0, 
        "muon3MinIpChi2": 5.0, 
        "muon3MinIpChi2Down": 5.0, 
        "muonMinIpChi2": 9.0, 
        "muonMinIpChi2Down": 9.0, 
        "pMinPt": 500.0, 
        "pi0MinPt": 0.0, 
        "protonPIDp": 5.0, 
        "protonPIDpTight": 15.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

RnS = {
    "BUILDERTYPE": "RnSConf", 
    "CONFIG": {
        "DiDOCA": 0.3, 
        "IP": 0.5, 
        "IPChi2Max": 1500, 
        "IPChi2Min": 1.5, 
        "K0s2mmLinePostscale": 1, 
        "K0s2mmLinePrescale": 1, 
        "K0s2mmSBCut": 465, 
        "K0s2mmSBLinePostscale": 1, 
        "K0s2mmSBLinePrescale": 0.1, 
        "KSdira": 0, 
        "KSip": 0.9, 
        "KSlife": 1.610411922, 
        "KSsidebmaxMass": 1000, 
        "KSsidebminMass": 600, 
        "KSsignalmaxMass": 600, 
        "KSsignalminMass": 300, 
        "L0life": 4.734283679999999, 
        "MaxIpDistRatio": 0.016666666666666666, 
        "MaxMass": 450, 
        "MinBPVDira": 0, 
        "MinVDZ": 0, 
        "MultibodyChi2dof": 9, 
        "MultibodyIPChi2": 25, 
        "NoMuIDLinePostscale": 1, 
        "NoMuIDLinePrescale": 0.001, 
        "Rho": 4, 
        "SVZ": 650, 
        "SidebandLinePostscale": 1, 
        "SidebandLinePrescale": 0.2, 
        "SignalLinePostscale": 1, 
        "SignalLinePrescale": 1, 
        "TRACK_TRGHOSTPROB_MAX": 0.3, 
        "TTHits": -1, 
        "VertexChi2": 9, 
        "cosAngle": 0.999998, 
        "eIpChi2": 49, 
        "muIpChi2": 36, 
        "muTrChi2Dof": 5, 
        "piIpChi2": 100, 
        "protonIpChi2": 16
    }, 
    "STREAMS": [ "DiMuon" ], 
    "WGs": [ "RD" ]
}

Tau2LambdaMu = {
    "BUILDERTYPE": "Tau2LambdaMuLinesConf", 
    "CONFIG": {
        "Tau2LambdaMuPrescale": 1.0, 
        "TauPostscale": 1.0, 
        "TauPrescale": 1.0
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "RD" ]
}

ZVTOP = {
    "BUILDERTYPE": "ZVTOP_Conf", 
    "CONFIG": {
        "B2TauTauSS_LinePostscale": 1, 
        "B2TauTauSS_LinePrescale": 1, 
        "B2TauTau_LinePostscale": 1, 
        "B2TauTau_LinePrescale": 1, 
        "B_TAUPI_2NDMINIPS": "20", 
        "DIRA_B": "0.99", 
        "DIRA_TAU": "0.98", 
        "DOCA_TAU": "0.4", 
        "FDCHI2_B": "225", 
        "FDCHI2_TAU": "16", 
        "FDRHO_TAU": "0.1", 
        "FDZ_TAU": "2.0", 
        "FD_B": "90", 
        "High_LinePostscale": 1, 
        "High_LinePrescale": 1, 
        "IPCHI2_B_CHILD_BEST": "16", 
        "IPCHI2_B_TAU_CHILD_BEST": "150", 
        "IPCHI2_B_TAU_CHILD_WORSE": "16", 
        "IPCHI2_HAD_ALL_FINAL_STATE": "40", 
        "IPCHI2_TAU": "9", 
        "MASS_HIGH_B": "7000", 
        "MASS_HIGH_TAU": "1800", 
        "MASS_LOW_B": "2000", 
        "MASS_LOW_TAU": "500", 
        "MCOR_HIGH_B": "10000", 
        "MCOR_LOW_B": "0", 
        "MIPCHI2_B": "16", 
        "MIPCHI2_B_HIGH": "16", 
        "PID_HAD_ALL_FINAL_STATE": "5", 
        "PTMAX_HAD_ALL_FINAL_STATE": "500", 
        "PT_B_CHILD_BEST": "2000", 
        "PT_B_PIONS_TOTAL": "7000", 
        "PT_B_TAU_CHILD_BEST": "4000", 
        "PT_B_TM": "1900", 
        "PT_B_TM_HIGH": "2000", 
        "PT_B_TT": "1900", 
        "PT_B_TT_HIGH": "2000", 
        "PT_HAD_ALL_FINAL_STATE": "1200", 
        "PT_TAU": "1250", 
        "P_B_CHILD_BEST": "10000", 
        "P_HAD_ALL_FINAL_STATE": "6000", 
        "RelatedInfoTools": [
            {
                "Location": "MuonIsolationBDT", 
                "Type": "RelInfoBstautauMuonIsolationBDT", 
                "Variables": [
                    "BSTAUTAUMUONISOBDTFIRSTVALUE", 
                    "BSTAUTAUMUONISOBDTSECONDVALUE", 
                    "BSTAUTAUMUONISOBDTTHIRDVALUE"
                ]
            }, 
            {
                "Location": "MuonIsolation", 
                "Type": "RelInfoBstautauMuonIsolation", 
                "Variables": [
                    "BSTAUTAUMUONISOFIRSTVALUE", 
                    "BSTAUTAUMUONISOSECONDVALUE"
                ]
            }, 
            {
                "Location": "TauIsolationBDT", 
                "Type": "RelInfoBstautauTauIsolationBDT", 
                "Variables": [
                    "BSTAUTAUTAUISOBDTFIRSTVALUETAUP", 
                    "BSTAUTAUTAUISOBDTSECONDVALUETAUP", 
                    "BSTAUTAUTAUISOBDTTHIRDVALUETAUP", 
                    "BSTAUTAUTAUISOBDTFIRSTVALUETAUM", 
                    "BSTAUTAUTAUISOBDTSECONDVALUETAUM", 
                    "BSTAUTAUTAUISOBDTTHIRDVALUETAUM"
                ]
            }, 
            {
                "Location": "TauIsolation", 
                "Type": "RelInfoBstautauTauIsolation", 
                "Variables": [
                    "BSTAUTAUTAUISOFIRSTVALUETAUP", 
                    "BSTAUTAUTAUISOSECONDVALUETAUP", 
                    "BSTAUTAUTAUISOFIRSTVALUETAUM", 
                    "BSTAUTAUTAUISOSECONDVALUETAUM"
                ]
            }, 
            {
                "Location": "TrackIsolationBDT", 
                "Type": "RelInfoBstautauTrackIsolationBDT", 
                "Variables": [
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOBDTFIRSTVALUETAUMPIM2", 
                    "BSTAUTAUTRACKISOBDTSECONDVALUETAUMPIM2", 
                    "BSTAUTAUTRACKISOBDTTHIRDVALUETAUMPIM2"
                ]
            }, 
            {
                "Location": "TrackIsolation", 
                "Type": "RelInfoBstautauTrackIsolation", 
                "Variables": [
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIM", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIP1", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUPPIP2", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIP", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIM1", 
                    "BSTAUTAUTRACKISOFIRSTVALUETAUMPIM2"
                ]
            }, 
            {
                "Location": "CDFIso", 
                "Type": "RelInfoBstautauCDFIso"
            }, 
            {
                "Location": "ZVisoBDT", 
                "Type": "RelInfoBstautauZVisoBDT", 
                "Variables": [
                    "ZVISOTAUP", 
                    "ZVISOTAUM"
                ]
            }
        ], 
        "TRACKCHI2_HAD_ALL_FINAL_STATE": "4", 
        "TRGHOPROB_HAD_ALL_FINAL_STATE": "0.4", 
        "VCHI2_B": "90", 
        "VCHI2_TAU": "12"
    }, 
    "STREAMS": [ "Bhadron" ], 
    "WGs": [ "RD" ]
}

