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
                                      }

        #==========        SELECTION CUT VALUES         ==========#

        ## Velo reconstruction
        , "RV2PWithVelo"            : { "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      }
        ## Single LLP lines
        , "SingleHighMassSelection" : { "PreScale"             :  1.0
                                      , "MinRho"               :  1.5*units.mm
                                      , "MinMass"              : 15.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "SingleMediumSelection"   : { "PreScale"             :  1.0
                                      , "MinRho"               :  3.2*units.mm
                                      , "MinMass"              : 10.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "SingleHighFDSelection"   : { "PreScale"             :  1.0
                                      , "MinRho"               :  5.0*units.mm
                                      , "MinMass"              :  7.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  7
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        # prescaled
        , "SinglePSSelection"       : { "PreScale"             :  0.005
                                      , "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  3.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        ## Double LLP line
        , "DoubleSelection"         : { "PreScale"             :  1.0
                                      , "MinRho"               :  1.0*units.mm
                                      , "MinMass"              :  6.0*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Double only
                                      , "MinHighestMass"       : 7.0*units.GeV
                                      , "ApplyMatterVetoOne"   : True
                                      }

        ## Downstream reconstruction
        , "RV2PDown"                : { "MinRho"               :  3.0*units.mm
                                      , "MinMass"              :  4.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      }
        ## Single downstream line ### TODO implement MinZ = 150.0*mm
        , "SingleDownSelection"     : { "PreScale"             :  1.0
                                      , "MinRho"               :  3.0*units.mm
                                      , "MinMass"              :  4.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        #========== Other lines for efficiency studies ==========#
        , "SingleHLTPS"             : { "PreScale"             :  1.0 }
        , "DoubleHLTPS"             : { "PreScale"             :  1.0 }
        , "HltEffJPsiHLTSelection"  : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.7*units.mm
                                      , "MinMass"              :  5.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  6
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }
        , "HltEffMinBiasSelection"  : { "PreScale"             :  1.0
                                      , "MinRho"               :  0.7*units.mm
                                      , "MinMass"              :  5.5*units.GeV
                                      , "MinSumPT"             :  3.0*units.GeV
                                      , "MinNumTracks"         :  4
                                      , "ApplyMatterVeto"      : False
                                      , "MaxFractE1Track"      :  0.9
                                      , "MaxFractTrwHitBefore" :  0.49
                                      }

        #==========     HLT filters for all lines      ==========#
        , "HLT"                     : { "JPsiHLT"     : "HLT_PASS('Hlt2DiMuonJPsiDecision')"
                                      , "MinBias"     : "HLT_PASS('Hlt1MBNoBiasDecision')"
                                      , "SingleHLTPS" : "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose)PSDecision')"
                                      , "DoubleHLTPS" : "HLT_PASS_RE('Hlt2DisplVerticesDoublePSDecision')"
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
            , "SingleHighMassSelection"
            , "SingleMediumSelection"
            , "SingleHighFDSelection"
            , "SinglePSSelection"
            , "DoubleSelection"
            , "RV2PDown"
            , "SingleDownSelection"
            , "SingleHLTPS"
            , "DoubleHLTPS"
            , "HltEffJPsiHLTSelection"
            , "HltEffMinBiasSelection"
            , "HLT"
            )
    def validatedGetProps(self, selName, propsToSet):
        """
        Get the __configuration_keys__[selName] dictionary, checking it contains all propsToSet
        """
        settings = self.configurationParameter(selName)
        for p in propsToSet:
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

    singleCuts = [ "MinNumTracks", "MinRho", "MinMass", "ApplyMatterVeto", "MaxFractE1Track", "MaxFractTrwHitBefore" ]
## TODO add MinSumPT again if needed
    doubleResonanceCuts = [ "MinHighestMass", "ApplyMatterVetoOne" ]
    recoCuts = [ "MaxChi2NonVeloOnly", "UseVeloTracks", "ComputeMatterVeto" ]

    llpSelectionPreambulo = [ "from LoKiPhys.decorators import *"
                            , "from LoKiTracks.decorators import *"
                            , "INMATTER                           = ( INFO( 51, -1000.0 ) > 0.5 )"
                            , "ENDVERTEXRHO                       =   INFO( 52, -1000.0 )"
                            #, "VSIGMARHO                          =   INFO( 53, -1000.0 )"
                            #, "VSIGMAZ                            =   INFO( 54, -1000.0 )"
                            , "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT =   INFO( 55, -1000.0 )"
                            , "MAXFRACTENERGYINSINGLETRACK        =   INFO( 56, -1000.0 )"
                            ]

    def getLLPSelection(self, cutValues):
        selCuts   = [ "~ISBASIC"
                    , "M > %(MinMass)s"
                    , "NDAUGS >= %(MinNumTracks)s"
                    , "ENDVERTEXRHO > %(MinRho)s"
                    , "FRACTDAUGHTERTRACKSWITHUPSTREAMHIT < %(MaxFractTrwHitBefore)s"
                    , "MAXFRACTENERGYINSINGLETRACK < %(MaxFractE1Track)s"
                    ]
        if cutValues["ApplyMatterVeto"]:
            selCuts.insert(1, "~INMATTER")

        selCode = "\n & ".join( "( %s )" % ( cut % cutValues ) for cut in selCuts )

        return selCode

    def getResonanceSelection(self, cutValues):
        combCuts = [ "AHASCHILD( M > %(MinHighestMass)s )"
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
            ## TODO ADD THE VeloWithIP algorithm
            print "### WARNING: VeloWithIP has not been added to the configuration file yet ###"

        # Displaced Vertex reconstruction with best tracks (dominated by those with a Velo segment)
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, LSAdaptPVFitter

        withVeloVertexFinder = PVOfflineTool( "%sWithVeloVertexFinder" % self.name()
                                 , PVsChi2Separation = 0
                                 , PVsChi2SeparationLowMult = 0
                                 , PVSeedingName   = "PVSeed3DTool"
                                 , PVFitterName    = "LSAdaptPV3DFitter"
                                 , PropertiesPrint = True
                                 )
        withVeloVertexFinder.addTool(PVSeed3DTool)
        withVeloVertexFinder.PVSeed3DTool.MinCloseTracks       = 3
        withVeloVertexFinder.PVSeed3DTool.PropertiesPrint      = True

        withVeloVertexFinder.addTool(LSAdaptPV3DFitter)
        withVeloVertexFinder.LSAdaptPV3DFitter.maxIP2PV        = 2.0*units.mm
        withVeloVertexFinder.LSAdaptPV3DFitter.MinTracks       = 4
        withVeloVertexFinder.LSAdaptPV3DFitter.PropertiesPrint = True

        withVeloVertexAlg = PatPV3D( "%sWithVeloVertexAlg", PropertiesPrint = True )
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
                         #, OutputLevel          = VERBOSE
                         )
        self.validatedSetProps( "RV2PWithVelo", DisplVerticesLinesConf.recoCuts + DisplVerticesLinesConf.singleCuts, rv2pWithVelo, ignoreIfMissing=True )

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
                                        , PropertiesPrint = True
                                        )
        downVertexFinder.addTool(PVSeed3DTool)
        downVertexFinder.PVSeed3DTool.TrackPairMaxDistance = 2.0*units.mm
        downVertexFinder.PVSeed3DTool.zMaxSpread           = 20.0*units.mm
        downVertexFinder.PVSeed3DTool.MinCloseTracks       = 4
        downVertexFinder.PVSeed3DTool.PropertiesPrint      = True
        downVertexFinder.addTool(LSAdaptPVFitter)
        downVertexFinder.LSAdaptPVFitter.MinTracks          = 4
        downVertexFinder.LSAdaptPVFitter.maxChi2            = 400.0
        downVertexFinder.LSAdaptPVFitter.maxDeltaZ          = 0.0005 *units.mm
        downVertexFinder.LSAdaptPVFitter.maxDeltaChi2NDoF   = 0.002
        downVertexFinder.LSAdaptPVFitter.acceptTrack        = 0.000000001
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2       = 9
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2Remove = 64
        downVertexFinder.LSAdaptPVFitter.PropertiesPrint    = True

        downVertexAlg = PatPV3D( "%sDownVertexAlg" % self.name()
                          , PropertiesPrint = True )
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
        self.validatedSetProps( "RV2PDown", DisplVerticesLinesConf.recoCuts + DisplVerticesLinesConf.singleCuts, rv2pDown, ignoreIfMissing=True )

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
            if "Down" in lAcroName:
                candidates = downCandidates

            lineFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = DisplVerticesLinesConf.llpSelectionPreambulo
                           , Code               = self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts) )
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

