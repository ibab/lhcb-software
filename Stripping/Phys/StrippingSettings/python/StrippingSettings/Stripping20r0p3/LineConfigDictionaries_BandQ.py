# Line config dictionaries for BandQ WG
# 


from GaudiKernel.SystemOfUnits import *

CC2DD = {        
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'CC2DDConf',
    'CONFIG'   :  {'D0MassWin'               : "60*MeV",
                   'D0PT'                    : "0*MeV",
                   'D0VtxChi2Ndof'           : 10,
                   'D0Bpvdira'               : -10.,
                   'D0Bpvdls'                : 4.,
                   'D0daughterBpvIpChi2'     : 3.,
                   'D0daughterPT'            : "500*MeV",
                   'D0daughterP'             : "5*GeV",
                   'D0daughterTrkChi2'       : 3,
                   'D0daughterTrkGhostProb'  : 0.3,
                   'D0daughterKaonPIDK'      : 5,
                   'D0daughterPionPIDK'      : 10,
############################
                   'DpmMassWin'              : "60*MeV",
                   'DpmPT'                   : "0*MeV",
                   'DpmVtxChi2Ndof'          : 10,
                   'DpmBpvdira'              : -10.,
                   'DpmBpvdls'               : 4.,
                   'DpmdaughterBpvIpChi2'    : 3.,
                   'DpmdaughterPT'           : "500*MeV",
                   'DpmdaughterP'            : "5*GeV",
                   'DpmdaughterTrkChi2'      : 3,
                   'DpmdaughterTrkGhostProb' : 0.3,
                   'DpmdaughterKaonPIDK'     : 5,
                   'DpmdaughterPionPIDK'     : 10,
# no mass constraint
                   'CCMassCut'               : "(AM>0)",
                   'CCVtxChi2Ndof'           : 10,
                   },
    'STREAMS' : [
        'CharmCompleteEvent' 
        ],
}   
