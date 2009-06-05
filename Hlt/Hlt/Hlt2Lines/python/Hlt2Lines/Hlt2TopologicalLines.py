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
## 
from Gaudi.Configuration import * 
from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser
from HltLine.HltLine import Hlt2Line
from HltLine.HltLine import Hlt2Member

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :

    # steering variables
    #------------------------
    # Don't touch my variables!
    __slots__ = { 'RobustPointingUL'        : 0.2
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
                }

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.Topo2Body import Topo2Body
        from Hlt2SharedParticles.Topo3Body import Topo3Body
        from Hlt2SharedParticles.GoodParticles import GoodPions
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles
        from Hlt2SharedParticles.TopoTF2Body import TopoTF2Body
        from Hlt2SharedParticles.TopoTF3Body import TopoTF3Body


        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))


        ##
        #
        # Topological track-by-track
        #
        ##

        #Just for readability, add the refit options here as well.
        importOptions( "$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        # First pass: include the topological sequencing almost verbatim

        ##
        #
        # Filters for standard topological
        #
        ##
        ###################################################################
        # 2-body robust filter
        ###################################################################
        filterTopo2Body = FilterDesktop('Hlt2FilterTopo2Body'
                               , InputLocations = [Topo2Body.outputSelection()]
                               , Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.2)"
                               )

        ###################################################################
        # 3-body robust filter
        ###################################################################
        filterTopo3Body = filterTopo2Body.clone('Hlt2FilterTopo3Body')
        filterTopo3Body.InputLocations = [Topo3Body.outputSelection()]

        ###################################################################
        # 4-body robust filter
        # Clone the 3-body in the shared particles for common cuts
        # Does the configuration track?
        ###################################################################
        combineTopo4Body = CombineParticles("Hlt2SharedTopo3Body").clone('Hlt2CombineTopo4Body')
        combineTopo4Body.DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
        combineTopo4Body.InputLocations = [ GoodPions.outputSelection(), Topo3Body.outputSelection() ]
        combineTopo4Body.CombinationCut += "& (4*GeV<AM)"
        combineTopo4Body.MotherCut += "& (BPVTRGPOINTINGWPT<0.2)"


        ###################################################################
        # 2-body post-track-fit filter
        ###################################################################
        filterTopoTF2Body = FilterDesktop('Hlt2FilterTopoTF2Body')
        filterTopoTF2Body.InputLocations = [TopoTF2Body.outputSelection()]
        filterTopoTF2Body.Code = "(M > 4*GeV) & (BPVTRGPOINTINGWPT<0.1)"

        ###################################################################
        # 3-body post-track-fit filter
        ###################################################################
        filterTopoTF3Body = filterTopoTF2Body.clone('Hlt2FilterTopoTF3Body')
        filterTopoTF3Body.InputLocations = ["Hlt2SharedTopoTF3Body"]

        ###################################################################
        # 4-body post-track-fit filter
        # Clone the 3-body in the shared particles for common cuts
        # Does the configuration track?
        ###################################################################
        combineTopoTF4Body = CombineParticles("Hlt2SharedTopoTF3Body").clone('Hlt2CombineTopoTF4Body')
        combineTopoTF4Body.DecayDescriptors = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+"]
        combineTopoTF4Body.InputLocations = [ TopoTFInputParticles.outputSelection(), TopoTF3Body.outputSelection() ]
        combineTopoTF4Body.CombinationCut += "& (AM > 4*GeV)"
        combineTopoTF4Body.MotherCut += "& (BPVTRGPOINTINGWPT<0.1)"


        ##
        #
        # Standalone lines for the robust topological.  Heavily post-scaled.
        #
        ##
        ###################################################################
        # Line for the 2-body robust.
        ###################################################################
        Hlt2Line('Topo2BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo2Body, filterTopo2Body ]
                )

        ###################################################################
        # Line for the 3-body robust.
        ###################################################################
        Hlt2Line('Topo3BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo3Body, filterTopo3Body ]
                )

        ###################################################################
        # Line for the 4-body robust.
        ###################################################################
        Hlt2Line('Topo4BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ GoodPions, Topo3Body, combineTopo4Body ]
                )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo2BodySADecision" : 50700 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo3BodySADecision" : 50710 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo4BodySADecision" : 50720 } )


        ##
        #
        # Standalone lines for the post-TF topological.  Heavily pre-scaled.
        #
        ##
        ###################################################################
        # Line for monitoring 2-body post-track-fit.
        ###################################################################
        Hlt2Line('TopoTF2BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ TopoTFInputParticles, TopoTF2Body, filterTopoTF2Body ]
                )

        ###################################################################
        # Line for monitoring 3-body post-track-fit.
        ###################################################################
        Hlt2Line('TopoTF3BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ TopoTFInputParticles, TopoTF3Body, filterTopoTF3Body ]
                )

        ###################################################################
        # Line for monitoring 4-body post-track-fit.
        ###################################################################
        Hlt2Line('TopoTF4BodySA'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ TopoTFInputParticles, TopoTF3Body, combineTopoTF4Body ]
                )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodySADecision" : 50730 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodySADecision" : 50770 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodySADecision" : 50810 } )


        ##
        #
        # Lines with Any-body robust followed by 2-body post-TF topological.
        #
        ##
        Hlt2Line('TopoTF2BodyReq2Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo2Body, filterTopo2Body, TopoTFInputParticles, TopoTF2Body, filterTopoTF2Body ]
                )

        Hlt2Line('TopoTF2BodyReq3Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo3Body, filterTopo3Body, TopoTFInputParticles, TopoTF2Body, filterTopoTF2Body ]
                )

        Hlt2Line('TopoTF2BodyReq4Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ GoodPions, Topo3Body, combineTopo4Body, TopoTFInputParticles, TopoTF2Body, filterTopoTF2Body ]
                )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq2YesDecision" : 50740 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq3YesDecision" : 50750 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq4YesDecision" : 50760 } )


        ##
        #
        # Lines with Any-body robust followed by 3-body post-TF topological.
        #
        ##
        Hlt2Line('TopoTF3BodyReq2Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo2Body, filterTopo2Body, TopoTFInputParticles, TopoTF3Body, filterTopoTF3Body ]
                )

        Hlt2Line('TopoTF3BodyReq3Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo3Body, filterTopo3Body, TopoTFInputParticles, TopoTF3Body, filterTopoTF3Body ]
                )

        Hlt2Line('TopoTF3BodyReq4Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ GoodPions, Topo3Body, combineTopo4Body, TopoTFInputParticles, TopoTF3Body, filterTopoTF3Body ]
                )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq2YesDecision" : 50780 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq3YesDecision" : 50790 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq4YesDecision" : 50800 } )


        ##
        #
        # Lines with Any-body robust followed by 4-body post-TF topological.
        #
        ##
        Hlt2Line('TopoTF4BodyReq2Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo2Body, filterTopo2Body, TopoTFInputParticles, TopoTF3Body, combineTopoTF4Body ]
                )

        Hlt2Line('TopoTF4BodyReq3Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ Topo3Body, filterTopo3Body, TopoTFInputParticles, TopoTF3Body, combineTopoTF4Body ]
                )

        Hlt2Line('TopoTF4BodyReq4Yes'
                 , prescale = self.prescale
                 , postscale = self.postscale
                 , algos = [ GoodPions, Topo3Body, combineTopo4Body, TopoTFInputParticles, TopoTF3Body, combineTopoTF4Body ]
                )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq2YesDecision" : 50820 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq3YesDecision" : 50830 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq4YesDecision" : 50840 } )




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

        ###################################################################
        # Monitoring line for 2-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo2BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ Topo2Body, filterTopo2BodyCharm ]
                  )

        ###################################################################
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF2BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' : "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")" }
                  )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo2BodyCharmSADecision" : 50850 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyCharmSignalDecision" : 50880 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyCharmWideMassDecision" : 50890 } )


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

        ###################################################################
        # Monitoring line for 3-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo3BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ Topo3Body, filterTopo3BodyCharm ]
                  )

        ###################################################################
        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF3BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' : "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<0.1)" }
                  )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo3BodyCharmSADecision" : 50860 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmSignalDecision" : 50910 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmWideMassDecision" : 50920 } )


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

        ###################################################################
        # Monitoring line for 4-body charm robust.  Heavily post-scaled.
        ###################################################################
        line.clone('Topo4BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ GoodPions, Topo3Body, combine ]
                  )

        ###################################################################
        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line.clone( 'TopoTF4BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' :  "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<0.1)" }
                  )

        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo4BodyCharmSADecision" : 50870 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyCharmSignalDecision" : 50950 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyCharmWideMassDecision" : 50960 } )

