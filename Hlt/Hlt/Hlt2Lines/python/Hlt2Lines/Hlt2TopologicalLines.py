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

class Hlt2TopologicalLinesConf(HltLinesConfigurableUser) :

    def __apply_configuration__(self) :
        from Configurables import HltANNSvc
        ##
        #
        # Topological track-by-track
        #
        ##

        #Just for readability, add the refit options here as well.
        importOptions( "$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        ###
        #    ID 50700 - Topological 2-body, track-by-track, no track fit
        #    ID 50730-60 - Topological 2-body, track-by-track, after track fit
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo2BodyDEV.py" )
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopoTF2BodyDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo2BodyDEVSADecision" : 50700 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodySADEVDecision" : 50730 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq2YesDEVDecision" : 50740 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq3YesDEVDecision" : 50750 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyReq4YesDEVDecision" : 50760 } )

        ###
        #    ID 50710 - Topological 3-body, track-by-track, no track fit
        #    ID 50770-800 - Topological 3-body, track-by-track, after track fit
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo3BodyDEV.py" )
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopoTF3BodyDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo3BodyDEVSADecision" : 50710 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodySADEVDecision" : 50770 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq2YesDEVDecision" : 50780 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq3YesDEVDecision" : 50790 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyReq4YesDEVDecision" : 50800 } )

        ###
        #    ID 50720 - Topological 4-body, track-by-track, no track fit
        #    ID 50810-40 - Topological 4-body, track-by-track, after track fit
        ###
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo4BodyDEV.py" )
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopoTF4BodyDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo4BodyDEVSADecision" : 50720 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodySADEVDecision" : 50810 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq2YesDEVDecision" : 50820 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq3YesDEVDecision" : 50830 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyReq4YesDEVDecision" : 50840 } )

        ##
        #
        # Toplogical for charm
        #
        ##

        #Just for readability, add the refit options here as well.
        importOptions( "$HLTCONFROOT/options/Hlt2TrackFitForTopo.py")

        ###
        #    ID 50850 - Topological 2-body, charm mass, track-by-track, no track fit
        #    ID 50880-900 - Topological 2-body, charm mass, track-by-track, after track fit
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo2BodyCharmDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo2BodyCharmDEVSADecision" : 50850 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyCharmSignalDEVDecision" : 50880 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyCharmLowMassDEVDecision" : 50890 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF2BodyCharmHighMassDEVDecision" : 50900 } )

        ###
        #    ID 50860 - Topological 3-body, charm mass, track-by-track, no track fit
        #    ID 50910-940 - Topological 3-body, charm mass, track-by-track, after track fit
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo3BodyCharmDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo3BodyCharmDEVSADecision" : 50860 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmSignalDEVDecision" : 50910 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmLowMassDEVDecision" : 50920 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmMidMassDEVDecision" : 50930 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF3BodyCharmHighMassDEVDecision" : 50940 } )

        ###
        #    ID 50870 - Topological 4-body, charm mass, track-by-track, no track fit
        #    ID 50950-970 - Topological 4-body, charm mass, track-by-track, after track fit
        ##
        importOptions( "$HLTSELECTIONSROOT/options/Hlt2LineTopo4BodyCharmDEV.py" )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2Topo4BodyCharmDEVSADecision" : 50870 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyCharmSignalDEVDecision" : 50950 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyCharmLowMassDEVDecision" : 50960 } )
        HltANNSvc().Hlt2SelectionID.update( { "Hlt2TopoTF4BodyCharmHighMassDEVDecision" : 50970 } )

