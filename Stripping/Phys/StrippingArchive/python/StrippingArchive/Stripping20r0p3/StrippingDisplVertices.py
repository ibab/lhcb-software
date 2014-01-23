"""
Configuration for the Exotic Displaced Vertex Stripping lines
"""

__author__ = ['Neal Gauvin', 'Victor Coco', "Veerle Heijne", "Pieter David"]
__date__   = '26/12/2013'

__all__ = ( 'DisplVerticesLinesConf' )

from Gaudi.Configuration import *
from GaudiKernel import SystemOfUnits as units
from LHCbKernel.Configuration import DEBUG, VERBOSE

import logging
logger = logging.getLogger(__name__)

from PhysSelPython.Wrappers import AutomaticData, Selection, PassThroughSelection, EventSelection
from StrippingConf.StrippingLine import StrippingLine, bindMembers
from StrippingUtils.Utils import LineBuilder

from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from GaudiConfUtils.ConfigurableGenerators import GaudiSequencer as GaudiSequenceroid
from Configurables import GaudiSequencer
from Configurables import LoKi__HDRFilter  as HltFilter
from Configurables import LoKi__ODINFilter as ODINFilter

from StrippingSelections.DisplVertices_Utils import SelectionPatPV3DWrapper

def addPrivateToolAndGet( motherConf, toolConfType, name=None ):
    from GaudiKernel.Configurable import Configurable
    if not issubclass(toolConfType, Configurable) or not isinstance(motherConf, Configurable):
        logger.fatal("addPrivateToolAndGet: first argument should be a configurable instance, second a configurable class")
        return
    if name is None:
        name = toolConfType.__name__
    motherConf.addTool(toolConfType, name=name)
    return getattr(motherConf, name)


# constants
LLPLHCbName = "~chi_10"

config = {
        ## Velo GEC
          "VeloGEC"                 : { "Apply"                : True
                                      , "MaxVeloRatio"         : 0.1
                                      , "MaxPhiVectorSize"     : 500.
                                      }
        ## Velo tracks filter
        , "FilterVelo"              : { "Apply"                : True
                                      , "MinIP"                : 0.1*units.mm
                                      , "MinIPChi2"            : -1.0
                                      , "MinNumTracks"         : 4
                                      , "PVLocation"           : "Rec/Vertex/Primary"
                                      , "RemoveBackwardTracks" : True
                                      , "RemovePVTracks"       : True
                                      , "RemoveVeloClones"     : True
                                      }

        #==========        SELECTION CUT VALUES         ==========#

        ## Velo reconstruction
        , "RV2PWithVelo"            : { "MinRho"               :  0.4*units.mm
                                      , "MinMass"              :  0.0*units.GeV
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
        #, "SingleSelection"         : { "PreScale"             :  1.0
        #                                                         #       HighMass,         Medium,         HighFD
        #                              , "MinRho"               : [  1.0*units.mm ,  2.*units.mm ,  4.*units.mm  ]
        #                              , "MinMass"              : [ 8.*units.GeV,  6.5*units.GeV,  4.5*units.GeV ]
        #                              , "MinSumPT"             :  3.0*units.GeV
        #                              , "MinNumTracks"         : [  8            ,  7            ,  7             ]
        #                              , "ApplyMatterVeto"      : True
        #                              , "MaxFractE1Track"      :  0.8
        #                              , "MaxFractTrwHitBefore" :  0.49
        #                              }
        ## with the jet sequence
        , "JetSingleLowMassSelection" : { "PreScale"           :  1.0
                                      , "MinRho"               :  0.4
                                      , "MinMass"              :  0.0*units.GeV
                                      , "MinSumPT"             : 10.0*units.GeV
                                      , "MinNumTracks"         :  5
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.8
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Jet-related cuts
                                      , "MinNumJets"           :  1
                                      , "SingleJet"            :  True
                                      , "MinNJetMass"          : 10.0*units.GeV
                                      , "MinNJetTransvMass"    : 33.0*units.GeV
                                      , "JetIDCut"             : "( JNWITHPVINFO >= 5 )"
                                      } # Tuning: done, 0.053 % 
        , "JetSingleHighMassSelection" : { "PreScale"          :  1.0
                                      , "MinRho"               :  0.4
                                      , "MinMass"              :  5.0*units.GeV
                                      , "MinSumPT"             :  7.0*units.GeV
                                      , "MinNumTracks"         :  5
                                      , "ApplyMatterVeto"      : True
                                      , "MaxFractE1Track"      :  0.8
                                      , "MaxFractTrwHitBefore" :  0.49
                                      ## Jet-related cuts
                                      , "MinNumJets"           :  2
                                      , "SingleJet"            :  False
                                      , "MinNJetMass"          :  0.0*units.GeV
                                      , "MinNJetTransvMass"    : 20.0*units.GeV
                                      , "JetIDCut"             : None
                                      }
        ## jet sequence on top of Hlt2 candidates
        ## NOTE the only case where the mass is "wrong", is for a tightened JetID in the Hlt-based jet line (which will produce an error at config time)
        , "JetHltSingleLowMassSelection": { "PreScale"         :  1.0
                                      ## Jet-related cuts
                                      , "MinNumJets"           :  1
                                      , "SingleJet"            :  True
                                      , "MinNJetMass"          :  8.0*units.GeV
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : "( JNWITHPVINFO >= 3 ) & ( JMPT > 1800. )"
                                      } # Tuning: done - 0.04 %
        , "JetHltSingleHighMassSelection" : { "PreScale"       :  1.0
                                      ## Jet-related cuts
                                      , "MinNumJets"           :  2
                                      , "SingleJet"            :  False
                                      , "MinNJetMass"          :  0.0*units.GeV
                                      , "MinNJetTransvMass"    : None
                                      , "JetIDCut"             : None
                                      } # Tuning: fine like this - can cut harder on mass

        ## Double LLP line
        # , "DoubleSelection"         : { "PreScale"             :  1.0
        #                               , "MinRho"               :  0.6*units.mm
        #                               , "MinMass"              :  3.0*units.GeV
        #                               , "MinSumPT"             :  3.0*units.GeV
        #                               , "MinNumTracks"         :  6
        #                               , "ApplyMatterVeto"      :  False
        #                               , "MaxFractE1Track"      :  0.8
        #                               , "MaxFractTrwHitBefore" :  0.49
        #                               ## Double only
        #                               , "MinHighestMass"       : 3.0*units.GeV
        #                               , "ApplyMatterVetoOne"   : True
        #                               }

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
        # , "SingleDownSelection"     : { "PreScale"             :  1.0
        #                               , "MinRho"               :  2.0*units.mm
        #                               , "MinMass"              :  3.0*units.GeV
        #                               , "MinSumPT"             :  0.0*units.GeV
        #                               , "MinNumTracks"         :  4
        #                               , "ApplyMatterVeto"      : False
        #                               , "MaxFractE1Track"      :  10.
        #                               , "MaxFractTrwHitBefore" :  10.
        #                               # Down only
        #                               , "MinZ"                 :  170.*units.mm
        #                               }

        #========== Other lines for efficiency studies ==========#
        # , "HLTPS"             : { "PreScale"             :  1.0 }
        # , "HltEffCharmHLTSelection"  : { "PreScale"             :  1.0
        #                               , "MinRho"               :  0.6*units.mm
        #                               , "MinMass"              :  6.*units.GeV
        #                               , "MinSumPT"             :  3.0*units.GeV
        #                               , "MinNumTracks"         :  6
        #                               , "ApplyMatterVeto"      : False
        #                               , "MaxFractE1Track"      :  10.
        #                               , "MaxFractTrwHitBefore" :  10.
        #                               }

        #==========     HLT filters for all lines      ==========#
        , "HLT"                     : {# "CharmHLT"     : "HLT_PASS('Hlt2CharmHadD02HH_D02KPiDecision')"
        #                              , "HLTPS"        : [ ( ("0x001c0028", "0x002f002c"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
        #                                                 , ( ("0x00340032", "0x00730035"), "HLT_PASS_RE('Hlt2DisplVerticesSinglePostScaledDecision')" )
        #                                                 , ( ("0x00750037", "0x007b0038"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
        #                                                 , ( ("0x007e0039", "0x0097003d"), "HLT_PASS_RE('Hlt2DisplVertices(Single|Double|SingleMV)PostScaledDecision')" )
        #                                                 , ( ("0x00990042", "0x40000000"), "HLT_PASS_RE('Hlt2DisplVertices(Single|SingleLoose|Double)PSDecision')" )
        #                                                 ]
                                      ## For reviving Hlt2 candidates
                                        "SignalLines"  : [ ( ("0x001c0028", "0x002f002c"), ["Hlt2DisplVerticesSingleDecision"] )
                                                         , ( ("0x00340032", "0x00730035"), ["Hlt2DisplVerticesHighFDSingleDecision", "Hlt2DisplVerticesHighMassSingleDecision", "Hlt2DisplVerticesLowMassSingleDecision", "Hlt2DisplVerticesSingleDownDecision"] )
                                                         , ( ("0x00750037", "0x007b0038"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x007e0039", "0x0097003d"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDPostScaledDecision", "Hlt2DisplVerticesSingleHighMassPostScaledDecision"] )
                                                         , ( ("0x00990042", "0x40000000"), ["Hlt2DisplVerticesSingleDecision", "Hlt2DisplVerticesSingleDownDecision", "Hlt2DisplVerticesSingleHighFDDecision", "Hlt2DisplVerticesSingleHighMassDecision", "Hlt2DisplVerticesSingleVeryHighFDDecision"] )
                                                         ]
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
              "VeloGEC"
            , "FilterVelo"
            , "RV2PWithVelo"
            #, "SingleSelection"
            #, "SinglePSSelection"
            , "JetSingleLowMassSelection"
            , "JetSingleHighMassSelection"
            , "JetHltSingleLowMassSelection"
            , "JetHltSingleHighMassSelection"
            #, "DoubleSelection"
            , "RV2PDown"
            #, "SingleDownSelection"
            #, "HLTPS"
            #, "HltEffCharmHLTSelection"
            , "HLT"
            )

    def validatedGetProps(self, selName, propsToSet):
        """
        Get the __configuration_keys__[selName] dictionary, checking it contains all propsToSet
        """
        settings = self.configurationParameter(selName)
        for p in propsToSet:
            if not p in settings:
                logger.error("Missing configuration parameter for %s : %s", selName, p )
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

    veloGECCuts = [ "MaxVeloRatio", "MaxPhiVectorSize" ]
    veloWithIPCuts = [ "MinNumTracks", "PVLocation", "RemoveBackwardTracks", "RemoveVeloClones", "RemovePVTracks", "MinIP", "MinIPChi2" ]
    singleCuts = [ "MinNumTracks", "MinRho", "MinMass", "MinSumPT", "ApplyMatterVeto", "MaxFractE1Track", "MaxFractTrwHitBefore" ]
    jetCuts = [ "MinNJetMass", "MinNJetTransvMass", "SingleJet", "MinNumJets", "JetIDCut" ]
    jetHltCuts = [ "HltLines" ]
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

    jetSelectionPreambulo = [ "from DisplVertices.JetFunctions import *"
                            , "from LoKiCore.math import sqrt"
                            , "ISLLP = ( ABSID == '%s' )" % LLPLHCbName
                            , "ISJET = ( ABSID == 'CELLjet' )"
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

    def getLLPJetSelection(self, cutValues, JetIDCut=None):
        cut = "ALL"
        if cutValues["SingleJet"]:
            sel = "( MM > {MinNJetMass} )".format(**cutValues)
            if cutValues["MinNJetTransvMass"] is not None:
                sel = "{0} & ( sqrt(MM**2+PT**2) > {MinNJetTransvMass} )".format(sel, **cutValues)
            cut = sel
            if JetIDCut is not None:
                cut = "{0} & INTREE( ISJET & {1} )".format(sel, JetIDCut)
        else:
            if JetIDCut is not None:
                logger.error("N-jet selection - there should be no need to tighted the JetID. Change it in the reco instead")
            cut = "( NINTREE(ISJET) >= {MinNumJets} ) & ( MM > {MinNJetMass} )".format(**cutValues)
        return "ISLLP & {0}".format(cut)

    def getResonanceSelection(self, cutValues):
        combCuts = [ "AHASCHILD( MM > %(MinHighestMass)s )"
                   ]
        if cutValues["ApplyMatterVetoOne"]:
            combCuts.insert(0, "AHASCHILD( ~INMATTER )")

        combCode = "\n & ".join( "( %s )" % ( cut % cutValues ) for cut in combCuts )

        motherCode = "ALL"

        return combCode, motherCode

    def makeJetCandidateAlg(self, name, JetIDCut=None, MinNumJets=1):
        #######################################################################
        ###                                                                 ###
        ###     JET RECONSTRUCTION                                          ###
        ###                                                                 ###
        #######################################################################

        from Configurables import DisplacedVertexJetCandidateMakerS20p3

        jetCandidateAlg = DisplacedVertexJetCandidateMakerS20p3(name)

        from CommonParticles.StdParticleFlow import locations as pfLocations
        jetCandidateAlg.PFInputLocation = pfLocations.keys()[0]
        jetCandidateAlg.Preambulo += ( self.jetSelectionPreambulo
                                     + [ "PF = LHCb.ParticleFlowType" ] )
        jetCandidateAlg.PFInputCut = "in_list( PFTYPE, PF.ChargedHadron, PF.Muon, PF.Electron, PF.Photon, PF.Pi0, PF.NeutralHadron, PF.NeutralRecovery, PF.V0, PF.Charged0Momentum, PF.BadPhoton, PF.IsolatedPhoton, PF.MergedPi0, PF.ResolvedPi0 )"

        # partitioning
        jetCandidateAlg.MinIP2PV = 0.
        jetCandidateAlg.MaxIP2DV = 2.
        jetCandidateAlg.MinIPChi22PV = 20.
        jetCandidateAlg.MaxIPChi22DVDown = 30.

        from Configurables import LoKi__FastJetMaker
        jetMaker = addPrivateToolAndGet( jetCandidateAlg, LoKi__FastJetMaker )
        jetCandidateAlg.JetMaker = jetMaker.getTitleName()
        jetMaker.Type = 2 # anti-kt
        jetMaker.RParameter = 0.7
        jetMaker.PtMin = 5.*units.GeV
        jetMaker.Recombination = 0
        jetMaker.JetID = 98

        from Configurables import PerPVOffsetJECS20p3
        jecTool = addPrivateToolAndGet( jetCandidateAlg, PerPVOffsetJECS20p3, name="JEC" )
        jetCandidateAlg.ParticleReFitters["JEC"] = jecTool.getTitleName()
        jecTool.Apply = False
        jecTool.HistoPath = "JEC"
        from Configurables import HistogramSvc
        hSvc = HistogramSvc("HistogramDataSvc")
        jecFile = "%s DATAFILE='$PARAMFILESROOT/data/JetEnergyCorrections_Reco12_v2.root' TYP='ROOT'" % jecTool.HistoPath
        if jecFile not in hSvc.Input:
            hSvc.Input.append(jecFile)

        from Configurables import AddJetIDInfoS20p3
        jetIDTool = addPrivateToolAndGet( jetCandidateAlg, AddJetIDInfoS20p3, name="JetIDInfo" )
        jetCandidateAlg.JetIDTool = jetIDTool.getTitleName()

        jetCandidateAlg.JetIDCut = ( JetIDCut if JetIDCut is not None
                else " & ".join((
                  "in_range( 0., JMTF, 0.7 )"
                , "( 0.1  <  JCPF )"
                , "( 0.   <= JN90 )"
                , "( 900. <  JMPT )"
                )))
        jetCandidateAlg.MinNumberOfJets = MinNumJets
        jetCandidateAlg.CandidatePID = LLPLHCbName

        return jetCandidateAlg

    def __init__( self, name, config ):
        LineBuilder.__init__( self, name, config )

        #######################################################################
        ###                                                                 ###
        ###     VELO BASED VERTEXING SEQUENCE                               ###
        ###                                                                 ###
        #######################################################################

        bestTracks = AutomaticData("Rec/Track/Best")

        withVeloTracksForVertexing = bestTracks

        if self.configurationParameter("VeloGEC")["Apply"]:
            from GaudiConfUtils.ConfigurableGenerators import VeloEventShapeCutsS20p3

            veloGEC = VeloEventShapeCutsS20p3()
            self.validatedSetProps( "VeloGEC", DisplVerticesLinesConf.veloGECCuts, veloGEC )

            withVeloTracksForVertexing = PassThroughSelection( "%sVeloGEC" % self.name()
                                           , RequiredSelection = withVeloTracksForVertexing
                                           , Algorithm = veloGEC
                                           )

        if self.configurationParameter("FilterVelo")["Apply"]:
            from GaudiConfUtils.ConfigurableGenerators import SelectVeloTracksNotFromPVS20p3

            veloWithIP = SelectVeloTracksNotFromPVS20p3()
            self.validatedSetProps( "FilterVelo", DisplVerticesLinesConf.veloWithIPCuts, veloWithIP )

            withVeloTracksForVertexing = Selection( "%sVeloFilteredTracks" % self.name()
                                           , RequiredSelections = [ withVeloTracksForVertexing ]
                                           , Algorithm = veloWithIP
                                           )

        # Displaced Vertex reconstruction with best tracks (dominated by those with a Velo segment)
        from Configurables import PatPV3D, PVOfflineTool, PVSeed3DTool, LSAdaptPV3DFitter, LSAdaptPVFitter

        withVeloVertexAlg = PatPV3D( "%sWithVeloVertexAlg" % self.name() )
        withVeloVertexFinder = addPrivateToolAndGet(withVeloVertexAlg, PVOfflineTool)
        withVeloVertexFinder.PVsChi2Separation = 0
        withVeloVertexFinder.PVsChi2SeparationLowMult = 0
        withVeloSeeder = addPrivateToolAndGet(withVeloVertexFinder, PVSeed3DTool )
        withVeloVertexFinder.PVSeedingName = withVeloSeeder.getTitleName()
        withVeloSeeder.MinCloseTracks = 3
        withVeloFitter = addPrivateToolAndGet(withVeloVertexFinder, LSAdaptPV3DFitter)
        withVeloVertexFinder.PVFitterName = withVeloFitter.getTitleName()
        withVeloFitter.maxIP2PV       = 2.0*units.mm
        withVeloFitter.MinTracks      = 4

        withVeloVertexing = SelectionPatPV3DWrapper( "%sWithVeloVertexing" % self.name()
                              , withVeloVertexAlg
                              , RequiredSelections = [ withVeloTracksForVertexing ]
                              )

        # Make Particles out of the RecVertices
        from GaudiConfUtils.ConfigurableGenerators import LLParticlesFromRecVertices

        rv2pWithVelo = LLParticlesFromRecVertices(
                           VerticesFromVeloOnly = False
                         , RequireUpstreamPV    = False
                         , WriteP2PVRelations   = False
                         , ForceP2PVBuild       = False
                         , VeloProtoParticlesLocation = "Phys/%s/VeloProtoP" % self.name()
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
        downVertexAlg = PatPV3D( "%sDownVertexAlg" % self.name() )
        downVertexFinder = addPrivateToolAndGet( downVertexAlg, PVOfflineTool )
        downVertexFinder.RequireVelo = False
        downVertexFinder.PVsChi2Separation = 0
        downVertexFinder.PVsChi2SeparationLowMult = 0
        downSeeder = addPrivateToolAndGet( downVertexFinder, PVSeed3DTool )
        downVertexFinder.PVSeedingName  = downSeeder.getTitleName()
        downSeeder.TrackPairMaxDistance = 2.0*units.mm
        downSeeder.zMaxSpread           = 20.0*units.mm
        downSeeder.MinCloseTracks       = 4
        downFitter = addPrivateToolAndGet( downVertexFinder, LSAdaptPVFitter )
        downVertexFinder.PVFitterName = downFitter.getTitleName()
        downFitter.MinTracks          = 4
        downFitter.maxChi2            = 400.0
        downFitter.maxDeltaZ          = 0.0005 *units.mm
        downFitter.maxDeltaChi2NDoF   = 0.002
        downFitter.acceptTrack        = 0.000000001
        downFitter.trackMaxChi2       = 9
        downFitter.trackMaxChi2Remove = 64

        downVertexing = SelectionPatPV3DWrapper( "%sDownVertexing" % self.name()
                          , downVertexAlg
                          , RequiredSelections = [ downTracks ]
                          )

        # Make Particles out of the RecVertices
        rv2pDown = LLParticlesFromRecVertices(
                       VerticesFromVeloOnly = False
                     , RequireUpstreamPV    = False
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
        ###     HLT JET SEQUENCE                                            ###
        ###                                                                 ###
        #######################################################################
        # timing is already fine, so one algo with loose JetID is sufficient

        # Hlt prefilter and vertex candidates from Hlt2
        from Configurables import HltVertexConverterS20p3
        revivedHlt2Candidates = "Phys/%sHlt2Cand/Particles" % self.name()
        hltCandReviver = GaudiSequenceroid(ModeOR = True, ShortCircuit = False,
            Members = [ GaudiSequencer( "%sHlt2CandFilterTCK%s-%s" % (self.name(), tckBegin, tckEnd),
                                        Members = [ ODINFilter("%sODINFilterTCK%s-%s" % (self.name(), tckBegin, tckEnd), Code="in_range( %s, ODIN_TCK, %s )" % (tckBegin, tckEnd))
                                                  , HltFilter("%sHltDecisionFilterTCK%s-%s" % (self.name(), tckBegin, tckEnd), Code=" | ".join("HLT_PASS('%s')" % ln for ln in hltLines))
                                                  , HltVertexConverterS20p3("%sHltConverter%s-%s" % (self.name(), tckBegin, tckEnd), HltLines=hltLines, Recursive=True, Output=revivedHlt2Candidates, WriteP2PVRelations=False, ForceP2PVBuild=False)
                                                  ] )
                        for (tckBegin, tckEnd), hltLines in self.configurationParameter("HLT")["SignalLines"]
                      ]
            )
        hltCandSelection = EventSelection( "%sHltCandidates" % self.name()
                          , Algorithm = hltCandReviver
                          )

        hltVeloGEC = VeloEventShapeCutsS20p3()
        self.validatedSetProps( "VeloGEC", DisplVerticesLinesConf.veloGECCuts, hltVeloGEC )
        hltVeloGEC.HistoProduce = False
        hltVeloGECSel = EventSelection( "%sHltVeloGEC" % self.name(), Algorithm=hltVeloGEC )

        hlt2CandAndGECSelection = Selection( "".join(( self.name(), "Hlt2CandVertices" ))
                          , RequiredSelections = [ hltCandSelection, AutomaticData(revivedHlt2Candidates), hltVeloGECSel ]
                          , Algorithm = FilterDesktop(
                                            Code = "( ABSID == '{pid}' )".format(pid=LLPLHCbName)
                                          , WriteP2PVRelations = False
                                          , ForceP2PVBuild = False
                                          )
                          )

        hlt2CandWithJets = Selection( "".join(( self.name(), "Hlt2CandWithJets" ))
                       , RequiredSelections = [ hlt2CandAndGECSelection ]
                       , Algorithm          = self.makeJetCandidateAlg("".join((self.name(), "HltJetAlg")))
                       )

        #######################################################################
        ###                                                                 ###
        ###     LINE DEFINITIONS                                            ###
        ###                                                                 ###
        #######################################################################
        # one line for every configuratoin key of the format
        # "Single.*Selection"
        # "JetSingle.*Selection"
        # "JetHltSingle.*Selection"
        # "Double.*Selection"
        # ".*HLTPS"
        # "HltEff.*Selection"

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
                     )
            if lShortName in self.configurationParameter("HLT"):
                line.HLT = self.configurationParameter("HLT")[lShortName]

            self.registerLine(line)

        ##========================= Single with jets ==============================##

        jetSingleLineNames = [ p.split("JetSingle")[1].split("Selection")[0]
                                for p in self.configKeys()
                                    if p.startswith("JetSingle")
                                    and p.endswith("Selection")
                             ]

        for lAcroName in jetSingleLineNames:
            lShortName = "JetSingle%s" % lAcroName          # JetSingleMedium
            lSelName   = "%sSelection" % lShortName         # JetSingleMediumSelection
            lLineName  = "%s%s" % (self.name(), lShortName) # DisplVerticesJetSingleMedium

            # Choose between Velo-based and downstream vertexing input
            vertexCandidates = withVeloCandidates
            code = self.getLLPSelection( self.validatedGetProps(lSelName, DisplVerticesLinesConf.singleCuts) )

            vertexFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = DisplVerticesLinesConf.llpSelectionPreambulo
                           , Code               = code
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           #, OutputLevel        = VERBOSE
                           )

            goodVertices = Selection( "".join(( self.name(), lSelName, "Vertices" ))
                           , RequiredSelections = [ vertexCandidates ]
                           , Algorithm          = vertexFilter
                           )

            jetProps = self.validatedGetProps(lSelName, DisplVerticesLinesConf.jetCuts)
            vertWithJets = Selection( "".join(( self.name(), lSelName, "Jets" ))
                           , RequiredSelections = [ goodVertices ]
                           , Algorithm          = self.makeJetCandidateAlg("".join((self.name(), lSelName, "JetAlg"))
                                                      , MinNumJets = jetProps["MinNumJets"]
                                                      )
                           )
            jetCode = self.getLLPJetSelection(jetProps, JetIDCut= jetProps["JetIDCut"])
            jetCandFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = self.jetSelectionPreambulo
                           , Code               = jetCode
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           )
            lineSel = Selection( "".join(( self.name(), lSelName ))
                        , RequiredSelections = [ vertWithJets ]
                        , Algorithm          = jetCandFilter
                        )

            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lSelName, ["PreScale"])["PreScale"]
                     , selection = lineSel
                     )
            if lShortName in self.configurationParameter("HLT"):
                line.HLT = self.configurationParameter("HLT")[lShortName]

            self.registerLine(line)

        ##============= Single with jets based on Hlt candidate ==================##

        jetHltSingleLineNames = [ p.split("JetHltSingle")[1].split("Selection")[0]
                                   for p in self.configKeys()
                                       if p.startswith("JetHltSingle")
                                       and p.endswith("Selection")
                                ]

        for lAcroName in jetHltSingleLineNames:
            lShortName = "JetHltSingle%s" % lAcroName       # JetHltSingleMedium
            lSelName   = "%sSelection" % lShortName         # JetHltSingleMediumSelection
            lLineName  = "%s%s" % (self.name(), lShortName) # DisplVerticesJetHltSingleMedium

            jetProps = self.validatedGetProps(lSelName, DisplVerticesLinesConf.jetCuts)
            jetCode = self.getLLPJetSelection(jetProps, JetIDCut=jetProps["JetIDCut"])
            jetCandFilter = FilterDesktop(
                             DecayDescriptor    = LLPLHCbName
                           , Preambulo          = self.jetSelectionPreambulo
                           , Code               = jetCode
                           , WriteP2PVRelations = False
                           , ForceP2PVBuild     = False
                           )
            lineSel = Selection( "".join(( self.name(), lSelName ))
                        , RequiredSelections = [ hlt2CandWithJets ]
                        , Algorithm          = jetCandFilter
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

            hltSelAlg = GaudiSequenceroid(ModeOR = True, ShortCircuit = False,
                Members = [ GaudiSequencer( "%sHltFilterTCK%s-%s" % (lLineName, tckBegin, tckEnd),
                                            Members = [ ODINFilter("%sODINFilterTCK%s-%s" % (lLineName, tckBegin, tckEnd), Code="in_range( %s, ODIN_TCK, %s )" % (tckBegin, tckEnd))
                                                      , HltFilter("%sHltDecisionFilterTCK%s-%s" % (lLineName, tckBegin, tckEnd), Code=hltFilter) ])
                            for (tckBegin, tckEnd), hltFilter in self.validatedGetProps("HLT", [lShortName])[lShortName]
                          ]
                )

            hltSelection = EventSelection( "%sHltFilter" % lLineName
                              , Algorithm = hltSelAlg
                              )

            line = StrippingLine(lLineName
                     , prescale  = self.validatedGetProps(lShortName, ["PreScale"])["PreScale"]
                     , selection = hltSelection
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
        ##for l in self.lines():
        ##    for alg in l._members:
        ##        alg.PropertiesPrint = True

