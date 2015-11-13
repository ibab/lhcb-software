__author__ = ['Pablo Ruiz Valls']
__date__ = '11/11/2015'
__version__ = '$Revision: 1.0 $'

__all__ = ('StrippingB2XGammaNoBiasConf', 'makePhoton', 'makePhi2KK', 'makeKstar', 'makeBs2PhiGamma', 'makeBd2KstGamma', 'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseAllPhotons

name = 'Beauty2XGammaNoBias'
default_config = {
    'NAME'        : 'Beauty2XGammaNoBias',
    'WGs'         : ['RD'],
    'BUILDERTYPE' : 'Beauty2XGammaNoBiasConf',
    'CONFIG'      : {
           'TrChi2'              : 100.        # dimensionless, useless (default is 4)
          
          ,'PhiMassWin'          : 400.       # MeV was 20
          ,'KstMassWin'          : 3000.      # MeV was 150

          ,'photonPT'            : 2600.     # MeV

          ,'BsMassWin'           : 3000.     # MeV was 1000
          ,'B0MassWin'           : 3000.     # MeV was 1000

          # Pre- and postscales
          ,'Bs2PhiGammaPreScale'               : 1.0
          ,'Bs2PhiGammaPostScale'              : 1.0
          ,'Bd2KstGammaPreScale'               : 1.0
          ,'Bd2KstGammaPostScale'              : 1.0
          ,'TISTOSLinesDict' : {
                   'Hlt2RadiativeBs2PhiGammaUnbiased.*Decision%TOS':0,
                   'Hlt2RadiativeBd2KstGammaULUnbiased.*Decision%TOS':0,
                   }
          },
    'STREAMS' : ['Leptonic'],
    }

class Beauty2XGammaNoBiasConf(LineBuilder):
    __configuration_keys__ = ('TrChi2'
                              ,'PhiMassWin'
                              ,'KstMassWin'
                              ,'photonPT'
                              ,'BsMassWin'
                              ,'B0MassWin'
                              # Pre- and postscales
                              ,'Bs2PhiGammaPreScale'
                              ,'Bs2PhiGammaPostScale'
                              ,'Bd2KstGammaPreScale'
                              ,'Bd2KstGammaPostScale'
                              ,'TISTOSLinesDict'
                              )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        # Selection of B daughters: photon, phi and kstar
        self.name = name
        self.selPhoton = makePhoton('PhotonSel' + self.name,
                                    config['photonPT'])
        self.selPhi2KK = makePhi2KK('PhiSel' + self.name,
                                    config['TrChi2'],
                                    config['PhiMassWin'])
        self.selKst = makeKstar('KStarSel' + self.name,
                                config['TrChi2'],
                                config['KstMassWin'])

        # Bs->Phi Gamma selections
        self.selBs2PhiGamma = makeBs2PhiGamma(self.name + 'Bs2PhiGamma',
                                              self.selPhi2KK,
                                              self.selPhoton,
                                              config['BsMassWin'])
        # Bd->Kst Gamma selections
        self.selBd2KstGamma = makeBd2KstGamma(self.name + 'Bd2KstGamma',
                                              self.selKst,
                                              self.selPhoton,
                                              config['B0MassWin'])
        # Stripping lines
        self.Bs2PhiGammaLine = StrippingLine(self.name + 'Bs2PhiGammaLine',
                                             prescale=config['Bs2PhiGammaPreScale'],
                                             postscale=config['Bs2PhiGammaPostScale'],
                                             EnableFlavourTagging = True,
                                             RequiredRawEvents = ["Calo"],
                                             MDSTFlag = True,
                                             RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                             selection=tisTosSelection(self.selBs2PhiGamma,config['TISTOSLinesDict']))
        self.registerLine(self.Bs2PhiGammaLine)

        self.Bd2KstGammaLine = StrippingLine(self.name + 'Bd2KstGammaLine',
                                             prescale=config['Bd2KstGammaPreScale'],
                                             postscale=config['Bd2KstGammaPostScale'],
                                             EnableFlavourTagging = True,
                                             RequiredRawEvents = ["Calo"],
                                             MDSTFlag = True,
                                             RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                             selection=tisTosSelection(self.selBd2KstGamma,config['TISTOSLinesDict']))
        self.registerLine(self.Bd2KstGammaLine)
                                
        
def makePhoton(name, photonPT):
    """Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

    @arg name: name of the Selection.
    @arg photonPT: PT of the photon
    
    @return: Selection object
    
    """
    _code = "(PT> %(photonPT)s*MeV)" % locals()
    _gammaFilter = FilterDesktop(Code=_code)
    _stdGamma = StdLooseAllPhotons
    return Selection(name, Algorithm=_gammaFilter, RequiredSelections=[_stdGamma])

def makePhi2KK(name, TrChi2, PhiMassWin) :
    """
    Create and return a Phi->KK Selection object, starting from DataOnDemand 'Phys/StdLoosePhi2KK'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ tracks
    @arg PhiMassWin: selected Phi mass window
    
    @return: Selection object
    
    """
    _preambulo = ["goodKaon = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodPhi = ((ADMASS('phi(1020)') < %(PhiMassWin)s*MeV))" % locals()]
    _code = 'goodPhi & CHILDCUT( goodKaon, 1 ) & CHILDCUT( goodKaon, 2 )'
    _phiFilter = FilterDesktop(Preambulo=_preambulo, Code=_code)
    _stdPhi2KK = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
    return Selection(name, Algorithm=_phiFilter, RequiredSelections=[_stdPhi2KK])

def makeKstar(name, TrChi2, KstMassWin) :
    """
    Create and return a K*->Kpi Selection object, starting from DataOnDemand 'Phys/StdVeryLooseDetachedKst2Kpi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: tracks chi2
    @arg KstMassWin: K* mass window
    
    @return: Selection object
    
    """
    _preambulo = ["goodTrack = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodKstar = ((ADMASS('K*(892)0') < %(KstMassWin)s*MeV))" % locals()]
    _code = "goodKstar & CHILDCUT( goodTrack , 1 ) & CHILDCUT( goodTrack , 2 )"
    _kstFilter = FilterDesktop(Preambulo=_preambulo, Code=_code)
    _stdKst2Kpi = DataOnDemand(Location="Phys/StdVeryLooseDetachedKst2Kpi/Particles")
    return Selection(name, Algorithm=_kstFilter, RequiredSelections=[_stdKst2Kpi])

def makeBs2PhiGamma(name, phiSel, gammaSel, BsMassWin):
    """
    Create and return a Bs -> Phi Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg phiSel: Phi -> K+ K+ selection
    @arg gammaSel: photon selection
    @arg BsMassWin: Bs mass window
    
    @return: Selection object
    
    """  
    _motherCut = "(ADMASS('B_s0')<%(BsMassWin)s*MeV)" % locals()
    _Bs = CombineParticles(DecayDescriptor="B_s0 -> phi(1020) gamma",
                           MotherCut=_motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=_Bs, RequiredSelections=[gammaSel, phiSel])

def makeBd2KstGamma(name, kstSel, gammaSel, B0MassWin):
    """
    Create and return a Bd -> K* Gamma Selection object, starting with the daughters' selections.

    @arg name: name of the Selection.
    @arg kstSel: K* -> K pi selection
    @arg gammaSel: photon selection
    @arg B0MassWin: B0 mass window
    
    @return: Selection object

    """  
    _motherCut = "(ADMASS('B0')<%(B0MassWin)s*MeV)" % locals()
    _Bd = CombineParticles(DecayDescriptor="[B0 -> K*(892)0 gamma]cc",
                           MotherCut=_motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=_Bd, RequiredSelections=[gammaSel, kstSel])
        
def makeTISTOSFilter(name,dict_TISTOS):
    specs = dict_TISTOS
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    return tisTosFilter

def tisTosSelection(sel,TISTOSdict):
    '''Filters Selection sel to be TOS OR TIS.'''
    tisTosFilter = makeTISTOSFilter(sel.name(),TISTOSdict)
    return Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter, RequiredSelections=[sel])

# EOF


