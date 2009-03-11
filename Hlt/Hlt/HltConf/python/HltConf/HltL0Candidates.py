#
#==============================================================================
# $Id: HltL0Candidates.py,v 1.16 2009-03-11 17:31:04 depaula Exp $
#==============================================================================
#
# Module to define the conversion of L0 candidates across several HltLines
# Using this module will insure that multiple inclusions will still only
# execute once, and it guarantees a uniform naming convention which is 
# defined in one single place
#
#
# The naming convention is as follows:
#
#  those L0{Muon,Calo}Candidates, which fired a specified L0 channel
#   (i.e. their Pt(Et) value in ADC counts exceed the threshold in the
#   configuration of the specified L0 Channel):
#
#    Hlt1L0XXXDecision, where XXX is the L0 Channel
#
#  *all* L0Candidates of a specified type:
#
#    Hlt1L0AllXXXCandidates, where XXX is the L0 type
#
# this convention is implemented by the _name function below.
#
# In addition, if an L0 TCK is explicity specified, this module checks 
# that the requested L0 Channels actually exist in the corresponding
# L0 configuration
#
#
# WARNING: this module makes two assumptions about L0 configurations:
#
#       i) they do not include the word 'All' as part of a channel name
#      ii) L0 muon candidates only appear in channels with the word 'Muon' in it,
#          all other channels are assume to use L0 calo candidates (or no candidates)
#

import re
from Gaudi.Configuration import *
from Configurables import HltL0CaloCandidates, HltL0MuonCandidates
from Configurables import HadronSeedTool, ElectronSeedTool
from Configurables import L0DUMultiConfigProvider
from GaudiKernel.Configurable import ConfigurableGeneric

# utilities to pack and unpack L0 conditions into Condition property...
# given the 'Conditions' property of an L0DUCOnfig tool instance, 
# return a pythonized table of settings
# i.e. the inverse of 'genItems'
def _parseL0settings( settings ) :
    return [ _parseL0setting(i) for i in settings ]
def _parseL0setting( setting ) :
    p = re.compile('(.*)= *\[(.*)\]')
    val = {}
    for s in setting :
        m = p.match(s)
        val[m.group(1)]=m.group(2)
    return val

def _name(i) : 
    return 'Hlt1L0'+i+'Candidates' if i.startswith('All') else 'Hlt1L0'+i+'Decision'

def _muon( channel ) :
    name = _name(channel)
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    return { channel : HltL0MuonCandidates(name, L0Channel = channel, OutputSelection = name) }

def _calo( channel ) :
    name = _name(channel)
    ### TODO: this should be triggerd by the 'type' and NOT by channel
    ###       maybe every L0CaloCandidates instance should be configured with 'all' makers
    _calomaker = {  'Hadron'      : HadronSeedTool
                 ,  'Electron'    : ElectronSeedTool
                 ,  'Photon'      : ElectronSeedTool
                 ,  'LocalPi0'    : ElectronSeedTool
                 }
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    x = { channel: HltL0CaloCandidates(name, L0Channel = channel, OutputSelection = name) }
    c = channel.lstrip('All')
    if c in _calomaker :
        HltL0CaloCandidates(name).addTool(_calomaker[c],name='CaloMakerTool')
        HltL0CaloCandidates(name).CaloMakerTool = _calomaker[c].__name__
    return x

def _converter( channel ) :
    _conv = _muon if channel.find('Muon') != -1 else _calo
    return _conv(channel)

global _dict,_l0Channels
_dict       = None
_l0Channels = None

########################################## public visible part of this module...
### setupL0Channels _must_ be called before L0Channels, convertL0Candidates or HltL0Candidates is
### invoked...
def decodeL0Channels( L0TCK , skipDisabled = True) :
    # we assume somebody has done an 'importOptions' of the relevant L0 setup...
    channels = _parseL0settings( ConfigurableGeneric('ToolSvc.L0DUConfig.TCK_'+L0TCK).Channels )
    return [ i['name'] for i in channels if ( not skipDisabled or 'DISABLE' not in i or i['DISABLE'].upper().find('TRUE') == -1 ) ]

def setupL0Channels( channels ) :
    global _l0Channels,_dict
    if _dict or _l0Channels  : raise RunTimeError('HltL0Candidates already initialized -- 2nd call to setupL0Channels...')
    _l0Channels = channels
    if not _l0Channels:
        _l0Channels = [ 'MuonNoGlob','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]

    print '# recognized L0 channels: ' + str(_l0Channels)
    # the types are basically hardwired and are thus not likely to change...
    _l0Types = [ 'Muon','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
    _dict = dict()
    for i in _l0Channels + [ 'All' + j for j in _l0Types ] : _dict.update( _converter( i ) )

def L0Channels() :
    global _l0Channels
    if not _l0Channels : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    return _l0Channels

def convertL0Candidates(channel) :
    global _dict
    if not _dict : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    return _dict[channel]

def HltL0Candidates(channel) :
    global _dict
    if not _dict : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _name(channel)
