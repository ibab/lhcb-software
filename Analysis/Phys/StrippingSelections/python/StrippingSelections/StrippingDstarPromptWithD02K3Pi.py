__author__='Philip Hunt'
__date__='10/5/2010'
__version__='$Revision: 1.1 $'

'''
D*(2010)+ -> D0(K3Pi) pi+ stripping selection
'''
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from StrippingConf.StrippingLine import bindMembers, StrippingLine, StrippingMember
from Configurables import CombineParticles, ConjugateNeutralPID

class StrippingDstarPromptWithD02K3PiConf(LHCbConfigurableUser):
  __slots__ = {
    # prefit cuts 
    "PreFitDstarMassWindow" : 200 # MeV
    ,"PreFitDstarPt" : 2.5 # GeV
    ,"PreFitD0MassWindow" : 120 # MeV
    ,"PreFitD0Pt" : 2.2 # GeV

    # mass windows
    , "DeltaMass_Lower" : -15 # MeV
    , "DeltaMass_Upper" : 15 # MeV
    , "D0MassWindow" : 100 # MeV

    # vertex + track chi2 / d.o.f.
    , "DstarVertexChi2DOF" : 10 # no units
    , "D0VertexChi2DOF" : 5 # no units
    , "TrackChi2DOF" : 10 # no units
    
    # momentum
    , "DstarPt" : 3.2# GeV
    , "SlowPionPt" : 0.19 # GeV
    , "D0Pt" : 3.1 # GeV
    , "PionPt" : 0.175 # GeV
    , "PionP" : 1 # GeV
    , "KaonPt": 0.55 # GeV
    , "KaonP": 6 # GeV
   # , "D0DaughterPiMaxPt" : 0.75 # GeV

    , "RobustDstarPt" : 3.055 # GeV
    , "RobustSlowPionPt" : 0.130 #GeV
    , "RobustD0Pt" : 2.445 # GeV
    , "RobustPionPt" : 0.175 # GeV
    , "RobustPionP" : 1 # GeV
    , "RobustKaonPt" : 0.735 # GeV
    , "RobustKaonP" : 6 # GeV
    
    # "pointing" cuts
    , "SlowPionIP" : 0.5 # mm
    , "D0FDChi2PV" : 23 # no units
    , "D0DIRA" : 0.99987 # cosine of angle (0-1)
    , "D0MaxIPChi2" : 10 # cut on maximum IP chi2 of the D0 daughters

    , "D0MaxDOCA" : 0.214 # mm
    , "DstarDOCA" : 0.175 # mm
    , "RobustSlowPionIP" : 0.5 # mm
    , "D0FDPV" : 2.135 # mm
    , "RobustD0DIRA" : 0.99989 # cosine of angle (0-1)
    
    # cut on number of tracks in event
    , "CutOnTracksInEvent" : True
    , "MaxTracksInEvent" : 240

    , "KaonUseLoosePID" : True
    , "PionUseLoosePID" : False

    , "RobustKaonUseLoosePID" : False
    , "RobustPionUseLoosePID" : False
    
    # prescales
    , "PrescaleFavoured" : 1.
    , "PrescaleDCS" : 1.
    , "PrescaleRobustFavoured" : 1.
    , "PrescaleRobustDCS" : 1.
    }
    

  def _kaonLocation(self):
    if (self.getProp("KaonUseLoosePID")):
      return "StdLooseKaons"
    else:
      return "StdNoPIDsKaons"

  def _pionLocation(self):
    if (self.getProp("PionUseLoosePID")):
      return "StdLoosePions"
    else:
      return "StdNoPIDsPions"

  def _robustKaonLocation(self):
    if (self.getProp("RobustKaonUseLoosePID")):
      return "StdLooseKaons"
    else:
      return "StdNoPIDsKaons"

  def _robustPionLocation(self):
    if (self.getProp("RobustPionUseLoosePID")):
      return "StdLoosePions"
    else:
      return "StdNoPIDsPions"
    
  def _kaonCuts(self):
    trackChi2Cut = "(TRCHI2DOF<%(TrackChi2DOF)s)" %self.getProps()
    ptCut = "(PT>%(KaonPt)s*GeV)" %self.getProps()
    momentumCut = "(P>%(KaonP)s*GeV)" % self.getProps()
    return  ' & '.join( (trackChi2Cut, ptCut, momentumCut) )

  def _pionCuts(self):
    trackChi2Cut = "(TRCHI2DOF<%(TrackChi2DOF)s)" %self.getProps()
    ptCut = "(PT>%(PionPt)s*GeV)" %self.getProps()
    momentumCut = "(P>%(PionP)s*GeV)" % self.getProps()
    return ' & '.join( (trackChi2Cut, ptCut, momentumCut) )

  def _slowPionCuts(self):
    trackChi2Cut = "(TRCHI2DOF<%(TrackChi2DOF)s)" %self.getProps()
    ptCut = "(PT>%(SlowPionPt)s*GeV)" %self.getProps()
    ipCut = "(BPVIP()<%(SlowPionIP)s*mm)" %self.getProps()
    return ' & '.join( (trackChi2Cut, ptCut, ipCut) )

  def _d0PreFitCuts(self):
    d0MassWindow = "(ADAMASS('D0')<%(PreFitD0MassWindow)s*MeV)" %self.getProps()
    d0Pt = "(APT>%(PreFitD0Pt)s*GeV)" %self.getProps()
    return ' & '.join( (d0MassWindow, d0Pt) )
  
  def _d0PostFitCuts(self):
    d0MassWindowCut = "(ADMASS('D0')<%(D0MassWindow)s*MeV)" %self.getProps()
    d0PtCut  = "(PT>%(D0Pt)s*GeV)" %self.getProps()
    d0VertexChi2Cut = "(VFASPF(VCHI2PDOF)<%(D0VertexChi2DOF)s)" %self.getProps()
    d0DIRACut = "(BPVDIRA>%(D0DIRA)s)" %self.getProps()
    D0FDChi2Cut = "(BPVVDCHI2>%(D0FDChi2PV)s)" %self.getProps()
    D0DaughterMaxIPChi2Cut = "(MAXTREE(TRUE, BPVIPCHI2())>%(D0MaxIPChi2)s)" %self.getProps()
   # D0DaughterMaxPtCut = "(MAXTREE('pi+'==ABSID,PT)>%(D0DaughterPiMaxPt)s)" %self.getProps() 
    return ' & '.join( (d0MassWindowCut, d0PtCut, #D0DaughterMaxPtCut,
                        D0DaughterMaxIPChi2Cut, d0VertexChi2Cut, D0FDChi2Cut,
                        d0DIRACut ) )

  def _dstarPreFitCuts(self):
      ptCut = "(APT>%(PreFitDstarPt)s*GeV)" %self.getProps()
      massWindowCut = "(ADAMASS('D*(2010)+')<%(PreFitDstarMassWindow)s*MeV)" %self.getProps()
      return ' & '.join( (massWindowCut, ptCut) )

  def _dstarPostFitCuts(self):
    deltaMass_Upper="(M-MAXTREE('D0'==ABSID,M)-145.5*MeV<%(DeltaMass_Upper)s*MeV)" %self.getProps()
    deltaMass_Lower="(M-MAXTREE('D0'==ABSID,M)-145.5*MeV>%(DeltaMass_Lower)s*MeV)" %self.getProps()
    ptCut = "(PT>%(DstarPt)s*GeV)" %self.getProps()
    vertexChi2Cut = "(VFASPF(VCHI2PDOF)<%(DstarVertexChi2DOF)s)" %self.getProps()
    return ' & '.join((deltaMass_Upper, deltaMass_Lower, vertexChi2Cut, ptCut))

  def _robustKaonCuts(self):
    ptCut = "(PT>%(RobustKaonPt)s*GeV)" %self.getProps()
    momentumCut = "(P>%(RobustKaonP)s*GeV)" % self.getProps()
    return  ' & '.join( (ptCut, momentumCut) )

  def _robustPionCuts(self):
    ptCut = "(PT>%(RobustPionPt)s*GeV)" %self.getProps()
    momentumCut = "(P>%(RobustPionP)s*GeV)" % self.getProps()
    return  ' & '.join( (ptCut, momentumCut) )

  def _robustSlowPionCuts(self):
    ptCut = "(PT>%(RobustSlowPionPt)s*GeV)" %self.getProps()
    ipCut = "(BPVIP()<%(RobustSlowPionIP)s*mm)" %self.getProps()
    return ' & '.join( (ptCut, ipCut) )
  
  def _robustD0PostFitCuts(self):
    d0MassWindowCut = "(ADMASS('D0')<%(D0MassWindow)s*MeV)" %self.getProps()
    d0PtCut  = "(PT>%(RobustD0Pt)s*GeV)" %self.getProps()
    d0DIRACut = "(BPVDIRA>%(RobustD0DIRA)s)" %self.getProps()
    d0MAXDOCAcut = "(DOCAMAX<%(D0MaxDOCA)s)" %self.getProps()
    d0FDcut = "(BPVVD>%(D0FDPV)s)" %self.getProps()
    #    D0DaughterMaxPtCut = "(MAXTREE('pi+'==ABSID,PT)>%(D0DaughterPiMaxPt)s)" %self.getProps() 
    return ' & '.join( (d0MassWindowCut, d0PtCut,
                        d0DIRACut, d0MAXDOCAcut, d0FDcut ) )

  def _robustDstarPostFitCuts(self):
    deltaMass_Upper="(M-MAXTREE('D0'==ABSID,M)-145.5*MeV<%(DeltaMass_Upper)s*MeV)" %self.getProps()
    deltaMass_Lower="(M-MAXTREE('D0'==ABSID,M)-145.5*MeV>%(DeltaMass_Lower)s*MeV)" %self.getProps()
    ptCut = "(PT>%(RobustDstarPt)s*GeV)" %self.getProps()
    dstarDOCAcut = "(DOCAMAX<%(DstarDOCA)s)" %self.getProps()
    return ' & '.join( (deltaMass_Upper, deltaMass_Lower, ptCut, dstarDOCAcut ) )

  def _makeD0Favoured(self, robustLine=False):
    combD0=None
    if not robustLine:      
      combD0=CombineParticles("D02K3PiFromPromptDstar")
      combD0.InputLocations = [self._kaonLocation(), self._pionLocation()]
      combD0.DecayDescriptor = '[D0 -> K- pi+ pi- pi+]cc'
      combD0.CombinationCut = self._d0PreFitCuts()
      combD0.MotherCut = self._d0PostFitCuts()
      combD0.DaughtersCuts = { 'K+': self._kaonCuts(), 'pi+': self._pionCuts() }
    else:
      combD0=CombineParticles("RobustD02K3PiFromPromptDstar")
      combD0.InputLocations = [self._robustKaonLocation(), self._robustPionLocation()]
      combD0.DecayDescriptor = '[D0 -> K- pi+ pi- pi+]cc'
      combD0.CombinationCut = self._d0PreFitCuts()
      combD0.MotherCut = self._robustD0PostFitCuts()
      combD0.DaughtersCuts = { 'K+': self._robustKaonCuts(), 'pi+': self._robustPionCuts() }
    return combD0

  def _makeD0Suppressed(self, robustLine=False):
    combD0=self._makeD0Favoured(robustLine)
    linePrefix=""
    if robustLine:
      linePrefix="Robust"
    conjD0=ConjugateNeutralPID(linePrefix+"D02K3PiDCSFromPromptDstar")
    conjD0.InputLocations = [combD0.name()]
    return conjD0

  def _makeDstar(self, favouredLine=True, robustLine=False):
    combD0=None
    combDstar=None
    
    combName="PromptDstarWithD02K3Pi"
    if favouredLine:
      combD0=self._makeD0Favoured(robustLine)
    else:
      combD0=self._makeD0Suppressed(robustLine)
      combName+="DCS"

    if not robustLine:
      combDstar = CombineParticles(combName)
      combDstar.InputLocations = [ "StdNoPIDsPions", combD0.name()]
      combDstar.DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
      combDstar.CombinationCut = self._dstarPreFitCuts()
      combDstar.MotherCut = self._dstarPostFitCuts()
      combDstar.DaughtersCuts = { 'pi+' : self._slowPionCuts() }

    else:
      combDstar = CombineParticles("Robust"+combName)
      combDstar.InputLocations = [ "StdNoPIDsPions", combD0.name()]
      combDstar.DecayDescriptor = '[D*(2010)+ -> D0 pi+]cc'
      combDstar.CombinationCut = self._dstarPreFitCuts()
      combDstar.MotherCut = self._robustDstarPostFitCuts()
      combDstar.DaughtersCuts = { 'pi+' : self._robustSlowPionCuts() }

    return combDstar
  
  def _makeDstarSequence(self, favouredLine=True, robustLine=False):
    seqPrefix=""
    if robustLine:
      seqPrefix="Robust"
    seq = None
    if favouredLine:
      seq = bindMembers(
        "Seq"+seqPrefix+"PromptDstarWithD02K3Pi"
        , [self._makeD0Favoured(robustLine)
           ,self._makeDstar(favouredLine, robustLine)]
        )
    else:
      seq = bindMembers(
        "Seq"+seqPrefix+"PromptDstarWithD02K3PiDCS"
        , [self._makeD0Favoured(robustLine)
           ,self._makeD0Suppressed(robustLine)
           ,self._makeDstar(favouredLine, robustLine)]
        )
    return seq

  def _filterNTracksInEvent(self):
    '''
    Returns a VoidFilter instance to filter on the maximum number of tracks in an event
    '''
    if not self.getProp("CutOnTracksInEvent"):
      return None
    from Configurables import LoKi__VoidFilter as VoidFilter
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    modules = CoreFactory('CoreFactory').Modules
    for i in ['LoKiTrigger.decorators']:
      if i not in modules : modules.append(i)

    filterTooManyIP = VoidFilter(
      'FilterNTracks'
      ,Code = "TrSOURCE('Rec/Track/Best') >> (TrSIZE < %(MaxTracksInEvent)s )" %self.getProps()
      )

    return filterTooManyIP

  def makeFavouredLine(self):
    algos = []
    if self.getProp("CutOnTracksInEvent"):
      algos.append(self._filterNTracksInEvent())
    algos.append(self._makeDstarSequence(True, False))
    
    return StrippingLine(
      "PromptDstarWithD02K3PiLine"
      , prescale = self.getProp("PrescaleFavoured")
      , algos = algos
      )

  def makeDCSLine(self):
    algos = []
    if self.getProp("CutOnTracksInEvent"):
      algos.append(self._filterNTracksInEvent())
    algos.append(self._makeDstarSequence(False, False))
    
    return StrippingLine(
      "PromptDstarWithD02K3PiDCSLine"
      , prescale = self.getProp("PrescaleDCS")
      , algos = algos
      )
  def makeRobustFavouredLine(self):
    algos = []
    if self.getProp("CutOnTracksInEvent"):
      algos.append(self._filterNTracksInEvent())
    algos.append(self._makeDstarSequence(True, True))
    
    return StrippingLine(
      "PromptDstarWithD02K3PiRobustLine"
      , prescale = self.getProp("PrescaleRobustFavoured")
      , algos = algos
      )

  def makeRobustDCSLine(self):
    algos = []
    if self.getProp("CutOnTracksInEvent"):
      algos.append(self._filterNTracksInEvent())
    algos.append(self._makeDstarSequence(False, True))
    
    return StrippingLine(
      "PromptDstarWithD02K3PiDCSRobustLine"
      , prescale = self.getProp("PrescaleRobustDCS")
      , algos = algos
      )
  
  def lines(self):
    return [ self.makeFavouredLine(), self.makeDCSLine()
             ,self.makeRobustFavouredLine(), self.makeRobustDCSLine()]

  def getProps(self):
    d=dict()
    for (k,v) in self.getDefaultProperties().iteritems() :
      d[k] = getattr(self, k) if hasattr(self,k) else v
    return d
