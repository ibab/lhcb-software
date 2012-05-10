#!/usr/bin/env gaudirun.py
# =============================================================================
## Confurable for Calorimeter Reconstruction
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
Configurable for Calorimeter Reconstruction
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.16 $"
# =============================================================================
__all__ = (
    'HltCaloRecoConf'     ,
    'OffLineCaloRecoConf' ,
    'CaloRecoConf',
    'CaloProcessor',
    'CaloLines'
    )
# =============================================================================

from LHCbKernel.Configuration import *


from GaudiKernel.SystemOfUnits import MeV, GeV
from CaloKernel.ConfUtils     import ( addAlgs        ,
                                       printOnDemand  ,
                                       prntCmp        ,
                                       hltContext     , 
                                       setTheProperty ,
                                       getAlgo
                                       )
from Reconstruction           import ( clusterReco    , 
                                       photonReco     ,
                                       electronReco   ,
                                       mergedPi0Reco  ) 

from Configurables            import CaloDigitConf
from Configurables            import CaloPIDsConf
##from Configurables            import GlobalRecoConf
from Configurables            import GaudiSequencer


import logging
_log = logging.getLogger ('CaloReco')

# =============================================================================
## @class CaloRecoConf
#  Configurable for Calorimeter Reconstruction
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class CaloRecoConf(LHCbConfigurableUser):
    """
    Class/Configurable to define the calorimeter reconstruction

    """
   ## define the slots
    __slots__ = {
        ##
        "Context"              : ''   # The context to run (default = offline)
        , "MeasureTime"        : True        # Measure the time for sequencers
        , "OutputLevel"        : INFO        # The global output level
        ##
        , 'Sequence'           : None        # The sequencer to add the CALO reconstruction algorithms to
        , 'RecList'            : [ 'Digits'       ,
                                   'Clusters'     ,
                                   'Photons'      ,
                                   'MergedPi0s'   ,
                                   'SplitPhotons' , # the same action as 'MergedPi0s'
                                   'Electrons'    ] ##
        , 'ForceDigits'         : True       # Force digits recontruction to be run with Clusters
        , 'CreateADCs'          : False      # Create Calo-ADCs
        , 'UseTracks'           : True       # Use Tracks as Neutrality Criteria 
        , 'UseSpd'              : False      # Use Spd as Neutrality Criteria
        , 'UsePrs'              : False      # Use Prs to select Neutral clusters
        , 'UseTracksE'          : True       # Use Tracks as Charge  Criteria 
        , 'UseSpdE'             : True       # Use Spd as Charged Criteria
        , 'UsePrsE'             : True       # Use Prs to select Charged clusters
        , 'UseTracksM'          : True       # Use Tracks for MergedPi0s-ID
        , 'CaloStandalone'      : False      # useTrackX = false + usePrs/Spd = true
        , 'NeutralID'           : False      # Apply neutralID (CaloPIDs is in charge + problems with onDemand to be fixed)
        , 'EnableRecoOnDemand'  : False      # Enable Reco-On-Demand (only for components on RecList)
        , 'TrackLocations'      : []         # Track locations (Neutral/Charged cluster selection with UseTrack(E) )
        , 'ExternalClusters'    : ''         # Start the reconstruction sequence from an external cluster container (HLT)
        , 'FastReco'            : False      # faster reconstruction (HLT)
        , 'SkipNeutrals'        : False
        , 'SkipCharged'         : False
        , 'ForceOnDemand'       : False      # force DoD for ALL components (incl. those not in RecList)
        , 'PhotonPt'            : 150.*MeV
        , 'ElectronPt'          : 150.*MeV
        , 'MergedPi0Pt'         : 2.*GeV
        , 'ClusterPt'           : 0.
        , 'MakeExternalClustersWithTag': ''
        ##
        }
    ## documentation lines 
    _propertyDocDct = {
        ##
        "Context"              : """ The context to run """
        , "MeasureTime"        : """ Measure the time for sequencers """ 
        , "OutputLevel"        : """ The global output level """ 
        ##
        , 'Sequence'           : """ The sequencer to add the CALO reconstruction algorithms to """
        , 'RecList'            : """ The recontruction sketch """
        , 'ForceDigits'        : """ Force digits recontruction to be run with Clusters """ 
        , 'CreateADCs'         : """ Create Calo-ADCs """ 
        , 'UseTracks'          : """ Use Tracks as Neutrality criterion """ 
        , 'UseSpd'             : """ Use Spd as Neutrality criterion """ 
        , 'UsePrs'             : """ Use Prs as  EM criterion for neutrals """ 
        , 'UseTracksE'         : """ Use Tracks as Charge criterion """ 
        , 'UseSpdE'            : """ Use Spd as Charge criterion """ 
        , 'UsePrsE'            : """ Use Prs as  EM criterion for charged """ 
        , 'UseTracksM'         : """ Use Tracks for MergedPi0s-ID """
        , 'CaloStandalone'     : """ UseTrack = false + usePrs/Spd = true """ 
        , 'NeutralID'          : """ Apply neutralID """ 
        , 'EnableRecoOnDemand' : """ Enable Reco-On-Demand (for components in RecList) """ 
        , 'TrackLocations'     : """ TrackLocations (Photon/Electron selection)""" 
        , 'SkipNeutrals'       : """ Skip Neutral reco components in RecList""" 
        , 'SkipCharged'        : """ Skip Charged reco components in RecList"""
        , 'ForceOnDemand'      : """ force DoD for ALL components"""
        , 'FastReco'           : """ Speed-up reconstruction (empty so far)"""
        , 'ExternalClusters'   : """ Start the reconstruction sequence from an external cluster container (HLT)"""
        , 'PhotonPt'           : """ Photon Hypo Pt cut """ 
        , 'ElectronPt'         : """ Electron Hypo Pt cut """
        , 'MergedPi0Pt'        : """ MergedPi0 Pt cut """
        , 'ClusterPt'          : """ Cluster Pt """
        , 'MakeExternalClustersWithTag'   :""" Build tagged external clusters in the sequence (only if ExternalClusters != '')"""
        ##
    }
    
    ## used configurables 
    __used_configurables__ = (
        (CaloDigitConf,None) ,
        )
    
    ## configure processing of Digits
    def digits   ( self ) :
        """
        Configure processing of Digits
        
        """
        from CaloDAQ.CaloDigits import caloDigits
       
	myCaloDigitConf = CaloDigitConf(self._instanceName(CaloDigitConf))
	myCaloDigitConf.Context 		= "Offline"
	myCaloDigitConf.OutputLevel 		= self.getProp ('OutputLevel'       )
	myCaloDigitConf.EnableDigitsOnDemand 	= self.getProp ('EnableRecoOnDemand')
	myCaloDigitConf.CreateADCs		= self.getProp ('CreateADCs'        ) 

        return caloDigits (
            'Offline'                        ,  
            #self.getProp ('Context'           ) ,
            self.getProp ('EnableRecoOnDemand') ,
            self.getProp ('CreateADCs'        ) 
            )
    
    ## Configure reconstruction of Ecal Clusters
    def clusters ( self ) :
        """
        Configure reconstruction of Ecal Clusters
        """
        cmp = clusterReco   ( self.getProp('Context')             ,
                              self.getProp('EnableRecoOnDemand' ) ,
                              self.getProp('ClusterPt')           ,
                              self.getProp('FastReco')            ,
                              self.getProp('ExternalClusters')    ,
                              self.getProp('MakeExternalClustersWithTag')
                             )
        
        if self.getProp ( 'ForceDigits' ) :
            ## insert digits into Cluster Sequence
            cmp.Members = [ self.digits() ] + cmp.Members 
            
        _log.info ('Configured Ecal Clusters  Reco : %s ' % cmp.name()  )
        ##
        return cmp
    
    ## reconstruct the photons
    def photons  ( self ) :
        """
        Define the reconstruction of Photons
        """

        ## confuse configurable on purpose 
        _locs = self.getProp ( 'TrackLocations'    )
        _elocs = []
        for l in _locs :
            if l.find( '/Event/' )  != 0 :
                l = '/Event/' + l
            _elocs.append( l )


        uTracks = self.getProp ( 'UseTracks'      ) 
        uSpd = self.getProp ( 'UseSpd'            ) 
        uPrs = self.getProp ( 'UsePrs'            ) 
        if self.getProp('CaloStandalone') :
            uTracks = False
            uSpd    = True
            uPrs    = True
            
        cmp = photonReco   ( self.getProp ( 'Context'           ) ,
                             self.getProp ( 'EnableRecoOnDemand') ,
                             uTracks,
                             uSpd,
                             uPrs,
                             _elocs,
                             self.getProp ( 'NeutralID' ) ,
                             self.getProp('PhotonPt')            ,
                             self.getProp('FastReco')            ,
                             self.getProp('ExternalClusters')     
                             )
        _log.info ('Configured Single Photons Reco : %s ' % cmp.name()  )
        ##
        return cmp

    ## Configure recontruction of Electrons
    def electrons ( self ) :
        """
        Configure recontruction of Electrons
        """

        ## confuse configurable on purpose 
        _locs = self.getProp ( 'TrackLocations'    )
        _elocs = []
        for l in _locs :
            if l.find( '/Event/' )  != 0 :
                l = '/Event/' + l
            _elocs.append( l )



        uTracks = self.getProp ( 'UseTracksE'      ) 
        uSpd = self.getProp ( 'UseSpdE'            ) 
        uPrs = self.getProp ( 'UsePrsE'            ) 
        if self.getProp('CaloStandalone') :
            uTracks = False
            uSpd    = True
            uPrs    = True


        cmp = electronReco ( self.getProp( 'Context'            ) ,
                             self.getProp( 'EnableRecoOnDemand' ) ,
                             uTracks,
                             uSpd,
                             uPrs,
                             _elocs ,
                             self.getProp('ElectronPt')          ,
                             self.getProp('FastReco')            ,
                             self.getProp('ExternalClusters')
                             )


        _log.info ('Configured Electron Hypos Reco : %s ' % cmp.name()  )
        ##
        return cmp

    ## Configure recontruction of Merged Pi0
    def mergedPi0s ( self ) :
        """
        Configure recontruction of Merged Pi0
        """

        uTracks = self.getProp ( 'UseTracksM'        )
        if self.getProp('CaloStandalone') :
            uTracks = False
        

        cmp = mergedPi0Reco ( self.getProp ( 'Context'            ) ,
                              self.getProp ( 'EnableRecoOnDemand' ) ,
                              False ,
                              self.getProp ( 'NeutralID'         )  ,
                              uTracks,
                              self.getProp('MergedPi0Pt')         ,
                              self.getProp('FastReco')            ,
                              self.getProp('ExternalClusters')
                              )
        

        _log.info ('Configured Merged Pi0     Reco : %s ' % cmp.name()  )
        ##
        return cmp
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        _log.debug('Configuration is not checked !')

        
    ## Calorimeter Reconstruction Configuration
    def applyConf ( self ) :
        """
        Calorimeter Reconstruction Configuration
        """
        
        self.checkConfiguration()
        
        _log.info ('Apply Calo Reco Configuration ')
        _log.info ( self )

        recList = self.getProp ( 'RecList') 
        skipNeutrals  = self.getProp('SkipNeutrals')
        skipCharged   = self.getProp('SkipCharged')
        forceOnDemand = self.getProp('ForceOnDemand') 

        seq     = []

        # configure all components for DoD
        if forceOnDemand :
            _log.info('Force Data-On-Demand for all components')
            self.setProp ( 'EnableRecoOnDemand', 'True' )
            self.digits()
            self.clusters()
            self.photons()
            self.mergedPi0s();
            self.electrons();

        # add only the requested components to the sequence
        if 'Digits'     in recList : addAlgs ( seq , self.digits() ) 
        if 'Clusters'   in recList : addAlgs ( seq , self.clusters() ) 
        if not skipNeutrals :
            if 'Photons'    in recList : addAlgs ( seq , self.photons() )
            if 'MergedPi0s' in recList or 'SplitPhotons' in recList : addAlgs ( seq , self.mergedPi0s() )
        if not skipCharged :
            if 'Electrons'  in recList : addAlgs ( seq , self.electrons() )
        
        setTheProperty ( seq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( seq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( seq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )
        
        if self.isPropertySet('Sequence') :
            main = self.getProp('Sequence') 
            addAlgs  ( main , seq ) 
            _log.info ('Configure main Calo Reco Sequence  : %s '% main.name() )
            _log.info ( prntCmp ( main ) ) 
        else :
            _log.info ('Configure Calorimeter Reco blocks ' )            
            _log.info ( prntCmp ( seq  ) )

        if self.getProp( 'EnableRecoOnDemand' )  :
            _log.info ( printOnDemand () ) 
            
# =============================================================================
## @class HltCaloRecoConf
#  Configurable for Calorimeter Reconstruction in Hlt context 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class HltCaloRecoConf(CaloRecoConf):
    """
    Class/Configurable to define the calorimeter reconstruction for Hlt
    
    """
    __slots__ = {}

    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        if not hltContext ( self.getProp('Context') ) :
            raise AttributeError, 'Invalid context for HltCaloRecoConf'
        
# =============================================================================
## @class OffLineCaloRecoConf
#  Configurable for Calorimeter Reconstruction in OffLine context 
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class OffLineCaloRecoConf(CaloRecoConf):
    """
    Class/Configurable to define the calorimeter reconstruction for Off-Line
    
    """
    __slots__ = {}

    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        if hltContext ( self.getProp( 'Context' ) ) :
            raise AttributeError, 'Invalid context for OffLineCaloRecoConf'
        




# =============================================================================
class CaloProcessor( CaloRecoConf ):
    """
    Class/Configurable to define the Full calorimeter reconstruction
    """

## -- re-use CaloRecoConf and add caloPIDs and ProtoP [double-inheritance fails due to conflicts]

   ## define the additional slots
    __slots__ = {
        'CaloReco'           : True , ## process CaloReco part
        'CaloPIDs'           : True , ## process CaloPID part
        'EnableOnDemand'     : False, ## overwrite EnableRecoOnDemand & EnablePIDsOnDemand
        'ProtoOnDemand'      : False,
        'NeutralProtoLocation':'',
        'ChargedProtoLocation':'',
        'CaloSequencer'       : None,
        'ProtoSequencer'      : None,
        'NeutralProtoSequencer'  : None,
        'ChargedProtoSequencer'  : None,
        ## PRIVATE TrackTypes & TrackCuts property (as in GlobalRecoConf) : for ChargedProto creation OnD.
        "TrackTypes"  : [ "Long", "Upstream", "Downstream" ],
        "TrackCuts"   : {  "Long"       : { "Chi2Cut" : [0,10] }
                           ,"Upstream"   : { "Chi2Cut" : [0,10] }
                           ,"Downstream" : { "Chi2Cut" : [0,10] } },
        ##
        'DataType'               : 'MC09',
        'PIDList'            : ['InAcceptance',
                                  'Match',
                                  'Energy',
                                  'Chi2',
                                  'DLL',
                                  'NeutralPID'
                                  ] # List of PID fragments to be included (alternative full sequence per technique : [ 'EcalPID', 'BremPID', 'HcalPID', 'PrsPID', 'SpdPID', 'NeutralPID' ] )
        }
    
    ## used configurables 
    __used_configurables__ = (
        (CaloPIDsConf,None ),
        (CaloDigitConf,None )
	)


    ## Configure recontruction of Calo Charged  PIDs (copy-past from CaloPIDs)
    def caloPIDs ( self ) :

        from CaloPIDs.PIDs import caloPIDs
        from CaloPIDs.PIDs import referencePIDs


        ## confuse configurable on purpose 
        _locs = self.getProp ( 'TrackLocations'    )
        _elocs = []
        for l in _locs :
            if l.find( '/Event/' )  != 0 :
                l = '/Event/' + l
            _elocs.append( l )



        cmp = caloPIDs ( self.getProp( 'Context'            )  ,
                         self.getProp( 'EnableOnDemand' )  ,
                         self.getProp('PIDList'),
                         _elocs,
                         self.getProp('SkipNeutrals'),
                         self.getProp('SkipCharged'),
                         self.getProp('FastReco'),
                         self.getProp('ExternalClusters'),
                         self.getName()
                         ) 
        referencePIDs( self.getProp('DataType') )
       
        log.info ('Configured Calo PIDs           : %s ' % cmp.name()  ) 
        ##
        return cmp 

    def caloSequence ( self,   tracks=[]  ) :
        seq  = GaudiSequencer ( 'CaloRecoPIDs' + self.getName() )
        seq.Members[:] = []
        self.setProp("CaloSequencer", seq)
        context = self.getProp('Context')
        if context == '' :
            self.setProp("Context", self.getName() )        
        if tracks != [] :
            self.setProp("TrackLocations", tracks)
        self.applyConf()
        return seq
    def protoSequence ( self,   tracks=[] , protoPrefix = '' ) :
        seq  = GaudiSequencer ( 'CaloProtoPUpdate' + self.getName() )
        seq.Members[:] = []
        self.setProp("ProtoSequencer", seq)
        if self.getProp('Context') == '' : 
            self.setProp("Context", self.getName() )
        if tracks != [] :
            self.setProp("TrackLocations", tracks)        
        if protoPrefix != '' :
            nloc = protoPrefix + '/Neutrals'
            cloc = protoPrefix + '/Charged'
            nloc = nloc.replace('//', '/')
            cloc = cloc.replace('//', '/')
            self.setProp("ChargedProtoLocation",cloc)
            self.setProp("NeutralProtoLocation",nloc)            
        self.applyConf()
        return seq
    def chargedProtoSequence ( self,   tracks=[] , protoPrefix = '' ) :
        seq  = GaudiSequencer ( 'CaloChargedProtoPUpdate' + self.getName() )
        seq.Members[:] = []
        self.setProp("ChargedProtoSequencer", seq)
        if self.getProp('Context') == '' : 
            self.setProp("Context", self.getName() )
        if tracks != [] :
            self.setProp("TrackLocations", tracks)        
        if protoPrefix != '' :
            cloc = protoPrefix
            if protoPrefix.find('/Charged') == -1 :
                cloc = protoPrefix + '/Charged'
                cloc = cloc.replace('//', '/')
            self.setProp("ChargedProtoLocation",cloc)
        self.applyConf()
        return seq
    def neutralProtoSequence ( self,   tracks=[] , protoPrefix = '' ) :
        seq  = GaudiSequencer ( 'CaloNeutralProtoPUpdate' + self.getName() )
        seq.Members[:] = []
        self.setProp("NeutralProtoSequencer", seq) 
        if self.getProp('Context') == '' : 
            self.setProp("Context", self.getName() )
        if tracks != [] :
            self.setProp("TrackLocations", tracks)        
        if protoPrefix != '' :
            nloc = protoPrefix
            if protoPrefix.find('/Neutrals') == -1 :
                nloc = protoPrefix + '/Neutrals'
                nloc = nloc.replace('//', '/')
            self.setProp("NeutralProtoLocation",nloc)            
        self.applyConf()
        return seq
    def sequence ( self,   tracks=[], protoPrefix = ''  ) :
        seq  = GaudiSequencer ( 'CaloProcessor' + self.getName() )
        seq.Members[:] = []
        self.setProp("Sequence", seq)
        if self.getProp('Context') == '' : 
            self.setProp("Context", self.getName() )
        if tracks != [] :
            self.setProp("TrackLocations", tracks)        
        if protoPrefix != '' :
            nloc = protoPrefix + '/Neutrals'
            cloc = protoPrefix + '/Charged'
            nloc = nloc.replace('//', '/')
            cloc = cloc.replace('//', '/')
            self.setProp("ChargedProtoLocation",cloc)
            self.setProp("NeutralProtoLocation",nloc)            
        self.applyConf()
        return seq

        
    def applyConf ( self ) :
        
        _log.info ('Apply configuration for  full calo processing ')
        _log.info ( self )
        from Configurables import ( GaudiSequencer,
                                    ChargedProtoParticleAddEcalInfo,
                                    ChargedProtoParticleAddBremInfo,
                                    ChargedProtoParticleAddHcalInfo,
                                    ChargedProtoParticleAddPrsInfo,
                                    ChargedProtoParticleAddSpdInfo,
                                    ChargedProtoParticleMaker,
                                    ChargedProtoCombineDLLsAlg
                                    )

        fullSeq     = []


        if self.getName() == 'CaloProcessor' and ( self.getProp('Context') == '' or self.getProp('Context') == 'CaloProcessor' ) :
            self.setProp('Context','Offline') # default is Offline is neither context nor name is specified

        # overwrite Reco & PID onDemand
        dod = self.getProp('EnableOnDemand')
        pdod = self.getProp('ProtoOnDemand')

        if dod :
            pdod = dod            
        self.setProp('EnableRecoOnDemand',dod)
            
        ## define the calo sequence
        caloSeq     = []
        
        doReco = self.getProp('CaloReco')
        doPIDs = self.getProp('CaloPIDs')
        skipNeutrals = self.getProp('SkipNeutrals')
        skipCharged  = self.getProp('SkipCharged')
        context = self.getProp('Context')

        # CaloReco sequence
        recoSeq = getAlgo( GaudiSequencer , "CaloReco"+self.getName() , context ) 
        recoSeq.Members[:] = []
        recList = self.getProp ( 'RecList')         


        # configure all components by default (DoD)
        forceOnDemand = self.getProp('ForceOnDemand')
        if forceOnDemand :
            self.setProp('EnableOnDemand','True')
            self.setProp('EnableRecoOnDemand','True')
            dig=self.digits()
            clu=self.clusters()
            pho=self.photons()
            mer=self.mergedPi0s();
            ele=self.electrons();


        #  add only the requested components to the sequence
	if 'Digits'     in recList : addAlgs ( recoSeq , self.digits() ) 
        if 'Clusters'   in recList : addAlgs ( recoSeq , self.clusters() ) 
        if not skipNeutrals :
            if 'Photons'    in recList : addAlgs ( recoSeq , self.photons() )
            if 'MergedPi0s' in recList or 'SplitPhotons' in recList : addAlgs ( recoSeq , self.mergedPi0s() )
        if not skipCharged :
            if 'Electrons'  in recList : addAlgs ( recoSeq , self.electrons() )

        # CaloPIDs sequence
        #        pidSeq = getAlgo( GaudiSequencer , "CaloPIDsSeq" , context ) 
        #        addAlgs ( pidSeq , self.caloPIDs  () )
        
        pidSeq=self.caloPIDs()

                
        # update CaloSequence
        if doReco :
            addAlgs ( caloSeq , recoSeq  )
        if doPIDs        :
            addAlgs ( caloSeq , pidSeq )            


        ## propagate the global properties
        setTheProperty ( caloSeq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( caloSeq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( caloSeq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )



        ######## ProtoParticle update ##########
        protoSeq     = []
        cProtoSeq    = []
        nProtoSeq    = []
        #  ProtoParticle locations
        nloc = self.getProp('NeutralProtoLocation')
        cloc = self.getProp('ChargedProtoLocation')
        # try automatic location if not explicit for HLT's sequence
        if hltContext ( self.getProp('Context') ) :
            if nloc == '' : 
                if nloc.find('/') == -1 :
                    nloc = context + '/ProtoP/Neutrals'
                else :
                    nloc = context.replace('/','/ProtoP/',1)+ '/Neutrals'
            if cloc == '' : 
                if cloc.find('/') == -1 :
                    cloc = context + '/ProtoP/Charged'
                else :
                    cloc = context.replace('/','/ProtoP/',1)+ '/Charged'

        # Confuse Configurable
        if cloc != '':
            if cloc.find( '/Event/' ) != 0 :
                cloc = '/Event/' + cloc

        if nloc != '':
            if nloc.find( '/Event/' ) != 0 :
                nloc = '/Event/' + nloc
        

        # ChargedProtoParticle
        if not self.getProp('SkipCharged') :
            ecal = getAlgo( ChargedProtoParticleAddEcalInfo,"ChargedProtoPAddEcal", context)
            brem = getAlgo( ChargedProtoParticleAddBremInfo,"ChargedProtoPAddBrem", context)
            hcal = getAlgo( ChargedProtoParticleAddHcalInfo,"ChargedProtoPAddHcal", context)
            prs  = getAlgo( ChargedProtoParticleAddPrsInfo ,"ChargedProtoPAddPrs" , context)
            spd  = getAlgo( ChargedProtoParticleAddSpdInfo ,"ChargedProtoPAddSpd" , context)            
            comb = getAlgo( ChargedProtoCombineDLLsAlg, "ChargedProtoPCombineDLLs", context)

            # ChargedProtoP Maker on demand (not in any sequencer)  ####
            maker = getAlgo( ChargedProtoParticleMaker, "ChargedProtoMaker" , context, cloc , pdod )
            # protoPMaker settings (from GlobalRecoConf)
            from Configurables import DelegatingTrackSelector,GaudiSequencer
            ## ppConf = GlobalRecoConf('DummyConf',RecoSequencer=GaudiSequencer('DummySeq')) 
            ##ttypes = ppConf.getProp('TrackTypes')
            ##tcuts  = ppConf.getProp('TrackCuts')

            ttypes = self.getProp('TrackTypes')
            tcuts  = self.getProp('TrackCuts')
            
            maker.addTool( DelegatingTrackSelector, name="TrackSelector" )
            maker.TrackSelector.TrackTypes = ttypes
            from Configurables import TrackSelector
            for type in ttypes : 
                maker.TrackSelector.addTool(TrackSelector,name=type)
                ts = getattr(maker.TrackSelector,type)
                ts.TrackTypes = [type]
                if type in tcuts :
                    for name,cut in tcuts[type].iteritems() :
                        ts.setProp("Min"+name,cut[0])
                        ts.setProp("Max"+name,cut[1])
            #########################################
            if cloc != '' :
                maker.Output = cloc

        ## confuse configurable on purpose 
            _locs = self.getProp ( 'TrackLocations'    )
            _elocs = []
            for l in _locs :
                if l.find( '/Event/' )  != 0 :
                    l = '/Event/' + l
                _elocs.append( l )

            if not hltContext( self.getProp( 'Context' ) ) and _elocs: 
                maker.Inputs = _elocs
            
            # location
            if cloc != '' :
                ecal.ProtoParticleLocation = cloc
                brem.ProtoParticleLocation = cloc
                hcal.ProtoParticleLocation = cloc
                prs.ProtoParticleLocation = cloc
                spd.ProtoParticleLocation = cloc            
                comb.ProtoParticleLocation = cloc            
            # Fill the sequence
            cpSeq = getAlgo( GaudiSequencer , "ChargedProtoPCaloUpdateSeq", context )
            cpSeq.Members = [ ecal,brem,hcal,prs,spd,comb ]
            addAlgs(protoSeq , cpSeq )
            addAlgs(cProtoSeq, [ecal,brem,hcal,prs,spd,comb] )

        # NeutralProtoParticleProtoP components        
        if not self.getProp('SkipNeutrals') :
            from Configurables import NeutralProtoPAlg
            neutral = getAlgo( NeutralProtoPAlg,"NeutralProtoPMaker", context)
            # location
            if nloc != '' :
                neutral.ProtoParticleLocation = nloc
            # fill the sequence
            addAlgs(protoSeq, neutral )
            addAlgs(nProtoSeq, neutral )


        ## propagate the global properties
        setTheProperty ( protoSeq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( protoSeq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( protoSeq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )

        setTheProperty ( nProtoSeq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( nProtoSeq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( nProtoSeq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )

        setTheProperty ( cProtoSeq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( cProtoSeq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( cProtoSeq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )

        # Full sequence
        addAlgs( fullSeq, caloSeq )
        addAlgs( fullSeq, protoSeq )
                    
        
        ## define the sequencers
        if self.isPropertySet('Sequence') :
            main = self.getProp('Sequence') 
            main.Members[:]=[]
            addAlgs  ( main , fullSeq ) 
            _log.info ('Configure main Calo processing Sequence  : %s '% main.name() )
            _log.info ( prntCmp ( main ) ) 

        if self.isPropertySet('CaloSequencer') :
            calo = self.getProp('CaloSequencer') 
            calo.Members[:]=[]
            addAlgs  ( calo , caloSeq ) 

        if self.isPropertySet('ProtoSequencer') :
            proto = self.getProp('ProtoSequencer') 
            proto.Members[:]=[]
            addAlgs  ( proto , protoSeq ) 


        if self.isPropertySet('ChargedProtoSequencer') :
            cproto = self.getProp('ChargedProtoSequencer') 
            cproto.Members[:]=[]
            addAlgs  ( cproto , cProtoSeq ) 

        if self.isPropertySet('NeutralProtoSequencer') :
            nproto = self.getProp('NeutralProtoSequencer') 
            nproto.Members[:]=[]
            addAlgs  ( nproto , nProtoSeq ) 

        if self.getProp( 'EnableOnDemand' )  :
            _log.info ( printOnDemand () ) 

#####################################
class CaloLines(LHCbConfigurableUser):
    """
    Class/Configurable to define the HLT2 fast reconstruction for high-ET photon, low-ET photon/pi0 & low-ET electrons
    """


#    __used_configurables__ = [ CaloProcessor ]

   ## define the additional slots
    __slots__ = {
        "Context"            : '' ,    # The context to run (default = offline)
        'TrackLocations'     : [] ,    # Track locations (Neutral/Charged cluster selection with UseTrack(E) )
        'HighPhoton'         : True , ## process the highEt-threshold photon reconstruction
        'LowPhoton'          : True , ## process the lowEt-threshold photon reconstruction
        'LowElectron'        : True , ## process the LowEt-threshold electron reconstruction
        'EnableOnDemand'     : False, ## overwrite EnableRecoOnDemand & EnablePIDsOnDemand
        'HighEt'             : 2000.*MeV ,
        'LowEt'              : 300.*MeV,
        'ClusterEtFactor'    : 1.,     # pre-cut on cluster Et is factor * Low(High)ET (should be <=1)
        'L0Calo2Calo'        : True,
        'ClusterizationLevel': 2,      # clusterizationLevel (when L0Calo2Calo == True only)   
        'ProtoOnDemand'      : False,
        'Sequencer'          : None,
        'OutputLevel'        : INFO,
        'HighEtProtoPPrefix' : '',
        'LowEtProtoPPrefix'  : ''
        }

    


    def sequence ( self,   tracks=[]  ) :
        seq  = GaudiSequencer ( 'CaloLines' + self.getName() )
        seq.Members[:] = []
        self.setProp("Sequencer", seq)
        context = self.getProp('Context')
        if context == '' :
            self.setProp("Context", self.getName() )        
        self.setProp("TrackLocations", tracks)        
        self.applyConf()
        return seq
 
        
    def applyConf ( self ) :
        

        from Configurables import ( GaudiSequencer,
                                    HltL0CaloCandidates
                                    )


        if self.getName() == 'CaloLines' and ( self.getProp('Context') == '' or self.getProp('Context') == 'CaloLines' ) :
            self.setProp('Context','Offline') # default is Offline is neither context nor name is specified



        # overwrite Reco & PID onDemand
        dod = self.getProp('EnableOnDemand')
        pdod = self.getProp('ProtoOnDemand')
        if dod :
            pdod = dod            

        tracks = self.getProp('TrackLocations')

        ###
        caloLines = GaudiSequencer( 'CaloLinesSeq' + self.getName() )
        caloLines.Members[:] = []

        if self.getProp('L0Calo2Calo') : 
            l0calo2calo=HltL0CaloCandidates('L0Calo2Calo')
            if self.getProp('ClusterizationLevel') > 0 : 
                level = self.getProp('ClusterizationLevel')
                l0calo2calo.ClusterizationLevel=level
            addAlgs( caloLines,  l0calo2calo )
            tagHighP = ''
            tagLowP = ''
            tagLowE = ''
        else :
            tagHighP = 'HighPhoton'
            tagLowP  = 'LowPhoton'
            tagLowE  = 'LowElectron'
            
        name = self.getName()
        fac  = self.getProp('ClusterEtFactor') 

        if self.getProp('HighPhoton') :            
            context = self.getProp('Context')
            if  context != '' :
                context = context +'HighPhoton'
            hp = CaloProcessor(name+'HighPhoton'
                               ,TrackLocations = tracks
                               ,Context = context
                               ,RecList = ['Digits','Clusters','Photons']
                               ,CaloPIDs = False
                               ,ExternalClusters="/Event/Rec/Calo/HighEtPhotons"
                               ,ClusterPt = self.getProp('HighEt')*fac
                               ,PhotonPt = self.getProp('HighEt')
                               ,MakeExternalClustersWithTag = tagHighP
                               )
            
            addAlgs( caloLines, hp.caloSequence(tracks=tracks) )

            if self.getProp('HighEtProtoPPrefix') == '' :
                hploc = name+'HighPhoton/ProtoP'
            else :
                hploc = self.getProp('HighEtProtoPPrefix')
            addAlgs( caloLines, hp.neutralProtoSequence(protoPrefix = hploc, tracks=tracks )  )

        if self.getProp('LowPhoton') :
            context = self.getProp('Context')
            if  context != '' :
                context = context +'LowPhoton'
            lp = CaloProcessor(name+'LowPhoton'
                               ,TrackLocations = tracks
                               ,Context = context
                               ,RecList = ['Digits','Clusters','Photons','MergedPi0s','SplitPhotons']
                               ,ExternalClusters="/Event/Rec/Calo/LowEtPhotons"
                               ,CaloPIDs = False
                               ,ClusterPt = self.getProp('LowEt')*fac
                               ,PhotonPt = self.getProp('LowEt')
                               ,MakeExternalClustersWithTag = tagLowP
                               )
            addAlgs( caloLines , lp.caloSequence(tracks=tracks) )
            if self.getProp('LowEtProtoPPrefix') == '' :
                lploc = name+'LowPhoton/ProtoP'
            else :
                lploc = self.getProp('LowEtProtoPPrefix')
            addAlgs( caloLines ,  lp.neutralProtoSequence(protoPrefix=lploc, tracks=tracks))
            

        if self.getProp('LowElectron') :
            context = self.getProp('Context')
            if  context != '' :
                context = context +'LowElectron'
            le = CaloProcessor(name+'LowElectron'
                               ,TrackLocations = tracks
                               ,Context = context
                               ,RecList = ['Digits','Clusters','Electrons']
                               ,ExternalClusters="/Event/Rec/Calo/LowEtElectrons"
                               ,ClusterPt = self.getProp('LowEt')*fac
                               ,ElectronPt = self.getProp('LowEt')
                               ,SkipNeutrals = True
                               ,ProtoOnDemand = pdod
                               ,MakeExternalClustersWithTag = tagLowE
                               )
            addAlgs( caloLines , le.caloSequence(tracks=tracks))
            if self.getProp('LowEtProtoPPrefix') == '' :
                leloc = name+'LowElectron/ProtoP'
            else :
                leloc = self.getProp('LowEtProtoPPrefix')
            addAlgs( caloLines , le.chargedProtoSequence(protoPrefix=leloc, tracks=tracks))


        caloLines.IgnoreFilterPassed = True
        ## propagate the global properties
        setTheProperty ( caloLines , 'OutputLevel'     , self.getProp ( 'OutputLevel'     ) )

        ## define the sequencers
        if self.isPropertySet('Sequencer') :
            main = self.getProp('Sequencer') 
            addAlgs  ( main , caloLines ) 



# =============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__
    
            
    
