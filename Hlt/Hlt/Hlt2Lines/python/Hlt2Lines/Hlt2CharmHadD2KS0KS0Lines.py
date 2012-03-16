from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmHadD2KS0KS0LinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                 ## KS0KS0
                    'KS0_PT_MIN'                :  750 * MeV
                  , 'KS0_P_MIN'                 : 5000 * MeV
                  , 'KS0_MIPCHI2DV_MIN'         :    9.0      # unitless
                  #, 'Daughter2IPChi2'           :   10.0     # unitless Added by Feng ZHANG
                  , 'D_BPVVDCHI2_MIN'           :  36.0    # unitless
                  , 'D_VCHI2PDOF_MAX'           :   20.0     # unitless
                  , 'D_BPVIPCHI2_MAX'           :   30.0     # unitless
                  , 'D_DIRA_MIN'                : 0.99985 
                  , 'D_ASUMPT_MIN'              : 2500.0 * MeV
                  #, 'DOCAChi2'                  :   50.0     # Maximum Chi2 Between two daughters BY Feng ZHANG
                  , 'GEC_Filter_NTRACK'         : True     # do or do not
                  , 'GEC_NTRACK_MAX'            : 180      # max number of tracks
                  , 'name_prefix'              : 'CharmHadD2KS0KS0'
                  # prescales
                  , 'Prescale'                  : {
                        'Hlt2CharmHadD2KS0KS0WideMass'    : 0.1
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHadD2KS0KS0Decision'         : 50925
                        , 'Hlt2CharmHadD2KS0KS0WideMassDecision' : 50926
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


    def __KS0KS0Combine(self, name, inputSeq, decayDesc) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        _daughters_cut = "(PT> %(KS0_PT_MIN)s )" \
                         "& (P> %(KS0_P_MIN)s )" \
                         "& (MIPCHI2DV(PRIMARY)> %(KS0_MIPCHI2DV_MIN)s )" % self.getProps()

        combcuts = "(AM<2100*MeV)" \
                   "& ((APT1+APT2) > %(D_ASUMPT_MIN)s) " % self.getProps()
        mothercuts = "(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > %(D_DIRA_MIN)s)"\
                     "& (BPVIPCHI2() < %(D_BPVIPCHI2_MAX)s)" % self.getProps()

        combineCharmKS0KS0 = Hlt2Member( CombineParticles
                          , "Combine_Stage2"
                          , DecayDescriptors = decayDesc
                          , Inputs = inputSeq 
                          , DaughtersCuts = { 'KS0' : _daughters_cut }
                          , CombinationCut = combcuts
                          , MotherCut = mothercuts
                          )

        return bindMembers(name, [PV3D()] + inputSeq + [combineCharmKS0KS0])

    def __KS0KS0Filter(self, name, inputSeq, extracode = None) :

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
        from HltTracking.HltPVs import PV3D
        from Hlt2SharedParticles.Ks import KsLLTF
        
        # Make KS0KS0 
        CharmKS0KS0Combine = self.__KS0KS0Combine (  name = self.getProp('name_prefix')
                                                  , inputSeq = [KsLLTF  ]
                                                  , decayDesc = ["D0 -> KS0 KS0"]

                                                 )   

        # KS0KS0 line
        modeName = self.getProp('name_prefix')
        wideMassName = modeName + 'WideMass'
        Hlt2CharmKS0KS0 = self.__KS0KS0Filter ( name = modeName, inputSeq = [CharmKS0KS0Combine], extracode = "in_range(1800*MeV, M, 2040*MeV)")
        # KS0KS0 WideMass line - with prescale
        Hlt2CharmKS0KS0WideMass = self.__KS0KS0Filter (name = wideMassName, inputSeq = [CharmKS0KS0Combine], extracode = "in_range(1700*MeV, M, 2100*MeV)")

        ###########################################################################
        # Define the Hlt2 Lines
        #
        ##########################################################################

        line = Hlt2Line(modeName, prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), KsLLTF, Hlt2CharmKS0KS0]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName , prescale = self.prescale
                        , algos = [ PV3D(), self.__seqGEC(), KsLLTF, Hlt2CharmKS0KS0WideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
