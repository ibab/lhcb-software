### @file
#
#  Hlt2 Tracking and PID
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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.3 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from HltTrackNames import Hlt2LongTracksName, HltSharedTracksPrefix
from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import Hlt2ForwardTracksName, Hlt2SeedingTracksName
from HltTrackNames import Hlt2MatchTracksName, Hlt2DownstreamTracksName   
from HltTrackNames import Hlt2VeloTracksName, HltSharedRZVeloTracksName
from HltTrackNames import Hlt2ChargedProtoParticleSuffix, Hlt2TrackingRecognizedTrackTypes
from HltTrackNames import Hlt2TrackingRecognizedFitTypes
from HltTrackNames import _baseTrackLocation, _baseProtoPLocation
from HltTrackNames import HltMuonTracksName, HltAllMuonTracksName
from HltTrackNames import HltMuonIDSuffix, HltRICHIDSuffix, HltCALOIDSuffix, HltSharedPIDPrefix 
from HltTrackNames import HltUnfittedTracksSuffix, HltUniDirectionalKalmanFitSuffix
from HltTrackNames import HltGlobalTrackLocation                
from HltTrackNames import Hlt2ChargedProtoParticleSuffix, Hlt2NeutralProtoParticleSuffix  

from Configurables import CaloProcessor, RichRecSysConf

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

#############################################################################################
#
# Hlt2 Tracking
#
class Hlt2Tracking(LHCbConfigurableUser):
#############################################################################################
#
# First of all the dependencies and the slots
#
#############################################################################################
    __used_configurables__ = 	[ 	(CaloProcessor	,	None), 
					(RichRecSysConf	,	None)
					# This next part defines all the Hlt2 Lines since they 
					# configured after the tracking. This means that each
					# Hlt2Lines configurable MUST be a singleton AND this
					# list must be EXACTLY the same as the one in 
					# $HLTCONFROOT/python/HltConf/Hlt2.py 
					, Hlt2TopologicalLinesConf
	                             	, Hlt2B2DXLinesConf
        	                     	, Hlt2CharmLinesConf
                	             	, Hlt2InclusiveDiMuonLinesConf
                        	     	, Hlt2InclusiveMuonLinesConf
	                             	, Hlt2InclusivePhiLinesConf
        	                     	, Hlt2B2JpsiXLinesConf
                	             	, Hlt2B2PhiXLinesConf
                        	     	, Hlt2B2XGammaLinesConf
	                             	, Hlt2B2HHLinesConf
        	                     	, Hlt2B2LLXLinesConf                         
                	             	, Hlt2DisplVerticesLinesConf
                        	     	, Hlt2CommissioningLinesConf
	                             	, Hlt2ExpressLinesConf
        	                     	, Hlt2diphotonDiMuonLinesConf
				]
    __slots__ = {
          "DataType"                   		: '2009' # datatype  2009, MC09, DC06...
        , "Hlt2Tracks"                 		: Hlt2ForwardTracksName
        , "Prefix"                     		: HltSharedTracksPrefix 
        , "FastFitType"                		: HltUnfittedTracksSuffix
        , "UseRICHForHadrons"          		: False 
        , "UseCALOForHadrons"	       		: False
        , "DoSeeding"                  		: False
        , "DoFastFit"				: False 
        , "DoCloneKilling"			: False
	, "MakeNeutrals"			: False
	, "RichHypos"				: ["pion","kaon"]
	, "RichRadiators"			: ["Rich1Gas","Rich2Gas"]
	# TODO : make these variables, not slots 	
	, "__hlt2ChargedCaloProtosSeq__"	: 0
	, "__hlt2ChargedHadronProtosSeq__" 	: 0
        , "__hlt2ChargedProtosSeq__"		: 0
	, "__hlt2MuonProtosSeq__"		: 0
	, "__hlt2NeutralProtosSeq__"		: 0
	, "__hlt2VeloTrackingSeq__"		: 0
	, "__hlt2ForwardTrackingSeq__"		: 0
	, "__hlt2SeedTrackingSeq__"		: 0
	, "__hlt2MatchTrackingSeq__"		: 0
	, "__hlt2DownstreamTrackingSeq__"	: 0
	, "__hlt2PrepareTracksSeq__"		: 0
	, "__hlt2MuonIDSeq__"			: 0
	, "__hlt2RICHIDSeq__"			: 0
	, "__hlt2CALOIDSeq__"			: 0
        }
#############################################################################################
#############################################################################################
#
# Externally visible methods to return the created bindMembers objects
#
#############################################################################################
#############################################################################################
    #
    # Charged ProtoParticles
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return self.getProp("__hlt2ChargedCaloProtosSeq__")
#############################################################################################
    #
    def hlt2ChargedHadronProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return self.getProp("__hlt2ChargedHadronProtosSeq__")	
#############################################################################################
    #
    def hlt2ChargedProtos(self):
        """
        Charged protoparticles which know nothing about PID
        """
        return self.getProp("__hlt2ChargedProtosSeq__")
#############################################################################################
    #
    def hlt2MuonProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return self.getProp("__hlt2MuonProtosSeq__")
#############################################################################################
    #
    # Neutral ProtoParticles
    #
    def hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        """
        return self.getProp("__hlt2NeutralProtosSeq__")
#############################################################################################
    #
    # Velo tracking for the PV making sequence
    #
    def hlt2VeloTracking(self):
        """ 
        Velo tracks
        """
        return self.getProp("__hlt2VeloTrackingSeq__")
#############################################################################################
    #
    # Forward tracking
    #   
    def hlt2ForwardTracking(self):
        """  
        Forward tracks
        """
        return self.getProp("__hlt2ForwardTrackingSeq__")
#############################################################################################
    #
    # Seed tracking
    #   
    def hlt2SeedTracking(self):
        """  
        Seed tracks
        """
        return self.getProp("__hlt2SeedTrackingSeq__")
#############################################################################################
    #
    # Match tracking
    #   
    def hlt2MatchTracking(self):
        """  
        Match tracks
        """
        return self.getProp("__hlt2MatchTrackingSeq__")
#############################################################################################
    #
    # Downstream tracking
    #   
    def hlt2DownstreamTracking(self):
        """  
        Downstream tracks
        """
        return self.getProp("__hlt2DownstreamTrackingSeq__")
#############################################################################################
    #   
    # The Muon ID sequence: makes the tracks and Muon IDs them
    #   
    def hlt2MuonID(self):
        """  
        Muon PID
        """
        return self.getProp("__hlt2MuonIDSeq__")
#############################################################################################
    #   
    # The RICH ID sequence: makes the tracks and RICH IDs them
    #   
    def hlt2RICHID(self):
        """  
        RICH PID
        """
        return self.getProp("__hlt2RICHIDSeq__")
#############################################################################################
    #   
    # The CALO ID: makes the tracks and CALO IDs them
    #   
    def hlt2CALOID(self):
        """  
        CALO PID
        """
        return self.getProp("__hlt2CALOIDSeq__")
#############################################################################################
    #
    # Track preparation
    #
    # This will automatically do the required tracking and then, if required
    # do the specified fast-fit as well; this is the only one of the tracking sequences
    # whose output is used in the protoparticle making
    #
    def hlt2PrepareTracks(self):
        """
	The staged fast fit
	"""
	return self.getProp("__hlt2PrepareTracksSeq__")
#############################################################################################
#############################################################################################
#
# The apply configuration method, makes the bindMembers objects which can then
# be exported, and also checks that the configuration is not inconsistent  
#
#############################################################################################
#############################################################################################
    def __apply_configuration__(self) :
        log.warning('############################################################'	  )
        log.warning('## INFO You have configured an instance of the Hlt2 tracking'	  )
        log.warning('## INFO ----------------------------------------------------'	  )	
        log.warning('## INFO The data type is '	  + str(self.getProp("DataType"         )))
        log.warning('## INFO Tracks to make are ' + str(self.getProp("Hlt2Tracks"       )))
        log.warning('## INFO The prefix is '      + str(self.getProp("Prefix"           )))
        log.warning('## INFO The fit type is '    + str(self.getProp("FastFitType"      )))
        log.warning('## INFO Fast Fit? = '	  + str(self.getProp("DoFastFit"        )))
        log.warning('## INFO Seeding? = '         + str(self.getProp("DoSeeding"        )))
        log.warning('## INFO Clone Killing? = '   + str(self.getProp("DoCloneKilling"   )))
        log.warning('## INFO Use RICH? = '  	  + str(self.getProp("UseRICHForHadrons")))
        log.warning('## INFO Use CALO? = '  	  + str(self.getProp("UseCALOForHadrons")))	 
        log.warning('## INFO Make Neutrals? = '   + str(self.getProp("MakeNeutrals"     )))
	log.warning('## INFO Rich hypos = '       + str(self.getProp("RichHypos"	)))
	log.warning('## INFO Rich radiators = '   + str(self.getProp("RichRadiators"	)))
	log.warning('############################################################'	  )
	#
        # First of all check that I have been called with a sensible set of options
        #
	outputOfHlt2Tracking = self.__shortTrackLocation()
        if (outputOfHlt2Tracking == 'Unknown') :
            self.__fatalErrorUnknownTrackType()
            return []
	#
	if (self.getProp("FastFitType") not in Hlt2TrackingRecognizedFitTypes) :
    	    self.__fatalErrorUnknownTrackFitType()
            return []
	#
	# The base tracking sequences
	#
	self.setProp("__hlt2VeloTrackingSeq__",         self.__hlt2VeloTracking()       )
        self.setProp("__hlt2ForwardTrackingSeq__",      self.__hlt2ForwardTracking()    )
        self.setProp("__hlt2SeedTrackingSeq__",         self.__hlt2SeedTracking()       )
        self.setProp("__hlt2MatchTrackingSeq__",        self.__hlt2MatchTracking()      )
        self.setProp("__hlt2DownstreamTrackingSeq__",   self.__hlt2DownstreamTracking() )
	#
	# The full tracking sequence whose output is used to make the protoparticles
	# Also runs the fast fit, or inserts the error parametrization based on the track PT,
	# depending on the options given to the code.
	#
	self.setProp("__hlt2PrepareTracksSeq__",        self.__hlt2StagedFastFit()      )
	#
	# The PID sequences
	#
	self.setProp("__hlt2MuonIDSeq__",           	self.__hlt2MuonID()         	)
	self.setProp("__hlt2RICHIDSeq__",               self.__hlt2RICHID()             )
	self.setProp("__hlt2CALOIDSeq__",               self.__hlt2CALOID()             )
	#
	# The protoparticles
	#
	self.setProp("__hlt2ChargedProtosSeq__",        self.__hlt2ChargedProtos()      )
	self.setProp("__hlt2ChargedCaloProtosSeq__",	self.__hlt2ChargedCaloProtos()	)
        self.setProp("__hlt2ChargedHadronProtosSeq__",	self.__hlt2ChargedHadronProtos())
        self.setProp("__hlt2MuonProtosSeq__",		self.__hlt2MuonProtos()		)
        self.setProp("__hlt2NeutralProtosSeq__",	self.__hlt2NeutralProtos()	)
#############################################################################################
#############################################################################################
#
# Actual implementation of the functions
#
#############################################################################################
    # 
    # Start with the naming conventions
    #
    # The "short" track location for the derived tracks
    # This function checks that the track asked for by the Hlt2Tracking instance
    # is in the recognised track types, and returns "Unknown" or the correct
    # suffix based on the configuration of the Hlt2Tracking. 
    #
    def __shortTrackLocation(self) :
        if (self.getProp("Hlt2Tracks") not in Hlt2TrackingRecognizedTrackTypes) :
            return "Unknown"
        elif (self.getProp("Hlt2Tracks") == Hlt2LongTracksName) :
            if self.getProp("DoSeeding") :
                return Hlt2LongTracksName
            else : 
		self.__warningAskLongGetForward()	
                return Hlt2ForwardTracksName
        elif (self.getProp("Hlt2Tracks") == Hlt2ForwardTracksName) :
            if self.getProp("DoSeeding") :
		self.__warningAskForwardGetLong()
                return Hlt2LongTracksName
            else :
                return Hlt2ForwardTracksName
        elif (self.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName) :
            return Hlt2DownstreamTracksName
    #
    # Now the "long" track location, for the tracks which will be used to
    # make particles, protoparticles, etc. baseTrack/ProtoPLocation live in
    # HltTrackNames.py  
    # 
    def __trackLocation(self):
	thisTrackLocation 	= self.getProp("FastFitType") + "/" + self.__shortTrackLocation()
        return _baseTrackLocation(self.getProp("Prefix"), thisTrackLocation)
    ## For protos, the format is e.g. Hlt2/ProtoP/Unfitted/Charged/Forward #   
    def __protosLocation(self,protosType):
	thisProtosLocation	= self.getProp("FastFitType") + "/" + self.__shortTrackLocation() + "/" + protosType
        return _baseProtoPLocation(self.getProp("Prefix"), thisProtosLocation)
    #
    # The trackified Muon ID location
    #
    def _trackifiedMuonIDLocation(self) :
        return self.__hltBasePIDLocation() + "/"+ HltMuonTracksName
    #
    # The trackified AllMuon ID location
    # 
    def _trackifiedAllMuonIDLocation(self) :
        return self.__hltBasePIDLocation() + "/"+ HltAllMuonTracksName
    #
    # The PID objects themselves
    #
    def __hltBasePIDLocation(self) : 
        return self.__trackLocation() + "/" + HltSharedPIDPrefix
    #
    def __muonIDLocation(self) :
        return self.__hltBasePIDLocation() + "/" + HltMuonIDSuffix
    #
    def __richIDLocation(self) :
        return self.__hltBasePIDLocation() + "/" + HltRICHIDSuffix
    #
    def __caloIDLocation(self) :
        caloBase =  self.__hltBasePIDLocation() + "/" + HltCALOIDSuffix + "/"
	if (self.getProp("MakeNeutrals")) : caloBase += "Neutrals"
	else : caloBase += "Charged"
        return caloBase
    #
    # The prefixes for the various tools and algorithms used
    #
    def __baseAlgosAndToolsPrefix(self) :
        return self.getProp("Prefix") + self.__shortTrackLocation()
    #
    def __usedAlgosAndToolsPrefix(self) :
        return self.getProp("Prefix") + self.getProp("FastFitType") + self.__shortTrackLocation()
    #
    # Explicitly tell users about the new long/forward naming convention: long tracks are the
    # forward+match tracks (seeding required), forward tracks are just PatForward, no seeding.
    # Since we are not bastards, if someone asks for forward tracks and turns the seeding on we
    # will give them long tracks, but in this case we should print a warning to tell them what
    # happened.
    #
    def __warningAskForwardGetLong(self) :
	log.warning( '##################################################################################')
	log.warning( '## WARNING You asked for Forward tracks with seeding on. I will assume that you   ')
        log.warning( '## WARNING would like both forward and match tracks, and I will put the combined  ')
        log.warning( '## WARNING output of these into the long track container. You can still retrieve  ')
        log.warning( '## WARNING the forward tracks alone by asking for the forward track container.    ')
        log.warning( '##################################################################################')    
    #
    def __warningAskLongGetForward(self) :
        log.warning( '##################################################################################')
        log.warning( '## WARNING You asked for long tracks with seeding off. I will assume that you     ')
        log.warning( '## WARNING would like only forward tracks, which will be placed in the forward    ')
        log.warning( '## WARNING forward track container. The long track container will be left empty.  ')
        log.warning( '##################################################################################')    
    #
    # In case something went wrong when specifying the options, warn me before you die
    #
    def __fatalErrorUnknownTrackType(self) :
        log.fatal( '##################################################################################')
        log.fatal( '## FATAL You specified an unknown track type %s for the Hlt2 Reconstruction' % self.getProp("Hlt2Tracks") )
        log.fatal( '## FATAL I will now die, you need to make Hlt2Tracking aware of this track type!  ')
        log.fatal( '##################################################################################')
    #
    def __fatalErrorUnknownTrackFitType(self) :
	log.fatal( '################################################################################')
        log.fatal( '## FATAL You specified an unknown fit type %s for the Hlt2 Reconstruction' % self.getProp("FastFitType") )
        log.fatal( '## FATAL I will now die, you need to make Hlt2Tracking aware of this fit type!  ')
        log.fatal( '################################################################################')
    #########################################################################################
    #
    # Electron Protos
    #
    def __hlt2ChargedCaloProtos(self) :
        """
        Charged Calo protoparticles = electrons
        Requires chargedProtos
        """
        from Configurables import ChargedProtoCombineDLLsAlg 
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        #
        chargedProtos 			= self.__hlt2ChargedProtos()
        chargedProtosOutputLocation 	= chargedProtos.outputSelection()
        # Fill the Combined DLL information in the charged protoparticles
	combine_name			= self.__usedAlgosAndToolsPrefix()+"ChargedCaloProtoPCombDLLs"
        combine 			= ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation 	= chargedProtosOutputLocation
        # Fill the sequence
        myCALOProcessorChargedSeq 	= self.__getCALOSeq("Charged")
        # we need the calo reconstruction
        caloreco 			= self.__hlt2CALOID()
        
	from HltLine.HltLine import bindMembers
	# Build the bindMembers 
	bm_name 	= self.__usedAlgosAndToolsPrefix()+"ChargedCaloProtosSeq"
        bm_members 	= [ chargedProtos, caloreco, myCALOProcessorChargedSeq, combine ]
	bm_output	= chargedProtosOutputLocation  

	return bindMembers(bm_name, bm_members).setOutputSelection(bm_output) 
        
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
       
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        chargedProtos 			= self.__hlt2ChargedProtos()
        chargedProtosOutputLocation 	= chargedProtos.outputSelection()
        #
        muon_name			= self.__usedAlgosAndToolsPrefix()+"ChargedProtoPAddMuon"
        muon 				= ChargedProtoParticleAddMuonInfo(muon_name)
        muon.ProtoParticleLocation 	=  chargedProtosOutputLocation
        # Get the MuonID from the MuonID sequence
        muonID 				= self.__hlt2MuonID()
        muon.InputMuonPIDLocation 	= muonID.outputSelection()
        # Fill the Combined DLL information in the charged protoparticles
	combine_name			= self.__usedAlgosAndToolsPrefix()+"ChargedMuonProtoPCombDLLs" 
        combine 			= ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation 	= chargedProtosOutputLocation
    
        from HltLine.HltLine import bindMembers
	# Build the bindMembers 
        bm_name         = self.__usedAlgosAndToolsPrefix()+"MuonProtosSeq" 
        bm_members      = [ muonID, chargedProtos, muon, combine ]
        bm_output       = chargedProtosOutputLocation

   	return bindMembers(bm_name, bm_members).setOutputSelection(bm_output) 
    #########################################################################################
    #
    # Charged hadron ProtoParticles
    # Does not necessarily use RICH or CALO but can do if so configured
    #
    def __hlt2ChargedHadronProtos(self):
        """
        Charged hadron protoparticles = pion, kaon, proton
        If RICH is on, then requires rich, calo and does dll combination
        This is off by default
        """
        from Configurables import ( ChargedProtoParticleAddRichInfo,
                                    ChargedProtoCombineDLLsAlg)
    
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
	#
        chargedProtos 			= self.__hlt2ChargedProtos()
        chargedProtosOutputLocation 	= chargedProtos.outputSelection()
        #
	# Now set up the RICH sequence 
	#
	addRICHInfo 			= self.__hlt2RICHID()
	#
	# And the CALO RECO sequence
	#
	addCALOInfo 			= self.__hlt2CALOID()
	myCALOProcessorChargedSeq 	= self.__getCALOSeq("Charged")
	#
	# Add the RICH info to the DLL
	#	
	richDLL_name			= self.__usedAlgosAndToolsPrefix()+"ChargedHadronProtoPAddRich"
	richDLL 			= ChargedProtoParticleAddRichInfo(richDLL_name)
	richDLL.InputRichPIDLocation	= addRICHInfo.outputSelection()
        richDLL.ProtoParticleLocation	= chargedProtosOutputLocation
	#
	# The combined DLL 
	#   
        combine_name 			= self.__usedAlgosAndToolsPrefix()+"ChargedHadronProtoPCombDLLs"

	if (self.getProp("UseRICHForHadrons")) : combine_name += "UseRICHForHadrons" 
	if (self.getProp("UseCALOForHadrons")) : combine_name += "UseCALOForHadrons"
	
	combine				= ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation 	= chargedProtosOutputLocation
	#
	# What are we returning?
	#
	sequenceToReturn = []
	sequenceToReturn += [chargedProtos]
	if (self.getProp("UseRICHForHadrons")) : 
		sequenceToReturn += [addRICHInfo]
		sequenceToReturn += [richDLL]
	if (self.getProp("UseCALOForHadrons")) :
		sequenceToReturn += [addCALOInfo]
		sequenceToReturn += [myCALOProcessorChargedSeq]
	if (self.getProp("UseRICHForHadrons") or self.getProp("UseCALOForHadrons")) :
		sequenceToReturn += [combine]

	from HltLine.HltLine import bindMembers
        # Build the bindMembers 
        bm_name         = self.__usedAlgosAndToolsPrefix()+"ChargedHadronProtosSeq"
        bm_members      = sequenceToReturn
        bm_output       = chargedProtosOutputLocation

	return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Charged ProtoParticles
    #
    def __hlt2ChargedProtos(self):
        """
        Charged protoparticles
        
        Requires tracks, fitted if necessary
        """
        from Configurables import ChargedProtoParticleMaker
        
        chargedProtosOutputLocation	= self.__protosLocation(Hlt2ChargedProtoParticleSuffix)
    
	charged_name			= self.__usedAlgosAndToolsPrefix()+'ChargedProtoPAlg'
        charged 			= ChargedProtoParticleMaker(charged_name)

        # Need to allow for fitted tracks
        # This is now done inside the staged fast fit based on the fastFitType passed
        tracks 				= self.__hlt2StagedFastFit()
        charged.InputTrackLocation 	= [tracks.outputSelection()]
        
	charged.OutputProtoParticleLocation = chargedProtosOutputLocation
    
        from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"ChargedProtosSeq" 
        bm_members      = [ tracks , charged ]
        bm_output       = chargedProtosOutputLocation
        
        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Neutral ProtoParticles
    #
    def __hlt2NeutralProtos(self):
        """
        Neutral protoparticles 
        Requires caloID 
        """

	# the sequence to run
        myCALOProcessorNeutralSeq 	= self.__getCALOSeq("Neutral")
        
	caloID 				= self.__hlt2CALOID()
        
        from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"NeutralProtosSeq" 
        bm_members      = [ caloID, myCALOProcessorNeutralSeq ]
        bm_output       = myCALOProcessorNeutralSeq.outputSelection()

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # MuonID
    #
    def __hlt2MuonID(self) :
        """
        Muon ID options 
        Requires tracks
        """
	    
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg

	
        cm 					= ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))
	HltMuonIDAlg_name			= self.__usedAlgosAndToolsPrefix()+"MuonIDAlg"
        HltMuonIDAlg 				= cm.configureMuonIDAlg(HltMuonIDAlg_name)
        #The tracks to use
        tracks					= self.__hlt2StagedFastFit()
        #Enforce naming conventions
        HltMuonIDAlg.TrackLocation		= tracks.outputSelection() 
        HltMuonIDAlg.MuonIDLocation		= self.__muonIDLocation() #output 
        HltMuonIDAlg.MuonTrackLocation		= self._trackifiedMuonIDLocation() 
        HltMuonIDAlg.MuonTrackLocationAll	= self._trackifiedAllMuonIDLocation() 
        # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
        #       a factor 3-4
        HltMuonIDAlg.FindQuality		= False
        
        from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"MuonIDSeq"
        bm_members      = [ tracks, MuonRec(), HltMuonIDAlg ]
        bm_output       = HltMuonIDAlg.MuonIDLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Calo ID
    #
    def __hlt2CALOID(self) :
        """
        Calo ID
        Does calo reconstruction and Calo ID
        Requires tracks 
        """
	tracks			= self.__hlt2StagedFastFit()

	# the sequence to run
	myCALOProcessorPIDSeq	= self.__getCALOSeq("PID")

	from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"CALOIDSeq"
        bm_members      = [ tracks, myCALOProcessorPIDSeq ]
        bm_output       = myCALOProcessorPIDSeq.outputSelection()

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #   
    # RICH ID
    #   
    def __hlt2RICHID(self) :
	"""
	Set up the sequence for doing the RICH PID on the tracks
	"""
        from HltLine.HltLine import bindMembers
	from Configurables import RichRecSysConf
	#configure the rich reco
	# first give it a nice name, in this case constructed 
	# from the name of our configurable and the RichRecoSys...
	richSeqName			= self._instanceName(RichRecSysConf) 
        richSeq				= GaudiSequencer(richSeqName+"Seq")
        richSeq.MeasureTime		= True
        # The RICH COnfigurable 
        # note, the context MUST contain the string HLT
        # in whatever case
	richConf			= RichRecSysConf(richSeqName)
	richConf.Context		= richSeqName
	richConf.RichPIDLocation	= self.__richIDLocation()
	richConf.PidConfig              = "FastGlobal"
	richConf.CheckProcStatus	= False
	richConf.InitPixels		= True
        richConf.InitTracks		= True
        richConf.InitPhotons		= True
        richConf.TracklessRingAlgs	= []
	richConf.Particles 		= self.getProp("RichHypos")
	richConf.Radiators		= self.getProp("RichRadiators")
	# Set the sequence to run the RICH PID in
        richConf.setProp("RecoSequencer",richSeq)
	# The input location of the tracks
	tracks				= self.__hlt2StagedFastFit()
	richConf.trackConfig().InputTracksLocation = tracks.outputSelection()
	
	from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"RICHIDSeq"
        bm_members      = [tracks, richSeq]
        bm_output       = richConf.RichPIDLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
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
    
        from HltLine.HltLine import bindMembers

        #Make the original tracks in case this has not been run already
        tracks					= self.__hlt2Tracking()
        #And my output location is?
        hlt2StagedFastFitOutputLocation		= self.__trackLocation()
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
                    				, SlowContainer  = True)
	    # Build the bindMembers        
            bm_name         = self.__usedAlgosAndToolsPrefix()+"NoFastFitSeq" 
            bm_members      = [tracks, recoCopy]
            bm_output       = hlt2StagedFastFitOutputLocation

            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    
        from Configurables import TrackEventFitter, TrackMasterFitter
	Hlt2StagedFastFit_name			= self.__usedAlgosAndToolsPrefix()+'StagedFastFit'
        Hlt2StagedFastFit			= TrackEventFitter(Hlt2StagedFastFit_name)
        Hlt2StagedFastFit.TracksInContainer	= tracks.outputSelection() 
        Hlt2StagedFastFit.TracksOutContainer	= hlt2StagedFastFitOutputLocation  
        
	Hlt2StagedFastFitSeq_name               = Hlt2StagedFastFit_name+'Seq'
        Hlt2StagedFastFitSeq                    = GaudiSequencer(Hlt2StagedFastFitSeq_name)
        Hlt2StagedFastFitSeq.Members		= [ Hlt2StagedFastFit ]
      
	Hlt2StagedFastFit.addTool(TrackMasterFitter, name = 'Fitter')
        from TrackFitter.ConfiguredFitters import ConfiguredFastFitter
        fitter = ConfiguredFastFitter( getattr(Hlt2StagedFastFit,'Fitter'))
	# The following only applies for the bidirectional fit
	if (self.getProp("FastFitType") == HltBiDirectionalKalmanFitSuffix) :
		fitter.NodeFitter.BiDirectionalFit	= True
        	fitter.NodeFitter.Smooth		= True
        	fitter.AddDefaultReferenceNodes		= True    # says Wouter
        
	# Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"FastFitSeq"
        bm_members      = [tracks, Hlt2StagedFastFitSeq]
        bm_output       = hlt2StagedFastFitOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Track reconstruction
    #
    def __hlt2Tracking(self) :
        """
        Track reconstruction
        """
	from HltLine.HltLine import bindMembers    
        #
        # Some "inevitable" special stuff here
        # If we ask for long tracks and no seeding, the final output will be
        # the forward tracking, otherwise the final output is the long 
        # (forward + match) tracking.
        #
        hlt2TrackingOutput		= _baseTrackLocation(self.getProp("Prefix"),self.__shortTrackLocation())
        
	# The clone killing
        from Configurables import TrackEventCloneKiller, TrackCloneFinder
        cloneKiller = TrackEventCloneKiller( self.__baseAlgosAndToolsPrefix()+'FastCloneKiller'
                    			   , TracksInContainers 	= [hlt2TrackingOutput]
                    			   , SkipSameContainerTracks 	= False
                    			   , CopyTracks 		= False)
        cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
        cloneKiller.CloneFinderTool.RestrictedSearch = True
        
	# Finally make the sequence
        # The sequence called depends on the track type, so far we recognise two track types
        # for Long tracks the options doSeeding, doCloneKilling, etc. are meaningful
        # for SeedTT tracks the seeding is mandatory and the CloneKilling is irrelevant 
        # This part gets done in either case
        trackRecoSequence = []
        if (self.__shortTrackLocation() == Hlt2LongTracksName) :
	    # Do the forward, seeding + matching
    	    trackRecoSequence		=	[self.__hlt2ForwardTracking()]
	    trackRecoSequence		+=	[self.__hlt2MatchTracking()]
	    # The copy sequence, which merges the forward and
            # match tracks into the long track container
            hlt2TracksToMergeIntoLong	=	[]
	    hlt2TracksToMergeIntoLong	+=	[ self.__hlt2ForwardTracking().outputSelection()]
	    hlt2TracksToMergeIntoLong   +=	[ self.__hlt2MatchTracking().outputSelection()  ]
            from Configurables import CreateFastTrackCollection
            #### CreateFastTrackCollection
            recoCopy = CreateFastTrackCollection(self.__baseAlgosAndToolsPrefix()+'TrackCopy'
                                               , InputLocations = hlt2TracksToMergeIntoLong
                                               , OutputLocation = hlt2TrackingOutput
                                               , SlowContainer 	= True) 
	    ##############################
	    trackRecoSequence		+= 	[recoCopy]
	elif (self.__shortTrackLocation() == Hlt2ForwardTracksName) :
	    trackRecoSequence		= 	[self.__hlt2ForwardTracking()]
	elif (self.__shortTrackLocation() == Hlt2DownstreamTracksName ) :
    	    trackRecoSequence 		=	[self.__hlt2DownstreamTracking()]
	# Do the clone killing if required 
        if self.getProp("DoCloneKilling") : 
            trackRecoSequence		+=      [cloneKiller]
        #
        # Hacking of errors
        # @todo Needs to be revised
        #
        # check if the context contains the magic phrase 'Fitted', if so
        # either the tracks were already fitted, 
        # or we need to do the Kalman fit on-demand afterwards, but
        # either way we do NOT parametarize the errors
        # Also, do not insert errors for downstream tracks as these would be meaningless
        if ((self.getProp("Hlt2Tracks") == Hlt2DownstreamTracksName) and (not self.getProp("DoFastFit"))) :
            from Configurables import HltInsertTrackErrParam
	    HltInsertTrackErrParam_name 	 =  self.__baseAlgosAndToolsPrefix()+"InsertTrackErrParam"
       	    HltInsertTrackErrParam		 =  HltInsertTrackErrParam(HltInsertTrackErrParam_name)
       	    HltInsertTrackErrParam.InputLocation =  hlt2TrackingOutput
	    trackRecoSequence           	 += HltInsertTrackErrParam

	# Build the bindMembers        
        bm_name         = self.__usedAlgosAndToolsPrefix()+"TrackingSeq"
        bm_members      = trackRecoSequence
        bm_output       = hlt2TrackingOutput

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Velo Reconstruction
    #
    def __hlt2VeloTracking(self):
        """
        Velo track reconstruction for Hlt2
        """
        from Configurables 	import Tf__PatVeloSpaceTracking
	from Configurables      import Tf__PatVeloGeneralTracking
	from Configurables      import Tf__PatVeloSpaceTool
        #From HltReco we just get the shared stuff between Hlt1 and Hlt2
        from HltReco		import MinimalRZVelo
        from HltLine.HltLine	import bindMembers 
    
        veloTracksOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2VeloTracksName) 
       
        recoVelo		 = Tf__PatVeloSpaceTracking(self.getProp("Prefix")+'RecoVelo'
                                       	, InputTracksName  = MinimalRZVelo.outputSelection() 
                                       	, OutputTracksName = veloTracksOutputLocation )
    
        recoVelo.addTool( Tf__PatVeloSpaceTool(), name="PatVeloSpaceTool" )
        recoVelo.PatVeloSpaceTool.MarkClustersUsed=True
    
        recoVeloGeneral		 = Tf__PatVeloGeneralTracking(self.getProp("Prefix")+'RecoVeloGeneral'
                                       	, OutputTracksLocation = veloTracksOutputLocation )
   
	# Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"VeloTracking"
        bm_members      = MinimalRZVelo.members() + [recoVelo,recoVeloGeneral]
        bm_output       = veloTracksOutputLocation
    
        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Forward Reconstruction
    #
    def __hlt2ForwardTracking(self) :
        """
        Forward track reconstruction for Hlt2
        """
        from Configurables	import PatForward
	from Configurables      import PatForwardTool
    	from HltLine.HltLine	import bindMembers
    
        forwardTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2ForwardTracksName) 
    
        recoForward		   = PatForward( self.getProp("Prefix")+'RecoForward'
                           		, InputTracksName  = self.__hlt2VeloTracking().outputSelection() 
                            		, OutputTracksName = forwardTrackOutputLocation )

        PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
        recoForward.addTool(PatForwardTool, name='PatForwardTool')
        recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"

	# Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"ForwardTracking"
        bm_members      = self.__hlt2VeloTracking().members() + self.__hlt2TrackerDecoding().members() + [recoForward]
        bm_output       = forwardTrackOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Seeding for forward,downstream reconstruction
    #
    def __hlt2SeedTracking(self):
        """
        Seeding in the trackers for later use in Match Forward reconstruction
        or in downstream tracking.
        """
        from Configurables	import PatSeeding
        from Configurables      import PatSeedingTool
    	from HltLine.HltLine	import bindMembers

	# We depend on the forward tracking
	fwdtracks = self.__hlt2ForwardTracking()
	# Now our output location 
        seedTrackOutputLocation	= _baseTrackLocation(self.getProp("Prefix"),Hlt2SeedingTracksName)
    
        #### Seeding 
        recoSeeding = PatSeeding(self.getProp("Prefix")+'Seeding', OutputTracksName = seedTrackOutputLocation)
        recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")
        recoSeeding.PatSeedingTool.UseForward		= True
        recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
        recoSeeding.PatSeedingTool.InputTracksName	= fwdtracks.outputSelection()
   
	# Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"SeedTracking" 
        bm_members      = self.__hlt2TrackerDecoding().members() + [recoSeeding]
        bm_output       = seedTrackOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Match Forward Reconstruction
    #
    def __hlt2MatchTracking(self):
        """
        Forward track reconstruction for Hlt2 using seeding
        """
        from Configurables	import PatMatch
    	from HltLine.HltLine	import bindMembers
    
        matchTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2MatchTracksName)
 
        #### Matching
        recoMatch		 = PatMatch(self.getProp("Prefix")+'Match'
                         		, VeloInput = self.__hlt2VeloTracking().outputSelection()
                         		, SeedInput = self.__hlt2SeedTracking().outputSelection()
                                	, MatchOutput = matchTrackOutputLocation)
   
	# Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"MatchTracking"
        bm_members      = self.__hlt2VeloTracking().members() + self.__hlt2SeedTracking().members() + [recoMatch]
        bm_output       = matchTrackOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 Downstream Forward Reconstruction
    #
    def __hlt2DownstreamTracking(self):
        """
        Downstream track reconstruction for Hlt2 using seeding
        """
        from Configurables	import PatDownstream
    	from HltLine.HltLine	import bindMembers
    
        downstreamTrackOutputLocation	= _baseTrackLocation(self.getProp("Prefix"),Hlt2DownstreamTracksName)
    
        ### Downstream tracking
        PatDownstream			= PatDownstream(self.getProp("Prefix")+'PatDownstream')
        PatDownstream.InputLocation	= self.__hlt2SeedTracking().outputSelection()
        PatDownstream.OutputLocation	= downstreamTrackOutputLocation
        #PatDownstream.UseForward	= True
        #PatDownstream.SeedFilter	= True
        PatDownstream.RemoveUsed	= True
        PatDownstream.RemoveAll		= True
   
	# Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"DownstreamTracking" 
        bm_members      = self.__hlt2SeedTracking().members() + [PatDownstream]
        bm_output       = downstreamTrackOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Hlt2 tracker decoding
    #
    def __hlt2TrackerDecoding(self):
        """
        Decode the ST for Hlt2
        """
        from HltLine.HltDecodeRaw 	import DecodeTT, DecodeIT
        from HltLine.HltLine		import bindMembers
	return bindMembers(self.getProp("Prefix")+"DecodeSTSeq", DecodeTT.members() + DecodeIT.members())
    #########################################################################################
    #
    # Helper function to set up the CALO processor and return the correct sequence 
    #
    def __getCALOSeq(self,mode):
	"""
	Defines the CALO processor and, depending on the mode,
	returns the PID, charged proto, or neutral proto sequences 
	"""
	tracks 				= self.__hlt2StagedFastFit()
	chargedProtos 			= self.__hlt2ChargedProtos()       
 
        outputCALOPID			= self.__caloIDLocation()
        
        from Configurables	import CaloProcessor
	from HltLine.HltLine	import bindMembers

        caloProcessorName 		= self._instanceName(CaloProcessor)   
 
        myCALOProcessor 		= CaloProcessor(caloProcessorName) 
        myCALOProcessor.Context 	= outputCALOPID
        # Do the reconstruction and the PID but do not make or update the
        # protoparticles here! 
        myCALOProcessor.CaloReco 	= True 
        myCALOProcessor.CaloPIDs 	= True
        #
	# Check if we are making neutrals or not
	#
	myCALOProcessor.SkipNeutrals    = True
        myCALOProcessor.SkipCharged     = False
	if (self.getProp("MakeNeutrals") == True): 
		myCALOProcessor.SkipNeutrals	= False
        	myCALOProcessor.SkipCharged 	= True

	# The sequences are given the track and protoparticle locations when initializing 
	myPIDSeq 	= myCALOProcessor.caloSequence(		[tracks.outputSelection()]								)
	myChargedSeq 	= myCALOProcessor.chargedProtoSequence(	[tracks.outputSelection()],	chargedProtos.outputSelection()				)
	myNeutralSeq 	= myCALOProcessor.neutralProtoSequence(	[tracks.outputSelection()],	self.__protosLocation(Hlt2NeutralProtoParticleSuffix)	)

	if ( mode == "PID" ) :
		return bindMembers( self.__usedAlgosAndToolsPrefix()+"CALOPIDSeq", [tracks, myPIDSeq]).setOutputSelection(outputCALOPID)
	if ( mode == "Neutral" ) :
		return bindMembers( self.__usedAlgosAndToolsPrefix()+"CaloNeutralSeq", [tracks, myPIDSeq, myNeutralSeq]).setOutputSelection(myCALOProcessor.NeutralProtoLocation)
	if ( mode == "Charged" ) :
		return bindMembers( self.__usedAlgosAndToolsPrefix()+"CaloChargedSeq", [tracks, myPIDSeq, chargedProtos, myChargedSeq])
