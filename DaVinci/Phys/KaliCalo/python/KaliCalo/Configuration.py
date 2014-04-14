#!/usr/bin/env python
# =============================================================================
# $Id$
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
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
#
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
__version__ = " version $Revision$ "
# =============================================================================
# the only one  "vizible" symbol 
__all__  = (
    'KaliPi0Conf' ,                         ## the only one vizible symbol
    'firstPass'   , ## helper symbol to create the preconfigured application
    'secondPass'    ## helper symbol to create the preconfigured application
    )
# =============================================================================
from Gaudi.Configuration       import *
from LHCbKernel.Configuration  import *
from GaudiKernel.SystemOfUnits import GeV,MeV 
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
        , 'OtherAlgs'           : []    ## List of "other" algorithms to be run, e.g. electorn calibration
        , 'Mirror'              : False ## Use Albert's trick for combinatorial background evaluation
        , 'Histograms'          : False ## Create monitoring histograms
        , 'RecoAll'             : False ## Global Recontruction ?
        ## for first pass only
        , 'Pi0VetoDeltaMass'    : -1    ## mass-window for pi0-veto 
        , 'Pi0VetoChi2'         : -1    ## chi2        for pi0-veto
        , 'Filter'              : ''    ## event filter
        ## mis/re-calibration
        , 'KaliDB'              : {}    ## the map of { 'dbase' : 'bbase_name' , 'ecal' : 'key for Ecal' , 'prs' : 'key for Prs'}
        , 'Coefficients'        : {}    ## The map of (mis)calibration coefficients
        , 'PrsCoefficients'     : {}    ## The map of (mis)calibration coefficients for Prs 
        ## ``Physics''
        , 'PtGamma'             : 300 * MeV ## Pt-cut for photons 
        , 'Pi0Cut'              : " PT > 200 * MeV * ( 7 - ETA ) " ## Cut for pi0  
        , 'SpdCut'              : 0.1 * MeV ## Spd-cuts for photons 
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
        , 'NTupleProduce'       : True   ## Produce NTuples
        }
    ## documentation lines 
    _propertyDocDct = {
        ## Own flags 
        'FirstPass'             : """ The first (specific) pass on (x)DST ?"""
        , 'DestroyTES'          : """ Destroy TES containers """
        , 'DestroyList'         : """ The list of input TES-locations for Destroyer """
        , 'OtherAlgs'           : """ The list of 'other' algorithm to run, e.g. electron calibration """
        , 'Mirror'              : """ Use Albert's trick for combinatorial background evaluation """ 
        , 'Histograms'          : """ Activate monitoring histograms creation """
        , 'RecoAll'             : """ Global Reconstruction? """
        ## the first pass only 
        , 'Pi0VetoDeltaMass'    : """ Mass-window for pi0-veto """
        , 'Pi0VetoChi2'         : """ Chi2        for pi0-veto """ 
        , 'Filter'              : """ Void-filter to be used   """ 
        ## mis/re-calibration        
        , 'KaliDB'              : """ The map of { 'name' : 'bbase_name' , 'ecal' : 'key for Ecal' , 'prs' : 'key for Prs'} """
        , 'Coefficients'        : """ The map of (mis)calibration coefficients """
        , 'PrsCoefficients'     : """ The map of (mis)calibration coefficients for Prs """
        ## ``Physics''
        , 'PtGamma'             : """ Pt-cut for photons """
        , 'Pi0Cut'              : """ Cut for pi0 (LoKi/Bender expression)""" 
        , 'SpdCut'              : """ Spd-cuts for photons """ 
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
        , 'NTupleProduce'       : """ Produce calibration NTuples """
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
                ## UsePrs             = self.getProp ( 'UsePrs'      ) ,
                ForceDigits        = self.getProp ( 'ForceDigits' ) ,
                MeasureTime        = self.getProp ( 'MeasureTime' ) ,
                OutputLevel        = self.getProp ( 'OutputLevel' ) )
                
        else :
            
            OffLineCaloRecoConf (
                EnableRecoOnDemand = True  ,
                UseTracks          = False ,         ## do not use tracks!
                UseSpd             = self.getProp ( 'UseSpd'      ) ,
                ## UsePrs             = self.getProp ( 'UsePrs'      ) ,
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
        
        import KaliCalo.Kali      as Kali
        import KaliCalo.ZipShelve as ZipShelve
        
        ecal  = {}
        prs   = {}

        ## DBASE has been specified 
        if self.getProp('KaliDB') :
            kali_db = self.getProp('KaliDB')
            if not kali_db.has_key('name') :
                raise AttributeError, "KaliDB property has no 'name' atribute "
            # open data base 
            dbase = ZipShelve.open ( kali_db['name'] , 'r' )
            if kali_db.has_key ( 'ecal') : ecal = dbase[ kali_db [ 'ecal'] ]
            if kali_db.has_key ( 'prs' ) : prs  = dbase[ kali_db [ 'prs' ] ]
            dbase.close()
            
        ##
        if ecal and self.getProp ( 'Coefficients' ) :
            raise ArrtibuteError, 'Ecal coefficients & data base key are exclusive ' 
        if self.getProp ( 'Coefficients' ) :
            ecal = self.getProp ( 'Coefficients' )
        ##
        if prs and self.getProp ( 'PrsCoefficients' ) :
            raise ArrtibuteError, 'Prs coefficients & data base key are exclusive ' 
        if self.getProp ( 'PrsCoefficients' ) :
            prs = self.getProp ( 'PrsCoefficients' )

        if ecal :
            ## fix to allow 'pickable' configuration 
            _ecal = {}
            for key in ecal :
                newkey = ( 'Ecal' , key.areaName () , int(key.row()) , int(key.col ()) ) 
                _ecal [ newkey ] = ecal [ key ]
            from Configurables import Kali__MisCalibrateCalo
            alg = Kali__MisCalibrateCalo ( "KaliEcal" , Coefficients = _ecal )
            lst += [ alg ]
            
        ##
        if prs :
            ## fix to allow 'pickable' configuration 
            _prs = {}
            for key in prs :
                newkey = ( 'Prs' , key.areaName () , int(key.row()) , int(key.col ()) ) 
                _prs [ newkey ] = prs [ key ] 
            from Configurables import Kali__MisCalibrateCalo
            alg = Kali__MisCalibrateCalo ( "KaliPrs" , Coefficients = _prs )
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
        from Configurables import Kali__Pi0
        
        kali = Kali__Pi0 (
            "KaliPi0"                                       ,
            ## specific cuts :
            Cuts = { 'PtGamma' : self.getProp ( 'PtGamma' ) ,
                     'SpdCut'  : self.getProp ( 'SpdCut'  ) } ,
            ## cut for pi0 :
            Pi0Cut         = self.getProp ( 'Pi0Cut' )      ,
            ## general configuration 
            NTupleLUN      = "KALIPI0"                      ,
            HistoPrint     = True                           ,
            NTuplePrint    = True                           ,
            Inputs         = [ 'Phys/StdLooseAllPhotons' ]  ,
            OutputLevel    = self.getProp ( 'OutputLevel' ) ,
            Mirror         = self.getProp ( 'Mirror'      ) , 
            HistoProduce   = self.getProp ( 'Histograms'  ) ,
            NTupleProduce  = self.getProp ( 'NTupleProduce' )
            )
        
        if self.getProp ('Mirror' ) :
            _log.warning ("KaliPi0: Albert's trick is   activated") 
        else :
            _log.warning ("KaliPi0: Albert's trick is deactivated")
            
        if self.getProp('FemtoDST'):
            _log.warning ("KaliPi0: FemtoDSTs will be produced")
        else:
            _log.warning ("KaliPi0: NO FemtoDSTs will be produced!!!")

        if self.getProp ( 'Histograms' ) :
            _log.warning ( "KaliPi0: Monitoring histograms are   activated") 
        else :
            _log.warning ( "KaliPi0: Monitoring histograms are deactivated")

        if self.getProp ( 'NTupleProduce' ) :
            _log.warning ( "KaliPi0: Creation of calibration NTuples is   activated")
        else :
            _log.warning ( "KaliPi0: Creation of calibration NTuples is deactivated")
            
        if self.getProp ( 'FirstPass' ) :
            
            if 0 <= self.getProp ('Pi0VetoDeltaMass') :
                _dm   = self.getProp ( 'Pi0VetoDeltaMass' ) 
                _log.warning ("KaliPi0: Pi0Veto is activated DM   =%s" % _dm   ) 
                kali.Pi0VetoDeltaMass = _dm
                
            if 0 <= self.getProp ('Pi0VetoChi2') :
                _chi2 = self.getProp ( 'Pi0VeloChi2' ) 
                _log.warning ("KaliPi0: Pi0Veto is activated CHI2 =%s" % _chi2 ) 
                kali.Pi0VetoChi2      = _chi2
                
        kali.InputPrimaryVertices = 'None'  ## NB: it saves a lot of CPU time!
        _log.warning("KaliPi0: Primary Vertices are disabled for Kali") 
        
        return kali 

    ## 7. The configuration of femtoDST
    def fmDst ( self ) :
        """
        The configuration of femto-DST
        """
        from Gaudi.Configuration import OutputStream
        
        items =  [
            # general 
            "/Event#1"                 ,
            "/Event/DAQ#1"             ,
            "/Event/DAQ/ODIN#1"        ,
            # event header for time decoder
            "/Event/Rec#1"             ,
            "/Event/Rec/Header#1"      ,
            ## GEC counters 
            "/Event/Counters#1"        , 
            "/Event/Counters/Kali#1"
            ]
        calos = [
            # calorimeter 
            "/Event/Raw#1"             , 
            "/Event/Raw/Spd#1"         ,
            "/Event/Raw/Spd/Digits#1"  ,
            "/Event/Raw/Prs#1"         ,
            "/Event/Raw/Prs/Digits#1"  ,
            "/Event/Raw/Ecal#1"        ,
            "/Event/Raw/Ecal/Digits#1" ,
            "/Event/Raw/Hcal#1"        ,
            "/Event/Raw/Hcal/Digits#1" 
            ]
        tracks = [
            # tracks, e.g. for electrons  
            "/Event/Rec#1"             ,
            "/Event/Rec/Track#1"       ,
            "/Event/Rec/Track/Best#1"  ,
            ] 
        
        if not self.getProp ( 'DestroyTES' ) :
            calos  = [ i.replace('/Event/Raw', '/Event/Kali/Raw') for i in calos  ]
            tracks = [ i.replace('/Event/Rec', '/Event/Kali/Rec') for i in tracks ]

        item_list  = items + calos + tracks 
        
        writer = OutputStream (
            'FMDST' , 
            ItemList    = item_list ,
            # 
            AcceptAlgs  = self.getProp ( 'DestroyList' ) ,
            RequireAlgs =              [ 'Destroyer'   ] ,
            )

        
        from GaudiConf.IOHelper import IOHelper
        ioh = IOHelper()

        return ioh.outputAlgs ( self.getProp ( 'FemtoDST' ) ,
                                writer                      ,
                                writeFSR = False            )
    
                    
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

        if self.getProp ( 'FirstPass') :
            from Configurables import LoKi__CounterAlg as CounterAlg
            cnt = CounterAlg( 
                'KaliCounters'   , 
                Location  = "Counters/Kali" , 
                Preambulo = [ "from LoKiTracks.decorators import *"],
                Variables = {
                    "nSpd"          : "CONTAINS ( 'Raw/Spd/Digits' )                " ,
                    "nVelo"         : "TrNUM    ( 'Rec/Track/Best' , TrVELO       ) " ,
                    "nLong"         : "TrNUM    ( 'Rec/Track/Best' , TrLONG       ) " ,
                    "nPV"           : "CONTAINS ( 'Rec/Vertex/Primary'    ) "         ,
                    "nOT"           : "CONTAINS ( 'Raw/OT/Times'          ) "         ,
                    "nITClusters"   : "CONTAINS ( 'Raw/IT/Clusters'       ) "         ,
                    "nTTClusters"   : "CONTAINS ( 'Raw/TT/Clusters'       ) "         ,
                    "nVeloClusters" : "CONTAINS ( 'Raw/Velo/Clusters'     ) "         ,
                    "nEcalClusters" : "CONTAINS ( 'Rec/Calo/EcalClusters' ) "         ,
                    "nEcalDigits"   : "CONTAINS ( 'Raw/Ecal/Digits'       ) "         ,
                    }
                )
            kaliSeq.Members += [ cnt ] 

        if not not proto   : kaliSeq.Members += [ proto  ]
        if not not photon  : kaliSeq.Members += [ photon ]

        kaliSeq.Members += [ kali ]

        ## collect the actual sequence of algorithms:
        algs = []
        
        if not self.getProp ( 'FirstPass') :
            from Configurables import Kali__MakeDir
            algs += [ Kali__MakeDir  ( 'DirMaker' ) ]
            from Configurables import Kali__DataMove
            algs += [ Kali__DataMove ( 'DataMove' ) ]
            
        algs += [ kaliSeq ]

        # run 'Other' algorithms ? 
        if self.getProp('OtherAlgs') : algs += self.getProp('OtherAlgs') 
        
        ## 7. Destroy TES if needed 
        if self.getProp ( 'DestroyTES' ) or self.getProp ( 'DestroyList' ) :
            from Configurables import Kali__Destroyer, Kali__Cloner
            tesList = [ 'Phys/' + loc + '/Particles' for loc in self.getProp('DestroyList') ]

            
            Processor = Kali__Destroyer if self.getProp ( 'DestroyTES' ) else Kali__Cloner 
            destroyer = Processor (
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
            _log.warning ( "KaliPi0: TES containers will be destroyed " )
        else :
            _log.warning ( "KaliPi0: TES containers will be copied    " )        
            

        from Configurables import DaVinci

        # unpacking is enabled only for first pass on DST 
        unPack = self.getProp (  'FirstPass' )

        fltrs  = []
        if self.getProp('FirstPass' ) :
            _fltr = self.getProp('Filter')
            if _fltr :
                from Configurables import LoKi__VoidFilter as Filter
                fltr = Filter ( 'KaliFilter' ,
                                Code = _fltr )
                fltr.Preambulo += [
                    "from LoKiTracks.decorators  import *" , 
                    "from LoKiCore.functions     import *"
                    ]
                fltrs.append(fltr)    
                _log.warning('KaliCalo: Event Filter is used \n%s' % _fltr )
                
                
        dv = DaVinci (
            EventPreFilters = fltrs                         ,
            UserAlgorithms  = algs                          ,
            DataType        = self.getProp ( 'DataType'   ) ,
            Simulation      = self.getProp ( 'Simulation' ) ,
            EvtMax          = self.getProp ( 'EvtMax'     ) ,
            PrintFreq       = self.getProp ( 'PrintFreq'  ) ,
            Lumi            = False                         ,
            EnableUnpack    = []
            )
        if unPack : dv.EnableUnpack = ["Reconstruction"]
        
        ## 7. The configuration of femtoDST
        if self.getProp('FemtoDST'):
            fmDST = self.fmDst()
            from Gaudi.Configuration import ApplicationMgr 
            
            ApplicationMgr ( OutStream = fmDST )     
            
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
        #dod = DataOnDemandSvc  ( Dump = True )
        dod = DataOnDemandSvc  ( Dump = False )

        ## 10. postConfig
        if not self.getProp('FirstPass'):
            appendPostConfigAction ( action )



## =============================================================================
## define "typical" first pass configuration  
def firstPass ( **args ) :
    """
    Define ``typical'' first pass configuration

    Usage:
    
    >>> kali = firstPass ( ..... )
    
    """
    
    kali = KaliPi0Conf (
        ##
        FirstPass        = True ,
        ##
        UseTracks        = args.pop ( 'UseTracks'        , True     ) ,
        UseSpd           = args.pop ( 'UseSpd'           , True     ) ,
        UsePrs           = args.pop ( 'UsePrs'           , False    ) ,
        Mirror           = args.pop ( 'Mirror'           , True     ) , 
        Histograms       = args.pop ( 'Histograms'       , True     ) ,
        ##
        Pi0VetoDeltaMass = args.pop ( 'Pi0VetoDeltaMass' ,  -1      ) ,
        Pi0VetoChi2      = args.pop ( 'Pi0VetoChi2'      ,  -1      ) ,
        ##
        Filter           = args.pop ( 'Filter'           , """
        ( 200 > monitor ( CONTAINS ( 'Raw/Spd/Digits'    ) , '#SPD'   , 1 ) ) & 
        ( 2.  > monitor ( CONTAINS ( 'Rec/Vertex/Primary') , '#PV'    , 1 ) )
        """ ) , ## event filter
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
        ##
        FirstPass  = False ,
        ##
        UseTracks  = args.pop ( 'UseTracks'  , False ) ,
        UseSpd     = args.pop ( 'UseSpd'     , False ) ,
        UsePrs     = args.pop ( 'UsePrs'     , False ) ,
        Mirror     = args.pop ( 'Mirror'     , True  ) , 
        Histograms = args.pop ( 'Histograms' , True  ) ,
        **args 
        )
    
    return kali
        

# =============================================================================
## Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
def  action ( ) :
    """
    Reset DV-init sequence. IMPORTANT: It saves a lot of CPU time!!!
    """

    from Gaudi.Configuration import allConfigurables

    ## reser DaVinci sequences:
    for seq in ( 'DaVinciInitSeq'      ,
                 'DaVinciEventInitSeq' ,
                 'DaVinviEventInitSeq' ,
                 'MonitoringSeq'       ,
                 'LumiSeq'             ,
                 'IntegratorSeq'       ) : 
        if not seq in allConfigurables : continue
        iseq = getConfigurable ( seq )
        if iseq and hasattr ( iseq , 'Members' ) :
            iseq.Members = []
            _log.warning ( 'KaliPi0Conf: Sequence %s is cleared ' % seq )

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
