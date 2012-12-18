__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( GaudiSequencer, LoKi__PFJetMaker, LoKi__FastJetMaker, HistogramSvc )

class JetMakerConf:

    def __init__(self, _name, Inputs = ['Phys/PFParticles/Particles'],
                 PFTypes = ['ChargedHadron','Muon','Electron','Photon','Pi0','HCALNeutrals','NeutralRecovery','V0','Charged0Momentum','ChargedInfMomentum','BadPhotonMatchingT','BadPhoton','IsolatedPhoton'],
                 R = 0.5 ,
                 PtMin = 5000.,
                 AssociateWithPV = True ,
                 JetEnergyCorrection = True ,
                 JetIDCut = True,
                 jetidnumber=98 ,
                 algtype="anti-kt"):
        
        jetname_dict = {"kt":0,"Cambridge":1,"anti-kt":2}
        dictOfPFType = {'ChargedHadron':2,'Muon':3,'Electron':4,
                        'Photon':11,'Pi0':12,'MergedPi0':13,'ResolvedPi0':14,'HCALNeutrals':15,'NeutralRecovery':16,
                        'V0':101,'D':102,'B':103,
                        'Charged0Momentum':1001,'ChargedInfMomentum':1002,'BadPhotonMatchingT':1003,'BadPhoton':1004,'IsolatedPhoton':1005}
        self.name = _name
        self.Inputs = Inputs
        self.AssociateWithPV = AssociateWithPV
        self.R = R
        self.PtMin = PtMin
        self.JetEnergyCorrection = JetEnergyCorrection
        self.JetIDCut = JetIDCut
        self.jetMakerTool =  'LoKi__FastJetMaker'
        self.jetMakerType =  jetname_dict[algtype]
        self.jetidnumber = jetidnumber
        self.PFParticleTypes = [ dictOfPFType[k] for k in PFTypes ]
        if 'Pi0' in PFTypes:
            self.PFParticleTypes.append(dictOfPFType['MergedPi0'])
            self.PFParticleTypes.append(dictOfPFType['ResolvedPi0'])
        self.algorithms = []
        self.setupJetMaker()
        hsvc = HistogramSvc ( 'HistogramDataSvc' );
        hsvc.Input += [ "JEC DATAFILE='$PARAMFILESROOT/data/JetEnergyCorrections_Reco12.root' TYP='ROOT'" ]
        
    def setupJetMaker(self):
        jetMakerName = self.name
        algo =  LoKi__PFJetMaker ( jetMakerName )
        algo.JetMaker = self.jetMakerTool
        algo.Associate2Vertex = self.AssociateWithPV
        algo.addTool ( LoKi__FastJetMaker )
        algo.Inputs = self.Inputs
        algo.PFParticleTypes = self.PFParticleTypes
        tool = getattr ( algo , 'LoKi__FastJetMaker' )
        tool.Type = self.jetMakerType 
        tool.RParameter = self.R
        tool.PtMin = self.PtMin
        tool.Recombination = 0
        tool.JetID = self.jetidnumber
        if self.JetEnergyCorrection:
            algo.ApplyJEC = True
            algo.HistoPath = 'JEC/'
        if self.JetIDCut :
            algo.ApplyJetID = True
        self.algorithms.append(algo)
