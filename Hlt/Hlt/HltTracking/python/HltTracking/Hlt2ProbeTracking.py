### @file
#
#  Hlt2 Probe tracking for tracking efficiency lines
#
#  !!!DO NOT USE THESE TRACKS FOR NORMAL HLT2 LINES!!!
#
#  @author M. Kolpin   michael.kolpin@cern.ch
#
#  @date 2015-01-29
#
##
# =============================================================================
__author__  = "M. Kolpin michael.kolpin@cern.ch"
__version__ = "1.0"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
#from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import HltDefaultFitSuffix
from HltTrackNames import TrackName, TrackEffNames, Hlt2TrackRoot, Hlt2TrackLoc, Hlt1TrackLoc, HltSharedTrackLoc, Hlt2TrackEffLoc

from HltTrackNames import _baseTrackLocation, _baseProtoPLocation
from HltTrackNames import HltMuonTracksName, HltAllMuonTracksName
from HltTrackNames import HltMuonIDSuffix, HltRICHIDSuffix, HltCALOIDSuffix, HltSharedPIDPrefix

from Configurables import CaloProcessor, RichRecSysConf, TrackSelector

#################################################################################################
#
# Hlt2 Probe Tracking
#
class Hlt2ProbeTracking(LHCbConfigurableUser):
    #############################################################################################
    #
    # First of all the dependencies and the slots
    #
    #############################################################################################
    __slots__ = { "DataType"                        : '2012'
                , "ProbeTrack"                      : "MuonTT"  #Define probe track type
                , "Prefix"                          : "Hlt2"   # Why should we need this? It's never changed
                }
    #############################################################################################
    #############################################################################################
    #
    # Externally visible methods to return the created bindMembers objects
    #
    #############################################################################################
    #############################################################################################
    #
    # Charged ProtoParticles with Muon ID
    #
    def hlt2ProbeMuonProtos(self):
        """
        Charged probe protoparticles
        """
        return self.__hlt2ProbeMuonProtos()
    #############################################################################################
    #
    # VeloMuon tracking
    #
    def hlt2VeloMuonTracking(self):
        """
        VeloMuon tracks
        """
        return self.__hlt2VeloMuonTracking()
    #############################################################################################
    #
    # MuonTT tracking
    #
    def hlt2MuonTTTracking(self):
        """
        MuonTT tracks
        """
        return self.__hlt2MuonTTTracking()
    #############################################################################################
    #
    # Full Downstream tracking (seeding with all hits)
    #
    def hlt2FullDownstreamTracking(self):
        """
        Full Downstream tracks
        """
        return self.__hlt2FullDownstreamTracking()
    #############################################################################################
    #############################################################################################
    #
    # The apply configuration method, makes the bindMembers objects which can then
    # be exported, and also checks that the configuration is not inconsistent
    #
    #############################################################################################
    #############################################################################################
    def __apply_configuration__(self) :
        log.debug('############################################################')
        log.debug('## INFO You have configured an instance of the Hlt2 probe tracking')
        log.debug('## INFO ----------------------------------------------------')
        log.debug('## INFO The data type is %s'     % self.getProp("DataType"       ))
        log.debug('## INFO Tracks to make are %s'   % self.getProp("ProbeTrack"     ))
        log.debug('#################################%##########################')
        #
        # First of all check that I have been called with a sensible set of options
        #
        outputOfHlt2ProbeTracking = self.trackType()
        if (outputOfHlt2ProbeTracking == 'Unknown') :
            self.__fatalErrorUnknownTrackType()
            return []
    #############################################################################################
    #############################################################################################
    #
    # Actual implementation of the functions
    #
    #############################################################################################
    #
    # Start with the naming conventions
    #
    # The track type for the derived tracks
    # This function checks that the track asked for by the Hlt2Tracking instance
    # is in the recognised track types, and returns "Unknown" or the correct
    # suffix based on the configuration of the Hlt2Tracking.
    #
    def trackType(self) :
        if ( self.getProp("ProbeTrack") == "MuonTT" ) :
            return "MuonTT"
        elif ( self.getProp("ProbeTrack") == "VeloMuon") :
            return "VeloMuon"
        elif ( self.getProp("ProbeTrack") == "FullDownstream") :
            return "FullDownstream"
        else :
            return "Unknown"

    #
    # The prefixes for the various tools and algorithms used
    #
    def __trackingAlgosAndToolsPrefix(self) :
        return self.getProp("Prefix") + self.trackType()
    #
    def __pidAlgosAndToolsPrefix(self) :
        return self.name()
    #
    # In case something went wrong when specifying the options, warn me before you die
    #
    def __fatalErrorUnknownTrackType(self) :
        log.fatal( '##################################################################################')
        log.fatal( '## FATAL You specified an unknown track type %s for the Hlt2 Probe Reconstruction' % self.getProp("ProbeTrack") )
        log.fatal( '## FATAL I will now die, you need to make Hlt2ProbeTracking aware of this track type!  ')
        log.fatal( '##################################################################################')
    #
    #########################################################################################
    #
    # Probe Muon Protos
    #
    def __hlt2ProbeMuonProtos(self):
        """
        Charged muon protoparticles
        Requires chargedProtos and muon ID
        """
        from Configurables import ( ChargedProtoParticleAddMuonInfo
                                   , ChargedProtoCombineDLLsAlg
                                   , MuonIDAlg
                                   , ChargedProtoParticleMaker
                                   , DelegatingTrackSelector
				   )
        from MuonID	      import ConfiguredMuonIDs

        ProbeMuonProtosOutputLocation = _baseProtoPLocation("Hlt2", HltDefaultFitSuffix+"/"+TrackEffNames[self.trackType()])

        Hlt2ProbeMuonProtoMaker = ChargedProtoParticleMaker(self.__pidAlgosAndToolsPrefix()+'ProbeProtoPAlg')


        if (self.trackType() == "MuonTT" ) :
                # create the protos
                bm_members = [ self.__hlt2MuonTTTracking() ]
                Hlt2ProbeMuonProtoMaker.Inputs = [self.__hlt2MuonTTTracking().outputSelection()]
                Hlt2ProbeMuonProtoMaker.addTool( DelegatingTrackSelector )
                Hlt2ProbeMuonProtoMaker.Output = ProbeMuonProtosOutputLocation
                Hlt2ProbeMuonProtoMaker.DelegatingTrackSelector.TrackTypes = [ "Long" ]
                bm_members += [Hlt2ProbeMuonProtoMaker]

        elif (self.trackType() == "VeloMuon" ) :
                #build protos out of tracks
                bm_members = [ self.__hlt2VeloMuonTracking() ]
                Hlt2ProbeMuonProtoMaker.Inputs = [self.__hlt2VeloMuonTracking().outputSelection()]
                Hlt2ProbeMuonProtoMaker.Output = ProbeMuonProtosOutputLocation
                Hlt2ProbeMuonProtoMaker.addTool(DelegatingTrackSelector, name='delTrackSel')
                Hlt2ProbeMuonProtoMaker.delTrackSel.TrackTypes = ['Long']
                bm_members += [Hlt2ProbeMuonProtoMaker]

        elif (self.trackType() == "FullDownstream" ) :
                # add muon ID
                bm_members = [ self.__hlt2FullDownstreamTracking() ]
                idalgname = self.__pidAlgosAndToolsPrefix()+"IDalg"
                cm=ConfiguredMuonIDs.ConfiguredMuonIDs( data = self.getProp("DataType") )
                idalg = cm.configureMuonIDAlgLite(idalgname)
                idalg.TracksLocations = [ self.__hlt2FullDownstreamTracking().outputSelection() ]
                idalg.MuonIDLocation = Hlt2TrackRoot+HltDefaultFitSuffix+"/"+HltSharedPIDPrefix+"/"+HltMuonIDSuffix
                idalg.MuonTrackLocation = Hlt2TrackRoot+HltDefaultFitSuffix+"/"+HltSharedPIDPrefix+"/"+HltMuonTracksName
                # make protos
                Hlt2ProbeMuonProtoMaker.Inputs = [self.__hlt2FullDownstreamTracking().outputSelection()]
                Hlt2ProbeMuonProtoMaker.Output = ProbeMuonProtosOutputLocation
                Hlt2ProbeMuonProtoMaker.addTool( DelegatingTrackSelector, name="TrackSelector" )
                tracktypes = ["Downstream"]
                Hlt2ProbeMuonProtoMaker.TrackSelector.TrackTypes = tracktypes
                addmuonpid = ChargedProtoParticleAddMuonInfo("addmuonpid")
                addmuonpid.InputMuonPIDLocation = idalg.MuonIDLocation
                addmuonpid.ProtoParticleLocation = Hlt2ProbeMuonProtoMaker.Output
                combinedll = ChargedProtoCombineDLLsAlg("combineDLL")
                combinedll.ProtoParticleLocation = Hlt2ProbeMuonProtoMaker.Output
                bm_members += [ idalg, Hlt2ProbeMuonProtoMaker, addmuonpid, combinedll ]

        from HltLine.HltLine import bindMembers
        # Build the bindMembers

        bm_name         = self.__pidAlgosAndToolsPrefix()+"ProbeMuonProtosSeq"
        bm_output       = ProbeMuonProtosOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Probe Track reconstruction
    #
    def __hlt2ProbeTracking(self) :
        """
        Track reconstruction
        """
        from HltLine.HltLine import bindMembers
        #
        # Set output location
        #
        hlt2TrackingOutput      = Hlt2TrackLoc[self.trackType()]

        # Finally make the sequence
        trackRecoSequence = []
        if (self.trackType() == "MuonTT") :
            trackRecoSequence        =    [self.__hlt2MuonTTTracking()]
        elif (self.trackType() == "VeloMuon") :
            trackRecoSequence        =     [self.__hlt2VeloMuonTracking()]
        elif (self.trackType() == "FullDownstream" ) :
            trackRecoSequence         =    [self.__hlt2FullDownstreamTracking()]

        # Build the bindMembers
        bm_name         = self.__trackingAlgosAndToolsPrefix()+"TrackingSeq"
        bm_members      = trackRecoSequence
        bm_output       = hlt2TrackingOutput

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 VeloMuon Reconstruction
    #
    def __hlt2VeloMuonTracking(self):
        """
        VeloMuon track reconstruction for Hlt2
        """
        #HltSharedTracking decides which Velo sequence is used
        from Configurables  import Hlt2Conf, VeloMuonBuilder, StandaloneMuonRec, TrackMasterFitter
        from Configurables  import MeasurementProviderT_MeasurementProviderTypes__VeloLiteR_, MeasurementProviderT_MeasurementProviderTypes__VeloLitePhi_
        from HltLine.HltLine    import bindMembers

        # Define output location
        VeloMuonTracksOutputLocation = Hlt2TrackEffLoc["VeloMuon"]

        # Get Muon standalone track
        Hlt2StandaloneMuon = StandaloneMuonRec("Hlt2StandaloneMuon")
        Hlt2StandaloneMuon.OutputMuonTracksName = Hlt2TrackEffLoc["StandaloneMuon"]

        #build VeloMuon track
        Hlt2VeloMuonBuild = VeloMuonBuilder('Hlt2VeloMuonBuild')
        Hlt2VeloMuonBuild.MuonLocation = Hlt2StandaloneMuon.OutputMuonTracksName
        Hlt2VeloMuonBuild.VeloLocation = HltSharedTrackLoc["Velo"] #Velo track location in Hlt
        Hlt2VeloMuonBuild.lhcbids = 4
        Hlt2VeloMuonBuild.OutputLocation = VeloMuonTracksOutputLocation
        Hlt2VeloMuonBuild.addTool( TrackMasterFitter )
        from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
        ConfiguredMasterFitter( Hlt2VeloMuonBuild.TrackMasterFitter , SimplifiedGeometry = True, LiteClusters = True) #on par with Hlt track fits
        Hlt2VeloMuonBuild.TrackMasterFitter.OutputLevel = 5

        # Build the bindMembers
        bm_name         = self.getProp("Prefix")+"VeloMuonTracking"
        bm_members      = [ Hlt2StandaloneMuon, Hlt2VeloMuonBuild ]
        bm_output       = VeloMuonTracksOutputLocation


        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 MuonTT Reconstruction
    #
    def __hlt2MuonTTTracking(self) :
        """
        MuonTT track reconstruction for Hlt2
        """
        from Configurables      import Hlt2Conf
        from Configurables      import MuonTTTrack, TrackMasterFitter, PatAddTTCoord, MuonCombRec, MuonHitDecode, TrackMasterExtrapolator
        from Configurables  	import MeasurementProviderT_MeasurementProviderTypes__TTLite_
        from HltLine.HltLine    import bindMembers
        from Configurables      import HltRecoConf
        from HltRecoConf import CommonForwardTrackingOptions, MuonTTOptions
        from HltTracking.HltPVs import PV3D
        #        from Hlt1Lines.HltConfigurePR import ConfiguredPR

        pvreco = PV3D('Hlt2')
        Hlt2MuonTTTrack = MuonTTTrack("Hlt2MuonTTTrack")
        Hlt2MuonTTTrack.AddTTHits = True
        Hlt2MuonTTTrack.FillMuonStubInfo = False
        Hlt2MuonTTTrack.ToolName = "MuonCombRec"
        Hlt2MuonTTTrack.OutputLevel = 6
        Hlt2MuonTTTrack.MC = False
        Hlt2MuonTTTrack.PVLocation = pvreco.output
        Hlt2MuonTTTrack.addTool( MuonCombRec )
        Hlt2MuonTTTrack.MuonCombRec.ClusterTool       = "MuonFakeClustering"
        Hlt2MuonTTTrack.MuonCombRec.CloneKiller       = False
        Hlt2MuonTTTrack.MuonCombRec.StrongCloneKiller = False
        Hlt2MuonTTTrack.MuonCombRec.SeedStation       = MuonTTOptions["SeedStation"]
        Hlt2MuonTTTrack.MuonCombRec.DecodingTool      = "MuonHitDecode"
        Hlt2MuonTTTrack.MuonCombRec.PadRecTool        = "MuonPadRec"
        Hlt2MuonTTTrack.MuonCombRec.AssumePhysics     = True
        Hlt2MuonTTTrack.MuonCombRec.MeasureTime       = True
        Hlt2MuonTTTrack.MuonCombRec.addTool(MuonHitDecode("MuonHitDecode"))
        Hlt2MuonTTTrack.MuonCombRec.MuonHitDecode.SkipHWNumber = True
        Hlt2MuonTTTrack.addTool( PatAddTTCoord )
        Hlt2MuonTTTrack.PatAddTTCoord.YTolSlope  = MuonTTOptions["YTolSlope"]
        Hlt2MuonTTTrack.PatAddTTCoord.XTol       = MuonTTOptions["XTol"]
        Hlt2MuonTTTrack.PatAddTTCoord.XTolSlope  = MuonTTOptions["XTolSlope"]
        Hlt2MuonTTTrack.PatAddTTCoord.MaxChi2Tol = MuonTTOptions["MaxChi2Tol"]
        Hlt2MuonTTTrack.PatAddTTCoord.MinAxProj  = MuonTTOptions["MinAxProj"]
        Hlt2MuonTTTrack.PatAddTTCoord.MajAxProj  = MuonTTOptions["MaxAxProj"]
        Hlt2MuonTTTrack.addTool( TrackMasterFitter )
        from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
        ConfiguredMasterFitter( Hlt2MuonTTTrack.TrackMasterFitter , SimplifiedGeometry = True, LiteClusters = True) #on par with Hlt track fits
        Hlt2MuonTTTrack.TrackMasterFitter.OutputLevel = 5
        Hlt2MuonTTTrack.addTool( TrackMasterExtrapolator )
        Hlt2MuonTTTrack.TrackMasterExtrapolator.MaterialLocator = "SimplifiedMaterialLocator"
        Hlt2MuonTTTrack.Output = Hlt2TrackEffLoc["MuonTT"]


        # Build the bindMembers
        bm_name         = self.getProp("Prefix")+"MuonTTTracking"
        bm_members      = [pvreco, Hlt2MuonTTTrack]
        bm_output       = Hlt2MuonTTTrack.Output

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Full Downstream Reconstruction
    #
    def __hlt2FullDownstreamTracking(self):
        """
        Full Downstream track reconstruction and fitting for tracking efficiency lines
        """
        from Configurables    import PatSeeding
        from Configurables      import PatSeedingTool
        from HltLine.HltLine    import bindMembers

        from Configurables import TrackEventFitter, TrackMasterFitter
        DownstreamFit = TrackEventFitter('DownstreamFitter')
        DownstreamFit.TracksInContainer = Hlt2TrackLoc["Downstream"] #use unfitted Downstream tracks out of Hlt2Tracking
        DownstreamFit.TracksOutContainer = Hlt2TrackEffLoc["FullDownstream"] #our outputlocation
        DownstreamFit.addTool(TrackMasterFitter, name = 'Fitter')
        from TrackFitter.ConfiguredFitters import ConfiguredHltFitter, ConfiguredMasterFitter
        DownstreamFitter = getattr(DownstreamFit,'Fitter')
        ConfiguredMasterFitter(DownstreamFitter, SimplifiedGeometry = True, LiteClusters = True) #on par with Hlt track fits
        DownstreamFitter.OutputLevel = 5

        # Build the bindMembers
        bm_name         = self.getProp("Prefix")+"FullDownstreamTracking"
        bm_members      = [ DownstreamFit ]
        bm_output       = Hlt2TrackEffLoc["FullDownstream"]

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
