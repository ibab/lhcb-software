__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__JetMaker, LoKi__FastJetMaker )


class JetMakerConf:
    def __init__(self, _name, Inputs = ['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], R = 0.6 , PtMin = 5000., AssociateWithPV = False ):
         self.name = _name
         self.Inputs = Inputs
         self.AssociateWithPV = AssociateWithPV
         self.R = R
         self.PtMin = PtMin
         self.jetMakerTool =  'LoKi__FastJetMaker'
         self.jetMakerType =  2
         self.algorithms = []
         self.setupJetMaker()

    def setupJetMaker(self):
        from Configurables         import LoKi__JetMaker, LoKi__FastJetMaker
        algo =  LoKi__JetMaker ( self.name )
        algo.JetMaker = self.jetMakerTool
        algo.Associate2Vertex = self.AssociateWithPV
        algo.addTool ( LoKi__FastJetMaker )
        algo.Inputs = self.Inputs
        tool = getattr ( algo , 'LoKi__FastJetMaker' )
        tool.Type = self.jetMakerType 
        tool.RParameter = self.R
        tool.PtMin = self.PtMin
        tool.Recombination = 0
        self.algorithms.append(algo)
