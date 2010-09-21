'''
Module for construction of D0->K3Pi tagged (from prompt D*) and untagged stripping selections.
Provides functions to build tagged and untagged selections for physics analysis and cross section measurement.
Provides classes D02K3PiForXSectionConf and D02K3PiConf, which construct the selections and stripping lines given a configuration dictionary.
Currently, only the class for the cross-section measurement is implemented.
Exported symbols (use python help):
  - D02K3PiForXSectionConf
  - makeD02K3PiForXSection
  - makeTightD02K3PiForXSection
  - makeTightMassD02K3PiForXSection
  - makePromptDstarForXSection
'''

__author__ = ['Philip Hunt']
__date__ = '29/07/20010'
__version__ = '$Revision: 1.1 $'

__all__ = ('StrippingD02K3PiForXSectionConf', 'makeD02K3PiForXSection'
           ,'makeTightD02K3PiForXSection','makeTightD02K3PiForXSection'  
           ,'makePromptDstarForXSection')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

class StrippingD02K3PiForXSectionConf(object):
  """
  Constructor of D*->D0(K3Pi)pi stripping selection / stripping line for cross-section measurement.
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> config = {...}
  >>> myConf = StrippingD02K3PiForXSectionConf('D02K3PiForXSection',config)
  >>> myLines = myConf.lines
  >>> for line in line :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as instance data members:
    selD02K3PiSignal     : nominal D0 -> K3Pi Selection object with signal mass window
    selD02K3PiSideband   : nominal D0 -> K3Pi Selection object with sideband mass window
    selDstar2DpiSignal   : nominal D*+ -> D0(K-pi+pi-pi+)pi+  Selection object with signal mass
    selDstar2DpiSideband   : nominal D*+ -> D0(K-pi+pi-pi+)pi+  Selection object with sideband mass
    
    signal_line_untagged : StrippingLine made out of selD02K3PiSignal
    signal_line_tagged : StrippingLine made out of selDstar2DpiSignal
    sideband_line_untagged : StrippingLine made out of selD02K3PiSideband
    sideband_line_tagged : StrippingLine made out of selDstar2DpiSideband
    lines                  : List of lines, [signal_line_untagged
                                             , sideband_line_untagged
                                             ,signal_line_tagged
                                             , sideband_line_tagged]

    Exports as class data member:
    StrippingD02K3PiForXSectionConf.__configuration_keys__ : List of required configuration parameters.
    """
  __configuration_keys__ = ('DMassSignalLower'
                            ,'DMassSignalUpper'
                            ,'D0SignalMassWin'
                            ,'DMassWideLower'
                            ,'DMassWideUpper'
                            ,'D0WideMassWin'
                                                    
                            ,'DstarDOCA'
                            ,'D0MaxDOCALoose'
                            ,'D0MaxDOCATight'
                            
                            ,'D0DaughterPt'
                            ,'D0DaughterP'
                            ,'D0DaughterIPchi2Loose'
                            ,'D0DaughterIPchi2Tight'
                            ,'D0Lifetime'
                            ,'D0SignedFD'
                            ,'D0FDchi2Loose'
                            ,'D0FDchi2Tight'
                            
                            ,'TrackChi2DOF'
                            ,'DstarVtxChi2DOF'
                            ,'D0VtxChi2DOF'
                            
                            ,'PrescaleUntaggedWide'
                            ,'PostscaleUntaggedWide'
                            
                            ,'PrescaleUntaggedSignal'
                            ,'PostscaleUntaggedSignal'
                            
                            ,'PrescaleTaggedWide'
                            ,'PostscaleTaggedWide'
                            
                            ,'PrescaleTaggedSignal'
                            ,'PostscaleTaggedSignal'
                            )

  """Default configuration for the cross-section lines. Some cuts, e.g. the mass windows, pt and IP chi2 cuts, are applied to the combinations before the vertex fitting, and again after the fitting.
List of cuts used in this configuration:
  - D0 signal mass window = 24 MeV/c^2
  - D0 wide mass window = 100 MeV/c^2
  - D0 max DOCA (untagged lines) < 0.19 mm
  - D0 max DOCA (tagged lines) < 0.26 mm
  - D0 signed FD > 0
  - D0 FD chi2 (untagged lines) > 25
  - D0 FD chi2 (tagged lines) > 4 
  - D0 lifetime > 0 ps
  - D0 vertex chi2 / d.o.f. < 20
  - D0 daughter pt > 220 MeV/c
  - D0 daughter momentum > 3 GeV/c
  - D0 daughter IP chi2 (unagged lines) > 1
  - D0 daughter IP chi2 (tagged lines) > 0.5
  - Delta mass signal window (lower, upper) = -2.3, +2.3 MeV/c^2
  - Delta mass wide mass window (lower, upper) = -5, +15 MeV/c^2
  - D* DOCA < 0.28 mm
  - D* vertex chi2 / d.o.f. < 50
  - Track vertex chi2 / d.o.f. < 4
  - D0 daughter kaon DLL(K-pi)>0 (StdTightKaons)
  - D0 daughter pion DLL(K-pi)<5 (StdTightPions)
  """
  _default_config = {
    'DMassSignalLower'                 : -2.3   #MeV
    ,'DMassSignalUpper'                : 2.3    #MeV
    ,'D0SignalMassWin'                 : 24     #MeV
    ,'DMassWideLower'                  : -5     #MeV
    ,'DMassWideUpper'                  : 15     #MeV 
    ,'D0WideMassWin'                   : 100     #MeV
    
    ,'DstarDOCA'                       : 0.28    # mm
    ,'D0MaxDOCATight'                  : 0.19    # mm
    ,'D0MaxDOCALoose'                  : 0.26    # mm
    
    ,'D0DaughterPt'                    : 0.22   # GeV
    ,'D0DaughterP'                     : 3      # GeV
    
    ,'D0DaughterIPchi2Loose'   : 0.5
    ,'D0DaughterIPchi2Tight'   : 1
 
    ,'D0Lifetime'                      : 0      # ps
    ,'D0SignedFD'                      : 0      # mm
    ,'D0FDchi2Tight'                   : 25 
    ,'D0FDchi2Loose'                   : 4 
    
    ,'TrackChi2DOF'                    : 4
    ,'DstarVtxChi2DOF'                 : 50
    ,'D0VtxChi2DOF'                    : 20
    
    ,'PrescaleUntaggedWide'            : 1
    ,'PostscaleUntaggedWide'           : 1
    
    ,'PrescaleUntaggedSignal'          : 1
    ,'PostscaleUntaggedSignal'         : 1
    
    ,'PrescaleTaggedWide'              : 1
    ,'PostscaleTaggedWide'             : 1
    
    ,'PrescaleTaggedSignal'            : 1
    ,'PostscaleTaggedSignal'           : 1
                            
   }
  

  def __init__(self
               ,name='D02K3PiForXSection'
               ,config=None):
    checkConfig(StrippingD02K3PiForXSectionConf.__configuration_keys__
                , config)
    self.name=name
    signal_name = name+'Sig'
    wide_name = name+'Wide'
    
    self.selD02K3PiWideTagged = makeD02K3PiForXSection(
      wide_name+'D0ForTagged'
      , massWin= config['D0WideMassWin']
      , maxDOCA = config['D0MaxDOCALoose']
      , vertexChi2DOF = config['D0VtxChi2DOF']
      , lifetime = config['D0Lifetime']
      , signedFD = config['D0SignedFD']
      , FDchi2 = config['D0FDchi2Loose']
      , daughterPt = config['D0DaughterPt']
      , daughterMom = config['D0DaughterP']
      , daughterIPchi2 = config['D0DaughterIPchi2Loose']
      , trackChi2DOF = config['TrackChi2DOF']
      )

    # create a tighter D0 for the untagged selection
    self.selD02K3PiWideUntagged = makeTightD02K3PiForXSection(
      wide_name+'D0ForUntagged'
      , looseD0Sel = self.selD02K3PiWideTagged
      , maxDOCA = config['D0MaxDOCATight']
      , FDchi2 = config['D0FDchi2Tight']
      , daughterIPchi2 = config['D0DaughterIPchi2Tight']
    )

    # D0 with tight mass window for untagged selection
    self.selD02K3PiSignalUntagged = makeTightMassD02K3PiForXSection(
      signal_name+'D0ForUntagged'
      , looseD0Sel = self.selD02K3PiWideUntagged
      , massWin = config['D0SignalMassWin']
      )

    # D0 with tight mass for tagged selection
    self.selD02K3PiSignalTagged = makeTightMassD02K3PiForXSection(
      signal_name+'D0ForTagged'
      , looseD0Sel = self.selD02K3PiWideTagged
      , massWin = config['D0SignalMassWin']
      )

    # prompt D* wide mass region
    self.selPromptDstarWide = makePromptDstarForXSection(
      wide_name+'Dstar'
      , d0Sel = self.selD02K3PiWideTagged
      , DMassLower = config['DMassWideLower']
      , DMassUpper = config['DMassWideUpper']
      , DOCA = config['DstarDOCA']
      , vertexChi2DOF = config['DstarVtxChi2DOF']
      , trackChi2DOF = config['TrackChi2DOF']
      )

    # prompt D* signal mass region
    self.selPromptDstarSignal = makePromptDstarForXSection(
      signal_name+'Dstar'
      , d0Sel = self.selD02K3PiSignalTagged
      , DMassLower = config['DMassSignalLower']
      , DMassUpper = config['DMassSignalUpper']
      , DOCA = config['DstarDOCA']
      , vertexChi2DOF = config['DstarVtxChi2DOF']
      , trackChi2DOF = config['TrackChi2DOF']
      )
    
    self.signal_line_tagged = StrippingLine(
      signal_name+"TaggedLine"
      ,prescale=config['PrescaleTaggedSignal']
      ,postscale=config['PostscaleTaggedSignal']
      ,algos=[self.selPromptDstarSignal]
      )

    self.wide_line_tagged = StrippingLine(
      wide_name+"TaggedLine"
      ,prescale=config['PrescaleTaggedWide']
      ,postscale=config['PostscaleTaggedWide']
      ,algos=[self.selPromptDstarWide]
      )
    self.signal_line_untagged = StrippingLine(
      signal_name+"UntaggedLine"
      ,prescale=config['PrescaleUntaggedSignal']
      ,postscale=config['PostscaleUntaggedSignal']
      ,algos=[self.selD02K3PiSignalUntagged]
      )
    self.wide_line_untagged = StrippingLine(
      wide_name+"UntaggedLine"
      ,prescale=config['PrescaleUntaggedWide']
      ,postscale=config['PostscaleUntaggedWide']
      ,algos=[self.selD02K3PiWideUntagged]
      )
    
    self.lines = [self.signal_line_tagged
                  , self.wide_line_tagged
                  , self.signal_line_untagged
                  , self.wide_line_untagged
                  ]

def makeD02K3PiForXSection (
  name
  , massWin
  , maxDOCA
  , vertexChi2DOF
  , lifetime
  , signedFD
  , FDchi2
  , daughterPt
  , daughterMom
  , daughterIPchi2
  , trackChi2DOF
  ):
  """Creates a D0->K3Pi Selection object
, with cuts for cross-section measurement.
Uses DataOnDemand objects 'Phys/StdLooseKaons'
and 'Phys/StdLoosePions'
Arguments:
  -  name : name of Selection
  - massWin : mass window cut (MeV/c^2)
  - maxDOCA : maximum DOCA of D0 daughters (mm)
  - vertexChi2DOF : D0 vertex chi2 / d.o.f. (unitless)
  - lifetime : minimum lifetime of D0 (ps)
  - signedFD : minimum D0 signed flight distance (mm)
  - FDchi2 : minimum D0 flight distance chi2 (unitless)
  - daughterPt : minimum transverse monentum of D0 daughters (GeV/c)
  - daughterMom : minimum momentum of D0 daughters (GeV/c)
  - daughterIPchi2 : smallest impact parameter chi2 of D0 daughters (unitless)
  - trackChi2DOF : maximum track chi2 / d.o.f. of D0 daughters (unitless)
  """
  _prefitMassCut = "(ADAMASS('D0')<%(massWin)s*MeV)" %locals()
  _postfitMassCut = "(ADMASS('D0')<%(massWin)s*MeV)" %locals()

  _prefitIPChi2Cuts = "(ANUM(BPVIPCHI2()>%(daughterIPchi2)s)==4)" %locals()
  _prefitPtCuts = "(ANUM(PT>%(daughterPt)s*GeV)==4) & (ANUM(P>%(daughterMom)s*GeV)==4)" %locals()
  
  _prefitCuts = _prefitMassCut + " & (ADOCAMAX('')<%(maxDOCA)s*mm)" %locals()
  _prefitCuts += " & %s & %s" %( _prefitPtCuts, _prefitIPChi2Cuts)

  _motherCuts = _postfitMassCut + " & (DOCAMAX<%(maxDOCA)s*mm) & (BPVVDSIGN>%(signedFD)s) & (BPVVDCHI2>%(FDchi2)s) & (VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s) & (BPVLTIME('PropertimeFitter/ProperTime:PUBLIC')>%(lifetime)s*ps)" %locals()

  _daughterCuts = "(PT>%(daughterPt)s*GeV) & (P>%(daughterMom)s*GeV) & (TRCHI2DOF<%(trackChi2DOF)s) & (BPVIPCHI2()>%(daughterIPchi2)s)" %locals()

  _stdTightKaons = DataOnDemand(Location="Phys/StdTightKaons")
  _stdTightPions = DataOnDemand(Location="Phys/StdTightPions")
  
  _d0 = CombineParticles( '_'+name
                          ,DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc"
                          ,CombinationCut = _prefitCuts
                          ,MotherCut = _motherCuts
                          ,DaughtersCuts = { "K+" : _daughterCuts
                                             ,"pi+" : _daughterCuts } )

  return Selection ( name
                     ,Algorithm = _d0
                     ,RequiredSelections = [_stdTightKaons, _stdTightPions] )


def makeTightD02K3PiForXSection(name, looseD0Sel 
  ,maxDOCA, FDchi2, daughterIPchi2):
  """Filter the given D0 Selection with tighter cuts."""
  _code = "(DOCAMAX<%(maxDOCA)s*mm)" %locals()
  _code += " & (BPVVDCHI2>%(FDchi2)s)" %locals()
  _code += " & (MINTREE((('K+'==ABSID)|('pi+'==ABSID)), BPVIPCHI2())>%(daughterIPchi2)s)" %locals()
  _filterD0 = FilterDesktop('_'+name
                            ,Code=_code)

  return Selection(name
                   ,Algorithm=_filterD0
                   ,RequiredSelections=[looseD0Sel])

def makeTightMassD02K3PiForXSection(name, looseD0Sel, massWin):
  """Filter the given D0 Selection with a tight mass window."""
  _code="(ADMASS('D0')<%(massWin)s)" %locals()
  _filterD0 = FilterDesktop('_'+name
                            ,Code=_code)

  return Selection(name
                   ,Algorithm=_filterD0
                   ,RequiredSelections=[looseD0Sel])

def makePromptDstarForXSection(
  name
  ,d0Sel
  ,DMassLower
  ,DMassUpper
  ,DOCA
  ,vertexChi2DOF
  ,trackChi2DOF
  ):
  """Creates a D*->D0pi Selection object
  , with cuts for cross-section measurement.
  Uses DataOnDemand object 'Phys/StdNoPIDPions'
  Arguments:
  -  name : name of Selection
  - d0Sel : D0 Selection object
  - DMassLower : lower edge of delta mass window (MeV/c^2)
  - DMassUpper : upper edge of delta mass window (MeV/c^2)
  - DOCA : distance of closest approach (mm)
  - vertexChi2DOF : maximum vertex chi2 / d.o.f. (unitless)
  - trackChi2DOF : track chi2 / d.o.f. of the bachelor pion (unitless)
Note that the delta mass is defined here as the difference between the D* and D0 reconstructed masses subtracted from the PDG value for the D* - D0 mass difference.
  """
  _prefitMassCut = "(AM-AM1-145.5*MeV>%(DMassLower)s*MeV) & (AM-AM1-145.5*MeV<%(DMassUpper)s*MeV)" %locals()
  _postfitMassCut = "(M-M1-145.5*MeV>%(DMassLower)s*MeV) & (M-M1-145.5*MeV<%(DMassUpper)s*MeV)" %locals()
     
  _prefitCuts = _prefitMassCut + " & (ADOCAMAX('')<%(DOCA)s*mm)" %locals()
 
  _motherCuts = _postfitMassCut + " & (DOCAMAX<%(DOCA)s*mm) & (VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s)" %locals()
  _slowPionCuts = "(TRCHI2DOF<%(trackChi2DOF)s)" %locals()
  _stdNoPIDsPions = DataOnDemand(Location="Phys/StdNoPIDsPions")

  _dstar = CombineParticles (
    "_"+name
    ,DecayDescriptor="[D*(2010)+ -> D0 pi+]cc"
    ,CombinationCut = _prefitCuts
    ,MotherCut = _motherCuts
    ,DaughtersCuts = {"D0":"ALL", "pi+" : _slowPionCuts}
    )

  return Selection(
    name
    ,Algorithm=_dstar
    ,RequiredSelections=[_stdNoPIDsPions, d0Sel]
    )

