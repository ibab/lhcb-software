### @file
#
#  Hlt2 Tracking.
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.5 $"
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

from Hlt2Lines.Hlt2B2DXLines            import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines         import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2PhiXLines          import Hlt2B2PhiXLinesConf
from Hlt2Lines.Hlt2InclusiveDiMuonLines import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines   import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines    import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines     import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2B2XGammaLines        import Hlt2B2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines            import Hlt2B2HHLinesConf
from Hlt2Lines.Hlt2B2LLXLines           import Hlt2B2LLXLinesConf
from Hlt2Lines.Hlt2CharmLines           import Hlt2CharmLinesConf
from Hlt2Lines.Hlt2DisplVerticesLines   import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2CommissioningLines   import Hlt2CommissioningLinesConf
from Hlt2Lines.Hlt2ExpressLines         import Hlt2ExpressLinesConf
from Hlt2Lines.Hlt2diphotonDiMuonLines  import Hlt2diphotonDiMuonLinesConf

###################################################################################
#
# Hlt2 Tracking
#
class Hlt2Tracking(LHCbConfigurableUser):
    __used_configurables__ = [ 	"HltCaloRecoConf", 
				"HltCaloPIDsConf", 
				"RichRecSysConf", 
				"Hlt2TopologicalLinesConf",
				"Hlt2B2DXLinesConf",
                                "Hlt2CharmLinesConf",
                                "Hlt2InclusiveDiMuonLinesConf",
                                "Hlt2InclusiveMuonLinesConf",
                                "Hlt2InclusivePhiLinesConf",
                                "Hlt2B2JpsiXLinesConf",
                                "Hlt2B2PhiXLinesConf",
                                "Hlt2B2XGammaLinesConf",
                                "Hlt2B2HHLinesConf",
                                "Hlt2DisplVerticesLinesConf",
                                "Hlt2CommissioningLinesConf",
                                "Hlt2ExpressLinesConf",
                                "Hlt2diphotonDiMuonLinesConf" 
			     ]
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
	# TODO : make these variables, not slots 	
	, "__hlt2ChargedCaloProtosSeq__"	: 0
	, "__hlt2ChargedHadronProtosSeq__" 	: 0
        , "__hlt2ChargedProtosSeq__"		: 0
	, "__hlt2MuonProtosSeq__"		: 0
	, "__hlt2NeutralProtosSeq__"		: 0
	, "__hlt2VeloTrackingSeq__"		: 0
	, "__hlt2PrepareTracksSeq__"		: 0
        }
###################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return self.getProp("__hlt2ChargedCaloProtosSeq__")
###################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return self.getProp("__hlt2ChargedHadronProtosSeq__")	
###################################################################################
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles which know nothing about PID
        """
        return self.getProp("__hlt2ChargedProtosSeq__")
###################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return self.getProp("__hlt2MuonProtosSeq__")
###################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return self.getProp("__hlt2NeutralProtosSeq__")
###################################################################################
    #
    # Velo tracking for the PV making sequence
    #
    def hlt2VeloTracking(self):
        """ 
        Velo tracks
        """
        return self.getProp("__hlt2VeloTrackingSeq__")
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
	return self.getProp("__hlt2PrepareTracksSeq__")
###################################################################################
    #
    # Apply the configuration
    # 
    # Make what will later be used...
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
	self.setProp("__hlt2ChargedCaloProtosSeq__",	 self.__hlt2ChargedCaloProtos()		)
        self.setProp("__hlt2ChargedHadronProtosSeq__",	 self.__hlt2ChargedHadronProtos()	)
        self.setProp("__hlt2ChargedProtosSeq__",	 self.__hlt2ChargedProtos()		)
        self.setProp("__hlt2MuonProtosSeq__",		 self.__hlt2MuonProtos()		)
        self.setProp("__hlt2NeutralProtosSeq__",	 self.__hlt2NeutralProtos()		)
        self.setProp("__hlt2VeloTrackingSeq__",		 self.__hlt2VeloTracking()		)
        self.setProp("__hlt2PrepareTracksSeq__",	 self.__hlt2StagedFastFit()		)
#########################################################################################
#########################################################################################
#
# Actual implementation of the functions
#
#########################################################################################
    def __baseAlgosAndToolsPrefix(self) :
        return self.getProp("Prefix") + _shortTrackLocation(self)
    #
    def __usedAlgosAndToolsPrefix(self) :
        return self.getProp("Prefix") + self.getProp("FastFitType") + _shortTrackLocation(self)
    # In case something went wrong when specifying the options, warn me before you die
    def __printWarningBeforeDeath(self) :
        log.fatal( '###############################################################')
        log.fatal( '## WARNING You specified an unknown track type %s for the Hlt2 Reconstruction##' % self.getProp("Hlt2Tracks") )
        log.fatal( '## WARNING I will now die, you need to make Hlt2Tracking aware of this track type!##')
        log.fatal( '###############################################################')
    #########################################################################################
    #
    # Electron Protos
    #
    def __hlt2ChargedCaloProtos(self) :
        """
        Charged Calo protoparticles = electrons
        Requires chargedProtos
        """
        from Configurables import ( ChargedProtoParticleAddEcalInfo,
                                    ChargedProtoParticleAddBremInfo,
                                    ChargedProtoParticleAddPrsInfo,
                                    ChargedProtoParticleAddSpdInfo,
                                    ChargedProtoCombineDLLsAlg )
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        #
        # First of all check that I have been called with a sensible set of options
        #
        outputOfHlt2Tracking = _shortTrackLocation(self)
        if (outputOfHlt2Tracking == 'Unknown') :
            self.__printWarningBeforeDeath()
            return [] 
        #
        chargedProtos = self.__hlt2ChargedProtos()
        chargedProtosOutputLocation = chargedProtos.outputSelection()
        # The tools
        ecal = ChargedProtoParticleAddEcalInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddEcal")
        brem = ChargedProtoParticleAddBremInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddBrem")
        prs  = ChargedProtoParticleAddPrsInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddPrs")
        spd  = ChargedProtoParticleAddSpdInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddSpd")
        # The output locations
        ecal.ProtoParticleLocation =  chargedProtosOutputLocation
        brem.ProtoParticleLocation =  chargedProtosOutputLocation
        prs.ProtoParticleLocation =  chargedProtosOutputLocation 
        spd.ProtoParticleLocation =  chargedProtosOutputLocation
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPCombDLLsAfterCalo")
        combine.ProtoParticleLocation =  chargedProtosOutputLocation
        # Fill the sequence
        # we need the calo reconstruction, this does not yet work for
        # fitted tracks...
        caloreco = self.__hlt2Calo()
        from HltLine import bindMembers
        return bindMembers( None, [ caloreco, chargedProtos, ecal,brem,prs,spd,combine ]).setOutputSelection(chargedProtosOutputLocation) 
        
    #########################################################################################
    #
    # Muon Protos
    #
    def __hlt2MuonProtos(self):
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
            self.__printWarningBeforeDeath()
            return []
        #
     
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        chargedProtos = self.__hlt2ChargedProtos()
        chargedProtosOutputLocation = chargedProtos.outputSelection()
        #
        
        muon = ChargedProtoParticleAddMuonInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddMuon")
        muon.ProtoParticleLocation =  chargedProtosOutputLocation
        #Enforce naming conventions
        muonID = self.__hlt2Muon()
        muon.InputMuonPIDLocation = muonID.outputSelection()
        # Fill the Combined DLL information in the charged protoparticles
        combine = ChargedProtoCombineDLLsAlg(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPCombDLLsAfterMuon")
        combine.ProtoParticleLocation =  chargedProtosOutputLocation
    
        from HltLine import bindMembers
        return bindMembers( None, [ muonID, chargedProtos, muon, combine ] ).setOutputSelection(chargedProtosOutputLocation)  
    
    #########################################################################################
    #
    # Charged ProtoParticles
    # Does not necessarily use RICH
    #
    def __hlt2ChargedHadronProtos(self):
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
            self.__printWarningBeforeDeath()
            return []
        #
    
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        chargedProtos = self.__hlt2ChargedProtos()
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
    	    RichTrackCreatorConfig().InputTracksLocation = self.__hlt2StagedFastFit().outputSelection()
            # The Rich info 
            rich = ChargedProtoParticleAddRichInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddRich")
            rich.ProtoParticleLocation =  chargedProtosOutputLocation 
    	    # The combined DLL
            combine = ChargedProtoCombineDLLsAlg(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPCombDLLsAfterRich")
            combine.ProtoParticleLocation =  chargedProtosOutputLocation
    
            return bindMembers( None , [ chargedProtos, richseq, rich, combine ] ).setOutputSelection(chargedProtosOutputLocation)
        else :
            #delegate everything to the charged proto maker
    	    #because the CALO does not work for now! This needs to be fixed!!!
            return bindMembers( None , [ chargedProtos ] ).setOutputSelection(chargedProtosOutputLocation)
    
        # The HCAL info
        #hcal = ChargedProtoParticleAddHcalInfo(self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddHcal")
        #hcal.ProtoParticleLocation = protoslocation
        # The calo DLL
        #caloID = __hlt2Calo(Hlt2Tracks, prefix, fastFitType)
        
    #########################################################################################
    #
    # Charged ProtoParticles
    #
    def __hlt2ChargedProtos(self):
        """
        Charged protoparticles
        
        Requires tracks, fitted if necessary
        """

        # First of all check that I have been called with a sensible set of options
        #
        outputOfHlt2Tracking = _shortTrackLocation(self)
        if (outputOfHlt2Tracking == 'Unknown') :
            self.__printWarningBeforeDeath()
            return []
        #
    
        from Configurables import ( ChargedProtoParticleMaker,
                                    DelegatingTrackSelector  )
        
        chargedProtosOutputLocation = _protosLocation(self,Hlt2ChargedProtoParticleSuffix)
    
        charged = ChargedProtoParticleMaker(self.__usedAlgosAndToolsPrefix()+'ChargedProtoPAlg')
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
        tracks = self.__hlt2StagedFastFit()
        charged.InputTrackLocation = tracks.outputSelection()
    
        charged.OutputProtoParticleLocation =  chargedProtosOutputLocation
    
        from HltLine import bindMembers
        return bindMembers ( None, [ tracks , charged ] ).setOutputSelection(chargedProtosOutputLocation)
    
    #########################################################################################
    #
    # Neutral ProtoParticles
    #
    def __hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        Requires caloID
        """
    
        # First of all check that I have been called with a sensible set of options
        #
        outputOfHlt2Tracking = _shortTrackLocation(self)
        if (outputOfHlt2Tracking == 'Unknown') :
            self.__printWarningBeforeDeath()
            return []
        #
    
        from Configurables import NeutralProtoPAlg
        HltNeutralProtoPAlg = NeutralProtoPAlg(self.__usedAlgosAndToolsPrefix()+'NeutralProtoPAlg')
        # Overwrite some default offline settings with HLT special settings (
        # taken from CaloReco.opts)
        HltNeutralProtoPAlg.PhotonIDName = "HltPhotonPID"
        from Configurables import CaloPhotonEstimatorTool
        ToolSvc().addTool(CaloPhotonEstimatorTool, name="HltPhotonPID")
        ToolSvc().HltPhotonPID.TableLocation = "Hlt/Calo/ClusterMatch"
        importOptions( "$CALOPIDSROOT/options/HltPhotonPDF.opts" )
        
        caloID = self.__hlt2Calo()
        
        from HltLine import bindMembers
        return bindMembers ( None, [ caloID, HltNeutralProtoPAlg ]) 
        
    #########################################################################################
    #
    # MuonID
    #
    def __hlt2Muon(self) :
        """
        Muon ID options 
        Requires tracks
        """
    
        # First of all check that I have been called with a sensible set of options
        #
        outputOfHlt2Tracking = _shortTrackLocation(self)
        if (outputOfHlt2Tracking == 'Unknown') :
            self.__printWarningBeforeDeath()
            return []
        #
    
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg
        cm = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
        HltMuonIDAlg = cm.configureMuonIDAlg(self.__usedAlgosAndToolsPrefix()+"MuonIDAlg")
        #The tracks to use
        tracks = self.__hlt2StagedFastFit()
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
    def __hlt2Calo(self) :
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
        tracks = self.__hlt2StagedFastFit()
        return bindMembers( None, [ tracks, Hlt2CaloRecoSeq, Hlt2CaloPIDsSeq   ] )
    #########################################################################################
    #
    # Staged fast fit
    #
    def __hlt2StagedFastFit(self) :
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
            self.__printWarningBeforeDeath()
            return []
        #Make the original tracks in case this has not been run already
        tracks = self.__hlt2Tracking()
        #And my output location is?
        hlt2StagedFastFitOutputLocation = _trackLocation(self)
        # Second check: have I actually been asked to fit the tracks?
        # If not, just return the unfitted tracks copied into the container specified by
        # the fast-fit type field
        if (not self.getProp("DoFastFit")) :
    	    from Configurables import CreateFastTrackCollection
            #### CreateFastTrackCollection
            # this can only be done ONCE per long track output
            recoCopy = CreateFastTrackCollection( self.__usedAlgosAndToolsPrefix()+'TrackCopy'
                    , InputLocations = [tracks.outputSelection()]
                    , OutputLocation = hlt2StagedFastFitOutputLocation
                    , SlowContainer = True)
    	    return bindMembers(None, [tracks, recoCopy]).setOutputSelection(hlt2StagedFastFitOutputLocation)
    
        from Configurables import TrackEventFitter, TrackMasterFitter
        Hlt2StagedFastFitSeq = GaudiSequencer( self.__usedAlgosAndToolsPrefix()+"StagedFastFitSeq" )
        
        Hlt2StagedFastFit = TrackEventFitter(self.__usedAlgosAndToolsPrefix()+'StagedFastFit')
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
    def __hlt2Tracking(self) :
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
            self.__printWarningBeforeDeath()
    	    return []  
        hlt2TrackingOutput = _baseTrackLocation(self.getProp("Prefix"),_shortTrackLocation(self))
        
	from HltLine import bindMembers
        
	# The clone killing
        from Configurables import TrackEventCloneKiller, TrackCloneFinder
        cloneKiller = TrackEventCloneKiller(self.__baseAlgosAndToolsPrefix()+'FastCloneKiller'
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
        trackRecoSequence = GaudiSequencer( self.__baseAlgosAndToolsPrefix()+'TrackRecoSequence')
        if (self.getProp("Hlt2Tracks") == Hlt2LongTracksName) :
    	    trackRecoSequence.Members =     	self.__hlt2ForwardTracking().members()
	    # Do the seeding +matching if required
    	    if doSeeding :
        	# The copy sequence, which merges the forward and
        	# match tracks into the long track container
        	# This only runs if the seeding is run and we ask for Long tracks
        	hlt2TracksToMergeIntoLong = [ self.__hlt2ForwardTracking().outputSelection(), self.__hlt2MatchTracking().outputSelection() ]
        	from Configurables import CreateFastTrackCollection
        	#### CreateFastTrackCollection
        	recoCopy = CreateFastTrackCollection(   self.__baseAlgosAndToolsPrefix()+'TrackCopy',
                                                InputLocations = hlt2TracksToMergeIntoLong,
                                                OutputLocation = hlt2TrackingOutput,
                                                SlowContainer = True) 
    		trackRecoSequence.Members += 	self.__hlt2MatchTracking().members()
    		trackRecoSequence.Members += 	[recoCopy]
	    # Do the clone killing if required 
            if self.getProp("DoCloneKilling") : trackRecoSequence.Members += [cloneKiller]
	elif (self.getProp("Hlt2Tracks") == Hlt2ForwardTracksName) :
	    trackRecoSequence.Members = 	self.__hlt2ForwardTracking().members()
            # Do the clone killing if required 
            if self.getProp("DoCloneKilling") : trackRecoSequence.Members += [cloneKiller]
	elif (self.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName ) :
    	    trackRecoSequence.Members = 	self.__hlt2DownstreamTracking().members()
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
       	    HltInsertTrackErrParam = HltInsertTrackErrParam(self.__baseAlgosAndToolsPrefix()+"InsertTrackErrParam")
       	    HltInsertTrackErrParam.InputLocation = hlt2TrackingOutput
     	    return bindMembers( None, [ HltRecoSequence, HltInsertTrackErrParam ]).setOutputSelection(hlt2TrackingOutput) 
    #########################################################################################
    #
    # Hlt2 Velo Reconstruction
    #
    def __hlt2VeloTracking(self):
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
    def __hlt2ForwardTracking(self) :
        """
        Forward track reconstruction for Hlt2
        """
        from Configurables import PatForward, PatForwardTool
    	from HltLine import bindMembers
    
        forwardTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2ForwardTracksName) 
    
        recoForward = PatForward( self.getProp("Prefix")+'RecoForward'
                            , InputTracksName = self.__hlt2VeloTracking().outputSelection() 
                            , OutputTracksName = forwardTrackOutputLocation )
        PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
        recoForward.addTool(PatForwardTool, name='PatForwardTool')
        recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
        return bindMembers(None, self.__hlt2VeloTracking().members() + self.__hlt2TrackerDecoding().members() + [recoForward]).setOutputSelection(forwardTrackOutputLocation)
    #########################################################################################
    #
    # Hlt2 Seeding for forward,downstream reconstruction
    #
    def __hlt2SeedTracking(self):
        """
        Seeding in the trackers for later use in Match Forward reconstruction
        or in downstream tracking.
        """
        from Configurables import PatSeeding, PatSeedingTool
    	from HltLine import bindMembers
 
        seedTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2SeedingTracksName)
    
        #### Seeding 
        recoSeeding = PatSeeding(self.getProp("Prefix")+'Seeding', OutputTracksName = seedTrackOutputLocation)
        recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
        recoSeeding.PatSeedingTool.UseForward = True
        recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
        recoSeeding.PatSeedingTool.InputTracksName = self.__hlt2ForwardTracking().outputSelection()
    
        return bindMembers(None, self.__hlt2TrackerDecoding().members() + [recoSeeding]).setOutputSelection(seedTrackOutputLocation)
    
    #########################################################################################
    #
    # Hlt2 Match Forward Reconstruction
    #
    def __hlt2MatchTracking(self):
        """
        Forward track reconstruction for Hlt2 using seeding
        """
        from Configurables import PatMatch
    	from HltLine import bindMembers
    
        matchTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2MatchTracksName)
    
        #### Matching
        recoMatch = PatMatch(self.getProp("Prefix")+'Match'
                         , VeloInput = self.__hlt2VeloTracking().outputSelection()
                         , SeedInput = self.__hlt2SeedTracking().outputSelection()
                                 , MatchOutput = matchTrackOutputLocation)
    
        return bindMembers(None, self.__hlt2VeloTracking().members() + self.__hlt2SeedTracking().members() + [recoMatch]).setOutputSelection(matchTrackOutputLocation)
    
    #########################################################################################
    #
    # Hlt2 Downstream Forward Reconstruction
    #
    def __hlt2DownstreamTracking(self):
        """
        Downstream track reconstruction for Hlt2 using seeding
        """
        from Configurables import PatDownstream
    	from HltLine import bindMembers
    
        downstreamTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2DownstreamTracksName)
    
        ### Downstream tracking
        PatDownstream = PatDownstream(self.getProp("Prefix")+'PatDownstream')
        PatDownstream.InputLocation = self.__hlt2SeedTracking().outputSelection()
        PatDownstream.OutputLocation = downstreamTrackOutputLocation
        #PatDownstream.UseForward = True
        #PatDownstream.SeedFilter = True
        PatDownstream.RemoveUsed = True
        PatDownstream.RemoveAll = True
    
        return bindMembers(None, self.__hlt2SeedTracking().members() + [PatDownstream]).setOutputSelection(downstreamTrackOutputLocation)
    #########################################################################################
    #
    # Hlt2 tracker decoding
    #
    def __hlt2TrackerDecoding(self):
        """
        Decode the ST for Hlt2
        """
        from HltDecodeRaw import DecodeTT, DecodeIT
        from HltLine import bindMembers
	return bindMembers(None, DecodeTT.members() + DecodeIT.members())
