'''
D+ -> pi+ (pi0->e+ e- gamma) lines, prompt and smmileptonic                                                                                                                                                                           '''

__author__ = ['Ed Greening', 'Brian Meadows']
__date__ = '30/03/2013'
__version__ = '$Revision: 1.0 $'


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles, FilterDesktop
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.SystemOfUnits import MeV, micrometer
from LHCbKernel.Configuration import *
from GaudiKernel.PhysicalConstants import c_light


confdict_D2PiPi0 = { 'Daug_IPCHI2_MIN'    : 9         #NOT USED as doesn't seem to be helpful in MC study, computes the chi^2 distance of track to PV, 9 is 3 sigma from PV
                     ,'Daug_PT_MIN'       : 150  *MeV
                     ,'Daug_P_MIN'        : 1000 *MeV  #ensures good range for RICH
                     ,'Daug_TRCHI2DOF_MAX': 5          #to ensure good track quality
                     ,'TRGHOSTPROB_MAX'   : 0.5
                     ,'ElectronPID_MIN'   : 1
                     ,'Comb_PT_Min'       : 2000 *MeV
                     ,'DielectonMass_MAX' : 200  *MeV  #to ensure that dielectrons don't have mass exceeding pi0
                     ,'CombMass_MIN'      : 70   *MeV
                     ,'CombMass_MAX'      : 200  *MeV
                     ,'DMass_MIN'         : 1700 *MeV
                     ,'DMass_MAX'         : 2100 *MeV
                     ,'VFASPF_MAX'        : 5          #computes the chi^2 on the end vertex of the particle
                     ,'BPVLTIME_MIN'      : 50 *micrometer
                     ,'DTF_CHI2NDOF_MAX'  : 5          #computes the chi^2 of the fit
                     ,'MuonPID_MIN'       : 1
                     ,'BMass_MIN'         : 2200 *MeV
                     ,'BMass_MAX'         : 6300 *MeV
                     }

default_name = 'D2PiPi0'

class D2PiPi0Conf(LineBuilder) :
    __configuration_keys__ = ( 'Daug_IPCHI2_MIN'
                               ,'Daug_PT_MIN'
                               ,'Daug_P_MIN'
                               ,'Daug_TRCHI2DOF_MAX'
                               ,'TRGHOSTPROB_MAX'
                               ,'ElectronPID_MIN'
                               ,'Comb_PT_Min'
                               ,'DielectonMass_MAX'
                               ,'CombMass_MIN'
                               ,'CombMass_MAX'
                               ,'DMass_MIN'
                               ,'DMass_MAX'
                               ,'VFASPF_MAX'
                               ,'BPVLTIME_MIN'
                               ,'DTF_CHI2NDOF_MAX'
                               ,'MuonPID_MIN'
                               ,'BMass_MIN'
                               ,'BMass_MAX'
                               )
    
    def __init__( self, name, config ) :
        LineBuilder.__init__( self, name, config )
        self._myname = name

        #Make prompt candidates
        _daughterCut  = "(P > %(Daug_P_MIN)s)" \
                        "& (PT > %(Daug_PT_MIN)s)" \
                        "& (TRCHI2DOF < %(Daug_TRCHI2DOF_MAX)s)" %config
                        #"& (MIPCHI2DV(PRIMARY) > %(Daug_IPCHI2_MIN)s)" %config
        _electronsCut = _daughterCut + "& (PIDe-PIDpi > %(ElectronPID_MIN)s)" %config
        _gammaCut     = _daughterCut
        _pipCut       = _daughterCut + "& (HASRICH)& (in_range( 2, ETA, 5))& (TRGHOSTPROB < %(TRGHOSTPROB_MAX)s)" %config
        _DpComb       = "(APT > %(Comb_PT_Min)s)" \
                        "& (in_range( %(DMass_MIN)s,AM,%(DMass_MAX)s ))" \
                        "& (AM23 < %(DielectonMass_MAX)s)" \
                        "& (in_range( %(CombMass_MIN)s,M234,%(CombMass_MAX)s ))" %config
        _DpMother     = "(VFASPF(VCHI2PDOF) < %(VFASPF_MAX)s)" \
                        "& (BPVLTIME(25) * c_light > %(BPVLTIME_MIN)s)" \
                        "& (DTF_CHI2NDOF(True) < %(DTF_CHI2NDOF_MAX)s)" %config

        self.PromptSelection = makePromptCandidate( self._myname + 'PromptSelection',
                                                    _electronsCut, #cuts
                                                    _gammaCut,
                                                    _pipCut,
                                                    _DpComb,
                                                    _DpMother )
        self.PromptLine      = StrippingLine( self._myname + 'PromptLine',
                                              selection = self.PromptSelection )
        self.registerLine( self.PromptLine )
        
        
        #Make semileptonic candidates
        _muonCut  = _daughterCut + "& (PIDmu-PIDpi > %(MuonPID_MIN)s)" %config
        _DpCut    = _daughterCut
        _B0Comb   = "in_range( %(BMass_MIN)s,AM,%(BMass_MAX)s )" %config
        _B0Mother = "(BPVLTIME(25) * c_light > %(BPVLTIME_MIN)s)" \
                    "& (DTF_CHI2NDOF(True) < %(DTF_CHI2NDOF_MAX)s)" %config #No VFASPF (end vertex) quality cut as losing the neutrino reduces this

        self.SemiLeptonicSelection = makeSemiLeptonicCandidate( self._myname + 'SemiLeptonicSelection',
                                                                self.PromptSelection, #D candidates from prompt line
                                                                _muonCut,             #cuts
                                                                _DpCut,
                                                                _B0Comb,
                                                                _B0Mother )
        self.SemiLeptonicLine      = StrippingLine( self._myname + 'SemiLeptonicLine'
                                                    ,selection = self.SemiLeptonicSelection )
        self.registerLine( self.SemiLeptonicLine )


def makePromptCandidate( name, _electronsCut, _gammaCut, _pipCut, _DpComb, _DpMother ) :
    #Filter with defined cuts
    _electronFilter = FilterDesktop(Code=_electronsCut)
    _gammaFilter = FilterDesktop(Code=_gammaCut)
    _pipFilter = FilterDesktop(Code=_pipCut)
    
    #Load common particles
    _stdelectron = DataOnDemand( Location="Phys/StdLooseElectrons/Particles" )
    _stdgamma    = DataOnDemand( Location="Phys/StdVeryLooseAllPhotons/Particles" )
    _stdpip      = DataOnDemand( Location="Phys/StdLoosePions/Particles" )
    
    #Apply cuts to common particles
    _electron = Selection( "Electronfor"+name, Algorithm=_electronFilter, RequiredSelections=[_stdelectron] )
    _gamma = Selection( "Gammafor"+name, Algorithm=_gammaFilter, RequiredSelections=[_stdgamma] )
    _pip = Selection( "PiPlusfor"+name, Algorithm=_pipFilter, RequiredSelections=[_stdpip] )
    
    #Define shortcut to tidy up code
    _preambulo = ["M234 = LoKi.AParticles.InvariantMass(2,3,4)"]
    
    #Combiner
    _combine = CombineParticles( DecayDescriptor = '[D+ -> pi+ e+ e- gamma]cc'
                                 ,CombinationCut = _DpComb
                                 ,MotherCut      = _DpMother
                                 ,WriteP2PVRelations = False
                                 ,Preambulo      = _preambulo
                                 )
    #Return candidates
    return Selection ( "Dpfor"+name,
                       Algorithm = _combine
                       ,RequiredSelections = [ _pip, _electron, _gamma ]
                       )



def makeSemiLeptonicCandidate( name, _Dp, _muonCut, _DpCut, _B0Comb, _B0Mother ) :
    #Filter with defined cuts
    _muonFilter = FilterDesktop(Code=_muonCut)
    
    #Load common particles
    _stdmuon = DataOnDemand( Location="Phys/StdLooseMuons/Particles" )
    
    #Apply cuts to common particles
    _muon = Selection( "Muonfor"+name, Algorithm=_muonFilter, RequiredSelections=[_stdmuon] )
   
    #Combiner
    _combine = CombineParticles( DecayDescriptors =  ['[B0 -> D+ mu-]cc', '[B+ -> D+ mu+]cc']
                                 ,CombinationCut = _B0Comb
                                 ,MotherCut      = _B0Mother
                                 ,WriteP2PVRelations = False
                                 )
    
    #Return candidates
    return Selection ( "B0for"+name,
                       Algorithm = _combine
                       ,RequiredSelections = [ _Dp, _muon ]
                       )

 
    






