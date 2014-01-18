"""
Line config dictionaries for the Charm WG
"""

from GaudiKernel.SystemOfUnits import *
#Denis Derkach
#StrippingD2KShh_samesign
#Added in Stripping20rXp3
D2KShh_samesign = {
  'BUILDERTYPE' : 'D2KShhBuilder' ,
    'CONFIG'      : {
        'LongTrackGEC'          :  150        ## Global event cut on number of long tracks
        ,'KSLLCutDIRA'          :    0.9997   ## unitless
        ,'KSDDCutDIRA'          :    0.99995  ## unitless
        ,'KSLLCutMass'          :   20.0      ## in MeV -- Mat changed from 11.4 to 20.0 in S20
        ,'KSDDCutMass'          :   40.0      ## in MeV -- Mat changed from 24.9 to 40.0 in S20
        ,'KSLLCutFDChi2'        :  100        ## unitless
        ,'KSDDCutFDChi2'        :  100        ## unitless
        ,'KSCutZFDFromD'        :   10.0      ## in mm -- Mat added in S20
        ,'trackFromDCutP'       : 1500.0      ## in MeV
        ,'trackFromDCutPIDe'    :   10.0      ## unitless -- note that this means (DLLe - DLLx) < 10 for x=K,pi
        ,'trackFromDCutPIDp'    :   15.0      ## unitless -- note that this means (DLLp - DLLx) < 15 for x=K,pi
        ,'pionFromDCutPIDK'     :   -1.0
        ,'kaonFromDCutPIDpi'    :   -3.0
        ,'pionFromDCutTRCHI2DOF':    4.0      ## unitless
        ,'kaonFromDCutTRCHI2DOF':    5.0      ## unitless
        ,'DCutVtxChi2_KK'       :   21.0      ## unitless
        ,'DCutVtxChi2_PP'       :   13.0      ## unitless
        ,'preFitDCutPT'         : 1500.0      ## in MeV
        ,'preFitDMassCut_LL'    :  130        ## in MeV
        ,'preFitDMassCut_DD'    :  270        ## in MeV
        ,'wideDMassCut_KKLL'    :   70.0      ## in MeV -- Mat changed from  57.2 to  70.0 in S20
        ,'wideDMassCut_PPLL'    :  120.0      ## in MeV -- Mat changed from 110.0 to 120.0 in S20
        ,'wideDMassCut_KKDD'    :   85.0      ## in MeV -- Mat changed from  72.0 to  85.0 in S20
        ,'wideDMassCut_PPDD'    :  150.0      ## in MeV -- Mat changed from 140.0 to 150.0 in S20
        ,'preFitDstarMassCut'   :  200.0      ## in MeV; make sure it's well above the largest D mass window
        ,'wideDMCutLower'       :  -10.0      ## in MeV: allow negative tail made by CombineParticles -- Mat changed from -2.0 to -10.0 in S20
        ,'wideDMCutUpper'       :   15.0      ## in MeV -- Mat changed from 15.0 to 25.0 in S20
        ,'DstarCutPT_KK'        : 2200.0      ## in MeV
        ,'DstarCutPT_PP'        : 2200.0      ## in MeV
        ,'DstarCutChi2NDOF_KK'  :   60.0      ## unitless
        ,'DstarCutChi2NDOF_PP'  :   20.0      ## unitless
        ,'SoftPionCutPIDe'      :    2.0      ## unitless -- Mat changed from 5.0 to 2.0 in S20
        ,'KKLLPrescale'         :    0.01
        ,'KKLLPostscale'        :    1.00
        ,'PPLLPrescale'         :    0.01
        ,'PPLLPostscale'        :    1.0
        ,'KKDDPrescale'         :    0.01
        ,'KKDDPostscale'        :    1.0
        ,'PPDDPrescale'         :    0.01
        ,'PPDDPostscale'        :    1.0
      },
    'WGs' : ['Charm'],
    'STREAMS' : [ 'Charm' ]
    }

# Re-run a subset of the Xicc lines which include a hyperon
# in the final state to take proper advantage of triggers
# introduced in 2012.
#
# Responsible: Mat Charles
# Output to micro-DST

Xicc = {
    'BUILDERTYPE' : 'XiccBuilder',
    'CONFIG'      : {
    'LongTrackGEC'            : 150
    ,'controlPrescaleLc'       : 0.0
    ,'controlPrescaleXic'      : 0.1
    , 'controlPrescaleDp'      : 0.0
    , 'controlPrescaleD0'      : 0.0
    , 'controlPrescaleDsp'     : 0.0
    , 'signalPrescaleViaLc'    : 0.0
    , 'signalPrescaleViaXic'   : 1.0
    , 'signalPrescaleViaDp'    : 0.0
    , 'signalPrescaleViaD0'    : 0.0
    , 'signalPrescaleViaLcWC'  : 0.0
    , 'signalPrescaleViaLcDCS' : 0.0
    , 'signalPrescaleViaXicWC' : 1.0
    , 'signalPrescaleViaDpWC'  : 0.0
    , 'signalPrescaleViaD0WC'  : 0.0
    , 'signalPrescaleViaD0DCS' : 0.0
    , 'Hlt2TisTosSpec' : { 'Hlt2.*Decision%TOS' : 0 }
    },
    'WGs' : [ 'Charm' ],
    'STREAMS' : [ 'Charm' ]
    }
