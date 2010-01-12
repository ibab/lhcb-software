## $Id: Hlt2CharmLines.py,v 1.1 2010-01-12 15:49:26 spradlin Exp $
__author__  = 'Patrick Spradlin'
__date__    = '$Date: 2010-01-12 15:49:26 $'
__version__ = '$Revision: 1.1 $'

"""
## ######################################################################
## Defines a configurable to define and configure Hlt2 lines for selecting
## hadronic decays of charm hadrons.
## ######################################################################
"""
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
        line = Hlt2Line(lineName
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = algos
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


    def __apply_configuration__(self) :
        from HltLine.HltLine import Hlt2Member
        from HltLine.HltLine import bindMembers
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles


        ###################################################################
        # Start with a GEC on all events with more than 120 tracks
        #  
        # This is a temporary fix only and will be removed once proper
        # GEC lines are implemented
        ###################################################################
        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        modules =  CoreFactory('CoreFactory').Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from Configurables import Hlt2PID
        tracks = Hlt2PID().hlt2Tracking()
        Hlt2KillTooManyCharmIPAlg = VoidFilter('Hlt2KillTooManyCharmIPAlg'
                                              , Code = "TrSOURCE('Hlt/Track/Forward') >> (TrSIZE < %(ComRobGEC)s )" % self.getProps()
                                              )
        Hlt2KillTooManyCharmIP = bindMembers( None, [ tracks, Hlt2KillTooManyCharmIPAlg ] )
        ###################################################################
        # Construct a combined sequence for the input particles to the robust
        #   stage.
        ###################################################################
        encasePions = GaudiSequencer('Hlt2SharedTopo2Body_Pions'
                         , Members =  GoodPions.members()
                         )
        encaseKaons = GaudiSequencer('Hlt2SharedTopo2Body_Kaons'
                         , Members =  GoodKaons.members()
                         )
        orInput = GaudiSequencer('Hlt2SharedTopo2Body_PionsORKaons'
                         , Members =  [ encasePions, encaseKaons ]
                         , ModeOR = True
                         , ShortCircuit = False
                         )

        daugcuts = """(PT> %(ComRobAllTrkPtLL)s *MeV)
                      & (P> %(ComRobAllTrkPLL)s *MeV)
                      & (MIPDV(PRIMARY)> %(ComRobAllTrkPVIPLL)s )""" % self.getProps()
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = [GoodPions, GoodKaons]
                            , Code = daugcuts
                           )
        if self.getProp('ComRobUseGEC') :
            lclInputParticles = bindMembers( 'CharmInputParticles', [ Hlt2KillTooManyCharmIP, orInput, filter ] )
        else :
            lclInputParticles = bindMembers( 'CharmInputParticles', [ orInput, filter ] )



        ###################################################################
        # Now absorb the post-track fit shared particles into this configurable
        # This largely repeats the work of the robust on a different set
        #   of input particles.
        ###################################################################
        # import options for the track fit
        importOptions("$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        ###################################################################
        # Filter for the post-track-fit input particles
        ###################################################################
        incuts = """(PT> %(ComTFAllTrkPtLL)s *MeV)
                    & (P> %(ComTFAllTrkPLL)s *MeV)
                    & (MIPCHI2DV(PRIMARY)> %(ComTFAllTrkPVIPChi2LL)s )
                    & (TRCHI2DOF< %(ComTFAllTrkChi2UL)s )""" % self.getProps()

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = ['Hlt2TFPionsForTopo', 'Hlt2TFKaonsForTopo']
                            , Code = incuts
                           )
        lclTFInputParticles = bindMembers('CharmTFIn', [ GaudiSequencer('SeqHlt2TFParticlesForTopo'), filter ])




        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ###################################################################

        # CombineParticles for the robust 2-body combinations.
        ###################################################################
        charm2Body = self.robustCombine(  name = 'Charm2Body'
                                  , inputSeq = [ lclInputParticles ]
                                  , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
                                  , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComRobPairMinDocaUL)s )" % self.getProps() }
                                  )

        # CombineParticles for the robust 3-body combinations.
        ###################################################################
        charm3Body = self.robustCombine(  name = 'Charm3Body'
                                  , inputSeq = [ lclInputParticles, charm2Body ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"])



        ###################################################################
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################

        # post-track-fit 2-body combinations
        ###################################################################
        charmTF2Body = self.tfCombine(  name = 'CharmTF2Body'
                                , inputSeq = [ lclTFInputParticles ]
                                , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"]
                                , extracuts = { 'CombinationCut' : "(AMINDOCA('LoKi::TrgDistanceCalculator')< %(ComTFPairMinDocaUL)s )" % self.getProps() }
                               )

        # post-track-fit 3-body combinations
        ###################################################################
        charmTF3Body = self.tfCombine(  name = 'CharmTF3Body'
                                , inputSeq = [ lclTFInputParticles, charmTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                               )



        ##
        #
        # Charm lines
        #
        ##
        ###################################################################
        # Construct a bindMember for the charm robust 2-body decision
        ###################################################################
        charmRobust2BodySeq = self.robustFilter('CharmRobust2Body', [charm2Body], extracode = '(SUMQ==0)')

        # Construct a bindMember for the charm robust 3-body decision
        ###################################################################
        charmRobust3BodySeq = self.robustFilter('CharmRobust3Body', [charm3Body], extracode = '((SUMQ==1) | (SUMQ == -1))')

        # Construct a bindMember for the charm robust 4-body decision
        # CombineParticles for the 4-body combinations.
        ###################################################################
        charmRob4Body = self.robustCombine(  name = 'CharmRobust4Body'
                                  , inputSeq = [lclInputParticles, charm3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                  , extracuts = {'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)'
                                                , 'MotherCut'     : "(SUMQ == 0) & (BPVTRGPOINTINGWPT< %(RobustPointingUL)s )" % self.getProps()
                                                }
                                  )
        charmRobust4BodySeq = self.robustFilter('CharmRobust4Body', [charmRob4Body])


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
        ###################################################################
        # Main 2-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF2BodySeq = self.tfFilter('CharmPostTF2Body'
                                     , [charmTF2Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV) & (SUMQ == 0)')

        self.makeLine('CharmTF2BodySignal'
                 , algos = [ charmRobust2BodySeq, charmTF2BodySeq ])


        # Main 3-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF3BodySeq = self.tfFilter('CharmPostTF3Body'
                                     , [charmTF3Body]
                                     , extracode = '(((M>1844*MeV) & (M<1894*MeV)) | ((M>1943*MeV) & (M<1993*MeV)) & ((SUMQ == -1) |(SUMQ == 1)))')

        self.makeLine('CharmTF3BodySignal'
                 , algos = [ charmRobust3BodySeq, charmTF3BodySeq ])


        # Main 4-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF4Body = self.tfCombine(  name = 'CharmTF4Body'
                                , inputSeq = [ lclTFInputParticles, charmTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>1839*MeV) & (AM<1889*MeV)'
                                              , 'MotherCut' : '(SUMQ==0) & (BPVTRGPOINTINGWPT< %(TFPointUL)s )' % self.getProps()
                                              }
                                 )
        charmTF4BodySeq = self.tfFilter('CharmPostTF4Body'
                                     , [charmTF4Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')

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
                                , inputSeq = [ lclTFInputParticles, charmTF3Body ]
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


