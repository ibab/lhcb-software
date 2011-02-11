"""
Place holder for stripping dictionary configurations under development.
These will not be used for production, only for testing.
Dictionaries have the name of the line builder instance.
"""

#bob=1

_dict = {}

# standard Bs->Jpsi/Phi prescaled and detatched lines
Bs2JpsiPhiPrescaledAndDetatched = {
    'BUILDERTYPE' : 'Bs2JpsiPhiPrescaledAndDetatchedConf',
    'CONFIG' : {'PhiPT' : 500.,
                'BsMassWin' : 300.,
                'BsLTFITCHI2' : 14.,
                'BsVCHI2' : 20.,
                'DetatchedBsLTIME' : 15.,
                'PrescaledLinePrescale' : 1.0,
                'PrescaledLinePostscale' : 0.1,
                'DetatchedLinePrescale' : 1.0,
                'DetatchedLinePostscale' : 1.0
                }
    }

# Prescaled Bs->Jpsi/Phi prescaled and detatched lines
# Copy standard parameters, change pre-scales.
#Bs2JpsiPhiPrescaledAndDetatchedPrescaled = dict(Bs2JpsiPhiPrescaledAndDetatched)
#Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['PrescaledLinePrescale'] = 0.1
#Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['DetatchedLinePrescale'] = 0.1

B2XGamma = {
    'BUILDERTYPE' : 'StrippingBsXGammaConf',
    'CONFIG' : {'TrIPchi2Phi'          : 10.       # Dimensionless
                ,'TrIPchi2Kst'         : 10.       # Dimensionless
                ,'PhiMassWin'          : 15.       # MeV
                ,'KstMassWin'          : 100.      # MeV
                ,'KstMassWinSB'        : 200.      # MeV
                ,'BsMassWin'           : 1000.     # MeV
                ,'B0MassWin'           : 1000.     # MeV
                ,'BMassWinSB'          : 2000.     # MeV
                ,'BsDirAngle'          : 0.02      # radians
                ,'B0DirAngle'          : 0.02      # radians
                ,'BDirAngleMoni'       : 0.06      # radians
                ,'BsPVIPchi2'          : 15.       # Dimensionless
                ,'B0PVIPchi2'          : 15.       # Dimensionless
                ,'photonPT'            : 2600.     # MeV
                ,'PhiVCHI2'            : 15.       # dimensionless
                ,'KstVCHI2'            : 15.       # dimensionless
                ,'TrChi2'              : 5.        # dimensionless
                # Pre- and postscales
                ,'Bs2PhiGammaWideBMassPreScale'      : 0.1
                ,'Bs2PhiGammaWideBMassPostScale'     : 1.0
                ,'Bs2PhiGammaLooseDiraPreScale'      : 0.1
                ,'Bs2PhiGammaLooseDiraPostScale'     : 1.0
                ,'Bs2PhiGammaPreScale'               : 1.0
                ,'Bs2PhiGammaPostScale'              : 1.0
                ,'Bd2KstGammaWideBMassPreScale'      : 0.05
                ,'Bd2KstGammaWideBMassPostScale'     : 1.0
                ,'Bd2KstGammaLooseDiraPreScale'      : 0.05
                ,'Bd2KstGammaLooseDiraPostScale'     : 1.0
                ,'Bd2KstGammaWideKstMassPreScale'    : 0.05
                ,'Bd2KstGammaWideKstMassPostScale'   : 1.0
                ,'Bd2KstGammaWidePreScale'           : 0.05
                ,'Bd2KstGammaWidePostScale'          : 1.0
                ,'Bd2KstGammaPreScale'               : 1.0
                ,'Bd2KstGammaPostScale'              : 1.0
                }
    }

B2hhLTUnbiased = {
    'BUILDERTYPE' : 'StrippingB2hhLTUnbiasedConf' ,
    'CONFIG' : {'TrackChi2'               :    5.0
                , 'DaughterPtMin'         :    1.5 # GeV
                , 'DaughterPtMax'         :    2.0 # GeV
                , 'DaughterPMin'          :   10.0 # GeV
                , 'DaughterPIDKMax'       :    0.1
                , 'BMassMin'              :    5.0 # GeV
                , 'BMassMax'              :    6.0 # GeV
                , 'DOCA'                  :    0.07
                , 'VertexChi2'            :   25.0
                , 'PrescaleLoose'         :    0.005
                , 'PostscaleLoose'        :    1.0
                , 'PrescaleNB'            :    1.0
                , 'PostscaleNB'           :    1.0
                , 'NetCut'                :   -1.0
                }
    }

NeuroBayesMuMu = {
    'BUILDERTYPE' : 'StrippingNeuroBayesMuMuConf',
    'CONFIG' : {'trackChi2'              :    5.0
                , 'MuMuMassMin'          :    0.0
                , 'MuMuMassMax'          :   20.0
                , 'JPsiMassMin'          :    3.04 # GeV
                , 'JPsiMassMax'          :    3.14 # GeV
                , 'vertexChi2'           :   25.0
                , 'NBCutJPsi'            :    0.7
                , 'NBCutMuMu'            :    0.95
                , 'ExpertiseJPsi'        : 'Muon/mumu_net_noip.nb'
                , 'ExpertiseMuMu'        : 'Muon/mumu_net_full.nb'
                , 'NBVersionJPsi'        : "TuneSep2010"
                , 'NBVersionMuMu'        : "TuneSep2010"
                , 'PrescaleMuMu'         :  1.0
                , 'PostscaleMuMu'        :  1.0
                , 'PrescaleJPsi'         :  1.0
                , 'PostscaleJPsi'        :  1.0
                }
    }
