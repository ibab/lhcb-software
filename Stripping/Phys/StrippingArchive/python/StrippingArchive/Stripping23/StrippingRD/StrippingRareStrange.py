'''
Module for construction of stripping selections for some rare strange decays
Exported symbols (use python help!):
'''

__author__ = ['Francesco Dettori','Andrea Contu']
__date__ = '31/07/2014'
__version__ = '$Revision: 2.0 $'

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
from Configurables import FilterDesktop, CombineParticles
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
        'SigmaPPi0CalPrescale' : 1,
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
        'KMaxDOCAMassMeasDown' : 9999.,
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
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMu : 'P2CVSigma09', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMu : 'P2CVSigma10', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMu : 'P2CVSigma11', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton11', 
                                                }
                                              }
                                            ]
                                     )
        self.SigmaPMuMuDownLine = StrippingLine(spmumudown_name+"Line",
                                            prescale = config['SigmaPMuMuDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selSigmaPMuMuDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuDown : 'P2CVSigma09', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuDown : 'P2CVSigma10', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuDown : 'P2CVSigma11', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton11', 
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
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEE : 'P2CVSigma09', 
                                                "Phys/StdAllLooseElectrons"   : ['P2CVEl09_1','P2CVEl09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEE : 'P2CVSigma10', 
                                                "Phys/StdAllLooseElectrons"   : ['P2CVEl10_1','P2CVEl10_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEE : 'P2CVSigma11', 
                                                "Phys/StdAllLooseElectrons"   : ['P2CVEl11_1','P2CVEl11_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton11', 
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
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0 : 'P2CVSigma09',
                                                "StdLooseDalitzPi0"    : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi024e"       : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi02gee"      : ['P2CVEl09_1','P2CVEl09_2'],
                                                "Phys/StdAllLooseProton"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0 : 'P2CVSigma10',
                                                "StdLooseDalitzPi0"    : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi024e"       : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi02gee"      : ['P2CVEl09_1','P2CVEl09_2'],
                                                "Phys/StdAllLooseProton"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0 : 'P2CVSigma11',
                                                "StdLooseDalitzPi0"    : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi024e"       : ['P2CVEl09_1','P2CVEl09_2'],
                                                #"StdLoosePi02gee"      : ['P2CVEl09_1','P2CVEl09_2'],
                                                "Phys/StdAllLooseProton"   : 'P2CVProton11', 
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
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0Cal : 'P2CVSigma09', 
                                                "Phys/StdLooseMergedPi0"   : ['P2CVEl09_1','P2CVEl09_2'],
                                                "Phys/StdLooseResolvedPi0"   : ['P2CVEl09_1','P2CVEl09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0Cal : 'P2CVSigma10', 
                                                "Phys/StdLooseMergedPi0"   : ['P2CVEl10_1','P2CVEl10_2'],
                                                "Phys/StdLooseResolvedPi0"   : ['P2CVEl09_1','P2CVEl09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPPi0Cal : 'P2CVSigma11', 
                                                "Phys/StdLooseMergedPi0"   : ['P2CVEl11_1','P2CVEl11_2'],
                                                "Phys/StdLooseResolvedPi0"   : ['P2CVEl09_1','P2CVEl09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton11', 
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
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEEDown : 'P2CVSigma09', 
                                                "Phys/StdNoPIDsDownElectrons"   : ['P2CVEl09_1','P2CVEl09_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEEDown : 'P2CVSigma10', 
                                                "Phys/StdNoPIDsDownElectrons"   : ['P2CVEl10_1','P2CVEl10_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPEEDown : 'P2CVSigma11', 
                                                "Phys/StdNoPIDsDownElectrons"   : ['P2CVEl11_1','P2CVEl11_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton11', 
                                                }
                                              }
                                            ]
                                     )
        self.SigmaPMuMuLFVLine = StrippingLine(spmumulfv_name+"Line",
                                               prescale = config['SigmaPMuMuLFVPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selSigmaPMuMuLFV ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFV : 'P2CVSigma09', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFV : 'P2CVSigma10', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFV : 'P2CVSigma11', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdAllLooseProton"   : 'P2CVProton11', 
                                                }
                                              }
                                            ]
                                     )

        self.SigmaPMuMuLFVDownLine = StrippingLine(spmumulfvdown_name+"Line",
                                               prescale = config['SigmaPMuMuLFVDownPrescale'],
                                               postscale = config['Postscale'],
                                               algos = [ self.selSigmaPMuMuLFVDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFVDown : 'P2CVSigma09', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFVDown : 'P2CVSigma10', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selSigmaPMuMuLFVDown : 'P2CVSigma11', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdNoPIDsDownProtons"   : 'P2CVProton11', 
                                                }
                                              }
                                            ]
                                     )

        self.KPiMuMuLine = StrippingLine(kpimumu_name+"Line",
                                             prescale = config['KPiMuMuPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiMuMu ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMu: 'P2CVK09', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMu : 'P2CVK10', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMu : 'P2CVK11', 
                                                "Phys/StdAllLooseMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion11', 
                                                }
                                              }
                                            ]
                                     )
        self.KPiMuMuLFVLine = StrippingLine(kpimumulfv_name+"Line",
                                         prescale = config['KPiMuMuLFVPrescale'],
                                         postscale = config['Postscale'],
                                         algos = [ self.selKPiMuMuLFV ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                            {
                                              'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                              'RecursionLevel' : 1, 
                                              'Locations' : {
                                              self.selKPiMuMuLFV : 'P2CVK09', 
                                              "Phys/StdAllLooseMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion09', 
                                              }
                                            },
                                            {
                                              'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                              'RecursionLevel' : 1, 
                                              'Locations' : {
                                              self.selKPiMuMuLFV : 'P2CVK10', 
                                              "Phys/StdAllLooseMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion10', 
                                              }
                                            },
                                            {
                                              'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                              'RecursionLevel' : 1, 
                                              'Locations' : {
                                              self.selKPiMuMuLFV : 'P2CVK11', 
                                              "Phys/StdAllLooseMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdAllLoosePions"   : 'P2CVPion11', 
                                              }
                                            }
                                          ]
                                         )
        
        self.KPiPiPiLine = StrippingLine(kpipipi_name+"Line",
                                             prescale = config['KPiPiPiPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPi ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                             RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPi : 'P2CVK09', 
                                                "Phys/StdAllLoosePions"   : ['P2CVPion09_1','P2CVPion09_2','P2CVPion09_3']
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPi : 'P2CVK10', 
                                                "Phys/StdAllLoosePions"   : ['P2CVPion10_1','P2CVPion10_2','P2CVPion10_3']
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPi : 'P2CVK11',
                                                "Phys/StdAllLoosePions"   : ['P2CVPion11_1','P2CVPion11_2','P2CVPion11_3']
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiMassMeasLine = StrippingLine(kpipipimassmeas_name+"Line",
                                             prescale = config['KPiPiPiMassMeasPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiMassMeas ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeas : 'P2CVK09', 
                                                "Phys/StdAllLoosePions"   : ['P2CVPion09_1','P2CVPion09_2','P2CVPion09_3']
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeas : 'P2CVK10', 
                                                "Phys/StdAllLoosePions"   : ['P2CVPion10_1','P2CVPion10_2','P2CVPion10_3']
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeas : 'P2CVK11',
                                                "Phys/StdAllLoosePions"   : ['P2CVPion11_1','P2CVPion11_2','P2CVPion11_3']
                                                }
                                              }
                                            ]
                                     )
        self.KPiMuMuDownLine = StrippingLine(kpimumudown_name+"Line",
                                             prescale = config['KPiMuMuDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiMuMuDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuDown : 'P2CVK09', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion09', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuDown : 'P2CVK10', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion10', 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuDown : 'P2CVK11', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion11', 
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiDownLine = StrippingLine(kpipipidown_name+"Line",
                                             prescale = config['KPiPiPiDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiDown : 'P2CVK09', 
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion09_1','P2CVPion09_2','P2CVPion09_3'] 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiDown : 'P2CVK10', 
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion10_1','P2CVPion10_2','P2CVPion10_3'] 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiDown : 'P2CVK11',
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion11_1','P2CVPion11_2','P2CVPion11_3'] 
                                                }
                                              }
                                            ]
                                     )
        self.KPiPiPiMassMeasDownLine = StrippingLine(kpipipimassmeasdown_name+"Line",
                                             prescale = config['KPiPiPiMassMeasDownPrescale'],
                                            postscale = config['Postscale'],
                                            algos = [ self.selKPiPiPiMassMeasDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeasDown : 'P2CVK09', 
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion09_1','P2CVPion09_2','P2CVPion09_3'] 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeasDown : 'P2CVK10', 
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion10_1','P2CVPion10_2','P2CVPion10_3'] 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiPiPiMassMeasDown : 'P2CVK11',
                                                "Phys/StdNoPIDsDownPions"   : ['P2CVPion11_1','P2CVPion11_2','P2CVPion11_3'] 
                                                }
                                              }
                                            ]
                                     )
     
        self.KPiMuMuLFVDownLine = StrippingLine(kpimumulfvdown_name+"Line",
                                         prescale = config['KPiMuMuLFVDownPrescale'],
                                         postscale = config['Postscale'],
                                         algos = [ self.selKPiMuMuLFVDown ],
                                            RequiredRawEvents = ["Velo"],
                                            MDSTFlag=True,
                                            RelatedInfoTools = [
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 0.9, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuLFVDown : 'P2CVK09', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu09_1','P2CVMu09_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion09'
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.0, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuLFVDown : 'P2CVK10', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu10_1','P2CVMu10_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion10' 
                                                }
                                              },
                                              {
                                                'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.1, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
                                                'RecursionLevel' : 1, 
                                                'Locations' : {
                                                self.selKPiMuMuLFVDown : 'P2CVK11', 
                                                "Phys/StdLooseDownMuons"   : ['P2CVMu11_1','P2CVMu11_2'], 
                                                "Phys/StdNoPIDsDownPions"   : 'P2CVPion11', 
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
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p+ mu+ mu-]cc"
    #SigmaPMuMu.addTool( OfflineVertexFitter )
    #SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                 "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                                 }

    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  

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
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p~- mu+ mu+]cc"
    #SigmaPMuMu.addTool( OfflineVertexFitter )
    #SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                 "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                                 }
    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  


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
    
    from Configurables import OfflineVertexFitter
    SigmaPEE = CombineParticles("Combine"+name)
    SigmaPEE.DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc"
    #SigmaPEE.addTool( OfflineVertexFitter )
    #SigmaPEE.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPEE.OfflineVertexFitter.useResonanceVertex = False

    
    SigmaPEE.DaughtersCuts = { "e+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDe > %(electronPIDe)s ) &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2)s)"%config,
                               "p+" : "(TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDp)s)"%config
                               }
    
    SigmaPEE.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPEE.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
    
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

    
    from Configurables import OfflineVertexFitter
    SigmaPPi0 = CombineParticles("Combine"+name)
    SigmaPPi0.DecayDescriptor = "[Sigma+ -> p+ pi0]cc"
    #SigmaPPi0.addTool( OfflineVertexFitter )
    #SigmaPPi0.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPPi0.OfflineVertexFitter.useResonanceVertex = False

    
    SigmaPPi0.DaughtersCuts = { "pi0" : "(PT > %(pi0MinPt)s)"%config,
                                "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDpTight)s )"%config
                               }
    
    SigmaPPi0.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPPi0MassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPPi0.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaPPi0MassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
    

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

    
    from Configurables import OfflineVertexFitter
    SigmaPPi0 = CombineParticles("Combine"+name)
    SigmaPPi0.DecayDescriptor = "[Sigma+ -> p+ pi0]cc"
    
    SigmaPPi0.DaughtersCuts = { "pi0" : "(PT > %(pi0MinPt)s)"%config,
                               "p+" : "(PT > %(pMinPt)s) & (TRCHI2DOF<3) & (TRGHOSTPROB<0.3) & (PIDp > %(protonPIDpTight)s )"%config
                               }
    
    SigmaPPi0.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPPi0MassWin)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCA)s *mm)"%config

    SigmaPPi0.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2)s)  & (PT> %(SigmaMinPt)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaPPi0MassWin)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRA)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPs)s * ps)" %config  
    

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
    
    from Configurables import OfflineVertexFitter
    KPiMuMu = CombineParticles("Combine"+name)
    KPiMuMu.DecayDescriptor = "[K+ -> pi+ mu+ mu-]cc"
    #KPiMuMu.addTool( OfflineVertexFitter )

    #KPiMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiMuMu.OfflineVertexFitter.useResonanceVertex = False
    KPiMuMu.DaughtersCuts = { "mu+" : "(P>3000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                              "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                              }

    KPiMuMu.CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config
    
    KPiMuMu.MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & (BPVDIRA > %(KMinDIRA)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config

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
    
    from Configurables import OfflineVertexFitter
    KPiMuMuLFV = CombineParticles("Combine"+name)
    KPiMuMuLFV.DecayDescriptor = "[K+ -> pi- mu+ mu+]cc"
    #KPiMuMuLFV.addTool( OfflineVertexFitter )

    #KPiMuMuLFV.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiMuMuLFV.OfflineVertexFitter.useResonanceVertex = False
    KPiMuMuLFV.DaughtersCuts = { "mu+" : "(P>3000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                              "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config,
                              }

    KPiMuMuLFV.CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config
    
    KPiMuMuLFV.MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & ((BPVDIRA > %(KMinDIRA)s))"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config

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
    
    from Configurables import OfflineVertexFitter
    KPiPiPi = CombineParticles("Combine"+name)
    KPiPiPi.DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc"
    #KPiPiPi.addTool( OfflineVertexFitter )
    #KPiPiPi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiPiPi.OfflineVertexFitter.useResonanceVertex = False
    KPiPiPi.DaughtersCuts = { "pi+":"(P>1000) & (TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2)s)"%config
                                 }
    KPiPiPi.CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config
    KPiPiPi.MotherCut ="(PT> %(KMinPT)s) & (ADMASS('K+') < %(KMassWin)s *MeV) & (BPVDIRA > %(KMinDIRA)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2)s) & (BPVVDCHI2 > %(KMinVDChi2)s) & (BPVIPCHI2()< %(KMaxIpChi2)s )"%config


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
    
    from Configurables import OfflineVertexFitter
    KPiPiPi = CombineParticles("Combine"+name)
    KPiPiPi.DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc"
    #KPiPiPi.addTool( OfflineVertexFitter )

    #KPiPiPi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiPiPi.OfflineVertexFitter.useResonanceVertex = False
    KPiPiPi.DaughtersCuts = {   "pi+":"(P>1000) & (MIPCHI2DV(PRIMARY) > 25.0) & (TRCHI2DOF<5) & (TRGHOSTPROB < 0.3)"
                                 }
    KPiPiPi.CombinationCut = "(ADAMASS('K+') < %(KMassWinMassMeas)s *MeV) & (AMAXDOCA('')< %(KMaxDOCAMassMeas)s *mm)"%config
    
    KPiPiPi.MotherCut ="(PT> %(KMinPTMassMeas)s) & (ADMASS('K+') < %(KMassWinMassMeas)s *MeV) & (BPVDIRA > %(KMinDIRAMassMeas)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2MassMeas)s) & (BPVVDCHI2 > %(KMinVDChi2MassMeas)s) & (BPVIPCHI2()< %(KMaxIpChi2MassMeas)s )"%config


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
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p+ mu+ mu-]cc"
    #SigmaPMuMu.addTool( OfflineVertexFitter )
    #SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF< %(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2Down)s)"%config,
                                 "p+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)"%config
                                 }

    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWinDown)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config  


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
    
    from Configurables import OfflineVertexFitter
    SigmaPMuMu = CombineParticles("Combine"+name)
    SigmaPMuMu.DecayDescriptor = "[Sigma+ -> p~- mu+ mu+]cc"
    #SigmaPMuMu.addTool( OfflineVertexFitter )
    #SigmaPMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPMuMu.OfflineVertexFitter.useResonanceVertex = False
    SigmaPMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(muonMinIpChi2)s)"%config,
                                 "p~-" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)"%config
                                 }

    SigmaPMuMu.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config

    SigmaPMuMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaMassWinDown)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config  


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
    
    from Configurables import OfflineVertexFitter
    SigmaPEE = CombineParticles("Combine"+name)
    SigmaPEE.DecayDescriptor = "[Sigma+ -> p+ e+ e-]cc"
    #SigmaPEE.addTool( OfflineVertexFitter )
    #SigmaPEE.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #SigmaPEE.OfflineVertexFitter.useResonanceVertex = False

    
    SigmaPEE.DaughtersCuts = { "e+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3)  &  (MIPCHI2DV(PRIMARY)>%(electronMinIpChi2Down)s)"%config,
                               "p+" : "(TRCHI2DOF<%(SigmaDauTrChi2Down)s) & (TRGHOSTPROB<0.3) "%config
                               }
    
    SigmaPEE.CombinationCut = "(ADAMASS('Sigma+')<%(SigmaPEEMassWinDown)s *MeV) & (AMAXDOCA('')< %(SigmaMaxDOCADown)s *mm)"%config

    SigmaPEE.MotherCut = "(VFASPF(VCHI2/VDOF)< %(SigmaVtxChi2Down)s)  & (PT> %(SigmaMinPtDown)s *MeV)"\
                              "& (ADMASS('Sigma+') < %(SigmaPEEMassWinDown)s *MeV )"\
                              "& (BPVDIRA > %(SigmaMinDIRADown)s) "\
                              "& (BPVIPCHI2()< %(SigmaMaxIpChi2Down)s)"\
                              "& (BPVLTIME()> %(SigmaMinTauPsDown)s * ps)" %config
    
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
    
    from Configurables import OfflineVertexFitter
    KPiMuMu = CombineParticles("Combine"+name)
    KPiMuMu.DecayDescriptor = "[K+ -> pi+ mu+ mu-]cc"
    #KPiMuMu.addTool( OfflineVertexFitter )

    #KPiMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiMuMu.OfflineVertexFitter.useResonanceVertex = False
    KPiMuMu.DaughtersCuts = {  "mu+" : "(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                              "pi+":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                              }

    KPiMuMu.CombinationCut = "(ADAMASS('K+') < %(KMassWinDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCADown)s *mm)"%config
    
    KPiMuMu.MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config

    
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
    
    from Configurables import OfflineVertexFitter
    KPiMuMu = CombineParticles("Combine"+name)
    KPiMuMu.DecayDescriptor = "[K+ -> pi- mu+ mu+]cc"
    #KPiMuMu.addTool( OfflineVertexFitter )

    #KPiMuMu.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiMuMu.OfflineVertexFitter.useResonanceVertex = False
    KPiMuMu.DaughtersCuts = { "mu+" : "(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB<0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                              "pi-":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config,
                              }

    KPiMuMu.CombinationCut = "(ADAMASS('K+') < %(KMassWin)s *MeV) & (AMAXDOCA('')< %(KMaxDOCA)s *mm)"%config
    
    KPiMuMu.MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config

    
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
    
    from Configurables import OfflineVertexFitter
    KPiPiPi = CombineParticles("Combine"+name)
    KPiPiPi.DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc"
    #KPiPiPi.addTool( OfflineVertexFitter )
    #KPiPiPi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiPiPi.OfflineVertexFitter.useResonanceVertex = False

    KPiPiPi.DaughtersCuts = {  "pi+":"(TRCHI2DOF<%(KDauTrChi2)s) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2Down)s)"%config
                               }
    KPiPiPi.CombinationCut = "(ADAMASS('K+') < %(KMassWinDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCADown)s *mm)"%config
    
    KPiPiPi.MotherCut ="(PT> %(KMinPTDown)s) & (ADMASS('K+') < %(KMassWinDown)s *MeV) & (BPVDIRA > %(KMinDIRADown)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2Down)s) & (BPVVDCHI2 > %(KMinVDChi2Down)s) & (BPVVDZ > %(KVDPVMinDown)s*mm) & (BPVVDZ < %(KVDPVMaxDown)s*mm)"%config

    
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
    
    from Configurables import OfflineVertexFitter
    KPiPiPi = CombineParticles("Combine"+name)
    KPiPiPi.DecayDescriptor = "[K+ -> pi+ pi+ pi-]cc"
    #KPiPiPi.addTool( OfflineVertexFitter )
    #KPiPiPi.ParticleCombiners.update( { "" : "OfflineVertexFitter"} )
    #KPiPiPi.OfflineVertexFitter.useResonanceVertex = False

    KPiPiPi.DaughtersCuts = {  "pi+":"(TRCHI2DOF<5) & (TRGHOSTPROB < 0.3) & (MIPCHI2DV(PRIMARY)>%(KDauMinIpChi2MassMeasDown)s)"%config
                                 }
    KPiPiPi.CombinationCut = "(ADAMASS('K+') < %(KMassWinMassMeasDown)s *MeV) & (AMAXDOCA('')< %(KMaxDOCAMassMeasDown)s *mm)"%config
    
    KPiPiPi.MotherCut ="(PT> %(KMinPTMassMeasDown)s) & (ADMASS('K+') < %(KMassWinMassMeasDown)s *MeV) & (BPVDIRA > %(KMinDIRAMassMeasDown)s)"\
                        "& (VFASPF(VCHI2) < %(KVtxChi2MassMeasDown)s) & (BPVVDCHI2 > %(KMinVDChi2MassMeasDown)s)  & (BPVVDZ > %(KVDPVMinMassMeasDown)s*mm) & (BPVVDZ < %(KVDPVMaxMassMeasDown)s*mm)"%config

    
    _Pions = DataOnDemand(Location = "Phys/StdNoPIDsDownPions/Particles")
    
    return Selection (name,
                      Algorithm = KPiPiPi,
                      RequiredSelections = [  _Pions ])


