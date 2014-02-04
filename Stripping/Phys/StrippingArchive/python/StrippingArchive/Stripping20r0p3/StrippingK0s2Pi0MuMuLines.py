'''
Module for construction of K0s-->Pi0MuMu stripping selections and lines

Exported symbols (use python help!):
   - K0s2MuMuLinesConf
'''

__author__ = ['Xabier Cid Vidal','Diego Martinez Santos']
__date__ = '13/12/2013'
__version__ = '$Revision: 1.0 $'

__all__ = ('config_default','K0s2Pi0MuMuLinesConf')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


#### This is the dictionary of all tunable cuts ########
config_default={
    'SignalLinePrescale'    : 1,
    'SignalLinePostscale'   : 1,
    'SidebandLinePrescale'  : 0.2,
    'SidebandLinePostscale' : 1,
    "muIpChi2"              : 36,#25,
    "muTrChi2Dof"           : 5,
    "KSsignalminMass"       : 300,
    "KSsignalmaxMass"       : 600,
    "KSsidebminMass"        : 600,
    "KSsidebmaxMass"        : 1000,
    "KSdoca"                : 0.3,
    "KSdira"                : 0,
    "KSlife"                : 0.06*89.53,#0.05*89.53,
    "KSip"                  : 0.9#1
    }                
  

class K0s2Pi0MuMuLinesConf(LineBuilder) :
    """
    Builder of:
     - K0s-> pi0mumu stripping lines: signal and sideband

    Exports as instance data members:
    signal   : nominal K0s2Pi0MuMu stripping line
    sideband : convers right sideband of K0s2Pi0MuMu
    lines    : list of lines:  [ defaultLine ]
    
    Exports as class data member:
    K0s2Pi0MuMuLinesConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = (
                              'SignalLinePrescale',
                              'SignalLinePostscale',
                              'SidebandLinePrescale',
                              'SidebandLinePostscale',
                              "muIpChi2",
                              "muTrChi2Dof",
                              "KSsignalminMass",
                              "KSsignalmaxMass",
                              "KSsidebminMass",
                              "KSsidebmaxMass",
                              "KSdoca",
                              "KSdira",
                              "KSlife",
                              "KSip"
                              )
    
  
    
    def __init__(self, 
                 name = 'K0s2Pi0MuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        self.config = config

        
        self._stdLoosePi0s = DataOnDemand(Location = "Phys/StdLooseResolvedPi0/Particles")
        self.PseudoJPsiSel = self.makePseudoJPsi(name+"PseudoJPsi")

        norm_name=name+'Signal'
        self.signal = self.makeSignalRegion(norm_name)

        side_name=name+'Sideband'
        self.sideband = self.makeSidebandRegion(side_name)
       
        self.signalLine = StrippingLine(norm_name+"Line",
                                        prescale = config[ 'SignalLinePrescale'],
                                        postscale = config['SignalLinePostscale'],
                                        algos = [ self.signal]
                                        )

        self.sidebandLine = StrippingLine(side_name+"Line",
                                          prescale = config[ 'SidebandLinePrescale'],
                                          postscale = config['SidebandLinePostscale'],
                                          algos = [ self.sideband]
                                          )
        
        self.registerLine(self.signalLine)
        self.registerLine(self.sidebandLine)
       


    def makePseudoJPsi(self,name) :
        """
        Make PseudoJpsi (combination of two detached muons)
        Arguments:
        name : name of the selection
        """
        PseudoJPsi = CombineParticles("Combine"+name)
        PseudoJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        PseudoJPsi.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> %(muIpChi2)s )"\
                                     "&(TRCHI2DOF < %(muTrChi2Dof)s )" %self.config}

        PseudoJPsi.CombinationCut = " (AMAXDOCA('')<%(KSdoca)s*mm)" %self.config
        PseudoJPsi.MotherCut = "ALL"

        _stdLooseMuons = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
        return Selection (name,
                          Algorithm = PseudoJPsi,
                          RequiredSelections = [ _stdLooseMuons])
                                         

    def makeSignalRegion(self,name) :
        """
        K0s2pi0mumu selection object in signal region. 
        Arguments:
        name        : name of the Selection
        """
        from Configurables import OfflineVertexFitter
        K0s2Pi0MuMu1 = CombineParticles("Combine"+name)
        K0s2Pi0MuMu1.DecayDescriptor = "KS0 -> pi0 J/psi(1S)"
        K0s2Pi0MuMu1.addTool( OfflineVertexFitter )
        K0s2Pi0MuMu1.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
        K0s2Pi0MuMu1.OfflineVertexFitter.useResonanceVertex = False
        K0s2Pi0MuMu1.ReFitPVs = True
        

        K0s2Pi0MuMu1.CombinationCut = "  (AM > %(KSsignalminMass)s * MeV)"\
                                      "& (AM < %(KSsignalmaxMass)s * MeV)" %self.config
                                      
        
        K0s2Pi0MuMu1.MotherCut = "((BPVDIRA> %(KSdira)s ) "\
                                 "& ((BPVVDSIGN*M/P) > %(KSlife)s*2.9979e-01) "\
                                 "& (MIPDV(PRIMARY)<%(KSip)s*mm) "\
                                 "& (M> %(KSsignalminMass)s * MeV) "\
                                 "& ( (M< %(KSsignalmaxMass)s * MeV) ))" %self.config

        return Selection (name,
                          Algorithm = K0s2Pi0MuMu1,
                          RequiredSelections = [ self.PseudoJPsiSel,
                                                 self._stdLoosePi0s])


    def makeSidebandRegion(self,name) :
        """
        K0s2pi0mumu selection object in sideband region. 
        Arguments:
        name        : name of the Selection
        """
        from Configurables import OfflineVertexFitter
        K0s2Pi0MuMu2 = CombineParticles("Combine"+name)
        K0s2Pi0MuMu2.DecayDescriptor = "KS0 -> pi0 J/psi(1S)"
        K0s2Pi0MuMu2.addTool( OfflineVertexFitter )
        K0s2Pi0MuMu2.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
        K0s2Pi0MuMu2.OfflineVertexFitter.useResonanceVertex = False
        K0s2Pi0MuMu2.ReFitPVs = True
        

        K0s2Pi0MuMu2.CombinationCut = "  (AM > %(KSsidebminMass)s * MeV)"\
                                      "& (AM < %(KSsidebmaxMass)s * MeV)" %self.config
                                      
        
        K0s2Pi0MuMu2.MotherCut = "((BPVDIRA> %(KSdira)s ) "\
                                 "& ((BPVVDSIGN*M/P) > %(KSlife)s*2.9979e-01) "\
                                 "& (MIPDV(PRIMARY)<%(KSip)s*mm) "\
                                 "& (M> %(KSsidebminMass)s * MeV) "\
                                 "& ( (M< %(KSsidebmaxMass)s * MeV) ))" %self.config


        return Selection (name,
                          Algorithm = K0s2Pi0MuMu2,
                          RequiredSelections = [ self.PseudoJPsiSel,
                                                 self._stdLoosePi0s])
