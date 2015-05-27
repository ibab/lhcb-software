###########################################################################################
##                          S T R I P P I N G  2 3                                       ##
##                                                                                       ##
##  Configuration for Charm WG                                                           ##
##  Contact person: Eva Gersabeck & Jinlin Fu (evelina.gersabeck@cern.ch & j.fu@cern.ch) ##
###########################################################################################

from GaudiKernel.SystemOfUnits import *

B2ppipiSigmacmm_Lcpi = {
  'NAME': 'B2ppipiSigmacmm_Lcpi',
  'WGs' : ['Charm'],
  'BUILDERTYPE' : 'StrippingB2ppipiSigmacmm_Lcpi_Conf',
  'STREAMS' : [ 'Charm' ],
  'CONFIG' : {
  'protonMINP':10.0*GeV
  ,'protonMINIPCHI2':8.0
  ,'protonMINPT': 500.0*MeV
  ,'protonMINPIDp':5.0
  ,'protonMINPIDp_K':-5.0
  ,'pionMINP':5.0*GeV
  ,'pionMINPT':400.0*MeV
  ,'pionMINIPCHI2':8.0
  ,'piSigmaMINP':2.0*GeV
  ,'piSigmaMINPT':150.0*MeV
  ,'piSigmaMAXP':10000.0*GeV
  ,'piSigmaMAXPT':1000.0*GeV
  ,'piSigmaMINIPCHI2':8.0
  ,'ApplyDeltaMCut' : False
  ,'DelmUpper':3.0*GeV
  ,'DelmLower':0.0
  ,'BMassWind':200.0*MeV
  ,'LcMassWind':100.0*MeV
  ,'threepartMassLow':1.5*GeV
  ,'threepartMassHigh':2.8*GeV
  ,'fourpartMassLow':1.8*GeV
  ,'fourpartMassHigh':3.0*GeV
  ,'threepartMAXDOCA':0.15*mm
  ,'fourpartMAXDOCA':0.15*mm
  ,'threepartVtxChi2DOF':5.0
  ,'fourpartVtxChi2DOF':5.0
  ,'BMAXDOCA':0.20*mm
  ,'SigmaMAXDOCA':0.20*mm
  ,'BVtxChi2DOF':5.0
  ,'fourpartFDChi2':49.0
  ,'threepartFDChi2':49.0
  ,'threepartMinIPChi2':6.0
  ,'fourpartMinIPChi2':6.0
  ,'threepartPT':1.0*GeV
  ,'fourpartPT':1.0*GeV
  ,'BFDChi2':64.0
  ,'SigmaFDChi2':36.0
  ,'SigmaVtxChi2DOF':10.0
  ,'SigmaLcDeltaMlow':0.0*MeV
  ,'SigmaLcDeltaMhigh':1.0*GeV
  ,'SigmaPT':0.0*GeV
  ,'LcFDChi2':36.0
  ,'LcVtxChi2DOF':5.0
  ,'BDIRA':0.998
  ,'TrackChi2DOF':3.0
  ,'ApplyPionPIDK':True
  ,'PionPIDK':0.0
  ,'CheckPV':True
  ,'ApplyGECs':False
  ,'MaxLongTracks':200
  ,'ApplyGhostProbCut' :True
  ,'GhostProbCut':0.3
  ,'UseTOS': True
  ,'Hlt2TOS': { "Hlt2.*Decision%TOS" : 0 }
  ,'Prescalefourpart':1.0
  ,'Postscalefourpart':1.0
  ,'PrescaleComplete':1.0
  ,'PostscaleComplete':1.0
  ,'PrescaleCompleteNorm':1.0
  ,'PostscaleCompleteNorm':1.0
      }
    }

