
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet
from GaudiKernel.SystemOfUnits import GeV

__author__=["Xabier Cid Vidal","Cedric Potterat","William Barter"]
__all__ = ["default_name","default_config","FullDiJetsConf"]

## if you want to prescale this line, please contact the authors before!
default_name = 'FullDiJets'

default_config = {
    'NAME': default_name,
    'BUILDERTYPE'  : 'FullDiJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'EW' ],
    'CONFIG':{'FullDiJetsLine_Prescale'   : 0.05, 
              'FullDiJetsLine_Postscale'  : 1.0,
              'min_jet_pT' : 20. * GeV
              }
    }


class FullDiJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'FullDiJetsLine_Prescale',
                               'FullDiJetsLine_Postscale',
                               'min_jet_pT'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config
        
        self.sel_FullDiJets  = self.makeJetGroup (self._myname + 'FullDiJets')
        
        self.line_FullDiJetsLine = StrippingLine( self._myname + 'Line',
                                                  prescale  = config[ 'FullDiJetsLine_Prescale' ],
                                                  postscale = config[ 'FullDiJetsLine_Postscale' ],
                                                  RequiredRawEvents = ["Calo"],
                                                  checkPV   = False,
                                                  selection = self.sel_FullDiJets
                                                  )
    
        self.registerLine( self.line_FullDiJetsLine )
      
     



    def makeJetGroup(self,_name):

        JetGroup = CombineParticles("Combine"+_name)
        JetGroup.DecayDescriptor = "H_10 -> CELLjet CELLjet"
       
      
        
        JetGroup.ParticleCombiners = {"" : "LoKi::VertexFitter"}
        JetGroup.addTool( LoKi__VertexFitter, name="LoKi::VertexFitter" )
        vfitter = getattr ( JetGroup , "LoKi::VertexFitter" )
        vfitter.Jets = ""
        
        JetGroup.DaughtersCuts = { "CELLjet" :" (PT > %(min_jet_pT)s ) " %self._config }
     
        JetGroup.CombinationCut = "AALLSAMEBPV "
        JetGroup.MotherCut = "ALL"
        
        requiredSelections = [DataOnDemand(Location = "Phys/StdJets/Particles")]
        
        return Selection ("Sel"+_name,
                          Algorithm = JetGroup,
                          RequiredSelections = requiredSelections)



  
