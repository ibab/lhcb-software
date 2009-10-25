#!/usr/bin/env python
# =============================================================================
# $Id: Configuration.py,v 1.4 2009-10-25 17:45:28 ibelyaev Exp $
# =============================================================================
# @file  KaliCalo/Configuration.py
# The basic configuration for Calorimeetr Calibrations 
# @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
# @date   2009-09-28
# =============================================================================
"""
The basic configuration to (re)run Ecal pi0-calibration

The application produces:

 - ROOT NTuple for analysis
 - 'femto-DST' output, suibale for fast reprocessing 
 - monitoring histograms

The usage is fairly trivial:

    # configure the application:

    from Gaudi.Configuration import * 
    from Configurables       import KaliPi0Conf

    KaliPi0Conf (
    FirstPass = True    ,  ## set for the first pass on DST
    EvtMax    = 10000      ## number of events to process
    )

    # define input data :
    EventSelector(
    Input = [ .. ] 
    )

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $"
# =============================================================================
# the only one  "vizible" symbol 
__all__  = (
    'KaliPi0Conf' ,                         ## the only one vizible symbol
    )
# =============================================================================
from Gaudi.Configuration      import *
from LHCbKernel.Configuration import *

# =============================================================================
## @class KaliPi0Conf
#  Configurable for Calorimeter Iterative Calibration with pi0
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-09-28
class  KaliPi0Conf(LHCbConfigurableUser):
    """
    The main Configurable for Calorimeter Iterative Calibration with pi0

    The application produces:
    
    - ROOT NTuple for analysis
    - 'femto-DST' output, suibale for fast reprocessing 
    - monitoring histograms
    
    The usage is fairly trivial:

    # configuration of application:
    form Gaudi.Configuration import *  
    from Configurables       import KaliPi0Conf

    KaliPi0Conf (
    FirstPass = True    ,  ## set for the first pass on DST
    EvtMax    = 10000      ## number of events to process
    )

    # event input:
    EventSelector(
    Input = [ .. ] 
    )


    - Some reasonable configurations for (x)DST input ('First Pass')
    
    
    1. FirstPass = True , UseTracks = True  , UseSpd = False
    The 'standard' Calorimeter reconstruction, probably the best S/B condition
    
    2. FirstPass = True , UseTracks = True  , UseSpd = True
    The high pi0-efficiency (Track|Spd), the best usage of (x)DST
    
    3. FirstPass = True , UseTracks = False , UseSpd = True
    The fast recontruction, moderate S/B ratio, suitable for 'on-line' emulation
    
    4. FirstPass = True , UseTracks = False , UseSpd = False 
    Very fast recontruction, the highest efficiency and the worst S/B ratio, suitable for 'on-line' emulation


    - The reasonable configurations for femto-DST input ('Second Pass') 
            
    a. FirstPass = False , UseTracks = False , UseSpd = True
    The fast recontruction, moderate S/B ratio
    
    b. FirstPass = False , UseTracks = False , UseSpd = False
    Very fast recontruction, the worst S/B ratio

    
    """
    
    ## Explicitly Used Configurables
    __used_configurables__ = [
        'OffLineCaloRecoConf' ,
        'OffLineCaloPIDsConf' ,
        'CaloDstUnPackConf'   ,
        'DaVinci'
        ]
    
    ## the own slots 
    __slots__ = {
        ## Own flags:
        'FirstPass'             : False      ## The first (specific) pass on (x)DST ?
        , 'DestroyTES'          : True       ## Destroy TES containers 
        , 'Coefficients'        : {}         ## The map of (mis)calibration coefficients
        ## CaloReco Flags:
        , 'UseTracks'           : True       ## Use Tracks for the first pass ?
        , 'UseSpd'              : True       ## Use Spd as neutrality criteria ?
        , 'ForceDigits'         : False      ## Force Digits witgh Cluster Recontruction
        ## IO-related 
        , 'NTuple'              : 'KaliPi0_Tuples.root' ## The output NTuple-file
        , 'Histos'              : 'KaliPi0_Histos.root' ## The output Histo-file
        , 'FemtoDST'            : 'KaliPi0.fmDST'       ## The output femto-DST
        ## forwarded to DaVinci & other configurables 
        , 'EvtMax'              :  -1    ## Number of events to run (DaVinci)             
        , 'DataType'            : 'MC09' ## Data type               (DaVinci)
        , 'Hlt'                 : False  ## Hlt type                (DaVinci)
        , 'Simulation'          : True   ## Simulation              (DaVinci) 
        , 'MeasureTime'         : True   ## Measure the time for sequencers
        , 'OutputLevel'         : INFO   ## The global output level
        }
    ## documentation lines 
    _propertyDocDct = {
        ## Own flags 
        'FirstPass'             : """ The first (specific) pass on (x)DST ?"""
        , 'DestroyTES'          : """ Destroy TES containers ? """
        , 'Coefficients'        : """ The map of (mis)calibration coefficients """
        ## CaloReco flags 
        , 'UseTracks'           : """ Use Tracks for the first pass ? """
        , 'UseSpd'              : """ Use Spd as neutrality criteria ? """
        , 'ForceDigits'         : """ Force Digits witgh Cluster Recontruction """
        ## IO-related 
        , 'NTuple'              : """ The output NTuple-file """ 
        , 'Histos'              : """ The output Histo-file """
        , 'FemtoDST'            : """ The output femto-DST """
        ## DaVinci & Co configuration: 
        , 'EvtMax'              : """ Number of events to run (DaVinci) """
        , 'DataType'            : """ Data type               (DaVinci) """
        , 'Hlt'                 : """ Hlt                     (DaVinci) """
        , 'Simulation'          : """ Simulation              (DaVinci) """
        , 'MeasureTime'         : """ Measure the time for sequencers """
        , 'OutputLevel'         : """ The global output level """
        }
 
    ## 1. General Calorimeter Reconstruction Confifuration
    def caloConf ( self ) :
        """        
        Define the General Calorimeter Recontruction Configuration

        """
        ## General Calorimeter Reconstruction Configuration

        from Configurables  import OffLineCaloRecoConf
        from Configurables  import OffLineCaloPIDsConf
        
        if self.getProp( 'FirstPass' ) and self.getProp( 'UseTracks' ) :
            
            OffLineCaloRecoConf (
                EnableRecoOnDemand = True  ,
                UseTracks          = True  ,        ## Use Tracks For First Pass
                UseSpd             = self.getProp ( 'UseSpd'      ) ,
                ForceDigits        = self.getProp ( 'ForceDigits' ) ,
                MeasureTime        = self.getProp ( 'MeasureTime' ) ,
                OutputLevel        = self.getProp ( 'OutputLevel' ) )
                
        else :
            
            OffLineCaloRecoConf (
                EnableRecoOnDemand = True  ,
                UseTracks          = False ,         ## do not use tracks!
                UseSpd             = self.getProp ( 'UseSpd'      ) ,
                ForceDigits        = self.getProp ( 'ForceDigits' ) ,
                MeasureTime        = self.getProp ( 'MeasureTime' ) ,
                OutputLevel        = self.getProp ( 'OutputLevel' ) )
        
        OffLineCaloPIDsConf (
            EnablePIDsOnDemand = True                         ,
            MeasureTime        = self.getProp ( 'MeasureTime' ) ,
            OutputLevel        = self.getProp ( 'OutputLevel' ) )


    ## 2. (Optional) CaloDigit (mis)calibration
    def misCali ( self ) :
        """
        
        (Optional) CaloDigit (mis)calibration
        
        """
        if self.getProp ( 'Coefficients' ) :
            from Configurables import Kali__MisCalibrateCalo
            return Kali__MisCalibrateCalo (
                "KaliEcal" ,
                Coefficients = self.getProp('Coefficients' )
                )
        return None
        
    ## 3. "Light-mode" for Neutral ProtoParticles: no PIDs 
    def protos ( self ) :
        """
        'Light-mode' for Neutral ProtoParticles: no PIDs
        """
        ## 3. "Light-mode" for Neutral ProtoParticles: no PIDs         
        from GlobalReco.GlobalRecoConf import NeutralProtoPAlg
        log.warning("KaliPi0: \"Light\"-mode is activated for Neutral ProtoParticles") 
        return  NeutralProtoPAlg (
            LightMode      = True ,                  ## "ligght-mode", no PIDs
            HyposLocations = [ 'Rec/Calo/Photons']   ## only single photons 
            )

    ## 4. Specific settings for Photon Maker
    def photons ( self ) : 
        """
        Specific settings for Photon Maker
        """
        ## 4. Specific settings for Phtoton Maker 
        from ParticleMaker.ParticleMakerConf import PhotonMakerAlg, PhotonMaker
        from DaVinciTools.DaVinciToolsConf   import PhysDesktop 
        
        maker =  PhotonMakerAlg (
            'StdLooseAllPhotons' ,
            DecayDescriptor = 'Gamma'
            )

        maker.addTool ( PhotonMaker , name = 'PhotonMaker' )
        photon = maker.PhotonMaker
        photon.UseCaloTrMatch     = False
        photon.UseCaloDepositID   = False
        photon.UseShowerShape     = False
        photon.UsePhotonID        = False
        photon.ConvertedPhotons   = True 
        photon.UnconvertedPhotons = True 
        photon.PtCut              = 250 
        maker.addTool ( PhysDesktop )
        maker.PhysDesktop.InputPrimaryVertices = "None" ## NB: it saves a lot of CPU time 
        log.warning("KaliPi0: PID-info is disabled for PhotonMaker") 
        log.warning("KaliPi0: Primary Vertices are disabled for StdLooseAllPhotons") 
        
        return maker 

    ## 5. The configuration for Kali-Pi0 algorithm
    def kaliPi0 ( self ) :
        """
        The configuration for Kali-Pi0 algorithm
        """
        from Configurables import Kali__Pi0, PhysDesktop        
        kali = Kali__Pi0 (
            "Kali"                                         ,
            NTupleLUN      = "KALIPI0"                     ,
            HistoPrint     = True                          ,
            NTuplePrint    = True                          ,
            InputLocations = [ 'StdLooseAllPhotons' ]      ,
            Destroy        = self.getProp( 'DestroyTES'  ) ,
            OutputLevel    = self.getProp( 'OutputLevel' ) 
            )
        kali.addTool ( PhysDesktop )
        desktop = kali.PhysDesktop
        desktop.InputPrimaryVertices = "None"  ## NB: it saves a lot of CPU time!
        log.warning("KaliPi0: Primary Vertices are disabled for Kali") 
        
        if self.getProp( 'DestroyTES' ) : 
            log.warning("KaliPi0: TES containers will be destroyed")
        else :
            log.warning("KaliPi0: TES containers will be preserved")
                
        return kali 

    ## 7. The configuration of femtoDST
    def fmDst ( self ) :
        """
        The configurtaion of femto-DST
        """
        from Gaudi.Configuration import OutputStream
        return OutputStream (
            'WRITER', 
            ItemList =  [
            "/Event#1"            ,
            "/Event/DAQ#1"        ,
            "/Event/DAQ/ODIN#1"   ,
            #
            "/Event/Rec#1"        ,
            "/Event/pRec#1"       ,
            #
            "/Event/Raw#1"        ,
            "/Event/Raw/Spd#1"    ,
            "/Event/Raw/Prs#1"    ,
            "/Event/Raw/Ecal#1"   ,
            "/Event/Raw/Hcal#1"   ,
            "/Event/Raw/Spd/Digits#1"  ,
            "/Event/Raw/Prs/Digits#1"  ,
            "/Event/Raw/Ecal/Digits#1" ,
            "/Event/Raw/Hcal/Digits#1" 
            ] ,
            Output = "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='REC'" % self.getProp('FemtoDST') ,
            RequireAlgs = [ "Kali" ] 
            )

                
    ## Apply the configuration
    def applyConf (self):
        """
        Apply the configuration:

        - General CaloReconstructiion settings
        - (Optional) CaloDigit (mis)calibration
        - Light-mode for Neutral ProtoParticle Maker
        - Specific setting for Photon Maker
        - KaliPi0 itself
        - Configuration of DaVinci 
        - FemtoDST output stream
        - NTuples & Histos
        
        """
        log.info ( "KaliPi0Conf: ConfApplying Kali-pi0 configuration" )
        log.info (  self )

        ## 1. General Calorimeter Reconstruction configuration 
        self.caloConf() 

        ## 2. (Optional) CaloDigit (mis)calibration 
        misKali = self.misCali ()
        
        ## 3. "Light-Mode" for Neutral ProtoParticle maker
        proto   = self.protos  ()
        
        ## 4. Specific settings for Photon Maker
        photon  = self.photons ()
        
        ## 5. The configuration for Kali-Pi0 algorithm
        kali    = self.kaliPi0 ()

        ## 6. The general configuration of DaVinci
        from Gaudi.Configuration import GaudiSequencer

        kaliSeq = GaudiSequencer ( 'KaliSeq' )
        if not not misKali : kaliSeq.Members.append ( misKali )
        if not not proto   : kaliSeq.Members.append ( proto   )
        if not not photon  : kaliSeq.Members.append ( photon  )
        kaliSeq.Members.append ( kali   )
        
        kaliSeq.MeasureTime = self.getProp ( 'MeasureTime' )
        kaliSeq.OutputLevel = self.getProp ( 'OutputLevel' )
        
        from Configurables import DaVinci

        dv = DaVinci (
            UserAlgorithms = [ kaliSeq ] 
            , DataType       = self.getProp ('DataType'   ) 
            , Simulation     = self.getProp ('Simulation' ) 
            , EvtMax         = self.getProp ('EvtMax'     ) 
            , Hlt            = self.getProp ('Hlt'        )  
            ) 
        
        log.info( dv )
        
        ## 7. The configuration of femtoDST
        fmDST = self.fmDst()
        from Gaudi.Configuration import ApplicationMgr 
        mgr = ApplicationMgr ( OutStream = [ fmDST ] ) 

        ## 8. The configuration of NTuples & Histograms   
        from Gaudi.Configuration import NTupleSvc, HistogramPersistencySvc 
        NTupleSvc (
            Output = [ "KALIPI0 DATAFILE='%s' TYPE='ROOT' OPT='NEW'" % self.getProp('NTuple')] 
            )
        HistogramPersistencySvc ( OutputFile = self.getProp('Histos') ) 


# =============================================================================
## Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
def  action ( ) :
    """
    Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
    """
    dvinit = getConfigurable('DaVinciInitSeq')
    dvinit.Members = []
    log.warning ( 'KaliPi0Conf: DaVinciInitSeq is cleared!')

# =============================================================================
## Important: use Post Config action! 
appendPostConfigAction ( action )


if '__main__' == __name__ :
    print __author__
    print __version__
    print __doc__
    
# =============================================================================
# The END 
# =============================================================================
