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
    __slots__ = { 'ComRobAllTrkPtLL'        : 300    # in MeV
                , 'ComRobAllTrkPLL'         : 2000    # in MeV
                , 'ComRobAllTrkPVIPLL'      : 0.05    # in mm
                , 'ComRobPairMinDocaUL'     : 0.10    # in mm
                , 'ComRobPairMaxDocaUL'     : 1    # in mm
                , 'ComRobTrkMaxPtLL'        : 1500   # in MeV
                , 'ComRobVtxPVDispLL'       : 2      # in mm
                , 'ComRobVtxPVRDispLL'      : 0.2      # in mm
                , 'RobustPointingUL'        : 0.20       # unitless
                , 'ComTFAllTrkPtLL'         : 300    # in MeV
                , 'ComTFAllTrkPLL'          : 2000    # in MeV
                , 'ComTFAllTrkPVIPChi2LL'   : 9    # unitless
                , 'ComTFAllTrkChi2UL'       : 10    # unitless
                , 'ComTFPairMinDocaUL'      : 0.10    # in mm
                , 'ComTFPairMaxDocaUL'      : 1    # in mm
                , 'ComTFTrkMaxPtLL'         : 1500   # in MeV
                , 'ComTFVtxPVDispChi2LL'    : 100      # unitles
                , 'TFPointUL'               : 0.10       # unitless
                , 'CharmRobustPointUL'      : 0.10       # unitless
                , 'CharmTFPointUL'          : 0.10       # unitless
                , 'DXRobustCoplanUL'        : 0.10       # unitless
                , 'DXTFPointUL'             : 0.20       # unitless
                , 'Prescale'                : {'Hlt2TopoTF2BodySA' : 0.001
                                               , 'Hlt2TopoTF3BodySA' : 0.001
                                               , 'Hlt2TopoTF4BodySA' : 0.001
                                               , 'Hlt2TopoTF2BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF3BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF4BodyCharmWideMass' : 0.05
                                               , 'Hlt2DX3BodyCopNoKs' : 0.01
                                               , 'Hlt2DX4BodyCopNoKs' : 0.01
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
                                   # Charm lines
                                   , 'Hlt2Topo2BodyCharmSADecision'   : 50850
                                   , 'Hlt2TopoTF2BodyCharmSignalDecision' : 50880
                                   , 'Hlt2TopoTF2BodyCharmWideMassDecision' : 50890
                                   , 'Hlt2Topo3BodyCharmSADecision'   : 50860
                                   , 'Hlt2TopoTF3BodyCharmSignalDecision' : 50910
                                   , 'Hlt2TopoTF3BodyCharmWideMassDecision' : 50920
                                   , 'Hlt2Topo4BodyCharmSADecision'   : 50870
                                   , 'Hlt2TopoTF4BodyCharmSignalDecision' : 50950
                                   , 'Hlt2TopoTF4BodyCharmWideMassDecision' : 50960
                                   # DX lines.  Here temporarily.
                                   , 'Hlt2DX3BodyCopKsDDDecision'     : 62000
                                   , 'Hlt2DX4BodyCopKsDDDecision'     : 62010
                                   , 'Hlt2DX3BodyCopNoKsDecision'     : 62020
                                   , 'Hlt2DX4BodyCopNoKsDecision'     : 62030
                                   }
                }



    def updateHltANNSvc(self,line) :
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )

    def __apply_configuration__(self) :
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles


        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))


        ###################################################################
        # Absorb the shared particle reconstruction into this configurable.
        #
        # This needs to be cleaned up.
        ###################################################################
        ###################################################################
        # Construct a combined sequence for the input particles.
        ###################################################################
        orInput = GaudiSequencer('Hlt2SharedTopo2Body_PionsORKaons'
                         , Members =  GoodPions.members() + GoodKaons.members()
                         , ModeOR = True
                         , ShortCircuit = False
                         )

        str_allpt = "(PT>" + _cut('ComRobAllTrkPtLL') + "*MeV)"
        str_allp  = "(P>" + _cut('ComRobAllTrkPLL') + "*MeV)"
        str_allip = "(MIPDV(PRIMARY)>" + _cut('ComRobAllTrkPVIPLL') + ")"
        daugcuts = str_allpt + "&" + str_allp + "&" + str_allip
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = [GoodPions, GoodKaons]
                            , Code = daugcuts
                           )
        lclInputParticles = bindMembers( 'TopoInputParticles', [ orInput, filter ] )



        ###################################################################
        # Function to configure common particle combinations used by inclusive
        #   hadronic lines.
        # Configuring these combinations here is more flexible than
        #   constructing them in Hlt2SharedParticles.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        ###################################################################
        def robustCombine(name, inputSeq, decayDesc) :
            # Construct a cut string for the combination.
            str_cmbdoca = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComRobPairMaxDocaUL') + ")"
            # Special case for the 2-body.  Use tighter doca requirement.
            if (name.find('Topo2Body') <> -1) :
                str_cmbdoca = "(AMINDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComRobPairMinDocaUL') + ")"
            combcuts = str_cmbdoca + "&(AALLSAMEBPV)"
            # Special case for the 4-body.  Use a mass lower limit to reduce
            #    combinatorics.  Must be at or below any client mass LL.
            if (name.find('Topo4Body') <> -1) :
                combcuts = combcuts + "&(AM>1700*MeV)"

            # Construct a cut string for the vertexed combination.
            str_parpt = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>" + _cut('ComRobTrkMaxPtLL') + "*MeV)"
            str_pardisp = "(BPVVD>" + _cut('ComRobVtxPVDispLL') + ")"
            str_parrdisp = "(BPVVDR>" + _cut('ComRobVtxPVRDispLL') + ")"
            parentcuts = str_parpt + "&" + str_pardisp + "&" + str_parrdisp

            combineTopoNBody = Hlt2Member( CombineParticles
                                        , 'Combine'
                                        , DecayDescriptors = decayDesc
                                        , InputLocations = inputSeq
                                        , CombinationCut = combcuts
                                        , MotherCut = parentcuts
                                         )

            topoNBody = bindMembers( name, inputSeq + [ combineTopoNBody ] )
            return topoNBody


        ###################################################################
        # CombineParticles for the 2-body combinations.
        ###################################################################
        topo2Body = robustCombine(  name = 'TmpTopo2Body'
                                  , inputSeq = [ lclInputParticles ]
                                  , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"])

        ###################################################################
        # CombineParticles for the 3-body combinations.
        ###################################################################
        topo3Body = robustCombine(  name = 'TmpTopo3Body'
                                  , inputSeq = [ lclInputParticles, topo2Body ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"])

        ###################################################################
        # CombineParticles for the 4-body combinations.
        # It may be better to make a special case for the 4-body in which
        #   some cuts are applied.
        ###################################################################
        topo4Body = robustCombine(  name = 'Topo4Body'
                                  , inputSeq = [lclInputParticles, topo3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"])


        ###################################################################
        # Function to configure a robust filter for the topological.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def robustFilter(inputSeq) :
            # Build a cut string from the configuration variables.
            str_mass = "(M > 4*GeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('RobustPointingUL') + ")"
            filter = Hlt2Member( FilterDesktop
                            , 'RobustFilter'
                            , InputLocations = [inputSeq]
                            , Code = str_mass + "&" + str_point
                               )
            return filter


        ###################################################################
        # Construct a bindMember for the topological robust 2-body decision
        ###################################################################
        filter = robustFilter(topo2Body)
        robust2BodySeq = bindMembers( 'RobustTopo2Body', [topo2Body, filter] )

        ###################################################################
        # Construct a bindMember for the topological robust 3-body decision
        ###################################################################
        filter = robustFilter(topo3Body)
        robust3BodySeq = bindMembers( 'RobustTopo3Body', [topo3Body, filter] )


        ###################################################################
        # Construct a bindMember for the topological robust 4-body decision
        ###################################################################
        filter = robustFilter(topo4Body)
        robust4BodySeq = bindMembers( 'RobustTopo4Body', [topo4Body, filter] )



        ###################################################################
        # Now absorb the post-track fit shared particles into this configurable
        # This largely repeats the work of the robust on a different set
        #   of input particles.
        ###################################################################

        ###################################################################
        # (Re)filter the post-track-fit input particles
        ###################################################################
        str_allpt = "(PT>" + _cut('ComTFAllTrkPtLL') + "*MeV)"
        str_allp  = "(P>" + _cut('ComTFAllTrkPLL') + "*MeV)"
        str_allipchi2 = "(MIPCHI2DV(PRIMARY)>" + _cut('ComTFAllTrkPVIPChi2LL') + ")"
        str_alltrkchi2 = "(TRCHI2DOF<" + _cut('ComTFAllTrkChi2UL') + ")"
        incuts = str_allpt + "&" + str_allp + "&" + str_allipchi2 + "&" + str_alltrkchi2

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = [TopoTFInputParticles]
                            , Code = incuts
                           )
        lclTFInputParticles = bindMembers( 'TopoTFIn', [ TopoTFInputParticles, filter ] )



        ###################################################################
        # Function to configure common particle combinations used by inclusive
        #   hadronic lines.
        # Configuring these combinations here is more flexible than
        #   constructing them in Hlt2SharedParticles.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        ###################################################################
        def tfCombine(name, inputSeq, decayDesc) :
            # Construct a cut string for the combination.
            str_cmbdoca = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComTFPairMaxDocaUL') + ")"
            # Special case for the 2-body.  Use tighter doca requirement.
            if (name.find('TopoTF2Body') <> -1) :
                str_cmbdoca = "(AMINDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComTFPairMinDocaUL') + ")"
            combcuts = str_cmbdoca + "&(AALLSAMEBPV)"
            # Special case for the 4-body.  Use a mass lower limit to reduce
            #    combinatorics.  Must be at or below any client mass LL.
            if (name.find('4Body') <> -1) :
                combcuts = combcuts + "&(AM>1700*MeV)"

            # Construct a cut string for the vertexed combination.
            str_parpt = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>" + _cut('ComTFTrkMaxPtLL') + "*MeV)"
            str_pardisp = "(BPVVDCHI2>" + _cut('ComTFVtxPVDispChi2LL') + ")"
            parentcuts = str_parpt + "&" + str_pardisp

            combineTopoNBody = Hlt2Member( CombineParticles
                                        , 'Combine'
                                        , DecayDescriptors = decayDesc
                                        , InputLocations = inputSeq
                                        , CombinationCut = combcuts
                                        , MotherCut = parentcuts
                                         )

            topoNBody = bindMembers( name, inputSeq + [ combineTopoNBody ] )
            return topoNBody



        ###################################################################
        # Function to configure a post-track-fit filter for the topological.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def tfFilter(inputSeq) :
            str_mass = "(M > 4*GeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('TFPointUL') + ")"
            filter = Hlt2Member( FilterDesktop
                            , 'TFFilter'
                            , InputLocations = [inputSeq]
                            , Code = str_mass + "&" + str_point
                               )
            return filter


        ###################################################################
        # post-track-fit 2-body combinations
        ###################################################################
        topoTF2Body = tfCombine(  name = 'TmpTopoTF2Body'
                                , inputSeq = [ lclTFInputParticles ]
                                , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"])

        ###################################################################
        # post-track-fit 3-body combinations
        ###################################################################
        topoTF3Body = tfCombine(  name = 'TmpTopoTF3Body'
                                , inputSeq = [ lclTFInputParticles, topoTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                               )

        ###################################################################
        # Construct a bindMember for the topological post-TF 4-body that
        # serves the same purpose as the shared particles TopoTF2Body....
        ###################################################################
        topoTF4Body = tfCombine(  name = 'TopoTF4Body'
                                , inputSeq = [ lclTFInputParticles, topoTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                               )


        ###################################################################
        # Construct a bindMember for the topological post-TF 2-body decision
        ###################################################################
        filter = tfFilter(topoTF2Body)
        tf2BodySeq = bindMembers( 'PostTFTopo2Body', [topoTF2Body, filter] )

        ###################################################################
        # Construct a bindMember for the topological post-TF 3-body decision
        ###################################################################
        filter = tfFilter(topoTF3Body)
        tf3BodySeq = bindMembers( 'PostTFTopo3Body', [topoTF3Body, filter] )

        ###################################################################
        # Construct a bindMember for the topological post-TF 4-body decision
        ###################################################################
        filter = tfFilter(topoTF4Body)
        tf4BodySeq = bindMembers( 'PostTFTopo4Body', [topoTF4Body, filter] )


        ###################################################################
        # Robust sequences
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
        # Toplogical for charm
        #
        ##
        ###################################################################
        # Function to configure a robust filter for the charm topo.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def charmRobustFilter(inputSeq) :
            filter = Hlt2Member( FilterDesktop
                            , 'CharmRobustFilter'
                            , InputLocations = [inputSeq]
                            , Code = "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmRobustPointUL') + ")"
                               )
            return filter

        ###################################################################
        # Construct a bindMember for the charm topo robust 2-body decision
        ###################################################################
        filter = charmRobustFilter(topo2Body)
        charmRobustTopo2BodySeq = bindMembers( 'CharmRobustTopo2Body', [topo2Body, filter] )

        ###################################################################
        # Construct a bindMember for the charm topo robust 3-body decision
        ###################################################################
        filter = charmRobustFilter(topo3Body)
        charmRobustTopo3BodySeq = bindMembers( 'CharmRobustTopo3Body', [topo3Body, filter] )

        ###################################################################
        # Construct a bindMember for the charm topo robust 4-body decision
        ###################################################################
        filter = charmRobustFilter(topo4Body)
        charmRobustTopo4BodySeq = bindMembers( 'CharmRobustTopo4Body', [topo4Body, filter] )



        ##
        #
        # Define the 2-body charm lines
        #
        ##
        ###################################################################
        # Concrete instance for 2-body charm robust stage filtering
        ###################################################################

        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = [topoTF2Body]
                            , Code = "(M > 1839*MeV) & (M < 1889*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")"
                           )

        ###################################################################
        # Main 2-body charm line
        ###################################################################
        line2Body = Hlt2Line('TopoTF2BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ charmRobustTopo2BodySeq, topoTF2Body, filter ]
                       )
        self.updateHltANNSvc('TopoTF2BodyCharmSignal')

        ###################################################################
        # Monitoring line for 2-body charm robust.  Heavily post-scaled.
        ###################################################################
        line2Body.clone('Topo2BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ charmRobustTopo2BodySeq ]
                  )
        self.updateHltANNSvc('Topo2BodyCharmSA')

        ###################################################################
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line2Body.clone( 'TopoTF2BodyCharmWideMass'
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


        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter  = Hlt2Member( FilterDesktop
                             , 'Filter'
                             , InputLocations = [topoTF3Body]
                             , Code = "(((M > 1844*MeV) & (M < 1894*MeV)) | ((M > 1943.) & (M < 1993.)))& (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")"
                            )

        ###################################################################
        # Main 3-body charm line
        ###################################################################
        line3Body = Hlt2Line('TopoTF3BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ charmRobustTopo3BodySeq, topoTF3Body, filter ]
                       )
        self.updateHltANNSvc('TopoTF3BodyCharmSignal')

        ###################################################################
        # Monitoring line for 3-body charm robust.  Heavily post-scaled.
        ###################################################################
        line3Body.clone('Topo3BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ charmRobustTopo3BodySeq ]
                  )
        self.updateHltANNSvc('Topo3BodyCharmSA')

        ###################################################################
        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line3Body.clone( 'TopoTF3BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' : "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")" }
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

        ###################################################################
        # Hlt2Member instance for tiling mass instances
        ###################################################################
        filter = Hlt2Member( FilterDesktop, 'Filter'
                   , InputLocations = [topoTF4Body]
                   , Code = "(M > 1839*MeV) & (M < 1889*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")"
                   )

        ##
        #
        # Define the 4-body charm lines
        #
        ##
        ###################################################################
        # Main 4-body charm line
        ###################################################################
        line4Body = Hlt2Line('TopoTF4BodyCharmSignal'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ charmRobustTopo4BodySeq, topoTF4Body, filter ]
                       )
        self.updateHltANNSvc('Topo4BodyCharmSA')

        ###################################################################
        # Monitoring line for 4-body charm robust.  Heavily post-scaled.
        ###################################################################
        line4Body.clone('Topo4BodyCharmSA'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , algos = [ charmRobustTopo4BodySeq ]
                  )
        self.updateHltANNSvc('TopoTF4BodyCharmSignal')

        ###################################################################
        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        line4Body.clone( 'TopoTF4BodyCharmWideMass'
                   , prescale = self.prescale
                   , postscale = self.postscale
                   , Filter = { 'Code' :  "(M > 1700*MeV) & (M < 2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")" }
                  )
        self.updateHltANNSvc('TopoTF4BodyCharmWideMass')




        ##
        #
        # DX inclusive lines
        #
        ##
        from Hlt2SharedParticles.V0 import KsDDFit


        ###################################################################
        # Function to configure a robust filter for the inclusive DX.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def dxRobustFilter(inputSeq) :
            filter = Hlt2Member( FilterDesktop
                            , 'DXRobustFilter'
                            , InputLocations = [inputSeq]
                            , Code = "(M>2000*MeV) & (M<6000*MeV) & (BPVDVDOCA()<" + _cut('DXRobustCoplanUL') + ")"
                               )
            return filter

        ###################################################################
        # Construct a bindMember for the DX robust 3-body decision
        ###################################################################
        filter = dxRobustFilter(topo3Body)
        dxRobust3BodySeq = bindMembers( 'DXRobust3BodyCop', [topo3Body, filter] )

        ###################################################################
        # Construct a bindMember for the DX robust 4-body decision
        # Note that the original DX trigger had a different set of decay
        #   descriptors that included
        #   ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
        # The default 4-body construction in topo4Body currently does not.
        ###################################################################
        filter = dxRobustFilter(topo4Body)
        dxRobust4BodySeq = bindMembers( 'DXRobust4BodyCop', [topo4Body, filter] )


        ###################################################################
        # DX 2-body combinations with a Ks.
        # This replaces the shared particles DXTF2BodyKsDD
        # Performance of this must be checked.  The original DXTF2BodyKsDD
        #   has no doca requirement.  This will default to the
        #   ComRobPairMaxDocaUL cut.
        ###################################################################
        dxTF2BodyKsDD = tfCombine(  name = 'TmpDXTF2BodyKsDD'
                                  , inputSeq = [ lclTFInputParticles, KsDDFit ]
                                  , decayDesc = ["K*(892)0 -> KS0 pi+","K*(892)0 -> KS0 pi-","K*(892)0 -> KS0 K+","K*(892)0 -> KS0 K-"]
                                  )

        ###################################################################
        # DX 3-body combinations with a Ks.
        # This replaces the shared particles DXTF3BodyKsDD
        ###################################################################
        dxTF3BodyKsDD = tfCombine(  name = 'TmpDXTF3BodyKsDD'
                                  , inputSeq = [ lclTFInputParticles, dxTF2BodyKsDD ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 pi+","D*(2010)+ -> K*(892)0 pi-","D*(2010)+ -> K*(892)0 K+","D*(2010)+ -> K*(892)0 K-"]
                                  )


        ###################################################################
        # DX 4-body combinations with a Ks.
        # In the original, the mass windows and the pointing cut were
        #   applied in this CombineParticles.  I have implemented it here
        #   in two stages, just for consistency among the reconstructions,
        #   but if it is a problem, the additional cuts should be reintroduced
        #   as a special case.
        ###################################################################
        dxTF4BodyKsDD = tfCombine(  name = 'TmpDXTF4BodyKsDD'
                                  , inputSeq = [ lclTFInputParticles, dxTF3BodyKsDD ]
                                  , decayDesc = ["B0 -> pi- D*(2010)+","B0 -> pi+ D*(2010)+","B0 -> K- D*(2010)+","B0 -> K+ D*(2010)+"]
                                  )

        ###################################################################
        # Function to configure a post-track-fit filter for the topological.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def dxTFFilter(inputSeq) :
            str_mass = "(M>2500*MeV) & (M<6000*MeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('DXTFPointUL') + ")"
            filter = Hlt2Member( FilterDesktop
                            , 'DXTFFilter'
                            , InputLocations = [inputSeq]
                            , Code = str_mass + "&" + str_point
                               )
            return filter


        ###################################################################
        # Construct a bindMember for the post-seeding DX 3-body
        ###################################################################
        filter = dxTFFilter(dxTF3BodyKsDD)
        dxTF3BodyKsDDSeq = bindMembers( 'DXPostTF3BodyKsDD', [dxTF3BodyKsDD, filter] )

        ###################################################################
        # Construct a bindMember for the post-seeding DX 4-body
        ###################################################################
        filter = dxTFFilter(dxTF4BodyKsDD)
        dxTF4BodyKsDDSeq = bindMembers( 'DXPostTF4BodyKsDD', [dxTF4BodyKsDD, filter] )


        ###################################################################
        # Construct a bindMember for the non-Ks 3-body
        ###################################################################
        filter = dxTFFilter(topoTF3Body)
        dxTF3BodySeq = bindMembers( 'DXPostTF3Body', [topoTF3Body, filter] )

        ###################################################################
        # Construct a bindMember for the non-Ks 4-body
        ###################################################################
        filter = dxTFFilter(topoTF4Body)
        dxTF4BodySeq = bindMembers( 'DXPostTF4Body', [topoTF4Body, filter] )




        ###################################################################
        #   The 3 body stuff it's 2 lines in total with coplanarity and w/wo ks
        ###################################################################
        lineDX = Hlt2Line('DX3BodyCopKsDD'
                        , prescale = self.prescale
                        , postscale = self.postscale
                        , algos = [ dxRobust3BodySeq, dxTF3BodyKsDDSeq ]
                        )
        self.updateHltANNSvc('DX3BodyCopKsDD')


        lineDX.clone( 'DX3BodyCopNoKs'
                     , prescale = self.prescale
                     , postscale = self.postscale
                     , algos = [ dxRobust3BodySeq, dxTF3BodySeq ] )
        self.updateHltANNSvc('DX3BodyCopNoKs')


        ###########################################################
        # The 4 body stuff it's 2 lines in total w/wo Ks #
        ###########################################################

        ##
        # Combine after the Track Fitting, with and without ks
        ##

        ##
        # 4 body lines. 2 lines in total w/wo ks
        ##

        lineDX.clone( 'DX4BodyCopKsDD'
                     , prescale = self.prescale
                     , postscale = self.postscale
                     , algos = [ dxRobust4BodySeq, dxTF4BodyKsDDSeq ])
        self.updateHltANNSvc('DX4BodyCopKsDD')

        lineDX.clone( 'DX4BodyCopNoKs'
                    , prescale = self.prescale
                    , postscale = self.postscale
                    , algos = [ dxRobust4BodySeq, dxTF4BodySeq ])
        self.updateHltANNSvc('DX4BodyCopNoKs')


