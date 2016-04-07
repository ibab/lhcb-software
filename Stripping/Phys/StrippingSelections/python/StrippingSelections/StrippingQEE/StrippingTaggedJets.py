
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet
from GaudiKernel.SystemOfUnits import GeV

__author__=["Xabier Cid Vidal","Cedric Potterat"]
__all__ = ["default_config","TaggedJetsConf"]

## if you want to prescale this line, please contact the authors before!

default_config = {
  'NAME'        : 'TaggedJets',
  'BUILDERTYPE' : 'TaggedJetsConf',
  'WGs'         : [ 'QEE' ],
  'STREAMS'     : [ 'BhadronCompleteEvent' ],
  'CONFIG': {
    'TaggedJetsPair_Prescale'               : 1.0,
    'TaggedJetsPair_Postscale'              : 1.0,
    'TaggedJetsPairExclusiveDiJet_Prescale' : 1.0,
    'TaggedJetsPairExclusiveDiJet_Postscale': 1.0,
    'DiTaggedJetsPair_Prescale'             : 1.0,
    'DiTaggedJetsPair_Postscale'            : 1.0,
    'min_jet_pT'                            : 25. * GeV,
    'min_jet_pT_ExclusiveDiJet'             : 20. * GeV,
  }
}


class TaggedJetsConf( LineBuilder ) :

  __configuration_keys__ = default_config['CONFIG'].keys()

  def __init__( self, name, config ) :

    LineBuilder.__init__( self, name, config )

    self._config = config

    ## B TAGGING!!
    filterJet = FilterJet('BDTTagJets')
    filterJet.Inputs = ["Phys/StdJets/Particles"]
    filterJet.tagToolName = 'LoKi::BDTTag'
    filterJet.Cut = 0
    taggedJetsLocation = updateDoD ( filterJet )
    self.__taggedJets = DataOnDemand(Location = taggedJetsLocation.keys()[0])

    ## Making selections        
    self.sel_DiJet  = self.makeJetGroup (name + 'DiJet', ptcut=1)
    self.sel_DiJet_ExclusiveDiJet  = self.makeJetGroup (name + 'DiJetExclusiveDiJet',ptcut=2)
    self.sel_DiJet_noPT  = self.makeJetGroup (name + 'DiJetNoPT')  
    self.sel_DiJetPair  = self.makeDiJetPair (name + 'DiJetPair')
        
    ## Filter events with more than two jets
    filterJets = " ( CONTAINS('Phys/StdJets/Particles') == 2 ) "

    ## Register lines
    self.registerLine(StrippingLine( name + 'JetPairLine',
      prescale  = config[ 'TaggedJetsPair_Prescale' ],
      postscale = config[ 'TaggedJetsPair_Postscale' ],
      RequiredRawEvents = ["Calo"],
      checkPV   = False,
      selection = self.sel_DiJet,
    ))

    self.registerLine(StrippingLine( name + 'JetPairLineExclusiveDiJet',
     prescale   = config[ 'TaggedJetsPairExclusiveDiJet_Prescale' ],
     postscale  = config[ 'TaggedJetsPairExclusiveDiJet_Postscale' ],
     FILTER     = filterJets,
     RequiredRawEvents = ["Calo"],
     checkPV    = False,
     selection  = self.sel_DiJet_ExclusiveDiJet,
    ))

        
    self.registerLine(StrippingLine( name + 'DiJetPairLine',
      prescale  = config[ 'DiTaggedJetsPair_Prescale' ],
      postscale = config[ 'DiTaggedJetsPair_Postscale' ],
      RequiredRawEvents = ["Calo"],
      checkPV   = False,
      selection = self.sel_DiJetPair,
    ))


  def makeJetGroup(self,_name,ptcut=0):

    JetGroup = CombineParticles("Combine"+_name)
    JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet"
    JetGroup.ParticleCombiners = {"" : "LoKi::VertexFitter"}
    JetGroup.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
    vfitter = getattr ( JetGroup , "LoKi::VertexFitter" )
    vfitter.Jets = ""
    
    if ptcut==1: JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s ) " %self._config }
    elif ptcut==2: JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT_ExclusiveDiJet)s ) " %self._config }    
    
    JetGroup.MotherCut = "ALL"
    
    requiredSelections = [self.__taggedJets]
    
    return Selection ("Sel"+_name,
                      Algorithm = JetGroup,
                      RequiredSelections = requiredSelections)



  def makeDiJetPair(self,_name):

    DiJetPair = CombineParticles("Combine"+_name)
    DiJetPair.DecayDescriptor = "H_20 -> H_10 H_10"
    DiJetPair.ParticleCombiners = {"" : "LoKi::VertexFitter"}
    DiJetPair.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
    
    DiJetPair.MotherCut = "ALL"
    
    requiredSelections = [self.sel_DiJet_noPT]

    return Selection ("Sel"+_name,
                      Algorithm = DiJetPair,
                      RequiredSelections = requiredSelections)
