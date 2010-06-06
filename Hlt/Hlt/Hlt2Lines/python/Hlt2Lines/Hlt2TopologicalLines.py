## $Id: Hlt2TopologicalLines.py,v 1.42 2010-06-06 22:21:20 gligorov Exp $
__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-06-06 22:21:20 $'
__version__ = '$Revision: 1.42 $'

###
#
# This is the 'umbrella' file for the topological trigger. 
# It should hopefully improve readability since the topological
# trigger is made up of several different selections (2,3,4-body),
# which can be run before and after the track fit. 
#
# The naming convention is:
#
# Hlt2TopoNBody: the N-body (N=2,3,4) selection before any track fit
#
# Hlt2TopoNBodySA : the decision of the N-body (N=2,3,4) selection
#                      before any track fit which is prescaled in order
#                       to provide a low rate calibration stream.
#
# Hlt2TopoTFNBody : the CombineParticles instance of the N-body (N=2,3,4)
#                      selection after the track fit. These files do not
#                      define any Hlt2Lines they just define the selections
#                      which can then be added to the sequence later.
#
# Hlt2TopoTFNBodySA: the N-body (N=2,3,4) selection after the track fit,
#                       which runs irrespective of the result of any other
#                       Hlt2 selections.
#
#
# Hlt2TopoTFNBodyReqMYes: the N-body (N=2,3,4) selection after the track
#                             fit, which only runs if the M-Body selection
#                             before the track fit (Hlt2LineTopoMBody) has
#                             passed (M=2,3,4).
#
## 
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = { ## Cut values for robust stages.
                  'ComRobAllTrkPtLL'        : 300.0      # in MeV
                , 'ComRobAllTrkPLL'         : 2000.0     # in MeV
                , 'ComRobAllTrkPVIPLL'      : 0.025      # in mm
                , 'ComRobPairMinDocaUL'     : 0.20       # in mm
                , 'ComRobPairMaxDocaUL'     : 1.0        # in mm
                , 'ComRobTrkMaxPtLL'        : 1500.0     # in MeV
                , 'ComRobVtxPVDispLL'       : 2.0        # in mm
                , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                , 'RobustPointingUL'        : 0.28       # unitless
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 120        # max number of tracks
                ## Cut values for post-track fit stages
                , 'ComTFAllTrkPtLL'         : 300.0      # in MeV
                , 'ComTFAllTrkPLL'          : 2000.0     # in MeV
                , 'ComTFAllTrkPVIPChi2LL'   : 9.0        # unitless
                , 'ComTFAllTrkChi2UL'       : 10.0       # unitless
                , 'ComTFPairMinDocaUL'      : 0.20       # in mm
                , 'ComTFPairMaxDocaUL'      : 1.0        # in mm
                , 'ComTFTrkMaxPtLL'         : 1500.0     # in MeV
                , 'ComTFVtxPVDispChi2LL'    : 64.0       # unitless
                , 'TFPointUL'               : 0.28       # unitless
                ## Cut values for one stage track-fit (OSTF) lines.
                , 'OSTFAllTrkPtLL'          : 300.0      # in MeV
                , 'OSTFAllTrkPLL'           : 2000.0     # in MeV
                , 'OSTFAllTrkPVIPChi2LL'    : 9.0        # unitless
                , 'OSTFAllTrkChi2UL'        : 10.0       # unitless
                , 'OSTFPairMinDocaUL'       : 0.20       # in mm
                , 'OSTFPairMaxDocaUL'       : 1.0        # in mm
                , 'OSTFTrkMaxPtLL'          : 1500.0     # in MeV
                , 'OSTFVtxPVDispChi2LL'     : 64.0       # unitless
                , 'OSTFPointUL'             : 0.28       # unitless
                ## Pre- and Post-scales for lines.
                , 'Prescale'                : {'Hlt2TopoTF2BodySA' : 1.00
                                               , 'Hlt2TopoTF3BodySA' : 0.001
                                               , 'Hlt2TopoTF4BodySA' : 0.001
                                              }
                , 'Postscale'               : {'Hlt2Topo2BodySA' : 0.02
                                               , 'Hlt2Topo3BodySA' : 0.001
                                               , 'Hlt2Topo4BodySA' : 0.001
                                              }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : {'Hlt2Topo2BodySADecision'         : 50700
                                   , 'Hlt2Topo3BodySADecision'        : 50710
                                   , 'Hlt2Topo4BodySADecision'        : 50720
                                   , 'Hlt2TopoTF2BodySADecision'      : 50730
                                   , 'Hlt2TopoTF3BodySADecision'      : 50770
                                   , 'Hlt2TopoTF4BodySADecision'      : 50810
                                   , 'Hlt2TopoTF2BodyReq2YesDecision' : 50740
                                   , 'Hlt2TopoTF2BodyReq3YesDecision' : 50750
                                   , 'Hlt2TopoTF2BodyReq4YesDecision' : 50760
                                   , 'Hlt2TopoTF3BodyReq2YesDecision' : 50780
                                   , 'Hlt2TopoTF3BodyReq3YesDecision' : 50790
                                   , 'Hlt2TopoTF3BodyReq4YesDecision' : 50800
                                   , 'Hlt2TopoTF4BodyReq2YesDecision' : 50820
                                   , 'Hlt2TopoTF4BodyReq3YesDecision' : 50830
                                   , 'Hlt2TopoTF4BodyReq4YesDecision' : 50840
                                   , 'Hlt2TopoOSTF2BodyDecision'      : 50733
                                   , 'Hlt2TopoOSTF3BodyDecision'      : 50773
                                   , 'Hlt2TopoOSTF4BodyDecision'      : 50813
                                   , 'Hlt2TopoRobTF2BodyDecision'     : 50735
                                   , 'Hlt2TopoRobTF3BodyDecision'     : 50775
                                   , 'Hlt2TopoRobTF4BodyDecision'     : 50815
                                   }
                  }



    def __updateHltANNSvc(self,line) : # {
        """
        Wrapper for updating the HltANNSvc after a new line has been
        constructed.  This should eventually become obsolete.
        """
        from Configurables import HltANNSvc
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )
    # }


    def __makeLine(self, lineName, algos) : # {
        """
        Wrapper for line construction that also registers it to the HltANNSvc.
        """
        from HltLine.HltLine import Hlt2Line

        lclAlgos = []
        ## Prepend a filter on the number of tracks
        Hlt2TopoKillTooManyInTrk = self.__seqGEC()
        lclAlgos = [ Hlt2TopoKillTooManyInTrk ]
        lclAlgos.extend(algos)

        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                       )
        self.__updateHltANNSvc(lineName)
    # }


    def __robustCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure common particle combinations used by the
        #   robust stages of the topological lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """
            (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComRobTrkMaxPtLL)s *MeV)
            & (BPVVD> %(ComRobVtxPVDispLL)s )
            & (BPVVDR> %(ComRobVtxPVRDispLL)s )""" % self.getProps()
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


    def __robustFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the robust stages of the
        #   topological.  It lashes the new FilterDesktop to a bindMembers
        #   with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Build a cut string from the configuration variables.
        codestr = "(M > 4*GeV) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'RobustFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __tfCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure post-track-fit particle combinations
        #   used by the topological lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(ComTFVtxPVDispChi2LL)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(ComTFTrkMaxPtLL)s *MeV)""" % self.getProps()

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


    def __tfFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a post-track-fit filter for the topological.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(BPVTRGPOINTINGWPT< %(TFPointUL)s ) & (M>4*GeV)" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
                           )
        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def __ostfCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure track fit particle combinations
        #   used by the one stage topological lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        # Construct a cut string for the combination.
        combcuts = "(AMAXDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

        # extracuts allows additional cuts to be applied for special
        #   cases, including the tight doca requirement of the 2-body and
        #   the additional cuts to reduce stored combinations in the 4-body.
        if extracuts and extracuts.has_key('CombinationCut') :
            combcuts = combcuts + '&' + extracuts['CombinationCut']

        # Construct a cut string for the vertexed combination.
        parentcuts = """(BPVVDCHI2> %(OSTFVtxPVDispChi2LL)s )
            & (MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)> %(OSTFTrkMaxPtLL)s *MeV)""" % self.getProps()

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


    def __ostfFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a filter for the one stage track-fit
        #   topological lines.  It lashes the new FilterDesktop to a
        #   bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(BPVTRGPOINTINGWPT< %(OSTFPointUL)s ) & (M>4*GeV)" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                             , 'TFFilter'
                             , InputLocations = inputSeq
                             , Code = codestr
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

        ## By default, configure as a pass-all filter with similar code.
        filtCode = "CONTAINS('"+tracks.outputSelection()+"') > -1"
        if self.getProp('ComRobUseGEC') : # {
            filtCode = "CONTAINS('"+tracks.outputSelection()+"') < %(ComRobGEC)s" % self.getProps()
        # }
            
        Hlt2TopoKillTooManyInTrkAlg = VoidFilter('Hlt2TopoKillTooManyInTrkAlg'
                                                 , Code = filtCode
                                                )
        Hlt2TopoKillTooManyInTrk = bindMembers( None, [ tracks, Hlt2TopoKillTooManyInTrkAlg ] )

        return Hlt2TopoKillTooManyInTrk
    # }


    def __robInPartFilter(self, name, inputSeq) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the topological.  It lashes the new FilterDesktop
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
        #   post-track-fit stages of the topological.  It lashes the new
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
        filterSeq = bindMembers( name, [ PV3D()] + inputContainers + [filter ] )

        return filterSeq
    # }


    def __ostfInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   one stage track fit lines.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltTracking.HltPVs import PV3D

        incuts = """(PT> %(OSTFAllTrkPtLL)s *MeV)
                    & (P> %(OSTFAllTrkPLL)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(OSTFAllTrkPVIPChi2LL)s )
                    & (TRCHI2DOF< %(OSTFAllTrkChi2UL)s )""" % self.getProps()

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
        ## Filter the input particles.
        ###################################################################
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        lclRobInputKaons = self.__robInPartFilter('TopoInputKaons', [ GoodKaons ])
        lclRobInputPions = self.__robInPartFilter('TopoInputPions', [ GoodPions ])


        ###################################################################
        ## CombineParticles for the robust 2-body combinations.
        ## Create only the heaviest KK combinations.  We only need to know
        ##   if the heaviest mass hypothesis for a set of tracks passes our
        ##   mass lower limit.
        ###################################################################
        topo2Body = self.__robustCombine(  name = 'Topo2Body'
                                  , inputSeq = [ lclRobInputKaons ]
                                  , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                  , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                  )

        ## CombineParticles for the robust 3-body combinations.
        ###################################################################
        topo3Body = self.__robustCombine(  name = 'Topo3Body'
                                  , inputSeq = [ lclRobInputKaons, topo2Body ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"])

        ## CombineParticles for the robust 4-body combinations.
        ## Unlike the 3-body and 4-body, apply a mass lower limit.
        ###################################################################
        ## There seems to be a lot of CPUT consumed in managing the large number
        ##   of 4-body candidates.  The list needs to be as small as possible.
        topo4Body = self.__robustCombine(  name = 'Topo4Body'
                                  , inputSeq = [lclRobInputKaons, topo3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
                                  , extracuts = { 'CombinationCut' : '(AM>4*GeV)'
                                                , 'MotherCut'      : "(BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps() }
                                  )


        ###################################################################
        # Create full decision sequences for the robust stages of the
        #   topological trigger.
        # These will only be used in topological trigger lines.
        ###################################################################

        # Construct a bindMember for the topological robust 2-body decision
        ###################################################################
        robust2BodySeq = self.__robustFilter('RobustTopo2Body', [topo2Body])

        # Construct a bindMember for the topological robust 3-body decision
        ###################################################################
        robust3BodySeq = self.__robustFilter('RobustTopo3Body', [topo3Body])

        # Construct a bindMember for the topological robust 4-body decision
        ###################################################################
        robust4BodySeq = self.__robustFilter('RobustTopo4Body', [topo4Body])



        ###################################################################
        ## Filter post-track fit input particles.
        ###################################################################
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedKaons, BiKalmanFittedPions
        lclTFInputKaons = self.__tfInPartFilter('TopoTFInputKaons', [ BiKalmanFittedKaons ] )
        lclTFInputPions = self.__tfInPartFilter('TopoTFInputPions', [ BiKalmanFittedPions ] )

        # post-track-fit 2-body combinations
        ###################################################################
        topoTF2Body = self.__tfCombine(  name = 'TopoTF2Body'
                                , inputSeq = [ lclTFInputKaons ]
                                , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # post-track-fit 3-body combinations
        ###################################################################
        topoTF3Body = self.__tfCombine(  name = 'TopoTF3Body'
                                , inputSeq = [ lclTFInputKaons, topoTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                               )

        # post-track-fit 4-body combinations
        # Unlike the 3-body and 4-body, apply a mass lower limit.
        ###################################################################
        topoTF4Body = self.__tfCombine(  name = 'TopoTF4Body'
                                , inputSeq = [ lclTFInputKaons, topoTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
                                , extracuts = { 'CombinationCut' : '(AM>4*GeV)'
                                              , 'MotherCut'      : "(BPVTRGPOINTINGWPT< %(TFPointUL)s)" % self.getProps()
                                              }
                               )


        ###################################################################
        # Create full decision sequences for the post-track-fit stages of the
        #   topological trigger.
        # These will only be used in topological trigger lines.
        ###################################################################

        # Construct a bindMember for the topological post-TF 2-body decision
        ###################################################################
        tf2BodySeq = self.__tfFilter('PostTFTopo2Body', [topoTF2Body])

        # Construct a bindMember for the topological post-TF 3-body decision
        ###################################################################
        tf3BodySeq = self.__tfFilter('PostTFTopo3Body', [topoTF3Body])

        # Construct a bindMember for the topological post-TF 4-body decision
        ###################################################################
        tf4BodySeq = self.__tfFilter('PostTFTopo4Body', [topoTF4Body])


        ###################################################################
        # Map Robust sequences to partial line names
        ###################################################################
        robustNBodySeq = {  'Topo2Body' : robust2BodySeq
                          , 'Topo3Body' : robust3BodySeq
                          , 'Topo4Body' : robust4BodySeq
                         }
        # Same sequenes, different names.
        robustNReqSeq = {  'Req2Yes' : robust2BodySeq
                         , 'Req3Yes' : robust3BodySeq
                         , 'Req4Yes' : robust4BodySeq
                        }

        # Map Post-track-fit sequences to partial line names
        ###################################################################
        tfNBodySeq = {  'TopoTF2Body' : tf2BodySeq
                      , 'TopoTF3Body' : tf3BodySeq
                      , 'TopoTF4Body' : tf4BodySeq
                     }


        ###################################################################
        # 'Factory' for standalone monitoring lines for the robust topological. 
        # Heavily post-scaled.
        ###################################################################
        for robSeq in robustNBodySeq.keys() :
            lineName = robSeq + 'SA'
            self.__makeLine(lineName, algos = [robustNBodySeq[robSeq]])


        ###################################################################
        # 'Factory' for standalone monitoring lines for the post-TF topological.
        # Heavily pre-scaled.
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            lineName = tfSeq + 'SA'
            self.__makeLine(lineName, algos = [tfNBodySeq[tfSeq]] )


        ###################################################################
        # 'Factory' for main topological lines
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            for robSeq in robustNReqSeq.keys() :
                lineName = tfSeq + robSeq
                self.__makeLine(lineName, algos = [ robustNReqSeq[robSeq], tfNBodySeq[tfSeq] ])



        ###################################################################
        # Combinations for one stage lines that run on fitted tracks, for
        # low rate running.
        ###################################################################
        lclOSTFInputKaons = self.__ostfInPartFilter('TopoOSTFInputKaons', [ BiKalmanFittedKaons ] )
        lclOSTFInputPions = self.__ostfInPartFilter('TopoOSTFInputPions', [ BiKalmanFittedPions ] )

        # one stage 2-body combinations
        ###################################################################
        topoOSTF2Body = self.__ostfCombine(  name = 'TopoOSTF2Body'
                                , inputSeq = [ lclOSTFInputKaons ]
                                , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(OSTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # one stage 3-body combinations
        ###################################################################
        topoOSTF3Body = self.__ostfCombine(  name = 'TopoOSTF3Body'
                                , inputSeq = [ lclOSTFInputKaons, topoOSTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                               )

        # one stage 4-body combinations
        # Unlike the 3-body and 4-body, apply a mass lower limit.
        ###################################################################
        topoOSTF4Body = self.__ostfCombine(  name = 'TopoOSTF4Body'
                                , inputSeq = [ lclOSTFInputKaons, topoOSTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
                                , extracuts = { 'CombinationCut' : '(AM>4*GeV)'
                                              , 'MotherCut'      : "(BPVTRGPOINTINGWPT< %(OSTFPointUL)s)" % self.getProps()
                                              }
                               )


        ###################################################################
        # Create full decision sequences for the one stage lines.
        ###################################################################

        # 2-body decision
        ###################################################################
        ostf2BodySeq = self.__ostfFilter('OSTFTopo2Body', [topoOSTF2Body])

        # 3-body decision
        ###################################################################
        ostf3BodySeq = self.__ostfFilter('OSTFTopo3Body', [topoOSTF3Body])

        # 4-body decision
        ###################################################################
        ostf4BodySeq = self.__ostfFilter('OSTFTopo4Body', [topoOSTF4Body])

        # Line creation for one stage track fit lines.
        ###################################################################
        self.__makeLine('TopoOSTF2Body', algos = [ostf2BodySeq])
        self.__makeLine('TopoOSTF3Body', algos = [ostf3BodySeq])
        self.__makeLine('TopoOSTF4Body', algos = [ostf4BodySeq])


        ###################################################################
        # Combinations for pseudo-robust lines---standalone lines that run
        # the robust reconstruction and combinatorics on track-fit particles.
        ###################################################################
        lclRobTFInputKaons = self.__robInPartFilter('TopoRobTFInputKaons', [ BiKalmanFittedKaons ] )
        lclRobTFInputPions = self.__robInPartFilter('TopoRobTFInputPions', [ BiKalmanFittedPions ] )

        # one stage 2-body combinations
        ###################################################################
        topoRobTF2Body = self.__robustCombine(  name = 'TopoRobTF2Body'
                                , inputSeq = [ lclRobTFInputKaons ]
                                , decayDesc = ["K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-"]
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                               )

        # one stage 3-body combinations
        ###################################################################
        topoRobTF3Body = self.__robustCombine(  name = 'TopoRobTF3Body'
                                , inputSeq = [ lclRobTFInputKaons, topoRobTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 K+", "D*(2010)+ -> K*(892)0 K-"]
                               )

        # one stage 4-body combinations
        # Unlike the 3-body and 4-body, apply a mass lower limit.
        ###################################################################
        topoRobTF4Body = self.__robustCombine(  name = 'TopoRobTF4Body'
                                , inputSeq = [ lclRobTFInputKaons, topoRobTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
                                , extracuts = { 'CombinationCut' : '(AM>4*GeV)'
                                              , 'MotherCut'      : "(BPVTRGPOINTINGWPT< %(RobustPointingUL)s)" % self.getProps()
                                              }
                               )


        ###################################################################
        # Create full decision sequences for the one stage lines.
        ###################################################################

        # 2-body decision
        ###################################################################
        robTF2BodySeq = self.__robustFilter('RobTFTopo2Body', [topoRobTF2Body])

        # 3-body decision
        ###################################################################
        robTF3BodySeq = self.__robustFilter('RobTFTopo3Body', [topoRobTF3Body])

        # 4-body decision
        ###################################################################
        robTF4BodySeq = self.__robustFilter('RobTFTopo4Body', [topoRobTF4Body])

        # Line creation for one stage track fit lines.
        ###################################################################
        self.__makeLine('TopoRobTF2Body', algos = [robTF2BodySeq])
        self.__makeLine('TopoRobTF3Body', algos = [robTF3BodySeq])
        self.__makeLine('TopoRobTF4Body', algos = [robTF4BodySeq])


