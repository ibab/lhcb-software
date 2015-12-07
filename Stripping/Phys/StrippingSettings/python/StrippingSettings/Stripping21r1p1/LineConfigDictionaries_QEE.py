##################################################################################
##                          S T R I P P I N G  21r1p1                           ##
##                                                                              ##
##  Configuration for QEE WG                                                    ##
##  Contact person: Chitsanu Khurewathanakul (chitsanu.khurewathanakul@cern.ch) ##
##################################################################################

from GaudiKernel.SystemOfUnits import *

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
                "prescale": 0.05
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
                "prescale": 0.05
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
                "prescale": 0.05
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
                "prescale": 0.05
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
                "prescale": 0.1
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
                "prescale": 0.1
            }
        }, 
        "DITAU": {
            "ee": {
                "ccuts": {
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
                    "min_AM": 12000.0, 
                    "min_APTMAX": 9000.0
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
            "max_ETA": 3.75, 
            "min_ETA": 2.25, 
            "min_HCALFrac": 0.05, 
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
                "min_PT": 4000.0
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

FullDiJets = {
    "BUILDERTYPE": "FullDiJetsConf", 
    "CONFIG": {
        "FullDiJetsLine_Postscale": 1.0, 
        "FullDiJetsLine_Prescale": 0.05, 
        "min_jet_pT": 20000.0
    }, 
    "STREAMS": [ "EW" ], 
    "WGs": [ "QEE" ]
}

Lb2dp = {
    "BUILDERTYPE": "Lb2dpConf", 
    "CONFIG": {
        "KaonP": 25000, 
        "KaonPT": 1000, 
        "LbDIRA": 0.9999, 
        "LbFDChi2": 150, 
        "LbMassMax": 6000.0, 
        "LbMassMin": 5200.0, 
        "LbPT": 1500, 
        "LbVtxChi2": 10.0, 
        "LbVtxChi2_pipi": 10.0, 
        "PionPIDKpi": 2, 
        "PionPT": 500, 
        "Prescale": 1.0, 
        "Prescale_pipi": 1.0, 
        "ProtonP": 15000, 
        "ProtonPIDpK": 10, 
        "ProtonPIDppi": 10, 
        "ProtonPT": 1000, 
        "SumPT": 3000, 
        "TrackChi2Ndof": 3.0, 
        "TrackGhostProb": 0.5, 
        "TrackIPChi2": 16.0
    }, 
    "STREAMS": [ "BhadronCompleteEvent" ], 
    "WGs": [ "QEE" ]
}

MicroDiJets = {
    "BUILDERTYPE": "MicroDiJetsConf", 
    "CONFIG": {
        "MicroDiJetsLine_Postscale": 1.0, 
        "MicroDiJetsLine_Prescale": 0.5, 
        "min_jet_pT": 20000.0
    }, 
    "STREAMS": [ "Leptonic" ], 
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
