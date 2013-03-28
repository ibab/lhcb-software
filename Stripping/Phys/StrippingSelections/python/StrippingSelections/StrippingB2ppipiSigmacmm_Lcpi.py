'''
Module for construction of B+->Sigma_c~--(2455) p pi+pi+ with Sigma_c~--(2455) -> Lc- pi-, it includes:
  - B->Sigma_c-- p pi+ pi+ with Sigma-- -> Lc- pi- (partially reconstructed)
  - B->Sigma_c-- p pi+ pi+ with Sigma-- -> Lc- pi- with Lc- -> p- K+ pi- (fully reconstructed)
Provides functions to build selections for physics analysis.
Provides class StrippingB2ppipiSigmacmm_Lcpi_Conf, which constructs the selections and stripping lines given a configuration dictionary.


Exported symbols (use python help):
  - 'StrippingB2ppipiSigmacmm_Lcpi_Conf'
  - 'makeThreePart'
  - 'makeFourPart'
  - 'makeSigmacmm2Lcpi'
  - 'makeB2ppipiSigmacmm'
'''

__author__ = ['Andrea Contu']
__date__ = '15/03/2013'
__version__ = '$Revision: 1.0 $'

__all__ = ('StrippingB2ppipiSigmacmm_Lcpi_Conf'
           ,'makepifromSigmacmm'
           ,'makeThreePart'
           ,'makeFourPart'
           ,'makeSigmacmm2Lcpi'
           ,'makeB2ppipiSigmacmm'
           )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import checkConfig, LineBuilder
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from copy import copy

name='StrippingB2ppipiSigmacmm_Lcpi'

# What follows documents the cuts to
# be implemented by the Stripping team in
# StrippingSettings (valid for Stripping 17,
# 26/08/2011).

config_default = {
  'protonMINP':10.0*GeV
  ,'protonMINIPCHI2':6.0
  ,'protonMINPT': 500.0*MeV
  ,'protonMINPIDp':5.0
  ,'protonMINPIDp_K':0.0
  ,'pionMINP':5.0*GeV
  ,'pionMINPT':500.0*MeV
  ,'pionMINIPCHI2':6.0
  ,'piSigmaMINP':2.0*GeV
  ,'piSigmaMINPT':150.0*MeV
  ,'piSigmaMAXP':10.0*GeV
  ,'piSigmaMAXPT':1.0*GeV
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
  ,'threepartMinIPChi2':5.0
  ,'fourpartMinIPChi2':5.0
  ,'threepartPT':1.0*GeV
  ,'fourpartPT':1.0*GeV
  ,'BFDChi2':100.0
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
  ,'GhostProbCut':0.5
  ,'Prescalefourpart':1.0
  ,'Postscalefourpart':1.0
  ,'PrescaleComplete':1.0
  ,'PostscaleComplete':1.0
    }

config_default_FULLDST = {
  'protonMINP':10.0*GeV
  ,'protonMINIPCHI2':6.0
  ,'protonMINPT': 500.0*MeV
  ,'protonMINPIDp':5.0
  ,'protonMINPIDp_K':0.0
  ,'pionMINP':5.0*GeV
  ,'pionMINPT':500.0*MeV
  ,'pionMINIPCHI2':6.0
  ,'piSigmaMINP':2.0*GeV
  ,'piSigmaMINPT':150.0*MeV
  ,'piSigmaMAXP':10.0*GeV
  ,'piSigmaMAXPT':1.0*GeV
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
  ,'threepartMinIPChi2':5.0
  ,'fourpartMinIPChi2':5.0
  ,'threepartPT':1.0*GeV
  ,'fourpartPT':1.0*GeV
  ,'BFDChi2':100.0
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
  ,'GhostProbCut':0.5
  ,'Prescalefourpart':0.05
  ,'Postscalefourpart':1.0
  ,'PrescaleComplete':1.0
  ,'PostscaleComplete':1.0
    }


class StrippingB2ppipiSigmacmm_Lcpi_Conf(LineBuilder):
  """
  Constructor of  of B+->Sigma_c~--(2455) p pi+pi+ with Sigma_c~--(2455) -> Lc- pi- stripping selections / stripping lines for
  physics analysis (Lc absolute BF measurement).
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> someconfig = {...}
  >>> myConf = StrippingB2ppipiSigmacmm_Lcpi_Conf( name="StrippingB2ppipiSigmacmm_Lcpi_first", config=someconfig )
  >>> myLines = myConf.lines()
  >>> for line in myLines :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as class data member:
    StrippingB2ppipiSigmacmm_Lcpi_Conf.__configuration_keys__ : List of required configuration parameters.
    """
  __configuration_keys__ = (
  'protonMINP'    
	,'protonMINIPCHI2'
  ,'protonMINPT'    
  ,'protonMINPIDp'  
  ,'protonMINPIDp_K'
  ,'pionMINP'       
	,'pionMINPT'      
	,'pionMINIPCHI2'  
	,'piSigmaMINP'    
	,'piSigmaMINPT'   
	,'piSigmaMAXP'    
	,'piSigmaMAXPT'   
	,'piSigmaMINIPCHI2' 
  ,'ApplyDeltaMCut'
  ,'DelmUpper'        
  ,'DelmLower'        
	,'BMassWind'        
  ,'LcMassWind'       
  ,'threepartMassLow' 
  ,'threepartMassHigh' 
  ,'fourpartMassLow' 
  ,'fourpartMassHigh' 
  ,'threepartMAXDOCA' 
	,'fourpartMAXDOCA'  
  ,'threepartVtxChi2DOF'  
	,'fourpartVtxChi2DOF'   
	,'BMAXDOCA' 
	,'SigmaMAXDOCA'  
  ,'BVtxChi2DOF'     
  ,'fourpartFDChi2'  
	,'threepartFDChi2' 
  ,'threepartMinIPChi2'  
  ,'fourpartMinIPChi2'   
  ,'threepartPT'
  ,'fourpartPT' 
	,'BFDChi2'         
	,'SigmaFDChi2'     
  ,'SigmaVtxChi2DOF' 
  ,'SigmaLcDeltaMlow' 
  ,'SigmaLcDeltaMhigh'
  ,'SigmaPT'   
	,'LcFDChi2'         
  ,'LcVtxChi2DOF'     
  ,'BDIRA'           
  ,'TrackChi2DOF'     
  ,'ApplyPionPIDK'    
  ,'PionPIDK'         
  ,'CheckPV'          
  ,'ApplyGECs'        
  ,'MaxLongTracks'    
  ,'ApplyGhostProbCut' 
  ,'GhostProbCut'     
  ,'Prescalefourpart'   
  ,'Postscalefourpart'  
	,'PrescaleComplete'   
  ,'PostscaleComplete'  
    )

  def __init__(self, name, config):
    LineBuilder.__init__(self, name, config)

    selpifromSigmacmm = makepifromSigmacmm(
       name = name
      ,MINP = config['piSigmaMINP']
      ,MINPT = config['piSigmaMINPT']
      ,MAXP = config['piSigmaMAXP']
      ,MAXPT = config['piSigmaMAXPT']
      ,MINIPCHI2 = config['piSigmaMINIPCHI2']
      ,trackChi2DOF = config['TrackChi2DOF']
      )
    
    selThreePart = makeThreePart(
      name = name
      ,pMINP = config['protonMINP']
      ,pMINPT = config['protonMINPT']
      ,pMINIPCHI2 = config['protonMINIPCHI2']
      ,pMINPIDp = config['protonMINPIDp']
      ,pMINPIDp_K = config['protonMINPIDp_K']
      ,piMINP = config['pionMINP']
      ,piMINPT = config['pionMINPT']
      ,piMINIPCHI2 = config['pionMINIPCHI2']
      ,trackChi2DOF = config['TrackChi2DOF']
      ,applyPionPIDK = config['ApplyPionPIDK']
      ,pionPIDK = config['PionPIDK']
      ,applyGhostProbCut = config['ApplyGhostProbCut']
      ,ghostProbCut = config['GhostProbCut']
      ,threepartMassLow = config['threepartMassLow']
      ,threepartMassHigh = config['threepartMassHigh']
      ,threepartMAXDOCA = config['threepartMAXDOCA']
      ,threepartVtxChi2DOF = config['threepartVtxChi2DOF']
      ,threepartFDChi2 = config['threepartFDChi2']
      ,threepartPT = config['threepartPT']
      ,threepartMinIPChi2 = config['threepartMinIPChi2']
      )
    
    selFourPart = makeFourPart(
      name = name
      ,selectionthree = selThreePart
      ,selectionpiSigma = selpifromSigmacmm
      ,fourpartMassLow = config["fourpartMassLow"]
      ,fourpartMassHigh = config["fourpartMassHigh"]
      ,fourpartMAXDOCA = config["fourpartMAXDOCA"]
      ,fourpartVtxChi2DOF = config["fourpartVtxChi2DOF"]
      ,fourpartFDChi2 = config["fourpartFDChi2"]
      ,fourpartPT = config['fourpartPT']
      ,fourpartMinIPChi2 = config['fourpartMinIPChi2']
      ,ApplyDeltaMCut = config['ApplyDeltaMCut']
      ,DelmUpper = config['DelmUpper']
      ,DelmLower = config['DelmLower']
      )
    
    
    selSigmacmm2Lcpi = makeSigmacmm2Lcpi(
      name = name
      ,selection = selpifromSigmacmm
      ,LcMassWind = config['LcMassWind']
      ,LcFDChi2 = config['LcFDChi2']
      ,LcVtxChi2DOF = config['LcVtxChi2DOF']
      ,SigmaMAXDOCA = config['SigmaMAXDOCA']
      ,SigmaFDChi2 = config['SigmaFDChi2']
      ,SigmaVtxChi2DOF = config['SigmaVtxChi2DOF']
      ,SigmaLcDeltaMlow = config['SigmaLcDeltaMlow']
      ,SigmaLcDeltaMhigh = config['SigmaLcDeltaMhigh']
      ,SigmaPT = config['SigmaPT']
      )
    
    selB2ppipiSigmacmm = makeB2ppipiSigmacmm(
      name = name
      ,selectionthree = selThreePart
      ,selectionSigma = selSigmacmm2Lcpi
      ,BMassWind = config['BMassWind']
      ,BMAXDOCA = config['BMAXDOCA']
      ,BVtxChi2DOF = config['BVtxChi2DOF']
      ,BFDChi2 = config['BFDChi2']
      ,BDIRA = config['BDIRA']
      )

    _GECfilter = None

    if config['ApplyGECs']:
      _filter = ""

      nLong = config["MaxLongTracks"]
      if nLong is not None:
        if _filter != "":
          _filter+=" & "
        _filter += "( recSummaryTrack(LHCb.RecSummary.nLongTracks,TrLONG) < %s )" %nLong

      if _filter != "":
        _GECfilter = {'Code' : _filter,
                      'Preambulo' : ["from LoKiTracks.decorators import *",
                                     'from LoKiNumbers.decorators    import *',
                                     'from LoKiCore.functions    import *']
                      }


    self.line_fourpart = StrippingLine(
      name+"_PartReco_Line"
      ,prescale=config['Prescalefourpart']
      ,postscale=config['Postscalefourpart']
      ,selection=selFourPart
      ,checkPV=config['CheckPV']
      ,FILTER=_GECfilter)
    self.registerLine(self.line_fourpart)
    
    self.line_B2ppipiSigmacmm= StrippingLine(
      name+"_Closed_Line"
      ,prescale=config['PrescaleComplete']
      ,postscale=config['PostscaleComplete']
      ,selection=selB2ppipiSigmacmm
      ,checkPV=config['CheckPV']
      ,FILTER=_GECfilter)
    self.registerLine(self.line_B2ppipiSigmacmm)

def makepifromSigmacmm(
       name
      ,MINP
      ,MINPT
      ,MAXP
      ,MAXPT
      ,MINIPCHI2
      ,trackChi2DOF
      ):
  _cuts = "(TRCHI2DOF<%(trackChi2DOF)s)" \
              " &(PT>%(MINPT)s)&(P>%(MINP)s)" \
              " &(PT<%(MAXPT)s)&(P<%(MAXP)s)" \
              " & (MIPCHI2DV(PRIMARY)>%(MINIPCHI2)s)" \
              %locals()
  
  from StandardParticles import StdAllNoPIDsPions
  
  _filter = FilterDesktop(Code = _cuts)
  
  _selection = Selection('pifromSigmamm_'+name
                          ,Algorithm=_filter
                          ,RequiredSelections=[StdAllNoPIDsPions])
  return _selection

def makeThreePart(
      name
      ,pMINP
      ,pMINPT
      ,pMINIPCHI2
      ,pMINPIDp
      ,pMINPIDp_K
      ,piMINP
      ,piMINPT
      ,piMINIPCHI2
      ,trackChi2DOF
      ,applyPionPIDK
      ,pionPIDK
      ,applyGhostProbCut
      ,ghostProbCut
      ,threepartMassLow
      ,threepartMassHigh
      ,threepartMAXDOCA
      ,threepartVtxChi2DOF
      ,threepartFDChi2
      ,threepartPT
      ,threepartMinIPChi2
  ):
  _prefitCuts = "(AMAXDOCA('')<%(threepartMAXDOCA)s) & (APT>%(threepartPT)s)" %locals()

  _motherCuts = "(VFASPF(VCHI2/VDOF)<%(threepartVtxChi2DOF)s) & " \
                "(BPVVDCHI2>%(threepartFDChi2)s) & " \
                "(M<%(threepartMassHigh)s) & (M>%(threepartMassLow)s) & " \
                "(PT>%(threepartPT)s) & (BPVIPCHI2()>%(threepartMinIPChi2)s)" %locals()

  _pionCuts = "(TRCHI2DOF<%(trackChi2DOF)s)" \
              " &(PT>%(piMINPT)s)&(P>%(piMINP)s)" \
              " & (MIPCHI2DV(PRIMARY)>%(piMINIPCHI2)s)" \
              %locals()
  if applyPionPIDK:
    _tmpc = "((PIDK-PIDpi)>%(pionPIDK)s)" %locals()
    _pionCuts = _pionCuts + " & "+_tmpc
     
  _protonCuts = "(TRCHI2DOF<%(trackChi2DOF)s) &  ((PIDp-PIDpi)>%(pMINPIDp)s) & ((PIDp-PIDK)>%(pMINPIDp_K)s)" \
              " &(PT>%(pMINPT)s)&(P>%(pMINP)s)" \
              " & (MIPCHI2DV(PRIMARY)>%(pMINIPCHI2)s)" \
              %locals()
              
  if applyGhostProbCut:
    _ghostCut = "( TRGHOSTPROB < %(ghostProbCut)s )" %locals()
    _pionCuts = _pionCuts + " & "+_ghostCut
     
  from StandardParticles import StdLoosePions, StdLooseProtons
  
  _combpart = CombineParticles(DecayDescriptor = "[D+ -> pi+ pi+ p+]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = {"pi+":_pionCuts, "p+":_protonCuts}
                               )
  _selection = Selection('threepart_'+name
                          ,Algorithm=_combpart
                          ,RequiredSelections=[StdLoosePions, StdLooseProtons])
  return _selection


def makeSigmacmm2Lcpi(
  name
  ,selection
  ,LcMassWind
  ,LcFDChi2
  ,LcVtxChi2DOF
  ,SigmaMAXDOCA
  ,SigmaFDChi2
  ,SigmaVtxChi2DOF
  ,SigmaLcDeltaMlow
  ,SigmaLcDeltaMhigh
  ,SigmaPT
  ):
  _prefitCuts = "(AMAXDOCA('')<%(SigmaMAXDOCA)s) & (APT>%(SigmaPT)s)" %locals()
  
  _LcCuts = "(ADMASS('Lambda_c~-')<%(LcMassWind)s) & (VFASPF(VCHI2/VDOF)<%(LcVtxChi2DOF)s) & (BPVVDCHI2>%(LcFDChi2)s) " %locals()

  _motherCuts = "(VFASPF(VCHI2/VDOF)<%(SigmaVtxChi2DOF)s) & " \
                "(BPVVDCHI2>%(SigmaFDChi2)s) & (PT>%(SigmaPT)s) &" \
                 "((M-M1)>%(SigmaLcDeltaMlow)s) & ((M-M1)<%(SigmaLcDeltaMhigh)s)" %locals()

  _combpart = CombineParticles (
    DecayDescriptor="[Sigma_c~-- -> Lambda_c~- pi-]cc"
    ,CombinationCut = _prefitCuts
    ,MotherCut = _motherCuts
    ,DaughtersCuts = {"Lambda_c~-":_LcCuts, "pi-" : "ALL"}
    )
  mylcs=DataOnDemand( Location = 'Phys/StdLooseLambdac2PKPi/Particles' )
  _selection = Selection(
    'Sigmacmm2Lcpi'+name
    ,Algorithm=_combpart
    ,RequiredSelections=[mylcs, selection]
    )
  return _selection

def makeFourPart(
  name
  ,selectionthree
  ,selectionpiSigma
  ,fourpartMassLow
  ,fourpartMassHigh
  ,fourpartMAXDOCA
  ,fourpartVtxChi2DOF
  ,fourpartFDChi2
  ,fourpartPT
  ,fourpartMinIPChi2
  ,ApplyDeltaMCut
  ,DelmUpper
  ,DelmLower
  ):
  _prefitCuts = "(AMAXDOCA('')<%(fourpartMAXDOCA)s) & (APT>%(fourpartPT)s)" %locals()

  _motherCuts = "(VFASPF(VCHI2/VDOF)<%(fourpartVtxChi2DOF)s) & (BPVIPCHI2()>%(fourpartMinIPChi2)s) & " \
                "(BPVVDCHI2>%(fourpartFDChi2)s) & (PT>%(fourpartPT)s) & " \
                  "((M)>%(fourpartMassLow)s) & ((M)<%(fourpartMassHigh)s)"%locals()

  if(ApplyDeltaMCut):
    _deltamcut="((M-M2)>%(DelmLower)s) & ((M-M2)<%(DelmUpper)s)"%locals()
    _motherCuts=motherCuts+" & "+_deltamcut
    
  _combpart = CombineParticles (
    DecayDescriptor="[B+ -> pi- D+]cc"
    ,CombinationCut = _prefitCuts
    ,MotherCut = _motherCuts
    ,DaughtersCuts = {"pi-": "ALL", "D+" : "ALL"}
    )

  _selection = Selection(
    'fourpart'+name
    ,Algorithm=_combpart
    ,RequiredSelections=[selectionthree, selectionpiSigma]
    )
  return _selection


def makeB2ppipiSigmacmm(
  name
  ,selectionthree
  ,selectionSigma
  ,BMassWind
  ,BMAXDOCA
  ,BVtxChi2DOF
  ,BFDChi2
  ,BDIRA
  ):
  _prefitCuts = "(AMAXDOCA('')<%(BMAXDOCA)s)" %locals()

  _motherCuts = "(ADMASS('B+')<%(BMassWind)s) & " \
                "(BPVVDCHI2>%(BFDChi2)s) & (BPVDIRA>%(BDIRA)s) & " \
                 "(VFASPF(VCHI2/VDOF)<%(BVtxChi2DOF)s)" %locals()

  _combpart = CombineParticles (
    DecayDescriptor="[B+ -> Sigma_c~-- D+]cc"
    ,CombinationCut = _prefitCuts
    ,MotherCut = _motherCuts
    ,DaughtersCuts = {"Sigma_c~--": "ALL", "D+" : "ALL"}
    )

  _selection = Selection(
    'B2ppipiSigmacmm'+name
    ,Algorithm=_combpart
    ,RequiredSelections=[selectionthree, selectionSigma]
    )
  return _selection
  