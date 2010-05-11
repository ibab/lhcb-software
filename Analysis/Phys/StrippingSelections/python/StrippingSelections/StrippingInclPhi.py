# $Id: StrippingInclPhi.py,v 1.3 2010-05-11 17:25:44 schleich Exp $

__author__ = 'Andrew Powell, Sebastian Schleich'
__date__ = '2010/04/27'
__version__ = '$Revision: 1.3 $'

'''
InclPhi stripping selection
Two lines: Phi->(Long K, Long K) and Phi->(Down K, Down K)
see: http://indico.cern.ch/conferenceDisplay.py?confId=82203
- since then:
  tag&probe for pt as well (adaption done for trigger studies)
  phi mass window 55 -> 35 MeV/cc
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
                , 'KaonP'               : 0        # MeV (formerly 2000 => eff100%)
                , 'KaonDLL'             : 15       # adimensional
                , 'KaonTRPCHI2'         : 0.0      # adimensional
                , 'PhiVertexCHI2pDOF'   : 10       # adimensional
                , 'PhiMassWindow'       : 35       # MeV
                , 'Prescale'            : 1        # adimensional
                }

    def InclPhiLine( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units


        INRICH_2 =   "( 2 == NINTREE ( HASRICH ) )"
        GOODKAON =   "( 0 != NINTREE ( (%(KaonDLL)s < PPINFO(LHCb.ProtoParticle.RichDLLk,-500,-1000)) ) )" % self.getProps()
        HIGHPTKAON = "( 0 != NINTREE ( (PT > %(KaonPT)s *MeV) ) )" % self.getProps()

        Phi2KK_DC = "(P > %(KaonP)s *MeV) & (TRPCHI2 > %(KaonTRPCHI2)s)" % self.getProps()
        coarsemasswindow = 45+ self.getProps()['PhiMassWindow']
        Phi2KK_CC = "(ADAMASS('phi(1020)') < %s*MeV)" % coarsemasswindow
        Phi2KK_MC = INRICH_2 +\
                   "& " + GOODKAON +\
                   "& " + HIGHPTKAON +\
                   "& (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s)"\
                   "& (ADMASS('phi(1020)') < %(PhiMassWindow)s*MeV)" % self.getProps()
        ps =  self.getProps()['Prescale']

        print "DC, CC and MC for InclPhi stripping line."
        print "Prescale: ", ps
        print Phi2KK_DC
        print Phi2KK_CC
        print Phi2KK_MC

        Phi2KK = StrippingMember ( CombineParticles
                          , "Phi2KK"
                          , InputLocations = ["StdNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )

        line = StrippingLine('InclPhiLine'
              , HLT = "HLT_PASS_RE('Hlt1MBMicro.*Decision')"
              , prescale = ps
              , postscale = 1
              , algos = [Phi2KK]
              )

        return line

    def InclPhiDDLine( self ) :
        '''
        Downstream kaons track selector
        '''
        from Configurables         import NoPIDsParticleMaker
        #from CommonParticles.Utils import *
        pmalg =  NoPIDsParticleMaker ( 'DownNoPIDsKaons' , Particle = 'kaon' )
        pmalg.Particle = 'Kaon'
        from Configurables import TrackSelector
        pmalg.addTool(TrackSelector)
        pmalg.TrackSelector.TrackTypes = ['Downstream']
        DownNoPIDsKaons = pmalg

        '''
        Downstream K stripping
        Differences: DownNoPIDsKaons is added to algorithms and input location adapted. TODO: Possible to edit algorithm list via cloning??
        '''
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember, StrippingTool
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        INRICH_2 = "( 2 == NINTREE ( HASRICH ) )"
        GOODKAON = "( 0 != NINTREE ( (%(KaonDLL)s < PPINFO(LHCb.ProtoParticle.RichDLLk,-500,-1000)) ) )" % self.getProps()
        HIGHPTKAON = "( 0 != NINTREE ( (PT > %(KaonPT)s *MeV) ) )" % self.getProps()

        Phi2KK_DC = "(P > %(KaonP)s *MeV) & (TRPCHI2 > %(KaonTRPCHI2)s)" % self.getProps()
        coarsemasswindow = 45+ self.getProps()['PhiMassWindow']
        Phi2KK_CC = "(ADAMASS('phi(1020)') < %s*MeV)" % coarsemasswindow
        Phi2KK_MC = INRICH_2 +\
                   "& " + GOODKAON +\
                   "& " + HIGHPTKAON +\
                   "& (VFASPF(VCHI2/VDOF) < %(PhiVertexCHI2pDOF)s)"\
                   "& (ADMASS('phi(1020)') < %(PhiMassWindow)s*MeV)" % self.getProps()
        ps =  self.getProps()['Prescale']

        print "DC, CC and MC for InclPhi DD stripping line."
        print "Prescale: ", ps
        print Phi2KK_DC
        print Phi2KK_CC
        print Phi2KK_MC

        Phi2KK = StrippingMember ( CombineParticles
                          , "Phi2KK"
                          , InputLocations = ["DownNoPIDsKaons"]
                          , DecayDescriptor = "phi(1020) -> K+ K-"
                          , DaughtersCuts = {"K+": Phi2KK_DC}
                          , CombinationCut = Phi2KK_CC
                          , MotherCut = Phi2KK_MC
                          )

        line = StrippingLine('InclPhiDDLine'
              , HLT = "HLT_PASS_RE('Hlt1MBMicro.*Decision')"
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

