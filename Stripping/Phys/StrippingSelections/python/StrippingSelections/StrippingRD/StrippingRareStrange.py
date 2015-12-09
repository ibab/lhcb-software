'''
Module for construction of stripping selections for some rare strange decays
Exported symbols (use python help!):
'''

__author__ = ['Francesco Dettori','Andrea Contu']
__date__ = '31/07/2014'
__version__ = '$Revision: 2.1 $'

__all__ = ('RareStrangeLinesConf',
           'config_default',
           'makeSigmaPMuMu',
           'makeSigmaPMuMuDet',
           'myDiMuonDetached',
           'myDiElectronDetached',
           'makeDiElectronDetached',
           'makeSigmaPEMu',
           'makeSigmaPEE',
           'makeSigmaPEEDet',
           'makeSigmaPPi0',
           'makeSigmaPPi0Cal',
           'makeSigmaPMuMuLFV',
           'makeSigmaPMuMuDown',
           'makeSigmaPEEDown',
           'makeSigmaPMuMuLFVDown',
           'makeSigmaMuMuMu',
           'makeSigmaMuMuMuDown',           
           'makeKPiMuMu',
           'makeKPiMuMuLFV',
           'makeKPiPiPi',
           'makeKPiPiPiMassMeas',
           'makeKPiMuMuDown',
           'makeKPiMuMuLFVDown',
           'makeKPiPiPiDown',
           'makeKPiPiPiMassMeasDown',
           'makePhiKMu'
           'makeLambdaPPi',
           'makeLambdaPPiEE',
           )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from CommonParticles import StdNoPIDsDownElectrons
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from GaudiKernel.PhysicalConstants import c_light
from StandardParticles import ( StdAllNoPIDsPions, StdNoPIDsDownPions,
                                StdLoosePions, StdLooseDownMuons,
                                StdLooseMergedPi0, StdLooseResolvedPi0, StdLooseDalitzPi0)
                                
#from CommonParticles.StdLooseDalitzPi0 import (StdLoosePi02gee, StdLoosePi024e)
                                
_monitor = False

default_config = {
  'NAME': 'RareStrange',
  'WGs' : ['RD'],
  'BUILDERTYPE' : 'RareStrangeLinesConf',
  'STREAMS' : [ 'Leptonic' ],
  'CONFIG' : {
        'Postscale'           :1,
        'SigmaPMuMuPrescale' : 1,
        'SigmaPMuMuDownPrescale' :1,
        'SigmaPMuMuDetPrescale' : 1,
        'SigmaPEMuPrescale' : 1,
        'SigmaPEEPrescale' : 1,
        'SigmaPEEDetPrescale' : 1,
        'SigmaPPi0Prescale' : 1,
        'SigmaPPi0CalPrescale' : 1.,
        'SigmaPEEDownPrescale' : 0.1,
        'SigmaPMuMuLFVPrescale' :1,
        'SigmaPMuMuLFVDownPrescale' :0.1,
        'SigmaMuMuMuPrescale' : 1,
        'SigmaMuMuMuDownPrescale' :1,   
        'KPiPiPiPrescale' : 0.01,
        'KPiPiPiMassMeasPrescale' :1,
        'KPiMuMuPrescale' :1,
        'KPiMuMuLFVPrescale' :1,
        'KPiMuMuDownPrescale' :1,
        'KPiPiPiDownPrescale' : 0.1,
        'KPiPiPiMassMeasDownPrescale' :1,
        'KPiMuMuLFVDownPrescale' :1,
        'PhiKMuPrescale' : 0.01,
        'LambdaPPiPrescale' : 0.01 ,
        'LambdaPPiEEPrescale' : 1. ,
        'SigmaMinTauPs' : 6.,
        'SigmaMinPt' : 500.,
        'SigmaMaxDOCA' : 2.,
        'SigmaMassWin' : 500.,
        'SigmaMinDIRA' : 0.9,
        'SigmaMaxIpChi2' : 36.,
        'SigmaVtxChi2' : 36.,
        'SigmaMinTauPsDown' : 7.,
        'SigmaMinPtDown' : 0.,
        'SigmaMaxDOCADown' : 10.,
        'SigmaMassWinDown' : 500.,
        'SigmaPEEMassWinDown' : 100.,
        'SigmaMinDIRADown' : 0.9,
        'SigmaMaxIpChi2Down' : 25.,
        'SigmaVtxChi2Down' : 25.,
        'SigmaDauTrChi2Down': 9.,
        'SigmaPPi0MassWin' : 150.,
        'muonMinIpChi2' : 9.,
        'protonProbNNp': 0.05,
        'protonProbNNpTight': 0.5,
        'pMinPt': 500.,
        'electronPIDe':2.,
        'electronMinIpChi2': 9.,
        'pi0MinPt': 700.,
        'muonMinIpChi2Down' : 9.,
        'electronMinIpChi2Down': 4.,
        'DiMuonMaxMass' : 700,
        'DiMuonMaxDOCA' : 2,
        'DiMuonMinPt'   : 0,
        'DiMuonVtxChi2'  : 36,
        'DiMuonMinIpChi2'   : 9,
        'DiElectronMaxMass' : 1000,
        'DiElectronVtxChi2'  : 36,
        'DiElectronMinIpChi2'   : 9,
        'DiElectronPIDe' : 2, 
        'SigmaDetVtxChi2'   : 25,
        'Sigma3MinTauPs' : 3,
        'Sigma3MinPt' : 0.,
        'Sigma3MaxDOCA' : 2.,
        'Sigma3MassWin' : 500.,
        'Sigma3MinDIRA' : 0.9,
        'Sigma3MaxIpChi2' : 36.,
        'Sigma3VtxChi2' : 36.,
        'Sigma3MinTauPsDown' : 2,
        'Sigma3MinPtDown' : 0.,
        'Sigma3MaxDOCADown' : 2.,
        'Sigma3MassWinDown' : 500.,
        'Sigma3MinDIRADown' : 0.1,
        'Sigma3MaxIpChi2Down' : 100.,
        'Sigma3VtxChi2Down' : 100.,
        'Sigma3DauTrChi2Down': 9.,
        'muon3MinIpChi2' : 5.,
        'muon3MinIpChi2Down' : 5.,
        # Kaons 
        'KMaxDOCA' : 3.,
        'KMinPT' : 100. ,
        'KMassWin' : 100.,
        'KMinDIRA' : 0.98,
        'KMaxIpChi2' : 25.,
        'KVtxChi2': 25.,
        'KMinVDChi2' : 36.,
        'KDauMinIpChi2' : 9.,
        'KDauTrChi2' : 3.,
        'KMaxDOCADown' : 10.,
        'KMinPTDown' : 0. ,
        'KMassWinDown' : 100.,
        'KMinDIRADown' : 0.98,
        'KVtxChi2Down': 25.,
        'KMinVDChi2Down' : 49.,
        'KDauMinIpChi2Down' : 5.,
        'KVDPVMinDown' : 500.,
        'KVDPVMaxDown' : 2500.,
        #Kaons stuff
        'KMaxDOCAMassMeas' : 2.,
        'KMinPTMassMeas' : 300. ,
        'KMassWinMassMeas' : 50.,
        'KMinDIRAMassMeas' : 0.9998,
        'KMaxIpChi2MassMeas' : 25.,
        'KVtxChi2MassMeas': 10.,
        'KMinVDChi2MassMeas' : 100.,
        'KDauMinIpChi2MassMeas' : 8., 
        #Kaons stuff downstream
        'KMaxDOCAMassMeasDown' : 2.,
        'KMinPTMassMeasDown' : 250. ,
        'KMassWinMassMeasDown' : 100.,
        'KMinDIRAMassMeasDown' : 0.999,
        'KVtxChi2MassMeasDown': 20.,
        'KMinVDChi2MassMeasDown' : 64.,
        'KDauMinIpChi2MassMeasDown' : 4., 
        'KVDPVMinMassMeasDown' : 900.,
        'KVDPVMaxMassMeasDown' : 2200.,
        # Phi lines
        'PhiDauMinPT' :  400.,
        'PhiMassMin' : 800,
        'PhiMassMax' : 1200,
        'PhiMaxDOCA' : 0.1,
        'PhiMinDIRA' : 0.5,
        'PhiMinPT'   : 700,
        'PhiVtxChi2' : 9 ,
        'PhiProbNNk' : 0.3,
        #Lambda lines 
        'LambdaMinTauPs' : 6.,
        'LambdaMinPt' : 500.,
        'LambdaMaxDOCA' : 2.,
        'LambdaMassWin' : 500.,
        'LambdaMassWinTight' : 50.,
        'LambdaMinDIRA' : 0.9,
        'LambdaMaxIpChi2' : 36.,
        'LambdaVtxChi2' : 25.,
        'pionMinIpChi2' : 9.,
        'protonMinIpChi2' : 9.,
        
        }
  }
  



#============================================================
# Line builder 
class RareStrangeLinesConf(LineBuilder) :
    """
    Builder 
 
    """

    __configuration_keys__ = (    
        'Postscale'           ,
        'SigmaPMuMuPrescale',
        'SigmaPMuMuDownPrescale',
        'SigmaPMuMuDetPrescale',
        'SigmaPEMuPrescale',
        'SigmaPEEPrescale',
        'SigmaPEEDetPrescale' ,
        'SigmaPPi0Prescale',
        'SigmaPPi0CalPrescale',
        'SigmaPEEDownPrescale',
        'SigmaPMuMuLFVPrescale',
        'SigmaPMuMuLFVDownPrescale',
        'SigmaMuMuMuPrescale',
        'SigmaMuMuMuDownPrescale',
        'LambdaPPiPrescale',
        'LambdaPPiEEPrescale' ,
        'KPiPiPiPrescale',
        'KPiPiPiMassMeasPrescale',
        'KPiMuMuPrescale',
        'KPiMuMuLFVPrescale',
        'KPiMuMuDownPrescale',
        'KPiPiPiDownPrescale',
        'KPiPiPiMassMeasDownPrescale',
        'KPiMuMuLFVDownPrescale',
        'PhiKMuPrescale', 
        'SigmaMinTauPs',
        'SigmaMinPt',
        'SigmaMaxDOCA',
        'SigmaMassWin',
        'SigmaMinDIRA',
        'SigmaMaxIpChi2',
        'SigmaVtxChi2',
        'SigmaMinTauPsDown',
        'SigmaMinPtDown',
        'SigmaMaxDOCADown',
        'SigmaMassWinDown',
        'SigmaPEEMassWinDown',
        'SigmaMinDIRADown',
        'SigmaMaxIpChi2Down',
        'SigmaVtxChi2Down',
        'SigmaDauTrChi2Down',
        'SigmaPPi0MassWin',
        'muonMinIpChi2',
        'protonProbNNp',
        'protonProbNNpTight',
        'pMinPt',
        'electronPIDe',
        'electronMinIpChi2',
        'muonMinIpChi2Down',
        'pi0MinPt',
        'electronMinIpChi2Down',
        'DiMuonMaxMass' ,
        'DiMuonMaxDOCA' ,
        'DiMuonMinPt'   ,
        'DiMuonVtxChi2' ,
        'DiMuonMinIpChi2'   ,
        'DiElectronMaxMass' ,
        'DiElectronVtxChi2'  ,
        'DiElectronMinIpChi2',
        'DiElectronPIDe' , 
        'SigmaDetVtxChi2' ,
        'Sigma3MinTauPs',
        'Sigma3MinPt',
        'Sigma3MaxDOCA',
        'Sigma3MassWin',
        'Sigma3MinDIRA',
        'Sigma3MaxIpChi2',
        'Sigma3VtxChi2',
        'Sigma3MinTauPsDown',
        'Sigma3MinPtDown',
        'Sigma3MaxDOCADown',
        'Sigma3MassWinDown',
        'Sigma3MinDIRADown',
        'Sigma3MaxIpChi2Down',
        'Sigma3VtxChi2Down',
        'Sigma3DauTrChi2Down',
        'muon3MinIpChi2',
        'muon3MinIpChi2Down',                
        'KMaxDOCA',
        'KMinPT',
        'KMassWin',
        'KMinDIRA',
        'KMaxIpChi2',
        'KVtxChi2',
        'KMinVDChi2',
        'KDauMinIpChi2',
        'KDauTrChi2',
        'KMaxDOCADown',
        'KMinPTDown',
        'KMassWinDown',
        'KMinDIRADown',
        'KVtxChi2Down',
        'KMinVDChi2Down',
        'KDauMinIpChi2Down',
        'KVDPVMinDown',
        'KVDPVMaxDown',
        'KMaxDOCAMassMeas',
        'KMinPTMassMeas',
        'KMassWinMassMeas',
        'KMinDIRAMassMeas',
        'KMaxIpChi2MassMeas',
        'KVtxChi2MassMeas',
        'KMinVDChi2MassMeas',
        'KDauMinIpChi2MassMeas', 
        'KMaxDOCAMassMeasDown',
        'KMinPTMassMeasDown',
        'KMassWinMassMeasDown',
        'KMinDIRAMassMeasDown',
        'KVtxChi2MassMeasDown',
        'KMinVDChi2MassMeasDown',
        'KDauMinIpChi2MassMeasDown',
        'KVDPVMinMassMeasDown',
        'KVDPVMaxMassMeasDown',
        'PhiDauMinPT' ,
        'PhiMassMin' ,
        'PhiMassMax' ,
        'PhiMaxDOCA' ,
        'PhiMinDIRA' ,
        'PhiMinPT'   ,
        'PhiVtxChi2'  ,
        'PhiProbNNk',
        'LambdaMinTauPs' ,
        'LambdaMinPt' ,
        'LambdaMaxDOCA' ,
        'LambdaMassWin' ,
        'LambdaMassWinTight' ,
        'LambdaMinDIRA' ,
        'LambdaMaxIpChi2',
        'LambdaVtxChi2' ,
        'pionMinIpChi2' ,
        'protonMinIpChi2' ,
        )
    
    #### This is the dictionary of all tunable cuts ########
    
    
    
    def __init__(self, 
                 name = 'RareStrange',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        

        spmumu_name=name+'SigmaPMuMu'
        spmumudown_name=name+'SigmaPMuMuDown'
        dimudet_name=name+'DiMuonDetached'
        dielectrondet_name=name+'DiElectronDetached'
        spmumudet_name=name+'SigmaPMuMuDet'
        spemu_name=name+'SigmaPEMu'
        spee_name=name+'SigmaPEE'
        speedet_name=name+'SigmaPEEDet'
        sppi0_name=name+'SigmaPPi0'
        sppi0cal_name=name+'SigmaPPi0Cal'
        speedown_name=name+'SigmaPEEDown'
        spmumulfv_name=name+'SigmaPMuMuLFV'
        spmumulfvdown_name=name+'SigmaPMuMuLFVDown'
        smumumu_name=name+'SigmaMuMuMu'
        smumumudown_name=name+'SigmaMuMuMuDown'                        
        kpimumu_name=name+'KPiMuMu'
        kpimumulfv_name=name+'KPiMuMuLFV'
        kpipipi_name=name+'KPiPiPi'
        kpipipimassmeas_name=name+'KPiPiPiMassMeas'
        kpimumudown_name=name+'KPiMuMuDown'
        kpimumulfvdown_name=name+'KPiMuMuLFVDown'
        kpipipidown_name=name+'KPiPiPiDown'
        kpipipimassmeasdown_name=name+'KPiPiPiMassMeasDown'
        phikmu_name = name +"PhiKMu"
        lambdappi_name=name+'LambdaPPi'
        lambdappiee_name=name+'LambdaPPiEE'

        self._myDiMuons = makeDiMuonDetached(dimudet_name, config)
        self._myDiElectrons = makeDiElectronDetached(dielectrondet_name, config)
        self._stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
        self._stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
        
        self.selSigmaPMuMu = makeSigmaPMuMu(spmumu_name, config)
        self.selSigmaPMuMuDown = makeSigmaPMuMuDown(spmumudown_name, config)
        self.selSigmaPMuMuDet = makeSigmaPMuMuDet(spmumudet_name, config, [self._myDiMuons, self._stdLooseProtons])
        self.selSigmaPEMu = makeSigmaPEMu(spemu_name, config)
        self.selSigmaPEE   = makeSigmaPEE(spee_name, config)
        self.selSigmaPEEDet   = makeSigmaPEEDetached(speedet_name, config, [self._myDiElectrons, self._stdLooseProtons])
        self.selSigmaPPi0   = makeSigmaPPi0(sppi0_name, config)
        self.selSigmaPPi0Cal   = makeSigmaPPi0Cal(sppi0cal_name, config)
        self.selSigmaPEEDown   = makeSigmaPEEDown(speedown_name, config)
        self.selSigmaPMuMuLFV = makeSigmaPMuMuLFV(spmumulfv_name, config)
        self.selSigmaPMuMuLFVDown = makeSigmaPMuMuLFVDown(spmumulfvdown_name, config)
        self.selSigmaMuMuMu = makeSigmaMuMuMu(smumumu_name, config)
        self.selSigmaMuMuMuDown = makeSigmaMuMuMuDown(smumumudown_name, config)
        self.selKPiMuMu = makeKPiMuMu(kpimumu_name, config)
        self.selKPiMuMuLFV = makeKPiMuMuLFV(kpimumulfv_name, config)
        self.selKPiPiPi = makeKPiPiPi(kpipipi_name, config)
        self.selKPiPiPiMassMeas = makeKPiPiPiMassMeas(kpipipimassmeas_name, config)
        self.selKPiMuMuDown = makeKPiMuMuDown(kpimumudown_name, config)
        self.selKPiMuMuLFVDown = makeKPiMuMuLFVDown(kpimumulfvdown_name, config)
        self.selKPiPiPiDown = makeKPiPiPiDown(kpipipidown_name, config)
        self.selKPiPiPiMassMeasDown = makeKPiPiPiMassMeasDown(kpipipimassmeasdown_name, config)
        self.selPhiKMu = makePhiKMu(phikmu_name, config)
        self.selLambdaPPi = makeLambdaPPi(lambdappi_name, config)
        self.selLambdaPPiEE = makeLambdaPPiEE(lambdappiee_name, config, [self._myDiElectrons, self._stdLooseProtons, self._stdAllLoosePions])

               
        self.SigmaPMuMuLine = StrippingLine(spmumu_name+"Line",
                                            prescale = config['SigmaPMuMuPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPMuMu ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu09_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu09_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu10_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu10_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu11_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu11_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )
        self.SigmaPMuMuDownLine = StrippingLine(spmumudown_name+"Line",
                                            prescale = config['SigmaPMuMuDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPMuMuDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu09_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu09_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu10_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu10_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^mu+ mu-]CC" : 'P2CVMu11_1', 
                                                            "[Sigma+ -> p+ mu+ ^mu-]CC" : 'P2CVMu11_2',
                                                            "[Sigma+ -> ^p+ mu+ mu-]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )

        self.SigmaPMuMuDetLine = StrippingLine(spmumudet_name+"Line",
                                            prescale = config['SigmaPMuMuDetPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPMuMuDet ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ (KS0 -> ^mu+ mu-)]CC" : 'P2CVMu09_1', 
                                                            "[Sigma+ -> p+ (KS0 -> mu+ ^mu-)]CC" : 'P2CVMu09_2',
                                                            "[Sigma+ -> ^p+ (KS0 -> mu+ mu-)]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ (KS0 -> ^mu+ mu-)]CC" : 'P2CVMu10_1',  
                                                            "[Sigma+ -> p+ (KS0 -> mu+ ^mu-)]CC" : 'P2CVMu10_2',
                                                            "[Sigma+ -> ^p+ (KS0 -> mu+ mu-)]CC" : 'P2CVProton10'

                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                          "[Sigma+ -> p+ (KS0 -> ^mu+ mu-)]CC" : 'P2CVMu11_1',  
                                                            "[Sigma+ -> p+ (KS0 -> mu+ ^mu-)]CC" : 'P2CVMu11_2',
                                                            "[Sigma+ -> ^p+ (KS0 -> mu+ mu-)]CC" : 'P2CVProton11'

                                                }
                                              }
                                            ]
                                     )
        self.SigmaPEMuLine = StrippingLine(spemu_name+"Line",
                                            prescale = config['SigmaPEMuPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPEMu ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[[Sigma+ -> p+ ^e+ mu-]CC, [Sigma+ -> p+ ^e- mu+]CC] " : 'P2CVE09', 
                                                            "[[Sigma+ -> p+ e+ ^mu-]CC, [Sigma+ -> p+ e- ^mu+]CC]" : 'P2CVMu09',
                                                            "[[Sigma+ -> ^p+ e+ mu-]CC, [Sigma+ -> ^p+ e- mu+]CC]" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : { "[[Sigma+ -> p+ ^e+ mu-]CC, [Sigma+ -> p+ ^e- mu+]CC] " : 'P2CVE10', 
                                                                        "[[Sigma+ -> p+ e+ ^mu-]CC, [Sigma+ -> p+ e- ^mu+]CC]" : 'P2CVMu10',
                                                                        "[[Sigma+ -> ^p+ e+ mu-]CC, [Sigma+ -> ^p+ e- mu+]CC]" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : { "[[Sigma+ -> p+ ^e+ mu-]CC, [Sigma+ -> p+ ^e- mu+]CC] " : 'P2CVE10',
                                                                        "[[Sigma+ -> p+ e+ ^mu-]CC, [Sigma+ -> p+ e- ^mu+]CC]" : 'P2CVMu10',
                                                                        "[[Sigma+ -> ^p+ e+ mu-]CC, [Sigma+ -> ^p+ e- mu+]CC]" : 'P2CVProton10'

                                                }
                                              }
                                            ]
                                     )

        self.SigmaPEELine = StrippingLine(spee_name+"Line",
                                            prescale = config['SigmaPEEPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPEE ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE09_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE09_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE10_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE10_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE11_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE11_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                          )

        self.SigmaPEEDetLine = StrippingLine(speedet_name+"Line",
                                            prescale = config['SigmaPEEDetPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPEEDet ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ (KS0 -> ^e+ e-)]CC" : 'P2CVE09_1', 
                                                            "[Sigma+ -> p+ (KS0 -> e+ ^e-)]CC" : 'P2CVE09_2',
                                                            "[Sigma+ -> ^p+ (KS0 -> e+ e-)]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                           "[Sigma+ -> p+ (KS0 -> ^e+ e-)]CC" : 'P2CVE10_1', 
                                                           "[Sigma+ -> p+ (KS0 -> e+ ^e-)]CC" : 'P2CVE10_2',
                                                           "[Sigma+ -> ^p+ (KS0 -> e+ e-)]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                           "[Sigma+ -> p+ (KS0 -> ^e+ e-)]CC" : 'P2CVE11_1', 
                                                           "[Sigma+ -> p+ (KS0 -> e+ ^e-)]CC" : 'P2CVE11_2',
                                                           "[Sigma+ -> ^p+ (KS0 -> e+ e-)]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                          )
        self.SigmaPPi0Line = StrippingLine(sppi0_name+"Line",
                                           prescale = config['SigmaPPi0Prescale'],
                                           postscale = config['Postscale'],
                                           algos = [ self.selSigmaPPi0 ],
                                           RequiredRawEvents = ["Velo"],
                                           MDSTFlag=True,
                                           RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi009', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi010', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi011', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )
        self.SigmaPPi0CalLine = StrippingLine(sppi0cal_name+"Line",
                                           prescale = config['SigmaPPi0CalPrescale'],
                                           postscale = config['Postscale'],
                                           algos = [ self.selSigmaPPi0Cal ],
                                           RequiredRawEvents = ["Velo"],
                                           MDSTFlag=True,
                                           RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi009', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi010', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^pi0]CC" : 'P2CVPi011', 
                                                            "[Sigma+ -> ^p+ pi0]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )


        self.SigmaPEEDownLine = StrippingLine(speedown_name+"Line",
                                            prescale = config['SigmaPEEDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPEEDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                             RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE09_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE09_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE10_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE10_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p+ ^e+ e-]CC" : 'P2CVE11_1', 
                                                            "[Sigma+ -> p+ e+ ^e-]CC" : 'P2CVE11_2',
                                                            "[Sigma+ -> ^p+ e+ e-]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )
        self.SigmaPMuMuLFVLine = StrippingLine(spmumulfv_name+"Line",
                                               prescale = config['SigmaPMuMuLFVPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selSigmaPMuMuLFV ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu09_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu09_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu10_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu10_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu11_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu11_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )

        self.SigmaPMuMuLFVDownLine = StrippingLine(spmumulfvdown_name+"Line",
                                               prescale = config['SigmaPMuMuLFVDownPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selSigmaPMuMuLFVDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu09_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu09_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu10_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu10_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Sigma+ -> p~- ^mu+ mu+]CC" : 'P2CVMu11_1', 
                                                            "[Sigma+ -> p~- mu+ ^mu+]CC" : 'P2CVMu11_2',
                                                            "[Sigma+ -> ^p~- mu+ mu+]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )


        self.SigmaMuMuMuLine = StrippingLine(smumumu_name+"Line",
                                             prescale = config['SigmaMuMuMuPrescale'],
                                             postscale = config['Postscale'],
                                             algos = [ self.selSigmaMuMuMu ],
                                             RequiredRawEvents = ["Velo"],
                                             MDSTFlag=False,
                                             RelatedInfoTools =
                                             [{'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                               'Location' : 'P2CVSigma09',
                                               'DaughterLocations' : {"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu09_1',
                                                                      "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu09_2',
                                                                      "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu09_3'}},
                                              {'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                               'Location' : 'P2CVSigma10',
                                               'DaughterLocations' :{"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu10_1',
                                                                     "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu10_2',
                                                                     "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu10_3'}},
                                              {'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                               'Location' : 'P2CVSigma11',
                                               'DaughterLocations' :{"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu11_1',
                                                                     "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu11_2',
                                                                     "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu11_3'}}])
                                              
        self.SigmaMuMuMuDownLine = StrippingLine(smumumudown_name+"Line",
                                                 prescale = config['SigmaMuMuMuDownPrescale'],
                                                 postscale = config['Postscale'],
                                                 algos = [ self.selSigmaMuMuMuDown ],
                                                 RequiredRawEvents = ["Velo"],
                                                 MDSTFlag=False,
                                                 RelatedInfoTools =
                                                 [{'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                   'Location' : 'P2CVSigma09',
                                                   'DaughterLocations' : {"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu09_1',
                                                                          "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu09_2',
                                                                          "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu09_3'}},
                                                  {'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                   'Location' : 'P2CVSigma10',
                                                   'DaughterLocations' :{"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu10_1',
                                                                         "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu10_2',
                                                                         "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu10_3'}},
                                                  {'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'],
                                                   'Location' : 'P2CVSigma11',
                                                   'DaughterLocations' :{"[Sigma+ -> ^mu+ mu+ mu-]CC":'P2CVMu11_1',
                                                                         "[Sigma+ -> mu+ ^mu+ mu-]CC":'P2CVMu11_2',
                                                                         "[Sigma+ -> mu+ mu+ ^mu-]CC":'P2CVMu11_3'}}])
        
        self.KPiMuMuLine = StrippingLine(kpimumu_name+"Line",
                                             prescale = config['KPiMuMuPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiMuMu ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False, 
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu09_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu09_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu10_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu10_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu11_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu11_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
        self.KPiMuMuLFVLine = StrippingLine(kpimumulfv_name+"Line",
                                         prescale = config['KPiMuMuLFVPrescale'],
                                         postscale = config['Postscale'],
                                         algos = [ self.selKPiMuMuLFV ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu09_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu09_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu10_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu10_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu11_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu11_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                         )
        
        self.KPiPiPiLine = StrippingLine(kpipipi_name+"Line",
                                             prescale = config['KPiPiPiPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPi ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi09_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi09_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi10_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi10_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi11_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi11_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiMassMeasLine = StrippingLine(kpipipimassmeas_name+"Line",
                                             prescale = config['KPiPiPiMassMeasPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiMassMeas ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi09_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi09_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi10_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi10_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi11_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi11_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
        self.KPiMuMuDownLine = StrippingLine(kpimumudown_name+"Line",
                                             prescale = config['KPiMuMuDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiMuMuDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu09_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu09_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu10_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu10_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi+ ^mu+ mu-]CC" : 'P2CVMu11_1', 
                                                            "[K+ -> pi+ mu+ ^mu-]CC" : 'P2CVMu11_2',
                                                            "[K+ -> ^pi+ mu+ mu-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiDownLine = StrippingLine(kpipipidown_name+"Line",
                                             prescale = config['KPiPiPiDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi09_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi09_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi10_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi10_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi11_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi11_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiMassMeasDownLine = StrippingLine(kpipipimassmeasdown_name+"Line",
                                             prescale = config['KPiPiPiMassMeasDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiMassMeasDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi09_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi09_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi10_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi10_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> ^pi+ pi+ pi-]CC" : 'P2CVPi11_1', 
                                                            "[K+ -> pi+ ^pi+ pi-]CC" : 'P2CVPi11_2',
                                                            "[K+ -> pi+ pi+ ^pi-]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                     )
     
        self.KPiMuMuLFVDownLine = StrippingLine(kpimumulfvdown_name+"Line",
                                         prescale = config['KPiMuMuLFVDownPrescale'],
                                         postscale = config['Postscale'],
                                         algos = [ self.selKPiMuMuLFVDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu09_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu09_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu10_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu10_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[K+ -> pi- ^mu+ mu+]CC" : 'P2CVMu11_1', 
                                                            "[K+ -> pi- mu+ ^mu+]CC" : 'P2CVMu11_2',
                                                            "[K+ -> ^pi- mu+ mu+]CC" : 'P2CVPi11'
                                                }
                                              }
                                            ]
                                         )

        self.PhiKMuLine = StrippingLine(phikmu_name+"Line",
                                        prescale = config['PhiKMuPrescale'],
                                        postscale = config['Postscale'],
                                        algos = [ self.selPhiKMu ],
                                        RequiredRawEvents = ["Velo"],
                                        MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK09',
                                                'DaughterLocations' : {
                                                            "[phi(1020) -> K+ ^mu-]CC" : 'P2CVMu09', 
                                                            "[phi(1020) -> ^K+ mu-]CC" : 'P2CVK09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK10',
                                                'DaughterLocations' : {
                                                            "[phi(1020) -> K+ ^mu-]CC" : 'P2CVMu10', 
                                                            "[phi(1020) -> ^K+ mu-]CC" : 'P2CVK10'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVK11',
                                                'DaughterLocations' : {
                                                            "[phi(1020) -> K+ ^mu-]CC" : 'P2CVMu11', 
                                                            "[phi(1020) -> ^K+ mu-]CC" : 'P2CVK11'
                                                }
                                              }
                                            ]
                                         )

        self.LambdaPPiLine = StrippingLine(lambdappi_name+"Line",
                                            prescale = config['LambdaPPiPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selLambdaPPi ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi-]CC" : 'P2CVPi09', 
                                                            "[Lambda0 -> ^p+ pi-]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi-]CC" : 'P2CVPi10', 
                                                            "[Lambda0 -> ^p+ pi-]CC" : 'P2CVProton10'
                                                }
                                                
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi-]CC" : 'P2CVPi11', 
                                                            "[Lambda0 -> ^p+ pi-]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )

        self.LambdaPPiEELine = StrippingLine(lambdappiee_name+"Line",
                                            prescale = config['LambdaPPiEEPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selLambdaPPiEE ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=False,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma09',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi- KS0]CC" : 'P2CVPi09', 
                                                            "[Lambda0 -> ^p+ pi- KS0]CC" : 'P2CVProton09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma10',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi- KS0]CC" : 'P2CVPi10', 
                                                            "[Lambda0 -> ^p+ pi- KS0]CC" : 'P2CVProton10'
                                                }
                                                
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'Location' : 'P2CVSigma11',
                                                'DaughterLocations' : {
                                                            "[Lambda0 -> p+ ^pi- KS0]CC" : 'P2CVPi11', 
                                                            "[Lambda0 -> ^p+ pi- KS0]CC" : 'P2CVProton11'
                                                }
                                              }
                                            ]
                                     )



        self.registerLine(self.SigmaPMuMuLine)
        self.registerLine(self.SigmaPEELine)
        self.registerLine(self.SigmaPEEDetLine)
        self.registerLine(self.SigmaPEMuLine)
        self.registerLine(self.SigmaPMuMuDetLine)
        self.registerLine(self.SigmaPPi0Line)
        self.registerLine(self.SigmaPPi0CalLine)
        self.registerLine(self.SigmaPEEDownLine)
        self.registerLine(self.SigmaPMuMuLFVLine)
        self.registerLine(self.SigmaPMuMuLFVDownLine)
        self.registerLine(self.SigmaPMuMuDownLine)
        self.registerLine(self.SigmaMuMuMuLine)
        self.registerLine(self.SigmaMuMuMuDownLine)
        self.registerLine(self.KPiMuMuLine)
        self.registerLine(self.KPiMuMuLFVLine)
        self.registerLine(self.KPiMuMuLFVDownLine)
        self.registerLine(self.KPiPiPiLine)
        self.registerLine(self.KPiPiPiMassMeasLine)
        self.registerLine(self.KPiMuMuDownLine)
        self.registerLine(self.KPiPiPiDownLine)
        self.registerLine(self.KPiPiPiMassMeasDownLine)
        self.registerLine(self.PhiKMuLine)
        self.registerLine(self.LambdaPPiLine)
        self.registerLine(self.LambdaPPiEELine)



#============================================================
# Sigma+ -> p mu mu 
def makeSigmaPMuMu(name, config):
    """
    Line for the selection of Sigma+ -> p+ mu+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    #from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> p+ mu+ mu-]cc",
                                    DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config, 
                                                     "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdLooseProtons ])

#============================================================
# Sigma+ > p- mu+ mu+ 
def makeSigmaPMuMuLFV(name, config):
    """
    Line for the selection of Sigma+ -> p- mu+ mu+ 
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    SigmaPMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> p~- mu+ mu+]cc",
                                    DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                                        "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                                "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                                                "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                                "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                                "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  

                                )
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdLooseProtons ])
#============================================================
#  Sigma+ -> p mu mu  with detached dimuon
def makeDiMuonDetached(name, config):
    """
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """

    myDiMuon = CombineParticles(  DecayDescriptor = "KS0 -> mu+ mu-",
                                  DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config },
                                  CombinationCut = "(AM <%(DiMuonMaxMass)s *MeV) & (AMAXDOCA('')< %(DiMuonMaxDOCA)s *mm)"%config,
                                  MotherCut = "(VFASPF(VCHI2/VDOF)< %(DiMuonVtxChi2)s)  & (PT> %(DiMuonMinPt)s *MeV)" %config  
                                )
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    
    return Selection (name,
                      Algorithm = myDiMuon,
                      RequiredSelections = [ _stdAllLooseMuons ])



def makeSigmaPMuMuDet(name, config, inputs):
    """
    Line for the selection of Sigma+ -> p+ mu+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """

    #from Configurables import OfflineVertexFitter
    SigmaPMuMuDet = CombineParticles(  DecayDescriptor = "[Sigma+ -> p+ KS0 ]cc",
                                       DaughtersCuts = { "KS0" : "(MIPCHI2DV(PRIMARY)>%(DiMuonMinIpChi2)s)"%config, 
                                                         "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                       CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV)"%config,
                                       MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaDetVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                       "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                                       "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                       "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                       "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                       )
    
    return Selection (name,
                      Algorithm = SigmaPMuMuDet,
                      RequiredSelections = inputs )

#============================================================
#  Very soft Dielectron for Sigma-> p ee  and Lambda-> p pi e e 

def makeDiElectronDetached(name, config):
    from CommonParticles.Utils import trackSelector 
    from Configurables import DiElectronMaker,ProtoParticleCALOFilter
    from GaudiKernel.SystemOfUnits import MeV
    dieLL = DiElectronMaker('MyDiElectron')
    dieLL.Particle = "KS0"
    selector = trackSelector ( dieLL , trackTypes = [ "Long"]) 
    dieLL.addTool( ProtoParticleCALOFilter('Electron'))
#    dieLL.Electron.Selection = ["RequiresDet='CALO' CombDLL(e-pi)>'%(DiElectronPIDe)s'"%config ]
    dieLL.Electron.Selection = ["RequiresDet='CALO' PROBNNe>'0.2'" ]
    
    dieLL.DiElectronMassMax = 2000.*MeV
    dieLL.DiElectronMassMin = 0.*MeV
    dieLL.DiElectronPtMin =   0.*MeV

    return Selection (name,
                      Algorithm = dieLL
                      )

#============================================================
#  Sigma+ -> p e e 
def makeSigmaPEEDetached(name, config, inputs):
    """
    Line for the selection of Sigma+ -> p+ e+ e-, 
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPEE = CombineParticles(    DecayDescriptor = "[Sigma+ -> p+ KS0]cc",
                                    DaughtersCuts = {  "KS0" : "(MIPCHI2DV(PRIMARY)>%(DiElectronMinIpChi2)s)"%config, 
                                                       "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                                "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                                                "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                                "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                                "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )

    return Selection (name,
                      Algorithm = SigmaPEE,
                      RequiredSelections = inputs )

#============================================================
#  Sigma+ -> p e e 
def makeSigmaPEE(name, config):
    """
    Line for the selection of Sigma+ -> p+ e+ e- 
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPEE = CombineParticles(    DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc",
                                    DaughtersCuts = { "e+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDe > %(electronPIDe)s ) &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2)s)"%config,
                                                    "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                                "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                                                "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                                "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                                "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )
    # AllLooseElectrons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseElectrons   = DataOnDemand(Location = "Phys/StdAllLooseElectrons/Particles")
    
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")

    
    return Selection (name,
                      Algorithm = SigmaPEE,
                      RequiredSelections = [ _stdAllLooseElectrons,
                                             _stdLooseProtons ])

#============================================================
# Sigma+ -> p e mu 
def makeSigmaPEMu(name, config):
    """
    Line for the selection of Sigma+ -> p+ e+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    #from Configurables import OfflineVertexFitter
    SigmaPEMu = CombineParticles(  DecayDescriptors = [ "[Sigma+ -> p+ e+ mu-]cc","[Sigma+ -> p+ mu+ e-]cc"],
                                    DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                                      "e+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config, 
                                                     "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    _stdAllLooseElectrons   = DataOnDemand(Location = "Phys/StdAllLooseElectrons/Particles")
    return Selection (name,
                      Algorithm = SigmaPEMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdAllLooseElectrons,
                                             _stdLooseProtons ])



#============================================================
#  Sigma+ -> p pi0 with Dalitz 
def makeSigmaPPi0(name, config):
    """
    Line for the selection of Sigma+ -> p+ pi0
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    pi0sel = MergedSelection("Pi0For" + name, RequiredSelections = [  StdLooseDalitzPi0,
                                                                      #StdLoosePi024e, StdLoosePi02gee
                                                                      ] ) 

    
    SigmaPPi0 = CombineParticles(   DecayDescriptor = "[Sigma+ -> p+ pi0]cc",
                                    DaughtersCuts = { "pi0" : "(PT > %(pi0MinPt)s)"%config,
                                    "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp > %(protonProbNNpTight)s )"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPPi0MassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                                "& (ADMASS('Sigma+') < %(SigmaPPi0MassWin)s *MeV )"\
                                                "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                                "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                                "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )

#    _stdPizeros   =  DataOnDemand(Location = "Phys/StdLooseMergedPi0/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPPi0,
                      RequiredSelections = [ pi0sel,
                                             _stdLooseProtons ])

#============================================================
#  Sigma+ -> p pi0 with Merged and Resolved pi0
def makeSigmaPPi0Cal(name, config):
    """
    Line for the selection of Sigma+ -> p+ pi0
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    pi0sel = MergedSelection("Pi0For" + name, RequiredSelections = [ StdLooseMergedPi0, StdLooseResolvedPi0 ] ) 

    
    SigmaPPi0 = CombineParticles(   DecayDescriptor = "[Sigma+ -> p+ pi0]cc",
                                    DaughtersCuts = { "pi0" : "(PT > %(pi0MinPt)s)"%config,
                                    "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp > %(protonProbNNpTight)s )"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPPi0MassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                                    "& (ADMASS('Sigma+') < %(SigmaPPi0MassWin)s *MeV )"\
                                    "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                                    "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                                    "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
                                )

#    _stdPizeros   =  DataOnDemand(Location = "Phys/StdLooseMergedPi0/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPPi0,
                      RequiredSelections = [ pi0sel,
                                             _stdLooseProtons ])




#============================================================
# Sigma+ -> mu mu mu
# Contact: Xabier Cid Vidal
def makeSigmaMuMuMu(name, config):
    """
    Line for the selection of Sigma+ -> mu+ mu+ mu-,
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts
    """
    
    #from Configurables import OfflineVertexFitter
    SigmaMuMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> mu+ mu+ mu-]cc",
                                     DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muon3MinIpChi2)s)"%config },
                                     CombinationCut = "(ADAMASS('Sigma+')<%(Sigma3MassWin)s *MeV) & (AMAXDOCA('')< %(Sigma3MaxDOCA)s *mm)"%config,
                                     MotherCut = "(VFASPF(VCHI2/VDOF)< %(Sigma3VtxChi2)s)  & (PT> %(Sigma3MinPt)s *MeV)"\
                                     "& (ADMASS('Sigma+') < %(Sigma3MassWin)s *MeV )"\
                                     "& (BPVDIRA > %(Sigma3MinDIRA)s) "\
                                     "& (BPVIPCHI2()< %(Sigma3MaxIpChi2)s)"\
                                     "& (BPVLTIME()> %(Sigma3MinTauPs)s * ps)" %config
                                     )
    
    
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaMuMuMu,
                      RequiredSelections = [ _stdAllLooseMuons])


# ============================================================
# Sigma+ -> mu mu mu down
# Contact: Xabier Cid Vidal
def makeSigmaMuMuMuDown(name, config):
    """
    Line for the selection of Sigma+ -> mu+ mu+ mu-,
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts
    """
    
    #from Configurables import OfflineVertexFitter
    SigmaMuMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> mu+ mu+ mu-]cc",
                                     DaughtersCuts = { "mu+" : "(TRCHI2DOF< %(Sigma3DauTrChi2Down)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muon3MinIpChi2Down)s)"%config},
                                     CombinationCut = "(ADAMASS('Sigma+')<%(Sigma3MassWinDown)s *MeV) & (AMAXDOCA('')< %(Sigma3MaxDOCADown)s *mm)"%config,
                                     MotherCut = "(VFASPF(VCHI2/VDOF)< %(Sigma3VtxChi2Down)s)  & (PT> %(Sigma3MinPtDown)s *MeV)"\
                                     "& (ADMASS('Sigma+') < %(Sigma3MassWinDown)s *MeV )"\
                                     "& (BPVDIRA > %(Sigma3MinDIRADown)s) "\
                                     "& (BPVIPCHI2()< %(Sigma3MaxIpChi2Down)s)"\
                                     "& (BPVLTIME()> %(Sigma3MinTauPsDown)s * ps)" %config
                                     )
                                     
    
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _Muons   = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaMuMuMu,
                      RequiredSelections = [ _Muons])
        

# ============================================================
# K -> pi mu mu 
def makeKPiMuMu(name, config):
    """
    Line for the selection of K+ -> pi+ mu+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiMuMu = CombineParticles( DecayDescriptor = "[K+ -> pi+ mu+ mu-]cc",
                                DaughtersCuts = { "mu+" : "(P>3000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                                                    "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & (BPVDIRA > %(KMinDIRA)s)"\
                                    "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config
                                )
    _stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")    
    return Selection (name,
                      Algorithm = KPiMuMu,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdAllLoosePions ])

# ============================================================
# K -> pi mu mu LFV
def makeKPiMuMuLFV(name, config):
    """
    Line for the selection of K+ -> pi- mu+ mu+,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiMuMuLFV = CombineParticles(  DecayDescriptor = "[K+ -> pi- mu+ mu+]cc",
                                    DaughtersCuts = { "mu+" : "(P>3000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                                        "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config},
                                    CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config,
                                    MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & ((BPVDIRA > %(KMinDIRA)s))"\
                                        "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config
                                )
    _stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")    
    return Selection (name,
                      Algorithm = KPiMuMuLFV,
                      RequiredSelections = [ _stdAllLooseMuons,
                                             _stdAllLoosePions ])



# ============================================================
# K-> pi pi pi loose selection
def makeKPiPiPi(name, config):
    """
    Line for the selection of K+ -> pi+ pi+ pi-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiPiPi = CombineParticles( DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc",
                                DaughtersCuts = { "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & (BPVDIRA > %(KMinDIRA)s)"\
                                "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config
                            )

    _stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")

    return Selection (name,
                      Algorithm = KPiPiPi,
                      RequiredSelections = [     _stdAllLoosePions ])

# ============================================================
# K-> pi pi pi tight selection for mass measurement
def makeKPiPiPiMassMeas(name, config):
    """
    Line for the selection of K+ -> pi+ pi+ pi-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiPiPi = CombineParticles( DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc",
                                DaughtersCuts = {   "pi+":"(P>1000) & (MIPCHI2DV(PRIMARY) > 25.0) & (TRCHI2DOF<5) & (TRGHOSTPROB < 0.4)"},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWinMassMeas)s *MeV) & (AMAXDOCA('')< %(KMaxDOCAMassMeas)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPTMassMeas)s) & (ADMASS('K+') < %(KMassWinMassMeas)s *MeV) & (BPVDIRA > %(KMinDIRAMassMeas)s)"\
                                "& (VFASPF(VCHI2) < %(KVtxChi2MassMeas)s) & (BPVVDCHI2 > %(KMinVDChi2MassMeas)s) & (BPVIPCHI2()< %(KMaxIpChi2MassMeas)s )"%config
                                )

    _stdAllLoosePions = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")

    return Selection (name,
                      Algorithm = KPiPiPi,
                      RequiredSelections = [     _stdAllLoosePions ])



#============================================================
# Downstrem lines 
#============================================================
# Sigma+ -> p mu mu 
def makeSigmaPMuMuDown(name, config):
    """
    Line for the selection of Sigma+ -> p+ mu+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> p+ mu+ mu-]cc",
                                    DaughtersCuts = { "mu+" : "(TRCHI2DOF< %(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2Down)s)"%config,
                                                "p+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                                        "& (ADMASS('Sigma+') < %(SigmaMassWinDown)s *MeV )"\
                                        "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                                        "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                                        "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config  
                                    )
    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _Muons   = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")
    _Protons = DataOnDemand(Location = "Phys/StdNoPIDsDownProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _Muons,
                                             _Protons ])
                                             
def makeSigmaPMuMuLFVDown(name, config):
    """
    Line for the selection of Sigma+ -> p- mu+ mu+,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPMuMu = CombineParticles(  DecayDescriptor = "[Sigma+ -> p~- mu+ mu+]cc",
                                    DaughtersCuts = { "mu+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                    "p~-" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)"%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                                        "& (ADMASS('Sigma+') < %(SigmaMassWinDown)s *MeV )"\
                                        "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                                        "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                                        "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config  
                                    )

    # AllLooseMuons are needed in order to avoid the pt and ip cuts used in StdLooseMuons: do not change this!
    _Muons   = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")
    _Protons = DataOnDemand(Location = "Phys/StdNoPIDsDownProtons/Particles")
    
    return Selection (name,
                      Algorithm = SigmaPMuMu,
                      RequiredSelections = [ _Muons,
                                             _Protons ])


def makeSigmaPEEDown(name, config):
    """
    Line for the selection of Sigma+ -> p+ e+ e- gamma,  (with photon undetected)
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPEE = CombineParticles(    DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc",
                                    DaughtersCuts = { "e+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)  &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2Down)s)"%config,
                                    "p+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) "%config},
                                    CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPEEMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config,
                                    MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                                    "& (ADMASS('Sigma+') < %(SigmaPEEMassWinDown)s *MeV )"\
                                    "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                                    "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                                    "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config
                                )
    _stdAllLooseElectrons   = DataOnDemand(Location = "Phys/StdNoPIDsDownElectrons/Particles")
    
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdNoPIDsDownProtons/Particles")

    
    return Selection (name,
                      Algorithm = SigmaPEE,
                      RequiredSelections = [ _stdAllLooseElectrons,
                                             _stdLooseProtons ])



# Kaon lines 
def makeKPiMuMuDown(name, config):
    """
    Line for the selection of K+ -> pi+ mu+ mu-,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiMuMu = CombineParticles( DecayDescriptor = "[K+ -> pi+ mu+ mu-]cc",
                                DaughtersCuts = {  "mu+" : "(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                                                    "pi+":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWinDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCADown)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                                "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config
                                )
    
    _Muons   = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")
    _Pions = DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")
    
    return Selection (name,
                      Algorithm = KPiMuMu,
                      RequiredSelections = [ _Muons,
                                             _Pions ])
                                            
# Kaon lines 
def makeKPiMuMuLFVDown(name, config):
    """
    Line for the selection of K+ -> pi- mu+ mu+,  
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiMuMu = CombineParticles( DecayDescriptor = "[K+ -> pi- mu+ mu+]cc",
                                DaughtersCuts = { "mu+" : "(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                                "pi-":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                                "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config
                                )
    
    _Muons   = DataOnDemand(Location = "Phys/StdLooseDownMuons/Particles")
    _Pions = DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")
    
    return Selection (name,
                      Algorithm = KPiMuMu,
                      RequiredSelections = [ _Muons,
                                             _Pions ])

def makeKPiPiPiDown(name, config):
    """
    Line for the selection of K+ -> pi+ pi+ pi-
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiPiPi = CombineParticles( DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc",
                                DaughtersCuts = {  "pi+":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWinDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCADown)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                                    "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config
                                )
    
    _Pions = DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")
    
    return Selection (name,
                      Algorithm = KPiPiPi,
                      RequiredSelections = [  _Pions ])


def makeKPiPiPiMassMeasDown(name, config):
    """
    Line for the selection of K+ -> pi+ pi+ pi-
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    KPiPiPi = CombineParticles( DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc",
                                DaughtersCuts = {  "pi+":"(TRCHI2DOF<5) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2MassMeasDown)s)"%config},
                                CombinationCut = "(ADAMASS('K+') < %(KMassWinMassMeasDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCAMassMeasDown)s *mm)"%config,
                                MotherCut ="(PT> %(KMinPTMassMeasDown)s) & (ADMASS('K+') < %(KMassWinMassMeasDown)s *MeV) & (BPVDIRA > %(KMinDIRAMassMeasDown)s)"\
                                "& (VFASPF(VCHI2) < %(KVtxChi2MassMeasDown)s) & (BPVVDCHI2 > %(KMinVDChi2MassMeasDown)s)  & (BPVVDZ > %(KVDPVMinMassMeasDown)s*mm) & (BPVVDZ < %(KVDPVMaxMassMeasDown)s*mm)"%config
                                )
    
    _Pions = DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")
    
    return Selection (name,
                      Algorithm = KPiPiPi,
                      RequiredSelections = [  _Pions ])


#============================================================
# Phi-> K K with K-> mu
# Contact: Vava Gligorov
def makePhiKMu(name, config):
    """
    Line for the selection of phi-> K K with K-> mu 
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    PhiKMu = CombineParticles( DecayDescriptor = "[phi(1020) -> K+ mu-]cc",
                               DaughtersCuts = {  "K+":"(TRCHI2DOF<5) & (TRGHOSTPROB < 0.3) & (PT>%(PhiDauMinPT)s) & (PROBNNk > %(PhiProbNNk)s)"%config,
                                                   "mu+":"(TRCHI2DOF<5) & (TRGHOSTPROB < 0.3) & (PT>%(PhiDauMinPT)s)"%config,
                                                   },
                               CombinationCut = "(AWM ('K+','mu+') > %(PhiMassMin)s *MeV) & (AWM ('K+','mu+') < %(PhiMassMax)s *MeV) & (AMAXDOCA('')< %(PhiMaxDOCA)s *mm)"%config,
                               MotherCut ="(PT> %(PhiMinPT)s *  MeV) &  (BPVDIRA > %(PhiMinDIRA)s)"\
                               "& (VFASPF(VCHI2/VDOF) < %(PhiVtxChi2)s)"%config
                                )
    
    _stdAllLooseMuons   = DataOnDemand(Location = "Phys/StdAllLooseMuons/Particles")
    _stdAllLooseKaons   = DataOnDemand(Location = "Phys/StdAllLooseKaons/Particles")
    return Selection (name,
                      Algorithm = PhiKMu,
                      RequiredSelections = [  _stdAllLooseMuons, _stdAllLooseKaons ])


#============================================================
# Lambda-> p pi control channel for other searches
def makeLambdaPPi(name, config):
    """
    Line for the selection of Lambda-> p pi as control channel
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    LambdaPPi = CombineParticles( DecayDescriptor = "[Lambda0 -> p+ pi-]cc",
                                  DaughtersCuts = { "pi+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(pionMinIpChi2)s)"%config, 
                                                     "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s) & (MIPCHI2DV(PRIMARY)>%(protonMinIpChi2)s)"%config
                                                    },
                                  CombinationCut = "(ADAMASS('Lambda0')<%(LambdaMassWinTight)s *MeV) & (AMAXDOCA('')< %(LambdaMaxDOCA)s *mm)"%config,
                                  MotherCut = "(VFASPF(VCHI2/VDOF)< %(LambdaVtxChi2)s)  & (PT> %(LambdaMinPt)s *MeV)"\
                                  "& (ADMASS('Lambda0') < %(LambdaMassWinTight)s *MeV )"\
                                  "& (BPVDIRA > %(LambdaMinDIRA)s) "\
                                  "& (BPVIPCHI2()< %(LambdaMaxIpChi2)s)"\
                                  "& (BPVLTIME()> %(LambdaMinTauPs)s * ps)" %config  
                                  )
    if _monitor : 
        LambdaPPi.Preambulo       = [
            ## define historam type (shortcut)
            "Histo  = Gaudi.Histo1DDef"  ,
            ## monitor LL-case
            "massLam     = monitor ( M / GeV ,             Histo ( 'Lambda' , 1.015, 1.215, 100 ) , 'M_Lambda'    ) " ,
            ]
        LambdaPPi.Monitor         = True ,
        LambdaPPi.HistoProduce    = True ,
        LambdaPPi.MotherMonitor = """ process ( massLam )
        >> EMPTY
        """


    _stdAllLoosePions   = DataOnDemand(Location = "Phys/StdAllLoosePions/Particles")
    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
    return Selection (name,
                      Algorithm = LambdaPPi,
                      RequiredSelections = [  _stdAllLoosePions, _stdLooseProtons ])

#============================================================
# Lambda-> p pi e e FCNC 
def makeLambdaPPiEE(name, config, inputs):
    """
    Line for the selection of Lambda-> p pi e e  FCNC
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    LambdaPPi = CombineParticles( DecayDescriptor = "[Lambda0 -> p+ pi- KS0]cc",
                                  DaughtersCuts = { "pi+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(pionMinIpChi2)s)"%config, 
                                                     "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PROBNNp> %(protonProbNNp)s) & (MIPCHI2DV(PRIMARY)>%(protonMinIpChi2)s)"%config,
                                                    "KS0" : "(MIPCHI2DV(PRIMARY)>%(DiElectronMinIpChi2)s)"%config, 
                                                    },
                                  CombinationCut = "(ADAMASS('Lambda0')<%(LambdaMassWin)s *MeV) & (AMAXDOCA('')< %(LambdaMaxDOCA)s *mm)"%config,
                                  MotherCut = "(VFASPF(VCHI2/VDOF)< %(LambdaVtxChi2)s)  & (PT> %(LambdaMinPt)s *MeV)"\
                                  "& (ADMASS('Lambda0') < %(LambdaMassWin)s *MeV )"\
                                  "& (BPVDIRA > %(LambdaMinDIRA)s) "\
                                  "& (BPVIPCHI2()< %(LambdaMaxIpChi2)s)"\
                                  "& (BPVLTIME()> %(LambdaMinTauPs)s * ps)" %config  
                                  )

    return Selection (name,
                      Algorithm = LambdaPPi,
                      RequiredSelections = inputs ) 



