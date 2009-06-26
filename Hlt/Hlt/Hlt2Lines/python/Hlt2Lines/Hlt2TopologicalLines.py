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
    __slots__ = { 'ComRobAllTrkPtLL'        : 300        # in MeV
                , 'ComRobAllTrkPLL'         : 2000       # in MeV
                , 'ComRobAllTrkPVIPLL'      : 0.05       # in mm
                , 'ComRobPairMinDocaUL'     : 0.10       # in mm
                , 'ComRobPairMaxDocaUL'     : 1          # in mm
                , 'ComRobTrkMaxPtLL'        : 1500       # in MeV
                , 'ComRobVtxPVDispLL'       : 2          # in mm
                , 'ComRobVtxPVRDispLL'      : 0.2        # in mm
                , 'ComTFAllTrkPtLL'         : 300        # in MeV
                , 'ComTFAllTrkPLL'          : 2000       # in MeV
                , 'ComTFAllTrkPVIPChi2LL'   : 9          # unitless
                , 'ComTFAllTrkChi2UL'       : 10         # unitless
                , 'ComTFPairMinDocaUL'      : 0.10       # in mm
                , 'ComTFPairMaxDocaUL'      : 1          # in mm
                , 'ComTFTrkMaxPtLL'         : 1500       # in MeV
                , 'ComTFVtxPVDispChi2LL'    : 100        # unitless
                , 'RobustPointingUL'        : 0.20       # unitless
                , 'TFPointUL'               : 0.10       # unitless
                , 'CharmRobustPointUL'      : 0.10       # unitless
                , 'CharmTFPointUL'          : 0.10       # unitless
                , 'DXRobustCoplanUL'        : 0.02       # in mm
                , 'DXTFPointUL'             : 0.20       # unitless
                , 'Prescale'                : {'Hlt2TopoTF2BodySA' : 0.001
                                               , 'Hlt2TopoTF3BodySA' : 0.001
                                               , 'Hlt2TopoTF4BodySA' : 0.001
                                               , 'Hlt2TopoTF2BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF3BodyCharmWideMass' : 0.05
                                               , 'Hlt2TopoTF4BodyCharmWideMass' : 0.05
                                               , 'Hlt2DX3BodyCopNoKs' : 0.01
                                               , 'Hlt2DX4BodyCopNoKs' : 0.01
                                               , 'Hlt2DX4BodyLowRobCopNoKs' : 0.01
                                              }
                , 'Postscale'               : {'Hlt2Topo2BodySA' : 0.02
                                               , 'Hlt2Topo3BodySA' : 0.001
                                               , 'Hlt2Topo4BodySA' : 0.001
                                               , 'Hlt2Topo2BodyCharmSA' : 0.002
                                               , 'Hlt2Topo3BodyCharmSA' : 0.0005
                                               , 'Hlt2Topo4BodyCharmSA' : 0.0005
                                              }
                # I should have the ability to turn lines on and off.
                # I don't necessarily want to do this just with prescalers.
                , 'IncludeLines'  : {'Hlt2Topo2BodySA'              : True
                                   , 'Hlt2Topo3BodySA'              : True
                                   , 'Hlt2Topo4BodySA'              : False
                                   , 'Hlt2TopoTF2BodySA'            : True
                                   , 'Hlt2TopoTF3BodySA'            : True
                                   , 'Hlt2TopoTF4BodySA'            : False
                                   , 'Hlt2TopoTF2BodyReq2Yes'       : True
                                   , 'Hlt2TopoTF2BodyReq3Yes'       : True
                                   , 'Hlt2TopoTF2BodyReq4Yes'       : False
                                   , 'Hlt2TopoTF3BodyReq2Yes'       : True
                                   , 'Hlt2TopoTF3BodyReq3Yes'       : True
                                   , 'Hlt2TopoTF3BodyReq4Yes'       : False
                                   , 'Hlt2TopoTF4BodyReq2Yes'       : False
                                   , 'Hlt2TopoTF4BodyReq3Yes'       : False
                                   , 'Hlt2TopoTF4BodyReq4Yes'       : False
                                   # Charm lines
                                   , 'Hlt2Topo2BodyCharmSA'         : True
                                   , 'Hlt2TopoTF2BodyCharmSignal'   : True
                                   , 'Hlt2TopoTF2BodyCharmWideMass' : True
                                   , 'Hlt2Topo3BodyCharmSA'         : True
                                   , 'Hlt2TopoTF3BodyCharmSignal'   : True
                                   , 'Hlt2TopoTF3BodyCharmWideMass' : True
                                   , 'Hlt2Topo4BodyCharmSA'         : False
                                   , 'Hlt2TopoTF4BodyCharmSignal'   : False
                                   , 'Hlt2TopoTF4BodyCharmWideMass' : False
                                   # DX lines.  Here temporarily.
                                   , 'Hlt2DX3BodyCopKsDD'           : True
                                   , 'Hlt2DX4BodyCopKsDD'           : False
                                   , 'Hlt2DX3BodyCopNoKs'           : True
                                   , 'Hlt2DX4BodyCopNoKs'           : False
                                   , 'Hlt2DX4BodyLowRobCopKsDD'     : False
                                   , 'Hlt2DX4BodyLowRobCopNoKs'     : False
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
                                   , 'Hlt2DX4BodyLowRobCopKsDDDecision' : 62040
                                   , 'Hlt2DX4BodyLowRobCopNoKsDecision' : 62050
                                   }
                }



    def updateHltANNSvc(self,line) :
        lineName = 'Hlt2' + line + 'Decision'
        id = self._scale(lineName,'HltANNSvcID')
        HltANNSvc().Hlt2SelectionID.update( { lineName : id } )

    # The mechanism for deciding whether or not to include a line needs to be
    #    refined.
    def checkIncludeLine(self,line) :
        import re
        lclline = 'Hlt2' + line
        for (expr,value) in self.getProp('IncludeLines').iteritems() :
            if re.match(expr,lclline) : return value
        return False

    def __apply_configuration__(self) :
        from Configurables import FilterDesktop,CombineParticles
        from Hlt2SharedParticles.GoodParticles import GoodPions, GoodKaons
        from Hlt2SharedParticles.TopoTFInputParticles import TopoTFInputParticles


        ## alias to get the slot associated to a name
        _cut = lambda x: str(self.getProp(x))


        ###################################################################
        # Wrapper for line construction that checks if it is to be included,
        #   and registers it to the HltANNSvc.
        ###################################################################
        def makeLine(lineName, algos) :
            if self.checkIncludeLine(lineName) :
                line = Hlt2Line(lineName
                     , prescale = self.prescale
                     , postscale = self.postscale
                     , algos = algos
                    )
                self.updateHltANNSvc(lineName)



        ###################################################################
        # Absorb the shared particle reconstruction into this configurable.
        #
        # This needs to be cleaned up.
        ###################################################################
        ###################################################################
        # Construct a combined sequence for the input particles to the robust
        #   stage.
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
        #   hadronic lines.  It lashes the new CombineParticles to a
        #   bindMembers with its antecedents.
        # Configuring these combinations here is more flexible than
        #   constructing them in Hlt2SharedParticles.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        ###################################################################
	# There seems to be a lot of CPUT consumed in managing the large number
	#   of 4-body candidates.  The list needs to be as small as possible.
	def hackRobustParentCut() :
            str_topoCut = "(BPVTRGPOINTINGWPT<" + _cut('RobustPointingUL') + ")"
            str_charmCut = "(BPVTRGPOINTINGWPT<" + _cut('CharmRobustPointUL') + ")"
	    str_dxCut = "(BPVDVDOCA()<" + _cut('DXRobustCoplanUL') + ")"
	    hackCut = '(' + str_topoCut + '|' + str_charmCut + '|' + str_dxCut + ')'
	    return hackCut

        def robustCombine(name, inputSeq, decayDesc, extracuts = None) :
            # Construct a cut string for the combination.
            str_cmbdoca = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComRobPairMaxDocaUL') + ")"
            # Special case for the 2-body.  Use tighter doca requirement.
            if (name.find('Topo2Body') <> -1) :
                str_cmbdoca = "(AMINDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComRobPairMinDocaUL') + ")"
            combcuts = str_cmbdoca + "&(AALLSAMEBPV)"
            # Special case for the 4-body.  Use a mass lower limit to reduce
            #    combinatorics.  Must be at or below any client mass LL.
            if extracuts and extracuts.has_key('CombinationCut') :
                combcuts = combcuts + '&' + extracuts['CombinationCut']

            # Construct a cut string for the vertexed combination.
            str_parpt = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>" + _cut('ComRobTrkMaxPtLL') + "*MeV)"
            str_pardisp = "(BPVVD>" + _cut('ComRobVtxPVDispLL') + ")"
            str_parrdisp = "(BPVVDR>" + _cut('ComRobVtxPVRDispLL') + ")"
            parentcuts = str_parpt + '&' + str_pardisp + '&' + str_parrdisp
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


        ###################################################################
        # Function to configure a robust filter for the topological.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def robustFilter(name, inputSeq, extracode = None) :
            # Build a cut string from the configuration variables.
            str_mass = "(M > 4*GeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('RobustPointingUL') + ")"
            codestr = str_mass + "&" + str_point
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'RobustFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers( name, inputSeq + [ filter ] )
            return filterSeq


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
        str_allpt = "(PT>" + _cut('ComTFAllTrkPtLL') + "*MeV)"
        str_allp  = "(P>" + _cut('ComTFAllTrkPLL') + "*MeV)"
        str_allipchi2 = "(MIPCHI2DV(PRIMARY)>" + _cut('ComTFAllTrkPVIPChi2LL') + ")"
        str_alltrkchi2 = "(TRCHI2DOF<" + _cut('ComTFAllTrkChi2UL') + ")"
        incuts = str_allpt + "&" + str_allp + "&" + str_allipchi2 + "&" + str_alltrkchi2

        filter = Hlt2Member( FilterDesktop
                            , 'Filter'
                            , InputLocations = ['Hlt2TFPionsForTopo', 'Hlt2TFKaonsForTopo']
                            , Code = incuts
                           )
        lclTFInputParticles = bindMembers('TopoTFIn', [ GaudiSequencer('SeqHlt2TFParticlesForTopo'), filter ])


        ###################################################################
        # Function to configure post-track-fit common particle combinations
        #   used by inclusive hadronic lines.  It lashes the new
        #   CombineParticles to a bindMembers with its antecedents.
        # Configuring these combinations here is more flexible than
        #   constructing them in Hlt2SharedParticles.
        # Its arguments:
        #     name      : string name
        #     inputSeq  : list of input particle sequences
        #     decayDesc : list of string decay descriptors
        #     extracuts : dictionary of extra cuts to be applied.
        #                 Can include cuts at the CombinationCut or at
        #                 the MotherCut level.
        #                 e.g. : { 'CombinationCut' : '(AM>4*GeV)'
        #                        , 'MotherCut'      : '(BPVDIRA>0.5)' }
        ###################################################################
        def tfCombine(name, inputSeq, decayDesc, extracuts = None) :
            # Construct a cut string for the combination.
            str_cmbdoca = "(AMAXDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComTFPairMaxDocaUL') + ")"
            # Special case for the 2-body.  Use tighter doca requirement.
            if (name.find('TopoTF2Body') <> -1) :
                str_cmbdoca = "(AMINDOCA('LoKi::TrgDistanceCalculator')<" + _cut('ComTFPairMinDocaUL') + ")"
            combcuts = str_cmbdoca + "&(AALLSAMEBPV)"
            # Special case for the 4-body.  Use a mass lower limit to reduce
            #    combinatorics.  Must be at or below any client mass LL.
            if extracuts and extracuts.has_key('CombinationCut') :
                combcuts = combcuts + '&' + extracuts['CombinationCut']

            # Construct a cut string for the vertexed combination.
            str_parpt = "(MAXTREE((('pi+'==ABSID) | ('K+'==ABSID)) ,PT)>" + _cut('ComTFTrkMaxPtLL') + "*MeV)"
            str_pardisp = "(BPVVDCHI2>" + _cut('ComTFVtxPVDispChi2LL') + ")"
            parentcuts = str_parpt + "&" + str_pardisp
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


        ###################################################################
        # Function to configure a post-track-fit filter for the topological.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def tfFilter(name, inputSeq, extracode = None) :
            str_mass = "(M > 4*GeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('TFPointUL') + ")"
            codestr = str_mass + "&" + str_point
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'TFFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers( name, inputSeq + [ filter ] )
            return filterSeq



        ###################################################################
        # Create sequences for the shared combinatorics of the robust stages.
        ###################################################################

        # CombineParticles for the robust 2-body combinations.
        ###################################################################
        topo2Body = robustCombine(  name = 'TmpTopo2Body'
                                  , inputSeq = [ lclInputParticles ]
                                  , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-", "K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"])

        # CombineParticles for the robust 3-body combinations.
        ###################################################################
        topo3Body = robustCombine(  name = 'TmpTopo3Body'
                                  , inputSeq = [ lclInputParticles, topo2Body ]
                                  , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"])

        # CombineParticles for the robust 4-body combinations.
        # Unlike the 3-body and 4-body, apply a mass lower limit.
        # This will not usable in the charm, and will only cover part of
        #   the mass range for the DX robust stage.
        ###################################################################
        topo4Body = robustCombine(  name = 'Topo4Body'
                                  , inputSeq = [lclInputParticles, topo3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                  , extracuts = { 'CombinationCut' : '(AM>4*GeV)'
                                                , 'MotherCut'      : hackRobustParentCut() }
                                  )


        ###################################################################
        # Create full decision sequences for the robust stages of the
        #   topological trigger.
        # These will only be used in topological trigger lines.
        ###################################################################

        # Construct a bindMember for the topological robust 2-body decision
        ###################################################################
        robust2BodySeq = robustFilter('RobustTopo2Body', [topo2Body])

        # Construct a bindMember for the topological robust 3-body decision
        ###################################################################
        robust3BodySeq = robustFilter('RobustTopo3Body', [topo3Body])

        # Construct a bindMember for the topological robust 4-body decision
        ###################################################################
        robust4BodySeq = robustFilter('RobustTopo4Body', [topo4Body])


        ###################################################################
        # Create sequences for the shared combinatorics of the post-track-fit
        #   stages.
        ###################################################################

        # post-track-fit 2-body combinations
        ###################################################################
        topoTF2Body = tfCombine(  name = 'TmpTopoTF2Body'
                                , inputSeq = [ lclTFInputParticles ]
                                , decayDesc = ["K*(892)0 -> pi+ pi+", "K*(892)0 -> pi+ pi-", "K*(892)0 -> pi- pi-", "K*(892)0 -> K+ K+", "K*(892)0 -> K+ K-", "K*(892)0 -> K- K-", "K*(892)0 -> K+ pi-","K*(892)0 -> pi+ K-", "K*(892)0 -> K+ pi+", "K*(892)0 -> K- pi-"])

        # post-track-fit 3-body combinations
        ###################################################################
        topoTF3Body = tfCombine(  name = 'TmpTopoTF3Body'
                                , inputSeq = [ lclTFInputParticles, topoTF2Body ]
                                , decayDesc = ["D*(2010)+ -> K*(892)0 pi+", "D*(2010)+ -> K*(892)0 pi-"]
                               )

        # post-track-fit 4-body combinations
        # Unlike the 3-body and 4-body, apply a mass lower limit.
        # This will not usable in the charm, and will only cover part of
        #   the mass range for the DX robust stage.
        ###################################################################
        topoTF4Body = tfCombine(  name = 'TopoTF4Body'
                                , inputSeq = [ lclTFInputParticles, topoTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>4*GeV)' }
                               )


        ###################################################################
        # Create full decision sequences for the post-track-fit stages of the
        #   topological trigger.
        # These will only be used in topological trigger lines.
        ###################################################################

        # Construct a bindMember for the topological post-TF 2-body decision
        ###################################################################
        tf2BodySeq = tfFilter('PostTFTopo2Body', [topoTF2Body])

        # Construct a bindMember for the topological post-TF 3-body decision
        ###################################################################
        tf3BodySeq = tfFilter('PostTFTopo3Body', [topoTF3Body])

        # Construct a bindMember for the topological post-TF 4-body decision
        ###################################################################
        tf4BodySeq = tfFilter('PostTFTopo4Body', [topoTF4Body])


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
            makeLine(lineName, algos = [robustNBodySeq[robSeq]])


        ###################################################################
        # 'Factory' for standalone monitoring lines for the post-TF topological.
        # Heavily pre-scaled.
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            lineName = tfSeq + 'SA'
            makeLine(lineName, algos = [tfNBodySeq[tfSeq]] )


        ###################################################################
        # 'Factory' for main topological lines
        ###################################################################
        for tfSeq in tfNBodySeq.keys() :
            for robSeq in robustNReqSeq.keys() :
                lineName = tfSeq + robSeq
                makeLine(lineName, algos = [ robustNReqSeq[robSeq], tfNBodySeq[tfSeq] ])





        ##
        #
        # Toplogical for charm
        #
        ##
        ###################################################################
        # Function to configure a robust filter for the charm topo.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def charmRobustFilter(name, inputSeq, extracode = None) :
            codestr = "(M>1700*MeV) & (M<2100*MeV) & (BPVTRGPOINTINGWPT<" + _cut('CharmRobustPointUL') + ")"
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'CharmRobustFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers( name, inputSeq + [ filter ] )
            return filterSeq


        ###################################################################
        # Function to configure a post-track-fit filter for the charm.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a list of bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def charmTFFilter(name, inputSeq, extracode = None) :
            codestr = "(BPVTRGPOINTINGWPT<" + _cut('CharmTFPointUL') + ")"
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'TFFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers( name, inputSeq + [ filter ] )
            return filterSeq


        ###################################################################
        # Construct a bindMember for the charm topo robust 2-body decision
        ###################################################################
        charmRobustTopo2BodySeq = charmRobustFilter('CharmRobustTopo2Body', [topo2Body])

        # Construct a bindMember for the charm topo robust 3-body decision
        ###################################################################
        charmRobustTopo3BodySeq = charmRobustFilter('CharmRobustTopo3Body', [topo3Body])

        # Construct a bindMember for the charm topo robust 4-body decision
        # CombineParticles for the 4-body combinations.
        # Unlike the 3-body and 4-body, the charm cannot reuse the topo
        #   combinatorics because of the disjoint mass windows.
        ###################################################################
        charmRob4Body = robustCombine(  name = 'CharmRobust4Body'
                                  , inputSeq = [lclInputParticles, topo3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                  , extracuts = {'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)'
                                                , 'MotherCut'     : hackRobustParentCut() }
                                  )
        charmRobustTopo4BodySeq = charmRobustFilter('CharmRobustTopo4Body', [charmRob4Body])


        # Map charm robust sequences to monitoring line names
        ###################################################################
        charmRobustNBodySeq = {  'Topo2BodyCharmSA' : charmRobustTopo2BodySeq
                               , 'Topo3BodyCharmSA' : charmRobustTopo3BodySeq
                               , 'Topo4BodyCharmSA' : charmRobustTopo4BodySeq
                              }


        ###################################################################
        # 'Factory' for standalone monitoring lines for the robust stage of
        #   the charm trigger. 
        # Heavily post-scaled.
        ###################################################################
        for charmSeq in charmRobustNBodySeq.keys() :
            makeLine(charmSeq, algos = [charmRobustNBodySeq[charmSeq]] )


        ##
        #
        # Define the main charm lines
        #
        ##
        ###################################################################
        # Main 2-body charm post-track-fit sequence and line.
        ###################################################################
        charmTFTopo2BodySeq = charmTFFilter('CharmPostTF2Body'
                                     , [topoTF2Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')

        makeLine('TopoTF2BodyCharmSignal'
                 , algos = [ charmRobustTopo2BodySeq, charmTFTopo2BodySeq ])


        # Main 3-body charm post-track-fit sequence and line.
        ###################################################################
        charmTFTopo3BodySeq = charmTFFilter('CharmPostTF3Body'
                                     , [topoTF3Body]
                                     , extracode = '(((M>1844*MeV) & (M<1894*MeV)) | ((M>1943*MeV) & (M<1993*MeV)))')

        makeLine('TopoTF3BodyCharmSignal'
                 , algos = [ charmRobustTopo3BodySeq, charmTFTopo3BodySeq ])


        # Main 4-body charm post-track-fit sequence and line.
        ###################################################################
        charmTF4Body = tfCombine(  name = 'CharmTF4Body'
                                , inputSeq = [ lclTFInputParticles, topoTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>1839*MeV) & (AM<1889*MeV)' }
                                 )
        charmTFTopo4BodySeq = charmTFFilter('CharmPostTF4Body'
                                     , [charmTF4Body]
                                     , extracode = '(M>1839*MeV) & (M<1889*MeV)')

        makeLine('TopoTF4BodyCharmSignal'
                 , algos = [ charmRobustTopo4BodySeq, charmTFTopo4BodySeq])



        ##
        #
        # Define additional charm lines for mass sidebands.  Heavily prescaled.
        #
        ##
        # Line for 2-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTFTopo2BodySBSeq = charmTFFilter('CharmPostTF2BodyWideMass'
                                     , [topoTF2Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        makeLine('TopoTF2BodyCharmWideMass'
                 , algos = [ charmRobustTopo2BodySeq, charmTFTopo2BodySBSeq ])


        # Line for 3-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTFTopo3BodySBSeq = charmTFFilter('CharmPostTF3BodyWideMass'
                                     , [topoTF3Body]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        makeLine('TopoTF3BodyCharmWideMass'
                 , algos = [ charmRobustTopo3BodySeq, charmTFTopo3BodySBSeq ])


        # Line for 4-body charm mass sidebands.  Heavily pre-scaled.
        ###################################################################
        charmTF4BodySB = tfCombine(  name = 'CharmWMTF4Body'
                                , inputSeq = [ lclTFInputParticles, topoTF3Body ]
                                , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                , extracuts = { 'CombinationCut' : '(AM>1700*MeV) & (AM<2100*MeV)' }
                                 )
        charmTFTopo4BodySBSeq = charmTFFilter('CharmPostTF4BodyWideMass'
                                     , [charmTF4BodySB]
                                     , extracode = '(M>1700*MeV) & (M<2100*MeV)')

        makeLine('TopoTF4BodyCharmWideMass'
                 , algos = [ charmRobustTopo4BodySeq, charmTFTopo4BodySBSeq])



        ##
        #
        # DX inclusive lines
        #
        ##
        # Obtain the Ks -> DD
        from Hlt2SharedParticles.V0 import KsDDFit


        ###################################################################
        # Function to configure a robust filter for the inclusive DX.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def dxRobustFilter(name, inputSeq, extracode = None) :
            codestr = "(M>2000*MeV) & (M<6000*MeV) & (BPVDVDOCA()<" + _cut('DXRobustCoplanUL') + ")"
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'DXRobustFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers(name, inputSeq + [ filter ])
            return filterSeq


        ###################################################################
        # Function to configure a post-track-fit filter for the topological.
        #   It lashes the new FilterDesktop to a bindMembers with its
        #   antecedents.
        # The argument inputSeq should be a bindMember sequences that
        #   produces the particles to filter.
        ###################################################################
        def dxTFFilter(name, inputSeq, extracode = None) :
            str_mass = "(M>2500*MeV) & (M<6000*MeV)"
            str_point = "(BPVTRGPOINTINGWPT<" + _cut('DXTFPointUL') + ")"
            codestr = str_mass + '&' + str_point
            if extracode :
              codestr = codestr + '&' + extracode
            filter = Hlt2Member( FilterDesktop
                            , 'DXTFFilter'
                            , InputLocations = inputSeq
                            , Code = codestr
                               )
            filterSeq = bindMembers(name, inputSeq + [ filter ])
            return filterSeq



        # Construct a sequence for the DX robust 3-body decision.
        ###################################################################
        dxRobust3BodySeq = dxRobustFilter('DXRobust3BodyCop', [topo3Body])

        # Construct sequences for the DX robust 4-body decision.
        # Requiers 2 sequences:
        #   1) Sequence based on topo4Body, which has a mass window that
        #      covers only half of the DX robust mass window.
        #   2) Sequence based on a new filter that covers the remainder of
        #      the robust mass window.
        # This is done to prevent repetition of the 4-body combinatorics in
        #   the mass region where this overlaps with the topological.  This
        #   also leads to a duplication of lines, one based on each of the
        #   complementary robust mass regions.  This may become a problem if
        #   the complementary lines are independently prescaled....
        # Note that the original DX trigger had a different set of decay
        #   descriptors that included
        #   ["B0 -> D*(2010)+ K-","B0 -> D*(2010)+ K+"]
        # The default 4-body construction in topo4Body currently does not.
        ###################################################################
        dxRobust4BodySeq = dxRobustFilter('DXRobust4BodyCop', [topo4Body])

        # CombineParticles for the complementary 4-body combinations that
        #   covers the DX mass range not covered by robust topological.
        ###################################################################
        dx4BodyLow = robustCombine(  name = 'DXRobust4BodyLow'
                                  , inputSeq = [lclInputParticles, topo3Body ]
                                  , decayDesc = ["B0 -> D*(2010)+ pi-","B0 -> D*(2010)+ pi+"]
                                  , extracuts = {'CombinationCut' : '(AM>2000*MeV) & (AM<4000*MeV)'
                                                , 'MotherCut'      : hackRobustParentCut() }
                                  )
        dxRobust4BodyLowSeq = dxRobustFilter('DXRobust4BodyLowCop', [dx4BodyLow])


        ###################################################################
        # Sequences for new combinatorics with Ks candidates.
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
                                  , extracuts = { 'CombinationCut' : "(AM>2500*MeV) & (AM<6000*MeV)" }
                                 )


        ###################################################################
        # DX lines
        ###################################################################

        # Line for 3-body with Ks
        ###################################################################
        dxTF3BodyKsDDSeq = dxTFFilter('DXPostTF3BodyKsDD', [dxTF3BodyKsDD])
        makeLine('DX3BodyCopKsDD'
                 , algos = [ dxRobust3BodySeq, dxTF3BodyKsDDSeq ])


        # Lines for 4-body with Ks
        ###################################################################
        dxTF4BodyKsDDSeq = dxTFFilter('DXPostTF4BodyKsDD', [dxTF4BodyKsDD])
        makeLine('DX4BodyCopKsDD'
                 , algos = [ dxRobust4BodySeq, dxTF4BodyKsDDSeq ])

        makeLine('DX4BodyLowRobCopKsDD'
                 , algos = [ dxRobust4BodyLowSeq, dxTF4BodyKsDDSeq ])


        # Line for 3-body without Ks
        ###################################################################
        dxTF3BodySeq = dxTFFilter('DXPostTF3Body', [topoTF3Body])
        makeLine('DX3BodyCopNoKs'
                 , algos = [ dxRobust3BodySeq, dxTF3BodySeq ])

        # Lines for 4-body without Ks
        ###################################################################
        dxTF4BodySeq = dxTFFilter('DXPostTF4Body', [topoTF4Body])
        makeLine('DX4BodyCopNoKs'
                 , algos = [ dxRobust4BodySeq, dxTF4BodySeq ])

        makeLine('DX4BodyLowRobCopNoKs'
                 , algos = [ dxRobust4BodyLowSeq, dxTF4BodySeq ])


