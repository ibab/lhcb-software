"""

Modified from StrippingRD/StrippingBeauty2XGammaExclusive.py
Module for construction of Z boson Radiative Decays Stripping Selections and StrippingLines.
Provides functions to build K*, rho, omega, D0, Jpsi, and Upsilon selections.

"""

__author__ = ['Hang Yin']
__date__ = '04/03/2016'
__version__ = '$Revision: 1.0 $'

__all__ = 'default_config', 'Z0RareDecayConf' 

from Gaudi.Configuration import *
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdLooseAllPhotons

default_config = {
  'NAME'        : 'Z0RareDecay',
  'BUILDERTYPE' : 'Z0RareDecayConf',
  'WGs'         : ['QEE'],
  'STREAMS'     : ['EW'],
  'CONFIG'      : {
    'TrChi2'          : 5.,        # dimensionless, useless (default is 4)
    'VtxChi2'         : 20.,       # dimensionless, useless (default is 4)
          
    'RhoMassWin'      : 230.  * MeV,
    'OmegaMassWin'    : 230.  * MeV,
    'KstMassWin'      : 120.  * MeV,
    'PhiMassWin'      : 200.  * MeV,
    'D0MassWin'       : 100.  * MeV,
    'JpsiMassMin'     : 3000. * MeV,
    'JpsiMassMax'     : 3200. * MeV,
    'UpsilonMassMin'  : 8500. * MeV,

    'MuonP'           : -8000. * MeV,
    'MuonPT'          : 650.   * MeV,
    'photonPT'        : 2500.  * MeV,
    'pion0PT'         : 860.   * MeV,
    'Pi0Pi0PT'        : 10000. * MeV,
    'MesonPT'         : 13000. * MeV,

    'ZMassWin'        : 60000. * MeV,

    # Pre- and postscales
    'Z2GammaGammaPreScale'  : 1.0,
    'Z2GammaGammaPostScale' : 1.0,
    'Z2Pi0GammaPreScale'    : 1.0,
    'Z2Pi0GammaPostScale'   : 1.0,
    'Z2Pi0Pi0PreScale'      : 1.0,
    'Z2Pi0Pi0PostScale'     : 1.0,
    'Z2RhoGammaPreScale'    : 1.0,
    'Z2RhoGammaPostScale'   : 1.0,
    'Z2OmegaGammaPreScale'  : 1.0,
    'Z2OmegaGammaPostScale' : 1.0,
    'Z2KstGammaPreScale'    : 1.0,
    'Z2KstGammaPostScale'   : 1.0,
    'Z2PhiGammaPreScale'    : 1.0,
    'Z2PhiGammaPostScale'   : 1.0,
    'Z2D0GammaPreScale'     : 1.0,
    'Z2D0GammaPostScale'    : 1.0,
    'Z2QONGammaPreScale'    : 1.0,
    'Z2QONGammaPostScale'   : 1.0,

    'RawEvents' : ["Muon", "Calo", "Rich", "Velo", "Tracker"],
  },
}

class Z0RareDecayConf(LineBuilder):
    __configuration_keys__ = default_config['CONFIG'].keys()
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)
        ##
        ## Selection of Z daughters: photon, pion0, rho, omega, 
        ##   kstar, phi, Jpsi and Upsilon 
        ##

        # make photon
        selPhoton = makePhoton('PhotonSel' + name,
                                    config['photonPT'])
        selPhotonHi = makePhoton('PhotonHighSel' + name,
                                      config['Pi0Pi0PT'])
        # make pion0
        selPion0 = makePion0('Pion0SelMy',
                                  config['pion0PT'])
        # make rho
        selRho = makeRho('RhoSel' + name,
                              config['TrChi2'],
                              config['MesonPT'],
                              config['RhoMassWin'])
        # make omega
        selOmega = makeOmega('OmegaSel' + name,
                                 config['MesonPT'],
                                 config['OmegaMassWin'],
                                 selPion0)
        # make Kstar
        selKst = makeKstar('KStarSel' + name,
                                config['TrChi2'],
                                config['MesonPT'],
                                config['KstMassWin'])
        # make phi
        selPhi2KK = makePhi2KK('PhiSel' + name,
                                    config['TrChi2'],
                                    config['MesonPT'],
                                    config['PhiMassWin'])
        # make D0 
        selD02KPi = makeD02KPi('D0Sel' + name,
                                    config['TrChi2'],
                                    config['MesonPT'],
                                    config['D0MassWin'])
        # make quarkonium
        selQON = makeQON('QniumSel' + name,
                              config['VtxChi2'],
                              config['TrChi2'],
                              config['MesonPT'],
                              config['MuonPT'],
                              config['MuonP'],
                              config['JpsiMassMin'],
                              config['JpsiMassMax'],
                              config['UpsilonMassMin'])

        ##
        ## make event selections 
        ##

        # Z -> Gamma Gamma selections
        selZ2GammaGamma = makeZ2GammaGamma(name + 'Z2GammaGamma',
                                                selPhoton,
                                                config['Pi0Pi0PT'],
                                                config['ZMassWin'])
        # Z -> Pi0 Gamma selections
        selZ2Pi0Gamma = makeZ2Pi0Gamma(name + 'Z2Pi0Gamma',
                                            selPion0,
                                            selPhoton,
                                            config['Pi0Pi0PT'],
                                            config['ZMassWin'])
        # Z -> Pi0 Pi0 selections
        selZ2Pi0Pi0 = makeZ2Pi0Pi0(name + 'Z2Pi0Pi0',
                                        selPion0,
                                        config['Pi0Pi0PT'],
                                        config['ZMassWin'])
        # Z -> rho Gamma selections
        selZ2RhoGamma = makeZ2RhoGamma(name + 'Z2RhoGamma',
                                            selRho,
                                            selPhoton,
                                            config['ZMassWin'])
        # Z -> Omega Gamma selections
        selZ2OmegaGamma = makeZ2OmegaGamma(name + 'Z2OmegaGamma',
                                                selOmega,
                                                selPhoton,
                                                config['ZMassWin'])
        # Z ->Kst Gamma selections
        selZ2KstGamma = makeZ2KstGamma(name + 'Z2KstGamma',
                                            selKst,
                                            selPhotonHi,
                                            config['ZMassWin'])
        # Z ->Phi Gamma selections
        selZ2PhiGamma = makeZ2PhiGamma(name + 'Z2PhiGamma',
                                            selPhi2KK,
                                            selPhoton,
                                            config['ZMassWin'])
        # Z ->D0 Gamma selections
        selZ2D0Gamma = makeZ2D0Gamma(name + 'Z2D0Gamma',
                                          selD02KPi,
                                          selPhoton,
                                          config['ZMassWin'])
        # Z -> Jpsi/Upsilon Gamma selections
        selZ2QONGamma = makeZ2QONGamma(name + 'Z2QONGamma',
                                            selQON,
                                            selPhoton,
                                            config['ZMassWin'])

        ##
        ## Stripping lines
        ##

        # Z-> gamma gamma line
        Z2GammaGammaLine = StrippingLine(name + 'Z2GammaGammaLine',
                                              prescale=config['Z2GammaGammaPreScale'],
                                              postscale=config['Z2GammaGammaPostScale'],
                                              RequiredRawEvents = config['RawEvents'],
                                              RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                              selection=selZ2GammaGamma)
        self.registerLine(Z2GammaGammaLine)

        # Z-> pi0 gamma line
        Z2Pi0GammaLine = StrippingLine(name + 'Z2Pi0GammaLine',
                                            prescale=config['Z2Pi0GammaPreScale'],
                                            postscale=config['Z2Pi0GammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                            selection=selZ2Pi0Gamma)
        self.registerLine(Z2Pi0GammaLine)

        # Z-> pi0 pi0 line
        Z2Pi0Pi0Line = StrippingLine(name + 'Z2Pi0Pi0Line',
                                          prescale=config['Z2Pi0Pi0PreScale'],
                                          postscale=config['Z2Pi0Pi0PostScale'],
                                          RequiredRawEvents = config['RawEvents'],
                                          RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                          selection=selZ2Pi0Pi0)
        self.registerLine(Z2Pi0Pi0Line)

        # Z-> rho gamma line
        Z2RhoGammaLine = StrippingLine(name + 'Z2RhoGammaLine',
                                            prescale=config['Z2RhoGammaPreScale'],
                                            postscale=config['Z2RhoGammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                            selection=selZ2RhoGamma)
        self.registerLine(Z2RhoGammaLine)

        # Z-> omega gamma line
        Z2OmegaGammaLine = StrippingLine(name + 'Z2OmegaGammaLine',
                                              prescale=config['Z2OmegaGammaPreScale'],
                                              postscale=config['Z2OmegaGammaPostScale'],
                                              RequiredRawEvents = config['RawEvents'],
                                              RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                              selection=selZ2OmegaGamma)
        self.registerLine(Z2OmegaGammaLine)

        # Z-> K* gamma line
        Z2KstGammaLine = StrippingLine(name + 'Z2KstGammaLine',
                                            prescale=config['Z2KstGammaPreScale'],
                                            postscale=config['Z2KstGammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                            selection=selZ2KstGamma)
        self.registerLine(Z2KstGammaLine)
        
        # Z-> phi gamma line
        Z2PhiGammaLine = StrippingLine(name + 'Z2PhiGammaLine',
                                            prescale=config['Z2PhiGammaPreScale'],
                                            postscale=config['Z2PhiGammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                            selection=selZ2PhiGamma)
        self.registerLine(Z2PhiGammaLine)

        # Z-> D0 gamma line
        Z2D0GammaLine = StrippingLine(name + 'Z2D0GammaLine',
                                           prescale=config['Z2D0GammaPreScale'],
                                           postscale=config['Z2D0GammaPostScale'],
                                           RequiredRawEvents = config['RawEvents'],
                                           RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                           selection=selZ2D0Gamma)
        self.registerLine(Z2D0GammaLine)

        # Z-> J/psi, Upsilon gamma line
        Z2QONGammaLine = StrippingLine(name + 'Z2QONGammaLine',
                                            prescale=config['Z2QONGammaPreScale'],
                                            postscale=config['Z2QONGammaPostScale'],
                                            RequiredRawEvents = config['RawEvents'],
                                            RelatedInfoTools = [{'Type' : 'RelInfoVertexIsolation', 'Location': "VertexIsoInfo"  }],
                                            selection=selZ2QONGamma)
        self.registerLine(Z2QONGammaLine)

#################################################################################

def makePhoton(name, photonPT):
    """Create photon Selection object starting from DataOnDemand 'Phys/StdLooseAllPhotons'.

    @arg name: name of the Selection.
    @arg photonPT: PT of the photon
    
    @return: Selection object
    
    """
    code = "(PT> %(photonPT)s)" % locals()
    gammaFilter = FilterDesktop(Code=code)
    stdGamma = StdLooseAllPhotons
    return Selection(name, Algorithm=gammaFilter, RequiredSelections=[stdGamma])

def makePion0(name, pion0PT):
    """Create pion0 Selection object starting for resloved and merged pi0.

    @arg name: name of the Selection.
    @arg pion0PT: PT of pi0

    @return: Selection object

    """
    from StandardParticles import StdLooseResolvedPi0 as _pi0resolved
    from StandardParticles import StdLooseMergedPi0 as _pi0merged
    code1 = "(PT > %(pion0PT)s)" % locals()
    filter_pi0resolved = FilterDesktop(Code = code1)
    selpi0resolved = Selection("Selection_"+name+"_pi0resolved", RequiredSelections=[_pi0resolved], Algorithm=filter_pi0resolved)
    code2 = "(PT > %(pion0PT)s)" % locals()
    filter_pi0merged = FilterDesktop(Code = code2) 
    selpi0merged = Selection("Selection_"+name+"_pi0merged", RequiredSelections=[_pi0merged], Algorithm=filter_pi0merged)
    return MergedSelection(name, RequiredSelections=[selpi0resolved,selpi0merged])

def makeRho(name, TrChi2, MesonPT, RhoMassWin) :
    """
    Create and return a rho -> pi+ pi- Selection object, starting from DataOnDemand 'Phys/StdLooseRho0'.
    
    @arg name: name of the Selection.
    @arg TrChi2: tracks chi2
    @arg MesonPT: PT of combined particle
    @arg RhoMassWin: rho(770)0 mass window
    
    @return: Selection object
    
    """
    preambulo = ["goodTrack = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodRho = ((ADMASS('rho(770)0') < %(RhoMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = "goodRho & CHILDCUT( goodTrack , 1 ) & CHILDCUT( goodTrack , 2 )"
    rhoFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdRho2pipi = DataOnDemand(Location="Phys/StdLooseRho0/Particles")
    return Selection(name, Algorithm=rhoFilter, RequiredSelections=[stdRho2pipi])

def makeOmega(name, MesonPT, OmegaMassWin, selPion0) :
    """
    Create and return a omega -> pi+ pi- pi0 Selection object, starting from DataOnDemand 'Phys/StdLoosePions'.
    
    @arg name: name of the Selection.
    @arg MesonPT: PT of combined particle
    @arg OmegaMassWin: omega(782) mass window
    @arg selPion0: selected pion0 candidates
    
    @return: Selection object
    
    """
    stdPions = DataOnDemand(Location="Phys/StdLoosePions/Particles")
    omega2pipipizero = CombineParticles()
    omega2pipipizero.DecayDescriptor = "omega(782) -> pi+ pi- pi0"
    omega2pipipizero.CombinationCut = "(ADAMASS('omega(782)') < %(OmegaMassWin)s )" % locals()
    omega2pipipizero.MotherCut = "((ADMASS('omega(782)') < %(OmegaMassWin)s ) & (PT > %(MesonPT)s))" % locals()
    omegaConf = omega2pipipizero.configurable("Combine_"+name+"_PiPiPi0")

    return Selection(name, Algorithm=omegaConf, RequiredSelections=[stdPions, selPion0])

def makeKstar(name, TrChi2, MesonPT, KstMassWin) :
    """
    Create and return a K*->Kpi Selection object, starting from DataOnDemand 'Phys/StdVeryLooseDetachedKst2Kpi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: tracks chi2
    @arg KstMassWin: K* mass window
    
    @return: Selection object
    
    """
    preambulo = ["goodTrack = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodKstar = ((ADMASS('K*(892)0') < %(KstMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = "goodKstar & CHILDCUT( goodTrack , 1 ) & CHILDCUT( goodTrack , 2 )"
    kstFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdKst2Kpi = DataOnDemand(Location="Phys/StdVeryLooseDetachedKst2Kpi/Particles")
    return Selection(name, Algorithm=kstFilter, RequiredSelections=[stdKst2Kpi])

def makePhi2KK(name, TrChi2, MesonPT, PhiMassWin) :
    """
    Create and return a Phi->KK Selection object, starting from DataOnDemand 'Phys/StdLoosePhi2KK'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+ tracks
    @arg MesonPT: selected Phi PT 
    @arg PhiMassWin: selected Phi mass window
    
    @return: Selection object
    
    """
    preambulo = ["goodKaon = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodPhi = ((ADMASS('phi(1020)') < %(PhiMassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = 'goodPhi & CHILDCUT( goodKaon, 1 ) & CHILDCUT( goodKaon, 2 )'
    phiFilter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdPhi2KK = DataOnDemand(Location="Phys/StdLoosePhi2KK/Particles")
    return Selection(name, Algorithm=phiFilter, RequiredSelections=[stdPhi2KK])

def makeD02KPi(name, TrChi2, MesonPT, D0MassWin) :
    """
    Create and return a D0->KPi Selection object, starting from DataOnDemand 'Phys/StdLooseD02KPi'.
    
    @arg name: name of the Selection.
    @arg TrChi2: minimum chi2 of the K+, pi tracks
    @arg MesonPT: selected D0 PT 
    @arg D0MassWin: selected D0 mass window
    
    @return: Selection object
    
    """
    preambulo = ["goodKaon = ((TRCHI2DOF < %(TrChi2)s))" % locals(),
                  "goodD0 = ((ADMASS('D0') < %(D0MassWin)s) & (PT > %(MesonPT)s))" % locals()]
    code = 'goodD0 & CHILDCUT( goodKaon, 1 ) & CHILDCUT( goodKaon, 2 )'
    D0Filter = FilterDesktop(Preambulo=preambulo, Code=code)
    stdD02KPi = DataOnDemand(Location="Phys/StdLooseD02KPi/Particles")
    return Selection(name, Algorithm=D0Filter, RequiredSelections=[stdD02KPi])

def makeQON(name, VtxChi2, TrChi2, MesonPT, MuonPT, MuonP, JpsiMassMin, JpsiMassMax, UpsilonMassMin) :
    """
    Create and return a Jpsi-> mu mu and Upsilon -> mu mu Selection object, starting from DataOnDemand 'Phys/StdLooseDiMuon'.
    
    @arg name: name of the Selection.
    @arg VtxChi2: vertex/dof chi2
    @arg TrChi2: tracks chi2
    @arg MesonPT: PT of combined particle
    @arg MuonPT: PT of Muon 
    @arg MuonP: P of Muon 
    @arg JpsiMassMin: Jpsi mass lower cut
    @arg JpsiMassMax: Jpsi mass higher cut
    @arg UpsilonMassMin: Upsilon mass lower cut
    
    @return: Selection object
    
    """
    MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s ) & (MINTREE('mu+'==ABSID,P) > %(MuonP)s ) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(TrChi2)s)" % locals()
    MuMuCut = "(((MM > %(JpsiMassMin)s ) & (MM < %(JpsiMassMax)s )) | (MM > %(UpsilonMassMin)s )) & (VFASPF(VCHI2PDOF)< %(VtxChi2)s) & (PT > %(MesonPT)s )"%locals()

    DiMuFilter = FilterDesktop(Code= MuonCut + "&" + MuMuCut)
    stdDiMuon = DataOnDemand(Location="Phys/StdLooseDiMuon/Particles")
    return Selection(name, Algorithm=DiMuFilter, RequiredSelections=[stdDiMuon])

####################################################################################

def makeZ2GammaGamma(name, gammaSel, Pi0Pi0PT, ZMassWin):
    """
    Create and return a Z -> Gamma Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg gammaSel: photon selection
    @arg Pi0Pi0PT: PT of photon
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    strCutdau = "PT > %(Pi0Pi0PT)s" % locals()
    daughtersCuts = {'gamma' : strCutdau}
    Z0 = CombineParticles(DecayDescriptor="Z0 -> gamma gamma",
                           MotherCut=motherCut,
                           DaughtersCuts = daughtersCuts,
                           ParticleCombiners = {"" : "MomentumCombiner:PUBLIC"}, 
                           ReFitPVs=False)#True)


    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel])

def makeZ2Pi0Gamma(name, Pion0Sel, gammaSel, Pi0Pi0PT, ZMassWin):
    """
    Create and return a Z -> pi0 Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg Pion0Sel: pi0 selection
    @arg gammaSel: photon selection
    @arg Pi0Pi0PT: PT of pi0 and photon
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    strCutdau = "PT > %(Pi0Pi0PT)s" % locals()
    daughtersCuts = {'pi0' : strCutdau, 'gamma' : strCutdau}
    Z0 = CombineParticles(DecayDescriptor="Z0 -> pi0 gamma",
                           MotherCut=motherCut,
                           DaughtersCuts = daughtersCuts,
                           ParticleCombiners = {"" : "MomentumCombiner:PUBLIC"}, 
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, Pion0Sel])

def makeZ2Pi0Pi0(name, Pion0Sel, Pi0Pi0PT, ZMassWin):
    """
    Create and return a Z -> pi0 pi0 Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg Pion0Sel: pi0 selection
    @arg Pi0Pi0PT: PT of pi0
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    strCutdau = "PT > %(Pi0Pi0PT)s" % locals()
    daughtersCuts = {'pi0' : strCutdau } 
    Z0 = CombineParticles(DecayDescriptor="Z0 -> pi0 pi0",
                           MotherCut=motherCut,
                           DaughtersCuts = daughtersCuts,
                           ParticleCombiners = {"" : "MomentumCombiner:PUBLIC"}, 
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[Pion0Sel])

def makeZ2RhoGamma(name, RhoSel, gammaSel, ZMassWin):
    """
    Create and return a Z -> Rho(770)0 Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg Rho: Rho -> pi+ pi- selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="Z0 -> rho(770)0 gamma",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, RhoSel])

def makeZ2OmegaGamma(name, OmegaSel, gammaSel, ZMassWin):
    """
    Create and return a Z -> Omega(782)0 Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg Omega: Omega -> pi+ pi- pi0 selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="Z0 -> omega(782) gamma",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, OmegaSel])

def makeZ2KstGamma(name, KstSel, gammaSel, ZMassWin):
    """
    Create and return a Z -> K* Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg KstSel: K* -> K+ pi- selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="[Z0 -> K*(892)0 gamma]cc",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, KstSel])

def makeZ2PhiGamma(name, phiSel, gammaSel, ZMassWin):
    """
    Create and return a Z -> Phi Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg phiSel: Phi -> K+ K+ selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="Z0 -> phi(1020) gamma",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, phiSel])

def makeZ2D0Gamma(name, D0Sel, gammaSel, ZMassWin):
    """
    Create and return a Z -> D0 Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg D0Sel: D0 -> K+ pi- selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="Z0 -> D0 gamma",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, D0Sel])

def makeZ2QONGamma(name, QONSel, gammaSel, ZMassWin):
    """
    Create and return a Z -> Jpsi/Upsilon Gamma Selection object, starting with the daughters' selections.
  
    @arg name: name of the Selection.
    @arg QONSel: Jpsi/Upsilon -> mu mu selection
    @arg gammaSel: photon selection
    @arg ZMassWin: Z0 mass window
    
    @return: Selection object
    
    """  
    motherCut = "(ADMASS('Z0') < %(ZMassWin)s)" % locals()
    Z0 = CombineParticles(DecayDescriptor="Z0 -> J/psi(1S) gamma",
                           MotherCut=motherCut,
                           ReFitPVs=False)#True)
    return Selection(name, Algorithm=Z0, RequiredSelections=[gammaSel, QONSel])

