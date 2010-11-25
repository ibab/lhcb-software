# $Id: StrippingInclPhi.py, 2010-10-25  01:15:00 schleich Exp $

'''
Inclusive phi -> K+ K- stripping selection(s) for phi cross section measurement and RICH performance studies.
The phi candidates are unbiased in the sense that a RICH cut is only required on _one_ arbitrary kaon.

Provides class InclPhiConf with stripping line objects.

Exports the following StrippingLines as instance data members:
- InclPhiHighPtLine      : at least one Kaon-PT > 500 MeV/c
- InclPhiLowPtLine       : both Kaon-PT < 500 MeV/c
- InclPhiHighPtPhiLine   : phi-PT > 1000-20 MeV/c
- InclPhiLowPtPhiLine    : phi-PT < 1000+20 MeV/c
- InclPhiLDLine          : whole pt range, combining also Downstream with long tracks
- lines                  : List of all lines
The phase space is split up ("HighPT" and "LowPT") because of high combinatorics at low pt. For the purpose of RICH calibration it is advantageous do do this cut on K-pt whereas for the inclusive phi studies, the cut is made on Phi-pt (the "...PhiLine"s)
When cutting on phi-pt, an overlap is provided to allow later CM frame boosting.
The Phi- and K- Lines have of course a large overlap, which would vanish in the case of prescaling. To save bandwith, the lines do NOT use the "official" prescalers, but feature their own one, which acts at the same time for the Kaon- and the phi- Line.

The mother- and combination cuts and additional pt cuts are centralized in the following internal methods:
_Phi2KK_MC
_Phi2KK_CC
_LOWPTK
_LOWPTPHI

Further reading:
see: http://indico.cern.ch/conferenceDisplay.py?confId=82203
- changes since then:
  tag&probe for pt and RICH as well
  phi mass window 55 -> 30 MeV/cc
  MB* trigger
  PV requirement removed
  prescaled selection for low PT
  prescaled LD line
'''

__author__  = ['Andrew Powell','Sebastian Schleich']
__date__    = '2010/09/02'
__version__ = '$Revision: 1.7 $'
__all__     = ('InclPhiConf')

from Gaudi.Configuration   import *
from LHCbKernel.Configuration import *
from Configurables import LoKi__VoidFilter as VoidFilter
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingSelections.Utils import checkConfig
from Configurables import FilterDesktop, CombineParticles
import GaudiKernel.SystemOfUnits as Units
from Configurables import DeterministicPrescaler as Scaler


modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)



class InclPhiConf(object):
    __configuration_keys__ = ( 
                  'KaonPT'                   # MeV
                , 'KaonDLL'                  # adimensional
                , 'PhiMassWindow'            # MeV
                , 'HighPtPrescale'           # adimensional
                , 'LowPtPrescale'            # adimensional
                , 'LDPrescale'               # adimensional
                )

    config_default={
                  'KaonPT'              : 500      # MeV
                , 'KaonDLL'             : 15       # adimensional
                , 'PhiMassWindow'       : 30       # MeV
                , 'HighPtPrescale'      : 0.0015    # adimensional
                , 'LowPtPrescale'       : 0.0005    # adimensional
                , 'LDPrescale'          : 0.00001   # adimensional
                }



    def __init__(self, name = 'InclPhi', config= None) :
      checkConfig(InclPhiConf.__configuration_keys__, config)

      self.InclPhiHighPtLine    = self._InclPhiHighPt_X_Line( name, config , False )
      self.InclPhiLowPtLine     = self._InclPhiLowPt_X_Line(  name, config , False )
      self.InclPhiHighPtPhiLine = self._InclPhiHighPt_X_Line( name, config , True )
      self.InclPhiLowPtPhiLine  = self._InclPhiLowPt_X_Line(  name, config , True )
      self.InclPhiLDLine        = self._InclPhiLDLine( name, config )
      self.lines = [self.InclPhiHighPtLine,
		    self.InclPhiLowPtLine,
		    self.InclPhiHighPtPhiLine,
		    self.InclPhiLowPtPhiLine,
		    self.InclPhiLDLine]

    # ====================================================================
    # mother and combination cut definition
    def _Phi2KK_MC( self, config, CutOnKPT, CutOnPhiPT ) :
      GOODKAON = "(0!=NINTREE( " +\
            "HASRICH"
      if CutOnKPT == True :
        GOODKAON = GOODKAON +  " & (PT>%(KaonPT)s*MeV)" % config
      GOODKAON = GOODKAON +\
            " & (%(KaonDLL)s<PPINFO(LHCb.ProtoParticle.RichDLLk,-500,-1000))"% config +\
            " ))" % config
      if CutOnPhiPT == True :
        GOODKAON = GOODKAON +  " & (PT>(2*%(KaonPT)s-20)*MeV)" % config
      return GOODKAON + " & (ADMASS('phi(1020)')<%(PhiMassWindow)s*MeV)" % config
    def _LOWPTK( self, config ) :
      lowptk = "&(0==NINTREE( " +\
        "('K+'==ABSID)&(PT>%(KaonPT)s*MeV)" % config +\
        " ))" % config
      return lowptk
    def _LOWPTPHI( self, config ) :
      return "&(PT<(2*%(KaonPT)s+20)*MeV)" % config
    def _Phi2KK_CC( self, config ) :
      coarsemasswindow = 30 + config['PhiMassWindow']
      return "(ADAMASS('phi(1020)')<%s*MeV)" % coarsemasswindow

    # ====================================================================
    # phi stripping lines definition
    def _InclPhiHighPt_X_Line( self, name_prefix, config, Phi) :

        if Phi==True :
          name = name_prefix+"HighPtPhiLine"
          Phi2KK_MC = self._Phi2KK_MC(config, False, True)
        else :
          name = name_prefix+"HighPtLine"
          Phi2KK_MC = self._Phi2KK_MC(config, True, False)

        Phi2KK_CC = self._Phi2KK_CC(config)
        ps =  config['HighPtPrescale']
        
        #print "Creating "+name+" now!"
        #print "Prescale: ", ps
        #print Phi2KK_DC
        #print Phi2KK_CC
        #print Phi2KK_MC

        StrippingInclPhiHighPtScaler = Scaler ("StrippingInclPhiHighPtScaler", AcceptFraction=ps)
        _StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")
        _Phi2KK =  CombineParticles(
                           "_"+name+"Phi2KK"
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )
        Phi2KK = Selection( name+"Phi2KK", Algorithm = _Phi2KK, RequiredSelections = [_StdNoPIDsKaons] )

        return StrippingLine( name
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
             #, prescale = ps   Prescale realized by my own prescaler (shared between Phi and K)->Do not set here!
              , checkPV = False
              , postscale = 1
              , algos = [StrippingInclPhiHighPtScaler, Phi2KK]
              )



    def _InclPhiLowPt_X_Line( self, name_prefix, config, Phi) :
        if Phi==True :
          name = name_prefix+"LowPtPhiLine"
          Phi2KK_MC = self._Phi2KK_MC(config, False, False) + self._LOWPTPHI(config)
        else :
          name = name_prefix+"LowPtLine"
          Phi2KK_MC = self._Phi2KK_MC(config, False, False) + self._LOWPTK(config)

        Phi2KK_CC = self._Phi2KK_CC(config)
        ps =  config['LowPtPrescale']

        #print "Creating " + name + " now!"
        #print "Prescale: ", ps
        #print Phi2KK_DC
        #print Phi2KK_CC
        #print Phi2KK_MC

        StrippingInclPhiLowPtScaler = Scaler ("StrippingInclPhiLowPtScaler", AcceptFraction=ps)
        _StdNoPIDsKaons = DataOnDemand(Location = "Phys/StdNoPIDsKaons")

        _Phi2KK = CombineParticles(
                           "_"+name+"Phi2KK"
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )
        Phi2KK = Selection( name+"Phi2KK", Algorithm = _Phi2KK, RequiredSelections = [_StdNoPIDsKaons] )

	return StrippingLine(name
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
             #, prescale = ps   Prescale realized by my own prescaler (shared between Phi and K)->Do not set here!
              , checkPV = False
              , postscale = 1
              , algos = [StrippingInclPhiLowPtScaler, Phi2KK]
              )


   


    def _InclPhiLDLine( self, name_prefix, config ) :
        name = name_prefix + "LDLine"
        ## Downstream kaons track selector
        from Configurables import NoPIDsParticleMaker
        pmalg =  NoPIDsParticleMaker ( name+'NoPIDsKaons' , Particle = 'kaon' )
        pmalg.Particle = 'Kaon'
        from Configurables import TrackSelector
        pmalg.addTool(TrackSelector)
        pmalg.TrackSelector.TrackTypes = ['Downstream','Long']
        DownNoPIDsKaons = pmalg
        _DownNoPIDsKaons = DataOnDemand(Location = "Phys/"+name+'NoPIDsKaons' )

        #Phi2KK_DC = ""
        Phi2KK_CC = self._Phi2KK_CC(config)
        Phi2KK_MC = self._Phi2KK_MC(config, False, False)
        ps =  config['LDPrescale']

        #print "CC and MC for InclPhi LD stripping line."
        #print "Prescale: ", ps
        #print Phi2KK_CC
        #print Phi2KK_MC

        _Phi2KK = CombineParticles(
                           "_"+name+"Phi2KK"
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                         #, DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )
        Phi2KK = Selection( name+"Phi2KK", Algorithm = _Phi2KK, RequiredSelections = [_DownNoPIDsKaons] )

        return StrippingLine(name
              , HLT = "HLT_PASS_RE('Hlt1MB.*Decision')"
              , prescale = ps
              , postscale = 1
              , checkPV = False
              , algos = [DownNoPIDsKaons, Phi2KK]
              )

