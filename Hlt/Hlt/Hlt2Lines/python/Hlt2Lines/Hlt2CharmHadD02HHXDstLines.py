__author__  = [ 'Vladimir Gligorov' ]
__date__    = '$Date: 23/04/2012$'
__version__ = '$Revision: 0$'

from Gaudi.Configuration import *
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from GaudiKernel.SystemOfUnits import MeV, GeV, mm

class Hlt2CharmHadD02HHXDstLinesConf(HltLinesConfigurableUser) :
    __slots__ = {
                  ## D* combination
                  'TrkPt_SlowPion'              : 300.0 * MeV 
                  , 'TrkP_SlowPion'               : 3000.0 * MeV
                  , 'TrkChi2_SlowPion'            : 2.25
                  , 'TrkChi2_MAX_Child_MAX'       : 2.25
                  , 'IPCHI2_MAX_Child_MIN'        : 36.
                  , 'IPCHI2_PiSlow_MAX'           : 9.      
                  , 'PairMaxDoca_Dstar'           : 100.0 * mm
                  , 'PT_Dstar_MIN'                : 3750.0 * MeV  
                  , 'DIRA_D0_MIN'                 : 0.99
                  , 'FDCHI2_D0_MIN'               : 100.
                  , 'VCHI2_D0_MAX'                : 10. 
                  , 'M_MAX'                       : 1900. 
                  , 'DeltaM_MIN'                  : 0.0 * MeV
                  , 'DeltaM_MAX'                  : 250.0 * MeV
                  , 'DeltaM_MINwide'              : 0.0 * MeV
                  , 'DeltaM_MAXwide'              : 500.0 * MeV
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : True       # do or do not
                  , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD02HHXDst'
                  # prescales
                  , 'Prescale'                  : {
                          'Hlt2CharmHadD02HHXDst_hhXWideMass' : 0.05
                        }
                  , 'HltANNSvcID'  : {
                         'Hlt2CharmHadD02HHXDst_hhXDecision'         : 62318
                        , 'Hlt2CharmHadD02HHXDst_hhXWideMassDecision' : 62319
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

        kstchildcut = "   (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)),TRCHI2DOF) < %(TrkChi2_MAX_Child_MAX)s)" \
                      " & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)),MIPCHI2DV(PRIMARY)) > %(IPCHI2_MAX_Child_MIN)s)" \
                      " & (BPVVDCHI2 > %(FDCHI2_D0_MIN)s)"\
                      " & (BPVDIRA > %(DIRA_D0_MIN)s)" \
                      " & (M < %(M_MAX)s)" \
                      " & (VFASPF(VCHI2PDOF) < %(VCHI2_D0_MAX)s)" % self.getProps() 
        slowpioncut = " ( MIPCHI2DV(PRIMARY) < %(IPCHI2_PiSlow_MAX)s)" % self.getProps()
        childcuts = {"K*(892)0" : kstchildcut ,
                     "pi+"      : slowpioncut }

        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator') < %(PairMaxDoca_Dstar)s)" \
                   "& (AALLSAMEBPV)" % self.getProps()
        mothercuts = "(PT > %(PT_Dstar_MIN)s) &" % self.getProps()
        mothercuts += masscut
        
        combineDstar = Hlt2Member( CombineParticles
                                   , "Combine_Dstar"
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq 
                                   , DaughtersCuts = childcuts 
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
        
        from Hlt2CharmHadTwoBodyForD02HHHH import Hlt2CharmHadTwoBodyForD02HHHHConf
        Hlt2Charm2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyHHXSequence()
        
        #
        # Make 4Body : that's where differentiation between modes starts...
        #

        modeName = self.getProp('name_prefix')

        # Prepare mass cuts
        DeltaMSigMassCut = "(M-MAXTREE('K*(892)0'==ABSID,M)<%s )" \
                           "& (M-MAXTREE('K*(892)0'==ABSID,M)>%s )" \
                           % (self.getProp('DeltaM_MAX'), self.getProp('DeltaM_MIN'))
                           
        DeltaMWideMassCut = "(M-MAXTREE('K*(892)0'==ABSID,M)<%s )" \
                            "& (M-MAXTREE('K*(892)0'==ABSID,M)>%s )" \
                            % (self.getProp('DeltaM_MAXwide'), self.getProp('DeltaM_MINwide'))


        #Prepare the slow pion to make D* later
        pionsForDstar = self.__SlowPionFilter( name = self.getProp('name_prefix')+'SlowPion'
                                               , inputContainers = [  BiKalmanFittedPions ]
                                             )


        # D0->hhX ###################################################################################

        modeName =  modeName + '_hhX'
        wideMassName = modeName + 'WideMass'
        #
        CharmHHXCombineToS = self.__filterHlt1TOS( modeName, Hlt2Charm2BodyFor4Body )
        #
        Hlt2CharmHHX = self.__DstarCombine( name = modeName
                                              , inputSeq = [ CharmHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
        
        Hlt2CharmHHXWideMass = self.__DstarCombine( name = wideMassName
                                                      , inputSeq = [ CharmHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )


        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        #
        # Make the 4Body lines 
        #
        
        # D0->pi+pi-pi+pi- ###################################################################################

        # 1) Tagged decays
        line = Hlt2Line(modeName, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassName, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmHHXWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassName + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################

