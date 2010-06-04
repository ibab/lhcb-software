__author__ = 'Ulrich Kerzel'
__date__ = '03/06/2010'
__version__ = '$Revision: 1.1 $'

"""
B->hh selection, unbiased lifetime
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingB2hhLTUnbiasedConf(LHCbConfigurableUser):
    """
    Definition of lifetime unbiased B->hh  stripping
    """
    __slots__ = { 
        'TrackChi2'             :  10
        , 'DaughterPtMin'         :   1.0 # GeV
        , 'DaughterPtMax'         :   2.0 # GeV
        , 'DaughterPtMinLoose'    :   1.0 # GeV
        , 'DaughterPtMaxLoose'    :   1.1 # GeV
        , 'DaughterPIDKMin'       :  -2.0
        , 'DaughterPIDKMax'       :  -2.0
        , 'DaughterPIDKMinLoose'  :  -2.0
        , 'DaughterPIDKMaxLoose'  :  -2.0
        , 'BMassMin'              :   5.0 # GeV
        , 'BMassMax'              :   5.9 # GeV
        , 'BMassMinLoose'         :   4.5 # GeV
        , 'BMassMaxLoose'         :   6.0 # GeV
        , 'DOCA'                  :   0.2
        , 'DOCALoose'             :   0.5
        , 'VertexChi2'            :  25.0
                   }


    #
    # default line
    #
    def StripB2hhLTUnbiasedLoose( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        StripB2hhLTUnbiasedLoose                 = CombineParticles("StripB2hhLTUnbiasedLoose")
        #StripB2hhLTUnbiasedLoose.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiasedLoose.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiasedLoose.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMinLoose)s *GeV) & (PIDK > %(DaughterPIDKMinLoose)s) & (~ISMUON)"% self.getProps() }
        StripB2hhLTUnbiasedLoose.CombinationCut  = "((AM > %(BMassMinLoose)s *GeV) & (AM < %(BMassMaxLoose)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCALoose)s))" % self.getProps()
        StripB2hhLTUnbiasedLoose.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMaxLoose)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMaxLoose)s )" % self.getProps()
        

        StripB2hhLTUnbiasedLoose.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiasedLoose', prescale = 1, algos = [ StripB2hhLTUnbiasedLoose ] )


    #
    # loose version
    #
    def StripB2hhLTUnbiased( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units

        StripB2hhLTUnbiased                 = CombineParticles("StripB2hhLTUnbiased")
        #StripB2hhLTUnbiased.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiased.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiased.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMin)s *GeV) & (PIDK > %(DaughterPIDKMin)s) & (~ISMUON)"% self.getProps() }
        StripB2hhLTUnbiased.CombinationCut  = "((AM > %(BMassMin)s *GeV) & (AM < %(BMassMax)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCA)s))" % self.getProps()
        StripB2hhLTUnbiased.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMax)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMax)s )" % self.getProps()
        

        StripB2hhLTUnbiased.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiased', prescale = 1, algos = [ StripB2hhLTUnbiased] )    
    

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d
