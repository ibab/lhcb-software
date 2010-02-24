## $Id: Hlt2CharmLines.py,v 1.6 2010-02-24 04:20:24 gligorov Exp $
__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-02-24 04:20:24 $'
__version__ = '$Revision: 1.6 $'

## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

class Hlt2CharmLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = { 'ComRobAllTrkPtLL'        : 300        # in MeV
                , 'ComRobAllTrkPLL'         : 2000       # in MeV
                , 'ComRobAllTrkPVIPLL'      : 0.05       # in mm
                , 'ComRobPairMinDocaUL'     : 0.10       # in mm
                , 'ComRobPairMaxDocaUL'     : 1          # in mm
                , 'ComRobTrkMaxPtLL'        : 1500       # in MeV
                , 'ComRobVtxPVDispLL'       : 2          # in mm
                , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                , 'ComRobUseGEC'            : True       # do or do not 
                , 'ComRobGEC'               : 120        # max number of tracks
                , 'ComTFAllTrkPtLL'         : 300        # in MeV
                , 'ComTFAllTrkPLL'          : 2000       # in MeV
                , 'ComTFAllTrkPVIPChi2LL'   : 9          # unitless
                , 'ComTFAllTrkChi2UL'       : 10         # unitless
                , 'ComTFPairMinDocaUL'      : 0.10       # in mm
                , 'ComTFPairMaxDocaUL'      : 1          # in mm
                , 'ComTFTrkMaxPtLL'         : 1500       # in MeV
                , 'ComTFVtxPVDispChi2LL'    : 100        # unitless
                , 'RobustPointingUL'        : 0.10       # unitless
                , 'TFPointUL'               : 0.10       # unitless
                , 'Prescale'                : {'Hlt2CharmTF2BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF3BodyWideMass' : 0.05
                                               , 'Hlt2CharmTF4BodyWideMass' : 0.05
                                              }
                , 'Postscale'               : {'Hlt2Charm2BodySA' : 0.002
                                               , 'Hlt2Charm3BodySA' : 0.0005
                                               , 'Hlt2Charm4BodySA' : 0.0005
                                              }
                # The HltANNSvc ID numbers for each line should be configurable.
                , 'HltANNSvcID'  : {'Hlt2Charm2BodySADecision'   : 50850
                                   , 'Hlt2CharmTF2BodySignalDecision' : 50880
                                   , 'Hlt2CharmTF2BodyWideMassDecision' : 50890
                                   , 'Hlt2Charm3BodySADecision'   : 50860
                                   , 'Hlt2CharmTF3BodySignalDecision' : 50910
                                   , 'Hlt2CharmTF3BodyWideMassDecision' : 50920
                                   , 'Hlt2Charm4BodySADecision'   : 50870
                                   , 'Hlt2CharmTF4BodySignalDecision' : 50950
                                   , 'Hlt2CharmTF4BodyWideMassDecision' : 50960
                                   }
                  }



    def updateHltANNSvc(self,line) : # {
        """
        Wrapper for updating the HltANNSvc after a new line has been
        constructed.  This should eventually become obsolete.
        """
        from Configurables import HltANNSvc
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )
    # }


    def makeLine(self, lineName, algos) : # {
        """
        Wrapper for line construction that also registers it to the HltANNSvc.
        """
        from HltLine.HltLine import Hlt2Line

        lclAlgos = []
        ## Prepend a filter on the number of tracks, if required.
        if self.getProp('ComRobUseGEC') : # {
            Hlt2TopoKillTooManyInTrk = self.seqGEC()
            lclAlgos = [ Hlt2TopoKillTooManyInTrk ]
        # }
        lclAlgos.extend(algos)

        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = lclAlgos
                       )
        self.updateHltANNSvc(lineName)
    # }


    def robustCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure common particle combinations used by the
        #   robust stages of the charm lines.  It lashes the new
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
        combcuts = "(AM<2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

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

        combineNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        charmNBody = bindMembers( name, inputSeq + [ combineNBody ] )
        return charmNBody
    # }


    def robustFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a robust filter for the charm.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(M>1700*MeV) & (M<2100*MeV) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
        if extracode :
          codestr = codestr + '&' + extracode
        filter = Hlt2Member( FilterDesktop
                            , 'CharmRobustFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                           )

        filterSeq = bindMembers( name, inputSeq + [ filter ] )
        return filterSeq
    # }


    def tfCombine(self, name, inputSeq, decayDesc, extracuts = None) : # {
        """
        # Function to configure post-track-fit particle combinations
        #   used by the charm lines.  It lashes the new
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
        combcuts = "(AM<2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMaxDocaUL)s ) & (AALLSAMEBPV)" % self.getProps()

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

        combineNBody = Hlt2Member( CombineParticles
                                       , 'Combine'
                                       , DecayDescriptors = decayDesc
                                       , InputLocations = inputSeq
                                       , CombinationCut = combcuts
                                       , MotherCut = parentcuts
                                     )

        charmNBody = bindMembers( name, inputSeq + [ combineNBody ] )
        return charmNBody
    # }


    def tfFilter(self, name, inputSeq, extracode = None) : # {
        """
        # Function to configure a post-track-fit filter for the charm.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles

        codestr = "(BPVTRGPOINTINGWPT< %(TFPointUL)s )" % self.getProps()
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


    def seqGEC(self) : # {
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

	from Configurables import Hlt2Tracking

        Hlt2TopoKillTooManyInTrkAlg = VoidFilter('Hlt2TopoKillTooManyInTrkAlg'
                                              , Code = "TrSOURCE('"+(Hlt2Tracking().hlt2Tracking()).outputSelection()+"') >> (TrSIZE < %(ComRobGEC)s )" % self.getProps()
                                              )
        Hlt2TopoKillTooManyInTrk = bindMembers( None, [ Hlt2Tracking().hlt2Tracking(), Hlt2TopoKillTooManyInTrkAlg ] )

        return Hlt2TopoKillTooManyInTrk
    # }


    def robInPartFilter(self, name, inputSeq) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the topological.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.HltPVs import PV3D

        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
                      & (P> %(ComRobAllTrkPLL)s *MeV)
                      & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = inputSeq
                            , Code = daugcuts
                           )

        ## Require the PV3D reconstruction before our cut on IP.
        filterSeq = bindMembers( name, [ PV3D ] + inputSeq + [ filter ] )

        return filterSeq
    # }


    def tfInPartFilter(self, name, inputContainers) : # {
        """
        # Function to configure a filter for the input particles of the
        #   robust stages of the topological.  It lashes the new FilterDesktop
        #   to a bindMembers with its antecedents.
        # The argument inputContainer should be 
        #   a list of bindMember sequences that produces the particles
        #   to filter.
        """
        from HltLine.HltLine import Hlt2Member, bindMembers
        from Configurables import FilterDesktop, CombineParticles
        from HltLine.HltPVs import PV3D

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
        filterSeq = bindMembers( name, [ PV3D] + inputContainers + [filter] )

        return filterSeq
    # }


    def __apply_configuration__(self) :
        ###################################################################
        ## Filter the input particles.
        ###################################################################
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        lclRobInputKaons = self.robInPartFilter('TopoInputKaons', [ GoodKaons ])
        lclRobInputPions = self.robInPartFilter('TopoInputPions', [ GoodPions ])


        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ###################################################################

        # CombineParticles for the robust 2-body combinations.
        ###################################################################
        charm2Body = self.robustCombine(  name = 'Charm2Body'
                                  , inputSeq = [ lclRobInputKaons, lclRobInputPions ]
                                  #, decayDesc = ["K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-" ]
                                  , decayDesc = [ "K*(892)0 -> pi+ pi+"
                                                  , "K*(892)0 -> pi+ pi-"
                                                  , "K*(892)0 -> pi- pi-"
                                                  , "K*(892)0 -> K+ K+"
                                                  , "K*(892)0 -> K+ K-"
                                                  , "K*(892)0 -> K- K-"
                                                  , "K*(892)0 -> K+ pi-"
                                                  , "K*(892)0 -> pi+ K-"
                                                  , "K*(892)0 -> K+ pi+"
                                                  , "K*(892)0 -> K- pi-" ]
                                  , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                  )

        # CombineParticles for the robust 3-body combinations.
        ###################################################################
        charm3Body = self.robustCombine(  name = 'Charm3Body'
                                  , inputSeq = [ lclRobInputPions, charm2Body ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"])


        # Construct a bindMember for the charm robust 4-body decision
        # CombineParticles for the 4-body combinations.
        ###################################################################
        charmRob4Body = self.robustCombine(  name = 'CharmRobust4Body'
                                  , inputSeq = [lclRobInputPions, charm3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                  , extracuts = {'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)'
                                                , 'MotherCut'     : "(SUMQ == 0) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
                                                }
                                  )


        ###################################################################
        # Construct a bindMember for the charm robust 2-body decision
        ###################################################################
        charmRobust2BodySeq = self.robustFilter('CharmRobust2Body', [charm2Body], extracode = '(SUMQ==0)')


        # Construct a bindMember for the charm robust 3-body decision
        ###################################################################
        charmRobust3BodySeq = self.robustFilter('CharmRobust3Body', [charm3Body], extracode = '((SUMQ==1) | (SUMQ == -1))')


        # Construct a bindMember for the charm robust 4-body decision
        ###################################################################
        charmRobust4BodySeq = self.robustFilter('CharmRobust4Body', [charmRob4Body])



        ###################################################################
        ## Filter post-track fit input particles.
        ###################################################################
        from Hlt2SharedParticles.TFBasicParticles import TFKaons, TFPions
        lclTFInputKaons = self.tfInPartFilter('TopoTFInputKaons', [ TFKaons] )
        lclTFInputPions = self.tfInPartFilter('TopoTFInputPions', [ TFPions] )


        ###################################################################
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################

        # post-track-fit 2-body combinations
        ###################################################################
        charmTF2Body = self.tfCombine(  name = 'CharmTF2Body'
                                , inputSeq = [ lclTFInputKaons, lclTFInputPions ]
                                #, decayDesc = ["K*(892)0 -> pi+ pi-", "K*(892)0 -> K+ K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-"]
                                , decayDesc = [ "K*(892)0 -> pi+ pi+"
                                                , "K*(892)0 -> pi+ pi-"
                                                , "K*(892)0 -> pi- pi-"
                                                , "K*(892)0 -> K+ K+"
                                                , "K*(892)0 -> K+ K-"
                                                , "K*(892)0 -> K- K-"
                                                , "K*(892)0 -> K+ pi-"
                                                , "K*(892)0 -> pi+ K-"
                                                , "K*(892)0 -> K+ pi+"
                                                , "K*(892)0 -> K- pi-" ]
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # post-track-fit 3-body combinations
        ###################################################################
        charmTF3Body = self.tfCombine(  name = 'CharmTF3Body'
                                , inputSeq = [ lclTFInputPions, charmTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                               )

        # post-track-fit 4-body combinations
        ###################################################################
        charmTF4Body = self.tfCombine(  name = 'CharmTF4Body'
                                , inputSeq = [ lclTFInputPions, charmTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>1839*MeV) & (AM<1889*MeV)'
                                              , 'MotherCut' : '(SUMQ==0) & (BPVTRGPOINTINGWPT< %(TFPointUL)s )' % self.getProps()
                                              }
                                 )
        ###################################################################
        # Main 2-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF2BodySeq = self.tfFilter('CharmPostTF2Body'
                                     , [charmTF2Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV) & (SUMQ == 0)')


        # Main 3-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF3BodySeq = self.tfFilter('CharmPostTF3Body'
                                     , [charmTF3Body]
                                     , extracode = '(((M>1844*MeV) & (M<1894*MeV)) | ((M>1943*MeV) & (M<1993*MeV)) & ((SUMQ == -1) |(SUMQ == 1)))')


        # Main 4-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF4BodySeq = self.tfFilter('CharmPostTF4Body'
                                     , [charmTF4Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')



        ##
        #
        # Charm lines
        #
        ##
        # Map charm robust sequences to monitoring line names
        ###################################################################
        charmRobustNBodySeq = {  'Charm2BodySA' : charmRobust2BodySeq
                               , 'Charm3BodySA' : charmRobust3BodySeq
                               , 'Charm4BodySA' : charmRobust4BodySeq
                              }

        ###################################################################
        # 'Factory' for standalone monitoring lines for the robust stage of
        #   the charm trigger. 
        # Heavily post-scaled.
        ###################################################################
        for charmSeq in charmRobustNBodySeq.keys() :
            self.makeLine(charmSeq, algos = [charmRobustNBodySeq[charmSeq]] )


        ##
        #
        # Define the main charm lines
        #
        ##
        self.makeLine('CharmTF2BodySignal'
                 , algos = [ charmRobust2BodySeq, charmTF2BodySeq ])

        self.makeLine('CharmTF3BodySignal'
                 , algos = [ charmRobust3BodySeq, charmTF3BodySeq ])

        self.makeLine('CharmTF4BodySignal'
                 , algos = [ charmRobust4BodySeq, charmTF4BodySeq])


        ##
        #
        # Define additional charm lines for mass sidebands.  Heavily prescaled.
        #
        ##
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF2BodySBSeq = self.tfFilter('CharmPostTF2BodyWideMass'
                                     , [charmTF2Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        self.makeLine('CharmTF2BodyWideMass'
                 , algos = [ charmRobust2BodySeq, charmTF2BodySBSeq ])


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF3BodySBSeq = self.tfFilter('CharmPostTF3BodyWideMass'
                                     , [charmTF3Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        self.makeLine('CharmTF3BodyWideMass'
                 , algos = [ charmRobust3BodySeq, charmTF3BodySBSeq ])


        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF4BodySB = self.tfCombine(  name = 'CharmWMTF4Body'
                                , inputSeq = [ lclTFInputPions, charmTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)' 
                                              , 'MotherCut' : '(BPVTRGPOINTINGWPT< %(TFPointUL)s )' % self.getProps()
                                              }
                                 )

        charmTF4BodySBSeq = self.tfFilter('CharmPostTF4BodyWideMass'
                                     , [charmTF4BodySB]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        self.makeLine('CharmTF4BodyWideMass'
                 , algos = [ charmRobust4BodySeq, charmTF4BodySBSeq])


