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
Bs2JpsiPhiPrescaledAndDetatchedPrescaled = dict(Bs2JpsiPhiPrescaledAndDetatched)
Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['PrescaledLinePrescale'] = 0.1
Bs2JpsiPhiPrescaledAndDetatchedPrescaled['CONFIG']['DetatchedLinePrescale'] = 0.1

B2XGamma = {
    'BUILDERTYPE' : 'StrippingB2XGammaConf',
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

B2twobody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {'nbody':              2,
                'MinBMass':        4500.0,
                'MaxBMass':        8000.0,
                'MinBPt':          2000.0,
                'MaxBVertChi2DOF':   10.0,
                'MinBPVVDChi2':     225.0,
                'MaxBPVIPChi2':      15.0,
                'MinBPVDIRA':         0.0,
                'MinNvc':             2,
                'doPi':            True,
                'doK':             True,
                'dop':             True,
                'doKs':            True,
                'doLm':            True,
                'doDz':            True,
                'doDp':            True,
                'doDs':            True,
                'doLc':            True,
                'doPh':            True,
                'doKS':            True,
                'doJp':            True,
                'doDS':            True,
                'MinPiPt':         1500.0,
                'MinPiIPChi2DV':     25.0,
                'MaxPiChi2':          5.0,
                'MinKPt':          1500.0,
                'MinKIPChi2DV':      25.0,
                'MaxKChi2':           5.0,
                'MinpPt':          1500.0,
                'MinpIPChi2DV':      25.0,
                'MaxpChi2':           5.0,
                'MaxKsDeltaM':       40.0,
                'MinKsPt':         1500.0,
                'MaxKsVertChi2DOF':  10.0,
                'MinKsPVVDChi2':    400.0,
                'MinKsIPChi2':        0.0,
                'MinKsDauPt':       200.0,
                'MinKsDauIPChi2':    16.0,
                'MaxKsDauTrkChi2':   10.0,
                'MaxLmDeltaM':       15.0,
                'MinLmPt':         1500.0,
                'MaxLmVertChi2DOF':  10.0,
                'MinLmPVVDChi2':    400.0,
                'MinLmIPChi2':        0.0,
                'MinLmPrtPt':       500.0,
                'MinLmPiPt':        100.0,
                'MinLmPrtIPChi2':     4.0,
                'MinLmPiIPChi2':     16.0,
                'MaxLmPrtTrkChi2':   10.0,
                'MaxLmPiTrkChi2':    10.0,
                'MaxDzDeltaM':      40.0,
                'MinDzPt':         1500.0,
                'MaxDzVertChi2DOF':  10.0,
                'MinDzPVVDChi2':    225.0,
                'MinDzIPChi2':        0.0,
                'MinDzDauPt':       250.0,
                'MinDzDauIPChi2':     9.0,
                'MaxDzDauTrkChi2':    5.0,
                'MinDzPIDK':         -5.0,
                'MaxDpDeltaM':       40.0,
                'MinDpPt':         1500.0,
                'MaxDpVertChi2DOF':  10.0,
                'MinDpPVVDChi2':    225.0,
                'MinDpIPChi2':        0.0,
                'MinDpDauPt':       250.0,
                'MinDpDauIPChi2':     9.0,
                'MaxDpDauTrkChi2':    5.0,
                'MinDpPIDK':         -5.0,
                'MaxDsDeltaM':       40.0,
                'MinDsPt':         1500.0,
                'MaxDsVertChi2DOF':  10.0,
                'MinDsPVVDChi2':    225.0,
                'MinDsIPChi2':        0.0,
                'MinDsDauPt':       250.0,
                'MinDsDauIPChi2':     9.0,
                'MaxDsDauTrkChi2':    5.0,
                'MinDsPIDK':         -5.0,
                'MaxLcDeltaM':       40.0,
                'MinLcPt':         1500.0,
                'MaxLcVertChi2DOF':  10.0,
                'MinLcPVVDChi2':    225.0,
                'MinLcIPChi2':        0.0,
                'MinLcDauPt':       250.0,
                'MinLcDauIPChi2':     9.0,
                'MaxLcDauTrkChi2':    5.0,
                'MinLcPIDK':         -5.0,
                'MinLcPIDp':         -5.0,
                'MaxPhDeltaM':       30.0,
                'MinPhPt':         1500.0,
                'MaxPhVertChi2DOF':  10.0,
                'MinPhPVVDChi2':    100.0,
                'MinPhIPChi2':       16.0,
                'MinPhDauPt':       500.0,
                'MinPhDauIPChi2':     9.0,
                'MaxPhDauTrkChi2':   10.0,
                'MaxKSDeltaM':      150.0,
                'MinKSPt':         2000.0,
                'MaxKSVertChi2DOF':  10.0,
                'MinKSPVVDChi2':    100.0,
                'MinKSIPChi2':       16.0,
                'MinKSDauPt':       500.0,
                'MinKSDauIPChi2':    16.0,
                'MaxKSDauTrkChi2':    5.0,
                'MaxJpDeltaM':     3000.0,
                'MinJpPt':         1000.0,
                'MaxJpVertChi2DOF':  10.0,
                'MinJpPVVDChi2':     16.0,
                'MinJpIPChi2':        0.0,
                'MinJpDauPt':       500.0,
                'MinJpDauIPChi2':     0.0,
                'MaxJpDauTrkChi2':   10.0,
                'MaxDSDeltaM':      153.0,
                'MinDSPt':         1500.0,
                'MaxDSVertChi2DOF':  10.0,
                'MinDSPVVDChi2':      0.0,
                'MinDSIPChi2':        0.0,
                'MinDSPiPt':        100.0,
                'MinDSPiIPChi2':      0.0,
                'MaxDSPiTrkChi2':     5.0,
                'MaxDSD0DeltaM':     40.0,
                'MinDSD0Pt':       1250.0,
                'MaxDSD0VertChi2DOF':10.0,
                'MinDSD0PVVDChi2':  225.0,
                'MinDSD0IPChi2':      0.0,
                'MinDSD0DauPt':     250.0,
                'MinDSD0DauIPChi2':   9.0,
                'MaxDSD0DauTrkChi2':  5.0
                }

B2threebody=confdict_2body.copy()
B2threebody['CONFIG']['nbody']=3
B2threebody['CONFIG']['doKS']=False
