__author__ = 'Ulrich Kerzel'
__date__ = '03/06/2010'
__version__ = '$Revision: 1.3 $'

"""
B->hh selection, unbiased lifetime
"""

__author__ = ['Ulrich Kerzel']
__date__ = '04/02/2011'
__version__ = '$Revision: 1.1 $'

__all__ = ('StrippingB2hhLTUnbiasedConf',
           'makeB2hh')


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder



#from Gaudi.Configuration import *
#from LHCbKernel.Configuration import *
#from StrippingConf.StrippingLine import StrippingLine, StrippingMember
#from Configurables import FilterDesktop, CombineParticles, LoKi__VoidFilter
#import GaudiKernel.SystemOfUnits as Units
#from GaudiKernel.PhysicalConstants import c_light

default_name = "B2hhLTUnbiased"


class StrippingB2hhLTUnbiasedConf(LineBuilder):
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

    __configuration_keys__ = (
        'TrackChi2'               #    5.0
        #'SpdMult'               # 1000.0
        , 'DaughterPtMin'         #    1.5 # GeV
        , 'DaughterPtMax'         #    2.0 # GeV
        , 'DaughterPMin'          #   10.0 # GeV
        , 'DaughterPIDKMax'       #    0.1
        , 'BMassMin'              #    5.0 # GeV
        , 'BMassMax'              #    6.0 # GeV
        , 'DOCA'                  #    0.07
        , 'VertexChi2'            #   25.0
        , 'PrescaleLoose'         #    0.005
        , 'PostscaleLoose'        #    1.0
        , 'PrescaleNB'
        , 'PostscaleNB'   
        , 'NetCut'                 #   -1.0
        , 'BLifetime'             #   -999.0 
        )
    


    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        noCut_name = name+"NoCut"
        loose_name = name+"Loose"
        NB_name    = name+"NeuroBayes" 

        self.SelB2hhLoose    = self.B2hhLoose(loose_name,
                                              BMassMin        = config['BMassMin'],
                                              BMassMax        = config['BMassMax'],
                                              TrackChi2       = config['TrackChi2'],
                                              DaughterPtMin   = config['DaughterPtMin'],
                                              DaughterPtMax   = config['DaughterPtMax'],
                                              DaughterPMin    = config['DaughterPMin'],
                                              DaughterPIDKMax = config['DaughterPIDKMax'],
                                              DOCA            = config['DaughterPIDKMax'],
                                              VertexChi2      = config['VertexChi2'],
                                              BLifetime       = config['BLifetime']
                                              )
        
        self.SelB2hhNB       = self.B2hhNeuroBayes (NB_name,
                                                    self.SelB2hhLoose,
                                                    NetCut  = config['NetCut'])
        
        self.loose_line      = StrippingLine(loose_name+"Line",
                                             prescale = config['PrescaleLoose'],
                                             postscale = config['PostscaleLoose'],
                                             selection = self.SelB2hhLoose
                                             )
        self.NeuroBayesLine  = StrippingLine(NB_name+"Line",
                                             prescale  = config['PrescaleNB'],
                                             postscale = config['PostscaleNB'],
                                             selection = self.SelB2hhNB
                                             )
        
        self.registerLine(self.loose_line)
        self.registerLine(self.NeuroBayesLine)
        
    #
    # loose line  - also used as input for NeuroBayes based nominal line
    #
    def B2hhLoose( self, Name, BMassMin, BMassMax, TrackChi2,
                   DaughterPtMin, DaughterPtMax, DaughterPMin, DaughterPIDKMax,
                   DOCA, VertexChi2, BLifetime ) :
        from GaudiKernel.SystemOfUnits import GeV
        
        
        kaonCut   = "(ISLONG) & (HASRICH) & (PPCUT(PP_RICHTHRES_K)) & (TRCHI2DOF < %(TrackChi2)s) & (PT > %(DaughterPtMin)s*GeV) & (P> %(DaughterPMin)s*GeV)" % locals()
        combCut   = "(AM > %(BMassMin)s *GeV) & (AM < %(BMassMax)s *GeV) & (AMAXDOCA('LoKi::DistanceCalculator') < %(DOCA)s)"                                 % locals()
        motherCut = "(VFASPF(VCHI2/VDOF) < %(VertexChi2)s) & (MAXTREE(('K+'==ABSID) ,PT) > %(DaughterPtMax)s*GeV) & (MAXTREE(('K+'==ABSID) , PIDK) > %(DaughterPIDKMax)s) & (BPVLTIME()>%(BLifetime)s*ps)"   % locals()
            
            
        from StandardParticles import StdAllNoPIDsKaons
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        kaonFilter = FilterDesktop(Code = kaonCut)
        myKaons    = Selection(Name+'KaonSel', Algorithm = kaonFilter, RequiredSelections = [StdAllNoPIDsKaons])

        _Bs = CombineParticles (DecayDescriptor = "B_s0 -> K+ K-",
                                CombinationCut  = combCut,
                                MotherCut       = motherCut)
            
        B2hhLooseSel = Selection(Name, Algorithm = _Bs, RequiredSelections = [myKaons])
        
        return B2hhLooseSel

        
    #
    # default version
    #
    def B2hhNeuroBayes (self, Name, BsLooseInputSel, NetCut):
            
        from Configurables import StrippingNBBhh
        BhhNB                    = StrippingNBBhh("BhhNB")
        #BhhNB.OutputLevel        = 2
        BhhNB.PlotHisto          = False
        BhhNB.PlotMassMin        =  5.0
        BhhNB.PlotMassMax        =  6.0
        BhhNB.PlotNBins          = 120
        BhhNB.Expertise          = 'bshhet.nb'
        #BhhNB.Expertise          = 'bshhnet_zeroiter.nb'
        BhhNB.NetworkVersion     = "TuneMC10"
        BhhNB.NetworkCut         = NetCut

        BhhNBSel     = Selection(Name     , Algorithm = BhhNB     , RequiredSelections = [ BsLooseInputSel ] )

        return BhhNBSel
            
    
    
    
    
    
    
    
    #def SpdMultFilterLoose(self):
    #    return LoKi__VoidFilter("SpdMultFilterLoose",
    #                            Code = "( recSummary(LHCb.RecSummary.nSpd,'Raw/Spd/Digits')<%(SpdMultLoose)s )" % self.getProps()
    #                            )
    #
    #def SpdMultFilter(self):
    #    return LoKi__VoidFilter("SpdMultFiltere",
    #                            Code = "( recSummary(LHCb.RecSummary.nSpd,'Raw/Spd/Digits')<%(SpdMult)s )" % self.getProps()
    #                            ) 
    
    
    
