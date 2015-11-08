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
           'makeSigmaPEE',
           'makeSigmaPPi0',
           'makeSigmaPPi0Cal',
           'makeSigmaPMuMuLFV',
           'makeSigmaPMuMuDown',
           'makeSigmaPEEDown',
           'makeSigmaPMuMuLFVDown',
           'makeKPiMuMu',
           'makeKPiMuMuLFV',
           'makeKPiPiPi',
           'makeKPiPiPiMassMeas',
           'makeKPiMuMuDown',
           'makeKPiMuMuLFVDown',
           'makeKPiPiPiDown',
           'makeKPiPiPiMassMeasDown',
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
                                


default_config = {
  'NAME': 'RareStrange',
  'WGs' : ['RD'],
  'BUILDERTYPE' : 'RareStrangeLinesConf',
  'STREAMS' : [ 'Leptonic' ],
  'CONFIG' : {
        'Postscale'           :1,
        'SigmaPMuMuPrescale' : 1,
        'SigmaPMuMuDownPrescale' :1,
        'SigmaPEEPrescale' : 1,
        'SigmaPPi0Prescale' : 1,
        'SigmaPPi0CalPrescale' : 0.25,
        'SigmaPEEDownPrescale' : 0.1,
        'SigmaPMuMuLFVPrescale' :0.1,
        'SigmaPMuMuLFVDownPrescale' :0.1,
        'KPiPiPiPrescale' : 0.01,
        'KPiPiPiMassMeasPrescale' :1,
        'KPiMuMuPrescale' :1,
        'KPiMuMuLFVPrescale' :1,
        'KPiMuMuDownPrescale' :1,
        'KPiPiPiDownPrescale' : 0.1,
        'KPiPiPiMassMeasDownPrescale' :1,
        'KPiMuMuLFVDownPrescale' :1,
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
        'protonPIDp': 5.,
        'protonPIDpTight': 15.,
        'pMinPt': 500.,
        'electronPIDe':2.,
        'electronMinIpChi2': 9.,
        'pi0MinPt': 0.,
        'muonMinIpChi2Down' : 9.,
        'electronMinIpChi2Down': 4.,
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
        'KVDPVMaxMassMeasDown' : 2200.
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
        'SigmaPEEPrescale',
        'SigmaPPi0Prescale',
        'SigmaPPi0CalPrescale',
        'SigmaPEEDownPrescale',
        'SigmaPMuMuLFVPrescale',
        'SigmaPMuMuLFVDownPrescale',
        'KPiPiPiPrescale',
        'KPiPiPiMassMeasPrescale',
        'KPiMuMuPrescale',
        'KPiMuMuLFVPrescale',
        'KPiMuMuDownPrescale',
        'KPiPiPiDownPrescale',
        'KPiPiPiMassMeasDownPrescale',
        'KPiMuMuLFVDownPrescale',
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
        'protonPIDp',
        'protonPIDpTight',
        'pMinPt',
        'electronPIDe',
        'electronMinIpChi2',
        'muonMinIpChi2Down',
        'pi0MinPt',
        'electronMinIpChi2Down',
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
        'KVDPVMaxMassMeasDown'
                                  )
    
    #### This is the dictionary of all tunable cuts ########
    
    
    
    def __init__(self, 
                 name = 'RareStrange',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        

        spmumu_name=name+'SigmaPMuMu'
        spmumudown_name=name+'SigmaPMuMuDown'
        spee_name=name+'SigmaPEE'
        sppi0_name=name+'SigmaPPi0'
        sppi0cal_name=name+'SigmaPPi0Cal'
        speedown_name=name+'SigmaPEEDown'
        spmumulfv_name=name+'SigmaPMuMuLFV'
        spmumulfvdown_name=name+'SigmaPMuMuLFVDown'
        kpimumu_name=name+'KPiMuMu'
        kpimumulfv_name=name+'KPiMuMuLFV'
        kpipipi_name=name+'KPiPiPi'
        kpipipimassmeas_name=name+'KPiPiPiMassMeas'
        kpimumudown_name=name+'KPiMuMuDown'
        kpimumulfvdown_name=name+'KPiMuMuLFVDown'
        kpipipidown_name=name+'KPiPiPiDown'
        kpipipimassmeasdown_name=name+'KPiPiPiMassMeasDown'


        self.selSigmaPMuMu = makeSigmaPMuMu(spmumu_name, config)
        self.selSigmaPMuMuDown = makeSigmaPMuMuDown(spmumudown_name, config)
        self.selSigmaPEE   = makeSigmaPEE(spee_name, config)
        self.selSigmaPPi0   = makeSigmaPPi0(sppi0_name, config)
        self.selSigmaPPi0Cal   = makeSigmaPPi0Cal(sppi0cal_name, config)
        self.selSigmaPEEDown   = makeSigmaPEEDown(speedown_name, config)
        self.selSigmaPMuMuLFV = makeSigmaPMuMuLFV(spmumulfv_name, config)
        self.selSigmaPMuMuLFVDown = makeSigmaPMuMuLFVDown(spmumulfvdown_name, config)
        self.selKPiMuMu = makeKPiMuMu(kpimumu_name, config)
        self.selKPiMuMuLFV = makeKPiMuMuLFV(kpimumulfv_name, config)
        self.selKPiPiPi = makeKPiPiPi(kpipipi_name, config)
        self.selKPiPiPiMassMeas = makeKPiPiPiMassMeas(kpipipimassmeas_name, config)
        self.selKPiMuMuDown = makeKPiMuMuDown(kpimumudown_name, config)
        self.selKPiMuMuLFVDown = makeKPiMuMuLFVDown(kpimumulfvdown_name, config)
        self.selKPiPiPiDown = makeKPiPiPiDown(kpipipidown_name, config)
        self.selKPiPiPiMassMeasDown = makeKPiPiPiMassMeasDown(kpipipimassmeasdown_name, config)
        
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

        self.registerLine(self.SigmaPMuMuLine)
        self.registerLine(self.SigmaPEELine)
        self.registerLine(self.SigmaPPi0Line)
        self.registerLine(self.SigmaPPi0CalLine)
        self.registerLine(self.SigmaPEEDownLine)
        self.registerLine(self.SigmaPMuMuLFVLine)
        self.registerLine(self.SigmaPMuMuLFVDownLine)
        self.registerLine(self.SigmaPMuMuDownLine)
        self.registerLine(self.KPiMuMuLine)
        self.registerLine(self.KPiMuMuLFVLine)
        self.registerLine(self.KPiMuMuLFVDownLine)
        self.registerLine(self.KPiPiPiLine)
        self.registerLine(self.KPiPiPiMassMeasLine)
        self.registerLine(self.KPiMuMuDownLine)
        self.registerLine(self.KPiPiPiDownLine)
        self.registerLine(self.KPiPiPiMassMeasDownLine)
        



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
                                                     "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config},
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
                                                        "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config},
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
#  Sigma+ -> p pi0 (->e+ e- gamma)
def makeSigmaPEE(name, config):
    """
    Line for the selection of Sigma+ -> p+ e+ e- gamma,  (with photon undetected)
    for normalisation purposes
    Before prescaling this line, please contact the authors listed above
    
    Arguments:
    name        : name of the Selection.
    config      : dictionary of tunable cuts 
    """
    
    SigmaPEE = CombineParticles(    DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc",
                                    DaughtersCuts = { "e+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDe > %(electronPIDe)s ) &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2)s)"%config,
                                                    "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config},
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
#  Sigma+ -> p pi0 
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
                                    "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDpTight)s )"%config},
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
                                    "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDpTight)s )"%config},
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


