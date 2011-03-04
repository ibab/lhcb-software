"""
Line config dictionaries for Gamma from loops WG.

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""

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
                , 'NetCut'                :    0.9
                },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : ['GammaFromLoops']
    }


Hb2Charged2Body = {
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {
     'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 1,
     'MinPTB2Charged2Body'      : 1100,    'MinPTB2PPbar' : 900,
     'MinIPB2Charged2Body'      : 0.15,
     'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 9,
     'TrChi2'                   : 5,
     'PIDppi'                   : 0,
     'PIDpk'                    : -2,
     'MaxPTB2Charged2Body'      : 2700,   'MaxPTB2PPbar' : 2100,
     'MaxIPB2Charged2Body'      : 0.3,
     'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 25,
     'CombMassLow'              : 4800,
     'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
     'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16,
     'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
     'BIP'                      : 0.06,
     'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36,
     'BDIRA'                    : 0.9995,
     'BTAU'                     : 0.0009,
     'MassLow'                  : 4800,
     'MassHigh'                 : 5800
    },
    'STREAMS'     : [ 'Bhadron' ],
    'WGs'         : ['GammaFromLoops']
    }



Bu3hFrom2h = {
    'BUILDERTYPE' : 'Bu3hFrom2hBuilder',
    'WGs'         : ['GammaFromLoops'],
    'STREAMS'     : ['Bhadron'],
    'CONFIG' : {
    'MaxTrSIZE':  None,         ## GEC  maximim Rec/Track/Best TrSIZE
    '_h_PT': 700. ,             ## tracks min PT
    '_h_P': 5000. ,             ## tracks min P  
    '_h_IPCHI2': 16. ,          ## min tracks PI wrt OWNPV
    '_h_TRCHI2DOF': 3.0 ,       ## max tracks CHI2DOF
    '_2hLoose_DOCA': .12 ,      ## max two track DOCA
    '_2h_PTmax': 2000. ,        ## min PT of the 2h higests PT track 
    '_2h_PTsum':4000.  ,        ## min of 2h tracks PT sum 
    '_2h_PVIPCHI2sum': 100. ,   ## min of the 2h tracks IP wrt best 2h PV
    '_2h_FDCHI2': 150. ,        ## min 2h FDCHI2 wrt best 2h PV 
    '_2h_DIRA': 0.99 ,          ## min cos angle between 2h momentum and PV decayV  direction
    '_2h_CORRMmax': 6000. ,     ## max corrected mass for 2h candidate
    '_2h_CORRMmin': 4000. ,     ## min corrected mass for 2h candidate 
    '_2h_CharmVetoIPCHI2': 16. ,## charm veto: (Mhh>_2h_CharmVetoM||IP>_2h_CharmVetoIPCHI2)
    '_2h_CharmVetoM': 2500. ,   ##
    '_2h_TRKCHIDOFmin': 3. ,    ## max tracks CHI2DOF for the track with smalest CHI2DOF
    '_2h_Mmin': 3500. ,         ## min 2h mass
    '_2h_PVDOCAmin': 1.0 ,      ## min value of the 2h doca wrt any PV
    '_2h_CHI2': 14.0 ,          ## max 2h vertex CHI2
    '_2h_PTRANS': 4000. ,       ## 2h transverse mom wrt the PV decayV  direction
    '_3h_DOCA': .14 ,           ## max DOCA between h and 2h 
    '_3h_PTmax': 2000 ,         ## min PT of the 3h higests PT track
    '_3h_DIRA': .9998 ,         ## min cos angle between 3h momentum and PV decayV  direction   
    '_3h_FDCHI2': 150. ,        ## min 3h FDCHI2 wrt best 3h PV  
    '_3h_PVDOCAmin': 1.0 ,      ## min value of the 3h doca wrt any PV
    '_3h_CHI2': 14.0 ,          ## max 3h vertex CHI2 
    '_3h_IP': 0.1 ,             ## max 3h IP wrt best 3h PV
    '_3h_PT': 1000. ,           ## min 3h PT   
    '_3h_PTsum':4250.  ,        ## min of 3h tracks PT sum 
    '_3h_PVIPCHI2sum': 150. ,   ## min of the 3h tracks IP wrt best 3h PV
    '_3h_Charge': 1 ,           ## 3h tracks charge sum ==+-1
    '_3h_CORRMmax': 7000. ,     ## max corrected mass for 3h candidate  
    '_3h_CORRMmin': 4000. ,     ## min corrected mass for 3h candidate   
    '_3h_Mmax': 6000. ,         ## max 3h mass        
    '_3h_Mmin': 4000. ,         ## min 3h mass     
    '2hLinePrescale': 1. ,
    '2hLinePostscale': 1 ,
    '3hLinePrescale': 1 ,
    '3hLinePostscale': 1
    }
    }


B2HHPi0 = { 
    'BUILDERTYPE' : 'StrippingB2HHPi0Conf',
    'WGs'         : [ 'GammaFromLoops' ] ,
    'STREAMS'     : [ 'Bhadron' ],
    'CONFIG'      : {
    'PiMinPT'              : 500       # MeV                                                                                                 
    ,'PiMinP'               : 5000      # MeV                                                                                                
    ,'PiMinTrackProb'       : 0.000001  # unitless                                                                                           
    ,'PiMinIPChi2'          : 25        # unitless                                                                                           
    ,'Pi0MinPT_M'           : 2500      # MeV                                                                                                
    ,'Pi0MinPT_R'           : 1500      # MeV                                                                                                
    ,'ResPi0MinMM'          : 100       # MeV  # no effect if bellow 105                                                                     
    ,'ResPi0MaxMM'          : 170       # MeV  # no effect if above  165                                                                     
    ,'ResPi0MinGamCL'       : 0.2       # unitless                                                                                           
    ,'BMinM'                : 4200      # MeV                                                                                                
    ,'BMaxM'                : 6400      # MeV                                                                                                
    ,'BMinPT_M'             : 3000      # MeV                                                                                                
    ,'BMinPT_R'             : 2500      # MeV                                                                                                
    ,'BMinVtxProb'          : 0.001     # unitless                                                                                           
    ,'BMaxIPChi2'           : 9         # unitless                                                                                           
    ,'BMinDIRA'             : 0.99995   # unitless                                                                                           
    ,'BMinVVDChi2'          : 64        # unitless                                                                                           
    ,'MergedLinePrescale'   : 1.        # unitless                                                                                           
    ,'MergedLinePostscale'  : 1.        # unitless                                                                                           
    ,'ResolvedLinePrescale' : 1.        # unitless                                                                                           
    ,'ResolvedLinePostscale': 1.        # unitless                                                                                           
    }
    }




