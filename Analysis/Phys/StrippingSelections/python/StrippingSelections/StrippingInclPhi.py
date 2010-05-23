# $Id: StrippingInclPhi.py,v 1.4 2010-05-23 12:32:25 schleich Exp $

__author__ = 'Andrew Powell, Sebastian Schleich'
__date__ = '2010/05/23'
__version__ = '$Revision: 1.4 $'

'''
InclPhi stripping selection
see: http://indico.cern.ch/conferenceDisplay.py?confId=82203
- changes since then:
  tag&probe for pt and RICH as well
  phi mass window 55 -> 30 MeV/cc
  MB* trigger
  prescaled selection for low PT
  prescaled LD line
'''

from Gaudi.Configuration   import *
from LHCbKernel.Configuration import *

from Configurables import LoKi__VoidFilter as VoidFilter
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)



class StrippingInclPhiConf(LHCbConfigurableUser):
    __slots__ = {
                  'KaonPT'              : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.5      # adimensional
                , 'LowPtPrescale'       : 0.16     # adimensional
                , 'LDPrescale'          : 0.005    # adimensional
                }

    _InclPhiHighPtLine = None # only to be used in InclPhiHighPtLine()
    _InclPhiLowPtLine = None # only to be used in InclPhiLowPtLine()
    # ====================================================================
    # mother and combination cut definition
    def _Phi2KK_MC( self, CutOnKPT, CutOnPhiPT ) :
      GOODKAON = "(0!=NINTREE( " +\
            "HASRICH"
      if CutOnKPT == True :
        GOODKAON = GOODKAON +  " & (PT>%(KaonPT)s*MeV)" % self.getProps()
      GOODKAON = GOODKAON +\
            " & (%(KaonDLL)s<PPINFO(LHCb.ProtoParticle.RichDLLk,-500,-1000))"% self.getProps() +\
            " ))" % self.getProps()
      if CutOnPhiPT == True :
        GOODKAON = GOODKAON +  " & (PT>2*%(KaonPT)s*MeV)" % self.getProps()
      return GOODKAON + " & (ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % self.getProps()
    def _LOWPTK( self ) :
      lowptk = "&(0==NINTREE( " +\
        "('K+'==ABSID)&(PT>%(KaonPT)s*MeV)" % self.getProps() +\
        " ))" % self.getProps()
      return lowptk
    def _LOWPTPHI( self ) :
      return "&(PT<2*%(KaonPT)s*MeV)" % self.getProps()
    def _Phi2KK_CC( self ) :
      coarsemasswindow = 30 + self.getProps()['PhiMassWindow']
      return "(ADAMASS('phi(1020)')<%s*MeV)" % coarsemasswindow

    # ====================================================================
    # phi stripping lines definition
    def InclPhiHighPtLine( self ) :
      if StrippingInclPhiConf._InclPhiHighPtLine != None :
        print "InclPhiLine already created."
      else :
        print "Creating InclPhiHighPtLine now!"
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        #Phi2KK_DC = ""
        Phi2KK_CC = self._Phi2KK_CC()
        Phi2KK_MC = self._Phi2KK_MC(True, False)
        ps =  self.getProps()['HighPtPrescale']

        print "CC and MC for InclPhiHighPt stripping line."
        print "Prescale: ", ps
        #print Phi2KK_DC
        print Phi2KK_CC
        print Phi2KK_MC
        from Configurables import DeterministicPrescaler as Scaler
        StrippingInclPhiHighPtScaler = Scaler ("StrippingInclPhiHighPtScaler", AcceptFraction=ps)

        Phi2KK = StrippingMember ( CombineParticles
                          , "Phi2KK"
                          , InputLocations = ["StdNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )
        StrippingInclPhiConf._InclPhiHighPtLine = StrippingLine('InclPhiHighPtLine'
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
             #, prescale = ps   Prescale realized by my own prescaler (shared between Phi and K)->Do not set here!
              , postscale = 1
              , algos = [StrippingInclPhiHighPtScaler, Phi2KK]
              )
      return StrippingInclPhiConf._InclPhiHighPtLine


    def InclPhiLowPtLine( self ) :
      if StrippingInclPhiConf._InclPhiLowPtLine != None :
        print "InclPhiLine already created."
      else :
        print "Creating InclPhiLowPtLine now!"
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        #Phi2KK_DC = ""
        Phi2KK_CC = self._Phi2KK_CC()
        Phi2KK_MC = self._Phi2KK_MC(False, False) + self._LOWPTK()
        ps =  self.getProps()['LowPtPrescale']

        print "CC and MC for InclPhiLowPt stripping line."
        print "Prescale: ", ps
        #print Phi2KK_DC
        print Phi2KK_CC
        print Phi2KK_MC
        from Configurables import DeterministicPrescaler as Scaler
        StrippingInclPhiLowPtScaler = Scaler ("StrippingInclPhiLowPtScaler", AcceptFraction=ps)

        Phi2KK = StrippingMember ( CombineParticles
                          , "Phi2KK"
                          , InputLocations = ["StdNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )
        StrippingInclPhiConf._InclPhiLowPtLine = StrippingLine('InclPhiLowPtLine'
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
             #, prescale = ps   Prescale realized by my own prescaler (shared between Phi and K)->Do not set here!
              , postscale = 1
              , algos = [StrippingInclPhiLowPtScaler, Phi2KK]
              )
      return StrippingInclPhiConf._InclPhiLowPtLine


    def InclPhiHighPtPhiLine( self ) :
      print "Creating InclPhiHighPtPhiLine line"
      Phi2KK_MC = self._Phi2KK_MC(False, True)
      print "MC for InclPhiHighPtPhiLine stripping line: "
      print Phi2KK_MC
      line_tmp = self.InclPhiHighPtLine()
      return line_tmp.clone('InclPhiHighPtPhiLine'
                     , CombineParticlesPhi2KK = {"MotherCut" : Phi2KK_MC}
                     )
    def InclPhiLowPtPhiLine( self ) :
      print "Creating InclPhiLowPtPhiLine line"
      Phi2KK_MC = self._Phi2KK_MC(False, False) + self._LOWPTPHI()
      print "MC for InclPhiLowPtPhiLine stripping line: "
      print Phi2KK_MC
      line_tmp = self.InclPhiLowPtLine()
      return line_tmp.clone('InclPhiLowPtPhiLine'
                     , CombineParticlesPhi2KK = {"MotherCut" : Phi2KK_MC}
                     )


    def InclPhiLDLine( self ) :
        '''
        Downstream kaons track selector
        '''
        from Configurables import NoPIDsParticleMaker
        #from CommonParticles.Utils import *
        pmalg =  NoPIDsParticleMaker ( 'DownNoPIDsKaons' , Particle = 'kaon' )
        pmalg.Particle = 'Kaon'
        from Configurables import TrackSelector
        pmalg.addTool(TrackSelector)
        pmalg.TrackSelector.TrackTypes = ['Downstream','Long']
        DownNoPIDsKaons = pmalg

        '''
        Downstream K stripping
        Differences: DownNoPIDsKaons is added to algorithms and input location adapted. TODO: Possible to edit algorithm list via cloning??
        '''
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        #Phi2KK_DC = ""
        Phi2KK_CC = self._Phi2KK_CC()
        Phi2KK_MC = self._Phi2KK_MC(False, False)
        ps =  self.getProps()['LDPrescale']

        print "CC and MC for InclPhi LD stripping line."
        print "Prescale: ", ps
        print Phi2KK_CC
        print Phi2KK_MC

        Phi2KK = StrippingMember ( CombineParticles
                          , "Phi2KK"
                          , InputLocations = ["DownNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )

        line = StrippingLine('InclPhiLDLine'
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
              , prescale = ps
              , postscale = 1
              , algos = [DownNoPIDsKaons, Phi2KK]
              )

        return line



    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

