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

    In here, the lifetime is measured in ns (rather than mm) (from the LoKi web-page)
    The speed of light = 299 792 458 m / s
    """
    __slots__ = { 
        'TrackChi2'               :    5.0
        , 'CTauMin'               :    0.1 # mm 
        , 'DaughterPtMin'         :    1.8 # GeV
        , 'DaughterPtMax'         :    2.5 # GeV
        , 'DaughterPMin'          :   15.0 # GeV
        , 'DaughterPMinLoose'     :   10.0 # GeV  
        , 'DaughterPtMinLoose'    :    1.5 # GeV
        , 'DaughterPtMaxLoose'    :    2.0 # GeV
        , 'DaughterPIDKMin'       : -999.0
        , 'DaughterPIDKMax'       :    1.0
        , 'DaughterPIDKMinLoose'  : -999.0
        , 'DaughterPIDKMaxLoose'  :    0.1
        , 'BMassMin'              :    5.0 # GeV
        , 'BMassMax'              :    5.9 # GeV
        , 'BMassMinLoose'         :    4.9 # GeV
        , 'BMassMaxLoose'         :    6.0 # GeV
        , 'DOCA'                  :    0.07
        , 'DOCALoose'             :    0.10
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
        StripB2hhLTUnbiasedLoose.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMinLoose)s *GeV) & (P> %(DaughterPMinLoose)s *GeV) & (PIDK > %(DaughterPIDKMinLoose)s) & ((PIDK < -0.1) | (PIDK > 0.1)) "% self.getProps() }
        StripB2hhLTUnbiasedLoose.CombinationCut  = "((AM > %(BMassMinLoose)s *GeV) & (AM < %(BMassMaxLoose)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCALoose)s))" % self.getProps()
        StripB2hhLTUnbiasedLoose.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMaxLoose)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMaxLoose)s) " % self.getProps()
        

        StripB2hhLTUnbiasedLoose.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiasedLoose', prescale = 0.01,  postscale = 1.0, algos = [ StripB2hhLTUnbiasedLoose ] )


    #
    # default version
    #
    def StripB2hhLTUnbiased( self ) :
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember
        from Configurables import FilterDesktop, CombineParticles
        import GaudiKernel.SystemOfUnits as Units
        from GaudiKernel.PhysicalConstants import c_light

        StripB2hhLTUnbiased                 = CombineParticles("StripB2hhLTUnbiased")
        StripB2hhLTUnbiased.Preambulo      += [ "from GaudiKernel.PhysicalConstants import c_light "]
        #StripB2hhLTUnbiased.ReFitPVs        = True
        #StripB2hhLTUnbiased.OutputLevel    =   1
        StripB2hhLTUnbiased.DecayDescriptor = "B_s0 -> K+ K-"
        StripB2hhLTUnbiased.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMin)s * GeV) & (P> %(DaughterPMin)s *GeV) & ((PIDK < -0.1) | (PIDK > 0.1)) & (PIDK > %(DaughterPIDKMin)s)"% self.getProps() }
        StripB2hhLTUnbiased.CombinationCut  = "((AM > %(BMassMin)s *GeV) & (AM < %(BMassMax)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCA)s))" % self.getProps()
        StripB2hhLTUnbiased.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMax)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMax)s) & ((BPVLTIME('PropertimeFitter/properTime:PUBLIC')*c_light)>%(CTauMin)s)" % self.getProps()


        # plots
        from Configurables import LoKi__Hybrid__PlotTool as PlotTool
        StripB2hhLTUnbiased.HistoProduce = TRUE
        StripB2hhLTUnbiased.addTool( PlotTool("MotherPlots") )
        StripB2hhLTUnbiased.MotherPlots.Preambulo += [ "from GaudiKernel.PhysicalConstants import c_light "]
        StripB2hhLTUnbiased.MotherPlots.Histos = { "P/GeV"                                                              : ('P'     ,  20  ,  50) ,
                                                   "PT/GeV"                                                             : ('Pt'    ,  0   ,  10) ,
                                                   "VFASPF(VCHI2/VDOF)"                                                 : ('Chi2'  ,  0   ,  25) ,
                                                   "BPVLTIME('PropertimeFitter/properTime:PUBLIC')*c_light"             : ('tau '  , -0.2 ,   3) ,
                                                   "(MAXTREE(('K+'==ABSID) , PIDK))"                                    : ('PidMax', -1   ,  20)}
        

        StripB2hhLTUnbiased.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiased', prescale = 1, postscale = 1.0, algos = [ StripB2hhLTUnbiased] )

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
        StripB2hhLTUnbiasedTrigger.DaughtersCuts   = { "K+" : "ISLONG & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMinLoose)s *GeV) & (P> %(DaughterPMinLoose)s *GeV) " % self.getProps() }
        StripB2hhLTUnbiasedTrigger.CombinationCut  = "((AM > %(BMassMin)s *GeV) & (AM < %(BMassMax)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCA)s)) " % self.getProps()
        StripB2hhLTUnbiasedTrigger.MotherCut       = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMax)s)" % self.getProps()
        

        StripB2hhLTUnbiasedTrigger.InputLocations = [ 'Phys/StdNoPIDsKaons' ]
        return StrippingLine('B2hhLTUnbiasedTrigger',
                             prescale = 1.0, postscale = 0.1,
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
