"""
Line config dictionaries for the Charm WG

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""


from GaudiKernel.SystemOfUnits import *

# MicroDST, Grant McGregor
# Test: Yes
DstarD02KMuNu = { 
    'BUILDERTYPE' : 'DstarD02KMuNuConf',
    'CONFIG'      : {
    'TRACK_Chi2' :  3., 
    'TRACK_MinIPChi2' :  7.,
    'KAON_PIDK' : 5.,
    'KAON_MinPT' : 600., 
    'KAON_MinP' : 5200.,
    'MUON_MinPT' :  630., 
    'PION_PIDmu' :  -1., 
    'PION_MaxP'  :  25000.,
    'PION_IPMin' : 20.,
    'D0_MassMax' : 2100.,
    'D0_MassMin' : 800., 
    'D0_MinPT'  : 1600.,
    'D0_DOCA' :  0.10,
    'D0_MinP' : 20000.,
    'D0_BPVVDZ' : 0.8,
    'D0_VtxChi2' : 9.,
    'D0_DIRA'  : 0.9996,
    'D0_MIP'  :  0.14,
    'Dstar_MassMin'  : 800.,
    'Dstar_DOCA'  : 0.4,
    'Dstar_VtxChi2' : 9.,
    'Dstar_DeltaMass'  :  200.,
    'PrescaledLinePrescale'  : 1.0,
    'PrescaledLinePostscale' : 1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


# Micro-DST, Alexandr Kozlinskiy
# Test: Yes
D02MuMu = {
    'BUILDERTYPE' : 'D02MuMuConf',
    'CONFIG'      : {
    'MinDauPT'                 : 1.0, # GeV
    'MinDauIPCHI2'             : 1.0,
    'MinD0LT'                  : 0.1, # ps
    'D02MuMuPrescale'          : 1.0,
    'DstD02MuMuPrescale'       : 1.0,
    'D02PiPiPrescale'          : 0.1,
    'DstD02PiPiPrescale'       : 0.1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

#Micro-DST, Vanya BELYAEV
#Test: Yes
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
# Test: Yes- Required fix

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
    'STREAMS' : [ 'CharmFull' ]
    }

#Micro-DST, Sam Gregson
# Test: 
D2KS0H = { 
    'BUILDERTYPE' : 'D2KS0HConf' ,
    'CONFIG'      : {
    'KS0DaugP' : 2000       # Cut in common particles
    ,'KS0DaugPT' : 300        # Cut in trigger
    ,'KS0DaugTrackChi2' : 4  
    ,'KS0DaugMIPChi2' : 100
    ,'KS0DaugPIDK' : 10
    ,'KS0MassWindow' :35   # Done by stdLoose KS0s
    ,'KS0MIPChi2'  : 9  # taken away   
    ,'KS0VertexChi2' : 10  
    ,'KS0PT'       : 900   
    ,'BachPionP' : 5000               
    ,'BachPionPT'    : 500     
    ,'BachPionTrackChi2': 3
    ,'BachPionMIPChi2'  : 30
    ,'BachPionPIDK'     : 0
    ,'BachKaonP' : 5000                 
    ,'BachKaonPT'   : 500     
    ,'BachKaonTrackChi2' : 3
    ,'BachKaonMIPChi2'  : 30 
    ,'BachKaonPIDK'      : 0
    ,'DMesonComboLowMass' : 1760          
    ,'DMesonComboHighMass'  : 2080
    ,'DMesonComboDOCA'      : 0.6
    ,'DMesonMotherLowMass' : 1770 
    ,'DMesonMotherHighMass'   : 2070
    ,'DMesonMotherVertexChi2' : 10
    ,'DMesonMotherMIPChi2'    : 20
    ,'DMesonMotherPT' : 2000
    ,'PionLinePrescale' : 1 
    ,'KaonLinePrescale' : 1 
    ,'PionLinePostscale' : 1 
    ,'KaonLinePostscale' : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

#Micro-DST, Diego Milanes
# Test: Yes - needed a fix

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


# Micro-DST, Erica Polycarpo
# Test: Yes

D2hhh = { 
    'BUILDERTYPE' : 'D2hhhConf' ,
    'CONFIG' : {
    'DaughterPT' : 250.,
    'DaughterP' : 2000.,
    'DaughterIPChi2' : 4.,
    'Daughter2IPChi2' : 10.,
    'D2KKKDaughterIPChi2' : 2.,
    'D2KKKDaughter2IPChi2' : 4.,
    'D2KKKDaughter1IPChi2' : 10.,
    'KPIDK' : 3.,
    'piPIDK' : 10.,
    'PTSum' : 2500.,
    'DOCAChi2' : 50.,
    'DPt' : 1000.,
    'DIPChi2' : 15.,
    'DdcaFDChi2' : 100.,
    'DDIRA' : 0.0,
    'DVXChi2NDOF' : 10.,
    'MinMassPosFit' : 1800.,
    'MaxMassPosFit' : 2040.,
    'D2KPPMaxMassPosFit' : 1940.,
    'D2HHHIncMinMassPosFit' : 1100.,
    'D2HHHIncMaxMassPosFit' : 2070.,
    'MaxTracksInEvent' : 500,
    'D2KKPLinePrescale' : 1.0,
    'D2KKPLinePostscale' : 1.0 ,
    'D2KPPLinePrescale' : 1.0,
    'D2KPPLinePostscale' : 1.0,
    'D2PPPLinePrescale' : 1.0,
    'D2PPPLinePostscale' : 1.0,
    'D2KPPosLinePrescale' : 0.4,
    'D2KPPosLinePostscale' : 1.0,
    'D2KKKLinePrescale' : 1.0,
    'D2KKKLinePostscale' : 1.0,
    'D2HHHIncLinePrescale' : 0.04,
    'D2HHHIncLinePostscale' : 1.0
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }


# Full DST, Malcolm John
# Test:Yes

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
    'STREAMS' : [ 'CharmFull' ]
    }


## Full DST, Walter Bonivento
## Test: Yes
DstarD02xx ={
    'BUILDERTYPE' : 'StrippingDstarD02xxConf',
    'CONFIG'      :  {
    'PrescalepipiBox'     : 0.5
    , 'PrescalemumuBox'   : 1.
    , 'PrescaleKpiBox'    : 0.3
    , 'PrescaleemuBox'    : 1.
    , 'PrescaleeKBox'     : 1.
    , 'PrescaleepiBox'    : 1.
    , 'PrescalepimuBox'   : 1.
    , 'PrescaleKmuBox'    : 1.
    , 'Prescalepipi_untagged_Box'   : 0.2
    , 'Prescalemumu_untagged_Box'   : 1.
    , 'PrescaleKpi_untagged_Box'    : 0.2
    , 'Prescalepimu_untagged_Box'   : 1.
    , 'PrescaleKmu_untagged_Box'    : 0.2  
    ,'DMassWin'           : 70.       # MeV
    ,'DMassWinMuMu'       : 300       #MeV
    ,'DMassWinEMu'        : 300
    ,'doca'               : 0.1        # mm
    ,'XminPT'             : 750.       # MeV
    ,'XmaxPT'             : 1100.      # MeV
    ,'XminP'              : 4000.      # MeV
    ,'XTrackChi2'         : 5.        # adimensional
    ,'XTrackChi2Pi'       : 7.        # adimensional
    ,'XminIPChi2'         : 3        # adimensional
    ,'XmaxIPChi2'         : 8        # adimensional
    ,'DMinFlightChi2'     :  20.
    ,'DDira'              : 0.9997     # adimensional
    ,'D0MinPT'            : 1800.      # MeV
    ,'DMaxIPChi2'         : 15.
    ,'DVChi2'             : 10. 
    ,'PiMinPT'            : 110.       # MeV
    ,'PiMaxIPCHI2'        : 10.         # adimensional
    ,'DstMassWin'         : 110.       # MeV
    ,'DstD0DMWin'         : 10.        # MeV
    ,'DstD0DMWinMuMu'      : 30.        # MeV  
    ,'RequireHlt'         : 1          # 
    ,'prefix'         : '' 
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmFull' ]
    }


# Philip Xing, Full DST
# Test: Yes,  Needed to update

DstarPromptWithD02HH = { 
    'BUILDERTYPE' : 'StrippingDstarPromptWithD02HHConf',
    'CONFIG'      : {
    'DaugPt'            : 0.9,        ## GeV
    'DaugP'             : 5.,         ## GeV
    'DaugTrkChi2'       : 5.,         ## unitless
    'D0Pt'              : 3.3,       ## GeV
    'D0MassWindowCentre': 1.865,     ## GeV
    'D0MassWindowWidth' : 0.075,     ## GeV 
    'D0VtxChi2Ndof'     : 13,         ## unitless
    'D0PVDisp'          : 0.9,        ## mm
    'SlowPiPt'          : 0.260,      ## GeV
    'DstarPt'           : 3.6,        ## GeV
    'DstarVtxChi2Ndof'  : 13,         ## unitless
    'RSLinePrescale'    : 1.,         ## unitless
    'RSLinePostscale'   : 1.,         ## unitless
    'WSLinePrescale'    : 1.,         ## unitless
    'WSLinePostscale'   : 1.,         ## unitless
    'HHLinePrescale'    : 1.,         ## unitless
    'HHLinePostscale'   : 1.,         ## unitless
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'CharmFull' ]
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
    

# Philip Hunt, Micro-DST
# Test: Yes

DstarPromptWithD02HHHHNoPID = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8.25*MeV
    ,'CombDelmUpper'    : 16.5*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 15*MeV
    ,'CombD0MassWin'    : 82.5*MeV
    ,'D0MassWin'        : 75*MeV
    ,'DstarDOCA'        : 0.45*mm
    ,'D0MaxDOCA'        : 0.5*mm
    ,'D0DauPt'          : 300*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 70*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 1.7
    ,'D0FDChi2'         : 48
    ,'D0IPChi2'         : 30
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 5
    ,'ApplyKaonPIDK'    : False 
    ,'KaonPIDK'         : -100
    ,'ApplyPionPIDK'    : False 
    ,'PionPIDK'         : 100 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 150
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'Prescale'         : 0.10
    ,'Postscale'        : 1
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }

DstarPromptWithD02HHHH = {
    'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
    'CONFIG'      : {
    'CombDelmLower'     : -8.25*MeV
    ,'CombDelmUpper'    : 16.5*MeV
    ,'DelmLower'        : -7.5*MeV
    ,'DelmUpper'        : 15*MeV
    ,'CombD0MassWin'    : 82.5*MeV
    ,'D0MassWin'        : 75*MeV
    ,'DstarDOCA'        : 0.45*mm
    ,'D0MaxDOCA'        : 0.5*mm
    ,'D0DauPt'          : 300*MeV
    ,'D0DauP'           : 3*GeV
    ,'D0Pt'             : 3*GeV
    ,'DstarPt'          : 3*GeV
    ,'SlowPionPt'       : 70*MeV
    ,'DstarVtxChi2DOF'  : 20
    ,'D0VtxChi2DOF'     : 10
    ,'D0DauMaxIPChi2'   : 30
    ,'D0DauIPChi2'      : 1.7
    ,'D0FDChi2'         : 48
    ,'D0IPChi2'         : 30
    ,'D0DIRA'           : 0.9998
    ,'TrackChi2DOF'     : 5
    ,'ApplyKaonPIDK'    : True
    ,'KaonPIDK'         : 0
    ,'ApplyPionPIDK'    : True
    ,'PionPIDK'         : 3 # not applied to the slow pion
    ,'CheckPV'          : True # reject events with no PVs
    ,'ApplyGECs'        : True # set to False to apply no GECs
    ,'MaxLongTracks'    : 150
    ,'MaxVeloTracks'    : None
    ,'MaxSpdDigits'     : None
    ,'MaxITClusters'    : None
    ,'Prescale'         : 1
    ,'Postscale'        : 1
    },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }
   

# Mat Charles, Full DST
# Test: Yes

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
    'STREAMS' : [ 'CharmFull' ]
    }


