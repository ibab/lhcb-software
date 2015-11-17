##################################################################################
##                          S T R I P P I N G  2 4                              ##
##                                                                              ##
##  Configuration for QEE WG                                                    ##
##  Contact person: Chitsanu Khurewathanakul (chitsanu.khurewathanakul@cern.ch) ##
##################################################################################

from GaudiKernel.SystemOfUnits import *


DY2MuMu = {
    "BUILDERTYPE": "DY2MuMuConf", 
    "CONFIG": {
        "DY1MaxMass": 5000.0, 
        "DY1MinMass": 3200.0, 
        "DY2MaxMass": 10000.0, 
        "DY2MinMass": 5000.0, 
        "DY2MuMu1HLT2": "HLT_PASS_RE( 'Hlt2.*DiMuonDY.*Decision' )", 
        "DY2MuMu1LineHltPrescale": 0.5, 
        "DY2MuMu1LinePrescale": 0.05, 
        "DY2MuMu2HLT2": "HLT_PASS_RE( 'Hlt2.*DiMuonDY.*Decision' )", 
        "DY2MuMu2LineHltPrescale": 1.0, 
        "DY2MuMu2LinePrescale": 0.25, 
        "DY2MuMu3LinePrescale": 1.0, 
        "DY2MuMu4LinePrescale": 1.0, 
        "DY2MuMuLinePostscale": 1.0, 
        "DY3MaxMass": 20000.0, 
        "DY3MinMass": 10000.0, 
        "DY4MaxMass": 40000.0, 
        "DY4MinMass": 20000.0, 
        "RawEvents": [
            "Muon", 
            "Calo", 
            "Rich", 
            "Velo", 
            "Tracker"
        ], 
        "p": 10000.0, 
        "pT1": 1500.0, 
        "pT2": 3000.0, 
        "pid": -3.0, 
        "trkpchi2": 0.001
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

DY2ee = {
    "BUILDERTYPE": "DY2eeConf", 
    "CONFIG": {
        "DY2eeLine3Prescale": 1.0, 
        "DY2eeLine4Prescale": 1.0, 
        "DY2eeLinePostscale": 1.0, 
        "DY3MaxMass": 20000.0, 
        "DY3MinMass": 10000.0, 
        "DY4MaxMass": 40000.0, 
        "DY4MinMass": 20000.0, 
        "ECalMin": 0.1, 
        "HCalMax": 0.05, 
        "PrsCalMin": 50.0, 
        "ePID": 1.0, 
        "p3": 10000.0, 
        "p4": 10000.0, 
        "pT3": 3000.0, 
        "pT4": 5000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

Dijets = {
    "BUILDERTYPE": "DijetsConf", 
    "CONFIG": {
        "DIJET": {
            "MAX_COSDPHI": -0.8
        }, 
        "DISVR": {
            "MAX_COSDALPHA": 0.99, 
            "MAX_COSDPHI": 0, 
            "MIN_M": 2000.0, 
            "MIN_SUM_PT": 10000.0
        }, 
        "HLT": {
            "LINE": "Hlt1TrackMuon.*Decision"
        }, 
        "JET": {
            "JEC": False, 
            "MIN_PT": 19000.0, 
            "PF": True, 
            "R": 0.7, 
            "STDJETS": False
        }, 
        "MUO": {
            "MIN_MIPCHI2DV": 16, 
            "MIN_PT": 2000.0
        }, 
        "PRESCALE": 0.03, 
        "SVR": {
            "MAX_CHI2": 8, 
            "MAX_CHI2DOCA": 8, 
            "MAX_M": 7000.0, 
            "MIN_BPVDIRA": 0, 
            "MIN_BPVVDCHI2": 100, 
            "MIN_SUM_PT": 2000.0
        }, 
        "TRK": {
            "MAX_GHP": 0.4, 
            "MAX_MULT": 250, 
            "MIN_MIPCHI2DV": 16, 
            "MIN_P": 5000.0, 
            "MIN_PT": 500.0
        }
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

DisplVertices = {
    "BUILDERTYPE": "DisplVerticesLinesConf", 
    "CONFIG": {
        "DoubleSelection": {
            "ApplyMatterVeto": False, 
            "ApplyMatterVetoOne": True, 
            "MaxFractE1Track": 0.8, 
            "MaxFractTrwHitBefore": 0.49, 
            "MinHighestMass": 3000.0, 
            "MinMass": 3000.0, 
            "MinNumTracks": 6, 
            "MinRho": 0.6, 
            "MinSumPT": 3000.0, 
            "PreScale": 1.0
        }, 
        "FilterVelo": {
            "Apply": True, 
            "MinDOCABeamLine": -2.0, 
            "MinIP": 0.1, 
            "MinIPChi2": -1.0, 
            "MinNumTracks": 4, 
            "PVLocation": "Rec/Vertex/Primary", 
            "RemoveBackwardTracks": True, 
            "RemovePVTracks": False, 
            "RemoveVeloClones": True
        }, 
        "HLT": {
            "CharmHLT": "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')", 
            "HLTPS": [
                [
                    [
                        "0x001c0028", 
                        "0x002f002c"
                    ], 
                    "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')"
                ], 
                [
                    [
                        "0x00340032", 
                        "0x00730035"
                    ], 
                    "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')"
                ], 
                [
                    [
                        "0x00750037", 
                        "0x007b0038"
                    ], 
                    "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')"
                ], 
                [
                    [
                        "0x007e0039", 
                        "0x0097003d"
                    ], 
                    "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')"
                ], 
                [
                    [
                        "0x00990042", 
                        "0x40000000"
                    ], 
                    "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')"
                ]
            ], 
            "SignalLines": [
                [
                    [
                        "0x001c0028", 
                        "0x002f002c"
                    ], 
                    [ "Hlt2DisplVerticesSingleDecision" ]
                ], 
                [
                    [
                        "0x00340032", 
                        "0x00730035"
                    ], 
                    [
                        "Hlt2DisplVerticesHighFDSingleDecision", 
                        "Hlt2DisplVerticesHighMassSingleDecision", 
                        "Hlt2DisplVerticesLowMassSingleDecision", 
                        "Hlt2DisplVerticesSingleDownDecision"
                    ]
                ], 
                [
                    [
                        "0x00750037", 
                        "0x007b0038"
                    ], 
                    [
                        "Hlt2DisplVerticesSingleDecision", 
                        "Hlt2DisplVerticesSingleDownDecision", 
                        "Hlt2DisplVerticesSingleHighFDPostScaledDecision", 
                        "Hlt2DisplVerticesSingleHighMassPostScaledDecision"
                    ]
                ], 
                [
                    [
                        "0x007e0039", 
                        "0x0097003d"
                    ], 
                    [
                        "Hlt2DisplVerticesSingleDecision", 
                        "Hlt2DisplVerticesSingleDownDecision", 
                        "Hlt2DisplVerticesSingleHighFDPostScaledDecision", 
                        "Hlt2DisplVerticesSingleHighMassPostScaledDecision"
                    ]
                ], 
                [
                    [
                        "0x00990042", 
                        "0x40000000"
                    ], 
                    [
                        "Hlt2DisplVerticesSingleDecision", 
                        "Hlt2DisplVerticesSingleDownDecision", 
                        "Hlt2DisplVerticesSingleHighFDDecision", 
                        "Hlt2DisplVerticesSingleHighMassDecision", 
                        "Hlt2DisplVerticesSingleVeryHighFDDecision"
                    ]
                ]
            ]
        }, 
        "HLTPS": {
            "PreScale": 0.2
        }, 
        "HltEffCharmHLTSelection": {
            "ApplyMatterVeto": False, 
            "MaxFractE1Track": 10.0, 
            "MaxFractTrwHitBefore": 10.0, 
            "MinMass": 6000.0, 
            "MinNumTracks": 6, 
            "MinRho": 0.6, 
            "MinSumPT": 3000.0, 
            "PreScale": 1.0
        }, 
        "JetHltSingleHighMassSelection": {
            "ConeSize": 0.7, 
            "JetIDCut": None, 
            "MinDOCABL": -2.0, 
            "MinNJetMass": 0.0, 
            "MinNJetTransvMass": None, 
            "MinNumJets": 2, 
            "PreScale": 1.0, 
            "SingleJet": False
        }, 
        "JetHltSingleLowMassSelection": {
            "ConeSize": 1.2, 
            "JetIDCut": "( JNWITHPVINFO >= 5 ) & ( JMPT > 1800. ) & ( (PT/M) > 1.5 )", 
            "MinDOCABL": 0.1, 
            "MinNJetMass": 0.0, 
            "MinNJetTransvMass": None, 
            "MinNumJets": 1, 
            "PreScale": 1.0, 
            "SingleJet": True
        }, 
        "JetID": "( 0.8 > JMPF ) & ( 0.1 < JCPF ) & ( 900. < JMPT )", 
        "JetSingleHighMassSelection": {
            "ApplyMatterVeto": True, 
            "ConeSize": 0.7, 
            "JetIDCut": None, 
            "MaxFractE1Track": 0.8, 
            "MaxFractTrwHitBefore": 0.49, 
            "MinDOCABL": -2.0, 
            "MinMass": 5000.0, 
            "MinNJetMass": 0.0, 
            "MinNJetTransvMass": None, 
            "MinNumJets": 2, 
            "MinNumTracks": 5, 
            "MinRho": 0.4, 
            "MinSumPT": 7000.0, 
            "PreScale": 1.0, 
            "SingleJet": False
        }, 
        "JetSingleLowMassSelection": {
            "ApplyMatterVeto": True, 
            "ConeSize": 1.2, 
            "JetIDCut": "( JNWITHPVINFO >= 5 ) & ( JMPT > 1800. ) & ( (PT/M) > 2.5 )", 
            "MaxFractE1Track": 0.8, 
            "MaxFractTrwHitBefore": 0.49, 
            "MinDOCABL": 0.1, 
            "MinMass": 0.0, 
            "MinNJetMass": 0.0, 
            "MinNJetTransvMass": None, 
            "MinNumJets": 1, 
            "MinNumTracks": 5, 
            "MinRho": 0.4, 
            "MinSumPT": 10000.0, 
            "PreScale": 1.0, 
            "SingleJet": True
        }, 
        "RV2PDown": {
            "ApplyMatterVeto": False, 
            "ComputeMatterVeto": False, 
            "FirstPVMaxRho": 0.3, 
            "FirstPVMaxZ": 500.0, 
            "FirstPVMinNumTracks": 10, 
            "FirstPVMinZ": -300.0, 
            "MaxChi2NonVeloOnly": 5.0, 
            "MaxFractE1Track": 10.0, 
            "MaxFractTrwHitBefore": 10.0, 
            "MinMass": 3000.0, 
            "MinNumTracks": 4, 
            "MinRho": 2.0, 
            "MinSumPT": 0.0, 
            "UseVeloTracks": False
        }, 
        "RV2PWithVelo": {
            "ApplyMatterVeto": False, 
            "ComputeMatterVeto": True, 
            "FirstPVMaxRho": 0.3, 
            "FirstPVMaxZ": 500.0, 
            "FirstPVMinNumTracks": 10, 
            "FirstPVMinZ": -300.0, 
            "MaxChi2NonVeloOnly": 5.0, 
            "MaxFractE1Track": 10.0, 
            "MaxFractTrwHitBefore": 10.0, 
            "MinMass": 0.0, 
            "MinNumTracks": 4, 
            "MinRho": 0.4, 
            "MinSumPT": 0.0, 
            "UseVeloTracks": False
        }, 
        "SinglePSSelection": {
            "ApplyMatterVeto": False, 
            "MaxFractE1Track": 10.0, 
            "MaxFractTrwHitBefore": 10.0, 
            "MinMass": 3000.0, 
            "MinNumTracks": 5, 
            "MinRho": 0.5, 
            "MinSumPT": 0.0, 
            "PreScale": 0.005
        }, 
        "VeloGEC": {
            "Apply": True, 
            "MaxPhiVectorSize": 250.0, 
            "MaxVeloRatio": 0.1
        }
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

Ditau = {
    "BUILDERTYPE": "DitauConf", 
    "CONFIG": {
        "CONSTRUCTORS": {
            "EX": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 1.0
            }, 
            "EXnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.04
            }, 
            "EXssnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.04
            }, 
            "HH": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 1.0
            }, 
            "HHnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.04
            }, 
            "HHssnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.04
            }, 
            "MX": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 1.0
            }, 
            "MXnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.07
            }, 
            "MXssnoiso": {
                "RelatedInfoTools": [
                    {
                        "ConeAngle": 0.5, 
                        "DaughterLocations": {
                            " Z0 -> ^e+   X": "taueplus", 
                            " Z0 -> ^e-   X": "taueminus", 
                            " Z0 -> ^mu+  X": "taumuplus", 
                            " Z0 -> ^mu-  X": "taumuminus", 
                            " Z0 -> ^pi+  X": "tauh1plus", 
                            " Z0 -> ^pi-  X": "tauh1minus", 
                            " Z0 -> ^tau+ X": "tauh3plus", 
                            " Z0 -> ^tau- X": "tauh3minus", 
                            "[Z0 ->  e-   ^e-   ]CC": "taue2", 
                            "[Z0 ->  mu-  ^mu-  ]CC": "taumu2", 
                            "[Z0 ->  pi-  ^pi-  ]CC": "tauh12", 
                            "[Z0 ->  tau- ^tau- ]CC": "tauh32", 
                            "[Z0 -> ^e-    e-   ]CC": "taue1", 
                            "[Z0 -> ^mu-   mu-  ]CC": "taumu1", 
                            "[Z0 -> ^pi-   pi-  ]CC": "tauh11", 
                            "[Z0 -> ^tau-  tau- ]CC": "tauh31"
                        }, 
                        "IgnoreUnmatchedDescriptors": True, 
                        "Type": "RelInfoConeVariables"
                    }
                ], 
                "prescale": 0.07
            }
        }, 
        "DITAU": {
            "ee": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "e": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "eh1": {
                "ccuts": {
                    "extracut": "ATRUE", 
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "e": {
                        "extracut": "ALL"
                    }, 
                    "pi": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "eh3": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "e": {
                        "extracut": "ALL"
                    }, 
                    "tau": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "emu": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "e": {
                        "extracut": "ALL"
                    }, 
                    "mu": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "h1h1": {
                "ccuts": {
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
                }, 
                "dcuts": {
                    "pi": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "h1h3": {
                "ccuts": {
                    "min_AM": 16000.0, 
                    "min_APTMAX": 12000.0
                }, 
                "dcuts": {
                    "pi": {
                        "extracut": "ALL"
                    }, 
                    "tau": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "h1mu": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "mu": {
                        "extracut": "ALL"
                    }, 
                    "pi": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "h3h3": {
                "ccuts": {
                    "min_AM": 16000.0, 
                    "min_APTMAX": 12000.0
                }, 
                "dcuts": {
                    "tau": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "h3mu": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "mu": {
                        "extracut": "ALL"
                    }, 
                    "tau": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }, 
            "mumu": {
                "ccuts": {
                    "min_AM": 8000.0, 
                    "min_APTMAX": 4000.0
                }, 
                "dcuts": {
                    "mu": {
                        "extracut": "ALL"
                    }
                }, 
                "mcuts": {
                    "extracut": "ALL"
                }
            }
        }, 
        "PVRefitter": None, 
        "TES_e": "Phys/StdAllNoPIDsElectrons/Particles", 
        "TES_mu": "Phys/StdAllLooseMuons/Particles", 
        "TES_pi": "Phys/StdAllNoPIDsPions/Particles", 
        "ditau_pcomb": {
            "": "MomentumCombiner:PUBLIC"
        }, 
        "iso_e": {
            "min_PTFrac05C": 0.7
        }, 
        "iso_h1": {
            "min_PTFrac05C": 0.8
        }, 
        "iso_h3": {
            "min_PTFrac05C": 0.8
        }, 
        "iso_mu": {
            "min_PTFrac05C": 0.8
        }, 
        "preambulo": "\n", 
        "tau_e": {
            "ABSID": 11, 
            "ISMUONLOOSE": False, 
            "extracut": "ALL", 
            "max_ETA": 4.5, 
            "min_CaloPrsE": 50.0, 
            "min_ECALFrac": 0.1, 
            "min_ETA": 2.0, 
            "min_PT": 4000.0, 
            "min_TRPCHI2": 0.01
        }, 
        "tau_h1": {
            "ISMUONLOOSE": False, 
            "ISPIONORKAON": True, 
            "extracut": "ALL", 
            "max_ETA": 4.5, 
            "min_ETA": 2.0, 
            "min_PT": 4000.0, 
            "min_TRPCHI2": 0.01
        }, 
        "tau_h3": {
            "ccuts": {
                "max_AM": 1500.0, 
                "min_AM": 600.0
            }, 
            "dcuts": {
                "ISMUONLOOSE": False, 
                "ISPIONORKAON": True, 
                "extracut": "ALL", 
                "max_ETA": 4.5, 
                "min_ETA": 2.0, 
                "min_PT": 500.0, 
                "min_TRPCHI2": 0.01
            }, 
            "mcuts": {
                "max_DRTRIOMAX": 0.4, 
                "max_VCHI2PDOF": 20.0, 
                "min_PTTRIOMIN": 500.0
            }
        }, 
        "tau_mu": {
            "ABSID": 13, 
            "ISMUON": True, 
            "extracut": "ALL", 
            "max_ETA": 4.5, 
            "min_ETA": 2.0, 
            "min_PT": 4000.0, 
            "min_TRPCHI2": 0.01
        }
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

H24Mu = {
    "BUILDERTYPE": "H24MuLineConf", 
    "CONFIG": {
        "A1Dira": 0, 
        "A1Doca": 0.2, 
        "A1DocaTight": 0.1, 
        "A1Doca_loose": 10, 
        "A1FDChi2": 4, 
        "A1Vchi2": 7.5, 
        "A1Vchi2Tight": 1, 
        "A1Vchi2_loose": 20, 
        "A1maxIPchi2": 25, 
        "A1maxMass": 2000, 
        "A1maxMass_loose": 5000, 
        "DefaultPostscale": 1, 
        "DetachedLinePrescale": 1, 
        "HVchi2": 10, 
        "HVchi2Tight": 2, 
        "HVchi2_loose": 50, 
        "HmaxDOCA": 0.75, 
        "HmaxDOCATight": 0.25, 
        "HmaxDOCA_loose": 1000000, 
        "HpT": 1200, 
        "HpT_loose": 300, 
        "LooseLinePrescale": 0.01, 
        "MuGhostProb": 0.4, 
        "MuMaxIPchi2": 3, 
        "MuMaxIPchi2_loose": 1000000, 
        "MuMinIPchi2": 1, 
        "MuNShared": 3, 
        "MuPIDdll": -3, 
        "MuTrackChi2DoF": 3, 
        "MuTrackChi2DoF_loose": 10, 
        "MupT_loose": 0, 
        "MupTdetached": 250, 
        "MupTprompt": 375, 
        "PromptLinePrescale": 1, 
        "SimpleLinePrescale": 1
    }, 
    "STREAMS": [ "Leptonic" ], 
    "WGs": [ "QEE" ]
}

HighPtTau = {
    "BUILDERTYPE": "HighPtTauConf", 
    "CONFIG": {
        "2p_pi_PT": 5000.0, 
        "2p_sumPT": 8000.0, 
        "3p_pi_PT": 2000.0, 
        "3p_sumPT": 8000.0, 
        "CORRM_MAX": 2000.0, 
        "CORRM_MIN": 1200.0, 
        "DOCA_MAX": 0.1, 
        "FDCHI2_MIN": 10, 
        "FDT_MIN": 0.5, 
        "HLT2_2Prong": "HLT_PASS_RE('Hlt2EWSingleTauHighPt2ProngDecision')", 
        "HLT2_3Prong": "HLT_PASS_RE('Hlt2EWSingleTauHighPt3ProngDecision')", 
        "HighPtTau2ProngLoose_Postscale": 1.0, 
        "HighPtTau2ProngLoose_Prescale": 0.1, 
        "HighPtTau2Prong_Postscale": 1.0, 
        "HighPtTau2Prong_Prescale": 1.0, 
        "HighPtTau3ProngLoose_Postscale": 1.0, 
        "HighPtTau3ProngLoose_Prescale": 0.1, 
        "HighPtTau3Prong_Postscale": 1.0, 
        "HighPtTau3Prong_Prescale": 1.0, 
        "ISO_MAX": 4000.0, 
        "M_MAX": 2000.0, 
        "M_MIN": 0.0, 
        "PT_2Prong": 15000.0, 
        "PT_3Prong": 15000.0, 
        "RHO_M_MAX": 2000.0, 
        "RHO_M_MIN": 0.0, 
        "RHO_PT_MIN": 0.0, 
        "VCHI2_NDOF_MAX": 20, 
        "maxchildPT": 5000.0, 
        "nJets_MAX": -1, 
        "pi0_PT": 5000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

InclbJets = {
    "BUILDERTYPE": "InclbJetsConf", 
    "CONFIG": {
        "DZSVPVCut": 1.0, 
        "HLT2": "HLT_PASS_RE('Hlt2.*Topo.*Decision')", 
        "NrPVsCut": 1, 
        "NrSeedsCut": 2, 
        "PrtIPSCut": 2.5, 
        "PrtMomCut": 2.0, 
        "PrtPtCut": 0.6, 
        "SumMomSVCut": 1.0, 
        "TrkChi2Cut": 3.0, 
        "VertexFitter": "LoKi::VertexFitter:PUBLIC", 
        "VtxChi2Cut": 20.0, 
        "scale": 0.05
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

LLP2MuX = {
    "BUILDERTYPE": "LLP2MuXConf", 
    "CONFIG": {
        "HLT1": "HLT_PASS_RE('Hlt1.*SingleMuonHighPTDecision')", 
        "HLT2": "HLT_PASS_RE('Hlt2.*SingleMuon.*High.*Decision')", 
        "L0DU": "L0_CHANNEL('Muon')", 
        "MinIP": 0.25, 
        "MinPT": 12000.0, 
        "RawEvents": [
            "Trigger", 
            "Muon", 
            "Calo", 
            "Rich"
        ]
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

LowMultINC = {
    "BUILDERTYPE": "LowMultINCLines", 
    "CONFIG": {
        "DecisionHlt1NoBiasNonBeamBeam": "HLT_PASS_RE('Hlt1NoBiasNonBeamBeamDecision')", 
        "DecisionHlt2PassThrough": "HLT_PASS_RE('Hlt2PassThroughDecision')", 
        "DecisionL0DiEM": None, 
        "DecisionL0DiHadron": None, 
        "DecisionL0DiMuon": None, 
        "DecisionL0Electron": None, 
        "DecisionL0Muon": None, 
        "DecisionL0Photon": None, 
        "DecisionLowMultChiC2HH": "HLT_PASS_RE('Hlt2LowMultChiC2HHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHWSDecision')", 
        "DecisionLowMultChiC2HHHH": "HLT_PASS_RE('Hlt2LowMultChiC2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultChiC2HHHHWSDecision')", 
        "DecisionLowMultChiC2PP": "HLT_PASS_RE('Hlt2LowMultChiC2PPDecision') | HLT_PASS_RE('Hlt2LowMultChiC2PPWSDecision')", 
        "DecisionLowMultD2K3Pi": "HLT_PASS_RE('Hlt2LowMultD2K3PiDecision') | HLT_PASS_RE('Hlt2LowMultD2K3PiWSDecision')", 
        "DecisionLowMultD2KKPi": "HLT_PASS_RE('Hlt2LowMultD2KKPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KKPiWSDecision')", 
        "DecisionLowMultD2KPi": "HLT_PASS_RE('Hlt2LowMultD2KPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiWSDecision')", 
        "DecisionLowMultD2KPiPi": "HLT_PASS_RE('Hlt2LowMultD2KPiPiDecision') | HLT_PASS_RE('Hlt2LowMultD2KPiPiWSDecision')", 
        "DecisionLowMultDiElectron": "HLT_PASS_RE('Hlt2LowMultDiElectronDecision')", 
        "DecisionLowMultDiElectron_noTrFilt": "HLT_PASS_RE('Hlt2LowMultDiElectron_noTrFiltDecision')", 
        "DecisionLowMultDiMuon": "HLT_PASS('Hlt2LowMultDiMuonDecision')", 
        "DecisionLowMultDiMuon_PS": "HLT_PASS('Hlt2LowMultDiMuon_PSDecision')", 
        "DecisionLowMultDiPhoton": "HLT_PASS_RE('Hlt2LowMultDiPhotonDecision')", 
        "DecisionLowMultDiPhoton_HighMass": "HLT_PASS_RE('Hlt2LowMultDiPhoton_HighMassDecision')", 
        "DecisionLowMultHadron_noTrFilt": "HLT_PASS_RE('Hlt2LowMultHadron_noTrFiltDecision')", 
        "DecisionLowMultL2pPi": "HLT_PASS_RE('Hlt2LowMultL2pPiDecision') | HLT_PASS_RE('Hlt2LowMultL2pPiWSDecision')", 
        "DecisionLowMultLMR2HH": "HLT_PASS_RE('Hlt2LowMultLMR2HHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWSDecision')", 
        "DecisionLowMultLMR2HHHH": "HLT_PASS_RE('Hlt2LowMultLMR2HHHHDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWSDecision')", 
        "DecisionLowMultLMR2HHHH_heavyPS": "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_heavyPSDecision')", 
        "DecisionLowMultLMR2HHHH_mediumPS": "HLT_PASS_RE('Hlt2LowMultLMR2HHHH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHHHWS_mediumPSDecision')", 
        "DecisionLowMultLMR2HH_heavyPS": "HLT_PASS_RE('Hlt2LowMultLMR2HH_heavyPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_heavyPSDecision')", 
        "DecisionLowMultLMR2HH_mediumPS": "HLT_PASS_RE('Hlt2LowMultLMR2HH_mediumPSDecision') | HLT_PASS_RE('Hlt2LowMultLMR2HHWS_mediumPSDecision')", 
        "DecisionLowMultMuon": "HLT_PASS('Hlt2LowMultMuonDecision')", 
        "DecisionLowMultNonBeamBeamNoBias": "HLT_PASS_RE('Hlt2NoBiasNonBeamBeamDecision')", 
        "DecisionLowMultPi0": "HLT_PASS_RE('Hlt2LowMultPi0Decision')", 
        "DecisionLowMultTMP1": None, 
        "DecisionLowMultTMP2": None, 
        "DecisionLowMultTechnical": "HLT_PASS_RE('Hlt2LowMultTechnical_MinBiasDecision')", 
        "LowMultRequiredRawEvents": [
            "Velo", 
            "Calo", 
            "HC", 
            "Trigger", 
            "Muon", 
            "Rich", 
            "Tracker"
        ], 
        "PrescaleHlt1NoBiasNonBeamBeam": 1, 
        "PrescaleHlt2PassThrough": 1, 
        "PrescaleL0DiEM": 0, 
        "PrescaleL0DiHadron": 0, 
        "PrescaleL0DiMuon": 0, 
        "PrescaleL0Electron": 0, 
        "PrescaleL0Muon": 0, 
        "PrescaleL0Photon": 0, 
        "PrescaleLowMultBXTYPE": 1, 
        "PrescaleLowMultChiC2HH": 1, 
        "PrescaleLowMultChiC2HHHH": 1, 
        "PrescaleLowMultChiC2PP": 1, 
        "PrescaleLowMultD2K3Pi": 1, 
        "PrescaleLowMultD2KKPi": 1, 
        "PrescaleLowMultD2KPi": 1, 
        "PrescaleLowMultD2KPiPi": 1, 
        "PrescaleLowMultDiElectron": 1, 
        "PrescaleLowMultDiElectron_noTrFilt": 1, 
        "PrescaleLowMultDiMuon": 1, 
        "PrescaleLowMultDiMuon_PS": 1, 
        "PrescaleLowMultDiPhoton": 1, 
        "PrescaleLowMultDiPhoton_HighMass": 1, 
        "PrescaleLowMultHadron_noTrFilt": 1, 
        "PrescaleLowMultL2pPi": 1, 
        "PrescaleLowMultLMR2HH": 1, 
        "PrescaleLowMultLMR2HHHH": 1, 
        "PrescaleLowMultLMR2HHHH_heavyPS": 1, 
        "PrescaleLowMultLMR2HHHH_mediumPS": 1, 
        "PrescaleLowMultLMR2HH_heavyPS": 1, 
        "PrescaleLowMultLMR2HH_mediumPS": 1, 
        "PrescaleLowMultMuon": 1, 
        "PrescaleLowMultNonBeamBeamNoBias": 1, 
        "PrescaleLowMultPi0": 1, 
        "PrescaleLowMultTMP1": 0, 
        "PrescaleLowMultTMP2": 0, 
        "PrescaleLowMultTechnical": 1
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

MuMuSS = {
    "BUILDERTYPE": "MuMuSSConf", 
    "CONFIG": {
        "MuMuSSLine1MaxMass": 5000.0, 
        "MuMuSSLine1MinMass": 3200.0, 
        "MuMuSSLine1Prescale": 0.1, 
        "MuMuSSLine2MaxMass": 10000.0, 
        "MuMuSSLine2MinMass": 5000.0, 
        "MuMuSSLine2Prescale": 1.0, 
        "MuMuSSLine3MaxMass": 20000.0, 
        "MuMuSSLine3MinMass": 10000.0, 
        "MuMuSSLine3Prescale": 1.0, 
        "MuMuSSLine4MinMass": 20000.0, 
        "MuMuSSLine4Prescale": 1.0, 
        "MuMuSSLinePostscale": 1.0, 
        "p": 10000.0, 
        "pT1": 1500.0, 
        "pT2": 3000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

SbarSCorrelations = {
    "BUILDERTYPE": "SbarSCorrelationsConf", 
    "CONFIG": {
        "F2prescale": 1.0, 
        "Fisher": 10, 
        "HLT": "HLT_PASS_RE('Hlt1.*NoBias.*Decision')|HLT_PASS_RE('Hlt1.*MB.*Bias.*Decision')|HLT_PASS_RE('Hlt1.*MicroBias.*Decision')", 
        "KAON_PIDK_MIN": 8, 
        "KAON_PIDKp_MIN": 0, 
        "KAON_ipChi2_MAX": 49, 
        "LambdaCprescale": 1.0, 
        "Lambda_Adamass": 50.0, 
        "Lambda_V_Chi2_Max": 9, 
        "Lambda_ipChi2_MAX": 49, 
        "LongTrackGEC": 1000, 
        "PION_P_MIN": 2000.0, 
        "PION_ipChi2_MIN": 9, 
        "PROTON_P_MIN": 2000.0, 
        "PROTON_ipChi2_MIN": 9, 
        "Phiprescale": 0.05, 
        "Trk_P_MIN": 5000.0, 
        "isLong": "(ISLONG)", 
        "postscale": 1.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

SingleTrackTIS = {
    "BUILDERTYPE": "SingleTrackTISConf", 
    "CONFIG": {
        "SingleTrackTISLow_Prescale": 0.01, 
        "SingleTrackTIS_Postscale": 1.0, 
        "SingleTrackTIS_Prescale": 0.1, 
        "pT": 20000.0, 
        "pTlow": 15000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

StrangeBaryons = {
    "BUILDERTYPE": "StrippingStrangeBaryonsConf", 
    "CONFIG": {
        "CHI2VTX_L": 15.0, 
        "CHI2VTX_Omega": 9.0, 
        "CHI2VTX_Xi": 25.0, 
        "COS_L_Omega": 0.9996, 
        "COS_L_Xi": 0.9996, 
        "HLT1": "HLT_PASS_RE('Hlt1.*NoBias.*Decision')|HLT_PASS_RE('Hlt1.*MB.*Bias.*Decision')|HLT_PASS_RE('Hlt1.*MicroBias.*Decision')", 
        "L_FDCHI2_OWNPV": 100.0, 
        "L_FDCHI2_OWNPV_LL": 150.0, 
        "L_FDCHI2_OWNPV_LL_Omega": 70.0, 
        "L_FDCHI2_OWNPV_Omega": 70.0, 
        "Lambda0MassWindow": 30.0, 
        "Lambda0MassWindowPost": 6.0, 
        "OmegaMassWindow": 50.0, 
        "Omega_FDCHI2_OWNPV": 10.0, 
        "PionPIDpiK": 0.0, 
        "PreScale": 1, 
        "ProtonPIDppi": -5.0, 
        "RawEvents": [
            "Muon", 
            "Calo", 
            "Rich"
        ], 
        "TRCHI2DOF": 4.0, 
        "XiMassWindow": 50.0, 
        "Xi_FDCHI2_OWNPV": 5.0, 
        "Xi_FDCHI2_OWNPV_DDL": 15.0, 
        "Xi_FDCHI2_OWNPV_LLL": 30.0, 
        "checkPV": True, 
        "minCHI2IPPV_Bachelor": 3.0, 
        "minCHI2IPPV_K_Bachelor_D": 3.0, 
        "minCHI2IPPV_K_Bachelor_L": 3.0, 
        "minCHI2IPPV_L": 2.0, 
        "minCHI2IPPV_L_LL": 9.0, 
        "minCHI2IPPV_Pi_Bachelor_DDD": 4.0, 
        "minCHI2IPPV_Pi_Bachelor_LLL": 10.0, 
        "minCHI2IPPV_pPi": 4.0, 
        "minCHI2IPPV_pPi_LL": 20.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

StrangeBaryonsNoPID = {
    "BUILDERTYPE": "StrippingStrangeBaryonsNoPIDConf", 
    "CONFIG": {
        "CHI2VTX_L": 15.0, 
        "CHI2VTX_Omega": 9.0, 
        "CHI2VTX_Xi": 25.0, 
        "COS_L_Omega": 0.9996, 
        "COS_L_Xi": 0.9996, 
        "HLT1": "HLT_PASS_RE('Hlt1.*NoBias.*Decision')|HLT_PASS_RE('Hlt1.*MB.*Bias.*Decision')|HLT_PASS_RE('Hlt1.*MicroBias.*Decision')", 
        "L_FDCHI2_OWNPV": 100.0, 
        "L_FDCHI2_OWNPV_LL": 150.0, 
        "L_FDCHI2_OWNPV_LL_Omega": 70.0, 
        "L_FDCHI2_OWNPV_Omega": 70.0, 
        "Lambda0MassWindow": 30.0, 
        "Lambda0MassWindowPost": 6.0, 
        "OmegaMassWindow": 50.0, 
        "Omega_FDCHI2_OWNPV": 10.0, 
        "PionPIDpiK": 0.0, 
        "PreScale": 1, 
        "ProtonPIDppi": -5.0, 
        "RawEvents": [
            "Muon", 
            "Calo", 
            "Rich"
        ], 
        "TRCHI2DOF": 4.0, 
        "XiMassWindow": 50.0, 
        "Xi_FDCHI2_OWNPV": 5.0, 
        "Xi_FDCHI2_OWNPV_DDL": 15.0, 
        "Xi_FDCHI2_OWNPV_LLL": 30.0, 
        "checkPV": False, 
        "minCHI2IPPV_Bachelor": 3.0, 
        "minCHI2IPPV_K_Bachelor_D": 3.0, 
        "minCHI2IPPV_K_Bachelor_L": 3.0, 
        "minCHI2IPPV_L": 2.0, 
        "minCHI2IPPV_L_LL": 9.0, 
        "minCHI2IPPV_Pi_Bachelor_DDD": 4.0, 
        "minCHI2IPPV_Pi_Bachelor_LLL": 10.0, 
        "minCHI2IPPV_pPi": 4.0, 
        "minCHI2IPPV_pPi_LL": 20.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

TaggedJets = {
    "BUILDERTYPE": "TaggedJetsConf", 
    "CONFIG": {
        "DiTaggedJetsPair_Postscale": 1.0, 
        "DiTaggedJetsPair_Prescale": 1.0, 
        "TaggedJetsEight_Postscale": 1.0, 
        "TaggedJetsEight_Prescale": 1.0, 
        "TaggedJetsFour_Postscale": 1.0, 
        "TaggedJetsFour_Prescale": 1.0, 
        "TaggedJetsPairExclusiveDiJet_Postscale": 1.0, 
        "TaggedJetsPairExclusiveDiJet_Prescale": 1.0, 
        "TaggedJetsPair_Postscale": 1.0, 
        "TaggedJetsPair_Prescale": 1.0, 
        "TaggedJetsSix_Postscale": 1.0, 
        "TaggedJetsSix_Prescale": 1.0, 
        "min_jet_pT": 25000.0, 
        "min_jet_pT_ExclusiveDiJet": 20000.0
    }, 
    "STREAMS": [ "BhadronCompleteEvent" ], 
    "WGs": [ "QEE" ]
}

WMu = {
    "BUILDERTYPE": "WMuConf", 
    "CONFIG": {
        "HLT1_SingleTrackNoBias": "HLT_PASS( 'Hlt1MBNoBiasDecision' )", 
        "HLT2_Control10": "HLT_PASS_RE('Hlt2.*SingleMuon.*High.*Decision')", 
        "HLT2_Control4800": "HLT_PASS_RE('Hlt2.*SingleMuonLow.*Decision')", 
        "RawEvents": [
            "Muon", 
            "Calo", 
            "Rich", 
            "Velo", 
            "Tracker"
        ], 
        "STNB_Prescale": 0.2, 
        "SingMuon10_Prescale": 0.01, 
        "SingMuon10_pT": 10000.0, 
        "SingMuon48_Prescale": 0.4, 
        "SingMuon48_pT": 4800.0, 
        "WMuLow_Prescale": 0.1, 
        "WMu_Postscale": 1.0, 
        "WMu_Prescale": 1.0, 
        "pT": 20000.0, 
        "pTlow": 15000.0, 
        "pTvlow": 5000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

We = {
    "BUILDERTYPE": "WeConf", 
    "CONFIG": {
        "ECalMin": 0.1, 
        "HCalMax": 0.05, 
        "PrsCalMin": 50.0, 
        "WeLow_Prescale": 0.1, 
        "We_Postscale": 1.0, 
        "We_Prescale": 1.0, 
        "pT": 20000.0, 
        "pTlow": 15000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

WeAKTJets = {
    "BUILDERTYPE": "WeAKTJetsConf", 
    "CONFIG": {
        "ECalMin": 0.1, 
        "HCalMax": 0.05, 
        "PrsCalMin": 50.0, 
        "WeAKTJets_Postscale": 1.0, 
        "WeAKTJets_Prescale": 1.0, 
        "dr_lepton_jet": 0.5, 
        "max_e_pT": 200000.0, 
        "min_e_pT": 10.0, 
        "min_jet_pT": 15.0
    }, 
    "STREAMS": [ "BhadronCompleteEvent" ], 
    "WGs": [ "QEE" ]
}

WmuAKTJets = {
    "BUILDERTYPE": "WmuAKTJetsConf", 
    "CONFIG": {
        "WmuAKTJets_Postscale": 1.0, 
        "WmuAKTJets_Prescale": 1.0, 
        "dr_lepton_jet": 0.5, 
        "max_mu_pT": 200000.0, 
        "min_jet_pT": 15.0, 
        "min_mu_pT": 10.0
    }, 
    "STREAMS": [ "BhadronCompleteEvent" ], 
    "WGs": [ "QEE" ]
}

Z02MuMu = {
    "BUILDERTYPE": "Z02MuMuConf", 
    "CONFIG": {
        "MMmin": 40000.0, 
        "Z02MuMu_Postscale": 1.0, 
        "Z02MuMu_Prescale": 1.0, 
        "pT": 3000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

Z02ee = {
    "BUILDERTYPE": "Z02eeConf", 
    "CONFIG": {
        "ECalMin": 0.1, 
        "HCalMax": 0.05, 
        "MMmin": 40000.0, 
        "PrsCalMin": 50.0, 
        "Z02ee_Postscale": 1.0, 
        "Z02ee_Prescale": 1.0, 
        "pT": 10000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

