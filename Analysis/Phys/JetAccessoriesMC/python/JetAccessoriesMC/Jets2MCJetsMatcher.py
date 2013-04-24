__version__ = "$Id: MCJetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, MCJets2JetsAlg,  LoKi__MCJets2Jets )

class Jets2MCJetsConf:

    def __init__(self, _name,
                 JetsLocation = "Phys/StdJets/Particles",
                 MCJets2MCPTable = "Relations/Phys/MCJets2MCParticles",
                 MCJets2JetsTable = "Relations/Phys/StdMCJets2StdJets",
                 Jets2MCJetsTable = "Relations/Phys/StdJets2StdMCJets",
                 MinRForP2MC = 0.05,
                 PartTypes = ['pi+','K+','p+','mu+','e+','gamma','pi0','KS0','KL0','n0'],
                 R = 0.5 ,
                 PtMin = 5000.,
                 MotherCut = "GNONE" ,
                 ToBanCut = "GNONE" ,
                 SimpleAcceptance = False ,
                 jetidnumber = 99 ,
                 algtype="anti-kt",
                 OutputTable = "",
                 SaveMotherOnly = False ):
        
        self.name = _name
        
        from JetAccessoriesMC.MCJetMaker_Config import MCJetMakerConf
        mcjetmaker = MCJetMakerConf("StdMC", PartTypes , R  , PtMin, MotherCut  , ToBanCut ,  SimpleAcceptance , jetidnumber  ,algtype, MCJets2MCPTable , SaveMotherOnly )
        self.JetsLocation = JetsLocation
        self.MCJets2MCPTable = MCJets2MCPTable
        self.MCJets2JetsTable = MCJets2JetsTable
        self.Jets2MCJetsTable = Jets2MCJetsTable
        self.MinRForP2MC = MinRForP2MC
        self.algorithms = mcjetmaker.algorithms
        self.setupJet2Jet()
        
    def setupJet2Jet(self):   
        algo =  MCJets2JetsAlg ( self.name+'Matcher' )
        algo.Jets2Jets = "LoKi__MCJets2Jets"
        algo.addTool ( LoKi__MCJets2Jets )
        algo.JetsALocation = "Phys/"+self.name+"Jets/Particles"
        algo.JetsBLocation = self.JetsLocation
        algo.OutputTable = self.MCJets2JetsTable
        algo.OutputInverseTable = self.Jets2MCJetsTable
        tool = getattr ( algo , 'LoKi__MCJets2Jets' )
        tool.MinRForP2MC = 0.05
        tool.MCJet2MCP = self.MCJets2MCPTable
        self.algorithms.append(algo)
