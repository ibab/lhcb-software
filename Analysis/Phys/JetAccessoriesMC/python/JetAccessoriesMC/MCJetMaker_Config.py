__version__ = "$Id: MCJetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__MCJetMaker,  LoKi__HEPMCJetMaker, LoKi__FastJetMaker, HistogramSvc )

class HEPMCJetMakerConf:

    def __init__(self, _name, PartTypes = ['pi+','K+','p+','mu+','e+','gamma','pi0','KS0','KL0','n0','f_0(980)','f_0(1370)'],
                 R = 0.5 ,
                 PtMin = 5000.,
                 MotherCut = "GNONE" ,
                 ToBanCut = "GNONE" ,
                 SimpleAcceptance = False ,
                 jetidnumber = 999 ,
                 algtype="anti-kt",
                 OutputTable = "",
                 SaveMotherOnly = False ):


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
        self.SaveMotherOnly = SaveMotherOnly
        self.OutputTable = OutputTable
        self.algorithms = []
        self.setupHEPJetMaker()

    def setupHEPJetMaker(self):
        from GaudiKernel.ProcessJobOptions import importOptions
        importOptions("$LOKIGENMCROOT/python/LoKiGenMC/HepMC2MC_Configuration.py")
        importOptions("$LOKIGENMCROOT/python/LoKiGenMC/MC2Collision_Configuration.py")
        jetMakerName = self.name
        algo =  LoKi__HEPMCJetMaker ( jetMakerName )
        algo.CodeForMotherSelection = self.CodeForMotherSelection
        algo.CodeForBannedSelection = self.CodeForBannedSelection
        algo.OutputTable = self.OutputTable
        algo.SaveMCJetsFromMotherOnly = self.SaveMotherOnly
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




class MCJetMakerConf:

    def __init__(self, _name,
                 R = 0.5 ,
                 PtMin = 5000.,
                 MotherCut = "MCNONE" ,
                 ToBanCut = "MCNONE" ,
                 SimpleAcceptance = False ,
                 jetidnumber = 99 ,
                 algtype="anti-kt",
                 OutputTableMC = "",
                 SaveMotherOnly = False ):


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
        self.SaveMotherOnly = SaveMotherOnly
        self.OutputTableMC = OutputTableMC
        self.algorithms = []
        self.setupMCJetMaker()


    def setupMCJetMaker(self):
        from GaudiKernel.ProcessJobOptions import importOptions
        jetMakerName = self.name
        algo =  LoKi__MCJetMaker ( jetMakerName )
        algo.CodeForMotherSelectionMC = self.CodeForMotherSelection
        algo.CodeForBannedSelectionMC = self.CodeForBannedSelection
        algo.OutputTableMC = self.OutputTableMC
        algo.SaveMCJetsFromMotherOnly = self.SaveMotherOnly
        algo.SimpleAcceptance = self.SimpleAcceptance
        algo.JetMaker = self.jetMakerTool
        algo.addTool ( LoKi__FastJetMaker )
        tool = getattr ( algo , 'LoKi__FastJetMaker' )
        tool.Type = self.jetMakerType
        tool.RParameter = self.R
        tool.PtMin = self.PtMin
        tool.Recombination = 0
        tool.JetID = self.jetidnumber
        self.algorithms.append(algo)
