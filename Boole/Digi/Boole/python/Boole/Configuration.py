"""
High level configuration tools for Boole
"""
__version__ = ""
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from Gaudi.Configuration  import *
import GaudiKernel.ProcessJobOptions
from Configurables import ( LHCbConfigurableUser, LHCbApp, ProcessPhase, L0Conf,
                            DigiConf, SimConf, RichDigiSysConf, DecodeRawEvent )

class Boole(LHCbConfigurableUser):

    ## Default main sequences
    DefaultSequence = [ "ProcessPhase/Init",
                        "ProcessPhase/Digi",
                        "ProcessPhase/Link",
                        "ProcessPhase/Moni",  
                        "ProcessPhase/Filter" ]
    
    __slots__ = {
        "DetectorInit": {"DATA":['Data'],"MUON":['Muon']}
        ,"DetectorDigi": ['PuVeto', 'Velo', 'TT', 'IT', 'OT',       'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'L0']
        ,"DetectorLink": ['PuVeto', 'Velo', 'TT', 'IT', 'OT', 'Tr', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'L0']
        ,"DetectorMoni": ['PuVeto', 'Velo', 'TT', 'IT', 'OT',       'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon', 'L0', 'MC']
        ,"EvtMax"              : -1
        ,"SkipEvents"          : 0
        ,"UseSpillover"        : False
        ,"SpilloverPaths"      : []
        ,"TAEPrev"             : 0
        ,"TAENext"             : 0
        ,"TAESubdets"          : [ "Calo", "Muon" ]
        ,"Outputs"             : [ "DIGI" ]
        ,"InputDataType"       :  "SIM"         
        ,"DigiType"            : "Default"
        ,"Histograms"          : "Default"
        ,"NoWarnings"          : False
        ,"ProductionMode"      : False 
        ,"OutputLevel"         : INFO 
        ,"DatasetName"         : "Boole"
        ,"DataType"            : "2015"
        ,"DDDBtag"             : ""
        ,"CondDBtag"           : ""
        ,"VeloTell1Processing" : True
        ,"EnableL0"            : False
        ,"Monitors"            : []
        ,"MainSequence"        : []
        ,"InitSequence"        : []
        ,"DigiSequence"        : []
        ,"LinkSequence"        : []
        ,"MoniSequence"        : []
        ,"FilterSequence"      : []
        ,"EnablePack"          : True
        ,"SiG4EnergyDeposit"   : True
        ,"SplitRawEventFormat" : None #Where the raw event sits, DAQ/RawEvent!
        ,"WriteFSR"            : True  
        ,"MergeGenFSR"         : False
        }

    _propertyDocDct = { 
        "DetectorInit" : """ Dictionary specifying the detectors to take into account in initialisation """
       ,"DetectorDigi" : """ Dictionary specifying the detectors to take into account in digitization """
       ,"DetectorLink" : """ Dictionary specifying the detectors to make linkers """
       ,"DetectorMoni" : """ Dictionary specifying the detectors to monitoring """
       ,'EvtMax'       : """ Maximum number of events to process """
       ,'SkipEvents'   : """ Number of events to skip """
       ,'UseSpillover' : """ Flag to enable spillover (default False) """
       ,'SpilloverPaths':""" Paths to fill when spillover is enabled """
       ,'TAEPrev'      : """ Number of Prev Time Alignment Events to generate """
       ,'TAENext'      : """ Number of Next Time Alignment Events to generate """
       ,'TAESubdets'   : """ Subdetectors for which TAE are enabled """
       ,'Outputs'      : """ List of outputs: ['MDF','DIGI'] (default 'DIGI') """
       ,'InputDataType': """ Input Data Type 'XDST' (default 'SIM') """
       ,'DigiType'     : """ Defines content of DIGI file: ['Minimal','Default','Extended'] """
       ,'Histograms'   : """ Type of histograms: ['None','Default','Expert'] """
       ,'NoWarnings'   : """ OBSOLETE, kept for Dirac compatibility. Please use ProductionMode """
       ,'ProductionMode' : """ Enables special settings for running in production """
       ,'OutputLevel'  : """ The printout level to use (default INFO) """
       ,'DatasetName'  : """ String used to build output file names """
       ,'DataType'     : """ Data type. Default '2015' (use 'Upgrade' for LHCb Upgrade simulations)"""
       ,'DDDBtag'      : """ Tag for DDDB """
       ,'CondDBtag'    : """ Tag for CondDB """
       ,'VeloTell1Processing' : """ Use the Tell1 emulation bit perfect code to perform digitization and zero suppression of the simulated Velo data (default True) """
       ,'EnableL0'     : """ Flag to enable L0 emulation (default False, should be done in Moore step) """
       ,'Monitors'     : """ List of monitors to execute """
       ,'MainSequence' : """ The default main sequence, see self.DefaultSequence """
       ,'InitSequence' : """ List of initialisation sequences, see KnownInitSubdets """
       ,'DigiSequence' : """ List of subdetectors to digitize, see KnownDigiSubdets """
       ,'LinkSequence' : """ List of MC truth link sequences, see KnownLinkSubdets  """
       ,'MoniSequence' : """ List of subdetectors to monitor, see KnownMoniSubdets """
       ,'FilterSequence' : """ List of Filter sequences, see KnownFilterSubdets  """
       ,'EnablePack'   : """ Turn on/off packing of the data (where appropriate/available) """
       ,'SiG4EnergyDeposit': """ Modelling of energy loss for silicon trackers from Geant4 or in-house."""
       ,'WriteFSR'     : """ Flag whether to write out an FSR.  Default : True """
       ,'MergeGenFSR'  : """ Flag whether to merge the generatore level FSRs. Default False """ 
        }

    KnownFilterSubdets = [ "L0", "ODIN" ]
    KnownHistOptions   = ["","None","Default","Expert"]
    KnownSpillPaths    = [ "Prev", "PrevPrev", "Next", "NextNext" ]
    
    __used_configurables__ = [ LHCbApp, L0Conf, DigiConf, SimConf, RichDigiSysConf, DecodeRawEvent ]

    __detLinkListDigiConf = []


    def defineDB(self):
        if self.getProp("DataType") == "DC06" :
            raise RuntimeError( "DC06 data type no longer supported. Please use an earlier Boole version" )
        if self.getProp("DataType") == "MC09" :
            raise RuntimeError( "MC09 data type no longer supported. Please use an earlier Boole version" )
            
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["CondDBtag","DDDBtag","DataType"])
        LHCbApp().Simulation = True
        
    def setLHCbAppDetectors(self):
        from Configurables import LHCbApp
        # If detectors set in LHCbApp then use those        
        if hasattr(LHCbApp(),"Detectors"):
            if not LHCbApp().Detectors:
                LHCbApp().Detectors = self.getProp("DetectorDigi")
            else:
                print "WARNING:: Value of 'LHCbApp().Detectors' already set, using that value: %s" %(LHCbApp().Detectors)
        return


    def defineEvents(self):
        # Delegate handling to LHCbApp configurable
        self.setOtherProps(LHCbApp(),["EvtMax","SkipEvents"])
        # Setup SIM input
        self.setOtherProp(DigiConf(),"EnablePack")
        SimConf().setProp("EnableUnpack",self.getProp("EnablePack"))
        detListSim = []
        if 'Velo'    in self.getProp('DetectorDigi') :
            detListSim += ['Velo']
            detListSim += ['PuVeto'] # Add also PU Veto hits
        if 'VP'      in self.getProp('DetectorDigi') : detListSim += ['VP']
        if 'TT'      in self.getProp('DetectorDigi') : detListSim += ['TT']
        if 'UT'      in self.getProp('DetectorDigi') : detListSim += ['UT']
        if 'IT'      in self.getProp('DetectorDigi') : detListSim += ['IT']
        if 'OT'      in self.getProp('DetectorDigi') : detListSim += ['OT']
        if 'FT'      in self.getProp('DetectorDigi') : detListSim += ['FT']
        if [det for det in ['Rich1', 'Rich2', 'Rich1Pmt', 'Rich2Pmt'] if det in self.getProp("DetectorDigi")]:
            detListSim += ['Rich']
        #if [det for det in ['Prs','Spd','Ecal','Hcal'] if det in self.getProp("DetectorDigi")]:
        #    detListSim += ['Calo']
        if 'Spd'     in self.getProp('DetectorDigi') : detListSim += ['Spd']
        if 'Prs'     in self.getProp('DetectorDigi') : detListSim += ['Prs']
        if 'Ecal'    in self.getProp('DetectorDigi') : detListSim += ['Ecal']
        if 'Hcal'    in self.getProp('DetectorDigi') : detListSim += ['Hcal']
        if 'Muon'    in self.getProp('DetectorDigi') : detListSim += ['Muon']
        if 'HC'      in self.getProp('DetectorDigi') : detListSim += ['HC']
        SimConf().setProp("Detectors",detListSim)

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

        detListInit = []
        detListDigi = []
        detListLink = []
        detListMoni = []

        if 'Data' in self.getProp('DetectorInit')['DATA'] : detListInit += ['Data']

        for det in ['Velo', 'VP', 'TT', 'UT', 'IT', 'OT', 'FT', 'HC']:
            if det in self.getProp('DetectorDigi') :
                detListDigi += [det]
                if det in self.getProp('DetectorLink'): detListLink += [det]
                if det in self.getProp('DetectorMoni'): detListMoni += [det]

        if 'Tr' in self.getProp('DetectorLink') : detListLink += ['Tr']
            
        if [det for det in ["Rich1","Rich2"] if det in self.getProp("DetectorDigi")]:
            detListDigi += ['Rich']
            if [det for det in ["Rich1","Rich2"] if det in self.getProp("DetectorLink")]:
                detListLink += ["Rich"]
            if [det for det in ["Rich1","Rich2"] if det in self.getProp("DetectorMoni")]:
                detListMoni += ["Rich"]

        if [det for det in ["Rich1Pmt","Rich2Pmt"] if det in self.getProp("DetectorDigi")]:
            detListDigi += ['Rich']
            if [det for det in ["Rich1Pmt","Rich2Pmt"] if det in self.getProp("DetectorLink")]:
                detListLink += ["Rich"]

            if [det for det in ["Rich1Pmt","Rich2Pmt"] if det in self.getProp("DetectorMoni")]:
                detListMoni += ["Rich"]
            
        if True:
            if [det for det in ['Prs','Spd','Ecal','Hcal'] if det in self.getProp("DetectorDigi")]:
                detListDigi += ['Calo']
                if [det for det in ['Prs','Spd','Ecal','Hcal'] if det in self.getProp("DetectorLink")]:
                    detListLink += ['Calo']
                if [det for det in ['Prs','Spd','Ecal','Hcal'] if det in self.getProp("DetectorMoni")]:
                    detListMoni += ['Calo']
                
        if False:
            for det in ['Prs', 'Spd', 'Ecal', 'Hcal']:
                if det in self.getProp('DetectorDigi') :
                    detListDigi += [det]
                    if det in self.getProp('DetectorLink') : detListLink += [det]
                    if det in self.getProp('DetectorMoni') : detListMoni += [det]
            
        if 'Muon' in self.getProp('DetectorInit')['MUON'] : detListInit += ['Muon']
        if 'Muon' in self.getProp('DetectorDigi') :
            detListDigi += ['Muon']
            if 'Muon' in self.getProp('DetectorLink') : detListLink += ['Muon']
            if 'Muon' in self.getProp('DetectorMoni') : detListMoni += ['Muon']

        if self.getProp("EnableL0"):
            if 'L0' in self.getProp('DetectorDigi') :
                detListDigi += ['L0']
                if 'L0' in self.getProp('DetectorLink') : detListLink += ['L0']
                if 'L0' in self.getProp('DetectorMoni') : detListMoni += ['L0']

        if 'MC' in self.getProp('DetectorMoni') : detListMoni += ['MC']

        # Add requested linkers to Digi output
        for det in detListLink:
            self.__detLinkListDigiConf.append(det)

        # Now that requested linkers are saved, add any additional linkers needed for monitoring
        for det in ['Velo', 'VP', 'TT', 'UT', 'IT', 'OT', 'FT', 'Calo']:
            if not (det in detListLink) and (det in detListMoni): 
                detListLink += [det]

        initDets   = self._setupPhase( "Init",   detListInit )
        digiDets   = self._setupPhase( "Digi",   detListDigi )
        linkDets   = self._setupPhase( "Link",   detListLink )
        moniDets   = self._setupPhase( "Moni",   detListMoni )

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
        initBoole = GaudiSequencer("InitBooleSeq")
        initBoole.Members += [ "BooleInit" ]

        # Kept for Dirac backward compatibility
        if self.getProp( "NoWarnings" ) :
            log.warning("Boole().NoWarnings=True property is obsolete and maintained for Dirac compatibility. Please use Boole().ProductionMode=True instead")
            self.setProp( "ProductionMode", True )
            
        # Special settings for production
        if self.getProp( "ProductionMode" ) :
            self.setProp("OutputLevel", ERROR)
            if not LHCbApp().isPropertySet( "TimeStamp" ) :
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

        # Load the spillover branches, then kill those not required to prevent further access
        spillPaths = self.getProp("SpilloverPaths")
        killPaths  = []
        if len( spillPaths ) == 0:
            spillPaths = self.KnownSpillPaths
            self.setProp("SpilloverPaths",spillPaths)

        if self.getProp("UseSpillover"):
            if tae:
                killPaths = self.KnownSpillPaths
            else:
                self.setOtherProp(SimConf(),"SpilloverPaths")
                # Kill any spillover paths not required
                for spill in self.KnownSpillPaths:
                    if spill not in spillPaths:
                        killPaths.append( spill )
        else:
            # Kill all spillover paths
            killPaths = self.KnownSpillPaths

        from Configurables import EventNodeKiller, TESCheck
        spillLoader = TESCheck("SpilloverLoader")
        spillLoader.Inputs = spillPaths
        spillLoader.Stop   = False # In case no spillover on input file
        spillLoader.OutputLevel = ERROR
        spillKiller = EventNodeKiller("SpilloverKiller")
        spillKiller.Nodes = killPaths
        spillHandler = GaudiSequencer("SpilloverHandler")
        spillHandler.Members += [ spillLoader, spillKiller ]
        spillHandler.IgnoreFilterPassed = True # In case no spillover on input file
        initBoole.Members += [ spillHandler ]
  
        if "Muon" in initDets:
            # Muon Background
            from Configurables import MuonBackground
            GaudiSequencer("InitMuonSeq").Members += [ MuonBackground("MuonLowEnergy") ]
            importOptions( "$MUONBACKGROUNDROOT/options/MuonLowEnergy-G4.opts" )
            if not tae:
                flatSpillover = MuonBackground("MuonFlatSpillover")
                GaudiSequencer("InitMuonSeq").Members += [ flatSpillover ]
                if self.getProp("DataType") == "2010" :
                    flatSpillover.NBXFullFull = 344
                if self.getProp("DataType") == "2009" :
                    flatSpillover.NBXFullFull = 4
                importOptions( "$MUONBACKGROUNDROOT/options/MuonFlatSpillover-G4.opts" )

    def configureDigi(self,digiDets):

        """
        Set up the digitization sequence
        """
        importOptions("$STDOPTS/PreloadUnits.opts") # needed by VELO and ST
        if "Velo"    in digiDets : self.configureDigiVelo(    GaudiSequencer("DigiVeloSeq"), "" )
        if "VP"      in digiDets : self.configureDigiVP(      GaudiSequencer("DigiVPSeq"), "" )
        if "TT"      in digiDets : self.configureDigiST(      GaudiSequencer("DigiTTSeq"), "TT", "" )
        if "UT"      in digiDets : self.configureDigiST(      GaudiSequencer("DigiUTSeq"), "UT", "" )
        if "IT"      in digiDets : self.configureDigiST(      GaudiSequencer("DigiITSeq"), "IT", "" )
        if "OT"      in digiDets : self.configureDigiOT(      GaudiSequencer("DigiOTSeq"), "" )
        if "FT"      in digiDets : self.configureDigiFT(      GaudiSequencer("DigiFTSeq"), "" )
        if "Rich"    in digiDets : self.configureDigiRich(    GaudiSequencer("DigiRichSeq"), "")
        #if "RichPmt" in digiDets : self.configureDigiRichPmt(    GaudiSequencer("DigiRichSeq"), "")
        if "Calo"    in digiDets : self.configureDigiCalo(    GaudiSequencer("DigiCaloSeq"), "" )        
        if "Muon"    in digiDets : self.configureDigiMuon(    GaudiSequencer("DigiMuonSeq"), "" )
        if "L0"      in digiDets : self.configureDigiL0(      GaudiSequencer("DigiL0Seq"), "" )
        if "HC"      in digiDets : self.configureDigiHC(      GaudiSequencer("DigiHCSeq"), "" )

    def configureDigiVelo(self, seq, tae ):
        # Velo digitisation and clustering (also for PuVeto and trigger)            
        if tae == "":
            from Configurables import (VeloSim, PuVetoFillRawBuffer)
            toolName = "SiDepositedCharge" # In house Si deposited energy
            if self.getProp("SiG4EnergyDeposit"):
                # use GEANT energy deposit directly
                toolName = "SiGeantDepositedCharge"
            VeloSim('VeloSim').DepChargeTool = toolName
            VeloSim('VeloPUSim').DepChargeTool = toolName
            seq.Members += [ VeloSim("VeloSim") ]
            seq.Members += [ VeloSim("VeloPUSim") ]
            # note VeloSim.py checks the DepChargeTool and corrects the options
            # call after choosing SiDepositedCharge or SiGeantDepositedCharge
            importOptions("$VELOSIMULATIONROOT/options/VeloSim.py")
            if True != self.getProp("VeloTell1Processing"):
                from Configurables import (VeloDataProcessor, VeloClusterMaker,
                                           PrepareVeloRawBuffer )
                importOptions("$VELOALGORITHMSROOT/options/VeloAlgorithms.opts")
                seq.Members += [ VeloDataProcessor("VeloDataProcessor") ]
                if self.getProp("DataType") == "Upgrade" :
                    from Configurables import VeloSpillSubtraction                
                    seq.Members += [ VeloSim("VeloSimPrev") ]
                    seq.Members += [ VeloDataProcessor("VeloDataProcessorPrev") ]
                    seq.Members += [ VeloSpillSubtraction() ]
                    importOptions("$BOOLEUMCROOT/options/VeloSimPrev.opts")
                    VeloSim('VeloSimPrev').DepChargeTool = toolName
                    seq.Members += [ VeloSim("VeloSimNext") ]
                    seq.Members += [ VeloDataProcessor("VeloDataProcessorNext") ]
                    seq.Members += [ VeloSpillSubtraction("VeloSpillSubtractionNext") ]
                    importOptions("$BOOLEUMCROOT/options/VeloSimNext.opts")
                    VeloSim('VeloSimNext').DepChargeTool = toolName
                seq.Members += [ VeloClusterMaker("VeloClusterMaker") ]
                seq.Members += [ PrepareVeloRawBuffer("PrepareVeloRawBuffer") ]
            else:
                from Configurables import VeloTell1DataProcessor, VeloSimTell1ClusterMaker
                seq.Members += [ VeloTell1DataProcessor() ]
                seq.Members += [ VeloSimTell1ClusterMaker() ]
            seq.Members += [ PuVetoFillRawBuffer() ]
        else:
            raise RuntimeError("TAE not implemented for VELO")

    def configureDigiVP(self, seq, tae ):
        # VP digitisation and clustering
        if tae == "":
            from Configurables import VPDepositCreator, VPDigitCreator
            seq.Members += [VPDepositCreator(), VPDigitCreator()]
            from Configurables import VPSuperPixelBankEncoder
            seq.Members += [VPSuperPixelBankEncoder()]
        else:
            raise RuntimeError("TAE not implemented for VP")

    def configureDigiST(self, seq, det, tae ):
        # Silicon Tracker digitisation
        from Configurables import ( MCSTDepositCreator, MCSTDigitCreator, STDigitCreator,
                                    STClusterCreator, STClusterKiller, STClustersToRawBankAlg )
        if tae == "":
            if det == "IT":
                importOptions("$STDIGIALGORITHMSROOT/python/itDigi.py")
                #MCITDepositCreator.DepChargeTool =  " SiGeantDepositedCharge"
            elif det == "TT":
                importOptions("$STDIGIALGORITHMSROOT/python/ttDigi.py")
            elif det == "UT":
                importOptions("$STDIGIALGORITHMSROOT/python/utDigi.py")
            else:
                raise RuntimeError("Unknown ST detector '%s'"%det)

            toolName = "SiDepositedCharge"
            if self.getProp("SiG4EnergyDeposit"):
                toolName = "SiGeantDepositedCharge"
            MCSTDepositCreator("MC%sDepositCreator%s"%(det,tae),DetType=det).DepChargeTool = toolName            
            seq.Members += [ MCSTDepositCreator("MC%sDepositCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ MCSTDigitCreator("MC%sDigitCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ STDigitCreator("%sDigitCreator%s"%(det,tae),DetType=det) ]

            if self.getProp("DataType") == "Upgrade" :
                from Configurables import STSpilloverSubtraction, TTSpilloverSubtraction 
                if det == "IT":
                    importOptions("$BOOLEUMCROOT/options/itDigiPrev.opts")
                elif det == "TT":
                    TTSpilloverSubtractionNext = TTSpilloverSubtraction("TTSpilloverSubtractionNext")
                    TTSpilloverSubtractionPrevPrevFromPrev = TTSpilloverSubtraction("TTSpilloverSubtractionPrevPrevFromPrev")
                    TTSpilloverSubtractionPrevPrev = TTSpilloverSubtraction("TTSpilloverSubtractionPrevPrev")
                    importOptions("$BOOLEUMCROOT/options/ttDigiPrevPrevFromPrev.opts")
                    importOptions("$BOOLEUMCROOT/options/ttDigiPrevPrev.opts")
                    importOptions("$BOOLEUMCROOT/options/ttDigiPrev.opts")
                    importOptions("$BOOLEUMCROOT/options/ttDigiNext.opts")
                    seq.Members += [ MCSTDigitCreator("MCTTDigitCreatorNext",DetType=det) ]
                    seq.Members += [ MCSTDigitCreator("MCTTDigitCreatorPrevPrev",DetType=det) ]
                    seq.Members += [ STDigitCreator("TTDigitCreatorNext",DetType=det) ]
                    seq.Members += [ STDigitCreator("TTDigitCreatorPrevPrev",DetType=det) ]
                elif det == "UT":
                    STDigitCreator("%sDigitCreator%s"%(det,tae),DetType=det).Saturation = 31
                else:
                    raise RuntimeError("Unknown ST detector '%s'"%det)
                if det not in ["UT"]:
                    seq.Members += [ MCSTDigitCreator("MC%sDigitCreator%sPrev"%(det,tae),DetType=det) ]
                    seq.Members += [ STDigitCreator("%sDigitCreator%sPrev"%(det,tae),DetType=det) ]
                if det =="IT":
                    seq.Members += [ STSpilloverSubtraction("%sSpilloverSubtraction%s"%(det,tae),DetType=det) ]
                if det =="TT":
                    seq.Members += [ TTSpilloverSubtraction("%sSpilloverSubtraction%s"%(det,tae),DetType=det) ]
                    seq.Members += [ TTSpilloverSubtraction("TTSpilloverSubtractionNext",DetType=det) ]
                    seq.Members += [ TTSpilloverSubtraction("TTSpilloverSubtractionPrevPrevFromPrev",DetType=det) ]
                    seq.Members += [ TTSpilloverSubtraction("TTSpilloverSubtractionPrevPrev",DetType=det) ]
            seq.Members += [ STClusterCreator("%sClusterCreator%s"%(det,tae),DetType=det) ]
            seq.Members += [ STClusterKiller("%sClusterKiller%s"%(det,tae),DetType=det) ]
            seq.Members += [ STClustersToRawBankAlg("create%sRawBuffer%s"%(det,tae),DetType=det) ]
        else:
            raise RuntimeError("TAE not implemented for %s"%det)

    def configureDigiOT(self, seq, tae ):
        # Outer Tracker digitisation
        from Configurables import MCOTDepositCreator, MCOTTimeCreator, OTFillRawBuffer
        seq.Members += [ MCOTDepositCreator("MCOTDepositCreator%s"%tae) ]
        mcOTTimeCreator = MCOTTimeCreator("MCOTTimeCreator%s"%tae)
        seq.Members += [ mcOTTimeCreator ]
        hsvc = HistogramSvc ( 'HistogramDataSvc' )
        hsvc.Input += [ "OTDoublePulse DATAFILE='$PARAMFILESROOT/data/OTDoublePulse.root' TYP='ROOT'" ]       
        if self.getProp("DataType") == "Upgrade" :
            # 3=75ns (default) 2=50ns 
            mcOTTimeCreator.numberOfBX = 2
        seq.Members += [ OTFillRawBuffer("OTFillRawBuffer%s"%tae) ]   


    def configureDigiFT(self, seq, tae ):
        # Fibre Tracker digitisation
        if tae == "":
            from Configurables import MCFTDepositCreator, MCFTDigitCreator, FTClusterCreator, FTRawBankEncoder
            seq.Members += [
                "MCFTDepositCreator",
                "MCFTDigitCreator",
                "FTClusterCreator",
                "FTRawBankEncoder"
                ]
            from Configurables import MCFTDepositMonitor
            seq.Members += [ "MCFTDepositMonitor" ]
        else:
            raise RuntimeError("TAE not implemented for RICH")


    def configureDigiRich(self, seq, tae ):
        if tae == "":
            from Configurables import RichDigiSysConf
            self.setOtherProp(RichDigiSysConf(),"UseSpillover")
            RichDigiSysConf().Sequencer = GaudiSequencer("DigiRichSeq")
            # Apply some additional settings if using upgrade Rich
            if [det for det in self.getProp("DetectorDigi") if det in ["Rich1Pmt", "Rich2Pmt"]]:
                RichDigiSysConf().ChargeShareModel = "None"
                RichDigiSysConf().ResponseModel = "Copy"
                RichDigiSysConf().OutputLevel = INFO
                RichDigiSysConf().RawDataFormatVersion = 3                
                RichDigiSysConf().UseSpillover = False
        else:
            raise RuntimeError("TAE not implemented for RICH")
            
    def configureDigiCalo(self, seq, tae ):
        # Calorimeter digitisation
        from Configurables import CaloSignalAlg, CaloDigitAlg, CaloFillPrsSpdRawBuffer, CaloFillRawBuffer
        if tae != "":
            seq.Context = "TAE"

        if "Spd" in self.getProp("DetectorDigi"):
            seq.Members += [CaloSignalAlg("SpdSignal%s"%tae)]
        if "Prs" in self.getProp("DetectorDigi"):
            seq.Members += [CaloSignalAlg("PrsSignal%s"%tae)]
        seq.Members += [CaloSignalAlg("EcalSignal%s"%tae),
                        CaloSignalAlg("HcalSignal%s"%tae)]

        if "Spd" in self.getProp("DetectorDigi"):
            seq.Members += [CaloDigitAlg("SpdDigit%s"%tae)]
        if "Prs" in self.getProp("DetectorDigi"):
            seq.Members += [CaloDigitAlg("PrsDigit%s"%tae)]
        seq.Members += [CaloDigitAlg("EcalDigit%s"%tae),
                        CaloDigitAlg("HcalDigit%s"%tae) ]

        if [det for det in ["Spd","Prs"] if det in self.getProp("DetectorDigi")]:
            rawPrsSpd = CaloFillPrsSpdRawBuffer( "CaloFillPrsSpdRawBuffer%s"%tae, DataCodingType = 3 )
            seq.Members += [ rawPrsSpd ]

        rawEcal = CaloFillRawBuffer( "EcalFillRawBuffer%s"%tae, DataCodingType = 2 )
        rawHcal = CaloFillRawBuffer( "HcalFillRawBuffer%s"%tae, DataCodingType = 2 )
        seq.Members += [ rawEcal, rawHcal ]

        # DigiCaloSeq          INFO Member list: CaloSignalAlg/SpdSignal, CaloSignalAlg/PrsSignal, CaloSignalAlg/EcalSignal, CaloSignalAlg/HcalSignal, CaloDigitAlg/SpdDigit, CaloDigitAlg/PrsDigit, CaloDigitAlg/EcalDigit, CaloDigitAlg/HcalDigit, CaloFillPrsSpdRawBuffer, CaloFillRawBuffer/EcalFillRawBuffer, CaloFillRawBuffer/HcalFillRawBuffer

    def configureDigiMuon(self, seq, tae ):
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

            # Force use of DAQ/RawEvent in emulation
            #from Configurables import L0CaloAlg, CaloTriggerAdcsFromRaw, CaloTriggerBitsFromRaw
            from DAQSys.Decoders import DecoderDB
            l0Calo = DecoderDB["L0CaloAlg/L0Calo"]
            l0Calo.Active=True
            l0Calo.setup()
        else:
            raise RuntimeError("TAE not implemented for L0")
        
    def configureDigiHC(self, seq, tae):
        # HC digitisation
        if tae == "":
            from Configurables import HCDigitCreator
            seq.Members += [HCDigitCreator()]
        else:
            raise RuntimeError("TAE not implemented for HC")

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
            if self.getProp("EnableL0"):
                from Configurables import L0Conf
                L0Conf().FilterSequencer = GaudiSequencer("FilterL0Seq")
            else:
                raise RuntimeError("L0 filtering requested but L0 emulation not enabled")

        if "ODIN" in filterDets: 
            from Configurables import OdinTypesFilter
            odinFilter = OdinTypesFilter()
            GaudiSequencer("FilterODINSeq").Members += [ odinFilter ]

    def configureLink(self,linkDets,moniDets):
        """
        Set up the MC links sequence
        """

        doWriteTruth = ("DIGI" in self.getProp("Outputs")) and (self.getProp("DigiType").capitalize() != "Minimal")

        if "Velo" in linkDets:
            seq = GaudiSequencer("LinkVeloSeq")
            from DAQSys.Decoders import DecoderDB
            #from Configurables import DecodeVeloRawBuffer
            decodeVelo = DecoderDB["DecodeVeloRawBuffer/createVeloClusters"].setup()
            seq.Members += [ decodeVelo ]
            seq.Members += [ "VeloCluster2MCHitLinker" ]
            seq.Members += [ "VeloCluster2MCParticleLinker" ]
        
        if "VP" in linkDets:
            from Configurables import VPDigitLinker
            seq = GaudiSequencer("LinkVPSeq")
            seq.Members += [VPDigitLinker()]
        
        if (det for det in ["TT", "IT", "UT"] if det in linkDets):
            from Configurables import STDigit2MCHitLinker, STCluster2MCHitLinker, STCluster2MCParticleLinker
            if "TT" in linkDets:
                seq = GaudiSequencer("LinkTTSeq")
                seq.Members += [ STDigit2MCHitLinker("TTDigitLinker") ]
                seq.Members += [ STCluster2MCHitLinker("TTClusterLinker") ]
                seq.Members += [ STCluster2MCParticleLinker("TTTruthLinker") ]

            if "UT" in linkDets:
                seq = GaudiSequencer("LinkUTSeq")
                seq.Members += [ STDigit2MCHitLinker("UTDigitLinker", DetType = "UT") ]
                seq.Members += [ STCluster2MCHitLinker("UTClusterLinker", DetType = "UT") ]
                seq.Members += [ STCluster2MCParticleLinker("UTTruthLinker", DetType = "UT") ]

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

        if "Tr" in linkDets and doWriteTruth:
            from Configurables import BuildMCTrackInfo           
            seq = GaudiSequencer("LinkTrSeq")
            buildMCTrackInfo = BuildMCTrackInfo()
            seq.Members += [ buildMCTrackInfo ]                
            
            if "Velo" in linkDets:
                buildMCTrackInfo.WithVelo = True
            elif "VP" in linkDets:
                buildMCTrackInfo.WithVP = True
            else:
                raise RuntimeError("Need one of Velo or VP to build MCTrackInfo")
                
            if "UT" in linkDets:
                buildMCTrackInfo.WithUT = True
            elif "TT" in linkDets:
                buildMCTrackInfo.WithUT = False
            else:
                raise RuntimeError("Need one of TT or UT to build MCTrackInfo")
                
            if "FT" in linkDets:
                buildMCTrackInfo.WithFT = True
            if "IT" in linkDets:
                buildMCTrackInfo.WithIT = True
            if "OT" in linkDets:
                buildMCTrackInfo.WithOT = True

            if not (self.getProp("DataType") == "Upgrade") :
                if not("IT" in linkDets) and not ("OT" in linkDets):
                    raise RuntimeError("Need both IT and OT to build MCTrackInfo in non upgrade case")

        #if "Rich" in linkDets and doWriteTruth:
        if [det for det in linkDets if det in ["Rich1","Rich2","Rich"]] and doWriteTruth:
            seq = GaudiSequencer("LinkRichSeq")
            seq.Members += [ "Rich::MC::MCRichDigitSummaryAlg" ]

        if [det for det in linkDets if det in ["Rich1Pmt","Rich2Pmt","RichPmt"]] and doWriteTruth:
            seq = GaudiSequencer("LinkRichSeq")
            seq.Members += [ "Rich::MC::MCRichDigitSummaryAlg" ]

        if "Calo" in linkDets:
            from Configurables import CaloReCreateMCLinks, CaloDigitMCTruth
            seq = GaudiSequencer("LinkCaloSeq")
            from DAQSys.Decoders import DecoderDB as ddb
            from DAQSys.DecoderClass import decodersForBank
            #change the active decoders to the Non-Zero-Suppressed
            from DAQSys.Decoders import caloSetZeroSuppressed
            caloSetZeroSuppressed(ddb,False)
            #get the decoders, but avoid the pitfalls of the trigger-specific versions
            ecalDecoders = [d.setup() for d in decodersForBank(ddb,"EcalE") if "Trigger" not in d.FullName and "L0" not in d.FullName]
            seq.Members += ecalDecoders
            hcalDecoders = [d.setup() for d in decodersForBank(ddb,"HcalE")if "Trigger" not in d.FullName and "L0" not in d.FullName]
            seq.Members += hcalDecoders
            recreateLinks = CaloReCreateMCLinks()
            #note that these are defined in the C++ of the decoders, basically impossible to overwrite here
            recreateLinks.Digits   = ["Raw/Ecal/Digits", "Raw/Hcal/Digits" ]
            recreateLinks.MCDigits = ["MC/Ecal/Digits",  "MC/Hcal/Digits" ]
            seq.Members += [ recreateLinks ]
            seq.Members += [ CaloDigitMCTruth("EcalDigitMCTruth") ]
            hcalTruth = CaloDigitMCTruth("HcalDigitMCTruth")
            #note that these are defined in the C++ of the decoders, basically impossible to overwrite here
            hcalTruth.Input = "Raw/Hcal/Digits"
            hcalTruth.Detector = "/dd/Structure/LHCb/DownstreamRegion/Hcal"
            seq.Members += [ hcalTruth ]

        if "Muon" in linkDets and doWriteTruth:
            seq = GaudiSequencer("LinkMuonSeq")
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
            mainSeq.insert(digi,taePhase)
            taeSlots.append( "Prev%s"%taePrev )
            taePrev -= 1

        taeNext = self.getProp("TAENext")
        while taeNext>0:
            digi = mainSeq.index("ProcessPhase/Digi")
            taePhase = ProcessPhase( "DigiNext%s"%taeNext )
            taePhase.RootInTES = "Next%s/"%taeNext
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

            if "Velo" in taeDets:
                self.configureDigiVelo( GaudiSequencer("Digi%sVeloSeq"%taeSlot), taeSlot )
            if "VP" in taeDets:
                self.configureDigiVP( GaudiSequencer("Digi%sVPSeq"%taeSlot), taeSlot )
            if "TT" in taeDets:
                self.configureDigiST( GaudiSequencer("Digi%sTTSeq"%taeSlot), "TT", taeSlot )
            if "UT" in taeDets:
                self.configureDigiST( GaudiSequencer("Digi%sUTSeq"%taeSlot), "UT", taeSlot )
            if "IT" in taeDets:
                self.configureDigiST( GaudiSequencer("Digi%sITSeq"%taeSlot), "IT", taeSlot )
            if "OT" in taeDets:
                self.configureDigiOT( GaudiSequencer("Digi%sOTSeq"%taeSlot), taeSlot )
            if [det for det in taeDets if det in ['Rich1', 'Rich2']]:
                self.configureDigiRich( GaudiSequencer("Digi%sRichSeq"%taeSlot), taeSlot )
            #if [det for det in taeDets if det in ['Rich1Pmt', 'Rich2Pmt']]:
            #    self.configureDigiRichPmt( GaudiSequencer("Digi%sRichSeq"%taeSlot), taeSlot )
            if [det for det in taeDets if det in ['Spd','Prs','Ecal','Hcal']]:
                caloTAEDets = [det for det in taeDets if det in ['Spd','Prs','Ecal','Hcal']]
                self.configureDigiCalo( GaudiSequencer("Digi%sCaloSeq"%taeSlot), taeSlot )
            if "Muon" in taeDets:
                self.configureDigiMuon( GaudiSequencer("Digi%sMuonSeq"%taeSlot), taeSlot )
            if "L0" in taeDets:
                self.configureDigiL0( GaudiSequencer("Digi%sL0Seq"%taeSlot), taeSlot )
            if "HC" in taeDets:
                self.configureDigiHC( GaudiSequencer("Digi%sHCSeq"%taeSlot), taeSlot )
            

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
    
    def defineInput(self):
          """
          Setup input data type can be SIM or XDST
          """
          if self.getProp("InputDataType") == "XDST"  :

              importOptions("$BOOLEROOT/options/Boole-RunFromXDST.py")
          if self.getProp("InputDataType") == "XDIGI" :
              importOptions("$BOOLEROOT/options/Boole-RunFromXDIGI.py") 

    def defineOutput(self):

        """
        Set up output stream according to output data type
        """

        knownOptions = ["MDF","DIGI"]
        outputs = []
        for option in self.getProp("Outputs"):
            if option not in knownOptions:
                raise RuntimeError("Unknown Boole().Outputs value '%s'"%option)
            outputs.append( option )

        if "DIGI" in outputs:
            seq = GaudiSequencer("PrepareDIGI")
            ApplicationMgr().TopAlg += [ seq ]

            # In Minimal case, filter the MCVertices before writing
            if self.getProp("DigiType").capitalize() == "Minimal":
                seq.Members = ["FilterMCPrimaryVtx"]

            # In packed case, run the packing algorithms
            if self.getProp( "EnablePack" ):
                DigiConf().PackSequencer = seq

            writerName = "DigiWriter"
            digiWriter = OutputStream( writerName, Preload=False )
            

            digiWriter.RequireAlgs.append( "Filter" )
            if self.getProp( "NoWarnings" ) and not digiWriter.isPropertySet( "OutputLevel" ):
                digiWriter.OutputLevel = INFO

            # Set up the Digi content
            DigiConf().Writer     = writerName
            DigiConf().OutputName = self.outputName()
            DigiConf().setProp("Detectors",self.__detLinkListDigiConf)
            self.setOtherProps(DigiConf(),["DigiType","TAEPrev","TAENext","UseSpillover","DataType","WriteFSR"])
            if self.getProp("UseSpillover"):
                self.setOtherProps(DigiConf(),["SpilloverPaths"])

        if "MDF" in outputs:
            # Make sure that file will have no knowledge of other nodes
            from Configurables import EventNodeKiller
            nodeKiller = EventNodeKiller("MDFKiller")
            nodeKiller.Nodes  = [ "Rec", "Trig", "MC", "Raw", "Gen", "Link", "pSim" ]
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

        # Merge and write the genFSRs
        if self.getProp("WriteFSR"):
            if self.getProp("MergeGenFSR"):
                seqGenFSR = GaudiSequencer("GenFSRSeq")
                ApplicationMgr().TopAlg += [ seqGenFSR ]
                seqGenFSR.Members += [ "GenFSRMerge" ]
                
            FSRWriter = RecordStream( "FSROutputStreamDigiWriter")
            if not FSRWriter.isPropertySet( "OutputLevel" ):
                FSRWriter.OutputLevel = INFO
                                                                                                        
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

    def configureDecoders(self):
        "Configure decoders in case they are needed in the monitoring"
        #monitoring and linking needs decoders, but not on demand
        if not DecodeRawEvent().isPropertySet("OverrideInputs"):
            v=0.0
            if self.isPropertySet("SplitRawEventFormat") and self.getProp("SplitRawEventFormat") is not None:
                v=self.getProp("SplitRawEventFormat")
            DecodeRawEvent().setProp("OverrideInputs",v)
        if not DecodeRawEvent().isPropertySet("EvtClockBank"):
            DecodeRawEvent().setProp("EvtClockBank","")
        
    
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

        if "Velo" in moniDets:
            from Configurables import  VeloSimMoni,VeloDigit2MCHitLinker,VeloDigiMoni,VeloRawClustersMoni
            GaudiSequencer("MoniVeloSeq").Members += [ VeloSimMoni(), VeloDigit2MCHitLinker(),
                                                       VeloDigiMoni(), VeloRawClustersMoni()  ]
            if self.getProp("DataType") == "Upgrade" :
                from Configurables import VeloMonitorSpilloverSubtr
                GaudiSequencer("MoniVeloSeq").Members += [ VeloMonitorSpilloverSubtr() ]
            if True == self.getProp("VeloTell1Processing"):
                GaudiSequencer("MoniVeloSeq").Members.remove( VeloDigit2MCHitLinker() )
                GaudiSequencer("MoniVeloSeq").Members.remove( VeloDigiMoni() )

        if "IT" in moniDets or "TT" in moniDets or "UT" in moniDets:
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
            if self.getProp("DataType") == "Upgrade" :
                from Configurables import STSpilloverSubtrMonitor
                GaudiSequencer("MoniITSeq").Members += [ STSpilloverSubtrMonitor("ITSpilloverSubtrMonitor",DetType="IT") ]
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
            if self.getProp("DataType") == "Upgrade" :
                from Configurables import STSpilloverSubtrMonitor
                GaudiSequencer("MoniTTSeq").Members += [ STSpilloverSubtrMonitor("TTSpilloverSubtrMonitor",DetType="TT") ]
            if histOpt == "Expert":
                mcDepMoni.FullDetail   = True
                mcDigitMoni.FullDetail = True
                clusMoni.FullDetail    = True
                effCheck.FullDetail    = True

        if "UT" in moniDets:
            mcDepMoni   = MCSTDepositMonitor(  "MCUTDepositMonitor", DetType="UT" )
            mcDigitMoni = MCSTDigitMonitor(    "MCUTDigitMonitor",   DetType="UT" )
            digitMoni   = STDigitMonitor(      "UTDigitMonitor",     DetType="UT" )
            clusMoni    = ST__STClusterMonitor("UTClusterMonitor",   DetType="UT" )
            mcp2MCHit   = MCParticle2MCHitAlg( "MCP2UTMCHitAlg", MCHitPath = "MC/UT/Hits",
                                               OutputData = "/Event/MC/Particles2MCUTHits" )
            effCheck    = STEffChecker(        "UTEffChecker",       DetType="UT" )
            effCheck.addTool(MCParticleSelector)
            effCheck.MCParticleSelector.zOrigin = 50.0
            effCheck.MCParticleSelector.pMin = 1.0*GeV
            effCheck.MCParticleSelector.betaGammaMin = 1.0
            clusMoni.DetType = "UT"
            clusMoni.ClusterLocation = "Raw/UT/Clusters"
            effCheck.DetType = "UT"
            effCheck.InputData = "Raw/UT/Clusters"
            GaudiSequencer("MoniUTSeq").Members += [ mcDepMoni, mcDigitMoni, digitMoni, clusMoni,
                                                     mcp2MCHit, effCheck ]
            if False:
            #if self.getProp("DataType") == "Upgrade" :                
                from Configurables import STSpilloverSubtrMonitor
                GaudiSequencer("MoniUTSeq").Members += [ STSpilloverSubtrMonitor("UTSpilloverSubtrMonitor",DetType="UT") ]
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

        if "FT" in moniDets:
            from Configurables import MCFTDepositMonitor
            GaudiSequencer("MoniFTSeq").Members += ["MCFTDepositMonitor"]
            # not needed!
            #if histOpt == "Expert":
            #    from Configurables import FTRawBankDecoder
            #    FTRawBankDecoder().RawEventLocations = ["DAQ/RawEvent"]

        if "Rich" in moniDets:
            from Configurables import Rich__MC__Digi__DigitQC
            GaudiSequencer("MoniRichSeq").Members += [ Rich__MC__Digi__DigitQC("RiDigitQC") ]

        if "RichPmt" in moniDets:
            from Configurables import Rich__MC__Digi__DigitQC
            GaudiSequencer("MoniRichSeq").Members += [ Rich__MC__Digi__DigitQC("RiDigitQC") ]

        if "Calo" in moniDets:
            from Configurables import CaloDigitChecker
            importOptions("$CALOMONIDIGIOPTS/CaloDigitChecker.opts")
            GaudiSequencer("MoniCaloSeq").Members += [ CaloDigitChecker("SpdCheck")]
            GaudiSequencer("MoniCaloSeq").Members += [ CaloDigitChecker("PrsCheck")]
            GaudiSequencer("MoniCaloSeq").Members += [ CaloDigitChecker("EcalCheck")]
            GaudiSequencer("MoniCaloSeq").Members += [ CaloDigitChecker("HcalCheck")]
            
        if "Muon" in moniDets:
            from Configurables import MuonDigitChecker
            GaudiSequencer("MoniMuonSeq").Members += [ "MuonDigitChecker" ]

        if "L0" in moniDets:
            from Configurables import L0Conf
            L0Conf().MoniSequencer = GaudiSequencer("MoniL0Seq")
            # decode the L0 raw banks (needed by the monitors
            L0Conf().DecodeL0 = True

    # Problem comes in here !!
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
        self.defineInput()
        GaudiKernel.ProcessJobOptions.PrintOff()
        self.defineDB()
        self.setLHCbAppDetectors()
        self.configureDecoders()
        self.defineEvents()
        self.configurePhases()
        self.defineOutput()
        self.defineMonitors()
        self.saveHistos()
        GaudiKernel.ProcessJobOptions.PrintOn()
        log.info( self )
        GaudiKernel.ProcessJobOptions.PrintOff()
