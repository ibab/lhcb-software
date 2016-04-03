
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet
from GaudiKernel.SystemOfUnits import GeV

__author__  = "Xabier Cid Vidal", "Cedric Potterat", "William Barter"
__all__     = "default_config", "FullDiJetsConf"

## if you want to prescale this line, please contact the authors before!

default_config = {
  'NAME'        : 'FullDiJets',
  'BUILDERTYPE' : 'FullDiJetsConf',
  'WGs'         : [ 'QEE' ],
  'STREAMS'     : [ 'EW' ],
  'CONFIG' :{
    'FullDiJetsLine_Prescale'   : 0.05, 
    'FullDiJetsLine_Postscale'  : 1.0,
    'RequiredRawEvents'         : ['Calo'],
    'min_jet_pT'                : 20. * GeV,
    'TOS_HLT2'                  : None, # 'Hlt2JetsDiJetDecision'
  }
}


class FullDiJetsConf( LineBuilder ) :

  __configuration_keys__ = default_config['CONFIG'].keys()

  def __init__( self, name, config ) :

    LineBuilder.__init__( self, name, config )
        
    sel_FullDiJets  = self.makeJetGroup(name + 'FullDiJets', config )
        
    self.registerLine(StrippingLine( name + 'Line',
      prescale  = config[ 'FullDiJetsLine_Prescale' ],
      postscale = config[ 'FullDiJetsLine_Postscale' ],
      checkPV   = False,
      selection = sel_FullDiJets,
      RequiredRawEvents = config['RequiredRawEvents'],
    ))

  def makeJetGroup(self, name, config):

    JetGroup = CombineParticles("Combine"+name)
    JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet"    
      
    JetGroup.ParticleCombiners = {"" : "LoKi::VertexFitter"}
    JetGroup.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
    vfitter = getattr ( JetGroup , "LoKi::VertexFitter" )
    vfitter.Jets = ""
      
    JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s ) " %config }   
    JetGroup.CombinationCut = "AALLSAMEBPV "
    JetGroup.MotherCut = "ALL"
    
    ## TOS_HLT2 on-demand
    hlt = config['TOS_HLT2']
    if hlt:
      JetGroup.MotherCut += '& (TOS("%s", "Hlt2TriggerTisTos"))'%hlt

    requiredSelections = [DataOnDemand(Location = "Phys/StdJets/Particles")]
      
    return Selection ("Sel"+name,
                      Algorithm = JetGroup,
                      RequiredSelections = requiredSelections)

