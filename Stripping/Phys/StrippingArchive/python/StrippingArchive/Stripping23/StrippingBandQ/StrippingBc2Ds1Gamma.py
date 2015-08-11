"""
Module for construction of Radiative Decays Stripping Selections and StrippingLines.
Provides functions to build Bc, D0, Ds1 selections.
Provides class Bc2Ds1GammaConf, which constructs the Selections and
StrippingLines given a configuration dictionary.
Exported selection makers: 'makePhoton', 'makeD0', 'makeDs1', 'makeBc2Ds1Gamma',

"""

__author__ = ['Marco Pappagallo']
__date__ = '02/07/2014'
__version__ = '$Revision: 1.0 $'

__all__ = ('Bc2Ds1GammaConf',
           'makePhoton',
           'makeD0',
           'makeDs1',
           'makeDs1WS',
           'makeBc2Ds1Gamma',
           'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

#default_name = 'Bc2Ds1Gamma'
default_config = {
    'NAME'              : 'Bc2Ds1Gamma',
    'BUILDERTYPE'       : 'Bc2Ds1GammaConf',
    'CONFIG'    : {
                  #'TrIPchi2'            : 4.        # Dimensionless Already implemented in the particle lists
#                   'BcMassWin'           : 1500.     # MeV
                   'MinBcMass'           : 4800.     # MeV
                  ,'MaxBcMass'           : 7500.     # MeV  
                  ,'BcPVIPchi2'          : 16.       # Dimensionless
                  ,'photonPT'            : 1500.     # MeV
                  ,'KaonProbNN'          : 0.1       # Dimensionless
                  ,'pionProbNN'          : 0.1       # Dimensionless
                  ,'TrGhostProb'         : 0.5       # Dimensionless
                  ,'Ds1DeltaMassWin'     : 650       # MeV
                  ,'MaxDs1VertChi2DOF'   : 10        # Dimensionless
                  ,'Bc_PT'               : 1000      # MeV
                  ,'CTAU_Bc'             : 75        # microMeter
                  # Pre- and postscales
                  ,'Bc2Ds1GammaPreScale'               : 1.0
                  ,'Bc2Ds1GammaPostScale'              : 1.0
                  },
    'STREAMS' : [ 'Radiative' ],
    'WGs'    : [ 'BandQ' ]
    }

class Bc2Ds1GammaConf(LineBuilder):
    """
    Definition of Bc -> Ds1 Gamma stripping
    
    Constructs Bc -> Ds1 Gamma Selections and StrippingLines from
    a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> gammaConf = StrippingB2XGammaConf('StrippingB2XGammaTest',config)
    >>> gammaLines = gammaConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selPhoton                    : Photon Selection object
    selD0                        : nominal D0 -> K pi Selection object
    selDs1                       : nominal Ds1 -> D0 K Selection object 
    selDs1WS                     : nominal Ds1 -> D~0 K Selection object
    selBc2Ds1Gamma               : Bc -> Ds1 Gamma Selection object 
    selBc2Ds1GammaWS             : Bc -> Ds1 Gamma Selection object
    lines                  : List of lines

    Exports as class data member:
    StrippingB2XGammaConf.__configuration_keys__ : List of required configuration parameters.    
    """
    __configuration_keys__ = (#'BcMassWin'
                              'MinBcMass'
                              ,'MaxBcMass'
                              ,'BcPVIPchi2'
                              ,'photonPT'
                              ,'KaonProbNN'
                              ,'pionProbNN'
                              ,'TrGhostProb'
                              ,'Ds1DeltaMassWin'
                              ,'MaxDs1VertChi2DOF'
                              ,'Bc_PT'
                              ,'CTAU_Bc'
                              # Pre- and postscales
                              ,'Bc2Ds1GammaPreScale'
                              ,'Bc2Ds1GammaPostScale'
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        # if name not set outside, set it to empty
        #if name == None:
        #    name = ""   

        # Selection of Bc daughters: photon, D0, Ds1
        self.selPhoton = makePhoton('GammaFor%s' % name,
                                    config['photonPT'])

        self.selD0  = makeD0('D0For%s' % name,
                             config['KaonProbNN'],
                             config['pionProbNN'],
                             config['TrGhostProb'])

        self.selDs1 = makeDs1('Ds1For%s' % name,
                              self.selD0,
                              '[D_s1(2536)+ -> D0 K+]cc', #DecayDescriptor
                              config['KaonProbNN'],
                              config['TrGhostProb'],
                              config['Ds1DeltaMassWin'],
                              config['MaxDs1VertChi2DOF'])

        self.selDs1WS = makeDs1('Ds1WSFor%s' % name,
                              self.selD0,
                              '[D_s1(2536)+ -> D~0 K+]cc', #DecayDescriptor
                              config['KaonProbNN'],
                              config['TrGhostProb'],
                              config['Ds1DeltaMassWin'],
                              config['MaxDs1VertChi2DOF'])

        # Bc->Ds1 Gamma selections
        self.selBc2Ds1Gamma = makeBc2Ds1Gamma('Bc2Ds1GammaFor%s' % name,
                                              self.selDs1,
                                              self.selPhoton,
                                              config['BcPVIPchi2'],
#                                              config['BcMassWin'],
                                              config['MinBcMass'],
                                              config['MaxBcMass'],
                                              config['Bc_PT'],
                                              config['CTAU_Bc'])

        self.selBc2Ds1GammaWS = makeBc2Ds1Gamma('Bc2Ds1GammaWSFor%s' % name,
                                                self.selDs1WS,
                                                self.selPhoton,
                                                config['BcPVIPchi2'],
#                                                config['BcMassWin'],
                                                config['MinBcMass'],
                                                config['MaxBcMass'],
                                                config['Bc_PT'],
                                                config['CTAU_Bc'])

        # Create and register stripping lines
        self.Bc2Ds1GammaLine = StrippingLine("%s_Line" % name,
                                             prescale=config['Bc2Ds1GammaPreScale'],
                                             postscale=config['Bc2Ds1GammaPostScale'],
                                             selection=self.selBc2Ds1Gamma)
        self.registerLine(self.Bc2Ds1GammaLine)

        self.Bc2Ds1GammaLineWS = StrippingLine("%sWS_Line" % name,
                                               prescale=config['Bc2Ds1GammaPreScale'],
                                               postscale=config['Bc2Ds1GammaPostScale'],
                                               selection=self.selBc2Ds1GammaWS)
        self.registerLine(self.Bc2Ds1GammaLineWS)
        
def makePhoton(name, photonPT):
    """
    Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

    @arg name: name of the Selection.
    @arg photonPT: PT of the photon
    @return: Selection object
    
    """

    _code = "(PT> %(photonPT)s*MeV)" % locals()
    _gammaFilter = FilterDesktop(Code=_code)
    _stdGamma = DataOnDemand(Location='Phys/StdLooseAllPhotons/Particles')
    return Selection(name, Algorithm=_gammaFilter, RequiredSelections=[_stdGamma])

def makeD0(name,  KaonProbNN, pionProbNN, TrGhostProb) :
    """
    Create and return a D0->Kpi Selection object, starting from DataOnDemand 'Phys/StdLooseD02KPi'.
    
    @arg name: name of the Selection.
    @arg KaonProbNN: PID of Kaon
    @arg pionProbNN: PID of pion
    @arg TrGhostProb: Ghost probability of pion/kaon
    @return: Selection object
    
    """

    _code = "(MAXTREE(ABSID=='K+',PROBNNk) > %(KaonProbNN)s) & (MAXTREE(ABSID=='pi+',PROBNNpi) > %(pionProbNN)s) & CHILDCUT( (TRGHOSTPROB<%(TrGhostProb)s) , 1 ) & CHILDCUT( (TRGHOSTPROB<%(TrGhostProb)s) , 2 )" % locals()
    _D0Filter = FilterDesktop(Code=_code)
    _stdD02Kpi = DataOnDemand(Location="Phys/StdLooseD02KPi/Particles")
    return Selection(name, Algorithm=_D0Filter, RequiredSelections=[_stdD02Kpi])

def makeDs1(name, D0Sel, DecDescr, KaonProbNN, TrGhostProb, Ds1DeltaMassWin, MaxDs1VertChi2DOF):
    """
    Create and return a Ds1+ -> D0/D~0 K+ Selection object, starting with the daughters' selections.

    @arg name: name of the Selection.
    @arg D0Sel: D0 selection.
    @arg DecDescr: Decay descriptor
    @arg KaonProbNN: PID of Kaon
    @arg TrGhostProb: Ghost probability of kaon
    @arg Ds1DeltaMassWin: Mass difference m(D0K) - m(D0)
    @arg MaxDs1VertChi2DOF: chi2/NDOF
    @return: Selection object
    """
    _daughterCut = "(TRGHOSTPROB<%(TrGhostProb)s) & (PROBNNk > %(KaonProbNN)s)" % locals()
    _combinationCut = "((AM-AM1) < 1.25*%(Ds1DeltaMassWin)s*MeV)" % locals()
    _motherCut = "((M-M1) < %(Ds1DeltaMassWin)s*MeV) & (VFASPF(VCHI2/VDOF) < %(MaxDs1VertChi2DOF)s)" % locals()
    _Ds1 = CombineParticles(DecayDescriptor = DecDescr,
                            DaughtersCuts = {"K+" : _daughterCut},
                            CombinationCut = _combinationCut,
                            MotherCut=_motherCut)
    
    _stdANNKaon = DataOnDemand(Location="Phys/StdLooseANNKaons/Particles")
    return Selection(name, Algorithm=_Ds1, RequiredSelections=[D0Sel, _stdANNKaon])
                                
def makeBc2Ds1Gamma(name, Ds1Sel, gammaSel, BcPVIPchi2, MinBcMass, MaxBcMass, Bc_PT, CTAU_Bc):
    """
    Create and return a Bc+ -> Ds1+ Gamma Selection object, starting with the daughters' selections.

    @arg name: name of the Selection.
    @arg Ds1Sel: Ds1 -> D0/D~0 K selection
    @arg gammaSel: photon selection
    @arg BcPVIPchi2: IP chi2 of the Bc wrt the PV
    @arg MinBcMass: Lower bound of Bc mass 
    @arg MaxBcMass: Upper bound of Bc mass
    @arg Bc_PT : PT of Bc
    @arg CTAU_Bc: lifetime*c of the Bc
    @return: Selection object

    """  
#    _combinationCut = "ADAMASS('B_c+') < 1.5*%(BcMassWin)s*MeV" % locals()
#    _motherCut = "(ADMASS('B_c+')<%(BcMassWin)s*MeV) & (BPVIPCHI2() < %(BcPVIPchi2)s) & ((BPVLTIME()*c_light)>%(CTAU_Bc)s*micrometer) & (PT>%(Bc_PT)s*MeV)" % locals()

    _combinationCut = "(AM < %(MaxBcMass)s*MeV+500*MeV) & (AM > %(MinBcMass)s*MeV-500*MeV)" % locals()
    _motherCut = "(M < %(MaxBcMass)s*MeV) & (M > %(MinBcMass)s*MeV) & (BPVIPCHI2() < %(BcPVIPchi2)s) & ((BPVLTIME()*c_light)>%(CTAU_Bc)s*micrometer) & (PT>%(Bc_PT)s*MeV)" % locals()
        
    _Bc = CombineParticles(DecayDescriptor="[B_c+ -> D_s1(2536)+ gamma]cc",
                           CombinationCut=_combinationCut,
                           MotherCut=_motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=_Bc, RequiredSelections=[Ds1Sel, gammaSel])
        
# EOF
