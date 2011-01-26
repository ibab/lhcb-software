__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-09-09 15:20:09 $'
__version__ = '$Revision: 1.22 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## decays of D0 to two final state hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from Configurables import HltANNSvc
from HltLine.HltLine import Hlt2Line


class Hlt2CharmHadD02HHLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = {
                ## Cut values for one stage track-fit (OSTF) lines.
                ## 2-body!
                  'OSTFAllTrkPtLL_2Body'          : 500.0      # in MeV
                , 'OSTFAllTrkPLL_2Body'           : 5000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL_2Body'    : 2.0        # unitless
                , 'OSTFAllTrkChi2UL_2Body'        : 5.0        # unitless
                , 'OSTFPairMinDocaUL_2Body'       : 0.10       # in mm
                , 'OSTFTrkMaxPtLL_2Body'          : 1500.0     # in MeV
                , 'OSTFVtxPVDispChi2LL_2Body'     : 25.0       # unitless
                , 'OSTFPointLL_2Body'             : 0.99985     # unitless
                , 'OSTFVtxChi2UL_2Body'           : 10.0       # unitless
                , 'OSTFDPtLL_2Body'               : 2000.0     # in MeV
                ## 2-body inclusive!
                , 'OSTFAllTrkPtLL_2BodyInc'          : 500.0      # in MeV
                , 'OSTFAllTrkPLL_2BodyInc'           : 5000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL_2BodyInc'    : 16.0        # unitless
                , 'OSTFAllTrkChi2UL_2BodyInc'        : 3.0        # unitless
                , 'OSTFPairMinDocaUL_2BodyInc'       : 0.15       # in mm
                , 'OSTFVtxPVDispChi2LL_2BodyInc'     : 100.0       # unitless
                , 'OSTFTrkMaxPtLL_2BodyInc'          : 1000.0     # in MeV
                , 'OSTFDPtLL_2BodyInc'               : 1500.0     # in MeV
                , 'OSTFDSumPtLL_2BodyInc'            : 1800.0     # in MeV
                , 'OSTFDIPChi2LL_2BodyInc'           : 2.0       # unitless
                , 'OSTFMCOR_MIN_2BodyInc'            : 300.        # MeV
                , 'OSTFMCOR_MAX_2BodyInc'            : 3200.        # MeV
                ## GEC
                , 'ComRobUseGEC'            : True       # do or do not
                , 'ComRobGEC'               : 120        # max number of tracks
                , 'Prescale'                : { 'Hlt2Charm2BodyWideMass' : 0.05 }
                , 'Postscale'               : { }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : { ## Main One stage lines
                                     'Hlt2Charm2BodyDecision' : 50283
                                   , 'Hlt2Charm2BodyIncDecision' : 50284 
                                   ## One stage wide mass lines
                                   , 'Hlt2Charm2BodyWideMassDecision' : 50285
                                   }
                  }



    def __updateHltANNSvc(self,line) : # {
        """
        Wrapper for updating the HltANNSvc after a new line has been
        constructed.  This should eventually become obsolete.
        """
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )
    # }


    def __makeLine(self, lineName, algos) : # {
        """
        Wrapper for line construction that also registers it to the HltANNSvc.
        """
        lclAlgos = []
        ## Prepend a filter on the number of tracks, if required.
        Hlt2CharmKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [ Hlt2CharmKillTooManyInTrk ]
        lclAlgos.extend(algos)

        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                       )
        self.__updateHltANNSvc(lineName)
    # }


    def __ostfCombine2Body(self, name, inputSeq, decayDesc, extracuts = None) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2100*MeV) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL_2Body)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL_2Body)s *MeV)""" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        topoNBody = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return topoNBody
    # }

    def __ostfCombine2BodyInc(self, name, inputSeq, decayDesc, extracuts = None) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AM<2000*MeV) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL_2BodyInc)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL_2BodyInc)s *MeV)""" % self.getProps()

        if extracuts and extracuts.has_key('MotherCut') :
            parentcuts = parentcuts  + '&' + extracuts['MotherCut']

        combineTopoNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )    

        topoNBody = bindMembers( name, inputSeq + [ combineTopoNBody ] )
        return topoNBody
    # }


    def __ostfFilter2Body(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.Hlt2Monitoring import Hlt2Monitor

        codestr = "(BPVDIRA > %(OSTFPointLL_2Body)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , PreMonitor  =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_in',nbins=101) 
                             , PostMonitor =  Hlt2Monitor( "M","M(K#pi)",1865.,50,'M_out',nbins=101)   
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }

    def __ostfFilter2BodyInc(self, name, inputSeq, extracode = None) : # {
        """  
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        props = self.getProps()
        preambulo = ["PTRANS = P*sqrt( 1-BPVDIRA**2 )",
                     "MCOR = sqrt(M**2 + PTRANS**2) + PTRANS"]
        codestr = '(in_range(%s*MeV,MCOR,%s*MeV))' % (props['OSTFMCOR_MIN_2BodyInc'],props['OSTFMCOR_MAX_2BodyInc'])
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                             , Preambulo=preambulo 
                           )    
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


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

        from HltTracking.Hlt2TrackingConfigurations import Hlt2UnfittedForwardTracking
        tracks = Hlt2UnfittedForwardTracking().hlt2PrepareTracks()

        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('ComRobUseGEC') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(ComRobGEC)s" % self.getProps()
        # }
            
        Hlt2CharmKillTooManyInTrkAlg = VoidFilter('Hlt2CharmKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2CharmKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2CharmKillTooManyInTrkAlg ] )

        return Hlt2CharmKillTooManyInTrk
    # }


    def __robInPartFilter(self, name, inputSeq) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the charm lines.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
                      & (P> %(ComRobAllTrkPLL)s *MeV)
                      & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputSeq
                            , Code = daugcuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D() ] + inputSeq + [ filter ] )

        return filterSeq
    # }


    def __tfInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   post-track-fit stages of the charm lines.  It lashes the new
        #   FilterDesktop to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(ComTFAllTrkPtLL)s *MeV)
                    & (P> %(ComTFAllTrkPLL)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(ComTFAllTrkPVIPChi2LL)s )
                    & (TRCHI2DOF< %(ComTFAllTrkChi2UL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        ## Require tracking before attempts to fit the tracks.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter] )

        return filterSeq
    # }


    def __ostfInPartFilter2Body(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(OSTFAllTrkPtLL_2Body)s *MeV)
                    & (P> %(OSTFAllTrkPLL_2Body)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL_2Body)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL_2Body)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }

    def __ostfInPartFilter2BodyInc(self, name, inputContainers) : # {
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D 

        incuts = """(PT> %(OSTFAllTrkPtLL_2BodyInc)s *MeV)
                    & (P> %(OSTFAllTrkPLL_2BodyInc)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL_2BodyInc)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL_2BodyInc)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputContainers
                            , Code = incuts
                           )    

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }    

   


    def __apply_configuration__(self) :
        ###################################################################
        # Decay descriptors
        ###################################################################
        decayDesc2Body = [  "D0 -> pi+ pi-"
                          , "D0 -> K+ K-"   
                          , "D0 -> K+ pi-"  , "D0 -> pi+ K-"]
        decayDesc2BodyInc = [  "D0 -> pi+ pi-"
                          , "D0 -> pi+ pi+"   
                          , "D0 -> pi- pi-"]


        ###################################################################
        # Combinations for one stage lines that run on fitted tracks, for
        # low rate running.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        lclOSTFInputKaons2Body = self.__ostfInPartFilter2Body('CharmInputKaons2Body', [ BiKalmanFittedKaons] )
        lclOSTFInputPions2Body = self.__ostfInPartFilter2Body('CharmInputPions2Body', [ BiKalmanFittedPions] )
        lclOSTFInputKaons2BodyInc = self.__ostfInPartFilter2BodyInc('CharmInputKaons2BodyInc', [ BiKalmanFittedKaons] )
        lclOSTFInputPions2BodyInc = self.__ostfInPartFilter2BodyInc('CharmInputPions2BodyInc', [ BiKalmanFittedPions] )

        # 2-body combinations
        ###################################################################
        charmOSTF2Body = self.__ostfCombine2Body(  name = 'Charm2Body'
                                , inputSeq = [ lclOSTFInputKaons2Body, lclOSTFInputPions2Body ]
                                , decayDesc = decayDesc2Body 
                                , extracuts = { 'CombinationCut' : 
                                                "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(OSTFPairMinDocaUL_2Body)s )" % self.getProps() 
                                              }
                               )

        # 2-body inclusive combinations
        ###################################################################
        charmOSTF2BodyInc = self.__ostfCombine2BodyInc(  name = 'Charm2BodyInc'
                                , inputSeq = [ lclOSTFInputKaons2BodyInc, lclOSTFInputPions2BodyInc ]
                                , decayDesc = decayDesc2BodyInc 
                                , extracuts = { 'CombinationCut' : 
                                                "(AMINDOCA('LoKi::TrgDistanceCalculator') < %(OSTFPairMinDocaUL_2BodyInc)s )" % self.getProps() 
                                              }    
                               )


        ###################################################################
        # Decision sequences for the one stage lines
        ###################################################################

        # 2-body decision
        ###################################################################
        charmOSTF2BodySeq = self.__ostfFilter2Body('Charm2Body'
                                     , [charmOSTF2Body]
                                     , extracode = 'in_range(1839*MeV, M, 1889*MeV) & (PT > %(OSTFDPtLL_2Body)s) & (VFASPF(VCHI2PDOF) < %(OSTFVtxChi2UL_2Body)s)' % self.getProps()) 

        # 2-body inclusive decision
        ###################################################################
        charmOSTF2BodyIncSeq = self.__ostfFilter2BodyInc('Charm2BodyInc'
                                     , [charmOSTF2BodyInc]
                                     , extracode = "(SUMTREE(PT,('pi+'==ABSID),0.0) > %(OSTFDSumPtLL_2BodyInc)s) & (PT > %(OSTFDPtLL_2BodyInc)s) & (BPVIPCHI2() > %(OSTFDIPChi2LL_2BodyInc)s)" % self.getProps())

        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmOSTF2BodySBSeq = self.__ostfFilter2Body('CharmPostOSTF2BodyWideMass'
                                     , [charmOSTF2Body]
                                     , extracode = 'in_range(1700*MeV, M, 2100*MeV)')



        # Line creation for one stage track fit lines.
        ###################################################################
        charmOSNBodySeq = {  '2Body' : {  'Std'  : charmOSTF2BodySeq
                                        , 'Wide' : charmOSTF2BodySBSeq }
                          }

        for charmSeq in charmOSNBodySeq.keys() :
            osLineName   = 'Charm' + charmSeq
            oswmTFLineName = 'Charm' + charmSeq + 'WideMass'
            self.__makeLine(osLineName, algos = [ charmOSNBodySeq[charmSeq]['Std'] ])
            self.__makeLine(oswmTFLineName, algos = [ charmOSNBodySeq[charmSeq]['Wide'] ])

        #Make the inclusive line!
        self.__makeLine("Charm2BodyInc",algos=[charmOSTF2BodyIncSeq])

