from Gaudi.Configuration import *
from Configurables import HltL0CaloCandidates, HltL0MuonCandidates
from Configurables import HadronSeedTool, EcalSeedForVeloMatch
from Configurables import HltSelReportsMaker

def _name(i) : return 'HltL0'+i+'Candidates'

_calomaker = {  'Hadron'   : HadronSeedTool( decodeCalos = False )
             ,  'Electron' : EcalSeedForVeloMatch() }


_dict = dict()
for i in [ 'Muon','DiMuon','MuonNoGlob' ] :
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    _dict.update( { i: HltL0MuonCandidates(_name(i), L0Channel = i, OutputSelection = _name(i)) } )

### a special instance which just convertes 'everything'...
_dict.update( { 'AllMuon': HltL0MuonCandidates(_name('AllMuon'), L0Channel = 'Ignore', OutputSelection = _name('AllMuon')) } )

for i in [ 'Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ] :
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
