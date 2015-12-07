
from Gaudi.Configuration import *
from Configurables       import FilterDesktop, CombineParticles, LoKi__VertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import  StdJets
from CommonParticles.Utils import *
from Configurables import FilterJet, ClearDaughters
from GaudiKernel.SystemOfUnits import GeV

__author__=["Xabier Cid Vidal","Cedric Potterat","William Barter"]
__all__ = ["default_name","default_config","MicroDiJetsConf"]

## if you want to prescale this line, please contact the authors before!
default_name = 'MicroDiJets'

default_config = {
    'NAME': default_name,
    'BUILDERTYPE'  : 'MicroDiJetsConf',
    'WGs' : [ 'QEE' ],
    'STREAMS' : [ 'Leptonic' ],
    'CONFIG':{'MicroDiJetsLine_Prescale'   : 0.5,
              'MicroDiJetsLine_Postscale'  : 1.0,
              'min_jet_pT' : 20. * GeV
              }
    }


class MicroDiJetsConf( LineBuilder ) :

    __configuration_keys__ = ( 'MicroDiJetsLine_Prescale',
                               'MicroDiJetsLine_Postscale',
                               'min_jet_pT'
                               )

    def __init__( self, name, config ) :

        LineBuilder.__init__( self, name, config )

        self._myname = name
        self._config = config
        
        self.emptySel = ClearDaughters("MDSTJets", Inputs = ["Phys/StdJets/Particles"])
        self.emptySelLoc = updateDoD(self.emptySel)
        #print self.emptySelLoc
        self.emptySelLoc = self.emptySelLoc.keys()[0]
        #print self.emptySelLoc

        self.sel_MicroDiJets  = self.makeJetGroup (self._myname + 'MicroDiJets')
        
        self.line_MicroDiJetsLine = StrippingLine( self._myname + 'Line',
                                                  prescale  = config[ 'MicroDiJetsLine_Prescale' ],
                                                  postscale = config[ 'MicroDiJetsLine_Postscale' ],
                                                  RequiredRawEvents = ["Calo"],
                                                  checkPV   = False,
                                                  selection = self.sel_MicroDiJets
                                                  )

        self.registerLine( self.line_MicroDiJetsLine )
     
     



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
        
        return Selection("Sel"+_name, Algorithm = JetGroup, RequiredSelections = [DataOnDemand(self.emptySelLoc)])
        


  
