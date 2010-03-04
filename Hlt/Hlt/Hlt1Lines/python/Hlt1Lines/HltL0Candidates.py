#
#==============================================================================
# $Id: HltL0Candidates.py,v 1.16 2010-03-04 14:04:36 graven Exp $
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
from HltLine.HltLine import bindMembers


### TODO: for a given channel name, pick up the conditions; 
### next, pick up the conditions, and see whether the corresponding 'data' is in _dataWithoutCandidates
### and if only those, skip making candidates (as there are no candidates for those data...

_dataCandidates = { 'Spd(Mult)'    : False 
                  , 'PUHits(Mult)' : False
                  , 'Sum(Et)'      : False
                  , 'Electron(Et)' : True
                  , 'Photon(Et)'   : True
                  , 'Hadron(Et)'   : True
                  , 'Muon1(Pt)'    : True
                  , 'Muon2(Pt)'    : True
                  , 'DiMuon(Pt)'   : True
                  , 'LocalPi0(Et)' : True
                  , 'GlobalPi0(Et)': True
                  }

# utilities to pack and unpack L0 conditions into Condition property...
# given the 'Conditions' property of an L0DUCOnfig tool instance, 
# return a pythonized table of settings
# i.e. the inverse of 'genItems'
def _parseL0settings( settings ) :
    return  [ _parseL0setting(i) for i in settings ]
def _parseL0setting( setting ) :
    p = re.compile('([^ ]+) *= *\[(.+)\]')
    val = {}
    for s in setting :
        m = p.match(s)
        val[m.group(1)]=m.group(2)
    return val

def _name(i) : 
    return 'Hlt1L0'+i+'Candidates' if i.startswith('All') else 'Hlt1L0'+i+'Decision'

def _muon( channel ) :
    from HltLine.HltDecodeRaw import DecodeL0MUON
    from Configurables import HltL0MuonCandidates,L0MuonCandidatesFromRaw
    name = _name(channel)
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    # return { channel : HltL0MuonCandidates(name, L0Channel = channel, OutputSelection = name) }
    return { channel : bindMembers(None, [ DecodeL0MUON,HltL0MuonCandidates(name, L0Channel = channel, OutputSelection = name)]) }

def _calo( channel ) :
    from HltLine.HltDecodeRaw import DecodeL0CALO
    from Configurables import HltL0CaloCandidates
    from Configurables import HadronSeedTool, ElectronSeedTool
    name = _name(channel)
    ### TODO: this should be triggerd by the 'type' and NOT by channel
    ###       maybe every L0CaloCandidates instance should be configured with 'all' makers
    _calomaker = {  'Hadron'      : HadronSeedTool
                 ,  'Electron'    : ElectronSeedTool
                 ,  'Photon'      : ElectronSeedTool
                 ,  'LocalPi0'    : ElectronSeedTool
                 }
    #note: explicitly set the OutputSelection so we can pick it up downstream...
    # x = { channel:                               HltL0CaloCandidates(name, L0Channel = channel, OutputSelection = name) }
    x = { channel: bindMembers(None,[ DecodeL0CALO,HltL0CaloCandidates(name, L0Channel = channel, OutputSelection = name) ]) }
    c = channel.lstrip('All')
    if c in _calomaker :
        HltL0CaloCandidates(name).addTool(_calomaker[c],name='CaloMakerTool')
        HltL0CaloCandidates(name).CaloMakerTool = _calomaker[c].__name__
    return x

def _converter( channel ) :
    # TODO: check the conditions of the channel, and dispatch based on that
    #       so that we become agnostic to the actual names...
    if channel.upper().find('MUON') != -1 : return _muon(channel)
    if channel in [ 'SPD','PU','SPD40','PU20','PU40','B1gas','B2gas' ] : return { channel : None }
    return _calo(channel)

def _parseMask( mask ) :
    mask = mask.upper()
    if mask in [ '0','0X0','000'           ] : return 'Disable'
    if mask in [ '1','0X1','001','PHYSICS' ] : return 'Physics'
    if mask in [ '2','0X2','010','BEAM1'   ] : return 'Beam1'
    if mask in [ '3','0X3','011'           ] : return 'Physics+Beam1'
    if mask in [ '4','0X4','100','BEAM2'   ] : return 'Beam2'
    if mask in [ '5','0X5','101'           ] : return 'Physics+Beam2'
    if mask in [ '6','0X6','110'           ] : return 'Beam1+Beam2'
    if mask in [ '7','0X7','111'           ] : return 'Physics+Beam1+Beam2'
    return None


def _updateL0Config( L0TCK, singleL0Configuration ) :
    from Configurables import L0DUConfigProvider
    orig =  L0DUConfigProvider('ToolSvc.L0DUConfig.TCK_'+L0TCK)
    if not singleL0Configuration : return orig
    del allConfigurables['ToolSvc.L0DUConfig']
    single = L0DUConfigProvider('ToolSvc.L0DUConfig')
    for p,v in orig.getValuedProperties().items() : setattr(single,p,v)
    single.TCK = L0TCK
    from Configurables import L0DUFromRawTool, L0DUFromRawAlg
    l0du   = L0DUFromRawAlg("L0DUFromRaw")
    l0du.addTool(L0DUFromRawTool,name = "L0DUFromRawTool")
    getattr( l0du, 'L0DUFromRawTool' ).L0DUConfigProviderType = 'L0DUConfigProvider'
    from Configurables import L0DUAlg
    L0DUAlg('L0DU').L0DUConfigProviderType = 'L0DUConfigProvider'
    return single


global _dict,_l0Channels, _l0Masks
_dict       = None
_l0Channels = None
_l0Masks    = None

########################################## public visible part of this module...
### setupL0Channels _must_ be called before L0Channels, convertL0Candidates or HltL0Candidates is
### invoked...


def decodeL0Channels( L0TCK , skipDisabled = True, forceSingleL0Configuration = True) :
    importOptions('$L0TCK/L0DUConfig.opts')
    from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
    if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
        raise KeyError('requested L0 TCK %s is not known'%L0TCK)
    if 'ToolSvc.L0DUConfig.TCK_%s'%L0TCK not in allConfigurables :
        raise KeyError('requested L0DUConfigProvider for TCK %s is not known'%L0TCK)
    l0config = _updateL0Config( L0TCK, forceSingleL0Configuration )
    channels = _parseL0settings( l0config.Channels )

    print '# decoded L0 channels for L0TCK=%s: %s'%(L0TCK, channels)
    global _l0Masks
    if _l0Masks : raise RunTimeError('HltL0Candidates already initialized -- 2nd call to decodeL0Channels...')
    _l0Masks = {}
    for i in channels :
        _l0Masks[i['name']] = _parseMask(i['MASK'])  if 'MASK' in i else None
    def _hasBeenDisabled( d ) :
        if 'DISABLE' in d and d['DISABLE'].upper().find('TRUE') != -1 : return True # old style
        if 'MASK'    in d and _parseMask(d['MASK']) == 'Disable' : return True    
        return False
    return [ i['name'] for i in channels if ( not skipDisabled or not _hasBeenDisabled(i)) ]


def L0Mask( channel ) :
    global _dict,_l0Masks
    if channel not in _dict.keys() : raise KeyError('Unknown L0 channel requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _l0Masks[channel]

def L0Mask2ODINPredicate( mask ) :
    _predicate = { 'Disable'              : 'ODIN_NONE'
                 , 'Physics'              : 'ODIN_ALL'
                 , 'Beam1'                : '( ODIN_BXTYP == LHCb.ODIN.Beam1 )'
                 , 'Physics+Beam1'        : 'ODIN_ALL'
                 , 'Beam2'                : '( ODIN_BXTYP == LHCb.ODIN.Beam2 )'
                 , 'Physics+Beam2'        : 'ODIN_ALL'
                 , 'Beam1+Beam2'          : '( ODIN_BXTYP == LHCb.ODIN.Beam1 )|( ODIN_BXTYP == LHCb.ODIN.Beam2 )'
                 , 'Physics+Beam1+Beam2'  : 'ODIN_ALL'
                 }
    return _predicate[mask]

def setupL0Channels( channels ) :
    global _l0Channels,_dict
    if _dict or _l0Channels  : raise RunTimeError('HltL0Candidates already initialized -- 2nd call to setupL0Channels...')
    _l0Channels = channels
    #if not _l0Channels:
    #    _l0Channels = [ 'Muon','DiMuon','Muon,lowMult','DiMuon,lowMult','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]

    print '# recognized L0 channels: ' + str(_l0Channels)
    # the types are basically hardwired and are thus not likely to change...
    _l0Types = [ 'Muon','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
    _dict = dict()
    for i in _l0Channels + [ 'All' + j for j in _l0Types ] : _dict.update( _converter( i ) )

def L0Channels() :
    global _l0Channels
    if _l0Channels == None : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    return _l0Channels

def convertL0Candidates(channel) :
    global _dict
    if not _dict : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _dict[channel]

def HltL0Candidates(channel) :
    global _dict
    if not _dict : raise RunTimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _name(channel)
