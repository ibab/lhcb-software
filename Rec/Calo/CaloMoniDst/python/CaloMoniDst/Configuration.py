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
                                       protosMoni     ,
                                       pidCheck       ,
                                       recoCheck 
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
        "Context"              : "Offline"   # The context within which to run
        , "MeasureTime"        : False       # Measure the time for sequencers
        , "OutputLevel"        : INFO        # The global output level
        , 'MonitorSequence'    : None        # The monitor's sequence
        , 'CheckerSequence'    : None        # The checker's sequence 
        , 'MoniList'           : [ 'Digits'   ,
                                   'EFlow'    ,
                                   'Clusters' ,
                                   'Hypos'    ,
                                   'Pi0s'     ,
                                   'Protos'   ,
                                   'PIDs'     
                                   ] 
        , 'CheckList'          : ['Reco','PIDs']
        , 'Histograms'         : 'OfflineFull' # Default Histograms level
        , 'NoSpdPrs'           : False # Upgrade configuration without Prs/Spd
        , 'Verbose'            : False
        }

    ## Default Histogram set
    __known_histograms__ = [ "None", "Online", "OfflineExpress", "OfflineFull", "Expert" ]

 
    ## configure monitoring of Digits
    def digits   ( self ) :
        """
        Configure monitoring of Digits
        
        """
        cmp = digitsMoni   ( self.getProp('Context') , self.getProp('NoSpdPrs'))
        ##
        return cmp

    
    ## configure E-Flow monitoring 
    def eflow ( self ) :
        """
        Configure E-Flow monitoring
        """
        cmp = eflowMoni ( self.getProp('Context') )

        return cmp

    ## configure Clusters monitoring 
    def clusters ( self ) :
        """
        Configure Clusters monitoring
        """
        cmp = clustersMoni ( self.getProp('Context') )
        
        return cmp

    ## configure Hypos monitoring 
    def hypos ( self ) :
        """
        Configure Hypos monitoring
        """
        cmp = hyposMoni ( self.getProp('Context') )
        
        return cmp

    ## configure pi0 monitoring 
    def pi0s ( self ) :
        """
        Configure Pi0s monitoring
        """
        cmp = pi0sMoni ( self.getProp('Context') )
        
        return cmp

    ## configure pi0 monitoring 
    def protos ( self ) :
        """
        Configure Protos monitoring
        """
        cmp = protosMoni ( self.getProp('Context') )
        
        return cmp

    
    ## configure PID monitoring 
    def pids ( self ) :
        """
        Configure PIDs monitoring
        """
        cmp = pidsMoni ( self.getProp('Context'), self.getProp("Histograms") )
      
        return cmp


    #--- configure the checkers
    def pidChecker ( self ) :
        cmp = pidCheck( self.getProp('Context'), self.getProp("NoSpdPrs") )      
        return cmp
    def recoChecker ( self ) :
        cmp = recoCheck( self.getProp('Context'))
        return cmp

    
    
    ## Check the configuration
    def checkConfiguration ( self ) :
        """
        Check the configuration
        """

        if self.getProp( 'Histograms' ) not in self.__known_histograms__:
            raise RuntimeError("Unknown Histograms option '%s'" % self.getProp( 'Histograms' ))


    def printConf(self) :
        if self.getProp('NoSpdPrs') :
            log.info("CaloMoniDstConf : upgrade configuration without Spd/Prs")
        if self.getProp('Verbose') :
            log.info( self )

 
    ## Calorimeter Monitoring Configuration
    def applyConf ( self ) :
        """
        Calorimeter Monitor Configuration
        """
        log.info ('CaloMoniConf: Apply Calo Monitoring Configuration ')

        self.printConf()
        
        self.checkConfiguration()
        

        #----- MONITORS
        monList = self.MoniList        
        seq     = []        
        if 'Digits'     in monList : addAlgs ( seq , self.digits     () ) 
        if 'EFlow'      in monList : addAlgs ( seq , self.eflow      () ) 
        if 'Clusters'   in monList : addAlgs ( seq , self.clusters   () ) 
        if 'Hypos'      in monList : addAlgs ( seq , self.hypos      () )
        if 'Pi0s'       in monList : addAlgs ( seq , self.pi0s       () )
        if 'Protos'     in monList : addAlgs ( seq , self.protos     () )
        if 'PIDs'       in monList : addAlgs ( seq , self.pids       () )        
        setTheProperty ( seq , 'Context'     , self.getProp('Context') )        
        setTheProperty ( seq , 'OutputLevel' , self.getProp('OutputLevel') )
        setTheProperty ( seq , 'MeasureTime' , self.getProp('MeasureTime') )    
        if self.isPropertySet('MonitorSequence') :
            moniSeq=self.getProp('MonitorSequence')
            addAlgs  ( moniSeq , seq ) 
            log.info ('Configure main Calo Moni MonitorSequence  : %s '% moniSeq.name() )
            if self.getProp('Verbose') :
                log.info ( prntCmp ( moniSeq ) ) 


        #----- CHECKERS
        checkList = self.CheckList        
        cseq     = []        
        if 'Reco'     in checkList : addAlgs ( cseq , self.recoChecker     () ) 
        if 'PIDs'     in checkList : addAlgs ( cseq , self.pidChecker      () ) 
        setTheProperty ( cseq , 'Context'     , self.getProp('Context') )        
        setTheProperty ( cseq , 'OutputLevel' , self.getProp('OutputLevel') )
        setTheProperty ( cseq , 'MeasureTime' , self.getProp('MeasureTime') )    

        if self.isPropertySet('CheckerSequence') :
            checkSeq= self.getProp('CheckerSequence')
            addAlgs  ( checkSeq  , cseq ) 
            log.info ('Configure  CheckerSequence  : %s '% checkSeq.name() )
            if self.getProp('Verbose') :
                log.info ( prntCmp ( checkSeq ) ) 

            
