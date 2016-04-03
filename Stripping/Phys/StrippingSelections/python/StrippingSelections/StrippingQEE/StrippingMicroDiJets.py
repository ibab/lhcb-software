
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet, ClearDaughters
from GaudiKernel.SystemOfUnits import GeV

__author__  = "Xabier Cid Vidal", "Cedric Potterat", "William Barter"
__all__     = "default_config", "MicroDiJetsConf"

## if you want to prescale this line, please contact the authors before!

default_config = {
  'NAME'        : 'MicroDiJets',
  'BUILDERTYPE' : 'MicroDiJetsConf',
  'WGs'         : [ 'QEE' ],
  'STREAMS'     : [ 'Leptonic' ],
  'CONFIG': {
    'MicroDiJetsLine_Prescale'  : 0.5,
    'MicroDiJetsLine_Postscale' : 1.0,
    'RequiredRawEvents'         : ['Calo'],
    'min_jet_pT'                : 20. * GeV,
    'TOS_HLT2'                  : None, # 'Hlt2JetsDiJetDecision'
  },
}


class MicroDiJetsConf( LineBuilder ) :

  __configuration_keys__ = default_config['CONFIG'].keys()

  def __init__( self, name, config ) :

    LineBuilder.__init__( self, name, config )

    emptySel = ClearDaughters("MDSTJets", Inputs = ["Phys/StdJets/Particles"])
    emptySelLoc = updateDoD(emptySel)
    #print emptySelLoc
    self.emptySelLoc = emptySelLoc.keys()[0]
    #print self.emptySelLoc

    sel_MicroDiJets = self.makeJetGroup(name + 'MicroDiJets', config)

    self.registerLine(StrippingLine( name + 'Line',
      prescale  = config[ 'MicroDiJetsLine_Prescale' ],
      postscale = config[ 'MicroDiJetsLine_Postscale' ],
      MDSTFlag  = True,
      checkPV   = False,
      selection = sel_MicroDiJets,
      RequiredRawEvents = config['RequiredRawEvents'],
    ))


  def makeJetGroup(self, name, config):

    JetGroup = CombineParticles("Combine"+ name)
    JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet"

    JetGroup.ParticleCombiners = {"" : "MomentumCombiner"}
    # JetGroup.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
    # vfitter = getattr ( JetGroup , "LoKi::VertexFitter" )
    # vfitter.Jets = ""

    JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s ) " %config }

    JetGroup.CombinationCut = "AALLSAMEBPV "
    JetGroup.MotherCut = "ALL"

    ## TOS_HLT2 on-demand
    hlt = config['TOS_HLT2']
    if hlt:
      JetGroup.MotherCut += '& (TOS("%s", "Hlt2TriggerTisTos"))'%hlt

    return Selection("Sel"+ name, Algorithm = JetGroup, RequiredSelections = [DataOnDemand(self.emptySelLoc)])




