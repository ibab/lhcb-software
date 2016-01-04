"""
Module for construction of Radiative Decays Stripping Selections and StrippingLines.
Provides functions to build Bc, D0, Ds1 selections.
Provides class Bc2Ds1GammaConf, which constructs the Selections and
StrippingLines given a configuration dictionary.
Exported selection makers: 'makeXic', 'makeBc2Ds1Gamma',
"""

__author__ = ['Marco Pappagallo']
__date__ = '12/11/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('Omegab2XicKpiConf',
           'makeXic',
           'makeOmegab2XicKpi',
           'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

#default_name = 'Bc2Ds1Gamma'
default_config = {
    'NAME'              : 'Omegab2XicKpi',
    'BUILDERTYPE'       : 'Omegab2XicKpiConf',
    'CONFIG'    : {
                   # Omegab
                    'MinOmegabMass'      : 5500.     # MeV
                   ,'MaxOmegabMass'      : 6700.     # MeV
                   ,'Omegab_PT'          : 3500.     # MeV 
#                   (ASUM(SUMTREE(PT,(ISBASIC | (ID=='gamma')),0.0))>5000*MeV)
                   ,'OmegabVertChi2DOF'  : 10        # Dimensionless 
                   ,'OmegabLT'           : 0.2       # ps
                   ,'OmegabIPCHI2'       : 25        # Dimensionless
                   ,'OmegabDIRA'         : 0.999     # Dimensionless
                   ,'KaonProbNN'          : 0.1       # Dimensionless
                   ,'PionProbNN'          : 0.1       # Dimensionless
                   ,'ProtProbNN'          : 0.1       # Dimensionless
                   ,'XicPT'               : 1800      # MeV
                   ,'XicBPVVDCHI2'        : 36
                   ,'XicDIRA'             : 0.        # Dimensionless
                   ,'XicDeltaMassWin'     : 100       # MeV
                   ,'MaxXicVertChi2DOF'   : 10        # Dimensionless
                   # Pre- and postscales
                   ,'Omegab2XicKpiPreScale'               : 1.0
                   ,'Omegab2XicKpiPostScale'              : 1.0
                  },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : [ 'BandQ' ]
    }

class Omegab2XicKpiConf(LineBuilder):
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
                              'MinOmegabMass'
                              ,'MaxOmegabMass'
                              ,'Omegab_PT'
                              ,'OmegabVertChi2DOF'
                              ,'OmegabLT'
                              ,'OmegabIPCHI2'
                              ,'OmegabDIRA'
                              ,'KaonProbNN'
                              ,'PionProbNN'
                              ,'ProtProbNN'
                              ,'XicPT'
                              ,'XicBPVVDCHI2'
                              ,'XicDIRA'
                              ,'XicDeltaMassWin'
                              ,'MaxXicVertChi2DOF'
                              # Pre- and postscales
                              ,'Omegab2XicKpiPreScale'
                              ,'Omegab2XicKpiPostScale'
                              )

    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        # if name not set outside, set it to empty
        #if name == None:
        #    name = ""   

        # Selection of Bc daughters: photon, D0, Ds1

        self.selXic = makeXic('XicFor%s' % name,
                              config['KaonProbNN'],
                              config['PionProbNN'],
                              config['ProtProbNN'],
                              config['XicPT'],
                              config['XicDeltaMassWin'],
                              config['MaxXicVertChi2DOF'],
                              config['XicBPVVDCHI2'],
                              config['XicDIRA'])

        # Bc->Ds1 Gamma selections
        self.selOmegab2XicKpi = makeOmegab2XicKpi('Omegab2XicKpiFor%s' % name,
                                                  self.selXic,
                                                  '[Omega_b- -> Xi_c+ K- pi-]cc',
                                                  config['KaonProbNN'],
                                                  config['PionProbNN'],
                                                  config['MaxOmegabMass'],
                                                  config['MinOmegabMass'],
                                                  config['OmegabVertChi2DOF'],
                                                  config['OmegabIPCHI2'],
                                                  config['OmegabLT'],
                                                  config['Omegab_PT'],
                                                  config['OmegabDIRA'])
        
        self.selOmegab2XicKpiWS = makeOmegab2XicKpi('Omegab2XicKpiWSFor%s' % name,
                                                    self.selXic,
                                                    '[Omega_b- -> Xi_c+ K- pi+]cc',
                                                    config['KaonProbNN'],
                                                    config['PionProbNN'],
                                                    config['MaxOmegabMass'],
                                                    config['MinOmegabMass'],
                                                    config['OmegabVertChi2DOF'],
                                                    config['OmegabIPCHI2'],
                                                    config['OmegabLT'],
                                                    config['Omegab_PT'],
                                                    config['OmegabDIRA'])
        
        # Create and register stripping lines
        self.Omegab2XicKpiLine = StrippingLine("%sLine" % name,
                                               prescale=config['Omegab2XicKpiPreScale'],
                                               postscale=config['Omegab2XicKpiPostScale'],
                                               selection=self.selOmegab2XicKpi)
#                                               selection=self.selXic) 
        self.registerLine(self.Omegab2XicKpiLine)

        self.Omegab2XicKpiLineWS = StrippingLine("%sWSLine" % name,
                                               prescale=config['Omegab2XicKpiPreScale'],
                                               postscale=config['Omegab2XicKpiPostScale'],
                                               selection=self.selOmegab2XicKpiWS)
        self.registerLine(self.Omegab2XicKpiLineWS)
        
def makeXic(name, KaonProbNN, PionProbNN, ProtProbNN, XicPT, XicDeltaMassWin, MaxXicVertChi2DOF, XicBPVVDCHI2, XicDIRA):
    """
    Create and return a Ds1+ -> D0/D~0 K+ Selection object, starting with the daughters' selections.

    @arg name: name of the Selection.
    @arg D0Sel: D0 selection.
    @arg DecDescr: Decay descriptor
    @arg KaonProbNN: PID of Kaon
    @arg TrGhostProb: Ghost probability of kaon
    @arg XicDeltaMassWin: Mass difference m(D0K) - m(D0)
    @arg MaxXicVertChi2DOF: chi2/NDOF
    @return: Selection object
    """
    _daughterKCut  = "(PROBNNk  > %(KaonProbNN)s)" % locals()
    _daughterpiCut = "(PROBNNpi > %(PionProbNN)s)" % locals()
    _daughterpCut  = "(PROBNNp  > %(ProtProbNN)s)" % locals()

    _combinationCut = "(ASUM(PT)> %(XicPT)s*MeV) & (ADAMASS('Xi_c+') < 1.25*%(XicDeltaMassWin)s*MeV) & (ACUTDOCA(0.5*mm,'LoKi::DistanceCalculator'))" % locals()
    _motherCut = "(ADMASS('Xi_c+') < %(XicDeltaMassWin)s*MeV) & (VFASPF(VCHI2/VDOF)<%(MaxXicVertChi2DOF)s) & (BPVVDCHI2>%(XicBPVVDCHI2)s) & (BPVDIRA>%(XicDIRA)s)" % locals()
    _Xic = CombineParticles(DecayDescriptor = "[Xi_c+ -> p+ K- pi+]cc",
                            DaughtersCuts = {"K+"  : _daughterKCut,
                                             "pi+" : _daughterpiCut,
                                             "p+"  : _daughterpCut},
                            CombinationCut = _combinationCut,
                            MotherCut=_motherCut)
    
    _stdANNKaon    = DataOnDemand(Location="Phys/StdLooseANNKaons/Particles")
    _stdANNPion    = DataOnDemand(Location="Phys/StdLooseANNPions/Particles")
    _stdANNProtons = DataOnDemand(Location="Phys/StdLooseANNProtons/Particles")
    
    return Selection(name, Algorithm=_Xic, RequiredSelections=[_stdANNKaon, _stdANNPion, _stdANNProtons])
                                
def makeOmegab2XicKpi(name, XicSel, DecDescr, KaonProbNN, PionProbNN, MaxOmegabMass, MinOmegabMass, OmegabVertChi2DOF, OmegabIPCHI2, OmegabLT, Omegab_PT, OmegabDIRA):
    """
    Create and return a Omegab- -> Xic+ K- pi- Selection object, starting with the daughters' selections.
    """

    _stdANNKaonAll    = DataOnDemand(Location="Phys/StdAllLooseANNKaons/Particles")
    _stdANNPionAll    = DataOnDemand(Location="Phys/StdAllLooseANNPions/Particles")

    _daughterKCut  = "(PROBNNk  > %(KaonProbNN)s)" % locals()
    _daughterpiCut = "(PROBNNpi > %(PionProbNN)s)" % locals()
    
    _combinationCut = "(AM < %(MaxOmegabMass)s*MeV+500*MeV) & (AM > %(MinOmegabMass)s*MeV-500*MeV)" % locals()
    #_combinationCut = "(AM < %(MaxOmegabMass)s*MeV) & (AM > %(MinOmegabMass)s*MeV)" % locals()
    _motherCut = "(M < %(MaxOmegabMass)s*MeV) & (M > %(MinOmegabMass)s*MeV) & (VFASPF(VCHI2/VDOF)<%(OmegabVertChi2DOF)s) & (BPVIPCHI2() < %(OmegabIPCHI2)s) & (BPVLTIME()>%(OmegabLT)s*ps) & (PT>%(Omegab_PT)s*MeV) & (BPVDIRA>%(OmegabDIRA)s)" % locals()
        
    _Omegab = CombineParticles(DecayDescriptor = DecDescr,
                               DaughtersCuts = {"K+"  : _daughterKCut,
                                                "pi+" : _daughterpiCut
                                                },
                               CombinationCut = _combinationCut,
                               MotherCut=_motherCut)

    return Selection(name, Algorithm=_Omegab, RequiredSelections=[XicSel, _stdANNKaonAll, _stdANNPionAll])
        
# EOF
