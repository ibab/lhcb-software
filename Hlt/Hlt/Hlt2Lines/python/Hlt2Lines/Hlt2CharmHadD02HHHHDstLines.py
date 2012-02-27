__author__  = [ 'Tom Hampson', 'Benoit Viaud' ]
__date__    = '$Date: 24/02/2012$'
__version__ = '$Revision: 0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmHadD02HHHHDstLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 4Body
                    'TrkPt_4Body'                 : 300.0 * MeV
                  , 'TrkP_4Body'                  : 3000.0 * MeV
                  , 'TrkPVIPChi2_4Body'           : 1.8      # unitless
                  , 'TrkChi2_4Body'               : 3.0      # unitless
                  , 'PairMinDoca_4Body'           : 0.10 * mm
                  , 'PairMaxDoca_4Body'           : 0.25 * mm
                  , 'VtxPVDispChi2_4Body'         : 36.0     # unitless
                  , 'BPVVDR_4Body'                : 0.0     # rho(PV-SV) mm
                  , 'VtxChi2_4Body'               : 15.0     # unitless
                  , 'DIPChi2_4Body'               : 50.0     # unitless
                  , 'BPVDIRA_4Body'               : 0.9999   # unitless
                  , 'DSumPt_4Body'                : 1500.0 * MeV
                  , 'MCOR_MAX_4Body'              : 3500.0 * MeV
                  , 'Sig_M_MIN'                   : 1800.0 * MeV
                  , 'Sig_M_MAX'                   : 1930.0 * MeV
                  , 'WideMass_M_MIN'              : 1700.0 * MeV
                  , 'WideMass_M_MAX'              : 2100.0 * MeV
                  ## D* combination
                  , 'TrkPt_SlowPion'              : 300.0 * MeV 
                  , 'TrkP_SlowPion'               : 3000.0 * MeV
                  , 'TrkChi2_SlowPion'            : 100.0
                  , 'PairMaxDoca_Dstar'           : 100.0 * mm
                  , 'DeltaM_MIN'                  : 0.0 * MeV
                  , 'DeltaM_MAX'                  : 170.0 * MeV
                  , 'DeltaM_MINwide'              : 0.0 * MeV
                  , 'DeltaM_MAXwide'              : 170.0 * MeV
                  ## 2-body Input for 4Body
                  , 'TrkChi2_2BodyFor4Body'       : 5.0      # unitless
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : False       # do or do not
                  , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD02HHHHDst'
                  # prescales
                  , 'Prescale'                  : {
                          'Hlt2CharmHadD02HHHHDst_4piWideMass' : 0.1
                        , 'Hlt2CharmHadD02HHHHDst_K3piWideMass' : 0.1
                        , 'Hlt2CharmHadD02HHHHDst_KKpipiWideMass' : 0.1
                        , 'Hlt2CharmHadD02HHHHDst_2K2piWideMass' : 0.1
                        , 'Hlt2CharmHadD02HHHHDst_3KpiWideMass' : 0.1
                        , 'Hlt2CharmHadD02HHHHDst_Ch2'         : 0.0
                        , 'Hlt2CharmHadD02HHHHDst_Ch2WideMass' : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHadD02HHHHDst2BodyDecision'    : 62007
                        , 'Hlt2CharmHadD02HHHHDst_4piDecision'         : 62008
                        , 'Hlt2CharmHadD02HHHHDst_4piWideMassDecision' : 62009
                        , 'Hlt2CharmHadD02HHHHDst_K3piDecision'         : 62010
                        , 'Hlt2CharmHadD02HHHHDst_K3piWideMassDecision' : 62011
                        , 'Hlt2CharmHadD02HHHHDst_KKpipiDecision'         : 62012
                        , 'Hlt2CharmHadD02HHHHDst_KKpipiWideMassDecision' : 62013
                        , 'Hlt2CharmHadD02HHHHDst_2K2piDecision'         : 62014
                        , 'Hlt2CharmHadD02HHHHDst_2K2piWideMassDecision' : 62015
                        , 'Hlt2CharmHadD02HHHHDst_3KpiDecision'         : 62016
                        , 'Hlt2CharmHadD02HHHHDst_3KpiWideMassDecision' : 62017
                        , 'Hlt2CharmHadD02HHHHDst_Ch2Decision'         : 62018
                        , 'Hlt2CharmHadD02HHHHDst_Ch2WideMassDecision' : 62019
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

        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter( filtName
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __InPartFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D


        incuts = "(TRCHI2DOF< %(TrkChi2_4Body)s)" \
                 "& (PT> %(TrkPt_4Body)s)" \
                 "& (P> %(TrkP_4Body)s)" \
                 "& (MIPCHI2DV(PRIMARY)> %(TrkPVIPChi2_4Body)s)" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , Inputs = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq


    def __4BodyCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        massmin = min(self.getProp('Sig_M_MIN'), self.getProp('WideMass_M_MIN'))
        massmax = max(self.getProp('Sig_M_MAX'), self.getProp('WideMass_M_MAX'))

        masscut = "in_range(%s,  M, %s)" % (massmin, massmax)

        combcuts = "(AM<2100*MeV)" \
                   "& ((APT1+APT2+APT3+APT4) > %(DSumPt_4Body)s)" \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(PairMinDoca_4Body)s)" \
                   "& (AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_4Body)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut + \
                     "& (VFASPF(VCHI2PDOF) < %(VtxChi2_4Body)s) " \
                     "& (BPVCORRM < %(MCOR_MAX_4Body)s)" \
                     "& (BPVVDCHI2> %(VtxPVDispChi2_4Body)s )" \
                     "& (BPVVDR> %(BPVVDR_4Body)s )" \
                     "& (BPVDIRA> %(BPVDIRA_4Body)s)" \
                     "& (BPVIPCHI2() < %(DIPChi2_4Body)s )" % self.getProps()
        combineCharm4Body = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )
        return bindMembers(name, [PV3D()] + inputSeq + [combineCharm4Body])

    def __4BodyFilter(self, name, inputSeq, extracode = None) :

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


    def __filterHlt1TOS(self, name, input) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import TisTosParticleTagger

        filterTOS = Hlt2Member( TisTosParticleTagger
                                , 'Hlt1TOSFilter'
                                , Inputs = [ input.outputSelection() ]
                                , TisTosSpecs = self.getProp('TisTosParticleTaggerSpecs')
                              )

        return bindMembers(name, [ input, filterTOS ])
    # }

    def __SlowPionFilter(self, name, inputContainers) : 
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D
        
        incuts = "(TRCHI2DOF< %(TrkChi2_SlowPion)s )" \
                 "& (PT> %(TrkPt_SlowPion)s )" \
                 "& (P> %(TrkP_SlowPion)s )" % self.getProps()
        
        filter = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , Inputs = inputContainers
                             , Code = incuts
                           )

        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )
        
        return filterSeq

    def __DstarCombine(self, name, inputSeq, decayDesc, masscut) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Dstar)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = masscut
        
        combineDstar = Hlt2Member( CombineParticles
                                   , "Combine_Dstar"
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq 
                                   , CombinationCut = combcuts
                                   , MotherCut = mothercuts
                                   )
        return bindMembers(name, [PV3D()] + inputSeq + [combineDstar])


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions, BiKalmanFittedKaons
        from HltTracking.HltPVs import PV3D
        
        # Filter pions and kaons in the final state (F)
        pionName = self.getProp('name_prefix') + 'PionsF'
        kaonName = self.getProp('name_prefix') + 'KaonsF'
        pionsF = self.__InPartFilter(pionName, [ BiKalmanFittedPions] )
        kaonsF = self.__InPartFilter(kaonName, [ BiKalmanFittedKaons] )

         
        from Hlt2CharmHadTwoBodyForD02HHHH import Hlt2CharmHadTwoBodyForD02HHHHConf
        Hlt2Charm2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodySequence()

        
        #
        # Make 4Body : that's where differentiation between modes starts...
        #

        modeName = self.getProp('name_prefix')

        # Prepare mass cuts
        sigMassCut  = "in_range(%s, M, %s)" \
                      % (self.getProp('Sig_M_MIN'), self.getProp('Sig_M_MAX'))
        wideMassCut = "in_range(%s, M, %s)" \
                      % (self.getProp('WideMass_M_MIN'), \
                         self.getProp('WideMass_M_MAX'))
        
        DeltaMSigMassCut = "(M-MAXTREE('D0'==ABSID,M)<%s )" \
                           "& (M-MAXTREE('D0'==ABSID,M)>%s )" \
                           % (self.getProp('DeltaM_MAX'), self.getProp('DeltaM_MIN'))
                           
        DeltaMWideMassCut = "(M-MAXTREE('D0'==ABSID,M)<%s )" \
                            "& (M-MAXTREE('D0'==ABSID,M)>%s )" \
                            % (self.getProp('DeltaM_MAXwide'), self.getProp('DeltaM_MINwide'))


        #Prepare the slow pion to make D* later
        pionsForDstar = self.__SlowPionFilter( name = self.getProp('name_prefix')+'SlowPion'
                                               , inputContainers = [  BiKalmanFittedPions ]
                                             )


        # D0->pi+pi-pi+pi- ###################################################################################

        modeName4pi =  modeName + '_4pi'
        wideMassName4pi = modeName4pi + 'WideMass'
        #
        Charm4BodyCombine4pi = self.__4BodyCombine ( name = modeName4pi
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)0 pi+ pi-"]
                                                )   
        #
        Charm4BodyCombineToS4pi = self.__filterHlt1TOS( modeName4pi, Charm4BodyCombine4pi )
        #
        Charm4BodyMassWin4pi = self.__4BodyFilter ( name = modeName4pi+'MassWin', inputSeq = [Charm4BodyCombineToS4pi], extracode = sigMassCut )
        
        Charm4BodyWideMassWin4pi = self.__4BodyFilter (name = wideMassName4pi+'Win', inputSeq = [Charm4BodyCombineToS4pi], extracode = wideMassCut )        
        #                   
        Hlt2Charm4Body4pi = self.__DstarCombine( name = modeName4pi
                                              , inputSeq = [ Charm4BodyMassWin4pi , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMass4pi = self.__DstarCombine( name = wideMassName4pi
                                                      , inputSeq = [ Charm4BodyWideMassWin4pi , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################

        # D -> K-pi+pi+pi- / K+pi-pi-pi+ ########################################################################

        modeNameKm3pi =  modeName + '_K3pi'
        wideMassNameKm3pi = modeNameKm3pi + 'WideMass'
        #
        Charm4BodyCombineKm3pi = self.__4BodyCombine ( name = modeNameKm3pi
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)0 K- pi+","D0 -> K*(892)0 K+ pi-"]
                                                )
        
        #
        Charm4BodyCombineToSKm3pi = self.__filterHlt1TOS( modeNameKm3pi, Charm4BodyCombineKm3pi )
        #
        Charm4BodyMassWinKm3pi = self.__4BodyFilter ( name = modeNameKm3pi+'MassWin', inputSeq = [Charm4BodyCombineToSKm3pi], extracode = sigMassCut )
        
        Charm4BodyWideMassWinKm3pi = self.__4BodyFilter (name = wideMassNameKm3pi+'Win', inputSeq = [Charm4BodyCombineToSKm3pi], extracode = wideMassCut )        
        #                   
        Hlt2Charm4BodyKm3pi = self.__DstarCombine( name = modeNameKm3pi
                                              , inputSeq = [ Charm4BodyMassWinKm3pi , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMassKm3pi = self.__DstarCombine( name = wideMassNameKm3pi
                                                      , inputSeq = [ Charm4BodyWideMassWinKm3pi , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################


        # D -> K+K-pi+pi-  #########################################################################################

        modeNameKKpipi =  modeName + '_KKpipi'
        wideMassNameKKpipi = modeNameKKpipi + 'WideMass'
        #
        Charm4BodyCombineKKpipi = self.__4BodyCombine ( name = modeNameKKpipi
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)0 K+ K-"]
                                                )   
        #
        Charm4BodyCombineToSKKpipi = self.__filterHlt1TOS( modeNameKKpipi, Charm4BodyCombineKKpipi )
        #
        Charm4BodyMassWinKKpipi = self.__4BodyFilter ( name = modeNameKKpipi+'MassWin', inputSeq = [Charm4BodyCombineToSKKpipi], extracode = sigMassCut )
        
        Charm4BodyWideMassWinKKpipi = self.__4BodyFilter (name = wideMassNameKKpipi+'Win', inputSeq = [Charm4BodyCombineToSKKpipi], extracode = wideMassCut )        
        #                   
        Hlt2Charm4BodyKKpipi = self.__DstarCombine( name = modeNameKKpipi
                                              , inputSeq = [ Charm4BodyMassWinKKpipi , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMassKKpipi = self.__DstarCombine( name = wideMassNameKKpipi
                                                      , inputSeq = [ Charm4BodyWideMassWinKKpipi , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################


        # D -> K+K+pi-pi- / K-K-pi+pi+ #############################################################################

        modeName2Kp2Pm =  modeName + '_2K2pi'
        wideMassName2Kp2Pm = modeName2Kp2Pm + 'WideMass'
        #
        Charm4BodyCombine2Kp2Pm = self.__4BodyCombine ( name = modeName2Kp2Pm
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)+ pi- pi-","D0 -> K*(892)- pi+ pi+"]
                                                )   
        #
        Charm4BodyCombineToS2Kp2Pm = self.__filterHlt1TOS( modeName2Kp2Pm, Charm4BodyCombine2Kp2Pm )
        #
        Charm4BodyMassWin2Kp2Pm = self.__4BodyFilter ( name = modeName2Kp2Pm+'MassWin', inputSeq = [Charm4BodyCombineToS2Kp2Pm], extracode = sigMassCut )
        
        Charm4BodyWideMassWin2Kp2Pm = self.__4BodyFilter (name = wideMassName2Kp2Pm+'Win', inputSeq = [Charm4BodyCombineToS2Kp2Pm], extracode = wideMassCut )        
        #                   
        Hlt2Charm4Body2Kp2Pm = self.__DstarCombine( name = modeName2Kp2Pm
                                              , inputSeq = [ Charm4BodyMassWin2Kp2Pm , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMass2Kp2Pm = self.__DstarCombine( name = wideMassName2Kp2Pm
                                                      , inputSeq = [ Charm4BodyWideMassWin2Kp2Pm , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################

        # D -> K+K+K-pi- / K-K-K+pi+ ###############################################################################

        modeName3KPm =  modeName + '_3Kpi'
        wideMassName3KPm = modeName3KPm + 'WideMass'
        #
        Charm4BodyCombine3KPm = self.__4BodyCombine ( name = modeName3KPm
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)+ pi- K-","D0 -> K*(892)- pi+ K+"]
                                                )   
        #
        Charm4BodyCombineToS3KPm = self.__filterHlt1TOS( modeName3KPm, Charm4BodyCombine3KPm )
        #
        Charm4BodyMassWin3KPm = self.__4BodyFilter ( name = modeName3KPm+'MassWin', inputSeq = [Charm4BodyCombineToS3KPm], extracode = sigMassCut )
        
        Charm4BodyWideMassWin3KPm = self.__4BodyFilter (name = wideMassName3KPm+'Win', inputSeq = [Charm4BodyCombineToS3KPm], extracode = wideMassCut )        
        #                   
        Hlt2Charm4Body3KPm = self.__DstarCombine( name = modeName3KPm
                                              , inputSeq = [ Charm4BodyMassWin3KPm , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMass3KPm = self.__DstarCombine( name = wideMassName3KPm
                                                      , inputSeq = [ Charm4BodyWideMassWin3KPm , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################


        # D -> K+pi+pi+pi- / K-pi-pi-pi+ ###############################################################################

        modeNameCh2 =  modeName + '_Ch2'
        wideMassNameCh2 = modeNameCh2 + 'WideMass'
        #
        Charm4BodyCombineCh2 = self.__4BodyCombine ( name = modeNameCh2
                                                  , inputSeq = [ Hlt2Charm2BodyFor4Body, kaonsF, pionsF ]
                                                  , decayDesc = [ "D0 -> K*(892)0 pi- K-","D0 -> K*(892)0 pi+ K+"]
                                                )   
        #
        Charm4BodyCombineToSCh2 = self.__filterHlt1TOS( modeNameCh2, Charm4BodyCombineCh2 )
        #
        Charm4BodyMassWinCh2 = self.__4BodyFilter ( name = modeNameCh2+'MassWin', inputSeq = [Charm4BodyCombineToSCh2], extracode = sigMassCut )
        
        Charm4BodyWideMassWinCh2 = self.__4BodyFilter (name = wideMassNameCh2+'Win', inputSeq = [Charm4BodyCombineToSCh2], extracode = wideMassCut )        
        #                   
        Hlt2Charm4BodyCh2 = self.__DstarCombine( name = modeNameCh2
                                              , inputSeq = [ Charm4BodyMassWinCh2 , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2Charm4BodyWideMassCh2 = self.__DstarCombine( name = wideMassNameCh2
                                                      , inputSeq = [ Charm4BodyWideMassWinCh2 , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ############################################################################################################


        
        
        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        twoBodyName = self.getProp('name_prefix') + '2Body'
        line = Hlt2Line(twoBodyName, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + twoBodyName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )



        #
        # Make the 4Body lines 
        #
        
        # D0->pi+pi-pi+pi- ###################################################################################
        line = Hlt2Line(modeName4pi, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4Body4pi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName4pi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName4pi, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMass4pi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName4pi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################

        
        # D0-> K-pi+pi+pi- / K+pi-pi-pi+  ##########################################################################
        line = Hlt2Line(modeNameKm3pi, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyKm3pi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameKm3pi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameKm3pi, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMassKm3pi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameKm3pi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################

        
        # D0->K+K-pi+pi-  ###################################################################################
        line = Hlt2Line(modeNameKKpipi, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyKKpipi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameKKpipi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameKKpipi, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMassKKpipi]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameKKpipi + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################


        # D0->K+K+pi-pi-  / K-K-pi+pi+ #############################################################################
        line = Hlt2Line(modeName2Kp2Pm, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4Body2Kp2Pm]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName2Kp2Pm + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName2Kp2Pm, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMass2Kp2Pm]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName2Kp2Pm + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################


        # D0->K+K+K-pi- / K-K-K+pi+   ##############################################################################
        line = Hlt2Line(modeName3KPm, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4Body3KPm]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName3KPm + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName3KPm, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMass3KPm]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName3KPm + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################


        # D -> K+pi+pi+pi- / K-pi-pi-pi+   ##############################################################################
        line = Hlt2Line(modeNameCh2, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyCh2]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameCh2 + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameCh2, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, kaonsF, pionsF, Hlt2Charm4BodyWideMassCh2]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameCh2 + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################

