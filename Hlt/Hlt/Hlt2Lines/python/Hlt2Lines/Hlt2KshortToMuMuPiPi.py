__author__  = [ 'Mike Sokoloff' ]
__date__    = '$Date: 14-06-2012$'
__version__ = '$Revision: 0.0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2KshortToMuMuPiPiConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## HHmumu
                    'TrkPt_HHmumu'                 : 300.0 * MeV
                  , 'TrkP_HHmumu'                  : 3000.0 * MeV
                  , 'TrkPVIPChi2_HHmumu'           : 0.0      # unitless
                  , 'TrkPVIPChi2MAX_HHmumu'           : 12.0      # !!unitless
                  ,  'TrkPtMAX_HHmumu'                 : 0.0 * MeV
                  , 'TrkChi2_HHmumu'               : 4.0      # unitless
                  , 'PairMinDoca_HHmumu'           : 0.1 * mm
                  , 'PairMaxDoca_HHmumu'           : 0.2 * mm
                  , 'VtxPVDispChi2_HHmumu'         : 9.0    #  unitless
                  , 'VtxChi2_HHmumu'               : 15.0     # unitless
                  , 'DIPChi2_HHmumu'               : 16.0     #  unitless
                  , 'DSumPt_HHmumu'                : 1500.0 * MeV
                  , 'DDira'                        : 0.9999     # adimensional 0.9999 <--> 14 mr
                 , 'Sig_M_MIN'                   : 490.0 * MeV
                  , 'Sig_M_MAX'                   : 540.0 * MeV
                  , 'WideMass_M_MIN'              : 490.0 * MeV
                  , 'WideMass_M_MAX'              : 560.0 * MeV
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : False       # do or do not
                  , 'GEC_NTRACK_MAX'           : 120000        # max number of tracks
                  , 'HltANNSvcID'  : {
                          'Hlt2DimuonForKshortToMuMuPiPiDecision' : 61033
                        ## For new lines
                        , 'Hlt2KshortToMuMuPiPiDecision'          : 61034
                        }
                }


    def __seqGEC(self) : # {
        """
        # Defines a Global Event Cut (mild badness) on all events with more
        # than a configurable upper limit of tracks.
        #
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        """
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        from HltLine.HltLine import bindMembers

        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        tracks = Hlt2BiKalmanFittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('GEC_Filter_NTRACK') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()
        # }

        Hlt2KshortKillTooManyInTrkAlg = VoidFilter('Hlt2KshortToMuMuPiPiTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2KshortKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2KshortKillTooManyInTrkAlg ] )

        return Hlt2KshortKillTooManyInTrk
    # }


    def __InPartFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = "(TRCHI2DOF< %(TrkChi2_HHmumu)s )" \
                 "& (PT> %(TrkPt_HHmumu)s)" \
                 "& (P> %(TrkP_HHmumu)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_HHmumu)s )" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
  

    def __HHMuMuCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)


        combcuts = "(AM<550*MeV)" \
                   "& (AMAXCHILD(PT) > %(TrkPtMAX_HHmumu)s      * MeV) "  \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_HHmumu)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_HHmumu)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_HHmumu)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_HHmumu)s) " \
                     "& (BPVDIRA                 > %(DDira)s         ) " \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_HHmumu)s )" \
                     "& ( SUMTREE( ( (ID=='pi+') | (ID=='pi-') | (ID=='mu+') | (ID=='mu-') ), sqrt(BPVIPCHI2()) ) > %(TrkPVIPChi2MAX_HHmumu)s)"\
                     "& (BPVIPCHI2() < %(DIPChi2_HHmumu)s )" % self.getProps() 
        combineKshortHHmumu = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineKshortHHmumu])

    def __HHMuMuFilter(self, name, inputSeq, extracode = None) :

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
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions

        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons with LowIP Cut
        Pions = self.__InPartFilter('KshortMuMuPiPiInputPions', [ BiKalmanFittedPions] )
      
        from   Hlt2TwoMuonForKshortToMuMuPiPi import  Hlt2TwoMuonForKshortToMuMuPiPiConf

        Hlt2TwoMuonForKshortToMuMuPiPi =   Hlt2TwoMuonForKshortToMuMuPiPiConf().twoMuonSequence()
 
        # Make HHmumu 

        KshortPiPiMuMuCombine = self.__HHMuMuCombine ( name = 'Kshort2PiPiMuMu'
                                                  , inputSeq = [Hlt2TwoMuonForKshortToMuMuPiPi , Pions]
                                                  , decayDesc = [ "KS0 -> rho(770)0 pi+ pi-",
                                                                  "KS0 -> rho(770)+ pi- pi-",
                                                                  "KS0 -> rho(770)- pi+ pi+"]
                                                 )
        

        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))


        Hlt2KshortPiPiMuMu = self.__HHMuMuFilter ( name = 'KshortMuMuPiPi', inputSeq = [KshortPiPiMuMuCombine], extracode = sigMassCut )

        ###########################################################################
        # Define the Hlt2 Lines
        # 
        ##########################################################################
        Hlt2KshortKillTooManyInTrk = self.__seqGEC()

        line = Hlt2Line('DimuonForKshortToMuMuPiPi', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2KshortKillTooManyInTrk, Hlt2TwoMuonForKshortToMuMuPiPi]
                        , postscale = self.postscale
                        )
        decName = "Hlt2DimuonForKshortToMuMuPiPiDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
       
        ## PiPiMuMu
        line = Hlt2Line('KshortToMuMuPiPi', prescale = self.prescale
                        , algos = [ PV3D(), Hlt2KshortKillTooManyInTrk, Hlt2TwoMuonForKshortToMuMuPiPi, Pions, Hlt2KshortPiPiMuMu] 
                        , postscale = self.postscale
                        )
        decName = "Hlt2KshortToMuMuPiPiDecision"
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
