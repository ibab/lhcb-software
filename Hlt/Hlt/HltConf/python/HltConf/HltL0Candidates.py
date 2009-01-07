from Gaudi.Configuration import *
from Configurables import HltL0CaloCandidates, HltL0MuonCandidates
from Configurables import HadronSeedTool, EcalSeedForVeloMatch
from Configurables import HltSelReportsMaker

def _name(i) : return 'HltL0'+i+'Candidates'

### TODO: this should be triggerd by the 'type' and NOT by channel
###       maybe every L0CaloCandidates instance should be configured with 'all' makers
_calomaker = {  'Hadron'      : HadronSeedTool( decodeCalos = False )
             ,  'AllHadron'   : HadronSeedTool( decodeCalos = False )
             ,  'Electron'    : EcalSeedForVeloMatch() 
             ,  'AllElectron' : EcalSeedForVeloMatch() 
             }


_dict = dict()
### Note: AllMuon triggers an exception which causes the conversion of 'everything'...
for i in [ 'Muon','DiMuon','MuonNoGlob' ] + [ 'AllMuon' ]:
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    _dict.update( { i: HltL0MuonCandidates(_name(i), L0Channel = i, OutputSelection = _name(i)) } )

### Note: AllXXXX triggers an exception which causes the conversion of all candidates of type XXXX
l0calochannels = [ 'Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
for i in l0calochannels + [ 'All'+j for j in l0calochannels ] :
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    _dict.update( { i: HltL0CaloCandidates(_name(i), L0Channel = i, OutputSelection = _name(i)) } )
    if i in _calomaker :
        HltL0CaloCandidates(_name(i)).addTool(_calomaker[i],name='CaloMakerTool')
        HltL0CaloCandidates(_name(i)).CaloMakerTool = _calomaker[i]


for i in _dict.keys() :
    HltSelReportsMaker().SelectionMaxCandidates.update( { _name(i):500 } )
    HltSelReportsMaker().SelectionMaxCandidatesDebug.update( { _name(i):500 } )

def L0Channels() :
    #TODO: strip 'fake' 'all' entries...
    return [ i for i in _dict.keys() if not i.startswith('All') ]

def convertL0Candidates(channel) :
    return _dict[channel]

def HltL0Candidates(channel) :
    if channel not in _dict.keys() : raise KeyError('Unknown L0 Channel: %s'%channel)
    return _name(channel)
