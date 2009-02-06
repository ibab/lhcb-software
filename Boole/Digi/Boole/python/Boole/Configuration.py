"""
High level configuration tools for Boole
"""
__version__ = "$Id: Configuration.py,v 1.39 2009-02-06 16:11:48 cattanem Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import LHCbConfigurableUser, LHCbApp, ProcessPhase

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
       ,"SkipSpill"      : 0
       ,"UseSpillover"   : False
       ,"SpilloverPaths" : ["Prev", "PrevPrev", "Next"]
       ,"TAEPrev"        : 0
       ,"TAENext"        : 0
       ,"TAESubdets"     : [ "CALO", "MUON" ]
       ,"Outputs"        : [ "DIGI" ]
       ,"WriteL0Only"    : False
       ,"ExtendedDigi"   : False
       ,"Histograms"     : "Default"
       ,"NoWarnings"     : False
       ,"DatasetName"    : ''
       ,"DataType"       : "2008"
       ,"DDDBtag"        : ""
       ,"CondDBtag"      : ""
       ,"UseOracle"      : False
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
       ,'SkipSpill'    : """ Number of spillover events to skip """
       ,'UseSpillover' : """ Flag to enable spillover (default False) """
       ,'SpilloverPaths':""" Paths to fill when spillover is enabled """
       ,'TAEPrev'      : """ Number of Prev Time Alignment Events to generate """
       ,'TAENext'      : """ Number of Next Time Alignment Events to generate """
       ,'TAESubdets'   : """ Subdetectors for which TAE are enabled """
       ,'Outputs'      : """ List of outputs: ['MDF','DIGI','L0ETC'] (default 'DIGI') """
       ,'WriteL0Only'  : """ OBSOLETE. Add L0 to FilterSequence instead """
       ,'ExtendedDigi' : """ Flag to add MCHits to .digi output file (default False) """
       ,'Histograms'   : """ Type of histograms: ['None','Default','Expert'] """
       ,'NoWarnings'   : """ Flag to suppress all MSG::WARNING or below (default False) """ 
       ,'DatasetName'  : """ String used to build output file names """
       ,'DataType'     : """ Data type, can be ['DC06','2008']. Default '2008' """
       ,'DDDBtag'      : """ Tag for DDDB """
       ,'CondDBtag'    : """ Tag for CondDB """
       ,'UseOracle'    : """ Flag to enable Oracle CondDB. Default False (use SQLDDDB) """
       ,'Monitors'     : """ List of monitors to execute """
       ,'MainSequence' : """ The default main sequence, see self.DefaultSequence """
       ,'InitSequence' : """ List of initialisation sequences, see KnownInitSubdets """
       ,'DigiSequence' : """ List of subdetectors to digitize, see KnownDigiSubdets """
       ,'LinkSequence' : """ List of MC truth link sequences, see KnownLinkSubdets  """
       ,'MoniSequence' : """ List of subdetectors to monitor, see KnownMoniSubdets """
       ,'FilterSequence' : """ List of Filter sequences, see KnownFilterSubdets  """
       }
    
    __used_configurables__ = [ LHCbApp ]

    def defineDB(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag","UseOracle","DataType"])
        LHCbApp().Simulation = True


    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])

        skipSpill  = self.getProp("SkipSpill")
        if skipSpill  > 0 :
            if hasattr(EventSelector("SpilloverSelector"),"FirstEvent"):
                log.warning( "EventSelector('SpilloverSelector').FirstEvent and Boole().SkipSpill both defined, using Boole().SkipSpill")
            EventSelector("SpilloverSelector").FirstEvent = skipSpill + 1


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

        self.configureInit(tae)
        self.configureDigi()
        self.configureLink()
        self.configureMoni()
        self.configureFilter()
            
    def configureInit(self,tae):
        """
        Set up the initialization sequence
        """
        initDets = self._setupPhase( "Init", self.KnownInitSubdets )

        # Start the DataOnDemandSvc ahead of ToolSvc
        ApplicationMgr().ExtSvc  += [ "DataOnDemandSvc" ]
        ApplicationMgr().ExtSvc  += [ "ToolSvc" ]

        ProcessPhase("Init").DetectorList.insert(0,"Boole") # Always run Boole initialisation first!
        GaudiSequencer("InitBooleSeq").Members += [ "BooleInit" ]

        # Do not print event number at every event (done already by Boole)
        MessageSvc().OutputLevel = INFO
        EventSelector().PrintFreq = -1

        spill = self.getProp("UseSpillover")
        if tae :
            if spill :
                log.warning("Disabling spillover, incompatible with TAE")
                spill = False

        if spill :
            self.enableSpillover()
        else:
            if self.getProp("DataType") == "DC06" and not tae:
                log.warning("Spillover is disabled. Should normally be enabled for DC06!")

  
        if "MUON" in initDets:
            # Muon Background
            from Configurables import MuonBackground
            GaudiSequencer("InitMUONSeq").Members += [ MuonBackground("MuonLowEnergy") ]
            importOptions( "$MUONBACKGROUNDROOT/options/MuonLowEnergy-G4.opts" )
            if not tae:
                GaudiSequencer("InitMUONSeq").Members += [ MuonBackground("MuonFlatSpillover") ]
                importOptions( "$MUONBACKGROUNDROOT/options/MuonFlatSpillover-G4.opts" )


    def configureDigi(self):
        """
        Set up the digitization sequence
        """
        digiDets = self._setupPhase( "Digi", self.KnownDigiSubdets )

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

            mcdepCreator = MCSTDepositCreator("MC%sDepositCreator%s"%(det,tae),DetType=det)
            if self.getProp("DataType") == "DC06" :
                mcdepCreator.DepChargeTool = "SiDepositedCharge"
            seq.Members += [ mcdepCreator ]
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
        digitToRaw = MuonDigitToRawBuffer("MuonDigitToRawBuffer%s"%tae)
        if self.getProp("DataType") == "DC06" :
            digitToRaw.VType = 1 # DC06 RawBank type
        seq.Members += [ digitToRaw ]

    def configureDigiL0(self, seq, tae ):
        if tae == "":
            # L0 trigger Simulation
            seq.Members += [ GaudiSequencer("L0SimulationSeq") ]
            importOptions("$L0DUROOT/options/Boole.opts")
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

        l0yes = self.getProp( "WriteL0Only" )
        if l0yes :
            log.warning("WriteL0Only property is obsolete. Adding L0 to FilterSequence instead")
            if "L0" not in filterDets :
                filterDets += [ "L0" ]
                self.setProp("FilterSequence",filterDets)
        filterSeq = ProcessPhase("Filter", ModeOR = True )
        filterSeq.DetectorList += filterDets

        if "L0" in filterDets: 
            GaudiSequencer("FilterL0Seq").Members += [ "L0Filter" ]

        if "ODIN" in filterDets: 
            GaudiSequencer("FilterODINSeq").Members += [ "OdinTypesFilter" ]

    def configureLink(self):
        """
        Set up the MC links sequence
        """

        linkDets = self._setupPhase( "Link", self.KnownLinkSubdets )

        # Unpack MCParticles and MCVertices if not existing on input file
        DataOnDemandSvc().AlgMap["MC/Particles"] = "UnpackMCParticle"
        DataOnDemandSvc().AlgMap["MC/Vertices"]  = "UnpackMCVertex"

        # Pack them for the output if not on the input file...
        DataOnDemandSvc().AlgMap["pSim/MCParticles"] = "PackMCParticle"
        DataOnDemandSvc().AlgMap["pSim/MCVertices"]  = "PackMCVertex"

        if "VELO" in linkDets:
            from Configurables import DecodeVeloRawBuffer
            seq = GaudiSequencer("LinkVELOSeq")
            decodeVelo = DecodeVeloRawBuffer()
            decodeVelo.DecodeToVeloClusters     = True
            decodeVelo.DecodeToVeloLiteClusters = False
            seq.Members += [ decodeVelo ]
            seq.Members += [ "VeloCluster2MCHitLinker" ]
            seq.Members += [ "VeloCluster2MCParticleLinker" ]

        if "TT" in linkDets or "IT" in linkDets:
            from Configurables import STDigit2MCHitLinker, STCluster2MCHitLinker, STCluster2MCParticleLinker
            if "TT" in linkDets:
                seq = GaudiSequencer("LinkTTSeq")
                seq.Members += [ STDigit2MCHitLinker("TTDigitLinker") ]
                seq.Members += [ STCluster2MCHitLinker("TTClusterLinker") ]
                seq.Members += [ STCluster2MCParticleLinker("TTTruthLinker") ]

            if "IT" in linkDets:
                seq = GaudiSequencer("LinkITSeq")
                seq.Members += [ STDigit2MCHitLinker("ITDigitLinker", DetType   = "IT") ]
                seq.Members += [ STCluster2MCHitLinker("ITClusterLinker", DetType   = "IT") ]
                seq.Members += [ STCluster2MCParticleLinker("ITTruthLinker", DetType   = "IT") ]

        if "OT" in linkDets:
            seq = GaudiSequencer("LinkOTSeq")
            seq.Members += [ "OTMCDepositLinker" ]
            seq.Members += [ "OTMCHitLinker" ]
            seq.Members += [ "OTMCParticleLinker" ]

        if "Tr" in linkDets:
            seq = GaudiSequencer("LinkTrSeq")
            seq.Members += [ "BuildMCTrackInfo" ]

        if "RICH" in linkDets:
            seq = GaudiSequencer("LinkRICHSeq")
            seq.Members += [ "Rich::MC::MCRichDigitSummaryAlg" ]

        if "CALO" in linkDets:
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

        if "MUON" in linkDets:
            seq = GaudiSequencer("LinkMUONSeq")
            seq.Members += [ "MuonDigit2MCParticleAlg" ]
            seq.Members += [ "MuonTileDigitInfo" ]

        if "L0" in linkDets:
            seq = GaudiSequencer("LinkL0Seq")
            from Configurables import L0CaloToMCParticleAsct
            seq.Members += [ L0CaloToMCParticleAsct() ]
            seq.Members += [ L0CaloToMCParticleAsct("L0CaloFullTruth", InputContainer = "Trig/L0/FullCalo") ]


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
        if self.getProp("DataType") != "DC06" :
            log.warning("Spillover is enabled. Should normally be enabled only for DC06!")

        from Configurables import MergeEventAlg, UnpackMCParticle, UnpackMCVertex
        initDataSeq = GaudiSequencer( "InitDataSeq" )
        spillPaths  = self.getProp("SpilloverPaths")
        spillAlg    = MergeEventAlg( name = "SpilloverAlg", PathList = spillPaths )
        initDataSeq.Members += [ spillAlg ]
        importOptions("$DIGIALGROOT/options/Spillover.opts")
        from Configurables import MuonBackground, MuonDigitization
        MuonDigitization().EnableSpillover = True
        MuonBackground("MuonLowEnergy").EnableSpillover = True

        # Handle the unpacking of pSim containers
        for spill in spillPaths :
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
        if not hasattr( HistogramPersistencySvc(), "OutputFile" ):
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
            if option.upper() not in knownOptions:
                raise RuntimeError("Unknown Boole().Outputs value '%s'"%option)
            outputs.append( option.upper() )

        # POOL Persistency
        importOptions("$GAUDIPOOLDBROOT/options/GaudiPoolDbRoot.opts")

        if "DIGI" in outputs:
            # Objects to be written to output file
            importOptions("$STDOPTS/DigiContent.opts")
            extended = self.getProp("ExtendedDigi")
            if ( extended ): importOptions( "$STDOPTS/ExtendedDigi.opts" )

            MyWriter = OutputStream( "DigiWriter", Preload=False )
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output  = "DATAFILE='PFN:" + self.outputName() + ".digi' TYP='POOL_ROOTTREE' OPT='REC'"
            MyWriter.RequireAlgs.append( "Filter" )
            ApplicationMgr().OutStream.append( "DigiWriter" )

            # Add TAE RawEvents when enabled
            taePrev = self.getProp("TAEPrev")
            while taePrev > 0:
                MyWriter.ItemList += ["/Event/Prev%s/DAQ/RawEvent#1"%taePrev]
                taePrev -= 1
            taeNext = self.getProp("TAENext")
            while taeNext>0:
                MyWriter.ItemList += ["/Event/Next%s/DAQ/RawEvent#1"%taeNext]
                taeNext -= 1

        if "L0ETC" in outputs:
            importOptions( "$L0DUROOT/options/ETC.opts" ) # Adds "Sequencer/SeqWriteTag" to OutStreams
            MyWriter = TagCollectionStream( "WR" )
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output = "Collection='EVTTAGS/TagCreator/1' ADDRESS='/Event' DATAFILE='" + self.getProp("DatasetName") + "-L0ETC.root' TYP='POOL_ROOTTREE' OPT='RECREATE'"
            else: print MyWriter.getProp("Output")

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
            if not hasattr( MyWriter, "Output" ):
                MyWriter.Output = "DATAFILE='PFN:" + self.outputName() + ".mdf' SVC='LHCb::RawDataCnvSvc' OPT='REC'"
            MyWriter.RequireAlgs.append( "Filter" )
            ApplicationMgr().OutStream += [ nodeKiller, MyWriter ]

        nowarn = self.getProp( "NoWarnings" )
        if nowarn: importOptions( "$BOOLEOPTS/SuppressWarnings.opts" )

    def outputName(self):
        """
        Build a name for the output file, based in input options
        """
        outputName = self.getProp("DatasetName")
        if ( self.evtMax() > 0 ): outputName += '-' + str(self.evtMax()) + 'ev'
        if len(self.getProp( "FilterSequence" )) > 0 : outputName += '-filtered'
        if self.getProp("ExtendedDigi") : outputName += '-extended'
        return outputName

    def evtMax(self):
        return LHCbApp().evtMax()


    def configureMoni(self):
        # Set up monitoring
        moniDets = self._setupPhase( "Moni", self.KnownMoniSubdets )

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
                      STClusterMonitor, STEffChecker, MCParticle2MCHitAlg, MCParticleSelector )
            from GaudiKernel.SystemOfUnits import GeV

        if "IT" in moniDets:
            mcDepMoni   = MCSTDepositMonitor(  "MCITDepositMonitor", DetType="IT" )
            mcDigitMoni = MCSTDigitMonitor(    "MCITDigitMonitor",   DetType="IT" )
            digitMoni   = STDigitMonitor(      "ITDigitMonitor",     DetType="IT" )
            clusMoni    = STClusterMonitor(    "ITClusterMonitor",   DetType="IT" )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2ITMCHitAlg", MCHitPath = "MC/IT/Hits",
                                               OutputData = "/Event/MC/Particles2MCITHits" )
            effCheck    = STEffChecker(        "ITEffChecker",       DetType="IT" )
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
            clusMoni    = STClusterMonitor(    "TTClusterMonitor"   )
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
            GaudiSequencer("MoniL0Seq").Members += [ GaudiSequencer("L0MoniSeq") ]

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
