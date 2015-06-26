"""
High level configuration tools for Gauss
Beampipe configuration added.
"""
__version__ = "$Id: Configuration.py,v 1.30 2010/05/09 18:14:28 gcorti Exp $"
__author__  = "Gloria Corti <Gloria.Corti@cern.ch>"

from Gaudi.Configuration import *
import GaudiKernel.ProcessJobOptions
from GaudiKernel import SystemOfUnits
from Configurables import LHCbConfigurableUser, LHCbApp, SimConf


# CRJ - Its un-neccessary to import everythting by default. Better to
#       import as and when you need it ...
from Configurables import ( CondDBCnvSvc, EventClockSvc, FakeEventTime,
                            CondDBEntityResolver )
from Configurables import ( GenInit, Generation, MinimumBias, Inclusive,
#from Configurables import ( GenInit, Generation,
# Double imports
                            SignalPlain, SignalRepeatedHadronization,
                            SignalForcedFragmentation, StandAloneDecayTool,
                            Special,
                            PythiaProduction, HijingProduction,
# Not used
                            CollidingBeams, FixedTarget,
                            BeamSpotSmearVertex, FlatZSmearVertex,
                            EvtGenDecay )
from Configurables import ( SimInit, GiGaGeo, GiGaInputStream, GiGa,
                            GiGaDataStoreAlgorithm,
                            GiGaPhysListModular, GiGaRunActionSequence,
                            TrCutsRunAction, GiGaRunActionCommand,
                            GiGaEventActionSequence, GiGaMagFieldGlobal,
                            GiGaTrackActionSequence, GaussPostTrackAction,
                            GiGaStepActionSequence, SimulationSvc,
                            GiGaFieldMgr, GiGaRunManager, GiGaSetSimAttributes,
                            GiGaPhysConstructorOp, GiGaPhysConstructorHpd,
                            SpdPrsSensDet, EcalSensDet, HcalSensDet,
                            GaussSensPlaneDet )
from Configurables import ( GenerationToSimulation, GiGaFlushAlgorithm,
                            GiGaCheckEventStatus, SimulationToMCTruth,
                            GiGaGetEventAlg, GiGaGetHitsAlg,
                            GetTrackerHitsAlg, GetCaloHitsAlg, 
                            GetMCRichHitsAlg, GetMCRichOpticalPhotonsAlg,
                            GetMCRichSegmentsAlg, #GetMCRichTracksAlg,
                            Rich__MC__MCPartToMCRichTrackAlg,
                            Rich__MC__MCRichHitToMCRichOpPhotAlg)
from Configurables import ( GenMonitorAlg, MuonHitChecker, MCTruthMonitor,
                            VeloGaussMoni, MCHitMonitor, MCCaloMonitor,
                            DumpHepMC )
from Configurables import ( PackMCParticle, PackMCVertex,
                            UnpackMCParticle, UnpackMCVertex,
                            CompareMCParticle, CompareMCVertex )

from DetCond.Configuration import CondDB

## @class Gauss
#  Configurable for Gauss application
#  @author Gloria Corti <Gloria.Corti@cern.ch>
#  @date   2009-07-13

class Gauss(LHCbConfigurableUser):

    __knownDetectors__ = [
        'velo', 'puveto', 'vp',
        'tt' , 'ut',
        'it' , 'sl',
        'ot' , 'ft', 
        'rich',  'rich1', 'rich2', 'torch' , 
        'calo',  'spd', 'prs', 'ecal', 'hcal' ,
        'muon' ,
        'magnet',
        'rich1pmt', 'rich2pmt'
        ]

    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp, SimConf ]

    ## Steering options
    __slots__ = {
        "Histograms"        : "DEFAULT"
        ,"DatasetName"       : "Gauss"
        ,"DataType"          : ""
        # Simple lists of sub detectors
        ,"DetectorGeo"       : {"Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
        ,"DetectorSim"       : {"Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
        ,"DetectorMoni"      : {"Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
        ,"SpilloverPaths"    : []
        ,"PhysicsList"       : {"Em":'NoCuts', "Hadron":'FTFP_BERT', "GeneralPhys":True, "LHCbPhys":True, "Other": '' }
        ,"DeltaRays"         : True
        ,"RICHRandomHits"    : False
        ,"Phases"            : ["Generator","Simulation"] # The Gauss phases to include in the SIM file
        ,"BeamMomentum"      : 3.5*SystemOfUnits.TeV
        ,"BeamHCrossingAngle" : -0.520*SystemOfUnits.mrad
        ,"BeamVCrossingAngle" : 0.0
        ,"BeamEmittance"     : 0.0037*SystemOfUnits.mm
        ,"BeamBetaStar"      : 3.1*SystemOfUnits.m
        ,"BeamLineAngles"    : [ -0.075*SystemOfUnits.mrad,
                                  0.035*SystemOfUnits.mrad ]
        ,"InteractionPosition" : [  0.459*SystemOfUnits.mm ,
                                   -0.015*SystemOfUnits.mm ,
                                    0.5*SystemOfUnits.mm ]
        ,"BunchRMS"          : 82.03*SystemOfUnits.mm
        ,"Luminosity"        : 0.247*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
        ,"TotalCrossSection" : 91.1*SystemOfUnits.millibarn
        ,"OutputType"        : 'SIM'
        ,"Production"        : 'PHYS'
        ,"EnablePack"        : True
        ,"DataPackingChecks" : True
        ,"WriteFSR"          : True
        ,"Debug"             : False
        ,"BeamPipe" : "BeamPipeOn" # _beamPipeSwitch = 1
        ,"ReplaceWithGDML"   : [ { "volsToReplace" : [], "gdmlFile" : "" } ]
        #,"BeamPipe" : "BeamPipeOff"  # _beamPipeSwitch = 0
        #,"BeamPipe" : "BeamPipeInDet"  # _beamPipeSwitch = -1
      }
    
    _detectorsDefaults = {"Detectors": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'Magnet'] }
    _propertyDocDct = { 
        'Histograms'     : """ Type of histograms: ['NONE','DEFAULT'] """
       ,'DatasetName'    : """ String used to build output file names """
       ,"DataType"       : """ Must specify 'Upgrade' for upgrade simulations, otherwise not used """
       ,"DetectorGeo"    : """ Dictionary specifying the detectors to take into account in Geometry """
       ,"DetectorSim"    : """ Dictionary specifying the detectors to simulated (should be in geometry): """
       ,"DetectorMoni"   : """ Dictionary specifying the detectors to monitor (should be simulated) :"""
       ,'SpilloverPaths' : """ Spillover paths to fill: [] means no spillover, otherwise put ['Next', 'Prev', 'PrevPrev'] """
       ,'PhysicsList'    : """ Name of physics modules to be passed 'Em':['Std','Opt1,'Opt2','Opt3','NoCuts','LHCb', 'LHCbNoCuts', 'LHCbOldForE', 'LHCbNoCutsOldForE', 'LHCbTest', 'LHCbTestNoCut' ], 'GeneralPhys':[True,False], 'Hadron':['LHEP','QGSP','QGSP_BERT','QGSP_BERT_HP','QGSP_BERT_CHIPS','QGSP_FTFP_BERT','FTFP_BERT'], 'LHCbPhys': [True,False], 'Other': [''] """
       ,"DeltaRays"      : """ Simulation of delta rays enabled (default True) """
       ,'Phases'         : """ List of phases to run (Generator, Simulation, GenToMCTree) """
       ,'OutputType'     : """ Output: [ 'NONE', 'GEN', 'XGEN', 'RGEN', 'SIM', 'XSIM' ] (default 'SIM') """
       ,'Production'     : """ Generation type : ['PHYS', 'PGUN', 'MIB' (default 'PHYS')"""
       ,'EnablePack'     : """ Flag to turn on or off the packing of the SIM data """
       ,'DataPackingChecks' : """ Flag to turn on or off the running of some test algorithms to check the quality of the data packing """
       ,"WriteFSR"       : """Add file summary record, default True"""
       ,"BeamPipe"       : """Switch for beampipe definition; BeamPipeOn: On everywhere, BeamPipeOff: Off everywhere, BeamPipeInDet: Only in named detectors """
       ,"ReplaceWithGDML": """Replace a list of specified volumes with GDML description from file provided """
       }
    KnownHistOptions     = ['NONE','DEFAULT']
    TrackingSystem       = ['VELO','TT','IT','OT']
    PIDSystem            = ['RICH','CALO','MUON']
    _beamPipeStates = ['beampipeon', 'beampipeoff', 'beampipeindet']

    _incompatibleDetectors = {
        "Velo"       : [ "Velo", "VP" ],
        "VeloPuVeto" : [ "PuVeto", "VP" ],
        "TT"         : [ "TT", "UT" ],
        "Muon"       : [ "Muon", "MuonNoM1" ],
        "MuonTorch"  : [ "Muon", "Torch" ]
        }

    _beamPipeElements = {
        #"upstreamregion" : [
        #"/dd/Structure/LHCb/UpstreamRegion/PipeUpstream" , 
        #"/dd/Structure/LHCb/UpstreamRegion/MBXWHUp" ],
        #"beforemagnetregion" : [
        #    "/dd/Structure/LHCb/BeforeMagnetRegion/PipeJunctionBeforeVelo",
        #    "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/PipeBeforeVelo",
        #    "/dd/Structure/LHCb/BeforeMagnetRegion/BeforeVelo/PipeSupportBeforeVelo" ],
                         "velo" : [
            "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownStreamWakeFieldCone",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpStreamWakeFieldCone",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownstreamPipeSections",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VacTank" ],
                         "rich1" : [
            "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PipeInRich1BeforeSubM",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PipeInRich1SubMaster",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PipeInRich1AfterSubM",
            "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1BeamPipe" ],
                         "tt" : [
            "/dd/Structure/LHCb/BeforeMagnetRegion/TT/PipeInTT" ],
                         "ut" : [
            "/dd/Structure/LHCb/BeforeMagnetRegion/UT/PipeInUT" ],
                         "magnet" : [
            "/dd/Structure/LHCb/MagnetRegion/PipeInMagnet",
            "/dd/Structure/LHCb/MagnetRegion/PipeSupportsInMagnet" ],
                         "AfterMagnetRegion" : [
            "/dd/Structure/LHCb/AfterMagnetRegion/PipeAfterT",
            "/dd/Structure/LHCb/AfterMagnetRegion/PipeSupportsAfterMagnet" ],
                         "t" : [
            "/dd/Structure/LHCb/AfterMagnetRegion/T/PipeInT" ],
                         "rich2" : [
            "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2BeamPipe" ],
                         "downstreamregion" : [
            "/dd/Structure/LHCb/DownstreamRegion/PipeDownstream",
            "/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream",
            "/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream" ]
                         #"aftermuon" : [
                         #"/dd/Structure/LHCb/DownstreamRegion/AfterMuon/PipeAfterMuon",
            #"/dd/Structure/LHCb/DownstreamRegion/AfterMuon/MBXWSDown" ]
        }

    
#"""
#Helper
#HELPER
#
#  ><<     ><<             ><<                            
#  ><<     ><<             ><<                            
#  ><<     ><<    ><<      ><< >< ><<      ><<     >< ><<<
#  ><<<<<< ><<  ><   ><<   ><< ><  ><<   ><   ><<   ><<   
#  ><<     ><< ><<<<< ><<  ><< ><   ><< ><<<<< ><<  ><<   
#  ><<     ><< ><          ><< ><< ><<  ><          ><<   
#  ><<     ><<   ><<<<     ><< ><<        ><<<<     ><<<   
#                              ><<                        
#
#"""    
    ##
    ##
    def slotName(self,slot) :
        name = slot
        if slot == '' : name = "Main"
        return name
    
    ##
    ## Helper functions for spill-over
    def slot_( self, slot ):
        if slot != '':
            return slot + '/'
        return slot

    ##
    def setTrackersHitsProperties( self, alg , det , slot , dd ):
        alg.MCHitsLocation = '/Event/' + self.slot_(slot) + 'MC/' + det + '/Hits'
        if det == 'PuVeto':
            det = 'VeloPu'
        alg.CollectionName = det + 'SDet/Hits'
        alg.Detectors = ['/dd/Structure/LHCb/'+dd]

    ##
    def evtMax(self):
        return LHCbApp().evtMax()

    ##
    def eventType(self):
        from Configurables import Generation
        evtType = ''
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str( Generation("Generation").EventType )
        return evtType

    def setLHCbAppDetectors(self):
        from Configurables import LHCbApp
        # If detectors set in LHCbApp then use those        
        if hasattr(LHCbApp(),"Detectors"):
            if not LHCbApp().Detectors:
                LHCbApp().Detectors = self.getProp("DetectorGeo")["Detectors"]
            else:
                log.warning("Value of 'LHCbApp().Detectors' already set, using that value: %s" %(LHCbApp().Detectors))
        return

#"""
##########################################################################
##########################################################################
##########################################################################
##########################################################################
##########################################################################
#"""



# ><< ><<                                                                        
# ><    ><<                                                ><                    
# ><     ><<    ><<        ><<     ><<< ><< ><<  >< ><<       >< ><<      ><<    
# ><<< ><     ><   ><<   ><<  ><<   ><<  ><  ><< ><  ><<  ><< ><  ><<   ><   ><< 
# ><     ><< ><<<<< ><< ><<   ><<   ><<  ><  ><< ><   ><< ><< ><   ><< ><<<<< ><<
# ><      >< ><         ><<   ><<   ><<  ><  ><< ><< ><<  ><< ><< ><<  ><        
# ><<<< ><<    ><<<<      ><< ><<< ><<<  ><  ><< ><<      ><< ><<        ><<<<   
#                                                ><<          ><<                

    def validateBeamPipeSwitch ( self, bpString ):
        import string
        bpLower = self.getProp("BeamPipe").lower()
        if bpLower not in self._beamPipeStates:
            raise RuntimeError("ERROR: BeamPipe configuration '%s' not recognised!" %bpString)

    def removeBeamPipeElements( self, det ):
        det = det.lower()
        geo = GiGaInputStream('Geo')
        # Remove beampipe elements in <det> - will be included automatically
        if det in self._beamPipeElements.keys():
            for element in self._beamPipeElements[det]:
                # remove all instances of the element
                while element in geo.StreamItems:
                    geo.StreamItems.remove(element)

    def removeAllBeamPipeElements( self ):
        geo = GiGaInputStream('Geo')
        # Remove all beampipe elements
        for det in self._beamPipeElements.keys():
            for element in self._beamPipeElements[det]:
                # remove all instances of the element
                while element in geo.StreamItems:
                    geo.StreamItems.remove(element)
                    
    def defineBeamPipeGeo ( self, geo, basePieces, detPieces ):
        # Add all BeamPipe Elements in the BeamPipeElements dictionary

        # Here commences a hack to deal with daft DDDB structure
        ignoreList = ['ut', 'tt']
        # decide if TT or UT in dets to simulate
        ttDetectorList = [det for det in ['UT', 'TT'] if det in self.getProp('DetectorGeo')['Detectors']]
        # lower strings
        if ttDetectorList:
            # lower everything in ttDetectorList
            ttDetectorList = [det.lower() for det in ttDetectorList]
            # get the elements to ignore
            ignoreList = [det for det in ignoreList if det not in ttDetectorList]

        for region in self._beamPipeElements.keys():
            if region in ignoreList:
                continue
            for element in self._beamPipeElements[region]:
                geo.StreamItems.append(element)

        # Finally add in the TT or UT beampipe if we're not defining the detectors but want the BP anyway depending on DataType
        # Nasty and unclean - change the DDDB s.t. it makes sense please!
        if (
            ("UT" not in self.getProp("DetectorGeo")["Detectors"])
            and
            ("TT" not in self.getProp("DetectorGeo")["Detectors"])
            ):
            if self.getProp("DataType") not in ["Upgrade"]:
                for element in self._beamPipeElements["tt"]:
                    geo.StreamItems.append(element)
            else:
                for element in self._beamPipeElements["ut"]:
                    geo.StreamItems.append(element)


    def defineGDMLGeo ( self, geo, giGaGeo, gdmlDict ):

        # Define the GDML reader tool and add it to the sequence
        from Configurables import GDMLReader
        import os
        gdmlFile = gdmlDict["gdmlFile"]

        if gdmlFile:
            gdmlToolName = os.path.splitext(os.path.basename(gdmlFile))[0]
            gdmlTool = GDMLReader( gdmlToolName,
                                   FileName = gdmlFile )
            giGaGeo.addTool(gdmlTool, gdmlToolName)
            giGaGeo.GdmlReaders.append(gdmlToolName)
            
            # Remove the corresponding geometry from the Geo.InputStreams
            for item in gdmlDict["volsToReplace"]:
                if item in geo.StreamItems:
                    geo.StreamItems.remove(item)
                else:
                    raise RuntimeError("ERROR: Volume not in list of existing volumes, '%s'" %item)
        else:
           raise RuntimeError("ERROR: Invalid GDML file provided, '%s'" %gdmlFile)



#"""
#><<         ><<             ><<               ><<<<<     ><<<<<     ><<<<<     ><< ><<   
# ><<       ><<              ><<               ><<   ><<  ><<   ><<  ><<   ><<  ><    ><< 
#  ><<     ><<      ><<      ><<    ><<        ><<    ><< ><<    ><< ><<    ><< ><     ><<
#   ><<   ><<     ><   ><<   ><<  ><<  ><<     ><<    ><< ><<    ><< ><<    ><< ><<< ><   
#    ><< ><<     ><<<<< ><<  ><< ><<    ><<    ><<    ><< ><<    ><< ><<    ><< ><     ><<
#     ><<<<      ><          ><<  ><<  ><<     ><<   ><<  ><<   ><<  ><<   ><<  ><      ><
#      ><<         ><<<<    ><<<    ><<        ><<<<<     ><<<<<     ><<<<<     ><<<< ><< 
#"""

    def checkVeloDDDB( self ):
        """
        Check if the Velo geometry is compatible with the chosen tags
        """
        # set validity limits for  Velo geometry      
        # first postMC09 Velo geometry
        GTagLimit1 = "head-20091120"       
        GTagLimit1 = GTagLimit1.split('-')[1].strip()
        VeloLTagLimit1 = "velo-20091116"       
        VeloLTagLimit1 = VeloLTagLimit1.split('-')[1].strip()
        # Thomas L. Velo geometry
        GTagLimit2 = "head-20100119"       
        GTagLimit2 = GTagLimit2.split('-')[1].strip()
        VeloLTagLimit2 = "velo-20100114"       
        VeloLTagLimit2 = VeloLTagLimit2.split('-')[1].strip()
        
        # DDDB global tag used
        DDDBDate = LHCbApp().DDDBtag
        DDDBDate = DDDBDate.split('-')[1].strip()

        # check if/which local tag is used for Velo
        cdb = CondDB()
        cdbVeloDate = 0
        for p in cdb.LocalTags:
            if p == "DDDB":
                taglist = list(cdb.LocalTags[p])
                for ltag in taglist:
                    if ltag.find("velo")!=-1 :
                        cdbVeloDate = ltag.split('-')[1].strip()

        # Put this here rather than as an argument
        VeloPostMC09 = 0
        # check if the selected tags require one of the postMC09 Velo geometries 
        if (DDDBDate >= GTagLimit1) or (cdbVeloDate >= VeloLTagLimit1):
            VeloPostMC09 = 1
        if (DDDBDate >= GTagLimit2) or (cdbVeloDate >= VeloLTagLimit2):
            VeloPostMC09 = 2

        return VeloPostMC09
    ##
    ##


#"""
# ><<         ><< ><<<<<<<< ><<           ><<<<     
#  ><<       ><<  ><<       ><<         ><<    ><<  
#   ><<     ><<   ><<       ><<       ><<        ><<
#    ><<   ><<    ><<<<<<   ><<       ><<        ><<
#     ><< ><<     ><<       ><<       ><<        ><<
#      ><<<<      ><<       ><<         ><<     ><< 
#       ><<       ><<<<<<<< ><<<<<<<<     ><<<<     
#"""


    ##
    ##
    def veloMisAlignGeometry( self, VeloPostMC09 ):

        print "veloMisAlignGeometry: %s" %(VeloPostMC09)

        """
        File containing the list of detector element to explicitely set
        to have misalignement in the VELO.
        """
        Geo = GiGaInputStream('Geo')

        # remove Automatically included detector elements
        self.removeBeamPipeElements( "velo" )


        if "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" in Geo.StreamItems:
            Geo.StreamItems.remove("/dd/Structure/LHCb/BeforeMagnetRegion/Velo")

        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModulePU00")
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModulePU02")
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/ModulePU01")
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/ModulePU03")

        txt = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModuleXX"
        import math
        for i in range(42):
            nr = str(i)
            if len(nr) == 1 : nr = '0'+str(i)
            temp1 = txt.replace('XX',nr)
            if math.modf(float(nr)/2.)[0] > 0.1 :  temp1 = temp1.replace('Left','Right')
            Geo.StreamItems.append(temp1)

        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownStreamWakeFieldCone")
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpStreamWakeFieldCone")
        if (VeloPostMC09==1):
            # description postMC09 of Velo (head-20091120), problem with Velo Tank simulation  
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VacTank")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownstreamPipeSections")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpstreamPipeSections")
        elif (VeloPostMC09==2):
            # Thomas L. newer description postMC09 of Velo 
            # --- Velo Right
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/RFBoxRight")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/DetSupportRight")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/ConstSysRight")
            # --- Velo Left
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/RFBoxLeft")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/DetSupportLeft")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ConstSysLeft")
            # --- Velo
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownstreamPipeSections")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpstreamPipeSections")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VacTank")
        else:
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpStreamVacTank")
            Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownStreamVacTank")
        
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/RFFoilRight")
        Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/RFFoilLeft")
    ##
    ##         


    def defineVeloGeo( self , basePieces , detPieces ):
        # Alter in place BasePieces
        # check if the new velo geometry is required with the chosen DDDB tags
        
        VeloP = self.checkVeloDDDB()
        if (VeloP==1 or VeloP==2):
            basePieces['BeforeMagnetRegion']=[]

        # Also sort out mis-alignment
        VeloP = self.checkVeloDDDB()
        # No need to check, this is the case if this is called.
        # No need to misalign if only PuVeto exits - check me PSZ.
        if "Velo" in self.getProp('DetectorGeo')['Detectors']:
            self.veloMisAlignGeometry(VeloP) # To misalign VELO





    def configureVeloSim( self, slot, detHits ):
        region = "BeforeMagnetRegion"
        det = "Velo"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]




    def configureVeloMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.VeloHits = 'MC/Velo/Hits'

        ## Set the VeloMonitor
        detMoniSeq.Members += [ VeloGaussMoni( "VeloGaussMoni" + slot ) ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            from Configurables import DataPacking__Unpack_LHCb__MCVeloHitPacker_
            upVelo = DataPacking__Unpack_LHCb__MCVeloHitPacker_("UnpackVeloHits"+slot,
                                                                OutputName = "MC/Velo/HitsTest" )
            packCheckSeq.Members += [upVelo]

            from Configurables import DataPacking__Check_LHCb__MCVeloHitPacker_
            cVelo = DataPacking__Check_LHCb__MCVeloHitPacker_("CheckVeloHits"+slot)
            packCheckSeq.Members += [cVelo]




#"""
#><<         ><< ><<<<<<<< ><<           ><<<<      ><<<<<<<                
# ><<       ><<  ><<       ><<         ><<    ><<   ><<    ><<  ><          
#  ><<     ><<   ><<       ><<       ><<        ><< ><<    ><<     ><<   ><<
#   ><<   ><<    ><<<<<<   ><<       ><<        ><< ><<<<<<<   ><<   >< ><< 
#    ><< ><<     ><<       ><<       ><<        ><< ><<        ><<    ><    
#     ><<<<      ><<       ><<         ><<     ><<  ><<        ><<  ><  ><< 
#      ><<       ><<<<<<<< ><<<<<<<<     ><<<<      ><<        ><< ><<   ><<
#"""


    def defineVPGeo( self, detPieces ):
        self.removeBeamPipeElements( "velo" )
        if detPieces.has_key('BeforeMagnetRegion'):
            detPieces['BeforeMagnetRegion'] += ['VP']




    def configureVPSim( self, slot, detHits ):
        region = "BeforeMagnetRegion"
        det = "VP"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]






    def configureVPMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        ## in case of a non default detector, need to be overwritten
        #detMoniSeq = GaudiSequencer( "DetectorsMonitor" + slot ) 
        from Configurables import VPGaussMoni
        detMoniSeq.Members += [ VPGaussMoni( "VPGaussMoni" + slot ) ]

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCVPHitPacker_
            checkHits.VeloHits =  'MC/VP/Hits'
            # This is not done in the PuVeto Moni config
            #checkHits.PuVetoHits = ''             
            upVP = DataPacking__Unpack_LHCb__MCVPHitPacker_("UnpackVPHits"+slot,
                                                                      OutputName = "MC/VP/HitsTest" )
            packCheckSeq.Members += [upVP]

            from Configurables import DataPacking__Check_LHCb__MCVPHitPacker_
            cVP = DataPacking__Check_LHCb__MCVPHitPacker_("CheckVPHits"+slot)
            packCheckSeq.Members += [cVP]



#"""
#  ><<<<<<<     ><<     ><<    ><<     ><<
#  ><<    ><<   ><<  ><<   ><< ><<     ><<
#  ><<    ><<   ><< ><<        ><<     ><<
#  >< ><<       ><< ><<        ><<<<<< ><<
#  ><<  ><<     ><< ><<        ><<     ><<
#  ><<    ><<   ><<  ><<   ><< ><<     ><<
#  ><<      ><< ><<    ><<<<   ><<     ><<
#                                       
#"""


    def defineRich1GeoDet( self , detPieces ):
        self.removeBeamPipeElements( "rich1" )
        detPieces['BeforeMagnetRegion']+=['Rich1']

    def defineRich1GeoStream( self , geo ):
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces"]

    def defineRich2GeoDet( self , detPieces ):
        self.removeBeamPipeElements( "rich2" )
        detPieces['AfterMagnetRegion']+=['Rich2']

    def defineRich2GeoStream( self , geo ):
        geo.StreamItems += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]
        #_________________________________



    def defineRichPhys( self, gmpl ):
        gmpl.PhysicsConstructors.append("GiGaPhysConstructorOp")
        gmpl.PhysicsConstructors.append("GiGaPhysConstructorHpd")
        
    ##
    ##
    def configureRichSim (self, slot, detHits ):
        from Configurables import (
            GetMCRichHitsAlg,
            GetMCRichOpticalPhotonsAlg,
            GetMCRichSegmentsAlg,
            GetMCRichTracksAlg
            )

        #for slot in SpillOverSlots:
        richHitsSeq = GaudiSequencer( "RichHits" + slot )
        detHits.Members += [ richHitsSeq ]
        richHitsSeq.Members = [ GetMCRichHitsAlg( "GetRichHits"+slot),
                                GetMCRichOpticalPhotonsAlg("GetRichPhotons"+slot),
                                GetMCRichSegmentsAlg("GetRichSegments"+slot), 
                                GetMCRichTracksAlg("GetRichTracks"+slot), 
                                Rich__MC__MCPartToMCRichTrackAlg("MCPartToMCRichTrack"+slot),
                                Rich__MC__MCRichHitToMCRichOpPhotAlg("MCRichHitToMCRichOpPhot"+slot) ]


    def configureRichMoni ( self, slot, packCheckSeq, detMoniSeq, checkHits, configuredRichMoni ): 

        # reinstate checkHits default value
        checkHits.RichHits = 'MC/Rich/Hits'

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            from Configurables import DataPacking__Unpack_LHCb__MCRichHitPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichSegmentPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichTrackPacker_

            upRichHit  = DataPacking__Unpack_LHCb__MCRichHitPacker_("UnpackRichHits"+slot,
                                                                    OutputName = "MC/Rich/HitsTest" )
            upRichOpPh = DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_("UnpackRichOpPhot"+slot,
                                                                              OutputName = "MC/Rich/OpticalPhotonsTest" )
            upRichSeg  = DataPacking__Unpack_LHCb__MCRichSegmentPacker_("UnpackRichSegments"+slot,
                                                                        OutputName = "MC/Rich/SegmentsTest" )
            upRichTrk  = DataPacking__Unpack_LHCb__MCRichTrackPacker_("UnpackRichTracks"+slot,
                                                                      OutputName = "MC/Rich/TracksTest" )
            packCheckSeq.Members += [upRichHit,upRichOpPh,upRichSeg,upRichTrk]

            
            from Configurables import DataPacking__Check_LHCb__MCRichHitPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichOpticalPhotonPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichSegmentPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichTrackPacker_
            cRichHit  = DataPacking__Check_LHCb__MCRichHitPacker_("CheckRichHits"+slot )
            cRichOpPh = DataPacking__Check_LHCb__MCRichOpticalPhotonPacker_("CheckRichOpPhot"+slot )
            cRichSeg  = DataPacking__Check_LHCb__MCRichSegmentPacker_("CheckRichSegments"+slot )
            cRichTrk  = DataPacking__Check_LHCb__MCRichTrackPacker_("CheckRichTracks"+slot )
            packCheckSeq.Members += [cRichHit,cRichOpPh,cRichSeg,cRichTrk]
        # Out of loop
        if not configuredRichMoni[1]:
            importOptions("$GAUSSRICHROOT/options/RichAnalysis.opts")
            configuredRichMoni[1] = True

#"""
#  ><<<<<<<     ><<     ><<    ><<     ><<    ><<<<<<<   ><<       ><< ><<< ><<<<<<
#  ><<    ><<   ><<  ><<   ><< ><<     ><<    ><<    ><< >< ><<   ><<<      ><<    
#  ><<    ><<   ><< ><<        ><<     ><<    ><<    ><< ><< ><< > ><<      ><<    
#  >< ><<       ><< ><<        ><<<<<< ><<    ><<<<<<<   ><<  ><<  ><<      ><<    
#  ><<  ><<     ><< ><<        ><<     ><<    ><<        ><<   ><  ><<      ><<    
#  ><<    ><<   ><<  ><<   ><< ><<     ><<    ><<        ><<       ><<      ><<    
#  ><<      ><< ><<    ><<<<   ><<     ><<    ><<        ><<       ><<      ><<    
#                                                                                
#"""

    def defineRich1MaPmtGeoDet( self , detPieces ):
        self.removeBeamPipeElements( "rich1" )
        detPieces['BeforeMagnetRegion']+=['Rich1']

    def defineRich1MaPmtGeoStream( self , geo , giGaGeo ):
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
        geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichPMTSurfaces"]
        giGaGeo.UseAlignment = False
        giGaGeo.AlignAllDetectors = False


    def defineRich2MaPmtGeoDet( self , detPieces ):
        self.removeBeamPipeElements( "rich2" )
        detPieces['AfterMagnetRegion']+=['Rich2']

    def defineRich2MaPmtGeoStream( self , geo, giGaGeo ):
        geo.StreamItems += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]
        giGaGeo.UseAlignment = False
        giGaGeo.AlignAllDetectors = False


    def defineRichMaPmtPhys( self, gmpl):
        gmpl.PhysicsConstructors.append("GiGaPhysConstructorOpCkv")
        gmpl.PhysicsConstructors.append("GiGaPhysConstructorPhotoDetector")




    def configureRichMaPmtSim (self, slot, detHits ):
        from Configurables import (
            GetMCCkvHitsAlg,
            GetMCCkvOpticalPhotonsAlg,
            GetMCCkvSegmentsAlg,
            GetMCCkvTracksAlg
            )
        richHitsSeq = GaudiSequencer( "RichHits" + slot )
        detHits.Members += [ richHitsSeq ]
        richHitsSeq.Members = [ GetMCCkvHitsAlg( "GetRichHits"+slot),
                                GetMCCkvOpticalPhotonsAlg("GetRichPhotons"+slot),
                                GetMCCkvSegmentsAlg("GetRichSegments"+slot), 
                                GetMCCkvTracksAlg("GetRichTracks"+slot), 
                                Rich__MC__MCPartToMCRichTrackAlg("MCPartToMCRichTrack"+slot), 
                                Rich__MC__MCRichHitToMCRichOpPhotAlg("MCRichHitToMCRichOpPhot"+slot) ]


    def configureRichMaPmtMoni ( self, slot, packCheckSeq, detMoniSeq, checkHits, configuredRichMoni ): 
        checkHits.RichHits = 'MC/Rich/Hits'

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            from Configurables import DataPacking__Unpack_LHCb__MCRichHitPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichSegmentPacker_
            from Configurables import DataPacking__Unpack_LHCb__MCRichTrackPacker_

            upRichHit  = DataPacking__Unpack_LHCb__MCRichHitPacker_("UnpackRichHits"+slot,
                                                                    OutputName = "MC/Rich/HitsTest" )
            upRichOpPh = DataPacking__Unpack_LHCb__MCRichOpticalPhotonPacker_("UnpackRichOpPhot"+slot,
                                                                              OutputName = "MC/Rich/OpticalPhotonsTest" )
            upRichSeg  = DataPacking__Unpack_LHCb__MCRichSegmentPacker_("UnpackRichSegments"+slot,
                                                                        OutputName = "MC/Rich/SegmentsTest" )
            upRichTrk  = DataPacking__Unpack_LHCb__MCRichTrackPacker_("UnpackRichTracks"+slot,
                                                                      OutputName = "MC/Rich/TracksTest" )
            packCheckSeq.Members += [upRichHit,upRichOpPh,upRichSeg,upRichTrk]

            
            from Configurables import DataPacking__Check_LHCb__MCRichHitPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichOpticalPhotonPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichSegmentPacker_
            from Configurables import DataPacking__Check_LHCb__MCRichTrackPacker_
            cRichHit  = DataPacking__Check_LHCb__MCRichHitPacker_("CheckRichHits"+slot )
            cRichOpPh = DataPacking__Check_LHCb__MCRichOpticalPhotonPacker_("CheckRichOpPhot"+slot )
            cRichSeg  = DataPacking__Check_LHCb__MCRichSegmentPacker_("CheckRichSegments"+slot )
            cRichTrk  = DataPacking__Check_LHCb__MCRichTrackPacker_("CheckRichTracks"+slot )
            packCheckSeq.Members += [cRichHit,cRichOpPh,cRichSeg,cRichTrk]
        # Out of indent
        if not configuredRichMoni[1]:
            importOptions("$GAUSSCHERENKOVROOT/options/GaussCherenkovAnalysis.opts")
            configuredRichMoni[1] = True

#"""
#><<< ><<<<<<     ><<<<      ><<<<<<<         ><<    ><<     ><<
#     ><<       ><<    ><<   ><<    ><<    ><<   ><< ><<     ><<
#     ><<     ><<        ><< ><<    ><<   ><<        ><<     ><<
#     ><<     ><<        ><< >< ><<       ><<        ><<<<<< ><<
#     ><<     ><<        ><< ><<  ><<     ><<        ><<     ><<
#     ><<       ><<     ><<  ><<    ><<    ><<   ><< ><<     ><<
#     ><<         ><<<<      ><<      ><<    ><<<<   ><<     ><<
#                                                               
#"""
    def defineTorchGeo( self ):
        log.warning("Geo not defined for TORCH")
        pass

    def configureTorchSim( self, slot, detHits ):
        log.warning("Sim not defined for TORCH")
        pass

    def configureTorchMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        #detMoniSeq = GaudiSequencer( "DetectorsMonitor" + slot ) 
        log.warning ("Moni not defined for TORCH")
        pass




#"""
#><<< ><<<<<< ><<< ><<<<<<
#     ><<          ><<    
#     ><<          ><<    
#     ><<          ><<    
#     ><<          ><<    
#     ><<          ><<    
#     ><<          ><<    
#"""


    def defineTTGeo( self , detPieces ):
        self.removeBeamPipeElements( "tt" )
        if 'TT' not in detPieces['BeforeMagnetRegion']:
            detPieces['BeforeMagnetRegion']+=['TT']


    def configureTTSim( self, slot, detHits ):
        region   = "BeforeMagnetRegion"
        det = "TT"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]


    def configureTTMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.TTHits = 'MC/TT/Hits'

        myZStations = [
            2350.0*SystemOfUnits.mm,
            2620.0*SystemOfUnits.mm
            ]
        myZStationXMax = 150.*SystemOfUnits.cm
        myZStationYMax = 150.*SystemOfUnits.cm

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "TTHitMonitor" + slot ,
                mcPathString = "MC/TT/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
                
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )

            from Configurables import DataPacking__Unpack_LHCb__MCTTHitPacker_
            from Configurables import DataPacking__Check_LHCb__MCTTHitPacker_
            upTT   = DataPacking__Unpack_LHCb__MCTTHitPacker_("UnpackTTHits"+slot,
                                                              OutputName = "MC/TT/HitsTest" )
            cTT   = DataPacking__Check_LHCb__MCTTHitPacker_("CheckTTHits"+slot )
            packCheckSeq.Members += [upTT, cTT]



#"""
# ><<     ><< ><<< ><<<<<<
# ><<     ><<      ><<    
# ><<     ><<      ><<    
# ><<     ><<      ><<    
# ><<     ><<      ><<    
# ><<     ><<      ><<    
#   ><<<<<         ><<    
#"""


    def defineUTGeo( self , detPieces ):
        self.removeBeamPipeElements( "ut" )
        if 'UT' not in detPieces['BeforeMagnetRegion']:
            detPieces['BeforeMagnetRegion']+=['UT']


    def configureUTSim( self, slot, detHits ):
        region   = "BeforeMagnetRegion"
        det = "UT"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]


    def configureUTMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.UTHits = 'MC/UT/Hits'

        myZStations = [
            2350.0*SystemOfUnits.mm,
            2620.0*SystemOfUnits.mm
            ]
        myZStationXMax = 150.*SystemOfUnits.cm
        myZStationYMax = 150.*SystemOfUnits.cm

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "UTHitMonitor" + slot ,
                mcPathString = "MC/UT/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
                
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )

            from Configurables import DataPacking__Unpack_LHCb__MCUTHitPacker_
            from Configurables import DataPacking__Check_LHCb__MCUTHitPacker_
            upUT   = DataPacking__Unpack_LHCb__MCUTHitPacker_("UnpackUTHits"+slot,
                                                              OutputName = "MC/UT/HitsTest" )
            cUT   = DataPacking__Check_LHCb__MCUTHitPacker_("CheckUTHits"+slot )
            packCheckSeq.Members += [upUT, cUT]




#"""
#  ><<  ><<< ><<<<<<
#  ><<       ><<    
#  ><<       ><<    
#  ><<       ><<    
#  ><<       ><<    
#  ><<       ><<    
#  ><<       ><<    
#                
#"""

    def defineITGeo( self , detPieces ):
        self.removeBeamPipeElements( "t" )
        region = "AfterMagnetRegion"
        if 'T' not in detPieces[region]:
            detPieces[region]+=['T/IT']
        if 'T/PipeInT' not in detPieces[region]:
            detPieces[region]+=['T/PipeInT']


    def configureITSim( self, slot, detHits ):
        region   = "AfterMagnetRegion/T"
        det = "IT"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]


    def configureITMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.ITHits = 'MC/IT/Hits'

        myZStations = [
            7780.0*SystemOfUnits.mm,
            8460.0*SystemOfUnits.mm,
            9115.0*SystemOfUnits.mm
            ]
        myZStationXMax = 150.*SystemOfUnits.cm
        myZStationYMax = 150.*SystemOfUnits.cm

        # Upgrade
        #if self.getProp("DataType") == "Upgrade" :
        #    myZStations = [
        #        8015.0*SystemOfUnits.mm,
        #        8697.0*SystemOfUnits.mm,
        #        9363.0*SystemOfUnits.mm
        #        ]

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "ITHitMonitor" + slot ,
                mcPathString = "MC/IT/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCITHitPacker_
            upIT   = DataPacking__Unpack_LHCb__MCITHitPacker_("UnpackITHits"+slot,
                                                              OutputName = "MC/IT/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCITHitPacker_
            cIT   = DataPacking__Check_LHCb__MCITHitPacker_("CheckITHits"+slot )
            packCheckSeq.Members += [upIT, cIT]


#"""
#
#     Si IT
#                
#"""

    def defineSLGeo( self , detPieces ):
        self.removeBeamPipeElements( "t" )
        region = "AfterMagnetRegion"
        if 'T' not in detPieces[region]:
            detPieces[region]+=['T/SL']
        # PSZ - line below might need to go depending on SL definition
        if 'T/PipeInT' not in detPieces[region]:
            detPieces[region]+=['T/PipeInT']


    def configureSLSim( self, slot, detHits ):
        region   = "AfterMagnetRegion/T"
        det = "SL"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]
        pass

    #def configureSLMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
    #    pass
    def configureSLMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.SLHits = 'MC/SL/Hits'

        myZStations = [
            7780.0*SystemOfUnits.mm,
            #8460.0*SystemOfUnits.mm,
            9115.0*SystemOfUnits.mm
            ]
        myZStationXMax = 150.*SystemOfUnits.cm
        myZStationYMax = 150.*SystemOfUnits.cm

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "SLHitMonitor" + slot ,
                mcPathString = "MC/SL/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCSLHitPacker_
            upSL   = DataPacking__Unpack_LHCb__MCSLHitPacker_("UnpackSLHits"+slot,
                                                              OutputName = "MC/SL/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCSLHitPacker_
            cSL   = DataPacking__Check_LHCb__MCSLHitPacker_("CheckSLHits"+slot )
            packCheckSeq.Members += [upSL, cSL]



#"""
#   ><<<<<<<< ><<< ><<<<<<
#   ><<            ><<    
#   ><<            ><<    
#   ><<<<<<        ><<    
#   ><<            ><<    
#   ><<            ><<    
#   ><<            ><<    
#                      
#"""
    def defineFTGeo( self , detPieces ):
        self.removeBeamPipeElements( "t" )
        region = "AfterMagnetRegion"
        if 'T' not in detPieces[region]:
            detPieces[region]+=['T/FT']
        if 'T/PipeInT' not in detPieces[region]:
            detPieces[region]+=['T/PipeInT']


    def configureFTSim( self, slot, detHits ):
        region   = "AfterMagnetRegion/T"
        det = "FT"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]


    def configureFTMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.FTHits = 'MC/FT/Hits'


        # Upgrade
        myZStations = [
            7938.0*SystemOfUnits.mm,
            8625.0*SystemOfUnits.mm,
            9315.0*SystemOfUnits.mm
            ]
        myZStationXMax = 100.*SystemOfUnits.cm
        myZStationYMax = 100.*SystemOfUnits.cm

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "FTHitMonitor" + slot ,
                mcPathString = "MC/FT/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCFTHitPacker_
            upFT = DataPacking__Unpack_LHCb__MCFTHitPacker_("UnpackFTHits"+slot,
                                                              OutputName = "MC/FT/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCFTHitPacker_
            cFT  = DataPacking__Check_LHCb__MCFTHitPacker_("CheckFTHits"+slot )
            packCheckSeq.Members += [upFT, cFT]


#"""
#    ><<<<      ><<< ><<<<<<
#  ><<    ><<        ><<    
#><<        ><<      ><<    
#><<        ><<      ><<    
#><<        ><<      ><<    
#  ><<     ><<       ><<    
#    ><<<<           ><<    
#"""

    def defineOTGeo( self , detPieces ):
        self.removeBeamPipeElements( "t" )
        region = "AfterMagnetRegion"
        if 'T' not in detPieces[region]:
            detPieces[region]+=['T/OT']
        if 'T/PipeInT' not in detPieces[region]:
            detPieces[region]+=['T/PipeInT']



    def configureOTSim( self, slot, detHits ):
        region   = "AfterMagnetRegion/T"
        det = "OT"
        moni = GetTrackerHitsAlg(
            'Get' + det + 'Hits' + slot,
            MCHitsLocation = 'MC/' + det  + '/Hits',
            CollectionName = det + 'SDet/Hits',
            Detectors = [ '/dd/Structure/LHCb/' + region + '/' + det ]
            )
        detHits.Members += [ moni ]

    def configureOTMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        checkHits.OTHits = 'MC/OT/Hits'

        myZStations = [
            7938.0*SystemOfUnits.mm,
            8625.0*SystemOfUnits.mm,
            9315.0*SystemOfUnits.mm
            ]
        myZStationXMax = 100.*SystemOfUnits.cm
        myZStationYMax = 100.*SystemOfUnits.cm

        # Upgrade
        #if self.getProp("DataType") == "Upgrade" :
        #    myZStations = [
        #        7672.0*SystemOfUnits.mm,
        #        8354.0*SystemOfUnits.mm,
        #        9039.0*SystemOfUnits.mm
        #        ]

        detMoniSeq.Members += [ 
            MCHitMonitor( 
                "OTHitMonitor" + slot ,
                mcPathString = "MC/OT/Hits",
                zStations = myZStations,
                xMax = myZStationXMax,
                yMax = myZStationYMax
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCOTHitPacker_
            upOT   = DataPacking__Unpack_LHCb__MCOTHitPacker_("UnpackOTHits"+slot,
                                                              OutputName = "MC/OT/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCOTHitPacker_
            cOT   = DataPacking__Check_LHCb__MCOTHitPacker_("CheckOTHits"+slot )
            packCheckSeq.Members += [upOT, cOT]
     


#"""
#   ><<       ><<                              
#   >< ><<   ><<<                              
#   ><< ><< > ><< ><<  ><<    ><<     ><< ><<  
#   ><<  ><<  ><< ><<  ><<  ><<  ><<   ><<  ><<
#   ><<   ><  ><< ><<  ><< ><<    ><<  ><<  ><<
#   ><<       ><< ><<  ><<  ><<  ><<   ><<  ><<
#   ><<       ><<   ><<><<    ><<     ><<<  ><<
#
#"""

    def defineMuonGeo( self, detPieces ):
        #self.removeBeamPipeElements( "muon" )
        region = 'DownstreamRegion'
        detPieces[region]+=['Muon']


    def configureMuonSim ( self, slot, detHits ):
        det = "Muon"
        moni = GetTrackerHitsAlg( "Get"+det+"Hits"+slot,
                                  MCHitsLocation = 'MC/' + det + '/Hits',
                                  CollectionName = det + 'SDet/Hits',
                                  Detectors = ['/dd/Structure/LHCb/DownstreamRegion/'+det] )
        detHits.Members += [ moni ]


    def configureMuonMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):
        # reinstate checkHits default value
        det = "Muon"
        checkHits.MuonHits = 'MC/Muon/Hits'
        
        detMoniSeq.Members += [ MuonHitChecker( det + "HitChecker" + slot,
                                                FullDetail = True )]
        from Configurables import MuonMultipleScatteringChecker
        detMoniSeq.Members += [
            MuonMultipleScatteringChecker( "MuonMultipleScatteringChecker"+ slot )]

        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            
            from Configurables import DataPacking__Unpack_LHCb__MCMuonHitPacker_
            upMu   = DataPacking__Unpack_LHCb__MCMuonHitPacker_("UnpackMuonHits"+slot,
                                                                OutputName = "MC/Muon/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCMuonHitPacker_
            cMu   = DataPacking__Check_LHCb__MCMuonHitPacker_("CheckMuonHits"+slot )
            packCheckSeq.Members += [upMu, cMu]
            



#"""
#       ><<          ><        ><<           ><<<<     
#    ><<   ><<      >< <<      ><<         ><<    ><<  
#   ><<            ><  ><<     ><<       ><<        ><<
#   ><<           ><<   ><<    ><<       ><<        ><<
#   ><<          ><<<<<< ><<   ><<       ><<        ><<
#    ><<   ><<  ><<       ><<  ><<         ><<     ><< 
#      ><<<<   ><<         ><< ><<<<<<<<     ><<<<     
#                                                   
#"""

    def defineSpdGeo( self, detPieces ):
        self.removeBeamPipeElements("calo")
        region = 'DownstreamRegion'
        detPieces[region]+=['Spd']
        detPieces[region]+=['Converter']

    def definePrsGeo( self, detPieces ):
        region = 'DownstreamRegion'
        detPieces[region]+=['Prs']

    def defineEcalGeo( self, detPieces ):
        region = 'DownstreamRegion'
        detPieces[region]+=['Ecal']

    def defineHcalGeo( self, detPieces ):
        region = 'DownstreamRegion'
        detPieces[region]+=['Hcal']
        

    def configureSpdSim ( self, slot, detHits ):
        det = "Spd"
        moni = GetCaloHitsAlg( 
            "Get"+det+"Hits"+slot,
            MCHitsLocation = 'MC/' + det + '/Hits',
            CollectionName = det + 'Hits'
            )
        detHits.Members += [ moni ]

    def configurePrsSim ( self, slot, detHits ):
        det = "Prs"
        moni = GetCaloHitsAlg( 
            "Get"+det+"Hits"+slot,
            MCHitsLocation = 'MC/' + det + '/Hits',
            CollectionName = det + 'Hits'
            )
        detHits.Members += [ moni ]

    def configureEcalSim ( self, slot, detHits ):
        det = "Ecal"
        moni = GetCaloHitsAlg( 
            "Get"+det+"Hits"+slot,
            MCHitsLocation = 'MC/' + det + '/Hits',
            CollectionName = det + 'Hits'
            )
        detHits.Members += [ moni ]

    def configureHcalSim ( self, slot, detHits ):
        det = "Hcal"
        moni = GetCaloHitsAlg( 
            "Get"+det+"Hits"+slot,
            MCHitsLocation = 'MC/' + det + '/Hits',
            CollectionName = det + 'Hits'
            )
        detHits.Members += [ moni ]




    def configureSpdMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ): 
        # reinstate checkHits default value
        checkHits.CaloHits.append('MC/Spd/Hits')

        det = "Spd"
        detMoniSeq.Members += [ 
            MCCaloMonitor( 
                det + "Monitor" + slot,
                OutputLevel = 4,
                Detector = det,
                Regions = True,
                MaximumEnergy = 10.*SystemOfUnits.MeV,
                Threshold = 1.5*SystemOfUnits.MeV
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCSpdHitPacker_
            upSpd  = DataPacking__Unpack_LHCb__MCSpdHitPacker_("UnpackSpdHits"+slot,
                                                               OutputName = "MC/Spd/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCSpdHitPacker_
            cSpd  = DataPacking__Check_LHCb__MCSpdHitPacker_("CheckSpdHits"+slot)
            packCheckSeq.Members += [upSpd,cSpd]



    def configurePrsMoni( self, slot, packCheckSeq, detMoniSeq, checkHits): 
        # reinstate checkHits default value
        checkHits.CaloHits.append('MC/Prs/Hits')

        det = "Prs"
        detMoniSeq.Members += [ 
            MCCaloMonitor( 
                det + "Monitor" + slot,
                OutputLevel = 4,
                Detector = 'Prs',
                Regions = True,
                MaximumEnergy = 10.*SystemOfUnits.MeV,
                Threshold = 1.5*SystemOfUnits.MeV
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCPrsHitPacker_
            upPrs  = DataPacking__Unpack_LHCb__MCPrsHitPacker_("UnpackPrsHits"+slot,
                                                               OutputName = "MC/Prs/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCPrsHitPacker_
            cPrs  = DataPacking__Check_LHCb__MCPrsHitPacker_("CheckPrsHits"+slot)
            packCheckSeq.Members += [upPrs,cPrs]




    def configureEcalMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ): 
        # reinstate checkHits default value
        checkHits.CaloHits.append('MC/Ecal/Hits')

        det = "Ecal"
        detMoniSeq.Members += [ 
            MCCaloMonitor( 
                det + "Monitor" + slot,
                OutputLevel = 4,
                Detector = det,
                Regions = True,
                MaximumEnergy = 1000.*SystemOfUnits.MeV,
                Threshold = 10.*SystemOfUnits.MeV
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCEcalHitPacker_
            upEcal = DataPacking__Unpack_LHCb__MCEcalHitPacker_("UnpackEcalHits"+slot,
                                                                OutputName = "MC/Ecal/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCEcalHitPacker_
            cEcal = DataPacking__Check_LHCb__MCEcalHitPacker_("CheckEcalHits"+slot)
            packCheckSeq.Members += [upEcal,cEcal]




    def configureHcalMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ): 
        # reinstate checkHits default value
        checkHits.CaloHits.append('MC/Hcal/Hits')

        det = "Hcal"
        detMoniSeq.Members += [ 
            MCCaloMonitor( 
                det + "Monitor" + slot,
                OutputLevel = 4,
                Detector = det,
                Regions = True,
                MaximumEnergy = 1000.*SystemOfUnits.MeV,
                Threshold = 5.*SystemOfUnits.MeV
                )
            ]
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            
            packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
            from Configurables import DataPacking__Unpack_LHCb__MCHcalHitPacker_
            upHcal = DataPacking__Unpack_LHCb__MCHcalHitPacker_("UnpackHcalHits"+slot,
                                                                OutputName = "MC/Hcal/HitsTest" )
            from Configurables import DataPacking__Check_LHCb__MCHcalHitPacker_
            cHcal = DataPacking__Check_LHCb__MCHcalHitPacker_("CheckHcalHits"+slot)
            packCheckSeq.Members += [upHcal,cHcal]






#"""
#    ><<       ><<                                             ><<  
#    >< ><<   ><<<                                             ><<  
#    ><< ><< > ><<    ><<        ><<    ><< ><<      ><<     ><>< ><
#    ><<  ><<  ><<  ><<  ><<   ><<  ><<  ><<  ><<  ><   ><<    ><<  
#    ><<   ><  ><< ><<   ><<  ><<   ><<  ><<  ><< ><<<<< ><<   ><<  
#    ><<       ><< ><<   ><<   ><<  ><<  ><<  ><< ><           ><<  
#    ><<       ><<   ><< ><<<      ><<  ><<<  ><<   ><<<<       ><< 
#                           ><<                                 
#"""

    def defineMagnetGeo( self , basePieces, detPieces ):
        # Turn off magnet if false
        path = "dd/Structure/LHCb/MagnetRegion/"
        detPieces["MagnetRegion"] = ['Magnet','BcmDown']
        # PSZ - check why this is here
        if False:
            for element in detPieces['MagnetRegion']:
                myElement = path + element
                if myElement in geo.StreamItems:
                    geo.StreamItems.remove([ path + element ])

        # PSZ - clean me up
        if False:
            GiGaGeo().FieldManager           = "GiGaFieldMgr/FieldMgr"
            GiGaGeo().addTool( GiGaFieldMgr("FieldMgr"), name="FieldMgr" )
            GiGaGeo().FieldMgr.Stepper       = "ClassicalRK4"
            GiGaGeo().FieldMgr.Global        = True
            GiGaGeo().FieldMgr.MagneticField = "GiGaMagFieldGlobal/LHCbField"
            GiGaGeo().FieldMgr.addTool( GiGaMagFieldGlobal("LHCbField"), name="LHCbField" ) 
            GiGaGeo().FieldMgr.LHCbField.MagneticFieldService = "MagneticFieldSvc"


    def defineMagnetGeoField( self, giGaGeo ):
        # Only bother with the FIELD Geometry if simulated.
        if "Magnet" in self.getProp('DetectorSim')['Detectors']:
            GiGaGeo().FieldManager           = "GiGaFieldMgr/FieldMgr"
            GiGaGeo().addTool( GiGaFieldMgr("FieldMgr"), name="FieldMgr" )
            GiGaGeo().FieldMgr.Stepper       = "ClassicalRK4"
            GiGaGeo().FieldMgr.Global        = True
            GiGaGeo().FieldMgr.MagneticField = "GiGaMagFieldGlobal/LHCbField"
            GiGaGeo().FieldMgr.addTool( GiGaMagFieldGlobal("LHCbField"), name="LHCbField" ) 
            GiGaGeo().FieldMgr.LHCbField.MagneticFieldService = "MagneticFieldSvc"




#"""
# ><<<<<<<            ><<         ><<              ><<             
# ><<    ><<           ><<       ><<               ><<             
# ><<    ><< ><<  ><<   ><<     ><<      ><<     ><>< ><    ><<    
# ><<<<<<<   ><<  ><<    ><<   ><<     ><   ><<    ><<    ><<  ><< 
# ><<        ><<  ><<     ><< ><<     ><<<<< ><<   ><<   ><<    ><<
# ><<        ><<  ><<      ><<<<      ><           ><<    ><<  ><< 
# ><<          ><<><<       ><<         ><<<<       ><<     ><<    
#                                                                 
#"""

    def definePuVetoGeo( self ):
        pass

    def configurePuVetoSim( self, slot, detHits ):
        region = "BeforeMagnetRegion"
        det = "PuVeto"
        # This is still awful - PSZ
        detextra, detextra1 = 'VeloPu', 'Velo'
        if 'VP' not in self.getProp('DetectorSim')['Detectors']:
            moni = GetTrackerHitsAlg(
                'Get' + det + 'Hits' + slot,
                MCHitsLocation = 'MC/' + det  + '/Hits',
                CollectionName = 'VeloPuSDet/Hits',
                Detectors = [ '/dd/Structure/LHCb/' + region + '/Velo' ]
                )
            detHits.Members += [ moni ]

    def configurePuVetoMoni( self, slot, packCheckSeq, detMoniSeq, checkHits ):

        checkHits.PuVetoHits = 'MC/PuVeto/Hits'
        # Turn off the PuVeto hits if using modified detector
        if 'VP' not in self.getProp('DetectorSim')['Detectors']:
            checkHits.PuVetoHits = ''

        from Configurables import DataPacking__Unpack_LHCb__MCPuVetoHitPacker_
        upPuVe = DataPacking__Unpack_LHCb__MCPuVetoHitPacker_("UnpackPuVetoHits"+slot,
                                                              OutputName = "MC/PuVeto/HitsTest" )
        if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
            packCheckSeq.Members += [upPuVe]

            from Configurables import DataPacking__Check_LHCb__MCPuVetoHitPacker_
            # if there's no VP do PuVeto stuff
            if 'VP' not in self.getProp('DetectorSim')['Detectors']:
                cPuVe = DataPacking__Check_LHCb__MCPuVetoHitPacker_("CheckPuVetoHits"+slot)
                packCheckSeq.Members += [cPuVe]




#"""
##########################################################################
##########################################################################
##########################################################################
##########################################################################
##########################################################################
#"""



#"""
#    ><<                          ><<    ><<<<<<<                  ><<
# ><<   ><<                     ><       ><<    ><<                ><<
#><<          ><<    ><< ><<  ><>< ><    ><<    ><<  ><< ><<       ><<
#><<        ><<  ><<  ><<  ><<  ><<      >< ><<       ><<  ><< ><< ><<
#><<       ><<    ><< ><<  ><<  ><<      ><<  ><<     ><<  ><<><   ><<
# ><<   ><< ><<  ><<  ><<  ><<  ><<      ><<    ><<   ><<  ><<><   ><<
#   ><<<<     ><<    ><<<  ><<  ><<      ><<      ><<><<<  ><< ><< ><<
#"""


    ##
    ## Functions to configuration various services that are used
    ##
    def configureRndmEngine( self ):
        # Random number service
        rndmSvc = RndmGenSvc()
        rndmSvc.Engine = "HepRndm::Engine<CLHEP::MTwistEngine>"
        from Configurables import HepRndm__Engine_CLHEP__MTwistEngine_
        engine = HepRndm__Engine_CLHEP__MTwistEngine_("RndmGenSvc.Engine")
        engine.SetSingleton = True



#"""
#    ><<                          ><<    ><<                             ><<  
# ><<   ><<                     ><       ><<                             ><<  
#><<          ><<    ><< ><<  ><>< ><    ><< ><< ><<   >< ><<  ><<  ><<><>< ><
#><<        ><<  ><<  ><<  ><<  ><<      ><<  ><<  ><< ><  ><< ><<  ><<  ><<  
#><<       ><<    ><< ><<  ><<  ><<      ><<  ><<  ><< ><   ><<><<  ><<  ><<  
# ><<   ><< ><<  ><<  ><<  ><<  ><<      ><<  ><<  ><< ><< ><< ><<  ><<  ><<  
#   ><<<<     ><<    ><<<  ><<  ><<      ><< ><<<  ><< ><<       ><<><<   ><< 
#                                                      ><<                    
#"""

    def configureInput(self):
        # No events are read as input (this is not true if gen phase is
        # switched off
        ApplicationMgr().EvtSel = 'NONE'
        # Transient store setup
        EventDataSvc().ForceLeaves = True
        # May be needed by some options
        importOptions("$STDOPTS/PreloadUnits.opts")



#"""
#    ><<                                ><<         ><<<<<                   ><<          
# ><<   ><< ><<                         ><<         ><<   ><<   ><           ><<          
#><<        ><<         ><<        ><<< ><<  ><<    ><<    ><<        ><<< ><>< ><  ><<<< 
#><<        >< ><     ><   ><<   ><<    ><< ><<     ><<    ><< ><<  ><<      ><<   ><<    
#><<        ><<  ><< ><<<<< ><< ><<     ><><<       ><<    ><< ><< ><<       ><<     ><<< 
# ><<   ><< ><   ><< ><          ><<    ><< ><<     ><<   ><<  ><<  ><<      ><<       ><<
#   ><<<<   ><<  ><<   ><<<<       ><<< ><<  ><<    ><<<<<     ><<    ><<<    ><<  ><< ><<
#                                                                                         
#"""

    ## Raise an error if DetectorGeo/DetectorSim/DetectorMoni are not compatible
    def fixGeoSimMoniDictionary ( self ) :
        pass

    def checkGeoSimMoniDictionary ( self ) :
        for subdet in self.TrackingSystem + self.PIDSystem:
            # Could do something smarter here
            for det in self.getProp('DetectorSim')['Detectors']:
                if self.getProp('DetectorGeo')['Detectors'].count(det) == 0 :
                    raise RuntimeError("Simulation has been required for '%s' sub-detector but it has been removed from Geometry" %det)
            for det in self.getProp('DetectorMoni')['Detectors']:
                if self.getProp('DetectorSim')['Detectors'].count(det) == 0 :
                    raise RuntimeError("Monitoring has been required for '%s' sub-detector but it has been removed from Simulation" %det)


    def checkIncompatibleDetectors ( self ) :
        for section in self._incompatibleDetectors.keys():
            incompatList = self._incompatibleDetectors[section]
            myList = [det for det in self.getProp("DetectorGeo")['Detectors'] if det in incompatList]
            if len(myList) > 1:
                raise RuntimeError ( "Incompatible detectors: %s in %s section." %(myList, section) )


        
    ##
    def outputName(self):
        """
        Build a name for the output file, based on input options.
        Combines DatasetName, EventType, Number of events and Date
        """
        import time
        outputName = self.getProp("DatasetName")
        if self.eventType() != "":
            if outputName != "": outputName += '-'
            outputName += self.eventType()
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        if outputName == "": outputName = 'Gauss'
        idFile = str(time.localtime().tm_year)
        if time.localtime().tm_mon < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mon)
        if time.localtime().tm_mday < 10:
            idFile += '0'
        idFile += str(time.localtime().tm_mday)
        outputName += '-' + idFile
        return outputName


#"""
# ><<<<<                  ><<   ><<<<<<<                                        ><<  
# ><<   ><<             ><      ><<    ><<                          ><          ><<  
# ><<    ><<   ><<    ><>< ><   ><<    ><<   ><<    >< ><<< ><<<<       ><<<< ><>< ><
# ><<    ><< ><   ><<   ><<     ><<<<<<<   ><   ><<  ><<   ><<     ><< ><<      ><<  
# ><<    ><<><<<<< ><<  ><<     ><<       ><<<<< ><< ><<     ><<<  ><<   ><<<   ><<  
# ><<   ><< ><          ><<     ><<       ><         ><<       ><< ><<     ><<  ><<  
# ><<<<<      ><<<<     ><<     ><<         ><<<<   ><<<   ><< ><< ><< ><< ><<   ><< 
#"""


    ##

#"""
#
# ><<<<<<<                                   ><< <<                        ><<                            ><<
# ><<    ><<                               ><<    ><<  ><               ><<   ><<                       ><   
# ><<    ><< >< ><<<   ><<     >< ><<       ><<           ><<< ><< ><< ><<          ><<     ><< ><<   ><>< ><
# ><<<<<<<    ><<    ><<  ><<  ><  ><<        ><<     ><<  ><<  ><  ><<><<        ><<  ><<   ><<  ><<   ><<  
# ><<         ><<   ><<    ><< ><   ><<          ><<  ><<  ><<  ><  ><<><<       ><<    ><<  ><<  ><<   ><<  
# ><<         ><<    ><<  ><<  ><< ><<     ><<    ><< ><<  ><<  ><  ><< ><<   ><< ><<  ><<   ><<  ><<   ><<  
# ><<        ><<<      ><<     ><<           ><< <<   ><< ><<<  ><  ><<   ><<<<     ><<     ><<<  ><<   ><<  
#                              ><<                                                                           
#
#"""

    def propagateSimConf( self ):
        # Propagate properties to SimConf
        SimConf().setProp("Writer","GaussTape")
        self.setOtherProps( SimConf(), ["SpilloverPaths","EnablePack","Phases","DataType"] )

        
        # CRJ : Propagate detector list to SimConf. Probably could be simplified a bit
        #       by sychronising the options in Gauss() and SimConf()
        detlist = []
        if 'Velo'    in self.getProp('DetectorSim')['Detectors'] : detlist += ['Velo']
        if 'PuVeto'  in self.getProp('DetectorSim')['Detectors'] : detlist += ['PuVeto']
        if 'TT'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['TT']
        if 'IT'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['IT']
        if 'OT'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['OT']
        if [det for det in ['Rich1', 'Rich2', 'Rich1Pmt', 'Rich2Pmt'] if det in self.getProp('DetectorSim')['Detectors']] :
            detlist += ['Rich']
        if 'Muon'    in self.getProp('DetectorSim')['Detectors'] : detlist += ['Muon']
        if 'Spd'     in self.getProp('DetectorSim')['Detectors'] : detlist += ['Spd']
        if 'Prs'     in self.getProp('DetectorSim')['Detectors'] : detlist += ['Prs']
        if 'Ecal'    in self.getProp('DetectorSim')['Detectors'] : detlist += ['Ecal']
        if 'Hcal'    in self.getProp('DetectorSim')['Detectors'] : detlist += ['Hcal']
        # PSZ - add upgrade detectors here
        if 'VP'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['VP']
        if 'UT'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['UT']
        if 'FT'      in self.getProp('DetectorSim')['Detectors'] : detlist += ['FT']
        if 'SL'    in self.getProp('DetectorSim')['Detectors'] : detlist += ['SL']
        # if Skip4 then dont propagate the detector list
        if "GenToMCTree" in self.getProp("Phases"):
            detlist = []

        SimConf().setProp("Detectors",detlist)


        # Don't want SIM data unpacking enabled in DoD service
        SimConf().EnableUnpack = False


#"""
#      ><<                                                              ><<                     ><<  
#   ><<   ><<                                  ><                       ><<        ><           ><<  
#  ><<        >< ><<<   ><<     ><<<<  ><<<<      ><< ><<      ><<      ><<            ><<<<  ><>< ><
#  ><<         ><<    ><<  ><< ><<    ><<     ><<  ><<  ><<  ><<  ><<   ><<       ><< ><<       ><<  
#  ><<         ><<   ><<    ><<  ><<<   ><<<  ><<  ><<  ><< ><<   ><<   ><<       ><<   ><<<    ><<  
#   ><<   ><<  ><<    ><<  ><<     ><<    ><< ><<  ><<  ><<  ><<  ><<   ><<       ><<     ><<   ><<  
#     ><<<<   ><<<      ><<    ><< ><<><< ><< ><< ><<<  ><<      ><<    ><<<<<<<< ><< ><< ><<    ><< 
#                                                            ><<                                    
#"""
    def defineCrossingList( self ):
        crossingList = [ '' ]
        spillOverList = self.getProp("SpilloverPaths")
        while '' in spillOverList :
            spillOverList.remove('')
        crossingList += spillOverList
        return crossingList


#"""
#
#  ><< ><<                                          ><<<<<<<                                                      
#  ><    ><<                                        ><<    ><<                                                    
#  ><     ><<    ><<        ><<     ><<< ><< ><<    ><<    ><<    ><<     >< ><<<    ><<     ><<< ><< ><<   ><<<< 
#  ><<< ><     ><   ><<   ><<  ><<   ><<  ><  ><<   ><<<<<<<    ><<  ><<   ><<     ><<  ><<   ><<  ><  ><< ><<    
#  ><     ><< ><<<<< ><< ><<   ><<   ><<  ><  ><<   ><<        ><<   ><<   ><<    ><<   ><<   ><<  ><  ><<   ><<< 
#  ><      >< ><         ><<   ><<   ><<  ><  ><<   ><<        ><<   ><<   ><<    ><<   ><<   ><<  ><  ><<     ><<
#  ><<<< ><<    ><<<<      ><< ><<< ><<<  ><  ><<   ><<          ><< ><<< ><<<      ><< ><<< ><<<  ><  ><< ><< ><<
#"""
    #--Set the energy of the beam,
    #--the half effective crossing angle (in LHCb coordinate system),
    #--beta* and emittance
    #--and configure the colliding beam tool for all type of events in
    #--pp collisions.
    def setBeamParameters( self, CrossingSlots, genInit):

        from Configurables import ( MinimumBias , FixedNInteractions , HijingProduction )
        from Configurables import ( Special , Pythia8Production )
        from Configurables import ( Generation )

        #
        beamMom                        = self.getProp("BeamMomentum")
        xAngle                         = self.getProp("BeamHCrossingAngle")
        yAngle                         = self.getProp("BeamVCrossingAngle")
        xAngleBeamLine, yAngleBeamLine = self.getProp("BeamLineAngles")
        emittance                      = self.getProp("BeamEmittance")
        betaStar                       = self.getProp("BeamBetaStar")
        lumiPerBunch                   = self.getProp("Luminosity")
        totCrossSection                = self.getProp("TotalCrossSection")
        meanX, meanY, meanZ            = self.getProp("InteractionPosition")
        sigmaS                         = self.getProp("BunchRMS")


        # Give beam parameters to GenInit algorithm
        genInit.CreateBeam              = True
        genInit.BeamEnergy              = beamMom
        genInit.HorizontalCrossingAngle = xAngle
        genInit.VerticalCrossingAngle   = yAngle
        genInit.NormalizedEmittance     = emittance
        genInit.BetaStar                = betaStar
        genInit.HorizontalBeamlineAngle = xAngleBeamLine
        genInit.VerticalBeamlineAngle   = yAngleBeamLine
        genInit.Luminosity              = lumiPerBunch
        genInit.TotalCrossSection       = totCrossSection
        genInit.XLuminousRegion         = meanX
        genInit.YLuminousRegion         = meanY
        genInit.ZLuminousRegion         = meanZ
        genInit.BunchLengthRMS          = sigmaS
    
        gen_t0 = Generation("Generation")
        
        # the following is for beam gas events, the values are just to give the
        # nominal beam conditions in the data but 1 single interaction is 
        # forced selecting the appropriate pileup tool in the eventtype
        gen_t0.addTool(FixedNInteractions,name="FixedNInteractions")
        gen_t0.FixedNInteractions.NInteractions = 1

        # or with Hijing
        pInGeV   = beamMom*SystemOfUnits.GeV/SystemOfUnits.TeV
        txtP = "hijinginit efrm "+str(pInGeV)
        gen_t0.addTool(MinimumBias,name="MinimumBias")
        gen_t0.MinimumBias.addTool(HijingProduction,name="HijingProduction")
        gen_t0.MinimumBias.HijingProduction.Commands += [ txtP ]
    #--For beam gas events (with hijing) only the energy of the beams is set
    ## end of functions to set beam paramters and propagate them
    ##########################################################################



#"""
#      ><<                             ><<       ><<<<                        
#   ><<   ><<                        ><        ><    ><<                      
#  ><<           ><<     ><< ><<   ><>< ><    ><<            ><<     ><< ><<  
#  ><<         ><<  ><<   ><<  ><<   ><<      ><<          ><   ><<   ><<  ><<
#  ><<        ><<    ><<  ><<  ><<   ><<      ><<   ><<<< ><<<<< ><<  ><<  ><<
#   ><<   ><<  ><<  ><<   ><<  ><<   ><<       ><<    ><  ><          ><<  ><<
#     ><<<<      ><<     ><<<  ><<   ><<        ><<<<<      ><<<<    ><<<  ><<
#                                                                           
#"""
    def configureGen( self, SpillOverSlots ):
        """
        Set up the generator execution sequence and its sub-phases
        """
        
##         if "Gen" not in self.getProp("MainSequence") :
##             log.warning("No generator phase. Need input file")
##             return

        if self.evtMax() <= 0:
            raise RuntimeError( "Generating events but selected '%s' events. Use LHCbApp().EvtMax " %self.evtMax() )

        gaussGeneratorSeq = GaudiSequencer( "Generator", IgnoreFilterPassed = True )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussGeneratorSeq ]

        from Configurables import ( EvtGenDecay )
        EvtGenDecay().DecayFile = "$DECFILESROOT/dkfiles/DECAY.DEC"


        from Configurables import ( GenInit, Generation )
        for slot in SpillOverSlots:
            genSequence = GaudiSequencer("GeneratorSlot"+self.slotName(slot)+"Seq" )
            gaussGeneratorSeq.Members += [ genSequence ]

            TESNode = "/Event/"+self.slot_(slot)
            genInit = GenInit("GaussGen"+slot,
                              MCHeader = TESNode+"Gen/Header")

            if slot != '':
                genInitT0 = GenInit("GaussGen")
                if genInitT0.isPropertySet("RunNumber"):
                    genInit.RunNumber = genInitT0.RunNumber
                if genInitT0.isPropertySet("FirstEventNumber"):
                    genInit.FirstEventNumber = genInitT0.FirstEventNumber
                    

            genProc = 0
            genType = self.getProp("Production").upper()
            from Configurables import ParticleGun, MIBackground
            KnownGenTypes = ['PHYS','PGUN','MIB']
            if genType not in KnownGenTypes:
                raise RuntimeError("Unknown Generation type '%s'"%genType)
            if genType == 'PHYS':
                genProc = Generation("Generation"+slot) 
            elif genType == 'PGUN':
                genProc = ParticleGun("ParticleGun"+slot)
            else:
                genProc = MIBackground("MIBackground"+slot)

            genProc.GenHeaderLocation = TESNode+"Gen/Header"
            genProc.HepMCEventLocation = TESNode+"Gen/HepMCEvents" 
            genProc.GenCollisionLocation = TESNode+"Gen/Collisions"

            if slot != '':
                genProc.PileUpTool = 'FixedLuminosityForSpillOver' 

            genSequence.Members += [ genInit , genProc ]
    ## end of Gen configuration
    ##########################################################################


#"""
#     ><<                             ><<    ><<<<<<<                                                 
#  ><<   ><<                        ><       ><<    ><< ><<                                           
# ><<           ><<     ><< ><<   ><>< ><    ><<    ><< ><<         ><<      ><<<<     ><<      ><<<< 
# ><<         ><<  ><<   ><<  ><<   ><<      ><<<<<<<   >< ><     ><<  ><<  ><<      ><   ><<  ><<    
# ><<        ><<    ><<  ><<  ><<   ><<      ><<        ><<  ><< ><<   ><<    ><<<  ><<<<< ><<   ><<< 
#  ><<   ><<  ><<  ><<   ><<  ><<   ><<      ><<        ><   ><< ><<   ><<      ><< ><             ><<
#    ><<<<      ><<     ><<<  ><<   ><<      ><<        ><<  ><<   ><< ><<< ><< ><<   ><<<<    ><< ><<
#"""
    def configurePhases( self, SpillOverSlots  ):
        """
        Set up the top level sequence and its phases
        """

        gaussSeq = GaudiSequencer("GaussSequencer")
        ApplicationMgr().TopAlg = [ gaussSeq ]
##         mainSeq = self.getProp("MainSequence")
##         if len( mainSeq ) == 0:
##             mainSeq = self.DefaultSequence

##         mainSeq = map(lambda ph: ph.capitalize(), mainSeq)
##         self.setProp("MainSequence",mainSeq)
##         for phase in mainSeq:
##             raise RuntimeError("Unknown phase '%s'"%phase)

        ### Check for configuration consistency
        if ( ( "GenToMCTree" in self.getProp("Phases") ) and ( "Simulation" in self.getProp("Phases") ) ):
            raise RuntimeError("GenToMCTree and Simulation cannot be part of Phases simultaneously")
                
        self.configureGen( SpillOverSlots )
        if "GenToMCTree" in self.getProp("Phases"):
            self.configureSkipGeant4( SpillOverSlots ) 
        self.configureSim( SpillOverSlots )
        self.configureMoni( SpillOverSlots ) #(expert or default)
    ## end of phase configuration
    ##########################################################################



#"""
# ><<<<<                    ><<       ><<<<                 ><<                       ><<  
# ><<   ><<               ><        ><<    ><<              ><<                       ><<  
# ><<    ><<    ><<     ><>< ><   ><<        ><< ><<  ><< ><>< >< >< ><<   ><<  ><< ><>< ><
# ><<    ><<  ><   ><<    ><<     ><<        ><< ><<  ><<   ><<   ><  ><<  ><<  ><<   ><<  
# ><<    ><< ><<<<< ><<   ><<     ><<        ><< ><<  ><<   ><<   ><   ><< ><<  ><<   ><<  
# ><<   ><<  ><           ><<       ><<     ><<  ><<  ><<   ><<   ><< ><<  ><<  ><<   ><<  
# ><<<<<       ><<<<      ><<         ><<<<        ><<><<    ><<  ><<        ><<><<    ><< 
#                                                                 ><<                      
#"""

    def defineOutput( self, SpillOverSlots ):
        """
        Set up output stream according to phase processed, the spill-over slots and the type of output
        """

        #
        knownOptions = ['NONE','GEN','XGEN','RGEN','SIM','XSIM']
        output = self.getProp("OutputType").upper()
        if output == 'NONE':
            log.warning("No event data output produced")
            return
        
        simWriter = SimConf().writer()

        # define default file extensions depending on the phase that has been run
        fileDefaultExtension = ".gen"
        fileAllowedExtension = [fileDefaultExtension]
        if "GenToMCTree" in self.getProp("Phases"):
            fileDefaultExtension = ".xgen"
            fileAllowedExtension = [fileDefaultExtension, ".rgen"]
        elif "Simulation" in self.getProp("Phases"):
            fileDefaultExtension = ".sim"
            fileAllowedExtension = [fileDefaultExtension, ".xsim"]

        # choose the file extension from the one selected compatibly with the phase run
        if output not in knownOptions:
            print "WARNING: OutputType not supported. Use default for chosen phases : %s" %(fileDefaultExtension)
        fileExtension = "." + output.lower()
        if fileExtension not in fileAllowedExtension:
            fileExtension = fileDefaultExtension
            print "WARNING: OutputType not supported for this phase. Use default : %s" %(fileExtension)

        # set saving or not of HepMC depending on chosen file extension
        if SimConf().isPropertySet( "SaveHepMC" ):
            print "WARNING: SimConf().SaveHepMC will be ignored. Value set by Gauss()"
        saveHepMC = False
        if fileExtension in ['.gen','.xgen','.xsim']:
            saveHepMC = True
        SimConf().setProp("SaveHepMC", saveHepMC )
       
        outputFile=""
        from GaudiConf import IOHelper
        if simWriter.isPropertySet( "Output" ):
            outputFile=IOHelper().undressFile(simWriter.getProp("Output"))
        else:
            outputFile=self.outputName() + fileExtension
        
        IOHelper().outStream( outputFile, simWriter, self.getProp("WriteFSR") )
        
        simWriter.RequireAlgs.append( 'GaussSequencer' )
        if not FileCatalog().isPropertySet("Catalogs"):
            FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]
        
#"""
#
# ><<<<<                    ><<   ><<       ><<                            ><<                             
# ><<   ><<               ><      >< ><<   ><<<                       ><   ><<                             
# ><<    ><<    ><<     ><>< ><   ><< ><< > ><<    ><<     ><< ><<       ><>< ><    ><<     >< ><<<  ><<<< 
# ><<    ><<  ><   ><<    ><<     ><<  ><<  ><<  ><<  ><<   ><<  ><< ><<   ><<    ><<  ><<   ><<    ><<    
# ><<    ><< ><<<<< ><<   ><<     ><<   ><  ><< ><<    ><<  ><<  ><< ><<   ><<   ><<    ><<  ><<      ><<< 
# ><<   ><<  ><           ><<     ><<       ><<  ><<  ><<   ><<  ><< ><<   ><<    ><<  ><<   ><<        ><<
# ><<<<<       ><<<<      ><<     ><<       ><<    ><<     ><<<  ><< ><<    ><<     ><<     ><<<    ><< ><<
#
#"""
    
    def defineMonitors( self ):

        from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool
        ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        #SequencerTimerTool().OutputLevel = WARNING

        # Set printout level and longer algorithm" identifier in printout
        MessageSvc().OutputLevel = INFO
        #ToolSvc.EvtGenTool.OutputLevel = 4 is it still necessary to reduce print?
        MessageSvc().setWarning.append( 'XmlGenericCnv' )
        if not MessageSvc().isPropertySet("Format"):
            MessageSvc().Format = '% F%24W%S%7W%R%T %0W%M'


#"""
#   ><< <<                                        ><<     ><<               ><<                     
# ><<    ><<                                      ><<     ><<  ><           ><<                     
#  ><<          ><<     ><<     ><<    ><<        ><<     ><<      ><<<<  ><>< ><    ><<      ><<<< 
#    ><<      ><<  ><<   ><<   ><<   ><   ><<     ><<<<<< ><< ><< ><<       ><<    ><<  ><<  ><<    
#       ><<  ><<   ><<    ><< ><<   ><<<<< ><<    ><<     ><< ><<   ><<<    ><<   ><<    ><<   ><<< 
# ><<    ><< ><<   ><<     ><><<    ><            ><<     ><< ><<     ><<   ><<    ><<  ><<      ><<
#   ><< <<     ><< ><<<     ><<       ><<<<       ><<     ><< ><< ><< ><<    ><<     ><<     ><< ><<
#"""

    def saveHistos( self ):
        """
        Set up histogram service and file name unless done in job
        """

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        histOpt = self.getProp("Histograms").upper()
        if histOpt not in self.KnownHistOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)
            # HistogramPersistency needed to read in histogram for calorimeter
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # If not saving histograms do not set the name of the file
        if ( histOpt == 'NONE') :
            log.warning("No histograms produced")
            return
        
        # Use a default histogram file name if not already set
        if not HistogramPersistencySvc().isPropertySet( "OutputFile" ):
            histosName = self.getProp("DatasetName")
            histosName = self.outputName() + '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName
            







#"""
#GEO
#Geo
#
#     ><<<<                         
#   ><    ><<                       
#  ><<            ><<        ><<    
#  ><<          ><   ><<   ><<  ><< 
#  ><<   ><<<< ><<<<< ><< ><<    ><<
#   ><<    ><  ><          ><<  ><< 
#    ><<<<<      ><<<<       ><<    
#                                 
#"""



    def defineGeoBasePieces( self, basePieces ):
        #basePieces['BeforeMagnetRegion']=['Velo2Rich1']
        basePieces['UpstreamRegion']     = []
        basePieces['BeforeMagnetRegion'] = []
        basePieces['MagnetRegion']       = []
        basePieces['AfterMagnetRegion']  = []
        basePieces['DownstreamRegion']   = []

        #basePieces['UpstreamRegion']=[]
        #basePieces['BeforeMagnetRegion']=[]
        #basePieces['MagnetRegion']=['PipeInMagnet','PipeSupportsInMagnet']
        #basePieces['AfterMagnetRegion']=['PipeAfterT','PipeSupportsAfterMagnet']
        #basePieces['DownstreamRegion']=['PipeDownstream','PipeSupportsDownstream','PipeBakeoutDownstream']

    # This is where everything is parsed into geo items
    def defineStreamItemsGeo( self, geo, basePieces, detPieces ):
        for region in basePieces.keys():
            path = "/dd/Structure/LHCb/"+region+"/"
            if len(detPieces[region])==0 : continue
            # This should preserve order
            for element in basePieces[region] + detPieces[region]:
                myStreamItem = path + element
                if myStreamItem not in geo.StreamItems:
                    geo.StreamItems += [ myStreamItem ]
            #for element in detPieces[region]:
            #    myStreamItem = path + element
            #    if myStreamItem not in geo.StreamItems:
            #        geo.StreamItems += [ myStreamItem ]



#"""
#  ><<<<<                    ><<       ><<<<                         
#  ><<   ><<               ><        ><    ><<                       
#  ><<    ><<    ><<     ><>< ><    ><<            ><<        ><<    
#  ><<    ><<  ><   ><<    ><<      ><<          ><   ><<   ><<  ><< 
#  ><<    ><< ><<<<< ><<   ><<      ><<   ><<<< ><<<<< ><< ><<    ><<
#  ><<   ><<  ><           ><<       ><<    ><  ><          ><<  ><< 
#  ><<<<<       ><<<<      ><<        ><<<<<      ><<<<       ><<    
#
#"""

    def defineDetectorGeo( self, basePieces, detPieces, det ):
        import string
        lDet = det.lower()
        if lDet not in self.__knownDetectors__:
            log.warning("Geo Detector not known : %s" %(det))

        if lDet == "magnet":
            self.defineMagnetGeo( basePieces, detPieces )
        elif lDet == "puveto":
            self.definePuVetoGeo( )
        elif lDet == "velo":
            self.defineVeloGeo( basePieces, detPieces )
        elif lDet == "tt":
            self.defineTTGeo( detPieces )
        elif lDet == "it":
            self.defineITGeo( detPieces )
        elif lDet == "ot":
            self.defineOTGeo( detPieces )
        elif lDet == "muon":
            self.defineMuonGeo( detPieces )
        elif lDet == "rich1":
            self.defineRich1GeoDet( detPieces )
        elif lDet == "rich2":
            self.defineRich2GeoDet( detPieces )
        elif lDet == "spd":
            self.defineSpdGeo( detPieces )
        elif lDet == "prs":
            self.definePrsGeo( detPieces )
        elif lDet == "ecal":
            self.defineEcalGeo( detPieces )
        elif lDet == "hcal":
            self.defineHcalGeo( detPieces )
        # Upgrade detectors below
        elif lDet == "vp":
            self.defineVPGeo( detPieces )
        elif lDet == "torch":
            self.defineTorchGeo()
        elif lDet == "ft":
            self.defineFTGeo( detPieces )
        elif lDet == "rich1pmt":
            self.defineRich1MaPmtGeoDet( detPieces )
        elif lDet == "rich2pmt":
            self.defineRich2MaPmtGeoDet( detPieces )
        elif lDet == "ut":
            self.defineUTGeo( detPieces )
        elif lDet == "sl":
            self.defineSLGeo( detPieces )
        else:
            log.warning("Geo Detector not known : %s" %(det))
            
    def defineDetectorGeoStream ( self, geo, giGaGeo, det ):
        import string
        lDet = det.lower()
        if lDet not in self.__knownDetectors__:
            log.warning("Geo Stream Detector not known : %s" %(det))

        if lDet == "rich1":
            self.defineRich1GeoStream( geo )
        elif lDet == "rich2":
            self.defineRich2GeoStream( geo )
        elif lDet == "rich1pmt":
            self.defineRich1MaPmtGeoStream( geo,  giGaGeo )
        elif lDet == "rich2pmt":
            self.defineRich2MaPmtGeoStream( geo, giGaGeo )
        elif lDet == "magnet":
            self.defineMagnetGeoField( giGaGeo )



    def defineGDMLGeoStream ( self, geo, giGaGeo ):
        if self.getProp("ReplaceWithGDML"):
            gdmlOpt = self.getProp("ReplaceWithGDML")
            if gdmlOpt[0]["volsToReplace"]:
                for gdmlDict in self.getProp("ReplaceWithGDML"):
                    self.defineGDMLGeo ( geo, giGaGeo, gdmlDict )

            


    def defineGeo( self ):
        # Define the simulated geometry
        geo = GiGaInputStream( "Geo",
                               ExecuteOnce = True,
                               ConversionSvcName = "GiGaGeo",
                               DataProviderSvcName  = "DetectorDataSvc" )

        gaussSimulationSeq = GaudiSequencer("Simulation")
        gaussSimulationSeq.Members += [ geo ]

        # Detector geometry to simulate
        detPieces = {'UpstreamRegion':[], 'BeforeMagnetRegion':[],'AfterMagnetRegion':[],'DownstreamRegion':[],'MagnetRegion':[]}
        #detPieces = {'BeforeMagnetRegion':[],'AfterMagnetRegion':[],'DownstreamRegion':[],'MagnetRegion':[]}
        basePieces = {}

        # Must be first!
        # This used to list all the beampipe detector elements
        # which are not "inside" another detector.
        # i.e. top-level detector elements
        # DDDB structure may change in future
        self.defineGeoBasePieces( basePieces )

        # Define beampipe 
        self.validateBeamPipeSwitch ( self.getProp("BeamPipe") )
        if ("BeamPipeOn" == self.getProp("BeamPipe")):
            # BeamPipe on - add BP elements
            self.defineBeamPipeGeo ( geo, basePieces, detPieces )

        # Use information from SIMCOND and GeometryInfo
        # Allows to be set to False by RichXPmt
        giGaGeo = GiGaGeo()
        giGaGeo.UseAlignment      = True
        giGaGeo.AlignAllDetectors = True


        # Define detectors
        for det in self.getProp('DetectorGeo')['Detectors']:
            det = "%s" %det
            self.defineDetectorGeo( basePieces, detPieces, det )

        # StreamItems definition needs to be after det definition
        self.defineStreamItemsGeo( geo, basePieces, detPieces )

        # Define detector streams for RICHes
        for det in self.getProp('DetectorGeo')['Detectors']:
            det = "%s" %det
            self.defineDetectorGeoStream( geo, giGaGeo, det )

        # Seperate Calo opts
        # Returns a list containing all the elments common to both lists
        if [det for det in ['Spd', 'Prs', 'Ecal', 'Hcal'] if det in self.getProp('DetectorGeo')['Detectors']]:
            importOptions("$GAUSSCALOROOT/options/Calo.opts")

        # Call GDML description
        self.defineGDMLGeoStream( geo, giGaGeo )

        if self.getProp("Debug"):
            print "\nDEBUG Detector Geometry Elements:"
            print "\nkey : detPieces[key]"
            for key in detPieces.keys():
                print "%s : %s" %(key, detPieces[key])
            print "\nkey : detPieces[key]"
            
            for key in sorted(detPieces.keys()):
                print "%s : %s" %(key, detPieces[key])

            print "\nkey : basePieces[key]"
            for key in basePieces.keys():
                print "%s : %s" %(key, basePieces[key])

            print "\nkey : Sorted basePieces[key]"
            for key in sorted(basePieces.keys()):
                print "%s : %s" %(key, basePieces[key])

            print "\ngeo StreamItems:"
            for item in geo.StreamItems:
                print "%s" %(item)

            print "\ngeo StreamItems SORTED:"
            mySortedGeoStream = geo.StreamItems[:]
            mySortedGeoStream.sort()
            for item in mySortedGeoStream:
                print "%s" %(item)

        # No BP requested - therefore remove all elements from Geo.StreamItems
        if ("BeamPipeOff" == self.getProp("BeamPipe")):
            self.removeAllBeamPipeElements()


#"""
#     ><<                             ><<      ><< <<                    
#  ><<   ><<                        ><       ><<    ><<  ><              
# ><<           ><<     ><< ><<   ><>< ><     ><<           ><<< ><< ><< 
# ><<         ><<  ><<   ><<  ><<   ><<         ><<     ><<  ><<  ><  ><<
# ><<        ><<    ><<  ><<  ><<   ><<            ><<  ><<  ><<  ><  ><<
#  ><<   ><<  ><<  ><<   ><<  ><<   ><<      ><<    ><< ><<  ><<  ><  ><<
#    ><<<<      ><<     ><<<  ><<   ><<        ><< <<   ><< ><<<  ><  ><<
#"""

    def configureDetectorSim( self, slot, detHits, det, configuredRichSim ):
        import string
        det = det.lower()
        if det not in self.__knownDetectors__:
            log.warning("Sim Detector not known : %s" %(det))

        if det == "puveto":
            self.configurePuVetoSim( slot, detHits )
        elif det == "velo":
            self.configureVeloSim( slot, detHits )
        elif det == "tt":
            self.configureTTSim( slot, detHits )
        elif det == "it":
            self.configureITSim( slot, detHits )
        elif det == "ot":
            self.configureOTSim( slot, detHits )
        elif det == "muon":
            self.configureMuonSim( slot, detHits )
        elif det in ['rich1', 'rich2']:
            if not configuredRichSim[0]:
                self.configureRichSim( slot, detHits )
                configuredRichSim[0] = True
        elif det == "spd":
            self.configureSpdSim( slot, detHits )
        elif det == "prs":
            self.configurePrsSim( slot, detHits )
        elif det == "ecal":
            self.configureEcalSim( slot, detHits )
        elif det == "hcal":
            self.configureHcalSim( slot, detHits )
        # Upgrade detectors below
        elif det == "vp":
            self.configureVPSim( slot, detHits )
        elif det == "torch":
            self.configureTorchSim( slot, detHits )
        elif det == "ft":
            self.configureFTSim( slot, detHits )
        elif det in ['rich1pmt', 'rich2pmt']:
            if not configuredRichSim[0]:
                self.configureRichMaPmtSim( slot, detHits )
                configuredRichSim[0] = True
        elif det == "ut":
            self.configureUTSim( slot, detHits )
        elif det == "sl":
            self.configureSLSim( slot, detHits )
        else:
            log.warning("Sim Detector not known : %s" %(det))

    ##
    ##
    def configureSim( self, SpillOverSlots ):

        """
        Set up the simulation sequence
        """
        
        if "Simulation" not in self.getProp("Phases"):
            log.warning("No simulation phase.")
            return
        
        ApplicationMgr().ExtSvc += [ "GiGa" ]
        EventPersistencySvc().CnvServices += [ "GiGaKine" ]

        gaussSimulationSeq = GaudiSequencer( "Simulation" )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussSimulationSeq ]

        gigaStore = GiGaDataStoreAlgorithm( "GiGaStore" )
        gigaStore.ConversionServices = [ "GiGaKine" ]
        gaussSimulationSeq.Members += [ gigaStore ] 

        self.defineGeo()

        self.configureGiGa()

        for slot in SpillOverSlots:

            TESNode = "/Event/"+self.slot_(slot)
            
            mainSimSequence = GaudiSequencer( self.slotName(slot)+"EventSeq" )

            gaussSimulationSeq.Members += [ mainSimSequence ]

            mainSimSequence.Members +=  [ SimInit( self.slotName(slot)+"EventGaussSim",
                                                   GenHeader = TESNode + "Gen/Header" ,
                                                   MCHeader = TESNode + "MC/Header" ) ]

            simSeq = GaudiSequencer( self.slotName(slot)+"Simulation",
                                     RequireObjects = [ TESNode + "Gen/HepMCEvents" ] )
            mainSimSequence.Members += [ simSeq ]

            simSlotSeq = GaudiSequencer( "Make"+self.slotName(slot)+"Sim" )
            simSeq.Members += [simSlotSeq]

            # CRJ : Set RootInTES - Everything down stream will then use the correct location
            #       (assuming they use GaudiAlg get and put) so no need to set data locations
            #       by hand any more ...
            if slot != '' : simSlotSeq.RootInTES = slot

            genToSim = GenerationToSimulation( "GenToSim" + slot,
                                               LookForUnknownParticles = True )
            simSlotSeq.Members += [ genToSim ]
            
            simSlotSeq.Members += [ GiGaFlushAlgorithm( "GiGaFlush"+slot ) ]
            simSlotSeq.Members += [ GiGaCheckEventStatus( "GiGaCheckEvent"+slot ) ]
            simToMC = SimulationToMCTruth( "SimToMCTruth"+slot )
            simSlotSeq.Members += [ simToMC ]

            ## Detectors hits
            TESNode = TESNode + "MC/"
            detHits = GaudiSequencer( "DetectorsHits" + slot )  
            simSlotSeq.Members += [ detHits ]

            # Slight trick - configuredRichSim is a list and therefore MUTABLE!
            configuredRichSim = [ False ]
            for det in self.getProp('DetectorSim')['Detectors']:
                self.configureDetectorSim( slot, detHits, det, configuredRichSim )
            

            # Data packing ...
            if self.getProp("EnablePack") :
                packing = GaudiSequencer(self.slotName(slot)+"EventDataPacking")
                simSlotSeq.Members += [ packing ]
                SimConf().PackingSequencers[slot] = packing             
        # End of Sim Configuration

    


#"""
#  ><<       ><<                         
#  >< ><<   ><<<                       ><
#  ><< ><< > ><<    ><<     ><< ><<      
#  ><<  ><<  ><<  ><<  ><<   ><<  ><< ><<
#  ><<   ><  ><< ><<    ><<  ><<  ><< ><<
#  ><<       ><<  ><<  ><<   ><<  ><< ><<
#  ><<       ><<    ><<     ><<<  ><< ><<
#                                      
#
#"""


    def configureGeneratorMoni ( self, SpillOverSlots ):
            
        # Monitors for the generator:
        for slot in SpillOverSlots:

            genSequence = GaudiSequencer("GeneratorSlot" + self.slotName(slot) + "Seq" )
            genMoniSeq = GaudiSequencer("GenMonitor" + slot )
            genSequence.Members += [ genMoniSeq ]

            TESLocation = "/Event/"+self.slot_(slot)+"Gen/HepMCEvents"
            genMoniSeq.Members += [
                GenMonitorAlg(
                "GenMonitorAlg"+slot,
                HistoProduce=True,
                Input = TESLocation
                )
                ]
            #            if moniOpt == 'Debug':
            #                genMoniSeq.Members += [ DumpHepMC( "DumpHepMC"+slot,
            #                                                   OutputLevel=1,
            #                                                   Addresses = [TESLocation] ) ]


    def configureDetectorMoni( 
        self, 
        slot,
        packCheckSeq,
        detMoniSeq,
        checkHits,
        det,
        configuredRichMoni
        ):
        import string
        det = det.lower()
        if det not in self.__knownDetectors__:
            log.warning("Moni Detector not known : %s" %(det))

        if det == "puveto":
            self.configurePuVetoMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "velo":
            self.configureVeloMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "tt":
            self.configureTTMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "it":
            self.configureITMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "ot":
            self.configureOTMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "muon":
            self.configureMuonMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        # Since I'm switching on the subcomponents I shouldn't configure this twice.
        elif (det == "rich1") or (det == "rich2"):
            if not configuredRichMoni[0]:
                self.configureRichMoni(
                    slot,
                    packCheckSeq,
                    detMoniSeq,
                    checkHits,
                    configuredRichMoni
                    )
                configuredRichMoni[0] = True
        elif det == "prs":
            self.configurePrsMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "spd":
            self.configureSpdMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "ecal":
            self.configureEcalMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "hcal":
            self.configureHcalMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        # Upgrade detectors below
        elif det == "vp":
            self.configureVPMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "torch":
            self.configureTorchMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "ft":
            self.configureFTMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif (det == "rich1pmt") or (det == "rich2pmt"):
            if not configuredRichMoni[0]:
                self.configureRichMaPmtMoni(
                    slot,
                    packCheckSeq,
                    detMoniSeq,
                    checkHits,
                    configuredRichMoni
                    )
                configuredRichMoni[0] = True
        elif det == "ut":
            self.configureUTMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        elif det == "sl":
            self.configureSLMoni( slot, packCheckSeq, detMoniSeq, checkHits )
        else:
            log.warning("Moni Detector not known : %s" %(det))            

    def resetCheckHits( self, checkHits ):
        checkHits.TTHits     = ''
        checkHits.OTHits     = ''
        checkHits.ITHits     = ''
        checkHits.PuVetoHits = ''
        checkHits.VeloHits   = ''
        checkHits.MuonHits   = ''
        checkHits.RichHits   = ''
        checkHits.CaloHits   = []

        # checkHits has as default values:
        # |-TTHits                    = 'MC/TT/Hits'
        # |-OTHits                    = 'MC/OT/Hits'
        # |-ITHits                    = 'MC/IT/Hits'
        # |-PuVetoHits                = 'MC/PuVeto/Hits'
        # |-VeloHits                  = 'MC/Velo/Hits'
        # |-MuonHits                  = 'MC/Muon/Hits'
        # |-RichTracks                = ''
        # |-RichHits                  = 'MC/Rich/Hits'
        # |-CaloHits                  = ['MC/Spd/Hits', 'MC/Prs/Hits', 'MC/Ecal/Hits', 'MC/Hcal/Hits']



    def configureSimulationMoni( self, SpillOverSlots ):
        
        configuredRichMoni = [ False , False ]
            
        # Monitors for simulation
        for slot in SpillOverSlots:
            # Reset param to configure rich for slots.
            configuredRichMoni[0] = False
            TESNode = "/Event/"+self.slot_(slot)
     
            simSequence = GaudiSequencer( self.slotName(slot)+"Simulation" )
            simMoniSeq = GaudiSequencer( "SimMonitor" + slot )
            simSequence.Members += [ simMoniSeq ]

            # CRJ : Set RootInTES - Everything down stream will then use the correct location
            #       (assuming they use GaudiAlg get and put) so no need to set data locations
            #       by hand any more ...
            if slot != '' : simMoniSeq.RootInTES = slot

            # Basic monitors
            simMoniSeq.Members += [
                GiGaGetEventAlg("GiGaGet"+self.slotName(slot)+"Event"), 
                MCTruthMonitor( self.slotName(slot)+"MCTruthMonitor" ) ]
            
            # can switch off detectors, or rather switch them on (see options
            # of algorithm)
            checkHits = GiGaGetHitsAlg( "GiGaGetHitsAlg" + slot )

            # Possibly dangerous - set all strings to ''
            # due to silly default values
            self.resetCheckHits( checkHits )



            simMoniSeq.Members += [ checkHits ]

            # checkHits had default values:
            # |-TTHits                    = 'MC/TT/Hits'
            # |-OTHits                    = 'MC/OT/Hits'
            # |-ITHits                    = 'MC/IT/Hits'
            # |-PuVetoHits                = 'MC/PuVeto/Hits'
            # |-VeloHits                  = 'MC/Velo/Hits'
            # |-MuonHits                  = 'MC/Muon/Hits'
            # |-RichTracks                = ''
            # |-RichHits                  = 'MC/Rich/Hits'
            # |-CaloHits                  = ['MC/Spd/Hits', 'MC/Prs/Hits', 'MC/Ecal/Hits', 'MC/Hcal/Hits']
            # | (default: ['MC/Spd/Hits', 'MC/Prs/Hits', 'MC/Ecal/Hits', 'MC/Hcal/Hits'])


            # Should add here the switching off of properties of
            # GiGaGetHitsAlg when a given detector is not monitored
            #self.configureTTMoni( checkHits )

            # OverWrite some things if using VP
            #self.configureVPMoni( checkHits )

                  
            #if moniOpt == 'Debug':
            #    checkHits.OutputLevel = DEBUG

            detMoniSeq = GaudiSequencer( "DetectorsMonitor" + slot ) 
            simMoniSeq.Members += [ detMoniSeq ]

            packCheckSeq = None
            if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
                
                packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
                simMoniSeq.Members += [packCheckSeq]

                upMCV = UnpackMCVertex("UnpackMCVertex"+slot,
                                       OutputName = "MC/VerticesTest" )
                upMCP = UnpackMCParticle( "UnpackMCParticle"+slot,
                                          OutputName = "MC/ParticlesTest" )
                packCheckSeq.Members += [ upMCV, upMCP ]
                
                compMCV = CompareMCVertex( "CompareMCVertex"+slot,
                                           TestName = "MC/VerticesTest" )
                compMCP = CompareMCParticle( "CompareMCParticle"+slot,
                                             TestName = "MC/ParticlesTest" )
                packCheckSeq.Members += [ compMCV, compMCP ]
            #=================================================


            # Define detectors
            for det in self.getProp('DetectorMoni')['Detectors']:
                self.configureDetectorMoni(slot, packCheckSeq, detMoniSeq, checkHits, det, configuredRichMoni)
            

#"""
#     ><<                             ><<    ><<<<<                  ><<              ><<<<<<<                      ><<     
#  ><<   ><<                        ><       ><<   ><<               ><<              ><<    ><<                    ><<     
# ><<           ><<     ><< ><<   ><>< ><    ><<    ><<    ><<     ><>< ><    ><<     ><<    ><<    ><<        ><<< ><<  ><<
# ><<         ><<  ><<   ><<  ><<   ><<      ><<    ><<  ><<  ><<    ><<    ><<  ><<  ><<<<<<<    ><<  ><<   ><<    ><< ><< 
# ><<        ><<    ><<  ><<  ><<   ><<      ><<    ><< ><<   ><<    ><<   ><<   ><<  ><<        ><<   ><<  ><<     ><><<   
#  ><<   ><<  ><<  ><<   ><<  ><<   ><<      ><<   ><<  ><<   ><<    ><<   ><<   ><<  ><<        ><<   ><<   ><<    ><< ><< 
#    ><<<<      ><<     ><<<  ><<   ><<      ><<<<<       ><< ><<<    ><<    ><< ><<< ><<          ><< ><<<    ><<< ><<  ><<
#
#"""
#    def configureDataPackingMoni( self, SpillOverSlots, simMoniSeq ):
#        # Data packing checks
#        for slot in SpillOverSlots:
#            if self.getProp("EnablePack") and self.getProp("DataPackingChecks") :
#                
#                packCheckSeq = GaudiSequencer( "DataUnpackTest"+slot )
#                simMoniSeq.Members += [packCheckSeq]
#
#                upMCV = UnpackMCVertex("UnpackMCVertex"+slot,
#                                       OutputName = "MC/VerticesTest" )
#                upMCP = UnpackMCParticle( "UnpackMCParticle"+slot,
#                                          OutputName = "MC/ParticlesTest" )
#                packCheckSeq.Members += [ upMCV, upMCP ]
#                
#                compMCV = CompareMCVertex( "CompareMCVertex"+slot,
#                                           TestName = "MC/VerticesTest" )
#                compMCP = CompareMCParticle( "CompareMCParticle"+slot,
#                                             TestName = "MC/ParticlesTest" )
#                packCheckSeq.Members += [ compMCV, compMCP ]
#            #=================================================




#"""
#    ><<                             ><<    ><<       ><<                         
# ><<   ><<                        ><       >< ><<   ><<<                       ><
#><<           ><<     ><< ><<   ><>< ><    ><< ><< > ><<    ><<     ><< ><<      
#><<         ><<  ><<   ><<  ><<   ><<      ><<  ><<  ><<  ><<  ><<   ><<  ><< ><<
#><<        ><<    ><<  ><<  ><<   ><<      ><<   ><  ><< ><<    ><<  ><<  ><< ><<
# ><<   ><<  ><<  ><<   ><<  ><<   ><<      ><<       ><<  ><<  ><<   ><<  ><< ><<
#   ><<<<      ><<     ><<<  ><<   ><<      ><<       ><<    ><<     ><<<  ><< ><<
#                                                                                 
#"""




    def configureMoni( self, SpillOverSlots ):
        self.configureGeneratorMoni( SpillOverSlots )
        # Per-detector configuration done here:
        self.configureSimulationMoni( SpillOverSlots )
        #self.configureDataPackingMoni( SpillOverSlots , simMoniSeq )
                
        #if histOpt == 'Expert':
        #    # For the moment do nothing
        #    log.Warning("Not yet implemented")

        # END OF MONI CONFIG







#"""
#     ><<                             ><<       ><<<<                            
#  ><<   ><<                        ><        ><    ><<   ><                     
# ><<           ><<     ><< ><<   ><>< ><    ><<                ><<       ><<    
# ><<         ><<  ><<   ><<  ><<   ><<      ><<         ><<  ><<  ><<  ><<  ><< 
# ><<        ><<    ><<  ><<  ><<   ><<      ><<   ><<<< ><< ><<   ><< ><<   ><< 
#  ><<   ><<  ><<  ><<   ><<  ><<   ><<       ><<    ><  ><<  ><<  ><< ><<   ><< 
#    ><<<<      ><<     ><<<  ><<   ><<        ><<<<<    ><<      ><<    ><< ><<<
#                                                              ><<               
#"""
    ##
    ##
    def configureGiGa(self , skipG4 = False ):
         """
         Set up the configuration for the G4 settings: physics list, cuts and actions
         """
         richPmt = False
         giga = GiGa()

         # PSZ - Use self.getProp('DataType') in future
         if [det for det in ['Rich1Pmt', 'Rich2Pmt'] if det in self.getProp('DetectorSim')['Detectors']]:
             richPmt = True
         ## setup the Physics list and the productions cuts
         if skipG4:
             richPmt = False
         self.setPhysList(richPmt)
         
         ## Mandatory G4 Run action
         giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
         giga.RunAction = "GiGaRunActionSequence/RunSeq"
         if not skipG4:
             giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
             giga.RunSeq.Members += [ "TrCutsRunAction/TrCuts" ] 
             giga.RunSeq.addTool( GiGaRunActionCommand("RunCommand") , name = "RunCommand" ) 
             giga.RunSeq.Members += [ "GiGaRunActionCommand/RunCommand" ]
             giga.RunSeq.RunCommand.BeginOfRunCommands = [
                 "/tracking/verbose 0",
                 "/tracking/storeTrajectory  1",
                 "/process/eLoss/verbose -1" ]

         giga.EventAction = "GiGaEventActionSequence/EventSeq"
         giga.addTool( GiGaEventActionSequence("EventSeq") , name="EventSeq" ) 
         giga.EventSeq.Members += [ "GaussEventActionHepMC/HepMCEvent" ]

         giga.TrackingAction =   "GiGaTrackActionSequence/TrackSeq" 
         giga.addTool( GiGaTrackActionSequence("TrackSeq") , name = "TrackSeq" )
         giga.TrackSeq.Members += [ "GaussPreTrackAction/PreTrack" ]

         giga.SteppingAction =   "GiGaStepActionSequence/StepSeq"
         giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )

         if richPmt:
             from Configurables import (
                 GiGaPhysConstructorOpCkv,
                 GiGaPhysConstructorPhotoDetector
                 )
             # Line to remove AEROGEL warnings
             SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"
             importOptions("$GAUSSCHERENKOVROOT/options/GaussCherenkov.opts")
             # richPmt False unless there's > 1 upgrade Rich
             giga.ModularPL.addTool( GiGaPhysConstructorOpCkv,
                                     name="GiGaPhysConstructorOpCkv" )
             giga.ModularPL.GiGaPhysConstructorOpCkv.RichActivateRichPhysicsProcVerboseTag = True
             giga.StepSeq.Members += [ "RichG4StepAnalysis4/RichStepAgelExit" ]
             giga.StepSeq.Members += [ "RichG4StepAnalysis5/RichStepMirrorRefl" ]

         else:
             from Configurables import (
                 GiGaPhysConstructorOp,
                 GiGaPhysConstructorHpd
                 )

             if self.getProp("DataType") == "2015" :
                 # Line to remove AEROGEL warnings
                 SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/SimulationRICHesOff.xml"
             else:
                 SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/Simulation.xml"

             if [det for det in ['Rich1', 'Rich2'] if det in self.getProp('DetectorSim')['Detectors']]:
                 importOptions("$GAUSSRICHROOT/options/Rich.opts")
                 if self.getProp("DataType") == "2015" :
                     importOptions("$GAUSSRICHROOT/options/RichRemoveAerogel.opts")
                 
             else:
                 if not skipG4:
                     giga.ModularPL.addTool( GiGaPhysConstructorOp,
                                             name = "GiGaPhysConstructorOp" )
                     giga.ModularPL.addTool( GiGaPhysConstructorHpd,
                                             name = "GiGaPhysConstructorHpd" )
                     giga.ModularPL.GiGaPhysConstructorOp.RichOpticalPhysicsProcessActivate = False
                     giga.ModularPL.GiGaPhysConstructorHpd.RichHpdPhysicsProcessActivate = False

             if [det for det in ['Rich1', 'Rich2'] if det in self.getProp('DetectorSim')['Detectors']]:
                 giga.ModularPL.addTool( GiGaPhysConstructorOp,
                                         name="GiGaPhysConstructorOp" )
                 giga.ModularPL.GiGaPhysConstructorOp.RichActivateRichPhysicsProcVerboseTag = True
                 giga.StepSeq.Members += [ "RichG4StepAnalysis4/RichStepAgelExit" ]
                 giga.StepSeq.Members += [ "RichG4StepAnalysis5/RichStepMirrorRefl" ]
                 if self.getProp("RICHRandomHits") == True :
                     giga.ModularPL.GiGaPhysConstructorOp.Rich2BackgrHitsActivate = True
                     giga.ModularPL.GiGaPhysConstructorOp.Rich2BackgrHitsProbabilityFactor = 0.5
                 


         # END OF richPmt IF STATEMENT

         giga.TrackSeq.Members += [ "GaussPostTrackAction/PostTrack" ]
         giga.TrackSeq.Members += [ "GaussTrackActionHepMC/HepMCTrack" ]
         giga.TrackSeq.addTool( GaussPostTrackAction("PostTrack") , name = "PostTrack" ) 

         giga.TrackSeq.PostTrack.StoreAll          = False
         giga.TrackSeq.PostTrack.StorePrimaries    = True
         giga.TrackSeq.PostTrack.StoreMarkedTracks = True
         giga.TrackSeq.PostTrack.StoreForcedDecays = True
         giga.TrackSeq.PostTrack.StoreByOwnEnergy   = True
         giga.TrackSeq.PostTrack.OwnEnergyThreshold = 100.0 * SystemOfUnits.MeV 

         giga.TrackSeq.PostTrack.StoreByChildProcess  = True
         giga.TrackSeq.PostTrack.StoredChildProcesses = [ "RichG4Cerenkov", "Decay" ]
         giga.TrackSeq.PostTrack.StoreByOwnProcess  = True 
         giga.TrackSeq.PostTrack.StoredOwnProcesses = [ "Decay" ]
         giga.StepSeq.Members += [ "GaussStepAction/GaussStep" ]


         giga.addTool( GiGaRunManager("GiGaMgr") , name="GiGaMgr" ) 
         giga.GiGaMgr.RunTools += [ "GiGaSetSimAttributes" ]
         giga.GiGaMgr.RunTools += [ "GiGaRegionsTool" ]
         giga.GiGaMgr.addTool( GiGaSetSimAttributes() , name = "GiGaSetSimAttributes" )
         giga.GiGaMgr.GiGaSetSimAttributes.OutputLevel = 4
         # Second part went here

         #return giga


#"""
#
#     ><<                             ><<       ><<<<                
#  ><<   ><<                        ><        ><    ><<        ><<   
# ><<           ><<     ><< ><<   ><>< ><    ><<             > ><<   
# ><<         ><<  ><<   ><<  ><<   ><<      ><<            >< ><<   
# ><<        ><<    ><<  ><<  ><<   ><<      ><<   ><<<<  ><<  ><<   
#  ><<   ><<  ><<  ><<   ><<  ><<   ><<       ><<    ><  ><<<< >< ><<
#    ><<<<      ><<     ><<<  ><<   ><<        ><<<<<          ><<   
#                                                                   
#
#"""

    ##
    ## Configure the sequence to transform HepMC into MCParticles
    ## skipping Geant4
    def configureSkipGeant4( self, SpillOverSlots ):

        """
        Set up the sequence to transform HepMC into MCParticles
        """

        if "GenToMCTree" not in self.getProp("Phases"):
            log.warning("No GenToMCTree phase.")
            return

        # Do not do detector simulation in this case
        self.getProp('DetectorSim')['Detectors'] = []
        self.getProp('DetectorGeo')['Detectors'] = []
        self.getProp('DetectorMoni')['Detectors'] = []
        
        ApplicationMgr().ExtSvc += [ "GiGa" ]

        gaussSkipGeant4Seq = GaudiSequencer( "SkipGeant4" )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussSkipGeant4Seq ]

        self.configureGiGa( True )

        for slot in SpillOverSlots:

            TESNode = "/Event/"+self.slot_(slot)
            
            mainSkipGeant4Sequence = GaudiSequencer( self.slotName(slot)+"EventSeq" )

            gaussSkipGeant4Seq.Members += [ mainSkipGeant4Sequence ]

            mainSkipGeant4Sequence.Members +=  [ SimInit( self.slotName(slot)+"EventGaussSkipGeant4",
                                                          GenHeader = TESNode + "Gen/Header" ,
                                                          MCHeader = TESNode + "MC/Header" ) ]

            skipGeant4Seq = GaudiSequencer( self.slotName(slot)+"SkipGeant4",
                                            RequireObjects = [ TESNode + "Gen/HepMCEvents" ] )
            mainSkipGeant4Sequence.Members += [ skipGeant4Seq ]

            skipGeant4SlotSeq = GaudiSequencer( "Make"+self.slotName(slot)+"SkipGeant4" )
            skipGeant4Seq.Members += [skipGeant4SlotSeq]

            # CRJ : Set RootInTES - Everything down stream will then use the correct location
            #       (assuming they use GaudiAlg get and put) so no need to set data locations
            #       by hand any more ...
            if slot != '' : skipGeant4SlotSeq.RootInTES = slot

            genToSim = GenerationToSimulation( "GenToSim" + slot,
                                               SkipGeant = True )
            skipGeant4SlotSeq.Members += [ genToSim ]
            
            # Data packing ...
            if self.getProp("EnablePack") :
                packing = GaudiSequencer(self.slotName(slot)+"EventDataPacking")
                skipGeant4SlotSeq.Members += [ packing ]
                SimConf().PackingSequencers[slot] = packing


#"""
#   ><< <<                ><<      ><<<<<<<                             
# ><<    ><<              ><<      ><<    ><< ><<                       
#  ><<          ><<     ><>< ><    ><<    ><< ><<      ><<   ><<  ><<<< 
#    ><<      ><   ><<    ><<      ><<<<<<<   >< ><     ><< ><<  ><<    
#       ><<  ><<<<< ><<   ><<      ><<        ><<  ><<    ><<<     ><<< 
# ><<    ><< ><           ><<      ><<        ><   ><<     ><<       ><<
#   ><< <<     ><<<<       ><<     ><<        ><<  ><<    ><<    ><< ><<
#                                                       ><<             
#"""

    def setPhysList( self, richPmt ):

        giga = GiGa()
        giga.addTool( GiGaPhysListModular("ModularPL") , name="ModularPL" ) 
        giga.PhysicsList = "GiGaPhysListModular/ModularPL"
        gmpl = giga.ModularPL

        ## set production cuts 
        ecut = 5.0 * SystemOfUnits.mm
        if not self.getProp("DeltaRays"):
            ecut = 10000.0 * SystemOfUnits.m
        print 'Ecut value =', ecut
        gmpl.CutForElectron = ecut
        gmpl.CutForPositron = 5.0 * SystemOfUnits.mm 
        gmpl.CutForGamma    = 5.0 * SystemOfUnits.mm

        ## set up the physics list
        hadronPhys = self.getProp('PhysicsList')['Hadron']
        emPhys     = self.getProp('PhysicsList')['Em']
        lhcbPhys   = self.getProp('PhysicsList')['LHCbPhys']
        genPhys    = self.getProp('PhysicsList')['GeneralPhys']
        otherPhys  = ''
        if self.getProp('PhysicsList').has_key('Other'):
            otherPhys = self.getProp('PhysicsList')['Other']
        
        def gef(name):
            import Configurables
            return getattr(Configurables, "GiGaExtPhysics_%s_" % name)
        def addConstructor(template, name):
            gmpl.addTool(gef(template), name = name)
            gmpl.PhysicsConstructors.append(getattr(gmpl, name))
        
        ## --- EM physics: 
        if  (emPhys == "Opt1"):
            addConstructor("G4EmStandardPhysics_option1", "EmOpt1Physics")
        elif(emPhys == "Opt2"):
            addConstructor("G4EmStandardPhysics_option2", "EmOpt2Physics")
        elif(emPhys == "Opt3"):
            addConstructor("G4EmStandardPhysics_option3", "EmOpt3Physics")
        elif(emPhys == "Std"):
            addConstructor("G4EmStandardPhysics", "EmPhysics")
        elif(emPhys == "NoCuts"):
            addConstructor("G4EmStandardPhysics_option1NoApplyCuts", "EmOpt1NoCutsPhysics")
        elif(emPhys.find("LHCb") != -1):
            if(emPhys.find("Test") != -1 ):
                addConstructor("G4EmStandardPhysics_LHCbTest", "EmOpt1LHCbPhysics")
            else:
                addConstructor("G4EmStandardPhysics_option1LHCb", "EmOpt1LHCbPhysics")
            # overwrite cuts depending on choice of list
            if(emPhys.find("NoCuts") != -1 ):
                gmpl.EmOpt1LHCbPhysics.ApplyCuts = False
            if(emPhys.find("OldForE") != -1 ):
                gmpl.EmOpt1LHCbPhysics.NewModelForE = False
            
            #gmpl.EmOpt1LHCbPhysics.OutputLevel = VERBOSE
        else:
            raise RuntimeError("Unknown Em PhysicsList chosen ('%s')"%emPhys)
            
        ## --- general  physics (common to all PL): 
        if (genPhys == True):
        ## Decays
            addConstructor("G4DecayPhysics", "DecayPhysics" )
        ## EM physics: Synchroton Radiation & gamma,electron-nuclear Physics
            addConstructor("G4EmExtraPhysics", "EmExtraPhysics")
        ## Hadron physics: Hadron elastic scattering
        ##    addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
        ## now specialised for different constructors
        ## Ions physics
            addConstructor("G4IonPhysics", "IonPhysics")
        elif (genPhys == False):
            log.warning("The general physics (Decays, hadron elastic, ion ...) is disabled")
        else:        
            raise RuntimeError("Unknown setting for GeneralPhys PhysicsList chosen ('%s')"%genPhys)

        ## --- Hadron physics:
        if  (hadronPhys == "LHEP"):
            addConstructor("G4HadronElasticPhysicsLHEP", "ElasticPhysicsLHEP")
            addConstructor("HadronPhysicsLHEP", "LHEPPhysics")
        elif(hadronPhys == "QGSP"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsQGSP", "QGSPPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")
        elif(hadronPhys == "QGSP_BERT"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsQGSP_BERT", "QGSP_BERTPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")
        elif(hadronPhys == "QGSP_BERT_HP"):
            addConstructor("G4HadronElasticPhysicsHP", "ElasticPhysicsHP")
            addConstructor("HadronPhysicsQGSP_BERT_HP", "QGSP_BERT_HPPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            # overwrite the defaut value of the HighPrecision property of the 
            # G4HadronElasticPhysics constructor: no longer true, use dedicated
            # constructor
            #gmpl.ElasticPhysics.HighPrecision = True
            #gmpl.ElasticPhysics.OutputLevel = VERBOSE
        elif(hadronPhys == "QGSP_BERT_CHIPS"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsQGSP_BERT_CHIPS", "QGSP_BERT_CHIPSPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")            
        elif(hadronPhys == "QGSP_FTFP_BERT"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsQGSP_FTFP_BERT", "QGSP_FTFP_BERTPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")            
        elif(hadronPhys == "FTFP_BERT"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsFTFP_BERT", "FTFP_BERTPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")
        else:
            raise RuntimeError("Unknown Hadron PhysicsList chosen ('%s')"%hadronPhys)
        
        
        ## --- LHCb specific physics: 
        if  (lhcbPhys == True):
            if (richPmt == True):
                self.defineRichMaPmtPhys(gmpl)
            else:
                self.defineRichPhys(gmpl)

        ## LHCb particles unknown to default Geant4                
            gmpl.PhysicsConstructors.append("GiGaPhysUnknownParticles")
        elif (lhcbPhys == False):
            log.warning("The lhcb-related physics (RICH processed, UnknownParticles) is disabled")
        else:        
            raise RuntimeError("Unknown setting for LHCbPhys PhysicsList chosen ('%s')"%lhcbPhys)

        ## and other exotic physics
        if (otherPhys == 'Higgs'):
            log.info("Enabling physics processe for Higgs particles")
            gmpl.PhysicsConstructors.append("GiGaHiggsParticles")
        else:
            if (otherPhys != '' ):
               raise RuntimeError("Unknown setting for OtherPhys PhysicsList chosen ('%s')"%otherPhys) 
       


#"""
#      ><                           ><<                 ><<                             ><<
#     >< <<                         ><<              ><<   ><<                        ><   
#    ><  ><<     >< ><<   >< ><<    ><< ><<   ><<   ><<           ><<     ><< ><<   ><>< ><
#   ><<   ><<    ><  ><<  ><  ><<   ><<  ><< ><<    ><<         ><<  ><<   ><<  ><<   ><<  
#  ><<<<<< ><<   ><   ><< ><   ><<  ><<    ><<<     ><<        ><<    ><<  ><<  ><<   ><<  
# ><<       ><<  ><< ><<  ><< ><<   ><<     ><<      ><<   ><<  ><<  ><<   ><<  ><<   ><<  
#><<         ><< ><<      ><<      ><<<    ><<         ><<<<      ><<     ><<<  ><<   ><<  
#                ><<      ><<            ><<                                               
#"""
             
    ##
    ##
    ## Apply the configuration
    def __apply_configuration__(self):

        GaudiKernel.ProcessJobOptions.PrintOff()

        #defineDB() in Boole and
        self.configureRndmEngine()
        self.configureInput()  #defineEvents() in both Boole and Brunel
        LHCbApp( Simulation = True ) # in Boole? where?

        # raise an error if DetectorGeo/Sim/Moni dictionaries are incompatible
        self.checkGeoSimMoniDictionary()

        self.checkIncompatibleDetectors()

        self.setLHCbAppDetectors()
        
        #propagate info to SimConf
        self.propagateSimConf()
        
        #Construct Crossing List
        crossingList = self.defineCrossingList()

        # We want to pass this GenInit object to configure phases later
        from Configurables import ( Generation )
        genInitPrime = GenInit( "GaussGen" )
        
        self.setBeamParameters( crossingList, genInitPrime )
        # PSZ - everything happens here
        self.configurePhases( crossingList  )  # in Boole, defineOptions() in Brunel

        #--Configuration of output files and 'default' outputs files that can/should
        #--be overwritten in Gauss-Job.py
        self.defineOutput( crossingList )

        self.defineMonitors()
        self.saveHistos()
        
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()

        # Print out TES contents at the end of each event
        #from Configurables import StoreExplorerAlg
        #GaudiSequencer("GaussSequencer").Members += [ StoreExplorerAlg() ]
    
