'''
Module for construction of D0->hhll tagged (from prompt D*) selections, where D->hhll comprises:
  - D0->Kpi  (mumu,ee,e+mu-,mu+e-) Cabibbo Favoured
  - D0->Kpi  (mumu,ee,e+mu-,mu+e-) Cabibbo suppressed
  - D->KK    (mumu,ee,e+mu-,mu+e-)
  - D0->PiPi (mumu,ee,e+mu-,mu+e-)
Provides functions to build tagged D->hhhh selections for physics analysis.
Provides class DstarPromptWithD02HHLLConf, which constructs the selections and stripping lines given a configuration dictionary.
Exported symbols (use python help):
  - DstarPromptWithD02HHLLConf
  - makeD02hhll
  - makePromptDstar
'''

__author__ = ['Andrea Contu','Benoit VIAUD']
__date__ = '23/08/2012'
__version__ = '$Revision: 2.0 $'

__all__ = ('DstarPromptWithD02HHLLConf'
           , 'makeD02hhll'
           , 'makePromptDstar'
           , 'default_config')

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles, ConjugateNeutralPID
from PhysSelPython.Wrappers import Selection, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import checkConfig, LineBuilder
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from copy import copy

moduleName='DstarPromptWithD02HHLL'

# What follows documents the cuts to
# be implemented by the Stripping team in
# StrippingSettings (valid for Stripping 20,
# 24/08/2011).

default_config= {
  'NAME' : 'DstarPromptWithD02HHLL',
  'BUILDERTYPE' : 'DstarPromptWithD02HHLLConf',
  'WGs' : [ 'Charm' ],
  'STREAMS' : [ 'Charm' ],
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
  ,'D0DauIPChi2'      : 3.0
  ,'D0FDChi2'         : 30
  ,'D0IPChi2'         : 36
  ,'D0DIRA'           : 0.9998
  ,'TrackChi2DOF'     : 3
  ,'ApplyKaonPIDK'    : False
  ,'KaonPIDK'         : -100.
  ,'ApplyPionPIDK'    : False
  ,'PionPIDK'         : 100
  ,'ApplyElePIDe'    : False
  ,'ElePIDe'         : -5
  ,'CheckPV'          : True # reject events with no PVs
  ,'ApplyGECs'        : False # set to False to apply no GECs
  ,'MaxLongTracks'    : 250
  ,'MaxVeloTracks'    : None
  ,'MaxSpdDigits'     : None
  ,'MaxITClusters'    : None
  ,'ApplyGhostProbCut': False
  ,'GhostProbCut'     : 0.5
  ,'ConeAngles'     : {"11":1.1,"13":1.3,"15":1.5,"17":1.7,"19":1.9}
  ,'ConeVariables' : ['CONEANGLE', 'CONEMULT', 'CONEPTASYM']
  ,'Prescale'         : 1
  ,'Postscale'        : 1
  }
}


class DstarPromptWithD02HHLLConf(LineBuilder):
  """
  Constructor of D*->D0(hhll)pi stripping selection / stripping lines for
  physics analysis.
  Selections and StrippingLines created from a configuration dictionary.
  Usage:
  >>> config = {...}
  >>> myConf = StrippingPromptDstarWithD02HHLLConf('PromptDstarWithD02HHLL',config)
  >>> myLines = myConf.lines()
  >>> for line in myLines :
  >>>  print line.name(), line.outputLocation()
  The lines can be used directly to build a StrippingStream object.

  Exports as instance data members:
    line_tagged_d02hhll : StrippingLine made out of selDstar2Dpi

  Exports as class data member:
    DstarPromptWithD02HHLLConf.__configuration_keys__ : List of required configuration parameters.
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


    ,'ApplyElePIDe'
    ,'ElePIDe'


    ,'CheckPV'

    ,'ApplyGECs'
    ,'MaxLongTracks'
    ,'MaxVeloTracks'
    ,'MaxSpdDigits'
    ,'MaxITClusters'

    ,'ApplyGhostProbCut'
    ,'GhostProbCut'    

    ,'ConeAngles'     
    ,'ConeVariables' 

    ,'Prescale'
    ,'Postscale'
    )

  def __init__(self, moduleName, config):
    LineBuilder.__init__(self, moduleName, config)

    selD02hhll = makeD02hhll(
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
      ,applyElePIDe = config['ApplyElePIDe']
      ,elePIDe = config['ElePIDe']
      ,applyGhostProbCut = config['ApplyGhostProbCut']
      ,ghostProbCut = config['GhostProbCut']
      )

    selPromptDstar = makePromptDstar(
      moduleName = moduleName
      ,selection = selD02hhll
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
    coneinfo=[]
    for conekey, coneitem in (config['ConeAngles']).iteritems():
            coneinfo.append({ 
                                        'Type' : 'RelInfoConeVariables', 'ConeAngle' : coneitem, 'Variables' : config['ConeVariables'], 
                                        'Location' : 'P2CVDst'+conekey,
                                        'DaughterLocations' : {
                                          #'[^D*(2010)+ -> Charm pi+]CC' : 'P2CVDst'+conekey,
                                          '[D*(2010)+ -> ^Charm pi+]CC' : 'P2CVD0'+conekey,
                                          '[D*(2010)+ -> Charm ^pi+]CC' : 'P2CVpis'+conekey,
                                          
                                        } 
                                      })

    self.line_tagged_d02hhll = StrippingLine(
      moduleName+"Line"
      ,prescale=config['Prescale']
      ,postscale=config['Postscale']
      ,selection=selPromptDstar
      ,checkPV=config['CheckPV']
      ,FILTER=_GECfilter
      ,RelatedInfoTools=coneinfo
      )
    self.registerLine(self.line_tagged_d02hhll)

def makeD02hhll(
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
  ,applyElePIDe
  ,elePIDe
  ,applyGhostProbCut
  ,ghostProbCut  
  ):
  """Creates a D0->hhll Selection object, merging D0->Kpi(mumu,ee,e+mu-,mu+e-) CF
  , D0->Kpi(mumu,ee,e+mu-,mu+e-) DCS, D0->KK(mumu,ee,e+mu-,mu+e-) and D0->pipi(mumu,ee,e+mu-,mu+e-), with cuts for physics analysis.
  Uses StandardParticle objects 'StdAllLooseKaons', 'StdAllLoosePions'
  and 'StdAllLooseMuons'

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
                "(AHASCHILD(((ABSID=='K+') | (ABSID=='pi+') | (ABSID=='mu+') ) & " \
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
  _muonCuts = copy(_kaonCuts)
  _eleCuts = copy(_kaonCuts)


  PiMuPair_Mu_Cut = "(INTREE( (ID=='mu+') & (PT> %(dauPt)s *MeV) & (TRCHI2DOF < %(trackChi2DOF)s) & ((MIPCHI2DV(PRIMARY)) > %(dauIPChi2)s) & (TRGHOSTPROB<%(ghostProbCut)s ) ))" % locals()
  PiMuPair_Pi_Cut = "(INTREE( (ID=='pi-') & (PT> %(dauPt)s *MeV) & (TRCHI2DOF < %(trackChi2DOF)s) & ((MIPCHI2DV(PRIMARY)) > %(dauIPChi2)s) & (TRGHOSTPROB<%(ghostProbCut)s ) ))" % locals()
  _PiMuCuts = PiMuPair_Mu_Cut + " & " + PiMuPair_Pi_Cut 

  PiMuPair_Pi_Cut_PID = "(INTREE( (ID=='pi-') & (PT> %(dauPt)s *MeV) & (TRCHI2DOF < %(trackChi2DOF)s) & ((MIPCHI2DV(PRIMARY)) > %(dauIPChi2)s) & (PIDK<%(pionPIDK)s) & (TRGHOSTPROB<%(ghostProbCut)s ) ))" % locals()


  if applyKaonPIDK:
    _kaonCutsOLD = copy(_kaonCuts)
    _kaonCuts="(PIDK>%(kaonPIDK)s) & (HASRICH) & " %locals()
    _kaonCuts+=_kaonCutsOLD
  if applyPionPIDK:
    _pionCutsOLD = copy(_pionCuts)
    _pionCuts="(PIDK<%(pionPIDK)s) & (HASRICH) & " %locals()
    _pionCuts+=_pionCutsOLD
    _PiMuCuts = PiMuPair_Mu_Cut + " & " + PiMuPair_Pi_Cut_PID 

  if applyElePIDe:
    _eleCutsOLD = copy(_eleCuts)
    _eleCuts="(PIDe>%(elePIDe)s) & " %locals()
    _eleCuts+=_eleCutsOLD

  if applyGhostProbCut:
    _ghostCut = "( TRGHOSTPROB < %(ghostProbCut)s )" %locals()
    _kaonCutsOLD = copy(_kaonCuts)
    _pionCutsOLD = copy(_pionCuts)
    _eleCutsOLD = copy(_eleCuts)
    _kaonCuts = _kaonCutsOLD + " & " + _ghostCut
    _pionCuts = _pionCutsOLD + " & " + _ghostCut
    _eleCuts = _eleCutsOLD + " & " + _ghostCut



    


    

  from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons
  from StandardParticles import StdAllLoosePions, StdAllLooseKaons, StdAllLooseMuons 
  from StandardParticles import StdAllLooseElectrons 
  from StandardParticles import StdAllLooseMuPion


  _kaons = StdAllLooseKaons
  _pions = StdAllLoosePions
  _muons = StdAllLooseMuons
  _eles   = StdAllLooseElectrons
  _PiMus = StdAllLooseMuPion

  if applyElePIDe:
    _eles = StdAllLooseElectrons

### Select kpi modes

# MuMu

  _d02kpi = CombineParticles (DecayDescriptor = "[D0 -> K- pi+ mu+ mu-]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"pi+" : _pionCuts
                                                  ,"mu+" : _muonCuts}
                               )
  _selD02KPi = Selection('D02KPiFor'+moduleName
                          ,Algorithm=_d02kpi
                          ,RequiredSelections=[_muons,_kaons,_pions])

  _conjPID = ConjugateNeutralPID()

  _selD02KPiConj = Selection('D02KPiConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KPi])


# MuMu SS

  _d02kpiSS = CombineParticles (DecayDescriptor = "[D0 -> K- mu+ rho(770)0]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"mu+" : _muonCuts
                                                  ,"rho(770)0" : _PiMuCuts}
                               )
  _selD02KPiSS = Selection('D02KPiSSFor'+moduleName
                          ,Algorithm=_d02kpiSS
                           ,RequiredSelections=[_muons,_kaons,_PiMus])


  _selD02KPiSSConj = Selection('D02KPiSSConjFor'+moduleName
                             ,Algorithm=_conjPID
                               ,RequiredSelections=[_selD02KPiSS])



# EE

  _d02kpi_EE = CombineParticles (DecayDescriptor = "[D0 -> K- pi+ e+ e-]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"pi+" : _pionCuts
                                                  ,"e+" : _eleCuts}
                               )
  _selD02KPi_EE = Selection('D02KPi_EE_For'+moduleName
                          ,Algorithm=_d02kpi_EE
                          ,RequiredSelections=[_eles,_kaons,_pions])

  _selD02KPi_EE_Conj = Selection('D02KPi_EE_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KPi_EE])

# EMu

  _d02kpi_EMu = CombineParticles (DecayDescriptor = "[D0 -> K- pi+ e+ mu-]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"pi+" : _pionCuts
                                                  ,"mu+" : _muonCuts
                                                  ,"e+" : _eleCuts}
                               )

  _selD02KPi_EMu = Selection('D02KPi_EMu_For'+moduleName
                          ,Algorithm=_d02kpi_EMu
                          ,RequiredSelections=[_eles,_muons,_kaons,_pions])

  _selD02KPi_EMu_Conj = Selection('D02KPi_EMu_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KPi_EMu])


# MuE

  _d02kpi_MuE = CombineParticles (DecayDescriptor = "[D0 -> K- pi+ mu+ e-]cc"
                               ,CombinationCut = _prefitCuts
                               ,MotherCut = _motherCuts
                               ,DaughtersCuts = { "K+" : _kaonCuts
                                                  ,"pi+" : _pionCuts
                                                  ,"mu+" : _muonCuts
                                                  ,"e+" : _eleCuts}
                               )

  _selD02KPi_MuE = Selection('D02KPi_MuE_For'+moduleName
                          ,Algorithm=_d02kpi_MuE
                          ,RequiredSelections=[_eles,_muons,_kaons,_pions])

  _selD02KPi_MuE_Conj = Selection('D02KPi_MuE_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KPi_MuE])



### Select KK modes

# MuMu

  _d02kk = copy(_d02kpi)
  _d02kk.DecayDescriptor="D0 -> K+ K- mu+ mu-"

  _selD02KK = Selection('D02KKFor'+moduleName
                            ,Algorithm=_d02kk
                            ,RequiredSelections=[_muons,_kaons])

  _selD02KKConj = Selection('D02KKConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KK])


# EE

  _d02kk_EE = copy(_d02kpi_EE)
  _d02kk_EE.DecayDescriptor="D0 -> K+ K- e+ e-"

  _selD02KK_EE = Selection('D02KK_EE_For'+moduleName
                            ,Algorithm=_d02kk_EE
                            ,RequiredSelections=[_eles,_kaons])

  _selD02KK_EE_Conj = Selection('D02KK_EE_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KK_EE])

# EMu

  _d02kk_EMu = copy(_d02kpi_EMu)
  _d02kk_EMu.DecayDescriptor="D0 -> K+ K- e+ mu-"

  _selD02KK_EMu = Selection('D02KK_EMu_For'+moduleName
                            ,Algorithm=_d02kk_EMu
                            ,RequiredSelections=[_muons,_eles,_kaons])

  _selD02KK_EMu_Conj = Selection('D02KK_EMu_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KK_EMu])


# MuE


  _d02kk_MuE = copy(_d02kpi_MuE)
  _d02kk_MuE.DecayDescriptor="D0 -> K+ K- mu+ e-"

  _selD02KK_MuE = Selection('D02KK_MuE_For'+moduleName
                            ,Algorithm=_d02kk_MuE
                            ,RequiredSelections=[_muons,_eles,_kaons])

  _selD02KK_MuE_Conj = Selection('D02KK_MuE_ConjFor'+moduleName
                             ,Algorithm=_conjPID
                             ,RequiredSelections=[_selD02KK_MuE])




### Select PiPi modes

# MuMu

  _d02pipi = copy(_d02kpi)
  _d02pipi.DecayDescriptor="D0 -> pi+ pi- mu+ mu-"
  _d02pipi.DaughtersCuts={ "K+" : _kaonCuts
                           ,"pi+" : _pionCuts
                           ,"mu+" : _muonCuts}

  _selD02PiPi = Selection('D02PiPiFor'+moduleName
                           ,Algorithm=_d02pipi
                           ,RequiredSelections=[_muons,_pions])

  _selD02PiPiConj = Selection('D02PiPiConjFor'+moduleName
                                ,Algorithm=_conjPID
                                ,RequiredSelections=[_selD02PiPi])


# EE


  _d02pipi_EE = copy(_d02kpi_EE)
  _d02pipi_EE.DecayDescriptor="D0 -> pi+ pi- e+ e-"
  _d02pipi_EE.DaughtersCuts={ "K+" : _kaonCuts
                           ,"pi+" : _pionCuts
                           ,"e+" : _eleCuts}

  _selD02PiPi_EE = Selection('D02PiPi_EE_For'+moduleName
                           ,Algorithm=_d02pipi_EE
                           ,RequiredSelections=[_eles,_pions])

  _selD02PiPi_EE_Conj = Selection('D02PiPi_EE_ConjFor'+moduleName
                                ,Algorithm=_conjPID
                                ,RequiredSelections=[_selD02PiPi_EE])


# EMu



  _d02pipi_EMu = copy(_d02kpi_EMu)
  _d02pipi_EMu.DecayDescriptor="D0 -> pi+ pi- e+ mu-"
  _d02pipi_EMu.DaughtersCuts={ "K+" : _kaonCuts
                           ,"pi+" : _pionCuts
                           ,"mu+" : _muonCuts
                           ,"e+" : _eleCuts}

  _selD02PiPi_EMu = Selection('D02PiPi_EMu_For'+moduleName
                           ,Algorithm=_d02pipi_EMu
                           ,RequiredSelections=[_eles,_muons,_pions])

  _selD02PiPi_EMu_Conj = Selection('D02PiPi_EMu_ConjFor'+moduleName
                                ,Algorithm=_conjPID
                                ,RequiredSelections=[_selD02PiPi_EMu])




# MuE


  _d02pipi_MuE = copy(_d02kpi_MuE)
  _d02pipi_MuE.DecayDescriptor="D0 -> pi+ pi- mu+ e-"
  _d02pipi_MuE.DaughtersCuts={ "K+" : _kaonCuts
                           ,"pi+" : _pionCuts
                           ,"mu+" : _muonCuts
                           ,"e+" : _eleCuts}

  _selD02PiPi_MuE = Selection('D02PiPi_MuE_For'+moduleName
                           ,Algorithm=_d02pipi_MuE
                           ,RequiredSelections=[_eles,_muons,_pions])

  _selD02PiPi_MuE_Conj = Selection('D02PiPi_MuE_ConjFor'+moduleName
                                ,Algorithm=_conjPID
                                ,RequiredSelections=[_selD02PiPi_MuE])




### Wrapping everything up


  _d0Sel = MergedSelection('D02hhllFor'+moduleName
                           ,RequiredSelections=[_selD02KPi
                                                ,_selD02KPiConj
                                                , _selD02KPiSS
                                                , _selD02KPiSSConj
                                                ,_selD02KPi_EE
                                                ,_selD02KPi_EE_Conj
                                                ,_selD02KPi_EMu
                                                ,_selD02KPi_EMu_Conj
                                                ,_selD02KPi_MuE
                                                ,_selD02KPi_MuE_Conj
                                                ,_selD02KK
                                                ,_selD02KKConj
                                                ,_selD02KK_EE
                                                ,_selD02KK_EE_Conj
                                                ,_selD02KK_EMu
                                                ,_selD02KK_EMu_Conj
                                                ,_selD02KK_MuE
                                                ,_selD02KK_MuE_Conj
                                                ,_selD02PiPi
                                                ,_selD02PiPiConj
                                                ,_selD02PiPi_EE
                                                ,_selD02PiPi_EE_Conj
                                                ,_selD02PiPi_EMu
                                                ,_selD02PiPi_EMu_Conj
                                                ,_selD02PiPi_MuE
                                                ,_selD02PiPi_MuE_Conj]


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
