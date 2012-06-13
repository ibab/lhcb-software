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
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.25 $"
# =============================================================================
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *
from HltTrackNames import Hlt2LongTracksName, HltSharedTracksPrefix
from HltTrackNames import HltBiDirectionalKalmanFitSuffix
from HltTrackNames import Hlt2ForwardTracksName, Hlt2SeedingTracksName, Hlt2ForwardSecondLoopTracksName
from HltTrackNames import Hlt2MatchTracksName, Hlt2DownstreamTracksName   
from HltTrackNames import Hlt2VeloTracksName, HltSharedRZVeloTracksName
from HltTrackNames import Hlt2ChargedProtoParticleSuffix, Hlt2TrackingRecognizedTrackTypes
from HltTrackNames import Hlt2TrackingRecognizedFitTypes
from HltTrackNames import _baseTrackLocation, _baseProtoPLocation
from HltTrackNames import HltMuonTracksName, HltAllMuonTracksName
from HltTrackNames import HltMuonIDSuffix, HltRICHIDSuffix, HltCALOIDSuffix, HltSharedPIDPrefix 
from HltTrackNames import HltNoPIDSuffix, HltCaloProtosSuffix, HltMuonProtosSuffix, HltRichProtosSuffix
from HltTrackNames import HltUnfittedTracksSuffix, HltUniDirectionalKalmanFitSuffix
from HltTrackNames import HltGlobalTrackLocation                
from HltTrackNames import Hlt2ChargedProtoParticleSuffix, Hlt2NeutralProtoParticleSuffix  
from HltTrackNames import HltRichDefaultHypos, HltRichDefaultRadiators
from HltTrackNames import Hlt2TrackingRecognizedFitTypesForRichID 
from HltTrackNames import HltRichDefaultTrackCuts, HltDefaultTrackCuts

from Configurables import CaloProcessor, RichRecSysConf, TrackSelector

from Hlt2Lines.Hlt2CharmHadLambdaC2LambdaHLines import Hlt2CharmHadLambdaCLambdaHLinesConf 
from Hlt2Lines.Hlt2B2DXLines              import Hlt2B2DXLinesConf
from Hlt2Lines.Hlt2B2JpsiXLines           import Hlt2B2JpsiXLinesConf
from Hlt2Lines.Hlt2B2PhiXLines            import Hlt2B2PhiXLinesConf
from Hlt2Lines.Hlt2InclusiveDiMuonLines   import Hlt2InclusiveDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveMuonLines     import Hlt2InclusiveMuonLinesConf
from Hlt2Lines.Hlt2InclusivePhiLines      import Hlt2InclusivePhiLinesConf
from Hlt2Lines.Hlt2TopologicalLines       import Hlt2TopologicalLinesConf
from Hlt2Lines.Hlt2B2XGammaLines          import Hlt2B2XGammaLinesConf
#commented out v13r4p1
from Hlt2Lines.Hlt2D2XGammaLines          import Hlt2D2XGammaLinesConf
from Hlt2Lines.Hlt2B2HHLines              import Hlt2B2HHLinesConf
from Hlt2Lines.Hlt2B2LLXLines             import Hlt2B2LLXLinesConf
from Hlt2Lines.Hlt2CharmHadD02HHLines     import Hlt2CharmHadD02HHLinesConf
from Hlt2Lines.Hlt2CharmHadTwoBodyForMultiBody     import Hlt2CharmHadTwoBodyForMultiBodyConf
#needed for v13r4p1
#from Hlt2Lines.Hlt2CharmHadD02HHHHLines  import Hlt2CharmHadD02HHHHLinesConf
from Hlt2Lines.Hlt2CharmHadD2HHHLines     import Hlt2CharmHadD2HHHLinesConf
from Hlt2Lines.Hlt2CharmHadTwoBodyForD02HHHH  import Hlt2CharmHadTwoBodyForD02HHHHConf
from Hlt2Lines.Hlt2CharmHadD02HHHHDstLines  import Hlt2CharmHadD02HHHHDstLinesConf
from Hlt2Lines.Hlt2CharmHadD02HHHHDstNoHlt1Lines import Hlt2CharmHadD02HHHHDstNoHlt1LinesConf
from Hlt2Lines.Hlt2CharmSemilepD02HMuNuLines     import Hlt2CharmSemilepD02HMuNuLinesConf
from Hlt2Lines.Hlt2CharmSemilepTwoMuonForMuMuHad import Hlt2CharmSemilepTwoMuonForMuMuHadConf
from Hlt2Lines.Hlt2CharmSemilepD02HHMuMuLines   import Hlt2CharmSemilepD02HHMuMuLinesConf
from Hlt2Lines.Hlt2CharmSemilepD2HMuMuLines   import Hlt2CharmSemilepD2HMuMuLinesConf
from Hlt2Lines.Hlt2CharmRareDecayLines    import Hlt2CharmRareDecayLinesConf
from Hlt2Lines.Hlt2CharmHadD2KS0HLines    import Hlt2CharmHadD2KS0HLinesConf
from Hlt2Lines.Hlt2CharmHadD02HHKsLines   import Hlt2CharmHadD02HHKsLinesConf  
from Hlt2Lines.Hlt2DisplVerticesLines     import Hlt2DisplVerticesLinesConf
from Hlt2Lines.Hlt2HighPtJetsLines        import Hlt2HighPtJetsLinesConf
from Hlt2Lines.Hlt2CommissioningLines     import Hlt2CommissioningLinesConf
from Hlt2Lines.Hlt2ExpressLines           import Hlt2ExpressLinesConf
from Hlt2Lines.Hlt2diphotonDiMuonLines    import Hlt2diphotonDiMuonLinesConf
from Hlt2Lines.Hlt2InclusiveDiProtonLines import Hlt2InclusiveDiProtonLinesConf
from Hlt2Lines.Hlt2InclusiveDiPhiLines    import Hlt2InclusiveDiPhiLinesConf
from Hlt2Lines.Hlt2Dst2D2XXLines          import Hlt2Dst2D2XXLinesConf
from Hlt2Lines.Hlt2InclusiveDiElectronLines import Hlt2InclusiveDiElectronLinesConf
from Hlt2Lines.Hlt2InclusiveElectronLines   import Hlt2InclusiveElectronLinesConf
from Hlt2Lines.Hlt2B2KstareeLines           import Hlt2B2KstareeLinesConf
from Hlt2Lines.Hlt2B2HHLTUnbiasedLines      import Hlt2B2HHLTUnbiasedLinesConf
from Hlt2Lines.Hlt2B2KsHHLines              import Hlt2B2KsHHLinesConf
from Hlt2Lines.Hlt2B2HHPi0Lines             import Hlt2B2HHPi0LinesConf
from Hlt2Lines.Hlt2MuNTrackLines             import Hlt2MuNTrackLinesConf
from Hlt2Lines.Hlt2CharmSemilepD02HMuNuLines           import Hlt2CharmSemilepD02HMuNuLinesConf
from Hlt2Lines.Hlt2RadiativeTopoLines        import Hlt2RadiativeTopoConf
from Hlt2Lines.Hlt2CharmHadLambdaCLines import Hlt2CharmHadLambdaCLinesConf
from Hlt2Lines.Hlt2CharmHadMinBiasLines import Hlt2CharmHadMinBiasLinesConf
from Hlt2Lines.Hlt2CharmHadD2KS0KS0Lines import Hlt2CharmHadD2KS0KS0LinesConf
from Hlt2Lines.Hlt2CharmHadD02HHXDstLines import Hlt2CharmHadD02HHXDstLinesConf
from Hlt2Lines.Hlt2CEPLines import Hlt2CEPLinesConf
#################################################################################################
#
# Hlt2 Tracking
#
class Hlt2Tracking(LHCbConfigurableUser):
    #############################################################################################
    # 
    # First of all the dependencies and the slots
    #
    #############################################################################################
    __used_configurables__ = [ (CaloProcessor   ,   None)
                             , (RichRecSysConf  ,   None)
                             # This next part defines all the Hlt2 Lines since they 
                             # configured after the tracking. This means that each
                             # Hlt2Lines configurable MUST be a singleton AND this
                             # list must be EXACTLY the same as the one in 
                             # $HLTCONFROOT/python/HltConf/Hlt2.py 
                             , Hlt2TopologicalLinesConf
                             , Hlt2B2DXLinesConf
                             , Hlt2CharmHadD02HHLinesConf
                             , Hlt2CharmHadD2HHHLinesConf
                             , Hlt2CharmHadD2KS0HLinesConf 
                             , Hlt2CharmHadD02HHKsLinesConf
                             , Hlt2CharmHadTwoBodyForMultiBodyConf
                             , Hlt2CharmHadTwoBodyForD02HHHHConf
                             #needed for v13r4p1
                             #, Hlt2CharmHadD02HHHHLinesConf
                             , Hlt2CharmHadD02HHHHDstLinesConf
                             , Hlt2CharmHadD02HHHHDstNoHlt1LinesConf  
                             , Hlt2CharmSemilepD02HMuNuLinesConf
                             , Hlt2CharmSemilepD02HHMuMuLinesConf
                             , Hlt2CharmSemilepD2HMuMuLinesConf
                             , Hlt2CharmSemilepTwoMuonForMuMuHadConf
                             , Hlt2CharmRareDecayLinesConf
                             , Hlt2InclusiveDiMuonLinesConf
                             , Hlt2InclusiveMuonLinesConf
                             , Hlt2InclusivePhiLinesConf
                             , Hlt2B2JpsiXLinesConf
                             , Hlt2B2PhiXLinesConf
                             , Hlt2B2XGammaLinesConf
                             #commented out for v13r4p1
                             , Hlt2D2XGammaLinesConf
                             , Hlt2B2HHLinesConf
                             , Hlt2B2HHLTUnbiasedLinesConf
                             , Hlt2B2LLXLinesConf                         
                             , Hlt2DisplVerticesLinesConf
                             , Hlt2HighPtJetsLinesConf
                             , Hlt2CommissioningLinesConf
                             , Hlt2ExpressLinesConf
                             , Hlt2diphotonDiMuonLinesConf
                             , Hlt2InclusiveDiProtonLinesConf
                             , Hlt2InclusiveDiPhiLinesConf  
                             , Hlt2Dst2D2XXLinesConf
                             , Hlt2InclusiveDiElectronLinesConf
                             , Hlt2InclusiveElectronLinesConf
                             , Hlt2B2KstareeLinesConf  
                             , Hlt2B2HHPi0LinesConf
                             , Hlt2B2KsHHLinesConf
                             , Hlt2MuNTrackLinesConf
                             , Hlt2CharmSemilepD02HMuNuLinesConf
                             , Hlt2RadiativeTopoConf
                             , Hlt2CharmHadLambdaCLinesConf
                             , Hlt2CharmHadLambdaCLambdaHLinesConf
                             , Hlt2CharmHadMinBiasLinesConf
                             , Hlt2CharmHadD2KS0KS0LinesConf
                             , Hlt2CharmHadD02HHXDstLinesConf
                             , Hlt2CEPLinesConf
                             ]
    __slots__ = { "DataType"                        : '2012' # datatype  2009, MC09, DC06...
                , "EarlyDataTracking"               : False
                , "Hlt2Tracks"                      : Hlt2ForwardTracksName
                , "Prefix"                          : HltSharedTracksPrefix 
                , "FastFitType"                     : HltUnfittedTracksSuffix
                , "DoFastFit"                       : False
                , "DoSeeding"                       : False
                , "DoCloneKilling"                  : False
                , "RichHypos"                       : HltRichDefaultHypos
                , "RichRadiators"                   : HltRichDefaultRadiators
                , "RichTrackCuts"                   : HltRichDefaultTrackCuts
                , "TrackCuts"                       : HltDefaultTrackCuts
                , "Hlt2ForwardMaxVelo"              : 0
                # TODO : make these variables, not slots 
                , "__hlt2ChargedNoPIDsProtosSeq__"  : 0
                , "__hlt2ChargedNoPIDsSecondLoopProtosSeq__"  : 0
                , "__hlt2ChargedCaloProtosSeq__"    : 0
                , "__hlt2ChargedRichProtosSeq__"    : 0
                , "__hlt2ChargedMuonSecondLoopProtosSeq__"    : 0
                , "__hlt2ChargedMuonProtosSeq__"    : 0
                , "__hlt2ChargedMuonWithCaloProtosSeq__": 0
                , "__hlt2NeutralProtosSeq__"        : 0
                , "__hlt2PhotonsFromL0Seq__"        : 0
                , "__hlt2Pi0FromL0Seq__"            : 0
                , "__hlt2ElectronsFromL0Seq__"      : 0
                , "__hlt2VeloTrackingSeq__"         : 0
                , "__hlt2ForwardTrackingSeq__"      : 0
                , "__hlt2ForwardSecondLoopTrackingSeq__"      : 0
                , "__hlt2SeedTrackingSeq__"         : 0
                , "__hlt2MatchTrackingSeq__"        : 0
                , "__hlt2DownstreamTrackingSeq__"   : 0
                , "__hlt2PrepareTracksSeq__"        : 0
                , "__hlt2MuonIDSeq__"               : 0
                , "__hlt2RICHIDSeq__"               : 0
                , "__hlt2CALOIDSeq__"               : 0
                }
    #############################################################################################
    #############################################################################################
    #
    # Externally visible methods to return the created bindMembers objects
    #
    #############################################################################################
    #############################################################################################
    #   
    # Charged ProtoParticles with no PID information
    #
    def hlt2ChargedNoPIDsProtos(self,secondLoop=False):
        """
        Charged protoparticles from Calo (=electrons)
        """
        if not secondLoop:
            return self.getProp("__hlt2ChargedNoPIDsProtosSeq__")
        else :
            return self.getProp("__hlt2ChargedNoPIDsSecondLoopProtosSeq__")
    #############################################################################################
    #
    # Charged ProtoParticles with CALO ID
    #
    def hlt2ChargedCaloProtos(self):
        """
        Charged protoparticles from Calo (=electrons)
        """
        return self.getProp("__hlt2ChargedCaloProtosSeq__")
    #############################################################################################
    #
    # Charged ProtoParticles with RICH ID
    #
    def hlt2ChargedRichProtos(self):
        """
        Charged protoparticles using RICH (=pions,protons,kaons)
        """
        return self.getProp("__hlt2ChargedRichProtosSeq__")
    #############################################################################################
    #
    # Charged ProtoParticles with Muon ID
    #
    def hlt2ChargedMuonProtos(self, secondLoop=False):
        """
        Charged protoparticles using Muon (=Muons)
        """
        if not secondLoop:
            return self.getProp("__hlt2ChargedMuonProtosSeq__")
        else :
            return self.getProp("__hlt2ChargedMuonSecondLoopProtosSeq__")
        
    #############################################################################################
    #
    # Charged ProtoParticles with Muon and CALO ID
    #
    def hlt2ChargedMuonWithCaloProtos(self):
        """
        Charged protoparticles using Muon (=Muons)
        """
        return self.getProp("__hlt2ChargedMuonWithCaloProtosSeq__")
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
    # Photons from L0 Candidates
    #
    def hlt2PhotonsFromL0(self):
        """
        Neutral protoparticles 
        """
        return self.getProp("__hlt2PhotonsFromL0Seq__")
    #############################################################################################
    #
    # Electrons from L0 Candidates
    #
    def hlt2ElectronsFromL0(self):
        """
        Charged protoparticles
        """
        return self.getProp("__hlt2ElectronsFromL0Seq__")
    #############################################################################################
    #
    # Pi0 from L0 Candidates
    #
    def hlt2Pi0FromL0(self):
        """
        Neutral protoparticles
        """
        return self.getProp("__hlt2Pi0FromL0Seq__")
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
    # Forward second loop tracking
    #    
    def hlt2ForwardSecondLoopTracking(self):
        """  
        Forward tracks
        """
        return self.getProp("__hlt2ForwardSecondLoopTrackingSeq__")
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
        log.debug('############################################################')
        log.debug('## INFO You have configured an instance of the Hlt2 tracking')
        log.debug('## INFO ----------------------------------------------------')
        log.debug('## INFO The data type is '     + str(self.getProp("DataType"       )))
        log.debug('## INFO Early data tuning? '   + str(self.getProp("EarlyDataTracking")))
        log.debug('## INFO Tracks to make are '   + str(self.getProp("Hlt2Tracks"     )))
        log.debug('## INFO The prefix is '        + str(self.getProp("Prefix"         )))
        log.debug('## INFO The fit type is '      + str(self.getProp("FastFitType"    )))
        log.debug('## INFO Fast Fit? = '          + str(self.getProp("DoFastFit"      )))
        log.debug('## INFO Seeding? = '           + str(self.getProp("DoSeeding"      )))
        log.debug('## INFO Clone Killing? = '     + str(self.getProp("DoCloneKilling" )))
        log.debug('## INFO Rich hypos = '         + str(self.getProp("RichHypos"      )))
        log.debug('## INFO Rich radiators = '     + str(self.getProp("RichRadiators"  )))
        log.debug('############################################################')
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
        self.setProp( "__hlt2VeloTrackingSeq__"
                    ,  self.__hlt2VeloTracking()
                    )
        self.setProp( "__hlt2ForwardTrackingSeq__"
                    , self.__hlt2ForwardTracking()
                    )
        self.setProp( "__hlt2ForwardSecondLoopTrackingSeq__"
                    , self.__hlt2ForwardSecondLoopTracking()
                    )
        self.setProp( "__hlt2SeedTrackingSeq__"
                    , self.__hlt2SeedTracking()
                    )
        self.setProp( "__hlt2MatchTrackingSeq__"
                    , self.__hlt2MatchTracking()
                    )
        self.setProp( "__hlt2DownstreamTrackingSeq__"
                    , self.__hlt2DownstreamTracking()
                    )
        #
        # The full tracking sequence whose output is used to make the protoparticles
        # Also runs the fast fit, or inserts the error parametrization based on the track PT,
        # depending on the options given to the code.
        #
        self.setProp( "__hlt2PrepareTracksSeq__"
                    , self.__hlt2StagedFastFit()
                    )
        
        self.setProp(   "__hlt2ChargedNoPIDsProtosSeq__"    
                        , self.__hlt2ChargedNoPIDsProtos()  )
        self.setProp(   "__hlt2ChargedNoPIDsSecondLoopProtosSeq__"    
                        , self.__hlt2ChargedNoPIDsProtos(secondLoop=True)  )
        #
        # If something special is being done for the RICH then we only fill the
        # RICH sequences, to prevent the RICH and CALO clashing. In other words, we
        # only make the muon and calo protoparticles for the instances which use
        # the default settings for the RICH
        #
        if  (    self.getProp("RichHypos")         == HltRichDefaultHypos         and 
                 self.getProp("RichRadiators")     == HltRichDefaultRadiators    
            ) :
            #
            # The PID sequences
            #
            self.setProp(    "__hlt2MuonIDSeq__"            ,
                             self.__hlt2MuonID()                 )
            self.setProp(    "__hlt2CALOIDSeq__"            ,
                             self.__hlt2CALOID()                 )
            #
            # The protoparticles
            #
            self.setProp(    "__hlt2ChargedCaloProtosSeq__"        ,
                             self.__hlt2ChargedCaloProtos()        )
    
            self.setProp(    "__hlt2ChargedMuonProtosSeq__"        ,
                             self.__hlt2ChargedMuonProtos()        )

            self.setProp(    "__hlt2ChargedMuonSecondLoopProtosSeq__"        ,
                             self.__hlt2ChargedMuonProtos(secondLoop=True)        )
            
            self.setProp(    "__hlt2ChargedMuonWithCaloProtosSeq__"    ,
                             self.__hlt2ChargedMuonWithCaloProtos()  )
            self.setProp(    "__hlt2CALOIDSeq__"        ,
                             self.__hlt2CALOID()                 )
            self.setProp(    "__hlt2NeutralProtosSeq__"        ,       
                             self.__hlt2NeutralProtos()          )    
            self.setProp(    "__hlt2PhotonsFromL0Seq__"        ,       
                             self.__hlt2PhotonsFromL0()          )    
            self.setProp(    "__hlt2Pi0FromL0Seq__"            ,       
                             self.__hlt2Pi0FromL0()              )    
            self.setProp(    "__hlt2ElectronsFromL0Seq__"      ,       
                             self.__hlt2ElectronsFromL0()        )   
        #
        # The RICH needs fitted tracks!
        #
        if (self.getProp("FastFitType") in Hlt2TrackingRecognizedFitTypesForRichID) :
            #
            # Define the RICH ID and the RICH protos
            #
            self.setProp(    "__hlt2RICHIDSeq__"            ,
                             self.__hlt2RICHID()                 )
                   
            self.setProp(    "__hlt2ChargedRichProtosSeq__"        ,
                             self.__hlt2ChargedRichProtos()      )    
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
    def __shortTrackLocation(self,secondLoop=False) :
        if (self.getProp("Hlt2Tracks") not in Hlt2TrackingRecognizedTrackTypes) :
            return "Unknown"
        elif secondLoop:
            return Hlt2ForwardSecondLoopTracksName
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
    def __trackLocation(self,secondLoop=False):
        thisTrackLocation     = self.getProp("FastFitType") + "/" + \
                                self.__shortTrackLocation(secondLoop)
        return _baseTrackLocation(self.getProp("Prefix"), thisTrackLocation)
    ## For protos, the format is e.g. Hlt2/ProtoP/Unfitted/Forward/Charged #   
    def __protosLocation(self,protosType,secondLoop=False):
        thisProtosLocation = self.getProp("FastFitType") + "/" + \
                             self.__shortTrackLocation(secondLoop) + "/" + \
                             protosType
        return _baseProtoPLocation(self.getProp("Prefix"), thisProtosLocation)
    #
    # The hadron protos need subdirectories depending on the type of PID requested
    #
    def __chargedRichProtosSuffix(self):
        baseSuffix = HltRichProtosSuffix+"/"
        baseSuffix += HltRICHIDSuffix + "/" + self.__richIDSuffix()
        return baseSuffix
 
    #
    # The trackified Muon ID location
    #
    def _trackifiedMuonIDLocation(self,secondLoop=False) :
        return self.__hltBasePIDLocation(secondLoop) + "/"+ HltMuonTracksName

    #
    # The trackified AllMuon ID location
    # 
    def _trackifiedAllMuonIDLocation(self,secondLoop=False) :
        return self.__hltBasePIDLocation(secondLoop) + "/"+ HltAllMuonTracksName
    
        
    #
    # The PID objects themselves
    #
    def __hltBasePIDLocation(self,secondLoop=False) : 
        return self.__trackLocation(secondLoop) + "/" + HltSharedPIDPrefix
    #
    def __muonIDLocation(self,secondLoop=False) :
        return self.__hltBasePIDLocation(secondLoop) + "/" + HltMuonIDSuffix
    
    #
    def __richIDLocation(self) :
        return self.__hltBasePIDLocation() + "/" + HltRICHIDSuffix + "/" + self.__richIDSuffix()
    #
    def __richIDSuffix(self) :
        baseSuffix = ""
        for hypo in self.getProp("RichHypos") :
            baseSuffix += hypo
        baseSuffix += "/"
        for radiator in self.getProp("RichRadiators") :
            baseSuffix += radiator
        return baseSuffix
    #
    def __caloIDLocation(self) :
        caloBase =  self.__hltBasePIDLocation() + "/" + HltCALOIDSuffix
        return caloBase
    def __caloL0IDLocation(self) :
        caloBase =  self.__hltBasePIDLocation() + "/L0Calo" + HltCALOIDSuffix
        return caloBase
    #
    # The prefixes for the various tools and algorithms used
    #
    def __trackingAlgosAndToolsPrefix(self,secondLoop=False) :
        return self.getProp("Prefix") + self.__shortTrackLocation(secondLoop)
    #
    def __trackfitAlgosAndToolsPrefix(self,secondLoop=False) :
        return self.getProp("Prefix") + self.getProp("FastFitType") + self.__shortTrackLocation(secondLoop)
    #
    def __pidAlgosAndToolsPrefix(self) :
        return self.name()
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
    # No PIDs Protos
    #
    def __hlt2ChargedNoPIDsProtos(self,secondLoop=False) :
        """
        Charged No PID protoparticles
        """
        # Fill the sequence
        return self.__hlt2ChargedProtos(HltNoPIDSuffix,secondLoop)
    #########################################################################################
    #
    # Electron Protos
    #
    def __hlt2ChargedCaloProtos(self) :
        """
        Charged Calo protoparticles = electrons
        """
        # Fill the sequence
        myCALOProcessorChargedSeq     = self.__getCALOSeq("Charged")

        chargedProtosOutputLocation = myCALOProcessorChargedSeq.outputSelection()

        # For the charged we need a combined DLL
        from Configurables import ChargedProtoCombineDLLsAlg
        combine_name                    = self.__pidAlgosAndToolsPrefix()+"ChargedCaloProtoPCombDLLs"
        combine                         = ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation   = chargedProtosOutputLocation
        
        from HltLine.HltLine import bindMembers
        # Build the bindMembers 
        bm_name         = self.__pidAlgosAndToolsPrefix()+"ChargedCaloProtosSeq"
        bm_members      = [ myCALOProcessorChargedSeq, combine ]
        bm_output       = chargedProtosOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Muons with CALO ID added
    #
    def __hlt2ChargedMuonWithCaloProtos(self) :
        """
        Muons which have the CALO PID information filled for them
        """
        # Fill the CALO sequence first, this will make the charged protoparticles
        # and add the CALO ID to them
        myCALOProcessorChargedSeq       = self.__getCALOSeq("Charged")

        from Configurables import ChargedProtoParticleAddMuonInfo

        # Now add the muon information 
        # TODO : can I do this without overwriting?
        chargedProtosOutputLocation = myCALOProcessorChargedSeq.outputSelection()

        muon_name                       = self.__pidAlgosAndToolsPrefix()+"MwCDLL"
        muon                            = ChargedProtoParticleAddMuonInfo(muon_name)
        muon.ProtoParticleLocation      = chargedProtosOutputLocation    

        # The muon ID
        muonID                          = self.__hlt2MuonID()
        muon.InputMuonPIDLocation       = muonID.outputSelection()

        # For the charged we need a combined DLL
        from Configurables import ChargedProtoCombineDLLsAlg
        combine_name                    = self.__pidAlgosAndToolsPrefix()+"MwCCombDLLs"
        combine                         = ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation   = chargedProtosOutputLocation

        from HltLine.HltLine import bindMembers
        # Build the bindMembers 
        bm_name         = self.__pidAlgosAndToolsPrefix()+"MwCSeq"
        bm_members      = [ myCALOProcessorChargedSeq, muonID, muon, combine ]
        bm_output       = chargedProtosOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)   
 
    #########################################################################################
    #
    # Muon Protos
    #
    def __hlt2ChargedMuonProtos(self, secondLoop=False):
        """
        Charged muon protoparticles
        Requires chargedProtos and muon ID
        """
        from Configurables import ( ChargedProtoParticleAddMuonInfo,
                                    ChargedProtoCombineDLLsAlg )
       
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        chargedProtos                   = self.__hlt2ChargedProtos(HltMuonProtosSuffix, secondLoop)
        chargedProtosOutputLocation     = chargedProtos.outputSelection()
        #
        if not secondLoop :
            muon_name           = self.__pidAlgosAndToolsPrefix()+"ChargedProtoPAddMuon"
        else :
            muon_name           = self.__pidAlgosAndToolsPrefix()+"ChargedSecondLoopProtoPAddMuon"
            
        muon                = ChargedProtoParticleAddMuonInfo(muon_name)
        muon.ProtoParticleLocation  = chargedProtosOutputLocation
        # Get the MuonID from the MuonID sequence
        muonID                      = self.__hlt2MuonID(secondLoop)
        muon.InputMuonPIDLocation   = muonID.outputSelection()
    
        from HltLine.HltLine import bindMembers
        # Build the bindMembers
        
        if not secondLoop :
            bm_name         = self.__pidAlgosAndToolsPrefix()+"ChargedMuonProtosSeq" 
        else :
            bm_name         = self.__pidAlgosAndToolsPrefix()+"ChargedMuonSecondLoopProtosSeq" 

        bm_members      = [ muonID, chargedProtos, muon ]
        bm_output       = chargedProtosOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output) 
    #########################################################################################
    #
    # Charged hadron ProtoParticles
    # Does not necessarily use RICH or CALO but can do if so configured
    #
    def __hlt2ChargedRichProtos(self):
        """
        Charged hadron protoparticles = pion, kaon, proton
        If RICH is on, then requires rich (this is off by default)
        """
        from Configurables import ( ChargedProtoParticleAddRichInfo,
                                    ChargedProtoCombineDLLsAlg)
    
        #The different add PID algorithms
        #
        # The charged protoparticles and their output location
        #
        chargedProtos                   = self.__hlt2ChargedProtos(self.__chargedRichProtosSuffix())
        chargedProtosOutputLocation     = chargedProtos.outputSelection()
        #
        # Now set up the RICH sequence 
        #
        doRICHReco              = self.__hlt2RICHID()
        #
        # Add the RICH info to the DLL
        #
        richDLL_name            = self.__pidAlgosAndToolsPrefix()+"ChargedHadronProtoPAddRich"
        richDLL                 = ChargedProtoParticleAddRichInfo(richDLL_name)
        richDLL.InputRichPIDLocation    = doRICHReco.outputSelection()
        richDLL.ProtoParticleLocation   = chargedProtosOutputLocation
        #
        # What are we returning?
        #
        sequenceToReturn = []
        sequenceToReturn += [chargedProtos]
        sequenceToReturn += [doRICHReco]
        sequenceToReturn += [richDLL]
        #
        # The combined DLL is needed for cutting on later
        # 
        from Configurables import ChargedProtoCombineDLLsAlg
        combine_name                    = self.__pidAlgosAndToolsPrefix()+"RichCombDLLs"
        combine                         = ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation   = chargedProtosOutputLocation

        sequenceToReturn += [combine]

        from HltLine.HltLine import bindMembers
        # Build the bindMembers 
        bm_name         = self.__pidAlgosAndToolsPrefix()+"ChargedRichProtosSeq"
        bm_members      = sequenceToReturn
        bm_output       = chargedProtosOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Charged ProtoParticles
    #
    def __hlt2ChargedProtos(self,outputLocation,secondLoop=False):
        """
        Charged protoparticles
        
        Requires tracks, fitted if necessary
        """
        from Configurables import ChargedProtoParticleMaker
        from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        if not secondLoop :
            chargedProtosOutputLocation = self.__protosLocation(Hlt2ChargedProtoParticleSuffix) + "/" + outputLocation
            charged_name = self.__pidAlgosAndToolsPrefix()+outputLocation.replace("/","")+'ChargedProtoPAlg'
            charged      = ChargedProtoParticleMaker(charged_name)
            charged.addTool(TrackSelector,name="TrackSelector")
            ts=charged.TrackSelector
            cuts=self.getProp("TrackCuts")
            ts.setProp("MinChi2Cut",cuts['Chi2Cut'][0])
            ts.setProp("MaxChi2Cut",cuts['Chi2Cut'][1])
           
            # Need to allow for fitted tracks
            # This is now done inside the staged fast fit based on the fastFitType passed
            tracks          = self.__hlt2StagedFastFit()
            charged.Inputs  = [tracks.outputSelection()]
            charged.Output  = chargedProtosOutputLocation
            
            bm_name         = self.__pidAlgosAndToolsPrefix()+outputLocation.replace("/","")+"ChargedProtosSeq" 
            bm_members      = [ tracks , charged ]
            bm_output       = chargedProtosOutputLocation
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
        else :
            #If second loop is set needs its own sequence
            chargedProtosSecondLoopOutputLocation = self.__protosLocation(Hlt2ChargedProtoParticleSuffix,secondLoop=True) + \
                                                    "/" + outputLocation
            charged_secondLoop_name = self.__pidAlgosAndToolsPrefix()+outputLocation.replace("/","")+'ChargedSecondLoopProtoPAlg'
            charged_secondLoop      = ChargedProtoParticleMaker(charged_secondLoop_name)
            charged_secondLoop.addTool(TrackSelector,name="TrackSelector")
            ts=charged_secondLoop.TrackSelector
            cuts=self.getProp("TrackCuts")
            ts.setProp("MinChi2Cut",cuts['Chi2Cut'][0])
            ts.setProp("MaxChi2Cut",cuts['Chi2Cut'][1])
                       
            tracks_secondLoop       = self.__hlt2StagedFastFit(secondLoop=True)
            charged_secondLoop.Inputs  = [tracks_secondLoop.outputSelection()]
            charged_secondLoop.Output = chargedProtosSecondLoopOutputLocation           
 
            bm_name         = self.__pidAlgosAndToolsPrefix()+outputLocation.replace("/","")+"ChargedSecondLoopProtosSeq"
            bm_members      = [ tracks_secondLoop , charged_secondLoop ]
            bm_output       = chargedProtosSecondLoopOutputLocation
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
        myCALOProcessorNeutralSeq  = self.__getCALOSeq("Neutral")

        return myCALOProcessorNeutralSeq 
    #########################################################################################
    #
    # Photons built from L0
    #
    def __hlt2PhotonsFromL0(self):
        """
        Photons coming from L0
        """

        caloPhotonsFromL0 = self.__getNewCALOSeq('photon')
        return caloPhotonsFromL0
    #########################################################################################
    #
    # Photons built from L0
    #
    def __hlt2Pi0FromL0(self):
        """
        Pi0 coming from L0
        """

        caloPi0FromL0 = self.__getNewCALOSeq('pi0')
        return caloPi0FromL0
    #########################################################################################
    #
    # Photons built from L0
    #
    def __hlt2ElectronsFromL0(self):
        """
        Photons coming from L0
        """
        # Configure CaloElectron
        caloElectronsFromL0 = self.__getNewCALOSeq('electron')
        from Configurables import ChargedProtoCombineDLLsAlg
        combine_name                    = self.__pidAlgosAndToolsPrefix()+"LowEtChargedCaloProtoPCombDLLs"
        combine                         = ChargedProtoCombineDLLsAlg(combine_name)
        combine.ProtoParticleLocation   = caloElectronsFromL0.outputSelection()
        
        from HltLine.HltLine import bindMembers
        # Build the bindMembers 
        bm_name         = self.__pidAlgosAndToolsPrefix()+"LowEtChargedCaloProtosSeq"
        bm_members      = [caloElectronsFromL0, combine]
        bm_output       = caloElectronsFromL0.outputSelection()

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)

    #########################################################################################
    #
    # MuonID
    #
    def __hlt2MuonID(self,secondLoop=False) :
        """
        Muon ID options 
        Requires tracks
        """
   
        from MuonID import ConfiguredMuonIDs
        from Configurables import MuonRec, MuonIDAlg
        from HltLine.HltLine import bindMembers

        cm                  = ConfiguredMuonIDs.ConfiguredMuonIDs(data=self.getProp("DataType"))

        if not secondLoop :
            HltMuonIDAlg_name   = self.__pidAlgosAndToolsPrefix()+"MuonIDAlg"
            HltMuonIDAlg        = cm.configureMuonIDAlg(HltMuonIDAlg_name)
            #The tracks to use
            tracks              = self.__hlt2StagedFastFit()
            #Enforce naming conventions
            HltMuonIDAlg.TrackLocation          = tracks.outputSelection() 
            HltMuonIDAlg.MuonIDLocation         = self.__muonIDLocation() #output 
            HltMuonIDAlg.MuonTrackLocation      = self._trackifiedMuonIDLocation() 
            HltMuonIDAlg.MuonTrackLocationAll   = self._trackifiedAllMuonIDLocation() 
            # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
            #       a factor 3-4
            HltMuonIDAlg.FindQuality            = False
        
            # Build the bindMembers        
            bm_name         = self.__pidAlgosAndToolsPrefix()+"MuonIDSeq"
            bm_members      = [ tracks, MuonRec(), HltMuonIDAlg ]
            bm_output       = HltMuonIDAlg.MuonIDLocation
            
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
        else :
            HltMuonIDAlg_secondLoop_name   = self.__pidAlgosAndToolsPrefix()+"MuonSecondLoopIDAlg"
            HltMuonIDAlg_secondLoop        = cm.configureMuonIDAlg(HltMuonIDAlg_secondLoop_name)
            #The tracks to use
            tracks_secondLoop           = self.__hlt2StagedFastFit(secondLoop=True)
            #Enforce naming conventions
            HltMuonIDAlg_secondLoop.TrackLocation          = tracks_secondLoop.outputSelection() 
            HltMuonIDAlg_secondLoop.MuonIDLocation         = self.__muonIDLocation(secondLoop=True) #output 
            HltMuonIDAlg_secondLoop.MuonTrackLocation      = self._trackifiedMuonIDLocation(secondLoop=True) 
            HltMuonIDAlg_secondLoop.MuonTrackLocationAll   = self._trackifiedAllMuonIDLocation(secondLoop=True) 
            # CRJ : Disable FindQuality in HLT since it increases CPU time for MuonID by
            #       a factor 3-4
            HltMuonIDAlg_secondLoop.FindQuality            = False
            
            # Build the bindMembers        
            bm_name         = self.__pidAlgosAndToolsPrefix()+"MuonSecondLoopIDSeq"
            bm_members      = [ tracks_secondLoop, MuonRec(), HltMuonIDAlg_secondLoop ]
            bm_output       = HltMuonIDAlg_secondLoop.MuonIDLocation
        
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
        
    #########################################################################################
    #
    # Calo ID
    #
    def __hlt2CALOID(self) :
        """
        Calo ID
        Does calo reconstruction and Calo ID
        """
        # the sequence to run
        myCALOProcessorPIDSeq   = self.__getCALOSeq("PID")

        return myCALOProcessorPIDSeq 
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
        richSeqName         = self._instanceName(RichRecSysConf) 
        richSeq             = GaudiSequencer(richSeqName+"Seq")
        richSeq.MeasureTime = True
        # The RICH COnfigurable 
        # note, the context MUST contain the string HLT
        # in whatever case
        richConf            = RichRecSysConf(richSeqName)
        richConf.DataType   = self.getProp( "DataType" )
        richConf.Context    = richSeqName
        richConf.RichPIDLocation    = self.__richIDLocation()
        richConf.PidConfig          = "FastGlobal"
        richConf.CheckProcStatus    = False
        richConf.InitPixels         = True
        richConf.InitTracks         = True
        richConf.InitPhotons        = True
        richConf.TracklessRingAlgs  = []
        richConf.Particles          = self.getProp("RichHypos")
        # Set cuts on which tracks enter the RICH reco
        richConf.trackConfig().TrackCuts = self.getProp("RichTrackCuts")
        richConf.gpidConfig().TrackCuts  = self.getProp("RichTrackCuts") 
        richConf.Radiators          = self.getProp("RichRadiators")
        # Set the sequence to run the RICH PID in
        richConf.setProp("RecoSequencer",richSeq)
        # The input location of the tracks
        tracks                      = self.__hlt2StagedFastFit()
        richConf.trackConfig().InputTracksLocation = tracks.outputSelection()

        from HltLine.HltLine import bindMembers
        # Build the bindMembers        
        bm_name         = self.__pidAlgosAndToolsPrefix()+"RICHIDSeq"
        bm_members      = [tracks, richSeq]
        bm_output       = richConf.RichPIDLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Staged fast fit
    #
    def __hlt2StagedFastFit(self,secondLoop=False) :
        """
        The staged fast-fit. Currently set to
        be bidirectional and with smoothing to allow PID information
        to be used; special cases need to be justified separately
        """
        from HltLine.HltLine import bindMembers

        #Make the original tracks in case this has not been run already
        tracks                  = self.__hlt2Tracking()
        if secondLoop :
            tracks = self.__hlt2ForwardSecondLoopTracking() 
        #And my output location is?
        hlt2StagedFastFitOutputLocation = self.__trackLocation(secondLoop)
        # Second check: have I actually been asked to fit the tracks?
        # If not, just return the unfitted tracks copied into the container specified by
        # the fast-fit type field
        if (not self.getProp("DoFastFit")) :
            from Configurables import CreateFastTrackCollection
            #
            # Hacking of errors
            #
            # Do not insert errors for downstream tacks as these would be meaningless
            bm_members      = [tracks]
            if (self.getProp("Hlt2Tracks") <> Hlt2DownstreamTracksName) :
                from Configurables import HltInsertTrackErrParam
                HltInsertTrackErrParam_name                 =  self.__trackfitAlgosAndToolsPrefix(secondLoop)+"InsertTrackErrParam"
                HltInsertTrackErrParam                      =  HltInsertTrackErrParam(HltInsertTrackErrParam_name)
                HltInsertTrackErrParam.InputLocation        =  tracks.outputSelection()
                bm_members                                  += [HltInsertTrackErrParam]
            #### CreateFastTrackCollection
            # this can only be done ONCE per long track output
            recoCopy = CreateFastTrackCollection( self.__trackfitAlgosAndToolsPrefix(secondLoop)+'TrackCopyNF'
                                                , InputLocations = [tracks.outputSelection()]
                                                , OutputLocation = hlt2StagedFastFitOutputLocation
                                                , SlowContainer  = True)
            # Build the bindMembers        
            bm_name         = self.__trackfitAlgosAndToolsPrefix(secondLoop)+"NoFastFitSeq" 
            bm_members      = bm_members + [recoCopy]
            bm_output       = hlt2StagedFastFitOutputLocation

            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    
        from Configurables import TrackEventFitter, TrackMasterFitter
        Hlt2StagedFastFit_name      = self.__trackfitAlgosAndToolsPrefix(secondLoop)+'StagedFastFit'
        Hlt2StagedFastFit           = TrackEventFitter(Hlt2StagedFastFit_name)
        Hlt2StagedFastFit.TracksInContainer    = tracks.outputSelection() 
        Hlt2StagedFastFit.TracksOutContainer    = hlt2StagedFastFitOutputLocation  
        Hlt2StagedFastFit.addTool(TrackMasterFitter, name = 'Fitter')
        from TrackFitter.ConfiguredFitters import ConfiguredHltFitter
        fitter = ConfiguredHltFitter( getattr(Hlt2StagedFastFit,'Fitter'))
 
        # Build the bindMembers        
        bm_name         = self.__trackfitAlgosAndToolsPrefix(secondLoop)+"FastFitSeq"
        bm_members      = [tracks, Hlt2StagedFastFit]
        bm_output       = hlt2StagedFastFitOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Track reconstruction
    # Add an option to have a second loop from the forward tracking
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
        hlt2TrackingOutput      = _baseTrackLocation(self.getProp("Prefix"),self.__shortTrackLocation())
        
        # Finally make the sequence
        # The sequence called depends on the track type, so far we recognise two track types
        # for Long tracks the options doSeeding, doCloneKilling, etc. are meaningful
        # for SeedTT tracks the seeding is mandatory and the CloneKilling is irrelevant 
        # This part gets done in either case
        trackRecoSequence = []
        if (self.__shortTrackLocation() == Hlt2LongTracksName) :
            # Do the forward, seeding + matching
            trackRecoSequence        =    [self.__hlt2ForwardTracking()]
            trackRecoSequence       +=    [self.__hlt2MatchTracking()]
            # The copy sequence, which merges the forward and
            # match tracks into the long track container
            hlt2TracksToMergeIntoLong    =    []
            hlt2TracksToMergeIntoLong   +=    [ self.__hlt2ForwardTracking().outputSelection()]
            hlt2TracksToMergeIntoLong   +=    [ self.__hlt2MatchTracking().outputSelection()  ]
            # Do the clone killing if required 
            if self.getProp("DoCloneKilling") :
               
                from Configurables import TrackEventCloneKiller, TrackCloneFinder
                
                cloneKiller = TrackEventCloneKiller( self.__trackingAlgosAndToolsPrefix()+'FastCloneKiller'
                                                     , TracksInContainers         = hlt2TracksToMergeIntoLong
                                                     , TracksOutContainer         = hlt2TrackingOutput
                                                     , SkipSameContainerTracks    = False
                                                     , CopyTracks                 = True)
                cloneKiller.addTool(TrackCloneFinder, name = 'CloneFinderTool')
                cloneKiller.CloneFinderTool.RestrictedSearch = True
                
                trackRecoSequence        +=      [cloneKiller]
            else :
                from Configurables import CreateFastTrackCollection
                recoCopy = CreateFastTrackCollection(self.__trackingAlgosAndToolsPrefix()+'TrackCopy'
                                                     , InputLocations = hlt2TracksToMergeIntoLong
                                                     , OutputLocation = hlt2TrackingOutput
                                                     , SlowContainer  = True) 
                trackRecoSequence        +=     [recoCopy]

        elif (self.__shortTrackLocation() == Hlt2ForwardTracksName) :
            trackRecoSequence        =     [self.__hlt2ForwardTracking()]
        elif (self.__shortTrackLocation() == Hlt2DownstreamTracksName ) :
            trackRecoSequence         =    [self.__hlt2DownstreamTracking()]
  
        # Build the bindMembers        
        bm_name         = self.__trackingAlgosAndToolsPrefix()+"TrackingSeq"
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
        from Configurables      import Tf__PatVeloGeneralTracking
        from Configurables      import Tf__PatVeloSpaceTool, FastVeloTracking
        #From HltReco we just get the shared stuff between Hlt1 and Hlt2
        from HltReco        import MinimalVelo
        from HltLine.HltLine    import bindMembers 
        

        FastVelo = True
        veloTracksOutputLocation = _baseTrackLocation(HltSharedTracksPrefix,Hlt2VeloTracksName) 

        if self.getProp("EarlyDataTracking") :
            # Do something special in case of early data
            # For the moment just a dummy setting
            dummy = 0
   
        # Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"VeloTracking"
        if FastVelo:
            recoVeloExtra = FastVeloTracking( 'FastVeloHlt2', OutputTracksName = veloTracksOutputLocation )
            
            recoVeloExtra.HLT2Complement = True
            bm_members      = MinimalVelo.members() + [recoVeloExtra]
        else:
            recoVeloGeneral         = Tf__PatVeloGeneralTracking(self.getProp("Prefix")+'RecoVeloGeneral'
                                                                  , OutputTracksLocation = veloTracksOutputLocation )
            
            bm_members      = MinimalVelo.members() + [recoVeloGeneral]
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
        from Configurables    import PatForward
        from Configurables      import PatForwardTool
        from HltLine.HltLine    import bindMembers
        from HltTracking.HltReco import MaxOTHits
        #        from Hlt1Lines.HltConfigurePR import ConfiguredPR
        
        forwardTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2ForwardTracksName) 
    
        recoForward = PatForward( self.getProp("Prefix")+'RecoForward'
                                , InputTracksName  = self.__hlt2VeloTracking().outputSelection() 
                                , OutputTracksName = forwardTrackOutputLocation 
                                , maxOTHits = MaxOTHits
                                )

        if self.getProp('Hlt2ForwardMaxVelo') > 0 :
            recoForward.MaxNVelo = self.getProp('Hlt2ForwardMaxVelo')
        #JA: TODO: put something in like: if(early data):
        #recoForward.addTool(ConfiguredPR( "Forward" ))

        #        PatForwardTool( MinMomentum = 1000., MinPt = 1000., AddTTClusterName = "" )
        from HltTracking.HltReco import CommonForwardTrackingOptions
        recoForward.addTool(PatForwardTool, name='PatForwardTool')
        recoForward.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
        recoForward.PatForwardTool.SecondLoop = False
        recoForward.PatForwardTool.MaxChi2 = CommonForwardTrackingOptions["MaxChi2"]
        recoForward.PatForwardTool.MaxChi2Track = CommonForwardTrackingOptions["MaxChi2Track"]
        recoForward.PatForwardTool.MinHits = CommonForwardTrackingOptions["MinHits"]
        recoForward.PatForwardTool.MinOTHits = CommonForwardTrackingOptions["MinOTHits"]
        recoForward.PatForwardTool.MinMomentum = 3000
        recoForward.PatForwardTool.MinPt = 300
     
        if self.getProp("EarlyDataTracking") :
            from HltTracking.HltReco import CommonForwardTrackingOptions_EarlyData 
            # configure pattern reco with early data flags
            # first forward algorithm
            recoForward.PatForwardTool.MinXPlanes = CommonForwardTrackingOptions_EarlyData["MinXPlanes"]
            recoForward.PatForwardTool.MinPlanes = CommonForwardTrackingOptions_EarlyData["MinPlanes"]
            recoForward.PatForwardTool.MaxSpreadX = CommonForwardTrackingOptions_EarlyData["MaxSpreadX"]
            recoForward.PatForwardTool.MaxSpreadY = CommonForwardTrackingOptions_EarlyData["MaxSpreadY"]
            recoForward.PatForwardTool.MaxChi2 = CommonForwardTrackingOptions_EarlyData["MaxChi2"]
            recoForward.PatForwardTool.MaxChi2Track = CommonForwardTrackingOptions_EarlyData["MaxChi2Track"]
            recoForward.PatForwardTool.MinHits = CommonForwardTrackingOptions_EarlyData["MinHits"]
            recoForward.PatForwardTool.MinOTHits = CommonForwardTrackingOptions_EarlyData["MinOTHits"]

        # Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"ForwardTracking"
        bm_members      = self.__hlt2VeloTracking().members() + self.__hlt2TrackerDecoding().members() + [recoForward]
        bm_output       = forwardTrackOutputLocation

        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #    
    # Hlt2 forward tracking 2nd loop
    # Used for recovery of low momentum tracks not picked up by the first loop 
    #
    def __hlt2ForwardSecondLoopTracking(self) :
        """
        Forward track reconstruction for Hlt2 for recovery of lower momentum tracks
        limit is 300/3000 MeV (PT/P) as opposed to (500/5000) for first loop
        """
        from Configurables    import PatForward
        from Configurables      import PatForwardTool
        from HltLine.HltLine    import bindMembers

        forwardTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2ForwardSecondLoopTracksName)

        from HltTracking.HltReco import MaxOTHits
        recoForwardSecondLoop      = PatForward( self.getProp("Prefix")+'RecoForwardSecondLoop'
                                                , InputTracksName  = self.__hlt2VeloTracking().outputSelection()
                                                , maxOTHits = MaxOTHits
                                                , UnusedVeloSeeds  = True
                                                , VeloVetoTracksName   = self.__hlt2ForwardTracking().outputSelection()
                                                , OutputTracksName = forwardTrackOutputLocation )

        if self.getProp('Hlt2ForwardMaxVelo') > 0 :
            recoForwardSecondLoop.MaxNVelo = self.getProp('Hlt2ForwardMaxVelo')

        from HltTracking.HltReco import CommonForwardTrackingOptions
        recoForwardSecondLoop.addTool(PatForwardTool, name='PatForwardTool')
        recoForwardSecondLoop.PatForwardTool.AddTTClusterName = "PatAddTTCoord"
        recoForwardSecondLoop.PatForwardTool.SecondLoop     = True
        recoForwardSecondLoop.PatForwardTool.MaxChi2        = CommonForwardTrackingOptions["MaxChi2"]
        recoForwardSecondLoop.PatForwardTool.MaxChi2Track   = CommonForwardTrackingOptions["MaxChi2Track"]
        recoForwardSecondLoop.PatForwardTool.MinHits        = CommonForwardTrackingOptions["MinHits"]
        recoForwardSecondLoop.PatForwardTool.MinOTHits      = CommonForwardTrackingOptions["MinOTHits"]
        recoForwardSecondLoop.PatForwardTool.MinMomentum    = 1000
        recoForwardSecondLoop.PatForwardTool.MinPt          = 150

        # Build the bindMembers       
        # Explicitly require the regular forward tracking here 
        bm_name         = self.getProp("Prefix")+"ForwardSecondLoopTracking"
        bm_members      = self.__hlt2VeloTracking().members()    + \
                          self.__hlt2TrackerDecoding().members() + \
                          self.__hlt2ForwardTracking().members() + \
                          [recoForwardSecondLoop]
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
        from Configurables    import PatSeeding
        from Configurables      import PatSeedingTool
        from HltLine.HltLine    import bindMembers

        # We depend on the forward tracking
        fwdtracks = self.__hlt2ForwardTracking()
        # Now our output location 
        seedTrackOutputLocation    = _baseTrackLocation(self.getProp("Prefix"),Hlt2SeedingTracksName)
    
        #### Seeding 
        recoSeeding = PatSeeding(self.getProp("Prefix")+'Seeding', OutputTracksName = seedTrackOutputLocation)
        recoSeeding.addTool(PatSeedingTool, name="PatSeedingTool")

        #JA: TODO: put something in like: if(early data):
        #recoSeeding.addTool(ConfiguredPR( "PatSeeding" ))

        recoSeeding.PatSeedingTool.UseForward        = True
        recoSeeding.PatSeedingTool.ForwardCloneMergeSeg = True
        recoSeeding.PatSeedingTool.NDblOTHitsInXSearch = 2
        recoSeeding.PatSeedingTool.InputTracksName    = fwdtracks.outputSelection()
        recoSeeding.PatSeedingTool.MinMomentum = 3000 
        from HltTracking.HltReco import MaxOTHits
        recoSeeding.PatSeedingTool.MaxOTHits = MaxOTHits
 
        if self.getProp("EarlyDataTracking") :
            # Do something special in case of early data
            from HltTracking.HltReco import CommonSeedingTrackingOptions_EarlyData
            recoSeeding.PatSeedingTool.OTNHitsLowThresh = CommonSeedingTrackingOptions_EarlyData["OTNHitsLowThresh"]
            recoSeeding.PatSeedingTool.MinTotalPlanes = CommonSeedingTrackingOptions_EarlyData["MinTotalPlanes"]
            recoSeeding.PatSeedingTool.MaxMisses = CommonSeedingTrackingOptions_EarlyData["MaxMisses"]
            recoSeeding.PatSeedingTool.MaxTrackChi2LowMult = CommonSeedingTrackingOptions_EarlyData["MaxTrackChi2LowMult"]
            recoSeeding.PatSeedingTool.MaxFinalTrackChi2 = CommonSeedingTrackingOptions_EarlyData["MaxFinalTrackChi2"]
            recoSeeding.PatSeedingTool.MaxFinalChi2 = CommonSeedingTrackingOptions_EarlyData["MaxFinalChi2"]
            recoSeeding.PatSeedingTool.MaxTrackChi2 = CommonSeedingTrackingOptions_EarlyData["MaxTrackChi2"]
            recoSeeding.PatSeedingTool.MaxChi2HitIT = CommonSeedingTrackingOptions_EarlyData["MaxChi2HitIT"]
            recoSeeding.PatSeedingTool.MaxChi2HitOT = CommonSeedingTrackingOptions_EarlyData["MaxChi2HitOT"]
         
        # Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"SeedTracking" 
        bm_members      = self.__hlt2VeloTracking().members() + self.__hlt2TrackerDecoding().members() + \
                          self.__hlt2ForwardTracking().members() + [recoSeeding]
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
        from Configurables    import PatMatch
        from HltLine.HltLine    import bindMembers
    
        matchTrackOutputLocation = _baseTrackLocation(self.getProp("Prefix"),Hlt2MatchTracksName)
 
        #### Matching
        recoMatch         = PatMatch(self.getProp("Prefix")+'Match'
                                 , VeloInput = self.__hlt2VeloTracking().outputSelection()
                                 , SeedInput = self.__hlt2SeedTracking().outputSelection()
                                 , MatchOutput = matchTrackOutputLocation)
        from Configurables   import PatMatchTool
        recoMatch.addTool(PatMatchTool, name="PatMatchTool")
        recoMatch.PatMatchTool.MinMomentum = 3000
        recoMatch.PatMatchTool.MinPt = 300
   
        if self.getProp("EarlyDataTracking") :
            # Do something special in case of early data
            # For the moment just a dummy setting
            dummy = 0

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
        from Configurables    import PatDownstream
        from HltLine.HltLine    import bindMembers
    
        downstreamTrackOutputLocation    = _baseTrackLocation(self.getProp("Prefix"),Hlt2DownstreamTracksName)

        fwdtracks   = self.__hlt2ForwardTracking()    
        matchtracks = self.__hlt2MatchTracking()

        ### Downstream tracking
        PatDownstream                 = PatDownstream(self.getProp("Prefix")+'PatDownstream')
        PatDownstream.InputLocation   = self.__hlt2SeedTracking().outputSelection()
        PatDownstream.OutputLocation  = downstreamTrackOutputLocation
        PatDownstream.ForwardLocation = fwdtracks.outputSelection()
        PatDownstream.MatchLocation   = matchtracks.outputSelection()
        #Set to true to remove used seeds and tt hits
        PatDownstream.RemoveUsed     = False
        PatDownstream.RemoveAll      = True
        PatDownstream.MinMomentum = 0
        PatDownstream.MinPt = 0
  
        if self.getProp("EarlyDataTracking") :
            # Do something special in case of early data
            from HltTracking.HltReco import CommonDownstreamTrackingOptions_EarlyData 
            PatDownstream.xPredTol2     = CommonDownstreamTrackingOptions_EarlyData["xPredTol2"]
            PatDownstream.TolMatch      = CommonDownstreamTrackingOptions_EarlyData["TolMatch"]
            PatDownstream.TolUV         = CommonDownstreamTrackingOptions_EarlyData["TolUV"]
            PatDownstream.maxWindowSize = CommonDownstreamTrackingOptions_EarlyData["maxWindowSize"]
            PatDownstream.MaxChisq      = CommonDownstreamTrackingOptions_EarlyData["MaxChisq"]
            PatDownstream.MaxDistance   = CommonDownstreamTrackingOptions_EarlyData["MaxDistance"]
            PatDownstream.deltaP        = CommonDownstreamTrackingOptions_EarlyData["deltaP"]
            PatDownstream.errorZMagnet  = CommonDownstreamTrackingOptions_EarlyData["errorZMagnet"]
 
        # Build the bindMembers        
        bm_name         = self.getProp("Prefix")+"DownstreamTracking" 
        bm_members      = self.__hlt2MatchTracking().members() + [PatDownstream]
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
        from HltLine.HltDecodeRaw     import DecodeTT, DecodeIT
        from HltLine.HltLine        import bindMembers
        return bindMembers(self.getProp("Prefix")+"DecodeSTSeq", DecodeTT.members() + DecodeIT.members())
    #########################################################################################
    #
    # Helper function to set up the CALO processor and return the correct sequence 
    #
    def __getNewCALOSeq(self, mode):
        from HltLine.HltLine    import bindMembers      
        # Load tracks
        tracks        = self.__hlt2StagedFastFit()

        #neutralProtosLowEtOutputLocation = self.__protosLocation(Hlt2LowEtNeutralProtoParticleSuffix)
        #neutralProtosHighEtOutputLocation = self.__protosLocation(Hlt2HighEtNeutralProtoParticleSuffix)
 
        #outputCALOPID            = self.__caloIDLocation()
        caloName = "HLT2CaloLines"+ mode.capitalize() + self.__shortTrackLocation()
        # Create configurable
        from Configurables import CaloLines
        caloLines = CaloLines(caloName)
        bm_name    = self.__pidAlgosAndToolsPrefix() + mode.capitalize() + self.__shortTrackLocation()
        bm_members = [tracks]
        bm_output = ''
        if mode.lower() == 'photon':
            caloLines.HighPhoton = True
            caloLines.LowPhoton = False
            caloLines.LowElectron = False
            bm_name  += 'PhotonsFromL0'
            bm_output = '/Event/'+caloName+'HighPhoton/ProtoP/Neutrals'
        elif mode.lower() == 'electron':
            caloLines.HighPhoton = False
            caloLines.LowPhoton = False
            caloLines.LowElectron = True
            from Configurables import ChargedProtoParticleMaker
            bm_name  += 'ElectronsFromL0'
            bm_output = '/Event/'+caloName+'LowElectron/ProtoP/Charged'
            chargedName = self.__pidAlgosAndToolsPrefix()+bm_output.replace("/","")+'ChargedProtoPAlg'
            charged     = ChargedProtoParticleMaker(chargedName)
            charged.addTool(TrackSelector,name="TrackSelector")
            ts=charged.TrackSelector
            cuts=self.getProp("TrackCuts")
            ts.setProp("MinChi2Cut",cuts['Chi2Cut'][0])
            ts.setProp("MaxChi2Cut",cuts['Chi2Cut'][1])
           
            charged.Inputs = [tracks.outputSelection()]
            charged.Output = bm_output
            bm_members += [charged]
        elif mode.lower() == 'pi0':
            caloLines.HighPhoton = False
            caloLines.LowPhoton = True
            caloLines.LowElectron = False
            bm_name  += 'Pi0FromL0'
            bm_output = '/Event/'+caloName+'LowPhoton/ProtoP/Neutrals'
        seq = caloLines.sequence(tracks=[tracks.outputSelection()])
        bm_members += [seq]
        return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    #########################################################################################
    #
    # Helper function to set up the CALO processor and return the correct sequence 
    #
    def __getCALOSeq(self,mode):
        """
        Defines the CALO processor and, depending on the mode,
        returns the PID, charged proto, or neutral proto sequences 
        """
        tracks                      = self.__hlt2StagedFastFit()
        chargedProtos               = self.__hlt2ChargedProtos(HltCaloProtosSuffix)       
        chargedProtosOutputLocation = chargedProtos.outputSelection()
        neutralProtosOutputLocation = self.__protosLocation(Hlt2NeutralProtoParticleSuffix)
 
        outputCALOPID            = self.__caloIDLocation()
        
        from Configurables    import CaloProcessor
        from HltLine.HltLine    import bindMembers

        caloProcessorName         = self._instanceName(CaloProcessor)   
 
        myCALOProcessor         = CaloProcessor(caloProcessorName) 
        myCALOProcessor.Context     = outputCALOPID
        # Do the reconstruction and the PID but do not make or update the
        # protoparticles here! 
        myCALOProcessor.CaloReco     = True 
        myCALOProcessor.CaloPIDs     = True
        #
        # Check if we are making neutrals or not
        #
        myCALOProcessor.SkipNeutrals    = False
        myCALOProcessor.SkipCharged     = False

        # The sequences are given the track and protoparticle locations when initializing 
        myPIDSeq         = myCALOProcessor.caloSequence(        [tracks.outputSelection()]                    )
        myChargedSeq     = myCALOProcessor.chargedProtoSequence(    [tracks.outputSelection()],    chargedProtosOutputLocation    )
        myNeutralSeq     = myCALOProcessor.neutralProtoSequence(    [tracks.outputSelection()],    neutralProtosOutputLocation    )

        bm_name = self.__pidAlgosAndToolsPrefix()
        if ( mode == "PID" ) :
            bm_name         += "CALOPIDSeq"
            bm_members      = [tracks, myPIDSeq]
            bm_output       = outputCALOPID
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    
        if ( mode == "Neutral" ) :
            bm_name         += "CaloNeutralSeq"
            bm_members      = [tracks, myPIDSeq, myNeutralSeq]
            bm_output       = myCALOProcessor.NeutralProtoLocation
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
    
        if ( mode == "Charged" ) :
            bm_name         += "CaloChargedSeq"
            bm_members      = [tracks, myPIDSeq, chargedProtos, myChargedSeq]
            bm_output       = chargedProtosOutputLocation
            return bindMembers(bm_name, bm_members).setOutputSelection(bm_output)
