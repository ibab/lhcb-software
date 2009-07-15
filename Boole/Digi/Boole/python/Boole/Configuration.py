"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.53 2009-07-15 16:30:59 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import LHCbConfigurableUser, LHCbApp, ProcessPhase, L0Conf, DigiConf

class Boole(LHCbConfigurableUser):

    ## Known sequences, all run by default
    KnownInitSubdets = [ "Data", "MUON" ]
    KnownDigiSubdets = [ "VELO", "TT", "IT", "OT", "RICH", "CALO", "MUON", "L0" ]
    KnownLinkSubdets = [ "VELO", "TT", "IT", "OT", "Tr", "RICH", "CALO", "MUON", "L0" ]
    KnownMoniSubdets = [ "VELO", "TT", "IT", "OT", "RICH", "CALO", "MUON", "L0", "MC" ]
    KnownFilterSubdets = [ "L0", "ODIN" ]
    KnownHistOptions = ["","None","Default","Expert"]

    ## Default main sequences
    DefaultSequence = [ "ProcessPhase/Init",
                        "ProcessPhase/Digi",
                        "ProcessPhase/Link",
                        "ProcessPhase/Moni",  
                        "ProcessPhase/Filter" ]
    
    __slots__ = {
        "EvtMax"         : -1
       ,"SkipEvents"     : 0
       ,"UseSpillover"   : False
       ,"SpilloverPaths" : ["Prev", "PrevPrev", "Next"]
       ,"TAEPrev"        : 0
       ,"TAENext"        : 0
       ,"TAESubdets"     : [ "CALO", "MUON" ]
       ,"Outputs"        : [ "DIGI" ]
       ,"DigiType"       : "Default"
       ,"Histograms"     : "Default"
       ,"NoWarnings"     : False
       ,"OutputLevel"    : INFO 
       ,"DatasetName"    : "Boole"
       ,"DataType"       : "2009"
       ,"DDDBtag"        : ""
       ,"CondDBtag"      : ""
       ,"Monitors"       : []
       ,"MainSequence"   : []
       ,"InitSequence"   : []
       ,"DigiSequence"   : []
       ,"LinkSequence"   : []
       ,"MoniSequence"   : []
       ,"FilterSequence" : []
        }

    _propertyDocDct = { 
        'EvtMax'       : """ Maximum number of events to process """
       ,'SkipEvents'   : """ Number of events to skip """
       ,'UseSpillover' : """ Flag to enable spillover (default False) """
       ,'SpilloverPaths':""" Paths to fill when spillover is enabled """
       ,'TAEPrev'      : """ Number of Prev Time Alignment Events to generate """
       ,'TAENext'      : """ Number of Next Time Alignment Events to generate """
       ,'TAESubdets'   : """ Subdetectors for which TAE are enabled """
       ,'Outputs'      : """ List of outputs: ['MDF','DIGI','L0ETC'] (default 'DIGI') """
       ,'DigiType'     : """ Defines content of DIGI file: ['Minimal','Default',Extended'] """
       ,'Histograms'   : """ Type of histograms: ['None','Default','Expert'] """
       ,'NoWarnings'   : """ Flag to suppress all MSG::WARNING or below (default False) - OBSOLETE - Please use OutputLevel property instead, setting it to ERROR level."""
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'DatasetName'  : """ String used to build output file names """
       ,'DataType'     : """ Data type. Default '2008' """
       ,'DDDBtag'      : """ Tag for DDDB """
       ,'CondDBtag'    : """ Tag for CondDB """
       ,'Monitors'     : """ List of monitors to execute """
       ,'MainSequence' : """ The default main sequence, see self.DefaultSequence """
       ,'InitSequence' : """ List of initialisation sequences, see KnownInitSubdets """
       ,'DigiSequence' : """ List of subdetectors to digitize, see KnownDigiSubdets """
       ,'LinkSequence' : """ List of MC truth link sequences, see KnownLinkSubdets  """
       ,'MoniSequence' : """ List of subdetectors to monitor, see KnownMoniSubdets """
       ,'FilterSequence' : """ List of Filter sequences, see KnownFilterSubdets  """
       }
    
    __used_configurables__ = [ LHCbApp, L0Conf, DigiConf ]

    def defineDB(self):
        if self.getProp("DataType") == "DC06" :
            raise RuntimeError( "DC06 data type no longer supported. Please use an earlier Boole version" )
            
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag","DataType"])
        LHCbApp().Simulation = True


    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])


    def configurePhases(self):
        """
        Set up the top level sequence and its phases
        """
        booleSeq = GaudiSequencer("BooleSequencer")
        ApplicationMgr().TopAlg = [ booleSeq ]
        mainSeq = self.getProp("MainSequence")
        if len( mainSeq ) == 0:
            mainSeq = self.DefaultSequence
        booleSeq.Members += mainSeq

        if self.getProp("TAENext") > 0 or self.getProp("TAEPrev") > 0:
            tae = True
            self.enableTAE()
        else:
            tae = False

        initDets   = self._setupPhase( "Init",   self.KnownInitSubdets )
        digiDets   = self._setupPhase( "Digi",   self.KnownDigiSubdets )
        linkDets   = self._setupPhase( "Link",   self.KnownLinkSubdets )
        moniDets   = self._setupPhase( "Moni",   self.KnownMoniSubdets )

        self.configureInit(tae, initDets)
        self.configureDigi(digiDets)
        self.configureLink(linkDets,moniDets)
        self.configureMoni(moniDets)
        self.configureFilter()
            
    def configureInit(self,tae,initDets):
        """
        Set up the initialization sequence
        """
        # Start the DataOnDemandSvc ahead of ToolSvc
        ApplicationMgr().ExtSvc  += [ "DataOnDemandSvc" ]
        ApplicationMgr().ExtSvc  += [ "ToolSvc" ]

        ProcessPhase("Init").DetectorList.insert(0,"Boole") # Always run Boole initialisation first!
        GaudiSequencer("InitBooleSeq").Members += [ "BooleInit" ]

        # Better name for this would be "DiracMode"
        if self.getProp( "NoWarnings" ) :
            log.warning("Boole().NoWarnings=True property is obsolete and maintained for Dirac compatibility. Please use Boole().OutputLevel=ERROR instead")
            self.OutputLevel = ERROR
            LHCbApp().setProp( "TimeStamp", True )

        # OutputLevel
        self.setOtherProp(LHCbApp(),"OutputLevel")
        if self.isPropertySet( "OutputLevel" ) :
            level = self.getProp("OutputLevel")
            if level == ERROR or level == WARNING :
                # Suppress known warnings
                importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )
                # Additional information to be kept
                getConfigurable("BooleInit").OutputLevel  = INFO
            
        # Do not print event number at every event (done already by BooleInit)
        EventSelector().PrintFreq = -1

        if self.getProp("UseSpillover"):
            if tae :
                log.warning("Disabling spillover, incompatible with TAE")
            else:
                self.enableSpillover()

  
        if "MUON" in initDets:
            # Muon Background
            from Configurables import MuonBackground
            GaudiSequencer("InitMUONSeq").Members += [ MuonBackground("MuonLowEnergy") ]
            importOptions( "$MUONBACKGROUNDROOT/options/MuonLowEnergy-G4.opts" )
            if not tae:
                GaudiSequencer("InitMUONSeq").Members += [ MuonBackground("MuonFlatSpillover") ]
                importOptions( "$MUONBACKGROUNDROOT/options/MuonFlatSpillover-G4.opts" )


    def configureDigi(self,digiDets):
        """
        Set up the digitization sequence
        """
        importOptions("$STDOPTS/PreloadUnits.opts") # needed by VELO and ST
        if "VELO" in digiDets : self.configureDigiVELO( GaudiSequencer("DigiVELOSeq"), "" )
        if "TT"   in digiDets : self.configureDigiST(   GaudiSequencer("DigiTTSeq"), "TT", "" )
        if "IT"   in digiDets : self.configureDigiST(   GaudiSequencer("DigiITSeq"), "IT", "" )
        if "OT"   in digiDets : self.configureDigiOT(   GaudiSequencer("DigiOTSeq"), "" )
        if "RICH" in digiDets : self.configureDigiRICH( GaudiSequencer("DigiRICHSeq"), "" )
        if "CALO" in digiDets : self.configureDigiCALO( GaudiSequencer("DigiCALOSeq"), "" )
        if "MUON" in digiDets : self.configureDigiMUON( GaudiSequencer("DigiMUONSeq"), "" )
        if "L0"   in digiDets : self.configureDigiL0(   GaudiSequencer("DigiL0Seq"), "" )

    def configureDigiVELO(self, seq, tae ):
        # Velo digitisation and clustering (also for PuVeto and trigger)
        if tae == "":
            from Configurables import VeloSim
            importOptions("$VELOSIMULATIONROOT/options/VeloSim.opts")
            importOptions("$VELOALGORITHMSROOT/options/VeloAlgorithms.opts")
            seq.Members += [ VeloSim("VeloSim") ]
            seq.Members += [ VeloSim("VeloPUSim") ]
            seq.Members += [ "VeloDataProcessor" ]
            seq.Members += [ "VeloClusterMaker" ]
            seq.Members += [ "PrepareVeloRawBuffer" ]
        else:
            raise RuntimeError("TAE not implemented for VELO")

    def configureDigiST(self, seq, det, tae ):
        # Silicon Tracker digitisation
        from Configurables import ( MCSTDepositCreator, MCSTDigitCreator, STDigitCreator,
                                    STClusterCreator, STClusterKiller, STClustersToRawBankAlg )
        if tae == "":
            if det == "IT":
                importOptions("$STDIGIALGORITHMSROOT/options/itDigi.opts")
            elif det == "TT":
                importOptions("$STDIGIALGORITHMSROOT/options/ttDigi.opts")
            else:
                raise RuntimeError("Unknown ST detector '%s'"%det)

            seq.Members += [ MCSTDepositCreator("MC%sDepositCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ MCSTDigitCreator("MC%sDigitCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ STDigitCreator("%sDigitCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ STClusterCreator("%sClusterCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ STClusterKiller("%sClusterKiller%s"%(det,tae),DetType=det) ]
            seq.Members += [ STClustersToRawBankAlg("create%sRawBuffer%s"%(det,tae),DetType=det) ]
        else:
            raise RuntimeError("TAE not implemented for %s"%det)

    def configureDigiOT(self, seq, tae ):
        # Outer Tracker digitisation
        from Configurables import MCOTDepositCreator, MCOTTimeCreator, OTFillRawBuffer
        seq.Members += [ MCOTDepositCreator("MCOTDepositCreator%s"%tae) ]
        seq.Members += [ MCOTTimeCreator("MCOTTimeCreator%s"%tae) ]
        seq.Members += [ OTFillRawBuffer("OTFillRawBuffer%s"%tae) ]

    def configureDigiRICH(self, seq, tae ):
        if tae == "":
            from RichDigiSys.Configuration import RichDigiSysConf
            self.setOtherProp(RichDigiSysConf(),"UseSpillover")
            RichDigiSysConf().applyConf(GaudiSequencer("DigiRICHSeq"))
        else:
            raise RuntimeError("TAE not implemented for RICH")
            
    def configureDigiCALO(self, seq, tae ):
        # Calorimeter digitisation
        from Configurables import CaloSignalAlg, CaloDigitAlg, CaloFillPrsSpdRawBuffer, CaloFillRawBuffer
        if tae != "":
            seq.Context = "TAE"
        seq.Members += [CaloSignalAlg("SpdSignal%s"%tae),
                        CaloSignalAlg("PrsSignal%s"%tae),
                        CaloSignalAlg("EcalSignal%s"%tae),
                        CaloSignalAlg("HcalSignal%s"%tae),
                        CaloDigitAlg("SpdDigit%s"%tae),
                        CaloDigitAlg("PrsDigit%s"%tae),
                        CaloDigitAlg("EcalDigit%s"%tae),
                        CaloDigitAlg("HcalDigit%s"%tae) ]
        rawPrsSpd = CaloFillPrsSpdRawBuffer( "CaloFillPrsSpdRawBuffer%s"%tae, DataCodingType = 3 )
        rawEcal = CaloFillRawBuffer( "EcalFillRawBuffer%s"%tae, DataCodingType = 2 )
        rawHcal = CaloFillRawBuffer( "HcalFillRawBuffer%s"%tae, DataCodingType = 2 )
        seq.Members += [ rawPrsSpd, rawEcal, rawHcal ]

    def configureDigiMUON(self, seq, tae ):
        from Configurables import MuonDigitization, MuonDigitToRawBuffer
        seq.Members += [ MuonDigitization("MuonDigitization%s"%tae) ]
        seq.Members += [ MuonDigitToRawBuffer("MuonDigitToRawBuffer%s"%tae) ]

    def configureDigiL0(self, seq, tae ):
        if tae == "":
            # L0 trigger Simulation
            from Configurables import L0Conf
            L0Conf().L0Sequencer = seq
            L0Conf().SimulateL0  = True
            self.setOtherProps( L0Conf(), ["DataType"] )
        else:
            raise RuntimeError("TAE not implemented for L0")
                

    def configureFilter(self):
        """
        Set up the filter sequence to selectively write out events
        """
        filterDets = self.getProp("FilterSequence")
        for det in filterDets:
            if det not in self.KnownFilterSubdets :
                log.warning("Unknown subdet '%s' in FilterSequence"%det)

        filterSeq = ProcessPhase("Filter", ModeOR = True )
        filterSeq.DetectorList += filterDets

        if "L0" in filterDets: 
            from Configurables import L0Conf
            L0Conf().FilterSequencer = GaudiSequencer("FilterL0Seq")

        if "ODIN" in filterDets: 
            from Configurables import OdinTypesFilter
            odinFilter = OdinTypesFilter()
            GaudiSequencer("FilterODINSeq").Members += [ odinFilter ]
            if not odinFilter.isPropertySet( "TriggerTypes" ):
                odinFilter.TriggerTypes = ["RandomTrigger"]

    def configureLink(self,linkDets,moniDets):
        """
        Set up the MC links sequence
        """

        # Unpack MCParticles and MCVertices if not existing on input file
        DataOnDemandSvc().AlgMap["MC/Particles"] = "UnpackMCParticle"
        DataOnDemandSvc().AlgMap["MC/Vertices"]  = "UnpackMCVertex"

        doWriteTruth = ("DIGI" in self.getProp("Outputs")) and (self.getProp("DigiType").capitalize() != "Minimal")

        if "VELO" in linkDets or "VELO" in moniDets or "Tr" in linkDets:
            seq = GaudiSequencer("LinkVELOSeq")
            from Configurables import DecodeVeloRawBuffer
            decodeVelo = DecodeVeloRawBuffer()
            decodeVelo.DecodeToVeloClusters     = True
            decodeVelo.DecodeToVeloLiteClusters = False
            seq.Members += [ decodeVelo ]
            seq.Members += [ "VeloCluster2MCHitLinker" ]
            seq.Members += [ "VeloCluster2MCParticleLinker" ]

        if "TT" in linkDets or "IT" in linkDets or "TT" in moniDets or "IT" in moniDets or "Tr" in linkDets:
            from Configurables import STDigit2MCHitLinker, STCluster2MCHitLinker, STCluster2MCParticleLinker
            if "TT" in linkDets or "TT" in moniDets:
                seq = GaudiSequencer("LinkTTSeq")
                seq.Members += [ STDigit2MCHitLinker("TTDigitLinker") ]
                seq.Members += [ STCluster2MCHitLinker("TTClusterLinker") ]
                seq.Members += [ STCluster2MCParticleLinker("TTTruthLinker") ]

            if "IT" in linkDets or "IT" in moniDets:
                seq = GaudiSequencer("LinkITSeq")
                seq.Members += [ STDigit2MCHitLinker("ITDigitLinker", DetType   = "IT") ]
                seq.Members += [ STCluster2MCHitLinker("ITClusterLinker", DetType   = "IT") ]
                seq.Members += [ STCluster2MCParticleLinker("ITTruthLinker", DetType   = "IT") ]

        if "OT" in linkDets or "OT" in moniDets or "Tr" in linkDets:
            seq = GaudiSequencer("LinkOTSeq")
            seq.Members += [ "OTMCDepositLinker" ]
            seq.Members += [ "OTMCHitLinker" ]
            seq.Members += [ "OTMCParticleLinker" ]

        if "Tr" in linkDets and doWriteTruth:
            seq = GaudiSequencer("LinkTrSeq")
            seq.Members += [ "BuildMCTrackInfo" ]

        if "RICH" in linkDets and doWriteTruth:
            seq = GaudiSequencer("LinkRICHSeq")
            seq.Members += [ "Rich::MC::MCRichDigitSummaryAlg" ]

        if "CALO" in linkDets or "CALO" in moniDets:
            from Configurables import CaloDigitsFromRaw, CaloReCreateMCLinks, CaloDigitMCTruth
            seq = GaudiSequencer("LinkCALOSeq")
            seq.Members += [ CaloDigitsFromRaw("EcalFromRaw") ]
            seq.Members += [ CaloDigitsFromRaw("HcalFromRaw") ]
            recreateLinks = CaloReCreateMCLinks()
            recreateLinks.Digits   = ["Raw/Ecal/Digits", "Raw/Hcal/Digits" ]
            recreateLinks.MCDigits = ["MC/Ecal/Digits",  "MC/Hcal/Digits" ]
            seq.Members += [ recreateLinks ]
            seq.Members += [ CaloDigitMCTruth("EcalDigitMCTruth") ]
            hcalTruth = CaloDigitMCTruth("HcalDigitMCTruth")
            hcalTruth.Input = "Raw/Hcal/Digits"
            hcalTruth.Detector = "/dd/Structure/LHCb/DownstreamRegion/Hcal"
            seq.Members += [ hcalTruth ]

        if "MUON" in linkDets and doWriteTruth:
            seq = GaudiSequencer("LinkMUONSeq")
            seq.Members += [ "MuonDigit2MCParticleAlg" ]
            seq.Members += [ "MuonTileDigitInfo" ]

        if "L0" in linkDets and doWriteTruth:
            from Configurables import L0Conf
            L0Conf().LinkSequencer = GaudiSequencer("LinkL0Seq")


    def enableTAE(self):
        """
        switch to generate Time Alignment events (only Prev1 for now).
        """
        from GaudiKernel.SystemOfUnits import ns

        taeSlots = []
        mainSeq = GaudiSequencer("BooleSequencer").Members

        taePrev = self.getProp("TAEPrev")
        while taePrev > 0:
            digi = mainSeq.index("ProcessPhase/Digi")
            taePhase = ProcessPhase( "DigiPrev%s"%taePrev )
            taePhase.RootInTES = "Prev%s/"%taePrev
            taePhase.GlobalTimeOffset = -taePrev * 25 * ns
            mainSeq.insert(digi,taePhase)
            taeSlots.append( "Prev%s"%taePrev )
            taePrev -= 1

        taeNext = self.getProp("TAENext")
        while taeNext>0:
            digi = mainSeq.index("ProcessPhase/Digi")
            taePhase = ProcessPhase( "DigiNext%s"%taeNext )
            taePhase.RootInTES = "Next%s/"%taeNext
            taePhase.GlobalTimeOffset = taeNext * 25 * ns
            mainSeq.insert(digi+1,taePhase)
            taeSlots.append( "Next%s"%taeNext )
            taeNext -= 1
        GaudiSequencer("BooleSequencer").Members = mainSeq

        for taeSlot in taeSlots:
            taePhase = ProcessPhase( "Digi%s"%taeSlot )
            taeDets  = self.getProp("TAESubdets")
            taePhase.DetectorList = [ "Init" ] + taeDets
            from Configurables import BooleInit
            slotInit =  BooleInit("Init%s"%taeSlot, RootInTES = "%s/"%taeSlot )
            GaudiSequencer( "Digi%sInitSeq"%taeSlot ).Members = [ slotInit ]
            if "VELO" in taeDets:
                self.configureDigiVELO( GaudiSequencer("Digi%sVELOSeq"%taeSlot), taeSlot )
            if "TT" in taeDets:
                self.configureDigiST( GaudiSequencer("Digi%sTTSeq"%taeSlot), "TT", taeSlot )
            if "IT" in taeDets:
                self.configureDigiST( GaudiSequencer("Digi%sITSeq"%taeSlot), "IT", taeSlot )
            if "OT" in taeDets:
                self.configureDigiOT( GaudiSequencer("Digi%sOTSeq"%taeSlot), taeSlot )
            if "RICH" in taeDets:
                self.configureDigiRICH( GaudiSequencer("Digi%sRICHSeq"%taeSlot), taeSlot )
            if "CALO" in taeDets:
                self.configureDigiCALO( GaudiSequencer("Digi%sCALOSeq"%taeSlot), taeSlot )
            if "MUON" in taeDets:
                self.configureDigiMUON( GaudiSequencer("Digi%sMUONSeq"%taeSlot), taeSlot )
            if "L0" in taeDets:
                self.configureDigiL0( GaudiSequencer("Digi%sL0Seq"%taeSlot), taeSlot )
            

    def enableSpillover(self):
        """
        switch to generate spillover events.
        """
        from Configurables import MuonBackground, MuonDigitization
        MuonDigitization().SpilloverPathsSize = len(self.getProp("SpilloverPaths"))
        MuonBackground("MuonLowEnergy").SpilloverPathsSize = len(self.getProp("SpilloverPaths"))

        # Handle the unpacking of pSim containers
        for spill in self.getProp("SpilloverPaths") :
            from Configurables import UnpackMCParticle, UnpackMCVertex
            particleUnpacker = UnpackMCParticle( "UnpackMCP" + spill )
            particleUnpacker.RootInTES = spill
            vertexUnpacker = UnpackMCVertex( "UnpackMCV" + spill )
            vertexUnpacker.RootInTES = spill
            DataOnDemandSvc().AlgMap[ spill + "/MC/Particles" ] = particleUnpacker
            DataOnDemandSvc().AlgMap[ spill + "/MC/Vertices" ] = vertexUnpacker


    def defineMonitors(self):
        # get all defined monitors
        monitors = self.getProp("Monitors") + LHCbApp().getProp("Monitors")
        # Currently no Boole specific monitors, so pass them all to LHCbApp
        LHCbApp().setProp("Monitors", monitors)

        # Use TimingAuditor for timing, suppress printout from SequencerTimerTool
        from Configurables import ApplicationMgr, AuditorSvc, SequencerTimerTool
        ApplicationMgr().ExtSvc += [ 'AuditorSvc' ]
        ApplicationMgr().AuditAlgorithms = True
        AuditorSvc().Auditors += [ 'TimingAuditor' ] 
        SequencerTimerTool().OutputLevel = WARNING

    def saveHistos(self):

        # ROOT persistency for histograms
        importOptions('$STDOPTS/RootHist.opts')
        from Configurables import RootHistCnv__PersSvc
        RootHistCnv__PersSvc('RootHistCnv').ForceAlphaIds = True

        histOpt = self.getProp("Histograms").capitalize()
        if histOpt == "None" or histOpt == "":
            # HistogramPersistency still needed to read in Muon background.
            # so do not set ApplicationMgr().HistogramPersistency = "NONE"
            return

        # Use a default histogram file name if not already set
        if not HistogramPersistencySvc().isPropertySet( "OutputFile" ):
            histosName   = self.getProp("DatasetName")
            if histosName == "": histosName = "Boole"
            if (self.evtMax() > 0): histosName += '-' + str(self.evtMax()) + 'ev'
            if histOpt == "Expert": histosName += '-expert'
            histosName += '-histos.root'
            HistogramPersistencySvc().OutputFile = histosName
    
    def defineOutput(self):
        """
        Set up output stream according to output data type
        """

        knownOptions = ["MDF","DIGI","L0ETC"]
        outputs = []
        for option in self.getProp("Outputs"):
            if option not in knownOptions:
                raise RuntimeError("Unknown Boole().Outputs value '%s'"%option)
            outputs.append( option )

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        if "DIGI" in outputs:
            writerName = "DigiWriter"
            digiWriter = OutputStream( writerName, Preload=False )
            if not digiWriter.isPropertySet( "Output" ):
                digiWriter.Output  = "DATAFILE='PFN:" + self.outputName() + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
            digiWriter.RequireAlgs.append( "Filter" )
            if self.getProp( "NoWarnings" ) and not digiWriter.isPropertySet( "OutputLevel" ):
                digiWriter.OutputLevel = INFO

            # Set up the Digi content
            DigiConf().Writer = writerName
            self.setOtherProps(DigiConf(),["DigiType","TAEPrev","TAENext"])
            if self.getProp("UseSpillover"):
                self.setOtherProps(DigiConf(),["SpilloverPaths"])

            # In Minimal case, filter the MCVertices before writing
            if self.getProp("DigiType").capitalize() == "Minimal":
                ApplicationMgr().TopAlg.append( "FilterMCPrimaryVtx" )

        if "L0ETC" in outputs:
            from Configurables import L0Conf
            seq = GaudiSequencer("L0ETC")
            ApplicationMgr().OutStream += [ seq ]
            L0Conf().ETCSequencer = seq
            MyWriter = TagCollectionStream( "WR" )
            if not MyWriter.isPropertySet( "Output" ):
                L0Conf().ETCOutput = self.getProp("DatasetName") + "-L0ETC.root"
            if self.getProp( "NoWarnings" ) and not MyWriter.isPropertySet( "OutputLevel" ):
                MyWriter.OutputLevel = INFO

        if "MDF" in outputs:
            # Set up the MDF persistency
            importOptions("$STDOPTS/RawDataIO.opts")
            # Make sure that file will have no knowledge of other nodes
            from Configurables import EventNodeKiller
            nodeKiller = EventNodeKiller()
            nodeKiller.Nodes  = [ "Rec", "Trig", "MC", "Raw", "Gen", "Link", "pSim" ]
            nodeKiller.Nodes += self.getProp("SpilloverPaths")
            taePrev = self.getProp("TAEPrev")
            while taePrev > 0:
                nodeKiller.Nodes += ["Prev%"%taePrev]
                taePrev -= 1
            taeNext = self.getProp("TAENext")
            while taeNext>0:
                nodeKiller.Nodes += ["Next%s"%taeNext]
                taeNext -= 1

            MyWriter = OutputStream( "RawWriter", Preload = False, ItemList = ["/Event/DAQ/RawEvent#1"] )
            if not MyWriter.isPropertySet( "Output" ):
                MyWriter.Output = "DATAFILE='PFN:" + self.outputName() + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
            MyWriter.RequireAlgs.append( "Filter" )
            if self.getProp( "NoWarnings" ) and not MyWriter.isPropertySet( "OutputLevel" ):
                MyWriter.OutputLevel = INFO
            ApplicationMgr().OutStream += [ nodeKiller, MyWriter ]


    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        if len(self.getProp( "FilterSequence" )) > 0 : outputName += '-filtered'
        if self.getProp("DigiType") != "Default" : outputName += '-%s'%self.getProp("DigiType")
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()


    def configureMoni(self,moniDets):
        # Set up monitoring
        histOpt = self.getProp("Histograms").capitalize()
        if histOpt not in self.KnownHistOptions:
            raise RuntimeError("Unknown Histograms option '%s'"%histOpt)

        from Configurables import BooleInit, MemoryTool
        booleInit = BooleInit()
        booleInit.addTool( MemoryTool(), name = "BooleMemory" )
        booleInit.BooleMemory.HistoTopDir = "Boole/"
        booleInit.BooleMemory.HistoDir    = "MemoryTool"

        if "VELO" in moniDets:
            from Configurables import  VeloSimMoni,VeloDigit2MCHitLinker,VeloDigiMoni,VeloRawClustersMoni
            GaudiSequencer("MoniVELOSeq").Members += [ VeloSimMoni(), VeloDigit2MCHitLinker(),
                                                       VeloDigiMoni(), VeloRawClustersMoni()  ]

        if "IT" in moniDets or "TT" in moniDets:
            from Configurables import ( MCSTDepositMonitor, MCSTDigitMonitor, STDigitMonitor,
                      ST__STClusterMonitor, STEffChecker, MCParticle2MCHitAlg, MCParticleSelector )
            from GaudiKernel.SystemOfUnits import GeV

        if "IT" in moniDets:
            mcDepMoni   = MCSTDepositMonitor(  "MCITDepositMonitor", DetType="IT" )
            mcDigitMoni = MCSTDigitMonitor(    "MCITDigitMonitor",   DetType="IT" )
            digitMoni   = STDigitMonitor(      "ITDigitMonitor",     DetType="IT" )
            clusMoni    = ST__STClusterMonitor(    "ITClusterMonitor",   DetType="IT" )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2ITMCHitAlg", MCHitPath = "MC/IT/Hits",
                                               OutputData = "/Event/MC/Particles2MCITHits" )
            effCheck    = STEffChecker(         "ITEffChecker",      DetType="IT" )
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            GaudiSequencer("MoniITSeq").Members += [ mcDepMoni, mcDigitMoni, digitMoni, clusMoni,
                                                     mcp2MCHit, effCheck ]
            if histOpt == "Expert":
                mcDepMoni.FullDetail   = True
                mcDigitMoni.FullDetail = True
                clusMoni.FullDetail    = True
                effCheck.FullDetail    = True

        if "TT" in moniDets:
            mcDepMoni   = MCSTDepositMonitor(  "MCTTDepositMonitor" )
            mcDigitMoni = MCSTDigitMonitor(    "MCTTDigitMonitor"   )
            digitMoni   = STDigitMonitor(      "TTDigitMonitor"     )
            clusMoni    = ST__STClusterMonitor(    "TTClusterMonitor"   )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2TTMCHitAlg", MCHitPath = "MC/TT/Hits",
                                               OutputData = "/Event/MC/Particles2MCTTHits" )
            effCheck    = STEffChecker(        "TTEffChecker"       )
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            GaudiSequencer("MoniTTSeq").Members += [ mcDepMoni, mcDigitMoni, digitMoni, clusMoni,
                                                     mcp2MCHit, effCheck ]
            if histOpt == "Expert":
                mcDepMoni.FullDetail   = True
                mcDigitMoni.FullDetail = True
                clusMoni.FullDetail    = True
                effCheck.FullDetail    = True

        if "OT" in moniDets:
            from Configurables import MCOTDepositMonitor
            GaudiSequencer("MoniOTSeq").Members += [ MCOTDepositMonitor() ]
            if histOpt == "Expert":
                importOptions("$OTMONITORROOT/options/Boole.opts")

        if "RICH" in moniDets:
            from Configurables import Rich__MC__Digi__DigitQC
            GaudiSequencer("MoniRICHSeq").Members += [ Rich__MC__Digi__DigitQC("RiDigitQC") ]

        if "CALO" in moniDets:
            from Configurables import CaloDigitChecker
            importOptions("$CALOMONIDIGIOPTS/CaloDigitChecker.opts")
            GaudiSequencer("MoniCALOSeq").Members += [ CaloDigitChecker("SpdCheck"),
                                                       CaloDigitChecker("PrsCheck"),
                                                       CaloDigitChecker("EcalCheck"),
                                                       CaloDigitChecker("HcalCheck") ]

        if "MUON" in moniDets:
            from Configurables import MuonDigitChecker
            GaudiSequencer("MoniMUONSeq").Members += [ "MuonDigitChecker" ]

        if "L0" in moniDets:
            from Configurables import L0Conf
            L0Conf().MoniSequencer = GaudiSequencer("MoniL0Seq")

        if "MC" in moniDets:
            from Configurables import UnpackMCVertex, UnpackMCParticle, CompareMCVertex, CompareMCParticle
            # This sequence only makes sense if input data is unpacked. Should be moved to Gauss
            testMCV = UnpackMCVertex(   "TestMCVertex",   OutputName = "MC/VerticesTest" )
            testMCP = UnpackMCParticle( "TestMCParticle", OutputName = "MC/ParticlesTest" )
            GaudiSequencer("MoniMCSeq").Members += [ testMCV, testMCP,
                                                     CompareMCParticle(), CompareMCVertex() ]

    def _setupPhase( self, name, knownDets ):
        seq = self.getProp("%sSequence"%name)
        if len( seq ) == 0:
            seq = knownDets
            self.setProp("%sSequence"%name, seq)
        else:
            for det in seq:
                if det not in knownDets:
                    log.warning("Unknown subdet '%s' in %sSequence"%(det,seq))
        ProcessPhase(name).DetectorList += seq
        return seq

    def __apply_configuration__(self):
        GaudiKernel.ProcessJobOptions.PrintOff()
        
        self.defineDB()
        self.defineEvents()
        self.configurePhases()
        self.defineOutput()
        self.defineMonitors()
        self.saveHistos()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
