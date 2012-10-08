#!/usr/bin/env gaudirun.py
# =============================================================================
## Configurable for Calorimeter Monitoring
#  @author Vanya BELYAEV Ivan.Belyaev@nikhe.nl
#  @date 2008-07-17
# =============================================================================
"""
Configurable for Calorimeter Monitoring
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS tag $Name:  $, version $Revision: 1.4 $"
# =============================================================================
__all__  = (
    'CaloMoniDstConf',
    )
# =============================================================================
from Gaudi.Configuration      import *

from LHCbKernel.Configuration import *

from CaloKernel.ConfUtils     import ( addAlgs        ,
                                       printOnDemand  ,
                                       prntCmp        ,
                                       hltContext     , 
                                       setTheProperty )

from CaloMoniDst.Monitor      import ( digitsMoni     ,
                                       eflowMoni      , 
                                       clustersMoni   , 
                                       hyposMoni      ,
                                       pi0sMoni       ,
                                       pidsMoni       ,
                                       protosMoni
                                       )

# =============================================================================
## @class CaloMoniDstConf
#  Configurable for Calorimeter Reconstruction
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-07-17
class CaloMoniDstConf(LHCbConfigurableUser):
    """
    Class/Configurable to define the calorimeter monitoring
    
    """
   ## define the slots
    __slots__ = {
        ##
        "Context"              : "Offline"   # The context within which to run
        , "MeasureTime"        : False       # Measure the time for sequencers
        , "OutputLevel"        : INFO        # The global output level
        ##
        , 'Sequence'           : None        # The sequencer to add the CALO reconstruction algorithms to
        , 'MoniList'           : [ 'Digits'   ,
                                   'EFlow'    ,
                                   'Clusters' ,
                                   'Hypos'    ,
                                   'Pi0s'     ,
                                   'Protos'   ,
                                   'PIDs'     
                                   ] ##
        ##
        , 'Histograms'         : 'OfflineFull' # Default Histograms level
        }

    ## Default Histogram set
    __known_histograms__ = [ "None", "Online", "OfflineExpress", "OfflineFull", "Expert" ]

 
    ## configure monitoring of Digits
    def digits   ( self ) :
        """
        Configure monitoring of Digits
        
        """
        cmp = digitsMoni   ( self.Context )
        ##
        return cmp

    
    ## configure E-Flow monitoring 
    def eflow ( self ) :
        """
        Configure E-Flow monitoring
        """
        cmp = eflowMoni ( self.Context )

        return cmp

    ## configure Clusters monitoring 
    def clusters ( self ) :
        """
        Configure Clusters monitoring
        """
        cmp = clustersMoni ( self.Context )
        
        return cmp

    ## configure Hypos monitoring 
    def hypos ( self ) :
        """
        Configure Hypos monitoring
        """
        cmp = hyposMoni ( self.Context )
        
        return cmp

    ## configure pi0 monitoring 
    def pi0s ( self ) :
        """
        Configure Pi0s monitoring
        """
        cmp = pi0sMoni ( self.Context )
        
        return cmp

    ## configure pi0 monitoring 
    def protos ( self ) :
        """
        Configure Protos monitoring
        """
        cmp = protosMoni ( self.Context )
        
        return cmp

    
    ## configure PID monitoring 
    def pids ( self ) :
        """
        Configure PIDs monitoring
        """
        cmp = pidsMoni ( self.Context, self.getProp("Histograms") )
      
        return cmp
    
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """

        if self.getProp( 'Histograms' ) not in self.__known_histograms__:
            raise RuntimeError("Unknown Histograms option '%s'" % self.getProp( 'Histograms' ))
 
    ## Calorimeter Monitoring Configuration
    def applyConf ( self ) :
        """
        Calorimeter Reconstruction Configuration
        """
        
        self.checkConfiguration()
        
        log.info ('CaloMoniConf: Apply Calo Monitoring Configuration ')
        
        monList = self.MoniList
        
        seq     = []
        
        if 'Digits'     in monList : addAlgs ( seq , self.digits     () ) 
        if 'EFlow'      in monList : addAlgs ( seq , self.eflow      () ) 
        if 'Clusters'   in monList : addAlgs ( seq , self.clusters   () ) 
        if 'Hypos'      in monList : addAlgs ( seq , self.hypos      () )
        if 'Pi0s'       in monList : addAlgs ( seq , self.pi0s       () )
        if 'Protos'     in monList : addAlgs ( seq , self.protos       () )
        if 'PIDs'       in monList : addAlgs ( seq , self.pids       () )
        
        setTheProperty ( seq , 'Context'     , self.Context )
        
        setTheProperty ( seq , 'OutputLevel' , self.getProp('OutputLevel') )
        setTheProperty ( seq , 'MeasureTime' , self.getProp('MeasureTime') )
        
        if self.getProp('Sequence') :
            addAlgs  ( self.Sequence , seq ) 
            log.info ('Configure main Calo Moni Sequence  : %s '% self.Sequence.name() )
            log.info ( prntCmp ( self.Sequence ) ) 
            
        ##print ' CONFIGURATION!' , prntCmp ( seq )
        

# =============================================================================
# The END 
# =============================================================================
