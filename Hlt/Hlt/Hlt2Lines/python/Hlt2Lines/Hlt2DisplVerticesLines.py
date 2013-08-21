"""
Configuration of the Exotic Displaced Vertex Hlt2 lines

The selection is performed through following steps:
    - (optional) Velo tracks not originating from a PV are selected,
      through a minimum IP cutV
    - vertex reconstruction (PatPV3D with settings for low-multiplicity vertices)
    - LLParticlesFromRecVertices creates particles out of these vertices, applying some common cuts already (to keep timing low)
    - Each line applies cuts optimised for a different part of phasespace.
      "Single" lines add a FilterDesktop and "Double" lines a CombineParticles algorithm
"""

__author__ = [ "Neal Gauvin", "Victor Coco", "Pieter David" ]
__date__   = "27/03/2012"
__all__    = ( "Hlt2DisplVerticesLinesConf" )

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

from Configurables import FilterDesktop

from Gaudi.Configuration import * # TODO: explicit is better than implicit
import GaudiKernel.SystemOfUnits as units
from LHCbKernel.Configuration import DEBUG, VERBOSE

# constants
LLPLHCbName = "~chi_10"
# TODO move other constants (static members) here

class Hlt2DisplVerticesLinesConf(HltLinesConfigurableUser) :

    __slots__ = {
      ## Velo tracks filter
        "FilterVelo"                : True
      , "VeloIP"                    : 0.100*units.mm
      , "VeloIPChi2"                : -1.0
      , "VeloRejectSplash"          : False
      , "VeloRejectBackward"        : True
      , "VeloMinNumTracks"          : 4
      , "VeloMaxNumInput"           : -1
      , "VeloMaxNumOutput"          : -1

      #========== DEFAULT CUT VALUES, fallback if no other specified ==========#
      , "DefaultLLP"                : { "PreScale"             : 1.0
                                      , "MinNumTracks"         : 4
                                      , "MinRho"               : 0.4   *units.mm
                                      , "MinMass"              : 3200.0*units.MeV
                                      , "MinSumPT"             : 1000.0*units.MeV ## TODO: check
                                      , "MaxFractE1Track"      : 0.9
                                      , "MaxFractTrwHitBefore" : 0.49
                                      , "ApplyMatterVeto"      : True
                                      }
      ## Velo reconstruction
      , "RV2PVelo"                  : { "MinSumPT"             : 0.0   *units.MeV
                                      , "MinMass"              : 0.0   *units.MeV
                                      , "ApplyMatterVeto"      : False
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : True
                                      }
      ## Single LLP lines
      , "SingleVeryHighFDSelection" : { "MinNumTracks"         : 4     # 2-3 Hz
                                      , "MinRho"               : 5.0   *units.mm
                                      , "MinMass"              : 2000.0*units.MeV
                                      }
      , "SingleHighFDSelection"     : { "MinNumTracks"         : 5     # 4-5 Hz
                                      , "MinRho"               : 3.0   *units.mm
                                      , "MinMass"              : 2800.0*units.MeV
                                      }
      , "SingleSelection"           : { "MinNumTracks"         : 4     # 4 Hz
                                      , "MinRho"               : 1.5   *units.mm
                                      , "MinMass"              : 5000.0*units.MeV
                                      }
      , "SingleHighMassSelection"   : { "MinNumTracks"         : 5     # 5 Hz
                                      , "MinRho"               : 0.5   *units.mm
                                      , "MinMass"              : 8000.0*units.MeV
                                      }
      # prescaled
      , "SinglePSSelection"         : { "PreScale"             : 0.01  # 1 Hz
                                      , "MinNumTracks"         : 4
                                      , "MinRho"               : 1.5   *units.mm
                                      , "MinMass"              : 2500.0*units.MeV
                                      , "MinSumPT"             :    0.0*units.MeV
                                      }
      , "SingleLoosePSSelection"    : { "PreScale"             : 0.001 # 1-2 Hz
                                      , "MinNumTracks"         : 4
                                      , "MinRho"               : 0.4   *units.mm
                                      , "MinMass"              : 0.0   *units.MeV
                                      , "MinSumPT"             : 0.0   *units.MeV
                                      , "ApplyMatterVeto"      : False
                                      }

      ## Double LLP line
      , "DoubleSelection"           : { "ApplyMatterVeto"      : False
                                      , "MinMass"              : 2000.0*units.MeV
                                      ## Double only
                                      , "MinHighestMass"       : 2800.0*units.MeV
                                      , "ApplyMatterVetoOne"   : True
                                      }
      , "DoublePSSelection"         : { "PreScale"             : 0.01
                                      , "ApplyMatterVeto"      : False
                                      , "MinMass"              : 2000.0*units.MeV
                                      , "MinSumPT"             :    0.0*units.MeV
                                      ## Double only
                                      , "MinHighestMass"       : 2000.0*units.MeV
                                      , "ApplyMatterVetoOne"   : False
                                      }


      ## Downstream preselection
      , "PreSelDown"                : { "MinNumTracks"         : 5
                                      , "MinRho"               : 0.4   *units.mm
                                      , "MinMass"              : 2800.0*units.MeV
                                      }
      ## Downstream reconstruction
      , "RV2PDown"                  : { "MinNumTracks"         : 5
                                      , "MinMass"              : 2800.0*units.MeV
                                      , "ApplyMatterVeto"      : False
                                      ## Reco only
                                      , "MaxChi2NonVeloOnly"   : 5.0
                                      , "UseVeloTracks"        : False
                                      , "ComputeMatterVeto"    : False
                                      }
      ## Single downstream line
      , "SingleDownSelection"       : { "MinNumTracks"         : 5
                                      , "MinRho"               : 2.0   *units.mm
                                      , "MinMass"              : 3000.0*units.MeV
                                      , "ApplyMatterVeto"      : False
                                      }
      }

    def localise_props(self, name, propNames):
        # Get properties specified for this selection
        localised_props = dict( (k, v) for k, v in self.getProp(name).iteritems() if k in propNames )
        # Add default cut values
        default_cuts = self.getProp("DefaultLLP")
        for prop in propNames:
            if not prop in localised_props:
                assert prop in default_cuts # Not finding a value is a problem
                localised_props[prop] = default_cuts[prop]
        return localised_props

    def setProps(self, conf, propsToSet, ignoreIfMissing=False):
        availProps = conf.getProperties()
        for k, v in propsToSet.iteritems():
            if k in availProps:
                conf.setProp( k, v )
            elif not ignoreIfMissing:
                assert k in availProps

    singleCuts = [ "MinNumTracks", "MinRho", "MinMass", "MinSumPT", "ApplyMatterVeto", "MaxFractE1Track", "MaxFractTrwHitBefore" ]
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

    def __apply_configuration__(self) :

        from HltTracking.HltPVs import PV3D

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking
        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedDownstreamTracking

        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedPions
        from Hlt2SharedParticles.TrackFittedBasicParticles import BiKalmanFittedDownPions

        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, LSAdaptPVFitter

        from HltLine.HltLine import Hlt2Line, bindMembers, Hlt2Member
        from Configurables import HltANNSvc

        from Configurables import FilterDesktop, CombineParticles

        #######################################################################
        ###                                                                 ###
        ###     VELO BASED VERTEXING SEQUENCE                               ###
        ###                                                                 ###
        #######################################################################

        # Velo tracking
        hlt2VeloTracking = Hlt2BiKalmanFittedForwardTracking().hlt2VeloTracking()

        veloRecoAlgs = [ hlt2VeloTracking, PV3D() ]
        veloTracksForVertexing = [ hlt2VeloTracking.outputSelection() ]

        # optionally filter out forward Velo tracks with MinIP to primary vertices
        if self.getProp("FilterVelo"):
            from Configurables import SelectVeloTracksNotFromPV

            vWithIP = SelectVeloTracksNotFromPV( "Hlt2DisplVerticesVeloTrackFilter"
                        , Inputs               = list(veloTracksForVertexing)
                        , Output               = "Hlt2/Hlt2DVTrack/VeloNotFromPV"
                        , PVLocation           = "Hlt/Vertex/PV3D"
                        , MinIP                = self.getProp("VeloIP")
                        , MinIPChi2            = self.getProp("VeloIPChi2")
                        , RejectSplashEvents   = self.getProp("VeloRejectSplash")
                        , RemoveBackwardTracks = self.getProp("VeloRejectBackward")
                        , MinNumTracks         = self.getProp("VeloMinNumTracks")
                        , MaxNumInputTracks    = self.getProp("VeloMaxNumInput")
                        , MaxNumOutputTracks   = self.getProp("VeloMaxNumOutput")
                        )

            veloTracksForVertexing = [ vWithIP.Output ]

            veloRecoAlgs.append(vWithIP)

        # Displaced Vertex reconstruction from Velo tracks
        veloVertexFinder = PVOfflineTool("Hlt2DisplVerticesVeloOfflineTool"
                                        , InputTracks     = veloTracksForVertexing
                                        , PVSeedingName   = "PVSeed3DTool"
                                        , PVFitterName    = "LSAdaptPV3DFitter"
                                        )
        veloVertexFinder.addTool(PVSeed3DTool)
        veloVertexFinder.PVSeed3DTool.TrackPairMaxDistance = 0.2*units.mm
        veloVertexFinder.PVSeed3DTool.zMaxSpread           = 1.0*units.mm
        veloVertexFinder.PVSeed3DTool.MinCloseTracks       = 3
        veloVertexFinder.addTool(LSAdaptPV3DFitter)
        veloVertexFinder.LSAdaptPV3DFitter.maxIP2PV        = 2.0*units.mm
        veloVertexFinder.LSAdaptPV3DFitter.MinTracks       = 4

        veloVertexing = PatPV3D("Hlt2DisplVertices3DVelo"
                          , OutputVerticesName = "Hlt/Hlt2VeloVertices/Vertices"
                          )
        veloVertexing.addTool(veloVertexFinder, name="PVOfflineTool")

        veloRecoAlgs.append(veloVertexing)

        # Make Particles out of the RecVertices
        from Configurables import LLParticlesFromRecVertices
        recVertexToParticleVelo = LLParticlesFromRecVertices("Hlt2RV2PVelo"
                                    , Inputs             = [ BiKalmanFittedPions.outputSelection() ]
                                    , RecVertexLocations = [ veloVertexing.OutputVerticesName ]
                                    , Output             = "Hlt2/Hlt2RV2PVelo/Particles"
                                    , WriteP2PVRelations = False
                                    , ForceP2PVBuild     = False
                                    )
        ## Add the properties from the settings above
        self.setProps( recVertexToParticleVelo, self.localise_props("RV2PVelo", Hlt2DisplVerticesLinesConf.recoCuts + Hlt2DisplVerticesLinesConf.singleCuts), True )

        veloCandidates = bindMembers("VeloCandidatesForDV", veloRecoAlgs + [ BiKalmanFittedPions, recVertexToParticleVelo ]).setOutputSelection( recVertexToParticleVelo.Output )

        #######################################################################
        ###                                                                 ###
        ###     DOWNSTREAM VERTEXING SEQUENCE                               ###
        ###                                                                 ###
        #######################################################################

        hlt2DownstreamTracking = Hlt2BiKalmanFittedDownstreamTracking().hlt2DownstreamTracking()

        downRecoAlgs = [ hlt2DownstreamTracking ]
        downTracksForVertexing = [ hlt2DownstreamTracking.outputSelection() ]

        # Displaced Vertex reconstruction from downstream tracks
        downVertexFinder = PVOfflineTool("Hlt2DisplVerticesDownOfflineTool"
                                        , InputTracks     = downTracksForVertexing
                                        , RequireVelo     = False
                                        , PVSeedingName   = "PVSeed3DTool"
                                        , PVFitterName    = "LSAdaptPVFitter"
                                        )
        downVertexFinder.addTool(PVSeed3DTool)
        downVertexFinder.PVSeed3DTool.TrackPairMaxDistance = 2.0*units.mm
        downVertexFinder.PVSeed3DTool.zMaxSpread           = 20.0*units.mm
        downVertexFinder.PVSeed3DTool.MinCloseTracks       = 3
        downVertexFinder.addTool(LSAdaptPVFitter)
        downVertexFinder.LSAdaptPVFitter.MinTracks          = 3
        downVertexFinder.LSAdaptPVFitter.maxChi2            = 400.0
        downVertexFinder.LSAdaptPVFitter.maxDeltaZ          = 0.0005 *units.mm
        downVertexFinder.LSAdaptPVFitter.maxDeltaChi2NDoF   = 0.002
        downVertexFinder.LSAdaptPVFitter.acceptTrack        = 0.00000001
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2       = 9
        downVertexFinder.LSAdaptPVFitter.trackMaxChi2Remove = 64

        downVertexing = PatPV3D("Hlt2DisplVertices3DDown"
                          , OutputVerticesName = "Hlt/Hlt2DownVertices/Vertices"
                          )
        downVertexing.addTool(downVertexFinder, name="PVOfflineTool")

        downRecoAlgs.append(downVertexing)

        # Downstream pre-selection : require a (loosely selected) Velo candidate
        downPreSel = Hlt2Member( FilterDesktop, "PreSelection"
                       , Inputs             = [ veloCandidates.outputSelection() ]
                       , DecayDescriptor    = LLPLHCbName
                       , Preambulo          = Hlt2DisplVerticesLinesConf.llpSelectionPreambulo
                       , Code               = self.getLLPSelection( self.localise_props("PreSelDown", Hlt2DisplVerticesLinesConf.singleCuts) )
                       , WriteP2PVRelations = False
                       , ForceP2PVBuild     = False
                       )

        # Make Particles out of the RecVertices
        from Configurables import LLParticlesFromRecVertices
        recVertexToParticleDown = LLParticlesFromRecVertices("Hlt2RV2PDown"
                                    , Inputs             = [ BiKalmanFittedDownPions.outputSelection() ]
                                    , RecVertexLocations = [ downVertexing.OutputVerticesName ]
                                    , Output             = "Hlt2/Hlt2RV2PDown/Particles"
                                    , WriteP2PVRelations = False
                                    , ForceP2PVBuild     = False
                                    )
        ## Add the properties from the settings above
        self.setProps( recVertexToParticleDown, self.localise_props("RV2PDown", Hlt2DisplVerticesLinesConf.recoCuts + Hlt2DisplVerticesLinesConf.singleCuts), True )

        downCandidates = bindMembers("DownCandidatesForDV", veloCandidates.members() + [ downPreSel ] + downRecoAlgs + [ BiKalmanFittedDownPions, recVertexToParticleDown ]).setOutputSelection( recVertexToParticleDown.Output )


        #######################################################################
        ###                                                                 ###
        ###     LINE DEFINITIONS                                            ###
        ###                                                                 ###
        #######################################################################

        decisionIDCounter = 50281

        ##============================== Single ===================================##

        singleLineNames = [ p.split("Single")[1].split("Selection")[0]
                                for p in self.getProperties().iterkeys()
                                    if p.startswith("Single")
                                    and p.endswith("Selection")
                          ]
        singleLines = {}
        for lAcroName in singleLineNames:
            lShortName = "Single%s" % lAcroName
            lSelName   = "%sSelection" % lShortName
            lLineName  = "DisplVertices%s" % lShortName
            lLongName  = "Hlt2%s" % lLineName

            # Choose between Velo-based and downstream vertexing input
            candidates = veloCandidates
            if "Down" in lAcroName:
                candidates = downCandidates

            lineFilter = Hlt2Member( FilterDesktop, "Filter"
                           , Inputs             = [ candidates.outputSelection() ]
                           , DecayDescriptor    = LLPLHCbName
                           , Preambulo          = Hlt2DisplVerticesLinesConf.llpSelectionPreambulo
                           , Code               = self.getLLPSelection( self.localise_props(lSelName, Hlt2DisplVerticesLinesConf.singleCuts) )
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           )

            line = Hlt2Line(lLineName
                     , prescale  = self.localise_props(lSelName, ["PreScale"])["PreScale"]
                     , algos     = candidates.members() + [ lineFilter ]
                     , postscale  = self.postscale
                     )

            singleLines[lShortName] = line

            HltANNSvc().Hlt2SelectionID.update({ "%sDecision" % lLongName : decisionIDCounter })
            decisionIDCounter += 1

        ##============================== Double ===================================##

        doubleLineNames = [ p.split("Double")[1].split("Selection")[0]
                                for p in self.getProperties().iterkeys()
                                    if p.startswith("Double")
                                    and p.endswith("Selection")
                          ]
        doubleLines = {}
        for lAcroName in doubleLineNames:
            lShortName = "Double%s" % lAcroName
            lSelName   = "%sSelection" % lShortName
            lLineName  = "DisplVertices%s" % lShortName
            lLongName  = "Hlt2%s" % lLineName

            combinationCut, motherCut = self.getResonanceSelection( self.localise_props(lSelName, Hlt2DisplVerticesLinesConf.doubleResonanceCuts) )
            lineFilter = Hlt2Member( CombineParticles, "Filter"
                           , Inputs             = [ recVertexToParticleVelo.Output ]
                           , DecayDescriptor    = "H_10 -> %s %s" % (LLPLHCbName, LLPLHCbName)
                           , Preambulo          = Hlt2DisplVerticesLinesConf.llpSelectionPreambulo
                           , DaughtersCuts      = { LLPLHCbName : self.getLLPSelection( self.localise_props(lSelName, Hlt2DisplVerticesLinesConf.singleCuts) ) }
                           , CombinationCut     = combinationCut
                           , MotherCut          = motherCut
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           )
            line = Hlt2Line(lLineName
                     , prescale  = self.localise_props(lSelName, ["PreScale"])["PreScale"]
                     , algos     = veloCandidates.members() + [ lineFilter ]
                     , postscale = self.postscale
                     )

            doubleLines[lShortName] = line

            HltANNSvc().Hlt2SelectionID.update({ "%sDecision" % lLongName : decisionIDCounter })
            decisionIDCounter += 1
