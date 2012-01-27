__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__JetMaker, LoKi__FastJetMaker, HistogramSvc )


class JetMakerConf:
    def __init__(self, _name, Inputs = ['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], R = 0.5 , PtMin = 5000., AssociateWithPV = False , JetEnergyCorrection = False , JetID = False ):
         self.name = _name
         self.Inputs = Inputs
         self.AssociateWithPV = AssociateWithPV
         self.R = R
         self.PtMin = PtMin
         self.JetEnergyCorrection = JetEnergyCorrection
         self.JetID = JetID
         self.jetMakerTool =  'LoKi__FastJetMaker'
         self.jetMakerType =  2
         self.algorithms = []
         self.setupJetMaker()
         if self.JetID : self.setupJetID()

    def setupJetMaker(self):
        from Configurables         import LoKi__JetMaker, LoKi__FastJetMaker
        jetMakerName = self.name
        if self.JetID : jetMakerName+= 'NoJetID'
        algo =  LoKi__JetMaker ( jetMakerName )
        algo.JetMaker = self.jetMakerTool
        algo.Associate2Vertex = self.AssociateWithPV
        algo.addTool ( LoKi__FastJetMaker )
        algo.Inputs = self.Inputs
        tool = getattr ( algo , 'LoKi__FastJetMaker' )
        tool.Type = self.jetMakerType 
        tool.RParameter = self.R
        tool.PtMin = self.PtMin
        tool.Recombination = 0
        if self.JetEnergyCorrection:
            algo.ApplyJEC = True
            algo.HistoPath = 'JEC/'
        self.algorithms.append(algo)

    def setupJetID(self):
        from Configurables      import FilterDesktop
        filterPIDJets = FilterDesktop(self.name)
        filterPIDJets.Inputs = [ self.algorithms[0].Output ]
        filterPIDJets.Code="INFO(9002,-1.)>3 & INFO(9005,-1.)>2 & INFO(9003,1.)<0.8" 
        self.algorithms.append(filterPIDJets)
