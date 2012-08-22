"""
Configuration for the Exotic Displaced Vertex Stripping lines
"""

__author__ = ['Neal Gauvin', 'Victor Coco', "Veerle Heijne", "Pieter David"]
__date__   = '17/07/2012'

__all__ = ( 'DisplVerticesLinesConf' )

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from LHCbKernel.Configuration import DEBUG, VERBOSE

from PhysSelPython.Wrappers import AutomaticData, Selection
from StrippingConf.StrippingLine import StrippingLine, bindMembers
from StrippingUtils.Utils import LineBuilder

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles

from StrippingSelections.DisplVertices_Utils import SelectionPatPV3DWrapper

# constants
LLPLHCbName = "~chi_10"

config = {
        ## Velo tracks filter
          "FilterVelo"              : { "Apply"                : True
                                      , "MinIP"                : 0.1*units.mm
                                      , "MinIPChi2"            : -1.0
                                      , "MinNumTracks"         : 4
                                      , "PVLocation"           : "Rec/Vertex/Primary" ## TODO check if this makes sense
                                      , "RejectSplashEvents"   : False
                                      , "RemoveBackwardTracks" : True
                                      }

        #==========        SELECTION CUT VALUES         ==========#

        ## Velo reconstruction
        , "RV2PWithVelo"            : { "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  0.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      , "ApplyMatterVeto"      :  False
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : True
                                      , "FirstPVMaxRho"        : 0.3*units.mm
                                      , "FirstPVMinZ"          : -300.0*units.mm
                                      , "FirstPVMaxZ"          : 500.0*units.mm
                                      , "FirstPVMinNumTracks"  : 10
                                      }
        ## Single LLP line
        , "SingleSelection"         : { "PreScale"             :  1.0
                                                                 #       HighMass,         Medium,         HighFD
                                      , "MinRho"               : [  1.0*units.mm ,  2.*units.mm ,  4.*units.mm  ]
                                      , "MinMass"              : [ 8.*units.GeV,  6.5*units.GeV,  4.5*units.GeV ]
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         : [  8            ,  7            ,  6             ]
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.8
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        # prescaled
        , "SinglePSSelection"       : { "PreScale"             :  0.005
                                      , "MinRho"               :  0.5*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  0.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      :  False
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      }

        ## Double LLP line
        , "DoubleSelection"         : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.6*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      :  False
                                      , "MaxFractE1Track"      :  0.8
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Double only
                                      , "MinHighestMass"       : 3.0*units.GeV
                                      , "ApplyMatterVetoOne"   : True
                                      }

        ## Downstream reconstruction
        , "RV2PDown"                : { "MinRho"               :  2.0*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  0.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      , "ApplyMatterVeto"      :  False
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      , "FirstPVMaxRho"        : 0.3*units.mm
                                      , "FirstPVMinZ"          : -300.0*units.mm
                                      , "FirstPVMaxZ"          : 500.0*units.mm
                                      , "FirstPVMinNumTracks"  : 10
                                      }
        , "SingleDownSelection"     : { "PreScale"             :  1.0
                                      , "MinRho"               :  2.0*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  0.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      # Down only
                                      , "MinZ"                 :  170.*units.mm
                                      }

        #========== Other lines for efficiency studies ==========#
        , "HLTPS"             : { "PreScale"             :  1.0 }
        , "HltEffCharmHLTSelection"  : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.6*units.mm
                                      , "MinMass"              :  5.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  10.
                                      , "MaxFractTrwHitBefore" :  10.
                                      }

        #==========     HLT filters for all lines      ==========#
        , "HLT"                     : { "CharmHLT"     : "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')"
                                      , "HLTPS" : "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')"
                                      }
        }

class DisplVerticesLinesConf(LineBuilder):
    """
    Configuration for the Exotic Displaced Vertex Stripping lines

    The selection is achieved in 3 steps :
        - (optional) tracks not originating from a PV are selected,
          through a minimum IP cut
        - vertex reconstruction (PatPV3D with settings for low-multiplicity vertices)
        - LLParticlesFromRecVertices creates particles out of these vertices, applying some common cuts already (to keep timing low)
        - Each line applies cuts optimised for a different part of phasespace.
          "Single" lines add a FilterDesktop and "Double" lines a CombineParticles algorithm
    """
    __configuration_keys__ = (
              "FilterVelo"
            , "RV2PWithVelo"
            , "SingleSelection"
            , "SinglePSSelection"
            , "DoubleSelection"
            , "RV2PDown"
            , "SingleDownSelection"
            , "HLTPS"
            , "HltEffCharmHLTSelection"
            , "HLT"
            )

    def validatedGetProps(self, selName, propsToSet):
        """
        Get the __configuration_keys__[selName] dictionary, checking it contains all propsToSet
        """
        settings = self.configurationParameter(selName)
        for p in propsToSet:
            if not p in settings:
                print "Missing configuration parameter for %s : %s" % ( selName, p )
            assert p in settings
        return settings
    def validatedSetProps(self, selName, propsToSet, conf, ignoreIfMissing=False):
        """
        Helper method to validate (all propsToSet MUST be in __configuration_keys__[selName])
        and set all properties propNames of configurable conf
        """
        settings = self.validatedGetProps(selName, propsToSet)
        for p in propsToSet:
            if not ignoreIfMissing:
                setattr( conf, p, settings[p] )

    veloWithIPCuts = [ "MinIP", "MinIPChi2", "MinNumTracks", "PVLocation", "RejectSplashEvents", "RemoveBackwardTracks" ]
    singleCuts = [ "MinNumTracks", "MinRho", "MinMass", "MinSumPT", "ApplyMatterVeto", "MaxFractE1Track", "MaxFractTrwHitBefore" ]
    downCuts = [ "MinZ" ]
    doubleResonanceCuts = [ "MinHighestMass", "ApplyMatterVetoOne" ]
    recoCuts = [ "MaxChi2NonVeloOnly", "UseVeloTracks", "ComputeMatterVeto", "FirstPVMaxRho", "FirstPVMinZ", "FirstPVMaxZ", "FirstPVMinNumTracks" ]

    llpSelectionPreambulo = [ "from LoKiPhys.decorators import *"
                            , "from LoKiTracks.decorators import *"
                            , "INMATTER                           = ( INFO( 51, -1000.0 ) > 0.5 )"
                            , "ENDVERTEXRHO                       =   INFO( 52, -1000.0 )"
                            #, "VSIGMARHO                          =   INFO( 53, -1000.0 )"
                            #, "VSIGMAZ                            =   INFO( 54, -1000.0 )"
                            , "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT =   INFO( 55, -1000.0 )"
                            , "MAXFRACTENERGYINSINGLETRACK        =   INFO( 56, -1000.0 )"
                            , "SUMPT                              =   INFO( 57, -1000.0 )"
                            ]

    def getLLPSelection(self, cutValues):
        selCuts        = [ "~ISBASIC" ]
        selCutsConf    = { "MinMass"              : "MM > {cutVal}"
                         , "MinSumPT"             : "SUMPT > {cutVal}"
                         , "MinNumTracks"         : "NDAUGS >= {cutVal}"
                         , "MinRho"               : "ENDVERTEXRHO > {cutVal}"
                         , "MaxFractTrwHitBefore" : "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT < {cutVal}"
                         , "MaxFractE1Track"      : "MAXFRACTENERGYINSINGLETRACK < {cutVal}"
                         }
        if cutValues["ApplyMatterVeto"]:
            selCuts.insert(1, "~INMATTER")
        if "MinZ" in cutValues:
            selCutsConf["MinZ"] = "VFASPF(VZ) > {cutVal}"

        # Loop over cuts, remember which are complicated
        specificCuts = []
        for cut, funct in selCutsConf.iteritems():
            if hasattr(cutValues[cut], "__iter__"):
                specificCuts.append(cut)
            else:
                selCuts.append(funct.format(cutVal=cutValues[cut]))
        # concatenate AND
        commonCutsCode = "\n & ".join( "( %s )" % cutFunct for cutFunct in selCuts )
        if len(specificCuts) > 0:
            # Sanity check on "merged selection cut values"
            nPaths = len(cutValues[specificCuts[0]])
            for specCut in specificCuts[1:]:
                assert len(cutValues[specCut]) == nPaths
            # Construct and concatenate (AND) the merged selection paths
            specificCutsCodes = [ "( %s )" % " & ".join( "( %s )" % selCutsConf[cut].format(cutVal=cutValues[cut][i]) for cut in specificCuts ) for i in xrange(nPaths) ]
            # Return the total LoKi functor
            return "%s \n & ( %s )" % ( commonCutsCode, "\n   | ".join(specificCutsCodes) )
        else:
            return commonCutsCode

    def getResonanceSelection(self, cutValues):
        combCuts = [ "AHASCHILD( MM > %(MinHighestMass)s )"
                   ]
        if cutValues["ApplyMatterVetoOne"]:
            combCuts.insert(0, "AHASCHILD( ~INMATTER )")

        combCode = "\n & ".join( "( %s )" % ( cut % cutValues ) for cut in combCuts )

        motherCode = "ALL"

        return combCode, motherCode

    def __init__( self, name, config ):
        LineBuilder.__init__( self, name, config )

        #######################################################################
        ###                                                                 ###
        ###     VELO BASED VERTEXING SEQUENCE                               ###
        ###                                                                 ###
        #######################################################################

        bestTracks = AutomaticData("Rec/Track/Best")

        withVeloTracksForVertexing = bestTracks
        if self.configurationParameter("FilterVelo")["Apply"]:
            from GaudiConfUtils.ConfigurableGenerators import SelectVeloTracksNotFromPV

            veloWithIP = SelectVeloTracksNotFromPV()
            self.validatedSetProps( "FilterVelo", DisplVerticesLinesConf.veloWithIPCuts, veloWithIP )

            withVeloTracksForVertexing = Selection( "%sVeloFilteredTracks" % self.name()
                                           , RequiredSelections = [ bestTracks ]
                                           , Algorithm = veloWithIP
                                           )

        # Displaced Vertex reconstruction with best tracks (dominated by those with a Velo segment)
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, LSAdaptPVFitter

        withVeloVertexFinder = PVOfflineTool( "%sWithVeloVertexFinder" % self.name()
                                 , PVsChi2Separation = 0
                                 , PVsChi2SeparationLowMult = 0
                                 , PVSeedingName   = "PVSeed3DTool"
                                 , PVFitterName    = "LSAdaptPV3DFitter"
                                 )
        withVeloVertexFinder.addTool(PVSeed3DTool)
        withVeloVertexFinder.PVSeed3DTool.MinCloseTracks       = 3

        withVeloVertexFinder.addTool(LSAdaptPV3DFitter)
        withVeloVertexFinder.LSAdaptPV3DFitter.maxIP2PV        = 2.0*units.mm
        withVeloVertexFinder.LSAdaptPV3DFitter.MinTracks       = 4

        withVeloVertexAlg = PatPV3D( "%sWithVeloVertexAlg" )
        withVeloVertexAlg.addTool( withVeloVertexFinder, name="PVOfflineTool" )

        withVeloVertexing = SelectionPatPV3DWrapper( "%sWithVeloVertexing" % self.name()
                              , withVeloVertexAlg
                              , RequiredSelections = [ withVeloTracksForVertexing ]
                              )

        # Make Particles out of the RecVertices
        from GaudiConfUtils.ConfigurableGenerators import LLParticlesFromRecVertices

        rv2pWithVelo = LLParticlesFromRecVertices(
                           VerticesFromVeloOnly = False
                         , WriteP2PVRelations   = False
                         , ForceP2PVBuild       = False
                         )
        self.validatedSetProps( "RV2PWithVelo", DisplVerticesLinesConf.recoCuts + DisplVerticesLinesConf.singleCuts, rv2pWithVelo )

        withVeloCandidates = Selection( "%sWithVeloCandidates" % self.name()
                               , RequiredSelections = [ withVeloVertexing ]
                               , Algorithm          = rv2pWithVelo
                               , InputDataSetter    = "RecVertexLocations"
                               )

        #######################################################################
        ###                                                                 ###
        ###     DOWNSTREAM VERTEXING SEQUENCE                               ###
        ###                                                                 ###
        #######################################################################

        from GaudiConfUtils.ConfigurableGenerators import CopyDownstreamTracks

        downTracks = Selection( "%sDownstreamTracks" % self.name()
                       , RequiredSelections = [ bestTracks ]
                       , Algorithm          = CopyDownstreamTracks()
                       )

        # Displaced Vertex reconstruction from downstream tracks
        downVertexFinder = PVOfflineTool( "%sDownVertexFinder" % self.name()
                                        , RequireVelo     = False
                                        , PVsChi2Separation = 0
                                        , PVsChi2SeparationLowMult = 0
                                        , PVSeedingName   = "PVSeed3DTool"
                                        , PVFitterName    = "LSAdaptPVFitter"
                                        )
        downVertexFinder.addTool(PVSeed3DTool)
        downVertexFinder.PVSeed3DTool.TrackPairMaxDistance = 2.0*units.mm
        downVertexFinder.PVSeed3DTool.zMaxSpread           = 20.0*units.mm
        downVertexFinder.PVSeed3DTool.MinCloseTracks       = 4
        downVertexFinder.addTool(LSAdaptPVFitter)
        downVertexFinder.LSAdaptPVFitter.MinTracks          = 4
        downVertexFinder.LSAdaptPVFitter.maxChi2            = 400.0
        downVertexFinder.LSAdaptPVFitter.maxDeltaZ          = 0.0005 *units.mm
        downVertexFinder.LSAdaptPVFitter.maxDeltaChi2NDoF   = 0.002
        downVertexFinder.LSAdaptPVFitter.acceptTrack        = 0.000000001
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2       = 9
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2Remove = 64

        downVertexAlg = PatPV3D( "%sDownVertexAlg" % self.name() )
        downVertexAlg.addTool(downVertexFinder, name="PVOfflineTool")

        downVertexing = SelectionPatPV3DWrapper( "%sDownVertexing" % self.name()
                          , downVertexAlg
                          , RequiredSelections = [ downTracks ]
                          )

        # Make Particles out of the RecVertices
        rv2pDown = LLParticlesFromRecVertices(
                       VerticesFromVeloOnly = False
                     , WriteP2PVRelations   = False
                     , ForceP2PVBuild       = False
                     #, OutputLevel          = VERBOSE
                     )
        self.validatedSetProps( "RV2PDown", DisplVerticesLinesConf.recoCuts + DisplVerticesLinesConf.singleCuts, rv2pDown )

        downCandidates = Selection( "%sDownCandidates" % self.name()
                           , RequiredSelections = [ downVertexing ]
                           , Algorithm          = rv2pDown
                           , InputDataSetter    = "RecVertexLocations"
                           )

        #######################################################################
        ###                                                                 ###
        ###     LINE DEFINITIONS                                            ###
        ###                                                                 ###
        #######################################################################

        ##============================== Single ===================================##

        singleLineNames = [ p.split("Single")[1].split("Selection")[0]
                                for p in self.configKeys()
                                    if p.startswith("Single")
                                    and p.endswith("Selection")
                          ]

        for lAcroName in singleLineNames:
            lShortName = "Single%s" % lAcroName             # SingleMedium
            lSelName   = "%sSelection" % lShortName         # SingleMediumSelection
            lLineName  = "%s%s" % (self.name(), lShortName) # DisplVerticesSingleMedium

            # Choose between Velo-based and downstream vertexing input
            candidates = withVeloCandidates
            code = None
            if "Down" in lAcroName:
                candidates = downCandidates
                code = self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts + DisplVerticesLinesConf.downCuts) )
            else:
                code = self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts) )

            lineFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = DisplVerticesLinesConf.llpSelectionPreambulo
                           , Code               = code
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           #, OutputLevel        = VERBOSE
                           )

            lineSel = Selection( "".join(( self.name(), lSelName ))
                        , RequiredSelections = [ candidates ]
                        , Algorithm          = lineFilter
                        )

            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lSelName, ["PreScale"])["PreScale"]
                     , selection = lineSel
                     ## , FILTER    = { "Preambulo" : [ "nVeloTracks   = RECSUMMARY(13,  0)"
                     ##                               , "nVeloClusters = RECSUMMARY(30, -1)"
                     ##                               , "from LoKiCore.functions import *" ]
                     ##               , "Code"      : "monitor( 1.*nVeloTracks/nVeloClusters , Gaudi.Histo1DDef('Number of Velo Tracks / Clusters', 0., 1., 100) , 'NumVeloTracksPerCluster' ) < 0.2" }
                     )
            if lShortName in self.configurationParameter("HLT"):
                line.HLT = self.configurationParameter("HLT")[lShortName]

            self.registerLine(line)

        ##============================== Double ===================================##

        doubleLineNames = [ p.split("Double")[1].split("Selection")[0]
                                for p in self.configKeys()
                                    if p.startswith("Double")
                                    and p.endswith("Selection")
                          ]
        for lAcroName in doubleLineNames:
            lShortName = "Double%s" % lAcroName
            lSelName   = "%sSelection" % lShortName
            lLineName  = "%s%s" % (self.name(), lShortName)

            combinationCut, motherCut = self.getResonanceSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.doubleResonanceCuts) )
            lineFilter = CombineParticles(
                             DecayDescriptor    = "H_10 -> %s %s" % (LLPLHCbName, LLPLHCbName)
                           , Preambulo          = DisplVerticesLinesConf.llpSelectionPreambulo
                           , DaughtersCuts      = { LLPLHCbName : self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts) ) }
                           , CombinationCut     = combinationCut
                           , MotherCut          = motherCut
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           #, OutputLevel        = VERBOSE
                           )
            lineSel = Selection( "".join(( self.name(), lSelName ))
                        , RequiredSelections = [ withVeloCandidates ]
                        , Algorithm          = lineFilter
                        )
            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lSelName, ["PreScale"])["PreScale"]
                     , selection = lineSel
                     )
            if lShortName in self.configurationParameter("HLT"):
                line.HLT = self.configurationParameter("HLT")[lShortName]

            self.registerLine(line)

        ##============================== HLT PS ===================================##

        hltPSLineNames = [ p.split("HLTPS")[0]
                               for p in self.configKeys()
                                   if p.endswith("HLTPS")
                         ]
        for lAcroName in hltPSLineNames:
            lShortName = "%sHLTPS" % lAcroName
            lLineName  = "%s%s" % (self.name(), lShortName) # DisplVerticesSingleMedium

            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lShortName, ["PreScale"])["PreScale"]
                     # these lines MUST have an HLT filter
                     , HLT       = self.validatedGetProps("HLT", [lShortName])[lShortName]
                     , selection = withVeloCandidates
                     )

            self.registerLine(line)

        ##============================== OTHER  ===================================##

        hltEffLineNames = [ p.split("HltEff")[1].split("Selection")[0]
                                for p in self.configKeys()
                                    if p.startswith("HltEff")
                                    and p.endswith("Selection")
                          ]
        for lShortName in hltEffLineNames:
            lSelName  = "HltEff%sSelection" % lShortName
            lLineName = "%s%s" % (self.name(), lShortName)

            # HltEff lines are single, Velo-vertexing based lines
            lineFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = DisplVerticesLinesConf.llpSelectionPreambulo
                           , Code               = self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts) )
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           #, OutputLevel        = VERBOSE
                           )

            lineSel = Selection( "".join(( self.name(), lSelName ))
                        , RequiredSelections = [ withVeloCandidates ]
                        , Algorithm          = lineFilter
                        )

            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lSelName, ["PreScale"])["PreScale"]
                     # these lines MUST have an HLT filter
                     , HLT       = self.configurationParameter("HLT")[lShortName]
                     , selection = lineSel
                     )

            self.registerLine(line)


        ### set PropertiesPrint for all algorithms
        ## for l in self.lines():
        ##     for alg in l._members:
        ##         alg.PropertiesPrint = True

