'''
Module for construction of D0->K3Pi tagged (from prompt D*) and untagged selections.
Provides functions to build tagged and untagged selections for physics analysis.
At present, the untagged lines are not included. This may change in the future 
Provides class DstarPromptWithD02K3PiAllLinesConf, which constructs the selections and stripping lines given a configuration dictionary.
Exported symbols (use python help):
  - DstarPromptWithD02K3PiAllLinesConf
  - makeD02K3Pi
  - makeD02K3PiDCS
  - makePromptDstar
'''

__author__ = ['Philip Hunt']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.2 $'

__all__ = ('DstarPromptWithD02K3PiAllLinesConf'
    ,'makeD02K3Pi'
    ,'makeD02K3PiDCS'
    ,'makePromptDstar')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import checkConfig

confdict = {
  'DMassLower'             : -7.5     #MeV
  ,'DMassUpper'             : 15     #MeV 
  ,'D0MassWin'              : 75     #MeV
                            
  ,'DstarDOCA'                : 0.45    # mm
  ,'D0MaxDOCA'           : 0.5    # mm
                          
  ,'D0DaughterPt'                    : 0.3   # GeV
  ,'D0DaughterP'                     : 3.0      # GeV
  ,'D0Pt' : 3 # GeV
  ,'DstarPt' : 3 # GeV
  ,'SlowPionPt' : 0.07 # GeV

  ,'DstarVtxChi2DOF' : 20
  ,'D0VtxChi2DOF' : 10

  ,'D0DaughterMaxIPChi2' : 30
  ,'D0DaughterIPChi2' : 1.7
  
  ,'D0FDChi2' : 48
  ,'D0IPChi2' : 30
  ,'D0DIRA' : 0.9998
  ,'TrackChi2DOF'                    : 5

  ,'KaonPIDK' : 0
  ,'PionPIDK' : 3 # pions from D0 only, not the bachelor
  
  ,'Prescale'         : 1
  ,'Postscale'        : 1
  }

class DstarPromptWithD02K3PiAllLinesConf(object):
  """
  Constructor of D*->D0(K3Pi)pi stripping selection / stripping line for
  physics analysis.
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> config = {...}
  >>> myConf = StrippingPromptDstarWithD02K3PiConf('PromptDstarWithD02K3Pi',config)
  >>> myLines = myConf.lines
  >>> for line in line :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as instance data members:
    selD02K3Pi    : nominal D0 -> K3Pi Selection object 
    selDstar2Dpi   : nominal D*+ -> D0(K-pi+pi-pi+)pi+  Selection object

    line_tagged : StrippingLine made out of selDstar2Dpi
    Lines                  : List of lines, [line_tagged]

    Exports as class data member:
    DstarPromptWithD02K3PiAllLinesConf.__configuration_keys__ : List of required configuration parameters.
    """
  __configuration_keys__ = ('D0MassWin'
                            ,'DMassLower'
                            ,'DMassUpper'
                            
                            ,'DstarDOCA'
                            ,'D0MaxDOCA'
                            
                            ,'D0DaughterPt'
                            ,'D0DaughterP'
                            ,'D0Pt'
                            ,'DstarPt'
                            ,'SlowPionPt'
                            
                            ,'DstarVtxChi2DOF'
                            ,'D0VtxChi2DOF'
                            
                            ,'D0DaughterMaxIPChi2'
                            ,'D0DaughterIPChi2'
                            ,'D0FDChi2'
                            ,'D0IPChi2'

                            ,'D0DIRA'
                            
                            ,'TrackChi2DOF'

                            ,'KaonPIDK'
                            ,'PionPIDK'
                            
                            ,'Prescale'
                            ,'Postscale'
                                
                            )
  
  def __init__(self
               ,name='DstarPromptWithD02K3Pi'
               ,config=confdict):
    checkConfig(DstarPromptWithD02K3PiAllLinesConf.__configuration_keys__
                , config)
    self.name=name
    
       
    self.selD02K3Pi = makeD02K3Pi(
      name+'D0'
      , massWin= config['D0MassWin']
      , maxDOCA = config['D0MaxDOCA']
      , pt = config['D0Pt']
      , daughterPt = config['D0DaughterPt']
      , daughterMom = config['D0DaughterP']
      , vtxChi2DOF = config['D0VtxChi2DOF']
      , FDChi2 = config['D0FDChi2'] 
      , IPChi2 = config['D0IPChi2']
      , dauMaxIPChi2 = config['D0DaughterMaxIPChi2']
      , dauIPChi2 = config['D0DaughterIPChi2']
      , DIRA = config['D0DIRA']
      , trackChi2DOF = config['TrackChi2DOF']
      , kaonPIDK = config['KaonPIDK']
      , pionPIDK = config['PionPIDK']
      )

    self.selD02K3PiDCS = makeD02K3PiDCS(name+'D0DCS', self.selD02K3Pi)
    
    # prompt D* 
    self.selPromptDstar = makePromptDstar(
      name+'Dstar'
      , d0Sel = self.selD02K3Pi
      , d0ConjSel = self.selD02K3PiDCS
      , DMassLower = config['DMassLower']
      , DMassUpper = config['DMassUpper']
      , DOCA = config['DstarDOCA']
      , vertexChi2DOF = config['DstarVtxChi2DOF']
      , pt = config['DstarPt']
      , slowPionPt = config['SlowPionPt']
      , trackChi2DOF = config['TrackChi2DOF']
      )
    
    self.line_tagged = StrippingLine(
      name+"Line"
      ,prescale=config['Prescale']
      ,postscale=config['Postscale']
      ,algos=[self.selPromptDstar]
      )
   
    self.Lines = [self.line_tagged
                  ]

def makeD02K3PiDCS(
  name
  ,d0Sel):
  """Creates the conjugate D0->K3Pi Selection object
     for physics analysis.
"""
  from Configurables import ConjugateNeutralPID
  _d0Conj=ConjugateNeutralPID('_'+name)
  return Selection(
    name
    ,Algorithm=_d0Conj
    ,RequiredSelections=[d0Sel]
    )
    
  
def makeD02K3Pi (
  name
  , massWin
  , maxDOCA
  , pt
  , daughterPt
  , daughterMom
  , vtxChi2DOF
  , FDChi2
  , IPChi2
  , dauMaxIPChi2
  , dauIPChi2
  , DIRA
  , trackChi2DOF
  , kaonPIDK
  , pionPIDK
  ):
  """Creates a D0->K3Pi Selection object
, with cuts for physics analysis.
Uses DataOnDemand objects 'Phys/StdTightKaons'
and 'Phys/StdTightPions'
Arguments:
  -  name : name of Selection
  - massWin : mass window cut (MeV/c^2)
  - maxDOCA : maximum DOCA of D0 daughters (mm)
  - pt : minimum transverse momentum of D0 (GeV/c)
  - daughterPt : minimum transverse momentum of D0 daughters (GeV/c)
  - daughterMom : minimum momentum of D0 daughters (GeV/c)
  - vtxChi2DOF : maximum vertex chi2 / d.o.f.
  - FDChi2 : minimum vertex chi2
  - IPChi2 : maximum IP chi2
  - dauMaxIPChi2 :require at leat one D0 daughter with IP chi2 greather than this value
  - dauIPChi2 : minimum IP chi2 of D0 daughters
  - DIRA : cosine of angle sustended by momentum and flight direction vectors of D0
  - trackChi2DOF : maximum track chi2 / d.o.f. of D0 daughters (unitless)
  - kaonPIDK : DLL(K-pi) cut applied to kaon (only cut of >0 will have any affect)
  - pionPIDK : DLL(K-pi) cut applied to pions (only cut of <5 will have any affect)

Note: There is a hard-coded cut on the pion DLL(K-pi) of < 0    
  """
  
  _prefitCuts = "(ADAMASS('D0')<%(massWin)s*MeV) & (APT>%(pt)s*GeV)" \
   " & (AMAXDOCA('')<%(maxDOCA)s*mm)" \
   " & (AHASCHILD(((ABSID=='K+') | (ABSID=='pi+')) & (MIPCHI2DV(PRIMARY)>%(dauMaxIPChi2)s)))" \
   %locals()

  _motherCuts =  "(VFASPF(VCHI2/VDOF)<%(vtxChi2DOF)s)" \
                " & (BPVVDCHI2>%(FDChi2)s) & (BPVIPCHI2()<%(IPChi2)s)" \
                " & (BPVDIRA>%(DIRA)s) & (ADMASS('D0')<%(massWin)s*MeV)" \
                " & (PT>%(pt)s*GeV)" %locals()

  _daughterCuts = "(TRCHI2DOF<%(trackChi2DOF)s)" \
                  " &(PT>%(daughterPt)s*GeV)&(P>%(daughterMom)s*GeV)" \
                  " & (MIPCHI2DV(PRIMARY)>%(dauIPChi2)s)" \
                  %locals()
  _kaonPIDCuts = " & (PIDK>%(kaonPIDK)s)" %locals()
  _pionPIDCuts = " & (PIDK<%(pionPIDK)s)" %locals()

  _stdTightKaons = DataOnDemand(Location="Phys/StdTightKaons")
  _stdTightPions = DataOnDemand(Location="Phys/StdTightPions")
 
  # hard code the DLL cut for the pions

  _d0 = CombineParticles( '_'+name
                          ,DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc"
                          ,CombinationCut = _prefitCuts
                          ,MotherCut = _motherCuts
                          ,DaughtersCuts = {
                                "K+" : _daughterCuts+_kaonPIDCuts
                                ,"pi+" : _daughterCuts+_pionPIDCuts} )

  return Selection ( name
                     ,Algorithm = _d0
                     ,RequiredSelections = [_stdTightKaons, _stdTightPions] )

def makePromptDstar(
  name
  ,d0Sel
  ,d0ConjSel
  ,DMassLower
  ,DMassUpper
  ,DOCA
  ,vertexChi2DOF
  ,pt
  ,slowPionPt
  ,trackChi2DOF
  ):
  """Creates a D*->D0pi Selection object
  , with cuts for physics analysis.
  Uses DataOnDemand objects 'Phys/StdNoPIDPions and Phys/StdNoPIDsUpPions'
  Arguments:
  -  name : name of Selection
  - d0Sel : D0 Selection object
  - d0ConjSel : D0 DCS Selection object
  - DMassLower : lower edge of delta mass window (MeV/c^2)
  - DMassUpper : upper edge of delta mass window (MeV/c^2)
  - DOCA : distance of closest approach (mm)
  - vertexChi2DOF : maximum vertex chi2 / d.o.f
  - pt : minimum transverse momentum of D* (GeV/c)
  - slowPionPt : minimum transverse momentum of bachelor pion (GeV/c)
 - trackChi2DOF : track chi2 / d.o.f. of the bachelor pion (unitless)
Note that the delta mass is defined here as the difference between the D* and D0 reconstructed masses subtracted from the PDG value for the D* - D0 mass difference.
  """
     
  _prefitCuts = "(AM-AM1-145.5*MeV>%(DMassLower)s*MeV)" \
    " & (AM-AM1-145.5*MeV<%(DMassUpper)s*MeV)" \
    " & (APT>%(pt)s*GeV)" \
    " & (AMAXDOCA('')<%(DOCA)s*mm)" %locals()
 
  _motherCuts = "(VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s)" %locals()
  _slowPionCuts = "(TRCHI2DOF<%(trackChi2DOF)s) & (PT>%(slowPionPt)s)" %locals()

  # long tracks
  _stdNoPIDsPions = DataOnDemand(Location="Phys/StdNoPIDsPions")
  # upstream tracks
  _stdNoPIDsUpPions = DataOnDemand(Location="Phys/StdNoPIDsUpPions")
  
  from PhysSelPython.Wrappers import MergedSelection
  _slowPions = MergedSelection(
    name+"SlowPions"
    ,RequiredSelections=[_stdNoPIDsPions, _stdNoPIDsUpPions]
    )
  
  _dstar = CombineParticles (
    "_"+name
    ,DecayDescriptor="[D*(2010)+ -> D0 pi+]cc"
    ,CombinationCut = _prefitCuts
    ,MotherCut = _motherCuts
    ,DaughtersCuts = {"D0":"ALL", "pi+" : _slowPionCuts}
    )

  _d0s = MergedSelection(
    name+"D0Merged"
    ,RequiredSelections=[d0Sel, d0ConjSel]
    )

  return Selection(
    name
    ,Algorithm=_dstar
    ,RequiredSelections=[_slowPions, _d0s]
    )

