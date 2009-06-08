###
#
# This is the 'umbrella' file for the topological trigger. 
# It should hopefully improve readability since the topological
# trigger is made up of several different selections (2,3,4-body),
# which can be run before and after the track fit. 
#
# The naming convention is:
#
# Hlt2LineTopoXBody: the X-body (X=2,3,4) selection before any track fit
#
# Hlt2LineTopoXBodySA : the decision of the X-body (X=2,3,4) selection
#                      before any track fit which is prescaled in order
#                       to provide a low rate calibration stream.
#
# Hlt2LineTopoTFXBody : the CombineParticles instance of the X-body (X=2,3,4)
#                      selection after the track fit. These files do not
#                      define any Hlt2Lines they just define the selections
#                      which can then be added to the sequence later.
#
# Hlt2LineTopoTFXBodySA: the X-body (X=2,3,4) selection after the track fit,
#                       which runs irrespective of the result of any other
#                       Hlt2 selections. These files create Hlt2Lines. 
#
#
# Hlt2LineTopoTFXBodyReqXXYes: the X-body (X=2,3,4) selection after the track
#                             fit, which only runs if the XX-Body selection
#                             before the track fit (Hlt2LineTopoXXBody) has
#                             passed (XX=2,3,4). These files create Hlt2Lines. 
#
# To know the efficiency/rate of the topological trigger on your events,
# you need to query:
#
# Logical OR of (Hlt2LineTopo2Body,Hlt2LineTopo3Body,Hlt2LineTopo4Body) :
# gives the number of events passing the "HLT2A" stage of the topological,
# before any Kalman-filter track fit is performed. If any one of these
# lines accepts an event, it will be processed by the "HLT2B" stage.
#
# Logical OR of all Hlt2LineTopoTFXBodyReqXXYes (X=2,3,4, XX=2,3,4) :
# gives the number of events passing the "HLT2B" stage of the topological
# trigger. An event passing any one of these selections passes HLT2.  
#
#
# Consider making separate configurables for the topo and charm, and
# configuring them through through the umbrella topological configurable.
#
## 
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member
from HltLine.HltLine import bindMembers
from Configurables import HltANNSvc

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = { 'RobustPointingUL'        : 0.20
                , 'TFPointUL'               : 0.10
                , 'CharmRobustPointUL'      : 0.10
                , 'CharmTFPointUL'          : 0.10
                , 'Prescale'                : {'Hlt2TopoTF2BodySA' : 0.001
                                               , 'Hlt2TopoTF3BodySA' : 0.001
                                               , 'Hlt2TopoTF4BodySA' : 0.001
                                               , 'Hlt2TopoTF2BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF3BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF4BodyCharmWideMass' : 0.05
                                              }
                , 'Postscale'               : {'Hlt2Topo2BodySA' : 0.02
                                               , 'Hlt2Topo3BodySA' : 0.001
                                               , 'Hlt2Topo4BodySA' : 0.001
                                               , 'Hlt2Topo2BodyCharmSA' : 0.002
                                               , 'Hlt2Topo3BodyCharmSA' : 0.0005
                                               , 'Hlt2Topo4BodyCharmSA' : 0.0005
                                              }
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
                                   , 'Hlt2Topo2BodyCharmSADecision'   : 50850
                                   , 'Hlt2TopoTF2BodyCharmSignalDecision' : 50880
                                   , 'Hlt2TopoTF2BodyCharmWideMassDecision' : 50890
                                   , 'Hlt2Topo3BodyCharmSADecision'   : 50860
                                   , 'Hlt2TopoTF3BodyCharmSignalDecision' : 50910
                                   , 'Hlt2TopoTF3BodyCharmWideMassDecision' : 50920
                                   , 'Hlt2Topo4BodyCharmSADecision'   : 50870
                                   , 'Hlt2TopoTF4BodyCharmSignalDecision' : 50950
                                   , 'Hlt2TopoTF4BodyCharmWideMassDecision' : 50960
                                   }
                }



    def updateHltANNSvc(self,line) :
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )

    def __apply_configuration__(self) :
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.Topo2Body import Topo2Body
        from Hlt2SharedParticles.Topo3Body import Topo3Body
        from Hlt2SharedParticles.GoodParticles import GoodPions
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
        from Hlt2SharedParticles.TopoTF2Body import TopoTF2Body
        from Hlt2SharedParticles.TopoTF3Body import TopoTF3Body


        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))


        ###################################################################
        # Function to configure a robust filter for the topological.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def robustFilter(inputSeq) :
            filter = Hlt2Member( FilterDesktop
                            , 'RobustFilter'
                            , InputLocations = [inputSeq]
                            , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<" + _cut('RobustPointingUL') + ")"
                               )
            return filter


        ###################################################################
        # Construct a bindMember for the topological robust 2-body decision
        ###################################################################
        def robustTopo2Body() :
            filter = robustFilter(Topo2Body)
            robust = bindMembers( 'RobustTopo2Body', [Topo2Body, filter] )
            return robust

        ###################################################################
        # Construct a bindMember for the topological robust 3-body decision
        ###################################################################
        def robustTopo3Body() :
            filter = robustFilter(Topo3Body)
            robust = bindMembers( 'RobustTopo3Body', [Topo3Body, filter] )
            return robust

        ###################################################################
        # Construct a bindMember for the topological robust 4-body decision
        ###################################################################
        def robustTopo4Body() :

            ###################################################################
            # 4-body robust CombineParticles.
            # This can incorporate the cuts done in the subsequent filter.
            #   It probably should, because doing so reduces combinatorics.
            ###################################################################
            combineTopo4Body = Hlt2Member( CombineParticles
                                        , 'Combine'
                                        , DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
                                        , InputLocations = [ GoodPions, Topo3Body ]
                                        , CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
                                        , MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVD>2) & (BPVVDR>0.2)"
                                         )

            topo4Body = bindMembers( 'Topo4Body', [ GoodPions, Topo3Body, combineTopo4Body ] )

            filter = robustFilter(topo4Body)
            robust = bindMembers( 'RobustTopo4Body', [topo4Body, filter] )
            return robust


        ###################################################################
        # Function to configure a post-track-fit filter for the topological.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def tfFilter(inputSeq) :
            filter = Hlt2Member( FilterDesktop
                            , 'TFFilter'
                            , InputLocations = [inputSeq]
                            , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<" + _cut('TFPointUL') + ")"
                               )
            return filter

        ###################################################################
        # Construct a bindMember for the topological post-TF 2-body decision
        ###################################################################
        def tfTopo2Body() :
            filter = tfFilter(TopoTF2Body)
            postTF = bindMembers( 'PostTFTopo2Body', [TopoTF2Body, filter] )
            return postTF

        ###################################################################
        # Construct a bindMember for the topological post-TF 3-body decision
        ###################################################################
        def tfTopo3Body() :
            filter = tfFilter(TopoTF3Body)
            postTF = bindMembers( 'PostTFTopo3Body', [TopoTF3Body, filter] )
            return postTF

        ###################################################################
        # Construct a bindMember for the topological post-TF 4-body decision
        ###################################################################
        def tfTopo4Body() :

            ###################################################################
            # 4-body post-TF CombineParticles.
            # This can incorporate the cuts done in the subsequent filter.
            #   It probably should, because doing so reduces combinatorics.
            ###################################################################
            combineTopo4Body = Hlt2Member( CombineParticles
                                        , 'Combine'
                                        , DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
                                        , InputLocations = [ TopoTFInputParticles, TopoTF3Body ]

                                        , CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
                                        , MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2>100)"
                                         )

            topo4Body = bindMembers( 'TopoTF4Body', [ TopoTFInputParticles, TopoTF3Body, combineTopo4Body ] )

            filter = tfFilter(topo4Body)
            postTF = bindMembers( 'PostTFTopo4Body', [topo4Body, filter] )
            return postTF


        #Just for readability, add the refit options here as well.
        importOptions( "$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        ###################################################################
        # Robust sequences
        ###################################################################
        robust2BodySeq = robustTopo2Body()
        robust3BodySeq = robustTopo3Body()
        robust4BodySeq = robustTopo4Body()
        robustNBodySeq = {  'Topo2Body' : robust2BodySeq
                          , 'Topo3Body' : robust3BodySeq
                          , 'Topo4Body' : robust4BodySeq
                         }
        # Same sequenes, different names.
        robustNReqSeq = {  'Req2Yes' : robust2BodySeq
                         , 'Req3Yes' : robust3BodySeq
                         , 'Req4Yes' : robust4BodySeq
                        }



        ###################################################################
        # 'Factory' for standalone monitoring lines for the robust topological. 
        # Heavily post-scaled.
        ###################################################################
        for robSeq in robustNBodySeq.keys() :
            lineName = robSeq + 'SA'

            line = Hlt2Line(lineName
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ robustNBodySeq[robSeq] ]
                )
            self.updateHltANNSvc(lineName)


        ###################################################################
        # Post-track-fit sequences
        ###################################################################
        tf2BodySeq = tfTopo2Body()
        tf3BodySeq = tfTopo3Body()
        tf4BodySeq = tfTopo4Body()
        tfNBodySeq = {  'TopoTF2Body' : tf2BodySeq
                      , 'TopoTF3Body' : tf3BodySeq
                      , 'TopoTF4Body' : tf4BodySeq
                     }


        ###################################################################
        # 'Factory' for standalone monitoring lines for the post-TF topological.
        # Heavily pre-scaled.
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            lineName = tfSeq + 'SA'

            line = Hlt2Line(lineName
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ tfNBodySeq[tfSeq] ]
                )
            self.updateHltANNSvc(lineName)



        ###################################################################
        # 'Factory' for main topological lines
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            for robSeq in robustNReqSeq.keys() :
                lineName = tfSeq + robSeq

                line = Hlt2Line(lineName
                         , prescale = self.prescale
                         , postscale = self.postscale
                         , algos = [ robustNReqSeq[robSeq], tfNBodySeq[tfSeq] ]
                        )
                self.updateHltANNSvc(lineName)






        ##
        #
        # Toplogical for charm, 2-body
        #
        ##

        #Just for readability, add the refit options here as well.
        importOptions( "$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        ###################################################################
        # Concrete instance for 2-body charm robust stage filtering
        ###################################################################
        filterTopo2BodyCharm = FilterDesktop('Hlt2FilterTopo2BodyCharm'
                                    , InputLocations = [Topo2Body.outputSelection()]
                                    , Code = "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmRobustPointUL') + ")"
                                    )

        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = [TopoTF2Body]
                            , Code = "(M > 1839*MeV) & (M < 1889*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")"
                           )
        ##
        #
        # Define the 2-body charm lines
        #
        ##
        ###################################################################
        # Main 2-body charm line
        ###################################################################
        line = Hlt2Line('TopoTF2BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ Topo2Body, filterTopo2BodyCharm, TopoTFInputParticles, TopoTF2Body, filter]
                       )
        self.updateHltANNSvc('Topo2BodyCharmSA')

        ###################################################################
        # Monitoring line for 2-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo2BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ Topo2Body, filterTopo2BodyCharm ]
                  )
        self.updateHltANNSvc('TopoTF2BodyCharmSignal')

        ###################################################################
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF2BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' : "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")" }
                  )
        self.updateHltANNSvc('TopoTF2BodyCharmWideMass')


        ##
        #
        # Toplogical for charm, 3-body
        #
        ##
        ###################################################################
        # Concrete instance for 3-body charm robust stage filtering
        ###################################################################
        filterTopo3BodyCharm = filterTopo2BodyCharm.clone('Hlt2FilterTopo3BodyCharm')
        filterTopo3BodyCharm.InputLocations = [Topo3Body.outputSelection()]


        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter  = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [TopoTF3Body]
                             , Code = "(((M > 1844*MeV) & (M < 1894*MeV)) | ((M > 1943.) & (M < 1993.)))& (BPVTRGPOINTINGWPT<0.1)"
                            )


        ##
        #
        # Define the 3-body charm lines
        #
        ##
        ###################################################################
        # Main 3-body charm line
        ###################################################################
        line = Hlt2Line('TopoTF3BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ Topo3Body, filterTopo3BodyCharm, TopoTFInputParticles, TopoTF3Body, filter ]
                       )
        self.updateHltANNSvc('Topo3BodyCharmSA')

        ###################################################################
        # Monitoring line for 3-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo3BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ Topo3Body, filterTopo3BodyCharm ]
                  )
        self.updateHltANNSvc('TopoTF3BodyCharmSignal')

        ###################################################################
        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF3BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' : "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<0.1)" }
                  )
        self.updateHltANNSvc('TopoTF3BodyCharmWideMass')


        ##
        #
        # Toplogical for charm, 4-body
        #
        ##
        ###################################################################
        # Concrete instance for 4-body charm robust stage filtering
        ###################################################################
        combine = CombineParticles('Hlt2CombineTopo4BodyCharm'
                          , DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
                          , InputLocations = [ GoodPions.outputSelection(), Topo3Body.outputSelection() ]
                          , CombinationCut = "(AM > 1700*MeV) & (AM < 2100*MeV) & (AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
                          , MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVD>2) & (BPVVDR > 0.2) & (BPVTRGPOINTINGWPT<" + _cut('CharmRobustPointUL') + ")"
                          )

        ###################################################################
        # Concrete instance for 4-body charm post-TF combinatorics
        ###################################################################
        combineTF = CombineParticles('Hlt2CombineTopoTF4BodyCharm'
                            , DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
                            , InputLocations = [ TopoTFInputParticles.outputSelection(), TopoTF3Body.outputSelection() ]
                            , CombinationCut = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<1.0) & AALLSAMEBPV"
                            , MotherCut = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>1.5*GeV) & (BPVVDCHI2 > 100)"
                            )

        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter = Hlt2Member( FilterDesktop, 'Filter'
                   , InputLocations = [combineTF]
                   , Code = "(M > 1839*MeV) & (M < 1889*MeV) & (BPVTRGPOINTINGWPT<0.1)"
                   )

        ##
        #
        # Define the 4-body charm lines
        #
        ##
        ###################################################################
        # Main 4-body charm line
        ###################################################################
        line = Hlt2Line('TopoTF4BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ GoodPions, Topo3Body, combine, TopoTFInputParticles, TopoTF3Body, combineTF, filter ]
                       )
        self.updateHltANNSvc('Topo4BodyCharmSA')

        ###################################################################
        # Monitoring line for 4-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo4BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ GoodPions, Topo3Body, combine ]
                  )
        self.updateHltANNSvc('TopoTF4BodyCharmSignal')

        ###################################################################
        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF4BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' :  "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<0.1)" }
                  )
        self.updateHltANNSvc('TopoTF4BodyCharmWideMass')

