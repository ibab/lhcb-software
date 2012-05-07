from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmHadD2HHHLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## 3Body
                    'Trk_PT_MIN'                :  300 * MeV
                  , 'Trk_P_MIN'                 : 2000 * MeV
                  , 'Trk_MIPCHI2DV_MIN'         :    6.0      # unitless
                  #, 'Daughter2IPChi2'           :   10.0     # unitless Added by Feng ZHANG
                  , 'Trk_TRCHI2DOF_MAX'         :    3.      # unitless
                  , 'Pair_AMINDOCA_MAX'         :    0.08 * mm
                  , 'D_BPVVDCHI2_MIN'           :  175.0    # unitless
                  , 'D_VCHI2PDOF_MAX'           :   15.0     # unitless
                  , 'D_BPVIPCHI2_MAX'           :   15.0     # unitless
                  , 'D_ASUMPT_MIN'              : 2800.0 * MeV
                  #, 'DOCAChi2'                  :   50.0     # Maximum Chi2 Between two daughters BY Feng ZHANG
                  , 'GEC_Filter_NTRACK'         : True     # do or do not
                  , 'GEC_NTRACK_MAX'            : 180      # max number of tracks
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD2HHH'
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD2HHHWideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHadD2HHHDecision'         : 50911
                        , 'Hlt2CharmHadD2HHHWideMassDecision' : 50912
                        }
                }

    def __seqGEC(self) : 
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
        if self.getProp('GEC_Filter_NTRACK') : 
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(GEC_NTRACK_MAX)s" % self.getProps()

        filtName = 'Hlt2' + self.getProp('name_prefix')  + 'KillTooManyInTrkAlg'
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter(filtName
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk


    def __3BodyCombine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        _daughters_cut = "(PT> %(Trk_PT_MIN)s )" \
                         "& (TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                         "& (P> %(Trk_P_MIN)s )" \
                         "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" % self.getProps()

        combcuts = "(AM<2100*MeV)" \
                   "& ((APT1+APT2+APT3) > %(D_ASUMPT_MIN)s) " \
                   "& (AMINDOCA('LoKi::TrgDistanceCalculator') < %(Pair_AMINDOCA_MAX)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = "(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D_BPVVDCHI2_MIN)s )" \
                     "& (BPVIPCHI2() < %(D_BPVIPCHI2_MAX)s)" % self.getProps()

        combineCharm3Body = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , DaughtersCuts = { 'pi+' : _daughters_cut, 'K+' : _daughters_cut }
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )

        return bindMembers(name, [PV3D()] + inputSeq + [combineCharm3Body])

    def __3BodyFilter(self, name, inputSeq, extracode = None) :

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


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Line
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import HltANNSvc
        from Configurables import CombineParticles
        from Configurables import FilterDesktop
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions,BiKalmanFittedSecondLoopPions, BiKalmanFittedKaons, BiKalmanFittedSecondLoopKaons
        from HltTracking.HltPVs import PV3D
        
        # Make 3Body 
        Charm3BodyCombine = self.__3BodyCombine (  name = self.getProp('name_prefix')
                                                  , inputSeq = [ BiKalmanFittedPions, BiKalmanFittedKaons ]
                                                  , decayDesc = ["[D+ -> pi+ pi+ K-]cc", "[D+ -> pi+ K+ K-]cc", "[D+ -> K+ K+ K-]cc", "[D+ -> K+ K+ pi-]cc" , "[D+ -> pi+ pi+ pi-]cc", "[D+ -> pi+ pi- K+]cc"]

                                                 )   

        tosName = self.getProp('name_prefix')
        Charm3BodyCombineToS =  self.__filterHlt1TOS( tosName, Charm3BodyCombine )

        # 3Body line
        modeName = self.getProp('name_prefix')
        wideMassName = modeName + 'WideMass'
        Hlt2Charm3Body = self.__3BodyFilter ( name = modeName, inputSeq = [Charm3BodyCombineToS], extracode = "in_range(1800*MeV, M, 2040*MeV)")
        # 3Body WideMass line - with prescale
        Hlt2Charm3BodyWideMass = self.__3BodyFilter (name = wideMassName, inputSeq = [Charm3BodyCombineToS], extracode = "in_range(1700*MeV, M, 2100*MeV)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        ##########################################################################

        line = Hlt2Line(modeName, prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), BiKalmanFittedPions, BiKalmanFittedKaons, Hlt2Charm3Body]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName , prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), BiKalmanFittedPions, BiKalmanFittedKaons, Hlt2Charm3BodyWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
