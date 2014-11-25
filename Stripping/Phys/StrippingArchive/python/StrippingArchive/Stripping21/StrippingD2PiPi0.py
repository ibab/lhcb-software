'''
D+ -> h+ (pi0 -> e+ e- gamma) prompt and semileptonic lines (h+ is pi/K)
D+ -> h+ (eta -> pi+ pi- gamma) prompt lines (h+ is pi/K)
D+ -> h+ (eta-prime -> pi+ pi- gamma)prompt lines (h+ is pi/K)
'''
__author__ = ['Ed Greening', 'Brian Meadows']
__date__ = '23/08/2013'
__version__ = '$Revision: 2.0 $'

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV, micrometer
from LHCbKernel.Configuration import *
from GaudiKernel.PhysicalConstants import c_light
from StandardParticles import StdLoosePions, StdLooseKaons, StdLooseAllPhotons, StdLooseMuons
#from CommonParticles.StdLooseDiElectron import StdDiElectronFromTracks # cannot get this to work, import another way using AutomaticData

__all__=('default_config', 'D2PiPi0Conf')

default_config = { 
    'PromptHPi0' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 350  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 70   *MeV
        ,'ResMass_MAX'       : 210  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'PromptHEta' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 600  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 500   *MeV
        ,'ResMass_MAX'       : 600  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'PromptHEtaPrime' : {
        'Daug_IPCHI2_MIN'    : 25         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 1000  *MeV
        ,'Daug_P_MIN'        : 1000 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 5          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 900  *MeV
        ,'ResMass_MAX'       : 990  *MeV
        ,'D_PT_Min'          : 2000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 5          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 5          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       :-1          # when kaon in the decay 
        },
    'SLHPi0' : {
        'Daug_IPCHI2_MIN'    : 9         # computes the chi^2 distance of track to PV, 9 is 3 sigma from PV, 16 is 4 sigma. Notice the pattern?
        ,'Daug_PT_MIN'       : 150  *MeV
        ,'Daug_P_MIN'        : 500 *MeV  # ensures good range for RICH
        ,'Daug_TRCHI2DOF_MAX': 10          # to ensure good track quality
        ,'TRGHOSTPROB_MAX'   : 0.5
        ,'ResMass_MIN'       : 70   *MeV
        ,'ResMass_MAX'       : 210  *MeV
        ,'D_PT_Min'          : 1000 *MeV
        ,'DMass_MIN'         : 1600 *MeV
        ,'DMass_MAX'         : 2500 *MeV
        ,'VFASPF_MAX'        : 15          # computes the chi^2 on the end vertex of the particle
        ,'BPVLTIME_MIN'      : 50 *micrometer
        ,'DTF_CHI2NDOF_MAX'  : 10          # computes the chi^2 of the fit
        ,'KaonPID_MIN'       : -1          # when kaon in the decay
        ,'MuonPID_MIN'       : 0
        ,'BMass_MIN'         : 2200 *MeV
        ,'BMass_MAX'         : 6300 *MeV
        }
}

class D2PiPi0Conf(LineBuilder) :
    __configuration_keys__ = default_config.keys()

    def __init__( self, name, config ) :
        
        self._name = name
        self._config=config
        
        LineBuilder.__init__( self, self._name, self._config )

        # Make gamma, pion, muon and electrons
        self.pion = Selection( "PionFor{0}".format(name),
                               Algorithm = self._pionFilter(),
                               RequiredSelections = [StdLoosePions] )
        
        self.gamma = Selection( "GammaFor{0}".format(name),
                                Algorithm = self._gammaFilter(),
                                RequiredSelections = [StdLooseAllPhotons] )

        self.highPTgamma = Selection( "highPTGammaFor{0}".format(name),
                                Algorithm = self._highPTgammaFilter(),
                                RequiredSelections = [self.gamma] )

        self.veryhighPTgamma = Selection( "veryhighPTGammaFor{0}".format(name),
                                Algorithm = self._veryhighPTgammaFilter(),
                                RequiredSelections = [self.highPTgamma] )

        self.kaon = Selection( "KaonFor{0}".format(name),
                                Algorithm = self._kaonFilter(),
                                RequiredSelections = [StdLooseKaons] )
        
        self.muon = Selection( "Muon{0}".format(name),
                                Algorithm = self._muonFilter(),
                                RequiredSelections = [StdLooseMuons] )

        self.dielectrons = DataOnDemand(Location = "Phys/StdDiElectronFromTracks/Particles" )
                                
        # Make selections for decay
        SetConfig = config['PromptHPi0']
        PiPi0PromptSelection = self.makePromptCandidate( self._name + 'PiPi0PromptSelection',
                                                         '[D+ -> pi+ pi0]cc',
                                                         self.pion,
                                                         self.makeRes( 'Pi0+PiFor{0}'.format(name),
                                                                       'pi0 -> J/psi(1S) gamma',
                                                                       self.gamma,
                                                                       self.dielectrons,
                                                                       SetConfig ),
                                                         SetConfig )
        KPi0PromptSelection = self.makePromptCandidate( self._name + 'KPi0PromptSelection',
                                                        '[D+ -> K+ pi0]cc',
                                                        self.kaon,
                                                        self.makeRes( 'Pi0+KFor{0}'.format(name),
                                                                      'pi0 -> J/psi(1S) gamma',
                                                                      self.gamma,
                                                                      self.dielectrons,
                                                                      SetConfig ),
                                                        SetConfig )
        SetConfig = config['PromptHEta']
        PiEtaPromptSelection = self.makePromptCandidate( self._name + 'PiEtaPromptSelection',
                                                         '[D+ -> pi+ eta]cc',
                                                         self.pion,
                                                         self.makeRes( 'Eta+PiFor{0}'.format(name),
                                                                       'eta -> pi+ pi- gamma',
                                                                       self.highPTgamma,
                                                                       self.pion,
                                                                       SetConfig ),
                                                         SetConfig )
        KEtaPromptSelection = self.makePromptCandidate( self._name + 'KEtaPromptSelection',
                                                        '[D+ -> K+ eta]cc',
                                                        self.kaon,
                                                        self.makeRes( 'Eta+KFor{0}'.format(name),
                                                                      'eta -> pi+ pi- gamma',
                                                                      self.highPTgamma,
                                                                      self.pion,
                                                                      SetConfig ),
                                                        SetConfig )
        SetConfig = config['PromptHEtaPrime']
        PiEtaPrimePromptSelection = self.makePromptCandidate( self._name + 'PiEtaPrimePromptSelection',
                                                         '[D+ -> pi+ eta]cc',
                                                         self.pion,
                                                         self.makeRes( 'EtaPrime+PiFor{0}'.format(name),
                                                                       'eta -> pi+ pi- gamma',
                                                                       self.veryhighPTgamma,
                                                                       self.pion,
                                                                       SetConfig ),
                                                         SetConfig )
        KEtaPrimePromptSelection = self.makePromptCandidate( self._name + 'KEtaPrimePromptSelection',
                                                        '[D+ -> K+ eta]cc',
                                                        self.kaon,
                                                        self.makeRes( 'EtaPrime+KFor{0}'.format(name),
                                                                      'eta -> pi+ pi- gamma',
                                                                      self.veryhighPTgamma,
                                                                      self.pion,
                                                                      SetConfig ),
                                                        SetConfig )
        # For the SL mode (D2PiPi0 only)
        SetConfig = config['SLHPi0']
        PiPi0SLSelection = self.makePromptCandidate( self._name + 'PiPi0SLSelection',
                                                         '[D+ -> pi+ pi0]cc',
                                                         self.pion,
                                                         self.makeRes( 'SLPi0+PiFor{0}'.format(name),
                                                                       'pi0 -> J/psi(1S) gamma',
                                                                       self.gamma,
                                                                       self.dielectrons,
                                                                       SetConfig ),
                                                         SetConfig )
        SemiLeptonicSelection = self.makeSemiLeptonicCandidate( self._name + 'SemiLeptonicSelection',
                                                                PiPi0SLSelection, #D candidates from prompt line
                                                                self.muon,
                                                                SetConfig )
        # Create stripping lines
        PiPi0PromptLine      = StrippingLine( self._name + 'PiPi0PromptLine',
                                              selection = PiPi0PromptSelection )
        KPi0PromptLine       = StrippingLine( self._name + 'KPi0PromptLine',
                                             selection = KPi0PromptSelection )
        PiEtaPromptLine      = StrippingLine( self._name + 'PiEtaPromptLine',
                                              selection = PiEtaPromptSelection )
        KEtaPromptLine       = StrippingLine( self._name + 'KEtaPromptLine',
                                             selection = KEtaPromptSelection )
        PiEtaPrimePromptLine = StrippingLine( self._name + 'PiEtaPrimePromptLine',
                                              selection = PiEtaPrimePromptSelection )
        KEtaPrimePromptLine  = StrippingLine( self._name + 'KEtaPrimePromptLine',
                                             selection = KEtaPrimePromptSelection )
        SemiLeptonicLine     = StrippingLine( self._name + 'SemiLeptonicLine',
                                               selection = SemiLeptonicSelection )
        
        # Register lines
        self.registerLine( PiPi0PromptLine )
        self.registerLine( KPi0PromptLine )
        self.registerLine( PiEtaPromptLine )
        self.registerLine( KEtaPromptLine )
        self.registerLine( PiEtaPrimePromptLine )
        self.registerLine( KEtaPrimePromptLine )
        self.registerLine( SemiLeptonicLine )
   
 
    def _defaultTrackCuts( self ):
        return cut_string(["P > {0[Daug_P_MIN]}",
                           "PT > {0[Daug_PT_MIN]}",
                           "TRCHI2DOF < {0[Daug_TRCHI2DOF_MAX]}",
                           "MIPCHI2DV(PRIMARY) > {0[Daug_IPCHI2_MIN]}"],
                          self._config['PromptHPi0'])

    def _defaultHadronTrackCuts( self ):
        return self._defaultTrackCuts() + "&" + cut_string( ["HASRICH","in_range( 2, ETA, 5)","TRGHOSTPROB < {0[TRGHOSTPROB_MAX]}"],self._config['PromptHPi0'] )
    
    def _pionFilter( self ):
        _code = self._defaultHadronTrackCuts()
        return FilterDesktop( Code=_code )

    def _kaonFilter( self ):
        _code = self._defaultHadronTrackCuts() + "&" + cut_string( ["PIDK-PIDpi > {0[KaonPID_MIN]}"],self._config['PromptHPi0'] )
        return FilterDesktop( Code=_code )
   
    def _gammaFilter( self ):
        _code = self._defaultTrackCuts()
        return FilterDesktop( Code=_code )

    def _highPTgammaFilter( self ):
        _code = cut_string( [ "PT > {0[Daug_PT_MIN]}"], self._config['PromptHEta'] )
        return FilterDesktop( Code=_code )
    
    def _veryhighPTgammaFilter( self ):
        _code = cut_string( [ "PT > {0[Daug_PT_MIN]}"], self._config['PromptHEtaPrime'] )
        return FilterDesktop( Code=_code )

    def _muonFilter( self ):
        _code = self._defaultTrackCuts() + "&" + cut_string( ["PIDmu-PIDpi > {0[MuonPID_MIN]}"],self._config['SLHPi0'] )
        return FilterDesktop( Code=_code )

    def makeRes( self, name, decay, daughter1, daughter2, config ) :
        _combCut = cut_string( ["in_range( {0[ResMass_MIN]},AM,{0[ResMass_MAX]} )"],config )
        _combineRes = CombineParticles( DecayDescriptor = decay,
                                        CombinationCut = _combCut,
                                        MotherCut      = 'ALL' )
        return Selection ( "Resfor"+name,
                           Algorithm = _combineRes,
                           RequiredSelections = [ daughter1, daughter2 ] )
    
    def makePromptCandidate( self, name, decay, daughter1, daughter2, config ) :
        _DpComb       = cut_string( ["APT > {0[D_PT_Min]}","in_range( {0[DMass_MIN]},AM,{0[DMass_MAX]} )"],config )
        _DpMother     = cut_string( ["VFASPF(VCHI2PDOF) < {0[VFASPF_MAX]}","(BPVLTIME(25) * c_light) > {0[BPVLTIME_MIN]}", "DTF_CHI2NDOF(True) < {0[DTF_CHI2NDOF_MAX]}"],config )
        _combinePrompt = CombineParticles( DecayDescriptor = decay
                                           ,CombinationCut = _DpComb
                                           ,MotherCut      = _DpMother )
        return Selection ( "Dpfor"+name,
                           Algorithm = _combinePrompt,
                           RequiredSelections = [ daughter1, daughter2 ] )
    
    def makeSemiLeptonicCandidate( self, name, Dp, muon, config ) :

        _B0Comb   = "in_range( %(BMass_MIN)s,AM,%(BMass_MAX)s )" %config
        _B0Mother = "((BPVLTIME(25) * c_light) > %(BPVLTIME_MIN)s)" \
            "& (DTF_CHI2NDOF(True) < %(DTF_CHI2NDOF_MAX)s)" %config #No VFASPF (end vertex) quality cut as losing the neutrino reduces this

        _combine = CombineParticles( DecayDescriptors =  ['[B0 -> D+ mu-]cc', '[B+ -> D+ mu+]cc']
                                     ,CombinationCut = _B0Comb
                                     ,MotherCut      = _B0Mother      
                                     ,WriteP2PVRelations = False
                                     )
        
        return Selection ( "B0for"+name,
                           Algorithm = _combine
                           ,RequiredSelections = [ Dp, muon ]
                           )
    
def cut_string(cuts, config):
    return '({0})'.format(') & ('.join(cuts).format(config))
    
    
