#!/usr/bin/env python
# =============================================================================
# $Id: Configuration.py,v 1.14 2010-03-18 11:14:23 ibelyaev Exp $
# =============================================================================
# @file  KaliCalo/Configuration.py
#
# The basic configuration for Calorimeter Calibration
#
# The application produces:
# 
#  - ROOT NTuple for analysis
#  - 'femto-DST' output, suibale for fast reprocessing 
#  - monitoring histograms
#
# The usage is fairly trivial:
#
#    # configure the application:
#
#    from Gaudi.Configuration import * 
#    from Configurables       import KaliPi0Conf
#
#    KaliPi0Conf (
#    FirstPass = True    ,  ## set for the first pass on DST
#    EvtMax    = 10000      ## number of events to process
#    )
#
#    # define input data :
#    EventSelector(
#    Input = [ .. ] 
#    )
#
# Or one can rely on helper functions:
#
#     from KaliPi0.Configuration import firstPass, secondPass  
#
#
#     kali = firstPass ( EvtMax = 1000 )
#
#     # define input data :
#     EventSelector(
#     Input = [ .. ] 
#     )
#
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

Or one can rely on helper functions:

     from KaliPi0.Configuration import firstPass, secondPass  

     kali = firstPass ( EvtMax = 1000 )

     # define input data :
     EventSelector(
     Input = [ .. ] 
     )

"""
# =============================================================================
__author__  = " Vanya BELYAEV Ivan.Belyaev@nikhef.nl "
__date__    = " 2009-09-28 "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.14 $ "
# =============================================================================
# the only one  "vizible" symbol 
__all__  = (
    'KaliPi0Conf' ,                         ## the only one vizible symbol
    'firstPass'   , ## helper symbol to create the preconfigured application
    'secondPass'    ## helper symbol to create the preconfigured application
    )
# =============================================================================
from Gaudi.Configuration      import *
from LHCbKernel.Configuration import *

import logging

_log = logging.getLogger('KaliCalo')

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
        'GlobalRecoConf'      ,
        'DaVinci'
        ]

    ## the own slots 
    __slots__ = {
        ## Own flags:
        'FirstPass'             : False ## The first (specific) pass on (x)DST ?
        , 'DestroyTES'          : True  ## Destroy TES containers : List of Input Partcle Containers
        , 'DestroyList'         : ['KaliPi0' ]  ## The list of input TES-location for Destroyer
        , 'Coefficients'        : {}    ## The map of (mis)calibration coefficients
        , 'PrsCoefficients'     : {}    ## The map of (mis)calibration coefficients for Prs 
        , 'OtherAlgs'           : []    ## List of "other" algorithms to be run, e.g. electorn calibration
        , 'Mirror'              : False ## Use Albert's trick for combinatorial background evaluation
        , 'Histograms'          : False ## Create monitoring histograms
        , 'RecoAll'             : False ## Global Recontruction ?
        ## CaloReco Flags:
        , 'UseTracks'           : True  ## Use Tracks for the first pass ?
        , 'UseSpd'              : True  ## Use Spd as neutrality criteria ?
        , 'UsePrs'              : False ## Use Prs for photon selection ?
        , 'ForceDigits'         : False ## Force Digits witgh Cluster Recontruction
        ## IO-related 
        , 'NTuple'              : 'KaliPi0_Tuples.root' ## The output NTuple-file
        , 'Histos'              : 'KaliPi0_Histos.root' ## The output Histo-file
        , 'FemtoDST'            : 'KaliPi0.fmDST'       ## The output femto-DST
        ## forwarded to DaVinci & other configurables 
        , 'EvtMax'              :  -1    ## Number of events to run (DaVinci)             
        , 'DataType'            : '2009' ## Data type               (DaVinci)
        , 'Simulation'          : False  ## Simulation              (DaVinci) 
        , 'MeasureTime'         : True   ## Measure the time for sequencers
        , 'OutputLevel'         : INFO   ## The global output level
        , 'PrintFreq'           : 100000 ## The print frequency
        }
    ## documentation lines 
    _propertyDocDct = {
        ## Own flags 
        'FirstPass'             : """ The first (specific) pass on (x)DST ?"""
        , 'DestroyTES'          : """ Destroy TES containers """
        , 'DestroyList'         : """ The list of input TES-locations for Destroyer """
        , 'Coefficients'        : """ The map of (mis)calibration coefficients """
        , 'PrsCoefficients'     : """ The map of (mis)calibration coefficients for Prs """
        , 'OtherAlgs'           : """ The list of 'other' algorithm to run, e.g. electron calibration """
        , 'Mirror'              : """ Use Albert's trick for combinatorial background evaluation """ 
        , 'Histograms'          : """ Activate monitoring histograms creation """
        , 'RecoAll'             : """ Global Reconstruction? """ 
        ## CaloReco flags 
        , 'UseTracks'           : """ Use Tracks for the first pass ? """
        , 'UseSpd'              : """ Use Spd as neutrality criteria ? """
        , 'UsePrs'              : """ Use Prs for photon selection ? """
        , 'ForceDigits'         : """ Force Digits witgh Cluster Recontruction """
        ## IO-related 
        , 'NTuple'              : """ The output NTuple-file """ 
        , 'Histos'              : """ The output Histo-file """
        , 'FemtoDST'            : """ The output femto-DST """
        ## DaVinci & Co configuration: 
        , 'EvtMax'              : """ Number of events to run (DaVinci) """
        , 'DataType'            : """ Data type               (DaVinci) """
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
        from Configurables  import CaloDstUnPackConf 

        if self.getProp( 'FirstPass' ) and self.getProp( 'UseTracks' ) :

            OffLineCaloRecoConf (
                EnableRecoOnDemand = True  ,
                UseTracks          = True  ,        ## Use Tracks For First Pass
                UseSpd             = self.getProp ( 'UseSpd'      ) ,
                UsePrs             = self.getProp ( 'UsePrs'      ) ,
                ForceDigits        = self.getProp ( 'ForceDigits' ) ,
                MeasureTime        = self.getProp ( 'MeasureTime' ) ,
                OutputLevel        = self.getProp ( 'OutputLevel' ) )
                
        else :
            
            OffLineCaloRecoConf (
                EnableRecoOnDemand = True  ,
                UseTracks          = False ,         ## do not use tracks!
                UseSpd             = self.getProp ( 'UseSpd'      ) ,
                UsePrs             = self.getProp ( 'UsePrs'      ) ,
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
        lst = []
        ##
        if self.getProp ( 'Coefficients' ) :
            from Configurables import Kali__MisCalibrateCalo
            alg = Kali__MisCalibrateCalo (
                "KaliEcal" ,
                Coefficients = self.getProp('Coefficients' ) )
            lst += [ alg ]
        ##
        if self.getProp ( 'PrsCoefficients' ) :
            from Configurables import Kali__MisCalibrateCalo
            alg = Kali__MisCalibrateCalo (
                "KaliPrs" ,
                Coefficients = self.getProp( 'PrsCoefficients' ) )
            lst += [ alg ]
        #
        return lst 
        
    ## 3. "Light-mode" for Neutral ProtoParticles: no PIDs 
    def protos ( self ) :
        """
        'Light-mode' for Neutral ProtoParticles: no PIDs
        """
        ## 3. "Light-mode" for Neutral ProtoParticles: no PIDs         
        from GlobalReco.GlobalRecoConf import NeutralProtoPAlg
        _log.warning("KaliPi0: \"Light\"-mode is activated for Neutral ProtoParticles") 
        return  NeutralProtoPAlg (
            "NeutralProtoPMaker"  , 
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
        
        maker.InputPrimaryVertices = 'None' ## NB: it saves a lot of CPU time 
        _log.warning ( "KaliPi0: PID-info is disabled for PhotonMaker") 
        _log.warning ( "KaliPi0: Primary Vertices are disabled for StdLooseAllPhotons") 
        
        return maker 

    ## 5. The configuration for Kali-Pi0 algorithm
    def kaliPi0 ( self ) :
        """
        The configuration for Kali-Pi0 algorithm
        """
        from Configurables import Kali__Pi0, PhysDesktop        
        kali = Kali__Pi0 (
            "KaliPi0"                                       ,
            NTupleLUN      = "KALIPI0"                      ,
            HistoPrint     = True                           ,
            NTuplePrint    = True                           ,
            InputLocations = [ 'StdLooseAllPhotons' ]       ,
            OutputLevel    = self.getProp ( 'OutputLevel' ) ,
            Mirror         = self.getProp ( 'Mirror'      ) , 
            HistoProduce   = self.getProp ( 'Histograms'  )
            )
        if self.getProp ('Mirror' ) :
            _log.warning ("KaliPi0: Albert's trick is   activated") 
        else :
            _log.warning ("KaliPi0: Albert's trick is deactivated") 
        if self.getProp ( 'Histograms' ) :
            _log.warning ( "KaliPi0: Monitoring histograms are   activated") 
        else :
            _log.warning ( "KaliPi0: Monitoring histograms are deactivated") 
            
        kali.InputPrimaryVertices = 'None'  ## NB: it saves a lot of CPU time!
        _log.warning("KaliPi0: Primary Vertices are disabled for Kali") 
        
        return kali 

    ## 7. The configuration of femtoDST
    def fmDst ( self ) :
        """
        The configuration of femto-DST
        """
        from Gaudi.Configuration import OutputStream

        return OutputStream (
            'FMDST', 
            ItemList =  [
            # general 
            "/Event#1"                 ,
            "/Event/DAQ#1"             ,
            "/Event/DAQ/ODIN#1"        ,
            # calorimeter 
            "/Event/Raw#1"             , 
            "/Event/Raw/Spd#1"         ,
            "/Event/Raw/Spd/Digits#1"  ,
            "/Event/Raw/Prs#1"         ,
            "/Event/Raw/Prs/Digits#1"  ,
            "/Event/Raw/Ecal#1"        ,
            "/Event/Raw/Ecal/Digits#1" ,
            "/Event/Raw/Hcal#1"        ,
            "/Event/Raw/Hcal/Digits#1" ,
            # tracks, e.g. for electrons  
            "/Event/Rec#1"             ,
            "/Event/Rec/Track#1"       ,
            "/Event/Rec/Track/Best#1"  
            ] ,
            # 
            Output = "DATAFILE='PFN:%s' TYP='POOL_ROOTTREE' OPT='REC'" % self.getProp('FemtoDST')
            , AcceptAlgs  = self.getProp ( 'DestroyList' )
            , RequireAlgs =              [ 'Destroyer' ] 
            )

                
    ## Apply the configuration
    def __apply_configuration__ (self):
        """
        Apply the configuration:

        - General CaloReconstruction settings
        - (Optional) CaloDigit (mis)calibration
        - Light-mode for Neutral ProtoParticle Maker
        - Specific setting for Photon Maker
        - KaliPi0 itself
        - Configuration of DaVinci 
        - FemtoDST output stream
        - NTuples & Histos
        
        """
        _log.info ( "KaliPi0Conf: Applying KaliPi0 configuration" )
        _log.info (  self )

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
        from Configurables import GaudiSequencer,DaVinciInit


        kaliSeq = GaudiSequencer ( 'KaliSeq' )
        
        kaliSeq.MeasureTime = self.getProp ( 'MeasureTime' )
        kaliSeq.OutputLevel = self.getProp ( 'OutputLevel' )

        kaliSeq.Members = [ ]
        
        if not not misKali : kaliSeq.Members += misKali 
            
        if self.getProp ( 'RecoAll' ) :
            from Configurables import GlobalRecoConf
            kaliReco = GaudiSequencer("KaliReco")
            kaliSeq.Members += [ kaliReco ] 
            GlobalRecoConf ( TrackTypes    = [ 'Long'] ,
                             RecoSequencer = kaliReco  )
            
        if not not proto   : kaliSeq.Members += [ proto  ]
        if not not photon  : kaliSeq.Members += [ photon ]

        kaliSeq.Members += [ kali ]

        ## collect the actual sequence of algorithms:
        algs = []
        
        if not self.getProp ( 'FirstPass') :
            from Configurables import Kali__MakeDir
            algs += [ Kali__MakeDir( 'DirMaker' ) ]
            
        algs += [ kaliSeq ]

        # run 'Other' algorithms ? 
        if self.getProp('OtherAlgs') : algs += self.getProp('OtherAlgs') 
        
        ## 7. Destroy TES if needed 
        if self.getProp ( 'DestroyTES' ) or self.getProp ( 'DestroyList' ) :
            from Configurables import Kali__Destroyer
            tesList = [ 'Phys/' + loc + '/Particles' for loc in self.getProp('DestroyList') ]
            destroyer = Kali__Destroyer (
                'Destroyer'                             ,
                Particles = tesList                     , 
                Destroy   = self.getProp('DestroyTES' )  
                )
            from Configurables import  ( SpdEnergyForTrack  ,
                                         PrsEnergyForTrack  , 
                                         EcalEnergyForTrack ,
                                         HcalEnergyForTrack )
            
            destroyer.addTool ( SpdEnergyForTrack  , 'SpdDigits'  )
            destroyer.addTool ( PrsEnergyForTrack  , 'PrsDigits'  )
            destroyer.addTool ( EcalEnergyForTrack , 'EcalDigits' )
            destroyer.addTool ( HcalEnergyForTrack , 'HcalDigits' )

            destroyer.addTool ( SpdEnergyForTrack  , 'BremSpdDigits'  )
            destroyer.addTool ( PrsEnergyForTrack  , 'BremPrsDigits'  )
            destroyer.addTool ( EcalEnergyForTrack , 'BremEcalDigits' )

            destroyer.SpdDigits      .AddNeighbours = 1   ## 3x3
            destroyer.PrsDigits      .AddNeighbours = 2   ## 5x5 
            destroyer.EcalDigits     .AddNeighbours = 3   ## 7x7 
            destroyer.HcalDigits     .AddNeighbours = 1   ## 3x3 

            destroyer.Digits4Track = [
                destroyer.SpdDigits   ,
                destroyer.PrsDigits   ,
                destroyer.EcalDigits  ,
                destroyer.HcalDigits  
                ]
            
            destroyer.BremSpdDigits  .AddNeighbours = 2   ## 3x3
            destroyer.BremPrsDigits  .AddNeighbours = 2   ## 5x5 
            destroyer.BremEcalDigits .AddNeighbours = 3   ## 7x7
            
            destroyer.Digits4Track = [
                destroyer.BremSpdDigits   ,
                destroyer.BremPrsDigits   ,
                destroyer.BremEcalDigits  
                ]
            
            algs += [ destroyer ]
            
        if self.getProp( 'DestroyTES' ) :
            _log.warning ( "KaliPi0: TES containers will be destroyed" )
        else :
            _log.warning ( "KaliPi0: TES containers will be preserved" )        
            

        from Configurables import DaVinci

        # unpacking is enabled only for first pass on DST 
        unPack = self.getProp (  'FirstPass' )
        
        dv = DaVinci (
            UserAlgorithms = algs                          ,
            DataType       = self.getProp ( 'DataType'   ) ,
            Simulation     = self.getProp ( 'Simulation' ) ,
            EvtMax         = self.getProp ( 'EvtMax'     ) ,
            PrintFreq      = self.getProp ( 'PrintFreq'  ) ,
            EnableUnpack   = unPack 
            ) 

        ## 7. The configuration of femtoDST
        fmDST = self.fmDst()
        from Gaudi.Configuration import ApplicationMgr 
        
        ApplicationMgr ( OutStream = [ fmDST ] )     
            
        ## 8. The configuration of NTuples & Monitoring Histograms   
        from Gaudi.Configuration import NTupleSvc, HistogramPersistencySvc
        output = NTupleSvc().Output
        
        NTupleSvc ().Output += [
            "KALIPI0 DATAFILE='%s' TYPE='ROOT' OPT='NEW'" % self.getProp('NTuple')
            ]
        if ( self.getProp ( 'Histograms' ) ): 
          HistogramPersistencySvc ( OutputFile = self.getProp('Histos') ) 

        ## 9. the final decoration
        from Configurables import DataOnDemandSvc
        dod = DataOnDemandSvc  ( Dump = True )


## =============================================================================
## define "typical" first pass configuration  
def firstPass ( **args ) :
    """
    Define ``typical'' first pass configuration

    Usage:
    
    >>> kali = firstPass ( ..... )
    
    """
    kali = KaliPi0Conf (
        FirstPass  = True ,
        UseTracks  = args.get ( 'UseTracks'  , True  ) ,
        UseSpd     = args.get ( 'UseSpd'     , True  ) ,
        UsePrs     = args.get ( 'UsePrs'     , False ) ,
        Mirror     = args.get ( 'Mirror'     , True  ) , 
        Histograms = args.get ( 'Histograms' , True  ) ,
        **args 
        )
    
    return kali
    
## =============================================================================
## define "typical" second pass configuration  
def secondPass ( **args ) :
    """
    Define ``typical'' second pass configuration
    
    Usage:
    
    >>> kali = firstPass ( ..... )
    
    """
    
    kali = KaliPi0Conf (
        FirstPass  = False ,
        UseTracks  = args.get ( 'UseTracks'  , False ) ,
        UseSpd     = args.get ( 'UseSpd'     , False ) ,
        UsePrs     = args.get ( 'UsePrs'     , False ) ,
        Mirror     = args.get ( 'Mirror'     , True  ) , 
        Histograms = args.get ( 'Histograms' , True  ) ,
        **args 
        )
    
    return kali
    

    

# =============================================================================
## Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
def  action ( ) :
    """
    Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
    """
    dvInitSeq = getConfigurable('DaVinciInitSeq')
    dvInitSeq.Members = []
        
    _log.warning ( 'KaliPi0Conf: DaVinciInitSeq is cleared!')
    
    
# =============================================================================
## Important: use Post Config action! 
appendPostConfigAction ( action )


## temporary solve the problem with atexit/__del__ for AppMgr 
def _KaliAtExit_ () :
    """
    Temporary solve the problme with atexit/__del__ for AppMgr 
    """
    from GaudiPython.Bindings import _gaudi 
    if not not _gaudi : _gaudi.exit()


import atexit
atexit.register ( _KaliAtExit_ )

# =============================================================================
if '__main__' == __name__ :
    
    print '*'*120
    print                      __doc__
    print ' Author  : %s ' %   __author__    
    print ' Version : %s ' %   __version__
    print ' Date    : %s ' %   __date__
    print '*'*120  
    
# =============================================================================
# The END 
# =============================================================================
