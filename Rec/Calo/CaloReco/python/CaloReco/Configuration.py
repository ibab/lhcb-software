#!/usr/bin/env gaudirun.py
# =============================================================================
## Confurable for Calorimeter Reconstruction
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
Confurable for Calorimeter Reconstruction
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $"
# =============================================================================
__all__ = (
    'HltCaloRecoConf'     ,
    'OffLineCaloRecoConf'     
    )
# =============================================================================

from LHCbKernel.Configuration import *


from CaloKernel.ConfUtils     import ( addAlgs        ,
                                       printOnDemand  ,
                                       prntCmp        ,
                                       hltContext     , 
                                       setTheProperty )
from Reconstruction  import ( digitsReco     ,
                              clusterReco    , 
                              photonReco     ,
                              electronReco   ,
                              mergedPi0Reco  ) 

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
        "Context"              : "Offline"   # The context to run
        , "MeasureTime"        : True        # Measure the time for sequencers
        , "OutputLevel"        : INFO        # The global output level
        ##
        , 'Sequence'           : ''          # The sequencer to add the CALO reconstruction algorithms to
        , 'RecList'            : [ 'Digits'       ,
                                   'Clusters'     ,
                                   'Photons'      ,
                                   'MergedPi0s'   ,
                                   'SplitPhotons' , # the same action as 'MergedPi0s'
                                   'Electrons'    ] ##
        , 'ForceDigits'         : True       # Force digits recontruction to be run with Clusters
        , 'UseTracks'           : True       # Use Tracks as Neutrality Criteria 
        , 'UseSpd'              : False      # Use Spd as Neutrailty Criteria
        , 'EnableRecoOnDemand'  : False      # Enable Reco-On-Demand
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
        , 'UseTracks'          : """ Use Tracks as Neutrality criterion """ 
        , 'UseSpd'             : """ Use Spd as Neutrality criterion """ 
        , 'EnableRecoOnDemand' : """ Enable Reco-On-Demand """ 
        ##
    }

    ## configure processing of Digits
    def digits   ( self ) :
        """
        Configure proecssing of Digits
        
        """
        cmp = digitsReco   ( self.getProp('Context')            ,
                             self.getProp('EnableRecoOnDemand') )
        log.info ('Configured Digits Processiong  : %s ' % cmp.name()  )
        ##
        return cmp
        
    ## Configure reconstruction of Ecal Clusters
    def clusters ( self ) :
        """
        Configure reconstruction of Ecal Clusters
        """
        cmp = clusterReco   ( self.getProp('Context')             ,
                              self.getProp('EnableRecoOnDemand' ) ,
                              self.getProp('ForceDigits'        ) )
        log.info ('Configured Ecal Clusters  Reco : %s ' % cmp.name()  )
        ##
        return cmp
    
    ## reconstruct the photons
    def photons  ( self ) :
        """
        Define the reconstruction of Photons
        """
        cmp = photonReco   ( self.getProp ( 'Context'           ) ,
                             self.getProp ( 'EnableRecoOnDemand') ,
                             self.getProp ( 'UseTracks'         ) ,
                             self.getProp ( 'UseSpd'            ) )
        log.info ('Configured Single Photons Reco : %s ' % cmp.name()  )
        ##
        return cmp

    ## Configure recontruction of Electrons
    def electrons ( self ) :
        """
        Configure recontruction of Electrons
        """
        cmp = electronReco ( self.getProp( 'Context'            ) ,
                             self.getProp( 'EnableRecoOnDemand' ) ) 
        log.info ('Configured Electron Hypos Reco : %s ' % cmp.name()  )
        ##
        return cmp

    ## Configure recontruction of Merged Pi0
    def mergedPi0s ( self ) :
        """
        Configure recontruction of Merged Pi0
        """
        cmp = mergedPi0Reco ( self.getProp ( 'Context'            ) ,
                              self.getProp ( 'EnableRecoOnDemand' ) )
        log.info ('Configured Merged Pi0     Reco : %s ' % cmp.name()  )
        ##
        return cmp
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """
        log.warning('CaloRecoConf: Configuration is not checked!')

        
    ## Calorimeter Reconstruction Configuration
    def applyConf ( self ) :
        """
        Calorimeter Reconstruction Configuration
        """
        
        self.checkConfiguration()
        
        log.info ('CaloRecoConf: Apply Calo Reco Configuration ')
        log.info ( self )

        recList = self.getProp ( 'RecList') 

        seq     = []
        
        if 'Digits'     in recList : addAlgs ( seq , self.digits     () ) 
        if 'Clusters'   in recList : addAlgs ( seq , self.clusters   () ) 
        if 'Photons'    in recList : addAlgs ( seq , self.photons    () )
        if 'MergedPi0s' in recList or 'SplitPhotons' in recList :
            addAlgs ( seq , self.mergedPi0s () )
        if 'Electrons'  in recList : addAlgs ( seq , self.electrons  () )
        
        setTheProperty ( seq , 'Context'     , self.getProp ( 'Context'     ) )
        setTheProperty ( seq , 'OutputLevel' , self.getProp ( 'OutputLevel' ) )
        setTheProperty ( seq , 'MeasureTime' , self.getProp ( 'MeasureTime' ) )
        
        if self.getProp('Sequence') :
            addAlgs  ( self.Sequence , seq ) 
            log.info ('Configure main Calo Reco Sequence  : %s '% self.Sequence.name() )
            log.info ( prntCmp ( self.Sequence ) ) 
        else :
            log.info ('Configure Calorimeter Reco blocks ' )            
            log.info ( prntCmp ( seq ) )

        if self.getProp( 'EnableRecoOnDemand' )  :
            log.info ( printOnDemand () ) 
            
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
# The END
# =============================================================================

