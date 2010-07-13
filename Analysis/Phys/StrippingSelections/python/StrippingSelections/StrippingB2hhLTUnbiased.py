__author__ = 'Ulrich Kerzel'
__date__ = '03/06/2010'
__version__ = '$Revision: 1.3 $'

"""
B->hh selection, unbiased lifetime
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2hhLTUnbiasedConf(LHCbConfigurableUser):
    """
    Definition of lifetime unbiased B->hh  stripping
    The Stripping is aimed at an unbiased measurement of the Bs->KK lifetme.
    Two approaches are followed: An unbiased trigger for B->hh and a measurement of the
    BsKK lifetime relative to the (known) BdPiK lifetime.
    In order to efficiently select both the BsKK and BdPiK decay mode, only one kaon
    is required.

    The code ((PIDK < -0.1) | (PIDK > 0.1)) avoids the big peak at zero when pions and kaons
    cannot be separated from each other.
    """
    __slots__ = { 
        'TrackChi2'               :  10
        , 'DaughterPtMin'         :    2.5 # GeV
        , 'DaughterPtMax'         :    3.0 # GeV
        , 'DaughterPMin'          :   10.0 # GeV  
        , 'DaughterPtMinLoose'    :    1.5 # GeV
        , 'DaughterPtMaxLoose'    :    2.0 # GeV
        , 'DaughterPIDKMin'       : -999.0
        , 'DaughterPIDKMax'       :    0.1
        , 'DaughterPIDKMinLoose'  : -999.0
        , 'DaughterPIDKMaxLoose'  :    0.1
        , 'BMassMin'              :    5.0 # GeV
        , 'BMassMax'              :    5.9 # GeV
        , 'BMassMinLoose'         :    4.8 # GeV
        , 'BMassMaxLoose'         :    6.0 # GeV
        , 'DOCA'                  :    0.07
        , 'DOCALoose'             :    0.15
        , 'VertexChi2'            :   25.0
                   }


    #
    # loose line
    #
    def StripB2hhLTUnbiasedLoose( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        StripB2hhLTUnbiasedLoose                 = CombineParticles("StripB2hhLTUnbiasedLoose")
        #StripB2hhLTUnbiasedLoose.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiasedLoose.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiasedLoose.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMinLoose)s *GeV) & (P> %(DaughterPMin)s *GeV) & (PIDK > %(DaughterPIDKMinLoose)s)"% self.getProps() }
        StripB2hhLTUnbiasedLoose.CombinationCut  = "((AM > %(BMassMinLoose)s *GeV) & (AM < %(BMassMaxLoose)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCALoose)s))" % self.getProps()
        StripB2hhLTUnbiasedLoose.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMaxLoose)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMaxLoose)s )" % self.getProps()
        

        StripB2hhLTUnbiasedLoose.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiasedLoose', prescale = 100, algos = [ StripB2hhLTUnbiasedLoose ] )


    #
    # default version
    #
    def StripB2hhLTUnbiased( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        StripB2hhLTUnbiased                 = CombineParticles("StripB2hhLTUnbiased")
        #StripB2hhLTUnbiased.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiased.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiased.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMin)s *GeV) & (P> %(DaughterPMin)s *GeV) & ((PIDK < -0.1) | (PIDK > 0.1)) & (PIDK > %(DaughterPIDKMin)s)"% self.getProps() }
        StripB2hhLTUnbiased.CombinationCut  = "((AM > %(BMassMin)s *GeV) & (AM < %(BMassMax)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCA)s))" % self.getProps()
        StripB2hhLTUnbiased.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMax)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMax)s )" % self.getProps()
        

        StripB2hhLTUnbiased.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiased', prescale = 1, algos = [ StripB2hhLTUnbiased] )

    #
    # with Trigger
    #
    def StripB2hhLTUnbiasedTrigger( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        StripB2hhLTUnbiasedTrigger                 = CombineParticles("StripB2hhLTUnbiasedTrigger")
        #StripB2hhLTUnbiasedTrigger.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiasedTrigger.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiasedTrigger.DaughtersCuts   = { "K+" : "ISLONG" }
        StripB2hhLTUnbiasedTrigger.CombinationCut  = "((AM > %(BMassMinLoose)s *GeV) & (AM < %(BMassMaxLoose)s *GeV))" % self.getProps()
        StripB2hhLTUnbiasedTrigger.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s)" % self.getProps()
        

        StripB2hhLTUnbiasedTrigger.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiasedTrigger',
                             prescale = 1,
                             HLT = "HLT_PASS('Hlt1DiHadronLTUnbiasedDecision')" ,
                             algos = [ StripB2hhLTUnbiasedTrigger ] )
          

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
