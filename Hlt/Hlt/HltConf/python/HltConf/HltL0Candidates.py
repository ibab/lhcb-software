#
#==============================================================================
# $Id: HltL0Candidates.py,v 1.8 2009-01-16 07:56:33 graven Exp $
#==============================================================================
#
# Module to define the conversion of L0 candidates across several HltLines
# Using this module will insure that multiple inclusions will still only
# execute once, and it guarantees a uniform naming convention which is 
# defined in one single place
#
# The naming convention is as follows:
#
#  those L0{Muon,Calo}Candidates, which fired a specified L0 channel
#   (i.e. their Pt(Et) value in ADC counts exceed the threshold in the
#   configuration of the specified L0 Channel):
#
#    L0XXXDecision, where XXX is the L0 Channel
#
#  *all* L0Candidates of a specified type:
#
#    L0XXXCandidates, where XXX is the L0 type
#
#
from Gaudi.Configuration import *
from Configurables import HltL0CaloCandidates, HltL0MuonCandidates
from Configurables import HadronSeedTool, EcalSeedForVeloMatch
from Configurables import HltSelReportsMaker

def _cname(i) : return 'Hlt1L0'+i+'Decision'
def _tname(i) : return 'Hlt1L0'+i+'Candidates'

def _muon( channel, name ) :
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    return { channel : HltL0MuonCandidates(name, L0Channel = channel, OutputSelection = name) }

def _calo( channel, name ) :
    ### TODO: this should be triggerd by the 'type' and NOT by channel
    ###       maybe every L0CaloCandidates instance should be configured with 'all' makers
    _calomaker = {  'Hadron'      : HadronSeedTool( decodeCalos = False )
                 ,  'AllHadron'   : HadronSeedTool( decodeCalos = False )
                 ,  'Electron'    : EcalSeedForVeloMatch() 
                 ,  'AllElectron' : EcalSeedForVeloMatch() 
                 ## how about photon??
                 }
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    x = { channel: HltL0CaloCandidates(name, L0Channel = channel, OutputSelection = name) }
    if i in _calomaker :
        HltL0CaloCandidates(name).addTool(_calomaker[channel],name='CaloMakerTool')
        HltL0CaloCandidates(name).CaloMakerTool = _calomaker[channel]
    return x

## TODO: need to pick this up from a specified L0 config..
_l0MuonChannels = [ 'Muon','DiMuon','MuonNoGlob' ]
_l0CaloChannels = [ 'Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]

## these are basically hardwired and not likely to change...
_l0MuonTypes = [ 'Muon' ]
_l0CaloTypes = [ 'Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]


_dict = dict()
for i in _l0MuonChannels : _dict.update( _muon( i, _cname(i) ) )
for i in _l0MuonTypes :    _dict.update( _muon( 'All' + i, _tname( 'All' + i) ) )
for i in _l0CaloChannels : _dict.update( _calo( i, _cname(i) ) )
for i in _l0CaloTypes :    _dict.update( _calo( 'All' + i, _tname( 'All' + i) ) )

def L0Channels() :
    #TODO: strip 'fake' 'all' entries...
    return _l0MuonChannels + _l0CaloChannels

def convertL0Candidates(channel) :
    return _dict[channel]

def HltL0Candidates(channel) :
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates: %s'%channel)
    return _tname(channel) if channel.startswith('All') else _cname(channel)
