'''
Module for construction of D0->K3Pi tagged (from prompt D*) and untagged stripping selections.
Provides functions to build tagged and untagged selections for open charm cross section measurements.
Provides classes D02K3PiForXSectionConf, which construct the selections and stripping lines given
a configuration dictionary. An example configuration dictionary called default_config is included in this module
Exported symbols (use python help):
  D02K3PiForXSConf
  makeD02K3Pi
  makeDstar2D0pi
'''
__author__ = ['Philip Hunt']
__date__ = '21/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('D02K3PiForXSecConf', 'makeD02K3Pi'
           ,'makeDstar2D0pi', 'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder, checkConfig
import StandardParticles
if hasattr(StandardParticles, "StdAllLoosePions"):
  from StandardParticles import StdAllLoosePions as StdAllLoosePions
else:
  from StandardParticles import StdNoPIDsPions as StdAllLoosePions
#from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons, StdAllLoosePions
from StandardParticles import StdNoPIDsPions, StdNoPIDsKaons
from GaudiKernel.SystemOfUnits import *
from copy import copy

moduleName="D02K3PiForXSec"

class D02K3PiForXSecConf(LineBuilder):
  """
  Constructor of D*->D0(K3Pi)pi stripping selection / stripping line for cross-section measurement.
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> config = {...}
  >>> myConf = StrippingD02K3PiForXSecConf('D02K3PiForXSec',config)
  >>> myLines = myConf.lines
  >>> for line in myLines :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as instance data members:
    makeD02K3Pi     : nominal D0 -> K3Pi Selection object
    makeDstar2Dpi   : nominal D*+ -> D0(K-pi+pi-pi+)pi+  Selection object

    signal_line_untagged : StrippingLine made out of makeD02K3Pi
    signal_line_tagged : StrippingLine made out of makeDstar2Dpi
    lines                  : List of lines, [line_untagged
                                             ,line_tagged]
  """

  __configuration_keys__ = {
    'D0Cuts' : ( 'CombMassWin'
                 ,'MassWin'
                 ,'MaxDOCA'
                 ,'DauPtAllTracks'
                 ,'DauPtThreeTracks'
                 ,'DauPtTwoTracks'
                 ,'DauPtOneTrack'

                 ,'DauP'

                 ,'DauIPchi2AllTracks'
                 ,'DauIPchi2ThreeTracks'
                 ,'DauIPchi2TwoTracks'
                 ,'DauIPchi2OneTrack'

                 ,'FDchi2'
                 ,'DIRA'
                 ,'VtxChi2DOF'

                 ,'DauTrackChi2DOF'

                 ,'ApplyKaonPIDK'
                 ,'KaonPIDK'
                 ,'ApplyPionPIDK'
                 ,'PionPIDK'
                 ),

    'DstarCuts' : ( 'CombDeltaMLower'
                    ,'CombDeltaMUpper'
                    ,'DeltaMLower'
                    ,'DeltaMUpper'
                    ,'DOCA'
                    ,'SlowPiTrackChi2DOF'
                    ,'VtxChi2DOF'
                    ),
    'HltFilter' : ('Tagged', 'Untagged'),

    'CheckPV' : ( 'Tagged', 'Untagged' ),
    'Prescale' : ( 'Tagged', 'Untagged' ),
    'Postscale' : ( 'Tagged', 'Untagged' ),
    'ApplyGECs' : ( 'Tagged', 'Untagged' ),
    'MaxLongTracks' : ( 'Tagged', 'Untagged' ),
    'MaxSpdDigits' : ( 'Tagged', 'Untagged' ),
    'MaxITClusters' : ( 'Tagged', 'Untagged' ),
    'MaxVeloTracks' : ( 'Tagged', 'Untagged' )
    }


  ## Possible parameters and default values copied from the definition
  ##   of StrippingLine
  def _strippingLine ( self,
                        name             ,   # the base name for the Line
                        prescale  = 1.0  ,   # prescale factor
                        ODIN      = None ,   # ODIN predicate
                        L0DU      = None ,   # L0DU predicate
                        HLT       = None ,   # HltDecReports predicate
                        FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                        checkPV   = True ,   # Check PV before running algos
                        algos     = None ,   # the list of stripping members
                        selection = None ,
                        postscale = 1.0    ,   # postscale factor
                        MaxCandidates = "Override",   # Maxumum number
                        MaxCombinations = "Override", # Maxumum number
                        HDRLocation = None ) : # other configuration parameters
  # {

    if (prescale > 0) and (postscale > 0) : # {
      line = StrippingLine( name,
                            prescale        = prescale,
                            ODIN            = ODIN,
                            L0DU            = L0DU,
                            HLT             = HLT,
                            FILTER          = FILTER,
                            checkPV         = checkPV,
                            algos           = algos,
                            selection       = selection,
                            postscale       = postscale,
                            MaxCandidates   = MaxCandidates,
                            MaxCombinations = MaxCombinations,
                            HDRLocation     = HDRLocation )

      self.registerLine(line)
      return line
    # }
    else : 
      return False

  # }


  def __init__(self, moduleName, config):
    LineBuilder.__init__(self, moduleName, config)

    from Configurables import LoKi__VoidFilter as VoidFilter
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    modules = CoreFactory('CoreFactory').Modules
    for i in ['LoKiTracks.decorators', 'LoKiNumbers.decorators']:
      if i not in modules : modules.append(i)

    selD02K3Pi = makeD02K3Pi(moduleName, config['D0Cuts'])
    selDstar2D0pi = makeDstar2D0pi(moduleName, selD02K3Pi
                                   ,config['DstarCuts'])

    _lines = {"Untagged" : selD02K3Pi
              ,"Tagged" : selDstar2D0pi }

    self.line_untagged=None
    self.line_tagged=None

    for name, sel in _lines.iteritems():
     _filter=None
     if config['ApplyGECs'][name]:

       _filter = ""

       nLong = config["MaxLongTracks"][name]
       if nLong is not None:
         if _filter != "":
           _filter+=" & "
         _filter += "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s)" %nLong

       nDigits = config["MaxSpdDigits"][name]
       if nDigits is not None:
         if _filter != "":
           _filter+=" & "
         _filter += "( recSummary(LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits') < %s ) " %nDigits

       nClusters = config["MaxITClusters"][name]
       if nClusters is not None:
         if _filter != "":
           _filter+=" & "
         _filter += " ( recSummary(LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters') < %s ) )" %nClusters

       nVELO = config["MaxVeloTracks"][name]
       if nVELO is not None:
         if _filter != "":
           _filter+=" & "
         _filter += "(recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) < %s)" %nVELO

     if _filter is not None and _filter!="" and config['ApplyGECs'][name]:
         _filter={"Code":_filter, "Preambulo": ["from LoKiTracks.decorators import *", "from LoKiNumbers.decorators import *"]}

     _line = self._strippingLine(name = sel.name()+"Line"
                           ,prescale = config['Prescale'][name]
                           ,postscale = config['Postscale'][name]
                           ,selection = sel
                           ,checkPV = config['CheckPV'][name]
                           ,FILTER = _filter
                           ,HLT = config['HltFilter'][name])
     if name=='Untagged': self.line_untagged=_line
     else: self.line_tagged=_line



def makeD02K3Pi(moduleName, config):
  __configuration_keys__ = ( 'CombMassWin', 'MassWin', 'MaxDOCA'
                             ,'DauPtAllTracks', 'DauPtThreeTracks'
                             ,'DauPtTwoTracks', 'DauPtOneTrack'
                             ,'DauP', 'DauIPchi2AllTracks'
                             ,'DauIPchi2ThreeTracks', 'DauIPchi2TwoTracks'
                             ,'DauIPchi2OneTrack', 'FDchi2', 'DIRA'
                             ,'VtxChi2DOF', 'DauTrackChi2DOF'
                             ,'ApplyKaonPIDK' ,'KaonPIDK'
                             ,'ApplyPionPIDK', 'PionPIDK'
                             )
  checkConfig(__configuration_keys__, config)

  _prefitCuts = "(ADAMASS('D0')<%(CombMassWin)s) & " \
                "(ADOCAMAX('LoKi::TrgDistanceCalculator')<%(MaxDOCA)s) & " \
                "(ANUM(BPVIPCHI2()>%(DauIPchi2AllTracks)s)==4) & " \
                "(ANUM(BPVIPCHI2()>%(DauIPchi2ThreeTracks)s)>=3) & " \
                "(ANUM(BPVIPCHI2()>%(DauIPchi2TwoTracks)s)>=2) & " \
                "(ANUM(BPVIPCHI2()>%(DauIPchi2OneTrack)s)>=1) & " \
                "(ANUM(P>%(DauP)s)==4) & " \
                "(ANUM(PT>%(DauPtAllTracks)s)==4) & " \
                "(ANUM(PT>%(DauPtThreeTracks)s)>=3) & " \
                "(ANUM(PT>%(DauPtTwoTracks)s)>=2) & " \
                "(ANUM(PT>%(DauPtOneTrack)s)>=1)" %config

  _motherCuts = "(ADMASS('D0')<%(MassWin)s) & " \
                "(DOCAMAX<%(MaxDOCA)s) & " \
                "(BPVVDCHI2>%(FDchi2)s) & " \
                "(VFASPF(VCHI2/VDOF)<%(VtxChi2DOF)s)" %config

  _kaonCuts = "(TRCHI2DOF<%(DauTrackChi2DOF)s) & " \
              "(PT>%(DauPtAllTracks)s) & " \
              "(P>%(DauP)s) & " \
              "(BPVIPCHI2()>%(DauIPchi2AllTracks)s)" %config

  _pionCuts = "(TRCHI2DOF<%(DauTrackChi2DOF)s) & " \
              "(PT>%(DauPtAllTracks)s) & " \
              "(P>%(DauP)s) & " \
              "(BPVIPCHI2()>%(DauIPchi2AllTracks)s)" %config

  if config['ApplyKaonPIDK']:
    _kaonCutsOLD=copy(_kaonCuts)
    _kaonCuts="(PIDK>%(KaonPIDK)s) & (HASRICH) & " %config
    _kaonCuts+=_kaonCutsOLD

  if config['ApplyPionPIDK']:
    _pionCutsOLD=copy(_pionCuts)
    _pionCuts="(PIDK<%(PionPIDK)s) & (HASRICH) & " %config
    _pionCuts+=_pionCutsOLD

  _d0 = CombineParticles( DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc"
                          ,CombinationCut = _prefitCuts
                          ,MotherCut = _motherCuts
                          ,DaughtersCuts = { "K+" : _kaonCuts
                                             ,"pi+" : _pionCuts } )
  return Selection ( 'D02K3PiFor'+moduleName
                     ,Algorithm = _d0
                     ,RequiredSelections = [StdNoPIDsPions, StdNoPIDsKaons] )


def makeDstar2D0pi(moduleName, d0Sel, config):
  __configuration_keys__ = ( 'CombDeltaMLower', 'CombDeltaMUpper'
                             ,'DeltaMLower', 'DeltaMUpper'
                             ,'DOCA' ,'SlowPiTrackChi2DOF'
                             ,'VtxChi2DOF'
                             )
  checkConfig(__configuration_keys__, config)

  _prefitCuts = "(AM-AMAXCHILD(M,'D0'==ABSID)-145.4*MeV>%(CombDeltaMLower)s) & " \
                "(AM-AMAXCHILD(M,'D0'==ABSID)-145.4*MeV<%(CombDeltaMUpper)s) & " \
                "(ADOCAMAX('')<%(DOCA)s)" %config

  _motherCuts = "(M-MAXTREE('D0'==ABSID,M)-145.4*MeV>%(DeltaMLower)s) & " \
                "(M-MAXTREE('D0'==ABSID,M)-145.4*MeV<%(DeltaMUpper)s) & " \
                "(DOCAMAX<%(DOCA)s) & " \
                "(VFASPF(VCHI2/VDOF)<%(VtxChi2DOF)s)" %config

  _slowPiCuts = "(TRCHI2DOF<%(SlowPiTrackChi2DOF)s)" %config

  _dstar = CombineParticles ( DecayDescriptor="[D*(2010)+ -> D0 pi+]cc"
                              ,CombinationCut = _prefitCuts
                              ,MotherCut = _motherCuts
                              ,DaughtersCuts = {"D0":"ALL", "pi+" : _slowPiCuts}
                              )

  return Selection(
    'Dstar2D0pi_D02K3PiFor'+moduleName
    ,Algorithm=_dstar
    ,RequiredSelections=[StdAllLoosePions, d0Sel]
    )

default_config = {
  'D0Cuts' : { 'CombMassWin' : 80*MeV
               ,'MassWin' : 75*MeV
               ,'MaxDOCA' : 0.25*mm
               ,'DauPtAllTracks' : 250*MeV
               ,'DauPtThreeTracks' : 300*MeV
               ,'DauPtTwoTracks' : 350*MeV
               ,'DauPtOneTrack' : 400*MeV
               ,'DauP' : 3*GeV
               ,'DauIPchi2AllTracks' : 1.5
               ,'DauIPchi2ThreeTracks' : 3.5
               ,'DauIPchi2TwoTracks' : 7
               ,'DauIPchi2OneTrack' : 7
               ,'FDchi2' : 16
               ,'DIRA' : 0.99975
               ,'VtxChi2DOF' : 20
               ,'DauTrackChi2DOF' : 4
               ,'ApplyKaonPIDK' : True
               ,'KaonPIDK' : 5
               ,'ApplyPionPIDK' : False
               ,'PionPIDK' : 0
               }
  ,'DstarCuts' : { 'CombDeltaMLower' : -8.25*MeV
                   ,'CombDeltaMUpper' : 16.5*MeV
                   ,'DeltaMLower' : -7.5*MeV
                   ,'DeltaMUpper' : 15*MeV
                   ,'DOCA' : 0.45*mm
                   ,'SlowPiTrackChi2DOF' : 4
                   ,'VtxChi2DOF' : 100 }
  ,'HltFilter' : {'Tagged' : "HLT_PASS_RE('Hlt1MB.*')"
                  ,'Untagged' : "HLT_PASS_RE('Hlt1MB.*')"
                  }
  ,'CheckPV' : {'Tagged' : True
                ,'Untagged' : True }
  ,'Prescale' : {'Tagged' : 1
                  ,'Untagged' : 1 }
  ,'Postscale' : {'Tagged' : 1
                   ,'Untagged' : 1 }
  ,'ApplyGECs' : {'Tagged' : False
                  ,'Untagged' : False }
  ,'MaxLongTracks' : {'Tagged' : None
                      ,'Untagged' : None},
  'MaxSpdDigits' : {'Tagged' : None
                    ,'Untagged' : None },
  'MaxITClusters' : {'Tagged' : None
                     ,'Untagged' : None },
  'MaxVeloTracks' : {'Tagged' : None
                     , 'Untagged' : None },
  }
