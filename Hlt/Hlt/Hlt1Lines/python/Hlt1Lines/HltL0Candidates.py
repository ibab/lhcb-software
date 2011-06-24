#
#==============================================================================
# $Id: HltL0Candidates.py,v 1.20 2010-03-29 12:41:39 graven Exp $
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



# utilities to pack and unpack L0 conditions into Condition property...
# given the 'Conditions' property of an L0DUCOnfig tool instance, 
# return a pythonized table of settings
# i.e. the inverse of 'genItems'
def _parseL0settings( settings ) :
    d = {}
    for s in  [ _parseL0setting(i) for i in settings ] :
        d[s['name']] = s
    return d
def _parseL0setting( setting ) :
    p = re.compile('([^ ]+) *= *\[(.+)\]')
    val = {}
    for s in setting :
        m = p.match(s)
        key = m.group(1) 
        value = m.group(2)
        # adapt to ideosyncracies
        if key == 'rate=' : key = 'rate'
        if key == 'conditions' :
            value = '[%s]'%value # put back the []
            # make it a list of conditions which are ANDed
            if '&&' in value : value = value.split('&&')
            else             : value = [ value ]
            #  and get rid of the [] again...
            value =  [ re.sub('\[(.*)\]',r'\1',i.strip()) for i in value ]
        val.update( { key : value } )
    return val

def _name(i) : 
    return 'Hlt1L0'+i+'Candidates' if i.startswith('All') else 'Hlt1L0'+i+'Decision'

def _muon( channel ) :
    return { channel : None}

def _calo( channel ) :
    return {channel: None}

def _converter( channel ) :
    typeMapper =      { 'Hadron'       : lambda x : _calo( x )
                      , 'Electron'     : lambda x : _calo( x )
                      , 'Photon'       : lambda x : _calo( x )
                      , 'LocalPi0'     : lambda x : _calo( x )
                      , 'GlobalPi0'    : lambda x : _calo( x )
                      , 'Muon'         : lambda x : _muon( x )
                      }
    conditionMapper = { 'Electron(Et)' : 'Electron'
                      , 'Photon(Et)'   : 'Photon'
                      , 'GlobalPi0(Et)': 'GlobalPi0'
                      , 'LocalPi0(Et)' : 'LocalPi0'
                      , 'Hadron(Et)'   : 'Hadron'
                      , 'Muon1(Pt)'    : 'Muon'
                      , 'Muon2(Pt)'    : 'Muon'
                      , 'DiMuon(Pt)'   : 'Muon'
                      }
    if 'All' in channel : return typeMapper[ channel.lstrip('All') ](channel)
    conditionTypes = set()
    # TODO: go recursive, and substitute channels which act as conditions with their own conditions
    for i in _l0Channels[channel]['conditions'] :
        if i in _l0Conditions : 
            conditionData =  _l0Conditions[i]['data']
            if conditionData in conditionMapper : 
                conditionTypes.update( [ conditionMapper[conditionData]  ])
    if not conditionTypes :         return { channel : None }
    elif len(conditionTypes) == 1 : return typeMapper[conditionTypes.pop()](channel)
    else :                          return { channel : None }

def _parseMask( mask ) :
    # keys are  GaudiPython.gbl.LHCb.L0DUDecision.Name[i]
    map =  { 'Disable'             : [ '0','0X0','000'           ]
           , 'Physics'             : [ '1','0X1','001','PHYSICS' ]
           , 'Beam1'               : [ '2','0X2','010','BEAM1'   ]
           , 'Physics+Beam1'       : [ '3','0X3','011'           ]
           , 'Beam2'               : [ '4','0X4','100','BEAM2'   ]
           , 'Physics+Beam2'       : [ '5','0X5','101'           ]
           , 'Beam1+Beam2'         : [ '6','0X6','110'           ]
           , 'Physics+Beam1+Beam2' : [ '7','0X7','111'           ]
           }
    for (i,j) in map.iteritems() :
          if mask.upper() in j : return i
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


global _dict,_l0Channels, _l0Conditions
_dict       = None
_l0Channels = None
_l0Conditions = None

########################################## public visible part of this module...
### setupL0Channels _must_ be called before L0Channels, convertL0Candidates or HltL0Candidates is
### invoked...


def decodeL0Channels( L0TCK , skipDisabled = True, forceSingleL0Configuration = True) :
    importOptions('$L0TCK/L0DUConfig.opts')
    from Configurables import L0DUMultiConfigProvider,L0DUConfigProvider
    # canonicalize L0 TCK...
    L0TCK = int(L0TCK,16) if type(L0TCK)==str else L0TCK
    L0TCK = '0x%04X' %  L0TCK  
    if L0TCK not in L0DUMultiConfigProvider('L0DUConfig').registerTCK :
        raise KeyError('requested L0 TCK %s is not known'%L0TCK)
    if 'ToolSvc.L0DUConfig.TCK_%s'%L0TCK not in allConfigurables :
        raise KeyError('requested L0DUConfigProvider for TCK %s is not known'%L0TCK)
    l0config = _updateL0Config( L0TCK, forceSingleL0Configuration )
    global _l0Channels,_l0Conditions
    if _l0Channels or _l0Conditions : raise RuntimeError('HltL0Candidates already initialized -- 2nd call to decodeL0Channels...')

    _l0Channels   = _parseL0settings( l0config.Channels )
    _l0Conditions = _parseL0settings( l0config.Conditions )

    print '# decoded L0 channels for L0TCK=%s: %s'%(L0TCK, [ i for i in _l0Channels.iterkeys()])
    def _hasBeenDisabled( d ) :
        if 'DISABLE' in d and d['DISABLE'].upper().find('TRUE') != -1 : return True # old style
        if 'MASK'    in d and _parseMask(d['MASK']) == 'Disable' : return True    
        return False
    return [ i for i in _l0Channels.iterkeys() if ( not skipDisabled or not _hasBeenDisabled(_l0Channels[i])) ]


def L0Mask( channel ) :
    global _dict,_l0Channels
    if channel not in _dict.keys() : raise KeyError('Unknown L0 channel requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    x = _l0Channels[ channel ] 
    return _parseMask(x['MASK']) if 'MASK' in x else None


### From Olivier Deschamps:
### ODIN-L0  decision =     (L0-bit0)   OR  (L0-bit1 and ODIN.BeamCrossing=Beam1) OR (L0-bit2 and ODIN.BeamCrossing=Beam2)

def L0Mask2ODINPredicate( mask ) :
    _predicate = { 'Disable'              : 'ODIN_NONE'
                 , 'Physics'              : 'ODIN_ALL'
                 , 'Beam1'                : '( ODIN_BXTYP == LHCb.ODIN.Beam1 )|( ODIN_BXTYP == LHCb.ODIN.NoBeam )'
                 , 'Physics+Beam1'        : 'ODIN_ALL'
                 , 'Beam2'                : '( ODIN_BXTYP == LHCb.ODIN.Beam2 )|( ODIN_BXTYP == LHCb.ODIN.NoBeam )'
                 , 'Physics+Beam2'        : 'ODIN_ALL'
                 , 'Beam1+Beam2'          : '( ODIN_BXTYP == LHCb.ODIN.Beam1 )|( ODIN_BXTYP == LHCb.ODIN.Beam2 )| ( ODIN_BXTYP == LHCb.ODIN.NoBeam )'
                 , 'Physics+Beam1+Beam2'  : 'ODIN_ALL'
                 }
    return _predicate[mask]

def setupL0Channels( ) :
    global _l0Channels,_dict
    if _dict  : raise RuntimeError('HltL0Candidates already initialized -- 2nd call to setupL0Channels...')
    # the types are basically hardwired and are thus not likely to change...
    _l0Types = [ 'Muon','Electron','Photon','Hadron' ,'LocalPi0','GlobalPi0' ]
    _dict = dict()
    if not _l0Channels : 
         log.warning('HLT does not know about any L0 Channels.')
         log.warning('This is most likely because you have not specified (to the HLT) what L0 configuration you want to use.')
         log.warning('Either specify a ThresholdSettings, or an explicit L0TCK')
         log.warning('Continuing with an empty list of L0 Channels')
         log.warning('This implies that Hlt lines which explicitly use an L0 channel will NOT be configured')
         _l0Channels = {}
    for i in [ j for j in L0Channels() ] + [ 'All' + j for j in _l0Types ] : _dict.update( _converter( i ) )

def L0Channels() :
    global _l0Channels
    if _l0Channels == None : raise RuntimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    return _l0Channels.iterkeys()

def convertL0Candidates(channel) :
    global _dict
    if not _dict : raise RuntimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _dict[channel]

def HltL0Candidates(channel) :
    global _dict
    if not _dict : raise RuntimeError('HltL0Candidates not initialized -- no call to setupL0Channels...')
    if channel not in _dict.keys() : raise KeyError('Unknown L0 candidates requested: %s -- check that this channel is included in requested L0 configuration...'%channel)
    return _name(channel)
