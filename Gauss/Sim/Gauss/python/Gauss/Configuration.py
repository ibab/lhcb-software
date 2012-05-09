"""
High level configuration tools for Gauss
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
                            SignalPlain, SignalRepeatedHadronization,
                            SignalForcedFragmentation, StandAloneDecayTool,
                            Special,
                            PythiaProduction, HijingProduction,
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
                            GetMCRichSegmentsAlg, GetMCRichTracksAlg,
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

    ## Possible used Configurables
    __used_configurables__ = [ LHCbApp, SimConf ]

    ## Steering options
    __slots__ = {
        "Histograms"        : "DEFAULT"
       ,"DatasetName"       : "Gauss"
       ,"DataType"          : ""
       ,"DetectorGeo"       : {"VELO":['Velo','PuVeto'], "TT":['TT'], "IT":['IT'], "OT":['OT'], "RICH":['Rich1','Rich2'], "CALO":['Spd','Prs','Ecal','Hcal'], "MUON":['Muon'],"MAGNET": True }
       ,"DetectorSim"       : {"VELO":['Velo','PuVeto'], "TT":['TT'], "IT":['IT'], "OT":['OT'], "RICH":['Rich1','Rich2'], "CALO":['Spd','Prs','Ecal','Hcal'], "MUON":['Muon'],"MAGNET": True }
       ,"DetectorMoni"      : {"VELO":['Velo','PuVeto'], "TT":['TT'], "IT":['IT'], "OT":['OT'], "RICH":['Rich1','Rich2'], "CALO":['Spd','Prs','Ecal','Hcal'], "MUON":['Muon'],"MAGNET": True }
       ,"SpilloverPaths"    : []
       ,"PhysicsList"       : {"Em":'NoCuts', "Hadron":'LHEP', "GeneralPhys":True, "LHCbPhys":True, "Other": '' }
       ,"DeltaRays"         : True
       ,"Phases"            : ["Generator","Simulation"] # The Gauss phases to include in the SIM file
       ,"BeamMomentum"      : 3.5*SystemOfUnits.TeV
       ,"BeamCrossingAngle" : -0.520*SystemOfUnits.mrad
       ,"BeamEmittance"     : 0.0022*SystemOfUnits.mm
       ,"BeamBetaStar"      : 3.0*SystemOfUnits.m
       ,"BeamLineAngles"    : [ -0.075*SystemOfUnits.mrad, 0.035*SystemOfUnits.mrad ]
       ,"InteractionPosition" : [ 0.0*SystemOfUnits.mm, 0.0*SystemOfUnits.mm,
                                  0.0*SystemOfUnits.mm ]
       ,"InteractionSize"   : [ 0.030*SystemOfUnits.mm, 0.030*SystemOfUnits.mm,
                                5.7*SystemOfUnits.cm ]
       ,"Luminosity"        : 0.247*(10**30)/(SystemOfUnits.cm2*SystemOfUnits.s)
       ,"TotalCrossSection" : 91.1*SystemOfUnits.millibarn
       ,"Output"            : 'SIM'
       ,"Production"        : 'PHYS'
       ,"EnablePack"        : True
       ,"DataPackingChecks" : True
       ,"WriteFSR"          : True
       , "Persistency"      : None
      }
    
    _propertyDocDct = { 
        'Histograms'     : """ Type of histograms: ['NONE','DEFAULT'] """
       ,'DatasetName'    : """ String used to build output file names """
       ,"DataType"       : """ Must specify 'Upgrade' for upgrade simulations, otherwise not used """
       ,"DetectorGeo"    : """ Dictionary specifying the detectors to take into account in Geometry """
       ,"DetectorSim"    : """ Dictionary specifying the detectors to simulated (should be in geometry): """
       ,"DetectorMoni"   : """ Dictionary specifying the detectors to monitor (should be simulated) :"""
       ,'SpilloverPaths' : """ Spillover paths to fill: [] means no spillover, otherwise put ['Next', 'Prev', 'PrevPrev'] """
       ,'PhysicsList'    : """ Name of physics modules to be passed 'Em':['Std','Opt1,'Opt2','Opt3','NoCuts','LHCb', 'LHCbNoCuts', 'LHCbOldForE', 'LHCbNoCutsOldForE', 'LHCbTest', 'LHCbTestNoCut' ], 'GeneralPhys':[True,False], 'Hadron':['LHEP','QGSP','QGSP_BERT','QGSP_BERT_HP','QGSP_BERT_CHIPS','FTFP_BERT'], 'LHCbPhys': [True,False], 'Other': [''] """
       ,"DeltaRays"      : """ Simulation of delta rays enabled (default True) """
       ,'Phases'         : """ List of phases to run (Generator, Simulation, GenToMCTree) """
       ,'Output'         : """ Output: [ 'NONE', 'SIM'] (default 'SIM') """
       ,'Production'     : """ Generation type : ['PHYS', 'PGUN', 'MIB' (default 'PHYS')"""
       ,'EnablePack'     : """ Flag to turn on or off the packing of the SIM data """
       ,'DataPackingChecks' : """ Flag to turn on or off the running of some test algorithms to check the quality of the data packing """
       ,"WriteFSR"       : "Add file summary record, default True"
       , "Persistency"   : "ROOT or POOL persistency, overwrite the default"
       }
    KnownHistOptions = ['NONE','DEFAULT']
    TrackingSystem   = ['VELO','TT','IT','OT']
    PIDSystem        = ['RICH','CALO','MUON']

        
    ## Raise an error if DetectorGeo/DetectorSim/DetectorMoni are not compatible
    def checkGeoSimMoniDictionnary(self) :
        for subdet in self.TrackingSystem + self.PIDSystem:
            for det in self.getProp('DetectorSim')[subdet]:
                if self.getProp('DetectorGeo')[subdet].count(det) == 0 :
                    raise RuntimeError("Simulation have been required for '%s' sub-detector but it have been removed of Geometry",det)
            for det in self.getProp('DetectorMoni')[subdet]:
                if self.getProp('DetectorSim')[subdet].count(det) == 0 :
                    raise RuntimeError("Monitoring have been required for '%s' sub-detector but it have been removed of Simulations",det)
    
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
        evtType = ''
        if Generation("Generation").isPropertySet("EventType"):
            evtType = str( Generation("Generation").EventType )
        return evtType

    ##
    ## Functions to configuration various services that are used
    ##
    def configureRndmEngine( self ):

        # Random number service
        from Configurables import HepRndm__Engine_CLHEP__RanluxEngine_    
        rndmSvc = RndmGenSvc()
        engine = HepRndm__Engine_CLHEP__RanluxEngine_("RndmGenSvc.Engine")
        engine.SetSingleton = True


    def configureInput(self):

        # No events are read as input (this is not true if gen phase is
        # switched off
        ApplicationMgr().EvtSel = 'NONE'
        # Transient store setup
        EventDataSvc().ForceLeaves = True
        # May be needed by some options
        importOptions("$STDOPTS/PreloadUnits.opts")

        # TEMPORARILY until migration to new particle propertysvc set the
        # options to read table from database. Before it was done by DDDB
        from Configurables import Gaudi__ParticlePropertySvc
        Gaudi__ParticlePropertySvc().ParticlePropertiesFile = 'conddb:///param/ParticleTable.txt'

        
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


    ##
    def definePersistency(self):
        """
        ROOT or POOL propagated to LHCbApp
        """
        
        persistency=None
        
        if hasattr( self, "Persistency" ):
            persistency=self.getProp("Persistency")

        if persistency is not None:
            LHCbApp().setProp("Persistency",persistency)
        
    def defineOutput( self, SpillOverSlots ):
        """
        Set up output stream according to phase processed and spill-over slots
        """
        # and in the future extended or reduced DIGI
        
        # not required since it is now in LHCb App
        # POOL persistency 
        #importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")
        
        #
        knownOptions = ['NONE','SIM']
        output = self.getProp("Output").upper()
        if output == 'NONE':
            log.warning("No event data output produced")
            return
        
        simWriter = SimConf().writer()
        fileExtension = ".gen"
        if "GenToMCTree" in self.getProp("Phases"):
            fileExtension = ".xgen"
        elif "Simulation" in self.getProp("Phases"):
            fileExtension = ".sim"
        
        outputFile=""
        from GaudiConf import IOHelper
        if simWriter.isPropertySet( "Output" ):
            outputFile=IOHelper().undressFile(simWriter.getProp("Output"))
        else:
            outputFile=self.outputName() + fileExtension
        
        persistency=None
        
        if hasattr( self, "Persistency" ):
            persistency=self.getProp("Persistency")
        IOHelper(persistency,persistency).outStream( outputFile, simWriter, self.getProp("WriteFSR") )
        
        simWriter.RequireAlgs.append( 'GaussSequencer' )
        if not FileCatalog().isPropertySet("Catalogs"):
            FileCatalog().Catalogs = [ "xmlcatalog_file:NewCatalog.xml" ]
        
    
    def defineMonitors( self ):

        # get all defined monitors
        # monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Gauss specific monitors, so pass them all to LHCbApp
        # LHCbApp().setProp("Monitors", monitors)

        # Use TimingAuditor for timing
        # suppress printout from SequencerTimerTool ??
        from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool
        ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        #SequencerTimerTool().OutputLevel = WARNING

        # Set printout level and longer algorithm" identifier in printout
        MessageSvc().OutputLevel = INFO
        #ToolSvc.EvtGenTool.OutputLevel = 4 is it still necessart to reduce print?
        MessageSvc().setWarning.append( 'XmlGenericCnv' )
        if not MessageSvc().isPropertySet("Format"):
            MessageSvc().Format = '% F%24W%S%7W%R%T %0W%M'


    ##
    def saveHistos(self):
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
            

    ##
    ##
    def configureGen( self, SpillOverSlots ):
        """
        Set up the generator execution sequence and its sub-phases
        """
        
##         if "Gen" not in self.getProp("MainSequence") :
##             log.warning("No generator phase. Need input file")
##             return

        if self.evtMax() <= 0:
            raise RuntimeError("Generating events but selected '%s' events. Use LHCbApp().EvtMax "%self.evtMax())

        gaussGeneratorSeq = GaudiSequencer( "Generator", IgnoreFilterPassed = True )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussGeneratorSeq ]

        EvtGenDecay().DecayFile = "$DECFILESROOT/dkfiles/DECAY.DEC"

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


    ##
    ##
    def configureMoni( self, SpillOverSlots ):

        # Monitors for the generator:
        for slot in SpillOverSlots:

            genSequence = GaudiSequencer("GeneratorSlot" + self.slotName(slot) + "Seq" )
            genMoniSeq = GaudiSequencer("GenMonitor" + slot )
            genSequence.Members += [ genMoniSeq ]

            TESLocation = "/Event/"+self.slot_(slot)+"Gen/HepMCEvents"
            genMoniSeq.Members += [
                GenMonitorAlg( "GenMonitorAlg"+slot, HistoProduce=True,
                               Input = TESLocation ) ]
#            if moniOpt == 'Debug':
#                genMoniSeq.Members += [ DumpHepMC( "DumpHepMC"+slot,
#                                                   OutputLevel=1,
#                                                   Addresses = [TESLocation] ) ]

        # Monitors for simulation
        for slot in SpillOverSlots:

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
            simMoniSeq.Members += [ checkHits ]

            ## in case of a non default detector, need to be overwritten
            if self.getProp('DetectorSim')['VELO'].count('VeloPix')>0:    
                checkHits.VeloHits =  'MC/VeloPix/Hits'
                checkHits.PuVetoHits = ''             
                  
            #if moniOpt == 'Debug':
            #    checkHits.OutputLevel = DEBUG

            detMoniSeq = GaudiSequencer( "DetectorsMonitor" + slot ) 
            simMoniSeq.Members += [ detMoniSeq ]

            # velo
            
            ## Set the VeloMonitor
            if len(self.getProp('DetectorMoni')['VELO'])> 0:
                if self.getProp('DetectorMoni')['VELO'].count('VeloPix') > 0 :
                    from Configurables import   VeloPixGaussMoni
                    detMoniSeq.Members += [ VeloPixGaussMoni( "VeloPixGaussMoni" + slot ) ]
                else :
                    detMoniSeq.Members += [ VeloGaussMoni( "VeloGaussMoni" + slot ) ]
                    


            ## Hit monitoring for the other tracking subsystem
            TrackingSystemZStation = {'TT':[2350.*SystemOfUnits.mm, 2620.*SystemOfUnits.mm],
                                      'IT':[ 7780.0*SystemOfUnits.mm,8460.0*SystemOfUnits.mm,9115.0*SystemOfUnits.mm ],
                                      'OT':[ 7938.0*SystemOfUnits.mm, 8625.0*SystemOfUnits.mm,9315.0*SystemOfUnits.mm ] }

            TrackingSystemZStationXYMax = {'TT':[150.*SystemOfUnits.cm,150.*SystemOfUnits.cm],
                                           'IT':[150.*SystemOfUnits.cm ,150.*SystemOfUnits.cm ],
                                           'OT':[100.*SystemOfUnits.cm,100.*SystemOfUnits.cm]}

            if self.getProp("DataType") == "Upgrade" :
                TrackingSystemZStation = {'TT':[2350.*SystemOfUnits.mm, 2620.*SystemOfUnits.mm],
                                          'IT':[8015.0*SystemOfUnits.mm,8697.0*SystemOfUnits.mm,9363.0*SystemOfUnits.mm ],
                                          'OT':[7672.0*SystemOfUnits.mm,8354.0*SystemOfUnits.mm,9039.0*SystemOfUnits.mm ]}

            for sub in self.TrackingSystem:
                if sub == 'VELO' : continue
                for det in self.getProp('DetectorMoni')[sub]:
                    detMoniSeq.Members += [ MCHitMonitor( det+ "HitMonitor" + slot ,
                                                          mcPathString = "MC/" + det +"/Hits",
                                                          zStations = TrackingSystemZStation[sub],
                                                          xMax = TrackingSystemZStationXYMax[sub][0],
                                                          yMax = TrackingSystemZStationXYMax[sub][1])]

            ## Hit monitoring for the PID system
            CaloThreshold = {'Spd' :1.5*SystemOfUnits.MeV,
                             'Prs' :1.5*SystemOfUnits.MeV,
                             'Ecal':10.*SystemOfUnits.MeV,
                             'Hcal':5.*SystemOfUnits.MeV}
            CaloMaxEnergy = {'Spd' :10.*SystemOfUnits.MeV,
                             'Prs' :10.*SystemOfUnits.MeV,
                             'Ecal':1000.*SystemOfUnits.MeV,
                             'Hcal':1000.*SystemOfUnits.MeV }

            for sub in self.PIDSystem:
                if sub == 'CALO' :
                    for det in self.getProp('DetectorMoni')[sub]:
                        detMoniSeq.Members += [ MCCaloMonitor( det + "Monitor" + slot,
                                                               OutputLevel = 4,
                                                               Detector = det,
                                                               Regions = True,
                                                               MaximumEnergy = CaloMaxEnergy[det],
                                                               Threshold = CaloThreshold[det] ) ]
                if sub == 'MUON' :
                    for det in self.getProp('DetectorMoni')[sub]:
                        detMoniSeq.Members += [ MuonHitChecker( det + "HitChecker" + slot,
                                                                FullDetail = True )]
                        from Configurables import MuonMultipleScatteringChecker
                        detMoniSeq.Members += [
                            MuonMultipleScatteringChecker( "MuonMultipleScatteringChecker"+ slot )]

            # Data packing checks
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

                from Configurables import DataPacking__Unpack_LHCb__MCVeloHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCPuVetoHitPacker_
                
                from Configurables import DataPacking__Unpack_LHCb__MCVeloPixHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCTTHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCITHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCOTHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCMuonHitPacker_
                
                if self.getProp('DetectorSim')['VELO'].count('VeloPix')>0:
                    upVeloPix = DataPacking__Unpack_LHCb__MCVeloPixHitPacker_("UnpackVeloPixHits"+slot,
                                                                        OutputName = "MC/VeloPix/HitsTest" )
                    packCheckSeq.Members += [upVeloPix]
                else :
                    upVelo = DataPacking__Unpack_LHCb__MCVeloHitPacker_("UnpackVeloHits"+slot,
                                                                        OutputName = "MC/Velo/HitsTest" )
                    upPuVe = DataPacking__Unpack_LHCb__MCPuVetoHitPacker_("UnpackPuVetoHits"+slot,
                                                                          OutputName = "MC/PuVeto/HitsTest" )
                    packCheckSeq.Members += [upVelo,upPuVe]
                upTT   = DataPacking__Unpack_LHCb__MCTTHitPacker_("UnpackTTHits"+slot,
                                                                  OutputName = "MC/TT/HitsTest" )
                upIT   = DataPacking__Unpack_LHCb__MCITHitPacker_("UnpackITHits"+slot,
                                                                  OutputName = "MC/IT/HitsTest" )
                upOT   = DataPacking__Unpack_LHCb__MCOTHitPacker_("UnpackOTHits"+slot,
                                                                  OutputName = "MC/OT/HitsTest" )
                upMu   = DataPacking__Unpack_LHCb__MCMuonHitPacker_("UnpackMuonHits"+slot,
                                                                    OutputName = "MC/Muon/HitsTest" )
                packCheckSeq.Members += [upTT,upIT,upOT,upMu]
                
                from Configurables import DataPacking__Check_LHCb__MCVeloHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCPuVetoHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCVeloPixHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCTTHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCITHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCOTHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCMuonHitPacker_

                                
                if self.getProp('DetectorSim')['VELO'].count('VeloPix')>0:
                    cVeloPix = DataPacking__Check_LHCb__MCVeloPixHitPacker_("CheckVeloPixHits"+slot)
                    packCheckSeq.Members += [cVeloPix]
                else :
                    cVelo = DataPacking__Check_LHCb__MCVeloHitPacker_("CheckVeloHits"+slot)
                    cPuVe = DataPacking__Check_LHCb__MCPuVetoHitPacker_("CheckPuVetoHits"+slot)
                    packCheckSeq.Members += [cVelo,cPuVe]
                cTT   = DataPacking__Check_LHCb__MCTTHitPacker_("CheckTTHits"+slot )
                cIT   = DataPacking__Check_LHCb__MCITHitPacker_("CheckITHits"+slot )
                cOT   = DataPacking__Check_LHCb__MCOTHitPacker_("CheckOTHits"+slot )
                cMu   = DataPacking__Check_LHCb__MCMuonHitPacker_("CheckMuonHits"+slot )
                packCheckSeq.Members += [cTT,cIT,cOT,cMu]

                from Configurables import DataPacking__Unpack_LHCb__MCPrsHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCSpdHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCEcalHitPacker_
                from Configurables import DataPacking__Unpack_LHCb__MCHcalHitPacker_
                upPrs  = DataPacking__Unpack_LHCb__MCPrsHitPacker_("UnpackPrsHits"+slot,
                                                                  OutputName = "MC/Prs/HitsTest" )
                upSpd  = DataPacking__Unpack_LHCb__MCSpdHitPacker_("UnpackSpdHits"+slot,
                                                                   OutputName = "MC/Spd/HitsTest" )
                upEcal = DataPacking__Unpack_LHCb__MCEcalHitPacker_("UnpackEcalHits"+slot,
                                                                    OutputName = "MC/Ecal/HitsTest" )
                upHcal = DataPacking__Unpack_LHCb__MCHcalHitPacker_("UnpackHcalHits"+slot,
                                                                    OutputName = "MC/Hcal/HitsTest" )
                packCheckSeq.Members += [upPrs,upSpd,upEcal,upHcal]

                from Configurables import DataPacking__Check_LHCb__MCPrsHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCSpdHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCEcalHitPacker_
                from Configurables import DataPacking__Check_LHCb__MCHcalHitPacker_
                cPrs  = DataPacking__Check_LHCb__MCPrsHitPacker_("CheckPrsHits"+slot)
                cSpd  = DataPacking__Check_LHCb__MCSpdHitPacker_("CheckSpdHits"+slot)
                cEcal = DataPacking__Check_LHCb__MCEcalHitPacker_("CheckEcalHits"+slot)
                cHcal = DataPacking__Check_LHCb__MCHcalHitPacker_("CheckHcalHits"+slot)
                packCheckSeq.Members += [cPrs,cSpd,cEcal,cHcal]

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
                
                             
        #if histOpt == 'Expert':
        #    # For the moment do nothing
        #    log.Warning("Not yet implemented")

        if len(self.getProp('DetectorSim')['RICH'])> 0 :
            importOptions("$GAUSSRICHROOT/options/RichAnalysis.opts")


    ##
    ##
    def defineGeo( self ):
        """
        Set up the geometry to be simulated
        """

        geo = GiGaInputStream( "Geo",
                               ExecuteOnce = True,
                               ConversionSvcName = "GiGaGeo",
                               DataProviderSvcName  = "DetectorDataSvc" )
        gaussSimulationSeq = GaudiSequencer("Simulation")
        gaussSimulationSeq.Members += [ geo ]

    ##     # To set size of World/Universe i.e. the main mother volume (made of Air
    ##     # and in which any LHCb detector is positioned)
    ##     # Now it is half that is inconsistent with "XmlDDDB"
    ##     # GiGaGeo.XsizeOfWorldVolume = 50.0*m;
    ##     # GiGaGeo.YsizeOfWorldVolume = 50.0*m;
    ##     # GiGaGeo.ZsizeOfWorldVolume = 50.0*m; 

        # Detector geometry to simulate
        DetPiecies = {'BeforeMagnetRegion':[],'AfterMagnetRegion':[],'DownstreamRegion':[],'MagnetRegion':['Magnet','BcmDown']}
        BasePiecies = {}
        # check if the new velo geometry is required with the chosen DDDB tags
        VeloPostMC09 = 0
        VeloP = self.checkVeloDDDB(VeloPostMC09)
        if (VeloP==1 or VeloP==2):
            BasePiecies['BeforeMagnetRegion']=[]
        else:
            BasePiecies['BeforeMagnetRegion']=['Velo2Rich1']
        BasePiecies['MagnetRegion']=['PipeInMagnet','PipeSupportsInMagnet']
        BasePiecies['AfterMagnetRegion']=['PipeAfterT','PipeSupportsAfterMagnet']
        BasePiecies['DownstreamRegion']=['PipeDownstream','PipeSupportsDownstream','PipeBakeoutDownstream']

        for sub in self.TrackingSystem:
            if sub == 'VELO' or sub == 'TT':
                region = 'BeforeMagnetRegion'
                for det in self.getProp('DetectorGeo')[sub]:
                    if det!= 'PuVeto': DetPiecies[region]+=[det]
            if sub == 'IT' or sub == 'OT':
                region = 'AfterMagnetRegion'
                if 'T' not in DetPiecies[region]: 
                    DetPiecies[region]+=['T']
                

        for sub in self.PIDSystem:
            if sub == 'RICH':
                for det in self.getProp('DetectorGeo')[sub]:
                    region = ''
                    if det == 'Rich1':  region = 'BeforeMagnetRegion'
                    else :  region = 'AfterMagnetRegion'
                    DetPiecies[region]+=[det]
            if sub == 'CALO' or sub == 'MUON':
                region = 'DownstreamRegion'
                for det in self.getProp('DetectorGeo')[sub]:
                    DetPiecies[region]+=[det]
                    if det == 'Spd': DetPiecies[region]+=['Converter']


        for region in BasePiecies.keys():
            path = "/dd/Structure/LHCb/"+region+"/"
            if region == 'MagnetRegion':
                for element in BasePiecies[region]:
                        geo.StreamItems += [ path + element ]
                if self.getProp('DetectorGeo')['MAGNET']==True:
                    for element in DetPiecies[region]:
                        geo.StreamItems += [ path + element ]
            else:
                if len(DetPiecies[region])==0 : continue
                for element in BasePiecies[region]:
                    geo.StreamItems += [ path + element ]
                for element in DetPiecies[region]:
                    geo.StreamItems += [ path + element ]

        if len(self.getProp('DetectorGeo')['RICH'])>0:
            geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces"]
            geo.StreamItems += ["/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces"]
            geo.StreamItems += ["/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces"]


        if len(self.getProp('DetectorGeo')['CALO'])>0:
            importOptions("$GAUSSCALOROOT/options/Calo.opts")

        # Use information from SIMCOND and GeometryInfo
        giGaGeo = GiGaGeo()
        giGaGeo.UseAlignment      = True
        giGaGeo.AlignAllDetectors = True
        if self.getProp("DataType") != "Upgrade" :
            if len(self.getProp('DetectorGeo')['VELO'])>0:
#                importOptions('$GAUSSOPTS/SimVeloGeometry.py')  # To misalign VELO
                 self.veloGeometry(VeloP) # To misalign VELO

    ##     #if "VELO" in geoDets: configureGeoVELO( )
    ##     #if "TT  " in geoDets: configureGeoTT( )
    ##     #if "T" or "IT  " or "OT" in geoDets: configureGeoT( geoDets )
    ##     #if "RICH1" in geoDets: configureGeoRICH( )
    ##     #if "CALO" or "SPD&PRS" or "ECAL" or "HCAL" in geoDets:configureGeoCALO( geoDets )
    ##     #if "MUON" in geoDets: etc.

        ## Set up the magnetic field
        if self.getProp('DetectorSim')['MAGNET']==True:
            GiGaGeo().FieldManager           = "GiGaFieldMgr/FieldMgr"
            GiGaGeo().addTool( GiGaFieldMgr("FieldMgr"), name="FieldMgr" )
            GiGaGeo().FieldMgr.Stepper       = "ClassicalRK4"
            GiGaGeo().FieldMgr.Global        = True
            GiGaGeo().FieldMgr.MagneticField = "GiGaMagFieldGlobal/LHCbField"
            GiGaGeo().FieldMgr.addTool( GiGaMagFieldGlobal("LHCbField"), name="LHCbField" ) 
            GiGaGeo().FieldMgr.LHCbField.MagneticFieldService = "MagneticFieldSvc"


    ##
    ##
    def configureGiGa(self):
         """
         Set up the configuration for the G4 settings: physics list, cuts and actions
         """
         ## setup the Physics list and the productions cuts
         self.setPhysList()
         
         ## Mandatory G4 Run action
         giga = GiGa()
         giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
         giga.RunAction = "GiGaRunActionSequence/RunSeq"
         giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
         giga.RunSeq.addTool( GiGaRunActionCommand("RunCommand") , name = "RunCommand" ) 
         giga.RunSeq.Members += [ "TrCutsRunAction/TrCuts" ] 
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

         if len(self.getProp('DetectorSim')['RICH'])>0:
             importOptions("$GAUSSRICHROOT/options/Rich.opts")
         if len(self.getProp('DetectorSim')['RICH']) == 0:
             giga.ModularPL.addTool( GiGaPhysConstructorOp, name = "GiGaPhysConstructorOp" )
             giga.ModularPL.addTool( GiGaPhysConstructorHpd, name = "GiGaPhysConstructorHpd" )
             giga.ModularPL.GiGaPhysConstructorOp.RichOpticalPhysicsProcessActivate = False
             giga.ModularPL.GiGaPhysConstructorHpd.RichHpdPhysicsProcessActivate = False
             
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

         SimulationSvc().SimulationDbLocation = "$GAUSSROOT/xml/Simulation.xml"
         giga.addTool( GiGaRunManager("GiGaMgr") , name="GiGaMgr" ) 
         giga.GiGaMgr.RunTools += [ "GiGaSetSimAttributes" ]
         giga.GiGaMgr.RunTools += [ "GiGaRegionsTool" ]
         giga.GiGaMgr.addTool( GiGaSetSimAttributes() , name = "GiGaSetSimAttributes" )
         giga.GiGaMgr.GiGaSetSimAttributes.OutputLevel = 4

         # switch off when rich reduced, should not go into monitor?
         if len(self.getProp('DetectorSim')['RICH'])>0:
             giga.ModularPL.addTool( GiGaPhysConstructorOp,
                                     name="GiGaPhysConstructorOp" )
             giga.ModularPL.GiGaPhysConstructorOp.RichActivateRichPhysicsProcVerboseTag = True
             giga.StepSeq.Members += [ "RichG4StepAnalysis4/RichStepAgelExit" ]
             giga.StepSeq.Members += [ "RichG4StepAnalysis5/RichStepMirrorRefl" ]


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
        
        ApplicationMgr().ExtSvc += [ "GiGa" ]

        gaussSkipGeant4Seq = GaudiSequencer( "SkipGeant4" )
        gaussSeq = GaudiSequencer("GaussSequencer")
        gaussSeq.Members += [ gaussSkipGeant4Seq ]

        self.configureGiGa()

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

        #geoDets = [ "VELO" ]
        #defineGeo( geoDets )
        self.defineGeo( )

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

            detRegion = {}
            for sub in self.TrackingSystem:
                if sub == 'VELO' or sub == 'TT':
                    detRegion[sub] = 'BeforeMagnetRegion'
                if sub == 'IT' or sub == 'OT':
                    detRegion[sub] = 'AfterMagnetRegion/T'

            for sub in self.TrackingSystem:
                for det in self.getProp('DetectorSim')[sub]:
                    detextra,detextra1 = det,det
                    if det == 'PuVeto' : detextra,detextra1 = 'VeloPu','Velo'
                    moni = GetTrackerHitsAlg( "Get"+det+"Hits"+slot,
                                              MCHitsLocation = 'MC/' + det + '/Hits',
                                              CollectionName = detextra + 'SDet/Hits',
                                              Detectors = ['/dd/Structure/LHCb/'+detRegion[sub]+'/'+detextra1] )
                    detHits.Members += [ moni ]

            if len(self.getProp('DetectorSim')['MUON'])>0:
                for det in self.getProp('DetectorSim')['MUON']:
                    moni = GetTrackerHitsAlg( "Get"+det+"Hits"+slot,
                                              MCHitsLocation = 'MC/' + det + '/Hits',
                                              CollectionName = det + 'SDet/Hits',
                                              Detectors = ['/dd/Structure/LHCb/DownstreamRegion/'+det] )
                    detHits.Members += [ moni ]

            if len(self.getProp('DetectorSim')['CALO'])>0:
                for det in self.getProp('DetectorSim')['CALO']:
                    moni = GetCaloHitsAlg( "Get"+det+"Hits"+slot,
                                           #MCParticles = TESNode + "Particles",
                                           MCHitsLocation = 'MC/' + det + '/Hits',
                                           CollectionName = det + 'Hits' )
                    detHits.Members += [ moni ]

            if len(self.getProp('DetectorSim')['RICH'])>0:
                richHitsSeq = GaudiSequencer( "RichHits" + slot )
                detHits.Members += [ richHitsSeq ]
                richHitsSeq.Members = [ GetMCRichHitsAlg( "GetRichHits"+slot),
                                        GetMCRichOpticalPhotonsAlg("GetRichPhotons"+slot),
                                        GetMCRichSegmentsAlg("GetRichSegments"+slot), 
                                        GetMCRichTracksAlg("GetRichTracks"+slot), 
                                        Rich__MC__MCPartToMCRichTrackAlg("MCPartToMCRichTrack"+slot), 
                                        Rich__MC__MCRichHitToMCRichOpPhotAlg("MCRichHitToMCRichOpPhot"+slot) ]

            # Data packing ...
            if self.getProp("EnablePack") :
                packing = GaudiSequencer(self.slotName(slot)+"EventDataPacking")
                simSlotSeq.Members += [ packing ]
                SimConf().PackingSequencers[slot] = packing             
    
    #--Set the energy of the beam,
    #--the half effective crossing angle (in LHCb coordinate system),
    #--beta* and emittance
    #--and configure the colliding beam tool for all type of events in
    #--pp collisions.
    #--For beam gas events (with hijing) only the energy of the beams is set
    def setBeamParameters( self, CrossingSlots ):

        from Configurables import ( MinimumBias , FixedNInteractions , HijingProduction ) 
        from Configurables import ( BcVegPyProduction , Special , BcVegPyProduction ) 

        #
        beamMom                        = self.getProp("BeamMomentum")
        angle                          = self.getProp("BeamCrossingAngle")
        xAngleBeamLine, yAngleBeamLine = self.getProp("BeamLineAngles")
        emittance                      = self.getProp("BeamEmittance")
        betaStar                       = self.getProp("BeamBetaStar")
        lumiPerBunch                   = self.getProp("Luminosity")
        totCrossSection                = self.getProp("TotalCrossSection")
        meanX, meanY, meanZ            = self.getProp("InteractionPosition")
        sigmaX, sigmaY, sigmaZ         = self.getProp("InteractionSize")


        
        # Give beam parameters to GenInit algorithm
        genInit = GenInit( "GaussGen" )
        genInit.CreateBeam              = True
        genInit.BeamEnergy              = beamMom
        genInit.HorizontalCrossingAngle = angle
        genInit.VerticalCrossingAngle   = 0. 
        genInit.NormalizedEmittance     = emittance
        genInit.BetaStar                = betaStar
        genInit.HorizontalBeamlineAngle = xAngleBeamLine
        genInit.VerticalBeamlineAngle   = yAngleBeamLine
        genInit.Luminosity              = lumiPerBunch
        genInit.TotalCrossSection       = totCrossSection
        genInit.XLuminousRegion         = meanX
        genInit.YLuminousRegion         = meanY
        genInit.ZLuminousRegion         = meanZ
        genInit.BunchLengthRMS          = sigmaZ
    
        gen_t0 = Generation("Generation")
        
        # the following is for beam gas events, the values are just to give the
        # nominal beam conditions in the data but 1 single interaction is 
        # forced selecting the appropriate pileup tool in the eventtype
        gen_t0.addTool(FixedNInteractions,name="FixedNInteractions")
        gen_t0.FixedNInteractions.NInteractions = 1
    
        # Special signal  (Bc with BcVegPy)
        pInGeV   = beamMom*SystemOfUnits.GeV/SystemOfUnits.TeV
        ecmInGeV = 2*pInGeV
        txtECM = "upcom ecm "+str(ecmInGeV)
        gen_t0.addTool(Special,name="Special")
        gen_t0.Special.addTool(BcVegPyProduction,name="BcVegPyProduction")
        gen_t0.Special.BcVegPyProduction.BcVegPyCommands += [ txtECM ]

        # or with Hijing
        txtP = "hijinginit efrm "+str(pInGeV)
        gen_t0.addTool(MinimumBias,name="MinimumBias")
        gen_t0.MinimumBias.addTool(HijingProduction,name="HijingProduction")
        gen_t0.MinimumBias.HijingProduction.Commands += [ txtP ]

    ## end of functions to set beam paramters and propagate them
    ##########################################################################

    ##
    ##
    def configurePhases( self, SpillOverSlots ):
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


    ##
    ##
    def veloGeometry( self, VeloPostMC09 ):
        """
        File containing the list of detector element to explicitely set
        to have misalignement in the VELO.
        """
        print 'VeloPostMC09 ',VeloPostMC09
        Geo = GiGaInputStream('Geo')
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
    def checkVeloDDDB( self , VeloPostMC09):
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

        # check if the selected tags require one of the postMC09 Velo geometries 
        if (DDDBDate >= GTagLimit1) or (cdbVeloDate >= VeloLTagLimit1):
            VeloPostMC09 = 1
        if (DDDBDate >= GTagLimit2) or (cdbVeloDate >= VeloLTagLimit2):
            VeloPostMC09 = 2

        return VeloPostMC09
    ##
    ##
    def setPhysList( self ):

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
        elif(hadronPhys == "FTFP_BERT"):
            addConstructor("G4HadronElasticPhysics", "ElasticPhysics")
            addConstructor("HadronPhysicsFTFP_BERT", "FTFP_BERTPhysics")
            addConstructor("G4QStoppingPhysics", "QStoppingPhysics")
            addConstructor("G4NeutronTrackingCut", "NeutronTrkCut")
        else:
            raise RuntimeError("Unknown Hadron PhysicsList chosen ('%s')"%hadronPhys)
        
        
        ## --- LHCb specific physics: 
        if  (lhcbPhys == True):
        ## LHCb specific RICH processes to add if rich sim is enabled
            gmpl.PhysicsConstructors.append("GiGaPhysConstructorOp")
            gmpl.PhysicsConstructors.append("GiGaPhysConstructorHpd")
        ## LHCb particles unknown to default Geant4
            gmpl.PhysicsConstructors.append("GiGaPhysUnknownParticles")
        elif (lhcbPhys == False):
            log.warning("The lhcb-related physics (RICH processed, UnknowParticles) is disabled")
        else:        
            raise RuntimeError("Unknown setting for LHCbPhys PhysicsList chosen ('%s')"%lhcbPhys)

        ## and other exotic physics
        if (otherPhys == 'Higgs'):
            log.info("Enabling physics processe for Higgs particles")
            gmpl.PhysicsConstructors.append("GiGaHiggsParticles")
        else:
            if (otherPhys != '' ):
               raise RuntimeError("Unknown setting for OtherPhys PhysicsList chosen ('%s')"%otherPhys) 
       
             
    ##
    ##
    ## Apply the configuration
    def __apply_configuration__(self):
        
        GaudiKernel.ProcessJobOptions.PrintOff()

        #defineDB() in Boole and
        # defineGeometry() in Brunel, need the same + random seeds
        self.configureRndmEngine()
        self.configureInput()  #defineEvents() in both Boole and Brunel
        LHCbApp( Simulation = True ) # in Boole? where?

        #--Define sequences: generator, simulation
        #  each with its init, make, moni
        #  in the sim phase define the geometry to simulate and the settings
        
        self.checkGeoSimMoniDictionnary() ## raise an error if DetectorGeo/Sim/Moni dictionnaries are incompatible
        
        # Propagate properties to SimConf
        SimConf().setProp("Writer","GaussTape")
        self.setOtherProps( SimConf(), ["SpilloverPaths","EnablePack","Phases","DataType"] )

        #Setup persistency services
        self.definePersistency()
        
        # CRJ : Propagate detector list to SimConf. Probably could be simplified a bit
        #       by sychronising the options in Gauss() and SimConf()
        detlist = []
        if 'Velo'    in self.getProp('DetectorSim')['VELO'] : detlist += ['Velo']
        if 'PuVeto'  in self.getProp('DetectorSim')['VELO'] : detlist += ['PuVeto']
        if 'VeloPix' in self.getProp('DetectorSim')['VELO'] : detlist += ['VeloPix']
        if 'TT'      in self.getProp('DetectorSim')['TT']   : detlist += ['TT']
        if 'IT'      in self.getProp('DetectorSim')['IT']   : detlist += ['IT']
        if 'OT'      in self.getProp('DetectorSim')['OT']   : detlist += ['OT']
        if len(self.getProp('DetectorSim')['RICH'])>0      : detlist += ['Rich']
        if len(self.getProp('DetectorSim')['MUON'])>0      : detlist += ['Muon']
        if 'Spd'     in self.getProp('DetectorSim')['CALO'] : detlist += ['Spd']
        if 'Prs'     in self.getProp('DetectorSim')['CALO'] : detlist += ['Prs']
        if 'Ecal'    in self.getProp('DetectorSim')['CALO'] : detlist += ['Ecal']
        if 'Hcal'    in self.getProp('DetectorSim')['CALO'] : detlist += ['Hcal']
        SimConf().setProp("Detectors",detlist)


        # Don't want SIM data unpacking enabled in DoD service
        SimConf().EnableUnpack = False
        
        crossingList = [ '' ]
        spillOverList = self.getProp("SpilloverPaths")
        if '' in spillOverList : spillOverList.remove('')
        crossingList += spillOverList
        self.setBeamParameters( crossingList )
        self.configurePhases( crossingList )  # in Boole, defineOptions() in Brunel

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
    
