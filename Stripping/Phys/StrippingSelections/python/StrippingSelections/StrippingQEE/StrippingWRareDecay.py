"""

Modified from StrippingRD/StrippingBeauty2XGammaExclusive.py
Module for construction of W boson Radiative Decays Stripping Selections and StrippingLines.
Provides functions to build K*+, rho+, D+, D_s+, and B+ selections.

"""

__author__  = ['Hang Yin']
__date__    = '04/03/2016'
__version__ = '$Revision: 1.0 $'

__all__ = 'default_config', 'WRareDecayConf' 

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, GeV, mm, picosecond
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseAllPhotons

default_config = {
  'NAME'        : 'WRareDecay',
  'BUILDERTYPE' : 'WRareDecayConf',
  'WGs'         : ['QEE'],
  'STREAMS'     : ['EW'],
  'CONFIG'      : {
    'TrChi2'        : 5.    ,   # dimensionless
    'TrIPChi2'      : 9.    ,   # dimensionless
    'TrGhost'       : 0.4   ,   # dimensionless
          
    'RhoMassWin'    : 150. * MeV,
    'KSMassWin'     : 40.  * MeV,
    'KstMassWin'    : 120. * MeV,
    'DpMassWin'     : 150. * MeV,
    'DsMassWin'     : 150. * MeV,
    'BuMassWin'     : 400. * MeV,
    'KSLTIME'       : 0.5  * picosecond,
    'BPVLTIME'      : 0.2  * picosecond,

    'photonPT'      : 2500. * MeV,
    'photonHighPT'  : 10000.* MeV,
    'pion0PT'       : 800.  * MeV,
    'kaonPT'        : 400.  * MeV,
    'MesonPT'       : 13000.* MeV,
          
    'WMassWin'      : 50000. * MeV,

    # Pre- and postscales
    'W2PionGammaPreScale'   : 1.0,
    'W2PionGammaPostScale'  : 1.0,
    'W2KaonGammaPreScale'   : 1.0,
    'W2KaonGammaPostScale'  : 1.0,
    'W2KstPGammaPreScale'   : 1.0,
    'W2KstPGammaPostScale'  : 1.0,
    'W2RhoGammaPreScale'    : 1.0,
    'W2RhoGammaPostScale'   : 1.0,
    'W2DpGammaPreScale'     : 1.0,
    'W2DpGammaPostScale'    : 1.0,
    'W2DsGammaPreScale'     : 1.0,
    'W2DsGammaPostScale'    : 1.0,
    'W2BuGammaPreScale'     : 1.0,
    'W2BuGammaPostScale'    : 1.0,

    'RawEvents' : ["Muon", "Calo", "Rich", "Velo", "Tracker"],
  },
}

class WRareDecayConf(LineBuilder):
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        # Selection of W daughters: photon, Ds and kstar
        # make photon
        selPhoton = makePhoton('PhotonSel' + name,
                                    config['photonPT'])
        selPhotonHi = makePhoton('PhotonHighPTSel' + name,
                                      config['photonHighPT'])
        # make pions 
        selPions = makePions('PionsSel' + name,
                                  config['TrChi2'],
                                  config['TrIPChi2'],
                                  config['TrGhost'],
                                  config['kaonPT'])
        selPionsHi = makePions('PionsHighPTSel' + name,
                                    config['TrChi2'],
                                    config['TrIPChi2'],
                                    config['TrGhost'],
                                    config['MesonPT'])
        selPion0 = makePion0('Pion0Sel' + name,
                                  config['pion0PT'])
        # make Kaons 
        selKaons = makeKaons('KaonsSel' + name,
                                  config['TrChi2'],
                                  config['TrIPChi2'],
                                  config['TrGhost'],
                                  config['kaonPT'])
        selKaonsHi = makeKaons('KaonsHighPTSel' + name,
                                    config['TrChi2'],
                                    config['TrIPChi2'],
                                    config['TrGhost'],
                                    config['MesonPT'])
        selKS = makeKS('KSSel' + name,
                            config['KSLTIME'],
                            config['KSMassWin'])
        # make K*+ -> KS pi+, K+, pi0 
        selKstP1 = makeKstar('KstPSel1' + name,
                                  selKS,
                                  selPions,  
                                  config['KstMassWin'],
                                  config['MesonPT'])
        selKstP2 = makeKstar('KstPSel2' + name,
                                  selKaons,
                                  selPion0,  
                                  config['KstMassWin'],
                                  config['MesonPT'])
        selKstP = MergedSelection('KstPSel' + name, RequiredSelections=[selKstP1, selKstP2])
        # make rho+ -> pi+ pi0
        selRho2PiPi = makeRho2PiPi('RhoSel' + name,
                                        config['TrChi2'],
                                        config['RhoMassWin'],
                                        config['MesonPT'])
        # make D+ -> pi+ K- Pi+
        selDp2KPiPi = makeDp2KPiPi('DpSel' + name,
                                        config['TrChi2'],
                                        config['DpMassWin'],
                                        config['MesonPT'])
        # make D_s+ -> K+ K- Pi+
        selDs2KKPi = makeDs2KKPi('DsSel' + name,
                                      config['TrChi2'],
                                      config['DsMassWin'],
                                      config['MesonPT'])
        # make B+ -> Jpsi K+
        selBu2JpsiK = makeBu2JpsiK('BuSel' + name,
                                        config['TrChi2'],
                                        config['BuMassWin'],
                                        config['MesonPT'],
                                        config['BPVLTIME'])
        ##
        ## W event selection
        ##

        # W->pi+ Gamma selections
        selW2PionGamma = makeW2PionGamma(name + 'W2PionGamma',
                                              selPionsHi,
                                              selPhotonHi,
                                              config['WMassWin'])
        # W->K+ Gamma selections
        selW2KaonGamma = makeW2KaonGamma(name + 'W2KaonGamma',
                                              selKaonsHi,
                                              selPhotonHi,
                                              config['WMassWin'])
        # W->K*+ Gamma selections
        selW2KstPGamma = makeW2KstPGamma(name + 'W2KstPGamma',
                                              selKstP,
                                              selPhotonHi,
                                              config['WMassWin'])
        # W->rho+ Gamma selections
        selW2RhoGamma = makeW2RhoGamma(name + 'W2RhoGamma',
                                            selRho2PiPi,
                                            selPhotonHi,
                                            config['WMassWin'])
        # W->D+ Gamma selections
        selW2DpGamma = makeW2DpGamma(name + 'W2DpGamma',
                                          selDp2KPiPi,
                                          selPhoton,
                                          config['WMassWin'])
        # W->Ds+ Gamma selections
        selW2DsGamma = makeW2DsGamma(name + 'W2DsGamma',
                                          selDs2KKPi,
                                          selPhoton,
                                          config['WMassWin'])
        # W->B+ Gamma selections
        selW2BuGamma = makeW2BuGamma(name + 'W2BuGamma',
                                          selBu2JpsiK,
                                          selPhoton,
                                          config['WMassWin'])
        ##
        ## Stripping lines  
        ##

        # W-> pi gamma line
        W2PionGammaLine = StrippingLine(name + 'W2PionGammaLine',
                                             prescale=config['W2PionGammaPreScale'],
                                             postscale=config['W2PionGammaPostScale'],
                                             RequiredRawEvents = config['RawEvents'],
                                             RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                             selection=selW2PionGamma)
        self.registerLine(W2PionGammaLine)
        
        # W-> K gamma line
        W2KaonGammaLine = StrippingLine(name + 'W2KaonGammaLine',
                                             prescale=config['W2KaonGammaPreScale'],
                                             postscale=config['W2KaonGammaPostScale'],
                                             RequiredRawEvents = config['RawEvents'],
                                             RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                             selection=selW2KaonGamma)
        self.registerLine(W2KaonGammaLine)
        
        # W-> K* gamma line
        W2KstPGammaLine = StrippingLine(name + 'W2KstPGammaLine',
                                             prescale=config['W2KstPGammaPreScale'],
                                             postscale=config['W2KstPGammaPostScale'],
                                             RequiredRawEvents = config['RawEvents'],
                                             selection=selW2KstPGamma)
        self.registerLine(W2KstPGammaLine)

        # W-> rho gamma line
        W2RhoGammaLine = StrippingLine(name + 'W2RhoGammaLine',
                                            prescale=config['W2RhoGammaPreScale'],
                                            postscale=config['W2RhoGammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            selection=selW2RhoGamma)
        self.registerLine(W2RhoGammaLine)

        # W-> D+ gamma line
        W2DpGammaLine = StrippingLine(name + 'W2DpGammaLine',
                                           prescale=config['W2DpGammaPreScale'],
                                           postscale=config['W2DpGammaPostScale'],
                                           RequiredRawEvents = config['RawEvents'],
                                           RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                           selection=selW2DpGamma)
        self.registerLine(W2DpGammaLine)

        # W-> Ds gamma line
        W2DsGammaLine = StrippingLine(name + 'W2DsGammaLine',
                                           prescale=config['W2DsGammaPreScale'],
                                           postscale=config['W2DsGammaPostScale'],
                                           RequiredRawEvents = config['RawEvents'],
                                           RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                           selection=selW2DsGamma)
        self.registerLine(W2DsGammaLine)
        
        # W-> B+ gamma line
        W2BuGammaLine = StrippingLine(name + 'W2BuGammaLine',
                                           prescale=config['W2BuGammaPreScale'],
                                           postscale=config['W2BuGammaPostScale'],
                                           RequiredRawEvents = config['RawEvents'],
                                           RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                           selection=selW2BuGamma)
        self.registerLine(W2BuGammaLine)

###############################################################################
def makePhoton(name, photonPT):
    """Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

    @arg name: name of the Selection.
    @arg photonPT: PT of the photon
    
    @return: Selection object
    
    """
    code = "(PT > %(photonPT)s)" % locals()
    gammaFilter = FilterDesktop(Code=code)
    stdGamma = StdLooseAllPhotons
    return Selection(name, Algorithm=gammaFilter, RequiredSelections=[stdGamma])

def makePions(name, TrChi2, TrIPChi2, TrGhost, MesonPT):
    """Create pi+ Selection object starting from DataOnDemand 'Phys/StdLoosePions'.

    @arg name: name of the Selection.
    @arg TrChi2: track chi2/dof of the pions 
    @arg TrIPChi2: trakc IP chi2/dof of the pions 
    @arg TrGhost: trakc ghost probability of the pions 
    @arg MesonPT: PT of the pions 
    
    @return: Selection object
    
    """
    code = "(PT > %(MesonPT)s) & (TRGHOSTPROB < %(TrGhost)s) & (TRCHI2DOF<%(TrChi2)s) & ISBASIC & (MIPCHI2DV(PRIMARY) > %(TrIPChi2)s)" % locals()
    pionFilter = FilterDesktop(Code=code)
    stdpion = DataOnDemand(Location="Phys/StdLoosePions/Particles")
    return Selection(name, Algorithm=pionFilter, RequiredSelections=[stdpion])

def makeKaons(name, TrChi2, TrIPChi2, TrGhost, MesonPT):
    """Create pi+ Selection object starting from DataOnDemand 'Phys/StdLooseKaons'.

    @arg name: name of the Selection.
    @arg TrChi2: track chi2/dof of the kaons 
    @arg TrIPChi2: trakc IP chi2/dof of the kaons 
    @arg TrGhost: trakc ghost probability of the kaons 
    @arg MesonPT: PT of the kaons
    
    @return: Selection object
    
    """
    code = "(PT > %(MesonPT)s) & (TRGHOSTPROB < %(TrGhost)s) & (TRCHI2DOF<%(TrChi2)s) & ISBASIC & (MIPCHI2DV(PRIMARY) > %(TrIPChi2)s)" % locals()
    kaonFilter = FilterDesktop(Code=code)
    stdkaon = DataOnDemand(Location="Phys/StdLooseKaons/Particles")
    return Selection(name, Algorithm=kaonFilter, RequiredSelections=[stdkaon])

def makePion0(name, pion0PT):
    """Create pion0 Selection object starting for resloved and merged pi0.

    @arg name: name of the Selection.
    @arg pion0PT: PT of pi0

    @return: Selection object

    """
    from StandardParticles import StdLooseResolvedPi0 as pi0resolved
    from StandardParticles import StdLooseMergedPi0 as pi0merged
    code1 = "(PT > %(pion0PT)s)" % locals()
    filter_pi0resolved = FilterDesktop(Code = code1)
    selpi0resolved = Selection("Selection_"+name+"_pi0resolved", RequiredSelections=[pi0resolved], Algorithm=filter_pi0resolved)
    code2 = "(PT > %(pion0PT)s)" % locals()
    filter_pi0merged = FilterDesktop(Code = code2) 
    selpi0merged = Selection("Selection_"+name+"_pi0merged", RequiredSelections=[pi0merged], Algorithm=filter_pi0merged)
    return MergedSelection(name, RequiredSelections=[selpi0resolved,selpi0merged])

def makeKS(name, KSLTIME, KSMassWin):
    """
    Filter kshort from StdLooseKshort
    """
    from StandardParticles import StdLooseKsDD as ksdd
    from StandardParticles import StdLooseKsLL as ksll
    code = "(ADMASS('KS0') < %(KSMassWin)s) & (BPVLTIME() > %(KSLTIME)s)" % locals()
    filter_ksdd = FilterDesktop(Code = code)
    filter_ksll = FilterDesktop(Code = code)
    selksdd = Selection("Selection_"+name+"_Ksdd", RequiredSelections = [ ksdd ], Algorithm = filter_ksdd)
    selksll = Selection("Selection_"+name+"_Ksll", RequiredSelections = [ ksll ], Algorithm = filter_ksll)
    return MergedSelection("Selection_"+name+"_Kshort", RequiredSelections = [ selksdd, selksll ])

###############################################################################

def makeRho2PiPi(name, TrChi2, RhoMassWin, MesonPT):
    """
    Create and return a Rho+ -> Pi+ Pi0 Selection object, starting from DataOnDemand 'Phys/StdLoosePions'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ Pi+ tracks
    @arg RhoMassWin: selected Ds mass window
    @arg MesonPT: selected rho pT cut 
    
    @return: Selection object
    
    """
    preambulo = ["goodPion = (TRCHI2DOF < %(TrChi2)s)" % locals(),
                  "goodRho = ((ADMASS('rho(770)+') < %(RhoMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = 'goodRho & CHILDCUT( goodPion, 1 )'
    RhoFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdRho2PiPi = DataOnDemand(Location="Phys/StdLooseRhoPlus/Particles")
    return Selection(name, Algorithm=RhoFilter, RequiredSelections=[stdRho2PiPi])

def makeKstar(name, Kaons, Pions, KstMassWin, MesonPT):
    """
    Make K*(892)+ -> K+ pi0 or Kspi+
    """
    kstar = CombineParticles()
    kstar.DecayDescriptors = ["[K*(892)+ -> K+ pi0]cc", "[K*(892)+ -> KS0 pi+]cc"]
    kstar.MotherCut = "((ADMASS('K*(892)+') < %(KstMassWin)s ) & (PT > %(MesonPT)s))" % locals()

    kstarConf = kstar.configurable("Combine_"+name+"_KPi")
    return Selection(name, Algorithm = kstarConf, RequiredSelections = [ Kaons, Pions ] )

def makeDp2KPiPi(name, TrChi2, DpMassWin, MesonPT):
    """
    Create and return a D+->KPiPi Selection object, starting from DataOnDemand 'Phys/StdLooseDplus2KKPi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ Pi+ tracks
    @arg DpMassWin: selected D+ mass window
    @arg MesonPT: selected Dp pT cut 
    
    @return: Selection object
    
    """
    preambulo = ["goodKaon = (TRCHI2DOF < %(TrChi2)s)" % locals(),
                  "goodDp = ((ADMASS('D+') < %(DpMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = 'goodDp & CHILDCUT( goodKaon, 1 ) & CHILDCUT( goodKaon, 2 ) & CHILDCUT( goodKaon, 3 )'
    DpFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdDp2KPiPi = DataOnDemand(Location="Phys/StdLooseDplus2KPiPi/Particles")
    return Selection(name, Algorithm=DpFilter, RequiredSelections=[stdDp2KPiPi])

def makeDs2KKPi(name, TrChi2, DsMassWin, MesonPT):
    """
    Create and return a Ds->KKPi Selection object, starting from DataOnDemand 'Phys/StdLooseDplus2KKPi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ Pi+ tracks
    @arg DsMassWin: selected Ds mass window
    @arg MesonPT: selected Ds pT cut 
    
    @return: Selection object
    
    """
    preambulo = ["goodKaon = (TRCHI2DOF < %(TrChi2)s)" % locals(),
                  "goodDs = ((ADMASS('D_s+') < %(DsMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = 'goodDs & CHILDCUT( goodKaon, 1 ) & CHILDCUT( goodKaon, 2 ) & CHILDCUT( goodKaon, 3 )'
    DsFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdDs2KKPi = DataOnDemand(Location="Phys/StdLooseDplus2KKPi/Particles")
    return Selection(name, Algorithm=DsFilter, RequiredSelections=[stdDs2KKPi])

def makeBu2JpsiK(name, TrChi2, BuMassWin, MesonPT, BPVLTIME):
    """
    Create and return a Ds->KKPi Selection object, starting from DataOnDemand 'Phys/StdLooseDplus2KKPi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ Pi+ tracks
    @arg DsMassWin: selected Ds mass window
    @arg MesonPT: selected Ds pT cut 
    
    @return: Selection object
    
    """
    stdJpsiMuMu = DataOnDemand(Location="Phys/StdLooseJpsi2MuMu/Particles")
    stdKaon = DataOnDemand(Location="Phys/StdAllNoPIDsKaons/Particles")

    motherCut = "((ADMASS('B+') < %(BuMassWin)s ) & (PT > %(MesonPT)s) & (BPVLTIME() > %(BPVLTIME)s))" % locals()
    daughtersCutstr = "TRCHI2DOF < %(TrChi2)s" % locals()
    daughtersCuts = {'K+': daughtersCutstr, 'K-':daughtersCutstr }

    bujpsik = CombineParticles(DecayDescriptor="[B+ -> J/psi(1S) K+]cc",
                                MotherCut=motherCut,
                                DaughtersCuts = daughtersCuts,
                                ReFitPVs=True)
    return Selection(name, Algorithm = bujpsik, RequiredSelections = [ stdJpsiMuMu,  stdKaon] )

###############################################################################

def makeW2PionGamma(name, pionSel, gammaSel, WMassWin):
    """
    Create and return a W+ -> pi+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg pionSel: pion selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> pi+ gamma]cc",
                           MotherCut=motherCut,
                           #ParticleCombiners = {"" : "MomentumCombiner"},
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, pionSel])

def makeW2KaonGamma(name, kaonSel, gammaSel, WMassWin):
    """
    Create and return a W+ -> K+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg kaonSel: kaon selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> K+ gamma]cc",
                           MotherCut=motherCut,
                           #ParticleCombiners = {"" : "MomentumCombiner"},
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, kaonSel])

def makeW2KstPGamma(name, kstpSel, gammaSel, WMassWin):
    """
    Create and return a W+ -> K+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg kaonSel: kaon selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> K*(892)+ gamma]cc",
                           MotherCut=motherCut,
                           #ParticleCombiners = {"" : "MomentumCombiner"},
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, kstpSel])

def makeW2RhoGamma(name, rhoSel, gammaSel, WMassWin):
    """
    Create and return a W+ -> rho+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg rhoSel: rho+ -> pi+ pi0 selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> rho(770)+ gamma]cc",
                           MotherCut=motherCut,
                           #ParticleCombiners = {"" : "MomentumCombiner"},
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, rhoSel])

def makeW2DpGamma(name, DpSel, gammaSel, WMassWin):
    """
    Create and return a W -> D+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg DpSel: D+ -> Pi+ K- Pi+ selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> D+ gamma]cc",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, DpSel])

def makeW2DsGamma(name, DsSel, gammaSel, WMassWin):
    """
    Create and return a W -> Ds Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg DsSel: D_s+ -> K+ K- Pi+ selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> D+ gamma]cc",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, DsSel])

def makeW2BuGamma(name, BuSel, gammaSel, WMassWin):
    """
    Create and return a W -> B+ Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg BuSel: B+ -> Jpsi K+ selection
    @arg gammaSel: photon selection
    @arg WMassWin: W mass window 
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('W+') < %(WMassWin)s)" % locals()
    Wp = CombineParticles(DecayDescriptor="[W+ -> B+ gamma]cc",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Wp, RequiredSelections=[gammaSel, BuSel])
