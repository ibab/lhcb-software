"""
Line config dictionaries for Charm WG

Stripping14 definitions: 2010 re-stripping
Dictionaries have the name of the line builder instance.
"""

# Still to add: DstarD02KK*Regular lines with updated dictionary


CharmedAndCharmedStrangeSpectroscopy = {
    'BUILDERTYPE' : 'CharmedAndCharmedStrangeSpectroscopyConf' ,
    'CONFIG'      : {
    'DpKs_prescale'      : 1,
    'DstarpKs_prescale'  : 1,
    'D0K_prescale'       : 1,
    'DpPim_prescale'     : 1,
    'D0Pip_prescale'     : 1,
    'DstarpPim_prescale' : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

#Micro-DST, Vanya BELYAEV
#Test: Yes. Only the lambdac line is needed.
PromptCharm = {
    'BUILDERTYPE' : 'StrippingPromptCharmConf', 
    'CONFIG'      : {
    'D0Prescale'      : 1.00,
    'DstarPrescale'   : 1.00,
    'DsPrescale'      : 1.00,
    'DplusPrescale'   : 1.00,
    'LamCPrescale'    : 1.00
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

# Full, Marco Gersabeck
D2hh = { 
    'BUILDERTYPE' : 'D2hhConf',
    'CONFIG'      : {
    'DaugPtMin': 800.,
    'DaugPtMax': 1500.,
    'DaugP': 5000.,
    'DaugIPChi2': 9.,
    'DaugTrkChi2': 3.,
    'HighPIDK': 5.,
    'LowPIDK': 0.,
    'D0Pt': 2000.,
    'D0MassWindowCentre': 1865.,
    'D0MassWindowWidth': 100.,
    'D0P': 5000.,
    'D0VtxChi2Ndof': 10.,
    'D0FDChi2': 40.,
    'D0BPVDira': 0.9999,
    'D0DOCA': 0.07,
    'Daug_TRCHI2DOF_MAX': 5.,
    'Dstar_AMDiff_MAX': 165.,
    'Dstar_VCHI2VDOF_MAX': 100.,
    'Dstar_MDiff_MAX': 160.,
    'UntaggedCFLinePrescale': 1.,
    'UntaggedCFLinePostscale': 1.,
    'UntaggedSCSLinePrescale': 1.,
    'UntaggedSCSLinePostscale': 1.,
    'TaggedRSLinePrescale': 1.,
    'TaggedRSLinePostscale': 1.,
    'TaggedWSLinePrescale': 1.,
    'TaggedWSLinePostscale': 1.,
    'TaggedSCSLinePrescale': 1.,
    'TaggedSCSLinePostscale': 1.,
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }

# Malcolm John, Charm RD
D2XMuMu = {
    'BUILDERTYPE' : 'D2XMuMuConf',
    'CONFIG' : {
    'MuonP'         : 3000. ,    #MeV
    'MuonPT'        : 500.  ,    #MeV
    'MuonMINIPCHI2' : 4     ,    #adminensional
    'MuonTRCHI2'    : 8     ,    #adminensional
    'PionP'         : 2000. ,    #MeV
    'PionPT'        : 300.  ,    #MeV
    'PionMINIPCHI2' : 6     ,    #adminensional
    'PionTRCHI2'    : 8     ,    #adminensional
    'KaonP'         : 2000. ,    #MeV
    'KaonPT'        : 300.  ,    #MeV
    'KaonPIDK'      : -1.   ,    #adimensional
    'KaonMINIPCHI2' : 6     ,    #adminensional
    'KaonTRCHI2'    : 8     ,    #adminensional
    'DimuonMass'    : 250.  ,    #MeV
    'DVCHI2DOF'     : 5    ,    #adminensional                              
    'DDIRA'         : 0.9999,    #adimensional
    'DIPCHI2'       : 30    ,    #adimensional
    'DMassWin'      : 200.  ,    #MeV, mass window
    'DMassLow'      :1763.  ,    #MeV, low-mass veto
    'D2PiMuMuOSLinePrescale'  : 1 ,
    'D2PiMuMuOSLinePostscale' : 1 ,
    'D2PiMuMuSSLinePrescale'  : 1 ,
    'D2PiMuMuSSLinePostscale' : 1 ,
    'D2KMuMuOSLinePrescale'   : 1 ,
    'D2KMuMuOSLinePostscale'  : 1 ,
    'D2KMuMuSSLinePrescale'   : 1 ,
    'D2KMuMuSSLinePostscale'  : 1 ,
    'D2PiPiPiCalLinePrescale' : 0.01 ,
    'D2PiPiPiCalLinePostscale': 1 
    } ,
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


# Mat Charles, Micro-DST
# Test: Yes. "name" already decalred at top of file. Renamed to default_name to avoid clash

Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
    'LongTrackGEC' : 150
    ,'controlPrescaleLc' : 0.05
    ,'controlPrescaleXic' : 1.0
    ,'signalPrescaleViaLc' : 1.0
    ,'signalPrescaleViaXic' : 1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ] 
    }


# Benoit Vaud, Full ? Micro ? 
# Test : Yes 

DstarD02KKmumu = {
    'BUILDERTYPE' : 'StrippingDstarD02KKmumuRegularConf',
    'CONFIG'      : {
    'TrackChi2max' : 5.
    ,'DDauKPTmin'          : 200.
    , 'DDauKPmin'     : 1000.
    , 'DDauMuPTmin'         : 200.
    , 'DDauMuPmin'        : 1000.
    , 'DDauKDelPIDmin'        : -5.
    , 'DDauMuDelPIDmin'            : -10.
    , 'DCombMassWind'             : 60.
    , 'DCombAMAXDOCAmax'            : 0.4
    , 'DMothPTmin'            : 1500.
    , 'DMothBPVDIRAmin'             : 0.999
    , 'DMothMIPDVmax'          :  0.2
    , 'DMothMIPCHI2DVmax'  :  36.
    , 'DMothBPVVDmin'           : 1.0
    , 'DMothBPVDCHI2min'           : 4.
    , 'DMothBPVVDZmin'              : 0.
    , 'DMothVFASPFmax'               :  12.
    , 'DstarCombMassWind'               : 60.
    , 'DstarCombAMAXDOCAmax'                 :  0.4
    , 'DstarMothVFASPFmax'            :  12.
    , 'DstarMothPTmin'             : 1500.
    , 'DstarMothDMmin'               : 138.
    , 'DstarMothDMmax'           : 156.
    , 'DstarMothSlpiMIPDVmax'           : 0.400
    , 'DstarMothSlpiMIPCHI2DVmax'   : 36.
    , 'DstarMothSlpiPTmin'       : 250.
    , 'NTracksLim'               : 150
    , 'LinePrescale'   : 1.
    , 'LinePostscale'  : 1.
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
    

# Benoit Vaud, Full ? Micro ? 
# Test: Yes

DstarD02KKpipi = {
    'BUILDERTYPE' : 'StrippingDstarD02KKpipiRegularConf',
    'CONFIG'      : {
    'TrackChi2max' : 5.
    ,'DDauKPTmin'          : 500.
    , 'DDauKPmin'     : 3000.
    , 'DDauPiPTmin'         : 400.
    , 'DDauPiPmin'        : 3000.
    , 'DDauKDelPIDmin'        : -4.
    , 'DDauPiDelPIDmin'            : -5.
    , 'DCombMassWind'             : 60.
    , 'DCombAMAXDOCAmax'            : 0.25
    , 'DMothPTmin'            : 2500.
    , 'DMothBPVDIRAmin'             : 0.9996
    , 'DMothMIPDVmax'          :  0.15
    , 'DMothMIPCHI2DVmax'  :  25.
    , 'DMothBPVVDmin'           : 1.0
    , 'DMothBPVDCHI2min'           : 16.0
    , 'DMothBPVVDZmin'              : 0.
    , 'DMothVFASPFmax'               :  8.
    , 'DstarCombMassWind'               : 60.
    , 'DstarCombAMAXDOCAmax'                 :  0.250
    , 'DstarMothVFASPFmax'            :  8.
    , 'DstarMothPTmin'             : 2500.
    , 'DstarMothDMmin'               : 138.
    , 'DstarMothDMmax'           : 156.
    , 'DstarMothSlpiMIPDVmax'           : 0.250
    , 'DstarMothSlpiMIPCHI2DVmax'   : 25.
    , 'DstarMothSlpiPTmin'       : 250.
    , 'NTracksLim'               : 150
    , 'LinePrescale'   : 1.
    , 'LinePostscale'  : 1.
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }


DstarD2KShh = {
    'BUILDERTYPE' : 'DstarD2KShhBuilder',
    'CONFIG'      : {
    'LongTrackGEC'          : 150      ## Global event cut on number of long tracks
    ,'KSLLCutDIRA'          : 0.9997   ## unitless
    ,'KSDDCutDIRA'          : 0.99995  ## unitless
    ,'KSLLCutMass'          : 11.4     ## in MeV
    ,'KSDDCutMass'          : 24.9     ## in MeV
    ,'KSLLCutFDChi2'        : 100      ## unitless
    ,'KSDDCutFDChi2'        : 100      ## unitless
    ,'trackFromDCutP'       : 1500.0   ## in MeV
    ,'trackFromDCutPIDe'    : 10.0     ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
    ,'trackFromDCutPIDp'    : 15.0     ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
    ,'pionFromDCutPIDK'     : -5.0     ## unitless -- note that this means (DLLpi - DLLK) < -5, i.e. (DLLK-DLLpi) > +5
    ,'kaonFromDCutPIDpi'    : 0.0      ## unitless -- note that this means (DLLK - DLLpi) < 0
    ,'pionFromDCutTRCHI2DOF': 4.0      ## unitless
    ,'kaonFromDCutTRCHI2DOF': 5.0      ## unitless
    ,'DCutDIRA'             : 0.0      ## unitless -- remove "backwards-going" D0
    ,'DCutTau'              : 0.3      ## ps
    ,'DCutVtxChi2_KK'       : 21.0     ## unitless
    ,'DCutVtxChi2_KP'       : 15.0     ## unitless
    ,'DCutVtxChi2_PP'       : 13.0     ## unitless
    ,'preFitDCutPT'         : 1500.0   ## in MeV
    ,'preFitDMassCut_LL'    : 130      ## in MeV
    ,'preFitDMassCut_DD'    : 270      ## in MeV
    ,'wideDMassCut_KKLL'    : 57.2     ## in MeV
    ,'wideDMassCut_KPLL'    : 92.0     ## in MeV
    ,'wideDMassCut_PPLL'    : 110.0    ## in MeV
    ,'wideDMassCut_KKDD'    : 72.0     ## in MeV
    ,'wideDMassCut_KPDD'    : 120.0    ## in MeV
    ,'wideDMassCut_PPDD'    : 140.0    ## in MeV
    ,'preFitDstarMassCut'   : 200.0    ## in MeV; make sure it's well above the largest D mass window
    ,'wideDMCutLower'       : 0.0      ## in MeV
    ,'wideDMCutUpper'       : 15.0     ## in MeV
    ,'DstarCutPT_KK'        : 2500.0   ## in MeV
    ,'DstarCutPT_KP'        : 3000.0   ## in MeV
    ,'DstarCutPT_PP'        : 3000.0   ## in MeV
    ,'DstarCutChi2NDOF_KK'  : 60.0     ## unitless
    ,'DstarCutChi2NDOF_KP'  : 20.0     ## unitless
    ,'DstarCutChi2NDOF_PP'  : 20.0     ## unitless
    ,'SoftPionCutPIDe'      : 5.0      ## unitless
    ,'KKLLPrescale'         : 1.0
    ,'KKLLPostscale'        : 1.0
    ,'KPLLPrescale'         : 1.0
    ,'KPLLPostscale'        : 1.0
    ,'PPLLPrescale'         : 1.0
    ,'PPLLPostscale'        : 1.0
    ,'KKDDPrescale'         : 1.0
    ,'KKDDPostscale'        : 1.0
    ,'KPDDPrescale'         : 1.0
    ,'KPDDPostscale'        : 1.0
    ,'PPDDPrescale'         : 1.0
    ,'PPDDPostscale'        : 1.0
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'CharmCompleteEvent' ]
    }


