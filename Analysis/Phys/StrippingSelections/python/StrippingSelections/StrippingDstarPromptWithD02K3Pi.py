'''
Module for construction of D0->K3Pi tagged (from prompt D*) and untagged selections.
Provides functions to build tagged and untagged selections for physics analysis.
At present, the untagged lines are not included. This may change in the future 
Provides classes StrippingPromptDstarWithD02K3PiConf, which construct the selections and stripping lines given a configuration dictionary.
Exported symbols (use python help):
  - D02K3PiConf
  - makeD02K3Pi
  - makePromptDstar
'''

__author__ = ['Philip Hunt']
__date__ = '03/09/2010'
__version__ = '$Revision: 1.2 $'

__all__ = ('StrippingDstarPromptWithD02K3PiConf', 'makeD02K3Pi'
    'makeD02K3PiDCS'
            'makePromptDstar')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

class StrippingDstarPromptWithD02K3PiConf(object):
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

    line_untagged : StrippingLine made out of selD02K3Pi
    line_tagged : StrippingLine made out of selDstar2Dpi
    lines                  : List of lines, [line_untagged                                            
                                            ,line_tagged]

    Exports as class data member:
    StrippingPromptDstarWithD02K3PiConf.__configuration_keys__ : List of required configuration parameters.
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

                            ,'D0DaughterIPChi2'
                            ,'D0FDChi2'
                            ,'D0IPChi2'

                            ,'D0DIRA'
                            
                            ,'TrackChi2DOF'
                         
                            ,'Prescale'
                            ,'Postscale'
                                
                            )
  
  """Configuration for the cross-section lines. Some cuts, e.g. the mass windows, pt and IP chi2 cuts, are applied to the combinations before the vertex fitting, and again after the fitting. List of cuts used in this configuration:
   - D0 mass window = 100 MeV/c^2
  - D0 max DOCA < 0.5 mm
  - D0 daughter pt > 160 MeV/c
  - D0 daughter momentum > 3.2 GeV/c
  - Delta mass window (lower, upper) = -7.5 +15 MeV/c^2
  - D* DOCA < 0.5 mm
  - Track vertex chi2 / d.o.f. < 10
  - D0 daughter kaon DLL(K-pi)>0
  - D0 daughter pion DLL(K-pi)<0
  """
  _default_config = {    
    'DMassLower'             : -7.5     #MeV
    ,'DMassUpper'             : 15     #MeV 
    ,'D0MassWin'              : 75     #MeV
                            
    ,'DstarDOCA'                : 0.5    # mm
    ,'D0MaxDOCA'           : 0.5    # mm
                            
    ,'D0DaughterPt'                    : 0.3   # GeV
    ,'D0DaughterP'                     : 5.0      # GeV
    ,'D0Pt' : 2.0 # GeV
    ,'DstarPt' : 2.0 # GeV
    ,'SlowPionPt' : 0.2 # GeV

    ,'DstarVtxChi2DOF' : 50
    ,'D0VtxChi2DOF' : 9

    ,'D0DaughterIPChi2' : 0.5 
    ,'D0FDChi2' : 36
    ,'D0IPChi2' : 30
    , 'D0DIRA' : 0.9999
    ,'TrackChi2DOF'                    : 4
   
    ,'Prescale'         : 1
    ,'Postscale'        : 1
   }

  def __init__(self
               ,name='DstarPromptWithD02K3Pi'
               ,config=None):
    checkConfig(StrippingDstarPromptWithD02K3PiConf.__configuration_keys__
                , config)
    self.name=name
    
       
    self.selD02K3Pi = makeD02K3Pi(
      name+'D0'
      , massWin= config['D0MassWin']
      , maxDOCA = config['D0MaxDOCA']
      , pt = config['D0Pt']
      , daughterPt = config['D0DaughterPt']
      , daughterMom = config['D0DaughterP']
      , vertexChi2DOF = config['D0VtxChi2DOF']
      , FDChi2 = config['D0FDChi2'] 
      , IPChi2 = config['D0IPChi2']
      , daughterIPChi2 = config['D0DaughterIPChi2']
      , DIRA = config['D0DIRA']
      , trackChi2DOF = config['TrackChi2DOF']
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
   
    self.lines = [self.line_tagged
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
  , vertexChi2DOF
  , FDChi2
  , IPChi2
  , daughterIPChi2
  , DIRA
  , trackChi2DOF
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
  - vertexChi2DOF : maximum vertex chi2 / d.o.f.
  - FDChi2 : minimum vertex chi2
  - IPChi2 : maximum IP chi2 
  - daughterIPChi2 : minimum IP chi2 of D0 daughters
  - DIRA : cosine of angle sustended by momentum and flight direction vectors of D0
  - trackChi2DOF : maximum track chi2 / d.o.f. of D0 daughters (unitless)
Note: There is a hard-coded cut on the pion DLL(K-pi) of < 0    
  """
  _prefitMassCut = "(ADAMASS('D0')<%(massWin)s*MeV)" %locals()
  _postfitMassCut = "(ADMASS('D0')<%(massWin)s*MeV)" %locals()

  _prefitPtCuts = "(APT>%(pt)s*GeV) & (ANUM(PT>%(daughterPt)s*GeV)==4) & (ANUM(P>%(daughterMom)s*GeV)==4)" %locals()
 
  _prefitCuts = _prefitMassCut + " & (ADOCAMAX('')<%(maxDOCA)s*mm)" %locals()
  _prefitCuts += " & %s" %(_prefitPtCuts)

  _motherCuts = _postfitMassCut + " & (PT>%(pt)s*GeV) & (BPVDIRA>%(DIRA)s) & (BPVVDCHI2>%(FDChi2)s) & (BPVIPCHI2()<%(IPChi2)s) & (VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s) " %locals()

  _daughterCuts = "(PT>%(daughterPt)s*GeV) & (P>%(daughterMom)s*GeV) & (BPVIPCHI2()>%(daughterIPChi2)s) & (TRCHI2DOF<%(trackChi2DOF)s)" %locals()

  _stdTightKaons = DataOnDemand(Location="Phys/StdTightKaons")
  _stdTightPions = DataOnDemand(Location="Phys/StdTightPions")
 
  # hard code the DLL cut for the pions

  _d0 = CombineParticles( '_'+name
                          ,DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc"
                          ,CombinationCut = _prefitCuts
                          ,MotherCut = _motherCuts
                          ,DaughtersCuts = { "K+" : _daughterCuts
                                             ,"pi+" : _daughterCuts+" & (PIDK<0)"} )

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
  _prefitMassCut = "(AM-AM1-145.5*MeV>%(DMassLower)s*MeV) & (AM-AM1-145.5*MeV<%(DMassUpper)s*MeV)" %locals()
  _postfitMassCut = "(M-M1-145.5*MeV>%(DMassLower)s*MeV) & (M-M1-145.5*MeV<%(DMassUpper)s*MeV)" %locals()
     
  _prefitCuts = _prefitMassCut + " & (APT>%(pt)s*GeV) & (ADOCAMAX('')<%(DOCA)s*mm)" %locals()
 
  _motherCuts = _postfitMassCut + " & (PT>%(pt)s*GeV) & (VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s)" %locals()
  _slowPionCuts = "(PT>%(slowPionPt)s) & (TRCHI2DOF<%(trackChi2DOF)s)" %locals()

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

