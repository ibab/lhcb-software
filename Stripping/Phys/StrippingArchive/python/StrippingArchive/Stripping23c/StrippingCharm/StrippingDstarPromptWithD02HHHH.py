'''
Module for construction of D0->hhhh tagged (from prompt D*) selections, where D->hhhh comprises:
  - D->K3pi Cabibbo Favoured
  - D->K3pi Cabibbo suppressed
  - D->KKpipi
  - D->4pi
  - D->KKKpi Cabibbo Favoured // Paras Naik, October 2012
  - D->KKKpi Cabibbo Suppressed // Paras Naik, October 2012
Provides functions to build tagged D->hhhh selections for physics analysis.
Provides class DstarPromptWithD02HHHHConf, which constructs the selections and stripping lines given a configuration dictionary.
Exported symbols (use python help):
  - DstarPromptWithD02HHHHConf
  - makeD02K3Pi
  - makeD02K3PiDCS
  - makeD02KKPiPi
  - makeD02FourPi
  - makeD02Pi3K // Paras Naik, October 2012
  - makeD02Pi3KDCS // Paras Naik, October 2012
  - makePromptDstar
'''

__author__ = ['Philip Hunt']
__date__ = '26/08/2011'
__version__ = '$Revision: 1.2 $'

__all__ = ('DstarPromptWithD02K3PiConf'
           ,'makeD02K3Pi'
           ,'makeD02K3PiDCS'
           ,'makeD02KKPiPi'
           ,'makeD02FourPi'
           ,'makeD02Pi3K'
           ,'makeD02Pi3KDCS'
           ,'makePromptDstar')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import checkConfig, LineBuilder
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from copy import copy

moduleName='DstarPromptWithD02HHHH'

# What follows documents the cuts to
# be implemented by the Stripping team in
# StrippingSettings (valid for Stripping 17,
# 26/08/2011).

default_config = {
        'NAME' : 'DstarPromptWithD02HHMuMuControl',
        'BUILDERTYPE' : 'DstarPromptWithD02HHHHConf',
         'CONFIG' : {
            'CombDelmLower'     : -9.*MeV
       ,'CombDelmUpper'    : 20*MeV
       ,'DelmLower'        : -8*MeV
       ,'DelmUpper'        : 18*MeV
       ,'CombD0MassWin'    : 120*MeV
       ,'D0MassWin'        : 100*MeV
       ,'DstarDOCA'        : 0.3*mm
       ,'D0MaxDOCA'        : 0.3*mm
       ,'D0DauPt'          : 300*MeV
       ,'D0DauP'           : 3*GeV
       ,'D0Pt'             : 2.*GeV
       ,'DstarPt'          : 2.*GeV
       ,'SlowPionPt'       : 120*MeV
       ,'DstarVtxChi2DOF'  : 20
       ,'D0VtxChi2DOF'     : 20
       ,'D0DauMaxIPChi2'   : 9
       ,'D0DauIPChi2'      : 3.0 # !!!
       ,'D0FDChi2'         : 30
       ,'D0IPChi2'         : 36
       ,'D0DIRA'           : 0.9998
       ,'TrackChi2DOF'     : 3
       ,'ApplyKaonPIDK'    : False
       ,'KaonPIDK'         : 0.
       ,'ApplyPionPIDK'    : False
       ,'PionPIDK'         : 3 # not applied to the slow pion
       ,'CheckPV'          : True # reject events with no PVs
       ,'ApplyGECs'        : False # set to False to apply no GECs
       ,'MaxLongTracks'    : 250
       ,'MaxVeloTracks'    : None
       ,'MaxSpdDigits'     : None
       ,'MaxITClusters'    : None
       ,'ApplyGhostProbCut': False # !!!
       ,'GhostProbCut'     : 0.4 # !!!
       ,'Prescale'         : 0.05
       ,'Postscale'        : 1
       ,'RunK3pi'          : True
       ,'Run4pi'           : True
       ,'Run2K2pi'         : True
       ,'Run3Kpi'          : True
       } ,
       'WGs' : [ 'Charm' ],
       'STREAMS' : [ 'Charm' ]
       }


class DstarPromptWithD02HHHHConf(LineBuilder):
  """
  Constructor of D*->D0(4h)pi stripping selection / stripping lines for
  physics analysis.
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> config = {...}
  >>> myConf = StrippingPromptDstarWithD02HHHHConf('PromptDstarWithD02HHHH',config)
  >>> myLines = myConf.lines()
  >>> for line in myLines :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as instance data members:
    line_tagged_d02hhhh : StrippingLine made out of selDstar2Dpi

  Exports as class data member:
    DstarPromptWithD02HHHHConf.__configuration_keys__ : List of required configuration parameters.
    """
  __configuration_keys__ = (
    'CombD0MassWin'
    ,'CombDelmLower'
    ,'CombDelmUpper'
    ,'D0MassWin'
    ,'DelmLower'
    ,'DelmUpper'

    ,'DstarDOCA'
    ,'D0MaxDOCA'

    ,'D0DauPt'
    ,'D0DauP'
    ,'D0Pt'
    ,'DstarPt'
    ,'SlowPionPt'

    ,'DstarVtxChi2DOF'
    ,'D0VtxChi2DOF'

    ,'D0DauMaxIPChi2'
    ,'D0DauIPChi2'
    ,'D0FDChi2'
    ,'D0IPChi2'

    ,'D0DIRA'

    ,'TrackChi2DOF'

    ,'ApplyKaonPIDK'
    ,'KaonPIDK'

    ,'ApplyPionPIDK'
    ,'PionPIDK'

    ,'ApplyGhostProbCut'
    ,'GhostProbCut'

    ,'CheckPV'

    ,'ApplyGECs'
    ,'MaxLongTracks'
    ,'MaxVeloTracks'
    ,'MaxSpdDigits'
    ,'MaxITClusters'

    ,'RunK3pi'
    ,'Run4pi'
    ,'Run2K2pi'
    ,'Run3Kpi'

    ,'Prescale'
    ,'Postscale'
    )

  def __init__(self, moduleName, config):
    LineBuilder.__init__(self, moduleName, config)

    selD02hhhh = makeD02hhhh(
      moduleName = moduleName
      ,combMassWin = config['CombD0MassWin']
      ,massWin = config['D0MassWin']
      ,maxDOCA = config['D0MaxDOCA']
      ,pt = config['D0Pt']
      ,dauPt = config['D0DauPt']
      ,dauMom = config['D0DauP']
      ,vtxChi2DOF = config['D0VtxChi2DOF']
      ,FDChi2 = config['D0FDChi2']
      ,IPChi2 = config['D0IPChi2']
      ,dauMaxIPChi2 = config['D0DauMaxIPChi2']
      ,dauIPChi2 = config['D0DauIPChi2']
      ,DIRA = config['D0DIRA']
      ,trackChi2DOF = config['TrackChi2DOF']
      ,applyKaonPIDK = config['ApplyKaonPIDK']
      ,kaonPIDK = config['KaonPIDK']
      ,applyPionPIDK = config['ApplyPionPIDK']
      ,pionPIDK = config['PionPIDK']
      ,applyGhostProbCut = config['ApplyGhostProbCut']
      ,ghostProbCut = config['GhostProbCut']
      ,runK3pi = config['RunK3pi']
      ,run4pi = config['Run4pi']
      ,run2K2pi = config['Run2K2pi']
      ,run3Kpi = config['Run3Kpi']
      )

    selPromptDstar = makePromptDstar(
      moduleName = moduleName
      ,selection = selD02hhhh
      ,combDelmLower = config['CombDelmLower']
      ,combDelmUpper = config['CombDelmUpper']
      ,delmLower = config['DelmLower']
      ,delmUpper = config['DelmUpper']
      ,DOCA = config['DstarDOCA']
      ,vertexChi2DOF = config['DstarVtxChi2DOF']
      ,pt = config['DstarPt']
      ,slowPionPt = config['SlowPionPt']
      ,trackChi2DOF = config['TrackChi2DOF']
      )

    _GECfilter = None

    if config['ApplyGECs']:
      _filter = ""

      nLong = config["MaxLongTracks"]
      if nLong is not None:
        if _filter != "":
          _filter+=" & "
        _filter += "( recSummaryTrack(LHCb.RecSummary.nLongTracks,TrLONG) < %s )" %nLong

      nDigits = config["MaxSpdDigits"]
      if nDigits is not None:
        if _filter != "":
          _filter+=" & "
        _filter += "( recSummary(LHCb.RecSummary.nSpdhits,'Raw/Spd/Digits') < %s )" %nDigits

      nClusters = config["MaxITClusters"]
      if nClusters is not None:
        if _filter != "":
          _filter+=" & "
        _filter += " ( recSummary(LHCb.RecSummary.nITClusters,'Raw/IT/Clusters') < %s )" %nClusters

      nVELO = config["MaxVeloTracks"]
      if nVELO is not None:
        if _filter != "":
          _filter+=" & "
        _filter += "( recSummaryTrack(LHCb.RecSummary.nVeloTracks,TrVELO) < %s )" %nVELO


      if _filter != "":
        _GECfilter = {'Code' : _filter,
                      'Preambulo' : ["from LoKiTracks.decorators import *",
                                     'from LoKiNumbers.decorators    import *',
                                     'from LoKiCore.functions    import *']
                      }


    self.line_tagged_d02hhhh = StrippingLine(
      moduleName+"Line"
      ,prescale=config['Prescale']
      ,postscale=config['Postscale']
      ,selection=selPromptDstar
      ,checkPV=config['CheckPV']
      ,RelatedInfoTools = [
        { 
          'Type' : 'RelInfoConeVariables', 'ConeAngle' : 1.5, 'Variables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM'], 
          'RecursionLevel' : 1, 
          'Locations' : {
            selPromptDstar : 'P2CVDstar1', 
            selD02hhhh   : 'P2CVD1', 
          } 
        }, 
      ]
      ,FILTER=_GECfilter)
    self.registerLine(self.line_tagged_d02hhhh)

def makeD02hhhh (
  moduleName
  ,combMassWin
  ,massWin
  ,maxDOCA
  ,pt
  ,dauPt
  ,dauMom
  ,vtxChi2DOF
  ,FDChi2
  ,IPChi2
  ,dauMaxIPChi2
  ,dauIPChi2
  ,DIRA
  ,trackChi2DOF
  ,applyKaonPIDK
  ,kaonPIDK
  ,applyPionPIDK
  ,pionPIDK
  ,applyGhostProbCut
  ,ghostProbCut
  ,runK3pi
  ,run4pi
  ,run2K2pi
  ,run3Kpi
  ):
  """Creates a D0->hhhh Selection object, merging D0->K3pi CF
  , D0->K3pi DCS, D0->pi3K CF
  , D0->pi3K DCS, D0->KKpipi and D0->4pi, with cuts for physics analysis.
  Uses StandardParticle objects 'StdAllNoPIDsKaons' and 'StdAllNoPIDsPions'
  for lines without PID cuts, and 'StdAllLooseKaons' and 'StdAllLoosePions'
  for line with PID cuts.
  Arguments:
    - moduleName : name of Selection
    - combMassWin : mass window cut on combination (MeV/c^2)
    - massWin : mass window cut (MeV/c^2)
    - maxDOCA : maximum DOCA of D0 daughters (mm)
    - pt : minimum transverse momentum of D0 (MeV/c)
    - dauPt : minimum transverse momentum of D0 daughters (MeV/c)
    - dauMom : minimum momentum of D0 daughters (MeV/c)
    - vtxChi2DOF : maximum vertex chi2 / d.o.f.
    - FDChi2 : minimum vertex chi2
    - IPChi2 : maximum IP chi2
    - dauMaxIPChi2 :require at leat one D0 daughter with IP chi2 greather than this value
    - dauIPChi2 : minimum IP chi2 of D0 daughters
    - DIRA : cosine of angle sustended by momentum and flight direction vectors of D0
    - trackChi2DOF : maximum track chi2 / d.o.f. of D0 daughters (unitless)
    - applyKaonPIDK : boolean for whether we apply a kaon PIDK cut
    - kaonPIDK : DLL(K-pi) cut applied to kaon
    - applyPionPIDK : boolean for whether we apply a pion PIDK cut
    - pionPIDK : DLL(K-pi) cut applied to pions
  """
  _prefitCuts = "(ADAMASS('D0')<%(combMassWin)s) & (APT>%(pt)s) & " \
                "(AMAXDOCA('')<%(maxDOCA)s) & " \
                "(AHASCHILD(((ABSID=='K+') | (ABSID=='pi+')) & " \
                "(MIPCHI2DV(PRIMARY)>%(dauMaxIPChi2)s)))" %locals()

  _motherCuts = "(VFASPF(VCHI2/VDOF)<%(vtxChi2DOF)s) & " \
                "(BPVVDCHI2>%(FDChi2)s) & (BPVIPCHI2()<%(IPChi2)s) & " \
                "(BPVDIRA>%(DIRA)s) & (ADMASS('D0')<%(massWin)s) & " \
                "(PT>%(pt)s)" %locals()

  _kaonCuts = "(TRCHI2DOF<%(trackChi2DOF)s)" \
              " &(PT>%(dauPt)s)&(P>%(dauMom)s)" \
              " & (MIPCHI2DV(PRIMARY)>%(dauIPChi2)s)" \
              %locals()
  _pionCuts = copy(_kaonCuts)

  if applyKaonPIDK:
    _kaonCutsOLD = copy(_kaonCuts)
    _kaonCuts="(PIDK>%(kaonPIDK)s) & (HASRICH) & " %locals()
    _kaonCuts+=_kaonCutsOLD
  if applyPionPIDK:
    _pionCutsOLD = copy(_pionCuts)
    _pionCuts="(PIDK<%(pionPIDK)s) & (HASRICH) & " %locals()
    _pionCuts+=_pionCutsOLD
  if applyGhostProbCut:
    _ghostCut = "( TRGHOSTPROB < %(ghostProbCut)s )" %locals()
    _kaonCutsOLD = copy(_kaonCuts)
    _pionCutsOLD = copy(_pionCuts)
    _kaonCuts = _kaonCutsOLD + " & " + _ghostCut
    _pionCuts = _pionCutsOLD + " & " + _ghostCut
    

  from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons
  from StandardParticles import StdAllLoosePions, StdAllLooseKaons

  _kaons = StdAllNoPIDsKaons
  _pions = StdAllNoPIDsPions
  if applyKaonPIDK:
    _kaons = StdAllLooseKaons
  if applyPionPIDK:
    _pions = StdAllLoosePions

  _conjPID = ConjugateNeutralPID()

  _d02k3pi = CombineParticles (DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"pi+" : _pionCuts }
                               )
  _selD02K3Pi = Selection('D02K3PiFor'+moduleName
                          ,Algorithm=_d02k3pi
                          ,RequiredSelections=[_pions,_kaons])

  _selD02K3PiConj = Selection('D02K3PiConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02K3Pi])

  _d02pi3k = copy(_d02k3pi)
  _d02pi3k.DecayDescriptor="[D0 -> K+ K- K- pi+]cc"

  _selD02Pi3K = Selection('D02Pi3KFor'+moduleName
                          ,Algorithm=_d02pi3k
                          ,RequiredSelections=[_pions,_kaons])

  _selD02Pi3KConj = Selection('D02Pi3KConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02Pi3K])

  _d02kkpipi = copy(_d02k3pi)
  _d02kkpipi.DecayDescriptor="D0 -> K+ K- pi+ pi-"

  _selD02KKPiPi = Selection('D02KKPiPiFor'+moduleName
                            ,Algorithm=_d02kkpipi
                            ,RequiredSelections=[_pions,_kaons])

  _selD02KKPiPiConj = Selection('D02KKPiPiConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KKPiPi])

  _d02fourpi = copy(_d02k3pi)
  _d02fourpi.DecayDescriptor="D0 -> pi+ pi- pi+ pi-"
  _d02fourpi.DaughtersCuts={'pi+' : _pionCuts}

  _selD02FourPi = Selection('D02FourPiFor'+moduleName
                           ,Algorithm=_d02fourpi
                           ,RequiredSelections=[_pions])

  _selD02FourPiConj = Selection('D02FourPiConjFor'+moduleName
                                ,Algorithm=_conjPID
                                ,RequiredSelections=[_selD02FourPi])

  _selectionsToRun = []

  if runK3pi: 
    _selectionsToRun += [_selD02K3Pi, _selD02K3PiConj]
  if run4pi: 
    _selectionsToRun += [_selD02FourPi, _selD02FourPiConj]
  if run2K2pi: 
    _selectionsToRun += [_selD02KKPiPi, _selD02KKPiPiConj]
  if run3Kpi: 
    _selectionsToRun += [_selD02Pi3K, _selD02Pi3KConj]

  _d0Sel = MergedSelection('D02hhhhFor'+moduleName
                           ,RequiredSelections=_selectionsToRun
#                           ,RequiredSelections=[_selD02K3Pi
#                                                ,_selD02K3PiConj
#                                                ,_selD02KKPiPi
#                                                ,_selD02KKPiPiConj
#                                                ,_selD02FourPi
#                                                ,_selD02FourPiConj
#                                                ,_selD02Pi3K
#                                                ,_selD02Pi3KConj]
                           )

  return _d0Sel

def makePromptDstar(
  moduleName
  ,selection
  ,combDelmLower
  ,combDelmUpper
  ,delmUpper
  ,delmLower
  ,DOCA
  ,vertexChi2DOF
  ,pt
  ,slowPionPt
  ,trackChi2DOF
  ):

   """Creates a D*->D0pi Selection object
  , with cuts for physics analysis.
  Uses StandardParticles 'Phys/StdAllNoPIDPions and Phys/StdAllNoPIDsUpPions'
  Arguments:
  -  name : name of Selection
  - selection : D0 Selection object
  - combDelmLower : lower edge of delta mass window for combinations (MeV/c^2)
  - combDelmUpper : upper edge of delta mass window for combinations (MeV/c^2)
  - delmLower : lower edge of delta mass window (MeV/c^2)
  - delmUpper : upper edge of delta mass window (MeV/c^2)
  - DOCA : distance of closest approach (mm)
  - vertexChi2DOF : maximum vertex chi2 / d.o.f
  - pt : minimum transverse momentum of D* (GeV/c)
  - slowPionPt : minimum transverse momentum of bachelor pion (GeV/c)
 - trackChi2DOF : track chi2 / d.o.f. of the bachelor pion (unitless)
Note that the delta mass is defined here as the difference between the D* and D0 reconstructed masses subtracted from the PDG value for the D* - D0 mass difference.
  """

   _prefitCuts = "(AM-AMAXCHILD(M,'D0'==ABSID)-145.4*MeV>%(combDelmLower)s) & " \
                 "(AM-AMAXCHILD(M,'D0'==ABSID)-145.4*MeV<%(combDelmUpper)s) & " \
                 "(APT>%(pt)s) & " \
                 "(AMAXDOCA('')<%(DOCA)s)" %locals()

   _motherCuts = "(M-MAXTREE(M,'D0'==ABSID)-145.4*MeV>%(delmLower)s) & " \
                 "(M-MAXTREE(M,'D0'==ABSID)-145.4*MeV<%(delmUpper)s) & " \
                 "(VFASPF(VCHI2/VDOF)<%(vertexChi2DOF)s)" %locals()
   _slowPionCuts = "(TRCHI2DOF<%(trackChi2DOF)s) & (PT>%(slowPionPt)s)" %locals()

   from StandardParticles import StdAllNoPIDsPions

   _dstar = CombineParticles (
     DecayDescriptor="[D*(2010)+ -> D0 pi+]cc"
     ,CombinationCut = _prefitCuts
     ,MotherCut = _motherCuts
     ,DaughtersCuts = {"D0":"ALL", "pi+" : _slowPionCuts}
     )

   _selDstar = Selection(
     'PromptDstarFor'+moduleName
     ,Algorithm=_dstar
     ,RequiredSelections=[StdAllNoPIDsPions, selection]
     )
   return _selDstar
