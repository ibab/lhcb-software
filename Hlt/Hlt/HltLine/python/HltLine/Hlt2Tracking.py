### @file
#
#  Hlt Tracking.
#
#  @author V. Gligorov   Vladimir.Gligorov@cern.ch 
#
#  Based on code by P. Koppenburg 
#  
#  @date 2010-02-21
#
##
# =============================================================================
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from HltTrackNames import Hlt2LongTracksName, HltSharedTracksPrefix
from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import Hlt2ForwardTracksName, Hlt2SeedingTracksName
from HltTrackNames import Hlt2MatchTracksName, Hlt2DownstreamTracksName   
from HltTrackNames import Hlt2VeloTracksName, HltSharedRZVeloTracksName
from HltTrackNames import Hlt2ChargedProtoParticleSuffix, _trackLocation, _protosLocation
from HltTrackNames import _trackifiedMuonIDLocation, _trackifiedAllMuonIDLocation, _muonIDLocation 
from HltTrackNames import _baseTrackLocation, _shortTrackLocation
from HltTrackNames import HltMuonTracksName, HltAllMuonTracksName
from HltTrackNames import HltSharedMuonIDPrefix, HltSharedMuonIDSuffix 
from HltTrackNames import HltUnfittedTracksSuffix
from HltTrackNames import HltGlobalTrackLocation                
###################################################################################
#
# Hlt2 Tracking
#
class Hlt2Tracking(LHCbConfigurableUser):
    __used_configurables__ = [ "HltCaloRecoConf", "HltCaloPIDsConf", "RichRecSysConf" ]
    __slots__ = {
          "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
        , "Hlt2Tracks"                 : Hlt2ForwardTracksName
        , "Prefix"                     : HltSharedTracksPrefix 
        , "FastFitType"                : HltUnfittedTracksSuffix
        , "UseRICH"                    : False 
        , "UseCALO"		       : False
        , "DoSeeding"                  : False
        , "DoFastFit"                  : False 
        , "DoCloneKilling"             : False 	
        }
###################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return _hlt2ChargedCaloProtos(self)
###################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return _hlt2ChargedHadronProtos(self)
###################################################################################
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles which know nothing about PID
        """
        return _hlt2ChargedProtos(self)
###################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return _hlt2MuonProtos(self)
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return _hlt2NeutralProtos(self)
###################################################################################
    #
    # Velo tracking for the PV making sequence
    #
    def hlt2VeloTracking(self):
        """ 
        Velo tracks
        """
        return _hlt2VeloTracking(self)
###################################################################################
    #
    # Track preparation
    #
    # This will automatically do the required tracking and then, if required
    # do the specified fast-fit as well
    #
    def hlt2PrepareTracks(self):
        """
	The staged fast fit
	"""
	return _hlt2StagedFastFit(self)
###################################################################################
    #
    # Apply the configuration
    # 
    # For now we just print out a big info statement
    #
    def __apply_configuration__(self) :
        log.warning('##############################################################')
        log.warning('## WARNING You have configured an instance of the Hlt2 tracking')
        log.warning('## WARNING ----------------------------------------------------')
        log.warning('## WARNING The data type is '    + str(self.getProp("DataType")))
        log.warning('## WARNING Tracks to make are '  + str(self.getProp("Hlt2Tracks")))
        log.warning('## WARNING The prefix is '       + str(self.getProp("Prefix")))
        log.warning('## WARNING The fit type is '     + str(self.getProp("FastFitType")))
        log.warning('## WARNING Do Fast Fit? = '      + str(self.getProp("DoFastFit")))
        log.warning('## WARNING Do Seeding? = '       + str(self.getProp("DoSeeding")))
        log.warning('## WARNING Do Clone Killing? = ' + str(self.getProp("DoCloneKilling")))
        log.warning('## WARNING Use the RICH? = '     + str(self.getProp("UseRICH")))
        log.warning('## WARNING Use the CALO? = '     + str(self.getProp("UseCALO"))) 
        log.warning('##############################################################')
#########################################################################################
#########################################################################################
#
# Actual implementation of the functions
#
#########################################################################################
def _baseAlgosAndToolsPrefix(self) :
    return self.getProp("Prefix") + _shortTrackLocation(self)
#
def _usedAlgosAndToolsPrefix(self) :
    return self.getProp("Prefix") + self.getProp("FastFitType") + _shortTrackLocation(self)
# In case something went wrong when specifying the options, warn me before you die
def _printWarningBeforeDeath(self) :
    log.fatal( '###############################################################')
    log.fatal( '## WARNING You specified an unknown track type %s for the Hlt2 Reconstruction##' % self.getProp("Hlt2Tracks") )
    log.fatal( '## WARNING I will now die, you need to make Hlt2Tracking aware of this track type!##')
    log.fatal( '###############################################################')
#########################################################################################
#
# Electron Protos
#
def _hlt2ChargedCaloProtos(self) :
    """
    Charged Calo protoparticles = electrons
    Requires chargedProtos
    """
    from Configurables import ( ChargedProtoParticleAddEcalInfo,
                                ChargedProtoParticleAddBremInfo,
                                ChargedProtoParticleAddPrsInfo,
                                ChargedProtoParticleAddSpdInfo,
#                                ChargedProtoParticleAddVeloInfo,
                                ChargedProtoCombineDLLsAlg )
    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    #
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return [] 
    # 
    chargedProtos = _hlt2ChargedProtos(self)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    # The tools
    ecal = ChargedProtoParticleAddEcalInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddEcal")
    brem = ChargedProtoParticleAddBremInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddBrem")
    prs  = ChargedProtoParticleAddPrsInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddPrs")
    spd  = ChargedProtoParticleAddSpdInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddSpd")
    # The output locations
    ecal.ProtoParticleLocation =  chargedProtosOutputLocation
    brem.ProtoParticleLocation =  chargedProtosOutputLocation
    prs.ProtoParticleLocation =  chargedProtosOutputLocation 
    spd.ProtoParticleLocation =  chargedProtosOutputLocation
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPCombDLLsAfterCalo")
    combine.ProtoParticleLocation =  chargedProtosOutputLocation
    # Fill the sequence
    # we need the calo reconstruction, this does not yet work for
    # fitted tracks...
    caloreco = _hlt2Calo(self)
    from HltLine import bindMembers
    return bindMembers( None, [ caloreco, chargedProtos, ecal,brem,prs,spd,combine ]).setOutputSelection(chargedProtosOutputLocation) 
    
#########################################################################################
#
# Muon Protos
#
def _hlt2MuonProtos(self):
    """
    Charged muon protoparticles
    Requires chargedProtos and muon ID
    """
    from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                ChargedProtoCombineDLLsAlg )
   
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #
 
    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    chargedProtos = _hlt2ChargedProtos(self)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    #
    
    muon = ChargedProtoParticleAddMuonInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddMuon")
    muon.ProtoParticleLocation =  chargedProtosOutputLocation
    #Enforce naming conventions
    muonID = _hlt2Muon(self)
    muon.InputMuonPIDLocation = muonID.outputSelection()
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPCombDLLsAfterMuon")
    combine.ProtoParticleLocation =  chargedProtosOutputLocation

    from HltLine import bindMembers
    return bindMembers( None, [ muonID, chargedProtos, muon, combine ] ).setOutputSelection(chargedProtosOutputLocation)  

#########################################################################################
#
# Charged ProtoParticles
# Does not necessarily use RICH
#
def _hlt2ChargedHadronProtos(self):
    """
    Charged hadron protoparticles = pion, kaon, proton
    If RICH is on, then requires rich, calo and does dll combination
    This is off by default
    """
    from Configurables import (	ChargedProtoParticleAddRichInfo,
				ChargedProtoParticleAddMuonInfo,
                                ChargedProtoParticleAddHcalInfo,
                                ChargedProtoCombineDLLsAlg)
    from HltLine import bindMembers
    from RichRecSys.Configuration import (RichRecSysConf,
					  RichTrackCreatorConfig,
                               		  RichPixelCreatorConfig
					  )

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #

    #The different add PID algorithms
    #
    # The charged protoparticles and their output location
    chargedProtos = _hlt2ChargedProtos(self)
    chargedProtosOutputLocation = chargedProtos.outputSelection()
    #

    if ( self.getProp("UseRICH") and not self.getProp("UseCALO")):
        
        #configure the rich reco
	richseq = GaudiSequencer("HltRICHReco")
	richseq.MeasureTime = True
	# The RICH COnfigurable
	richConf = RichRecSysConf()
	# Set the sequence to run the RICH PID in
	richConf.setProp("RecoSequencer",richseq)
	# Configure for Fast HLT Global PID
	richConf.Context   = "HLT"
	richConf.PidConfig = "FastGlobal"
	# Don't bother checking ProcStatus online
	richConf.CheckProcStatus = False
	# Initial pixels tracks and photons
	richConf.InitPixels  = True
	richConf.InitTracks  = True
	richConf.InitPhotons = True
	# Turn off ring finding
	richConf.TracklessRingAlgs = []
	# Turn off book keeping for pixels, for speed
	RichPixelCreatorConfig().BookKeeping  = False
	RichPixelCreatorConfig().FindClusters = False
        
	# Create RICH PID using the Fitted tracks
        # This is something of a special case because
        # the RICH ID simply doesn't work without a bidirectional track fit
        # TODO: write some checks at the __apply_configuration__ stage
        #       that the created configuration makes sense.   
        # TODO : this will work so long as only one instance of Hlt2Tracking ever uses the RICH
        #        we either need to fix this or build in some kind of configuration time
        #        safety mechanism! 
	RichTrackCreatorConfig().InputTracksLocation = _hlt2StagedFastFit(self).outputSelection()
        # The Rich info 
        rich = ChargedProtoParticleAddRichInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddRich")
        rich.ProtoParticleLocation =  chargedProtosOutputLocation 
	# The combined DLL
        combine = ChargedProtoCombineDLLsAlg(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPCombDLLsAfterRich")
        combine.ProtoParticleLocation =  chargedProtosOutputLocation

        return bindMembers( None , [ chargedProtos, richseq, rich, combine ] ).setOutputSelection(chargedProtosOutputLocation)
    else :
    	#delegate everything to the charged proto maker
	#because the CALO does not work for now! This needs to be fixed!!!
        return bindMembers( None , [ chargedProtos ] ).setOutputSelection(chargedProtosOutputLocation)

    # The HCAL info
    #hcal = ChargedProtoParticleAddHcalInfo(_usedAlgosAndToolsPrefix(self)+"ChargedProtoPAddHcal")
    #hcal.ProtoParticleLocation = protoslocation
    # The calo DLL
    #caloID = _hlt2Calo(Hlt2Tracks, prefix, fastFitType)
    
#########################################################################################
#
# Charged ProtoParticles
#
def _hlt2ChargedProtos(self):
    """
    Charged protoparticles
    
    Requires tracks, fitted if necessary
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #

    from Configurables import ( ChargedProtoParticleMaker,
                                DelegatingTrackSelector  )
    
    chargedProtosOutputLocation = _protosLocation(self,Hlt2ChargedProtoParticleSuffix)

    charged = ChargedProtoParticleMaker(_usedAlgosAndToolsPrefix(self)+'ChargedProtoPAlg')
    charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
    # Next bit separates long and downstream tracks in the particle making
    # TODO: remove hardcoding of track types!
    # TODO: are these lines even necessary?? 	
    if (outputOfHlt2Tracking == Hlt2LongTracksName or outputOfHlt2Tracking == Hlt2ForwardTracksName) : 
	charged.TrackSelector.TrackTypes = [ "Long" ] 
    elif (outputOfHlt2Tracking == Hlt2DownstreamTracksName) :
	charged.TrackSelector.TrackTypes = [ "Downstream" ]
   
    # Need to allow for fitted tracks
    # This is now done inside the staged fast fit based on the fastFitType passed
    tracks = _hlt2StagedFastFit(self)
    charged.InputTrackLocation = tracks.outputSelection()

    charged.OutputProtoParticleLocation =  chargedProtosOutputLocation

    from HltLine import bindMembers
    return bindMembers ( None, [ tracks , charged ] ).setOutputSelection(chargedProtosOutputLocation)

#########################################################################################
#
# Neutral ProtoParticles
#
def _hlt2NeutralProtos(self):
    """
    Neutral protoparticles 
    Requires caloID
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #

    from Configurables import NeutralProtoPAlg
    HltNeutralProtoPAlg = NeutralProtoPAlg(_usedAlgosAndToolsPrefix(self)+'NeutralProtoPAlg')
    # Overwrite some default offline settings with HLT special settings (
    # taken from CaloReco.opts)
    HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
    from Configurables import CaloPhotonEstimatorTool
    ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
    ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
    importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )
    
    caloID = _hlt2Calo(self)
    
    from HltLine import bindMembers
    return bindMembers ( None, [ caloID, HltNeutralProtoPAlg ]) 
    
#########################################################################################
#
# MuonID
#
def _hlt2Muon(self) :
    """
    Muon ID options 
    Requires tracks
    """

    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #

    from MuonID import ConfiguredMuonIDs
    from Configurables import MuonRec, MuonIDAlg
    cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
    HltMuonIDAlg = cm.configureMuonIDAlg(_usedAlgosAndToolsPrefix(self)+"MuonIDAlg")
    #The tracks to use
    tracks = _hlt2StagedFastFit(self)
    #Enforce naming conventions
    HltMuonIDAlg.TrackLocation        = tracks.outputSelection() 
    HltMuonIDAlg.MuonIDLocation       = _muonIDLocation(self) #output 
    HltMuonIDAlg.MuonTrackLocation    = _trackifiedMuonIDLocation(self) 
    HltMuonIDAlg.MuonTrackLocationAll = _trackifiedAllMuonIDLocation(self) 
    # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
    #       a factor 3-4
    HltMuonIDAlg.FindQuality = False
    
    from HltLine import bindMembers
    return bindMembers ( None, [ tracks, MuonRec(), HltMuonIDAlg ] ).setOutputSelection(HltMuonIDAlg.MuonIDLocation)
#########################################################################################
#
# Calo ID
#
def _hlt2Calo(self) :
    """
    Calo ID
    Does calo reconstruction and Calo ID
    Requires tracks
    @todo : make this track location dependent
    """
    from CaloReco.Configuration import HltCaloRecoConf 
    from CaloPIDs.Configuration import HltCaloPIDsConf
    Hlt2CaloRecoSeq = GaudiSequencer("Hlt2CaloRecoSeq")
    Hlt2CaloPIDsSeq = GaudiSequencer("Hlt2CaloPIDsSeq")
    caloReco = HltCaloRecoConf(Sequence           = Hlt2CaloRecoSeq,
                               Context            = "HLT")
    caloPID  = HltCaloPIDsConf(Sequence           = Hlt2CaloPIDsSeq,
                               Context            = "HLT")
    Hlt2CaloSeq = GaudiSequencer("Hlt2CaloSeq", Context = "HLT" )
    from HltLine import bindMembers
    tracks = _hlt2StagedFastFit(self)
    return bindMembers( None, [ tracks, Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq   ] )
#########################################################################################
#
# Staged fast fit
#
def _hlt2StagedFastFit(self) :
    """
    The staged fast-fit. Currently set to
    be bidirectional and with smoothing to allow PID information
    to be used; special cases need to be justified separately
    """

    from HltLine import bindMembers
    
    # First of all check that I have been called with a sensible set of options
    #
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
        return []
    #Make the original tracks in case this has not been run already
    tracks = _hlt2Tracking(self)
    #And my output location is?
    hlt2StagedFastFitOutputLocation = _trackLocation(self)
    # Second check: have I actually been asked to fit the tracks?
    # If not, just return the unfitted tracks copied into the container specified by
    # the fast-fit type field
    if (not self.getProp("DoFastFit")) :
	from Configurables import CreateFastTrackCollection
    	#### CreateFastTrackCollection
    	# this can only be done ONCE per long track output
    	recoCopy = CreateFastTrackCollection( _usedAlgosAndToolsPrefix(self)+'TrackCopy'
                , InputLocations = [tracks.outputSelection()]
                , OutputLocation = hlt2StagedFastFitOutputLocation
                , SlowContainer = True)
	return bindMembers(None, [tracks, recoCopy]).setOutputSelection(hlt2StagedFastFitOutputLocation)

    from Configurables import TrackEventFitter, TrackMasterFitter
    Hlt2StagedFastFitSeq = GaudiSequencer( _usedAlgosAndToolsPrefix(self)+"StagedFastFitSeq" )
    
    Hlt2StagedFastFit = TrackEventFitter(_usedAlgosAndToolsPrefix(self)+'StagedFastFit')
    Hlt2StagedFastFit.TracksInContainer  = tracks.outputSelection() 
    Hlt2StagedFastFit.TracksOutContainer =  hlt2StagedFastFitOutputLocation  
    Hlt2StagedFastFit.addTool(TrackMasterFitter, name = 'Fitter')
   
    Hlt2StagedFastFitSeq.Members = [ Hlt2StagedFastFit ]
    
    from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
    fitter = ConfiguredFastFitter( getattr(Hlt2StagedFastFit,'Fitter'))
    fitter.NodeFitter.BiDirectionalFit = True
    fitter.NodeFitter.Smooth = True
    fitter.AddDefaultReferenceNodes = True    # says Wouter
    
    return bindMembers( None, [tracks, Hlt2StagedFastFitSeq] ).setOutputSelection(hlt2StagedFastFitOutputLocation)

#########################################################################################
#
# Track reconstruction
#
def _hlt2Tracking(self) :
    """
    Track reconstruction
    """

    #
    # Some "inevitable" special stuff here
    # If we ask for long tracks and no seeding, the final output will be
    # the forward tracking, otherwise the final output is the long 
    # (forward + match) tracking.
    #
    # First we figure out what the final output container will be
    outputOfHlt2Tracking = _shortTrackLocation(self)
    if (outputOfHlt2Tracking == 'Unknown') :
        _printWarningBeforeDeath(self)
	return []  
    hlt2TrackingOutput = _baseTrackLocation(self.getProp("Prefix"),_shortTrackLocation(self))
    #
    # The copy sequence, which merges the forward and
    # match tracks into the long track container
    # This only runs if the seeding is run and we ask for Long tracks
    # TODO: fix this ugly hacked way of getting the output locations...
    hlt2TracksToMergeIntoLong = [ _hlt2ForwardTracking(self)[0].OutputTracksName, _hlt2MatchTracking(self)[0].MatchOutput ]
    from Configurables import CreateFastTrackCollection
    #### CreateFastTrackCollection
    # this can only be done ONCE per long track output
    recoCopy = CreateFastTrackCollection(_baseAlgosAndToolsPrefix(self)+'TrackCopy'
                , InputLocations = hlt2TracksToMergeIntoLong
                , OutputLocation = _baseTrackLocation(self.getProp("Prefix"),_shortTrackLocation(self))
                , SlowContainer = True)
  
    from HltLine import bindMembers
    #Now let us make the reconstruction sequence here
    #The VELO parts
    hlt2Velotracking = _hlt2VeloTracking(self)
    # The tracking decoding
    from HltDecodeRaw import DecodeTT, DecodeIT
    hlt2TrackerDecoding = DecodeTT.members() + DecodeIT.members()
    # The forward tracking
    hlt2ForwardTracking = _hlt2ForwardTracking(self)
    # The seeding
    hlt2SeedTracking = _hlt2SeedTracking(self)
    # The matching
    hlt2MatchTracking = _hlt2MatchTracking(self)
    # The downstream tracking 
    hlt2DownstreamTracking = _hlt2DownstreamTracking(self)
    # The clone killing
    from Configurables import TrackEventCloneKiller, TrackCloneFinder
    cloneKiller = TrackEventCloneKiller(_baseAlgosAndToolsPrefix(self)+'FastCloneKiller'
                , TracksInContainers = [hlt2TrackingOutput]
                , SkipSameContainerTracks = False
                , CopyTracks = False)
    cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
    cloneKiller.CloneFinderTool.RestrictedSearch = True
    # Finally make the sequence
    # The sequence called depends on the track type, so far we recognise two track types
    # for Long tracks the options doSeeding, doCloneKilling, etc. are meaningful
    # for SeedTT tracks the seeding is mandatory and the CloneKilling is irrelevant 
    # This part gets done in either case
    trackRecoSequence = GaudiSequencer( _baseAlgosAndToolsPrefix(self)+'TrackRecoSequence'
                                  ,  Members = hlt2Velotracking.members() + hlt2TrackerDecoding + hlt2ForwardTracking) 
    if (self.getProp("Hlt2Tracks") == Hlt2LongTracksName) :
	# Do the seeding +matching if required
	if doSeeding : 
		trackRecoSequence.Members += hlt2SeedTracking
		trackRecoSequence.Members += hlt2MatchTracking
		trackRecoSequence.Members += [recoCopy]
	# Do the clone killing if required 
	if self.getProp("DoCloneKilling") : trackRecoSequence.Members += [cloneKiller]  
    elif (self.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName ) :
	# The seeding is mandatory for downstream tracks
	trackRecoSequence.Members += hlt2SeedTracking
	# Now do the downstream tracking
	trackRecoSequence.Members += hlt2DownstreamTracking
    # Bind it all together
    HltRecoSequence = bindMembers(None, trackRecoSequence.Members ) 
    #
    # Hacking of errors
    # @todo Needs to be revised
    #
    # check if the context contains the magic phrase 'Fitted', if so
    # either the tracks were already fitted, 
    # or we need to do the Kalman fit on-demand afterwards, but
    # either way we do NOT parametarize the errors
    # Also, do not insert errors for downstream tracks as these would be meaningless
    if (self.getProp("DoFastFit") or (self.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName)) :
	return bindMembers( None, [ HltRecoSequence ] ).setOutputSelection(hlt2TrackingOutput)
    else :
        from Configurables import HltInsertTrackErrParam
   	HltInsertTrackErrParam = HltInsertTrackErrParam(_baseAlgosAndToolsPrefix(self)+"InsertTrackErrParam")
   	HltInsertTrackErrParam.InputLocation = hlt2TrackingOutput
   	#
  	# Reco sequence
 	#
 	return bindMembers( None, [ HltRecoSequence, HltInsertTrackErrParam ]).setOutputSelection(hlt2TrackingOutput) 
#########################################################################################
#
# Hlt2 Velo Reconstruction
#
def _hlt2VeloTracking(self):
    """
    Velo track reconstruction for Hlt2
    """
    from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking, Tf__PatVeloSpaceTool
    #From HltReco we just get the shared stuff between Hlt1 and Hlt2
    from HltReco import MinimalRZVelo
    from HltLine import bindMembers 

    veloTracksOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2VeloTracksName) 
   
    recoVelo = Tf__PatVeloSpaceTracking(self.getProp("Prefix")+'RecoVelo'
                                   , InputTracksName = MinimalRZVelo.outputSelection() 
                                   , OutputTracksName = veloTracksOutputLocation )

    recoVelo.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
    recoVelo.PatVeloSpaceTool.MarkClustersUsed=True

    recoVeloGeneral = Tf__PatVeloGeneralTracking(self.getProp("Prefix")+'RecoVeloGeneral'
                                   , OutputTracksLocation = veloTracksOutputLocation )

    return bindMembers(None, MinimalRZVelo.members() + [recoVelo,recoVeloGeneral]).setOutputSelection(veloTracksOutputLocation)
#########################################################################################
#
# Hlt2 Forward Reconstruction
#
def _hlt2ForwardTracking(self) :
    """
    Forward track reconstruction for Hlt2
    """
    from Configurables import PatForward, PatForwardTool
    forwardTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2ForwardTracksName) 

    recoForward = PatForward( self.getProp("Prefix")+'RecoForward'
                        , InputTracksName = _hlt2VeloTracking(self).outputSelection() 
                        , OutputTracksName = forwardTrackOutputLocation )
    PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
    recoForward.addTool(PatForwardTool, name='PatForwardTool')
    recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
    return [recoForward]
#########################################################################################
#
# Hlt2 Seeding for forward,downstream reconstruction
#
def _hlt2SeedTracking(self):
    """
    Seeding in the trackers for later use in Match Forward reconstruction
    or in downstream tracking.
    """
    from Configurables import PatSeeding, PatSeedingTool

    seedTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2SeedingTracksName)

    #### Seeding 
    recoSeeding = PatSeeding(self.getProp("Prefix")+'Seeding', OutputTracksName = seedTrackOutputLocation)
    recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
    recoSeeding.PatSeedingTool.UseForward = True
    recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
    #TODO: Should fix this bit of hardcoded output type...
    recoSeeding.PatSeedingTool.InputTracksName = _hlt2ForwardTracking(self)[0].OutputTracksName

    return [recoSeeding]

#########################################################################################
#
# Hlt2 Match Forward Reconstruction
#
def _hlt2MatchTracking(self):
    """
    Forward track reconstruction for Hlt2 using seeding
    """
    from Configurables import PatMatch

    matchTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2MatchTracksName)

    #### Matching
    #TODO: Should fix this bit of hardcoded output type...
    recoMatch = PatMatch(self.getProp("Prefix")+'Match'
                     , VeloInput = _hlt2VeloTracking(self).outputSelection()
                     , SeedInput = _hlt2SeedTracking(self)[0].OutputTracksName
                             , MatchOutput = matchTrackOutputLocation)

    return [recoMatch]

#########################################################################################
#
# Hlt2 Downstream Forward Reconstruction
#
def _hlt2DownstreamTracking(self):
    """
    Downstream track reconstruction for Hlt2 using seeding
    """
    from Configurables import PatDownstream

    downstreamTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2DownstreamTracksName)

    ### Downstream tracking
    #TODO: Should fix this bit of hardcoded output type...
    PatDownstream = PatDownstream(self.getProp("Prefix")+'PatDownstream')
    PatDownstream.InputLocation = _hlt2SeedTracking(self)[0].OutputTracksName
    PatDownstream.OutputLocation = downstreamTrackOutputLocation
    #PatDownstream.UseForward = True
    #PatDownstream.SeedFilter = True
    PatDownstream.RemoveUsed = True
    PatDownstream.RemoveAll = True

    return [PatDownstream]
