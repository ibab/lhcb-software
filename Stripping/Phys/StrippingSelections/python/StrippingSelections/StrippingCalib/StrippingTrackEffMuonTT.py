###'''
###Module for construction of Jpsi -> mu mu /  Upsilon -> mu mu /  Z -> mu mu / B -> J/psi K to be used to measure tracking efficiency.  
###Provides functions to build a Jpsi / Upsilon / Z / B selection , with a long track and a muonTT track.
###Provides class StrippingTrackEffMuonTTConf, which constructs the Selections and 
###StrippingLines given a configuration dictionary.
###Original autor: Michel De Cian
###Exported symbols (use python help!):
###           - StrippingTrackEffMuonTTConf
###           - selFilterLongPartsMu
###           - selFilterLongPartsK
###           - selMakeMuonTT
###           - selMuonTTPParts
###           - selMuonTTParts
###           - selHlt1Jpsi
###           - selHlt1Upsilon
###           - selHlt1Z
###           - selHlt2Jpsi
###           - selHlt2Upsilon
###           - selHlt2Z
###           - selHlt2BJpsiKMu
###           - selHlt2BJpsiKK
###           - chargeFilter
###           - makeResonanceMuMuTrackEff
###           - makeBJpsiKTrackEff
###           - filterHLT2ForBJpsiK
###      
###'''

__author__ = ['Michael Kolpin']
__date__ = '23/03/2015'
__version__ = '$Revision: 1.1 $'


__all__ = ('StrippingTrackEffMuonTTConf', 
           'default_config',
           'selFilterLongPartsMu',
           'selFilterLongPartsMuUpsilonZ',
           'selFilterLongPartsK',
           'selMakeMuonTT',
           'selMuonTTPParts',
           'selMuonTTParts',
           'selHlt1Jpsi',
           'selHlt1Upsilon',
           'selHlt1Z',
           'selHlt2Jpsi',
           'selHlt2Upsilon',
           'selHlt2Z',
           'selHlt2BJpsiKMu',
           'selHlt2BJpsiKK',
           'chargeFilter',
           'makeResonanceMuMuTrackEff',
           'makeBJpsiKTrackEff',
           'filterHLT2ForBJpsiK')

from Gaudi.Configuration import *
from PhysSelPython.Wrappers import Selection, DataOnDemand, ChargedProtoParticleSelection
from StrippingConf.StrippingLine import StrippingLine
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StrippingUtils.Utils import LineBuilder, checkConfig
from StandardParticles import StdLooseKaons, StdLooseMuons, StdAllLooseMuons 


from Configurables import (MuonCombRec, 
                           MuonTTTrack,
			   MuonHitDecode,
                           PatAddTTCoord, 
                           TrackMasterFitter, 
                           TrackMasterExtrapolator, 
                           TrackSelector, 
                           NoPIDsParticleMaker,
                           TisTosParticleTagger
                           )


#default_name = 'TrackEffMuonTT'

default_config = {
    'NAME'        : 'TrackEffMuonTT',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'StrippingTrackEffMuonTTConf',
    'CONFIG'      : { 
			'JpsiMassWin'                 : 500,
			'UpsilonMassWin'              : 1500,
			'ZMassWin'                    : 40000,
			'BMassWin'                    : 500,
			'JpsiMuonTTPT'                : 0,
			'UpsilonMuonTTPT'             : 500,
			'ZMuonTTPT'                   : 500,
			'JpsiLongPT'                  : 1300,
			'UpsilonLongPT'               : 1000,
			'ZLongPT'                     : 10000,
			'JpsiPT'                      : 1000,
			'UpsilonPT'                   : 0,
			'ZPT'                         : 0,
			'JpsiLongMuonMinIP'           : 0.5,
			'UpsilonLongMuonMinIP'        : 0,
			'ZLongMuonMinIP'              : 0,
			'JpsiMINIP'                   : 3,
			'UpsilonMINIP'                : 10000, #this is a dummy
			'ZMINIP'                      : 10000, #this is a dummy
			'BJpsiKMINIP'                 : 10000, #this is a dummy
			'JpsiLongMuonTrackCHI2'       : 5,
			'UpsilonLongMuonTrackCHI2'    : 5,
			'ZLongMuonTrackCHI2'          : 5,
			'VertexChi2'                  : 5,
			'LongMuonPID'                 : 2,
            'JpsiHlt1Filter'              : 'Hlt1.*Decision',
            'JpsiHlt2Filter'              : 'Hlt2.*Decision',
			'JpsiHlt1Triggers'            :  { "Hlt1TrackMuonDecision%TOS" : 0},
            'Hlt1PassOnAll'               : True,
			'UpsilonHlt1Triggers'         :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
			'ZHlt1Triggers'               :  { "Hlt1SingleMuonHighPTDecision%TOS" : 0},
			'JpsiHlt2Triggers'            :  { "Hlt2SingleMuon.*Decision%TOS" : 0, "Hlt2TrackEffDiMuonMuonTT.*Decision%TOS" : 0},
			'UpsilonHlt2Triggers'         :  { "Hlt2SingleMuonLowPTDecision%TOS" : 0},
			'ZHlt2Triggers'               :  { "Hlt2SingleMuonHighPTDecision%TOS" : 0},
			'BJpsiKHlt2TriggersTUS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TUS" : 0},
			'BJpsiKHlt2TriggersTOS'       :  { "Hlt2TopoMu2BodyBBDTDecision%TOS" : 0},
			'JpsiPrescale'                : 1,
			'UpsilonPrescale'             : 1,
			'ZPrescale'                   : 1,
			'BJpsiKPrescale'              : 1,
			'Postscale'                   : 1
                    },
    'STREAMS'     : { 'Calibration' : ['StrippingTrackEffMuonTT_JpsiLine1'
                                      ,'StrippingTrackEffMuonTT_JpsiLine2'
                                      ,'StrippingTrackEffMuonTT_UpsilonLine1'
                                      ,'StrippingTrackEffMuonTT_UpsilonLine2'
                                      ,'StrippingTrackEffMuonTT_ZLine1'
                                      ,'StrippingTrackEffMuonTT_ZLine2'
                                      ,'StrippingTrackEffMuonTT_BJpsiKLine1'
                                      ,'StrippingTrackEffMuonTT_BJpsiKLine2']
                                      }
    }


class StrippingTrackEffMuonTTConf(LineBuilder) :
    """
    Builder of Jpsi->mu mu, Upsilon-> mu mu, Z-> mu mu and B -> J/psi K stripping Selection and StrippingLine for measuring the tracking efficiency.
    Constructs the Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> TrackEffMuonTTConfig = { .... }
    >>> from StrippingSelections import StrippingTrackEffMuonTT
    >>> confTrackEffMuonTT = StrippingTrackEffMuonTT.StrippingTrackEffMuonTTConf(name = "bla", config = TrackEffMuonTTConfig)
    >>> stream.appendLines( confTrackEffMuonTT.lines() )
    And:
    >>> trackEffLines = confTrackEffMuonTT.lines()
    >>> for line in trackEffLines :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Lines of interest:

    TrackEffMuonTT_JpsiLine1       : Jpsi-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTT_JpsiLine2       : Jpsi-> mu mu, long: plus, muonTT: minus

    TrackEffMuonTT_UpsilonLine1    : Upsilon-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTT_UpsilonLine2    : Upsilon-> mu mu, long: plus, muonTT: minus
    
    TrackEffMuonTT_ZLine1          : Z-> mu mu, long: minus, muonTT: plus
    TrackEffMuonTT_ZLine2          : Z-> mu mu, long: plus, muonTT: minus

    TrackEffMuonTT_BJpsiKLine1     : B->Jpsi K, long: minus, muon: plus
    TrackEffMuonTT_BJpsiKLine2     : B->Jpsi K, long: plus, muon: minus

    lines                          : List of all lines

    Exports as class data member:
    StrippingTrackEffMuonTTConf.__configuration_keys__ : List of required configuration parameters.
    """

    ################################################################################################

    __configuration_keys__ = ('JpsiMassWin',
                              'UpsilonMassWin',
                              'ZMassWin',
                              'BMassWin',
                              'JpsiMuonTTPT',
                              'UpsilonMuonTTPT',
                              'ZMuonTTPT',
                              'JpsiLongPT',
                              'UpsilonLongPT',
                              'ZLongPT',
                              'JpsiPT',
                              'UpsilonPT',
                              'ZPT',
                              'JpsiLongMuonMinIP',
                              'UpsilonLongMuonMinIP',
                              'ZLongMuonMinIP',
                              'JpsiMINIP',
                              'UpsilonMINIP',
                              'ZMINIP',
                              'BJpsiKMINIP',
                              'JpsiLongMuonTrackCHI2',
                              'UpsilonLongMuonTrackCHI2',
                              'ZLongMuonTrackCHI2',
                              'VertexChi2',
                              'LongMuonPID',
                              'JpsiHlt1Filter',
                              'JpsiHlt2Filter',
                              'JpsiHlt1Triggers',
                              'Hlt1PassOnAll',
                              'UpsilonHlt1Triggers',
                              'ZHlt1Triggers',
                              'JpsiHlt2Triggers',
                              'UpsilonHlt2Triggers',
                              'ZHlt2Triggers',
                              'BJpsiKHlt2TriggersTUS',
                              'BJpsiKHlt2TriggersTOS',
                              'JpsiPrescale',
                              'UpsilonPrescale',
                              'ZPrescale',
                              'BJpsiKPrescale',
                              'Postscale'
                              )

    def __init__(self, name, config = None):
            
        LineBuilder.__init__(self, name, config)

                
        # ########################################################################################
        # -->
        # --> INITIALIZING ALL THE OBJECTS NEEDED TO FORM THE LINES <--
        # -->
        # ########################################################################################

        # initialize all the general things 
        # idea: initialize the first object, that will be called, pass it to the second one, etc.
                                
        # make the muonTT tracks (two instances, as the Z has a different seed station for the muon system than the rest)
        self.SelMakeMuonTTJpsi = selMakeMuonTT(name+'Jpsi', XTolParam = 25.0, MaxChi2TolParam = 7.0, MinAxProjParam = 5.5, MajAxProjParam = 25.0, seedStation = 2)
        self.SelMakeMuonTTZ = selMakeMuonTT(name+'Z', XTolParam = 25.0, MaxChi2TolParam = 7.0, MinAxProjParam = 5.5, MajAxProjParam = 25.0, seedStation = 4)
        self.SelMuonTTPPartsJpsi = selMuonTTPParts(name+'Jpsi', muonTTTrackMaker = self.SelMakeMuonTTJpsi )
        self.SelMuonTTPPartsZ = selMuonTTPParts(name+'Z', muonTTTrackMaker = self.SelMakeMuonTTZ )
        self.SelMuonTTPartsJpsi = selMuonTTParts(name+'Jpsi', protoParticlesMaker = self.SelMuonTTPPartsJpsi )
        self.SelMuonTTPartsZ = selMuonTTParts(name+'Z', protoParticlesMaker = self.SelMuonTTPPartsZ )

        self.SelFilterLongPartsMuJpsi = selFilterLongPartsMu(name+'Jpsi')
        self.SelFilterLongPartsMuUpsilonZ  = selFilterLongPartsMuUpsilonZ(name+'UpsilonZ')
        # ####################################
        # J/psis
        self.SelHlt1JpsiMinus = selHlt1Jpsi(name+'JpsiMinus', longPartsFilter = self.SelFilterLongPartsMuJpsi, triggers = config['JpsiHlt1Triggers'], passonall = config['Hlt1PassOnAll'])
        self.SelHlt1JpsiPlus = selHlt1Jpsi(name+'JpsiPlus', longPartsFilter = self.SelFilterLongPartsMuJpsi, triggers = config['JpsiHlt1Triggers'], passonall = config['Hlt1PassOnAll'])
        self.SelHlt2JpsiMinus = selHlt2Jpsi(name+'JpsiMinus', hlt1Filter = self.SelHlt1JpsiMinus, triggers = config['JpsiHlt2Triggers'])
        self.SelHlt2JpsiPlus = selHlt2Jpsi(name+'JpsiPlus', hlt1Filter = self.SelHlt1JpsiPlus, triggers = config['JpsiHlt2Triggers'])

        self.muonTTMinusJpsi = chargeFilter(name+'MuonTTJpsiMinus', trackAlgo =  'MuonTT',   partSource =  self.SelMuonTTPartsJpsi , charge = -1)
        self.muonTTPlusJpsi = chargeFilter(name+'MuonTTJpsiPlus', trackAlgo = 'MuonTT',  partSource = self.SelMuonTTPartsJpsi,  charge = 1)
        self.longMinusJpsi = chargeFilter( name+'LongJpsiMinus', trackAlgo = 'LongMu',   partSource = self.SelHlt2JpsiMinus, charge = -1)
        self.longPlusJpsi = chargeFilter( name+'LongJpsiPlus', trackAlgo =  'LongMu',   partSource = self.SelHlt2JpsiPlus  , charge = 1)
        # ##########################################
        # Upsilons
        self.SelHlt1UpsilonMinus = selHlt1Upsilon(name+'UpsilonMinus', longPartsFilter = self.SelFilterLongPartsMuUpsilonZ, triggers = config['UpsilonHlt1Triggers'])
        self.SelHlt1UpsilonPlus = selHlt1Upsilon(name+'UpsilonPlus', longPartsFilter = self.SelFilterLongPartsMuUpsilonZ, triggers = config['UpsilonHlt1Triggers'])
        self.SelHlt2UpsilonMinus = selHlt2Upsilon(name+'UpsilonMinus', hlt1Filter = self.SelHlt1UpsilonMinus, triggers = config['UpsilonHlt2Triggers'])
        self.SelHlt2UpsilonPlus = selHlt2Upsilon(name+'UpsilonPlus', hlt1Filter = self.SelHlt1UpsilonPlus, triggers = config['UpsilonHlt2Triggers'])

        self.muonTTPlusUpsilon = chargeFilter(name+'MuonTTUpsilonPlus', trackAlgo = 'MuonTT',  partSource = self.SelMuonTTPartsJpsi ,  charge = 1)
        self.muonTTMinusUpsilon = chargeFilter(name+'MuonTTUpsilonMinus', trackAlgo =  'MuonTT',   partSource =  self.SelMuonTTPartsJpsi , charge = -1)
        self.longPlusUpsilon = chargeFilter( name+'LongUpsilonPlus', trackAlgo =  'LongMu',   partSource = self.SelHlt2UpsilonPlus  , charge = 1)
        self.longMinusUpsilon = chargeFilter( name+'LongUpsilonMinus', trackAlgo = 'LongMu',   partSource = self.SelHlt2UpsilonMinus, charge = -1)
        # ##########################################
        # Zs
        self.SelHlt1ZMinus = selHlt1Z(name+'ZMinus', longPartsFilter = self.SelFilterLongPartsMuUpsilonZ, triggers = config['ZHlt1Triggers'])
        self.SelHlt1ZPlus = selHlt1Z(name+'ZPlus', longPartsFilter = self.SelFilterLongPartsMuUpsilonZ, triggers = config['ZHlt1Triggers'])
        self.SelHlt2ZMinus = selHlt2Z(name+'ZMinus', hlt1Filter = self.SelHlt1ZMinus, triggers = config['ZHlt2Triggers'])
        self.SelHlt2ZPlus = selHlt2Z(name+'ZPlus', hlt1Filter = self.SelHlt1ZPlus, triggers = config['ZHlt2Triggers'])

        self.muonTTPlusZ = chargeFilter(name+'MuonTTZPlus', trackAlgo = 'MuonTT',  partSource = self.SelMuonTTPartsZ ,  charge = 1)
        self.muonTTMinusZ = chargeFilter(name+'MuonTTZMinus', trackAlgo =  'MuonTT',   partSource =  self.SelMuonTTPartsZ , charge = -1)
        self.longPlusZ = chargeFilter( name+'LongZPlus', trackAlgo =  'LongMu',   partSource = self.SelHlt2ZPlus  , charge = 1)
        self.longMinusZ = chargeFilter( name+'LongZMinus', trackAlgo = 'LongMu',   partSource = self.SelHlt2ZMinus, charge = -1)
        # ##########################################
        # B-> J/psi K
        # first for the J/psi object
        self.SelHlt1BJpsiKMinus = selHlt1Jpsi(name+'BJpsiKMinus', longPartsFilter = self.SelFilterLongPartsMuJpsi, triggers = config['JpsiHlt1Triggers'], passonall = config['Hlt1PassOnAll']) # HLT1 is the same as for Jpsis!
        self.SelHlt1BJpsiKPlus = selHlt1Jpsi(name+'BJpsiKPlus', longPartsFilter = self.SelFilterLongPartsMuJpsi, triggers = config['JpsiHlt1Triggers'], passonall = config['Hlt1PassOnAll'])
        self.SelHlt2BJpsiKMinus = selHlt2BJpsiKMu(name+'BJpsiKMinus', hlt1Filter = self.SelHlt1BJpsiKMinus, triggers = config['BJpsiKHlt2TriggersTUS'])
        self.SelHlt2BJpsiKPlus = selHlt2BJpsiKMu(name+'BJpsiKPlus', hlt1Filter = self.SelHlt1BJpsiKPlus, triggers = config['BJpsiKHlt2TriggersTUS'])
        
        # and now for the K (no distinction for charged needed, will be done automatically be the decay descriptor)
        self.SelFilterLongPartsBJpsiKK = selFilterLongPartsK(name+'BJpsiK')
        self.SelHlt2BJpsiKK = selHlt2BJpsiKK( name+'BJpsiK',longPartsFilter = self.SelFilterLongPartsBJpsiKK, triggers = config['BJpsiKHlt2TriggersTUS'])
        
        self.muonTTPlusBJpsiK = chargeFilter(name+'MuonTTBJpsiKPlus', trackAlgo = 'MuonTT',  partSource = self.SelMuonTTPartsJpsi ,  charge = 1)
        self.muonTTMinusBJpsiK = chargeFilter(name+'MuonTTBJpsiKMinus', trackAlgo =  'MuonTT',   partSource =  self.SelMuonTTPartsJpsi , charge = -1)
        self.longPlusBJpsiK = chargeFilter( name+'LongBJpsiKPlus', trackAlgo =  'LongMu',   partSource = self.SelHlt2BJpsiKPlus  , charge = 1)
        self.longMinusBJpsiK = chargeFilter( name+'LongBJpsiKMinus', trackAlgo = 'LongMu',   partSource = self.SelHlt2BJpsiKMinus, charge = -1)
        # ##################################################################################################################1
        # def makeResonanceMuMuTrackEff(name, resonanceName, decayDescriptor, plusCharge, minusCharge, 
        # mode, massWin, vertexChi2, resonancePT, muonTTPT, longPT, longMuonPID, longMuonMinIPCHI2, longMuonTrackCHI2):

        # J/psi -> mu mu, tag-and-probe
        self.makeJpsiMuMuTrackEff1 = makeResonanceMuMuTrackEff(name+'_MakeJpsiMuMuTrackEff1', 
                                                               resonanceName = 'J/psi(1S)',
                                                               decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                               plusCharge = self.muonTTPlusJpsi, 
                                                               minusCharge = self.longMinusJpsi,
                                                               mode = 1,
                                                               massWin = config['JpsiMassWin'], 
                                                               vertexChi2 = config['VertexChi2'],
                                                               resonancePT = config['JpsiPT'],
                                                               muonTTPT = config['JpsiMuonTTPT'], 
                                                               longPT = config['JpsiLongPT'], 
                                                               longMuonPID = config['LongMuonPID'],
                                                               longMuonMinIP = config['JpsiLongMuonMinIP'],
                                                               longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'],
                                                               motherMinIP = config['JpsiMINIP'])
        # J/psi -> mu mu, probe-and-tag
        self.makeJpsiMuMuTrackEff2 = makeResonanceMuMuTrackEff(name+'_MakeJpsiMuMuTrackEff2', 
                                                               resonanceName = 'J/psi(1S)',
                                                               decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                               minusCharge = self.muonTTMinusJpsi,
                                                               plusCharge = self.longPlusJpsi, 
                                                               mode = 2,
                                                               massWin = config['JpsiMassWin'], 
                                                               vertexChi2 = config['VertexChi2'],
                                                               resonancePT = config['JpsiPT'],
                                                               muonTTPT = config['JpsiMuonTTPT'], 
                                                               longPT = config['JpsiLongPT'], 
                                                               longMuonPID = config['LongMuonPID'],
                                                               longMuonMinIP = config['JpsiLongMuonMinIP'],
                                                               longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'],
                                                               motherMinIP = config['JpsiMINIP'])
        # ##################################################################################################################
        # # Upsilon -> mu mu, tag-and-probe
        self.makeUpsilonMuMuTrackEff1 = makeResonanceMuMuTrackEff(name+'_MakeUpsilonMuMuTrackEff1', 
                                                                  resonanceName = 'Upsilon(1S)',
                                                                  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
                                                                  plusCharge = self.muonTTPlusUpsilon, 
                                                                  minusCharge = self.longMinusUpsilon,
                                                                  mode = 1,
                                                                  massWin = config['UpsilonMassWin'], 
                                                                  vertexChi2 = config['VertexChi2'],
                                                                  resonancePT = config['UpsilonPT'],
                                                                  muonTTPT = config['UpsilonMuonTTPT'], 
                                                                  longPT = config['UpsilonLongPT'], 
                                                                  longMuonPID = config['LongMuonPID'],
                                                                  longMuonMinIP = config['UpsilonLongMuonMinIP'],
                                                                  longMuonTrackCHI2 = config['UpsilonLongMuonTrackCHI2'],
                                                                  motherMinIP = config['UpsilonMINIP'])
        # Upsilon -> mu mu, probe-and-tag
        self.makeUpsilonMuMuTrackEff2 = makeResonanceMuMuTrackEff(name+'_MakeUpsilonMuMuTrackEff2', 
                                                                  resonanceName = 'Upsilon(1S)',
                                                                  decayDescriptor = 'Upsilon(1S) -> mu+ mu-',
                                                                  plusCharge = self.longPlusUpsilon, 
                                                                  minusCharge = self.muonTTMinusUpsilon,
                                                                  mode = 2,
                                                                  massWin = config['UpsilonMassWin'], 
                                                                  vertexChi2 = config['VertexChi2'],
                                                                  resonancePT = config['UpsilonPT'],
                                                                  muonTTPT = config['UpsilonMuonTTPT'], 
                                                                  longPT = config['UpsilonLongPT'], 
                                                                  longMuonPID = config['LongMuonPID'],
                                                                  longMuonMinIP = config['UpsilonLongMuonMinIP'],
                                                                  longMuonTrackCHI2 = config['UpsilonLongMuonTrackCHI2'],
                                                                  motherMinIP = config['UpsilonMINIP'])
        # ##################################################################################################################
        # Z -> mu mu, tag-and-probe
        self.makeZMuMuTrackEff1 = makeResonanceMuMuTrackEff(name+'_MakeZMuMuTrackEff1', 
                                                            resonanceName = 'Z0',
                                                            decayDescriptor = 'Z0 -> mu+ mu-',
                                                            plusCharge = self.muonTTPlusZ, 
                                                            minusCharge = self.longMinusZ,
                                                            mode = 1,
                                                            massWin = config['ZMassWin'], 
                                                            vertexChi2 = config['VertexChi2'],
                                                            resonancePT = config['ZPT'],
                                                            muonTTPT = config['ZMuonTTPT'], 
                                                            longPT = config['ZLongPT'], 
                                                            longMuonPID = config['LongMuonPID'],
                                                            longMuonMinIP = config['ZLongMuonMinIP'],
                                                            longMuonTrackCHI2 = config['ZLongMuonTrackCHI2'],
                                                            motherMinIP = config['ZMINIP'])
        # Z -> mu mu, probe-and-tag
        self.makeZMuMuTrackEff2 = makeResonanceMuMuTrackEff(name+'_MakeZMuMuTrackEff2', 
                                                            resonanceName = 'Z0',
                                                            decayDescriptor = 'Z0 -> mu+ mu-',
                                                            plusCharge = self.longPlusZ, 
                                                            minusCharge = self.muonTTMinusZ,
                                                            mode = 2,
                                                            massWin = config['ZMassWin'], 
                                                            vertexChi2 = config['VertexChi2'],
                                                            resonancePT = config['ZPT'],
                                                            muonTTPT = config['ZMuonTTPT'], 
                                                            longPT = config['ZLongPT'], 
                                                            longMuonPID = config['LongMuonPID'],
                                                            longMuonMinIP = config['ZLongMuonMinIP'],
                                                            longMuonTrackCHI2 = config['ZLongMuonTrackCHI2'],
                                                            motherMinIP = config['ZMINIP'])
        # ##################################################################################################################
        # B -> J/psi K exclusive, tag-and-probe
        self.makeJpsiMuMuForBJpsiKTrackEff1 = makeResonanceMuMuTrackEff(name+'_MakeJpsiMuMuForBJpsiKTrackEff1', 
                                                                        resonanceName = 'J/psi(1S)',
                                                                        decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                                        plusCharge = self.muonTTPlusBJpsiK , 
                                                                        minusCharge = self.longMinusBJpsiK,
                                                                        mode = 1,
                                                                        massWin = config['JpsiMassWin'], 
                                                                        vertexChi2 = config['VertexChi2'],
                                                                        resonancePT = config['JpsiPT'],
                                                                        muonTTPT = config['JpsiMuonTTPT'], 
                                                                        longPT = config['JpsiLongPT'], 
                                                                        longMuonPID = config['LongMuonPID'],
                                                                        longMuonMinIP = config['JpsiLongMuonMinIP'],
                                                                        longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'],
                                                                        motherMinIP = config['BJpsiKMINIP'])
        self.makeBJpsiKTrackEff1 = makeBJpsiKTrackEff(name+'_MakeBJpsiKTrackEff1',
                                                      JpsiSel = self.makeJpsiMuMuForBJpsiKTrackEff1,
                                                      KSel = self.SelHlt2BJpsiKK,
                                                      massWin = config['BMassWin'], 
                                                      vertexChi2 = config['VertexChi2'])
        self.BJpsiKHLT2Filter1 = filterHLT2ForBJpsiK( name+'_BJpsiKHLT2Filter1', BJpsiKSel = self.makeBJpsiKTrackEff1, triggers = config['BJpsiKHlt2TriggersTOS']) # Event must be filtered to be sure its TOS on the tag muon and the Kaon
        
        # B -> J/psi K exclusive, probe-and-tag
        self.makeJpsiMuMuForBJpsiKTrackEff2 = makeResonanceMuMuTrackEff(name+'_MakeJpsiMuMuForBJpsiKTrackEff2', 
                                                                        resonanceName = 'J/psi(1S)',
                                                                        decayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                                                        plusCharge = self.longPlusBJpsiK , 
                                                                        minusCharge = self.muonTTMinusBJpsiK,
                                                                        mode = 2,
                                                                        massWin = config['JpsiMassWin'], 
                                                                        vertexChi2 = config['VertexChi2'],
                                                                        resonancePT = config['JpsiPT'],
                                                                        muonTTPT = config['JpsiMuonTTPT'], 
                                                                        longPT = config['JpsiLongPT'], 
                                                                        longMuonPID = config['LongMuonPID'],
                                                                        longMuonMinIP = config['JpsiLongMuonMinIP'],
                                                                        longMuonTrackCHI2 = config['JpsiLongMuonTrackCHI2'],
                                                                        motherMinIP = config['BJpsiKMINIP'])
        
        self.makeBJpsiKTrackEff2 = makeBJpsiKTrackEff(name+'_MakeBJpsiKTrackEff2',
                                                      JpsiSel = self.makeJpsiMuMuForBJpsiKTrackEff2,
                                                      KSel = self.SelHlt2BJpsiKK,
                                                      massWin = config['BMassWin'], 
                                                      vertexChi2 = config['VertexChi2'])
        
        self.BJpsiKHLT2Filter2 = filterHLT2ForBJpsiK( name+'_BJpsiKHLT2Filter2', BJpsiKSel = self.makeBJpsiKTrackEff2, triggers = config['BJpsiKHlt2TriggersTOS'])  # Event must be filtered to be sure its TOS on the tag muon and the Kaon
        # # ##################################################################################################################
    


        ################################################
        # Jpsi -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTJpsi_line1 = StrippingLine(name+'_JpsiLine1',
                                                      prescale = config['JpsiPrescale'],
                                                      postscale = config['Postscale'],
                                                      HLT1 = "HLT_PASS_RE('%(JpsiHlt1Filter)s')" % config,
                                                      HLT2 = "HLT_PASS_RE('%(JpsiHlt2Filter)s')" % config,
                                                      selection = self.makeJpsiMuMuTrackEff1)
        

        self.TrackEffMuonTTJpsi_line2 = StrippingLine(name+'_JpsiLine2',
                                                      prescale = config['JpsiPrescale'],
                                                      postscale = config['Postscale'],
                                                      HLT1 = "HLT_PASS_RE('%(JpsiHlt1Filter)s')" % config,
                                                      HLT2 = "HLT_PASS_RE('%(JpsiHlt2Filter)s')" % config,
                                                      selection = self.makeJpsiMuMuTrackEff2)

    
        ################################################
        # Upsilon -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTUpsilon_line1 = StrippingLine(name+'_UpsilonLine1',
                                                         prescale = config['UpsilonPrescale'],
                                                         postscale = config['Postscale'],
                                                         selection = self.makeUpsilonMuMuTrackEff1)
                                                         

        self.TrackEffMuonTTUpsilon_line2 = StrippingLine(name+'_UpsilonLine2',
                                                         prescale = config['UpsilonPrescale'],
                                                         postscale = config['Postscale'],
                                                         selection = self.makeUpsilonMuMuTrackEff2)
                                                         
        
        ################################################
        # Z -> mu mu stripping lines (long + muonTT)
        ################################################
        self.TrackEffMuonTTZ_line1 = StrippingLine(name+'_ZLine1',
                                                   prescale = config['ZPrescale'],
                                                   postscale = config['Postscale'],
                                                   selection = self.makeZMuMuTrackEff1)
        


        self.TrackEffMuonTTZ_line2 = StrippingLine(name+'_ZLine2',
                                                   prescale = config['ZPrescale'],
                                                   postscale = config['Postscale'],
                                                   selection = self.makeZMuMuTrackEff2)
        
        ################################################
        # B -> J/psi K stripping lines (long + muonTT + long)
        ################################################                                        
        self.TrackEffMuonTTBJpsiK_line1 = StrippingLine(name+'_BJpsiKLine1',
                                                        prescale = config['BJpsiKPrescale'],
                                                        postscale = config['Postscale'],
                                                        #HLT = "HLT_PASS('Hlt2MuTrackDecision')",
                                                        selection = self.BJpsiKHLT2Filter1
                                                        )
        
        self.TrackEffMuonTTBJpsiK_line2 = StrippingLine(name+'_BJpsiKLine2',
                                                        prescale = config['BJpsiKPrescale'],
                                                        postscale = config['Postscale'],
                                                        #HLT = "HLT_PASS('Hlt2MuTrackDecision')",
                                                        selection = self.BJpsiKHLT2Filter2)
                                                        

        
        ################################################
        # register the lines
        ################################################
        self.registerLine( self.TrackEffMuonTTJpsi_line1 )
        self.registerLine( self.TrackEffMuonTTJpsi_line2 )
        self.registerLine( self.TrackEffMuonTTUpsilon_line1 )
        self.registerLine( self.TrackEffMuonTTUpsilon_line2 )
        self.registerLine( self.TrackEffMuonTTZ_line1 )
        self.registerLine( self.TrackEffMuonTTZ_line2 )
        self.registerLine( self.TrackEffMuonTTBJpsiK_line1 )
        self.registerLine( self.TrackEffMuonTTBJpsiK_line2 )





# ########################################################################################
# -->
# --> PREPARE THE LONG TRACKS <--
# -->
# ########################################################################################

# ########################################################################################
# The long track preparation, including hard coded cuts
# ########################################################################################
def selFilterLongPartsMu(name):
    """
    Get Muons from StdLooseMuons for detached Jpsi
    """
    Filter = FilterDesktop()
    FilterLongPartsMu = Filter.configurable(name+"FilterLongPartsMu")
    FilterLongPartsMu.Code = "(HASMUON) & (ISMUON) & (PIDmu > 2.0) & (P > 10000) & (PT > 1300*MeV) & (MIPDV(PRIMARY) > 0.5) & (TRCHI2DOF < 2.0) & (MIPCHI2DV(PRIMARY) > 200)" 
    # These cuts are now tighter than the trigger that follows

    return Selection(name+"_SelFilterLongPartsMu", Algorithm = FilterLongPartsMu, RequiredSelections = [StdLooseMuons])
# ########################################################################################
def selFilterLongPartsMuUpsilonZ(name):
    """
    Get Muons from StdAllLooseMuons for Upsilon and Z
    """
    Filter = FilterDesktop()
    FilterLongPartsMuUpsilonZ = Filter.configurable(name+"FilterLongPartsMuUpsilonZ")
    FilterLongPartsMuUpsilonZ.Code = "(HASMUON) & (ISMUON) & (PIDmu > 2.0) & (P > 10000) & (PT > 3000*MeV) & (TRCHI2DOF < 2.0)"

    return Selection(name+"_SelFilterLongPartsMuUpsilonZ", Algorithm = FilterLongPartsMuUpsilonZ, RequiredSelections = [StdAllLooseMuons])
# ########################################################################################
def selFilterLongPartsK(name):
    """
    Get Kaons from StdLooseKaons
    """
    Filter = FilterDesktop()
    FilterLongPartsK = Filter.configurable(name+"FilterLongPartsK")
    FilterLongPartsK.Code = "((~HASMUON) | (~ISMUON)) & (P > 10000) & (PT > 800) & (PIDK > 0) & (TRCHI2DOF < 5.0) & (MIPDV(PRIMARY) > 0.1*mm)"
#
    return Selection(name+"_SelFilterLongPartsK", Algorithm = FilterLongPartsK, RequiredSelections = [StdLooseKaons])
# ########################################################################################

# ########################################################################################
# -->
# --> PATTERN RECOGNITION AND MUONTT TRACK MAKING <--
# -->
# ########################################################################################

# ########################################################################################
# The pattern recognition -> muonTT track stuff
# ########################################################################################
def selMakeMuonTT(name, XTolParam, MaxChi2TolParam, MinAxProjParam, MajAxProjParam, seedStation):
    """
    Make a muonTT track out of hits in the muon station and TT, and give it some options to configure
    """
    MakeMuonTT = MuonTTTrack(name+"MakeMuonTT")
    MakeMuonTT.ToolName = "MuonCombRec"
    MakeMuonTT.addTool( MuonCombRec )
    MakeMuonTT.MuonCombRec.MeasureTime = True
    MakeMuonTT.MuonCombRec.CloneKiller = False
    MakeMuonTT.MuonCombRec.SkipStation = -1 # -1=no skip, 0=M1, 1=M2, 2=M3, 3=M4, 4=M5
    MakeMuonTT.MuonCombRec.DecodingTool = "MuonHitDecode"
    MakeMuonTT.MuonCombRec.PadRecTool = "MuonPadFromCoord"
    MakeMuonTT.MuonCombRec.ClusterTool = "MuonFakeClustering" # to enable: "MuonClusterRec"
    MakeMuonTT.MuonCombRec.PhysicsTiming = True
    MakeMuonTT.MuonCombRec.AssumeCosmics = False
    MakeMuonTT.MuonCombRec.AssumePhysics = True
    MakeMuonTT.MuonCombRec.StrongCloneKiller = False 
    MakeMuonTT.MuonCombRec.SeedStation = seedStation # default seet station is M5
    MakeMuonTT.MuonCombRec.addTool( MuonHitDecode, ("MuonHitDecode") )
    MakeMuonTT.MuonCombRec.MuonHitDecode.SkipHWNumber = True
    # #############################################################
    MakeMuonTT.addTool( PatAddTTCoord )
    MakeMuonTT.PatAddTTCoord.YTolSlope = 400000.0
    MakeMuonTT.PatAddTTCoord.XTol = XTolParam
    MakeMuonTT.PatAddTTCoord.XTolSlope = 400000.0
    MakeMuonTT.PatAddTTCoord.MinAxProj = MinAxProjParam
    MakeMuonTT.PatAddTTCoord.MajAxProj = MajAxProjParam
    MakeMuonTT.PatAddTTCoord.MaxChi2Tol = MaxChi2TolParam
    # ################################################################
    MakeMuonTT.addTool( TrackMasterFitter)
    MakeMuonTT.TrackMasterFitter.MaterialLocator = "SimplifiedMaterialLocator"
    MakeMuonTT.addTool( TrackMasterExtrapolator ) 
    MakeMuonTT.TrackMasterExtrapolator.MaterialLocator = "SimplifiedMaterialLocator"
    # ################################################################
    MakeMuonTT.AddTTHits = True
    MakeMuonTT.MC = False
    MakeMuonTT.OutputLevel = 4
    #
    #return Selection(name+"_SelMakeMuonTT", Algorithm = MakeMuonTT,  InputDataSetter=None)
    return Selection(name+"_SelMakeMuonTT", Algorithm = MakeMuonTT,  OutputBranch='Rec', Extension='Tracks', InputDataSetter=None)
# ########################################################################################
# Make the protoparticles
# ########################################################################################
def selMuonTTPParts(name, muonTTTrackMaker):
    """
    Make ProtoParticles out of muonTT tracks
    """
    return ChargedProtoParticleSelection(name+"_SelMuonTTPParts", RequiredSelections=[muonTTTrackMaker] )
# ################################################################
def selMuonTTParts(name, protoParticlesMaker):
    """
    Make Particles out of the muonTT ProtoParticles
    """
    MuonTTParts = NoPIDsParticleMaker(name+"MuonTTParts")
    MuonTTParts.Particle = 'muon'
    MuonTTParts.addTool( TrackSelector )
    MuonTTParts.TrackSelector.TrackTypes = [ "Long" ]
    MuonTTParts.Input =  "Rec/"+name+"_SelMuonTTPParts/ProtoParticles"
    MuonTTParts.OutputLevel = 4
#
    return Selection(name+"_SelMuonTTParts", Algorithm = MuonTTParts, RequiredSelections = [protoParticlesMaker], InputDataSetter=None)
# ########################################################################################


# ########################################################################################
# -->
# --> HLT STUFF <--
# -->
# ########################################################################################


# ########################################################################################
# HLT 1 lines we run on
# ########################################################################################
def selHlt1Jpsi(name, longPartsFilter, triggers, passonall):
    """
    Filter the long track muon to be TOS on a HLT1 single muon trigger, for J/psi selection
    """
    Hlt1Jpsi = TisTosParticleTagger(name+"Hlt1Jpsi")
    #Hlt1Jpsi.TisTosSpecs = { "Hlt1TrackMuonDecision%TOS" : 0}
    Hlt1Jpsi.TisTosSpecs = triggers
    Hlt1Jpsi.ProjectTracksToCalo = False
    Hlt1Jpsi.CaloClustForCharged = False
    Hlt1Jpsi.CaloClustForNeutral = False
    Hlt1Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt1Jpsi.NoRegex = True
    Hlt1Jpsi.PassOnAll = passonall # TESTING!
#
    return Selection(name+"_SelHlt1Jpsi", Algorithm = Hlt1Jpsi, RequiredSelections = [ longPartsFilter ])
# ################################################################
def selHlt1Upsilon(name, longPartsFilter, triggers):
    """
    Filter the long track muon to be TOS on a HLT1 single muon trigger, for Upsilon selection
    """
    Hlt1Upsilon = TisTosParticleTagger(name+"Hlt1Upsilon")
    #Hlt1Upsilon.TisTosSpecs = { "Hlt1TrackMuonDecision%TOS" : 0, "Hlt1SingleMuonNoIPDecision%TOS" : 0, "Hlt1SingleMuonHighPTDecision%TOS" : 0} 
    Hlt1Upsilon.TisTosSpecs = triggers
    Hlt1Upsilon.ProjectTracksToCalo = False
    Hlt1Upsilon.CaloClustForCharged = False
    Hlt1Upsilon.CaloClustForNeutral = False
    Hlt1Upsilon.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt1Upsilon.NoRegex = True
    #Hlt1Upsilon.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt1Upsilon", Algorithm = Hlt1Upsilon, RequiredSelections = [ longPartsFilter ])
# ################################################################
def selHlt1Z(name, longPartsFilter, triggers):
    """
    Filter the long track muon to be TOS on a HLT1 single muon trigger, for Z selection
    """
    Hlt1Z = TisTosParticleTagger(name+"Hlt1Z")
    #Hlt1Z.TisTosSpecs = { "Hlt1SingleMuonHighPTDecision%TOS" : 0}
    Hlt1Z.TisTosSpecs = triggers
    Hlt1Z.ProjectTracksToCalo = False
    Hlt1Z.CaloClustForCharged = False
    Hlt1Z.CaloClustForNeutral = False
    Hlt1Z.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt1Z.NoRegex = True
    #Hlt1Z.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt1Z", Algorithm = Hlt1Z, RequiredSelections = [ longPartsFilter ])
# ################################################################

# ########################################################################################
# HLT 2 lines we run on
# ########################################################################################
def selHlt2Jpsi(name, hlt1Filter, triggers):
    """
    Filter the long track muon to be TOS on a HLT2 single muon trigger, for J/psi selection
    """
    Hlt2Jpsi = TisTosParticleTagger(name+"Hlt2Jpsi")
    #Hlt2Jpsi.TisTosSpecs = { "Hlt2SingleMuon.*Decision%TOS" : 0} # take all HLT2SingleMuon triggers
    Hlt2Jpsi.TisTosSpecs = triggers
    Hlt2Jpsi.ProjectTracksToCalo = False
    Hlt2Jpsi.CaloClustForCharged = False
    Hlt2Jpsi.CaloClustForNeutral = False
    Hlt2Jpsi.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2Jpsi.NoRegex = False
    #Hlt2Jpsi.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt2Jpsi", Algorithm = Hlt2Jpsi, RequiredSelections = [ hlt1Filter ])
# ################################################################
def selHlt2Upsilon(name, hlt1Filter, triggers):
    """
    Filter the long track muon to be TOS on a HLT2 single muon trigger, for Upsilon selection
    """
    Hlt2Upsilon = TisTosParticleTagger(name+"Hlt2Upsilon")
    #Hlt2Upsilon.TisTosSpecs = { "Hlt2SingleMuonLowPTDecision%TOS" : 0} 
    Hlt2Upsilon.TisTosSpecs = triggers
    Hlt2Upsilon.ProjectTracksToCalo = False
    Hlt2Upsilon.CaloClustForCharged = False
    Hlt2Upsilon.CaloClustForNeutral = False
    Hlt2Upsilon.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2Upsilon.NoRegex = True
    #Hlt2Upsilon.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt2Upsilon", Algorithm = Hlt2Upsilon, RequiredSelections = [ hlt1Filter ])
# ################################################################
def selHlt2Z(name, hlt1Filter, triggers):
    """
    Filter the long track muon to be TOS on a HLT2 single muon trigger, for Z selection
    """
    Hlt2Z = TisTosParticleTagger(name+"Hlt2Z")
    #Hlt2Z.TisTosSpecs = { "Hlt2SingleMuonHighPTDecision%TOS" : 0}
    Hlt2Z.TisTosSpecs = triggers
    Hlt2Z.ProjectTracksToCalo = False
    Hlt2Z.CaloClustForCharged = False
    Hlt2Z.CaloClustForNeutral = False
    Hlt2Z.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2Z.NoRegex = True
    #Hlt2Z.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt2Z", Algorithm = Hlt2Z, RequiredSelections = [ hlt1Filter ])
# ################################################################
def selHlt2BJpsiKMu(name, hlt1Filter, triggers):
    """
    Filter the long track muon to be TUS on a HLT2 mu+track trigger, for B->J/psi K selection (mainly a speed up)
    """
    Hlt2BJpsiKMu = TisTosParticleTagger(name+"Hlt2BJpsiKMu")
    #Hlt2BJpsiKMu.TisTosSpecs = { "Hlt2MuTrackDecision%TUS" : 0} # TUS, not TOS, not a typo!!!
    Hlt2BJpsiKMu.TisTosSpecs = triggers
    Hlt2BJpsiKMu.ProjectTracksToCalo = False
    Hlt2BJpsiKMu.CaloClustForCharged = False
    Hlt2BJpsiKMu.CaloClustForNeutral = False
    Hlt2BJpsiKMu.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2BJpsiKMu.NoRegex = True
    #Hlt2BJpsiKMu.PassOnAll = True # TESTING!
#
    return Selection(name+"_SelHlt2BJpsiKMu", Algorithm = Hlt2BJpsiKMu, RequiredSelections = [ hlt1Filter ])
# ################################################################
def selHlt2BJpsiKK(name, longPartsFilter, triggers): # No HLT1 needed, as event in HLT1 already triggered with single muon
    """
    Filter the long track Kaon to be TUS on a HLT2 mu+track trigger, for B->J/psi K selection (mainly a speed up)
    """
    Hlt2BJpsiKK = TisTosParticleTagger(name+"Hlt2BJpsiKK")
    #Hlt2BJpsiKK.TisTosSpecs = { "Hlt2MuTrackDecision%TUS" : 0}  # TUS, not TOS, not a typo!!!
    Hlt2BJpsiKK.TisTosSpecs = triggers
    Hlt2BJpsiKK.ProjectTracksToCalo = False
    Hlt2BJpsiKK.CaloClustForCharged = False
    Hlt2BJpsiKK.CaloClustForNeutral = False
    Hlt2BJpsiKK.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2BJpsiKK.NoRegex = True
    #Hlt2BJpsiKK.PassOnAll = True # TESTING!
    #
    return Selection(name+"_SelHlt2BJpsiKK", Algorithm = Hlt2BJpsiKK, RequiredSelections = [ longPartsFilter ])
# ########################################################################################
        
# ########################################################################################
# -->
# --> FILTERS AND RESONANCE MAKERS <--
# -->
# ########################################################################################


# ########################################################################################
# Charge filter, that filters, well, the charge and takes the particles from the right source (long or muonTT)
# ########################################################################################
def chargeFilter(name, trackAlgo,  partSource, charge):
    """
    Select plus or minus charge for muonTT or long track
    """
    Filter = FilterDesktop() #there is maybe a change needed
    myFilter1 = Filter.configurable(name+"chargeFilter1")
        
    if(charge == -1):
        myFilter1.Code = "(Q < 0)"
    if(charge == 1):
        myFilter1.Code = "(Q > 0)"    
        
    if(trackAlgo == 'MuonTT'):
        return Selection( name+'_chargeFilter'+'MuonTT', Algorithm = myFilter1, RequiredSelections = [  partSource ] )
    if(trackAlgo == 'LongMu'):
        return Selection( name+'_chargeFilter'+'LongMu', Algorithm = myFilter1, RequiredSelections = [  partSource ] )
# ################################################################


# ########################################################################################
# Resonance maker, that fits two muons to a resonance (J/psi, Upsilon, Z)
# ########################################################################################    
def makeResonanceMuMuTrackEff(name, resonanceName, decayDescriptor, plusCharge, minusCharge, 
                              mode, massWin, vertexChi2, resonancePT, muonTTPT, longPT, longMuonPID, longMuonMinIP, longMuonTrackCHI2, motherMinIP):    
    """
    Create and return a Resonance -> mu mu Selection object, with one track a long track
    and the other a MuonTT track.
    Arguments:
    name                 : name of the selection
    resonanceName        : name of the resonance
    decayDescriptor      : decayDescriptor of the decay
    plusCharge           : algorithm for selection positvely charged tracks
    minusCharge          : algorithm for selection negatively charged tracks
    mode                 : Tag(-)-and-Probe(+) (1) or  Tag(+)-and-Probe(-) (2)
    massWin              : mass window around J/psi mass
    VertexChi2           : vertex chi2 / ndof of mu mu vertex
    resonancePT          : Pt of the resonance particle (f.ex. J/psi)
    muonTTPT             : Pt of MuonTT-track muon
    longPT               : Pt of Long-track muon
    longMuonPID          : CombDLL(mu-pi) of long-track muon
    motherMinIP          : Minimal IP of the mother
    NOT IMPLEMENTED YET:
    longMuonMinIPCHI2    : MinIPCHI2 of long-track muon
    longMuonTrackCHI2    : Track-Chi2 of long-track muon
    """
    
    massWinCombCut = 2 * massWin
    
    Combine = CombineParticles()
    MuonTTResonance = Combine.configurable(name+"MuonTTResonance")
    MuonTTResonance.DecayDescriptor = decayDescriptor

    MuonTTResonance.OutputLevel = 4 

    if(mode == 1):
        MuonTTResonance.DaughtersCuts = {"mu+"	:	"PT > %(muonTTPT)s"  % locals(),
                                         "mu-"	:	"(PT > %(longPT)s) & (PIDmu > %(longMuonPID)s)"  % locals() }

        MuonTTResonance.CombinationCut = "ADAMASS('%(resonanceName)s') < %(massWinCombCut)s" % locals()
        MuonTTResonance.MotherCut = "(ADMASS('%(resonanceName)s') < %(massWin)s) & (VFASPF(VCHI2/VDOF) < %(vertexChi2)s) & (MIPDV(PRIMARY) < %(motherMinIP)s)" % locals()
    
        return Selection( name, Algorithm = MuonTTResonance, RequiredSelections = [minusCharge, plusCharge] )
     
    if(mode == 2):
        MuonTTResonance.DaughtersCuts = {"mu-"	:	"PT > %(muonTTPT)s"  % locals(),
                                         "mu+"	:	"(PT > %(longPT)s) & (PIDmu > %(longMuonPID)s)"  % locals() }

        MuonTTResonance.CombinationCut = "ADAMASS('%(resonanceName)s') < %(massWinCombCut)s" % locals()
        MuonTTResonance.MotherCut = "(ADMASS('%(resonanceName)s') < %(massWin)s) & (VFASPF(VCHI2/VDOF) < %(vertexChi2)s) & (MIPDV(PRIMARY) < %(motherMinIP)s)" % locals()
        
        return Selection( name, Algorithm = MuonTTResonance, RequiredSelections = [plusCharge, minusCharge] )
    
    
# ################################################################

# ########################################################################################
# Make a B->J/psi K out of a J/psi and a K
# ########################################################################################
def makeBJpsiKTrackEff(name, JpsiSel, KSel, massWin, vertexChi2): 
    """
    Create and return a B -> J/psi K Selection object, with a J/psi (long+muonTT) and a Kaon.

    Arguments:
    name                 : name of the selection
    massWin              : width of the mass window
    vertexChi2           : vertexChi2/ndof for vertex
    """

    massWinCombCut = 2 * massWin
    
    Combine = CombineParticles()
    MuonTTBJpsiK = Combine.configurable(name+"BJpsiK")
    MuonTTBJpsiK.DecayDescriptor = "[B+ -> J/psi(1S) K+]cc"

    MuonTTBJpsiK.OutputLevel = 4 
    
    MuonTTBJpsiK.DaughtersCuts = {"J/psi(1S)"	:	"DMASS('J/psi(1S)') < 500" ,
                                  "K+"          :	"ALL" }
             
    MuonTTBJpsiK.CombinationCut = "ADAMASS('B+') < %(massWinCombCut)s" % locals()
    MuonTTBJpsiK.MotherCut = "(ADMASS('B+') < %(massWin)s) & (VFASPF(VCHI2/VDOF) < %(vertexChi2)s) & (BPVDIRA > 0.95)" % locals()

    return Selection( name, Algorithm = MuonTTBJpsiK, RequiredSelections = [ KSel, JpsiSel ] )
# ################################################################

# ########################################################################################
# Filter the B->J/psi K to be triggered on the long muon and the Kaon
# ########################################################################################
def filterHLT2ForBJpsiK(name, BJpsiKSel, triggers):
    """
    Filter B->J/psi K to be triggered on mu+track for the long track muon and Kaon
    """
    Hlt2BJpsiK = TisTosParticleTagger(name+"_Hlt2BJpsiK")
    #Hlt2BJpsiK.TisTosSpecs = { "Hlt2MuTrackDecision%TOS" : 0}
    Hlt2BJpsiK.TisTosSpecs = triggers
    Hlt2BJpsiK.ProjectTracksToCalo = False
    Hlt2BJpsiK.CaloClustForCharged = False
    Hlt2BJpsiK.CaloClustForNeutral = False
    Hlt2BJpsiK.TOSFrac = { 4:0.0, 5:0.0 }
    Hlt2BJpsiK.NoRegex = True
    #Hlt2BJpsiK.PassOnAll = True
    return Selection( name, Algorithm = Hlt2BJpsiK, RequiredSelections = [BJpsiKSel] )
# ########################################################################################





