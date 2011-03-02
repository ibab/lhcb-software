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
    'STREAM' : 'Bhadron',
    'WGs'    : ['GammaFromLoops']
    }

