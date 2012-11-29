__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__JetMaker, LoKi__FastJetMaker, HistogramSvc )


class OldJetMakerConf:

    def __init__(self, _name, Inputs = ['Phys/PFBannedParticles/Particles','Phys/PFParticles/Particles'], R = 0.5 , PtMin = 5000., AssociateWithPV = False , JetEnergyCorrection = False , JetID = False, jetidnumber=98 , algtype="anti-kt"):
         jetname_dict = {"kt":0,"Cambridge":1,"anti-kt":2}
         self.name = _name
         self.Inputs = Inputs
         self.AssociateWithPV = AssociateWithPV
         self.R = R
         self.PtMin = PtMin
         self.JetEnergyCorrection = JetEnergyCorrection
         self.JetID = JetID
         self.jetMakerTool =  'LoKi__FastJetMaker'
         #self.jetMakerType =  2
         self.jetMakerType =  jetname_dict[algtype]
         self.jetidnumber = jetidnumber
         self.algorithms = []
         self.setupJetMaker()

    def setupJetMaker(self):
        from Configurables         import LoKi__JetMaker, LoKi__FastJetMaker
        jetMakerName = self.name
        #if self.JetID : jetMakerName+= 'NoJetID'
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
        tool.JetID = self.jetidnumber
        if self.JetEnergyCorrection:
            algo.ApplyJEC = True
            algo.HistoPath = 'JEC/'
        if self.JetID :
            algo.ApplyJetID = True
        self.algorithms.append(algo)

 ##    def setupJetID(self):
##         from Configurables      import FilterDesktop
##         filterPIDJets = FilterDesktop(self.name)
##         filterPIDJets.Inputs = [ 'Phys/'+self.name+ 'NoJetID'+'/Particles' ]
##         ##filterPIDJets.Preambul = [ 'n90 = INFO(9002,-1.)','mtf = INFO(9003,1.)','hasPVInfo = INFO(9005,-1.)' ]
##         filterPIDJets.Code="PT>5500." 
##         self.algorithms.append(filterPIDJets)

