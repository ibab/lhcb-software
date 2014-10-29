# Stripping Lines for highPT photon+ pair of AntiKT Jets
# X. Cid Vidal
#

from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdLoosePhotons, StdJets
from CommonParticles.Utils import *

__author__ = ["Xabier Cid Vidal"]
__all__ = ["default_name","default_config","HighPtGammaJetsConf"]

default_name = 'HighPtGammaJets'
## if you want to prescale this line, please contact X. Cid Vidal before!
default_config = {
    'NAME' : default_name,
    'BUILDERTYPE'  : 'HighPtGammaJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ],
    'CONFIG':{'Prescale'  : 1.0,
              'Postscale' : 1.0,
              'photonpT'  : 12.5 ,# GeV/c
              'photonIso' : 25,
              'minJetpT' : 12.5, # GeV/c              
              'PrescaleLoose'  : 0.1,
              'PostscaleLoose' : 1.0,
              'photonpTLoose'  : 7.5,
              'photonIsoLoose' : 20,
              'minJetpTLoose' : 7.5 # GeV/c
              }
    }



class HighPtGammaJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'Prescale',
                               'Postscale',
                               'photonpT',
                               'photonIso',
                               'minJetpT',
                               'PrescaleLoose',
                               'PostscaleLoose',                                                         
                               'photonpTLoose',
                               'photonIsoLoose',
                               'minJetpTLoose'
                               )

    
    
    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config
        
        self.sel_Photon = self.makePhoton( self._myname + 'Photon')
        self.sel_DiJet  = self.makeDiJet (self._myname + 'DiJet')
        self.sel_DiJetGamma  = self.makeDiJetGamma (self._myname + 'DiJetGamma')

        self.line_PhotonJets = StrippingLine( self._myname + 'Line',
                                              prescale  = config[ 'Prescale' ],
                                              postscale = config[ 'Postscale' ],
                                              RequiredRawEvents = ["Calo"],
                                              checkPV   = False,
                                              selection = self.sel_DiJetGamma
                                              )
        
        self.registerLine( self.line_PhotonJets )
        
        self.sel_PhotonLoose = self.makePhoton( self._myname + 'PhotonLoose',loose=1)
        self.sel_DiJetLoose  = self.makeDiJet (self._myname + 'DiJetLoose',loose=1)
        self.sel_DiJetGammaLoose  = self.makeDiJetGamma (self._myname + 'DiJetGammaLoose',loose=1)
        
        self.line_PhotonJetsLoose = StrippingLine( self._myname + 'LooseLine',
                                                   prescale  = config[ 'PrescaleLoose' ],
                                                   postscale = config[ 'PostscaleLoose' ],
                                                   RequiredRawEvents = ["Calo"],
                                                   checkPV   = False,
                                                   selection = self.sel_DiJetGammaLoose
                                                   )
        
        self.registerLine( self.line_PhotonJetsLoose )
        
        
        
    def makePhoton( self, _name,loose=0):
        # Define the photon cuts
        if loose: _cut    = "(PT>%(photonpTLoose)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloTrMatch,-1)>%(photonIsoLoose)s)" %self._config
        else: _cut    = "(PT>%(photonpT)s*GeV) & (PPINFO(LHCb.ProtoParticle.CaloTrMatch,-1)>%(photonIso)s)" %self._config
        
        
        _filter = FilterDesktop( _name,
                                 Code      = _cut
                                 )
        
        return Selection ( "sel"+_name,
                           Algorithm          = _filter,
                           RequiredSelections = [StdLoosePhotons]
                           )
    
    
    def makeDiJet(self,_name,loose=0):
        
        DiJet = CombineParticles("Combine"+_name)
        DiJet.DecayDescriptor = "H_10 -> CELLjet CELLjet"
        DiJet.ParticleCombiners = { '' : 'LoKi::VertexFitter:PUBLIC' }
        
        if loose: DiJet.DaughtersCuts = { "CELLjet" :" (PT > %(minJetpTLoose)s * GeV ) " %self._config }
        else: DiJet.DaughtersCuts = { "CELLjet" :" (PT > %(minJetpT)s * GeV ) " %self._config }

        DiJet.CombinationCut = "AALLSAMEBPV"
        DiJet.MotherCut = "ALL"
        
        return Selection ("Sel"+_name,
                          Algorithm = DiJet,
                          RequiredSelections = [StdJets])
    
    
    def makeDiJetGamma(self,_name,loose=0):
        
        DiJetGamma = CombineParticles("Combine"+_name)
        DiJetGamma.DecayDescriptor = "H_20 -> H_10 gamma"
        DiJetGamma.ParticleCombiners = { '' : 'LoKi::VertexFitter:PUBLIC' }
                
        DiJetGamma.MotherCut = "ALL"
        
        if loose: requiredSelections = [self.sel_PhotonLoose,self.sel_DiJetLoose]
        else: requiredSelections = [self.sel_Photon,self.sel_DiJet]
        
        return Selection ("Sel"+_name,
                          Algorithm = DiJetGamma,
                          RequiredSelections = requiredSelections)
    
    
