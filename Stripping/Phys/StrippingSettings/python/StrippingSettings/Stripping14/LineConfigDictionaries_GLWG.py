"""
Line config dictionaries for Gamma from loops WG.

Stripping13 definitions.
Dictionaries have the name of the line builder instance.
"""

B2KShh = {
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG' : {  'Trk_Chi2'         : 4.0,
                  'KS_DD_MassWindow' : 30.0,
                  'KS_DD_VtxChi2'    : 12.0,
                  'KS_DD_FDChi2'     : 50.0,
                  'KS_DD_Pmin'       : 6000.0,
                  'KS_LL_MassWindow' : 30.0,
                  'KS_LL_VtxChi2'    : 12.0,
                  'KS_LL_FDChi2'     : 50.0,
                  'B_Mlow'           : 200.0,
                  'B_Mhigh'          : 280.0,
                  'BDaug_MedPT_PT'   : 800.0,
                  'BDaug_MaxPT_IP'   : 0.05,
                  'hh_DOCA'          : 0.3,
                  'BDaug_DD_PTsum'   : 4300.0,
                  'BDaug_LL_PTsum'   : 4000.0,
                  'B_VtxChi2'        : 12.0,
                  'B_Dira'           : 0.999,
                  'B_IPwrtPV'        : 0.08,
                  'B_FDwrtPV'        : 1.0,
                  'B_DD_FDChi2'      : 30.0,
                  'B_LL_FDChi2'      : 40.0,
                  'GEC_MaxTracks'    : 250,
                  'Prescale'         : 1.0,
                  'Postscale'        : 1.0
                  },
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : ['GammaFromLoops']
    }





