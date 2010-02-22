### @file
#
#  Hlt PID. Two (so far one) configurables and then generic implementation methods
#
#  @author V. Gligorov   Vladimir.Gligorov@cern.ch 
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2010-02-21
#
##
# =============================================================================
__author__  = "P. Koppenburg Patrick.Koppenburg@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.14 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
###################################################################################
#
# Hlt2 PID
#
class Hlt2PID(LHCbConfigurableUser):
    __used_configurables__ = [ "HltCaloRecoConf", "HltCaloPIDsConf", "RichRecSysConf" ]
    __slots__ = {
        "DataType"                   : '2009'    # datatype is one of 2009, MC09, DC06...
        , "Hlt2Tracks"                 : "Long"
        , "Prefix"                     : "Hlt"     # default prefix for all instance names (almost common)
        , "Suffix"                     : "" #or Fitted, FittedRich, etc.
        , "UseRICH"                    : False 
        , "UseCALO"		       : False
        , "DoSeeding"                  : False 
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
        return _hlt2ChargedCaloProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                     prefix = self.getProp("Prefix"), 
                                     suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))
###################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return _hlt2ChargedHadronProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                       prefix = self.getProp("Prefix"),
                                       suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"),
                                       UseRICH= self.getProp("UseRICH"),
				       UseCALO= self.getProp("UseCALO"))
###################################################################################
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles which know nothing about PID
        """
        return _hlt2ChargedProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                       prefix = self.getProp("Prefix"),
                                       suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"))

###################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return _hlt2MuonProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                              DataType = self.getProp("DataType"),
                              prefix = self.getProp("Prefix"), 
			      suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling") )
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return _hlt2NeutralProtos(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
				 suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling") )
###################################################################################
    #
    # Tracking sequence
    #
    def hlt2Tracking(self, getOutput):
        """
        Tracks
        """
        return _hlt2Tracking(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
				 suffix = self.getProp("Suffix"),
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling"),
				 returnOutputLocation = getOutput)
###################################################################################
    #
    # Velo tracking for the PV making sequence
    #
    def hlt2VeloTracking(self,getOutput):
        """ 
        Velo tracks
        """
        return _hlt2VeloTracking(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
                                 suffix = self.getProp("Suffix"),
                                 returnOutputLocation = getOutput)
###################################################################################
    #
    # Track fitting
    #
    def hlt2StagedFastFit(self):
        """
	The staged fast fit
	"""
	return _hlt2StagedFastFit(Hlt2Tracks = self.getProp("Hlt2Tracks"),
                                 prefix = self.getProp("Prefix"),
                                 suffix = "Fitted",
                                 doSeeding = self.getProp("DoSeeding"),
                                 doCloneKilling = self.getProp("DoCloneKilling")) 
#########################################################################################
#########################################################################################
#
# Actual implementation of the functions
#
#########################################################################################
def _trackLocation(prefix,type,suffix,tracks):
    if (suffix == "") : return prefix + "/" + type + "/" + tracks
    else : return prefix + "/" + type + "/" + suffix + "/" + tracks

def _protosLocation(prefix,suffix):
    if (suffix == "") : return prefix+"/ProtoP/Charged" 
    else : return prefix+"/ProtoP/"+suffix+"/" + "Charged"
#########################################################################################
#
# Electron Protos
#
def _hlt2ChargedCaloProtos(Hlt2Tracks = "Long",
                          prefix="Hlt", suffix = "", doSeeding = False
                , doCloneKilling = False ):
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
    ecal = ChargedProtoParticleAddEcalInfo(prefix+suffix+"ChargedProtoPAddEcal")
    ecal.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    brem = ChargedProtoParticleAddBremInfo(prefix+suffix+"ChargedProtoPAddBrem")
    brem.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    prs  = ChargedProtoParticleAddPrsInfo(prefix+suffix+"ChargedProtoPAddPrs")
    prs.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    spd  = ChargedProtoParticleAddSpdInfo(prefix+suffix+"ChargedProtoPAddSpd")
    spd.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(prefix+suffix+"ChargedProtoPCombDLLsAfterCalo")
    combine.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    # Fill the sequence
    # we need the calo reconstruction, this does not yet work for
    # fitted tracks...
    caloreco = _hlt2Calo(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix, doSeeding, doCloneKilling)
    from HltLine.HltLine import bindMembers
    return bindMembers( None, [ caloreco, chargedProtos, ecal,brem,prs,spd,combine ])
    
#########################################################################################
#
# Muon Protos
#
def _hlt2MuonProtos(Hlt2Tracks = "Long",
                   DataType = "2009", 
                   prefix="Hlt",
         	   suffix = "", doSeeding = False
                , doCloneKilling = False ):
    """
    Charged muon protoparticles
    Requires chargedProtos and muon ID
    """
    from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                ChargedProtoCombineDLLsAlg )
    chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    muon = ChargedProtoParticleAddMuonInfo(prefix+suffix+"ChargedProtoPAddMuon")
    muon.ProtoParticleLocation =  _protosLocation(prefix,suffix)
    #Enforce naming conventions
    muon.InputMuonPIDLocation = _trackLocation(prefix,"Muon",suffix,"MuonPID")
    # Fill the Combined DLL information in the charged protoparticles
    combine = ChargedProtoCombineDLLsAlg(prefix+suffix+"ChargedProtoPCombDLLsAfterMuon")
    combine.ProtoParticleLocation =  _protosLocation(prefix,suffix)

    muonID = _hlt2Muon(Hlt2Tracks, DataType, prefix, suffix, doSeeding,doCloneKilling)
    from HltLine.HltLine import bindMembers
    return bindMembers( None, [ muonID, chargedProtos, muon, combine ] )  

#########################################################################################
#
# Charged ProtoParticles
# Does not necessarily use RICH
#
def _hlt2ChargedHadronProtos(Hlt2Tracks = "Long",
                            prefix="Hlt", 
			    suffix = "",
			    doSeeding = False,
                            doCloneKilling = False,
                            UseRICH=False, 
			    UseCALO = False):
    """
    Charged hadron protoparticles = pion, kaon, proton
    If RICH is on, then requires rich, calo and does dll combination
    This is off by default
    """
    from Configurables import (	ChargedProtoParticleAddRichInfo,
				ChargedProtoParticleAddMuonInfo,
                                ChargedProtoParticleAddHcalInfo,
                                ChargedProtoCombineDLLsAlg)
    from HltLine.HltLine import bindMembers
    from RichRecSys.Configuration import (RichRecSysConf,
					  RichTrackCreatorConfig,
                               		  RichPixelCreatorConfig
					  )

    if ( UseRICH and not UseCALO):
        
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
        
	#setup the protos
	charged = _hlt2ChargedProtos(Hlt2Tracks,prefix,suffix,doSeeding,doCloneKilling)
	# Create RICH PID using the Fitted tracks
        RichTrackCreatorConfig().InputTracksLocation = _trackLocation(prefix,"Track",suffix,Hlt2Tracks)
        # The Rich info 
        rich = ChargedProtoParticleAddRichInfo(prefix+suffix+"ChargedProtoPAddRich")
        rich.ProtoParticleLocation =  _protosLocation(prefix,suffix) 
	# The combined DLL
        combine = ChargedProtoCombineDLLsAlg(prefix+suffix+"ChargedProtoPCombDLLsAfterRich")
        combine.ProtoParticleLocation =  _protosLocation(prefix,suffix)

        return bindMembers( None , [ charged, richseq, rich, combine ] )
    else :
    	#delegate everything to the charged proto maker
	#because the CALO does not work for now! This needs to be fixed!!!
	chargedProtos = _hlt2ChargedProtos(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
        return bindMembers( None , [ chargedProtos ] )

    # The HCAL info
    #hcal = ChargedProtoParticleAddHcalInfo(prefix+suffix+"ChargedProtoPAddHcal")
    #hcal.ProtoParticleLocation = protoslocation
    # The calo DLL
    #caloID = _hlt2Calo(Hlt2Tracks, prefix, suffix)
    
#########################################################################################
#
# Charged ProtoParticles
#
def _hlt2ChargedProtos(Hlt2Tracks = "Long",
                      prefix="Hlt", 
                      suffix = "",
                      doSeeding = False,
                      doCloneKilling = False ):
    """
    Charged protoparticles
    
    I don't think a prefix different to Hlt works.
    For the moment we have some clumsy naming to start the
    protos with Hlt/...
    Will fix if Hlt2 works as a prefix
    Requires tracks, fitted if necessary
    """
    from Configurables import ( ChargedProtoParticleMaker,
                                DelegatingTrackSelector  )
    
    charged = ChargedProtoParticleMaker(prefix+suffix+'ChargedProtoPAlg')
    charged.addTool( DelegatingTrackSelector, name="TrackSelector" )
    charged.TrackSelector.TrackTypes = [ "Long" ]  # only long so far
   
    #Need to allow for fitted tracks
    charged.InputTrackLocation = _trackLocation(prefix,"Track",suffix,Hlt2Tracks)
    charged.OutputProtoParticleLocation =  _protosLocation(prefix,suffix)

    from HltLine.HltLine import bindMembers
    #The tracking as it stands is completely ignoring prefix and suffix anyway
    #because it is all hardcoded...
    if (suffix.find("Fitted") > -1) : tracks = _hlt2StagedFastFit(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    else : tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    return bindMembers ( None, [ tracks , charged ] )

#########################################################################################
#
# Neutral ProtoParticles
#
def _hlt2NeutralProtos(Hlt2Tracks = "Long",
                      prefix="Hlt", 
                      suffix = "",
                            doSeeding = False,
                            doCloneKilling = False ):
    """
    Neutral protoparticles 
    Requires caloID
    """
    from Configurables import NeutralProtoPAlg
    HltNeutralProtoPAlg = NeutralProtoPAlg(prefix+suffix+'NeutralProtoPAlg')
    # Overwrite some default offline settings with HLT special settings (
    # taken from CaloReco.opts)
    HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
    from Configurables import CaloPhotonEstimatorTool
    ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
    ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
    importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )
    
    caloID = _hlt2Calo(Hlt2Tracks, prefix,suffix,doSeeding,doCloneKilling)
    
    from HltLine.HltLine import bindMembers
    return bindMembers ( None, [ caloID, HltNeutralProtoPAlg ]) 
    
#########################################################################################
#
# MuonID
#
def _hlt2Muon(Hlt2Tracks = "Long"
           , DataType = '2009'
           , prefix="Hlt" 
           , suffix = "",
                            doSeeding = False,
                            doCloneKilling = False):
    """
    Muon ID options 
    Requires tracks
    """
    from MuonID import ConfiguredMuonIDs
    from Configurables import MuonRec, MuonIDAlg
    cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=DataType)
    HltMuonIDAlg = cm.configureMuonIDAlg(prefix+suffix+"MuonIDAlg")
    #Enforce naming conventions
    HltMuonIDAlg.TrackLocation        = _trackLocation(prefix,"Track",suffix,Hlt2Tracks) 
    HltMuonIDAlg.MuonIDLocation       = _trackLocation(prefix,"Muon",suffix,"MuonPID")    # output
    HltMuonIDAlg.MuonTrackLocation    = _trackLocation(prefix,"Track",suffix,"Muon")
    HltMuonIDAlg.MuonTrackLocationAll = _trackLocation(prefix,"Track",suffix,"AllMuon")
    # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
    #       a factor 3-4
    HltMuonIDAlg.FindQuality = False
    tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    
    from HltLine.HltLine import bindMembers
    return bindMembers ( None, [ tracks, MuonRec(), HltMuonIDAlg ] )
#########################################################################################
#
# Calo ID
#
def _hlt2Calo(Hlt2Tracks = "Long"
           , prefix="Hlt"
           , suffix = "",
                            doSeeding = False,
                            doCloneKilling = False):
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
    from HltLine.HltLine import bindMembers
    tracks = _hlt2Tracking(Hlt2Tracks, prefix,suffix,doSeeding,doCloneKilling)
    return bindMembers( None, [ tracks, Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq   ] )
#########################################################################################
#
# Staged fast fit
#
def _hlt2StagedFastFit(Hlt2Tracks = "Long"
                , prefix="Hlt"
                , suffix = "Fitted",
                            doSeeding = False,
                            doCloneKilling = False):
    """
    The staged fast-fit. Currently set to
    be bidirectional and with smoothing to allow PID information
    to be used; special cases need to be justified separately
    """
    from Configurables import TrackEventFitter, TrackMasterFitter
    Hlt2StagedFastFitSeq = GaudiSequencer( "Hlt2StagedFastFitSeq" )

    Hlt2StagedFastFit = TrackEventFitter('Hlt2StagedFastFit')
    Hlt2StagedFastFitSeq.Members = [ Hlt2StagedFastFit ]

    Hlt2StagedFastFit.TracksInContainer  = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling,True) 
    # Need a rule for the fitted tracks: the default
    # Long outputs to Hlt/Track/Fitted/Long 
    # But we need to accomodate different kinds of Fitted tracks... 
    Hlt2StagedFastFit.TracksOutContainer =  _trackLocation(prefix,"Track",suffix,Hlt2Tracks)  

    Hlt2StagedFastFit.addTool(TrackMasterFitter, name = 'Fitter')
    from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
    fitter = ConfiguredFastFitter( getattr(Hlt2StagedFastFit,'Fitter'))

    fitter.NodeFitter.BiDirectionalFit = True
    fitter.NodeFitter.Smooth = True
    fitter.AddDefaultReferenceNodes = True    # says Wouter
    
    from HltLine.HltLine import bindMembers
    #Make the original tracks in case this has not been run already
    tracks = _hlt2Tracking(Hlt2Tracks, prefix, suffix,doSeeding,doCloneKilling)
    return bindMembers( None, [tracks, Hlt2StagedFastFitSeq] )

#########################################################################################
#
# Track reconstruction
#
def _hlt2Tracking(Hlt2Tracks = "Long"
                , prefix="Hlt"
 		, suffix = ""
                , doSeeding = False
                , doCloneKilling = False, returnOutputLocation = False):
    """
    Track reconstruction
    @todo allow to set tracks in reco sequence
    """

    # TODO: this whole thing only works with an Hlt2Tracks
    #       value of 'Long' but in principle we should be more general
    #       and allow different types of tracks to be made
    # Where will we output the long tracks into?
    hlt2LongTrackOutput = _trackLocation(prefix,"Track","",Hlt2Tracks)
    if returnOutputLocation : return hlt2LongTrackOutput

    from HltLine.HltLine import bindMembers
    #From HltReco we just get the shared stuff between Hlt1 and Hlt2
    from HltLine.HltReco import MinimalRZVelo   
    #Now let us make the reconstruction sequence here
    #The VELO parts
    hlt2Velotracking = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix)
    # The tracking decoding
    from HltLine.HltDecodeRaw import DecodeTT, DecodeIT
    hlt2TrackerDecoding = DecodeTT.members() + DecodeIT.members()
    # The forward tracking
    hlt2ForwardTracking = _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix)   
    # The seeding
    hlt2SeedTracking = _hlt2SeedTracking(Hlt2Tracks,prefix,suffix)
    # Now the copy sequence, which basically merges the forward and
    # match tracks into the long track container
    # What is merged depends on whether we did the seeding or now
    if doSeeding : hlt2TracksToMergeIntoLong = [ _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix,True), _hlt2SeedTracking(Hlt2Tracks,prefix,suffix,True) ]
    else : hlt2TracksToMergeIntoLong = [ _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix,True)]
    from Configurables import CreateFastTrackCollection
    #### CreateFastTrackCollection
    # this can only be done ONCE per long track output
    recoCopy = CreateFastTrackCollection(prefix+'TrackCopy'+Hlt2Tracks
                , InputLocations = hlt2TracksToMergeIntoLong
                , OutputLocation = hlt2LongTrackOutput
                , SlowContainer = True)
    # The clone killing
    from Configurables import TrackEventCloneKiller, TrackCloneFinder
    cloneKiller = TrackEventCloneKiller(prefix+'FastCloneKiller'+Hlt2Tracks
                , TracksInContainers = [hlt2LongTrackOutput]
                , SkipSameContainerTracks = False
                , CopyTracks = False)
    cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
    cloneKiller.CloneFinderTool.RestrictedSearch = True
    # Finally make the sequence
    # This part gets done in either case
    trackRecoSequence = GaudiSequencer( prefix+'TrackRecoSequence'+Hlt2Tracks
                                  ,  Members = MinimalRZVelo.members() + hlt2Velotracking + hlt2TrackerDecoding + hlt2ForwardTracking)  
    # Do the seeding if required
    if doSeeding : trackRecoSequence.Members += hlt2SeedTracking
    # The copying is always done
    trackRecoSequence.Members += [recoCopy]
    # Do the clone killing if required 
    if doCloneKilling : trackRecoSequence.Members += [cloneKiller]  
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
    if (suffix.find("Fitted") > -1) :
	return bindMembers( None, [ HltRecoSequence ] )
    else :
    	from Configurables import HltInsertTrackErrParam
   	HltInsertTrackErrParam = HltInsertTrackErrParam(prefix+"InsertTrackErrParam"+Hlt2Tracks)
   	HltInsertTrackErrParam.InputLocation = hlt2LongTrackOutput
   	#
  	# Reco sequence
 	#
 	return bindMembers( None, [ HltRecoSequence, HltInsertTrackErrParam ]) 
#########################################################################################
#
# Hlt2 Velo Reconstruction
#
def _hlt2VeloTracking(Hlt2Tracks = "Long"
                , prefix="Hlt"
                , suffix = ""
                , returnOutputLocation = False):
    """
    Velo track reconstruction for Hlt2
    The returnOutputLocation property is used to let subsequent algorithms know
    where these reco steps outputed to
    """
    from Configurables import Tf__PatVeloSpaceTracking, Tf__PatVeloGeneralTracking

    veloTracksOutputLocation = _trackLocation(prefix,"Track","","Velo")
    if returnOutputLocation : return veloTracksOutputLocation
   
    # TODO: remove the hardcoded input location of the RZVelo tracks here 
    recoVelo = Tf__PatVeloSpaceTracking(prefix+'RecoVelo'
                                   , InputTracksName = _trackLocation("Hlt","Track","","RZVelo") 
                                   , OutputTracksName = veloTracksOutputLocation )

    recoVeloGeneral = Tf__PatVeloGeneralTracking(prefix+'RecoVeloGeneral'
                                   , OutputTracksLocation = veloTracksOutputLocation )
    return [recoVelo,recoVeloGeneral]
#########################################################################################
#
# Hlt2 Forward Reconstruction
#
def _hlt2ForwardTracking(Hlt2Tracks = "Long"
                , prefix="Hlt"
                , suffix = ""
                , returnOutputLocation = False):
    """
    Forward track reconstruction for Hlt2
    The returnOutputLocation property is used to let subsequent algorithms know
    where these reco steps outputed to
    """
    from Configurables import PatForward, PatForwardTool
    forwardTrackOutputLocation = _trackLocation(prefix,"Track","","Forward")
    if returnOutputLocation : return forwardTrackOutputLocation

    recoForward = PatForward( prefix+'RecoForward'
                        , InputTracksName = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix,True) 
                        , OutputTracksName = forwardTrackOutputLocation )
    PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
    recoForward.addTool(PatForwardTool, name='PatForwardTool')
    recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
    return [recoForward]
#########################################################################################
#
# Hlt2 Seeded Forward Reconstruction
#
def _hlt2SeedTracking(Hlt2Tracks = "Long"
                , prefix="Hlt"
                , suffix = ""
                , returnOutputLocation = False):
    """
    Forward track reconstruction for Hlt2 using seeding
    The returnOutputLocation property is used to let subsequent algorithms know
    where these reco steps outputed to
    """
    from Configurables import PatSeeding, PatSeedingTool, PatMatch

    matchTrackOutputLocation = _trackLocation(prefix,"Track","","Match")
    if returnOutputLocation : return matchTrackOutputLocation

    #### Seeding 
    recoSeeding = PatSeeding(prefix+'Seeding', OutputTracksName = _trackLocation(prefix,"Track","","Seeding"))
    recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
    recoSeeding.PatSeedingTool.UseForward = True
    recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
    recoSeeding.PatSeedingTool.InputTracksName = _hlt2ForwardTracking(Hlt2Tracks,prefix,suffix,True)

    #### Matching
    recoMatch = PatMatch(prefix+'Match'
                     , VeloInput = _hlt2VeloTracking(Hlt2Tracks,prefix,suffix,True)
                     , SeedInput = recoSeeding.OutputTracksName
                             , MatchOutput = matchTrackOutputLocation)

    return [recoSeeding, recoMatch]
