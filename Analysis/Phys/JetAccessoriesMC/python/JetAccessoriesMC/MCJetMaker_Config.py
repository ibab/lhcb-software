__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__MCJetMaker, LoKi__FastJetMaker, HistogramSvc )

class MCJetMakerConf:

    def __init__(self, _name, PartTypes = ['pi+','K+','p+','mu+','e+','gamma','pi0','KS0','KL0','n0'],
                 R = 0.5 ,
                 PtMin = 5000.,
                 MotherCut = "GNONE" ,
                 ToBanCut = "GNONE" ,
                 SimpleAcceptance = False ,
                 jetidnumber = 99 ,
                 algtype="anti-kt"):
        
        jetname_dict = {"kt":0,"Cambridge":1,"anti-kt":2}
        self.name = _name
        self.R = R
        self.SimpleAcceptance = SimpleAcceptance
        self.CodeForMotherSelection = MotherCut
        self.CodeForBannedSelection = ToBanCut
        self.PtMin = PtMin
        self.jetMakerTool =  'LoKi__FastJetMaker'
        self.jetMakerType =  jetname_dict[algtype]
        self.jetidnumber = jetidnumber
        self.MCParticleTypes = PartTypes
        self.algorithms = []
        self.setupJetMaker()
        
    def setupJetMaker(self):
        jetMakerName = self.name
        algo =  LoKi__MCJetMaker ( jetMakerName )
        algo.CodeForMotherSelection = self.CodeForMotherSelection
        algo.CodeForBannedSelection = self.CodeForBannedSelection
        algo.SimpleAcceptance = self.SimpleAcceptance
        algo.JetMaker = self.jetMakerTool
        algo.addTool ( LoKi__FastJetMaker )
        algo.MCParticleTypes = self.MCParticleTypes
        tool = getattr ( algo , 'LoKi__FastJetMaker' )
        tool.Type = self.jetMakerType 
        tool.RParameter = self.R
        tool.PtMin = self.PtMin
        tool.Recombination = 0
        tool.JetID = self.jetidnumber
        self.algorithms.append(algo)
