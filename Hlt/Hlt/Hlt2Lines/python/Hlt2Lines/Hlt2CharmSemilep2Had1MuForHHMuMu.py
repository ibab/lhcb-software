__author__  = [ 'Benoit F. Viaud' ]
__date__    = '$Date: 22-06-2011$'
__version__ = '$Revision: 0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

from Hlt2Lines.Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsLines import Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsLinesConf

        
class Hlt2CharmSemilep2Had1MuForHHMuMuConf(HltLinesConfigurableUser) :
    """
    Recieves as an input the hadron pair reconstructed by
    Hlt2CharmSemilepTwoHadForMuMuHH and add a normal muon.
    The product is transmitted to Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsLines,
    which adds an additional, second loop, muon.  

    NOTE:  Any Hlt2Line configurable that uses this MUST be on the list of
    __used_configurables__.
    """



    __used_configurables__ = [ Hlt2CharmSemilepD02HHMuMuHardHadronsSoftMuonsLinesConf ]

    
    __slots__ = {
                 ## HHmu
                    'TrkPt_HHmu'                 : 500.0 * MeV
                  , 'TrkP_HHmu'                  : 5000.0 * MeV
                  , 'TrkPVIPChi2_HHmu'           : 0.0      # unitless
                  , 'TrkPVIPChi2MAX_HHmu'           : 10.0      # unitless
                  ,  'TrkPtMAX_HHmu'                 : 0.0 * MeV
                  , 'TrkChi2_HHmu'               : 8.0      # unitless
                  , 'PairMinDoca_HHmu'           : 0.12 * mm
                  , 'PairMaxDoca_HHmu'           : 0.25 * mm
                  , 'VtxPVDispChi2_HHmu'         : 20.0    # unitless
                  , 'VtxChi2_HHmu'               : 30.0     # unitless
                  , 'DSumPt_HHmu'                : 0.0 * MeV
                   , 'MCOR_MAX_HHmu'              : 3500.0 * MeV
                   , 'WideMass_M_MIN'              :   0.0 * MeV
                  , 'WideMass_M_MAX'              : 1950.0 * MeV
                  ## 2-muonInput for HHmu
                  , 'TrkChi2_2MuonForHHmu'       : 5.0      # unitless
                 ##
                  , '__hlt2twoHadOneMuforHHMuMuSeq__'       : None
                }


    ##   Interface with next step (adding a 2nd Loop Muon)
    def  twoHadOneMuforHHMuMuSequence(self) : # {
        return self.getProp("__hlt2twoHadOneMuforHHMuMuSeq__")
    # }


    def __InPartFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_HHmu)s)" \
                 "& (PT> %(TrkPt_HHmu)s)" \
                 "& (P> %(TrkP_HHmu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmu)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq


    def __HHmuCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = self.getProp('WideMass_M_MIN')
        massmax = self.getProp('WideMass_M_MAX')

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)


        combcuts = "(AM<2100*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3) > %(DSumPt_HHmu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmu)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmu)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_HHmu)s)" \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_HHmu)s )" \
                     "& ( SUMTREE( ( (ID=='K+') | (ID=='K-') | (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmu)s)" % self.getProps() 
        combineCharmHHmu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmHHmu])

    def __HHmuFilter(self, name, inputSeq, extracode = None) :

        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        if extracode :
            filter = Hlt2Member( FilterDesktop
                                 , 'Filter'
                                 , Inputs = inputSeq
                                 , Code = extracode
                                 )
        else :
            filter = Hlt2Member( FilterDesktop
                                 , 'Filter'
                                 , Inputs = inputSeq
                                 , Code = 'ALL'
                                 )

        return bindMembers(name, inputSeq + [filter])
 
    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedMuons

        from HltTracking.HltPVs import PV3D
        
        # Filter muons with LowIP Cut
        MuonsLowIP = self.__InPartFilter('CharmHHmuForHHmumuInputMu', [ BiKalmanFittedMuons] )
         
        from  Hlt2CharmSemilepTwoHadForMuMuHH import  Hlt2CharmSemilepTwoHadForMuMuHHConf

        Hlt2Charm2HadForHHmu =   Hlt2CharmSemilepTwoHadForMuMuHHConf().twoHadforHHMuMuSequence()

        #Second Stage - picks up two low pt tracks too

        # Make HHmu 
        CharmHHmuCombine = self.__HHmuCombine ( name ='CharmHHMuForHHMuMu'
                                                  , inputSeq = [Hlt2Charm2HadForHHmu, MuonsLowIP]
                                                  , decayDesc = [ "D+ -> K*(892)0  mu+ ","D- -> K*(892)0  mu-"]
                                                 )   

        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))

        # Transmitted to the next stage:
        CharmSemilep2Had1MuForHHMuMu = self.__HHmuFilter ( name = 'CharmSemilep2Had1MuForHHMuMu', inputSeq = [CharmHHmuCombine], extracode = wideMassCut )

        self.setProp('__hlt2twoHadOneMuforHHMuMuSeq__', CharmSemilep2Had1MuForHHMuMu)
