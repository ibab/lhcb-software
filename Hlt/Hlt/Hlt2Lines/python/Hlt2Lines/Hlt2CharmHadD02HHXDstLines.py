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
                  , 'VCHI2_D0_MAX_V0'             : 20.
                  , 'M_MAX'                       : 2500. 
                  , 'DeltaM_MIN'                  : 0.0 * MeV
                  , 'DeltaM_MAX'                  : 285.0 * MeV
                  , 'DeltaM_MINwide'              : 0.0 * MeV
                  , 'DeltaM_MAXwide'              : 570.0 * MeV
                  ## GEC
                  , 'GEC_Filter_NTRACK'        : True       # do or do not
                  , 'GEC_NTRACK_MAX'           : 180        # max number of tracks
                  , 'TisTosParticleTaggerSpecs': { "Hlt1Track.*Decision%TOS":0 }
                  , 'name_prefix'              : 'CharmHadD02HHXDst'
                  # prescales
                  , 'Prescale'                  : {
                          'Hlt2CharmHadD02HHXDst_hhXWideMass'                       : 0.05
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWideMass'                 : 0.05
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSLLWideMass'         : 0.05
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0LLWideMass'    : 0.05
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSDDWideMass'         : 0.05
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0DDWideMass'    : 0.05
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWideMass'                 : 0.05
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSLLWideMass'         : 0.05
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0LLWideMass'    : 0.05
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSDDWideMass'         : 0.05
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0DDWideMass'    : 0.05
                        }
                  , 'HltANNSvcID'  : {
                          'Hlt2CharmHadD02HHXDst_hhXDecision'                               : 62318
                        , 'Hlt2CharmHadD02HHXDst_hhXWideMassDecision'                       : 62319
                        #
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXDecision'                         : 62320
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWideMassDecision'                 : 62321
                        #
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSLLDecision'                 : 62322
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSLLWideMassDecision'         : 62323
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0LLDecision'            : 62324
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0LLWideMassDecision'    : 62325
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSDDDecision'                 : 62326
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithKSDDWideMassDecision'         : 62327
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0DDDecision'            : 62328
                        , 'Hlt2CharmHadD02HHXDst_BaryonhhXWithLambda0DDWideMassDecision'    : 62329
                        #
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXDecision'                         : 62330
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWideMassDecision'                 : 62331
                        #
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSLLDecision'                 : 62332
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSLLWideMassDecision'         : 62333
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0LLDecision'            : 62334
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0LLWideMassDecision'    : 62335
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSDDDecision'                 : 62336
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithKSDDWideMassDecision'         : 62337
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0DDDecision'            : 62338
                        , 'Hlt2CharmHadD02HHXDst_LeptonhhXWithLambda0DDWideMassDecision'    : 62339 
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
                                , 'Hlt1TOSFilter_'+name
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

        kstchildcut = "   (MAXTREE((('pi+'==ABSID)|('K+'==ABSID)|('mu+'==ABSID)|('p+'==ABSID)),TRCHI2DOF) < %(TrkChi2_MAX_Child_MAX)s)" \
                      " & (MAXTREE((('pi+'==ABSID)|('K+'==ABSID)|('mu+'==ABSID)|('p+'==ABSID)),MIPCHI2DV(PRIMARY)) > %(IPCHI2_MAX_Child_MIN)s)" \
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
                                   , "Combine_Dstar_"+name
                                   , DecayDescriptors = decayDesc
                                   , Inputs = inputSeq 
                                   , DaughtersCuts = childcuts 
                                   , CombinationCut = combcuts
                                   , MotherCut = mothercuts
                                   )
        return bindMembers(name, [PV3D()] + inputSeq + [combineDstar])

    def __DstarCombine_V0(self, name, inputSeq, decayDesc, masscut) :
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        kstchildcut = "   (INGENERATION((('pi+'==ABSID)|('K+'==ABSID)|('mu+'==ABSID)|('p+'==ABSID))&\
                                        (TRCHI2DOF < %(TrkChi2_MAX_Child_MAX)s),1))" \
                      " & (INGENERATION((('pi+'==ABSID)|('K+'==ABSID)|('mu+'==ABSID)|('p+'==ABSID))&\
                                        (MIPCHI2DV(PRIMARY) > %(IPCHI2_MAX_Child_MIN)s),1))" \
                      " & (BPVVDCHI2 > %(FDCHI2_D0_MIN)s)"\
                      " & (BPVDIRA > %(DIRA_D0_MIN)s)" \
                      " & (M < %(M_MAX)s)" \
                      " & (VFASPF(VCHI2PDOF) < %(VCHI2_D0_MAX_V0)s)" % self.getProps()
        slowpioncut = " ( MIPCHI2DV(PRIMARY) < %(IPCHI2_PiSlow_MAX)s)" % self.getProps()
        childcuts = {"K*(892)0" : kstchildcut ,
                     "pi+"      : slowpioncut }
                                
        combcuts = "AM < 5000.0*MeV"
        mothercuts = "(PT > %(PT_Dstar_MIN)s) &" % self.getProps()
        mothercuts += masscut
        
        combineDstar = Hlt2Member( CombineParticles
                                   , "Combine_Dstar_"+name
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
        Hlt2CharmBaryon2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyBaryonHHXSequence()
        Hlt2CharmBaryonWithKSLL2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyBaryonHHXWithKSLLSequence()
        Hlt2CharmBaryonWithKSDD2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyBaryonHHXWithKSDDSequence()
        Hlt2CharmBaryonWithLambda0LL2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyBaryonHHXWithLambda0LLSequence()
        Hlt2CharmBaryonWithLambda0DD2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyBaryonHHXWithLambda0DDSequence()
        Hlt2CharmLepton2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyLeptonHHXSequence()       
        Hlt2CharmLeptonWithKSLL2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyLeptonHHXWithKSLLSequence()
        Hlt2CharmLeptonWithKSDD2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyLeptonHHXWithKSDDSequence()
        Hlt2CharmLeptonWithLambda0LL2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyLeptonHHXWithLambda0LLSequence()
        Hlt2CharmLeptonWithLambda0DD2BodyFor4Body =  Hlt2CharmHadTwoBodyForD02HHHHConf().twoBodyLeptonHHXWithLambda0DDSequence()
        
        #
        # Make 4Body : that's where differentiation between modes starts...
        #

        modeNameBase = self.getProp('name_prefix')

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

        modeName =  modeNameBase + '_hhX'
        wideMassName = modeName + 'WideMass'
        CharmHHXCombineToS = self.__filterHlt1TOS( modeName, Hlt2Charm2BodyFor4Body )
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
        #
        modeNameBaryon =  modeNameBase + '_BaryonhhX'
        wideMassNameBaryon = modeNameBaryon + 'WideMass'   
        CharmBaryonHHXCombineToS = self.__filterHlt1TOS( modeNameBaryon, Hlt2CharmBaryon2BodyFor4Body )
        Hlt2CharmBaryonHHX = self.__DstarCombine( name = modeNameBaryon
                                              , inputSeq = [ CharmBaryonHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
     
        Hlt2CharmBaryonHHXWideMass = self.__DstarCombine( name = wideMassNameBaryon
                                                      , inputSeq = [ CharmBaryonHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #
        modeNameBaryonWithKSLL =  modeNameBase + '_BaryonhhXWithKSLL'
        wideMassNameBaryonWithKSLL = modeNameBaryonWithKSLL + 'WideMass'   
        CharmBaryonWithKSLLHHXCombineToS = self.__filterHlt1TOS( modeNameBaryonWithKSLL, Hlt2CharmBaryonWithKSLL2BodyFor4Body )
        Hlt2CharmBaryonWithKSLLHHX = self.__DstarCombine_V0( name = modeNameBaryonWithKSLL
                                              , inputSeq = [ CharmBaryonWithKSLLHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
            
        Hlt2CharmBaryonWithKSLLHHXWideMass = self.__DstarCombine_V0( name = wideMassNameBaryonWithKSLL
                                                      , inputSeq = [ CharmBaryonWithKSLLHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #   
        modeNameBaryonWithLambda0LL =  modeNameBase + '_BaryonhhXWithLambda0LL'
        wideMassNameBaryonWithLambda0LL = modeNameBaryonWithLambda0LL + 'WideMass'   
        CharmBaryonWithLambda0LLHHXCombineToS = self.__filterHlt1TOS( modeNameBaryonWithLambda0LL, Hlt2CharmBaryonWithLambda0LL2BodyFor4Body )
        Hlt2CharmBaryonWithLambda0LLHHX = self.__DstarCombine_V0( name = modeNameBaryonWithLambda0LL
                                              , inputSeq = [ CharmBaryonWithLambda0LLHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
                                                
        Hlt2CharmBaryonWithLambda0LLHHXWideMass = self.__DstarCombine_V0( name = wideMassNameBaryonWithLambda0LL
                                                      , inputSeq = [ CharmBaryonWithLambda0LLHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #
        modeNameBaryonWithKSDD =  modeNameBase + '_BaryonhhXWithKSDD'
        wideMassNameBaryonWithKSDD = modeNameBaryonWithKSDD + 'WideMass'
        CharmBaryonWithKSDDHHXCombineToS = self.__filterHlt1TOS( modeNameBaryonWithKSDD, Hlt2CharmBaryonWithKSDD2BodyFor4Body )
        Hlt2CharmBaryonWithKSDDHHX = self.__DstarCombine_V0( name = modeNameBaryonWithKSDD
                                              , inputSeq = [ CharmBaryonWithKSDDHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )

        Hlt2CharmBaryonWithKSDDHHXWideMass = self.__DstarCombine_V0( name = wideMassNameBaryonWithKSDD
                                                      , inputSeq = [ CharmBaryonWithKSDDHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #   
        modeNameBaryonWithLambda0DD =  modeNameBase + '_BaryonhhXWithLambda0DD'
        wideMassNameBaryonWithLambda0DD = modeNameBaryonWithLambda0DD + 'WideMass'   
        CharmBaryonWithLambda0DDHHXCombineToS = self.__filterHlt1TOS( modeNameBaryonWithLambda0DD, Hlt2CharmBaryonWithLambda0DD2BodyFor4Body )
        Hlt2CharmBaryonWithLambda0DDHHX = self.__DstarCombine_V0( name = modeNameBaryonWithLambda0DD
                                              , inputSeq = [ CharmBaryonWithLambda0DDHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
                                                
        Hlt2CharmBaryonWithLambda0DDHHXWideMass = self.__DstarCombine_V0( name = wideMassNameBaryonWithLambda0DD
                                                      , inputSeq = [ CharmBaryonWithLambda0DDHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    ) 
        #   
        modeNameLepton =  modeNameBase + '_LeptonhhX'
        wideMassNameLepton = modeNameLepton + 'WideMass'
        CharmLeptonHHXCombineToS = self.__filterHlt1TOS( modeNameLepton, Hlt2CharmLepton2BodyFor4Body )
        Hlt2CharmLeptonHHX = self.__DstarCombine( name = modeNameLepton
                                              , inputSeq = [ CharmLeptonHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
     
        Hlt2CharmLeptonHHXWideMass = self.__DstarCombine( name = wideMassNameLepton
                                                      , inputSeq = [ CharmLeptonHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #
        modeNameLeptonWithKSLL =  modeNameBase + '_LeptonhhXWithKSLL'
        wideMassNameLeptonWithKSLL = modeNameLeptonWithKSLL + 'WideMass'   
        CharmLeptonWithKSLLHHXCombineToS = self.__filterHlt1TOS( modeNameLeptonWithKSLL, Hlt2CharmLeptonWithKSLL2BodyFor4Body )
        Hlt2CharmLeptonWithKSLLHHX = self.__DstarCombine_V0( name = modeNameLeptonWithKSLL
                                              , inputSeq = [ CharmLeptonWithKSLLHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
            
        Hlt2CharmLeptonWithKSLLHHXWideMass = self.__DstarCombine_V0( name = wideMassNameLeptonWithKSLL
                                                      , inputSeq = [ CharmLeptonWithKSLLHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #   
        modeNameLeptonWithLambda0LL =  modeNameBase + '_LeptonhhXWithLambda0LL'
        wideMassNameLeptonWithLambda0LL = modeNameLeptonWithLambda0LL + 'WideMass'   
        CharmLeptonWithLambda0LLHHXCombineToS = self.__filterHlt1TOS( modeNameLeptonWithLambda0LL, Hlt2CharmLeptonWithLambda0LL2BodyFor4Body )
        Hlt2CharmLeptonWithLambda0LLHHX = self.__DstarCombine_V0( name = modeNameLeptonWithLambda0LL
                                              , inputSeq = [ CharmLeptonWithLambda0LLHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )   
                                                
        Hlt2CharmLeptonWithLambda0LLHHXWideMass = self.__DstarCombine_V0( name = wideMassNameLeptonWithLambda0LL
                                                      , inputSeq = [ CharmLeptonWithLambda0LLHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #
        modeNameLeptonWithKSDD =  modeNameBase + '_LeptonhhXWithKSDD'
        wideMassNameLeptonWithKSDD = modeNameLeptonWithKSDD + 'WideMass'
        CharmLeptonWithKSDDHHXCombineToS = self.__filterHlt1TOS( modeNameLeptonWithKSDD, Hlt2CharmLeptonWithKSDD2BodyFor4Body )
        Hlt2CharmLeptonWithKSDDHHX = self.__DstarCombine_V0( name = modeNameLeptonWithKSDD
                                              , inputSeq = [ CharmLeptonWithKSDDHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )

        Hlt2CharmLeptonWithKSDDHHXWideMass = self.__DstarCombine_V0( name = wideMassNameLeptonWithKSDD
                                                      , inputSeq = [ CharmLeptonWithKSDDHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    )
        #   
        modeNameLeptonWithLambda0DD =  modeNameBase + '_LeptonhhXWithLambda0DD'
        wideMassNameLeptonWithLambda0DD = modeNameLeptonWithLambda0DD + 'WideMass'   
        CharmLeptonWithLambda0DDHHXCombineToS = self.__filterHlt1TOS( modeNameLeptonWithLambda0DD, Hlt2CharmLeptonWithLambda0DD2BodyFor4Body )
        Hlt2CharmLeptonWithLambda0DDHHX = self.__DstarCombine_V0( name = modeNameLeptonWithLambda0DD
                                              , inputSeq = [ CharmLeptonWithLambda0DDHHXCombineToS , pionsForDstar ]
                                              , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                              , masscut = DeltaMSigMassCut
                                            )
                                                
        Hlt2CharmLeptonWithLambda0DDHHXWideMass = self.__DstarCombine_V0( name = wideMassNameLeptonWithLambda0DD
                                                      , inputSeq = [ CharmLeptonWithLambda0DDHHXCombineToS , pionsForDstar ]
                                                      , decayDesc = [ "D*(2010)+ -> K*(892)0 pi+", "D*(2010)- -> K*(892)0 pi-" ]
                                                      , masscut = DeltaMWideMassCut
                                                    ) 
        ###########################################################################
        # Define the Hlt2 Lines
        #
        # Note: for the 2-loop approach you just need to explicitly add the second loop pions inbetween the two stages above 
        ##########################################################################
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()

        ############################################################################################################

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

        line = Hlt2Line(modeNameBaryon, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameBaryon + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameBaryon, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonHHXWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameBaryon + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } ) 

        line = Hlt2Line(modeNameBaryonWithKSLL, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithKSLLHHX]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + modeNameBaryonWithKSLL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } ) 

        line = Hlt2Line(wideMassNameBaryonWithKSLL, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithKSLLHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameBaryonWithKSLL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )    

        line = Hlt2Line(modeNameBaryonWithLambda0LL, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithLambda0LLHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameBaryonWithLambda0LL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
                        
        line = Hlt2Line(wideMassNameBaryonWithLambda0LL, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithLambda0LLHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameBaryonWithLambda0LL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeNameBaryonWithKSDD, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithKSDDHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameBaryonWithKSDD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
                        
        line = Hlt2Line(wideMassNameBaryonWithKSDD, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithKSDDHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameBaryonWithKSDD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeNameBaryonWithLambda0DD, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithLambda0DDHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameBaryonWithLambda0DD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameBaryonWithLambda0DD, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmBaryonWithLambda0DDHHXWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameBaryonWithLambda0DD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        ############################################################################################################

        line = Hlt2Line(modeNameLepton, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameLepton + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameLepton, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonHHXWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameLepton + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeNameLeptonWithKSLL, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithKSLLHHX]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + modeNameLeptonWithKSLL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } ) 

        line = Hlt2Line(wideMassNameLeptonWithKSLL, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithKSLLHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameLeptonWithKSLL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )    

        line = Hlt2Line(modeNameLeptonWithLambda0LL, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithLambda0LLHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameLeptonWithLambda0LL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
                        
        line = Hlt2Line(wideMassNameLeptonWithLambda0LL, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithLambda0LLHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameLeptonWithLambda0LL + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeNameLeptonWithKSDD, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithKSDDHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameLeptonWithKSDD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
                        
        line = Hlt2Line(wideMassNameLeptonWithKSDD, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithKSDDHHXWideMass]
                        , postscale = self.postscale
                        )   
        decName = 'Hlt2' + wideMassNameLeptonWithKSDD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(modeNameLeptonWithLambda0DD, prescale = self.prescale
                        , algos = [ PV3D(), Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithLambda0DDHHX]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + modeNameLeptonWithLambda0DD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )

        line = Hlt2Line(wideMassNameLeptonWithLambda0DD, prescale = self.prescale
                        , algos = [ PV3D(),Hlt2CharmKillTooManyInTrk,  Hlt2Charm2BodyFor4Body, Hlt2CharmLeptonWithLambda0DDHHXWideMass]
                        , postscale = self.postscale
                        )
        decName = 'Hlt2' + wideMassNameLeptonWithLambda0DD + 'Decision'
        annSvcID = self._scale(decName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { decName : annSvcID } )
        ############################################################################################################
